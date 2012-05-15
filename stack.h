#define _SET_STACK_AS(V, name, N, S) \
  typedef struct N { V v; struct N* n; } *N; \
  typedef struct { I l; N h; } *S; \
  void name##push(S s, V v) { \
    N n=s->h; DECL(N, h); h->n=n; h->v=v; s->h=h; s->l++; } \
  V name##pop(S s) { \
    N h=s->h; s->h=h->n; s->l--; V v=h->v; FREE(h); return v; } \
  V name##peek(S s) { return s->h->v; } \
  S name##new() { DECL(S, s); s->l=0; s->h=NULL; return s; } \
  void name##free(S s) { while(s->l) name##pop(s); FREE(s); }

#define SET_STACK_AS(V, name) \
  _SET_STACK_AS(V, name, name##Node, name##Stack)
#define SET_STACK(V) SET_STACK_AS(V, V)
