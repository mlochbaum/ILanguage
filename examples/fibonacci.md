How might we find the n'th Fibonacci number in I? The first Fibonacci
number less than or equal to 2000?

First up, the first two Fibonacci numbers:
```
2.i
 0;1
```
Not too hard.

To find the second Fibonacci number from the zeroth and first we just add:
```
2.i . +.r
1
```
But to keep going we need to maintain the last number of the ones we had
before:
```
2.i . G;(+.r)
 1;1
```
So `G;(+.r)` maintains the form of its argument. We can apply it multiple
times with power (`p`):
```
2.i . G;(+.r)p5
 5;8
```
These are the fifth and sixth Fibonacci numbers. If we want, say, just the
twelfth, we can do this with 12 and then take the first number in the
list:
```
2.i . G;(+.r)p12og
144
```
Things get a bit harder if we want to make this into a function of the
index 12, rather than the initial list `2.i` (and stay point-free).
We first construct the inner part using bind, and then make a verb which
will apply it to `2.i`. Finally we take the first element:
```
12 . G;(+.r)Bp
  G; +.r  p12
12  .  2.i ..o G;(+.r)Bp
 144;233
12  .  2.i ..o G;(+.r)Bp o g
144
```

What if we want to get the list of all Fibonacci numbers up to a certain
number? Rather than dropping previous elements, we should append to a
growing list. To find the next element, we take the last two and sum:
```
0,1,1,2,3  .  gb_2 o +.r
5
```
And to append it, we use `,`:
```
0,1,1,2,3  .  gb_2 o +.r H.~ ,
 0;1,1,2,3,5
```
Because of the way I uses reference-counting, the append will be done in
place, taking constant amortized time. To get all the numbers up to the
value we want, we can use while:
```
2.i  .  gb_2 o +.r H.~ , w G<2000
 0;1,1,2,3,5,8,13,21,34,55,89,144,233,377,610,987,1597,2584
```
