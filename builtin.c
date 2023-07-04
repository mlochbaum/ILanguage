#include <string.h>
#include "builtin.h"

// Builtin arrays are externed elsewhere; actually declare them here.
// (defined in builtin_auto.h)
DECLARE_BUILTINS

#define ON_ALL_NUMS(F) F(1) F(2) F(11) F(12) F(21) F(22)

D_D1(true){return 1;}   D_D2(true){return 3;}
D_D11(true){return 1;}  D_D12(true){return 3;}
D_D21(true){return 1;}  D_D22(true){return 3;}

D_L1(true){return 1;}   D_L2(true){return 3;}
D_L11(true){return 1;}  D_L12(true){return 3;}
D_L21(true){return 1;}  D_L22(true){return 3;}

D_L1(false){return 0;}   D_L2(false){return 0;}
D_L11(false){return 0;}  D_L12(false){return 0;}
D_L21(false){return 0;}  D_L22(false){return 0;}

#define DEF(n) D_T##n(V){return ALL_t & ~E_t;}
ON_ALL_NUMS(DEF)
#undef DEF
#define DEF(T,n) D_T##n(T){return T##_t;}
#define DEFO(n) DEF(O,n)
ON_ALL_NUMS(DEFO)
#undef DEFO
#define DEFZ(n) DEF(Z,n)
ON_ALL_NUMS(DEFZ)
#undef DEFZ
#define DEFR(n) DEF(R,n)
ON_ALL_NUMS(DEFR)
#undef DEFR
#define DEFL(n) DEF(L,n)
ON_ALL_NUMS(DEFL)
#undef DEFL
#undef DEF

D_T1(l){return l;}   D_T2(l){return l;}
D_T11(l){return T(l);}  D_T12(l){return T(l);}
D_T21(l){return T(l);}  D_T22(l){return T(l);}
D_T2(r){return r;}   D_T21(r){return T(r);}  D_T22(r){return T(r);}
D_T11(ll){return ll;}  D_T12(ll){return ll;}
D_T21(ll){return ll;}  D_T22(ll){return ll;}
D_T12(rr){return rr;}  D_T22(rr){return rr;}

// Initializers from correspondingly named files
void name_init();
void compose_init();
void map_init(); // mapping.c
void arith_init();
void compare_init();
void list_init();
void string_init();
void set_init();

void builtin_init() {
#define INIT(n) \
  B_l##n[i]=B_u##n[i]=&true_l##n; B_t##n[i]=NULL; \
  B_d##n[i]=&true_d##n; B_p##n[i]=NULL; B_r##n[i]=NULL;
  DO(i,256) { ON_ALL_NUMS(INIT) }
#undef INIT

  name_init();
  compose_init();
  map_init();
  arith_init();
  compare_init();
  list_init();
  string_init();
  set_init();
}

/////////////// Main definitions ///////////
void FfromB_P(V p, B b, U n, V* x) {
  DECL_ARR(V,xx,n); DO(i,n) xx[i]=cpy1(x[i]);
  setF(p, wrapF(newB(b),n,xx));
}

