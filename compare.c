int compare_arith(V l, V r) {
#define D_L(type, op) case type##_t: c=op(get##type(l), get##type(r)); break
#define CMP(l,r) (((l)>(r))-((l)<(r)))
  I c; switch (max(T(l),T(r))) { D_L(Z,CMP); D_L(R,CMP); }
  del(l); del(r); return c;
#undef CMP
#undef D_L
}
D_F2(equals) { return newZ(compare_arith(l,r)==0); }
D_F2(less_than) { return newZ(compare_arith(l,r)<0); }
D_F2(greater_than) { return newZ(compare_arith(l,r)>0); }
D_F2(less_than_or_eq) { return newZ(compare_arith(l,r)<=0); }
D_F2(greater_than_or_eq) { return newZ(compare_arith(l,r)>=0); }

EXTERN_BUILTINS;
void compare_init() {
  B_d2['='] = &arith_d2;
  B_f2['='] = &equals_f2;
  B_d2['<'] = &arith_d2;
  B_f2['<'] = &less_than_f2;
  B_d2['>'] = &arith_d2;
  B_f2['>'] = &greater_than_f2;
}
