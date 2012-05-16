D_F1(left)  { return l; }
D_F2(left)  { return l; }
D_F2(right) { return r; }

D_F2(apply) { return apply1(r,l); }
D_F2(flip)  { return apply1(l,r); }
D_F2(compose) { DECL_ARR(V,v,1); v[0]=l; return makeO(r,1,v); }
D_F11(compose) { DECL_ARR(V,v,1); v[0]=ll; return makeO(l,1,v); }
D_F12(compose) { DECL_ARR(V,v,2); v[0]=ll; v[1]=rr; return makeO(l,2,v); }

D_F11(flip)  { return apply2(l,ll,ll); }
D_F12(flip)  { return apply2(l,rr,ll); }

D_F21(bind)  { return apply2(l,ll,r); }
D_F21(rbind) { return apply2(r,l,ll); }

// TODO better name
D_F21(compose) { return apply1(r, apply1(l,ll)); }
D_F22(compose) { return apply2(r, apply1(l,ll), apply1(cpy(l),rr)); }

// TODO domain
D_F21(power) {
  I n=getFloorZ(r); del(r); DDO(i,n) ll=apply1(cpy(l),ll);
  del(l); return ll;
}
D_F22(power) {
  I n=getFloorZ(r); del(r); DDO(i,n) ll=apply2(cpy(l),ll,cpy(rr));
  del(l); del(rr); return ll;
}

I toBool(V v) { I r = (v->t!=Z_t || *(Z)v->v); del(v); return r; }
D_F21(while) {
  while(toBool(apply1(cpy(r), cpy(ll)))) ll=apply1(cpy(l),ll);
  del(l); del(r); return ll;
}
D_F22(while) {
  while(toBool(apply2(cpy(r), cpy(ll), cpy(rr))))
    ll=apply2(cpy(l),ll,cpy(rr));
  del(l); del(r); del(rr); return ll;
}

EXTERN_BUILTINS;
void compose_init() {
  B_f1['['] = B_f1[']'] = &left_f1;
  B_f2['['] = &left_f2;
  B_f2[']'] = &right_f2;

  B_f2['.'] = &apply_f2;
  B_f2['~'] = &flip_f2;
  B_f2['o'] = &compose_f2;
  B_f11['o'] = &compose_f11;
  B_f12['o'] = &compose_f12;
  B_f21['O'] = &compose_f21;
  B_f22['O'] = &compose_f22;

  B_f11['~'] = &flip_f11;
  B_f12['~'] = &flip_f12;

  B_f21['b'] = &bind_f21;
  B_f21['B'] = &rbind_f21;

  B_f21['p'] = &power_f21;
  B_f22['p'] = &power_f22;

  B_f21['w'] = &while_f21;
  B_f22['w'] = &while_f22;
}
