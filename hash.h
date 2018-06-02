U hash_string(Str);
U hash(V);

#define _SET_HASH_TABLE_H_AS(K, V, name, L, M) \
  typedef struct L { K k; V v; struct L* n; } *L;                        \
  typedef struct { U l; U n; L* v; } *M;                                 \
  M name##new(U l);                                                      \
  void name##set(M, K, V);                                               \
  void name##freeL(L l);                                                 \
  M name##resize(M m, U l);                                              \
  void name##free(M m);                                                  \
  V name##get(M m, K k);                                                 \
  void name##del(M m, K k);                                              \

#define SET_HASH_TABLE_H_AS(K, V, name) \
  _SET_HASH_TABLE_H_AS(K, V, name, name##_HashLink, name##_HashMap)
#define SET_HASH_TABLE_H(K, V) \
  SET_HASH_TABLE_H_AS(K, V, K##V)

#define _SET_HASH_TABLE_AS(K, V, hash, equals, freeK, freeV, name, L, M, N) \
  M name##new(U l) { l=next_pow_2(l);                                    \
    DECL(M, m); m->l=l; m->n=0; m->v=malloc(l*sizeof(*m->v));            \
    DO(i,l) m->v[i]=NULL; return m; }                                    \
  void name##freeL(L l) { freeK(l->k); freeV(l->v); FREE(l); }           \
  M name##resize(M m, U l) { M mm=name##new(l);                          \
    DO(i,l) { L n=m->v[i]; while(n){                                     \
      name##set(mm, n->k, n->v); L nn=n; n=n->n; name##freeL(nn);        \
    } } mm->l=l; mm->n=m->n; FREE(m->v); FREE(m); return mm; }           \
  void name##set(M m, K k, V v) {                                        \
    if (m->n++==m->l) m=name##resize(m, m->l*2);                         \
    U i=hash(k)%m->l; L n=m->v[i];                                       \
    for (L nn=n; nn; nn=nn->n) if(equals(k,nn->k)) {                     \
      freeK(k); freeV(nn->v); nn->v=v; m->n--; return;                   \
    } DECL(L, l); l->k=k; l->v=v; l->n=n; m->v[i]=l; }                   \
  void name##free(M m) { DO(i,m->l){ L n=m->v[i];                        \
    while(n){ L nn=n; n=n->n; name##freeL(nn); } } FREE(m->v); FREE(m); }\
  V name##get(M m, K k) { L n=m->v[hash(k)%m->l];                        \
    while(n){if(equals(k,n->k))return n->v; else n=n->n;} return (N); }  \
  void name##del(M m, K k) {                                             \
    L p=NULL, n=m->v[hash(k)%m->l]; while(n) { if (equals(k,n->k)) {     \
      if(p) p->n=n->n; else m->v[hash(k)%m->l]=n->n;                     \
        name##freeL(n); break;                                           \
      } else { p=n; n=n->n; } } }


#define SET_HASH_TABLE_AS(K, V, hash, equals, freeK, freeV, name, N) \
  _SET_HASH_TABLE_AS(K, V, hash, equals, freeK, freeV, name, \
      name##_HashLink, name##_HashMap, N)
#define SET_HASH_TABLE(K, V, hash, equals, freeK, freeV, N) \
  SET_HASH_TABLE_AS(K, V, hash, equals, freeK, freeV, K##V, N)
