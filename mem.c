// Find next power of two for length: or with
// all down bitshifts, then add one.
I next_pow_2(I l) {
  I c=l-1; I i=32; while (i) { c |= c>>i; i>>=1; } return c+1;
}
#define LINE(T) case T##_t: return sizeof(T);
I t_sizeof(T t) {switch(t){ON_TYPES(ALL,LINE) default: return sizeof(V);}}
#undef LINE

V wrapP(T t, P p) {
  if (!PURE(t)) { V v=*(V*)p; FREE(p); return cpy(v); }
  else { V v; T(v)=t; P(v)=p; return cpy(v); }
}

O wrapO(V f, I l, V* x) {
  DECL(O,oo); oo->r=1; oo->f=f; oo->l=l; oo->x=x; return oo;
}
F wrapF(V f, I l, V* x) {
  DECL(F,ff); ff->r=1; ff->f=f; ff->l=l; ff->x=x; return ff;
}
L wrapL(T t, I c, I l, I o, P p) {
  DECL(L,ll); ll->r=1; ll->t=t; ll->c=c; ll->l=l; ll->o=o; ll->p=p; return ll;
}

#define SET_NEW(T) V new##T(T vv) { DECL_V(T,v); T(v)=vv; return v; } \
  void set##T(V v,T t) { if (!PURE(v.t)) V(v)=new##T(t); else T(v)=t; }
ON_TYPES(ALL, SET_NEW);    // functions new##T : T->V
#undef SET_NEW

// Custom wrap functions
L wrapArray(T t, I l, P p) {
  I c=next_pow_2(l);
  p=realloc(p, c*t_sizeof(t));
  return wrapL(t,c,l,0,p);
}
L wrapList(I l, V* v) {
  I c=next_pow_2(l);
  T t=0; DDO(i,l)t|=T(v[i]);
  if(PURE(t)){
    I s=t_sizeof(t); P p=MALLOC(c*s);
    DO(i,l) { memcpy(p+i*s, P(v[i]), s); FREE(P(v[i])); }
    L r=wrapL(t,c,l,0,p); FREE(v); return r;
  } else {
    v=realloc(v,c*sizeof(V)); return wrapL(t,c,l,0,v);
  }
}
L wrapStr(Str s) { return wrapArray(S_t, strlen(s), s); }
V makeStr(Str s) { L l=wrapStr(s); return newL(l); }
V DErr(Str s) { DECL_V(E, v); E(v)=s; return v; }
V Err(Str s) { return DErr(strdup(s)); }
void Err_T(E* p, Str s) { *p = strdup(s); return; }


/////////////////// Utilities ////////////////////
void delP(T t, P p) {
  if (PURE(t)) switch (t) {
    case N_t: FREE(*(N*)p); break;
    case Q_t: FREE(*(Q*)p); break;
    case O_t: case F_t: { O o=*(O*)p; if (--o->r) break;
                          del(o->f); DDO(i,o->l) del(o->x[i]); FREE(o->x);
                          FREE(o); break; }
    case L_t: { L l=*(L*)p; if (--l->r) break;
                if (!PURE(l->t)) { DDO(i,l->l) del(LIST_AT(l,i)); }
                else { DDO(i,l->l) delP(l->t, LIST_PTR_AT(l,i)); }
                FREE(l->p); FREE(l); break; }
  } else {
    del(*(V*)p);
  }
}
void del(V v) { delP(T(v), P(v)); FREE(P(v)); }

P arrcpy(P aa, I s, I l, I c, I o) {
  l*=s; c*=s; o*=s; P a=MALLOC(c);
  memcpy(a,aa+o,min(c-o,l)); memcpy(a,aa,max(0,l+o-c));
  return a;
}
void mv_P(V p, V v) {
  if (!PURE(T(p))) V(p)=v;
  else if (T(p)==T(v)) {
    memcpy(P(p), P(v), t_sizeof(T(v))); FREE(P(v));
  } else printf("Internal error: type mismatch in mv_P");
}
void valcpy(P p, P pp, T t) { // from pp to p
  I s=t_sizeof(t);
  if (PURE(t)) switch (t) {
    case E_t: case N_t: case Q_t: *(E*)p=strdup(*(E*)pp); break;
    case O_t: case F_t: case L_t: (**(I**)pp)++;
    case B_t: case S_t: case Z_t: case R_t: case C_t:
                                  memcpy(p, pp, s); break;
  } else *(V*)p=cpy(*(V*)pp);
}

V cpy(V v) {
  V vv; T(vv)=T(v); P(vv)=MALLOC(t_sizeof(T(v)));
  valcpy(P(vv),P(v),T(v)); return vv;
}

void deln(I n, V* v) { DDO(i,n) del(v[i]); FREE(v); }
V* cpyn(I n, V* v) {DECL_ARR(V,vv,n); DDO(i,n)vv[i]=cpy(v[i]); return vv;}

V get(V v) {
  if (!(T(v)&COMP_t) || REF(v)==1) return v;
  else { REF(v)--; V r; switch (T(v)) {
    case O_t:
      { O o=O(v); r=newO(wrapO(cpy(o->f), o->l, cpyn(o->l, o->x))); break;}
    case F_t:
      { F f=F(v); r=newF(wrapF(cpy(f->f), f->l, cpyn(f->l, f->x))); break;}
    case L_t:
      { L l=L(v); I s=t_sizeof(l->t); P p=MALLOC(s*l->c);
        DDO(i,l->l) valcpy(p+i*s, LIST_PTR_ATS(l,i,s), l->t);
        r=newL(wrapL(l->t, l->c, l->l, 0, p)); break; }
  } FREE(P(v)); return r; }
}


// Numbers
// TODO: complex case
Z getZ(V v) { return Z(v); }
R getR(V v) {
  switch (T(v)) {
    case Z_t: return (R)Z(v);
    case R_t: return R(v);
  }
}

// List properties
V list_at(L l, I i) {
  if (PURE(l->t)) {
    V v; T(v)=l->t; P(v)=MALLOC(t_sizeof(l->t));
    valcpy(P(v), LIST_PTR_AT(l,i), l->t); return v;
  } else return cpy(LIST_AT(l, i));
}
V listV_at(V v, I i) { return list_at(L(v), i); }
