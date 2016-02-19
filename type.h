#include <stdlib.h>

typedef int I;
typedef void* P;
typedef char Char;
typedef char* Str;
typedef unsigned int UI;

typedef I T;

typedef struct { T t; P p; } V;

#define ON_TYPES(t, f) ON_##t##_TYPES(f)

typedef Str E;
typedef struct { T t; void (*f)(P,V,I,V*); P a; } S;
typedef Char B;
typedef struct { I r; V f; I l; V* x; } * O;
typedef struct { I r; V f; I l; V* x; } * F;
typedef Str N;
typedef Str Q;
typedef Char C;
typedef int64_t Z;
typedef double R;
typedef struct { R a; R b; } K;
typedef struct { I r; T t; I c; I l; I o; P p; } * L;

#define E_t           1 //Error
#define S_t           2 //Specialized function
#define B_t           4 //Builtin
#define O_t           8 //O Composition
#define F_t          16 //Function application
#define N_t          32 //Name
#define Q_t          64 //Quasiquote
#define C_t         128 //Char
#define Z_t         256 //Z Integer
#define R_t         512 //Real
#define K_t        1024 //K Complex
#define L_t        2048 //List

#define E(v) (*(E*)((v).p))
#define S(v) (*(S*)((v).p))
#define B(v) (*(B*)((v).p))
#define O(v) (*(O*)((v).p))
#define F(v) (*(F*)((v).p))
#define N(v) (*(N*)((v).p))
#define Q(v) (*(Q*)((v).p))
#define C(v) (*(C*)((v).p))
#define Z(v) (*(Z*)((v).p))
#define R(v) (*(R*)((v).p))
#define K(v) (*(K*)((v).p))
#define L(v) (*(L*)((v).p))
#define V(v) (*(V*)((v).p))

#define ALL_t     (E_t + S_t + B_t + O_t + F_t + N_t + Q_t + C_t + Z_t + R_t + K_t + L_t)
#define CONST_t   (E_t + C_t + Z_t + R_t + K_t)
#define ARITH_t   (Z_t + R_t + K_t)
#define FUNC_t    (S_t + B_t + O_t + F_t + N_t + Q_t)
#define COMP_t    (O_t + F_t + L_t)
#define NCONST_t  (S_t + B_t + O_t + F_t + N_t + Q_t + L_t)

#define ON_ALL_TYPES(f) f(E) f(S) f(B) f(O) f(F) f(N) f(Q) f(C) f(Z) f(R) f(K) f(L)
#define ON_CONST_TYPES(f) f(E) f(C) f(Z) f(R) f(K)
#define ON_ARITH_TYPES(f) f(Z) f(R) f(K)
#define ON_FUNC_TYPES(f) f(S) f(B) f(O) f(F) f(N) f(Q)
#define ON_COMP_TYPES(f) f(O) f(F) f(L)
#define ON_NCONST_TYPES(f) f(S) f(B) f(O) f(F) f(N) f(Q) f(L)

#include "mem.h"
#include "apply.h"
