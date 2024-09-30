
# AMD RX 570 (GCN4)

## Specs

* FP16: **5.095** TFLOPS
* FP32: **5.095** TFLOPS
* FP64: **318.5** GFLOPS
* Memory: 4GB, GDDR5, 256 bit, **224.0** GB/s (86 GB/s from tests)
* Driver: ???


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

