V apply    (V, I, V*);
I dom      (V, I, V*);
X mapclass (V);
V fmap     (V, I, V*, I);

// apply.c
#define DECLARE_APPLY(T) V apply_##T(T, I, V*);
ON_TYPES(NCONST,DECLARE_APPLY)
#undef DECLARE_APPLY
V apply_FB(F, I, V*);
//V apply_FQ(F, I, V*);

V apply1(V, V);
V apply2(V, V, V);

// fmap.c
V fmap_LIST(V, I, V*, I, I);

// dom.c
#define DECLARE_DOM(T) I dom_##T(T, I, V*);
DECLARE_DOM(B) DECLARE_DOM(F) DECLARE_DOM(N) DECLARE_DOM(Q)
#undef DECLARE_DOM

I dom_true(I);
