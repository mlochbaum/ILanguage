// Generated; see ic/.

#include "builtin.h"
#include <string.h>

D_P1(left) { mv_P(p,l); }
D_P1(right) { mv_P(p,l); }
D_P2(left) { mv_P(p,l); del(r); }
D_P2(right) { mv_P(p,r); del(l); }

D_P11(constant) { cp_P(p,l); del(ll); }
D_P12(constant) { cp_P(p,l); del(ll); del(rr); }

D_P2(flip) { apply1_P(p,l,r); del(l); }
D_T11(flip) { return apply2_T(l,ll,ll); }
D_P11(flip) { V ll_; apply2_P(p,l,ll_=cpy(ll),ll); FREE(P(ll_)); }
D_T12(flip) { return apply2_T(l,rr,ll); }
D_P12(flip) { apply2_P(p,l,rr,ll); }

D_T21(bind) { return apply2_T(l,ll,T(r)); }
D_P21(bind) { V r_; apply2_P(p,l,ll,r_=cpy(r)); FREE(P(r_)); }
D_T21(backbind) { return apply2_T(r,T(l),ll); }
D_P21(backbind) { V l_; apply2_P(p,r,l_=cpy(l),ll); FREE(P(l_)); }

D_T21(hook) { return apply2_T(r,apply1_T(l,ll),ll); }
D_P21(hook) { V a; apply2_P(p,r,a=apply1_d(l,cpy(ll)),ll); FREE(P(a)); }
D_T22(hook) { return apply2_T(r,apply1_T(l,ll),rr); }
D_P22(hook) { V a; apply2_P(p,r,a=apply1(l,ll),rr); FREE(P(a)); }
D_T21(backhook) { return apply2_T(l,ll,apply1_T(r,ll)); }
D_P21(backhook) { V a; apply2_P(p,l,ll,a=apply1_d(r,cpy(ll))); FREE(P(a)); }
D_T22(backhook) { return apply2_T(l,ll,apply1_T(r,rr)); }
D_P22(backhook) { V a; apply2_P(p,l,ll,a=apply1(r,rr)); FREE(P(a)); }

D_T21(compose) { return apply1_T(r,apply1_T(l,ll)); }
D_P21(compose) { V a; apply1_P(p,r,a=apply1(l,ll)); FREE(P(a)); }
D_T22(compose) { return apply2_T(r,apply1_T(l,ll),apply1_T(l,rr)); }
D_P22(compose) { V b,a; apply2_P(p,r,b=apply1(l,ll),a=apply1(l,rr)); FREE(P(b)); FREE(P(a)); }

D_P11(doubleu) { V a; apply1_P(p,a=apply1_d(l,cpy(ll)),ll); ddel(a); }
D_P12(doubleu) { V a; apply2_P(p,a=apply2_d(l,cpy(ll),cpy(rr)),ll,rr); ddel(a); }

D_P2(apply) { V(p) = apply1d(r,l); }
D_P2(compose) { DECL_ARR(V,v,1); v[0]=cpy1(l); setO(p, wrapO(cpy1(r),1,v)); }
D_P11(compose) { DECL_ARR(V,v,1); v[0]=cpy1(ll); setO(p, wrapO(cpy(l),1,v)); }
D_P12(compose) { DECL_ARR(V,v,2); v[0]=cpy1(ll); v[1]=cpy1(rr); setO(p, wrapO(cpy(l),2,v)); }
D_P12(split_compose) {
  DECL_ARR(V,lv,1); lv[0]=newB('['); DECL_ARR(V,rv,1); rv[0]=newB(']');
  DECL_ARR(V,v,2);
  v[0]=newO(wrapO(cpy1(ll),1,lv)); v[1]=newO(wrapO(cpy1(rr),1,rv));
  setO(p, wrapO(cpy(l),2,v));
}

I toBoold(V v) { I r = (T(v)!=Z_t || Z(v)); ddel(v); return r; }
I toBool(V v) { I r = (T(v)!=Z_t || Z(v)); del(v); return r; }

D_L2(power) { return 2*(!!(r&Z_t)) + 1; }
D_D2(power) { return power_l2(T(l), T(r)); }
D_T21(power) {
  I n=getFloorZ(r);
  DO(i,n) { T llt=apply1_T(l,ll); if(llt==ll)break; ll=llt; }
  return ll;
}
D_P21(power) {
  I n=getFloorZ(r); V vt,v=cpy1(ll); T t[2]; t[0]=T(v);
  DDO(i,n) {
    t[1]=t[0]; t[0]=apply_T(l,1,t); if (t[0]==t[1]) break;
    vt=v; T(v)=t[0]; P(v)=MALLOC(t_sizeof(T(v)));
    apply1_P(v, l, vt); FREE(P(vt)); if (err) break;
  }
  if (i<n && !err) {
    AS as; A a=&as;
    if (n-i>=ASM_MIN_ITER && apply_R_full(a,l,1,t)) {
      a->u|=1<<REG_LOOP; Reg ai; a->i=&ai; a->o=ai=REG_RES;
      RegM pop=start_A(a,n);
      ASM(a, PUSH,REG_ARG1,-);
      asm_load(a,T(v),REG_RES,REG_ARG1);
      ASM(a, MOV,REG_LOOP,REG_ARG0); I label=a->l;
      apply_A_full(a,l,1,t);
      ASM(a, LOOP,label-a->l,-);
      ASM(a, POP,REG_ARG1,-);
      asm_write(a,T(v),REG_ARG1,REG_RES);
      void (*f)(Z,P) = finish_A(a,pop);
      f(n-i, v.p); asm_unmap(a,f);
    } else {
      do { apply1_P(v, l, v); } while (++i<n&&(!err));
    }
  }
  if (!err) mv_P(p,v); FREE(P(v));
}
D_T22(power) {
  I n=getFloorZ(r);
  DO(i,n) { T llt=apply2_T(l,ll,rr); if(llt==ll)break; ll=llt; }
  return ll;
}
D_P22(power) {
  I n=getFloorZ(r); ll=cpy1(ll); DO(i,n) ll=apply2_d(l,ll,cpy(rr));
  del(rr); mv_P(p,ll); FREE(P(ll));
}

