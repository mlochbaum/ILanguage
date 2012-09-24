#define IS_ARITH(v) !!(v&ARITH_t)
D_S1(arith) { return IS_ARITH(l); }
D_S2(arith) { return 2*IS_ARITH(r) + IS_ARITH(l); }
D_D1(arith) { return arith_s1(T(l)); }
D_D2(arith) { return arith_s2(T(l),T(r)); }

D_T2(arith) { return max(l,r); }

//Monads
//TODO: complex case
#define OP(op) { switch (T(l)) { M_L(Z,op); M_L(R,op); } del(l); }
#define M_L(T, op) case T##_t: *(T*)p=op T(l); break
D_P1(negate) OP(-);
D_P1(reciprocal) OP(1/);
#undef M_L
#define M_L(T, op) case T##_t: *(T*)p=op(T(l)); break
D_P1(floor) OP(floor);
D_P1(ceiling) OP(ceiling);
#undef M_L
#undef OP

//Dyads
#define OP(op) { switch (max(T(l),T(r))) { D_L(Z,op); D_L(R,op); } del(l);del(r); }
#define D_L(T, op) case T##_t: *(T*)p = get##T(l)op get##T(r); break
D_P2(plus) OP(+);
D_P2(minus) OP(-);
D_P2(times) OP(*);
D_P2(divide) OP(/);
#undef D_L
#define D_L(T, op) case T##_t: *(T*)p = op(get##T(l), get##T(r)); break
#define MOD(l,r) (l) - (r)*floor((l)/(r))
#define D_L1(T, op) case T##_t: *(T*)p = get##T(l)op get##T(r); break
// D_P2(mod) OP(MOD);
D_P2(mod) { switch (max(T(l),T(r))) { D_L1(Z,%); D_L(R,MOD); } del(l);del(r); }
#undef D_L1
#undef MOD
D_P2(min) OP(min);
D_P2(max) OP(max);
#undef D_L
#undef OP

// EXPORT DEFINITIONS
EXTERN_BUILTINS;
void arith_init() {
#define SET(c, f) B_s1[c] = &arith_s1; B_t1[c] = &id##_t1; \
                  B_d1[c] = &arith_d1; B_p1[c] = &f##_p1
  SET('-', negate);
  SET('/', reciprocal);
  SET('m', floor);
  SET('M', ceiling);
#undef SET

#define SET(c, f) B_s2[c] = &arith_s2; B_t2[c] = &arith##_t2; \
                  B_d2[c] = &arith_d2; B_p2[c] = &f##_p2
  SET('+', plus);
  SET('-', minus);
  SET('*', times);
  SET('/', divide);
  SET('%', mod);
  SET('m', min);
  SET('M', max);
#undef SET
}
