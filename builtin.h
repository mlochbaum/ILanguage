#define D_L1(func) I    func##_l1(T l)
#define D_L2(func) I    func##_l2(T l, T r)
#define D_L11(func) I    func##_l11(V l, T ll)
#define D_L12(func) I    func##_l12(V l, T ll, T rr)
#define D_L21(func) I    func##_l21(V l, V r, T ll)
#define D_L22(func) I    func##_l22(V l, V r, T ll, T rr)
#define D_U1(func) I    func##_u1(T l)
#define D_U2(func) I    func##_u2(T l, T r)
#define D_U11(func) I    func##_u11(V l, T ll)
#define D_U12(func) I    func##_u12(V l, T ll, T rr)
#define D_U21(func) I    func##_u21(V l, V r, T ll)
#define D_U22(func) I    func##_u22(V l, V r, T ll, T rr)
#define D_T1(func) T    func##_t1(T l)
#define D_T2(func) T    func##_t2(T l, T r)
#define D_T11(func) T    func##_t11(V l, T ll)
#define D_T12(func) T    func##_t12(V l, T ll, T rr)
#define D_T21(func) T    func##_t21(V l, V r, T ll)
#define D_T22(func) T    func##_t22(V l, V r, T ll, T rr)
#define D_D1(func) I    func##_d1(V l)
#define D_D2(func) I    func##_d2(V l, V r)
#define D_D11(func) I    func##_d11(V l, V ll)
#define D_D12(func) I    func##_d12(V l, V ll, V rr)
#define D_D21(func) I    func##_d21(V l, V r, V ll)
#define D_D22(func) I    func##_d22(V l, V r, V ll, V rr)
#define D_P1(func) void func##_p1(P p, V l)
#define D_P2(func) void func##_p2(P p, V l, V r)
#define D_P11(func) void func##_p11(P p, V l, V ll)
#define D_P12(func) void func##_p12(P p, V l, V ll, V rr)
#define D_P21(func) void func##_p21(P p, V l, V r, V ll)
#define D_P22(func) void func##_p22(P p, V l, V r, V ll, V rr)
typedef I    (*L1)(T); L1 B_l1[256];
typedef I    (*L2)(T,T); L2 B_l2[256];
typedef I    (*L11)(V,T); L11 B_l11[256];
typedef I    (*L12)(V,T,T); L12 B_l12[256];
typedef I    (*L21)(V,V,T); L21 B_l21[256];
typedef I    (*L22)(V,V,T,T); L22 B_l22[256];
typedef I    (*U1)(T); U1 B_u1[256];
typedef I    (*U2)(T,T); U2 B_u2[256];
typedef I    (*U11)(V,T); U11 B_u11[256];
typedef I    (*U12)(V,T,T); U12 B_u12[256];
typedef I    (*U21)(V,V,T); U21 B_u21[256];
typedef I    (*U22)(V,V,T,T); U22 B_u22[256];
typedef TT   (*T1)(T); T1 B_t1[256];
typedef TT   (*T2)(T,T); T2 B_t2[256];
typedef TT   (*T11)(V,T); T11 B_t11[256];
typedef TT   (*T12)(V,T,T); T12 B_t12[256];
typedef TT   (*T21)(V,V,T); T21 B_t21[256];
typedef TT   (*T22)(V,V,T,T); T22 B_t22[256];
typedef I    (*D1)(V);  D1 B_d1[256];
typedef I    (*D2)(V,V);  D2 B_d2[256];
typedef I    (*D11)(V,V);  D11 B_d11[256];
typedef I    (*D12)(V,V,V);  D12 B_d12[256];
typedef I    (*D21)(V,V,V);  D21 B_d21[256];
typedef I    (*D22)(V,V,V,V);  D22 B_d22[256];
typedef void (*P1)(P,V); P1 B_p1[256];
typedef void (*P2)(P,V,V); P2 B_p2[256];
typedef void (*P11)(P,V,V); P11 B_p11[256];
typedef void (*P12)(P,V,V,V); P12 B_p12[256];
typedef void (*P21)(P,V,V,V); P21 B_p21[256];
typedef void (*P22)(P,V,V,V,V); P22 B_p22[256];
#define EXTERN_BUILTINS  \
  extern L1 B_l1[256]; \
  extern L2 B_l2[256]; \
  extern L11 B_l11[256]; \
  extern L12 B_l12[256]; \
  extern L21 B_l21[256]; \
  extern L22 B_l22[256]; \
  extern U1 B_u1[256]; \
  extern U2 B_u2[256]; \
  extern U11 B_u11[256]; \
  extern U12 B_u12[256]; \
  extern U21 B_u21[256]; \
  extern U22 B_u22[256]; \
  extern T1 B_t1[256]; \
  extern T2 B_t2[256]; \
  extern T11 B_t11[256]; \
  extern T12 B_t12[256]; \
  extern T21 B_t21[256]; \
  extern T22 B_t22[256]; \
  extern D1 B_d1[256]; \
  extern D2 B_d2[256]; \
  extern D11 B_d11[256]; \
  extern D12 B_d12[256]; \
  extern D21 B_d21[256]; \
  extern D22 B_d22[256]; \
  extern P1 B_p1[256]; \
  extern P2 B_p2[256]; \
  extern P11 B_p11[256]; \
  extern P12 B_p12[256]; \
  extern P21 B_p21[256]; \
  extern P22 B_p22[256];
