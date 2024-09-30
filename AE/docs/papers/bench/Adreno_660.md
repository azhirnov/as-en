
# Qualcomm Adreno 660

## Specs

* Clock: 840 MHz (790?)
* F16 GFLOPS: **3244** (680 GOp/s on MulAdd from tests)
* F32 GFLOPS: **1622** (364 GOp/s on FMA from tests)
* F64 GFLOPS: **405**
* GMem size: 1.5 Mb (bandwidth?)
* L2: ? (bandwidth?)
* ALUs: 1024
* Memory: 8 GB, LPDDR5-6400, QC 16bit, 3200MHz, **51.2** GB/s (34 GB/s from tests)
* Device: Asus ROG Phone 5 (Android 13, Driver 512.530.0)


## Shader

### Quads

* Test `subgroupQuadBroadcast( gl_HelperInvocation )` without texturing - helper invocations are **not** executed (or not detected). [[6](../GPU_Benchmarks.md#6-Subgroups)]
* Test `subgroupQuadBroadcast( gl_HelperInvocation )` with texturing - helper invocations are executed, even if `Nearest` immutable sampler is used. [[6](../GPU_Benchmarks.md#6-Subgroups)]<br/>
Red - no helper invocations, violet - 3 helper invocations per quad.<br/>
![](img/full-quad/adreno-660-tex-ht.png)

* Test `subgroupQuadBroadcast( constant )` with/without texturing - helper invocations are executed. [[6](../GPU_Benchmarks.md#6-Subgroups)]


### Subgroups

* Subgroups in fragment shader can fill multiple triangles, even if they have different `gl_InstanceIndex`. [[6](../GPU_Benchmarks.md#6-Subgroups)]
* Subgroups in fragment shader reserve threads for helper invocations, even if they are not executed. [[6](../GPU_Benchmarks.md#6-Subgroups)]

* Subgroup occupancy with texturing. Helper invocations are executed and included as active thread. Red color - full subgroup. [[6](../GPU_Benchmarks.md#6-Subgroups)]<br/>
![](img/full-subgroup/adreno-660-tex.png)

* Subgroup occupancy without texturing. Helper invocations are not executed but threads are reserved, so occupancy is low. Red color - full subgroup. [[6](../GPU_Benchmarks.md#6-Subgroups)]<br/>
![](img/full-subgroup/adreno-660.png)

* Subgroup occupancy for too small triangles. Red color - full subgroup. [[6](../GPU_Benchmarks.md#6-Subgroups)]<br/>
![](img/full-subgroup/adreno-660-large.png)

### Subgroup threads order

Result of `Rainbow( gl_SubgroupInvocationID / gl_SubgroupSize )` in fragment shader, gl_SubgroupSize: 64. [[6](../GPU_Benchmarks.md#6-Subgroups)]

![](img/graphics-subgroups/adreno-600.png)

Result of `Rainbow( gl_SubgroupInvocationID / gl_SubgroupSize )` in compute shader, gl_SubgroupSize: 64, workgroup size: 8x8. [[6](../GPU_Benchmarks.md#6-Subgroups)]

![](img/compute-subgroups/adreno-600.png)


### Instruction cost

* FP32 instruction benchmark [[2](../GPU_Benchmarks.md#2-fp32-instruction-performance)]:
	- Loop unrolling is fast during pipeline creation if loop < 256.
	- Loop unrolling is 1x - 1.4x faster, 2x slower on 1024, 1.1x slower on 256.
	- Loop index with `int` and `float` has same performance.
	- Compute dispatch on 2Kx2K grid is faster, 256x256 grid is a bit slower.
	- Graphics render area 2Kx2K is faster.
	- 128 subgroup size (wave128) has no effect on performance.

	| GOp/s | exec time (ms) | ops | max GFLOPS |
	|---|---|---|
	| **420** | 10.2 | F32Add, F32Mul    | 420 |
	| **364** | 11.8 | F32FMA, F32MulAdd | **728** |

* FP16 instruction benchmark [[1](../GPU_Benchmarks.md#1-fp16-instruction-performance)]:

	| GOp/s | exec time (ms) | ops | max GFLOPS |
	|---|---|---|
	| **830** | 5.16 | F16Add, F16Mul | 830 |
	| **707** | 6.06 | F16MulAdd      | **1414** |
	| **117** | 36.5 | F16FMA         | 234 |

## Resource access

* Texture access 67.1MPix: [[5](../GPU_Benchmarks.md#5-Texture-lookup-performance)]
	- expected read: 268MB per frame.
	- UV bias has no effect.

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|-------|------|
	| 0.5  | 5    | 53   | sequential access, scale x0.5   | too high calculated bandwidth because of texture cache |
	| 1    | 9.6  | 28   | sequential access, scale x1     | near to external memory bandwidth |
	| 1.3  | 12.6 | 21   | random access, noise 16x16      | |
	| 2    | 19   | 14   | sequential access, scale x1.5   | |
	| 2.1  | 20.5 | 13   | random access, noise 8x8        | |
	| 3.2  | 31   | 8.6  | sequential access, scale x2     | |
	| 5.4  | 52.5 | 5.1  | random access, noise 4x4        | |
	| 5.6  | 53.7 |  5   | random access, noise 4x4, off 1 | |
	| 15.6 | 150  | 1.78 | random access, noise 2x2        | |
	| 15.8 | 152  | 1.76 | random access, noise 2x2, off 1 | |
	| 60   | 580  | 0.46 | random access, noise 1x1        | |
	| 60   | 578  | 0.46 | random access, noise 1x1, off 1 | |

* Buffer/Image storage 16bpp 9.4MPix 2x151MB [[7](../GPU_Benchmarks.md#7-BufferImage-storage-access)]

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|------|----|
	| 1    | 8.7  | 34 | Image load/store                          | near to external memory bandwidth |
	| 1    | 8.7  | 34 | Buffer load/store                         | near to external memory bandwidth |
	| 1.06 | 9.2  | 33 | Image load/store with different order     | access pattern cause some cache misses |
	| 1.3  | 11.3 | 27 | Buffer load/store in FS                   | access pattern cause some cache misses - subgroup order in FS is differ than in CS |
	| 1.6  | 13.2 | 23 | Image read/write input attachment RGBA32F | RT compression is not supported, attachments stored into GMem which may be slower than L2 (?) |
	| 1.7  | 15.3 | 20 | Image read/write input attachment 4xRGBA8 | best 10.3ms, depends on RT compression |


## Render target compression

* RGBA8 268MPix downsample 1/2, compressed/uncompressed access: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- expected read: 1.07GB, write: 268MB, total: 1.34GB per frame.
	- 8x8 noise linear/fetch/nearest has same perf.
	- 16x16 noise linear is faster. Linear filter minimize L2 cache misses on high compression rate.
	- graphics to compute: solid color: 68ms, gradient: 72ms, 1x1 noise: 80ms.
	- with storage usage flag - perf same as for graphics.

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|------|----|
	| 1   | 72   | 18.6  | image storage | |
	| 1.7 | 42   | 31.9  | 1x1 noise     | near to external memory bandwidth |
	| 1.6 | 44   | 30.4  | 2x2 noise     | |
	| 2.2 | 32   | 41.9  | 4x4 noise     | |
	| 2.5 | 29   | 46.2  | 8x8 noise     | |
	| 3.3 | 22   | 60.9  | 16x16 noise   | **same as block size** |
	| 3.4 | 21   | 63.8  | gradient      | |
	| 6.9 | 10.4 | 128.8 | solid color   | |

* RGBA16_UNorm, RGBA16F - RT compression is supported. [[3](../GPU_Benchmarks.md#3-Render-target-compression)]

	| diff | exec time (ms) | name |
	|---|---|---|
	| 1   | 150 | image storage |
	| 2   |  75 | 1x1 noise     |
	| 3.3 |  45 | solid color   |

* RGBA32F - RT compression is **not** supported. [[3](../GPU_Benchmarks.md#3-Render-target-compression)]


## Texture cache

* RGBA8_UNorm texture with random access [[9](../GPU_Benchmarks.md#9-Texture-cache)]
	- Measured cache size: 2 KB, 128 KB.
	- 8 texels per pixel, dim ???

	| size (KB) | dimension (px) | exec time (ms) | diff | approx bandwidth (GB/s) |
	|---|---|---|---|
	|   1 |  16x16  |  -    |     | |
	|   2 |  32x16  |  2.3  |     | |
	|   4 |  32x32  |  7    | 3   | |
	|  16 |  64x64  |  12.4 | 1.8 | |
	| 128 | 256x128 |  14   |     | |
	| 256 | 256x256 |  44   | 3   | |