P finish_asm_B(A a, T t, RegM pop, V p, P* vp) {
  ASM(a,POP,REG_ARG0,-);
  asm_write(a,t,REG_ARG0,REG_RES);
  P f = finish_A(a,pop);
  *vp=P(p);
  if (IMPURE(T(p)) && PURE(t)) {
    V *v=*vp; T(*v)=t; *vp=P(*v)=MALLOC(t_sizeof(t));
  }
  return f;
}
#define FREE_A free(a->ar); free(a->cr); free(a->cv)
void apply_P_B1(V p, B b, V* x) {
  P1 f=B_p1[b]; if(f) return f(p,x[0]);
  R1 rb=B_r1[b]; if (rb) { AS as; A a=&as; init_A(a);
                           T t=rb(a, T(x[0])); FREE(a->ts); if (t) {
    a->l=0; a->o=0; Reg ai=REG_ARG1; a->i=&ai; a->ts=&t;
    RegM pop=start_A(a,1,0); a->ar--;

    ASM(a,PUSH,REG_ARG0,-);
    asm_load(a,T(x[0]),ai,ai);
    (B_a1[b])(a, T(x[0])); FREE_A;

    P vp; void(*af)(P,P) = finish_asm_B(a,t,pop,p,&vp);
    af(vp, P(x[0])); asm_unmap(a,af); return;
  } else { FREE_A; } }
  return FfromB_P(p,b,1,x);
}
void apply_P_B2(V p, B b, V* x) {
  P2 f=B_p2[b]; if(f) return f(p,x[0],x[1]);
  R2 rb=B_r2[b]; if (rb) { AS as; A a=&as; init_A(a);
                           T t=rb(a, T(x[0]), T(x[1])); FREE(a->ts); if (t) {
    a->l=0; a->o=0; Reg ai[2]={REG_ARG1,REG_ARG2}; a->i=ai; a->ts=&t;
    RegM pop=start_A(a,2,0); a->ar--;

    ASM(a,PUSH,REG_ARG0,-);
    asm_load(a,T(x[0]),ai[0],ai[0]);
    asm_load(a,T(x[1]),ai[1],ai[1]);
    (B_a2[b])(a, T(x[0]), T(x[1])); FREE_A;

    P vp; void(*af)(P,P,P) = finish_asm_B(a,t,pop,p,&vp);
    af(vp, P(x[0]), P(x[1])); asm_unmap(a,af); return;
  } else { FREE_A; } }
  return FfromB_P(p,b,2,x);
}
void apply_P_B(V p, B b, U n, V* x) {
  switch (n) {
    case 1: return apply_P_B1(p,b,x);
    case 2: return apply_P_B2(p,b,x);
  }
}

T apply_R_B(A a, B b, U n, T* x) {
  switch (n) {
    case 1: { R1 f=B_r1[b]; return f ? f(a,x[0]) : 0; }
    case 2: { R2 f=B_r2[b]; return f ? f(a,x[0],x[1]) : 0; }
  }
}
void apply_A_B(A a, B b, U n, T* x) {
  switch (n) {
    case 1: { A1 f=B_a1[b]; f(a,x[0]); break; }
    case 2: { A2 f=B_a2[b]; f(a,x[0],x[1]); break; }
  }
}

T apply_R11(A a, R11 f, V* x, T* xx) { return f(a, x[0], xx[0]); }
T apply_R12(A a, R12 f, V* x, T* xx) { return f(a, x[0], xx[0], xx[1]); }
T apply_R21(A a, R21 f, V* x, T* xx) { return f(a, x[0], x[1], xx[0]); }
T apply_R22(A a, R22 f, V* x, T* xx) { return f(a, x[0], x[1], xx[0], xx[1]); }

T apply_R_FB(A a, F f, U n, T* xx) {
#define LINE1(y,z,yz) case (2*y+z): { \
  B b=B(f->f); R##yz ff=B_r##yz[b]; \
  return ff ? apply_R##yz(a, ff, f->x, xx) : 0; }
#define LINE(a,b) LINE1(a,b,a##b)
  switch (2*f->l + n) { LINE(1,1) LINE(1,2) LINE(2,1) LINE(2,2) }
#undef LINE
#undef LINE1
}

void apply_A11(A a, A11 f, V* x, T* xx) { return f(a, x[0], xx[0]); }
void apply_A12(A a, A12 f, V* x, T* xx) { return f(a, x[0], xx[0], xx[1]); }
void apply_A21(A a, A21 f, V* x, T* xx) { return f(a, x[0], x[1], xx[0]); }
void apply_A22(A a, A22 f, V* x, T* xx) { return f(a, x[0], x[1], xx[0], xx[1]); }

void apply_A_FB(A a, F f, U n, T* xx) {
#define LINE1(y,z,yz) case (2*y+z): { \
  B b=B(f->f); A##yz ff=B_a##yz[b]; \
  return apply_A##yz(a, ff, f->x, xx); }
#define LINE(a,b) LINE1(a,b,a##b)
  switch (2*f->l + n) { LINE(1,1) LINE(1,2) LINE(2,1) LINE(2,2) }
#undef LINE
#undef LINE1
}

