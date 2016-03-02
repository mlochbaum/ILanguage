#include <string.h>
#include "builtin.h"
#include "arith.h"

D_P1(itemize) { setL(p, wrapL(T(l),1,1,0,P(cpy1(l)))); }
D_P2(cross) {
  T tl=T(l), tr=T(r), t=tl|tr; I s=t_sizeof(t); P v=MALLOC(2*s);
  if (PURE(t)) { memcpy(v,P(l),s); memcpy(v+s,P(r),s); }
  else { ((V*)v)[0]=cpy1(l); ((V*)v)[1]=cpy1(r); }
  setL(p, wrapL(t,2,2,0,v));
}

void resize(L l, I s, I c) { // Assume resizing up
  if (l->c>=c) return; l->p=realloc(l->p, s*c);
  if (l->l+l->o > l->c) memcpy(l->p+s*l->c, l->p, s*(l->l+l->o-l->c));
  l->c=c;
}

void listcpy1(L d, L s, I i) {
#define DS s1-s0
#define DD d1-d0
  I ss=t_sizeof(d->t);
  P d0=LIST_PTR_ATS(d,i,ss), d1=d->p+d->c*ss;
  P s0=LIST_PTR_ATS(s,0,ss), s1=s->p+s->c*ss;
  I l=s->l*ss;
  if (DS <= DD) {
    memcpy(d0,s0,min(l,DS)); if((l-=DS)<=0)return; s0=s->p; d0+=DS;
    memcpy(d0,s0,min(l,DD)); if((l-=DD)<=0)return; s0+=DD; d0=d->p;
  } else {
    memcpy(d0,s0,min(l,DD)); if((l-=DD)<=0)return; s0+=DD; d0=d->p;
    memcpy(d0,s0,min(l,DS)); if((l-=DS)<=0)return; s0=s->p; d0+=DS;
  }
  memcpy(d0,s0,l);
#undef DS
#undef DD
}
void listcpy(L d, L s, I i) {
  s->r--; if (s->l) {
    d->l += s->l;
    if (s->r && s->t&COMP_t) {
      // Increase reference count of source's children
      I ss=t_sizeof(s->t);
      DDO(j,s->l) (**(I**)(LIST_PTR_ATS(s,j,ss)))++;
    }
    if (PURE(d->t)==PURE(s->t)) listcpy1(d,s,i);
    else { DDO(j,s->l) LIST_AT(d,i+j) = cpy1(list_at(s,j)); }
  }
  if (!s->r) { FREE(s->p); FREE(s); }
}
/* Assume (^l->t)|r->t and l->c>=l->l+r->l.
 * Copy the contents of r into l. */
void append(L l, L r) { listcpy(l, r, l->l); }
/* Assume l->t|(^r->t) and r->c>=l->l+r->l.
 * Copy the contents of l into r. */
void prepend(L l, L r) { r->o += r->c-l->l; r->o%=r->c; listcpy(r,l,0); }

#define REL(v) return setL(p,v);
D_P2(concat) {
  if (T(l)==L_t) { L lv=L(l);
    if (T(r)==L_t) { L rv=L(r);
      T t=lv->t|rv->t; I ll=lv->l+rv->l, c=next_pow_2(ll);
      I s=t_sizeof(t);
      I ifl = lv->r==1 && PURE(lv->t)==PURE(t);
      I ifr = rv->r==1 && PURE(rv->t)==PURE(t);
      if (ifl  &&  lv->l >= ifr*rv->l) {
        resize(lv,s,c); append(lv,rv); REL(lv);
      } else if (ifr) {
        resize(rv,s,c); prepend(lv,rv); REL(rv);
      } else {
        L v=wrapL(t,c,0,0,MALLOC(s*c));
        append(v,lv); append(v,rv); REL(v);
      }
    } else {
      T t=lv->t|T(r); I ll=lv->l+1, c=next_pow_2(ll); I s=t_sizeof(t);
      L v; if (lv->r==1 && PURE(lv->t)==PURE(t)) {
        resize(lv,s,c); v=lv;
      } else {
        v=wrapL(t,c,0,0,MALLOC(s*c)); append(v,lv);
      }
      mv_P(list_at(v,v->l), r); v->l=ll; REL(v);
    }
  } else {
    return cross_p2(p,l,r);
  }
}
#undef REL

D_L1(list) { return !!(l&L_t); }
D_D1(list) { return list_l1(T(l)); }
D_P1(first) { mv_Pd(p, cpy(listV_at(l,0))); del(l); }
D_P1(last) { mv_Pd(p, cpy(listV_at(l,L(l)->l-1))); del(l); }

