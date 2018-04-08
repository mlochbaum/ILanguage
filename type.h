// Generated; see type.ijs

#include <stdlib.h>
#include <stdint.h>

typedef intptr_t I;
typedef void* P;
typedef char* Str;
typedef uintptr_t U;

typedef U T;

typedef struct { T t; P p; } V;

#define ON_TYPES(t, f) ON_##t##_TYPES(f)

typedef uint8_t B;
typedef int8_t C;
typedef Str E;
typedef struct { U r; V f; U l; V* x; } * O;
typedef struct { U r; V f; U l; V* x; } * F;
typedef Str N;
typedef Str Q;
typedef int64_t Z;
typedef double R;
typedef struct { R a; R b; } K;
typedef struct { U r; T t; U c; U l; U o; P p; } * L;

#define B_t           1 //Builtin
#define C_t           2 //Char
#define E_t           4 //Error
#define K_t           8 //K Complex
#define O_t          16 //O Composition
#define F_t          32 //Function application
#define N_t          64 //Name
#define Q_t         128 //Quasiquote
#define Z_t         256 //Z Integer
#define R_t         512 //Real
#define L_t        1024 //List

#define B(v) (*(B*)((v).p))
#define C(v) (*(C*)((v).p))
#define E(v) (*(E*)((v).p))
#define K(v) (*(K*)((v).p))
#define O(v) (*(O*)((v).p))
#define F(v) (*(F*)((v).p))
#define N(v) (*(N*)((v).p))
#define Q(v) (*(Q*)((v).p))
#define Z(v) (*(Z*)((v).p))
#define R(v) (*(R*)((v).p))
#define L(v) (*(L*)((v).p))
#define V(v) (*(V*)((v).p))

#define ALL_t     (B_t + C_t + E_t + K_t + O_t + F_t + N_t + Q_t + Z_t + R_t + L_t)
#define CONST_t   (E_t + C_t + Z_t + R_t + K_t)
#define ARITH_t   (Z_t + R_t + K_t)
#define FUNC_t    (B_t + O_t + F_t + N_t + Q_t)
#define COMP_t    (O_t + F_t + L_t)
#define NCONST_t  (B_t + O_t + F_t + N_t + Q_t + L_t)

#define ON_ALL_TYPES(f) f(B) f(C) f(E) f(K) f(O) f(F) f(N) f(Q) f(Z) f(R) f(L)
#define ON_CONST_TYPES(f) f(E) f(C) f(Z) f(R) f(K)
#define ON_ARITH_TYPES(f) f(Z) f(R) f(K)
#define ON_FUNC_TYPES(f) f(B) f(O) f(F) f(N) f(Q)
#define ON_COMP_TYPES(f) f(O) f(F) f(L)
#define ON_NCONST_TYPES(f) f(B) f(O) f(F) f(N) f(Q) f(L)

#include "mem.h"
#include "apply.h"
