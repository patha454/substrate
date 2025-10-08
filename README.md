# Substrate

Substrate will be a free-standing systems library, intended for use developing operating system kernels, dynamic 
linkers, and embedded systems. It is also designed to be a supplementary utility library alongside a C Standard Library.


## Components

- `/memory/span.h` - Fat pointers and functions for copying or moving spans, and setting their contents.
- `/memory/allocator.h` Memory management interface.
- `/io/stream.h` - Byte-level I/O.
