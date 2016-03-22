Suppose we wish to find the sum of all natural numbers below 100 that are
multiples of either 3 or 5. We'll ignore any arithmetic simplifications
here: we actually want to generate all these numbers and add them up.

An idiomatic approach in I would be to start with the natural numbers
less than 100 (`100.i`), then find which numbers satisfy the given
criterion and add those up. Rather than filter and add, we can just take
a dot product with a list of booleans telling which numbers fit.

The dot product is simple, and is a phrase worth memorizing:
```
1,2,3 +.r~* 2,4,1
13
```

We'll demonstrate some intermediate steps with `10.i` to avoid the large
printout. To find which numbers are divisible by 3 we use `%=0`:
```
10.i %=0 3
 1;0,0,1,0,0,1,0,0,1 
```

Then to find which numbers are divisible by either 3 or 5 we or (identical
to the maximum `M`) two divisibility functions together. Recall that `b`
binds its right argument as the right argument of the function on the
left, enabling us here to apply two different functions to `10.i` with a
fork.
```
10.i  .  %=0b3 M %=0b5
 1;0,0,1,0,1,1,0,0,1 
```
It would have worked equally well to use `[%3=0` in place of `%=0b3`, but
the first expression has the 3 tightly bound inside, making it harder to
manipulate. From the form above we can simplify to
```
10.i  .  3 %=0BbOM 5
 1;0,0,1,0,1,1,0,0,1 
```
which factors out the operation `%=0` in order to use it only once. It's
perhaps worth noting a possible detour which is less elegant, but, well...
```
10.i  .  %b3 =b0OM %b5
 1;0,0,1,0,1,1,0,0,1 
```
"`b0OM`"!

Once we have the divisibility test and the dot product, we just hook them
together:
```
100.i  .  3 %=0BbOM 5 h +.r~*
2318
```
