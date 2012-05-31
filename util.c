// General utilities
#define DO(var, max) for(var=0; var<max; var++)
#define DDO(var, max) I var; DO(var, max)

#define max(a,b) ((a)>(b) ? (a):(b))
#define min(a,b) ((a)<(b) ? (a):(b))
#define floor(a)   ((a)==(int)(a) ? a : (int)(a) - ((a)<0))
#define ceiling(a) ((a)==(int)(a) ? a : (int)(a) + ((a)>0))
#define sign(a)  (a)>0 ? 1 : -((a)<0)

// Find next power of two for length: or with
// all down bitshifts, then add one.
I next_pow_2(I l) {
  I c=l-1; I i=32; while (i) { c |= c>>i; i>>=1; } return c+1;
}
#define PURE(t)  !((t)&((t)-1))

// Numbers
// TODO: complex case
Z getZ(V v) { return Z(v); }
R getR(V v) {
  switch (*v) {
    case Z_t: return (R)Z(v);
    case R_t: return R(v);
  }
}
#define getOPZ(vv, OP) ((*(vv))==Z_t ? Z(vv) : OP(R(vv)))
#define getFloorZ(v) getOPZ(v, floor)
#define getCeilZ(v) getOPZ(v,ceiling)

// List properties
#define LIST_AT(l, i) ((V*)(l).p)[((i)+(l).o)%(l).c]
#define LIST_PTR_AT(l, i) (l).p+t_sizeof((l).t)*(((i)+(l).o)%(l).c)

#define LINE(T) case T##_t: return sizeof(T);
I t_sizeof(T t) {switch(t){ON_TYPES(ALL,LINE) default: return sizeof(V);}}
#undef LINE

V arr_at(L l, I i) {
  V v=MALLOC(sizeof(T)+t_sizeof(l.t)); *v=l.t;
  valcpy(v+1, LIST_PTR_AT(l,i), l.t); return v;
}
V list_at(L l, I i) {
  if (PURE(l.t)) return arr_at(l, i);
  else return cpy(LIST_AT(l, i));
}
V listV_at(V v, I i) { return list_at(L(v), i); }
