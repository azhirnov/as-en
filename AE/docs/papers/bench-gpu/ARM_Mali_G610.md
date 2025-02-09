
# ARM Mali G610 MC6 (Valhall gen3)

## Specs

* Cores: 6
* ALU: 4
* Warp width: 16
* FP32/cy per ALU: 32 (= 2 * warp)
* Total ALUs: 768
* Clock: 950 MHz
* FP16 GFLOPS: 2884 (  on MulAdd from tests)
* FP32 GFLOPS: 1442 (  on FMA from tests)

* L2 cache: 2 MB
* Memory: 8GB, LPDDR5-6400, 3200 MHz, QC 16bit, 51.2 Gbit/s

* Device: Infinix GT 20 Pro (MediaTek Dimensity 8200, Android 14, Driver 32.1.0)

Theoretical performance:
```
FLOPS = clock * warp_width*2 * ALUs * Cores

950M * 16*2 * 4 * 6 = 730G FMA ops per second = 1459 GFLOPS
```

