# The I programming language

I is a language which generalizes the array and functional capabilities of
the array-programming language J to deal with a much broader variety of
data structures.

Some features and design goals of I include:

* Automatic mapping. Like J's array operations, I will map over lists,
  sets, associated arrays, and functions.
* Soft typing built into the core of the language.
* Automatic optimization when types are known, using JIT compilation.
* Functional programming. Everything in I is a function.
* Near-nonexistent syntax. Everything is function application.
* Whitespace defines the order of precedence in a visually intuitive way.

## Portability

The master branch contains version 0.1, which uses JIT compilation to do
some operations very quickly. The compilation currently targets only the
x86-64 architecture. Nearly all 64-bit desktop or laptop processors should
use this architecture, but if your machine does not, you may want to use
version 0.0, branch `v0.0`. All versions of I have only been tested on
Linux and would probably require a small amount of modification to work
with other operating systems.

## Building I

To build, run `gcc *.c -o I`. Then `./I` is a command-line interpreter.

## Resources

See [doc/introduction.md](doc/introduction.md) to begin learning about I.

The flash of insight that started I, built-in mapping, is documented at
[doc/BuiltInMapping/BuiltInMapping.pdf](doc/BuiltInMapping/BuiltInMapping.pdf).
It is presented in a manner that should be accessible to programmers
without any specific language knowledge aside from functional programming.
