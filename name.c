I equalsStr(Str l, Str r) { return strcmp(l,r)==0; }
SET_HASH_TABLE(Str, V, hash_string, equalsStr, FREE, del);

typedef StrV_HashMap Name;

Name names;

D_D2(set) { return 2 + !!(l->t&N_t); }
D_F2(set) { StrVset(names, *(N)l->v, r); return l; }

D_D1(del) { return !!(l->t&N_t); }
D_F1(del) { StrVdel(names, *(N)l->v); return l; }

EXTERN_BUILTINS;
void name_init() {
  names = StrVnew(256);

  B_d2[':'] = &set_d2;
  B_f2[':'] = &set_f2;

  B_d1['d'] = &del_d1;
  B_f1['d'] = &del_f1;
}
