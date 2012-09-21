I dom_T(V f, I n, T* x) {
#define LINE(T) case T##_t: return dom_T_##T(T(f),n,x);
  switch (T(f)) {LINE(B) LINE(F) LINE(N) LINE(Q)}
#undef LINE
}
I dom(V f, I n, V* x) {
#define LINE(T) case T##_t: return dom_##T(T(f),n,x);
  switch (T(f)) {LINE(B) LINE(F) LINE(N) LINE(Q)}
#undef LINE
}

I dom_T_F(F f, I n, T* x) {
  switch (T(f->f)) {
    case B_t: return dom_T_FB(f, n, x);
    //case Q_t: return apply_FQ(f, n, x);
  }
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

I dom_T_N(N f, I n, T* x) { return dom_true(n); }
I dom_T_Q(Q f, I n, T* x) { return dom_true(n); }
