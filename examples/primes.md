Suppose we want to do things with prime numbers. We'll start by
generating them efficiently, using a prime sieve. Our sieve will keep a
list of booleans which is zero for each prime we've eliminated, as well
as the largest prime we have found so far. On each step we find the next
prime, then eliminate all of its multiples. So after finding the second
prime, 3, we will have something like:
```
  0;0,1,1,0,1,0,1,0,0,0... ;3
```
In fact, the boolean array for this sieve is already perfectly accurate
for the ten numbers shown. Every number divisible by a prime we have found
is ruled out, so the only composite numbers not ruled out are those whose
smallest divisor is larger than any we have found. The smallest such
number possible is the square of the next prime--here that prime is 5 and
its square is 25. To see a case where the sieve is wrong we have to go
back to the first prime, where we can see that 9 is not yet ruled out as
a prime.
```
  0;0,1,1,0,1,0,1,0,1,0... ;2
```
This square-of-the-next-prime rule has two important consequences. First,
we can begin eliminating at the square of the next prime. Second, to build
a sieve of length `n` which is completely accurate, we only need to
perform `n.Q`, the square root of `n`, steps of the sieving operation.

First of the two parts of a step is to find the next prime. This means we
must search the list on the left for a 1, starting at the element after
the index on the right. Suppose we have a list like
```
'l' : 0,0,1,0,0,0,0,1,0
```
and want to find the first 1 after index 2. We could write
```
2+1  .  [+1 w 'l'..B}=0
7
```
but we want to be able to pass `'l'` in as an argument. Actually, we'd
prefer to pass `'l'` as the left argument and 2 as the right, so that we
can call our function on the current sieve, prime pair with `r`. We can
produce the required function from `'l'` with
```
'l'  .  [+1.k w.o Bb}=0
  [+1 w  'l'B} =0
```
or perhaps
```
'l'  .  Bb}=0 o [+1Bw
  [+1 w  'l'B} =0
```
Then we want to call that on the left argument and `]+1` on the right, and
call the resulting function on the number. This is possible with `~.O`:
```
'l'.. ([+1.k w.o Bb}=0 ~.O ]+1) 2
7
'l'..;2 . ([+1.k w.o Bb}=0 ~.Oor ]+1)
7
```
We can shorten the find function a little by combining `[+1` with `w` and
moving to a hook. We name it `'f'`.
```
'f':(Bb}=0 [+1Bwh~.Oor ]+1)
'l'..;2 . 'f'
7
```

Now to update the sieve with our new prime. We want to take a list on the
left and the new prime on the right, and mark out multiples of the new
prime starting at its square and ending at the end of the list. Dyad `i`
is perfect to generate such a list:
```
46 ]i/*] 5
 25;30,35,40 
```
Well, not quite perfect: since `i` stops before its right argument, we
want to add one to get an extra element. And we should make sure that
no indices are produced if the square of the right argument is after the
end of the list by reducing the starting index, if necessary (otherwise,
`i` will start at the higher starting element and go down).
```
24,25,45  /om+1(mi[*])] . f  5
 N; 25.; , 25;30,35,40,45
```
We want to place a 0 at each of these indices, so we append a 0 to each
element and pass to `{.r`:
```
'u':(#h/om+1(mi[*])] o ,.fb0 {.r.o [)
0;0,1,1,1,1,1,1,1 'u' 2
 0;0,1,1,0,1,0,1,0 
```

Combining `'f'` and `'u'` is fairly straightforward:
```
0;0,1,1,0,1,0,1,0,1,0 ; 2  .  gh'u';] H 'f'
  0;0,1,1,0,1,0,1,0,0,0 ;3 
20 (i>1;0  ..O  gh'u';] H 'f' B p) 3
  0;0,1,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1 ;5 
```
However, for large sieves we will run into a performance problem. The
array `gh'u';]` is executed in order, so that the left half is not allowed
to destroy its arguments but the right half is. The left argument is our
current sieve, and is modified by `'u'`. But if `'u'` can't modify its
argument in place it will have to make a copy every time! This is no good.
To get the full power of I's smart reference counting we need to reverse
the order of execution. Note that simply swapping and using `;.~` won't
work as the same list is constructed. Instead we need the more unwieldy
`;.~.o.~`. Sigh. Hopefully in the future I will be smarter about the order
of execution. `;oz.~` would also work.
```
20 (i>1;0  ..O  gh'u'(;.~.o.~)] H 'f' B p) 3
  0;0,1,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1 ;5 
200000 (i>1;0  ..O  gh'u'(;.~.o.~)] H 'f' B p.G) 10001
104743
```
That's the 10,000th prime above, since iteration 1 produces the zeroth
prime, 2.

To find all of the primes below a given number, we need to run the sieve
operation a number of times equal to the square root of that number.
Here's how:
```
'sieve' : (i>1;0  ..o  gh'u'(;.~.o.~)] H 'f' p.g Qom)
```
The inner portion we've already discussed, but there are some interesting
ideas in the outer part. Replacing the inner, iterated function with
`(..)`, we have `(i>1;0  ..o  (..) p.g Qom)`. The `..o` fork means that
each tine is evaluated on the argument and then the right is executed on
the left. The left tine, `i>1;0`, is straightforward: it just produces
the initial, trivial sieve. The right, `(..) p.g Qom`, will apply `Qom`
to the argument and then form the function which executes our inner part
that many times. The reason it does this without us having to say it
explicitly is that `p` has a domain function which always accepts the left
argument, but rejects the right until it is a number. Thus `p` maps over
the right argument but not the left. Finally, the `.g` maps over `p`,
and anything that isn't a list. It could be placed pretty much anywhere,
with the result that we take the first half of the sieve after our
computation.

In action:
```
100 . 'sieve' . \
 2;3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97
200000 . 'sieve' . \}10000
104743
```

To summarize, we can (efficiently!) get a mask of all the primes up to `n`
with `n.'sieve'` using the following definitions:
```
'f':(Bb}=0 [+1Bwh~.Oor ]+1)
'u':(#h/om+1(mi[*])] o ,.fb0 {.r.o [)
'sieve' : (i>1;0  ..o  gh'u'(;.~.o.~)] H 'f' p.g Qom)
```
