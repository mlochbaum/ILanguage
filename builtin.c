#include "builtin.h"

#define ON_ALL_NUMS(F) F(1) F(2) F(11) F(12) F(21) F(22)

D_D1(true){return 1;}   D_D2(true){return 3;}
D_D11(true){return 1;}  D_D12(true){return 3;}
D_D21(true){return 1;}  D_D22(true){return 3;}

D_S1(false){return 0;}   D_S2(false){return 0;}
D_S11(false){return 0;}  D_S12(false){return 0;}
D_S21(false){return 0;}  D_S22(false){return 0;}

#define DEF(n) D_T##n(v){return ALL_t;}
ON_ALL_NUMS(DEF)
#undef DEF

#include "name.c"
// #include "compose.c"
// #include "mapping.c"
// #include "arith.c"
// #include "compare.c"
// #include "list.c"
// #include "string.c"
void builtin_init() {
#define INIT(n) \
  B_s##n[i]=&false_s##n; B_t##n[i]=NULL; \
  B_d##n[i]= &true_d##n; B_p##n[i]=NULL;
  DDO(i,256) { ON_ALL_NUMS(INIT) }
#undef INIT

  name_init();
  //compose_init();
  //map_init();
  //arith_init();
  //compare_init();
  //list_init();
  //string_init();
}

/////////////// Main definitions ///////////
void FfromB_P(P p, B b, I n, V* x) {
  DECL_ARR(V,xx,n); DDO(i,n) xx[i]=x[i];
  DECL_V(B,f); B(f)=b;
  *(F*)p=wrapF(f,n,xx);
}

void apply_P_B1(P p, B b, V* x) {
  P1 f=B_p1[b]; if(!f) return FfromB_P(p,b,1,x);
  else return f(p,x[0]);
}
void apply_P_B2(P p, B b, V* x) {
  P2 f=B_p2[b]; if(!f) return FfromB_P(p,b,2,x);
  else return f(p,x[0],x[1]);
}
void apply_P_B(P p, B b, I n, V* x) {
  switch (n) {
    case 1: return apply_P_B1(p,b,x);
    case 2: return apply_P_B2(p,b,x);
  }
}

I dom_B(B b, I n, V* x) {
  switch (n) {
    case 1: return B_d1[b](x[0]);
    case 2: return B_d2[b](x[0],x[1]);
  }
}


T apply_T_B1(B b, T* x) {
  T1 f=B_t1[b]; if(!f) return F_t; else return f(x[0]);
}
T apply_T_B2(B b, T* x) {
  T2 f=B_t2[b]; if(!f) return F_t; else return f(x[0],x[1]);
}
T apply_T_B(B b, I n, T* x) {
  switch (n) {
    case 1: return apply_T_B1(b,x);
    case 2: return apply_T_B2(b,x);
  }
}

I dom_T_B(B b, I n, T* x) {
  switch (n) {
    case 1: return B_s1[b](x[0]);
    case 2: return B_s2[b](x[0],x[1]);
  }
}


void apply_P11(P p, P11 f, V* x, V* xx) { return f(p, x[0], xx[0]); }
void apply_P12(P p, P12 f, V* x, V* xx) { return f(p, x[0], xx[0], xx[1]); }
void apply_P21(P p, P21 f, V* x, V* xx) { return f(p, x[0], x[1], xx[0]); }
void apply_P22(P p, P22 f, V* x, V* xx) { return f(p, x[0], x[1], xx[0], xx[1]); }

void apply_P_FB(P p, F f, I n, V* xx) {
#define LINE1(y,z,yz) case (2*y+z): { \
  B b=B(f->f); P##yz ff=B_p##yz[b]; \
  if(!ff) { DDO(i,z)del(xx[i]); *(E*)p = "Unknown builtin"; } \
  return apply_P##yz(p, ff, f->x, xx); }
#define LINE(a,b) LINE1(a,b,a##b)
  switch (2*f->l + n) { LINE(1,1) LINE(1,2) LINE(2,1) LINE(2,2) }
#undef LINE
#undef LINE1
}

T apply_T11(T11 f, V* x, T* xx) { return f(T(x[0]), xx[0]); }
T apply_T12(T12 f, V* x, T* xx) { return f(T(x[0]), xx[0], xx[1]); }
T apply_T21(T21 f, V* x, T* xx) { return f(T(x[0]), T(x[1]), xx[0]); }
T apply_T22(T22 f, V* x, T* xx) { return f(T(x[0]), T(x[1]), xx[0], xx[1]); }

T apply_T_FB(F f, I n, T* xx) {
#define LINE1(y,z,yz) case (2*y+z): { \
  B b=B(f->f); T##yz ff=B_t##yz[b]; \
  if(!ff) return E_t; \
  return apply_T##yz(ff, f->x, xx); }
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

I dom_T_FB(F f, I n, T* xx) {
  B b=B(f->f); V* x=f->x;
  switch (10*f->l + n) {
    case 11: return B_s11[b](T(x[0]), xx[0]);
    case 12: return B_s12[b](T(x[0]), xx[0], xx[1]);
    case 21: return B_s21[b](T(x[0]), T(x[1]), xx[0]);
    case 22: return B_s22[b](T(x[0]), T(x[1]), xx[0], xx[1]);
  }
}
