#include <string.h>
#include <sys/mman.h>
#include "type.h"
#include "name.h"
#include "asm.h"

// TODO support for compilers other than gcc
Reg get_reg(RegM u) { return __builtin_ctz(~u); }
Reg num_marked(RegM u) { return __builtin_popcount(u); }
Reg get_reg_mark(RegM *u, RegM v) {
  Reg r=get_reg(*u|v); *u|=1<<r; return r;
}
RegM input_mask(A a, U n) { RegM u=0; DO(i,n) u|=1<<a->i[i]; return u; }
U choose_reg(A a) {
  Reg i=a->i[0], o=a->o;
  if (i==NO_REG_NM) i=get_reg_mark(&a->u, 1<<o);
  if (o==NO_REG) o = (i<NO_REG && !(a->u&1<<i)) ? i : get_reg(a->u);
  if (i==NO_REG) i = o;
  a->i[0]=i; a->o=o; return i!=o;
}
U choose_regn(A a, U n) {
  Reg *i=a->i, o=a->o;
  DO(j,n) if (i[j]==NO_REG_NM) i[j]=get_reg_mark(&a->u, 1<<o);
  RegM ui=a->u; DO(j,n) if (i[j]==NO_REG) i[j]=get_reg_mark(&ui,0);
  U ii=0; if (o==NO_REG) {
    while (ii<n  &&  a->u & 1<<a->i[ii]) ii++;
    a->o = (ii==n) ? get_reg(a->u) : i[ii];
  } else {
    while (ii<n && o!=a->i[ii]) ii++;
  }
  return ii;
}
U choose_regs(A a) { return choose_regn(a,2); }

void protect_input(Reg *i, RegM *u) {
  if (*i<NO_REG) *u|=1<<*i; else *i=NO_REG_NM;
}

void a_append(A a, U l, Asm aa) {
  U nl = a->l+l; if (!nl) return;
  U n = next_pow_2(nl);
  if (a->l*2 <= n) { a->a = a->l ? realloc(a->a, n) : malloc(n); }
  memcpy(a->a+a->l, aa, l); a->l=nl;
}

void request_regs(A a, U n) { a->ar[*(U*)&a->i][0]+=n; }
void request_cv(A a, Z c) {
  if (a->lc>=MIN_ARR && PURE(a->lc)) {
    REALLOC(a->cr, 2*a->lc);
    REALLOC(a->cv, 2*a->lc);
  }
  a->cr[a->lc] = NO_REG; a->cv[a->lc++] = c;
}
Reg use_cr(A a) { return a->cr[a->lc++]; }

T apply_R_O(A a, O f, U n, T* x) {
  U l=f->l; T t[l]; request_regs(a,l);
  DO(i, l) { request_regs(a,-1); if (!(t[i] = apply_R(a, f->x[i], n, x))) return 0; }
  return apply_R(a, f->f, l, t);
}
void apply_A_O(A a, O f, U n, T* x) {
  U l=f->l; T t[l];
  if (l==1 && n==2 && T(f->f)==B_t && B(f->f)=='m' && T(f->x[0])==B_t && B(f->x[0])=='/' && x[0]==Z_t && x[1]==Z_t) {
    Reg r0=REG_IDIV_0, r1=REG_IDIV_1;
    C shortcut = a->i[0]==r0;
    U n=2; PROTECT_START(1<<r0|1<<r1);
    if (!shortcut) ASM(a, MOV,r0,a->i[0]);
    ASM(a, CQO,-,-);
    ASM(a, IDIV,a->i[1],-);

    ASM(a, TEST,r1,r1);
    ASM(a, CMOVE,r1,a->i[1]);
    ASM(a, XOR,r1,a->i[1]);
    ASM(a, SHRI1,r1,63);
    ASM(a, SUB,r0,r1);

    if (a->o==NO_REG) a->o = a->u&1<<r0 ? get_reg(a->u) : r0;
    if (a->o!=r0) ASM(a, MOV,a->o,r0);
    UNPROTECT; return;
  }
  AS ax=*a; Reg iF[l];
  RegM ua=ax.u; DO(i,n) protect_input(&a->i[i],&ax.u);
  DO(i,l) {
    if (i==l-1) ax.u = ua;
    ax.o = NO_REG;
    // TODO shortcut errors
    t[i] = apply_A_t(&ax, f->x[i], n, x);
    iF[i] = ax.o; ax.u |= ua |= 1<<ax.o;
  }
  UPDATE_A(a,ax);

  AS af=*a; af.i=iF;
  apply_A(&af, f->f, l, t);
  a->o=af.o; UPDATE_A(a,af);
}

