
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
	- fp32 has fastest square root: InvSqrt,  Sqrt (x1.0),  Software2 (x2.3)
	- fp32 has fastest cube root: ExpLog,  Pow (x1.0),  Software2 (x2.5),  Software3 (x2.9)
	- fp32 has fastest quad root: InvSqrt,  Pow (x1.0), Sqrt (x1.0)
	- fp32 has fastest sRGB curve: v3,  v1 (x1.4),  v2 (x1.5)
	- fp32 FastATan is x2.1 faster than native ATan
	- fp32 Pow uses MUL loop - performance depends on power
	- `int mediump` has same performance as `int lowp`.
	
* Shader instruction benchmark results: [[4](../GPU_Benchmarks.md#4-Shader-instruction-benchmark)]
	- base rate: 16 GOp/s

	- **float point**
	
	| op \ type | fp32 | mediump |
	|---|---|---|
	| Add           | 1   | 0.5 |
	| Mul           | 1   | 0.5 |
	| FMA           | 1.5 | 3   |
	| MulAdd        | 1.5 | 1   |
	| Lerp          | 3.5 | 2   |
	| Length        | 2   | 1   |
	| Normalize     | 2   | 1   |
	| Distance      | 3   | 1.5 |
	| Dot           | 2.5 | 1   |
	| Cross         | 3   | 1.5 |
	| Min/Max       | 1   | 0.5 |
	| Clamp(x,0,1)  | 2   | 1   |
	| Clamp(x,-1,1) | 2   | 1   |
	| Clamp         | 2   | 1   |
	| Step          | 2   | 1.5 |
	| SmoothStep    | 3   | 1.5 |
	| Abs           | 1   | 0.5 |
	| FastSign      | 3   | 2   |
	| SignOrZero    | 1   | 1   |
	| BitCast       | 1   | -   |
	| FloatToInt    | 1   | -   |
	| IntToFloat    | 1   | -   |
	| Ceil, Floor, Trunc, Round, RoundEven | 1 | 1 |
	| Fract         | 2   | 1.5 |
	| Exp, Exp2     | 5   | -   |
	| Log, Log2     | 5   | -   |
	| InvSqrt       | 5   | 3   |
	| Sqrt          | 5   | 6   |
	| Sin, Cos      | 6   | -   |
	| Div           | 6   | 6   |
	| Mod           | 6   | 6   |
	| Pow           | 3-12 | -  |
	| Tan           | 18  | -   |
	| ASin, ACos    | 8   | -   |
	| ATan          | 48  | -   |
	
	- **integer**
	
	| op \ type | i32 | u32 | int mediump | uint mediump | i16 | u16 |
	|---|---|---|---|---|---|---|
	| Add         | 1  | 1  |
	| Mul         | 4  | 4  |
	| MulAdd      | 5  | 5  |
	| Div         | 28 | 22 |
	| Mod         | 32 | 30 |
	| Min/Max     | 1  | 1  |
	| Clamp const | 1  | 1  |
	| Clamp       | 1  | 1  |
	| Abs         | 1  | -  |
	| SignOrZero  |
	| Shift const | 1  | 1  |
	| Shift       | 1  | 1  |
	| And         | 1  | 1  |
	| Or          | 1  | 1  |
	| Xor         | 1  | 1  |
	| BitCount    | 5  | 5  |
	| FindLSB     | 1  | 1  |
	| FindMSB     | 3  | 2  |
	| AddCarry    | -  | 4  |
	| SubBorrow   | -  | 3  |
	| MulExtended | 30 | 22 |

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


### Circle performance

* small circles. [[13](../GPU_Benchmarks.md#13-Circle-geometry)]
	- 8K objects
	- 4.15 MPix

	| shape | exec time (ms) | diff (%) |
	|---|---|---|
	| quad     | **4.95** | - |
	| fan      | 5.74 | 16 |
	| strip    | 5.70 | 15 |
	| max area | 5.60 | 13 |

* 4x4 circles with blending. [[13](../GPU_Benchmarks.md#13-Circle-geometry)]
	- 1.04 MPix
	- 64 layers

	| shape | exec time (ms) | diff (%) |
	|---|---|---|
	| quad     | **31.5** | - |
	| fan      | 25.3 | 24.5 |
	| strip    | 25.1 | 25.5 |
	| max area | 25.2 | 25   |


### Branching

* Mul vs Branch vs Matrix [[12](../GPU_Benchmarks.md#12-Branching)]
	- 262 KPix, 128 iter, 6 mul/branch ops.
	
	| op | exec time (ms) | diff |
	|---|---|---|
	| Mul uniform        | 70.8 | 1.6 |
	| Branch uniform     | **44.6** | - |
	| Matrix uniform     | 39.1 | 0.88 | faster because of vector architecture (?) |
	| - |
	| Mul non-uniform    | 82.8 | 1.9 |
	| Branch non-uniform | 92.7 | 2.1 |
	| Matrix non-uniform | 120  | 2.7 |
	| - |
	| Mul avg            | 76.8 | 1.72 |
	| Branch avg         | 68.6 | 1.54 |
	| Matrix avg         | 79.6 | 1.78 |

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

