ic =: 4 : 0
defn =. 'pdefn' conew~ x;y
DU =. get_DU__defn ''
coerase"0 (#~ >:&".&>&defn) conl 1
DU
)

NB. =========================================================
cocurrent 'pcommon'
NB. split into (chars,.prec)
sp1 =: ((,(<-~2),~<)`(}:@[ (,>:&.>) {:@[)@.(' '=])~ ,@boxopen)/@:|.&.(' '&,)
sp =: [: ({."1 ,. [:]&.> _ (_1}) 2 >./\ 0,>@{:"1) _2 ]\ sp1

NB. parse (does not work with parens)
stapply =: 3 (({. (<@|.@[ , [:>.&.>/0 2{])/@:|:@:, }.)@:{. , }.) ]
ifapp =: (-:\:~)@:(,&:({:"1) 2&{.) ` 0: @. (3>#@])
parse =: [: {."1 (([ , stapply^:ifapp^:_) ,:^:(2-#@$))/ &.: ((,:'$';_)&,)@:|.@:sp

NB. give op,args for a function
farg =: 1&{ ,`(|.@])@.((<'.')-:[) 0 2&{

NB. Function notation utils
paren =: '(',,&')'
list =: (,','&,)&.>/(>@:)^:(*@#)
cfun =: , [:paren list^:(0<L.)
apply =: ] cfun~ 'apply',":@<:@#@],'_',[

NB. =========================================================
cocurrent 'pdefn'
coinsert 'pcommon'
NB. ---------------------------------------------------------
NB. y is name;definition
create =: 3 : 0
name =: x [ 'x y' =. y
NB. Parse; split into LHS and RHS
p =. parse y
'Incorrect number of functions' assert 1=#p
p =. >{.p
'Must be an assignment' assert ':'-:1{::p
'L def1' =. 0 2{p

NB. Extract op and args from LHS
op =: >{: L =. <`([: (<@}.,$:@>@{.) farg)@.(1<#) L
L =. }: L
'LHS depth cannot exceed 2' assert 2>:#L
'Repeated args in LHS' assert *./@:~: ;L
NB. Number for use as a suffix
N =: {.@":"0 |. #@> L

NB. Turn LHS arguments into objects
cname =. ; ((] #&.> 'lr'{.~#@[)&.> 1+i.@-@#) L
nval =. ; (]"0&.> 1{.~#) L
vals =: 'pobject' conew"1~ cname ,. nval ;"0 _ ''
getval =: vals {~ (;;L)&i.

defn =: getobj def1

decln =: 0  NB. Number of complex declarations so far (a,b,...)
vars =: 0 2$a:  NB. List of variables to declare and free
)
getobj =: 3 : 0
if. 1=#y do. getval y return. end.
'pobject' conew~ 'fn';1;< getobj@> farg y
)
plusa =: 4 :'+&x&.(a.&i.) y'&'a'
newdecl =: 3 :',plusa <:decln =: >:decln'
addvar =: 3 : 'vars =: vars,y'

get_T =: 3 : 0
T =. get_T__defn :: ((<'V')"_) 2
(name ;~ 'return ',,&'; ')`(''&;)@.(0<L.) T
)
get_P =: 3 : 0
e =. {. get_P__defn 2  NB. vars is created as a side effect
f1 =. 4 :'(''FREE''cfun ''P''cfun])`(''ddel''&cfun)@.(nval__x)&.> y'/"1
e =. vars (('V ',[:list{:"1@]) ; (, f1))~^:(*@#@[) e
e =. e , 'del'&cfun&.> ;3 :'<del__y $0'"0 vals
name ;~ ; ,&'; '&.> e
)
get_DU =: 3 : 0
T =. get_T ''
P =. get_P ''
D =. ; 'TP' ('D_',[,N,(paren name),' { ',],'}',LF"_)^:(*@#@])&.> T,&{.P
DBX =. ';',~ 'DB' cfun (N,~tolower@[) ; (qop=.''''(,,[)op) ; ]
U =. ; T ((' ',LF),~&.>'TP' DBX&.> ,)`(LF,~';',~'D'cfun N;qop;])@.-:&{: P
D;U
)

NB. ---------------------------------------------------------
cocurrent 'pobject'
coinsert 'pcommon'
NB. y is name;nval;args
create =: 3 : 0
'name nval args' =: y
coinsert (*#args) { ;:'pvalue pfunction'
)
T =: 'T'&cfun

NB. get_T: y is 
NB.   0 if the type is required
NB.   1 if the value is required
NB.   2 if the type is required and the result is output

NB. get_P: y is
NB.   0 if the value will be consumed
NB.   1 if the value will not be consumed
NB.   2 if the result should be stored in p
NB. The value returned is (expression;freename) and any declarations are
NB.   added to the pdef object.
NB. freename is empty, or the temp name to use for FREE.

cocurrent 'pvalue'
get_T =: 3 : 'T^:(-.y+nval)`<@.(y=2) name'
Nu =: 0
get_P =: 3 : 0
v =. 0 > nval =: nval - y~:1
nval =: 0>.nval
if. y=2 do.
  '' ;~ (v{::'mv_P';'cp_P') cfun 'p';name
else.
  if. -.v do. name;'' else. ('cpy'&cfun ; ,&((Nu=:Nu+1)#'_')) name end.
end.
)
del =: 3 : 'nval # <name'

cocurrent 'pfunction'
get_T =: 3 : 0
[:^:(y=1) ''
'T' apply (4 :'<get_T__y x'"0~ 1{.~#) args
)
get_P =: 3 : 0
nval =: nval - y~:1
ord =. 2,}.4 :'name__x-:y'&'fn'"0 args
n=.":<:#g =. (4 :'get_P__y x'"0&.:((|.\:ord)&{)~ 1{.~#) args
'ge gd' =. <"_1|:g
if. y=2 do.
  suff =. '_P'
  a =. ''
else.
  d =. *./ *@#@> }.gd NB. if apply should delete the arguments
  gd =. '' ; (-.d) #&.> }.gd
  suff =. (#. d,~#@>{.gd) {:: '';'_d';'d';'dd'
  a =. newdecl__COCREATOR ''
end.
args addvar__COCREATOR@;^:(*@#@])&> gd
a ;~ ('apply',n,suff) cfun 'p'&;^:(y=2) gd (,'='&,)^:(*@#@[)&.> ge
)
