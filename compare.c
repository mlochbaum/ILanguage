#include "builtin.h"
#include "arith.h"

I compare_arith(V l, V r) {
#define D_L(type, op) case type##_t: c=op(get##type(l), get##type(r)); break
#define CMP(l,r) (((l)>(r))-((l)<(r)))
  I c; switch (max(T(l),T(r))) { D_L(Z,CMP); D_L(R,CMP); }
  del(l); del(r); return c;
#undef CMP
#undef D_L
}
D_P2(equals) { setZ(p, compare_arith(l,r)==0); }
D_P2(less_than) { setZ(p, compare_arith(l,r)<0); }
D_P2(greater_than) { setZ(p, compare_arith(l,r)>0); }
D_P2(less_than_or_eq) { setZ(p, compare_arith(l,r)<=0); }
D_P2(greater_than_or_eq) { setZ(p, compare_arith(l,r)>=0); }

void compare_init() {
#define D(c,f) B_u2[c]=DB(l2,c,arith); DB(d2,c,arith); DB(t2,c,Z); DB(p2,c,f);
  D('=',equals);
  D('<',less_than);
  D('>',greater_than);
}
