I equalsStr(Str l, Str r) { return 0==strcmp(l,r); }
SET_HASH_TABLE(Str, V, hash_string, equalsStr, FREE, del);

typedef StrV_HashMap Name;

Name names;

D_D2(set) { return 2 + !!(T(l)&N_t); }
D_F2(set) { StrVset(names, strdup(N(l)), r); return l; }

D_D1(name) { return !!(T(l)&N_t); }
D_F1(get) { V v=StrVget(names, N(l)); del(l); return cpy(v); }
D_F1(del) { StrVdel(names, N(l)); return l; }

EXTERN_BUILTINS;
void name_init() {
  names = StrVnew(256);

  B_d2[':'] = &set_d2;
  B_f2[':'] = &set_f2;

  B_d1['.'] = B_d1['d'] = &name_d1;
  B_f1['.'] = &get_f1;
  B_f1['d'] = &del_f1;
}
