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
hash_UI casts to UI* and filters as follows:
If the length is less than 256, hash everything.
Otherwise, for length-n data, hash n.q bytes, choosing the next byte using
the current hash.

Note that because everything is hashed for small arguments, deeply
recursive structures can still take O(n) time.
*/

UI hash_UI(I l, UI* v) {
  UI h=2184626789L;
  if (l<256) { DO(i,l) h = (i+1000003)*(*v++) ^ h<<1; }
  else { I i; for (i=0; i*i<l; i++) h = (i+1000003)**(v+h%l) ^ h<<1; }
  return h;
}
UI hash_string(Str v) {
  return hash_UI(strlen(v)/sizeof(UI), (UI*)v);
}
UI hash_LIST(L l) {
  UI h=2184626789L; I n=l->l;
  if (n<256) { DO(i,n) h = (i+1000003)*hash(list_at(l,i)) ^ h<<1; }
  else { I i; for (i=0; i*i<n; i++) h = (i+1000003)*hash(list_at(l,h%n)) ^ h<<1; }
  return h;
}

UI hash(V v) {
  UI h=2184626789L;
  switch (T(v)) {
    case E_t: case N_t: case Q_t: return hash_string(E(v));
    case B_t: case C_t: return h * B(v);
    case O_t: case F_t: { O o=O(v); I l = o->l+1; UI u[l];
                          DO(i,l-1) u[i]=hash(o->x[i]);
                          u[l]=hash(o->f); return hash_UI(l, u); }
#define LINE(T) case T##_t: return hash_UI(sizeof(T)/sizeof(UI), (UI*)P(v));
    LINE(Z) LINE(R) LINE(K)
#undef LINE
    case L_t: return hash_LIST(L(v));
  }
}
