#define ON_ALL_NUMS(F) F(1) F(2) F(11) F(12) F(21) F(22)
#undef V // TODO

#define D_D1(func) I func##_d1(V l)
#define D_D2(func) I func##_d2(V l, V r)
#define D_F1(func) V func##_f1(V l)
#define D_F2(func) V func##_f2(V l, V r)
typedef I (*D1)(V);     D1 B_d1[256];
typedef I (*D2)(V, V);  D2 B_d2[256];
typedef V (*F1)(V);     F1 B_f1[256];
typedef V (*F2)(V, V);  F2 B_f2[256];

#define D_D11(func) I func##_d11(V l, V ll)
#define D_D12(func) I func##_d12(V l, V ll, V rr)
#define D_D21(func) I func##_d21(V l, V r, V ll)
#define D_D22(func) I func##_d22(V l, V r, V ll, V rr)
#define D_F11(func) V func##_f11(V l, V ll)
#define D_F12(func) V func##_f12(V l, V ll, V rr)
#define D_F21(func) V func##_f21(V l, V r, V ll)
#define D_F22(func) V func##_f22(V l, V r, V ll, V rr)
typedef I (*D11)(V,V);      D11 B_d11[256];
typedef I (*D12)(V,V,V);    D12 B_d12[256];
typedef I (*D21)(V,V,V);    D21 B_d21[256];
typedef I (*D22)(V,V,V,V);  D22 B_d22[256];
typedef V (*F11)(V,V);      F11 B_f11[256];
typedef V (*F12)(V,V,V);    F12 B_f12[256];
typedef V (*F21)(V,V,V);    F21 B_f21[256];
typedef V (*F22)(V,V,V,V);  F22 B_f22[256];
#define EXTERN_BUILTINS  \
  extern F1 B_f1[256];   \
  extern F2 B_f2[256];   \
  extern D1 B_d1[256];   \
  extern D2 B_d2[256];   \
  extern D11 B_d11[256]; \
  extern D12 B_d12[256]; \
  extern D21 B_d21[256]; \
  extern D22 B_d22[256]; \
  extern F11 B_f11[256]; \
  extern F12 B_f12[256]; \
  extern F21 B_f21[256]; \
  extern F22 B_f22[256]
#define V(vv) ((vv).v)

D_D1(true){return 1;}   D_D2(true){return 3;}
D_D11(true){return 1;}  D_D12(true){return 3;}
D_D21(true){return 1;}  D_D22(true){return 3;}

#include "name.c"
#include "compose.c"
#include "mapping.c"
#include "arith.c"
#include "compare.c"
#include "list.c"
#include "string.c"
void builtin_init() {
#define INIT(n) B_d##n[i]=&true_d##n; B_f##n[i]=NULL;
  DDO(i,256) { ON_ALL_NUMS(INIT) }
#undef INIT

  name_init();
  compose_init();
  map_init();
  arith_init();
  compare_init();
  list_init();
  string_init();
}

/////////////// Main definitions ///////////
V FfromB(B b, I n, V* x) {
  DECL_ARR(V,xx,n); DDO(i,n) xx[i]=x[i];
  DECL_V(B,f); B(f)=b; return makeF(f,n,xx);
}

V apply_B1(B b, V* x) {
  F1 f=B_f1[b]; if(!f) return FfromB(b,1,x);
  else return f(x[0]);
}
V apply_B2(B b, V* x) {
  F2 f=B_f2[b]; if(!f) return FfromB(b,2,x);
  else return f(x[0],x[1]);
}

V apply_B(B b, I n, V* x) {
  switch (n) {
    case 1: return apply_B1(b,x);
    case 2: return apply_B2(b,x);
  }
}

I dom_B(B b, I n, V* x) {
  switch (n) {
    case 1: return B_d1[b](x[0]);
    case 2: return B_d2[b](x[0],x[1]);
  }
}

V apply_F11(F11 f, V* x, V* xx) { return f(x[0], xx[0]); }
V apply_F12(F12 f, V* x, V* xx) { return f(x[0], xx[0], xx[1]); }
V apply_F21(F21 f, V* x, V* xx) { return f(x[0], x[1], xx[0]); }
V apply_F22(F22 f, V* x, V* xx) { return f(x[0], x[1], xx[0], xx[1]); }

V apply_FB(F f, I n, V* xx) {
#define LINE1(y,z,yz) case (2*y+z): { \
  B b=B(f->f); del(f->f); F##yz ff=B_f##yz[b]; \
  if(!ff) { DDO(i,y)del(f->x[i]); DO(i,z)del(xx[i]); \
    return Err("Unknown builtin"); } \
  return apply_F##yz(ff, f->x, xx); }
#define LINE(a,b) LINE1(a,b,a##b)
  switch (2*f->l + n) { LINE(1,1) LINE(1,2) LINE(2,1) LINE(2,2) }
#undef LINE
#undef LINE1
}

I dom_FB(F f, I n, V* xx) {
  B b=B(f->f); V* x=f->x;
  switch (10*f->l + n) {
    case 11: return B_d11[b](x[0], xx[0]);
    case 12: return B_d12[b](x[0], xx[0], xx[1]);
    case 21: return B_d21[b](x[0], x[1], xx[0]);
    case 22: return B_d22[b](x[0], x[1], xx[0], xx[1]);
  }
}
