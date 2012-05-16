#define IS_ARITH(v) !!((v)->t&ARITH_t)
D_D1(arith) { return IS_ARITH(l); }
D_D2(arith) { return 2*IS_ARITH(r) + IS_ARITH(l); }

//Monads
//TODO: complex case
#define OP(op) { V v; switch (l->t) { M_L(Z,op); M_L(R,op); } del(l); return v; }
#define M_L(type, op) case type##_t: v=new##type(op *(type)l->v); break
D_F1(negate) OP(-);
D_F1(reciprocal) OP(1/);
#undef M_L
#define M_L(type, op) case type##_t: v=new##type(op(*(type)l->v)); break
D_F1(floor) OP(floor);
D_F1(ceiling) OP(ceiling);
#undef M_L
#undef OP

//Dyads
#define OP(op) { V v; switch (max(l->t,r->t)) { D_L(Z,op); D_L(R,op); } del(l);del(r); return v; }
#define D_L(type, op) case type##_t: v=new##type(get##type(l)op get##type(r)); break
D_F2(plus) OP(+);
D_F2(minus) OP(-);
D_F2(times) OP(*);
D_F2(divide) OP(/);
#undef D_L
#define D_L(type, op) case type##_t: v=new##type(op(get##type(l), get##type(r))); break
#define MOD(l,r) (l) - (r)*floor((l)/(r))
D_F2(mod) OP(MOD);
#undef MOD
D_F2(min) OP(min);
D_F2(max) OP(max);
#undef D_L
#undef OP

// EXPORT DEFINITIONS
EXTERN_BUILTINS;
void arith_init() {
#define SET(c, f) B_d1[c] = &arith_d1; B_f1[c] = &f##_f1
  SET('-', negate);
  SET('/', reciprocal);
  SET('m', floor);
  SET('M', ceiling);
#undef SET

#define SET(c, f) B_d2[c] = &arith_d2; B_f2[c] = &f##_f2
  SET('+', plus);
  SET('-', minus);
  SET('*', times);
  SET('/', divide);
  SET('%', mod);
  SET('m', min);
  SET('M', max);
#undef SET
}
