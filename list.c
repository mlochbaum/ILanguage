D_P1(itemize) { *(L*)p=wrapL(T(l),1,1,0,P(l)); }
D_P2(cross) {
  T tl=T(l), tr=T(r), t=tl|tr; I s=t_sizeof(t); P v=MALLOC(2*s);
  if (PURE(t)) { memcpy(v,P(l),s); memcpy(v+s,P(r),s); FREE(P(l)); FREE(P(r)); }
  else { ((V*)v)[0]=l; ((V*)v)[1]=r; }
  *(L*)p = wrapL(t,2,2,0,v);
}

void resize(L l, I s, I c) { // Assume resizing up
  if (l->c>=c) return; l->p=realloc(l->p, s*c);
  if (l->l+l->o > l->c) memcpy(l->p+s*l->c, l->p, s*(l->l+l->o-l->c));
  l->c=c;
}
D_P2(concat) {
  if (T(l)==L_t) { L lv=L(l);
    if (T(r)==L_t) { L rv=L(r);
      T t=lv->t|rv->t; I ll=lv->l+rv->l, c=next_pow_2(ll);
      I s=t_sizeof(t);
      if (PURE(t)) {
#define AT(a,i) a->p + s*((a->o+i)%a->c)
        if (lv->r==1 || rv->r==1) {
          if ((lv->r==1)*lv->l >= (rv->r==1)*rv->l) { // Copy to left
            resize(lv,s,c);
            DDO(i,rv->l) valcpy(AT(lv,lv->l+i),AT(rv,i),t);
            del(r); lv->l=ll; RE(l);
          } else { // Copy to right
            resize(rv,s,c);
            DDO(i,lv->l) valcpy(AT(rv,i-lv->l+rv->c),AT(lv,i),t);
            rv->l=ll; rv->o=(rv->o-lv->l+rv->c)%rv->c; del(l); RE(r);
          }
        } else {
          P v=malloc(s*c);
          DDO(i,lv->l) valcpy(v+s*i, AT(lv,i), t);
          DO(i,rv->l) valcpy(v+s*(i+lv->l), AT(rv,i), t);
          del(l); del(r); *(L*)p=wrapL(t, c, ll, 0, v);
        }
#undef AT
      } else {
#define AT(a,i) ((V*)a->p)[(a->o+i)%a->c]
        I le=(lv->r==1 && PURE(lv->t)), re=(rv->r==1 && PURE(rv->t));
        if (le||re) {
          if (le*lv->l >= re*rv->l) { // Copy to left
            resize(lv,s,c);
            DDO(i,rv->l) AT(lv,lv->l+i)=cpy(AT(rv,i));
            del(r); lv->l=ll; RE(l);
          } else { // Copy to right
            resize(rv,s,c);
            DDO(i,lv->l) AT(rv,i-lv->l+rv->c)=cpy(AT(lv,i));
            rv->l=ll; rv->o=(rv->o-lv->l+rv->c)%rv->c; del(l); RE(r);
          }
        } else {
          DECL_ARR(V,v,c);
          DDO(i,lv->l) v[i]=cpy(AT(lv,i));
          DO(i,rv->l) v[i+lv->l]=cpy(AT(rv,i));
          del(l); del(r); *(L*)p=wrapL(t, c, ll, 0, v);
        }
#undef AT
      }
    } else {
      T t=lv->t|T(r); I ll=lv->l+1, c=next_pow_2(ll);
      I s=t_sizeof(t);
      if (PURE(t)) {
#define AT(a,i) a->p + s*((a->o+i)%a->c)
        if (lv->r==1) {
          resize(lv,s,c); valcpy(AT(lv,lv->l), P(r), t);
          del(r); lv->l++; RE(l);
        } else {
          P v=malloc(s*c);
          DDO(i,lv->l) valcpy(v+s*i, AT(lv,i), t);
          valcpy(v+s*lv->l, P(r), t);
          del(r); *(L*)p=wrapL(t, c, ll, 0, v);
        }
#undef AT
      } else {
#define AT(a,i) ((V*)a->p)[(a->o+i)%a->c]
        if (lv->r==1 && PURE(lv->t)) {
          resize(lv,s,c); AT(lv,lv->l)=r; lv->l++; RE(l);
        } else {
          DECL_ARR(V,v,c);
          DDO(i,lv->l) v[i]=cpy(AT(lv,i)); v[lv->l]=r; del(l);
          *(L*)p=wrapL(t, c, ll, 0, v);
        }
#undef AT
      }
    }
  } else {
    return cross_p2(p,l,r);
  }
}

