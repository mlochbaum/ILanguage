#!/usr/bin/jconsole

require 'strings'

e =: &.>
lines   =: <;._2 :. unlines
unlines =: ;@:(,&LF e) :. lines

n=: #all =: {.@> comments=: lines 0 :0
Builtin
Char
Error
K Complex
O Composition
Function application
Name
Quasiquote
Z Integer
Real
List
)
typedefs =: lines 0 :0
B uint8_t
C int8_t
E Str
O struct { I r; V f; I l; V* x; } *
F struct { I r; V f; I l; V* x; } *
N Str
Q Str
Z int64_t
R double
K struct { R a; R b; }
L struct { I r; T t; I c; I l; I o; P p; } *
)

classes =:      'const arith func  comp'
(classes) =: ;: 'ECZRK ZRK   BOFNQ OFL'
classes =: 'all ',classes,' nconst'
nconst =: all-.const

echo^:(0 e.~:all) 'Error: non-unique letters'
echo^:(all-.@-:&(/:~){.@>typedefs) 'Error: typedefs don''t match types'

NB. type.h
preamble =: 0 :0
// Generated; see type.ijs

#include <stdlib.h>
#include <stdint.h>

typedef intptr_t I;
typedef void* P;
typedef char* Str;
typedef uintptr_t UI;

typedef I T;

typedef struct { T t; P p; } V;

#define ON_TYPES(t, f) ON_##t##_TYPES(f)
)
post =: 0 : 0
#include "mem.h"
#include "apply.h"
)

def =: '#define '
join =: 1 :'[,m,]'
typesum =: '(',')',~' + 'join/@:(,&'_t'"0)
fmt  =: rplc '\n';LF;'%T';];'%t';tolower
fmte =: (&fmt)e

types =: unlines ('typedef ',2&}.,' ',{.,';'"_)e typedefs
val =: unlines comments ((def,]),' //',[)e all,e '_t'<@,"1]_12{."1":,.2^i.n
get =: unlines '#define %T(v) (*(%T*)((v).p))'fmte all,'V'
typeclasses =: unlines ((18{.def,toupper,'_t '"_),typesum@:".)e ;:classes
fs=: [: ' 'join/ ('f(',],')'"_)"0
ON =: unlines ('#define ON_',toupper,'_TYPES(f) ',fs@".)e ;:classes

'type.h' (1!:2<)~ }:unlines preamble;types;val;get;typeclasses;ON;post

exit ''
