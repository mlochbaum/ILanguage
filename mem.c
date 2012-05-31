// Find next power of two for length: or with
// all down bitshifts, then add one.
I next_pow_2(I l) {
  I c=l-1; I i=32; while (i) { c |= c>>i; i>>=1; } return c+1;
}
#define LINE(T) case T##_t: return sizeof(T);
I t_sizeof(T t) {switch(t){ON_TYPES(ALL,LINE) default: return sizeof(V);}}
#undef LINE


V makeO(V f, I l, V* x) {
  DECL_V(O,v); O* oo=(O*)(v+1); oo->r=MALLOC(sizeof(I));
  *oo->r=1; oo->f=f; oo->l=l; oo->x=x; return v;
}
V makeF(V f, I l, V* x) {
  DECL_V(F,v); F* ff=(F*)(v+1); ff->r=MALLOC(sizeof(I));
  *ff->r=1; ff->f=f; ff->l=l; ff->x=x; return v;
}
V makeC(R a, R b) {
  DECL_V(C,v); C* cc=(C*)(v+1); cc->a=a; cc->b=b; return v;
}
V makeL(T t, I c, I l, I o, Ptr p) {
  DECL_V(L,v); L* ll=(L*)(v+1); ll->r=MALLOC(sizeof(I));
  *ll->r=1; ll->t=t; ll->c=c; ll->l=l; ll->o=o; ll->p=p; return v;
}

#define SET_NEW(T) V new##T(T vv) { DECL_V(T,v); T(v)=vv; return v; }
ON_TYPES(ALL, SET_NEW);    // functions new##T : T->V
#undef SET_NEW

// Custom make functions
V wrapArray(T t, I l, Ptr p) {
  I c=next_pow_2(l);
  p=realloc(p, c*t_sizeof(t));
  return makeL(t,c,l,0,p);
}
V wrapList(I l, V* v) {
  I c=next_pow_2(l);
  T t=0; DDO(i,l)t|=*v[i];
  if(PURE(t)){
    I s=t_sizeof(t); Ptr p=MALLOC(c*s);
    DO(i,l) { memcpy(p+i*s, v[i]+1, s); FREE(v[i]); }
    V r=makeL(t,c,l,0,p); FREE(v); return r;
  } else {
    v=realloc(v,c*sizeof(v)); return makeL(t,c,l,0,v);
  }
}
V makeStr(Str s) { return wrapArray(S_t, strlen(s), s); }
V DErr(Str s) { DECL_V(E, v); E(v)=s; return v; }
V Err(Str s) { return DErr(strdup(s)); }


/////////////////// Utilities ////////////////////
void delPtr(T t, Ptr p) {
  if (PURE(t)) switch (t) {
    case N_t: FREE(*(N*)p); break;
    case Q_t: FREE(*(Q*)p); break;
    case O_t: case F_t: { O* o=(O*)p; if (--*o->r) break;
                          del(o->f); FREE(o->r);
                          DDO(i,o->l) del(o->x[i]); FREE(o->x); break; }
    case L_t: { L* l=(L*)p; if (--*l->r) break; FREE(l->r);
                if (!PURE(l->t)) { DDO(i,l->l) del(LIST_AT(*l,i)); }
                else { DDO(i,l->l) delPtr(l->t, LIST_PTR_AT(*l,i)); }
                FREE(l->p); break; }
  } else {
    del(*(V*)p);
  }
}
void del(V v) { delPtr(*v, v+1); FREE(v); }

Ptr arrcpy(Ptr aa, I s, I l, I c, I o) {
  l*=s; c*=s; o*=s; Ptr a=MALLOC(c);
  memcpy(a,aa+o,min(c-o,l)); memcpy(a,aa,max(0,l+o-c));
  return a;
}
void valcpy(Ptr p, Ptr pp, T t) { // from pp to p
  I s=t_sizeof(t);
  if (PURE(t)) switch (t) {
    case E_t: case N_t: case Q_t: *(E*)p=strdup(*(E*)pp); break;
    case O_t: case F_t: case L_t: (**(I**)pp)++;
    case B_t: case S_t: case Z_t: case R_t: case C_t:
                                  memcpy(p, pp, s); break;
  } else *(V*)p=cpy(*(V*)pp);
}

V cpy(V v) {
  V vv=MALLOC(sizeof(T)+t_sizeof(*v)); *vv=*v;
  valcpy(vv+1,v+1,*v); return vv;
}

void deln(I n, V* v) { DDO(i,n) del(v[i]); FREE(v); }
V* cpyn(I n, V* v) {DECL_ARR(V,vv,n); DDO(i,n)vv[i]=cpy(v[i]); return vv;}

V get(V v) {
  if (!(*v&COMP_t) || REF(v)==1) return v;
  else { REF(v)--; switch (T(v)) {
    case O_t: case F_t:
      { O o=O(v); return makeO(cpy(o.f), o.l, cpyn(o.l, o.x)); }
    case L_t:
      { L l=L(v); I s=t_sizeof(l.t); Ptr p=MALLOC(s*l.c);
        DDO(i,l.l) valcpy(p+i*s, LIST_PTR_ATS(l,i,s), l.t);
        return makeL(l.t, l.c, l.l, 0, p); }
  } }
}


// Numbers
// TODO: complex case
Z getZ(V v) { return Z(v); }
R getR(V v) {
  switch (*v) {
    case Z_t: return (R)Z(v);
    case R_t: return R(v);
  }
}

// List properties
V arr_at(L l, I i) {
  V v=MALLOC(sizeof(T)+t_sizeof(l.t)); *v=l.t;
  valcpy(v+1, LIST_PTR_AT(l,i), l.t); return v;
}
V list_at(L l, I i) {
  if (PURE(l.t)) return arr_at(l, i);
  else return cpy(LIST_AT(l, i));
}
V listV_at(V v, I i) { return list_at(L(v), i); }
