#! /usr/bin/jconsole

require 'strings'

preamble =: 0 : 0
// Generated; see builtin.ijs

#undef T
#undef S
)
post =: 0 : 0
#define T(v) ((v).t)
#define S(v) (*(S*)((v).p))
)

chart =: ;:;._2 ]0 : 0
L  I     T
U  I     T
T  T     T
D  I
P  void  P
S  S     T
A  void  A
)
temptemp =: <;._2 ] 0 : 0
#define D_Q9(func) R func##_q9(fprotF)
typedef R (*Q9)(protF); extern Q9 B_q9[256];

  Q9 B_q9[256]; \
)
torplc =. (;:'Q R F') (,~ tolower&.>@:(2&{.))@:,@,."_ _1 chart
templates =: ; temptemp (>@[ <@rplc ])"0 1`[@.(a:=[)(<@)"0 _ torplc

'nums fprotT' =: ({."1 ,&< {:"1) (2&{. ; 4&}.);._2 ] 0 : 0
1   T l
2   T l, T r
11  V l, T ll
12  V l, T ll, T rr
21  V l, V r, T ll
22  V l, V r, T ll, T rr
)
nums =: -.&' '&.> nums
fprot =: rplc&(;:'T V')&.> fprotT
fprotP =: 'V p, '&,&.> fprot
fprotA =: 'A a, '&,&.> fprotT

addprot =. , (}.@[;(#~ e.&'AVT,')@])&>/"1
changes =: (<'9'),.nums,. ,./ addprot (<,.".)@,~&'fprot'@> 4{.<"0'TPA'

r1 =. |: changes <@(rplc~ >)"1 0/ templates
r2 =. r1 rplc (#&a: ,&< {.&(<'#define DECLARE_BUILTINS  \')) #changes
'builtin_auto.h' (1!:2<)~ post,~preamble, LF,~_3}. ; ,&LF&.> a:-.~,r2

exit ''
