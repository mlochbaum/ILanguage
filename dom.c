#include "type.h"

// From builtin.c
I domu_T_B(B b, I n, T* x);
I doml_T_B(B b, I n, T* x);
I dom_FB(F f, I n, V* xx);
I doml_T_FB(F f, I n, T* xx);
I domu_T_FB(F f, I n, T* xx);

// Internal declarations
#define DECLARE_DOM(T) I dom_##T(T, I, V*);
DECLARE_DOM(B) DECLARE_DOM(F) DECLARE_DOM(N) DECLARE_DOM(Q)
#undef DECLARE_DOM

I dom_true(I n) { return (1<<n)-1; }

I dom_N(N f, I n, V* x) { return dom_true(n); }
I dom_Q(Q f, I n, V* x) { return dom_true(n); }

I doml_T_N(N f, I n, T* x) { return dom_true(n); }
I domu_T_N(N f, I n, T* x) { return dom_true(n); }
I doml_T_Q(Q f, I n, T* x) { return dom_true(n); }
I domu_T_Q(Q f, I n, T* x) { return dom_true(n); }

I doml_T_F(F f, I n, T* x) {
  switch (T(f->f)) {
    case B_t: return doml_T_FB(f, n, x);
    //case Q_t: return doml_T_FQ(f, n, x);
  }
}
I domu_T_F(F f, I n, T* x) {
  switch (T(f->f)) {
    case B_t: return domu_T_FB(f, n, x);
    //case Q_t: return domu_T_FQ(f, n, x);
  }
}
I dom_F(F f, I n, V* x) {
  switch (T(f->f)) {
    case B_t: return dom_FB(f, n, x);
    //case Q_t: return dom_FQ(f, n, x);
  }
}

I doml_T(V f, I n, T* x) {
#define LINE(T) case T##_t: return doml_T_##T(T(f),n,x);
  switch (T(f)) {LINE(B) LINE(F) LINE(N) LINE(Q)}
#undef LINE
}
I domu_T(V f, I n, T* x) {
#define LINE(T) case T##_t: return domu_T_##T(T(f),n,x);
  switch (T(f)) {LINE(B) LINE(F) LINE(N) LINE(Q)}
#undef LINE
}
I dom(V f, I n, V* x) {
#define LINE(T) case T##_t: return dom_##T(T(f),n,x);
  switch (T(f)) {LINE(B) LINE(F) LINE(N) LINE(Q)}
#undef LINE
}
