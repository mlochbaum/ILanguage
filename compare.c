#include "builtin.h"
#include "arith.h"

// arith.c
I prepR(A,I,T,T);

I compare_arith(V l, V r) {
#define D_L(type, op) case type##_t: c=op(get##type(l), get##type(r)); break
#define CMP_(l,r) (((l)>(r))-((l)<(r)))
  I c; switch (max(T(l),T(r))) { D_L(Z,CMP_); D_L(R,CMP_); }
  del(l); del(r); return c;
#undef CMP_
#undef D_L
}
D_P2(equals) { setZ(p, compare_arith(l,r)==0); }
D_P2(less_than) { setZ(p, compare_arith(l,r)<0); }
D_P2(greater_than) { setZ(p, compare_arith(l,r)>0); }
D_P2(less_than_or_eq) { setZ(p, compare_arith(l,r)<=0); }
D_P2(greater_than_or_eq) { setZ(p, compare_arith(l,r)>=0); }

D_R2(compare_arith) {
  if ((l|r)&~(Z_t|R_t)) return 0;
  T t=arith_t2(l,r); return IMPURE(t) ? 0 : Z_t;
}
void compare_arith_a(A a, T l, T r, UC code) {
  choose_regs(a);
  switch (arith_t2(l,r)) {
    case Z_t: ASM(a, CMP,a->i[0],a->i[1]);
              break;
    case R_t: a_RfromT(a,l,a->i[0],a->i[0]);
              a_RfromT(a,r,a->i[1],a->i[1]);
              ASM(a, UCOMISD,a->i[0],a->i[1]); break;
    default: return;
  }
  ASM(a, SETX,a->o,code);
  ASM(a, MOVZX4,a->o,a->o);
}
#define D_A(name, cmp) D_A2(name) { return compare_arith_a(a,l,r,cmp); }
D_A(equals, C_E);
D_A(less_than, C_B);
D_A(greater_than, C_A);
D_A(less_than_or_eq, C_BE);
D_A(greater_than_or_eq, C_AE);
#undef D_A

void compare_init() {
#define D(c,f) B_u2[c]=DB(l2,c,arith); DB(d2,c,arith); DB(t2,c,Z); \
               DB(p2,c,f); DB(r2,c,compare_arith); DB(a2,c,f);
  D('=',equals);
  D('<',less_than);
  D('>',greater_than);
}
