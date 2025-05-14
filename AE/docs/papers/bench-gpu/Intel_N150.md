
# Intel N150 (Twin Lake)

## Specs

* CPU Arch: Twin Lake
* GPU: UHD Graphics 730? (Xe-LP, Alder Lake-N, Gen 12.2)
* Clock: 1000 MHz
* Execution Units: 24
* warp size: 32 *(16 with dual issue)*
* subgroupSize: 8 - 32 [vk]
* Total ALUs: 384
* FP16 GFLOPS: 768
* FP32 GFLOPS: 384
* FP64 GFLOPS:
* Memory: 16GB DDR4-3200


Theoretical performance:
```
FLOPS = clock * EU * warp_size/2
Total ALUs = EU * warp_size/2

1000M * 24 * 16 = 384G ADD ops per second = 384 GFLOPS
1000M * 24 * 16 / 2 = 192G FMA ops per second = 384 GFLOPS  (why FMA takes 2 cycles ???)
```
