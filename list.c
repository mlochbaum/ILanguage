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
  if (l->l+l->o > l->c) memcpy(LP(l)+s*l->c, LP(l), s*(l->l+l->o-l->c));
  l->c=c;
}

void listcpy1(L d, L s, I i) {
#define DS s1-s0
#define DD d1-d0
  I ss=t_sizeof(d->t);
  P d0=LIST_PTR_ATS(d,i,ss), d1=LP(d)+d->c*ss;
  P s0=LIST_PTR_ATS(s,0,ss), s1=LP(s)+s->c*ss;
  I l=s->l*ss;
  if (DS <= DD) {
    memcpy(d0,s0,min(l,DS)); if((l-=DS)<=0)return; s0=LP(s); d0+=DS;
    memcpy(d0,s0,min(l,DD)); if((l-=DD)<=0)return; s0+=DD; d0=LP(d);
  } else {
    memcpy(d0,s0,min(l,DD)); if((l-=DD)<=0)return; s0+=DD; d0=LP(d);
    memcpy(d0,s0,min(l,DS)); if((l-=DS)<=0)return; s0=LP(s); d0+=DS;
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
      DO(j,s->l) (**(I**)(LIST_PTR_ATS(s,j,ss)))++;
    }
    if (PURE(d->t)==PURE(s->t)) listcpy1(d,s,i);
    else { DO(j,s->l) LIST_AT(d,i+j) = cpy1(list_at(s,j)); }
  }
  if (!s->r) { FREEL(s); }
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
// drop a elements from the beginning and b from the end of l
void cropList(V p, V l, I a, I b) {
  L lv=L(l);
  if (lv->r > 1) {
    I nl=lv->l-a-b;
    T t=lv->t; I s=t_sizeof(t); P v=MALLOC(s*next_pow_2(nl));
    DO(i,nl) valcpy(v+s*i, LIST_PTR_ATS(lv,i+a,s), t);
    del(l); setL(p, wrapArray(t, nl, v));
  } else {
    if (a>0) { DO(j,a) del(list_at(lv,a)); }
    if (b>0) { DO(j,b) del(list_at(lv,lv->l-b+j)); }
    lv->o = (lv->o+a) % lv->c; lv->l -= a+b;
    mv_P(p, l);
  }
}
D_P2(take) {
  I i=Z(r), ll=L(l)->l; del(r);
  if (i>=0) cropList(p, l, 0, ll-i); else cropList(p, l, ll+i, 0);
}
D_P2(drop) {
  I i=Z(r); del(r);
  if (i>=0) cropList(p, l, i, 0); else cropList(p, l, 0, -i);
}

D_L2(list) { return 2*!!(r&L_t) + !!(l&L_t); }
D_D2(list) { return list_l2(T(l),T(r)); }
// Turn v into a modifiable list including type t.
void addtype(V v, T t) {
  L l=L(v); T lt=l->t;
  if (lt&t || IMPURE(lt)) get(v);
  else {
    I n=l->l, c=next_pow_2(n), s=t_sizeof(lt);
    DECL_ARR(V,vs,c); DO(i,n) { vs[i] = cpy1(list_ats(l,i,s)); }
    if (l->r<=1) {
      FREE(l->p); l->r=1; l->t|=t; l->c=c; l->o=0; l->p=(P)vs;
    } else {
      l->r--; L(v) = wrapL(lt|t, c, n, 0, vs);
    }
  }
}
D_P2(amend) {
  L rv=L(r);
#define ERR1(m) del(l); del(r); ERR(m)
  if (rv->l != 2) { ERR1("Argument to { must be a two-element list"); }
  V vi=list_at(rv,0); PURIFY(vi);
  if (T(vi)&CONST_t) {
    if (T(vi)!=Z_t) { ERR1("Index in { must be an integer"); }
    get(r); rv=L(r); V v=list_at(rv,1); PURIFY(v);
    addtype(l,T(v));
    V dst=listV_at(l,Z(vi)); del(dst);
    mv_Pd(dst, cpy(v)); del(r); mv_P(p, l);
  } else {
    ERR1("Form of argument to { is invalid");
  }
#undef ERR1
}

D_P1(length) { I n; if (T(l)&L_t) n=L(l)->l; else n=1; del(l); setZ(p,n); }
D_L2(copy) { return 2*!!(r&ARITH_t) + 1; }
D_D2(copy) { return copy_l2(T(l),T(r)); }
D_P2(copy) {
  I t=T(l), ll=Z(r), s=t_sizeof(T(l)); del(r);
  P v=MALLOC(next_pow_2(ll*s)); DO(i,ll) valcpy(v+i*s,P(l),t);
  del(l); setL(p, wrapArray(t, ll, v));
}

