D_F1(itemize) { DECL_ARR(V, v, 1); v[0]=l; return wrapList(1,v); }
D_F2(cross) { DECL_ARR(V, v, 2); v[0]=l; v[1]=r; return wrapList(2,v); }

void resize(L* l, I s, I c) { // Assume resizing up
  if (l->c>=c) return; l->p=realloc(l->p, s*c);
  if (l->l+l->o > l->c) memcpy(l->p+s*l->c, l->p, s*(l->l+l->o-l->c));
  l->c=c;
}
D_F2(concat) {
  if (T(l)==L_t) { L* lv=(L*)(l+1);
    if (T(r)==L_t) { L* rv=(L*)(r+1);
      T t=lv->t|rv->t; I ll=lv->l+rv->l, c=next_pow_2(ll);
      I s=t_sizeof(t);
      if (PURE(t)) {
#define AT(a,i) a->p + s*((a->o+i)%a->c)
        if (*lv->r==1 || *rv->r==1) {
          if ((*lv->r==1)*lv->l >= (*rv->r==1)*rv->l) { // Copy to left
            resize(lv,s,c);
            DDO(i,rv->l) valcpy(AT(lv,lv->l+i),AT(rv,i),t);
            del(r); lv->l=ll; return l;
          } else { // Copy to right
            resize(rv,s,c);
            DDO(i,lv->l) valcpy(AT(rv,i-lv->l+rv->c),AT(lv,i),t);
            rv->l=ll; rv->o=(rv->o-lv->l+rv->c)%rv->c; del(l); return r;
          }
        } else {
          Ptr v=malloc(s*c);
          DDO(i,lv->l) valcpy(v+s*i, AT(lv,i), t);
          DO(i,rv->l) valcpy(v+s*(i+lv->l), AT(rv,i), t);
          del(l); del(r); return makeL(t, c, ll, 0, v);
        }
#undef AT
      } else {
#define AT(a,i) ((V*)a->p)[(a->o+i)%a->c]
        I le=(*lv->r==1 && PURE(lv->t)), re=(*rv->r==1 && PURE(rv->t));
        if (le||re) {
          if (le*lv->l >= re*rv->l) { // Copy to left
            resize(lv,s,c);
            DDO(i,rv->l) AT(lv,lv->l+i)=cpy(AT(rv,i));
            del(r); lv->l=ll; return l;
          } else { // Copy to right
            resize(rv,s,c);
            DDO(i,lv->l) AT(rv,i-lv->l+rv->c)=cpy(AT(lv,i));
            rv->l=ll; rv->o=(rv->o-lv->l+rv->c)%rv->c; del(l); return r;
          }
        } else {
          DECL_ARR(V,v,c);
          DDO(i,lv->l) v[i]=cpy(AT(lv,i));
          DO(i,rv->l) v[i+lv->l]=cpy(AT(rv,i));
          del(l); del(r); return makeL(t, c, ll, 0, v);
        }
#undef AT
      }
    } else {
      T t=lv->t|T(r); I ll=lv->l+1, c=next_pow_2(ll);
      I s=t_sizeof(t);
      if (PURE(t)) {
#define AT(a,i) a->p + s*((a->o+i)%a->c)
        if (*lv->r==1) {
          resize(lv,s,c); valcpy(AT(lv,lv->l), r+1, t);
          del(r); lv->l++; return l;
        } else {
          Ptr v=malloc(s*c);
          DDO(i,lv->l) valcpy(v+s*i, AT(lv,i), t);
          valcpy(v+s*lv->l, r+1, t);
          del(r); return makeL(t, c, ll, 0, v);
        }
#undef AT
      } else {
#define AT(a,i) ((V*)a->p)[(a->o+i)%a->c]
        if (*lv->r==1 && PURE(lv->t)) {
          resize(lv,s,c); AT(lv,lv->l)=r; lv->l++; return l;
        } else {
          DECL_ARR(V,v,c);
          DDO(i,lv->l) v[i]=cpy(AT(lv,i)); v[lv->l]=r; del(l);
          return makeL(t, c, ll, 0, v);
        }
#undef AT
      }
    }
  } else {
    return cross_f2(l,r);
  }
}

D_D1(list) { return !!(T(l)&L_t); }
D_F1(first) { V v=listV_at(l,0); del(l); return v; }
D_F1(last) { V v=listV_at(l,L(l).l-1); del(l); return v; }
D_D2(select) { return 2*!!(T(r)&ARITH_t) + !!(T(l)&L_t); }
D_F2(select) {
  I i=Z(r), ll=L(l).l; if (i<0 && i>-ll) i+=ll;
  V v=listV_at(l,i); del(l);del(r);return v;
}
D_F2(take) {
  l=get(l); I i=Z(r), ll=L(l).l, c=L(l).c, o=L(l).o;
  if (i>=0) L(l).l=i; else { L(l).o=(o+ll+i)%c; L(l).l=-i; }
  del(r); return l;
}
D_F2(drop) {
  l=get(l); I i=Z(r), ll=L(l).l, c=L(l).c, o=L(l).o;
  if (i>=0) { L(l).l-=i; L(l).o=(o+i)%c; } else L(l).l+=i;
  del(r); return l;
}

