#define WRAP(type, d, dv) DECL(type,d); *d=dv;

V makeV(T t , Ptr vv) {
  DECL(V,v); v->t=t; v->r=1; v->v=vv; return v;
}
#define setWrap(type) V wrap##type(type d) { return makeV(type##_t, d); }
#define setNew(type) \
  V new##type(type##v dv) { WRAP(type, d, dv); return wrap##type(d); }
ON_TYPES(ALL, setWrap);    // functions wrapE : E->V, etc.
ON_TYPES(WRAPPED, setNew); // Define functions wrapE : Ev->V, etc.

/*
NB. make functions created from struct headers using J.
NB. makeF edited manually due to name conflicts.

'c s lb rb' =: <"_1;:', ; { }'
to   =: 1 : '{.~ i.&m'
from =: 1 : '}.~ [:>:i:&m'
param=: '(',')',~ }:^:(c-:{:)@:(rplc&(s,&<c))@:(rb to)@:(lb from)&.:;:
type =: [:; 1{ (rb from)@:;:
head =: 'V make',type,sig,' {'"_
body =: (([:tolower&.> 1{ (rb from)) ([,'->',],'=',],';'"_)&.> (#~ 1 |. s&E.)@:(rb to)@:(lb from))&.:;:
dec  =: 'DECL(',(,',',tolower)@:type,'); '"_
wrap =: ' return wrap',(,'(',tolower)@type,');'"_
make =: head,LF,'  ',dec,body,wrap,(LF,'}',LF)"_
NB. call with (make typedef)

*/
V makeF(V ff , I l , V * x) {
  DECL(F,f); f->f=ff; f->l=l; f->x=x; return wrapF(f);
}
V makeO(V f , I l , V * x) {
  DECL(O,o); o->f=f; o->l=l; o->x=x; return wrapO(o);
}
V makeC(Rv a , Rv b) {
  DECL(C,c); c->a=a; c->b=b; return wrapC(c);
}
V makeL(I t , I c , I ll , I o , V * v) {
  DECL(L,l); l->t=t; l->c=c; l->l=ll; l->o=o; l->v=v; return wrapL(l);
}
V makeA(I t , I c , I l , I o , Ptr v) {
  DECL(A,a); a->t=t; a->c=c; a->l=l; a->o=o; a->v=v; return wrapA(a);
}  

// Custom make functions
V wrapArrayList(T t, I l, Ptr v) {
  I c=next_pow_2(l);
  v=realloc(v, c*t_sizeof(t));
  return makeA(t, c, l, 0, v);
}
Ptr toHomogeneousArray(T t, I l, V* v) {
  I s=t_sizeof(t); char* c=MALLOC(l*s);
  DDO(i,l) memcpy(c+i*s, v[i]->v, s); return (Ptr) c;
}
V wrapList(I l, V* v) {
  I c=next_pow_2(l);
  T t=0; DDO(i,l)t|=v[i]->t;
  if(!(t&(t-1))){
    V r=wrapArrayList(t,l,toHomogeneousArray(t,l,v)); FREE(v); return r;
  }
  v=realloc(v,c*sizeof(v));
  return makeL(t,c,l,0,v);
}
V makeStr(Str s) { return wrapArrayList(S_t, strlen(s), s); }
V Err(Str s) { WRAP(Str*, e, strdup(s)); return wrapE(e); }
V DErr(Str s) { WRAP(Str*, e, s); return wrapE(e); }


#define LINE(T) case T##_t: OC1(T, (T)v->v); break;
#define LINE_A(T) case T##_t: FOR_EACH(i, a) F(arr_at(a,i)); break;
#define DECL_OC                                  \
  void PROTOTYPE;                                \
  DOC(O,o) { F(o->f); DDO(i, o->l) F(o->x[i]); } \
  DOC(F,g) { F(g->f); DDO(i, g->l) F(g->x[i]); } \
  DOC(L,l) { DFOR_EACH(i, l) F(l->v[i]); }       \
  DOC(A,a) {                                     \
    if (a->t & COMP_t) {                         \
      I i, i1;                                   \
      switch (a->t) { ON_COMP_TYPES(LINE_A) }    \
    }                                            \
  }                                              \
  void PROTOTYPE {                               \
    if (v->t & COMP_t) {                         \
      switch (v->t) { ON_COMP_TYPES(LINE) }      \
    }                                            \
  }

#define PROTOTYPE onChildren(void(*f)(V), V v)
#define F(arg) f(arg)
#define OC1(T, arg) onChildren##T(f, arg)
#define DOC(T, n) void onChildren##T(void(*f)(V), T n)
DECL_OC
#undef F
#undef OC1
#undef DOC
#undef PROTOTYPE

#define PROTOTYPE onChildrenWithI(void(*f)(V, I), V v, I n)
#define F(arg) f(arg, n)
#define OC1(T, arg) onChildrenWithI##T(f, arg, n)
#define DOC(T, name) void onChildrenWithI##T(void(*f)(V, I), T name, I n)
DECL_OC
#undef F
#undef OC1
#undef DOC
#undef PROTOTYPE

#undef LINE
#undef LINE_A
#undef DECL_OC

void freeV(V v) {
  switch (v->t) {
    case O_t: FREE(((O)v->v)->x); break;
    case F_t: FREE(((F)v->v)->x); break;
    case L_t: FREE(((L)v->v)->v); break;
    case A_t: FREE(((A)v->v)->v); break;
  }
  FREE(v->v); FREE(v);
}
void del(V v) { v->r--; onChildren(&del, v); if(!v->r) freeV(v); }

void incref(V v) { v->r++; onChildren(&incref, v); }
void increfn(V v, I n) { v->r+=n; onChildrenWithI(&increfn, v, n); }

Str cpyStr(Str s) { DECL_STR(ss, strlen(s)); return strcpy(ss,s); }
Ptr cpyS(Str* p) { DECL(Str*,pp); *pp=cpyStr(*p); return pp; }
Ptr cpyval(T t, Ptr v) {
  Ptr p=malloc(t_sizeof(t));
  if (t & (E_t+N_t+Q_t)) p=cpyS(v);
  else memcpy(p, v, t_sizeof(t));
}

V cpy(V v) {
  if (v->t & COMP_t) {
    incref(v); return v;
  } else { // Pass-by-value for small v
    return makeV(v->t, cpyval(v->t, v->v));
  }
}
