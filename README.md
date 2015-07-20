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

Build Instructions
------------------

On Unix, Linux, FreeBSD, OS X, and Windows:
```
    scons
```

This will create the main Lithium library (`lithium.lib` on Windows, 
`liblithium.a` elsewhere) and the Lithium Standard library (`lithium_std` on
Windows, `liblithium_std.a` elsewhere). All projects using Lithium will need
the Lithium library, and most will want the Lithium Standard library.
