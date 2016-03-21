I supports three types of numbers: integer, real, and complex.
These are converted implicitly so you don't have to worry about them.

Each of the following operations has only numbers in its domain.

* `+`, `-`, `*`, `/` dyad: work the same as in C, but with implicit
  conversions. At present `/` on integers is integer division, but this
  will be corrected.
* `%` dyad: modular division, defined for real numbers as the number
  between `0` and `r` (can be `0`, cannot be `r`) congruent to `l`, modulo
  `r`.
* `-`, `/` monad: negation and reciprocal.
* `q`, `Q` monad: square and square root (unimplemented).
* `m`, `M` dyad: minimum and maximum; monad: floor and ceiling.
