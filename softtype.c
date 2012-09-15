/* apply */
T apply1_T(V f, T l) { T x[1]; x[0]=l; return apply_T(f,1,x); }
T apply2_T(V f, T l, T r) { T x[2]; x[0]=l; x[1]=r; return apply_T(f,2,x); }

T apply_T(V f, I n, T* x) {
#define LINE(T) case T##_t: t+=apply_##T##_T(T(f),n,x); break;
  if (T(f) & CONST_t) return T(f);
  T t=0; if (T(f) & (O_t+L_t)) {
    switch (T(f)) {LINE(O) LINE(L)} return t;
  } else {
    I d = dom_T(f,n,x);
    if ((d+1)==1<<n) switch (T(f)) {LINE(B) LINE(F) LINE(N) LINE(Q)}
    else return fmap_T(f,n,x,d);
  }
#undef LINE
}

T apply_O_T(O o, I n, T* x) {
  T t[o.l];
  DDO(j, o.l) { t[j]=apply_T(o.x[j], n, x); }
  return apply_T(o.f, o.l, t);
}
T apply_F_T(F f, I n, T* x) {
  switch (T(f.f)) {
    case B_t: return apply_T_FB(f, n, x);
    //case Q_t: return apply_T_FQ(f, n, x);
  }
}
T apply_N_T(N m, I n, T* x) { return apply_T(StrVget(names, m), n, x); }
T apply_Q_T(Q q, I n, T* x) { return E_t; }
T apply_L_T(L l, I n, T* x) { return L_t; }


/* dom */
I dom_T(V f, I n, T* x) {
#define LINE(T) case T##_t: return dom_T_##T(T(f),n,x);
  switch (*f) {LINE(B) LINE(F) LINE(N) LINE(Q)}
#undef LINE
}

I dom_T_F(F f, I n, T* x) {
  switch (T(f.f)) {
    case B_t: return dom_T_FB(f, n, x);
    //case Q_t: return apply_FQ(f, n, x);
  }
}
I dom_T_N(N f, I n, T* x) { return dom_true(n); }
I dom_T_Q(Q f, I n, T* x) { return dom_true(n); }


/* fmap */
T mapclass_T(T t) {
  switch (t) {
#define CASE(T) case T##_t:
    ON_TYPES(CONST, CASE) return CONST_X;
    ON_TYPES(FUNC , CASE) return FUNC_X;
    case L_t: return LIST_X;
#undef CASE
  }
}

T fmap_T(V f, I n, T* x, I d) {
  T t=~0;
  DDO(i, n) if (! (d&1<<i)) {
    t &= mapclass_T(x[i]);
    if (!t) return E_t;
  }
  switch (t) {
    case CONST_X: return E_t;
    case FUNC_X:  return O_t;
    case LIST_X:  return L_t + E_t;
  }
}
