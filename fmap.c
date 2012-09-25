T mapclass_T(T t) {
  switch (t) {
#define CASE(T) case T##_t:
    ON_TYPES(CONST, CASE) return CONST_X;
    ON_TYPES(FUNC , CASE) return FUNC_X;
    case L_t: return LIST_X;
#undef CASE
  }
}
X mapclass(V v) {
  X x; switch (T(v)) {
#define CASE(T) case T##_t:
    ON_TYPES(CONST, CASE) x.t=CONST_X; break;
    ON_TYPES(FUNC , CASE) x.t=FUNC_X; break;
    case L_t: x.t=LIST_X; x.v=&(L(v)->l); break;
#undef CASE
  }
  return x;
}

I mapclasseq(X x, X y) {
  return x.t==y.t && (x.t!=LIST_X || *((I*)x.v)==*((I*)y.v));
}

// TODO move/simplify
V constant(V v) {
  if (T(v)&CONST_t) return v;
  DECL_V(B,f); B(f)='k'; WRAP(V*,vv,v); return makeF(f,1,vv);
}

V fmap(V f, I n, V* x, I d) {
  T t[n]; DDO(i,n) t[i]=T(x[i]);
  V v; T(v)=fmap_T(f,n,t,d,d); P(v)=MALLOC(t_sizeof(T(v)));
  fmap_P(P(v),f,n,x,d); return v;
}
T fmap_T(V f, I n, T* x, I dl, I du) { return fmap_TT(T(f),n,x,dl,du); }
T fmap_TT(T f, I n, T* x, I dl, I du) {
  T tl=~0, tu=~0;
  DDO(i, n) {
    if (! (dl&1<<i)) { tl &= mapclass_T(x[i]);
      if (! (du&1<<i)) tu &= mapclass_T(x[i]); }
  }
  if ((!tu) || (!PURE(tl))) return E_t;
  T t=0; if (!PURE(tu)) t|=E_t;
  if (tu & CONST_X) t |= E_t;
  if (tu & FUNC_X ) t |= O_t;
  if (tu & LIST_X ) t |= L_t + E_t;
  return t;
}
void fmap_P(P v, V f, I n, V* x, I d) {
  T t=T(f); P p=P(f);
  X m={0,NULL};
  DDO(i, n) if (! (d&1<<i)) {
    X mt=mapclass(x[i]);
    if (!m.t) m=mt;
    else if (!mapclasseq(m,mt)) {
      // TODO p might not have type V
      DDO(j,n)del(x[j]); *(V*)p = Err("Incompatible mapclasses");
    }
  }
  switch (m.t) {
    case CONST_X: DO(i,n)del(x[i]); Err_T(p, "Domain error");
    case FUNC_X:  { DECL_ARR(V,xx,n); DDO(i,n) {
                    if (d&1<<i) xx[i]=constant(x[i]); else xx[i]=x[i];
                  } *(O*)v = wrapO(cpy(f),n,xx); return; }
    case LIST_X:  return fmap_LIST_P(v, f, n, x, d, *(I*)m.v);
  }
}


void fmap_LIST_P(P v, V f, I n, V* x, I d, I l) {
  T ts[n]; { DDO(i,n) ts[i]=L(x[i])->t; } T t = apply_T(f, n, ts);
  I s=t_sizeof(t);
  DECL(L, ll); *(L*)v = ll; ll->p = MALLOC(l*s);
  I i[n], c[n]; V xi[n];
  DDO(j, n) if (!(d&1<<j)) { i[j]=L(x[j])->o; c[j]=L(x[j])->c; }
  DDO(k, l) {
    DO(j,n) {
      if (d&1<<j) { xi[j]=cpy(x[j]); }
      else { xi[j]=listV_at(x[j],i[j]); i[j]++; if(i[j]==c[j]) i[j]=0; }
    }
    apply_P(LIST_PTR_ATS(ll,k,s), f, n, xi);
  }
  DO(j, n) del(x[j]);
}
