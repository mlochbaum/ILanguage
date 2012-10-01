I equalsStr(Str l, Str r) { return 0==strcmp(l,r); }
SET_HASH_TABLE(Str, V, hash_string, equalsStr, FREE, del);

typedef StrV_HashMap Name;

Name names;

D_L2(set) { return 2 + !!(l&N_t); }
D_D2(set) { return 2 + !!(T(l)&N_t); }
D_T2(set) { return N_t; }
D_P2(set) { StrVset(names, strdup(N(l)), r); N(p)=strdup(N(l)); }

D_L1(name) { return !!(l&N_t); }
D_D1(name) { return !!(T(l)&N_t); }
D_P1(get) { V v=StrVget(names, N(l)); del(l); V(p)=cpy(v); }
D_T1(del) { return N_t; }
D_P1(del) { StrVdel(names, N(l)); N(p)=N(l); }

EXTERN_BUILTINS;
void name_init() {
  names = StrVnew(256);

  B_l2[':'] = B_u2[':'] = &set_l2;
  B_t2[':'] = &set_t2;
  B_d2[':'] = &set_d2;
  B_p2[':'] = &set_p2;

  B_l1['.'] = B_u1['.'] = B_l1['d'] = B_u1['d'] = &name_l1;
  B_d1['.'] = B_d1['d'] = &name_d1;
  B_t1['.'] = &V_t1;   B_p1['.'] = &get_p1;
  B_t1['d'] = &del_t1; B_p1['d'] = &del_p1;
}
