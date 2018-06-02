#include <string.h>

#include "type.h"
#include "name.h"

// Outward-facing utilities
V apply1(V f, V l) { return apply(f,1,&l); }
V apply2(V f, V l, V r) { V x[2]; x[0]=l; x[1]=r; return apply(f,2,x); }

V apply1d(V f, V l) { V v=apply1(f, l); del(f); return v; }
V apply2d(V f, V l, V r) { V v=apply2(f, l, r); del(f); return v; }
V apply1_d(V f, V l) { V v=apply1(f, l); FREE(P(l)); return v; }
V apply2_d(V f, V l, V r) { V v=apply2(f, l, r); FREE(P(l)); FREE(P(r)); return v; }
V apply1dd(V f, V l) { V v=apply1(f, l); ddel(f); FREE(P(l)); return v; }
V apply2dd(V f, V l, V r) { V v=apply2(f, l, r); ddel(f); FREE(P(l)); FREE(P(r)); return v; }

T apply1_T(V f, T l) { return apply_T(f,1,&l); }
T apply2_T(V f, T l, T r) { T x[2]; x[0]=l; x[1]=r; return apply_T(f,2,x); }
void apply1_P(V v, V f, V l) { return apply_P(v,f,1,&l); }
void apply2_P(V v, V f, V l, V r) { V x[2]; x[0]=l; x[1]=r; return apply_P(v,f,2,x); }


// Internal declarations
#define DECLARE_APPLY(t) void apply_P_##t(V, t, U, V*); \
                         T apply_T_##t(t, U, T*);
ON_TYPES(NCONST,DECLARE_APPLY)
#undef DECLARE_APPLY
void apply_P_FB(V, F, U, V*);
//void apply_P_FQ(V, F, U, V*);
T apply_T_FB(F, U, T*);
//T apply_T_FQ(F, U, T*);


E err;
// Outward-facing main apply functions
T apply_T(V f, U n, T* x) {
#define LINE(T) case T##_t: t|=apply_T_##T(T(f),n,x); break;
  PURIFY(f); T tf=T(f);
  if (tf & CONST_t) return tf;
  T t=0; if (tf & (O_t+L_t)) {
    switch (tf) {LINE(O) LINE(L)} return t;
  } else {
    U dl = doml_T(f,n,x), du = domu_T(f,n,x);
    if ((dl+1)==1u<<n) switch (tf) {LINE(B) LINE(F) LINE(N) LINE(Q)}
    if ((du+1)!=1u<<n) t|=fmap_T(f,n,x,dl,du);
    return t;
  }
#undef LINE
}
void apply_P(V v, V f, U n, V* x) {
#define LINE(T) case T##_t: apply_P_##T(v,T(f),n,x); break;
  T t=T(f);
  if (t & CONST_t) { DO(i,n) del(x[i]); return mv_P(v, f); }
  B delp[n]; V op[n]; DO(i,n) {
    delp[i] = 0!=IMPURE(T(x[i]));
    if (delp[i]) { op[i]=x[i]; x[i]=V(x[i]); PURIFY_D(x[i]); }
  }
  if (t & (O_t+L_t)) {
    switch (t) {LINE(O) LINE(L)}
  } else {
    U d = dom(f,n,x);
    if ((d+1)==1u<<n) switch (t) {LINE(B) LINE(F) LINE(N) LINE(Q)}
    else fmap_P(v,f,n,x,d);
  }
  DO(i,n) if (delp[i]) { FREE(P(x[i])); x[i]=op[i]; } return;
#undef LINE
}

V apply(V f, U n, V* x) {
  T t[n]; DO(i,n) t[i]=T(x[i]);
  V v; T(v)=apply_T(f, n, t); P(v)=MALLOC(t_sizeof(T(v)));
  apply_P(v, f, n, x);
  if (err) { FREE(P(v)); v=newE(err); err=NULL; } else { PURIFY_D(v); }
  return v;
}


// Internal functions
T apply_T_O(O o, U n, T* x) {
  T t[o->l];
  DO(j, o->l) { t[j]=apply_T(o->x[j], n, x); }
  return apply_T(o->f, o->l, t);
}
void apply_P_O(V v, O o, U n, V* x) {
  V xt[n]; V xx[o->l];
  DO(j, o->l-1) {
    DO(i,n)xt[i]=cpy(x[i]);
    xx[j]=apply(o->x[j], n, xt);
    DO(i,n)FREE(P(xt[i]));
  }
  xx[o->l-1]=apply(o->x[o->l-1], n, x);
  apply_P(v, o->f, o->l, xx); DO(j,o->l) FREE(P(xx[j]));
}


T apply_T_F(F f, U n, T* x) {
  switch (T(f->f)) {
    case B_t: return apply_T_FB(f, n, x);
    //case Q_t: return apply_T_FQ(f, n, x);
  }
}
void apply_P_F(V v, F f, U n, V* x) {
  switch (T(f->f)) {
    case B_t: return apply_P_FB(v, f, n, x);
    //case Q_t: return apply_P_FQ(v, f, n, x);
  }
}

T apply_T_N(N m, U n, T* x) {
  V mv=StrVget(names, m);
  return P(mv) ? apply_T(mv, n, x) : E_t;
}
void apply_P_N(V v, N m, U n, V* x) {
  V mv=StrVget(names, m);
  if (!P(mv)) { DO(i,n) del(x[i]); ERR("Value error"); }
  return apply_P(v, mv, n, x);
}

T apply_T_Q(Q q, U n, T* x) { return E_t; }
void apply_P_Q(V v, Q q, U n, V* x) {
  ERR("Dude, I have no clue what you're talking about.");
}

T apply_T_L(L l, U n, T* x) { return L_t; }
V list_at_pure(L l, I i) { V r=list_at(l,i); PURIFY(r); return r; }
void apply_P_L(V p, L l, U n, V* x) {
  if (!(l->t & NCONST_t)) { DO(i,n) del(x[i]); l->r++; setL(p,l); return; }
  T tx[n]; DO(i,n) tx[i]=T(x[i]);
  T tr=0; DO(i,l->l) tr|=apply_T(list_at_pure(l,i),n,tx);
  U s=t_sizeof(tr); P lp=MALLOC(l->l*s);
  V xt[n];
  L ll = wrapL(tr,next_pow_2(l->l),l->l,0,lp);
  DO(i, l->l-1) {
    DO(j,n) xt[j]=cpy(x[j]);
    apply_P(TP(tr, lp+s*i), list_at_pure(l,i), n, xt);
    DO(j,n) FREE(P(xt[j]));
  } apply_P(TP(tr, lp+s*(l->l-1)), list_at_pure(l,l->l-1), n, x);
  if (!err) setL(p, ll);
}
