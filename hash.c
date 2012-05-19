/*
Hashing function, to be used for I values.

This function is designed to take O(sqrt(n)) time for an input of
memory usage n.
This means not all data is necessarily examined, and the hash may show
poor performance on similar pieces of data.

The algorithm:
Hashes *v->v recursively according to the type.
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
UI hash_string(Str* v) {
  return hash_UI(strlen(*v)/sizeof(UI), (UI*)*v);
}
UI hash_LIST(V v) {
  UI h=2184626789L; I l=LIST_L(v);
  if (l<256) { DDO(i,l) h = (i+1000003)*hash(list_at(v,i)) ^ h<<1; }
  else { I i; for (i=0; i*i<l; i++) h = (i+1000003)*hash(list_at(v,h%l)) ^ h<<1; }
  return h;
}

UI hash(V v) {
  UI h=2184626789L;
  switch (v->t) {
    case E_t: case N_t: case Q_t: return hash_string(v->v);
    case B_t: case S_t: return h * *(B)v->v;
    case O_t: case F_t: { I l = ((O)v->v)->l+1; UI u[l];
                          DDO(i,l-1) u[i]=hash(((O)v->v)->x[i]);
                          u[l]=hash(((O)v->v)->f); return hash_UI(l, u); }
#define LINE(T) case T##_t: return hash_UI(sizeof(T##v)/sizeof(UI), v->v);
    LINE(Z) LINE(R) LINE(C)
#undef LINE
    case L_t: case A_t: return hash_LIST(v->v);
  }
}
