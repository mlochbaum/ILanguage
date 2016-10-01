#include <string.h>
#include <sys/mman.h>
#include "type.h"
#include "name.h"
#include "asm.h"

// TODO support for compilers other than gcc
Reg a_first_reg(RegM u) { return __builtin_ctz(~u); }
I choose_reg(A a) {
  Reg i=a->i[0], o=a->o;
  if (i==NO_REG_NM) a->u|=1<< (i = a_first_reg(a->u | 1<<o));
  if (o==NO_REG) o = (i<NO_REG && !(a->u&1<<i)) ? i : a_first_reg(a->u);
  if (i==NO_REG) i = o;
  a->i[0]=i; a->o=o; return i!=o;
}
I choose_regs(A a) {
  I n=a->n; Reg *i=a->i, o=a->o;
  DDO(j,n) if (i[j]==NO_REG_NM) { a->u|=1<< (i[j] = a_first_reg(a->u|1<<o)); }
  RegM ui=a->u; DO(j,n) if (i[j]==NO_REG) { ui|=1<< (i[j] = a_first_reg(ui)); }
  I ii=0; if (o==NO_REG) {
    while (ii<n  &&  a->u & 1<<a->i[ii]) ii++;
    a->o = (ii==n) ? a_first_reg(a->u) : i[ii];
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
    apply_A(&ax, f->x[i], n, x);
    t[i] = ax.t; iF[i] = ax.o; ax.u |= ua |= 1<<ax.o;
  }
  a->l=ax.l; a->a=ax.a;

  AS af=*a; af.n=l; af.i=iF;
  apply_A(&af, f->f, l, t);
  a->o=af.o; a->t=af.t; a->l=af.l; a->a=af.a;
}

/*
 *  i    ui       u
 *  MOV  PUSH     PUSH
 *       MOV,POP
 *                POP
 */
#define EACH_REG(U,R) Reg R; for (RegM ui=U; R=a_first_reg(~ui), ui; ui-=1<<R)
RegM clear_regs(A a, RegM u) {
  RegM uc = u & a->u, ui=0; DDO(ii,a->n) ui|=1<<a->i[ii];
  DO(ii,a->n) { Reg i=a->i[ii]; RegM si=1<<i; if (u&si) {
    Reg i_=a_first_reg(a->u|u|ui); ASM(a, MOV,i_,i); a->i[ii]=i_;
    ui-=si; ui|=1<<i_; if (uc&si) { /* TODO */ a->u-=si; uc-=si; }
  } }
  EACH_REG(uc,r) { ASM(a,PUSH,r,-); }
  return uc;
}
RegM push_regs(A a, RegM u) {
  RegM uc = u & REG_SAVE; EACH_REG(uc,r) { ASM(a,PUSH,r,-); } return uc;
}
void pop_regs(A a, RegM pop) {
  Reg rs[8*sizeof(RegM)]; I i=0; EACH_REG(pop,r) rs[i++]=r;
  while(--i>=0) { ASM(a,POP,rs[i],-); }
}

// REG_ARG0, REG_RES, and res must be modifiable
// Allocate l bytes and place the pointer at res.
void a_malloc(A a, I l, Reg res, RegM keep) {
  RegM pop = push_regs(a, keep);
  ASM(a, MOV_RI, res,(I)(Z)&malloc);
  ASM(a, MOV_RI, REG_ARG0,l);
  ASM(a, CALL, res,-);
  pop_regs(a, pop);
  if (res!=REG_RES) ASM(a, MOV, res,REG_RES);
}

void apply_A_L(A a, L f, I n, T* x) {
  I l=f->l; T t[l], tt=0; Reg o[l]; RegM au=a->u;

  RegM pop = clear_regs(a, 1<<REG_RES|1<<REG_ARG0);

  // Don't add any code to a; store in as
  AS ax=*a; DDO(i,n) protect_input(&a->i[i],&ax.u);
  Reg vals = a_first_reg(ax.u|1<<REG_RES|1<<REG_ARG0);
  a->u |= 1<<vals; ax.u |= 1<<vals;
  Asm as[l]; I al[l];
  DO(i, l) {
    if (i==l-1) ax.u = a->u;
    ax.l=0; ax.o=NO_REG; apply_A(&ax, list_at(f,i), n, x);
    o[i]=ax.o; as[i]=ax.a; al[i]=ax.l;
    tt |= t[i]=ax.t;
  }

  I s=t_sizeof(tt);
  I c=next_pow_2(l); a_malloc(a, s*c, vals, 1<<a->i[0] | 1<<a->i[1]);

  DO(i, l) {
    a_append(a, al[i], as[i]); FREE(as[i]);
    if (PURE(t[i]) > PURE(tt)) {
      // TODO: wrap ax.o
    }
    ASM_RAW(a, MOV_MR(vals,o[i],i*s));
  }

  choose_regs(a); Reg ll = a->o; L lt=NULL;
  a_malloc(a, sizeof(*lt), ll, 1<<vals);
#define M(N,T,V) ASM_RAW(a, MOV_M##T(ll,(V),(C)(Z)&lt->N))
  // ref      type       capacity     length       offset    value
  M(r,I,1); M(t,I,tt); M(c,I,f->c); M(l,I,f->l); M(o,I,0); M(p,R,vals);
#undef M

  pop_regs(a, pop); a->u=au; a->t=L_t;
}

