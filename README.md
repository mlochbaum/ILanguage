## Version 0.0

This branch contains I, version 0.0. It does not have the speed of the
JIT-compiled features from version 0.1, but is less architecture-dependent
and relatively stable, with very few memory bugs.

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

## Building I

To build, run `gcc *.c -o I`. Then `./I` is a command-line interpreter.

## Resources

See [doc/introduction.md](doc/introduction.md) to begin learning about I.

The flash of insight that started I, built-in mapping, is documented at
[doc/BuiltInMapping/BuiltInMapping.pdf](doc/BuiltInMapping/BuiltInMapping.pdf).
It is presented in a manner that should be accessible to programmers
without any specific language knowledge aside from functional programming.
