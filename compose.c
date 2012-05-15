D_F2(apply) { return apply1(r,l); }
D_F2(flip) { return apply1(l,r); }
D_F2(compose) { DECL_ARR(V,v,1); v[0]=l; return makeO(r,1,v); }

D_F11(flip)  { return apply2(l,ll,ll); }
D_F12(flip)  { return apply2(l,rr,ll); }

D_F21(bind)  { return apply2(l,ll,r); }
D_F21(rbind) { return apply2(r,l,ll); }

D_F21(compose) { return apply1(r, apply1(l,ll)); }
D_F22(compose) { return apply2(r, apply1(l,ll), apply1(cpy(l),rr)); }

void compose_init() {
  B_f2['.'] = &apply_f2;
  B_f2['~'] = &flip_f2;
  B_f2['o'] = &compose_f2;
  B_f21['O'] = &compose_f21;
  B_f22['O'] = &compose_f22;

  B_f11['~'] = &flip_f11;
  B_f12['~'] = &flip_f12;

  B_f21['b'] = &bind_f21;
  B_f21['B'] = &rbind_f21;
}
