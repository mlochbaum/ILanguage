// General utilities
#define DO(var, max) for(var=0; var<max; var++)
#define DDO(var, max) I var; DO(var, max)

#define max(a,b) ((a)>(b) ? (a):(b))
#define min(a,b) ((a)<(b) ? (a):(b))

Ptr* cpy_Ptr(Ptr* x, I n) { DECL_ARR(Ptr,xx,n); DDO(i,n)xx[i]=x[i]; return xx; }

// List properties
// TODO clean FOR_EACH
#define FOR_EACH(i, L) for(i=(L)->o, i##1=0; i##1<(L)->l; i=(++i<(L)->c)?i:0, i##1++)
#define DFOR_EACH(i, l) I i,i##1; FOR_EACH(i, l)
#define LIST_AT(l, i) (l)->v[((i)+(l)->o)%(l)->c]
#define LIST_PTR_AT(l, i) (l)->v+((i)+(l)->o)%(l)->c

// #define LIST_PROP(p) I list_##p(V v){ switch(v->t){ case L_t: return ((L)v->v)->p; case A_t: return ((A)v->v)->p; } }
// LIST_PROP(c); LIST_PROP(l); LIST_PROP(o);
#define LIST_C(l) ((L)l->v)->c
#define LIST_L(l) ((L)l->v)->l
#define LIST_O(l) ((L)l->v)->o
#define LIST_T(l) ((L)l->v)->t

#define LINE(T) case T##_t: return sizeof(T##v);
I t_sizeof(T t) { switch(t) {ON_TYPES(ALL,LINE)} }
#undef LINE
#define ARR_PTR_AT(a, i) (a)->v + t_sizeof((a)->t)*(((i)+(a)->o)%(a)->c)

V arr_at(A a, I i) {
  return makeV(a->t, cpyval(a->t, ARR_PTR_AT(a, i)));
}

V list_at(V v, I i) {
  switch (v->t) {
    case L_t: return cpy(LIST_AT(((L)v->v), i));
    case A_t: return arr_at((A)v->v, i);
  }
}
