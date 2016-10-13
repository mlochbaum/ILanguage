#include <string.h>
#include "type.h"
#include "asm.h"

T mapclass_T(T t) {
  return ((t&CONST_t)?CONST_X:0) | ((t&FUNC_t)?FUNC_X:0)
    | ((t&L_t)?LIST_X:0);
}
X mapclass(V v) {
  X x; switch (T(v)) {
#define CASE(T) case T##_t:
    ON_TYPES(CONST, CASE) x.t=CONST_X; break;
    ON_TYPES(FUNC , CASE) x.t=FUNC_X; break;
    case L_t: x.t=LIST_X; x.v=&(L(v)->l); break;
#undef CASE
  }
  return x;
}

I mapclasseq(X x, X y) {
  return x.t==y.t && (x.t!=LIST_X || *((I*)x.v)==*((I*)y.v));
}

// TODO move/simplify
V constant(V v) {
  if (T(v)&CONST_t) return v;
  DECL(V*, vv); *vv=v; return newF(wrapF(newB('k'),1,vv));
}

V fmap(V f, I n, V* x, I d) {
  T t[n]; DDO(i,n) t[i]=T(x[i]);
  V v; T(v)=fmap_T(f,n,t,d,d); P(v)=MALLOC(t_sizeof(T(v)));
  fmap_P(v,f,n,x,d); return v;
}
T fmap_T(V f, I n, T* x, I dl, I du) { return fmap_TT(T(f),n,x,dl,du); }
T fmap_TT(T f, I n, T* x, I dl, I du) {
  T tl=~0, tu=~0;
  DDO(i, n) {
    if (! (dl&1<<i)) { tl &= mapclass_T(x[i]);
      if (! (du&1<<i)) tu &= mapclass_T(x[i]); }
  }
  if (!tu) return E_t;
  T t=0;
  if (tu & FUNC_X) t |= O_t;
  if (tu & LIST_X) t |= L_t;
  if (t==0 && (IMPURE(tu) || tu & CONST_X)) t = E_t;
  return t;
}

void fmap_LIST_P(V, V, I, V*, I, I);
void fmap_P(V v, V f, I n, V* x, I d) {
  X m={0,NULL};
  DDO(i, n) if (! (d&1<<i)) {
    X mt=mapclass(x[i]);
    if (!m.t) m=mt;
    else if (!mapclasseq(m,mt)) {
      DDO(j,n)del(x[j]); ERR("Incompatible mapclasses");
    }
  }
  switch (m.t) {
    case CONST_X: DO(i,n)del(x[i]); ERR("Domain error");
    case FUNC_X:  { DECL_ARR(V,xx,n); DO(i,n) {
                    if (d&1<<i) xx[i]=constant(cpy1(x[i]));
                    else xx[i]=cpy1(x[i]);
                  } setO(v, wrapO(cpy(f),n,xx)); return; }
    case LIST_X:  return fmap_LIST_P(v, f, n, x, d, *(I*)m.v);
  }
}


void fmap_LIST_P(V v, V f, I n, V* x, I d, I l) {
  I i,j; T ts[n]; I ss[n];
  DO(j,n) {
    ts[j] = d&1<<j ? T(x[j]) : L(x[j])->t; ss[j]=t_sizeof(ts[j]);
    if (!(d&1<<j)) get(x[j]);
  }
  T t = apply_T(f, n, ts); I s=t_sizeof(t);
  I c=next_pow_2(l); L ll = wrapL(t,c,l,0,MALLOC(c*s));
  V xi[n]; P end[n]; DO(j,n) {
    V *u=xi+j; I sj=ss[j]; T(*u)=ts[j];
    if (d&1<<j) {
      P(*u)=MALLOC(sj); end[j]=NULL;
    } else {
      L lj=L(x[j]); P(*u)=LP(lj) + sj*(lj->o-1); end[j]=LP(lj) + sj*lj->c;
    }
  }
  if (n==1) {
    L l0=L(x[0]);
    I len0=l0->c-l0->o;
    AS as; A a=&as; a->n=n; a->l=0; a->t=0;
    a->u=REG_MASK|1<<REG_ARG0|1<<REG_ARG1|1<<REG_ARG2;
    Reg rx=REG_RES; a->i=&rx; a->o=rx;
    Reg ri=get_reg_mark(&a->u,1<<REG_RES);

    ASM(a, XOR4,ri,ri); I label=a->l;
    asm_load_at(a,ts[0],rx,REG_ARG0,ri);
    apply_A(a,f,n,ts);
    if (a->t) {
      asm_write_at(a,t,REG_ARG1,rx,ri);
      ASM(a, ADDI1,ri,1);
      ASM(a, CMP,ri,REG_ARG2);
      ASM(a, JB,label-a->l,-);
      if (len0 < l) {
        ASM(a, MOV_RI,REG_ARG2,(Z)(l));
        ASM(a, SUBI4,REG_ARG0, ss[0]*l0->c);
        ASM(a, CMP,ri,REG_ARG2);
        ASM(a, JB,label-a->l,-);
      } else {
        len0=l;
      }
      ASM_RAW(a, RET);
      void (*ff)(P,P,Z); ff=asm_mmap(a->l); memcpy(ff,a->a,a->l);
      ff(LP(l0)+ss[0]*l0->o, LP(ll), len0);
    } else {
      P px0=P(*xi)+ss[0]; i=0; do { for (;i<len0;i++) {
          P(*xi)=px0+ss[0]*i;
          apply_P(TP(t, LP(ll) + s*i), f, n, xi);
          if (err) { DDO(k,i) del(TP(t, LP(ll) + s*k)); FREEL(ll); break; }
      } px0-=ss[0]*l0->c; len0=l; } while (i<l);
    }
    FREE(a->a);
  } else {
    DO(i, l) {
      DO(j,n) {
        V *u=xi+j;
        if (d&1<<j) cp_P(*u,x[j]);
        else { P(*u)+=ss[j]; if (end[j] == P(*u)) P(*u)=LP(L(x[j])); }
      }
      apply_P(TP(t, LP(ll) + s*i), f, n, xi);
      if (err) { DDO(k,i) del(TP(t, LP(ll) + s*k)); FREEL(ll); break; }
    }
  }
  if (err) { DO(j,n) { if (!(d&1<<j)) {
    V *u=xi+j;
    I k; for(k=i+1;k<l;k++) {
      P(*u)+=ss[j]; if (end[j] == P(*u)) P(*u)=LP(L(x[j]));
      del(*u);
    }
  } } }
  DO(i, n) {
    if (d&1<<i) { del(x[i]); FREE(P(xi[i])); }
    else { FREEL(L(x[i])); }
  } if (!err) setL(v,ll);
}
