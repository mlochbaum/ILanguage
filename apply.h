typedef struct {T t; P v;} X;
#define CONST_X  1 //ESZRC
#define FUNC_X   2 //BOFNQ
#define LIST_X   4 //LA

V apply    (V, I, V*);
I dom      (V, I, V*);
X mapclass (V);
V fmap     (V, I, V*, I);

T apply_T    (V, I, T*);
I doml_T     (V, I, T*);
I domu_T     (V, I, T*);
T mapclass_T (T);
T fmap_T     (V, I, T*, I, I);
T fmap_TT    (T, I, T*, I, I);

void apply_P (V, V, I, V*);
void fmap_P  (V, V, I, V*, I);

// Return a version of the function specialized for input type
V apply_S(V, I, T*);

// apply.c
#define DECLARE_APPLY(t) void apply_P_##t(V, t, I, V*); \
                         T apply_T_##t(t, I, T*);
ON_TYPES(NCONST,DECLARE_APPLY)
#undef DECLARE_APPLY
void apply_P_FB(V, F, I, V*);
//void apply_P_FQ(V, F, I, V*);
T apply_T_FB(F, I, T*);
//T apply_T_FQ(F, I, T*);

V apply1(V, V);
V apply2(V, V, V);
V apply1d(V, V);
V apply2d(V, V, V);
V apply1_d(V, V);
V apply2_d(V, V, V);
V apply1dd(V, V);
V apply2dd(V, V, V);

T apply1_T(V, T);
T apply2_T(V, T, T);
void apply1_P(V, V, V);
void apply2_P(V, V, V, V);
void apply1d_P(V, V, V);
void apply2d_P(V, V, V, V);

// mapclass.c
I mapclasseq(X, X);

// dom.c
#define DECLARE_DOM(T) I dom_##T(T, I, V*);
DECLARE_DOM(B) DECLARE_DOM(F) DECLARE_DOM(N) DECLARE_DOM(Q)
#undef DECLARE_DOM

I dom_true(I);

// fmap.c
void fmap_LIST_P(V, V, I, V*, I, I);

// compose.c
I toBoold(V);
I toBool(V);
