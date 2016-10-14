#include <string.h>
#include <sys/mman.h>
#include "type.h"
#include "name.h"
#include "asm.h"

// TODO support for compilers other than gcc
Reg get_reg(RegM u) { return __builtin_ctz(~u); }
Reg get_reg_mark(RegM *u, RegM v) {
  Reg r=get_reg(*u|v); *u|=1<<r; return r;
}
RegM input_mask(A a) { RegM u=0; DDO(i,a->n) u|=1<<a->i[i]; return u; }
I choose_reg(A a) {
  Reg i=a->i[0], o=a->o;
  if (i==NO_REG_NM) i=get_reg_mark(&a->u, 1<<o);
  if (o==NO_REG) o = (i<NO_REG && !(a->u&1<<i)) ? i : get_reg(a->u);
  if (i==NO_REG) i = o;
  a->i[0]=i; a->o=o; return i!=o;
}
I choose_regs(A a) {
  I n=a->n; Reg *i=a->i, o=a->o;
  DDO(j,n) if (i[j]==NO_REG_NM) i[j]=get_reg_mark(&a->u, 1<<o);
  RegM ui=a->u; DO(j,n) if (i[j]==NO_REG) i[j]=get_reg_mark(&ui,0);
  I ii=0; if (o==NO_REG) {
    while (ii<n  &&  a->u & 1<<a->i[ii]) ii++;
    a->o = (ii==n) ? get_reg(a->u) : i[ii];
  } else {
    while (ii<n && o!=a->i[ii]) ii++;
  }
  return ii;
}

void protect_input(Reg *i, RegM *u) {
  if (*i<NO_REG) *u|=1<<*i; else *i=NO_REG_NM;
}

void a_append(A a, I l, Asm aa) {
  I nl = a->l+l; if (!nl) return;
  I n = next_pow_2(nl);
  if (a->l*2 <= n) { a->a = a->l ? realloc(a->a, n) : malloc(n); }
  memcpy(a->a+a->l, aa, l); a->l=nl;
}

void apply_A_O(A a, O f, I n, T* x) {
  I l=f->l; T t[l];
  AS ax=*a; Reg iF[l];
  RegM ua=ax.u; DDO(i,n) protect_input(&a->i[i],&ax.u);
  DO(i,l) {
    if (i==l-1) ax.u = ua;
    ax.o = NO_REG;
    // TODO shortcut errors
    ax.t=0; apply_A(&ax, f->x[i], n, x);
    if (ax.t==0) {a->l=ax.l; a->a=ax.a; a->t=0; return;}
    t[i] = ax.t; iF[i] = ax.o; ax.u |= ua |= 1<<ax.o;
  }
  a->l=ax.l; a->a=ax.a;

  AS af=*a; af.n=l; af.i=iF;
  apply_A(&af, f->f, l, t);
  a->o=af.o; a->t=af.t; a->l=af.l; a->a=af.a;
}

#define EACH_REG(U,R) for (RegM ui=U; R=get_reg(~ui), ui; ui-=1<<R)
#define DEACH_REG(U,R) Reg R; EACH_REG(U,R)
Prot2State clear_regs(A a, RegM u) {
  RegM uc = u&a->u, ui=input_mask(a);
  RegM uu = a->u|u|ui;
  DDO(ii,a->n) {
    Reg i=a->i[ii]; RegM si=1<<i;
    if (i>=NO_REG || u&si) {
      Reg i_ = a->i[ii] = get_reg(uu);
      uu |= 1<<i_;
      if (u&si) ASM(a, MOV,i_,i);
      else if (i==NO_REG_NM) a->u|=1<<i_;
    }
  }
  Reg o=a->o; if (u&1<<o) a->o=get_reg(uu);
  DEACH_REG(uc,r) { ASM(a,PUSH,r,-); }
  return (Prot2State){uc,o};
}
void pop_regs_2(A a, RegM pop, Reg o) {
  if (o<NO_REG && o!=a->o) { ASM(a, MOV,o,a->o); a->o=o; }
  pop_regs(a, pop);
}

Prot3State clear_regs_3(A a, RegM u) {
  RegM ui=input_mask(a);
  RegM p1 = ui&u&a->u, p2 = ((ui|a->u)&u)^p1, uu = a->u|u|ui;
  DDO(ii,a->n) {
    Reg i=a->i[ii]; RegM si=1<<i;
    if (i>=NO_REG || p2&si) {
      Reg i_ = a->i[ii] = get_reg(uu);
      uu |= 1<<i_;
      if (p2&si) ASM(a, MOV,i_,i);
      else if (i==NO_REG_NM) a->u|=1<<i_;
    }
  }
  Reg r; RegM m=0;
  EACH_REG(p2,r) { ASM(a,PUSH,r,-); }
  EACH_REG(p1,r) { ASM(a,PUSH,r,-); m|=get_reg(uu|m); }
  return (Prot3State){p1,m,p2};
}
void clear_regs_post(A a, RegM mov, RegM pop) {
  Reg rs[8*sizeof(RegM)]; I i=0;
  DEACH_REG(pop,r) {
    Reg m=get_reg(mov); mov-=1<<m;
    ASM(a, MOV,m,r); rs[i++]=r;
  }
  while(--i>=0) { ASM(a,POP,rs[i],-); }
}
void pop_regs(A a, RegM pop) {
  Reg rs[8*sizeof(RegM)]; I i=0; DEACH_REG(pop,r) rs[i++]=r;
  while(--i>=0) { ASM(a,POP,rs[i],-); }
}

