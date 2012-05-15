V fmap(V f, I n, V* x, I d) {
  X m={0,NULL};
  DDO(i, n) if (! (d&1<<i)) {
    X mt=mapclass(x[i]);
    if (!m.t) m=mt;
    else if (!mapclasseq(m,mt)) {
      DDO(j,n)del(x[j]); return Err("Incompatible mapclasses");
    }
  }
  switch (m.t) {
    case CONST_X: DO(i,n)del(x[i]); return Err("Domain error");
    case FUNC_X:  return makeO(f, n, (V*)cpy_Ptr((Ptr*)x,n));
    case LIST_X:  return fmap_LIST(f, n, x, d, *(I*)m.v);
  }
}

V fmap_LIST(V f, I n, V* x, I d, I l) {
  DECL_ARR(V, v, l); I i[n], c[n]; V xi[n];
  increfn(f,l);
  DDO(j, n) {
    if (d&1<<j) { increfn(x[j], l); xi[j]=x[j]; }
    else { i[j]=LIST_O(x[j]); c[j]=LIST_C(x[j]); }
  }
  DDO(k, n) {
    DO(j,n) if (!(d&1<<j)) {
      xi[j]=list_at(x[j],i[j]);
      i[j]++; if(i[j]==c[j]) i[j]=0;
    }
    v[k] = apply(f, n, xi);
  }
  DO(j, n) del(x[j]);
  return wrapList(l, v);
}
