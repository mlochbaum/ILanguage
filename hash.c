/*
Hashing function, to be used for I values.

This function is designed to take O(sqrt(n)) time for an input of
memory usage n.
This means not all data is necessarily examined, and the hash may show
poor performance on similar pieces of data.

The algorithm:
Hashes v recursively according to the type.
hash_UI casts to UI* and filters as follows:
If the length is less than 256, hash everything.
Otherwise, for length-n data, hash n.q bytes, choosing the next byte using
the current hash.

Note that because everything is hashed for small arguments, deeply
recursive structures can still take O(n) time.
*/

UI hash(V);

UI hash_UI(I l, UI* v) {
  UI h=2184626789L;
  if (l<256) { DDO(i,l) h = (i+1000003)*(*v++) ^ h<<1; }
  else { I i; for (i=0; i*i<l; i++) h = (i+1000003)**(v+h%l) ^ h<<1; }
  return h;
}
UI hash_string(Str v) {
  return hash_UI(strlen(v)/sizeof(UI), (UI*)v);
}
UI hash_LIST(L l) {
  UI h=2184626789L; I n=l->l;
  if (n<256) { DDO(i,n) h = (i+1000003)*hash(list_at(l,i)) ^ h<<1; }
  else { I i; for (i=0; i*i<n; i++) h = (i+1000003)*hash(list_at(l,h%n)) ^ h<<1; }
  return h;
}

UI hash(V v) {
  UI h=2184626789L;
  switch (T(v)) {
    case E_t: case N_t: case Q_t: return hash_string(E(v));
    case B_t: case S_t: return h * B(v);
    case O_t: case F_t: { O o=O(v); I l = o->l+1; UI u[l];
                          DDO(i,l-1) u[i]=hash(o->x[i]);
                          u[l]=hash(o->f); return hash_UI(l, u); }
#define LINE(T) case T##_t: return hash_UI(sizeof(T)/sizeof(UI), (UI*)P(v));
    LINE(Z) LINE(R) LINE(C)
#undef LINE
    case L_t: return hash_LIST(L(v));
  }
}


#define _SET_HASH_TABLE_AS(K, V, hash, equals, freeK, freeV, name, L, M) \
  typedef struct L { K k; V v; struct L* n; } *L;                        \
  typedef struct { I l; I n; L* v; } *M;                                 \
  M name##new(I l) { l=next_pow_2(l);                                    \
    DECL(M, m); m->l=l; m->n=0; m->v=malloc(l*sizeof(m->v));             \
    DDO(i,l) m->v[i]=NULL; return m; }                                   \
  void name##set(M, K, V);                                               \
  void name##freeL(L l) { freeK(l->k); freeV(l->v); FREE(l); }           \
  M name##resize(M m, I l) { M mm=name##new(l);                          \
    DDO(i,l) { L n=m->v[i],nn; while(n){                                 \
      name##set(mm, n->k, n->v); nn=n; n=n->n; name##freeL(nn);          \
    } } mm->l=l; mm->n=m->n; FREE(m->v); FREE(m); return mm; }           \
  void name##set(M m, K k, V v) {                                        \
    if (m->n==m->l) m=name##resize(m, m->l*2);                           \
    I i=hash(k)%m->l; L n=m->v[i];                                       \
    DECL(L, l); l->k=k; l->v=v; l->n=n; m->v[i]=l; }                     \
  void name##free(M m) { DDO(i,m->l){ L n=m->v[i],nn;                    \
    while(n){ nn=n; n=n->n; name##freeL(nn); } } FREE(m->v); FREE(m); }  \
  V name##get(M m, K k) { L n=m->v[hash(k)%m->l];                        \
    while(n){if(equals(k,n->k))return n->v; else n=n->n;} return Err(""); /* TODO */} \
  void name##del(M m, K k) {                                             \
    L p=NULL, n=m->v[hash(k)%m->l]; while(n) { if (equals(k,n->k)) {     \
      if(p) p->n=n->n; else m->v[hash(k)%m->l]=n->n;                     \
        name##freeL(n); break;                                           \
      } else { p=n; n=n->n; } } }


#define SET_HASH_TABLE_AS(K, V, hash, equals, freeK, freeV, name) \
  _SET_HASH_TABLE_AS(K, V, hash, equals, freeK, freeV, name, \
      name##_HashLink, name##_HashMap)
#define SET_HASH_TABLE(K, V, hash, equals, freeK, freeV) \
  SET_HASH_TABLE_AS(K, V, hash, equals, freeK, freeV, K##V)

// TODO complete equality testing
// I equalsV(V l, V r) { return compare_arith(l,r)==0; }
// SET_HASH_TABLE(V, V, hash, equalsV, del, del)
