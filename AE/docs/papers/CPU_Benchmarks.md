

# Copy Memory

[source code](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_MemCopy.cpp)<br/>
[results](bench-cpu/MemAccess.md)

Notes:
* `std::memcpy` is fast:
	- used non-aligned SIMD load for head.
	- used SIMD with 4x unrolling for blocks with 4x SIMD word size.
	- used SIMD without unrolling for tails with 1x SIMD word size.
	- used non-SIMD for small tails.
	- used non-temporary store for large blocks (>= 2MB).
* SIMD cached copy is fast until block size is less than 1/2 L3 bank size (depends on associativity?).
	- for larger sizes bandwidth decreased to 1/2 of RAM bandwidth, which caused by L3 to RAM writes.
* SIMD non-cached (non-temporary) copy is fast when block size is greater than 1/2 L3 bank.
	- performance is limited by RAM bandwidth.
	- Simultaneous multithreading (SMT) required to hide memory latency, when second thread is ALU bound.
* 4x unrolling is much faster (+40%).
	- 8x unrolling has no effect.
	- instruction latency is 4 cycles, so 4x unrolling can hide it.
* '4x read, 2x write' test shows that copy operation doesn't utilize full bandwidth of read operations, they limited by store op with is 2x slower.


# Find Char

[source code](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_FindSubString.cpp)<br/>
[results](bench-cpu/SIMD_FindChar.md)

Notes:
* CPU can execute 1-2 SIMD instructions per cycle.
* `[[likely]]` attributes in C++:
	- compiler place hot/likely path as linear code.
	- cold/unlikely path is placed under the hot path.
	- compiler may ignore `[[unlikely]]` if code in cold path is too small, but it cause performance penalty on high-loaded code.
* Same as [Copy Memory](#Copy-Memory):
	- 4x unrolling is fast.


# Floating Point SIMD Performance

[source code](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_SIMD.cpp)<br/>
[results](bench-cpu/SIMD_FLOPS.md)

CPU contains multiple FMA pipes, each can execute 1 FMA op per cycle (2FLOPS/cy).
	- Most common case when CPU have 2x 256bit FMA pipes *(Intel P-cores or before hybrid arch, AMDs before Zen4)*.
	- Intel Arrow Lake E-core have 4x 128bit FMA pipes which can emulate 256bit SIMD ops.
	- AMD Zen4 supports 512bit SIMD, but natively have 256bit FMA pipe, so it use dual issue to emulate 512bit.

Notes:
* SMT on AMD allows to utilize 2 FMA pipes (1 pipe per logical core).
* Without SMT you need to use ILP=8 mode to utilize 2 FMA pipes near to 100%.


# Scalar operation between SIMD

[source code line:107](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_SIMD3.cpp)

Scalar instruction between SIMD may cause 5x performance lost if have memory dependency with SIMD result.


# Vertical SIMD

[source code](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_VertSIMD.cpp)<br/>
[results](bench-cpu/VerticalSIMD.md)

It is GPU-style programming on SIMD.
* Modern scalar GPUs execute 1 scalar op per cycle per thread in warp.
* Warp is equal to SIMD unit on CPU, so each scalar in shader equals to single lane in SIMD register.
* Warp takes 4 cycles to execute ops on `float4` type, but this op has ILP=4 mode which hide 4 cycle latency for FMA op.
* GPUs can hide instruction latency by executing multiple warps, depends on register count it could be 8-20 warps per SM/CU.

Old layout:
```
Position (x, y, z, unused)
Velocity (x, y, z, unused)
```

New layout:
```
Positions
[xxxx xxxx] // float8
[yyyy yyyy]
[zzzz zzzz]

Velocities
[xxxx xxxx] // float8
[yyyy yyyy]
[zzzz zzzz]
```

Notes:
* This method allows to use AVX512 for small types like `float2`, `float3` with 100% utilization.
* `float3` type on SSE lose 1/4 of SIMD lanes when not use old style `float4` ops.
* This layout it better for Data Oriented Design (DOD).

References:
* GDC2015: SIMD at Insomniac Games ([video](https://gdcvault.com/play/1022248/SIMD-at-Insomniac-Games-How), [pdf](https://gdcvault.com/play/1022249/SIMD-at-Insomniac-Games-How), [pdf with notes](https://gdcvault.com/play/1022492/SIMD-at-Insomniac-Games-How))
* [The Implementation of Frustum Culling in Stingray](https://bitsquid.blogspot.com/2016/10/the-implementation-of-frustum-culling.html)

