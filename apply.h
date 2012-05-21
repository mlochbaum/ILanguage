V apply    (V, I, V*);
I dom      (V, I, V*); // Return a single int with multiple bits
X mapclass (V);
V fmap     (V, I, V*, I);

#define DECLARE(T) V apply_##T(T, I, V*);
ON_TYPES(NCONST,DECLARE)
#undef DECLARE

V apply_FB(F, I, V*);
//V apply_FQ(F, I, V*);

V fmap_LIST(V, I, V*, I, I);

V apply1(V, V);
V apply2(V, V, V);
