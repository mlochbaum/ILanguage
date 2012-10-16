D_T2(map) { return fmap_TT(r,1,&l,0,0); }
D_P2(map) { fmap_P(p,r,1,&l,0); del(r); }
D_T11(map) { return map_t2(ll,T(l)); }
D_P11(map) { V l_; map_p2(p,ll,l_=cpy(l)); FREE(P(l_)); }
D_T12(map) { T x[2]; x[0]=ll; x[1]=rr;
  return fmap_T(l,2,x,0,2*(mapclass_T(rr)!=mapclass_T(ll))); }
D_P12(map) { V x[2]; x[0]=ll; x[1]=rr;
  return fmap_P(p,l,2,x,2*(!mapclasseq(mapclass(rr),mapclass(ll)))); }

D_D21(dom) { return toBoold(apply1_d(r,cpy(ll))); }
D_T21(apply) { return apply1_T(l,ll); }
D_P21(apply) { apply1_P(p,l,ll); }

D_D22(dom) {
  V d=apply2_d(r, cpy(ll), cpy(rr));
  I dd=toBool(listV_at(d,0)) + 2*toBool(listV_at(d,1)); FREE(P(d)); return dd;
}
D_T22(apply) { return apply2_T(l,ll,rr); }
D_P22(apply) { apply2_P(p,l,ll,rr); }

D_P1(type) { setZ(p,T(l)); del(l); }

EXTERN_BUILTINS;
void map_init() {
#define D(n,c,f) DB(t##n,c,f); DB(p##n,c,f)
  D(2,'f',map); D(11,'f',map); D(12,'f',map);

  B_u21['D']=&false_l21; DB(d21,'D',dom); DB(t21,'D',apply); DB(p21,'D',apply);
  B_u22['D']=&false_l22; DB(d22,'D',dom); DB(t22,'D',apply); DB(p22,'D',apply);

  DB(t1,'t',Z); DB(p1,'t',type);
#undef D
}
