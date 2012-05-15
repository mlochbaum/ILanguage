D_F1(itemize) { DECL_ARR(V, v, 1); v[0]=l; return wrapList(1,v); }
D_F2(cross) { DECL_ARR(V, v, 2); v[0]=l; v[1]=r; return wrapList(2,v); }

/*D_F1(concat) {
  if (l->t & LIST_t) {
    if (r->t & LIST_t) {
      T t=*/

// EXPORT DEFINITIONS
EXTERN_BUILTINS;
void list_init() {
  B_f1[';'] = &itemize_f1;
  B_f2[';'] = &cross_f2;
}
