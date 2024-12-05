# Substrate Library

Substrate aims to provide a freestanding, modern, systems utility library.

- _Freestanding_ means Substrate has no dependencies, not even a C standard library.
  
  Substrate will run on any platform which can be targeted by a compliant C compiler.

- _Modern_ means C23 compliant and idiomatic.

  Substrate is forward-looking, intended for new projects, and has no interest in supporting legacy code or tools.

- _Systems utility library_ means Substrate types and functions useful for writing other programs and libraries.

  Domain specific logic and business logic belongs in a higher-level library.

Substrate is intended for:

- Programs with are statically linked or unable to dynamically link to other libraries:
  
  Operating systems kernels, dynamic linkers, and bare-metal software are specific targets.

- Programs which require a limited, auditable set of dependencies.

  Substrate should support reliability-sensitive and security-sensitive programs.


## What belongs in Substrate

- Error handling: Most programs need reliable error handling.
- String processing: Often useful, but complex.
- Container types: No one wants to re-implement a linked list again.

## What does not belong in Substrate

- I/O: I/O is platform-dependent, and belongs in a higher-level library.
- Memory management: Memory allocation is platform-dependent. Substrate depends on the user to allocate and free memory.