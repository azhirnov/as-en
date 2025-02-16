
# AMD RX 570 (GCN4)

## Specs

* CU: 32
* Warp width: 64
* Total ALUs: 2048 - number of simultaneously executing threads
* FP16: **5.095** TFLOPS (not supported in Vulkan)
* FP32: **5.095** TFLOPS (4.4 on FMA from tests)
* FP64: **318.5** GFLOPS
* Clock base: 1168 MHz, boost: 1244 MHz.
* Memory: 4GB, GDDR5, 256 bit, 1750 MHz, **224.0** GB/s (86 GB/s from tests)
* Driver: 2.0.279

Theoretical performance:
```
FLOPS = clock * TotalALUs
FLOPS = clock * CU * warp_width

1168M * 2048 = 2392G FMA ops per second = 4.8 TFLOPS
1244M * 2048 = 2548G FMA ops per second = 5.095 TFLOPS
```

## Shader

### Quads

* Test `subgroupQuadBroadcast( gl_HelperInvocation )` with/without texturing - helper invocations are executed. [[6](../GPU_Benchmarks.md#6-Subgroups)]
* Test `subgroupQuadBroadcast( constant )` with/without texturing - helper invocations are executed. [[6](../GPU_Benchmarks.md#6-Subgroups)]


### Subgroups

* Subgroups in fragment shader can fill multiple triangles, but only with the same `gl_InstanceIndex`. [[6](../GPU_Benchmarks.md#6-Subgroups)]
* Subgroups in fragment shader reserve threads for helper invocations, even if they are not executed. [[6](../GPU_Benchmarks.md#6-Subgroups)]


### Subgroup threads order

Result of `Rainbow( gl_SubgroupInvocationID / gl_SubgroupSize )` in fragment shader, gl_SubgroupSize: 64. [[6](../GPU_Benchmarks.md#6-Subgroups)]

![](img/graphics-subgroups/amd-gcn4.png)

Result of `Rainbow( gl_SubgroupInvocationID / gl_SubgroupSize )` in compute shader, gl_SubgroupSize: 64, workgroup size: 8x8. [[6](../GPU_Benchmarks.md#6-Subgroups)]

![](img/compute-subgroups/amd-gcn4.png)


### Instruction cost

* Shader instruction benchmark results: [[4](../GPU_Benchmarks.md#4-Shader-instruction-benchmark)]
	- base rate: 2.2 TOp/s
	- performance of fp mediump equal to fp32.

	- **float point**

	| op \ type | fp32 | fp64 |
	|---|---|---|
	| Add           | 1   | 8   |
	| Mul           | 1   | 16  |
	| FMA           | 1   | 16  |
	| MulAdd        | 1.5 | 16  |
	| Lerp          | 1   | 16  |
	| Length        | 1.5 | 60  |
	| Normalize     | 1.5 | 225 |
	| Distance      | 2.5 | 69  |
	| Dot           | 2   | 24  |
	| Cross         | 2   | 16  |
	| Min/Max       | 1   | 8   |
	| Clamp(x,0,1)  | 0-1 | 16  |
	| Clamp(x,-1,1) | 1   | 16  |
	| Clamp         | 1   | 16  |
	| Step          | 1.5 | 10  |
	| SmoothStep    | 2   | 224 |
	| Abs           | 0-1 | 0-8 |
	| SignOrZero    | 3   | 20  |
	| BitCast       | 1   | 8   |
	| FloatToInt    | 1   | 64  |
	| IntToFloat    | 1   | 28  |
	| Floor         | 1   | 16  |
	| Ceil, Trunc   | 1   | 36  |
	| Round, RoundEven | 1 | 78 |
	| Fract         | 1   | 8   |
	| Exp2          | 3   | -   |
	| Exp           | 4   | -   |
	| Log, Log2     | 3   | -   |
	| InvSqrt       | 3   | 16  |
	| Sqrt          | 3   | 16  |
	| Sin, Cos      | 8   | -   |
	| Div           | 4   | 164 |
	| Mod           | 7   | 208 |
	| Pow           | 7   | -   |
	| Tan           | 15  | -   |
	| ASin, ACos    | 17  | -   |
	| ATan          | 68  | -   |

	- **float point fast math**

	| op \ type | fp32 | fp64 |
	|---|---|---|
	| fast Sign     | 3   | 27 |
	| Cbrt (pow)    | 7   |
	| Cbrt (exp)    | 8   |
	| sRGB          | 15  |
	| fast sRGB     | 13  |
	| fast Cos      |
	| fast Sin      |
	| fast Tan      |
	| fast ASin     | 12  |
	| fast ACos     | 15  |
	| fast ATan     | 25  |
	| fast ASin v2  |
	| fast ACos v2  |
	| fast ATan v2  |

	- **integer**

	| op \ type | i32 | u32 | i64 | u64 |
	|---|---|---|---|---|
	| Add         | 1   | 1   | 2   | 2   |
	| Mul         | 3.5 | 3.5 | 12  | 12  |
	| MulAdd      | 4.5 | 4.5 | 16  | 16  |
	| Div         | 36  | 31  | 180 | 180 |
	| Mod         | 38  | 34  | 200 | 170 |
	| Min/Max     | 1   | 1   | 7   | 7   |
	| Clamp const | 1   | 1   | 16  | 16  |
	| Clamp       | 1   | 1   | 8   | 8   |
	| Abs         | 2   | -   | 14  | -   |
	| SignOrZero  |
	| Shift const | 1.5 | 1   | 6   | 6   |
	| Shift       | 1   | 1   | 6   | 6   |
	| And         | 1   | 1   | 2   | 2   |
	| Or          | 1   | 1   | 2   | 2   |
	| Xor         | 1   | 1   | 2   | 2   |
	| BitCount    | 1   | 1   | -   | -   |
	| FindLSB     | 1   | 1   | 3   | 3   |
	| FindMSB     | 4   | 3.5 | -   | -   |
	| AddCarry    | -   | 2.5 | -   | -   |
	| SubBorrow   | -   | 2.5 | -   | -   |
	| MulExtended | 8   | 8   | -   | -   |


* FP32 instruction performance: [[2](../GPU_Benchmarks.md#2-fp32-instruction-performance)]
	- Benchmarking in compute shader is a bit faster.

	| TOp/s | ops | max TFLOPS |
	|---|---|---|
	| 2.2 | Add, Mul | 2.2     |
	| 2.2 | FMA      | **4.4** |
	| 1.6 | MulAdd   | 3.2     |


### NaN / Inf

* FP32, Mediump. [[11](../GPU_Benchmarks.md#11-NaN)]

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
	| SignOrZero(x) | 1 | 1 | 1 | 1 | 1 | -1 | 1 | -1 |
	| SignOrZero(-x) | 1 | 1 | 1 | 1 | -1 | 1 | -1 | 1 |
	| SmoothStep(x,0,1) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| Normalize(x) | nan | nan | nan | nan | 0 | 0 | 0 | 0 |

* FP64 diff:

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| SignOrZero(x) | 0 | 0 | 0 | 0 | 1 | -1 | 1 | -1 |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 | -1 | 1 | -1 | 1 |
	| Normalize(x) | nan | nan | nan | nan | nan | nan | 0 | -0 |

### Shared memory
TODO

### Noise performance


### Circle performance

* small circles. [[13](../GPU_Benchmarks.md#13-Circle-geometry)]
	- 131K objects
	- 22.9 MPix

	| shape | exec time (ms) | diff (%) |
	|---|---|---|
	| quad     | **1.71** | - |
	| fan      | 2.0  | 17 |
	| strip    | 2.0  | 17 |
	| max area | 1.84 |  8 |

* 4x4 circles with blending. [[13](../GPU_Benchmarks.md#13-Circle-geometry)]
	- 5.7 MPix
	- 128 layers

	| shape | exec time (ms) | diff (%) |
	|---|---|---|
	| quad     | **98.1** | - |
	| fan      | 75.9 | 29 |
	| strip    | 77.1 | 27 |
	| max area | 78.6 | 25 |


### Branching

* Mul vs Branch vs Matrix [[12](../GPU_Benchmarks.md#12-Branching)]
	- 4.2 MPix, 128 iter, 6 mul/branch ops.

	| op | exec time (ms) | diff |
	|---|---|---|
	| Mul uniform        | 15.0 | 1.7 |
	| Branch uniform     | **8.9** | - |
	| Matrix uniform     | 8.4  | 0.94 |
	| - |
	| Mul non-uniform    | 20.7 | 2.3 |
	| Branch non-uniform | 14.4 | 1.6 |
	| Matrix non-uniform | 23.4 | 2.6 |
	| - |
	| Mul avg            | 17.9 | 2.0 |
	| Branch avg         | 11.7 | 1.3 |
	| Matrix avg         | 15.9 | 1.8 |

## Blending
TODO


## Resource access


## Render target compression

* RGBA8 268.4MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- expected read: 1.07GB, write: 268.4MB, total: 1.34GB per frame.
	- 8x8 noise: linear: 4.1ms, fetch/nearest: 4.7ms.
	- image storage: load: 15.5ms, fetch/linear: 12.5ms. Texture cache is faster than L2 access.
	- graphics to compute: 15.5ms. Compression disabled when used storage usage flag.

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|------|----|
	| 1   | 15.5 | 86  | image storage        | |
	| 1.3 | 12   | 112 | time for 1x1 noise   | |
	| 1.3 | 11.5 | 116 | time for 2x2 noise   | |
	| 3.0 | 5.2  | 258 | time for 4x4 noise   | **same as block size** |
	| 3.8 | 4.1  | 326 | time for 8x8 noise   | |
	| 3.9 | 4.0  | 335 | time for 16x16 noise | |
	| 3.9 | 4.0  | 335 | time for gradient    | |
	| 3.9 | 4.0  | 335 | time for solid color | |


* RGBA16_UNorm 151MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- expected read: 1.2GB, write: 302MB, total: 1.51GB per frame.

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|------|----|
	| 1   | 18   | 86  | image storage        | |
	| 1.3 | 13.6 | 114 | time for 1x1 noise   | |
	| 1.4 | 12.9 | 120 | time for 2x2 noise   | |
	| 2.5 | 7.1  | 218 | time for gradient    | |
	| 3.1 | 5.8  | 266 | time for 4x4 noise   | **same as block size** |
	| 3.9 | 4.6  | 336 | time for 8x8 noise   | |
	| 4.0 | 4.5  | 344 | time for solid color | |
	| 4.0 | 4.5  | 344 | time for 16x16 noise | |


* RGBA16F 151MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- expected read: 1.2GB, write: 302MB, total: 1.51GB per frame.

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|------|----|
	| 1   | 18   | 86  | image storage        | |
	| 1.3 | 13.6 | 114 | time for 1x1 noise   | |
	| 1.4 | 12.6 | 123 | time for 2x2 noise   | |
	| 3.1 | 5.8  | 266 | time for 4x4 noise   | **same as block size** |
	| 3.7 | 4.8  | 322 | time for gradient    | |
	| 3.9 | 4.6  | 336 | time for 8x8 noise   | |
	| 3.9 | 4.6  | 336 | time for 16x16 noise | |
	| 4.0 | 4.5  | 344 | time for solid color | |


* RGBA32F 67.1MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- expected read: 1.07GB, write: 268.4GB, total: 1.34GB per frame.

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|------|----|
	| 1   | 16.2 | 85  | image storage        | |
	| 1.1 | 14.7 | 93  | time for 1x1 noise   | |
	| 1.2 | 13.7 | 100 | time for 2x2 noise   | |
	| 1.2 | 13.2 | 104 | time for 4x4 noise   | **same as block size** |
	| 1.2 | 13.2 | 104 | time for 8x8 noise   | |
	| 1.2 | 13.2 | 104 | time for 16x16 noise | |
	| 1.2 | 14.0 | 98  | time for gradient    | |
	| 2.8 | 5.8  | 236 | time for solid color | |


## Texture cache
TODO