D_P1(iota) {
  I ll=getCeilZ(l); del(l);
  I w=(ll<0); if (w) ll=-ll;
  I c=next_pow_2(ll); DECL_ARR(Z,v,c);
  if (w) { DO(i,ll)v[i]=ll-1-i; } else { DO(i,ll)v[i]=i; }
  setL(p, wrapL(Z_t, c, ll, 0, v));
}
D_P2(iota) {
  switch (T(l)) {
    case Z_t: { Z lz=getZ(l), ll=getCeilZ(r)-lz; del(l); del(r);
                I s=sign(ll); ll*=s; I c=next_pow_2(ll); DECL_ARR(Z,v,c);
                DO(i,ll)v[i]=lz+s*i; return setL(p, wrapL(Z_t, c, ll, 0, v));
              }
    case R_t: { R lr=getR(l), llr=getR(r)-lr; del(l); del(r);
                I s=sign(llr), ll=ceiling(s*llr);
                I c=next_pow_2(ll); DECL_ARR(R,v,c);
                DO(i,ll)v[i]=lr+(R)s*i; return setL(p, wrapL(R_t, c, ll, 0, v));
              }
  }
}

D_L11(reduce) { return !!(ll&(L_t|CONST_t)); }
D_D11(reduce) { return reduce_l11(l,T(ll)); }
D_L12(reduce) { return 2 + !!(ll&(L_t|CONST_t)); }
D_D12(reduce) { return reduce_l12(l,T(ll),T(rr)); }
D_T1(identity_of) { return E_t; }
D_P1(identity_of) { del(l); ERR("Identity unknown"); }
D_P11(reduce) {
  if (!(T(ll)&L_t)) { return mv_P(p, ll); }
  I len=L(ll)->l;
  if (len==0) { del(ll); return identity_of_p1(p,l); }
  T t[3]; t[1]=t[2]=L(ll)->t; I s=t_sizeof(t[1]);
  get(ll); I i; V vt,v=cpy1(listV_ats(ll,0,s));
  for(i=1;i<len&&(!err);i++) {
    t[0]=t[2]; t[2]=apply_T(l,2,t); if(t[0]==t[2]) break;
    vt=v; T(v)=t[2]; P(v)=MALLOC(t_sizeof(T(v)));
    apply2_P(v, l, vt, listV_ats(ll,i,s)); FREE(P(vt));
  }
  if (i<len && !err) {
    AS as; A a=&as;
    if (len-i>=ASM_MIN_ITER && apply_R_full(a,l,2,t)) {
      RegM u=1<<REG_ARG0|1<<REG_ARG1; Reg ai[2]; a->i=ai;
      a->o=ai[0]=REG_RES; ai[1]=get_reg(a->u|u|1<<ai[0]);
      RegM pop=start_A(a,2,u);
      ASM(a, PUSH,REG_ARG2,-);
      asm_load(a,t[0],ai[0],REG_ARG2); I label=a->l;
      asm_load(a,t[1],ai[1],REG_ARG0);
      apply_A_full(a,l,2,t);
      ASM(a, ADDI1,REG_ARG0,s);
      ASM(a, CMP,REG_ARG0,REG_ARG1);
      ASM(a, JB,label-a->l,-);
      I c=L(ll)->c, o=L(ll)->o;
      P lp=LP(L(ll)), end = lp+s*((o+len-1)%c+1);
      if (o+len > c && o+i < c) {
        ASM(a, MOV,REG_ARG2,REG_ARG1);
        ASM(a, MOV_RI,REG_ARG1, (Z)(end));
        ASM(a, SUBI4,REG_ARG0, s*c);
        ASM(a, CMP,REG_ARG1,REG_ARG2);
        ASM(a, JNE,label-a->l,-);
        end = lp+s*c;
      }
      ASM(a, POP,REG_ARG2,-);
      asm_write(a,t[0],REG_ARG2,REG_RES);
      void (*f)(P,P,P) = finish_A(a,pop);
      f(lp+s*(o+i), end, v.p); asm_unmap(a,f);
    } else {
      do { apply2_P(v, l, v, listV_ats(ll,i,s)); } while (++i<len&&(!err));
    }
  }
  if (err) for(;i<len;i++) del(listV_ats(ll,i,s)); else mv_P(p,v);
  FREEL(L(ll)); FREE(P(v));
}
D_P12(reduce) {
  if (!(T(ll)&L_t)) { V(p)=apply2(l,ll,rr); return; }
  get(ll); I len=L(ll)->l; rr=cpy1(rr); V r_;
  DO(i,len) { rr=apply2(l,r_=rr,listV_at(ll,i)); FREE(P(r_)); }
  FREEL(L(ll)); mv_P(p,rr); FREE(P(rr));
}

D_P1(reverse) {
  get(l); L v=L(l); I n=v->l; T t=v->t;
  if (!PURE(t)) {
    DO(i,n/2) {
      V vt=LIST_AT(v,i); LIST_AT(v,i)=LIST_AT(v,n-i-1);
      LIST_AT(v,n-i-1)=vt;
    }
  } else {
#define AT(i) LIST_PTR_ATS(v,i,s)
    I s=t_sizeof(t); P p=MALLOC(s);
    DO(i,n/2) {
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
  B_u2['{']=DB(l2,'{',list); DB(d2,'{',list);
  DB(t2,'{',L); DB(p2,'{',amend);

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
