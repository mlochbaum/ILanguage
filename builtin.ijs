#! /usr/bin/jconsole

load 'strings'

templates =: <;._2 ] 0 : 0
#define D_S9(func) I    func##_s9(fprotT)
#define D_T9(func) T    func##_t9(fprotT)
#define D_D9(func) I    func##_d9(fprot)
#define D_P9(func) void func##_p9(fprotP)
typedef I    (*S9)(protT); S9 B_s9[256];
typedef TT   (*T9)(protT); T9 B_t9[256];
typedef I    (*D9)(prot); D9 B_d9[256];
typedef void (*P9)(protP); P9 B_p9[256];

  extern S9 B_s9[256]; \
  extern T9 B_t9[256]; \
  extern D9 B_d9[256]; \
  extern P9 B_p9[256]; \
)

'nums fprot' =: ({."1 ,&< {:"1) (2&{. ; 4&}.);._2 ] 0 : 0
1   V l
2   V l, V r
11  V l, V ll
12  V l, V ll, V rr
21  V l, V r, V ll
22  V l, V r, V ll, V rr
)
fprotT =: rplc&(;:'V T')&.> fprot
fprotP =: ('P p, '&,)&.> fprot

len =. ".@> '1b'&,&.>nums =. -.&' '&.>nums
prot  =: ([:}:'V,'$~+:)&.> len
protT =: ([:}:'T,'$~+:)&.> len
protP =: ('P,' , [:}:'V,'$~+:)&.> len

changes =: (<'9'),.nums,. ,./ (<,.".)@>;:'fprotT fprotP fprot protT protP prot'

r1 =. |: changes <@(rplc~ >)"1 0/ templates
r2 =. r1 rplc (#&a: ,&< {.&(<'#define EXTERN_BUILTINS  \')) #changes
'builtin.h' (1!:2<)~ LF,~_3}. ; ,&LF&.> a:-.~,r2

exit ''
