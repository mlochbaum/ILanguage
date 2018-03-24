A list is simply a list of objects. Inhomogeneous lists are supported,
although there is implicit support for true homogeneous lists which store
header data only once and do not require as much type-checking.
All homogeneous lists are automatically represented in this way.
Homogeneous lists can still form ragged arrays.

Strings are (necessarily homogeneous) lists of characters.
Therefore all operations here apply to them.

The following functions do things with lists. Unless otherwise stated,
their domain is everything.

* `;` monad: Make into a one-element list.
* `;` dyad: Combine into a two-element list.

* `,` monad: flatten into a list with no sublists (unimplemented).
* `,` dyad: if both arguments are lists, concatenate them. If only the
  left argument is a list, append the right argument. Otherwise, same as
  `;`.

* `l.#` is the length of list `l`.
* `l#r` is a list consisting of `r` copies of `l`.

* `l}r` is the `r`'th element of `l`. If `r` is negative, then `l.#+r` is
  used instead.
* `l.g` is `l}0` and `l.G` is `l}_1`.
* `lgr` is the list consisting of the first `r` elements of list `l`, and
* `lGr` is the list containing the rest.

* `l.i` is the list of nonnegative integers smaller than `l`, in
  ascending order.
* `lir` is `r-l.i+l`, or the list of all `l+k` less than `r` where
  `k` is a nonnegative integer.

* `z` monad: Reverse the argument list.

* `r` stands for "reduce".

`l . f.r` is a "fold" of `f` across `l`, for example
```
1;2,3,4 . +.r  =  1+2+3+4
```
`l f.r r` is the same fold but starting with `r`, so that
```
1;2,3,4 +.r 7  =  7+1+2+3+4
```

* `l\r` is identical to `l  #.f o ,.r  r`.
* `l.\` is identical to `l  .  \ H i.#`.
  Thus `l\r` is the same as `l }H\ r`.
