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

// EXPORT DEFINITIONS
EXTERN_BUILTINS;
void list_init() {
  B_f1[';'] = &itemize_f1;
  B_f2[';'] = &cross_f2;

  B_f2[','] = &concat_f2;
}