D_P21(while) {
  ll=cpy1(ll); while(toBoold(apply1_d(r, cpy(ll)))) ll=apply1_d(l,ll);
  mv_P(p,ll); FREE(P(ll));
}
D_P22(while) {
  ll=cpy1(ll); while(toBoold(apply2_d(r, cpy(ll), cpy(rr))))
    ll=apply2_d(l,ll,cpy(rr));
  del(rr); mv_P(p,ll); FREE(P(ll));
}

D_R11(flip) { T t[2]={ll,ll}; return apply_R(a,l,2,t); }
D_A11(flip) {
  AS ax=*a; Reg axi[2]={ax.i[0],ax.i[0]}; ax.i=axi;
  T t[2]={ll,ll};
  apply_A(&ax, l, 2, t);
  a->o=ax.o; a->i[0]=axi[0]; UPDATE_A(a,ax);
  return;
}
D_R21(hook) {
  T t[2]; t[1]=ll;
  return (t[0]=apply_R(a,l,1,&ll)) ? apply_R(a,r,2,t) : 0;
}
D_A21(hook) {
  AS ax=*a; ax.o=NO_REG; protect_input(ax.i, &ax.u);
  T tx=apply_A_t(&ax, l, 1, &ll);

  Reg axi[2]={ax.o,ax.i[0]}; ax.i=axi; ax.o=a->o; ax.u=a->u;
  T t[2]={tx,ll};
  apply_A(&ax, r, 2, t);

  a->o=ax.o; UPDATE_A(a,ax);
  return;
}
D_R22(hook) {
  T t[2]; t[1]=rr;
  return (t[0]=apply_R(a,l,1,&ll)) ? apply_R(a,r,2,t) : 0;
}
D_A22(hook) {
  AS ax=*a; ax.o=NO_REG; ax.u|=1<<a->i[1];
  T tx=apply_A_t(&ax, l, 1, &ll);

  I i=a->i[0]; ax.i[0]=ax.o; ax.o=a->o; ax.u=a->u;
  T t[2]={tx,rr};
  apply_A(&ax, r, 2, t);
  a->i[0]=i;

  a->o=ax.o; UPDATE_A(a,ax);
  return;
}
D_R22(compose) {
  T t[2]; t[1]=rr;
  if (!(t[0]=apply_R(a,l,1,&ll))) return 0;
  if (!(t[1]=apply_R(a,l,1,&rr))) return 0;
  return apply_R(a,r,2,t);
}
D_A22(compose) {
  AS ax=*a; Reg afi[2]; T tf[2];

  ax.i=a->i; ax.o=NO_REG; ax.u|=1<<a->i[1];
  tf[0]=apply_A_t(&ax, l, 1, &ll); afi[0]=ax.o;

  ax.i=a->i+1; ax.u=a->u|1<<ax.o; ax.o=NO_REG;
  tf[1]=apply_A_t(&ax, l, 1, &rr); afi[1]=ax.o;

  ax.i=afi; ax.o=a->o; ax.u=a->u;
  apply_A(&ax, r, 2, tf);

  a->o=ax.o; UPDATE_A(a,ax);
  return;
}

void compose_init() {
#define D(n,c,f) DB(t##n,c,f); DB(p##n,c,f)
  DB(t1,'[',l); DB(p1,'[',left);
  DB(t1,']',l); DB(p1,']',right);
  DB(t2,'[',l); DB(p2,'[',left);
  DB(t2,']',r); DB(p2,']',right);

  DB(t11,'k',l); DB(p11,'k',constant);
  DB(t12,'k',l); DB(p12,'k',constant);

  DB(t2,'~',V); DB(p2,'~',flip);
  D(11,'~',flip); DB(a11,'~',flip); DB(r11,'~',flip);
  D(12,'~',flip);

  D(21,'b',bind);
  D(21,'B',backbind);

  D(21,'h',hook); DB(a21,'h',hook); DB(r21,'h',hook);
  D(22,'h',hook); DB(a22,'h',hook); DB(r22,'h',hook);
  D(21,'H',backhook);
  D(22,'H',backhook);

  D(21,'O',compose);
  D(22,'O',compose); DB(a22,'O',compose); DB(r22,'O',compose);

  DB(t11,'w',V); DB(p11,'w',doubleu);
  DB(t12,'w',V); DB(p12,'w',doubleu);

  DB(t2,'.',V);  DB(p2,'.',apply);
  DB(t2,'o',O);  DB(p2,'o',compose);
  DB(t11,'o',O); DB(p11,'o',compose);
  DB(t12,'o',O); DB(p12,'o',compose);
  DB(t12,'O',O); DB(p12,'O',split_compose);

  B_u2['p']=DB(l2,'p',power); DB(d2,'p',power);
  D(21,'p',power); D(22,'p',power);

  DB(t21,'w',V); DB(p21,'w',while);
  DB(t22,'w',V); DB(p22,'w',while);
#undef D
}
