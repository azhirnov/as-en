
# Qualcomm Adreno 505

## Specs

* gmem: 128 Kb + 8 Kb (128x256 tile for RGBA8)
* L2 cache: ?
* texture cache: ?
* F16 GFLOPS: **86.4** (36 GFLOPS from tests)
* F32 GFLOPS: **43.2** (23 GFLOPS from tests)
* F64 GFLOPS: **10.8**
* Execution units: 1
* ALUs: 48 ?
* Clock: 450 MHz
* Memory v1: 2GB, LPDDR3, 933 MHz, 7.4GB/s (5GB/s from tests)
* Memory v2: 2GB, LPDDR3-1600, SC 32bit, 800MHz, 6.4 GB/s
* Device: Redmi 7A (Android ?, driver ?)


## Shader

### Instruction cost

* Shader instruction benchmark notes: [[4](../GPU_Benchmarks.md#4-Shader-instruction-benchmark)]
	- fp32 FMA is preferred than single FMul or separate FMulAdd
	- fp32 SignOrZero is x2.8 faster than Sign
	- fp32 has fastest Normalize,  Length (x1.1),  Distance (x1.4)
	- fp32 has fastest Clamp,  ClampSNorm (x1.0),  ClampUNorm (x1.0)
	- i32 FindMSB is x2.9 SLOWER than FindLSB
	- fp32 has fastest square root: InvSqrt,  Sqrt (x1.0),  Software2 (x2.3)
	- fp32 has fastest cube root: ExpLog,  Pow (x1.0),  Software2 (x2.5),  Software3 (x2.9)
	- fp32 has fastest quad root: InvSqrt,  Pow (x1.0), Sqrt (x1.0)
	- fp32 has fastest sRGB curve: v3,  v1 (x1.4),  v2 (x1.5)
	- fp32 FastATan is x2.1 faster than native ATan
	- fp32 Pow uses MUL loop - performance depends on power
	
* FP32 instruction performance: [[2](../GPU_Benchmarks.md#2-fp32-instruction-performance)]

	| GOp/s | ops | max GFLOPS |
	|---|---|---|
	| 16.3 | Add    | 16.3   |
	| 14.3 | Mul    | 14.3   |
	| 11.5 | MulAdd | **23** |
	| 10.8 | FMA    | 21.6   |
	
* FP Mediump instruction performance: [[2](../GPU_Benchmarks.md#2-fp32-instruction-performance)]

	| GOp/s | ops | max GFLOPS |
	|---|---|---|
	| 24.3 | Add    | 24.3     |
	| 25.4 | Mul    | 25.4     |
	| 18.3 | MulAdd | **36.6** |
	| 5.2  | FMA    | 10.4     |

### NaN / Inf

* FP32. [[11](../GPU_Benchmarks.md#11-NaN)]

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x | nan | nan | nan | nan | inf | -inf | max | -max |
	| Min(x,0) | 0 | 0 | 0 | 0 | 0 | -inf | 0 | -max |
	| Min(0,x) | 0 | 0 | 0 | 0 | 0 | -inf | 0 | -max |
	| Max(x,0) | 0 | 0 | 0 | 0 | inf | 0 | max | 0 |
	| Max(0,x) | 0 | 0 | 0 | 0 | inf | 0 | max | 0 |
	| Clamp(x,0,1) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| Clamp(x,-1,1) | -1 | -1 | -1 | -1 | 1 | -1 | 1 | -1 |
	| IsNaN | 1 | 1 | 1 | 1 | 0 | 0 | 0 | 0 |
	| IsInfinity | 0 | 0 | 0 | 0 | 1 | 1 | 0 | 0 |
	| bool(x) | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 |
	| x != x | 1 | 1 | 1 | 1 | 0 | 0 | 0 | 0 |
	| Step(0,x) | 1 | 1 | 1 | 1 | 1 | 0 | 1 | 0 |
	| Step(x,0) | 1 | 1 | 1 | 1 | 0 | 1 | 0 | 1 |
	| Step(0,-x) | 1 | 1 | 1 | 1 | 0 | 1 | 0 | 1 |
	| Step(-x,0) | 1 | 1 | 1 | 1 | 1 | 0 | 1 | 0 |
	| SignOrZero(x) | 0 | 0 | 0 | 0 | 1 | -1 | 1 | -1 |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 | -1 | 1 | -1 | 1 |
	| SmoothStep(x,0,1) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| Normalize(x) | nan | nan | nan | nan | nan | nan | 0 | -0 |

* FP Mediump diff:

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x | nan | nan | nan | nan | inf | -inf | 65504 | -65504 |
	| Min(x,0) | 0 | 0 | 0 | 0 | 0 | -inf | 0 | -65504 |
	| Min(0,x) | 0 | 0 | 0 | 0 | 0 | -inf | 0 | -65504 |
	| Max(x,0) | 0 | 0 | 0 | 0 | inf | 0 | 65504 | 0 |
	| Max(0,x) | 0 | 0 | 0 | 0 | inf | 0 | 65504 | 0 |
	| Normalize(x) | nan | nan | nan | nan | nan | nan | 255 | -255 |


## Render target compression

* RGBA8 67.1MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- expected read: 268MB, write: 67MB, total: 335MB per frame.
	- 8x8 noise: linear: 25ms, nearest: 40ms, fetch: 46ms.
	- image storage: load: 350ms, fetch/linear: 150ms.
	- graphics to compute: ??? ms.

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|------|----|
	| 0.19 | 350 | 0.95 | image storage | |
	| 1    | 66  | 5.0  | 1x1 noise     | |
	| 1.6  | 42  | 8.0  | 2x2 noise     | |
	| 2.5  | 26  | 12.9 | 4x4 noise     | **same as block size** |
	| 2.6  | 25  | 13.4 | 8x8 noise     | |
	| 2.6  | 25  | 13.4 | 16x16 noise   | |
	| 2.6  | 25  | 13.4 | gradient      | |
	| 2.7  | 24  | 14   | solid color   | |

