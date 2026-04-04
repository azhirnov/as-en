Performance per thread.

[Source](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_FindSubString.cpp)


# AMD Ryzen 3900X, 4.2 GHz, 7nm (Zen2)

* Cache:
	- L1D: 12x 32KB 8-way
	- L2: 12x 512KB 8-way
	- L3: 64MB: 4x 16MB 16-way
* Memory: DDR4-3200, DC, 24GB/s

Theoretical performance:<br/>
	1 SIMD per cycle, 32 chars per SIMD
	32 IOPS/cy * 4.2 GHz = 134 GIOPS

```
AVX2, MSVC
	bandwidth:		108.0GB/s
	time:			0.12s
	str size:		131 KB			- L2 cache
	instructions:	410M			= 131'176 * 100'000 / 32
	inst/s:			3.42 GInst/s	= ~3.4 GHz  - used 2 instruction (cmpeq, movemask) on 4.2GHz, it's ~1.6 inst/cy

AVX2, Clang
	bandwidth:		131.6GB/s
	time:			99.67ms
	str size:		131 KB			- L2 cache
	instructions:	410M			= 131'176 * 100'000 / 32
	inst/s:			4.11 GInst/s	= ~4.1 GHz  - used 2 instruction (cmpeq, movemask) on 4.2GHz, it's near to 2 inst/cy
```


# Intel N150, 3.6 GHz

* CPU Clock: 3.6 GHz
* L1: 32B/cy, 115 GB/s
* Cache:
	- L1D: 64KB per 2 cores (32KB per core)
	- L2: 2MB shared
	- L3: 6MB shared
* Memory: 16 GB DDR4-3200, single channel, 20GB/s

```
AVX2, MSVC
	bandwidth:		61.0GB/s
	time:			0.22 s
	str size:		131 KB			- L2 cache
	instructions:	410M			= 131'176 * 100'000 / 32
	inst/s:			1.86 GInst/s	= 3.73GHz with 1 inst/cy
```


# Apple M1

* CPU Clock: 3.2 GHz
* Cache:
	- L1D: 4x 128 KB
	- L2:  4x 3MB,  12 MB
* Memory: 16 GB, LPDDR4X-4266 DC 16bit, 2133 MHz, 128bit Bus, 68.25 GB/s

```
	bandwidth:		61.1 GB/s
	time:			0.21 s
	str size:		131 KB			- L2 cache
	instructions:	410M			= 131'176 * 100'000 / 32
	inst/s:			1.95 GInst/s	= 3.2 GHz with 1.2 inst/cy
```

# Intel Ultra 7 255H

```
AVX2, MSVC
	bandwidth:		130.5 GB/s
	time:			0.1 s
	str size:		131 KB			- L2 cache
	instructions:	410M			= 131'176 * 100'000 / 32
	inst/s:			4.11 GInst/s	- used 2 instruction (cmpeq, movemask) on 5.1GHz, it's 1.6 inst/cy

AVX2, Clang
	str size:		131 KB			- L2 cache
	instructions:	410M			= 131'176 * 100'000 / 32

	P-core:
		clock:			5.1 GHz
		bandwidth:		153.4 GB/s
		time:			85.5 ms
		inst/s:			4.8 GInst/s		- used 2 instruction (cmpeq, movemask), it's 1.9 inst/cy

	E-core:
		clock:			4.4 GHz
		bandwidth:		115.2 GB/s
		time:			110 ms
		inst/s:			3.73 GInst/s	- used 2 instruction (cmpeq, movemask), it's 1.7 inst/cy

	LP E-core:
		clock:			2.5 GHz
		bandwidth:		48.5 GB/s
		time:			270 ms
		inst/s:			1.52 GInst/s	- used 2 instruction (cmpeq, movemask), it's 1.2 inst/cy
```
