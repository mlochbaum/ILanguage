Str toString(V);
V scan(Str);
D_F1(print) { V v=makeStr(toString(l)); del(l); return v; }

D_D1(string) { return !!((l->t&A_t) && (LIST_T(l)==S_t)); }
Str arrToString(I l, I c, I o, Ptr v) {
  DECL_NSTR(s, l); memcpy(s,v+o,min(c-o,l)); memcpy(s,v,max(0,l+o-c)); return s;
}
D_F1(eval) { A a=l->v; Str s=arrToString(a->l, a->c, a->o, a->v);
  del(l); V v=scan(s); FREE(s); return v;
}

EXTERN_BUILTINS;
void string_init() {
  B_f1['s'] = &print_f1;
  B_d1['S'] = &string_d1;
  B_f1['S'] = &eval_f1;
}
