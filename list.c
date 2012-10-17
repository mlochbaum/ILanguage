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
  d->l += s->l;
  if (PURE(d->t)==PURE(s->t)) listcpy1(d,s,i);
  else { DDO(j,s->l) LIST_AT(d,i+j) = cpy1(list_P_at(s,j)); }
  FREE(s->p); FREE(s);
}
/* Assume (^l->t)|r->t and l->c>=l->l+r->l.
 * Copy the contents of r into l. */
void append(L l, L r) { listcpy(l, r, l->l); }
/* Assume l->t|(^r->t) and r->c>=l->l+r->l.
 * Copy the contents of l into r. */
void prepend(L l, L r) { r->o += r->c-l->l; r->o%=r->c; listcpy(r,l,0); }

#define REL(v) return setL(p,v);
#define RELL(v) return setL(p,L(v));
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
      mv_P(list_P_at(v,v->l), r); v->l=ll; REL(v);
    }
  } else {
    return cross_p2(p,l,r);
  }
}
#undef REL
#undef RELL

D_L1(list) { return !!(l&L_t); }
D_D1(list) { return list_l1(T(l)); }
D_P1(first) { V(p)=listV_at(l,0); del(l); }
D_P1(last) { V(p)=listV_at(l,L(l)->l-1); del(l); }

D_L2(select) { return 2*!!(r&ARITH_t) + !!(l&L_t); }
D_D2(select) { return select_l2(T(l),T(r)); }
D_P2(select) {
  I i=Z(r), ll=L(l)->l; if (i<0 && i>-ll) i+=ll;
  V(p) = listV_at(l,i); del(l);del(r);
}
D_P2(take) {
  l=get(l); I i=Z(r), ll=L(l)->l, c=L(l)->c, o=L(l)->o;
  if (i>=0) L(l)->l=i; else { L(l)->o=(o+ll+i)%c; L(l)->l=-i; }
  del(r); V(p)=l;
}
D_P2(drop) {
  l=get(l); I i=Z(r), ll=L(l)->l, c=L(l)->c, o=L(l)->o;
  if (i>=0) { L(l)->l-=i; L(l)->o=(o+i)%c; } else L(l)->l+=i;
  del(r); V(p)=l;
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
D_D12(reduce) { return 2 + reduce_l12(l,T(ll),T(rr)); }
D_T1(identity_of) { return E_t; }
D_P1(identity_of) { del(l); setE(p, strdup("Identity unknown")); }
D_P11(reduce) {
  if (!(T(ll)&L_t)) { V(p)=ll; }
  I len=L(ll)->l;
  if (len==0) { del(ll); identity_of_p1(p,l); }
  ll=get(ll); V lv; V v=cpy1(listV_at(ll,0)); I i=1;
  for(;i<len;i++) { V vt=apply2(l,v,list_at(L(ll),i)); FREE(P(v)); v=vt; }
  FREE(L(ll)->p); FREE(L(ll)); V(p)=v;
}
D_P12(reduce) {
  if (!(T(ll)&L_t)) { V(p)=apply2(l,ll,rr); }
  I len=L(ll)->l;
  DDO(i,len) rr=apply2(l,rr,listV_at(ll,i));
  del(ll); V(p)=rr;
}

D_P1(reverse) {
  l=get(l); L v=L(l); I n=v->l; T t=v->t;
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


EXTERN_BUILTINS;
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
