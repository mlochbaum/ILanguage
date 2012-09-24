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
V makeO(V f, I l, V* x) {
  DECL_V(O,v); DECL(O,oo); O(v) = oo;
  oo->r=1; oo->f=f; oo->l=l; oo->x=x; return v;
}
F wrapF(V f, I l, V* x) {
  DECL(F,ff); ff->r=1; ff->f=f; ff->l=l; ff->x=x; return ff;
}
V makeF(V f, I l, V* x) {
  DECL_V(F,v); DECL(F,ff); F(v) = ff;
  ff->r=1; ff->f=f; ff->l=l; ff->x=x; return v;
}
V makeC(R a, R b) {
  DECL_V(C,v); C* cc=P(v); cc->a=a; cc->b=b; return v;
}
V makeL(T t, I c, I l, I o, P p) {
  DECL_V(L,v); DECL(L,ll); L(v) = ll;
  ll->r=1; ll->t=t; ll->c=c; ll->l=l; ll->o=o; ll->p=p; return v;
}

#define SET_NEW(T) V new##T(T vv) { DECL_V(T,v); T(v)=vv; return v; }
ON_TYPES(ALL, SET_NEW);    // functions new##T : T->V
#undef SET_NEW

// Custom make functions
V wrapArray(T t, I l, P p) {
  I c=next_pow_2(l);
  p=realloc(p, c*t_sizeof(t));
  return makeL(t,c,l,0,p);
}
V wrapList(I l, V* v) {
  I c=next_pow_2(l);
  T t=0; DDO(i,l)t|=T(v[i]);
  if(PURE(t)){
    I s=t_sizeof(t); P p=MALLOC(c*s);
    DO(i,l) { memcpy(p+i*s, P(v[i]), s); FREE(P(v[i])); }
    V r=makeL(t,c,l,0,p); FREE(v); return r;
  } else {
    v=realloc(v,c*sizeof(V)); return makeL(t,c,l,0,v);
  }
}
V makeStr(Str s) { return wrapArray(S_t, strlen(s), s); }
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

void freeP(T t, P p) {
  if (PURE(t)) switch (t) {
    case N_t: FREE(*(N*)p); break;
    case Q_t: FREE(*(Q*)p); break;
    case O_t: case F_t: { O o=*(O*)p;
                          if(!(--o->r)) { FREE(o->x); FREE(o); }
                          break; }
    case L_t: { L l=*(L*)p; if(!(--l->r)) { FREE(l->p); FREE(l); } break; }
  } else {
    freeV(*(V*)p);
  }
}
void freeV(V v) { freeP(T(v), P(v)); FREE(P(v)); }

P arrcpy(P aa, I s, I l, I c, I o) {
  l*=s; c*=s; o*=s; P a=MALLOC(c);
  memcpy(a,aa+o,min(c-o,l)); memcpy(a,aa,max(0,l+o-c));
  return a;
}
void mv_P(P p, V v) { memcpy(p, P(v), t_sizeof(T(v))); }
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
      { O o=O(v); r=makeO(cpy(o->f), o->l, cpyn(o->l, o->x)); break;}
    case F_t:
      { F f=F(v); r=makeF(cpy(f->f), f->l, cpyn(f->l, f->x)); break;}
    case L_t:
      { L l=L(v); I s=t_sizeof(l->t); P p=MALLOC(s*l->c);
        DDO(i,l->l) valcpy(p+i*s, LIST_PTR_ATS(l,i,s), l->t);
        r=makeL(l->t, l->c, l->l, 0, p); break; }
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
V arr_at(L l, I i) {
  V v; T(v)=l->t; P(v)=MALLOC(t_sizeof(l->t));
  valcpy(P(v), LIST_PTR_AT(l,i), l->t); return v;
}
V list_at(L l, I i) {
  if (PURE(l->t)) return arr_at(l, i);
  else return cpy(LIST_AT(l, i));
}
V listV_at(V v, I i) { return list_at(L(v), i); }
