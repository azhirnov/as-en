
# Intel UHD 620 (Gen9)

## Specs

* Arch: Kaby Lake-R
* Clock: 300 / 1000 MHz
* Pixel Rate: 3.000 GPixel/s
* Texture Rate: 24.00 GTexel/s
* FP16 GFLOPS: **768.0** (397 GOp/s on FMA from tests)
* FP32 GFLOPS: **384.0** (208 GOp/s on FMA from tests)
* FP64 GFLOPS: **96.00**
* Execution Units: 24
* warp size: 16
* Memory: 8GB LPDDR3, 1867MHz, **29.8** GB/s (21 GB/s from tests)
* Driver: ???


## Shader

### Quads

* Test `subgroupQuadBroadcast( gl_HelperInvocation )` with/without texturing - helper invocations are executed. [[6](../GPU_Benchmarks.md#6-Subgroups)]
* Test `subgroupQuadBroadcast( constant )` with/without texturing - helper invocations are executed. [[6](../GPU_Benchmarks.md#6-Subgroups)]


### Subgroups

* Subgroups in fragment shader can not fill multiple triangles. This leads to unused threads in subgroup. [[6](../GPU_Benchmarks.md#6-Subgroups)]


### Subgroup threads order for Gen9.5

Result of `Rainbow( gl_SubgroupInvocationID / gl_SubgroupSize )` in fragment shader, gl_SubgroupSize: 16. [[6](../GPU_Benchmarks.md#6-Subgroups)]

![](img/graphics-subgroups/intel-gen9_5.png)

Result of `Rainbow( gl_SubgroupInvocationID / gl_SubgroupSize )` in compute shader, gl_SubgroupSize: 16, workgroup size: 8x8. [[6](../GPU_Benchmarks.md#6-Subgroups)]

![](img/compute-subgroups/intel-gen9_5.png)


### Instruction cost

* [[2](../GPU_Benchmarks.md#2-fp32-instruction-performance)]:
	- Better loop unrolling if count <= 128.
	- Compute is 1.07x faster than graphics.
	- Compute dispatch on 512x512 grid is faster.
	- **208** GOp/s on F32FMA, F32MulAdd F32Mul.
	- **397** GOp/s on F32Add.

* [[1](../GPU_Benchmarks.md#1-fp16-instruction-performance)]:
	- **835** GOp/s on F16Add.
	- **397** GOp/s on F16Mul, F16MulAdd, F16FMA.

## Resource access

* Buffer/Image storage RGBA32F 9.4MPix 2x151MB [[7](../GPU_Benchmarks.md#7-BufferImage-storage-access)]

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|------|----|
	| 1    | 13.3 | 22.7 | Image load/store                  | |
	| 1    | 13.3 | 22.7 | Buffer load/store                 | |
	| 1.03 | 13.7 | 22   | Image fetch/sample from another   | **1.5x faster with high RT compression rate** |
	| 1.07 | 14.2 | 21.3 | Image read/write input attachment | **RT compression is not enabled** |
	| 1.17 | 15.6 | 19.3 | Buffer load/store in FS           | |


## Render target compression

* RGBA8 67.1MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- expected read: 268.4MB, write: 67.1MB, total: 335MB per frame.
	- linear/nearest/fetch has same perf.
	- image storage linear/fetch/load has same perf.
	- graphics to compute 1x1 noise: load 20ms.
	- graphics to compute 8x8 noise: load 30ms. Decompression overhead?
	- graphics to compute 8x8 noise: linear sample 13ms (2.3x).

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|------|----|
	| 1   | 20   | 16.8 | image storage | |
	| 1.4 | 14.5 | 23.1 | 1x1 noise     | |
	| 1.4 | 14.5 | 23.1 | 2x2 noise     | |
	| 1.6 | 12.5 | 26.8 | 4x4 noise     | |
	| 2.2 | 9.0  | 37.2 | 8x8 noise     | **same as block size** |
	| 2.3 | 8.5  | 39.4 | 16x16 noise   | |
	| 2.4 | 8.2  | 40.9 | gradient      | |
	| 2.4 | 8.2  | 40.9 | solid color   | |


* RGBA16_UNorm 16.8MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- expected read: 134MB, write: 33.5MB, total: 168MB per frame.

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|------|----|
	| 1    | 8   | 21   | image storage |
	| 1.07 | 7.5 | 22.4 | 1x1 noise     |
	| 2    | 4   | 42   | 16x16 noise   |

* RGBA16F 16.7MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- expected read: 134MB, write: 33.5MB, total: 168MB per frame.

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|------|----|
	| 1    | 8   | 21   | image storage  |
	| 1.07 | 7.5 | 22.4 | 1x1 noise      |
	| 2    | 4   | 42   | 16x16 noise    |

