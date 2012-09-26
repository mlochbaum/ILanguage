// Memory and general utilities
#define MALLOC(l) malloc(l)
#define CALLOC(l, n) calloc(l, n)
#define FREE(ptr) free(ptr)

#define DECL(t, v) t v = MALLOC(sizeof(*v))
#define DECL_ARR(t, v, l) t *v = MALLOC(sizeof(t)*(l))
#define DECL_STR(v, l) Str v = MALLOC((l)+1)
#define DECL_NSTR(v, l) DECL_STR(v, l); v[l]='\0'
#define DECL_V(type, v) V v; T(v)=type##_t; P(v)=MALLOC(sizeof(type))
#define WRAP(type, d, dv) DECL(type,d); *d=dv

I t_sizeof(T);
I next_pow_2(I);
#define PURE(t)  !((t)&((t)-1))

#define T(v) ((v).t)
#define P(v) ((v).p)
#define REF(v) (**(I**)P(v))

V wrapP(T, P);

V makeO(V, I, V*);
O wrapO(V, I, V*);
V makeF(V, I, V*);
F wrapF(V, I, V*);
V makeC(R, R);
V makeL(T, I c, I l, I o, P);

#define NEW(T) V new##T(T);
ON_TYPES(ALL, NEW);
#undef NEW

L wrapArray(T, I, P);
V wrapList(I, V*);
V makeStr(Str);
V DErr(Str); V Err(Str);

void delP(T, P);  void del(V); // Delete this copy of v.
void freeP(T, P); void freeV(V);
void valcpy(P, P, T);
V cpy(V);    // Return a new copy of v.
V get(V);    // Ensure that the returned value is safely modifiable.


// util
#define DO(var, max) for(var=0; var<max; var++)
#define DDO(var, max) I var; DO(var, max)

#define max(a,b) ((a)>(b) ? (a):(b))
#define min(a,b) ((a)<(b) ? (a):(b))
#define floor(a)   ((a)==(int)(a) ? a : (int)(a) - ((a)<0))
#define ceiling(a) ((a)==(int)(a) ? a : (int)(a) + ((a)>0))
#define sign(a)  (a)>0 ? 1 : -((a)<0)

// numbers
Z getZ(V);
R getR(V);
#define getOPZ(vv, OP) ((T(vv))==Z_t ? Z(vv) : OP(R(vv)))
#define getFloorZ(v) getOPZ(v, floor)
#define getCeilZ(v) getOPZ(v,ceiling)

// lists
#define LIST_AT(l, i) ((V*)(l)->p)[((i)+(l)->o)%(l)->c]
#define LIST_PTR_AT(l, i) (l)->p+t_sizeof((l)->t)*(((i)+(l)->o)%(l)->c)
#define LIST_PTR_ATS(l, i, s) (l)->p+(s)*(((i)+(l)->o)%(l)->c)
V arr_at(L, I);
V list_at(L, I);
V listV_at(V, I);
