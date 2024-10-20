
# ARM Mali T830 (Midgard gen4)

## Specs

* Cores: 1
* ALU: 2
* L2: 64 Kb
* LS cache (L1?): 16 Kb
* Texture cache: 128 B
* Clock: 1000 MHz
* Tile bits/pixel: 128
* Bus width: 128 bits
* Memory: 2GB, LPDDR3, DC 32bit, 933MHz, **14.9**GB/s (4GB/s from tests)
* FP16 GFLOPS: **56** (10.4 on MulAdd from tests)
* FP32 GFLOPS: **32** (10.4 on MulAdd from tests)
* Device: Samsung J7 Neo (Android 9, Driver 28.0.0)

## Shader

Doesn't support quad and subgroups.

### Instruction cost

* Shader instruction benchmark notes: [[4](../GPU_Benchmarks.md#4-Shader-instruction-benchmark)]
	- fp32 FMA is x0.3 SLOWER than single FMul or separate FMulAdd
	- fp32 SignOrZero is x2.9 faster than FastSign
	- fp32 has fastest Length,  Distance (x1.6),  Normalize (x1.6)
	- fp32 has fastest ClampSNorm,  ClampUNorm (x1.0),  Clamp (x1.3)
	- i32 FindMSB is x1.3 faster than FindLSB
	- fp32 has fastest square root: InvSqrt,  Sqrt (x1.0),  Software2 (x3.3)
	- fp32 has fastest cube root: ExpLog,  Pow (x1.2),  Software2 (x2.4),  Software3 (x2.6)
	- fp32 has fastest quad root: InvSqrt,  Sqrt (x1.0), Pow (x1.6)
	- fp32 has fastest sRGB curve: v1,  v3 (x1.1),  v2 (x1.3)
	- fp32 FastACos is x1.5 faster than native ACos
	- fp32 FastASin is x1.6 faster than native ASin
	- fp32 Pow uses MUL loop - performance depends on power

* FP32 instruction performance: [[2](../GPU_Benchmarks.md#2-fp32-instruction-performance)]

	| GOp/s | ops | max GFLOPS |
	|---|---|---|
	| 6.2 | Add    | 6.2      |
	| 9.9 | Mul    | 9.9      |
	| 5.2 | MulAdd | **10.4** |
	| 1.3 | FMA    | 2.6      |
	
* FP Mediump instruction performance: [[2](../GPU_Benchmarks.md#2-fp32-instruction-performance)]

	| GOp/s | ops | max GFLOPS |
	|---|---|---|
	| 7.7  | Add    | 7.7  |
	| 10.1 | Mul    | 10.1 |
	| 5.2  | MulAdd | **10.4** |
	| 1.3  | FMA    | 2.6  |


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
	| SignOrZero(x) | 0 | 0 | 0 | 0 | 1 | -1 | 1 | -1 |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 | -1 | 1 | -1 | 1 |
	| SmoothStep(x,0,1) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| Normalize(x) | nan | nan | nan | nan | 0 | -0 | 0 | -0 |

### Noise performance

| name | thread count | exec time (ms) | per thread (ns) |
|---|---|---|---|
| SimplexNoise                  | 262K | 14.6 |   55.7 |
| ValueNoise                    | 262K | 15.4 |   58.8 |
| WaveletNoise                  | 262K | 18.1 |   69.1 |
| GradientNoise                 | 262K | 21.9 |   83.6 |
| Voronoi, 2D                   | 262K | 23.6 |   90.1 |
| PerlinNoise                   | 262K | 26.2 |  100   |
| SimplexNoiseFBM, octaves=4    | 65K  | 10.8 |  166   |
| ValueNoiseFBM, octaves=4      | 65K  | 13.5 |  208   |
| Voronoi, 3D                   | 65K  | 15.6 |  240   |
| WarleyNoise                   | 65K  | 16.7 |  257   |
| GradientNoiseFBM, octaves=4   | 65K  | 17.5 |  269   |
| PerlinNoiseFBM, octaves=4     | 65K  | 18.2 |  280   |
| VoronoiCircles                | 65K  | 19.8 |  305   |
| VoronoiContour3, 2D           | 65K  | 21.8 |  335   |
| VoronoiContour2, 2D           | 65K  | 22.2 |  341   |
| WarleyNoiseFBM, octaves=4     | 65K  | 58   |  892   |
| IQNoise                       | 65K  | 75.6 | 1 160  |
| VoronoiContour3, 3D           | 16K  | 23.1 | 1 444  |
| VoronoiContour2, 3D           | 65K  | 97.2 | 1 495  |
| IQNoiseFBM, octaves=4         | 16K  | 76.2 | 4 762  |
| VoronoiContour3FBM, octaves=4 | 4K   | 23.5 | 5 875  |
| VoronoiContourFBM, octaves=4  | 4K   | 23.9 | 5 975  |


### Circle performance

* small circles. [[13](../GPU_Benchmarks.md#13-Circle-geometry)]
	- 512 objects
	- 0.9 MPix

	| shape | exec time (ms) | diff |
	|---|---|---|
	| quad     | **55.7** | - |
	| fan      | 440  | 7.9 |
	| strip    | 370  | 6.6 |
	| max area | 410  | 7.4 |

* 4x4 circles with blending. [[13](../GPU_Benchmarks.md#13-Circle-geometry)]
	- 64 layers
	- 0.2 MPix

	| shape | exec time (ms) | diff (%) | comments |
	|---|---|---|---|
	| quad     | **41.3** | - |
	| fan      | 230  | 5.6 |
	| strip    | 200  | 4.8 |
	| max area | 210  | 5.1 |


### Branching

* Mul vs Branch vs Matrix [[12](../GPU_Benchmarks.md#12-Branching)]
	- 65 KPix, 128 iter, 6 mul/branch ops.
	
	| op | exec time (ms) | diff | comments |
	|---|---|---|---|
	| Mul uniform        | 40.8 | 1.5 |
	| Branch uniform     | **27.8** | - |
	| Matrix uniform     | 20.9 | 0.7 | faster because of vector architecture |
	| - |
	| Mul non-uniform    | 50.2 | 1.8 |
	| Branch non-uniform | 35.7 | 1.3 |
	| Matrix non-uniform | 66.4 | 2.4 |
	| - |
	| Mul avg            | 45.5 | 1.64 |
	| Branch avg         | 31.7 | 1.1  |
	| Matrix avg         | 43.7 | 1.57 |

## Render target compression

* RGBA8 16.8MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- **no compression**

	| diff (read) | read (MB) | write (MB) | name |
	|---|---|---|---|
	| -   | 67.1 | 16.7 | expected |
	| 0.9 | 75   | 17   | image storage |
	| 1   | 68.5 | 17   | solid color |
	| 1   | 68.5 | 17   | gradient |


