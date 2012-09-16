I dom_Ptr(T t, Ptr p, I n, V* x) {
#define LINE(T) case T##_t: return dom_##T(*(T*)p,n,x);
  switch (t) {LINE(B) LINE(F) LINE(N) LINE(Q)}
#undef LINE
}

I dom_F(F f, I n, V* x) {
  switch (T(f->f)) {
    case B_t: return dom_FB(f, n, x);
    //case Q_t: return apply_FQ(f, n, x);
  }
}
I dom_N(N f, I n, V* x) { return dom_true(n); }
I dom_Q(Q f, I n, V* x) { return dom_true(n); }

I dom_true(I n) { return (1<<n)-1; }
