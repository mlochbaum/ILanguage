#include "builtin.h"

#define IS_ARITH(v) !!(v&ARITH_t)
D_L1(arith) { return IS_ARITH(l); }
D_L2(arith) { return 2*IS_ARITH(r) + IS_ARITH(l); }
D_D1(arith) { return arith_l1(T(l)); }
D_D2(arith) { return arith_l2(T(l),T(r)); }

D_T2(arith) { return max(l,r); }

#define D_S(name) void name##_s(P p, V v, I n, V* vs)
//Monads
#define DF(n, T, op) D_S(n##T) { return set##T(v,op(T(vs[0]))); }
#define LINE(n, T) case T##_t: s.t=T##_t; s.f=&n##T##_s; break;
#define OP(n,op) DF(n,Z,op); DF(n,R,op); \
  D_S1(n) { S s; s.f=NULL; switch(l){ LINE(n,Z); LINE(n,R); } return s; }
OP(negate, -);
OP(reciprocal, 1/);
OP(floor, floor);
OP(ceiling, ceiling);
#undef DF
#undef LINE
#undef OP

//Dyads
#define ON(op,l,r) (l) op (r)
#define DFZZ(n, op) D_S(n##ZZ) { return setZ(v,ON(op,Z(vs[0]),Z(vs[1]))); }
#define DFZR(n, op) D_S(n##ZR) { return setR(v,ON(op,(R)Z(vs[0]),R(vs[1]))); }
#define DFRZ(n, op) D_S(n##RZ) { return setR(v,ON(op,R(vs[0]),(R)Z(vs[1]))); }
#define DFRR(n, op) D_S(n##RR) { return setR(v,ON(op,R(vs[0]),R(vs[1]))); }

#define LINE(n, T1,T2) case T2##_t: s.t=max(T1##_t,T2##_t); s.f=&n##T1##T2##_s; return s
#define DECL_S2(n)  D_S2(n) { S s; switch(l){ \
    case Z_t: switch(r){ LINE(n,Z,Z); LINE(n,Z,R); } \
    case R_t: switch(r){ LINE(n,R,Z); LINE(n,R,R); } \
    default: s.f=NULL; return s; } }
#define OP(n,op)  DFZZ(n,op) DFZR(n,op) DFRZ(n,op) DFRR(n,op) DECL_S2(n)
OP(plus, +);
OP(minus, -);
OP(times, *);
OP(divide, /);
#undef ON
#define ON(op,l,r) op(l,r)
OP(min, min);
OP(max, max);
#undef ON

Z modZZ(Z a, Z b) { Z m=a%b; return m+b*(m*b<0); }
R modRZ(R a, Z b) { return a-b*floor(a/b); }
D_S(modZZ) { return setZ(v,modZZ(Z(vs[0]),Z(vs[1]))); }
D_S(modRZ) { return setR(v,modRZ(R(vs[0]),Z(vs[1]))); }
D_S2(mod) {
  S s; s.f=NULL; if (r==Z_t) {
    switch(l){ case Z_t: s.t=Z_t; s.f=&modZZ_s; break;
               case R_t: s.t=R_t; s.f=&modRZ_s; break; }
  } return s;
}

#undef DFZZ
#undef DFZR
#undef DFRZ
#undef DFRR
#undef LINE
#undef OP

#define INIT if (l!=Z_t || r!=Z_t) return; I ii=choose_regs(a); a->t=Z_t
#define D(n, op, EXTRA) D_A2(n) { INIT; \
  if (ii==2) ASM(a, MOV,a->o,a->i[ii=0]); \
  ASM(a, op,a->o,a->i[1-ii]); EXTRA; }
D(plus,ADD,);
D(times,IMUL,);
D(minus,SUB, if(ii)ASM(a,NEG,-,a->o));
#undef D
#define D(n, jj) D_A2(n) { INIT; \
  I ifm=(ii==2); if (ifm) ii=1;       \
  ASM(a, CMP,a->i[1-(jj)],a->i[jj]);  \
  if (ifm) ASM(a, MOV,a->o,a->i[ii]); \
  ASM(a, CMOVLE,a->o,a->i[1-ii]); }
D(min,ii);
D(max,1-ii);
#undef D
D_A2(divide) {}
D_A2(mod) {}
#undef INIT

// EXPORT DEFINITIONS
void arith_init() {
#define SET(c, f) B_l1[c] = B_u1[c] = &arith_l1; B_t1[c] = &l_t1; \
                  B_d1[c] = &arith_d1; B_s1[c] = &f##_s1
  SET('-', negate);
  SET('/', reciprocal);
  SET('m', floor);
  SET('M', ceiling);
#undef SET

#define SET(c, f) B_l2[c] = B_u2[c] = &arith_l2; B_t2[c] = &arith##_t2; \
                  B_d2[c] = &arith_d2; B_s2[c] = &f##_s2; B_a2[c] = &f##_a2
  SET('+', plus);
  SET('-', minus);
  SET('*', times);
  SET('/', divide);
  SET('%', mod);
  SET('m', min);
  SET('M', max);
#undef SET
}
