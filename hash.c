#include <string.h>

#include "type.h"
#include "hash.h"

/*
Hashing function, to be used for I values.

This function is designed to take O(sqrt(n)) time for an input of
memory usage n.
This means not all data is necessarily examined, and the hash may show
poor performance on similar pieces of data.

The algorithm:
Hashes v recursively according to the type.
hash_UI casts to U* and filters as follows:
If the length is less than 256, hash everything.
Otherwise, for length-n data, hash n.q bytes, choosing the next byte using
the current hash.

Note that because everything is hashed for small arguments, deeply
recursive structures can still take O(n) time.
*/

U hash_UI(I l, U* v) {
  U h=2184626789L;
  if (l<256) { DO(i,l) h = (i+1000003)*(*v++) ^ h<<1; }
  else { I i; for (i=0; i*i<l; i++) h = (i+1000003)**(v+h%l) ^ h<<1; }
  return h;
}
U hash_string(Str v) {
  return hash_UI(strlen(v)/sizeof(U), (U*)v);
}
U hash_LIST(L l) {
  U h=2184626789L; I n=l->l;
  if (n<256) { DO(i,n) h = (i+1000003)*hash(list_at(l,i)) ^ h<<1; }
  else { I i; for (i=0; i*i<n; i++) h = (i+1000003)*hash(list_at(l,h%n)) ^ h<<1; }
  return h;
}

U hash(V v) {
  U h=2184626789L;
  switch (T(v)) {
    case E_t: case N_t: case Q_t: return hash_string(E(v));
    case B_t: case C_t: return h * B(v);
    case O_t: case F_t: { O o=O(v); I l = o->l+1; U u[l];
                          DO(i,l-1) u[i]=hash(o->x[i]);
                          u[l]=hash(o->f); return hash_UI(l, u); }
#define LINE(T) case T##_t: return hash_UI(sizeof(T)/sizeof(U), (U*)P(v));
    LINE(Z) LINE(R) LINE(K)
#undef LINE
    case L_t: return hash_LIST(L(v));
  }
}
