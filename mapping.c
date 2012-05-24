D_F2(map) { V x[1]; x[0]=l; return fmap(r, 1, x, 0); }
D_F11(map) { return map_f2(ll,l); }
D_F12(map) { V x[2]; x[0]=ll; x[1]=rr;
  return fmap(l,2,x,2*(!mapclasseq(mapclass(rr),mapclass(ll)))); }

D_D21(dom) { return toBool(apply1(cpy(r),cpy(ll))); }
D_F21(apply) { del(r); return apply1(l,ll); }
D_D22(dom) {
  V d=apply2(cpy(r), cpy(ll), cpy(rr));
  I dd=toBool(list_at(d,0)) + 2*toBool(list_at(d,1)); del(d); return dd;
}
D_F22(apply) { del(r); return apply2(l,ll,rr); }

D_F1(type) { V v=newZ(l->t); del(l); return v; }

EXTERN_BUILTINS;
void map_init() {
  B_f2['f']=&map_f2;
  B_f11['f']=&map_f11;
  B_f12['f']=&map_f12;

  B_d21['D']=&dom_d21;
  B_f21['D']=&apply_f21;
  B_d22['D']=&dom_d22;
  B_f22['D']=&apply_f22;

  B_f1['t']=&type_f1;
}
