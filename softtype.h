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
Ptr applyS(V, I, Ptr*);

V getS1(V, T);
V getS2(V, T, T);
Ptr applyS1(V, Ptr, Ptr);
Ptr applyS2(V, Ptr, Ptr);
