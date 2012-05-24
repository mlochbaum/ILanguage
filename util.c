// General utilities
#define DO(var, max) for(var=0; var<max; var++)
#define DDO(var, max) I var; DO(var, max)

#define max(a,b) ((a)>(b) ? (a):(b))
#define min(a,b) ((a)<(b) ? (a):(b))
#define floor(a)   ((a)==(int)(a) ? a : (int)(a) - ((a)<0))
#define ceiling(a) ((a)==(int)(a) ? a : (int)(a) + ((a)>0))
#define sign(a)  (a)>0 ? 1 : -((a)<0)

Ptr* cpy_Ptr(Ptr* x, I n) { DECL_ARR(Ptr,xx,n); DDO(i,n)xx[i]=x[i]; return xx; }

// Numbers
// TODO: complex case
Zv getZ(V v) { return *(Z)v->v; }
Rv getR(V v) {
  switch (v->t) {
    case Z_t: return (Rv)*(Z)v->v;
    case R_t: return *(R)v->v;
  }
}
#define getOPZ(vv, OP) (((vv)->t)==Z_t ? *(Z)(vv)->v : OP(*(R)(vv)->v))
#define getFloorZ(v) getOPZ(v, floor)
#define getCeilZ(v) getOPZ(v,ceiling)

// List properties
// TODO clean FOR_EACH
#define FOR_EACH(i, L) for(i=(L)->o, i##1=0; i##1<(L)->l; i=(++i<(L)->c)?i:0, i##1++)
#define DFOR_EACH(i, l) I i,i##1; FOR_EACH(i, l)
#define LIST_AT(l, i) (l)->v[((i)+(l)->o)%(l)->c]
#define LIST_PTR_AT(l, i) (l)->v+((i)+(l)->o)%(l)->c

// #define LIST_PROP(p) I list_##p(V v){ switch(v->t){ case L_t: return ((L)v->v)->p; case A_t: return ((A)v->v)->p; } }
// LIST_PROP(c); LIST_PROP(l); LIST_PROP(o);
#define LIST_C(l) ((L)l->v)->c
#define LIST_L(ll) ((L)ll->v)->l
#define LIST_O(l) ((L)l->v)->o
#define LIST_T(l) ((L)l->v)->t

#define LINE(T) case T##_t: return sizeof(T##v);
I t_sizeof(T t) { switch(t) {ON_TYPES(ALL,LINE)
  default: return sizeof(V); } }
#undef LINE
Ptr arr_Ptr_at(A a, I i) { return a->v + t_sizeof(a->t)*((i+a->o)%a->c); }

V arr_at(A a, I i) {
  return makeV(a->t, cpyval(a->t, arr_Ptr_at(a, i)));
}
V list_at(V v, I i) {
  switch (v->t) {
    case L_t: return cpy(LIST_AT(((L)v->v), i));
    case A_t: return arr_at((A)v->v, i);
  }
}

// Find next power of two for length: or with
// all down bitshifts, then add one.
I next_pow_2(I l) {
  I c=l-1; I i=32; while (i) { c |= c>>i; i>>=1; } return c+1;
}
