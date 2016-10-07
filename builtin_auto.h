// Generated; see builtin.ijs

#undef T
#define D_L1(func) I func##_l1(T l)
#define D_L2(func) I func##_l2(T l, T r)
#define D_L11(func) I func##_l11(V l, T ll)
#define D_L12(func) I func##_l12(V l, T ll, T rr)
#define D_L21(func) I func##_l21(V l, V r, T ll)
#define D_L22(func) I func##_l22(V l, V r, T ll, T rr)
#define D_U1(func) I func##_u1(T l)
#define D_U2(func) I func##_u2(T l, T r)
#define D_U11(func) I func##_u11(V l, T ll)
#define D_U12(func) I func##_u12(V l, T ll, T rr)
#define D_U21(func) I func##_u21(V l, V r, T ll)
#define D_U22(func) I func##_u22(V l, V r, T ll, T rr)
#define D_T1(func) T func##_t1(T l)
#define D_T2(func) T func##_t2(T l, T r)
#define D_T11(func) T func##_t11(V l, T ll)
#define D_T12(func) T func##_t12(V l, T ll, T rr)
#define D_T21(func) T func##_t21(V l, V r, T ll)
#define D_T22(func) T func##_t22(V l, V r, T ll, T rr)
#define D_D1(func) I func##_d1(V l)
#define D_D2(func) I func##_d2(V l, V r)
#define D_D11(func) I func##_d11(V l, V ll)
#define D_D12(func) I func##_d12(V l, V ll, V rr)
#define D_D21(func) I func##_d21(V l, V r, V ll)
#define D_D22(func) I func##_d22(V l, V r, V ll, V rr)
#define D_P1(func) void func##_p1(V p, V l)
#define D_P2(func) void func##_p2(V p, V l, V r)
#define D_P11(func) void func##_p11(V p, V l, V ll)
#define D_P12(func) void func##_p12(V p, V l, V ll, V rr)
#define D_P21(func) void func##_p21(V p, V l, V r, V ll)
#define D_P22(func) void func##_p22(V p, V l, V r, V ll, V rr)
#define D_A1(func) void func##_a1(A a, T l)
#define D_A2(func) void func##_a2(A a, T l, T r)
#define D_A11(func) void func##_a11(A a, V l, T ll)
#define D_A12(func) void func##_a12(A a, V l, T ll, T rr)
#define D_A21(func) void func##_a21(A a, V l, V r, T ll)
#define D_A22(func) void func##_a22(A a, V l, V r, T ll, T rr)
typedef I (*L1)(T); extern L1 B_l1[256];
typedef I (*L2)(T,T); extern L2 B_l2[256];
typedef I (*L11)(V,T); extern L11 B_l11[256];
typedef I (*L12)(V,T,T); extern L12 B_l12[256];
typedef I (*L21)(V,V,T); extern L21 B_l21[256];
typedef I (*L22)(V,V,T,T); extern L22 B_l22[256];
typedef I (*U1)(T); extern U1 B_u1[256];
typedef I (*U2)(T,T); extern U2 B_u2[256];
typedef I (*U11)(V,T); extern U11 B_u11[256];
typedef I (*U12)(V,T,T); extern U12 B_u12[256];
typedef I (*U21)(V,V,T); extern U21 B_u21[256];
typedef I (*U22)(V,V,T,T); extern U22 B_u22[256];
typedef T (*T1)(T); extern T1 B_t1[256];
typedef T (*T2)(T,T); extern T2 B_t2[256];
typedef T (*T11)(V,T); extern T11 B_t11[256];
typedef T (*T12)(V,T,T); extern T12 B_t12[256];
typedef T (*T21)(V,V,T); extern T21 B_t21[256];
typedef T (*T22)(V,V,T,T); extern T22 B_t22[256];
typedef I (*D1)(V); extern D1 B_d1[256];
typedef I (*D2)(V,V); extern D2 B_d2[256];
typedef I (*D11)(V,V); extern D11 B_d11[256];
typedef I (*D12)(V,V,V); extern D12 B_d12[256];
typedef I (*D21)(V,V,V); extern D21 B_d21[256];
typedef I (*D22)(V,V,V,V); extern D22 B_d22[256];
typedef void (*P1)(V,V); extern P1 B_p1[256];
typedef void (*P2)(V,V,V); extern P2 B_p2[256];
typedef void (*P11)(V,V,V); extern P11 B_p11[256];
typedef void (*P12)(V,V,V,V); extern P12 B_p12[256];
typedef void (*P21)(V,V,V,V); extern P21 B_p21[256];
typedef void (*P22)(V,V,V,V,V); extern P22 B_p22[256];
typedef void (*A1)(A,T); extern A1 B_a1[256];
typedef void (*A2)(A,T,T); extern A2 B_a2[256];
typedef void (*A11)(A,V,T); extern A11 B_a11[256];
typedef void (*A12)(A,V,T,T); extern A12 B_a12[256];
typedef void (*A21)(A,V,V,T); extern A21 B_a21[256];
typedef void (*A22)(A,V,V,T,T); extern A22 B_a22[256];
#define DECLARE_BUILTINS  \
  L1 B_l1[256]; \
  L2 B_l2[256]; \
  L11 B_l11[256]; \
  L12 B_l12[256]; \
  L21 B_l21[256]; \
  L22 B_l22[256]; \
  U1 B_u1[256]; \
  U2 B_u2[256]; \
  U11 B_u11[256]; \
  U12 B_u12[256]; \
  U21 B_u21[256]; \
  U22 B_u22[256]; \
  T1 B_t1[256]; \
  T2 B_t2[256]; \
  T11 B_t11[256]; \
  T12 B_t12[256]; \
  T21 B_t21[256]; \
  T22 B_t22[256]; \
  D1 B_d1[256]; \
  D2 B_d2[256]; \
  D11 B_d11[256]; \
  D12 B_d12[256]; \
  D21 B_d21[256]; \
  D22 B_d22[256]; \
  P1 B_p1[256]; \
  P2 B_p2[256]; \
  P11 B_p11[256]; \
  P12 B_p12[256]; \
  P21 B_p21[256]; \
  P22 B_p22[256]; \
  A1 B_a1[256]; \
  A2 B_a2[256]; \
  A11 B_a11[256]; \
  A12 B_a12[256]; \
  A21 B_a21[256]; \
  A22 B_a22[256];
#define T(v) ((v).t)
