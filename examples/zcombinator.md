# The Z combinator

This example is not intended for use in learning I. It does, however,
serve as an excellent and very demanding test of one's understanding of
I's style of point-free programming. If you understand the implementation
of the Z combinator at an intuitive level, you have mastered the art. You
would probably be the first, as I have yet to feel that I know what I am
doing when working on it.

Below is a definition of the Z combinator, along with an invocation of it
which gives the factorial function (which is then applied to 7). Read on
for explanations, if you dare.
```
'Z'  :  .H(..~)BbBo o ..~
-b1h.*[ pow [>1 B b  .  'Z'  ~  7
```

### Fixed-point combinators
The Z combinator, like its better-known cousin the Y combinator, is a
fixed-point combinator---it finds, for a given input function, an argument
on which that function has no effect. In a formula, if Y is a fixed-point
combinator and f a function, then `f.Y.f = f.Y`. One example of such a
combinator would use this directly as a definition:
```
'Y'  :  'Y' ..o [
```
gives the required functionality. But the Y and Z combinator achieve this
without explicit recursion.

### The Y combinator in I
We will begin with the Y combinator, which is simpler:
```
'Y'  :  ..~Bo o ..~
```
or, for the overly clever,
```
'Y'  :  ..~ . Bboho
```
Applying `'Y'` to `'f'` then gives
```
'f'.'Y'
'f'  .  ..~Bo o ..~
..~o'f' . ..~
..~o'f' . ..~o'f'
(..~o'f' . ..~).'f'
('f'.'Y').'f'
```
So the Y combinator fits the requirement for a fixed-point combinator.
Unfortunately, the Y combinator is only intended for use in lazy
languages, which do not evaluate functions until they are asked for a
result. When called on any argument (even a constant!) in I, it will
immediately overflow the stack, which currently causes a segfault.

### The Z combinator
The Z combinator fixes this issue by explicitly waiting for an argument.
This is a very I-like approach, where functions are manipulated endlessly
and finally passed an argument, causing them to execute. The rule for the
Z combinator is a corollary of the definition of a fixed-point function:
we set, for function `f` and argument `v`, `f.Z~x = f.Z.f~x`.
```
'Z'  :  .H(..~)BbBo o ..~
```
Then we have
```
'f'.'Z'
'f'  .  .H(..~)BbBo o ..~
'f' . .H(..~)BbBo . ..~
.H(..~)Bb o 'f' . ..~
.H(..~)Bb o 'f'  .  .H(..~)Bb o 'f'
.H(..~)Bb o 'f' . .H(..~)Bb . 'f'
.H(..~) b (.H(..~)Bb o 'f') . 'f'
'fZ'.'f'
```
For 'fZ' defined to be the argument to 'f' in the previous line. To show
that `'f'.'Z'~'x'` is equal to `'f'.'Z'.'f'~'x'`, we can demonstrate that
`'fZ'` is the same function as `'f'.'Z'`. We do this by applying 'fZ' to
an argument:
```
'x'  .  'fZ'
'x'  .  .H(..~) b (.H(..~)Bb o 'f')
'x'  .  .H(..~)Bb o 'f' . ..~
'x'  .  'f'.'Z'
```

### The factorial function
Here's an example of a function which has a meaningful fixed point.
```
'F'  :  -b1h.*[ pow [>1 B b
```
From the fixed-point formula for some fixed-point `'v'`, we have
```
'v'  =  'v'.'F'
'v'  =  -b1h.*[ pow [>1 b 'v'
'v'  =  -b1o'v'*[ pow [>1
```
So `'v'` is just a recursive factorial function! It was implemented
without any explicit looping or recursion. The `pow` in `'F'` can only
have right argument `0` or `1`, so it functions as an if clause. It could
be implemented with `}ow` instead.

Similarly, we can implement any recursive function of one argument by
making a function that takes the argument on the left, and "itself" on
the right, then applying curry. When passed to the Z combinator, we get
the function we wanted to define recursively.
```
'recurse' : Bbbo'Z'
```
