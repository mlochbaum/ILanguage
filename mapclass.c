typedef struct {T t; Ptr v;} X;
#define CONST_X  1 //ESZRC
#define FUNC_X   2 //BOFNQ
#define LIST_X   4 //LA

X mapclass(V v) {
  X x; switch (*v) {
#define CASE(T) case T##_t:
    ON_TYPES(CONST, CASE) x.t=CONST_X; break;
    ON_TYPES(FUNC , CASE) x.t=FUNC_X; break;
    case L_t: x.t=LIST_X; x.v=&(L(v).l); break;
#undef CASE
  }
  return x;
}

I mapclasseq(X x, X y) {
  if (x.t==y.t) {
    if (x.t==LIST_X) return *((I*)x.v)==*((I*)y.v); else return 1;
  }
}
