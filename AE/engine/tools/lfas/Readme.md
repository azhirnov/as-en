## Lock-Free Algorithms Sandbox (LFAS)

All lock-free algorithms which are used in the engine should be tested here.


## Features

A virtual machine that emulates a memory model as described in C++ standard ([memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)).

Detects:
* missed cache flushes.
* missed cache invalidations.
* data races.
