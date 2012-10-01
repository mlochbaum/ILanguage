#define RE(v) return mv_P(p,v)

D_P1(left)  { RE(l); }
D_P2(left)  { del(r); RE(l); }
D_P2(right) { del(l); RE(r); }

D_P11(constant) { del(ll); RE(l); }
D_P12(constant) { del(ll); del(rr); RE(l); }

D_P2(apply) { V(p) = apply1d(r,l); }
D_P2(flip)  { V(p) = apply1d(l,r); }
D_P2(compose) { DECL_ARR(V,v,1); v[0]=l; setO(p, wrapO(r,1,v)); }
D_P11(compose) { DECL_ARR(V,v,1); v[0]=ll; setO(p, wrapO(cpy(l),1,v)); }
D_P12(compose) { DECL_ARR(V,v,2); v[0]=ll; v[1]=rr; setO(p, wrapO(cpy(l),2,v)); }

D_T11(flip)  { return apply2_T(l,ll,ll); }
D_P11(flip)  { apply2_P(p,l,cpy(ll),ll); }
D_T12(flip)  { return apply2_T(l,rr,ll); }
D_P12(flip)  { apply2_P(p,l,rr,ll); }

D_T21(bind)     { return apply2_T(l,ll,T(r)); }
D_P21(bind)     { apply2_P(p,l,ll,cpy(r)); }
D_T21(backbind) { return apply2_T(r,T(l),ll); }
D_P21(backbind) { apply2_P(p,r,cpy(l),ll); }

D_T21(hook)     { return apply2_T(r,apply1_T(l,ll),ll); }
D_P21(hook)     { apply2_P(p,r,apply1(l,cpy(ll)),ll); }
D_T22(hook)     { return apply2_T(r,apply1_T(l,ll),rr); }
D_P22(hook)     { apply2_P(p,r,apply1(l,ll),rr); }
D_T21(backhook) { return apply2_T(l,ll,apply1_T(r,ll)); }
D_P21(backhook) { apply2_P(p,l,ll,apply1(r,cpy(ll))); }
D_T22(backhook) { return apply2_T(l,ll,apply1_T(r,rr)); }
D_P22(backhook) { apply2_P(p,l,ll,apply1(r,rr)); }

D_T21(compose) { return apply1_T(r, apply1_T(l,ll)); }
D_P21(compose) { apply1_P(p, r, apply1(l,ll)); }
D_T22(compose) { return apply2_T(r, apply1_T(l,ll), apply1_T(l,rr)); }
D_P22(compose) { apply2_P(p, r, apply1(l,ll), apply1(l,rr)); }

I toBool(V v) { I r = (T(v)!=Z_t || Z(v)); del(v); return r; }
/* TODO
D_L2(power) { return 2*(!!(r&Z_t)) + 1; }
D_D2(power) { return power_l2(T(l), T(r)); }
D_T21(power) {
  I n=getFloorZ(r);
  DDO(i,n) { T llt=apply1_T(l,ll); if(llt==ll)break; ll=llt; }
  return ll;
}
D_P21(power) {
  I n=getFloorZ(r); DDO(i,n) ll=apply1(l,ll);
  return ll;
}
D_F22(power) {
  I n=getFloorZ(r); DDO(i,n) ll=apply2(l,ll,cpy(rr));
  del(rr); return ll;
}

D_F21(while) {
  while(toBool(apply1(r, cpy(ll)))) ll=apply1(l,ll);
  return ll;
}
D_F22(while) {
  while(toBool(apply2(r, cpy(ll), cpy(rr))))
    ll=apply2(l,ll,cpy(rr));
  del(rr); return ll;
}
*/

EXTERN_BUILTINS;
void compose_init() {
#define D(n,c,f) DB(t##n,c,f); DB(p##n,c,f)
  DB(t1,'[',l);  DB(p1,'[',left);
  DB(t1,']',l);  DB(p1,']',left);
  DB(t2,'[',l);  DB(p2,'[',left);
  DB(t2,']',r);  DB(p2,']',right);

  DB(t11,'k',l); DB(p11,'k',constant);
  DB(t12,'k',l); DB(p12,'k',constant);

  DB(t2,'.',V);  DB(p2,'.',apply);
  DB(t2,'~',V);  DB(p2,'~',flip );
  DB(t2,'o',O);  DB(p2,'o',compose);
  DB(t11,'o',O); DB(p11,'o',compose);
  DB(t12,'o',O); DB(p12,'o',compose);
  D(22,'O',compose);
  D(21,'h',hook);     D(22,'h',hook);
  D(21,'H',backhook); D(22,'H',backhook);

  D(11,'~',flip);
  D(12,'~',flip);

  D(21,'b',bind);
  D(21,'B',backbind);

/*B_d2['p'] = &power_d2;
  B_f21['p'] = &power_f21;
  B_f22['p'] = &power_f22;

  B_f21['w'] = &while_f21;
  B_f22['w'] = &while_f22;*/
#undef D
}
