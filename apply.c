V apply1(V f, V l) {
  V x[1]; x[0]=l; V v=apply(f,1,x); return v;
}
V apply2(V f, V l, V r) {
  V x[2]; x[0]=l; x[1]=r; V v=apply(f,2,x); return v;
}

V apply(V f, I n, V* x) {
#define LINE(T) case T##_t: v=apply_##T(T(f),n,x); break;
  if (T(f) & CONST_t) { DDO(i,n) del(x[i]); return f; }
  V v; if (T(f) & (O_t+L_t)) {
    switch (T(f)) {LINE(O) LINE(L)}
  } else {
    I d = dom(f,n,x);
    if ((d+1)==1<<n) switch (T(f)) {LINE(B) LINE(F) LINE(N) LINE(Q)}
    else return fmap(f,n,x,d);
  }
  del(f); return v;
#undef LINE
}

V apply_O(O o, I n, V* x) {
  V xt[n]; V xx[o.l];
  DDO(j, o.l-1) { DDO(i,n)xt[i]=cpy(x[i]); xx[j]=apply(cpy(o.x[j]), n, xt); }
  xx[o.l-1]=apply(cpy(o.x[o.l-1]), n, x);
  return apply(cpy(o.f), o.l, xx);
}
V apply_F(F f, I n, V* x) {
  switch (T(f.f)) {
    case B_t: return apply_FB(f, n, x);
    //case Q_t: return apply_FQ(f, n, x);
  }
}
V apply_N(N m, I n, V* x) {
  V v=cpy(StrVget(names, m)); return apply(v, n, x);
}
V apply_Q(Q q, I n, V* x) {
  return Err("Dude, I have no clue what you're talking about.");
}
V apply_L(L l, I n, V* x) {
  V xt[n]; DECL_ARR(V, v, l.c);
  DDO(i, l.l-1) {
    DDO(j,n)xt[j]=cpy(x[j]); v[i] = apply(list_at(l,i), n, xt);
  } v[l.l-1]=apply(list_at(l,l.l-1), n, x);
  return wrapList(l.l, v);
}
