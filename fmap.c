#include <string.h>
#include "type.h"

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
  T t=0; if (!PURE(tu)) t|=E_t;
  if (tu & CONST_X) t |= E_t;
  if (tu & FUNC_X ) t |= O_t;
  if (tu & LIST_X ) t |= L_t + (n>1)*E_t;
  return t;
}
void fmap_P(V v, V f, I n, V* x, I d) {
  X m={0,NULL};
  DDO(i, n) if (! (d&1<<i)) {
    X mt=mapclass(x[i]);
    if (!m.t) m=mt;
    else if (!mapclasseq(m,mt)) {
      DDO(j,n)del(x[j]); return setE(v, strdup("Incompatible mapclasses"));
    }
  }
  switch (m.t) {
    case CONST_X: DO(i,n)del(x[i]); return setE(v, strdup("Domain error"));
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
  T t = apply_T(f, n, ts); I s=t_sizeof(t); f=apply_S(f, n, ts);
  I c=next_pow_2(l); L ll = wrapL(t,c,l,0,MALLOC(c*s));
  V xi[n]; P end[n]; DO(j,n) {
    V *u=xi+j; I sj=ss[j]; T(*u)=ts[j];
    if (d&1<<j) P(*u)=MALLOC(sj);
    else {
      L lj=L(x[j]); P(*u)=lj->p + sj*(lj->o-1); end[j]=lj->p + sj*lj->c;
    }
  }
  DO(i, l) {
    DO(j,n) {
      V *u=xi+j;
      if (d&1<<j) cp_P(*u,x[j]);
      else { P(*u)+=ss[j]; if (end[j] == P(*u)) P(*u)=L(x[j])->p; }
    }
    apply_P(TP(t, ll->p + s*i), f, n, xi);
  }
  DO(i, n) {
    if (d&1<<i) { del(x[i]); FREE(P(xi[i])); }
    else { FREE(L(x[i])->p); FREE(L(x[i])); }
  } ddel(f); return setL(v,ll);
}
