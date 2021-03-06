#include "builtin.h"

#define IS_ARITH(v) !!(v&ARITH_t)
D_L1(arith) { return IS_ARITH(l); }
D_L2(arith) { return 2*IS_ARITH(r) + IS_ARITH(l); }
D_D1(arith) { return arith_l1(T(l)); }
D_D2(arith) { return arith_l2(T(l),T(r)); }

D_T2(arith) { return (l&r&Z_t) | ((l|r)&(Z_t|R_t) ? (l|r)&R_t : 0); }

// Monads
#define OP(op) { switch (T(l)) { M_L(Z,op); M_L(R,op); } del(l); }
#define M_L(T, op) case T##_t: set##T(p, op T(l)); break
D_P1(negate) OP(-);
#undef M_L
#define M_L(T, op) case T##_t: setZ(p, op(T(l))); break
D_P1(floor) OP(floor);
D_P1(ceiling) OP(ceiling);
#undef M_L
#undef OP

D_R1(negate) {
  if (l!=Z_t && l!=R_t) return 0;
  if (l==R_t) request_regs(a, 1);
  return l;
}
D_A1(negate) {
  switch (l) {
    case Z_t: if (choose_reg(a)) ASM(a, MOV,a->o,a->i[0]);
              ASM(a, NEG,-,a->o); return;
    case R_t: { Reg i=a->i[0];
                if (!choose_reg(a)) {
                  i=get_reg(a->u|1<<i);
                  ASM(a, MOVSD,i,a->i[0]);
                }
                ASM(a, PXOR,a->o,a->o);
                ASM(a, SUBSD,a->o,i); return; }
  }
}
D_R1(reciprocal) {
  if (l&~(Z_t|R_t)) return 0;
  request_regs(a, 1); return R_t;
}
D_A1(reciprocal) {
  Reg i = choose_reg(a) ? a->i[0] : get_reg(a->u|1<<a->i[0]);
  a_RfromT(a,l,i,a->i[0]);
  ASM(a, MOV4_RI,a->o,1);
  ASM(a, CVTSI2SD,a->o,a->o);
  ASM(a, DIVSD,a->o,i);
}
#define ROUND(CMP,OP) \
  I c=choose_reg(a);                             \
  if (l==R_t) {                                  \
    Reg i=a->i[0], o=a->o, r=get_reg(a->u|1<<o); \
    ASM(a, CVTTSD2SI,o,i);                       \
    ASM(a, XOR4,r,r);                            \
    ASM(a, CVTSI2SD,r,o);                        \
    ASM(a, UCOMISD,r,i);                         \
    ASM(a, SET##CMP,r,-);                        \
    ASM(a, OP,o,r);                              \
  } else if (c) ASM(a, MOV,a->o,a->i[0]);
D_R1(floor) {
  if (l!=Z_t && l!=R_t) return 0;
  if (l==R_t) request_regs(a, 1); return Z_t;
}
D_A1(floor) { ROUND(A,SUB) }
D_R1(ceiling) { return floor_r1(a,l); }
D_A1(ceiling) { ROUND(B,ADD) }

D_R1(sqroot) { return (l!=Z_t && l!=R_t) ? 0 : R_t; }
D_A1(sqroot) {
  choose_reg(a);
  if (l==Z_t) ASM(a, CVTSI2SD,a->i[0],a->i[0]);
  ASM(a, SQRTSD,a->o,a->i[0]);
}
D_R1(square) { return (l!=Z_t && l!=R_t) ? 0 : l; }
D_A1(square) {
  switch (l) {
    case Z_t: if (choose_reg(a)) ASM(a, MOV,a->o,a->i[0]);
              ASM(a, IMUL,a->o,a->o); return;
    case R_t: if (choose_reg(a)) ASM(a, MOVSD,a->o,a->i[0]);
              ASM(a, MULSD,a->o,a->o); return;
  }
}


// Dyads
#define OP(op) { switch (max(T(l),T(r))) { DL(Z,op); DL(R,op); } del(l);del(r); }
#define DL(T, op) case T##_t: set##T(p, get##T(l)op get##T(r)); break
D_P2(plus) OP(+);
D_P2(minus) OP(-);
D_P2(times) OP(*);
#undef DL
#define DL(T, op) case T##_t: set##T(p, op(get##T(l), get##T(r))); break
D_P2(min) OP(min);
D_P2(max) OP(max);
#undef DL
#undef OP

// Make sure both arguments are in xmm registers. Cast if not.
// Move one argument to the result, and return its index in a->i.
I prepR(A a, I ii, T l, T r) {
  I zr = 2*(l==R_t) + (r==R_t) - 1;
  if (ii==2) ii = zr>=0&&zr<2 ? zr : 0;
  a_RfromT(a,ii?r:l,a->o,a->i[ii]);
  a_RfromT(a,ii?l:r,a->i[1-ii],a->i[1-ii]);
  return ii;
}

// arith_t2 if the output is pure and inputs are arithmetic
T pure_t2(T l, T r) {
  if ((l|r)&~(Z_t|R_t)) return 0;
  T t=arith_t2(l,r); return IMPURE(t) ? 0 : t;
}

D_R2(plus) { return pure_t2(l,r); }
D_A2(plus) {
  I ii=choose_regs(a);
  switch (*a->ts) {
    case Z_t: if (ii==2) ASM3(a, LEA1,a->o,a->i[0],a->i[1]);
              else ASM(a, ADD,a->o,a->i[1-ii]); break;
    case R_t: ii=prepR(a,ii,l,r);
              ASM(a, ADDSD,a->o,a->i[1-ii]); break;
  }
}
D_R2(minus) {
  if ((l|r)&~(Z_t|R_t)) return 0;
  T t=arith_t2(l,r); if IMPURE(t) return 0;
  if (t==R_t) request_regs(a, 1); return t;
}
D_A2(minus) {
  I ii=choose_regs(a);
  switch (*a->ts) {
    case Z_t: if (ii==2) ASM(a, MOV,a->o,a->i[ii=0]);
              ASM(a, SUB,a->o,a->i[1-ii]);
              if(ii) ASM(a,NEG,-,a->o);
              break;
    case R_t: ii=prepR(a,ii,l,r);
              if (ii) {
                Reg r=get_reg(a->u|1<<a->o);
                if (r!=a->i[0]) ASM(a, MOVSD,r,a->i[0]);
                ASM(a, SUBSD,r,a->o);
                ASM(a, MOVSD,a->o,r);
              } else {
                ASM(a, SUBSD,a->o,a->i[1]);
              }
              break;
  }
}
D_R2(times) { return pure_t2(l,r); }
D_A2(times) {
  I ii=choose_regs(a);
  switch (*a->ts) {
    case Z_t: if (ii==2) ASM(a, MOV,a->o,a->i[ii=0]);
              ASM(a, IMUL,a->o,a->i[1-ii]); break;
    case R_t: ii=prepR(a,ii,l,r);
              ASM(a, MULSD,a->o,a->i[1-ii]); break;
  }
}
D_R2(divide) {
  if ((l|r)&~(Z_t|R_t)) return 0;
  request_regs(a, 1); return R_t;
}
D_A2(divide) {
  Reg i1 = (choose_regs(a)==1) ? a->i[1]
                               : get_reg(a->u|1<<a->o|1<<a->i[0]);
  a_RfromT(a,r,i1,a->i[1]);
  a_RfromT(a,l,a->o,a->i[0]);
  ASM(a, DIVSD,a->o,i1);
}
D_R2(mod) {
  if ((l|r)&~(Z_t|R_t)) return 0;
  T t=arith_t2(l,r); request_regs(a, 1+(t==R_t)); return t;
}
D_A2(mod) {
  I n=2; // For PROTECT
  switch (*a->ts) {
    case Z_t: {
      Reg r_=REG_IDIV_0, r1=REG_IDIV_1;
      C shortcut = a->i[0]==r_;
      PROTECT_START(1<<r_|1<<r1);
      if (!shortcut) ASM(a, MOV,r_,a->i[0]);
      ASM(a, CQO,-,-);
      ASM(a, IDIV,a->i[1],-);

      ASM(a, MOV,r_,a->i[1]);
      ASM(a, XOR,r_,r1);
      ASM(a, JNS,0,-); I j=a->l;
      Reg rt;
      if (a->u&1<<a->i[1]) ASM(a, MOV,rt=get_reg(a->u|1<<r_|1<<r1),a->i[1]);
      else rt = a->i[1];
      ASM(a, ADD,rt,r1);
      ASM(a, TEST,r1,r1);
      ASM(a, CMOVNE,r1,rt);
      ((C*)a->a)[j-1] = a->l-j;

      if (a->o==NO_REG) a->o = a->u&1<<r1 ? get_reg(a->u) : r1;
      if (a->o!=r1) ASM(a, MOV,a->o,r1);
      UNPROTECT; return;
    }
    case R_t: {
      I ii=choose_regs(a); ii=prepR(a,ii,l,r);
      RegM u = a->u|1<<a->o|1<<a->i[0]|1<<a->i[1];
      Reg rd = get_reg(u), rs = get_reg(u|1<<rd);
      ASM(a, MOVSD,rd,a->i[0]);
      ASM(a, DIVSD,rd,a->i[1]);
      // Floor of rd (TODO: large values of rd)
      ASM(a, CVTTSD2SI,rd,rd);
      ASM(a, CVTSI2SD,rd,rd);
      ASM(a, XOR4,rs,rs);
      ASM(a, TEST,rd,rd);
      ASM(a, SETS,rs,-);
      ASM(a, CVTSI2SD,rs,rs);
      ASM(a, SUBSD,rd,rs);

      ASM(a, MULSD,rd,a->i[1]);
      if (a->i[0]!=a->o) ASM(a, MOVSD,a->o,a->i[0]);
      ASM(a, SUBSD,a->o,rd);
    }
  }
}

#define CMPZ(jj,OP) \
  I ii=choose_regs(a);                               \
  switch (*a->ts) {                                  \
    case Z_t: { I ifm=(ii==2); if (ifm) ii=1;        \
                ASM(a, CMP,a->i[1-(jj)],a->i[jj]);   \
                if (ifm) ASM(a, MOV,a->o,a->i[ii]);  \
                ASM(a, CMOVLE,a->o,a->i[1-ii]); break; } \
    case R_t: ii=prepR(a,ii,l,r);                    \
              ASM(a, OP##SD,a->o,a->i[1-ii]); break; \
  }
D_R2(min) { return pure_t2(l,r); }
D_A2(min) { CMPZ(ii,  MIN) }
D_R2(max) { return pure_t2(l,r); }
D_A2(max) { CMPZ(1-ii,MAX) }
#undef CMPZ

// EXPORT DEFINITIONS
void arith_init() {
#define SET(c, t, f) B_l1[c] = B_u1[c] = &arith_l1; B_t1[c] = &t##_t1; \
                  B_d1[c] = &arith_d1; B_r1[c] = &f##_r1; B_a1[c] = &f##_a1
  SET('-', l, negate); DB(p1,'-',negate);
  SET('/', R, reciprocal);
  SET('m', Z, floor); DB(p1,'m',floor);
  SET('M', Z, ceiling); DB(p1,'M',ceiling);
  SET('q', l, square);
  SET('Q', R, sqroot);
#undef SET

#define SET(c, f) B_l2[c] = B_u2[c] = &arith_l2; B_t2[c] = &arith_t2; \
                  B_d2[c] = &arith_d2; B_r2[c] = &f##_r2; B_a2[c] = &f##_a2
  SET('+', plus); DB(p2,'+',plus);
  SET('-', minus); DB(p2,'-',minus);
  SET('*', times); DB(p2,'*',times);
  SET('/', divide); DB(t2,'/',R);
  SET('%', mod);
  SET('m', min); DB(p2,'m',min);
  SET('M', max); DB(p2,'M',max);
#undef SET
}
