D_D1(arith) { return !!(l->t&ARITH_t); }
D_D2(arith) { return (2 * !!(r->t&ARITH_t)) + !!(l->t&ARITH_t); }

//Monads
//TODO: complex case
#define M_L(type, op) case type##_t: v=new##type(op *(type)l->v); break
#define OP(op) { V v; switch (l->t) { M_L(Z,op); M_L(R,op); } del(l); return v; }
D_F1(negate) OP(-);
D_F1(reciprocal) OP(1/);
#undef OP

//Dyads
//TODO: complex case
Zv getZ(V v) { return *(Z)v->v; }
Rv getR(V v) {
  switch (v->t) {
    case Z_t: return (Rv)*(Z)v->v;
    case R_t: return *(R)v->v;
  }
}
#define D_L(type, op) case type##_t: v=new##type(get##type(l)op get##type(r)); break
#define OP(op) { V v; switch (max(l->t,r->t)) { D_L(Z,op); D_L(R,op); } del(l);del(r); return v; }
D_F2(plus) OP(+);
D_F2(minus) OP(-);
D_F2(times) OP(*);
D_F2(divide) OP(/);
#undef OP

// EXPORT DEFINITIONS
EXTERN_BUILTINS;
void arith_init() {
#define SET(c, f) B_d1[c] = &arith_d1; B_f1[c] = &f##_f1
  SET('-', negate);
  SET('/', reciprocal);
#undef SET

#define SET(c, f) B_d2[c] = &arith_d2; B_f2[c] = &f##_f2
  SET('+', plus);
  SET('-', minus);
  SET('*', times);
  SET('/', divide);
#undef SET
}