#define EACH_REG(U,R) for (RegM ui=U; R=get_reg(~ui), ui; ui-=1<<R)
#define DEACH_REG(U,R) Reg R; EACH_REG(U,R)
void pop_regs(A a, RegM pop) {
  Reg rs[8*sizeof(RegM)]; U i=0; DEACH_REG(pop,r) rs[i++]=r;
  while(i--) { ASM(a,POP,rs[i],-); }
}
RegM push_regs(A a, RegM u) {
  DEACH_REG(u,r) { ASM(a,PUSH,r,-); } return u;
}

ProtState clear_regs(A a, U n, RegM u, C start) {
  RegM uc = u&a->u, ui=input_mask(a,n), uu = a->u|u|ui;
  DO(ii,n) {
    Reg i=a->i[ii]; RegM si=1<<i;
    if (i>=NO_REG || u&si) {
      Reg i_ = a->i[ii] = get_reg(uu);
      uu |= 1<<i_;
      if (u&si) ASM(a, MOV,i_,i);
      else if (i==NO_REG_NM) a->u|=1<<i_;
    }
  }
  DEACH_REG(uc,r) { ASM(a,PUSH,r,-); }
  Reg o=NO_REG; if (!start && u&1<<a->o) { o=a->o; a->o=get_reg(uu); }
  return (ProtState){uc,o};
}
void pop_regs_o(A a, RegM pop, Reg o) {
  if (o<NO_REG && o!=a->o) { ASM(a, MOV,o,a->o); a->o=o; }
  pop_regs(a, pop);
}

// REG_ARG0, REG_RES, and res must be modifiable
// Allocate l bytes and place the pointer at res.
void a_malloc(A a, U l, Reg res, RegM keep) {
  RegM pop = push_regs(a, keep&REG_SAVE);
  ASM(a, MOV4_RI, res,(I)(Z)&malloc);
  ASM(a, MOV4_RI, REG_ARG0,l);
  ASM(a, CALL, res,-);
  if (res!=REG_RES) ASM(a, MOV, res,REG_RES);
  pop_regs(a, pop);
}

typedef void (*del_t)(V); del_t del_S(T);
void a_del(A a, T t, Reg i) {
  del_t del=del_S(t); if (!del) return;
  RegM pop = push_regs(a, a->u&REG_SAVE);
  if (i!=REG_ARG1) ASM(a, MOV, REG_ARG1,i);
  if (PURE(t)) {
    ASM(a, SUBI1,REG_SP,8);
    ASM(a, MOV_MR0,REG_SP,REG_ARG1);
    ASM_RAW(a, {0x24});
    ASM(a, MOV,REG_ARG1,REG_SP);
  }
  ASM(a, MOV4_RI, REG_ARG0,t);
  ASM(a, MOV4_RI, REG_RES,(I)(Z)del);
  ASM(a, CALL, REG_RES,-);
  if (PURE(t)) {
    ASM(a, ADDI1,REG_SP,8);
  }
  pop_regs(a, pop);
}

T apply_R_L(A a, L f, U n, T* x) {
  U l=f->l;
  DO(i, l) { if (!apply_R(a, list_at(f,i), n, x)) return 0; }
  request_regs(a,1);
  return L_t;
}
void apply_A_L(A a, L f, U n, T* x) {
  U l=f->l; T t[l], tt=0; Reg o[l]; RegM au=a->u;

  PROTECT(REG_SAVE);

  // Don't add any code to a; store in as
  AS ax=*a; DO(i,n) protect_input(&a->i[i],&ax.u);
  Reg vals = get_reg_mark(&ax.u,0); a->u |= 1<<vals;
  Asm as[l]; U al[l];
  DO(i, l) {
    if (i==l-1) ax.u = a->u;
    ax.l=0; ax.o=NO_REG;
    tt |= t[i]=apply_A_t(&ax, list_at(f,i), n, x);
    o[i]=ax.o; as[i]=ax.a; al[i]=ax.l;
  }

  U s=t_sizeof(tt);
  U c=next_pow_2(l); a_malloc(a, s*c, vals, 1<<a->i[0] | 1<<a->i[1]);

  DO(i, l) {
    a_append(a, al[i], as[i]); FREE(as[i]);
    if (PURE(t[i]) > PURE(tt)) {
      // TODO: wrap ax.o
    }
    ASM3(a, MOV_MR,vals,o[i],i*s);
  }
  UPDATE_A_(a,ax);

  choose_regs(a); Reg ll = a->o; L lt=NULL;
  a_malloc(a, sizeof(*lt), ll, 1<<vals);
#define MI MOV4_MI
#define MR MOV_MR
#define M(N,T,V) ASM3(a, M##T,ll,(V),(C)(Z)&lt->N)
  // ref      type       capacity     length       offset    value
  M(r,I,1); M(t,I,tt); M(c,I,f->c); M(l,I,f->l); M(o,I,0); M(p,R,vals);
#undef M
#undef MI
#undef MR

  UNPROTECT; a->u=au;
}

