#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "type.h"

// Find next power of two for length: or with
// all down bitshifts, then add one.
U next_pow_2(U l) {
  I c=l-1; U i=32; while (i) { c |= c>>i; i>>=1; } return c+1;
}
U t_sizeof(T t) {
#if UINTPTR_MAX == 0xffffffffffffffff
  return IMPURE(t) ? 16 : 1 + 7*(t>2) + (t&8);
#else
#define LINE(T) case T##_t: return sizeof(T);
  switch(t){ON_TYPES(ALL,LINE) default: return sizeof(V);}
#undef LINE
#endif
}

V TP(T t, P p) { V v; T(v)=t; P(v)=p; return v; }

O wrapO(V f, U l, V* x) {
  DECL(O,oo); oo->r=1; oo->f=f; oo->l=l; oo->x=x; return oo;
}
F wrapF(V f, U l, V* x) {
  DECL(F,ff); ff->r=1; ff->f=f; ff->l=l; ff->x=x; return ff;
}
L wrapL(T t, U c, U l, U o, P p) {
  DECL(L,ll); ll->r=1; ll->t=t; ll->c=c; ll->l=l; ll->o=o; ll->p=p; return ll;
}

#define DECL_V(type, v) V v; T(v)=type##_t; P(v)=MALLOC(sizeof(type))
#define SET_NEW(T) V new##T(T vv) { DECL_V(T,v); T(v)=vv; return v; } \
  void set##T(V v,T t) { if (!PURE(v.t)) V(v)=new##T(t); else T(v)=t; }
ON_TYPES(ALL, SET_NEW);    // functions new##T : T->V
#undef SET_NEW
#undef DECL_V

// Custom wrap functions
L wrapArray(T t, U l, P p) {
  U c=next_pow_2(l);
  p=realloc(p, c*t_sizeof(t));
  return wrapL(t,c,l,0,p);
}
L wrapList(U l, V* v) {
  U c=next_pow_2(l);
  T t=0; DO(i,l)t|=T(v[i]);
  if(PURE(t)){
    U s=t_sizeof(t); P p=MALLOC(c*s);
    DO(i,l) { memcpy(p+i*s, P(v[i]), s); FREE(P(v[i])); }
    L r=wrapL(t,c,l,0,p); FREE(v); return r;
  } else {
    v=realloc(v,c*sizeof(V)); return wrapL(t,c,l,0,v);
  }
}
L wrapStr(Str s) { return wrapArray(C_t, strlen(s), s); }
V makeStr(Str s) { return newL(wrapStr(s)); }


/////////////////// Utilities ////////////////////
typedef void (*del_t)(V); del_t del_S(T);
void delO(V); void delN(V); void delL(V); void delV(V);
void delO(V v) {
  O o=O(v); if (--o->r) return;
  ddel(o->f); DO(i,o->l) ddel(o->x[i]); FREE(o->x); FREE(o);
}
void delN(V v) { FREE(N(v)); }
void delL(V v) {
  L l=L(v); if (--l->r) return;
  del_t d=del_S(l->t); if (d) { DO(i,l->l) d(list_at(l,i)); }
  FREEL(l);
}
void delV(V v) { ddel(V(v)); }
del_t del_S(T t) {
  if (IMPURE(t)) return &delV;
  switch (t) {
    case N_t: case Q_t: return &delN;
    case O_t: case F_t: return &delO;
    case L_t: return &delL;
    default: return NULL;
  }
}
void del(V v) { del_t d=del_S(T(v)); if (d) d(v); }
void ddel(V v) { del(v); FREE(P(v)); }

P arrcpy(P aa, U s, U l, U c, U o) {
  l*=s; c*=s; o*=s; P a=MALLOC(c);
  memcpy(a,aa+o,min(c,l+o)-o); memcpy(a,aa,max(0,(I)(l+o-c)));
  return a;
}
void mv_P(V p, V v) {
  if (!PURE(T(p))) V(p)=cpy1(v);
  else if (T(p)==T(v)) {
    memcpy(P(p), P(v), t_sizeof(T(v)));
  } else printf("Internal error: type mismatch in mv_P");
}
void mv_Pd(V p, V v) {
  if (!PURE(T(p))) V(p)=v;
  else if (T(p)==T(v)) {
    memcpy(P(p), P(v), t_sizeof(T(v))); FREE(P(v));
  } else printf("Internal error: type mismatch in mv_P");
}
void valcpy(P p, P pp, T t) { // from pp to p
  U s=t_sizeof(t);
  if (PURE(t)) switch (t) {
    case E_t: case N_t: case Q_t: *(E*)p=strdup(*(E*)pp); break;
    case O_t: case F_t: case L_t: (**(U**)pp)++; // Fall through
    default: memcpy(p, pp, s); break;
  } else *(V*)p=cpy(*(V*)pp);
}
void cp_P(V p, V v) { valcpy(P(p), P(v), T(p)); }

V cpy(V v) {
  V vv; T(vv)=T(v); P(vv)=MALLOC(t_sizeof(T(v)));
  valcpy(P(vv),P(v),T(v)); return vv;
}

V cpy1(V v) {
  V vv; T(vv)=T(v); U s=t_sizeof(T(v));
  P(vv)=MALLOC(s); memcpy(P(vv),P(v),s); return vv;
}

void deln(U n, V* v) { DO(i,n) del(v[i]); FREE(v); }
V* cpyn(U n, V* v) {DECL_ARR(V,vv,n); DO(i,n)vv[i]=cpy(v[i]); return vv;}

void get(V v) {
  if ((T(v)&COMP_t) && REF(v)>1) {
    REF(v)--; switch (T(v)) {
      case O_t:
        { O o=O(v); setO(v,wrapO(cpy(o->f), o->l, cpyn(o->l, o->x))); break;}
      case F_t:
        { F f=F(v); setF(v,wrapF(cpy(f->f), f->l, cpyn(f->l, f->x))); break;}
      case L_t:
        { L l=L(v); U s=t_sizeof(l->t); P p=MALLOC(s*l->c);
          DO(i,l->l) valcpy(p+i*s, LIST_PTR_ATS(l,i,s), l->t);
          setL(v,wrapL(l->t, l->c, l->l, 0, p)); break; }
    }
  }
}


// Numbers
// TODO: complex case
Z getZ(V v) { return Z(v); }
R getR(V v) {
  switch (T(v)) {
    case Z_t: return (R)Z(v);
    case R_t: return R(v);
  }
}

// List properties
V list_at(L l, U i) { return TP(l->t, LIST_PTR_AT(l,i)); }
V list_ats(L l, U i, U s) { return TP(l->t, LIST_PTR_ATS(l,i,s)); }
V listV_at(V v, U i) { return list_at(L(v), i); }
V listV_ats(V v, U i, U s) { return list_ats(L(v), i, s); }