D_F1(length) { I n; if (T(l)&L_t) n=L(l).l; else n=1;
    del(l); return newZ(n); }
D_D2(copy) { return 2*!!(T(r)&ARITH_t) + 1; }
D_F2(copy) {
  I t=T(l), ll=Z(r), s=t_sizeof(T(l)); del(r);
  Ptr p=MALLOC(next_pow_2(ll*s)); DDO(i,ll) valcpy(p+i*s,l+1,t);
  del(l); return wrapArray(t, ll, p);
}

// TODO clean iota
D_F1(iota) {
  I ll=getCeilZ(l); del(l);
  if (ll<0) {
    ll=-ll; I c=next_pow_2(ll);
    DECL_ARR(Z,v,c); DDO(i,ll)v[i]=ll-1-i; return makeL(Z_t, c, ll, 0, v);
  } else {
    I c=next_pow_2(ll);
    DECL_ARR(Z,v,c); DDO(i,ll)v[i]=i; return makeL(Z_t, c, ll, 0, v);
  }
}
D_F2(iota) {
  switch (T(l)) {
    case Z_t: { Z lz=getZ(l), ll=getCeilZ(r)-lz; del(l); del(r);
                I s=sign(ll); ll*=s; I c=next_pow_2(ll); DECL_ARR(Z,v,c);
                DDO(i,ll)v[i]=lz+s*i; return makeL(Z_t, c, ll, 0, v);
              }
    case R_t: { R lr=getR(l), llr=getR(r)-lr; del(l); del(r);
                I s=sign(llr), ll=ceiling(s*llr);
                I c=next_pow_2(ll); DECL_ARR(R,v,c);
                DDO(i,ll)v[i]=lr+(R)s*i; return makeL(R_t, c, ll, 0, v);
              }
  }
}

D_D11(reduce) { return !!(T(ll)&(L_t|CONST_t)); }
D_D12(reduce) { return 2 + !!(T(ll)&(L_t|CONST_t)); }
D_F1(identity_of) { del(l); return Err("Identity unknown"); }
D_F11(reduce) {
  if (!(T(ll)&L_t)) { del(l); return ll; }
  I len=L(ll).l;
  if (len==0) { del(ll); return identity_of_f1(l); }
  V v=listV_at(ll,0);
  I i=1; for(;i<len;i++) v=apply2(cpy(l),v,listV_at(ll,i));
  del(l); del(ll); return v;
}
D_F12(reduce) {
  if (!(T(ll)&L_t)) { return apply2(l,ll,rr); }
  I len=L(ll).l;
  DDO(i,len) rr=apply2(cpy(l),rr,listV_at(ll,i));
  del(l); del(ll); return rr;
}

D_F1(reverse) {
  l=get(l); L v=L(l); I n=v.l; T t=v.t;
  if (!PURE(t)) {
    DDO(i,n/2) {
      V vt=LIST_AT(v,i); LIST_AT(v,i)=LIST_AT(v,n-i-1);
      LIST_AT(v,n-i-1)=vt;
    }
  } else {
#define AT(i) v.p + s*(((i)+v.o)%v.c)
    I s=t_sizeof(t); Ptr p=MALLOC(s);
    DDO(i,n/2) {
      memcpy(p, AT(i), s); memcpy(AT(i), AT(n-i-1), s);
      memcpy(AT(n-i-1), p, s);
    }
    FREE(p);
#undef AT
  }
  return l;
}


EXTERN_BUILTINS;
void list_init() {
  B_f1[';'] = &itemize_f1;
  B_f2[';'] = &cross_f2;

  B_f2[','] = &concat_f2;

  B_d1['g'] = B_d1['G'] = &list_d1;
  B_f1['g'] = &first_f1;
  B_f1['G'] = &last_f1;
  B_d2['}'] = B_d2['g'] = B_d2['G'] = &select_d2;
  B_f2['}'] = &select_f2;
  B_f2['g'] = &take_f2;
  B_f2['G'] = &drop_f2;

  B_f1['#'] = &length_f1;
  B_d2['#'] = &copy_d2;
  B_f2['#'] = &copy_f2;

  B_f1['i'] = &iota_f1;
  B_f2['i'] = &iota_f2;

  B_d11['r'] = &reduce_d11;
  B_f11['r'] = &reduce_f11;
  B_d12['r'] = &reduce_d12;
  B_f12['r'] = &reduce_f12;

  B_f1['z'] = &reverse_f1;
}
