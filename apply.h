// apply and related functions

// A mapping class, which describes how an argument may be mapped over.
// The pointer v is null for most classes.
// Mapclasses with unequal type or incompatible pointer (based on class)
// are incompatible.
typedef struct {T t; P v;} X;
// Constant types: mapping gives an error
#define CONST_X  1 //ESZRC
// Function types: mapping is composition
#define FUNC_X   2 //BOFNQ
// List types: mapping is list map
#define LIST_X   4 //L

// Apply f to the n arguments x, consuming them. Do not consume f.
V apply    (V f, I n, V* x);
// Return an integer d so that (d>>i&1) gives whether x[i] is in the
// domain of f. If some arguments are not in the domain, f will map.
I dom      (V f, I n, V* x);
// Get the mapclass of the argument.
X mapclass (V);
// Map f over arguments x, where d is the domain returned by dom.
// This consumes each x[i]. Do not delete f.
V fmap     (V f, I n, V* x, I d);

// None of the _T functions change memory properties.
// Analogous to apply. Return the possible output types given input types.
T apply_T    (V, I, T*);
// Analogous to dom. Return minimum (doml) or maximum (domu) possible
// domain given types.
I doml_T     (V, I, T*);
I domu_T     (V, I, T*);
// Return possible mapclass types given type.
T mapclass_T (T);
// Analogous to fmap, but takes both a lower and upper domain.
// Currently just calls fmap_TT.
T fmap_T     (V, I, T*, I, I);
// Like fmap_TT, but takes only the type of the function.
T fmap_TT    (T, I, T*, I, I);

// Return a version of the function specialized for input type
// The output always has a pure type.
// Does not consume the argument.
V apply_S(V, I, T*);

// Apply a specialized function to arguments. The type of f must be pure.
// v must have the type returned by apply_T, and an already-allocated
// pointer. Consumes the arguments.
void apply_P (V v, V f, I, V*);
// Similar, with fmap. The additional argument is the domain.
void fmap_P  (V v, V f, I, V*, I);

// apply.c
// Shortcuts
V apply1(V, V);    // apply with one argument.
V apply2(V, V, V); // apply with two arguments.
// Forms of apply1 and apply2 with different memory effects
V apply1d(V, V);     // Consume function.
V apply2d(V, V, V);  // Consume function.
V apply1_d(V, V);    // Free argument pointers (delete, not consume, args).
V apply2_d(V, V, V); // Free argument pointers.
V apply1dd(V, V);    // Delete both function and arguments.
V apply2dd(V, V, V); // Delete both function and arguments.

T apply1_T(V, T);    // apply_T with one argument.
T apply2_T(V, T, T); // apply_T with two arguments.
void apply1_P(V, V, V);    // apply_P with one argument.
void apply2_P(V, V, V, V); // apply_P with two arguments.

// mapclass.c
I mapclasseq(X, X);  // Test mapclasses for equality

// dom.c
I dom_true(I);  // Domain returning true for all arguments

// compose.c
I toBool(V);   // Consume argument and return a boolean for it.
I toBoold(V);  // Delete argument and return boolean.
