#include <string.h>
#include "builtin.h"

B match(V l, V r) {
  T t=T(l);
  if (t!=T(r)) return 0;
  switch (t) {
    default: return 0==memcmp(l.p, r.p, t_sizeof(t));
    case E_t: case N_t: case Q_t: return 0==strcmp(E(l), E(r));
    case O_t: case F_t: {
      O ol=O(l), or=O(r); U len=ol->l;
      if (len!=or->l || !match(ol->f,or->f)) return 0;
      DO(i,len) { if (!match(ol->x[i], or->x[i])) return 0; }
      return 1;
    }
    case L_t: {
      L ll=L(l), lr=L(r); U n=ll->l; T tl=ll->t, tr=lr->t;
      if (n!=lr->l || PURE(tl)!=PURE(tr)) return 0;
      if (PURE(tl)) {
        if (tl!=tr) return 0;
        U s=t_sizeof(tl),
          cl=ll->c*s, cr=lr->c*s,
          ol=ll->o*s, or=lr->o*s,
          a=cl-ol, b=cr-or;
        n *= s;
        P pl=ll->p+ol, pr=lr->p+or;
#define MCMP(N) if (memcmp(pl,pr,N)) return 0; pl+=N; pr+=N; n-=N
        if (a<n) {
          if (b<a) { MCMP(b); pr-=cr; a-=b; b=n; }
          MCMP(a); pl-=cl; b-=a;
        }
        if (b<n) { MCMP(b); pr-=cr; }
        MCMP(n);
#undef MCMP
      } else {
        DO(i,n) { if (!match(LIST_AT(ll,i), LIST_AT(lr,i))) return 0; }
      }
      return 1;
    }
  }
}

D_P2(match) {
  setZ(p, match(l,r)); del(l); del(r);
}


void set_init() {
  DB(t2,'E',Z); DB(p2,'E',match);
}
