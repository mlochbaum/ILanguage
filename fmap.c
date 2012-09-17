X mapclass_Ptr(T t, Ptr p) {
  X x; switch (t) {
#define CASE(T) case T##_t:
    ON_TYPES(CONST, CASE) x.t=CONST_X; break;
    ON_TYPES(FUNC , CASE) x.t=FUNC_X; break;
    case L_t: x.t=LIST_X; x.v=&((*(L*)p)->l); break;
#undef CASE
  }
  return x;
}
X mapclass(V v) { return mapclass_Ptr(T(v), V(v)); }

I mapclasseq(X x, X y) {
  return x.t==y.t && (x.t!=LIST_X || *((I*)x.v)==*((I*)y.v));
}

V constant(V v) {
  if (T(v)&CONST_t) return v;
  DECL_V(B,f); B(f)='k'; WRAP(V*,vv,v); return makeF(f,1,vv);
}

V fmap_Ptr(T t, Ptr p, I n, V* x, I d) {
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
    case FUNC_X:  { DECL_ARR(V,xx,n); DDO(i,n) {
                    if (d&1<<i) xx[i]=constant(x[i]); else xx[i]=x[i];
                  } return makeO(wrapPtr(t,p),n,xx); }
    case LIST_X:  return fmap_LIST_Ptr(t, p, n, x, d, *(I*)m.v);
  }
}
V fmap(V f, I n, V* x, I d) { return fmap_Ptr(T(f), V(f), n, x, d); }

V fmap_LIST_Ptr(T t, Ptr p, I n, V* x, I d, I l) {
  DECL_ARR(V, v, l); I i[n], c[n]; V xi[n];
  DDO(j, n) if (!(d&1<<j)) { i[j]=L(x[j])->o; c[j]=L(x[j])->c; }
  DDO(k, l) {
    DO(j,n) {
      if (d&1<<j) { xi[j]=cpy(x[j]); }
      else { xi[j]=listV_at(x[j],i[j]); i[j]++; if(i[j]==c[j]) i[j]=0; }
    }
    v[k] = apply_Ptr(t, p, n, xi);
  }
  DO(j, n) del(x[j]);
  return wrapList(l, v);
}
