
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
	

# Find Char

[source code](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_FindSubString.cpp)<br/>
[results](bench-cpu/SIMD_FindChar.md)

Notes:
* `[[likely]]` attributes in C++:
	- compiler place hot/likely path as linear code.
	- cold/unlikely path is placed under the hot path.
	- compiler may ignore `[[unlikely]]` if code in cold path is too small, but it cause performance penalty on high-loaded code.
* Same as [Copy Memory](#Copy-Memory):
	- 4x unrolling is fast.

