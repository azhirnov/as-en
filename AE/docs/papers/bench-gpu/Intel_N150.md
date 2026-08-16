
# Intel N150 (Twin Lake)

## Specs

* CPU Arch: Twin Lake
* GPU: UHD Graphics 730 (Xe-LP ?, Alder Lake-N, Gen 12.1)
* Clock: 1000 MHz
* Execution Units: 24
* warp size: 16 *(8 with dual/quad issue)*
* subgroupSize: 8 - 32 [vk]
* Total ALUs: 384
* FP16 GFLOPS: 768
* FP32 GFLOPS: 384
* FP64 GFLOPS:
* Memory: 16GB DDR4-3200
* [Vulkan features](https://vulkan.gpuinfo.org/displayreport.php?id=39319)

Theoretical performance:
```
FLOPS = clock * EU * warp_size/2
Total ALUs = EU * warp_size/2

1000M * 24 * 16 = 384G ADD ops per second = 384 GFLOPS
1000M * 24 * 16 / 2 = 192G FMA ops per second = 384 GFLOPS  (why FMA takes 2 cycles ???)
```

## Shader

### Instruction cost

* FP32 instruction performance: [[2](../GPU_Benchmarks.md#2-fp32-instruction-performance)]
1.07 Gop
	| GOp/s | ops | max GFLOPS |
	|---|---|---|
	| 353 | Add    |**353**|
	| 192 | Mul    | 192   |
	| 105 | MulAdd | 210   |
	| 108 | FMA    | 216   |

* FP16 instruction performance: [[1](../GPU_Benchmarks.md#1-fp16-instruction-performance)]
1.07Gop
	| GOp/s | ops | max GFLOPS |
	|---|---|---|
	| 748 | Add    |**748**|
	| 359 | Mull   | 359   |
	| 251 | MulAdd | 502   |
	| 251 | FMA    | 502   |


### Tile size

* Subgroup scheduler prefer to put subgroup in tile 4x4 pix (in simd16 and simd8 mode).
  If subgroup is not full then it merged with other non-full subgroups. [[17](../GPU_Benchmarks.md#17-tile-size)]<br/>
* On high register count used simd8 mode, otherwise used simd16 mode.<br/>


### Merged instances

* Instances in VS are merged (in rare cases). [[17](../GPU_Benchmarks.md#17-tile-size)]
* Instances in FS are merged (in rare cases).
* Triangles with same instance are merged in FS.
* VS use simd8 mode.

Dark blue - single instance; light blue - single instance in FS, multiple in VS; red - multiple instances in FS.


## Texture cache

* RGBA8_UNorm texture with random access [[9](../GPU_Benchmarks.md#9-Texture-cache)]
	- Measured cache size: 64K, 4M

	| size (B) | dimension (px) | approx bandwidth (GB/s) | comments |
	|---|---|---|---|
    |  256 |   8x8   | 123   |
    |  512 |  16x8   | 104   |
	|   1K |  16x16  |  94   |
	|   2K |  32x16  |  86   |
	|   4K |  32x32  |  82   |
	|   8K |  64x32  |  80   |
	|  16K |  64x64  |  79   |
	|  32K | 128x64  |  78   |
	|  64K | 128x128 |  31   | L1 ? |
	| 128K | 256x128 |  30   |
	| 256K | 256x256 |  23   |
	| 512K | 512x256 |  21   | RAM speed |
	|   1M | 512x512 |  20   |
	|   2M |  1Kx512 |  20   |
	|   4M |  1Kx1K  |  12   | L2 ? |
	|   8M |  2Kx1K  |   5.5 |
	|  16M |  2Kx2K  |   2.6 |
