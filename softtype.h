T apply_T      (V, I, T*);
I dom_T        (V, I, T*);
T mapclass_T   (T);
T fmap_T       (V, I, T*, I);

#define DECLARE_APPLY(TT) T apply_T_##TT(TT, I, T*);
ON_TYPES(NCONST,DECLARE_APPLY)
#undef DECLARE_APPLY
T apply_T_FB(F, I, T*);
//T apply_T_FQ(F, I, T*);

T apply1_T     (V, T);
T apply2_T     (V, T, T);

V getS(V, I, T*);
P applyS(V, I, P*);

V getS1(V, T);
V getS2(V, T, T);
P applyS1(V, P, P);
P applyS2(V, P, P);
