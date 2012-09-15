typedef int I;
typedef void* Ptr;
typedef char Char;
typedef char* Str;
typedef unsigned int UI;

typedef I T;

typedef struct { T t; Ptr v; } V;

#define ON_TYPES(t, f) ON_##t##_TYPES(f)

typedef Str E;
typedef Char B;
typedef struct { I r; V f; I l; V* x; } * O;
typedef struct { I r; V f; I l; V* x; } * F;
typedef Str N;
typedef Str Q;
typedef Char S;
typedef int64_t Z;
typedef double R;
typedef struct { R a; R b; } C;
typedef struct { I r; T t; I c; I l; I o; Ptr p; } * L;

#define E_t           1 //Error
#define B_t           2 //Builtin
#define O_t           4 //O Composition
#define F_t           8 //Function application
#define N_t          16 //Name
#define Q_t          32 //Quasiquote
#define S_t          64 //String (char)
#define Z_t         128 //Z Integer
#define R_t         256 //Real
#define C_t         512 //Complex
#define L_t        1024 //List

typedef struct {T t; O o;} *VO;
typedef struct {T t; F f;} *VF;
typedef struct {T t; L l;} *VL;

#define E(vv) (*(E*)((vv).v))
#define B(vv) (*(B*)((vv).v))
#define O(vv) (*(O*)((vv).v))
#define F(vv) (*(F*)((vv).v))
#define N(vv) (*(N*)((vv).v))
#define Q(vv) (*(Q*)((vv).v))
#define S(vv) (*(S*)((vv).v))
#define Z(vv) (*(Z*)((vv).v))
#define R(vv) (*(R*)((vv).v))
#define C(vv) (*(C*)((vv).v))
#define L(vv) (*(L*)((vv).v))

#define LOWER_E e
#define LOWER_B b
#define LOWER_O o
#define LOWER_F f
#define LOWER_N n
#define LOWER_Q q
#define LOWER_S s
#define LOWER_Z z
#define LOWER_R r
#define LOWER_C c
#define LOWER_L l

#define ALL_t     E_t + B_t + O_t + F_t + N_t + Q_t + S_t + Z_t + R_t + C_t + L_t
#define WRAPPED_t E_t + B_t + N_t + Q_t + S_t + Z_t + R_t
#define CONST_t   E_t + S_t + Z_t + R_t + C_t
#define ARITH_t   Z_t + R_t + C_t
#define FUNC_t    B_t + O_t + F_t + N_t + Q_t
#define COMP_t    O_t + F_t + L_t
#define NCONST_t  B_t + O_t + F_t + N_t + Q_t + L_t

#define ON_ALL_TYPES(f) f(E) f(B) f(O) f(F) f(N) f(Q) f(S) f(Z) f(R) f(C) f(L)
#define ON_WRAPPED_TYPES(f) f(E) f(B) f(N) f(Q) f(S) f(Z) f(R)
#define ON_CONST_TYPES(f) f(E) f(S) f(Z) f(R) f(C)
#define ON_ARITH_TYPES(f) f(Z) f(R) f(C)
#define ON_FUNC_TYPES(f) f(B) f(O) f(F) f(N) f(Q)
#define ON_COMP_TYPES(f) f(O) f(F) f(L)
#define ON_NCONST_TYPES(f) f(B) f(O) f(F) f(N) f(Q) f(L)
