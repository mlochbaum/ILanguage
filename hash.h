UI hash_string(Str);
UI hash(V);

#define _SET_HASH_TABLE_H_AS(K, V, name, L, M) \
  typedef struct L { K k; V v; struct L* n; } *L;                        \
  typedef struct { I l; I n; L* v; } *M;                                 \
  M name##new(I l);                                                      \
  void name##set(M, K, V);                                               \
  void name##freeL(L l);                                                 \
  M name##resize(M m, I l);                                              \
  void name##free(M m);                                                  \
  V name##get(M m, K k);                                                 \
  void name##del(M m, K k);                                              \

#define SET_HASH_TABLE_H_AS(K, V, name) \
  _SET_HASH_TABLE_H_AS(K, V, name, name##_HashLink, name##_HashMap)
#define SET_HASH_TABLE_H(K, V) \
  SET_HASH_TABLE_H_AS(K, V, K##V)

#define _SET_HASH_TABLE_AS(K, V, hash, equals, freeK, freeV, name, L, M) \
  M name##new(I l) { l=next_pow_2(l);                                    \
    DECL(M, m); m->l=l; m->n=0; m->v=malloc(l*sizeof(m->v));             \
    DDO(i,l) m->v[i]=NULL; return m; }                                   \
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
