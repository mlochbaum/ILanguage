#!/usr/bin/jconsole

n=. #all =. {.@> comments=. <;._2 ]0 :0
Error
Builtin
O Composition
Function application
Name
Quasiquote
String (char)
Z Integer
Real
Complex
List
Array
)
smoutput^:(0 e.~:all) 'Error: non-unique letters'

classes =. 'wrapped const arith func comp list'
(classes) =. ;: 'EBNQSZR ESZRC ZRC BOFNQ OFLA LA'
classes =. 'all ',classes,' nconst'
nconst =. all-.const

def =. '#define '
join =. 1 :'[,m,]'
typesum =. ' + 'join/@:(,&'_t'"0)
tolines =. [:; ,&LF&.>

val =. tolines comments (],' //',[)&.> def&,&.> all,&.> '_t'<@,"1]_12{."1":,.2^i.n
lower =. tolines ('#define LOWER_',],' ',tolower)&.> all
typeclasses =. tolines ((18{.def,toupper,'_t '"_),typesum@:".)&.> ;:classes
fs=. [: ' 'join/ ('f(',],')'"_)"0
ON =. tolines ('#define ON_',toupper,'_TYPES(f) ',fs@".)&.> ;:classes

echo val,LF,lower,LF,typeclasses,LF,ON
