// Generated; see ic/.

#include "builtin.h"
#include <string.h>

/*GEN_DEFN*/

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
    AS as; A a=&as; a->n=1; a->l=0; a->t=0;
    a->u=REG_MASK|1<<REG_LOOP; Reg ai; a->i=&ai; a->o=ai=REG_RES;
    ASM(a, PUSH,REG_ARG1,-);
    asm_load(a,T(v),REG_RES,REG_ARG1);
    ASM(a, MOV,REG_LOOP,REG_ARG0); I label=a->l;
    apply_A(a,l,2,t);
    if (a->t) {
      ASM(a, LOOP,label-a->l,-);
      ASM(a, POP,REG_ARG1,-);
      asm_write(a,T(v),REG_ARG1,REG_RES);
      ASM_RAW(a, RET);
      void (*f)(Z,P); f=asm_mmap(a->l); memcpy(f,a->a,a->l);
      f(n-i, v.p);
    } else {
      do { apply1_P(v, l, v); } while (++i<n&&(!err));
    }
    FREE(a->a);
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

void compose_init() {
#define D(n,c,f) DB(t##n,c,f); DB(p##n,c,f)
  /*GEN_DECL*/

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
