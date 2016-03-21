In addition to the implicit mapping provided by I (which should always be
your first plan of attack), there are several ways to specify your own
mapping.

* `D` "domain"
  uses `r` (which should return a boolean) to specify the domain for `l`.
  If two arguments are given, `r` should return a list of two booleans.
* `f` "fmap"
  * `lfr`      maps `r` over `l` using the appropriate mapping.
  * `l . r.f`  does the same.
  * `l g.f r`  maps `g` over `l` and `r` using `l`'s mapping type.
* `t` "type"
  * `l.t` is the integer giving `l`'s type. Type numbers are subject to
    change, so you whould compare to the type of a constant with known
    type rather than hardcode the number.