T apply_R_N(A a, N f, U n, T* x) { return 0; } // TODO
void apply_A_N(A a, N f, U n, T* x) {
  V fv=StrVget(names, f);
  //TODO if (!P(fv)) { return newE(strdup("Value error")); }
  return apply_A(a, fv, n, x);
}

// builtin.c
T apply_R_B(A a, B b, U n, T* x);
T apply_R_FB(A a, F f, U n, T* x);
void apply_A_B(A a, B b, U n, T* x);
void apply_A_FB(A a, F f, U n, T* x);

T apply_R_F(A a, F f, U n, T* x) {
  if (T(f->f)==B_t) return apply_R_FB(a, f, n, x);
  else { return 0; }
}
void apply_A_F(A a, F f, U n, T* x) {
  if (T(f->f)==B_t) return apply_A_FB(a, f, n, x);
}

T apply_R_Z(A a, Z z, U n, T* x) { request_cv(a, z); return Z_t; }
void apply_A_Z(A a, Z z, U n, T* x) {
  Reg r = use_cr(a);
  RegM ui=input_mask(a,n)&~a->u; a->u|=ui;
  DO(i,n) { a->u-=ui&1<<a->i[i]; a_del(a, x[i], a->i[i]); }
  if (a->o==NO_REG) a->o=r;
  else ASM(a, MOV, a->o, r);
}

void init_A(A a) {
  *(U*)&a->i=0; a->o=NO_REG; a->lc=0; a->u=REG_MASK;
#define D(N) a->N=MALLOC(MIN_ARR*sizeof(*a->N))
  D(ar); D(cr); D(cv); a->a=D(ts);
#undef D
  a->ar[0][0]=a->ar[0][1]=0; a->l=1;
}
T apply_R_(A a, V f, U n, T* x) {
#define LINE(T) case T##_t: return apply_R_##T(a,T(f),n,x);
  PURIFY(f); T t=T(f);
  switch (t) { LINE(O) LINE(L) LINE(N) LINE(B) LINE(F) LINE(Z) }
#undef LINE
}
T apply_R(A a, V f, U n, T* x) {
  U pi=*(U*)&a->i, ci=a->l; *(U*)&a->i=ci;
  if (a->l>=MIN_ARR && PURE(a->l)) {
    REALLOC(a->ar, 2*a->l);
    T** aa=(T**)&a->a; I d=a->ts-*aa;
    REALLOC(*aa, 2*a->l); a->ts=*aa+d;
  }
  a->l++; a->ar[ci][0]=a->ar[ci][1]=0;
  U lc = a->lc;
  T t=apply_R_(a,f,n,x); if (!t) { a->l--; return 0; }
  *(a->ts++) = t;
  a->ar[ci][1] = a->lc-lc;
  if (a->ar[pi][0] < a->ar[ci][0]) a->ar[pi][0] = a->ar[ci][0];
  *(U*)&a->i=pi;
  return t;
}
void apply_A(A a, V f, U n, T* x) {
  RegM v=start_A(a,n,0);
#define LINE(T) case T##_t: apply_A_##T(a,T(f),n,x); break;
  PURIFY(f); T t=T(f);
  switch (t) { LINE(O) LINE(L) LINE(N) LINE(B) LINE(F) LINE(Z) }
#undef LINE
  a->u |= v;
  if (v&1<<a->o) { Reg r=get_reg(v|a->u); ASM(a,MOV,r,a->o); a->o=r; }
  pop_regs(a, v); a->ts++;
}
T apply_A_t(A a, V f, U n, T* x) { apply_A(a,f,n,x); return a->ts[-1]; }

#define FREE_A(a) free(a->ts); free(a->ar); free(a->cr); free(a->cv)
T apply_R_full(A a, V f, U n, T* x) {
  init_A(a); U lc = a->lc;
  T t=apply_R(a,f,n,x); a->ts=(T*)a->a;
  a->ar[0][1] = a->lc-lc;
  if (t) { a->l=a->lc=0; } else { FREE_A(a); }
  return t;
}
RegM start_A(A a, U n, RegM u) {
  U* ar=*(a->ar++); U nc=min(ar[1],MAX_C_REG), nr=ar[0]; Reg lc=0;
  DO(i,nc) lc += (a->cr[a->lc+i]==NO_REG);
  RegM pop = push_regs(a, a->u & ((1<<NO_REG)-(1<<(NO_REG-lc))));
  U j=0; DO(i,nc) if (a->cr[a->lc+i]==NO_REG) {
    Reg r=NO_REG-1-j++; a->cr[a->lc+i]=r; Z z=a->cv[i]; a->u|=1<<r;
    if (z&~(((1L)<<32)-1)) ASM(a, MOV_RI, r, z);
    else ASM(a, MOV4_RI, r, z);
  }
  RegM v=0, un=REG_NEVER|u|input_mask(a,n)|~a->u; a->u|=u;
  U nm = num_marked(~a->u & ((1<<max(nr,8))-1));
  if(nm<nr)DO(i,nr-nm) v|=1<<get_reg(un|v); a->u-=v; pop|=push_regs(a, v);
  return pop;
}
void apply_A_full(A a, V f, U n, T* x) {
  T* ts=a->ts; U (*ar)[2]=a->ar-1;
  apply_A(a,f,n,x);
  a->ts=ts; a->ar=ar; FREE_A(a);
}

