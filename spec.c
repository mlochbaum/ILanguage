V apply_S_O(O f, I n, T* x) {
  I l=f->l; T t[l]; DECL_ARR(V,s,l);
  DDO(i,l) { V v=f->x[i]; t[i]=apply_T(v,n,x); s[i]=apply_S(v,n,x); }
  return newO(wrapO(apply_S(f->f,l,t), l, s));
}

V apply_S_L(L f, I n, T* x) {
  DECL_ARR(V, vs, f->c);
  DDO(i, f->l) { vs[i] = apply_S(list_at(f,i),n,x); }
  return newL(wrapList(f->l, vs));
}

V apply_S_N(N f, I n, T* x) {
  return apply_S(StrVget(names, f), n, x);
}

V apply_S_B(B,I,T*);

V apply_S(V f, I n, T* x) {
#define LINE(T) case T##_t: return apply_S_##T(T(f),n,x);
  PURIFY(f); T t=T(f);
  switch (t) { LINE(O) LINE(L) LINE(N) LINE(B) }
  return cpy(f);
#undef LINE
}
