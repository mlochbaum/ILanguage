Str toString(V);
D_F1(print) { V v=makeStr(toString(l)); del(l); return v; }

EXTERN_BUILTINS;
void string_init() {
  B_f1['s'] = &print_f1;
}
