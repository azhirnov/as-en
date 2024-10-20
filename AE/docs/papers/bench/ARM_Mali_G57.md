
# ARM Mali G57 MC2 (Valhall gen1)

## Specs

* Cores: 2
* ALU: 2
* L2 cache: 512 Kb
* LS cache: 16 Kb
* Texture cache: 32 Kb
* Tile bits/pixel: 256 *(32 bytes/pixel, 2xRGBA32)*
* Warp width: 16
* FP16 GFLOPS: **242** (121 GOp/s on MulAdd from tests)
* FP32 GFLOPS: **121** (60.7 GOp/s on FMA from tests)
* Clock: 950 MHz
* Texture cache: 32 Kb
* Max work registers (32b): 64
* Memory: 4GB, LPDDR4X, DC 16bit, 2133 MHz, **17.07** GB/s (14.2 GB/s from tests)
* Device: Realme 8I (Android 13, Driver 32.1.0)


## Shader

### Quads

* Quads on edge between 2 triangles are not merged, so 2 near pixels may execute up to 6 helper invocations.

* Test `subgroupQuadBroadcast( gl_HelperInvocation )` without texturing - helper invocations are **not** executed. [[6](../GPU_Benchmarks.md#6-Subgroups)]

* Test `subgroupQuadBroadcast( constant )` without texturing - helper invocations are **not** executed. [[6](../GPU_Benchmarks.md#6-Subgroups)]<br/>
Red - full quad, blue - only 1 thread per quad.<br/>
![](img/full-quad/valhall-1-qd.png)

* Tests `subgroupQuadBroadcast( gl_HelperInvocation )` and `subgroupQuadBroadcast( constant )` with texturing: [[6](../GPU_Benchmarks.md#6-Subgroups)]
	- `textureGrad()`, `texelFetch()` - helper invocations are **not** executed.
	- `textureLod()` - helper invocations are **not** executed.
	- `texture()` - helper invocations are executed, even if `Nearest` immutable sampler is used.
	- helper invocations are executed if used any derivative.<br/>
	Red - no helper invocations, blue - 3 helper invocations per quad.<br/>
	![](img/full-quad/valhall-1-tex-ht.png)


### Subgroups

* Helper invocation can be early terminated, but threads are allocated and number of warps with helper invocations and without are same (from performance counters). [[6](../GPU_Benchmarks.md#6-Subgroups)]

* Subgroup occupancy for single triangle with texturing. Helper invocations are executed and included as active thread. Red color - full subgroup. [[6](../GPU_Benchmarks.md#6-Subgroups)]<br/>
![](img/full-subgroup/valhall-1-tex.png)

* Subgroup occupancy for single triangle without texturing. Helper invocations are not executed but threads are reserved, so occupancy is low. Red color - full subgroup. [[6](../GPU_Benchmarks.md#6-Subgroups)]<br/>
![](img/full-subgroup/valhall-1.png)

* Subgroup occupancy for too small triangles. Red color - full subgroup. [[6](../GPU_Benchmarks.md#6-Subgroups)]<br/>
![](img/full-subgroup/valhall-1-large.png)

* Result of `Rainbow( Hash( subgroupAdd( gl_FragCoord.xy )))` for 4 quads without instancing. [[6](../GPU_Benchmarks.md#6-Subgroups)]<br/>
![](img/unique-subgroups/valhall-1-tris.png)<br/>
Subgroup occupancy, red - full subgroup (16 threads), green: ~8 threads per subgroup.<br/>
![](img/unique-subgroups/valhall-1-tris-occup.png)
	
* Result of `Rainbow( Hash( subgroupAdd( gl_FragCoord.xy )))` for 4 quads with instancing, first instance - first triangle in quad, second instance - second triangle. [[6](../GPU_Benchmarks.md#6-Subgroups)]<br/>
Triangles with different `gl_InstanceIndex` can be merged into a single subgroup but this is a rare case.<br/>
![](img/unique-subgroups/valhall-1-inst.png)


### Subgroup threads order

Result of `Rainbow( gl_SubgroupInvocationID / gl_SubgroupSize )` in fragment shader, gl_SubgroupSize: 16, image size: 16x16. [[6](../GPU_Benchmarks.md#6-Subgroups)]

![](img/graphics-subgroups/valhall-1.png)

Unique subgroups, image size: 32x32, gl_SubgroupSize: 16. Each subgroup in tile scheduled by quads (2x2 pixels), each quad may have any position inside 32x32 pixel tile, but often they are placed inside 8x8 region. [[6](../GPU_Benchmarks.md#6-Subgroups)]

![](img/unique-subgroups/valhall-1-2tiles.png)

Result of `Rainbow( gl_SubgroupInvocationID / gl_SubgroupSize )` in compute shader, gl_SubgroupSize: 16, workgroup size: 8x8. [[6](../GPU_Benchmarks.md#6-Subgroups)]

![](img/compute-subgroups/valhall-1.png)


### Instruction cost

* Shader instruction benchmark notes: [[4](../GPU_Benchmarks.md#4-Shader-instruction-benchmark)]
	- Only fp32 FMA - *(fp16 and mediump use same fp32 FMA)*.
	- Fp32 FMA is preferred than FMul or FMulAdd.
	- Fp32 and i32 datapaths can execute in parallel in 2:1 rate
	- Fp16 and mediump is 2x faster than fp32 in FMull, FAdd.
	- Length is a bit faster than Distance and Normalize.
	- ClampUNorm and ClampSNorm are fast.
	- fp16x2 FMA is used, scalar FMA doesn't have x2 performance
	- fp32 FastACos is x2.3 faster than native ACos
	- fp32 FastASin is x2.6 faster than native ASin
	- fp16 FastATan is x1.8 faster than native ATan
	- fp16 FastACos is x3.7 faster than native ACos
	- fp16 FastASin is x4.2 faster than native ASin
	- fp32 Pow uses MUL loop - performance depends on power
	- fp32 SignOrZero is x2.3 faster than Sign
	
* FP32 instruction performance: [[2](../GPU_Benchmarks.md#2-fp32-instruction-performance)]
	- Loop unrolling doesn't change performance.
	- Manual loop unrolling doesn't change performance too.
	- Loop index with `int` is faster than `float`.
	- Graphics and compute has same performance.
	- Compute dispatch on 128 - 2K grid is faster.
	- Compiler can optimize only addition, so test combine Add and Sub.
	
	| Gop/s | op | GFLOPS |
	|---|---|---|
	| 60.7 | Add, Mul    | 60.7 |
	| 60.7 | MulAdd, FMA | **121** |
	
* FP16 instruction performance: [[1](../GPU_Benchmarks.md#1-fp16-instruction-performance)]
	- Measured at 950 MHz

	| Gop/s | op | GFLOPS | comments |
	|---|---|---|---|
	| 60  | FMA      | 120 | equal to F32FMA |
	| 121 | Add, Mul | 121 |
	| 121 | MulAdd   | **240** |


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
	| Step(0,x) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| Step(x,0) | 0 | 0 | 0 | 0 | 0 | 1 | 0 | 1 |
	| Step(0,-x) | 0 | 0 | 0 | 0 | 0 | 1 | 0 | 1 |
	| Step(-x,0) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| SignOrZero(x) | 0 | 0 | 0 | 0 | 1 | -1 | 1 | -1 |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 | -1 | 1 | -1 | 1 |
	| SmoothStep(x,0,1) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| Normalize(x) | nan | nan | nan | nan | 0 | -0 | 0 | -0 |


* FP32 Mediump diff:

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| Normalize(x) | -1 | -1 | -1 | -1 | 1 | -1 | 1 | -1 |
### Noise performance

| name | thread count | exec time (ms) | ALU (%) | per thread (ns) |
|---|---|---|---|---|
| ValueNoise                    | 1.05M | 2.4  | 89 | 2.3  |
| PerlinNoise                   | 1.05M | 3.6  | 93 | 3.4  |
| Voronoi, 2D                   | 1.05M | 3.6  | 91 | 3.4  |
| SimplexNoise                  | 1.05M | 3.7  | 93 | 3.5  |
| GradientNoise                 | 1.05M | 3.9  | 93 | 3.7  |
| WaveletNoise                  | 1.05M | 3.9  | 91 | 3.7  |
| ValueNoiseFBM, octaves=4      | 1.05M | 9.6  | 94 | 9.1  |
| Voronoi, 3D                   | 1.05M | 10.9 | 94 | 10.4 |
| WarleyNoise                   | 1.05M | 10.9 | 94 | 10.4 |
| VoronoiCircles                | 1.05M | 12.5 | 95 | 11.9 |
| SimplexNoiseFBM, octaves=4    | 1.05M | 14.8 | 96 | 14.1 |
| PerlinNoiseFBM, octaves=4     | 1.05M | 15.1 | 95 | 14.4 |
| GradientNoiseFBM, octaves=4   | 1.05M | 16.5 | 96 | 15.7 |
| VoronoiContour2, 2D           | 1.05M | 21.2 | 94 | 20.2 |
| VoronoiContour3, 2D           | 262K  | 5.4  | 92 | 20.6 |
| WarleyNoiseFBM, octaves=4     | 262K  | 12.1 | 95 | 46.2 |
| IQNoise                       | 262K  | 18   | 95 | 68.7 |
| VoronoiContour2, 3D           | 262K  | 28.5 | 95 | 109  |
| VoronoiContour3, 3D           | 65K   | 17.5 | **40** | 269 |
| IQNoiseFBM, octaves=4         | 65K   | 20.5 | 95 | 315  |
| VoronoiContourFBM, octaves=4  | 65K   | 29.3 | 94 | 451  |
| VoronoiContour3FBM, octaves=4 | 16K   | 21.5 | **34** | 1344 |


### Circle performance

* small circles. [[13](../GPU_Benchmarks.md#13-Circle-geometry)]
	- 8K objects
	- 10.4 MPix

	| shape | exec time (ms) | diff (%) | part quad (%) | mem traffic (MB) |
	|---|---|---|---|---|
	| quad     | **4.49** | -   | **20** | **297** |
	| fan      | 4.73 | 5.3 | 50 | 430 |
	| strip    | 4.65 | 3.5 | 50 | 412 |
	| max area | 4.62 | 2.9 | 46 | 412 |

* 4x4 circles with blending. [[13](../GPU_Benchmarks.md#13-Circle-geometry)]
	- 64 layers
	- ? MPix

	| shape | exec time (ms) | diff (%) | comments |
	|---|---|---|---|
	| quad     | 49.7 | -  | large area, high overdraw |
	| fan      | 40.9 | 21 |
	| strip    | **40.0** | **24** |
	| max area | 40.7 | 22 |


### Branching

* Mul vs Branch vs Matrix [[12](../GPU_Benchmarks.md#12-Branching)]
	- 262 KPix, 128 iter, 6 mul/branch ops.
	
	| op | exec time (ms) | diff |
	|---|---|---|
	| Mul uniform        | 15.1 | 2.1 |
	| Branch uniform     | **7.3** | - |
	| Matrix uniform     | 10.3 | 1.4 |
	| - |
	| Mul non-uniform    | 16.7 | 2.3 |
	| Branch non-uniform | 15.5 | 2.1 |
	| Matrix non-uniform | 25.5 | 3.5 |
	| - |
	| Mul avg            | 15.9 | 2.18 |
	| Branch avg         | 11.4 | 1.56 |
	| Matrix avg         | 17.9 | 2.45 |


## Blending

* Blend vs Discard in FS: TODO: use new test
	- 1x   opaque: 2.3ms
	- 3.2x discard: 7.3ms
	- 3.7x blend `src + dst * (1 - src.a)`: 8.5ms
	- 6.5x blend `src * (1 - dst.a) + dst * (1 - src.a)`: 15ms - accessing `dst` is slow!


## Resource access


* Buffer/Image storage RGBA32F 4.19MPix 2x67.1MB [[7](../GPU_Benchmarks.md#7-BufferImage-storage-access)]
	- 1.07MPix lost 2x of performance (350MHz, 5GB/s).

	| diff | exec time (ms) | traffic (GB/s) | L2 read miss (%) | name | comments |
	|---|---|---|---|-----------|------|
	| 1    | 9.0  | 13.2 | 50 | Image fetch/sample in FS with double buffering |
	| 1.01 | 9.1  | 13.0 | 15 | Image read/write attachment RGBA32F            | low L2 read miss because of prefetch (?), used 128bits/pixel in tile |
	| 1.03 | 9.3  | 13.7 | 15 | Image read/write attachment 2xRGBA16           |
	| 1.08 | 9.7  | 13.6 | 50 | Image load/store different order               | image access should be reordered to match Z-curve (?) |
	| 1.11 | 10   | 12.7 | 50 | Image load/store                               |
	| 1.23 | 11.1 | 11.9 | 40 | Buffer load/store                              | 16byte per load/store, which is less than cache line (32/64?) |
	| 1.35 | 12.2 | 10.7 | 34 | Buffer load/store in FS                        |
	| 1.82 | 16.4 |  3.5 | **90** | Image read/write attachment 4xRGBA8        | RT compression doesn't increase performance, because of 4 attachments (?) |


## Render target compression

* RGBA8 67.1MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- related specs: AFBC v1.3 with 4x4 block size; 16x16 tile size.
	- linear: 18.3ms, fetch: 33ms, nearest: 33ms. Linear filter minimize L2 cache misses on high compression rate.
	- graphics to compute r/w: 268MB / 66MB. Compression disabled when used storage usage flag.
	
	| diff (read) | diff (time) | read (MB) | exec time (ms) | name | comments |
	|---|---|---|---|---|---|
	| -   | -   | 268 | -  | expected      | |
	| 1   | -   | 268 |   | image storage | |
	| 0.9 | 1   | 284 | 19.8 | 1x1 noise     | 16MB of metadata included, 64B per tile (?) |
	| 0.9 | 1   | 284 | 19.8 | 2x2 noise     | |
	| 5.6 | 1.9 | 48  | 10.2 | 4x4 noise     | **same as block size** |
	| 7.4 | 2.0 | 36  | 9.8  | gradient      | |
	| 8.4 | 1.9 | 32  | 10.2 | 8x8 noise     | |
	| 19  | 3.9 | 14  | 5.10 | 16x16 noise   | same as tile size |
	| 19  | 3.9 | 14  | 5.10 | solid color   | |


* RG16F 67.1MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	
	| diff (read) | diff (time) | read (MB) | exec time (ms) | name | comments |
	|---|---|---|---|---|---|
	| -    | -   | 268 | -    | expected      | |
	| 1    | 1   | 268 | 73.5 | image storage | |
	| 1    | 1   | 268 | 19.1 | 1x1 noise     | |
	| 1.25 | 1.2 | 215 | 16.0 | 2x2 noise     | |
	| 3.4  | 1.8 | 78  | 10.6 | gradient      | |
	| 5.7  | 1.9 | 47  | 10.2 | 4x4 noise     | **same as block size** |
	| 8.4  | 1.9 | 32  | 10.2 | 8x8 noise     | |
	| 20   | 3.7 | 14  | 5.10 | 16x16 noise   | same as tile size |
	| 20   | 3.7 | 14  | 5.10 | solid color   | traffic equal to 16x16 noise |


## Texture cache

* RGBA8_UNorm texture with random access [[9](../GPU_Benchmarks.md#9-Texture-cache)]
	- Measured cache size: 32 KB, 512 KB.
	- From specs: 32 KB, 512 KB.

	| size (KB) | dimension (px) | L2 bandwidth (GB/s) | external bandwidth (GB/s) | comment |
	|---|---|---|---|---|
	| 16      |  64x64  | 0.009 | 0.004 | **used only texture cache** |
	| **32**  | 128x64  | 0.38  | 0.004 | |
	| 64      | 128x128 | 45    | 0.004 | **used L2 cache** |
	| 128     | 256x128 | 45    | 0.004 | |
	| 256     | 256x256 | 49    | 4     | |
	| **512** | 512x256 | 49    | 7.6   | **L2 cache with 15% miss** |
	| 1024    | 512x512 | 24    | 12.5  | **30% L2 miss, bottleneck on external memory** |
