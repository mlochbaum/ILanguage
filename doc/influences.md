# I's influences

While the design of I may seem quite exotic at first, very few of the
decisions are without precedent. The sources I used in creating it are
recorded here, to the best of my memory.

### Functions and mapping

- Other languages consisting only of functions include lambda calculus and
  SKI combinator calculus.
- The concept of mapping automatically is documented in the 
  [paper](doc/BuiltInMapping/BuiltInMapping.pdf) on it. Briefly, it comes
  from extending Haskell's `fmap` to match J's rank and forks.
- It's also worth noting that addition (and multiplication) of functions
  `(f+g)(x) = f(x)+g(x)` is completely standard in mathematics, as is
  addition of vectors. Addition of sets is fairly common in group theory.
- The actual choice of available builtin functions derives mostly from J.
  Some, like backbind `B`, are obvious extensions from J, and a few, such
  as split-compose `O` and double-application `w`, were the result of
  working with I for a while and noticing things missing.

### Order of operations

- The use of whitespace for precedence comes from Dijkstra's
  [EWD1300](http://www.cs.utexas.edu/users/EWD/ewd13xx/EWD1300.PDF), which
  advises placing more space around operators with a lower precedence but
  not using whitespace as the sole indicator of precedence. Years later I
  found that Nim has an option to use whitespace for precedence. Dijkstra
  also uses an infix dot for function application, but I think I came up
  with that on my own (due to its similarity with the circle for
  composition) before reading EWD1300.
- I's left-to-right evaluation came after a great amount of deliberation
  on a number of inconsistencies in mathematical notation, in particular
  the fact that subtraction and matrix multiplication go in opposite
  directions. Initially I took this as evidence that subtraction went the
  wrong way, but later I changed my mind. A major influence on that
  decision were stack-based or concatenative languages. The most famous of
  these is Forth, but Factor was the one I actually used and False was
  also influential.
- Built-in precedence is lacking in APL-based languages as well as Lisp,
  but C's incredibly complicated precedence rules had just as large of
  role in pushing me away from such schemes.

### Cosmetics

- The idea of single-character builtin names is found in APL and K, among
  others. However, I's concept of escaping longer names comes from a very
  obscure language whose name I have forgotten. This language had a mode
  in which letters and numbers were strung together for computation, and
  an escape to get back into normal mode. I simply reversed the assumption
  of which mode was normal.
- Escaping quotes in strings by doubling them comes directly from J.
- Builtin names are variously the J names, possibly mirrored, reversions
  to C (`/`, `%`), and mnemonics or random letters chosen by me.

### Implementation

- The soft typing system was inspired by J's use of bitmasks to represent
  types. In J, an object can have only one type, but bitmasks make it
  easier to check if a type is equal to one of several types.
- JIT compilation is a well-known concept, and I don't remember when I
  first came in contact with it. I think I had decided that I would need
  to generate code by the time I came up with the soft-typing system.

### Other

- My desire for dynamically-typed data structures including lists and maps
  probably stems from early work with Python. Of course any number of
  other languages would have worked similarly.
- Errors-as-values comes from C.
