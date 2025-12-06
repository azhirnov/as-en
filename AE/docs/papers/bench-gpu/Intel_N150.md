
# Intel N150 (Twin Lake)

## Specs

* CPU Arch: Twin Lake
* GPU: UHD Graphics 730 (Xe-LP ?, Alder Lake-N, Gen 12.1)
* Clock: 1000 MHz
* Execution Units: 24
* warp size: 32 *(16 with dual issue)*
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


## Nonuniform

* __depth pre-pass__ [[14.2](../GPU_Benchmarks.md#14-Nonuniform)]<br/>
	Scale=0.6, ObjCount=4K, Dim=4K, dpp=0.52ms

	| nonuniform              | per object (ms) | per warp (ms) | per quad (ms) | per pixel (ms) |
	|-------------------------|-----------------|---------------|---------------|----------------|
	| texture layer           | 2.7             | 4.4           | 5.6           | 10.9           |
	| texture index           | 3.2             | 3.6           | 8.3           | 17.6           |
	| texture & sampler index | 3.2             | 3.6           | 8.3           | 17.6           |

* __visibility buffer__ [[14.3](../GPU_Benchmarks.md#14-Nonuniform)]<br/>
	visibility buffer build 0.8= ms<br/>
	visibility buffer FS overhead = ms<br/>
	Scale=0.6, ObjCount=4K, Dim=4K

	| nonuniform              | per object (ms) | per warp (ms) | per quad (ms) | per pixel (ms) |
	|-------------------------|-----------------|---------------|---------------|----------------|
	| texture layer           | 2.52            | 2.6           | 2.7           | 3.2            |
	| texture index           | 2.95            | 2.65          | 4.2           | 7.7            |
	| texture & sampler index | 2.91            | 2.65          | 4.2           | 7.7            |
