#include <string.h>
#include <sys/mman.h>
#include "type.h"
#include "name.h"
#include "asm.h"

void a_append(A a, I l, Asm aa) {
  I nl = a->l+l; if (!nl) return;
  I n = next_pow_2(nl);
  if (a->l*2 <= n) { a->a = a->l ? realloc(a->a, n) : malloc(n); }
  memcpy(a->a+a->l, aa, l); a->l=nl;
}

// TODO support for compilers other than gcc
Reg a_first_reg(RegM u) { return __builtin_ctz(~u); }

void apply_A_O(A a, O f, I n, T* x) {
  I l=f->l; T t[l];
  AS ax=*a; Reg iF[l];
  RegM ua=0; DDO(i,n) { ua |= 1<<a->i[i]; } ua&=~ax.u; ax.u|=ua;
  DO(i,l) {
    if (i==l-1) ax.u &= ~ua;
    ax.o = NO_REG;
    // TODO shortcut errors
    apply_A(&ax, f->x[i], n, x);
    t[i] = ax.t; iF[i] = ax.o; ax.u |= 1<<ax.o;
  }
  a->l=ax.l; a->a=ax.a;

  AS af=*a; af.n=l; af.i=iF;
  apply_A(&af, f->f, l, t);
  a->o=af.o; a->t=af.t; a->l=af.l; a->a=af.a;
}

// TODO
void apply_A_L(A a, L f, I n, T* x) {
  I l=f->l; T t[l], tt; Reg o[l];
  //Asm DECL_ARR(V, vs, f->c);
  AS ax=*a; DDO(i,n) { ax.u |= 1<<a->i[i]; }
  Asm as[l]; I al[l];
  DO(i, l) {
    if (i==l-1) ax.u = a->u;
    ax.l=0; ax.o=NO_REG; apply_A(&ax, list_at(f,i), n, x);
    o[i]=ax.o; as[i]=ax.a; al[i]=ax.l;
    tt |= t[i]=ax.t;
  }
  DO(i, l) {
    a_append(a, al[i], as[i]); FREE(as[i]);
    if (PURE(t[i]) > PURE(tt)) {
      // wrap ax.o
    }
    // mov vs[i] ax.o
  }
  //Asm return newL(wrapList(l, vs));
  a->t = L_t;
}

void apply_A_N(A a, N f, I n, T* x) {
  V fv=StrVget(names, f);
  //TODO if (!P(fv)) { return newE(strdup("Value error")); }
  return apply_A(a, fv, n, x);
}

I choose_reg(A a) {
  Reg i=a->i[0], o=a->o;
  if (i==NO_REG_NM) i = a_first_reg(a->u | 1<<o);
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
#define ASM_RAW(A, OP) \
  do { UC aa[] = OP; a_append(A, sizeof(aa), aa); } while(0)
#define ASM(A, OP,O,I) ASM_RAW(A, OP(O,I))
// builtin.c
void apply_A_B(A a, B b, I n, T* x) {
  if (b=='-' && n==1 && x[0]==Z_t) {
    if (choose_reg(a)) { ASM(a, MOV,a->o,a->i[0]); }
    ASM(a, NEG,-,a->o); a->t=Z_t;
    return;
  }
  if ((b=='+'||b=='-'||b=='*') && n==2 && x[0]==Z_t && x[1]==Z_t) {
    I ii = choose_regs(a);
    if (ii==n) { ASM(a, MOV,a->o,a->i[ii=0]); } // TODO lea for +
    switch (b) {
      case '+': ASM(a, ADD, a->o,a->i[1-ii]); break;
      case '-': ASM(a, SUB, a->o,a->i[1-ii]); break;
      case '*': ASM(a, IMUL,a->o,a->i[1-ii]); break;
    }
    a->t=Z_t; return;
  }
  if ((b=='m'||b=='M') && n==2 && x[0]==Z_t && x[1]==Z_t) {
    I ii = choose_regs(a);
    I ifm=(ii==n); if (ifm) ii=1;
    { I jj=b=='m'?ii:1-ii; ASM(a, CMP,a->i[1-jj],a->i[jj]); }
    if (ifm) { ASM(a, MOV,a->o,a->i[ii]); }
    ASM(a, CMOVLE,a->o,a->i[1-ii]); a->t=Z_t;
    return;
  }
}

void apply_A(A a, V f, I n, T* x) {
#define LINE(T) case T##_t: return apply_A_##T(a,T(f),n,x);
  PURIFY(f); T t=T(f);
  switch (t) { LINE(O) LINE(L) LINE(N) LINE(B) }
  //Asm call apply(f,n,...)
#undef LINE
}

void *asm_mmap(size_t length) {
  void *mem = mmap(NULL, length, PROT_EXEC|PROT_READ|PROT_WRITE,
    (MAP_PRIVATE|MAP_ANON), -1, 0);
  return (mem == MAP_FAILED) ? NULL : mem;
}

S apply_SA(V f, I n, T* x) {
  AS a; a.n=n; a.o=0; a.u=0; a.l=0; a.t=0;
  Reg ai[n]; a.i=ai; DDO(i,n) ai[i]=7-i; // TODO More than 2 args

  if (n==1) ASM_RAW(&a,PRE1); else ASM_RAW(&a,PRE2);
  apply_A(&a,f,n,x);
  if (!a.t) { FREE(a.a); return (S){0,NULL,NULL}; }
  ASM_RAW(&a,POST);
  Asm aa = asm_mmap(a.l); memcpy(aa,a.a,a.l); FREE(a.a);
  return (S){a.t,aa,NULL};
}
