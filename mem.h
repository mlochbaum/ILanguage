#define MALLOC(l) malloc(l)
#define CALLOC(l, n) calloc(l, n)
#define FREE(ptr) free(ptr)
#define DECL(t, v) t v = MALLOC(sizeof(*v))
#define DECL_ARR(t, v, l) t *v = MALLOC(sizeof(v)*(l))
#define DECL_STR(v, l) Str v = MALLOC(sizeof(v)*((l)+1))
#define DECL_NSTR(v, l) DECL_STR(v, l); v[l]='\0'

V makeV(T, Ptr);

void del(V); // Delete this copy of v.
V cpy(V);    // Return a new copy of v.
V get(V);    // Ensure that the returned value is safely modifiable.

void valcpy(T, Ptr, Ptr);
Ptr cpyval(T, Ptr);
