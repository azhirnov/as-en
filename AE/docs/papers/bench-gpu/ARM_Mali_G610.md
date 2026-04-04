
# ARM Mali G610 MC6 (Valhall gen3)

## Specs

* Cores: 6  *(from name MC6)*
* ALU (SIMD/warps): 4  *(from specs)*
* Warp width: 16
* Clock: 950 MHz
* Device: Infinix GT 20 Pro (MediaTek Dimensity 8200, Android 14, Driver 38.1.0, vk 1.1.219)

### Memory

* L2 cache: 2 MB
* Memory: 8GB, LPDDR5-6400, 3200 MHz, QC 16bit, 51.2 GB/s

### Float point performance

* Total ALUs: 768 - number of simultaneously executing threads
* FP16 GFLOPS: 2884 (  on MulAdd from tests)
* FP32 GFLOPS: 1442 (  on FMA from tests)
* FP32/cy per ALU: 32 (= 2 * warp)

Theoretical performance:
```
FLOPS = clock * warp_width*2 * ALUs * Cores
Total ALUs = warp_width*2 * ALUs * Cores

950M * 16*2 * 4 * 6 = 730G FMA ops per second = 1459 GFLOPS
```


### Instruction cost

* FP32 instruction performance: [[2](../GPU_Benchmarks.md#2-fp32-instruction-performance)]

	| Gop/s | op | GFLOPS |
	|---|---|---|
	| 352 | Add         | 352 |
	| 354 | Mul         | 354 |
	| 266 | MulAdd, FMA | 532 |
### Branching

* Mul vs Branch vs Matrix [[12](../GPU_Benchmarks.md#12-Branching)]
	- 1.6 MPix, 128 iter, 6 mul/branch ops.

	| op | exec time (ms) | diff |
	|---|---|---|
	| Mul uniform          | 18.2 | 1.7 |
	| Branch uniform       | **10.7** | - |
	| Matrix uniform       | 12.8 | 1.2 |
	| - |
	| Mul non-uniform      | 22.6 | 2.1 |
	| Branch non-uniform   | 17.3 | 1.6 |
	| Matrix non-uniform   | 31.7 | 3.0 |
	| - |
	| Mul avg              | 20.4 | 1.9 |
	| Branch avg           | 14.0 | 1.3 |
	| Matrix avg           | 22.2 | 2.1 |
