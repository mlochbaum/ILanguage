D_F2(map) {
  V x[1]; x[0]=l; V v=fmap(r, 1, x, 0); return v;
}

D_D21(dom) { return toBool(apply1(cpy(r),cpy(ll))); }
D_F21(apply) { del(r); return apply1(l,ll); }
D_D22(dom) { return toBool(apply1(cpy(r),cpy(ll)))
                + 2*toBool(apply1(cpy(r),cpy(rr))); }
D_F22(apply) { del(r); return apply2(l,ll,rr); }

D_F1(type) { return newZ(l->t); }

EXTERN_BUILTINS;
void map_init() {
  B_f2['f']=&map_f2;

  B_d21['D']=&dom_d21;
  B_f21['D']=&apply_f21;
  B_d22['D']=&dom_d22;
  B_f22['D']=&apply_f22;

  B_f1['t']=&type_f1;
}
