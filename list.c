D_F1(itemize) { DECL_ARR(V, v, 1); v[0]=l; return wrapList(1,v); }
D_F2(cross) { DECL_ARR(V, v, 2); v[0]=l; v[1]=r; return wrapList(2,v); }

//TODO: in-place modification
D_F2(concat) {
  if (l->t & LIST_t) {
    if (r->t & LIST_t) {
      T tl=LIST_T(l), tr=LIST_T(r), t=tl|tr;
      I ll=LIST_L(l), lr=LIST_L(r), lv=ll+lr, c=next_pow_2(lv);
      if (0==t^(t-1)) {
        I s = t_sizeof(t);
        Ptr v=malloc(s*c);
        memcpy(v, ((A)l->v)->v, s*ll); memcpy(v+s*ll, ((A)r->v)->v, s*lr);
        del(l); del(r); return makeA(t, c, lv, 0, v);
      } else {
        DECL_ARR(V,v,c);
        DFOR_EACH(i, (L)l->v) v[i]=list_at(l,i);
        FOR_EACH(i, (L)r->v) v[i+ll]=list_at(r,i);
        del(l); del(r); return makeL(t, c, lv, 0, v);
      }
    } else {
      T tl=LIST_T(l), t=tl|r->t;
      I ll=LIST_L(l), c=next_pow_2(ll+1);
      if (A_t==l->t && 0==t^(t-1)) {
        A a=l->v; I s=t_sizeof(t);
        Ptr v=malloc(s*c);
        memcpy(v, a->v, s*a->l); memcpy(v+s*a->l, r->v, s);
        del(l); del(r); return makeA(t, c, ll+1, 0, v);
      } else {
        DECL_ARR(V,v,c);
        DFOR_EACH(i, (L)l->v) v[i]=list_at(l,i); v[ll]=r;
        del(l); return makeL(t, c, ll+1, 0, v);
      }
    }
  } else {
    return cross_f2(l,r);
  }
}

D_D2(select) { return 2*!!(r->t&ARITH_t) + !!(l->t&LIST_t); }
D_F2(select) {
  I i=*(Z)r->v, ll=LIST_L(l); if (i<0 && i>-ll) i+=ll;
  V v=list_at(l,i); del(l);del(r);return v;
}

// TODO clean iota
D_F1(iota) {
  I ll=getCeilZ(l); del(l);
  if (ll<0) {
    ll=-ll; I c=next_pow_2(ll);
    DECL_ARR(Zv,v,c); DDO(i,ll)v[i]=ll-1-i; return makeA(Z_t, c, ll, 0, v);
  } else {
    I c=next_pow_2(ll);
    DECL_ARR(Zv,v,c); DDO(i,ll)v[i]=i; return makeA(Z_t, c, ll, 0, v);
  }
}
D_F2(iota) {
  switch (l->t) {
    case Z_t: { Zv lz=getZ(l), ll=getCeilZ(r)-lz; del(l); del(r);
                I s=sign(ll); ll*=s; I c=next_pow_2(ll); DECL_ARR(Zv,v,c);
                DDO(i,ll)v[i]=lz+s*i; return makeA(Z_t, c, ll, 0, v);
              }
    case R_t: { Rv lr=getR(l), llr=getR(r)-lr; del(l); del(r);
                I s=sign(llr), ll=ceiling(s*llr);
                I c=next_pow_2(ll); DECL_ARR(Rv,v,c);
                DDO(i,ll)v[i]=lr+(Rv)s*i; return makeA(R_t, c, ll, 0, v);
              }
  }
}

D_D11(reduce) { return !!(ll->t&(LIST_t|CONST_t)); }
D_D12(reduce) { return 2 + !!(ll->t&(LIST_t|CONST_t)); }
D_F1(identity_of) { del(l); return Err("Identity unknown"); }
D_F11(reduce) {
  if (!(ll->t&LIST_t)) { del(l); return ll; }
  I len=LIST_L(ll);
  if (len==0) { del(ll); return identity_of_f1(l); }
  V v=list_at(ll,0);
  increfn(l, len-2); I i=1; for(;i<len;i++) v=apply2(l,v,list_at(ll,i));
  del(ll); return v;
}
D_F12(reduce) {
  if (!(ll->t&LIST_t)) { return apply2(l,ll,rr); }
  I len=LIST_L(ll);
  increfn(l, len-1); I i=0; for(;i<len;i++) rr=apply2(l,rr,list_at(ll,i));
  del(ll); return rr;
}

// EXPORT DEFINITIONS
EXTERN_BUILTINS;
void list_init() {
  B_f1[';'] = &itemize_f1;
  B_f2[';'] = &cross_f2;

  B_f2[','] = &concat_f2;

  B_f2['}'] = &select_f2;
  B_d2['}'] = &select_d2;

  B_f1['i'] = &iota_f1;
  B_f2['i'] = &iota_f2;

  B_d11['r'] = &reduce_d11;
  B_f11['r'] = &reduce_f11;
  B_d12['r'] = &reduce_d12;
  B_f12['r'] = &reduce_f12;
}