D_L2(select) { return 2*!!(r&ARITH_t) + !!(l&L_t); }
D_D2(select) { return select_l2(T(l),T(r)); }
D_P2(select) {
  I i=Z(r), ll=L(l)->l; if (i<0 && i>-ll) i+=ll;
  mv_Pd(p, cpy(listV_at(l,i))); del(l);del(r);
}
D_P2(take) {
  get(l); L lv=L(l); I i=Z(r), ll=lv->l, c=lv->c, o=lv->o;
  if (i>=0) { I j; for(j=i;j<ll;j++) del(list_at(lv,j)); lv->l=i; }
  else { DDO(j,ll+i) del(list_at(lv,j)); lv->o=(o+ll+i)%c; lv->l=-i; }
  del(r); mv_P(p, l);
}
D_P2(drop) {
  get(l); L lv=L(l); I i=Z(r), ll=lv->l, c=lv->c, o=lv->o;
  if (i>=0) { DDO(j,i) del(list_at(lv,j)); lv->l-=i; lv->o=(o+i)%c; }
  else { I j; for(j=ll+i;j<ll;j++) del(list_at(lv,j)); lv->l+=i; }
  del(r); mv_P(p, l);
}
D_T2(amend) { return E_t | L_t; }
// Turn v into a modifiable list including type t.
void addtype(V v, T t) {
  L l=L(v); T lt=l->t;
  if (lt&t || IMPURE(lt)) get(v);
  else {
    I n=l->l, c=next_pow_2(n), s=t_sizeof(lt);
    DECL_ARR(V,vs,c); DDO(i,n) { vs[i] = cpy1(list_ats(l,i,s)); }
    if (l->r<=1) FREE(l->p);
    l->r=1; l->t|=t; l->c=c; l->o=0; l->p=(P)vs;
  }
}
void modify(L l, I i, V v) { V dst=list_at(l,i); del(dst); mv_P(dst, v); }
D_P2(amend) {
  L lv,rv=L(r);
#define ERR(m) del(l); del(r); return setE(p, strdup(m))
  if (rv->l != 2) { ERR("Argument to { must be a two-element list"); }
  V vi=list_at(rv,0); PURIFY(vi);
  if (T(vi)&CONST_t) {
    if (T(vi)!=Z_t) { ERR("Index in { must be an integer"); }
    get(r); rv=L(r); V v=list_at(rv,1); PURIFY(v);
    addtype(l,T(v)); lv=L(l);
    V dst=list_at(lv,Z(vi)); del(dst);
    mv_Pd(dst, cpy(v)); del(r); mv_P(p, l);
  } else {
    ERR("Form of argument to { is invalid");
  }
#undef ERR
}

D_P1(length) { I n; if (T(l)&L_t) n=L(l)->l; else n=1; del(l); setZ(p,n); }
D_L2(copy) { return 2*!!(r&ARITH_t) + 1; }
D_D2(copy) { return copy_l2(T(l),T(r)); }
D_P2(copy) {
  I t=T(l), ll=Z(r), s=t_sizeof(T(l)); del(r);
  P v=MALLOC(next_pow_2(ll*s)); DDO(i,ll) valcpy(v+i*s,P(l),t);
  del(l); setL(p, wrapArray(t, ll, v));
}

D_P1(iota) {
  I ll=getCeilZ(l); del(l);
  I w=(ll<0); if (w) ll=-ll;
  I c=next_pow_2(ll); DECL_ARR(Z,v,c);
  if (w) { DDO(i,ll)v[i]=ll-1-i; } else { DDO(i,ll)v[i]=i; }
  setL(p, wrapL(Z_t, c, ll, 0, v));
}
D_P2(iota) {
  switch (T(l)) {
    case Z_t: { Z lz=getZ(l), ll=getCeilZ(r)-lz; del(l); del(r);
                I s=sign(ll); ll*=s; I c=next_pow_2(ll); DECL_ARR(Z,v,c);
                DDO(i,ll)v[i]=lz+s*i; return setL(p, wrapL(Z_t, c, ll, 0, v));
              }
    case R_t: { R lr=getR(l), llr=getR(r)-lr; del(l); del(r);
                I s=sign(llr), ll=ceiling(s*llr);
                I c=next_pow_2(ll); DECL_ARR(R,v,c);
                DDO(i,ll)v[i]=lr+(R)s*i; return setL(p, wrapL(R_t, c, ll, 0, v));
              }
  }
}