void apply_A_N(A a, N f, I n, T* x) {
  V fv=StrVget(names, f);
  //TODO if (!P(fv)) { return newE(strdup("Value error")); }
  return apply_A(a, fv, n, x);
}

// builtin.c
void apply_A_B(A a, B b, I n, T* x);

void apply_A_F(A a, F f, I n, T* x) {
  // flip
  if (f->l==1 && T(f->f)==B_t && B(f->f)=='~' && n==1) {
    AS ax=*a; ax.n=2; Reg axi[2]={ax.i[0],ax.i[0]}; ax.i=axi;
    T t[2]={x[0],x[0]};
    apply_A(&ax, f->x[0], 2, t);
    a->t=ax.t; a->o=ax.o; a->i[0]=axi[0]; a->l=ax.l; a->a=ax.a;
    return;
  }
  // hook 1
  if (f->l==2 && T(f->f)==B_t && B(f->f)=='h' && n==1) {
    AS ax=*a; ax.n=1; ax.o=NO_REG; protect_input(ax.i, &ax.u);
    apply_A(&ax, f->x[0], 1, x);

    ax.n=2; Reg axi[2]={ax.o,ax.i[0]}; ax.i=axi; ax.o=a->o; ax.u=a->u;
    T t[2]={ax.t,x[0]};
    apply_A(&ax, f->x[1], 2, t);

    a->t=ax.t; a->o=ax.o; a->l=ax.l; a->a=ax.a;
    return;
  }
  // hook 2
  if (f->l==2 && T(f->f)==B_t && B(f->f)=='h' && n==2) {
    AS ax=*a; ax.n=1; ax.o=NO_REG; ax.u|=1<<a->i[1];
    apply_A(&ax, f->x[0], 1, x);

    ax.n=2; I i=a->i[0]; ax.i[0]=ax.o; ax.o=a->o; ax.u=a->u; x[0]=ax.t;
    apply_A(&ax, f->x[1], 2, x);
    a->i[0]=i;

    a->t=ax.t; a->o=ax.o; a->l=ax.l; a->a=ax.a;
    return;
  }
  // compose
  if (f->l==2 && T(f->f)==B_t && B(f->f)=='O' && n==2) {
    AS ax=*a, af=*a; Reg afi[2]; T tf[2];

    ax.n=1; ax.i=a->i; ax.o=NO_REG; ax.u|=1<<a->i[1];
    apply_A(&ax, f->x[0], 1, x); afi[0]=ax.o; tf[0]=ax.t;

    ax.i=a->i+1; ax.u=a->u|1<<ax.o; ax.o=NO_REG;
    apply_A(&ax, f->x[0], 1, x+1); afi[1]=ax.o; tf[1]=ax.t;

    ax.n=2; ax.i=afi; ax.o=a->o; ax.u=a->u;
    apply_A(&ax, f->x[1], 2, tf);

    a->t=ax.t; a->o=ax.o; a->l=ax.l; a->a=ax.a;
    return;
  }
}

void apply_A(A a, V f, I n, T* x) {
#define LINE(T) case T##_t: return apply_A_##T(a,T(f),n,x);
  PURIFY(f); T t=T(f);
  switch (t) { LINE(O) LINE(L) LINE(N) LINE(B) LINE(F) }
  //Asm call apply(f,n,...)
#undef LINE
}

void *asm_mmap(size_t length) {
  void *mem = mmap(NULL, length, PROT_EXEC|PROT_READ|PROT_WRITE,
    (MAP_PRIVATE|MAP_ANON), -1, 0);
  return (mem == MAP_FAILED) ? NULL : mem;
}

S apply_SA(V f, I n, T* x) {
  AS a; a.n=n; a.o=0; a.u=REG_MASK; a.l=0; a.t=0;
  Reg ai[n]; a.i=ai; DDO(i,n) ai[i]=7-i; // TODO More than 2 args

  if (n==1) ASM_RAW(&a,PRE1); else ASM_RAW(&a,PRE2);
  apply_A(&a,f,n,x);
  if (!a.t) { FREE(a.a); return (S){0,NULL,NULL}; }
  ASM_RAW(&a,POST);
  Asm aa = asm_mmap(a.l); memcpy(aa,a.a,a.l); FREE(a.a);
  return (S){a.t,aa,NULL};
}
