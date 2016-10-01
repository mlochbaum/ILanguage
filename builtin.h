#include "type.h"
#include "asm.h"
#include "builtin_auto.h"

#define DB(s,c,n) (B_##s[c]=&n##_##s)
D_D1(true);   D_D2(true);
D_D11(true);  D_D12(true);
D_D21(true);  D_D22(true);

D_L1(true);   D_L2(true);
D_L11(true);  D_L12(true);
D_L21(true);  D_L22(true);

D_L1(false);   D_L2(false);
D_L11(false);  D_L12(false);
D_L21(false);  D_L22(false);

#define ON_ALL_NUMS(F) F(1) F(2) F(11) F(12) F(21) F(22)
#define DEF(n) D_T##n(V);
ON_ALL_NUMS(DEF)
#undef DEF
#define DEF(T,n) D_T##n(T);
#define DEFO(n) DEF(O,n)
ON_ALL_NUMS(DEFO)
#undef DEFO
#define DEFZ(n) DEF(Z,n)
ON_ALL_NUMS(DEFZ)
#undef DEFZ
#define DEFR(n) DEF(R,n)
ON_ALL_NUMS(DEFR)
#undef DEFR
#define DEFL(n) DEF(L,n)
ON_ALL_NUMS(DEFL)
#undef DEFL
#undef DEF
#undef ON_ALL_NUMS

D_T1(l);   D_T2(l);
D_T11(l);  D_T12(l);
D_T21(l);  D_T22(l);
D_T2(r);   D_T21(r);  D_T22(r);
D_T11(ll);  D_T12(ll);
D_T21(ll);  D_T22(ll);
D_T12(rr);  D_T22(rr);
