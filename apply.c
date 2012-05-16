V apply1(V f, V l) {
  V x[1]; x[0]=l; V v=apply(f,1,x); return v;
}
V apply2(V f, V l, V r) {
  V x[2]; x[0]=l; x[1]=r; V v=apply(f,2,x); return v;
}

V apply(V f, I n, V* x) {
#define LINE(T) case T##_t: v=apply_##T((T)f->v,n,x); break;
  if (f->t & CONST_t) { DDO(i,n) del(x[i]); return f; }
  V v; if (f->t & (O_t+L_t+A_t)) {
    switch (f->t) {LINE(O) LINE(L) LINE(A)}
  } else {
    I d = dom(f,n,x);
    if ((d+1)==1<<n) switch (f->t) {LINE(B) LINE(F) LINE(N) LINE(Q)}
    else return fmap(f,n,x,d);
  }
  del(f); return v;
#undef LINE
}

V apply_O(O o, I n, V* x) {
  DDO(i, n) increfn(x[i], o->l-1);
  V xx[o->l];
  DDO(j, o->l) xx[j]=apply(cpy(o->x[j]), n, x);
  return apply(cpy(o->f), o->l, xx);
}
V apply_F(F f, I n, V* x) {
  switch (f->f->t) {
    case B_t: return apply_FB(f, n, x);
    //case Q_t: return apply_FQ(f, n, x);
  }
}
V apply_N(N m, I n, V* x) {
  return Err("What's in a name?");
}
V apply_Q(Q q, I n, V* x) {
  return Err("Dude, I have no clue what you're talking about.");
}
V apply_L(L l, I n, V* x) {
  DDO(i,n) increfn(x[i],l->l-1);
  DECL_ARR(V, v, l->c);
  I j=0; I i1; FOR_EACH(i, l) { v[j] = apply(cpy(l->v[i]), n, x); j++; }
  return wrapList(l->l, v);
}
V apply_A(A a, I n, V* x) {
  DDO(i,n) increfn(x[i],a->l-1);
  DECL_ARR(V, v, a->c);
  DO(i, a->l) v[i] = apply(arr_at(a,i), n, x);
  return wrapList(a->l, v);
}
