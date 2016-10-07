# Introduction to I
Welcome to I! I is a language in the spirit of J and Lisp, which
generalizes the array and functional capabilities of J to deal with a much
broader variety of data structures.

After building I, you will have a simple (VERY simple) command-line
interpreter, which accepts one-line-long commands.
Commands and their ouputs will be printed here as you would see them on
the command line---input line, then output line.

Every entity in I is an operation (or function).
Syntactically, every I operation is an infix function, which means that
every one takes exactly two arguments, one on the left and one on the
right.
For operations like the standard four of arithmetic, this looks just like
normal math.
```
3*4 - 6/3
10.000000
```

Numbers are also operators. They return themselves:
```
3 4 5
4
```
Only having infix functions would seem to prevent prefix operations like
`-` meaning negation.
However, there is an I operator `.` which applies the right argument to
the left argument, so we have
```
3.-
_3
```
That `_3` is a shorthand for negative three, which isn't constrained by
our operator notation. It's useful.
Note that we just took `-` as an argument to another operator.
That's completely ordinary, because everything is an operator.
It's not any different from writing `3+4`, or `3 . 4`, which yields `4`
because `4` is a constant operator.
Look out: this is opposite from the order of evaluation in most languages!
Everthing goes left to right in I. Like telling a story.
It's fun! Well, it will be fun after you learn it. It will be absolute
hell for a little while at the beginning.
_NOTE: Sometimes functions of one and two arguments will be called "monads"
and "dyads," respectively, to save time._

## Order of operations
Evaluation in I proceeds from left to right.
Parentheses may be used to specify the order, but there's a (usually)
better way as well: whitespace! Fewer spaces means the function will be
executed earlier.
For example, the expression
```
b.-   +,-   b.q - 4*a*c . Q   /   2*a
```
parenthesizes as
```
((b.-)(+,-)(((b.q)-((4*a)*c)).Q))/(2*a)
```
I tend to think the first is _just a little_ nicer to look at.
Note that I used three spaces around `+,-` and `/` where two would have
sufficed.
Excessive space around operations that are next to long bits of code is
recommended for sanity; it's also extremely good practice to place the
same amount of space on both sides of an operator.
This is because EVERY token can be an operator. If you don't follow the
rule, you will quickly confuse which tokens are operators and which are
operands.
As an example of things you don't want to do, look at this innocuous
little bit of code:
```
27 / 1+2  -1 /2
1
```
Um. If you're willing to do the math, you'll notice that this should be
`(9-1)/2 = 4`. What happened?
```
(27 / 1+2)-1/2
4.000000
(27 / 1+2)(-1/)2
1
```
In the first version, parenthesizing lets us place the two simple
operations we want to do right next to the complicated expression. This is
the right way to do it.
The second is what actually happened: `(-1/)` was grouped together
(because it was the leftmost three of the four operations at the right),
and the `1` (a constant operation, remember) ate everything.
The precedence of an operator is actually the maximum of the number of
spaces on either side. This rule is pretty good, but not perfect; keep the
same number of spaces on either side of operators if you don't want to run
into the corners.
This rule may be made mandatory in the future.

## Tokenization
Tokens may be of five types:
* One alphabetic or ASCII special character. This is a built-in operator.
* A number, which starts with a numeric, contains numerics, and may contain
    a decimal point with numerics immediately surrounding (no whitespace).
    Numbers can also start with `_`, which indicates negation.
* A name, which is set off by single quotes and can contain any sequence
    of characters which are not single quotes.
* A string, set off by double quotes. Additional double quotes are escaped
    by doubling, as in `"The character "" is a double quote."`.
* A quasiquote, set off by backquotes and currently undefined.

Numbers and strings are constants; when applied as operators they'll
return themselves. Names can represent anything, as can quasiquotes,
although neither of these are implemented yet. Soon... very soon...
Builtins are the soul of I. They provide all the functionality you need
to build functions. No lambdas... no "define" constructs... just builtins.

## Operations
The available operations are detailed in the other files in this folder.
Aside from the information here, the ONLY things you need to learn about
I are which functions are built in and which data structures are defined.
After you finish this document, take a look around!

## Booleans
Things which aren't zero are true, and things which are zero are false.
There are no dedicated boolean types. `0` and `1` will be treated as the
canonical booleans, to be returned by functions which purport to return
booleans.

## Okay, how do i use this stuff?
It's, uh, different. Composing an operation basically involves taking
other operations as steps and combining them appropriately.
Knowing the composition operations is crucial; they are listed in
compose.txt.
For example, function composition is `o`. The symbol is meant to evoke
mathematical composition, but the operation goes in the opposite direction
and is better translated as "then."
As an example, look at
```
4 *o- 5
_20
```
The operation here is `*o-`, "Multiply then negate."
The right operand of `o` is always invoked with only one argument, while
the left can take one or two
(try `6.0 . /o-`, where `/` is the reciprocal, for a one-argument
example).
Another composition is `O`, which makes sure the _left_ operand is applied
as a single-argument function by applying it to each operand.
For example,
```
2 /O+ 0.4
3.000000
2./ + 0.4./
3.000000
```
The reason we use compositions like this is to compress a function that
originally had the arguments sprinkled all over the place to one that has
arguments only on the left and possibly right. After that, we can either
assign them or pass them as arguments to other functions. For an example
of this functionality, we introduce `b`, or "bind," which fixes its right
argument as an operand of its left, to be applied to one more argument
as the right argument:
```
2 . *b3
6
```
and `w`, or while, which executes the left argument while the right one is
true of its operands (so `a . fwg` applies `f` to `a` until `g` returns
false).

Let's find the first power of two after 1000:
```
1  .  *b2 w <b1000
1024
```
I would read that as "Take one, and multiply by two while it's less than
one thousand." Solves the problem.
Expect more guided examples in the future. But not in the present.
Because I haven't written them yet.

## Mapping
The one-argument form of `o` does something similar to the two-argument
form. I think an example is best:
```
5  - *.o +  4
9
5-4 * 5+4
9
```
Apply both the left and the right functions, and apply the middle one to
the two results. For those interested, difference of squares tells us
that's equal to `5 qO- 4` , or
"five and four---square 'em both, and subtract."
But what if, magically, we didn't have to write the `o`? If I knew what we
wanted to do and we could just say "the difference times the sum?"
Go ahead; try it.
```
5 -*+ 4
9
```
Worked. How?

Every operation has a _domain_ to which it can reasonably apply
(for example, arithmetic functions can only apply to numbers).
If an operation is applied to something outside of its domain, it attempts
to "map" over that argument, using a method that is based on the argument's
type.
Mapping over a function is composition, like we just saw.
For one argument, that changes things like `*.-` into `*o-`.
Essentially, we are applying `-` not to `*` , but to the _result_ of `*`.
I find this to be a natural way of thinking, and in fact it's applied
near-universally in mathematics (think f+g) despite never being
acknowledged meaningfully.
The mapping operation over a collection is to apply the function to
each element of the collection and give the collection of results. Like
```
1,2,3 * 2,3,4
 2;6,12 
```
Once sets are implemented, you'll also be able to take a set like `{_2,2}`
(not the actual notation) and square it, obtaining `{4}`.
For a more in-depth treatment of the subject, see
[the informal paper I wrote on it](https://docs.google.com/open?id=0B9JcSIlTRG6FZjA1NjE4ZTgtYzhiYy00YzA0LTkxODUtZDUwODg5OGEwYmE5).

Data structures and their mapping methods are described in the relevant
documentation files here.
The domain of a function is always given with its definition; if unstated,
it defaults to true.
