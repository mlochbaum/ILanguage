V apply1(V f, V l) {
  V x[1]; x[0]=l; V v=apply(f,1,x); return v;
}
V apply2(V f, V l, V r) {
  V x[2]; x[0]=l; x[1]=r; V v=apply(f,2,x); return v;
}

V apply1d(V f, V l) { V v=apply1(f, l); del(f); return v; }
V apply2d(V f, V l, V r) { V v=apply2(f, l, r); del(f); return v; }

V apply_Ptr(T t, Ptr p, I n, V* x) {
#define LINE(T) case T##_t: return apply_##T(*(T*)p,n,x); break;
  if (!PURE(t)) return apply(*(V*)p, n, x);
  if (t & CONST_t) { DDO(i,n) del(x[i]); return wrapPtr(t,p); }
  V v; if (t & (O_t+L_t)) {
    switch (t) {LINE(O) LINE(L)}
  } else {
    I d = dom_Ptr(t,p,n,x);
    if ((d+1)==1<<n) switch (t) {LINE(B) LINE(F) LINE(N) LINE(Q)}
    else return fmap_Ptr(t,p,n,x,d);
  }
#undef LINE
}

V apply(V f, I n, V* x) {
  return apply_Ptr(T(f), V(f), n, x);
}

V apply_O(O o, I n, V* x) {
  V xt[n]; V xx[o->l];
  DDO(j, o->l-1) { DDO(i,n)xt[i]=cpy(x[i]); xx[j]=apply(o->x[j], n, xt); }
  xx[o->l-1]=apply(o->x[o->l-1], n, x);
  return apply(o->f, o->l, xx);
}
V apply_F(F f, I n, V* x) {
  switch (T(f->f)) {
    case B_t: return apply_FB(f, n, x);
    //case Q_t: return apply_FQ(f, n, x);
  }
}
V apply_N(N m, I n, V* x) {
  return apply(StrVget(names, m), n, x);
}
V apply_Q(Q q, I n, V* x) {
  return Err("Dude, I have no clue what you're talking about.");
}
V apply_L(L l, I n, V* x) {
  if (!(l->t & (NCONST_t))) { DDO(i,n) del(x[i]); return newL(l); }
  V xt[n]; DECL_ARR(V, v, l->c);
  DDO(i, l->l-1) {
    DDO(j,n)xt[j]=cpy(x[j]); v[i] = apply_Ptr(l->t,LIST_PTR_AT(l,i), n, xt);
  } v[l->l-1]=apply_Ptr(l->t, LIST_PTR_AT(l,l->l-1), n, x);
  return wrapList(l->l, v);
}