void *asm_mmap(size_t length) {
  void *mem = mmap(NULL, length, PROT_EXEC|PROT_READ|PROT_WRITE,
    (MAP_PRIVATE|MAP_ANON), -1, 0);
  return (mem == MAP_FAILED) ? NULL : mem;
}
P finish_A(A a, RegM r) {
  pop_regs(a, r);
  ASM_RAW(a, RET);
  P f=asm_mmap(a->l); memcpy(f,a->a,a->l); FREE(a->a); return f;
}
void asm_unmap(A a, P f) { munmap(f,a->l); }

void asm_jump(A a, UC cond, U label) {
  I rel = label-a->l;
  if (rel==(UC)rel) ASM(a, JX, rel, cond);
  else ASM(a, J4X, rel, cond);
}

#define A(op) ASM3(a,op,o,i,e)
C asm_load_stub(A a, T t, Reg o, Reg i, Reg e) {
  switch (t) {
    case B_t: case C_t: A(MOV1_RM_STUB); return 0;
    case E_t: case O_t: case F_t: case N_t: case Q_t: case L_t:
    case Z_t: A(MOV_RM_STUB); return 3;
    case R_t: A(MOVSD_RM_STUB); return 3;
    /* TODO K */
  }
}
C asm_write_stub(A a, T t, Reg o, Reg i, Reg e) {
  switch (t) {
    case B_t: case C_t: A(MOV1_MR_STUB); return 0;
    case E_t: case O_t: case F_t: case N_t: case Q_t: case L_t:
    case Z_t: A(MOV_MR_STUB); return 3;
    case R_t: A(MOVSD_MR_STUB); return 3;
    /* TODO K */
  }
}
#undef A
void asm_load(A a, T t, Reg o, Reg i) {
  if (IMPURE(t)) {
    // TODO
  } else {
    ASM_MOV_PRE(a, t);
    asm_load_stub(a,t,o,i,0);
    ASM(a, AD_RM0,o,i);
  }
}
void asm_write(A a, T t, Reg o, Reg i) {
  if (IMPURE(t)) {
    // TODO
  } else {
    ASM_MOV_PRE(a, t);
    asm_write_stub(a,t,o,i,0);
    ASM(a, AD_MR0,o,i);
  }
}
void asm_load_at(A a, T t, Reg o, Reg i, Reg e) {
  if (IMPURE(t)) {
    ASM(a, MOV,o,e);
    ASM(a, SHLI1,o,4);
    ASM(a, ADD,o,i);
  } else {
    ASM_MOV_PRE(a, t);
    UC s=asm_load_stub(a,t,o,i,e);
    ASM_RAW(a, AD_RMRS(o,i,e,s));
  }
}
void asm_write_at(A a, T t, Reg o, Reg i, Reg e) {
  if (IMPURE(t)) {
    // TODO
  } else {
    ASM_MOV_PRE(a, t);
    UC s=asm_write_stub(a,t,o,i,e);
    ASM_RAW(a, AD_MRRS(o,i,e,s));
  }
}

void a_RfromV(A a, Reg o, Reg i) {
  V*v=NULL;
  ASM(a, CMP4_MI,i,R_t);
  ASM3(a, MOV_RM,i,i,(U)(Z)&P(*v));
  ASM(a, MOVSD_RM0,o,i);
  ASM(a, JE,0,-); U j=a->l;
  ASM(a, MOVQ,i,o);
  ASM(a, CVTSI2SD,o,i);
  ((C*)a->a)[j-1] = a->l-j;
}
void a_RfromT(A a, T t, Reg o, Reg i) {
  if (IMPURE(t)) a_RfromV(a,o,i);
  else if (t==Z_t) ASM(a, CVTSI2SD,o,i);
  else if (t==R_t && o!=i) ASM(a, MOVSD,o,i);
}

Reg reg_args[] = { REG_ARG0, REG_ARG1, REG_ARG2,
                   REG_ARG3, REG_ARG4, REG_ARG5 };
