Lithium ICL Interpreter
=======================

Copyright (C) 2015 Martin McDonough
Version 0.01

What is Lithium?
----------------

Lithium is an interpreter for a scripting language called ICL. It has a static
type system with weak typing, stateful object-oriented features, and a module
system.

ICL is useful in cases where a more bare-metal environment is desired than most
other scripting languages. It is very simple, but still allows prototyping
equations, logic, and behaviour easily. Lithium provides an easy to use and
powerful C++ API for extending and embedding ICL.

Lithium's Language, ICL
-----------------------

ICL is statically typed. There are four fundamental types:

 * Integers
 * Floating Point Numbers
 * Strings
 * Booleans

ICL is weakly typed. Types are coerced based on the higher associativity term
in any statement.

All ICL is executed in the context of an object. Other objects are accessed
through the module interface.

Variables and properties are distinct. A variable is associated with a scope,
while Properties are associated with the executing object. Using the module
interface, other object's properties can be retrieved or set.

Examples creating some variables:
```
int x 10
float y 100.0 - 27.386
string name "This is a name."
```

Note that there is no dedicated assignment operator for declaring variables,
and rather all declarations include an initial value. This also means that '='
can be used for mathematical equality in expressions rather than assignment.

Both properties and variables are accessed through the `get` and `set` 
keywords. Variables additionally require the `local` keyword to distinguish
them from properties.

Accessing some properties:
```
set X get X + 10
set Squared get Number * get Number
```

Accessing a variable is much the same, but with the added `local` keyword:
```
int two 2

int three 0
set local three get local two + 1

int four 2 + get local two
```

Scopes are declard starting with ':' and ending with '.'. Any variables
declared inside a scope are destroyed at the end of the scope. The scope
operators are used for loops and conditionals.

A loop is specified as so:
```
loop <condition>:
    <body>
.
```

An example of a loop:
```
int fib1 0
int fib2 1
int x 0
loop x<10:
    int temp get local fib2
    set local fib2 get local fib1 + get local fib2
    set local fib1 get local temp
    set local x get local x + 1
.
```

Conditionals are declared similarly:
```
string parity "even"
int number 17
if number%2 = 1:
    set local parity "odd"
.
```

Build Instructions
------------------

On Unix, Linux, FreeBSD, OS X, and Windows:
`scons`

This will create the main Lithium library (`lithium.lib` on Windows, 
`liblithium.a` elsewhere) and the Lithium Standard library (`lithium_std` on
Windows, `liblithium_std.a` elsewhere). All projects using Lithium will need
the Lithium library, and most will want the Lithium Standard library.
