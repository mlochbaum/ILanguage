#include "x86_64.h"

typedef void* Asm;

/*
 * i o (input used; output given)
 *   + t: output type
 * +   n: number of inputs
 * + + i: input registers
 * + + o: output register
 * +   u: mask of unmodified registers (can include inputs)
 * + + l: length of assembly code
 *   + a: assembly code
 * 
 * The callee increments l and appends code to a.
 *
 * When used as an input, o can be NO_REG to indicate that the callee
 * can choose. i can be NO_REG to indicate that the callee can choose and
 * modify, or NO_REG_NM to indicate that the callee can choose but not
 * modify.
 */
typedef struct { T t; UC n; Reg* i; Reg o; RegM u; I l; Asm a; } AS;
typedef AS* A;

// Append code for f on the given types to a.
void apply_A(A a, V f, I n, T* x);
