#define DECL_V(type, v) V v=MALLOC(sizeof(T)+sizeof(type)); *v=type##_t
V makeO(V f, I l, V* x) {
  DECL_V(O,v); O* oo=(O*)(v+1); oo->r=MALLOC(sizeof(I)); *oo->r=1; oo->f=f; oo->l=l; oo->x=x; return v;
}
V makeF(V f, I l, V* x) {
  DECL_V(F,v); F* ff=(F*)(v+1); ff->r=MALLOC(sizeof(I)); *ff->r=1; ff->f=f; ff->l=l; ff->x=x; return v;
}
V makeC(R a, R b) {
  DECL_V(C,v); C* cc=(C*)(v+1); cc->a=a; cc->b=b; return v;
}
V makeL(T t, I c, I l, I o, Ptr p) {
  DECL_V(L,v); L* ll=(L*)(v+1); ll->r=MALLOC(sizeof(I)); *ll->r=1; ll->t=t; ll->c=c; ll->l=l; ll->o=o; ll->p=p; return v;
}

#define setNew(type) \
  V new##type(type vv) { DECL_V(type,v); type(v)=vv; return v; }
ON_TYPES(ALL, setNew);    // functions new##T : T->V

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

V get(V v) { //TODO
  if (!(*v&COMP_t) || *v==1) return v;
  else { V vv=cpy(v); del(v); return vv; }
}
/*
    case O_t: case F_t: { O *o=(O*)p, oo=*(O*)pp; o->f=cpy(oo.f); o->l=oo.l;
                          o->x=malloc(sizeof(V)*oo.l);
                          DDO(i,oo.l) o->x[i]=cpy(oo.x[i]); break; }
    case L_t: { L *l=(L*)p, ll=*(L*)pp; l->t=ll.t; l->c=ll.c; l->l=ll.l; l->o=0;
                I s=t_sizeof(ll.t); l->p=malloc(s*ll.c);
                DDO(i,ll.l) valcpy(l->p+i*s, LIST_PTR_AT(ll,i), ll.t); break; }
                */

void deln(I n, V* v) { DDO(i,n) del(v[i]); FREE(v); }
V* cpyn(I n, V* v) {DECL_ARR(V,vv,n); DDO(i,n)vv[i]=cpy(v[i]); return vv;}
