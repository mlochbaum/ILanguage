I dom(V f, I n, V* x) {
#define LINE(T) case T##_t: return dom_##T((T)f->v,n,x);
  switch (f->t) {LINE(B) LINE(F) LINE(N) LINE(Q)}
#undef LINE
}

// TODO
I dom_F(V f, I n, V* x) { return dom_true(n); }
I dom_N(V f, I n, V* x) { return dom_true(n); }
I dom_Q(V f, I n, V* x) { return dom_true(n); }

I dom_true(I n) { return (1<<n)-1; }
