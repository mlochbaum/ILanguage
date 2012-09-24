V apply1(V f, V l) {
  V x[1]; x[0]=l; V v=apply(f,1,x); return v;
}
V apply2(V f, V l, V r) {
  V x[2]; x[0]=l; x[1]=r; V v=apply(f,2,x); return v;
}

V apply1d(V f, V l) { V v=apply1(f, l); del(f); return v; }
V apply2d(V f, V l, V r) { V v=apply2(f, l, r); del(f); return v; }

T apply1_T(V f, T l) { T x[1]; x[0]=l; return apply_T(f,1,x); }
T apply2_T(V f, T l, T r) { T x[2]; x[0]=l; x[1]=r; return apply_T(f,2,x); }

T apply_T(V f, I n, T* x) {
#define LINE(T) case T##_t: t|=apply_T_##T(T(f),n,x); break;
  T tf=T(f);
  if (tf & CONST_t) return tf;
  T t=0; if (tf & (O_t+L_t)) {
    switch (tf) {LINE(O) LINE(L)} return t;
  } else {
    I d = dom_T(f,n,x);
    switch (tf) {LINE(B) LINE(F) LINE(N) LINE(Q)}
    if ((d+1)!=1<<n) t|=fmap_T(f,n,x,d);
    return t;
  }
#undef LINE
}
void apply_P(P v, V f, I n, V* x) {
#define LINE(T) case T##_t: return apply_P_##T(v,*(T*)p,n,x); break;
  T t=T(f); P p=P(f);
  if (!PURE(t)) { V f=*(V*)p; return apply_P(v, f, n, x); }
  if (t & CONST_t) { DDO(i,n) del(x[i]); return valcpy(v, p, t); }
  if (t & (O_t+L_t)) {
    switch (t) {LINE(O) LINE(L)}
  } else {
    I d = dom(f,n,x);
    if ((d+1)==1<<n) switch (t) {LINE(B) LINE(F) LINE(N) LINE(Q)}
    else return fmap_P(v,f,n,x,d);
  }
#undef LINE
}

V apply(V f, I n, V* x) {
  T t[n]; DDO(i,n) t[i]=T(x[i]);
  V v; T(v)=apply_T(f, n, t); P(v)=malloc(t_sizeof(T(v)));
  apply_P(P(v), f, n, x); return v;
}


T apply_T_O(O o, I n, T* x) {
  T t[o->l];
  DDO(j, o->l) { t[j]=apply_T(o->x[j], n, x); }
  return apply_T(o->f, o->l, t);
}
void apply_P_O(P p, O o, I n, V* x) {
  V xt[n]; V xx[o->l];
  DDO(j, o->l-1) { DDO(i,n)xt[i]=cpy(x[i]); xx[j]=apply(o->x[j], n, xt); }
  xx[o->l-1]=apply(o->x[o->l-1], n, x);
  return apply_P(p, o->f, o->l, xx);
}


T apply_T_F(F f, I n, T* x) {
  switch (T(f->f)) {
    case B_t: return apply_T_FB(f, n, x);
    //case Q_t: return apply_T_FQ(f, n, x);
  }
}
void apply_P_F(P p, F f, I n, V* x) {
  switch (T(f->f)) {
    case B_t: return apply_P_FB(p, f, n, x);
    //case Q_t: return apply_P_FQ(p, f, n, x);
  }
}

T apply_T_N(N m, I n, T* x) { return apply_T(StrVget(names, m), n, x); }
void apply_P_N(P p, N m, I n, V* x) {
  return apply_P(p, StrVget(names, m), n, x);
}

T apply_T_Q(Q q, I n, T* x) { return E_t; }
void apply_P_Q(P p, Q q, I n, V* x) {
  *(E*)p = strdup("Dude, I have no clue what you're talking about.");
}

T apply_T_L(L l, I n, T* x) { return L_t; }
void apply_P_L(P p, L l, I n, V* x) {
  if (!(l->t & (NCONST_t))) { DDO(i,n) del(x[i]); *(L*)p=l; return; }
  // TODO
  V xt[n]; DECL_ARR(V, vs, l->c);
  DDO(i, l->l-1) {
    DDO(j,n)xt[j]=cpy(x[j]);
    apply_P(P(vs[i]), wrapP(l->t,LIST_PTR_AT(l,i)), n, xt);
  } apply_P(P(vs[l->l-1]), wrapP(l->t,LIST_PTR_AT(l,i)), n, xt);
  // wrapList(l->l, v);
}
