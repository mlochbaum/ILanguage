#! /usr/bin/jconsole

load 'strings'

templates =: <;._2 ] 0 : 0
#define D_L9(func) I    func##_l9(fprotT)
#define D_U9(func) I    func##_u9(fprotT)
#define D_T9(func) T    func##_t9(fprotT)
#define D_D9(func) I    func##_d9(fprot)
#define D_P9(func) void func##_p9(fprotP)
typedef I    (*L9)(protT); L9 B_l9[256];
typedef I    (*U9)(protT); U9 B_u9[256];
typedef TT   (*T9)(protT); T9 B_t9[256];
typedef I    (*D9)(prot);  D9 B_d9[256];
typedef void (*P9)(protP); P9 B_p9[256];

  extern L9 B_l9[256]; \
  extern U9 B_u9[256]; \
  extern T9 B_t9[256]; \
  extern D9 B_d9[256]; \
  extern P9 B_p9[256]; \
)

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
fprotP =: ('P p, '&,)&.> fprot
prot =: (#~ e.&'PVT,')&.> fprot
protT=: (#~ e.&'PVT,')&.> fprotT
protP=: (#~ e.&'PVT,')&.> fprotP

changes =: (<'9'),.nums,. ,./ (<,.".)@>;:'fprotT fprotP fprot protT protP prot'

r1 =. |: changes <@(rplc~ >)"1 0/ templates
r2 =. r1 rplc (#&a: ,&< {.&(<'#define EXTERN_BUILTINS  \')) #changes
'builtin.h' (1!:2<)~ LF,~_3}. ; ,&LF&.> a:-.~,r2

exit ''