U dom_B(B b, U n, V* x) {
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
T apply_T_B(B b, U n, T* x) {
  switch (n) {
    case 1: return apply_T_B1(b,x);
    case 2: return apply_T_B2(b,x);
  }
}

U domu_T_B(B b, U n, T* x) {
  switch (n) {
    case 1: return B_u1[b](x[0]);
    case 2: return B_u2[b](x[0],x[1]);
  }
}
U doml_T_B(B b, U n, T* x) {
  switch (n) {
    case 1: return B_l1[b](x[0]);
    case 2: return B_l2[b](x[0],x[1]);
  }
}


void apply_P11(V p, P11 f, V* x, V* xx) { return f(p, x[0], xx[0]); }
void apply_P12(V p, P12 f, V* x, V* xx) { return f(p, x[0], xx[0], xx[1]); }
void apply_P21(V p, P21 f, V* x, V* xx) { return f(p, x[0], x[1], xx[0]); }
void apply_P22(V p, P22 f, V* x, V* xx) { return f(p, x[0], x[1], xx[0], xx[1]); }

void apply_P_FB(V p, F f, U n, V* xx) {
#define LINE1(y,z,yz) case (2*y+z): { \
  B b=B(f->f); P##yz ff=B_p##yz[b]; \
  if(!ff) { DO(i,z)del(xx[i]); ERR("Unknown builtin"); } \
  return apply_P##yz(p, ff, f->x, xx); }
#define LINE(a,b) LINE1(a,b,a##b)
  switch (2*f->l + n) { LINE(1,1) LINE(1,2) LINE(2,1) LINE(2,2) }
#undef LINE
#undef LINE1
}

T apply_T11(T11 f, V* x, T* xx) { return f(x[0], xx[0]); }
T apply_T12(T12 f, V* x, T* xx) { return f(x[0], xx[0], xx[1]); }
T apply_T21(T21 f, V* x, T* xx) { return f(x[0], x[1], xx[0]); }
T apply_T22(T22 f, V* x, T* xx) { return f(x[0], x[1], xx[0], xx[1]); }

T apply_T_FB(F f, U n, T* xx) {
#define LINE1(y,z,yz) case (2*y+z): { \
  B b=B(f->f); T##yz ff=B_t##yz[b]; \
  if(!ff) return E_t; \
  return apply_T##yz(ff, f->x, xx); }
#define LINE(a,b) LINE1(a,b,a##b)
  switch (2*f->l + n) { LINE(1,1) LINE(1,2) LINE(2,1) LINE(2,2) }
#undef LINE
#undef LINE1
}

U dom_FB(F f, U n, V* xx) {
  B b=B(f->f); V* x=f->x;
  switch (10*f->l + n) {
    case 11: return B_d11[b](x[0], xx[0]);
    case 12: return B_d12[b](x[0], xx[0], xx[1]);
    case 21: return B_d21[b](x[0], x[1], xx[0]);
    case 22: return B_d22[b](x[0], x[1], xx[0], xx[1]);
  }
}

U doml_T_FB(F f, U n, T* xx) {
  B b=B(f->f); V* x=f->x;
  switch (10*f->l + n) {
    case 11: return B_l11[b](x[0], xx[0]);
    case 12: return B_l12[b](x[0], xx[0], xx[1]);
    case 21: return B_l21[b](x[0], x[1], xx[0]);
    case 22: return B_l22[b](x[0], x[1], xx[0], xx[1]);
  }
}
U domu_T_FB(F f, U n, T* xx) {
  B b=B(f->f); V* x=f->x;
  switch (10*f->l + n) {
    case 11: return B_u11[b](x[0], xx[0]);
    case 12: return B_u12[b](x[0], xx[0], xx[1]);
    case 21: return B_u21[b](x[0], x[1], xx[0]);
    case 22: return B_u22[b](x[0], x[1], xx[0], xx[1]);
  }
}
