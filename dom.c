#include "type.h"

// From builtin.c
U domu_T_B(B b, U n, T* x);
U doml_T_B(B b, U n, T* x);
U dom_FB(F f, U n, V* xx);
U doml_T_FB(F f, U n, T* xx);
U domu_T_FB(F f, U n, T* xx);

// Internal declarations
#define DECLARE_DOM(T) U dom_##T(T, U, V*);
DECLARE_DOM(B) DECLARE_DOM(F) DECLARE_DOM(N) DECLARE_DOM(Q)
#undef DECLARE_DOM

U dom_true(U n) { return (1<<n)-1; }

U dom_N(N f, U n, V* x) { return dom_true(n); }
U dom_Q(Q f, U n, V* x) { return dom_true(n); }

U doml_T_N(N f, U n, T* x) { return dom_true(n); }
U domu_T_N(N f, U n, T* x) { return dom_true(n); }
U doml_T_Q(Q f, U n, T* x) { return dom_true(n); }
U domu_T_Q(Q f, U n, T* x) { return dom_true(n); }

U doml_T_F(F f, U n, T* x) {
  switch (T(f->f)) {
    case B_t: return doml_T_FB(f, n, x);
    //case Q_t: return doml_T_FQ(f, n, x);
  }
}
U domu_T_F(F f, U n, T* x) {
  switch (T(f->f)) {
    case B_t: return domu_T_FB(f, n, x);
    //case Q_t: return domu_T_FQ(f, n, x);
  }
}
U dom_F(F f, U n, V* x) {
  switch (T(f->f)) {
    case B_t: return dom_FB(f, n, x);
    //case Q_t: return dom_FQ(f, n, x);
  }
}

U doml_T(V f, U n, T* x) {
#define LINE(T) case T##_t: return doml_T_##T(T(f),n,x);
  switch (T(f)) {LINE(B) LINE(F) LINE(N) LINE(Q)}
#undef LINE
}
U domu_T(V f, U n, T* x) {
#define LINE(T) case T##_t: return domu_T_##T(T(f),n,x);
  switch (T(f)) {LINE(B) LINE(F) LINE(N) LINE(Q)}
#undef LINE
}
U dom(V f, U n, V* x) {
#define LINE(T) case T##_t: return dom_##T(T(f),n,x);
  switch (T(f)) {LINE(B) LINE(F) LINE(N) LINE(Q)}
#undef LINE
}
