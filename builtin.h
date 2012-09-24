#define D_S1(func) I    func##_s1(T l)
#define D_S2(func) I    func##_s2(T l, T r)
#define D_S11(func) I    func##_s11(T l, T ll)
#define D_S12(func) I    func##_s12(T l, T ll, T rr)
#define D_S21(func) I    func##_s21(T l, T r, T ll)
#define D_S22(func) I    func##_s22(T l, T r, T ll, T rr)
#define D_T1(func) T    func##_t1(T l)
#define D_T2(func) T    func##_t2(T l, T r)
#define D_T11(func) T    func##_t11(T l, T ll)
#define D_T12(func) T    func##_t12(T l, T ll, T rr)
#define D_T21(func) T    func##_t21(T l, T r, T ll)
#define D_T22(func) T    func##_t22(T l, T r, T ll, T rr)
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
typedef I    (*S1)(T); S1 B_s1[256];
typedef I    (*S2)(T,T); S2 B_s2[256];
typedef I    (*S11)(T,T); S11 B_s11[256];
typedef I    (*S12)(T,T,T); S12 B_s12[256];
typedef I    (*S21)(T,T,T); S21 B_s21[256];
typedef I    (*S22)(T,T,T,T); S22 B_s22[256];
typedef TT   (*T1)(T); T1 B_t1[256];
typedef TT   (*T2)(T,T); T2 B_t2[256];
typedef TT   (*T11)(T,T); T11 B_t11[256];
typedef TT   (*T12)(T,T,T); T12 B_t12[256];
typedef TT   (*T21)(T,T,T); T21 B_t21[256];
typedef TT   (*T22)(T,T,T,T); T22 B_t22[256];
typedef I    (*D1)(V); D1 B_d1[256];
typedef I    (*D2)(V,V); D2 B_d2[256];
typedef I    (*D11)(V,V); D11 B_d11[256];
typedef I    (*D12)(V,V,V); D12 B_d12[256];
typedef I    (*D21)(V,V,V); D21 B_d21[256];
typedef I    (*D22)(V,V,V,V); D22 B_d22[256];
typedef void (*P1)(P,V); P1 B_p1[256];
typedef void (*P2)(P,V,V); P2 B_p2[256];
typedef void (*P11)(P,V,V); P11 B_p11[256];
typedef void (*P12)(P,V,V,V); P12 B_p12[256];
typedef void (*P21)(P,V,V,V); P21 B_p21[256];
typedef void (*P22)(P,V,V,V,V); P22 B_p22[256];
#define EXTERN_BUILTINS  \
  extern S1 B_s1[256]; \
  extern S2 B_s2[256]; \
  extern S11 B_s11[256]; \
  extern S12 B_s12[256]; \
  extern S21 B_s21[256]; \
  extern S22 B_s22[256]; \
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