D_L11(reduce) { return !!(ll&(L_t|CONST_t)); }
D_D11(reduce) { return reduce_l11(l,T(ll)); }
D_L12(reduce) { return 2 + !!(ll&(L_t|CONST_t)); }
D_D12(reduce) { return reduce_l12(l,T(ll),T(rr)); }
D_T1(identity_of) { return E_t; }
D_P1(identity_of) { del(l); setE(p, strdup("Identity unknown")); }
D_P11(reduce) {
  if (!(T(ll)&L_t)) { return mv_P(p, ll); }
  I len=L(ll)->l;
  if (len==0) { del(ll); return identity_of_p1(p,l); }
  get(ll); I i; V vt,v=cpy1(listV_at(ll,0));
  T s,t=L(ll)->t; if (len>1 && t==(s=apply2_T(l,t,t))) {
    T ts[2]; ts[0]=ts[1]=t; l=apply_S(l,2,ts);
    for(i=1;i<len;i++) { apply2_P(v,l,vt=v,listV_at(ll,i)); }
    ddel(l);
  } else if (len>2 && s==apply2_T(l,s,t)) {
    T ts[2]; ts[0]=s; ts[1]=t; l=apply_S(l,2,ts);
    v = apply2(l,vt=v,listV_at(ll,1)); FREE(P(vt));
    for(i=2;i<len;i++) { apply2_P(v,l,vt=v,listV_at(ll,i)); }
    ddel(l);
  } else {
    for(i=1;i<len;i++) { v = apply2(l,vt=v,listV_at(ll,i)); FREE(P(vt)); }
  }
  FREE(L(ll)->p); FREE(L(ll)); mv_P(p,v); FREE(P(v));
}
D_P12(reduce) {
  if (!(T(ll)&L_t)) { V(p)=apply2(l,ll,rr); return; }
  get(ll); I len=L(ll)->l; rr=cpy1(rr); V r_;
  DDO(i,len) { rr=apply2(l,r_=rr,listV_at(ll,i)); FREE(P(r_)); }
  FREE(L(ll)->p); FREE(L(ll)); mv_P(p,rr); FREE(P(rr));
}

D_P1(reverse) {
  get(l); L v=L(l); I n=v->l; T t=v->t;
  if (!PURE(t)) {
    DDO(i,n/2) {
      V vt=LIST_AT(v,i); LIST_AT(v,i)=LIST_AT(v,n-i-1);
      LIST_AT(v,n-i-1)=vt;
    }
  } else {
#define AT(i) v->p + s*(((i)+v->o)%v->c)
    I s=t_sizeof(t); P p=MALLOC(s);
    DDO(i,n/2) {
      memcpy(p, AT(i), s); memcpy(AT(i), AT(n-i-1), s);
      memcpy(AT(n-i-1), p, s);
    }
    FREE(p);
#undef AT
  }
  setL(p,L(l));
}


void list_init() {
  DB(t1,';',L); DB(p1,';',itemize);
  DB(t2,';',L); DB(p2,';',cross);

  DB(t2,',',L); DB(p2,',',concat);

  B_u1['g']=DB(l1,'g',list); DB(d1,'g',list); 
  DB(t1,'g',V); DB(p1,'g',first);
  B_u1['G']=DB(l1,'G',list); DB(d1,'G',list); 
  DB(t1,'G',V); DB(p1,'G',last);

  B_u2['}']=DB(l2,'}',select); DB(d2,'}',select); 
  B_u2['g']=DB(l2,'g',select); DB(d2,'g',select); 
  B_u2['G']=DB(l2,'G',select); DB(d2,'G',select); 
  DB(t2,'}',V); DB(p2,'}',select);
  DB(t2,'g',V); DB(p2,'g',take);
  DB(t2,'G',V); DB(p2,'G',drop);
  DB(t2,'{',amend); DB(p2,'{',amend);

  DB(t1,'#',Z); DB(p1,'#',length);
  B_u2['#']=DB(l2,'#',copy); DB(d2,'#',copy); 
  DB(t2,'#',L); DB(p2,'#',copy);

  B_u1['i']=DB(l1,'i',arith); DB(d1,'i',arith);
  DB(t1,'i',L); DB(p1,'i',iota);
  B_u2['i']=DB(l2,'i',arith); DB(d2,'i',arith);
  DB(t2,'i',L); DB(p2,'i',iota);

  B_u11['r']=DB(l11,'r',reduce); DB(d11,'r',reduce);
  DB(t11,'r',V); DB(p11,'r',reduce);
  B_u12['r']=DB(l12,'r',reduce); DB(d12,'r',reduce);
  DB(t12,'r',V); DB(p12,'r',reduce);

  B_u1['z']=DB(l1,'z',list); DB(d1,'z',list);
  DB(t1,'z',L); DB(p1,'z',reverse);
}