D_L1(list) { return !!(l&L_t); }
D_D1(list) { return list_l1(T(l)); }
D_P1(first) { *(V*)p=listV_at(l,0); del(l); }
D_P1(last) { *(V*)p=listV_at(l,L(l)->l-1); del(l); }

D_L2(select) { return 2*!!(r&ARITH_t) + !!(l&L_t); }
D_D2(select) { return select_l2(T(l),T(r)); }
D_P2(select) {
  I i=Z(r), ll=L(l)->l; if (i<0 && i>-ll) i+=ll;
  *(V*)p = listV_at(l,i); del(l);del(r);
}
D_P2(take) {
  l=get(l); I i=Z(r), ll=L(l)->l, c=L(l)->c, o=L(l)->o;
  if (i>=0) L(l)->l=i; else { L(l)->o=(o+ll+i)%c; L(l)->l=-i; }
  del(r); *(V*)p=l;
}
D_P2(drop) {
  l=get(l); I i=Z(r), ll=L(l)->l, c=L(l)->c, o=L(l)->o;
  if (i>=0) { L(l)->l-=i; L(l)->o=(o+i)%c; } else L(l)->l+=i;
  del(r); *(V*)p=l;
}

D_P1(length) { I n; if (T(l)&L_t) n=L(l)->l; else n=1; del(l); *(Z*)p=n; }
D_L2(copy) { return 2*!!(r&ARITH_t) + 1; }
D_D2(copy) { return copy_l2(T(l),T(r)); }
D_P2(copy) {
  I t=T(l), ll=Z(r), s=t_sizeof(T(l)); del(r);
  P v=MALLOC(next_pow_2(ll*s)); DDO(i,ll) valcpy(v+i*s,P(l),t);
  del(l); *(L*)p=wrapArray(t, ll, v);
}

D_P1(iota) {
  I ll=getCeilZ(l); del(l);
  I w=(ll<0); if (w) ll=-ll;
  I c=next_pow_2(ll); DECL_ARR(Z,v,c);
  if (w) { DDO(i,ll)v[i]=ll-1-i; } else { DDO(i,ll)v[i]=i; }
  *(L*)p=wrapL(Z_t, c, ll, 0, v);
}
D_P2(iota) {
  switch (T(l)) {
    case Z_t: { Z lz=getZ(l), ll=getCeilZ(r)-lz; del(l); del(r);
                I s=sign(ll); ll*=s; I c=next_pow_2(ll); DECL_ARR(Z,v,c);
                DDO(i,ll)v[i]=lz+s*i; *(L*)p=wrapL(Z_t, c, ll, 0, v);
              }
    case R_t: { R lr=getR(l), llr=getR(r)-lr; del(l); del(r);
                I s=sign(llr), ll=ceiling(s*llr);
                I c=next_pow_2(ll); DECL_ARR(R,v,c);
                DDO(i,ll)v[i]=lr+(R)s*i; *(L*)p=wrapL(R_t, c, ll, 0, v);
              }
  }
}

D_L11(reduce) { return !!(ll&(L_t|CONST_t)); }
D_D11(reduce) { return reduce_l11(l,T(ll)); }
D_L12(reduce) { return 2 + !!(ll&(L_t|CONST_t)); }
D_D12(reduce) { return 2 + reduce_l12(l,T(ll),T(rr)); }
D_T1(identity_of) { return E_t; }
D_P1(identity_of) { del(l); *(E*)p = strdup("Identity unknown"); }
D_P11(reduce) {
  if (!(T(ll)&L_t)) { *(V*)p=ll; }
  I len=L(ll)->l;
  if (len==0) { del(ll); identity_of_p1(p,l); }
  ll=get(ll); V lv; T(lv)=L(ll)->t; I s=t_sizeof(T(lv)); P(lv)=MALLOC(s);
  V v=listV_at(ll,0); I i=1;
  for(;i<len;i++) { memcpy(P(lv),LIST_PTR_AT(L(ll),i),s); v=apply2(l,v,lv); }
  FREE(L(ll)->p); FREE(L(ll)); FREE(P(ll)); *(V*)p=v;
}
D_P12(reduce) {
  if (!(T(ll)&L_t)) { *(V*)p=apply2(l,ll,rr); }
  I len=L(ll)->l;
  DDO(i,len) rr=apply2(l,rr,listV_at(ll,i));
  del(ll); *(V*)p=rr;
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
  *(L*)p=L(l);
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