RegM push_regs(A a, RegM u) {
  RegM uc = u & REG_SAVE; DEACH_REG(uc,r) { ASM(a,PUSH,r,-); } return uc;
}

// REG_ARG0, REG_RES, and res must be modifiable
// Allocate l bytes and place the pointer at res.
void a_malloc(A a, I l, Reg res, RegM keep) {
  RegM pop = push_regs(a, keep);
  ASM(a, MOV4_RI, res,(I)(Z)&malloc);
  ASM(a, MOV4_RI, REG_ARG0,l);
  ASM(a, CALL, res,-);
  if (res!=REG_RES) ASM(a, MOV, res,REG_RES);
  pop_regs(a, pop);
}

typedef void (*del_t)(V); del_t del_S(T);
void a_del(A a, T t, Reg i) {
  del_t del=del_S(t); if (!del) return;
  RegM pop = push_regs(a, a->u);
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

void apply_A_L(A a, L f, I n, T* x) {
  I l=f->l; T t[l], tt=0; Reg o[l]; RegM au=a->u;

  PROTECT_1of2(REG_SAVE);

  // Don't add any code to a; store in as
  AS ax=*a; DDO(i,n) protect_input(&a->i[i],&ax.u);
  Reg vals = get_reg_mark(&ax.u,0); a->u |= 1<<vals;
  Asm as[l]; I al[l];
  DO(i, l) {
    if (i==l-1) ax.u = a->u;
    ax.l=0; ax.o=NO_REG; ax.t=0; apply_A(&ax, list_at(f,i), n, x);
    o[i]=ax.o; as[i]=ax.a; al[i]=ax.l;
    if (ax.t==0) {DDO(j,i+1)if(al[i])FREE(as[i]); a->t=0; return;}
    tt |= t[i]=ax.t;
  }

  I s=t_sizeof(tt);
  I c=next_pow_2(l); a_malloc(a, s*c, vals, 1<<a->i[0] | 1<<a->i[1]);

  DO(i, l) {
    a_append(a, al[i], as[i]); FREE(as[i]);
    if (PURE(t[i]) > PURE(tt)) {
      // TODO: wrap ax.o
    }
    ASM3(a, MOV_MR,vals,o[i],i*s);
  }

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

  PROTECT_2of2; a->u=au; a->t=L_t;
}

void apply_A_N(A a, N f, I n, T* x) {
  V fv=StrVget(names, f);
  //TODO if (!P(fv)) { return newE(strdup("Value error")); }
  return apply_A(a, fv, n, x);
}

// builtin.c
void apply_A_B(A a, B b, I n, T* x);
void apply_A_FB(A a, F f, I n, T* x);

void apply_A_F(A a, F f, I n, T* x) {
  if (T(f->f)==B_t) return apply_A_FB(a, f, n, x);
}

void apply_A_Z(A a, Z z, I n, T* x) {
  if (a->o==NO_REG) a->o=get_reg(a->u);
  RegM ui=input_mask(a)&~a->u; a->u|=ui;
  DDO(i,n) { a->u-=ui&1<<a->i[i]; a_del(a, x[i], a->i[i]); }
  if (z&~(((1L)<<32)-1)) ASM(a, MOV_RI, a->o, z);
  else ASM(a, MOV4_RI, a->o, z);
  a->t=Z_t;
}

void apply_A(A a, V f, I n, T* x) {
#define LINE(T) case T##_t: return apply_A_##T(a,T(f),n,x);
  PURIFY(f); T t=T(f);
  switch (t) { LINE(O) LINE(L) LINE(N) LINE(B) LINE(F) LINE(Z) }
  //Asm call apply(f,n,...)
#undef LINE
}

void *asm_mmap(size_t length) {
  void *mem = mmap(NULL, length, PROT_EXEC|PROT_READ|PROT_WRITE,
    (MAP_PRIVATE|MAP_ANON), -1, 0);
  return (mem == MAP_FAILED) ? NULL : mem;
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
  ASM3(a, MOV_RM,i,i,(UI)(Z)&P(*v));
  ASM(a, MOVSD_RM0,o,i);
  ASM(a, JE,0,-); I j=a->l;
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

S apply_SA(V f, I n, T* x) {
  AS as; A a=&as; a->n=n; a->o=0; a->u=REG_MASK; a->l=0; a->t=0;
  Reg ai[n]; a->i=ai; DDO(i,n) a->i[i]=reg_args[i];

  ASM(a,PUSH,REG_ARG2,-);
  DO(i,n) { V*v=NULL; ASM3(a,MOV_RM,a->i[i],REG_ARG4,(UI)(Z)&v[i].p); }
  DO(i,n) { asm_load(a,x[i],a->i[i],a->i[i]); }

  apply_A(a,f,n,x);
  if (!a->t) { FREE(a->a); return (S){0,NULL,NULL}; }

  ASM(a,POP,REG_ARG2,-);
  asm_write(a,a->t,REG_ARG2,REG_RES);
  ASM_RAW(a,RET);

  Asm aa = asm_mmap(a->l); memcpy(aa,a->a,a->l); FREE(a->a);
  return (S){a->t,aa,NULL};
}
V apply_S(V f, I n, T* x) {
  S s=apply_SA(f, n, x); return (s.f) ? newS(s) : cpy(f);
}
