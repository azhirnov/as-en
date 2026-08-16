SIMD Performance per thread.

Sources:
* [SIMD](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_SIMD.cpp)
* [SIMD with mem read](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_SIMD2.cpp)
* [GEMM on SIMD](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_SimdGEMM.cpp)
* [GEMV on SIMD](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_SimdGEMV.cpp)

Results:
* [AMD Ryzen 9 3900X](#AMD-Ryzen-9-3900X)
* [AMD Phenom II X4 945](#AMD-Phenom-II-X4-945)
* [AMD Ryzen 7 8745HS](#AMD-Ryzen-7-8745HS)
* [Intel i5 8250U](#Intel-i5-8250U)
* [Intel N150](#Intel-N150)
* Intel Ultra 7 255H:
	- [P-core](#Intel-Ultra-7-255H-P-core)
	- [E-Core](#Intel-Ultra-7-255H-E-Core)
	- [LPE-Core](#Intel-Ultra-7-255H-LPE-Core)
	- [all cores, FMA per core](#Intel-Ultra-7-255H-per-core)
* [Apple M1, P-core](#Apple-M1-P-core)
* [Apple M1, E-core](#Apple-M1-E-core)
* [ARM Cortex X1 (Snapdragon 888)](#ARM-Cortex-X1-Snapdragon-888)
* [ARM Cortex A78 (Snapdragon 888)](#ARM-Cortex-A78-Snapdragon-888)
* [ARM Cortex A78 (MediaTek Dimensity 7020)](#ARM-Cortex-A78-MediaTek-Dimensity-7020)
* [ARM Cortex A78 (MediaTek Dimensity 8200 Ultimate)](#ARM-Cortex-A78-MediaTek-Dimensity-8200-Ultimate)
* [ARM Cortex A76 (MediaTek Helio G96)](#ARM-Cortex-A76-MediaTek-Helio-G96)
* [ARM Cortex A55 (MediaTek Helio G96)](#ARM-Cortex-A55-MediaTek-Helio-G96)
* [ARM Cortex A55 (MediaTek Dimensity 7020)](#ARM-Cortex-A55-MediaTek-Dimensity-7020)
* [ARM Cortex A55 (Snapdragon 888)](#ARM-Cortex-A55-Snapdragon-888)
* [ARM Cortex A55 (MediaTek Dimensity 8200 Ultimate)](#ARM-Cortex-A55-MediaTek-Dimensity-8200-Ultimate)
* [ARM Cortex A53 (Samsung Exynos 7870)](#ARM-Cortex-A53-Samsung-Exynos-7870)
* [ARM Cortex A53, P-core (Snapdragon 439)](#ARM-Cortex-A53-P-core-Snapdragon-439)
* [ARM Cortex A53, E-core (Snapdragon 439)](#ARM-Cortex-A53-E-core-Snapdragon-439)
* [Pico 4 Ultra P-core](#Pico-4-Ultra-P-core)
* [Pico 4 Ultra E-core](#Pico-4-Ultra-E-core)
* [Snapdragon 8 Elite gen 5 Prime-core](#Snapdragon-8-Elite-gen-5-Prime-core)
* [Snapdragon 8 Elite gen 5 P-core](#Snapdragon-8-Elite-gen-5-P-core)

# AMD Ryzen 9 3900X

* 4.2 GHz, 7nm (Zen2)
* Notes:
	- 32x fp32 FMA FLOPS/cy (2x FMA op/cy) [ref](https://en.wikichip.org/wiki/amd/microarchitectures/zen_2)
	- Compiling with AVX2 is 1.5 faster for non-simd vector.
* Theoretical performance (FMA):
	- 32 FLOPS * 3.8-4.2 GHz = 121-134 GFLOPS per core
	- 134 GFLOPS * 12 cores  = 1.6 TFLOPS per CPU
	- 2 FLOPS (scalar) * 3.8-4.2 GHz = 7.6 - 8.4 GFLOPS per core
* Theoretical performance (add/mul):
	- 32 FLOPS * 4.2 GHz  = 134 GFLOPS per core
* Theoretical performance (SFU):
	- 8 * 4.2 GHz       = 33.6 GOp/s (Rcp, RSqrt)
	- 1/2 * 8 * 4.2 GHz = 16.8 GOp/s
	- 1/3 * 8 * 4.2 GHz = 11.2 GOp/s (Div)
	- 1/4 * 8 * 4.2 GHz =  8.4 GOp/s (Sqrt)
	- 1/3 (scalar) * 4.2 GHz = 1.4 GOp/s (Div)
	- 1/8 (scalar) * 4.2 GHz = 0.52 GOp/s (Sqrt)

## Float SIMD (single thread)

| | time | FLOPS | SOL |
|----------------------------|-------------|---|
| Simd Float8 - FMA ilp16    | 111.9GFLOPS | 82% |
| Simd Float8 - FMA ilp8     | 104.3GFLOPS |
| Simd Float8 - FMA ilp7     | 92.4GFLOPS  |
| Simd Float8 - MulAdd       | 86.4GFLOPS  |
| Simd Float8 - FMA ilp6     | 79.8GFLOPS  |
| Simd Float8 - FMA ilp5     | 67.4GFLOPS  |
| Simd Float8 - Add ilp7     | 59.7GFLOPS  |
| Simd Float8 - Mul          | 61.5GFLOPS  |
| Simd Float8 - Add ilp8     | 61.4GFLOPS  |
| Simd Float8 - Add ilp6     | 57.0GFLOPS  |
| Simd Float8 - Add ilp5     | 55.1GFLOPS  |
| Simd Float8 - FMA ilp4     | 53.9GFLOPS  |
| Simd Float8 - Add ilp4     | 43.7GFLOPS  |
| Simd Float8 - Fast Sqrt    | 29.3GFLOPS  |
| Simd Float8 - Fast Div     | 24.7GFLOPS  |
| Simd Float8 - Div ilp4     | 9.87GFLOPS  | 89% |
| Simd Float8 - Div ilp8     | 9.72GFLOPS  | 89% |
| Simd Float8 - Precise Sqrt | 6.19GFLOPS  |

## Float SIMD with read (single thread)

| | time | FLOPS | bandwidth | SOL |
|----------------------------|--------------|-----------|---|
| Simd Float8 - FMA ilp8 r4  | 105.5GFLOPS  |  52.8GB/s | 78% |
| Simd Float8 - Add ilp8 r4  | 60.8GFLOPS   |  30.4GB/s | 89% |
| Simd Float8 - Add ilp8 r2  | 60.1GFLOPS   |  15.0GB/s |
| Simd Float8 - FMA ilp8 r8  | 57.3GFLOPS   |  57.3GB/s | 42% - memory bound |
| Simd Float8 - Add ilp8 r8  | 52.1GFLOPS   |  52.1GB/s |
| Simd Float8 - Add ilp4 r2  | 45.5GFLOPS   |  22.7GB/s |
| Simd Float8 - Add ilp4 r8  | 44.6GFLOPS   |  89.1GB/s |
| Simd Float8 - Add ilp4 r4  | 44.5GFLOPS   |  44.5GB/s |
| Simd Float8 - Add ilp8 r16 | 44.4GFLOPS   |  88.9GB/s |
| Simd Float8 - Add ilp4 r16 | 24.6GFLOPS   |  98.4GB/s |

## Float SIMD GEMM

**1024x1024x1024**

|         |   time   | diff  |     FLOPS    | bandwidth  | SOL |
|---------|----------|-------|--------------|------------|---|
| AVX2 v3 | 19.35 ms | -     | 110.9GFLOPS  |  867.0MB/s | 83% |
| AVX2 v2 | 27.90 ms | +44%  | 76.9GFLOPS   |  601.2MB/s |


## Float SIMD (4 threads, 2 cores)

| | time | diff | FLOPS | SOL |
|--------------------------|----------|------|------------|---|
| Simd Float8 - FMA ilp=7  | 0.10 s   | +16% | 68.8GFLOPS | 51% (102% per core) |
| Simd Float8 - FMA ilp=6  | 87.54 ms | -    | 68.5GFLOPS |
| Simd Float8 - FMA ilp=5  | 0.16 s   | +10% | 62.4GFLOPS |
| Simd Float8 - FMA ilp=8  | 0.14 s   | +39% | 56.7GFLOPS |
| Simd Float8 - FMA ilp=4  | 0.15 s   | +3%  | 55.0GFLOPS | 41% |
| Simd Float8 - FMA ilp=16 | 0.44 s   | +50% | 36.2GFLOPS |
| Simd Float8 - FMA + Add  | 0.23 s   | +13% | 35.1GFLOPS |
| Simd Float8 - Add ilp=8  | 0.23 s   | +1%  | 34.7GFLOPS | 52% (104% per core) |
| Simd Float8 - Add ilp=7  | 0.20 s   | +26% | 34.6GFLOPS |
| Simd Float8 - Add ilp=6  | 0.26 s   | +11% | 34.6GFLOPS |
| Simd Float8 - Add ilp=4  | 0.23 s   | +1%  | 34.3GFLOPS |
| Simd Float8 - Add ilp=5  | 0.29 s   | +13% | 34.0GFLOPS |

## Float SIMD (4 threads, 4 cores)

| | time | diff | FLOPS | SOL |
|--------------------------|----------|------|------------|---|
| Simd Float8 - FMA ilp=7  | 79.01 ms | +3%  | 88.6GFLOPS | 66% |
| Simd Float8 - FMA ilp=6  | 76.55 ms | -    | 78.4GFLOPS |
| Simd Float8 - Add ilp=8  | 0.12 s   | +14% | 66.9GFLOPS | 100% |
| Simd Float8 - Add ilp=6  | 0.13 s   | +13% | 66.8GFLOPS |
| Simd Float8 - Add ilp=7  | 0.10 s   | +33% | 66.7GFLOPS |
| Simd Float8 - FMA ilp=5  | 0.15 s   | +5%  | 66.6GFLOPS |
| Simd Float8 - FMA ilp=16 | 0.25 s   | +28% | 64.3GFLOPS |
| Simd Float8 - FMA + Add  | 0.14 s   | +2%  | 58.3GFLOPS |
| Simd Float8 - FMA ilp=8  | 0.14 s   | +4%  | 56.0GFLOPS |
| Simd Float8 - FMA ilp=4  | 0.15 s   | -    | 53.0GFLOPS |
| Simd Float8 - Add ilp=5  | 0.19 s   | +4%  | 51.4GFLOPS |
| Simd Float8 - Add ilp=4  | 0.19 s   | +24% | 42.8GFLOPS | 64% |

## Double SIMD (single thread)

| | time | diff | delta | FLOPS | SOL |
|-----------------------------|----------|--------|-------|------------|---|
| Simd Double4 - FMA ilp=8    | 72.54 ms | +1%    | +1%   | 55.1GFLOPS | 83% |
| Simd Double4 - FMA ilp=7    | 75.75 ms | +5%    | +4%   | 46.2GFLOPS |
| Simd Double4 - MulAdd       | 87.32 ms | +21%   | +15%  | 45.8GFLOPS |
| Simd Double4 - FMA ilp=6    | 72.02 ms | -      | -     | 41.7GFLOPS |
| Simd Double4 - FMA ilp=16   | 0.20 s   | +182%  | +1%   | 39.4GFLOPS |
| Simd Double4 - Mul          | 0.12 s   | +60%   | +14%  | 34.7GFLOPS |
| Simd Double4 - Add ilp=6    | 0.13 s   | +80%   | -     | 34.7GFLOPS |
| Simd Double4 - FMA ilp=5    | 0.14 s   | +100%  | -     | 34.7GFLOPS |
| Simd Double4 - Add ilp=7    | 0.10 s   | +41%   | +16%  | 34.6GFLOPS |
| Simd Double4 - Add ilp=8    | 0.12 s   | +61%   | -     | 34.6GFLOPS |
| Simd Double4 - FMA + Div    | 0.12 s   | +66%   | +3%   | 33.5GFLOPS |
| Simd Double4 - FMA + Add    | 0.13 s   | +80%   | +8%   | 30.9GFLOPS |
| Simd Double4 - FMA ilp=4    | 0.14 s   | +99%   | +10%  | 27.9GFLOPS |
| Simd Double2 - FMA ilp=8    | 0.15 s   | +102%  | +1%   | 27.6GFLOPS |
| Simd Double4 - Add ilp=5    | 0.19 s   | +162%  | +8%   | 26.5GFLOPS |
| Simd Double4 - FMA + Div2   | 0.16 s   | +119%  | +4%   | 25.4GFLOPS |
| Simd Double2 - FMA ilp=7    | 0.15 s   | +110%  | +4%   | 23.1GFLOPS |
| Simd Double4 - Add ilp=4    | 0.17 s   | +140%  | +10%  | 23.2GFLOPS |
| Simd Double2 - MulAdd       | 0.17 s   | +142%  | +1%   | 22.9GFLOPS |
| Simd Double2 - FMA ilp=16   | 0.38 s   | +422%  | +9%   | 21.3GFLOPS |
| Simd Double2 - FMA ilp=6    | 0.14 s   | +99%   | -     | 20.9GFLOPS |
| Simd Double2 - Add ilp=7    | 0.20 s   | +180%  | +7%   | 17.4GFLOPS |
| Simd Double2 - Add ilp=8    | 0.23 s   | +220%  | +14%  | 17.4GFLOPS |
| Simd Double2 - Mul          | 0.23 s   | +220%  | -     | 17.4GFLOPS |
| Simd Double2 - Add ilp=6    | 0.26 s   | +260%  | +1%   | 17.4GFLOPS |
| Simd Double2 - FMA ilp=5    | 0.29 s   | +299%  | -     | 17.4GFLOPS |
| Simd Double2 - FMA + Add    | 0.26 s   | +254%  | +11%  | 15.7GFLOPS |
| Simd Double2 - FMA + Div    | 0.26 s   | +256%  | -     | 15.6GFLOPS |
| Simd Double2 - FMA ilp=4    | 0.29 s   | +298%  | +11%  | 13.9GFLOPS |
| Simd Double2 - FMA + Div2   | 0.30 s   | +317%  | +5%   | 13.3GFLOPS |
| Simd Double2 - Add ilp=5    | 0.38 s   | +424%  | -     | 13.2GFLOPS |
| Simd Double2 - Add ilp=4    | 0.34 s   | +378%  | +15%  | 11.6GFLOPS |
| Simd Double4 - Div ilp=8    | 1.13 s   | +1475% | +201% | 3.53GFLOPS |
| Simd Double4 - Div ilp=4    | 1.13 s   | +1476% | -     | 3.53GFLOPS |
| Simd Double4 - Precise Sqrt | 1.94 s   | +2588% | +71%  | 2.07GFLOPS |
| Simd Double2 - Div ilp=8    | 2.27 s   | +3055% | +17%  | 1.76GFLOPS |
| Simd Double2 - Div ilp=4    | 2.27 s   | +3058% | -     | 1.76GFLOPS |
| Simd Double2 - Precise Sqrt | 3.87 s   | +5271% | +70%  | 1.03GFLOPS |


# AMD Phenom II X4 945

* 3 GHz, 45nm (Deneb, K10)
* Theoretical performance (FMA):
	- 8 FLOPS * 3 GHz = 24 GFLOPS per core
	- 24 GFLOPS * 4 cores = 96 GFLOPS per CPU

## Float SIMD

```
Scalar Float4 - par Add : 4.58 s             - 1.1GFLOPS
Scalar Float4 - MulAdd  : 4.63 s  +0.9%      - 1.1GFLOPS
Scalar Float4 - Mul     : 5.10 s  +11.3%     - 1.0GFLOPS
Scalar Float4 - seq2 Add: 5.16 s  +12.6%     - 1.0GFLOPS
Scalar Float4 - seq1 Add: 5.94 s  +29.6%     - 0.9GFLOPS
Scalar Float4 - Div     : 16.12 s  +251.6%   - 158.8MFLOPS
Scalar Float4 - Sqrt    : 25.09 s  +447.3%   - 102.0MFLOPS

Simd Float4 - MulAdd  : 0.75 s             - 6.8GFLOPS
Simd Float4 - par Add : 0.76 s  +0.7%      - 6.8GFLOPS
Simd Float4 - Mul     : 0.76 s  +1.4%      - 6.7GFLOPS
Simd Float4 - Sqrt    : 0.98 s  +30.1%     - 2.6GFLOPS
Simd Float4 - seq2 Add: 1.26 s  +67.4%     - 4.1GFLOPS
Simd Float4 - seq1 Add: 2.23 s  +196.6%    - 2.3GFLOPS
Simd Float4 - Div     : 3.99 s  +431.0%    - 0.6GFLOPS
```

## Double SIMD

```
Scalar Double2 - MulAdd  : 1.31 s             - 2.0GFLOPS
Scalar Double4 - MulAdd  : 1.47 s  +12.6%     - 1.7GFLOPS
Scalar Double2 - Mul     : 1.79 s  +37.0%     - 1.4GFLOPS
Scalar Double2 - par Add : 1.80 s  +37.4%     - 1.4GFLOPS
Scalar Double2 - seq2 Add: 1.85 s  +41.3%     - 1.4GFLOPS
Scalar Double4 - seq2 Add: 1.99 s  +52.0%     - 1.3GFLOPS
Scalar Double4 - seq1 Add: 2.02 s  +54.7%     - 1.3GFLOPS
Scalar Double4 - Mul     : 2.20 s  +68.0%     - 1.2GFLOPS
Scalar Double4 - par Add : 2.22 s  +70.2%     - 1.2GFLOPS
Scalar Double2 - seq1 Add: 2.65 s  +102.9%    - 1.0GFLOPS
Scalar Double2 - Div     : 9.23 s  +606.5%    - 138.7MFLOPS
Scalar Double4 - Div     : 10.18 s  +679.5%   - 125.7MFLOPS
Scalar Double2 - Sqrt    : 16.12 s  +1134.0%  - 79.4MFLOPS
Scalar Double4 - Sqrt    : 16.21 s  +1141.0%  - 79.0MFLOPS

Simd Double2 - Mul     : 0.75 s             - 3.4GFLOPS
Simd Double2 - par Add : 0.77 s  +2.2%      - 3.3GFLOPS
Simd Double2 - MulAdd  : 0.77 s  +2.3%      - 3.3GFLOPS
Simd Double2 - seq2 Add: 1.22 s  +62.4%     - 2.1GFLOPS
Simd Double2 - seq1 Add: 2.18 s  +190.3%    - 1.2GFLOPS
Simd Double2 - Div     : 4.63 s  +517.0%    - 276.7MFLOPS
Simd Double2 - Sqrt    : 6.25 s  +733.3%    - 204.8MFLOPS
```



# Intel i5 8250U

* 3.39 GHz, 14nm
* Notes:
	- FMA with 2op/cy [ref](https://en.wikichip.org/wiki/intel/microarchitectures/skylake)
* Theoretical performance (FMA):
	- 32 FLOPS * 3.39 GHz = 108.5 GFLOPS per core
	- 108.5 GFLOPS * 4 cores = 434 GFLOPS per CPU
	- 2 FLOPS (scalar) * 3.39 GHz = 6.8 GFLOPS per core

## Float SIMD

```

Scalar Float4 - MulAdd      : 0.16 s             - 5.05GFLOPS (74%)
Scalar Float4 - FMA         : 0.21 s  +32.3%     - 3.82GFLOPS
Scalar Float4 - Add         : 0.21 s  +33.6%     - 3.78GFLOPS
Scalar Float4 - Div         : 0.71 s  +348.9%    - 1.13GFLOPS
Scalar Float4 - Precise Sqrt: 1.98 s  +1150.0%   - 404.1MFLOPS
Scalar Float4 - Fast Sqrt   : 2.16 s  +1266.9%   - 369.5MFLOPS
Scalar Float4 - Mul         : 10.07 s  +6259.3%  - 79.4MFLOPS    ???

Simd Float8 - FMA         : 9.47 ms            - 84.5GFLOPS  (78%)
Simd Float8 - MulAdd      : 18.31 ms  +93.4%   - 43.7GFLOPS
Simd Float4 - FMA         : 18.90 ms  +99.5%   - 42.3GFLOPS
Simd Float8 - Add         : 20.87 ms  +120.4%  - 38.3GFLOPS
Simd Float8 - Fast Sqrt   : 32.38 ms  +242.0%  - 24.7GFLOPS
Simd Float4 - MulAdd      : 34.43 ms  +263.6%  - 23.2GFLOPS
Simd Float4 - Add         : 37.05 ms  +291.3%  - 21.6GFLOPS
Simd Float4 - Fast Sqrt   : 64.91 ms  +585.4%  - 12.3GFLOPS
Simd Float8 - Div         : 0.15 s  +1461.5%   - 5.41GFLOPS
Simd Float8 - Precise Sqrt: 0.18 s  +1772.9%   - 4.51GFLOPS
Simd Float4 - Div         : 0.18 s  +1776.0%   - 4.50GFLOPS
Simd Float4 - Precise Sqrt: 0.18 s  +1776.5%   - 4.50GFLOPS
Simd Float8 - Mul         : 2.04 s  +21438.6%  - 392.2MFLOPS    ???
Simd Float4 - Mul         : 3.25 s  +34217.1%  - 246.2MFLOPS    ???
```

## Double SIMD

```
Scalar Double2 - MulAdd      : 59.20 ms           - 6.76GFLOPS
Scalar Double2 - Add         : 61.55 ms  +4.0%    - 6.50GFLOPS
Scalar Double2 - FMA         : 66.16 ms  +11.7%   - 6.05GFLOPS
Scalar Double4 - MulAdd      : 0.19 s  +223.3%    - 2.09GFLOPS
Scalar Double4 - Add         : 0.19 s  +225.6%    - 2.07GFLOPS
Scalar Double4 - FMA         : 0.36 s  +508.5%    - 1.11GFLOPS
Scalar Double2 - Div         : 0.47 s  +698.9%    - 845.7MFLOPS
Scalar Double4 - Div         : 0.47 s  +700.0%    - 844.6MFLOPS
Scalar Double2 - Fast Sqrt   : 1.65 s  +2684.8%   - 242.6MFLOPS
Scalar Double4 - Fast Sqrt   : 1.84 s  +3002.2%   - 217.8MFLOPS
Scalar Double2 - Precise Sqrt: 2.13 s  +3497.4%   - 187.8MFLOPS
Scalar Double4 - Precise Sqrt: 2.13 s  +3502.6%   - 187.5MFLOPS
Scalar Double2 - Mul         : 3.27 s  +5416.1%   - 122.5MFLOPS    ???
Scalar Double4 - Mul         : 5.15 s  +8592.2%   - 77.7MFLOPS     ???

Simd Double4 - FMA         : 9.29 ms             - 43.1GFLOPS
Simd Double2 - FMA         : 18.24 ms  +96.3%    - 21.9GFLOPS
Simd Double4 - MulAdd      : 18.54 ms  +99.6%    - 21.6GFLOPS
Simd Double4 - Add         : 21.20 ms  +128.2%   - 18.9GFLOPS
Simd Double2 - MulAdd      : 34.53 ms  +271.7%   - 11.6GFLOPS
Simd Double2 - Add         : 37.38 ms  +302.3%   - 10.7GFLOPS
Simd Double2 - Div         : 0.24 s  +2445.9%    - 1.69GFLOPS
Simd Double4 - Div         : 0.24 s  +2448.7%    - 1.69GFLOPS
Simd Double4 - Precise Sqrt: 0.36 s  +3722.3%    - 1.13GFLOPS
Simd Double2 - Precise Sqrt: 0.36 s  +3725.8%    - 1.13GFLOPS
Simd Double4 - Mul         : 0.83 s  +8857.2%    - 480.7MFLOPS    ???
Simd Double2 - Mul         : 0.84 s  +8913.7%    - 477.6MFLOPS    ???
```


# ARM Cortex X1 (Snapdragon 888)

* Kryo 680 Prime, 2841 MHz, 5nm, Asus ROG Phone 5
* Notes:
	- 4x128b NEON ??? [ref](https://www.anandtech.com/show/15813/arm-cortex-a78-cortex-x1-cpu-ip-diverging/3)
* Theoretical performance (FMA):
	- 16 FLOPS * 2.8 GHz = 44.8 GFLOPS per core

## Float SIMD (single thread)

| | FLOPS | SOL |
|-------------------------------|----------------|
| Simd Float4 - FMA ilp8        | 31.4GFLOPS     |
| Simd Float4 - FMA ilp7        | 28.8GFLOPS     |
| Simd Float4 - FMA ilp6        | 25.6GFLOPS     |
| Simd Float4 - FMA ilp16       | 25.0GFLOPS     |
| Simd Float4 - FMA ilp5        | 23.3GFLOPS     |
| Simd Float4 - FMA ilp4        | 18.9GFLOPS     |
| Simd Float4 - Fast Sqrt       | 7.63GFLOPS     |
| Simd Float4 - Fast Div        | 6.70GFLOPS     |
| Simd Float4 - Precise Sqrt    | 2.21GFLOPS     |

## Float SIMD GEMM

**1024x1024x1024**

|         |  time    | diff  |    FLOPS    |  bandwidth | SOL |
|---------|----------|-------|-------------|------------|---|
| NEON v1 | 39.78 ms | -     | 54.0GFLOPS  |  421.7MB/s |
| AE GEMM | 47.28 ms | +19%  | 45.4GFLOPS  |  354.8MB/s |
| NEON v3 | 0.10 s   | +159% | 20.8GFLOPS  |  162.9MB/s |

**128x128x128**

|         |  time    | diff |    FLOPS    | bandwidth | SOL |
|---------|----------|------|-------------|-----------|---|
| AE GEMM | 86.66 us | -    | 48.2GFLOPS  |  3.03GB/s |
| NEON v1 | 87.26 us | +1%  | 47.9GFLOPS  |  3.00GB/s |
| NEON v3 | 0.12 ms  | +35% | 35.8GFLOPS  |  2.24GB/s |

## Float SIMD GEMV

**2048x2048**

|         |  time   | diff |   FLOPS     | bandwidth | SOL |
|---------|---------|------|-------------|-----------|---|
| NEON v1 | 0.74 ms | -    | 11.3GFLOPS  |  22.6GB/s |
| NEON v3 | 0.78 ms | +5%  | 10.7GFLOPS  |  21.5GB/s |
| NEON v4 | 0.83 ms | +12% | 10.1GFLOPS  |  20.2GB/s |
| NEON v2 | 0.84 ms | +13% | 10.0GFLOPS  |  20.0GB/s |

**256x256**

|         |  time    | diff |    FLOPS    | bandwidth | SOL |
|---------|----------|------|-------------|-----------|---|
| NEON v3 | 6.11 us  | -    | 21.5GFLOPS  |  43.3GB/s |
| NEON v1 | 6.65 us  | +9%  | 19.7GFLOPS  |  39.7GB/s |
| NEON v4 | 6.81 us  | +12% | 19.2GFLOPS  |  38.8GB/s |
| NEON v2 | 11.63 us | +90% | 11.3GFLOPS  |  22.7GB/s |


# ARM Cortex A78 (Snapdragon 888)

* Kryo 680 Gold, 2419 MHz, 5nm, Asus ROG Phone 5
* Notes:
	- 2x128b NEON [ref](https://www.anandtech.com/show/15813/arm-cortex-a78-cortex-x1-cpu-ip-diverging/3)
* Theoretical performance (FMA):
	- 16 FLOPS * 2.4 GHz = 38.4 GFLOPS per core

## Float SIMD (single thread)

| | FLOPS | SOL |
|-------------------------------|----------------|
| Simd Float4 - FMA ilp4        | 19.2GFLOPS     |
| Simd Float4 - FMA ilp5        | 16.9GFLOPS     |
| Simd Float4 - FMA ilp16       | 16.9GFLOPS     |
| Simd Float4 - FMA ilp8        | 15.9GFLOPS     |
| Simd Float4 - FMA ilp6        | 15.5GFLOPS     |
| Simd Float4 - FMA ilp7        | 15.4GFLOPS     |
| Simd Float4 - MulAdd          | 15.5GFLOPS     |
| Simd Float4 - Fast Sqrt       | 4.80GFLOPS     |
| Simd Float4 - Fast Div        | 4.79GFLOPS     |
| Simd Float4 - Precise Sqrt    | 1.37GFLOPS     |

## Float SIMD (3 threads, 3 cores)

| | FLOPS per thread | SOL |
|------------------------------|------------|
| Simd Float4 - FMA ilp4       | 18.8GFLOPS |
| Simd Float4 - FMA ilp5       | 16.5GFLOPS |
| Simd Float4 - FMA ilp16      | 16.5GFLOPS |
| Simd Float4 - FMA ilp8       | 15.6GFLOPS |
| Simd Float4 - FMA ilp6       | 15.3GFLOPS |
| Simd Float4 - FMA ilp7       | 15.2GFLOPS |

## Float SIMD GEMM

**1024x1024x1024**

|         |  time    | diff |    FLOPS    | bandwidth  | SOL |
|---------|----------|------|-------------|------------|---|
| NEON v1 | 61.91 ms | -    | 34.7GFLOPS  |  271.0MB/s |
| AE GEMM | 61.97 ms | -    | 34.6GFLOPS  |  270.7MB/s |
| NEON v3 | 0.11 s   | +82% | 19.0GFLOPS  |  148.6MB/s |

**128x128x128**

|         |  time   | diff |    FLOPS    | bandwidth | SOL |
|---------|---------|------|-------------|-----------|---|
| NEON v1 | 0.12 ms | -    | 35.4GFLOPS  |  2.22GB/s |
| AE GEMM | 0.12 ms | -    | 35.3GFLOPS  |  2.21GB/s |
| NEON v3 | 0.20 ms | +65% | 21.4GFLOPS  |  1.34GB/s |

## Float SIMD GEMV

**2048x2048**

|         |  time   | diff |   FLOPS    | bandwidth | SOL |
|---------|---------|-----|-------------|-----------|---|
| NEON v3 | 0.95 ms | -   | 8.83GFLOPS  |  17.7GB/s |
| NEON v2 | 0.95 ms | -   | 8.81GFLOPS  |  17.6GB/s |
| NEON v1 | 0.95 ms | -   | 8.80GFLOPS  |  17.6GB/s |
| NEON v4 | 0.97 ms | +2% | 8.68GFLOPS  |  17.4GB/s |

**256x256**

|         |  time    | diff |    FLOPS    | bandwidth | SOL |
|---------|----------|------|-------------|-----------|---|
| NEON v1 | 7.53 us  | -    | 17.4GFLOPS  |  35.1GB/s |
| NEON v4 | 7.84 us  | +4%  | 16.7GFLOPS  |  33.7GB/s |
| NEON v3 | 8.15 us  | +8%  | 16.1GFLOPS  |  32.4GB/s |
| NEON v2 | 12.21 us | +62% | 10.7GFLOPS  |  21.6GB/s |


# ARM Cortex A78 (MediaTek Dimensity 7020)

* 2200 MHz, 6nm, Motorola G54
* Notes:
	- 2x128b NEON [ref](https://www.anandtech.com/show/15813/arm-cortex-a78-cortex-x1-cpu-ip-diverging/3)
* Theoretical performance (FMA):
	- 16 FLOPS * 2.2 GHz = 35.2 GFLOPS per core

## Float SIMD

```
Scalar Float4 - seq2 Add: 0.15 s             - 17.5GFLOPS
Scalar Float4 - Mul     : 0.21 s  +43.6%     - 12.2GFLOPS
Scalar Float4 - par Add : 0.22 s  +53.1%     - 11.4GFLOPS
Scalar Float4 - seq1 Add: 0.29 s  +100.0%    - 8.7GFLOPS
Scalar Float4 - FMA     : 0.44 s  +200.8%    - 5.8GFLOPS
Scalar Float4 - MulAdd  : 0.44 s  +200.8%    - 5.8GFLOPS
Scalar Float4 - Div     : 1.08 s  +640.7%    - 1.2GFLOPS
Scalar Float4 - Sqrt    : 1.13 s  +673.9%    - 1.1GFLOPS

Simd Float4 - FMA     : 0.11 s             - 23.2GFLOPS
Simd Float4 - Mul     : 0.15 s  +31.7%     - 17.6GFLOPS
Simd Float4 - seq2 Add: 0.15 s  +32.6%     - 17.5GFLOPS
Simd Float4 - par Add : 0.15 s  +33.2%     - 17.4GFLOPS
Simd Float4 - seq1 Add: 0.29 s  +165.3%    - 8.7GFLOPS
Simd Float4 - MulAdd  : 0.29 s  +166.6%    - 8.7GFLOPS
Simd Float4 - Sqrt    : 0.43 s  +290.5%    - 3.0GFLOPS
Simd Float4 - Div     : 0.88 s  +694.8%    - 1.5GFLOPS
```

## Double SIMD

```
Scalar Double4 - seq1 Add: 0.15 s  +55.7%     - 8.8GFLOPS
Scalar Double2 - seq2 Add: 0.15 s  +55.8%     - 8.8GFLOPS
Scalar Double4 - Mul     : 0.16 s  +75.4%     - 7.8GFLOPS
Scalar Double2 - Mul     : 0.17 s  +85.3%     - 7.4GFLOPS
Scalar Double2 - par Add : 0.18 s  +94.7%     - 7.0GFLOPS
Scalar Double4 - par Add : 0.18 s  +94.8%     - 7.0GFLOPS
Scalar Double2 - seq1 Add: 0.29 s  +212.1%    - 4.4GFLOPS
Scalar Double2 - MulAdd  : 0.30 s  +214.3%    - 4.3GFLOPS
Scalar Double2 - FMA     : 0.30 s  +214.4%    - 4.3GFLOPS
Scalar Double4 - MulAdd  : 0.32 s  +236.2%    - 4.1GFLOPS
Scalar Double4 - FMA     : 0.32 s  +236.5%    - 4.1GFLOPS
Scalar Double2 - Div     : 1.17 s  +1144.7%   - 0.5GFLOPS
Scalar Double4 - Div     : 1.19 s  +1164.3%   - 0.5GFLOPS
Scalar Double2 - Sqrt    : 2.04 s  +2077.2%   - 313.1MFLOPS
Scalar Double4 - Sqrt    : 2.06 s  +2097.1%   - 310.3MFLOPS

Simd Double2 - FMA     : 0.11 s             - 11.6GFLOPS
Simd Double2 - Mul     : 0.15 s  +31.9%     - 8.8GFLOPS
Simd Double2 - seq2 Add: 0.15 s  +32.5%     - 8.8GFLOPS
Simd Double2 - par Add : 0.15 s  +33.2%     - 8.7GFLOPS
Simd Double2 - seq1 Add: 0.29 s  +165.3%    - 4.4GFLOPS
Simd Double2 - MulAdd  : 0.30 s  +167.8%    - 4.3GFLOPS
Simd Double2 - Sqrt    : 0.34 s  +205.1%    - 1.9GFLOPS
Simd Double2 - Div     : 1.17 s  +959.6%    - 0.5GFLOPS
```

# ARM Cortex A78 (MediaTek Dimensity 8200 Ultimate)

## High performance core

* Clock: 3.1 GHz
* Theoretical performance (FMA):
	- 2x 128bit FMA pipes
	- 16 FLOPS * 3.1 GHz =

### Float SIMD

| | FLOPS | SOL |
|----------------------------|------------|
| Simd Float4 - Add ilp5     | 99.5GFLOPS |
| Simd Float4 - Add ilp7     | 99.5GFLOPS |
| Simd Float4 - Add ilp4     | 99.3GFLOPS |
| Simd Float4 - Add ilp6     | 99.3GFLOPS |
| Simd Float4 - Mul          | 99.2GFLOPS |
| Simd Float4 - Div ilp4     | 33.1GFLOPS |
| Simd Float4 - Add ilp8     | 24.9GFLOPS |
| Simd Float4 - FMA ilp4     | 24.8GFLOPS |
| Simd Float4 - Div ilp8     | 24.8GFLOPS |
| Simd Float4 - FMA ilp5     | 21.8GFLOPS |
| Simd Float4 - FMA ilp16    | 21.8GFLOPS |
| Simd Float4 - FMA ilp8     | 21.2GFLOPS |
| Simd Float4 - MulAdd       | 21.1GFLOPS |
| Simd Float4 - FMA ilp6     | 20.3GFLOPS |
| Simd Float4 - FMA ilp7     | 20.1GFLOPS |
| Simd Float4 - Fast Div     | 6.22GFLOPS |
| Simd Float4 - Fast Sqrt    | 6.21GFLOPS |
| Simd Float4 - Precise Sqrt | 1.77GFLOPS |

### Float SIMD GEMM

**1024x1024x1024**

|         |  time    | diff  |    FLOPS    |  bandwidth | SOL |
|---------|----------|-------|-------------|------------|---|
| NEON v1 | 50.18 ms | -     | 42.8GFLOPS  |  334.3MB/s |
| AE GEMM | 51.71 ms | +3%   | 41.5GFLOPS  |  324.5MB/s |
| NEON v3 | 0.14 s   | +179% | 15.3GFLOPS  |  119.8MB/s |

**128x128x128**

|         |  time    | diff |    FLOPS    | bandwidth | SOL |
|---------|----------|------|-------------|-----------|---|
| AE GEMM | 89.86 us | -    | 46.5GFLOPS  |  2.92GB/s |
| NEON v1 | 89.93 us | -    | 46.5GFLOPS  |  2.92GB/s |
| NEON v3 | 0.15 ms  | +69% | 27.5GFLOPS  |  1.73GB/s |

### Float SIMD GEMV

**2048x2048**

|         |  time   | diff |   FLOPS     | bandwidth | SOL |
|---------|---------|------|-------------|-----------|---|
| NEON v2 | 0.74 ms | -    | 11.4GFLOPS  |  22.8GB/s |
| NEON v3 | 0.82 ms | +10% | 10.3GFLOPS  |  20.6GB/s |
| NEON v4 | 0.83 ms | +13% | 10.1GFLOPS  |  20.2GB/s |
| NEON v1 | 0.86 ms | +17% | 9.72GFLOPS  |  19.5GB/s |

**256x256**

|         |  time   | diff |    FLOPS    | bandwidth | SOL |
|---------|---------|------|-------------|-----------|---|
| NEON v1 | 5.95 us | -    | 22.0GFLOPS  |  44.4GB/s |
| NEON v4 | 6.23 us | +5%  | 21.0GFLOPS  |  42.4GB/s |
| NEON v3 | 6.35 us | +7%  | 20.7GFLOPS  |  41.6GB/s |
| NEON v2 | 9.33 us | +57% | 14.0GFLOPS  |  28.3GB/s |


## Performance core

* Clock: 3.0 GHz
* Theoretical performance (FMA):
	- 2x 128bit FMA pipes
	- 16 FLOPS * 3.0 GHz =

### Float SIMD

| | FLOPS | SOL |
|----------------------------|------------|
| Simd Float4 - Add ilp4     | 84.1GFLOPS |
| Simd Float4 - Add ilp5     | 81.6GFLOPS |
| Simd Float4 - Mul          | 81.3GFLOPS |
| Simd Float4 - Add ilp6     | 81.2GFLOPS |
| Simd Float4 - Add ilp7     | 79.5GFLOPS |
| Simd Float4 - Div ilp4     | 27.1GFLOPS |
| Simd Float4 - Div ilp8     | 20.6GFLOPS |
| Simd Float4 - FMA ilp4     | 20.5GFLOPS |
| Simd Float4 - Add ilp8     | 20.0GFLOPS |
| Simd Float4 - FMA ilp5     | 17.9GFLOPS |
| Simd Float4 - FMA ilp16    | 17.8GFLOPS |
| Simd Float4 - MulAdd       | 17.5GFLOPS |
| Simd Float4 - FMA ilp8     | 17.5GFLOPS |
| Simd Float4 - FMA ilp6     | 16.9GFLOPS |
| Simd Float4 - FMA ilp7     | 16.7GFLOPS |
| Simd Float4 - Fast Div     | 5.18GFLOPS |
| Simd Float4 - Fast Sqrt    | 5.12GFLOPS |
| Simd Float4 - Precise Sqrt | 1.47GFLOPS |

### Float SIMD GEMM

**1024x1024x1024**

|         |  time    | diff  |    FLOPS    | bandwidth  | SOL |
|---------|----------|-------|-------------|------------|---|
| AE GEMM | 67.41 ms | -     | 31.8GFLOPS  |  248.9MB/s |
| NEON v1 | 68.22 ms | +1%   | 31.5GFLOPS  |  245.9MB/s |
| NEON v3 | 0.18 s   | +168% | 11.9GFLOPS  |  92.7MB/s  |

**128x128x128**

|         |  time   | diff |    FLOPS    | bandwidth | SOL |
|---------|---------|------|-------------|-----------|---|
| NEON v1 | 0.12 ms | -    | 35.0GFLOPS  |  2.20GB/s |
| AE GEMM | 0.12 ms | -    | 35.0GFLOPS  |  2.19GB/s |
| NEON v3 | 0.19 ms | +62% | 21.6GFLOPS  |  1.35GB/s |

### Float SIMD GEMV

**2048x2048**

|         |  time   | diff |   FLOPS     | bandwidth | SOL |
|---------|---------|------|-------------|-----------|
| NEON v2 | 0.92 ms | -    | 9.10GFLOPS  |  18.2GB/s |
| NEON v1 | 1.21 ms | +31% | 6.96GFLOPS  |  13.9GB/s |
| NEON v4 | 1.21 ms | +32% | 6.91GFLOPS  |  13.8GB/s |
| NEON v3 | 1.23 ms | +33% | 6.83GFLOPS  |  13.7GB/s |

**256x256**

|         |  time    | diff |    FLOPS    | bandwidth | SOL |
|---------|----------|------|-------------|-----------|
| NEON v4 | 8.04 us  | -    | 16.3GFLOPS  |  32.9GB/s |
| NEON v1 | 8.31 us  | +3%  | 15.8GFLOPS  |  31.8GB/s |
| NEON v3 | 8.79 us  | +9%  | 14.9GFLOPS  |  30.1GB/s |
| NEON v2 | 12.21 us | +52% | 10.7GFLOPS  |  21.6GB/s |


# ARM Cortex A76 (MediaTek Helio G96)

* 2050 MHz, 12nm, Realme 8i
* Notes:
	- 2x128b NEON ?
* Theoretical performance (FMA):
	- 16 FLOPS * 2 GHz = 32 GFLOPS per core

## Float SIMD

| | FLOPS | SOL |
|----------------------------|------------|---|
| Simd Float4 - Add ilp4     | 65.3GFLOPS | incorrect? |
| Simd Float4 - Add ilp6     | 65.1GFLOPS |
| Simd Float4 - Add ilp5     | 65.0GFLOPS |
| Simd Float4 - Add ilp7     | 64.8GFLOPS |
| Simd Float4 - Mul          | 36.1GFLOPS |
| Simd Float4 - Div ilp4     | 21.6GFLOPS | incorrect? |
| Simd Float4 - FMA ilp4     | 16.3GFLOPS |
| Simd Float4 - Div ilp8     | 16.3GFLOPS | incorrect? |
| Simd Float4 - Add ilp8     | 16.2GFLOPS |
| Simd Float4 - FMA ilp5     | 13.7GFLOPS |
| Simd Float4 - FMA ilp16    | 13.7GFLOPS |
| Simd Float4 - FMA ilp6     | 13.6GFLOPS |
| Simd Float4 - FMA ilp8     | 13.6GFLOPS |
| Simd Float4 - MulAdd       | 13.5GFLOPS |
| Simd Float4 - FMA ilp7     | 13.5GFLOPS |
| Simd Float4 - Fast Sqrt    | 3.99GFLOPS |
| Simd Float4 - Fast Div     | 3.95GFLOPS |
| Simd Float4 - Precise Sqrt | 1.16GFLOPS |

## Float SIMD GEMM

**1024x1024x1024**

|         |  time  | diff  |     FLOPS   | bandwidth | SOL |
|---------|--------|-------|-------------|-----------|---|
| NEON v1 | 0.16 s | -     | 13.0GFLOPS  |  102.0MB/s |
| NEON v3 | 1.13 s | +589% | 1.89GFLOPS  |  14.8MB/s  |

**128x128x128**

|         |  time  | diff  |     FLOPS   | bandwidth | SOL |
|---------|--------|-------|-------------|-----------|---|
| NEON v1 | 0.14 ms | -    | 29.0GFLOPS  |  1.82GB/s |
| NEON v3 | 0.24 ms | +68% | 17.2GFLOPS  |  1.08GB/s |

## Float SIMD GEMV

**2048x2048**

|         |  time    | diff  |   FLOPS      | bandwidth | SOL |
|---------|----------|-------|--------------|-----------|
| NEON v3 | 2.02 ms  | -     | 4.16GFLOPS   |  8.32GB/s |
| NEON v4 | 2.35 ms  | +30%  | 3.57GFLOPS   |  7.16GB/s |
| NEON v2 | 5.00 ms  | +148% | 1.68GFLOPS   |  3.36GB/s |
| NEON v1 | 10.63 ms | +427% | 789.2MFLOPS  |  1.58GB/s |

**256x256**

|         |  time    | diff |    FLOPS    | bandwidth | SOL |
|---------|----------|------|-------------|-----------|
| NEON v3 | 11.10 us | -    | 11.8GFLOPS  |  23.8GB/s |
| NEON v1 | 11.93 us | +7%  | 11.0GFLOPS  |  22.1GB/s |
| NEON v4 | 13.86 us | +33% | 9.45GFLOPS  |  19.1GB/s |
| NEON v2 | 16.84 us | +52% | 7.78GFLOPS  |  15.7GB/s |

## Double SIMD

```
Scalar Double4 - seq1 Add   : 26.83 ms  +9974866.9% - 23.9GFLOPS,  23.9GB/s
Scalar Double4 - seq2 Add   : 33.05 ms  +12287260.6% - 19.4GFLOPS,  19.4GB/s
Scalar Double2 - seq1 Add   : 44.17 ms  +16420486.2% - 14.5GFLOPS,  14.5GB/s
Scalar Double4 - Add        : 45.68 ms  +16982542.0% - 14.0GFLOPS,  14.0GB/s
Scalar Double4 - seq1 Div   : 45.91 ms  +17066125.3% - 7.0GFLOPS,  13.9GB/s
Scalar Double4 - Mul        : 46.04 ms  +17113837.5% - 13.9GFLOPS,  13.9GB/s
Scalar Double2 - Mul        : 46.67 ms  +17347692.2% - 13.7GFLOPS,  13.7GB/s
Scalar Double2 - seq1 MulAdd: 48.73 ms  +18115447.6% - 13.1GFLOPS,  13.1GB/s
Scalar Double4 - seq1 MulAdd: 49.74 ms  +18492273.2% - 12.9GFLOPS,  12.9GB/s
Scalar Double2 - Add        : 51.80 ms  +19255127.1% - 12.4GFLOPS,  12.4GB/s
Scalar Double2 - seq1 Div   : 67.51 ms  +25096754.3% - 4.7GFLOPS,  9.5GB/s
Scalar Double2 - FMA        : 79.33 ms  +29489522.3% - 8.1GFLOPS,  8.1GB/s
Scalar Double2 - MulAdd     : 79.70 ms  +29630005.6% - 8.0GFLOPS,  8.0GB/s
Scalar Double4 - FMA        : 80.22 ms  +29822390.7% - 8.0GFLOPS,  8.0GB/s
Scalar Double4 - MulAdd     : 85.96 ms  +31954209.3% - 7.4GFLOPS,  7.4GB/s
Scalar Double2 - AddMul     : 86.41 ms  +32123139.8% - 7.4GFLOPS,  7.4GB/s
Scalar Double2 - AddMul2    : 86.43 ms  +32130337.5% - 7.4GFLOPS,  7.4GB/s
Scalar Double4 - AddMul2    : 87.06 ms  +32364775.5% - 7.4GFLOPS,  7.4GB/s
Scalar Double4 - AddMul     : 87.27 ms  +32443911.9% - 7.3GFLOPS,  7.3GB/s
Scalar Double2 - Div        : 0.65 s  +240907843.9% - 493.8MFLOPS,  1.0GB/s
Scalar Double4 - Div        : 0.83 s  +309185968.0% - 384.7MFLOPS,  0.8GB/s
Scalar Double2 - Sqrt       : 2.54 s  +945067371.7% - 125.9MFLOPS,  251.7MB/s
Scalar Double4 - Sqrt       : 3.18 s  +1182104317.8% - 100.6MFLOPS,  201.3MB/s

Simd Double2 - seq1 Add   : 45.07 ms             - 14.2GFLOPS,  14.2GB/s
Simd Double2 - Mul        : 46.56 ms  +3.3%      - 13.7GFLOPS,  13.7GB/s
Simd Double2 - seq1 MulAdd: 48.34 ms  +7.3%      - 13.2GFLOPS,  13.2GB/s
Simd Double2 - Add        : 52.62 ms  +16.7%     - 12.2GFLOPS,  12.2GB/s
Simd Double2 - seq2 Add   : 52.96 ms  +17.5%     - 12.1GFLOPS,  12.1GB/s
Simd Double2 - seq1 Div   : 55.77 ms  +23.7%     - 5.7GFLOPS,  11.5GB/s
Simd Double2 - FMA        : 58.49 ms  +29.8%     - 10.9GFLOPS,  10.9GB/s
Simd Double2 - MulAdd     : 79.31 ms  +76.0%     - 8.1GFLOPS,  8.1GB/s
Simd Double2 - AddMul2    : 84.14 ms  +86.7%     - 7.6GFLOPS,  7.6GB/s
Simd Double2 - AddMul     : 85.04 ms  +88.7%     - 7.5GFLOPS,  7.5GB/s
Simd Double2 - Sqrt       : 0.16 s  +256.3%    - 2.0GFLOPS,  4.0GB/s
Simd Double2 - Div        : 0.65 s  +1339.2%   - 493.3MFLOPS,  1.0GB/s
```


# ARM Cortex A55 (MediaTek Helio G96)

* 2000 MHz, 12nm, Realme 8i
* Theoretical performance (FMA):
	- 1x fp32 128-bit FMA pipe
	- 8 FLOPS * 2 GHz = 16 GFLOPS per core

## Float SIMD

| | FLOPS | SOL |
|----------------------------|------------|---|
| Simd Float4 - Add ilp4     | 31.3GFLOPS  |
| Simd Float4 - Add ilp5     | 31.3GFLOPS  |
| Simd Float4 - Add ilp6     | 31.3GFLOPS  |
| Simd Float4 - Add ilp7     | 31.3GFLOPS  |
| Simd Float4 - Mul          | 29.5GFLOPS  |
| Simd Float4 - Div ilp4     | 15.9GFLOPS  | incorrect? |
| Simd Float4 - Div ilp8     | 7.92GFLOPS  | incorrect? |
| Simd Float4 - FMA ilp7     | 7.91GFLOPS  |
| Simd Float4 - FMA ilp6     | 7.84GFLOPS  |
| Simd Float4 - MulAdd       | 7.82GFLOPS  |
| Simd Float4 - FMA ilp4     | 7.81GFLOPS  |
| Simd Float4 - FMA ilp5     | 7.77GFLOPS  |
| Simd Float4 - FMA ilp8     | 7.76GFLOPS  |
| Simd Float4 - Add ilp8     | 7.57GFLOPS  |
| Simd Float4 - FMA ilp16    | 7.22GFLOPS  |
| Simd Float4 - Fast Div     | 3.86GFLOPS  |
| Simd Float4 - Fast Sqrt    | 3.08GFLOPS  |
| Simd Float4 - Precise Sqrt | 873.8MFLOPS |

## Float SIMD GEMM

**1024x1024x1024**

|         |  time  | diff  |     FLOPS   | bandwidth | SOL |
|---------|--------|-------|-------------|-----------|---|
| NEON v1 | 0.35 s | -     | 6.15GFLOPS   |  48.1MB/s |
| NEON v3 | 3.07 s | +778% | 700.3MFLOPS  |  5.47MB/s |

**128x128x128**

|         |  time  | diff  |     FLOPS   | bandwidth | SOL |
|---------|--------|-------|-------------|-----------|---|
| NEON v1 | 0.49 ms | -    | 8.44GFLOPS  |  529.6MB/s |
| NEON v3 | 0.54 ms | +10% | 7.70GFLOPS  |  483.2MB/s |

## Float SIMD GEMV

**2048x2048**

|         |   time   |  diff |     FLOPS    |  bandwidth | SOL |
|---------|----------|-------|--------------|------------|---|
| NEON v2 | 5.93 ms  | -     | 1.41GFLOPS   |  2.83GB/s  |
| NEON v4 | 12.64 ms | +100% | 663.5MFLOPS  |  1.33GB/s  |
| NEON v3 | 31.12 ms | +424% | 269.6MFLOPS  |  539.7MB/s |
| NEON v1 | 46.30 ms | +680% | 181.2MFLOPS  |  362.7MB/s |

**256x256**

|         |  time    | diff |    FLOPS    | bandwidth | SOL |
|---------|----------|------|-------------|-----------|---|
| NEON v4 | 60.34 us | -    | 2.17GFLOPS  |  4.38GB/s |
| NEON v3 | 64.20 us | +6%  | 2.04GFLOPS  |  4.12GB/s |
| NEON v2 | 70.75 us | +17% | 1.85GFLOPS  |  3.73GB/s |
| NEON v1 | 90.83 us | +51% | 1.44GFLOPS  |  2.91GB/s |


## Double SIMD

```
Scalar Double4 - seq1 Add   : 76.84 ms  +22734062.4% - 8.3GFLOPS,  8.3GB/s
Scalar Double4 - seq2 Add   : 84.20 ms  +24911247.0% - 7.6GFLOPS,  7.6GB/s
Scalar Double4 - seq1 MulAdd: 0.11 s  +32550082.0% - 5.8GFLOPS,  5.8GB/s
Scalar Double2 - seq1 MulAdd: 0.12 s  +36764137.6% - 5.2GFLOPS,  5.2GB/s
Scalar Double2 - Add        : 0.13 s  +37131732.0% - 5.1GFLOPS,  5.1GB/s
Scalar Double4 - Mul        : 0.14 s  +40447123.4% - 4.7GFLOPS,  4.7GB/s
Scalar Double4 - seq1 Div   : 0.14 s  +41486356.5% - 2.3GFLOPS,  4.6GB/s
Scalar Double2 - Mul        : 0.15 s  +43552403.3% - 4.3GFLOPS,  4.3GB/s
Scalar Double2 - seq1 Add   : 0.16 s  +45972821.9% - 4.1GFLOPS,  4.1GB/s
Scalar Double2 - seq1 Div   : 0.16 s  +46590309.5% - 2.0GFLOPS,  4.1GB/s
Scalar Double2 - FMA        : 0.21 s  +62320366.3% - 3.0GFLOPS,  3.0GB/s
Scalar Double4 - AddMul     : 0.21 s  +62835748.8% - 3.0GFLOPS,  3.0GB/s
Scalar Double2 - MulAdd     : 0.21 s  +63293065.7% - 3.0GFLOPS,  3.0GB/s
Scalar Double4 - AddMul2    : 0.22 s  +64342976.6% - 2.9GFLOPS,  2.9GB/s
Scalar Double4 - FMA        : 0.22 s  +64563887.3% - 2.9GFLOPS,  2.9GB/s
Scalar Double4 - MulAdd     : 0.22 s  +65894656.2% - 2.9GFLOPS,  2.9GB/s
Scalar Double2 - AddMul     : 0.28 s  +82014959.2% - 2.3GFLOPS,  2.3GB/s
Scalar Double2 - AddMul2    : 0.28 s  +84128941.7% - 2.3GFLOPS,  2.3GB/s
Scalar Double4 - Add        : 0.82 s  +241542378.1% - 0.8GFLOPS,  0.8GB/s
Scalar Double2 - Div        : 1.04 s  +308848634.6% - 306.5MFLOPS,  0.6GB/s
Scalar Double4 - Div        : 1.07 s  +317760109.2% - 297.9MFLOPS,  0.6GB/s
Scalar Double2 - Sqrt       : 1.57 s  +464897621.9% - 203.6MFLOPS,  407.3MB/s
Scalar Double4 - Sqrt       : 1.87 s  +552724344.7% - 171.3MFLOPS,  342.6MB/s

Simd Double2 - seq1 MulAdd: 0.12 s             - 5.2GFLOPS,  5.2GB/s
Simd Double2 - Add        : 0.13 s  +5.5%      - 4.9GFLOPS,  4.9GB/s
Simd Double2 - seq2 Add   : 0.14 s  +9.9%      - 4.7GFLOPS,  4.7GB/s
Simd Double2 - Mul        : 0.14 s  +14.9%     - 4.5GFLOPS,  4.5GB/s
Simd Double2 - FMA        : 0.15 s  +20.5%     - 4.3GFLOPS,  4.3GB/s
Simd Double2 - seq1 Add   : 0.16 s  +26.4%     - 4.1GFLOPS,  4.1GB/s
Simd Double2 - seq1 Div   : 0.16 s  +32.5%     - 1.9GFLOPS,  3.9GB/s
Simd Double2 - MulAdd     : 0.21 s  +68.8%     - 3.1GFLOPS,  3.1GB/s
Simd Double2 - AddMul     : 0.22 s  +76.0%     - 2.9GFLOPS,  2.9GB/s
Simd Double2 - AddMul2    : 0.22 s  +76.1%     - 2.9GFLOPS,  2.9GB/s
Simd Double2 - Sqrt       : 0.30 s  +138.2%    - 1.1GFLOPS,  2.2GB/s
Simd Double2 - Div        : 1.03 s  +732.9%    - 310.0MFLOPS,  0.6GB/s
```

# ARM Cortex A55 (MediaTek Dimensity 7020)

* 2000 MHz, 6nm, Motorola G54
* Theoretical performance (FMA):
	- 1x fp32 128-bit FMA pipe
	- 8 FLOPS * 2 GHz = 16 GFLOPS per core

## Float SIMD

```
Scalar Float4 - Mul     : 0.53 s             - 4.8GFLOPS
Scalar Float4 - par Add : 0.62 s  +17.0%     - 4.1GFLOPS
Scalar Float4 - seq2 Add: 0.67 s  +27.5%     - 3.8GFLOPS
Scalar Float4 - seq1 Add: 0.91 s  +72.8%     - 2.8GFLOPS
Scalar Float4 - FMA     : 0.96 s  +80.9%     - 2.7GFLOPS
Scalar Float4 - MulAdd  : 0.96 s  +81.2%     - 2.7GFLOPS
Scalar Float4 - Div     : 1.88 s  +256.3%    - 0.7GFLOPS
Scalar Float4 - Sqrt    : 2.43 s  +359.8%    - 0.5GFLOPS

Simd Float4 - FMA     : 0.36 s             - 7.0GFLOPS
Simd Float4 - Mul     : 0.56 s  +52.4%     - 4.6GFLOPS
Simd Float4 - par Add : 0.56 s  +52.8%     - 4.6GFLOPS
Simd Float4 - MulAdd  : 0.65 s  +77.2%     - 4.0GFLOPS
Simd Float4 - seq2 Add: 0.65 s  +77.2%     - 4.0GFLOPS
Simd Float4 - Sqrt    : 0.77 s  +110.7%    - 1.7GFLOPS
Simd Float4 - seq1 Add: 0.88 s  +141.2%    - 2.9GFLOPS
Simd Float4 - Div     : 1.33 s  +264.2%    - 1.0GFLOPS
```

## Double SIMD

```
Scalar Double4 - Mul     : 0.45 s  +10.2%     - 2.8GFLOPS
Scalar Double2 - Mul     : 0.52 s  +26.4%     - 2.5GFLOPS
Scalar Double4 - seq1 Add: 0.52 s  +26.6%     - 2.5GFLOPS
Scalar Double4 - par Add : 0.52 s  +28.3%     - 2.4GFLOPS
Scalar Double2 - par Add : 0.56 s  +36.2%     - 2.3GFLOPS
Scalar Double2 - seq2 Add: 0.65 s  +58.4%     - 2.0GFLOPS
Scalar Double2 - FMA     : 0.65 s  +58.4%     - 2.0GFLOPS
Scalar Double2 - MulAdd  : 0.65 s  +58.4%     - 2.0GFLOPS
Scalar Double4 - MulAdd  : 0.74 s  +81.0%     - 1.7GFLOPS
Scalar Double4 - FMA     : 0.74 s  +81.0%     - 1.7GFLOPS
Scalar Double2 - seq1 Add: 0.88 s  +115.8%    - 1.5GFLOPS
Scalar Double2 - Div     : 2.07 s  +406.3%    - 309.9MFLOPS
Scalar Double4 - Div     : 2.19 s  +436.4%    - 292.5MFLOPS
Scalar Double2 - Sqrt    : 3.29 s  +705.6%    - 194.8MFLOPS
Scalar Double4 - Sqrt    : 3.59 s  +780.5%    - 178.2MFLOPS

Simd Double2 - FMA     : 0.36 s             - 3.5GFLOPS
Simd Double2 - Mul     : 0.56 s  +52.8%     - 2.3GFLOPS
Simd Double2 - par Add : 0.56 s  +52.9%     - 2.3GFLOPS
Simd Double2 - seq2 Add: 0.65 s  +77.6%     - 2.0GFLOPS
Simd Double2 - MulAdd  : 0.65 s  +77.7%     - 2.0GFLOPS
Simd Double2 - Sqrt    : 0.77 s  +111.1%    - 0.8GFLOPS
Simd Double2 - seq1 Add: 0.88 s  +141.6%    - 1.5GFLOPS
Simd Double2 - Div     : 2.07 s  +467.7%    - 309.7MFLOPS
```


# ARM Cortex A55 (Snapdragon 888)

* Kryo 680 Silver, 1804 MHz, 5nm, Asus ROG Phone 5
* Theoretical performance (FMA):
	- 1x fp32 128-bit FMA pipe
	- 8 FLOPS * 1.8 GHz = 14.4 GFLOPS per core

## Float SIMD (single thread)

| | FLOPS | SOL |
|-------------------------------|----------------|
| Simd Float4 - MulAdd          | 7.04GFLOPS     |
| Simd Float4 - FMA ilp4        | 7.03GFLOPS     |
| Simd Float4 - FMA ilp8        | 7.03GFLOPS     |
| Simd Float4 - FMA ilp6        | 7.03GFLOPS     |
| Simd Float4 - FMA ilp7        | 7.02GFLOPS     |
| Simd Float4 - FMA ilp5        | 7.03GFLOPS     |
| Simd Float4 - FMA ilp16       | 6.43GFLOPS     |
| Simd Float4 - Fast Div        | 3.51GFLOPS     |
| Simd Float4 - Fast Sqrt       | 2.74GFLOPS     |
| Simd Float4 - Precise Sqrt    | 781.1MFLOPS    |

## Float SIMD (4 threads, 4 cores)

| | FLOPS per thread | SOL |
|------------------------------|------------|
| Simd Float4 - FMA ilp6       | 6.94GFLOPS |
| Simd Float4 - FMA ilp7       | 6.96GFLOPS |
| Simd Float4 - FMA ilp8       | 6.96GFLOPS |
| Simd Float4 - FMA ilp4       | 6.86GFLOPS |
| Simd Float4 - FMA ilp5       | 6.98GFLOPS |
| Simd Float4 - FMA ilp16      | 6.71GFLOPS |

## Float SIMD GEMM

**1024x1024x1024**

|         |  time  | diff  |    FLOPS    | bandwidth | SOL |
|---------|--------|-------|-------------|-----------|---|
| NEON v1 | 0.38 s | -     | 5.72GFLOPS  |  44.7MB/s |
| AE GEMM | 0.38 s | +1%   | 5.66GFLOPS  |  44.2MB/s |
| NEON v3 | 0.98 s | +162% | 2.18GFLOPS  |  17.1MB/s |

**128x128x128**

|         |  time   | diff |    FLOPS   | bandwidth  | SOL |
|---------|---------|-----|-------------|------------|---|
| NEON v1 | 0.55 ms | -   | 7.58GFLOPS  |  475.6MB/s |
| AE GEMM | 0.55 ms | -   | 7.55GFLOPS  |  473.7MB/s |
| NEON v3 | 0.60 ms | +9% | 6.94GFLOPS  |  435.6MB/s |

## Float SIMD GEMV

**2048x2048**

|         |  time    | diff  |   FLOPS      | bandwidth | SOL |
|---------|----------|-------|--------------|------------|---|
| NEON v2 | 5.27 ms  | -     | 1.59GFLOPS   |  3.18GB/s  |
| NEON v4 | 8.23 ms  | +56%  | 1.02GFLOPS   |  2.04GB/s  |
| NEON v3 | 37.06 ms | +603% | 226.3MFLOPS  |  453.1MB/s |
| NEON v1 | 51.27 ms | +872% | 163.6MFLOPS  |  327.6MB/s |

**256x256**

|         |  time    | diff |    FLOPS    | bandwidth | SOL |
|---------|----------|------|-------------|-----------|---|
| NEON v3 | 64.05 us | -    | 2.05GFLOPS  |  4.12GB/s |
| NEON v4 | 64.12 us | -    | 2.04GFLOPS  |  4.12GB/s |
| NEON v2 | 77.36 us | +21% | 1.69GFLOPS  |  3.42GB/s |
| NEON v1 | 83.13 us | +30% | 1.58GFLOPS  |  3.18GB/s |


# ARM Cortex A55 (MediaTek Dimensity 8200 Ultimate)

* Clock: 2.0 GHz
* Theoretical performance (FMA):
	- 1x fp32 128-bit FMA pipe
	- 8 FLOPS * 2.0 GHz = 16 GFLOPS

## Float SIMD (single thread)

| | FLOPS | SOL |
|----------------------------|-------------|---|
| Simd Float4 - Mul          | 30.9GFLOPS  |
| Simd Float4 - Add ilp4     | 30.9GFLOPS  |
| Simd Float4 - Add ilp5     | 30.9GFLOPS  |
| Simd Float4 - Add ilp7     | 30.9GFLOPS  |
| Simd Float4 - Add ilp6     | 30.8GFLOPS  |
| Simd Float4 - Div ilp4     | 15.4GFLOPS  | incorrect? |
| Simd Float4 - MulAdd       | 7.72GFLOPS  |
| Simd Float4 - Add ilp8     | 7.72GFLOPS  |
| Simd Float4 - FMA ilp6     | 7.72GFLOPS  |
| Simd Float4 - FMA ilp7     | 7.72GFLOPS  |
| Simd Float4 - FMA ilp8     | 7.72GFLOPS  |
| Simd Float4 - Div ilp8     | 7.72GFLOPS  |
| Simd Float4 - FMA ilp4     | 7.71GFLOPS  |
| Simd Float4 - FMA ilp5     | 7.71GFLOPS  |
| Simd Float4 - FMA ilp16    | 7.05GFLOPS  |
| Simd Float4 - Fast Div     | 3.86GFLOPS  |
| Simd Float4 - Fast Sqrt    | 3.01GFLOPS  |
| Simd Float4 - Precise Sqrt | 857.2MFLOPS |

## Float SIMD GEMM

**1024x1024x1024**

|         |  time  | diff  |     FLOPS    | bandwidth | SOL |
|---------|--------|-------|--------------|-----------|---|
| NEON v1 | 0.36 s | -     | 6.01GFLOPS   |  47.0MB/s |
| AE GEMM | 0.36 s | +1%   | 5.95GFLOPS   |  46.5MB/s |
| NEON v3 | 2.82 s | +689% | 761.5MFLOPS  |  5.95MB/s |

**128x128x128**

|         |  time   | diff |    FLOPS    | bandwidth  | SOL |
|---------|---------|------|-------------|------------|---|
| NEON v1 | 0.51 ms | -    | 8.11GFLOPS  |  509.0MB/s |
| AE GEMM | 0.52 ms | -    | 8.11GFLOPS  |  508.9MB/s |
| NEON v3 | 0.57 ms | +10% | 7.36GFLOPS  |  461.6MB/s |

## Float SIMD GEMV

**2048x2048**

|         |  time    | diff  |   FLOPS      | bandwidth  | SOL |
|---------|----------|-------|--------------|------------|---|
| NEON v2 | 6.15 ms  | -     | 1.36GFLOPS   |  2.73GB/s  |
| NEON v4 | 8.09 ms  | +32%  | 1.04GFLOPS   |  2.08GB/s  |
| NEON v3 | 32.50 ms | +429% | 258.1MFLOPS  |  516.7MB/s |
| NEON v1 | 49.25 ms | +701% | 170.3MFLOPS  |  341.0MB/s |

**256x256**

|         |  time    | diff |    FLOPS    | bandwidth | SOL |
|---------|----------|------|-------------|-----------|---|
| NEON v4 | 61.18 us | -    | 2.14GFLOPS  |  4.32GB/s |
| NEON v3 | 66.69 us | +9%  | 1.97GFLOPS  |  3.96GB/s |
| NEON v2 | 71.34 us | +17% | 1.84GFLOPS  |  3.70GB/s |
| NEON v1 | 89.46 us | +46% | 1.47GFLOPS  |  2.95GB/s |


# ARM Cortex A53 (Samsung Exynos 7870)

* 1586 MHz, 14nm, Samsung J7 Neo
* Theoretical performance (FMA):
	- 1x fp32 128-bit FMA pipe
	- 8 FLOPS * 1.6 GHz = 12.7 GFLOPS per core

## Float SIMD

```
Scalar Float4 - MulAdd  : 0.35 s             - 0.9GFLOPS
Scalar Float4 - Mul     : 0.37 s  +3.4%      - 0.9GFLOPS
Scalar Float4 - seq2 Add: 0.37 s  +5.1%      - 0.9GFLOPS
Scalar Float4 - seq1 Add: 0.37 s  +5.6%      - 0.9GFLOPS
Scalar Float4 - par Add : 0.37 s  +5.8%      - 0.9GFLOPS
Scalar Float4 - Sqrt    : 1.08 s  +207.1%    - 147.5MFLOPS
Scalar Float4 - Div     : 1.27 s  +258.8%    - 126.3MFLOPS

Simd Float4 - Sqrt    : 0.16 s             - 1.0GFLOPS
Simd Float4 - MulAdd  : 0.33 s  +99.6%     - 1.0GFLOPS
Simd Float4 - Mul     : 0.34 s  +105.8%    - 0.9GFLOPS
Simd Float4 - seq2 Add: 0.34 s  +109.7%    - 0.9GFLOPS
Simd Float4 - par Add : 0.34 s  +110.3%    - 0.9GFLOPS
Simd Float4 - seq1 Add: 0.34 s  +110.3%    - 0.9GFLOPS
Simd Float4 - Div     : 0.30 s  +84.1%     - 0.5GFLOPS
```

## Double SIMD

```
Scalar Double4 - MulAdd  : 0.26 s             - 0.6GFLOPS
Scalar Double4 - par Add : 0.27 s  +7.4%      - 0.6GFLOPS
Scalar Double4 - Mul     : 0.28 s  +10.6%     - 0.6GFLOPS
Scalar Double4 - seq1 Add: 0.28 s  +11.0%     - 0.6GFLOPS
Scalar Double4 - seq2 Add: 0.28 s  +11.1%     - 0.6GFLOPS
Scalar Double2 - MulAdd  : 0.36 s  +39.2%     - 449.6MFLOPS
Scalar Double2 - Mul     : 0.36 s  +39.4%     - 449.1MFLOPS
Scalar Double2 - par Add : 0.36 s  +41.6%     - 441.9MFLOPS
Scalar Double2 - seq1 Add: 0.36 s  +41.9%     - 441.1MFLOPS
Scalar Double2 - seq2 Add: 0.36 s  +42.0%     - 440.8MFLOPS
Scalar Double4 - Sqrt    : 1.08 s  +321.3%    - 74.3MFLOPS
Scalar Double2 - Sqrt    : 1.09 s  +326.2%    - 73.4MFLOPS
Scalar Double4 - Div     : 1.14 s  +346.9%    - 70.0MFLOPS
Scalar Double2 - Div     : 1.18 s  +362.8%    - 67.6MFLOPS
```


# ARM Cortex A53, P-core (Snapdragon 439)

* 2016 MHz, 12nm, Xiaomi Redmi 7A
* Theoretical performance (FMA):
	- 1x fp32 128-bit FMA pipe
	- 8 FLOPS * 2 GHz = 16 GFLOPS per core

## Float SIMD

```
Scalar Float4 - MulAdd  : 0.28 s             - 1.1GFLOPS
Scalar Float4 - FMA     : 0.28 s  +0.0%      - 1.1GFLOPS
Scalar Float4 - Mul     : 0.29 s  +4.9%      - 1.1GFLOPS
Scalar Float4 - seq2 Add: 0.30 s  +7.0%      - 1.1GFLOPS
Scalar Float4 - par Add : 0.31 s  +10.9%     - 1.0GFLOPS
Scalar Float4 - seq1 Add: 0.31 s  +11.7%     - 1.0GFLOPS
Scalar Float4 - Sqrt    : 0.86 s  +206.6%    - 186.2MFLOPS
Scalar Float4 - Div     : 1.00 s  +258.3%    - 159.3MFLOPS

Simd Float4 - Sqrt    : 0.13 s             - 1.2GFLOPS
Simd Float4 - FMA     : 0.26 s  +96.7%     - 1.2GFLOPS
Simd Float4 - MulAdd  : 0.26 s  +97.5%     - 1.2GFLOPS
Simd Float4 - Mul     : 0.27 s  +104.0%    - 1.2GFLOPS
Simd Float4 - par Add : 0.28 s  +107.9%    - 1.2GFLOPS
Simd Float4 - seq1 Add: 0.28 s  +108.0%    - 1.2GFLOPS
Simd Float4 - seq2 Add: 0.28 s  +108.0%    - 1.2GFLOPS
Simd Float4 - Div     : 0.24 s  +81.8%     - 0.7GFLOPS
```

## Double SIMD

```
Scalar Double4 - MulAdd  : 0.20 s             - 0.8GFLOPS
Scalar Double4 - FMA     : 0.20 s  +0.5%      - 0.8GFLOPS
Scalar Double4 - par Add : 0.22 s  +7.5%      - 0.7GFLOPS
Scalar Double4 - Mul     : 0.22 s  +10.5%     - 0.7GFLOPS
Scalar Double4 - seq2 Add: 0.23 s  +11.3%     - 0.7GFLOPS
Scalar Double4 - seq1 Add: 0.23 s  +12.0%     - 0.7GFLOPS
Scalar Double2 - Mul     : 0.28 s  +39.8%     - 0.6GFLOPS
Scalar Double2 - FMA     : 0.29 s  +40.5%     - 0.6GFLOPS
Scalar Double2 - MulAdd  : 0.29 s  +40.6%     - 0.6GFLOPS
Scalar Double2 - seq2 Add: 0.29 s  +42.2%     - 0.6GFLOPS
Scalar Double2 - seq1 Add: 0.29 s  +42.2%     - 0.6GFLOPS
Scalar Double2 - par Add : 0.29 s  +42.2%     - 0.6GFLOPS
Scalar Double4 - Sqrt    : 0.85 s  +320.5%    - 93.6MFLOPS
Scalar Double2 - Sqrt    : 0.86 s  +325.2%    - 92.6MFLOPS
Scalar Double4 - Div     : 0.91 s  +346.3%    - 88.2MFLOPS
Scalar Double2 - Div     : 0.94 s  +361.5%    - 85.3MFLOPS
```


# ARM Cortex A53, E-core (Snapdragon 439)

* 1459 MHz, 12nm, Xiaomi Redmi 7A
* Theoretical performance (FMA):
	- 1x fp32 128-bit FMA pipe
	- 8 FLOPS * 1.46 GHz = 11.7 GFLOPS per core

## Float SIMD

```
Scalar Float4 - FMA     : 0.38 s             - 0.8GFLOPS
Scalar Float4 - MulAdd  : 0.38 s  +0.1%      - 0.8GFLOPS
Scalar Float4 - Mul     : 0.40 s  +3.7%      - 0.8GFLOPS
Scalar Float4 - par Add : 0.41 s  +5.7%      - 0.8GFLOPS
Scalar Float4 - seq1 Add: 0.41 s  +5.7%      - 0.8GFLOPS
Scalar Float4 - seq2 Add: 0.41 s  +5.7%      - 0.8GFLOPS
Scalar Float4 - Sqrt    : 1.18 s  +208.4%    - 135.2MFLOPS
Scalar Float4 - Div     : 1.38 s  +260.6%    - 115.6MFLOPS

Simd Float4 - Sqrt    : 0.18 s             - 0.9GFLOPS
Simd Float4 - FMA     : 0.36 s  +98.6%     - 0.9GFLOPS
Simd Float4 - MulAdd  : 0.36 s  +99.0%     - 0.9GFLOPS
Simd Float4 - Mul     : 0.37 s  +105.9%    - 0.9GFLOPS
Simd Float4 - seq1 Add: 0.38 s  +109.6%    - 0.8GFLOPS
Simd Float4 - seq2 Add: 0.38 s  +109.8%    - 0.8GFLOPS
Simd Float4 - par Add : 0.38 s  +110.4%    - 0.8GFLOPS
Simd Float4 - Div     : 0.33 s  +83.3%     - 484.1MFLOPS
```

## Double SIMD

```
Scalar Double4 - MulAdd  : 0.28 s             - 0.6GFLOPS
Scalar Double4 - FMA     : 0.28 s  +0.9%      - 0.6GFLOPS
Scalar Double4 - par Add : 0.30 s  +9.4%      - 0.5GFLOPS
Scalar Double4 - Mul     : 0.31 s  +11.3%     - 0.5GFLOPS
Scalar Double4 - seq2 Add: 0.31 s  +12.2%     - 0.5GFLOPS
Scalar Double4 - seq1 Add: 0.31 s  +12.4%     - 0.5GFLOPS
Scalar Double2 - Mul     : 0.39 s  +40.3%     - 410.3MFLOPS
Scalar Double2 - MulAdd  : 0.39 s  +40.9%     - 408.7MFLOPS
Scalar Double2 - FMA     : 0.39 s  +41.0%     - 408.3MFLOPS
Scalar Double2 - seq2 Add: 0.40 s  +42.9%     - 402.9MFLOPS
Scalar Double2 - seq1 Add: 0.40 s  +42.9%     - 402.9MFLOPS
Scalar Double2 - par Add : 0.40 s  +42.9%     - 402.9MFLOPS
Scalar Double4 - Sqrt    : 1.18 s  +325.2%    - 67.7MFLOPS
Scalar Double2 - Sqrt    : 1.19 s  +328.5%    - 67.2MFLOPS
Scalar Double4 - Div     : 1.25 s  +351.2%    - 63.8MFLOPS
Scalar Double2 - Div     : 1.29 s  +365.5%    - 61.8MFLOPS
```


# Apple M1, P-core

* 3.2 GHz, 5nm, Mac Mini M1 16GB, Firestorm arch
* 8x fp32 FLOPS/cy
* Theoretical performance:
	- 8 FLOPS * 3.2 GHz = 25.6 GFLOPS

## Float SIMD

```
Scalar Float4 - Add        : 0.31 s             - 16.7GFLOPS,  8.37GB/s
Scalar Float4 - AddMul     : 0.35 s  +15.8%     - 14.5GFLOPS,  7.23GB/s
Scalar Float4 - AddMul2    : 0.35 s  +15.9%     - 14.4GFLOPS,  7.22GB/s
Scalar Float4 - MulAdd     : 0.36 s  +16.1%     - 14.4GFLOPS,  7.21GB/s
Scalar Float4 - FMA        : 0.36 s  +16.2%     - 14.4GFLOPS,  7.21GB/s
Scalar Float4 - Mul        : 0.40 s  +31.0%     - 12.8GFLOPS,  6.39GB/s
Scalar Float4 - Div        : 0.55 s  +80.4%     - 4.64GFLOPS,  4.64GB/s
Scalar Float4 - Sqrt       : 0.75 s  +145.9%    - 3.40GFLOPS,  3.40GB/s
Scalar Float4 - seq1 Add   : 1.20 s  +293.0%    - 4.26GFLOPS,  2.13GB/s
Scalar Float4 - seq1 MulAdd: 1.40 s  +358.5%    - 3.65GFLOPS,  1.83GB/s
Scalar Float4 - seq1 Div   : 1.61 s  +427.0%    - 1.59GFLOPS,  1.59GB/s

Simd Float4 - FMA          : 0.20 s             - 25.5GFLOPS,  12.7GB/s
Simd Float4 - Add          : 0.30 s  +51.7%     - 16.8GFLOPS,  8.40GB/s
Simd Float4 - AddMul       : 0.35 s  +76.3%     - 14.5GFLOPS,  7.23GB/s
Simd Float4 - AddMul2      : 0.35 s  +76.5%     - 14.4GFLOPS,  7.22GB/s
Simd Float4 - MulAdd       : 0.36 s  +76.9%     - 14.4GFLOPS,  7.20GB/s
Simd Float4 - Mul          : 0.40 s  +99.4%     - 12.8GFLOPS,  6.39GB/s
Simd Float4 - Sqrt         : 0.50 s  +149.8%    - 5.10GFLOPS,  5.10GB/s
Simd Float4 - Div          : 0.55 s  +174.6%    - 4.64GFLOPS,  4.64GB/s
Simd Float4 - seq1 Add     : 1.20 s  +498.5%    - 4.26GFLOPS,  2.13GB/s
Simd Float4 - seq1 MulAdd  : 1.40 s  +598.1%    - 3.65GFLOPS,  1.83GB/s
Simd Float4 - seq1 Div     : 1.62 s  +705.4%    - 1.58GFLOPS,  1.58GB/s
```

## Double SIMD

```
Scalar Double4 - FMA        : 0.19 s             - 13.8GFLOPS,  13.8GB/s
Scalar Double4 - MulAdd     : 0.20 s  +8.0%      - 12.7GFLOPS,  12.7GB/s
Scalar Double4 - AddMul     : 0.20 s  +8.0%      - 12.7GFLOPS,  12.7GB/s
Scalar Double4 - AddMul2    : 0.20 s  +8.0%      - 12.7GFLOPS,  12.7GB/s
Scalar Double4 - Add        : 0.20 s  +8.0%      - 12.7GFLOPS,  12.7GB/s
Scalar Double4 - Mul        : 0.21 s  +10.3%     - 12.5GFLOPS,  12.5GB/s
Scalar Double2 - Add        : 0.31 s  +68.5%     - 8.17GFLOPS,  8.17GB/s
Scalar Double2 - FMA        : 0.35 s  +89.3%     - 7.27GFLOPS,  7.27GB/s
Scalar Double2 - AddMul2    : 0.36 s  +95.7%     - 7.03GFLOPS,  7.03GB/s
Scalar Double2 - MulAdd     : 0.37 s  +96.7%     - 7.00GFLOPS,  7.00GB/s
Scalar Double2 - AddMul     : 0.37 s  +98.6%     - 6.93GFLOPS,  6.93GB/s
Scalar Double2 - Mul        : 0.40 s  +115.3%    - 6.39GFLOPS,  6.39GB/s
Scalar Double4 - Sqrt       : 0.40 s  +115.6%    - 3.19GFLOPS,  6.38GB/s
Scalar Double4 - Div        : 0.48 s  +159.0%    - 2.66GFLOPS,  5.31GB/s
Scalar Double4 - seq1 Add   : 0.60 s  +223.3%    - 4.26GFLOPS,  4.26GB/s
Scalar Double2 - Div        : 0.69 s  +272.6%    - 1.85GFLOPS,  3.69GB/s
Scalar Double4 - seq1 MulAdd: 0.70 s  +277.3%    - 3.65GFLOPS,  3.65GB/s
Scalar Double2 - Sqrt       : 0.80 s  +331.3%    - 1.60GFLOPS,  3.19GB/s
Scalar Double4 - seq1 Div   : 1.00 s  +438.8%    - 1.28GFLOPS,  2.55GB/s
Scalar Double2 - seq1 Add   : 1.20 s  +546.2%    - 2.13GFLOPS,  2.13GB/s
Scalar Double2 - seq1 MulAdd: 1.40 s  +654.0%    - 1.83GFLOPS,  1.83GB/s
Scalar Double2 - seq1 Div   : 2.02 s  +984.2%    - 634.7MFLOPS, 1.27GB/s

Simd Double2 - FMA        : 0.20 s             - 12.6GFLOPS,  12.6GB/s
Simd Double2 - Add        : 0.30 s  +50.1%     - 8.40GFLOPS,  8.40GB/s
Simd Double2 - AddMul     : 0.35 s  +74.3%     - 7.23GFLOPS,  7.23GB/s
Simd Double2 - AddMul2    : 0.35 s  +74.6%     - 7.22GFLOPS,  7.22GB/s
Simd Double2 - MulAdd     : 0.36 s  +75.0%     - 7.20GFLOPS,  7.20GB/s
Simd Double2 - Mul        : 0.40 s  +97.2%     - 6.39GFLOPS,  6.39GB/s
Simd Double2 - Sqrt       : 0.50 s  +147.1%    - 2.55GFLOPS,  5.10GB/s
Simd Double2 - Div        : 0.65 s  +221.2%    - 1.96GFLOPS,  3.92GB/s
Simd Double2 - seq1 Add   : 1.20 s  +491.9%    - 2.13GFLOPS,  2.13GB/s
Simd Double2 - seq1 MulAdd: 1.40 s  +590.8%    - 1.83GFLOPS,  1.83GB/s
Simd Double2 - seq1 Div   : 2.02 s  +893.4%    - 634.6MFLOPS,  1.27GB/s
```


# Apple M1, E-core

* 2.06 GHz, 5nm, Mac Mini M1 16GB, Icestorm arch
* 4x fp32 FLOPS/cy
* Theoretical performance:
	- 4 FLOPS * 2.06 GHz = 8.2 GFLOPS

## Float SIMD

```
Scalar Float4 - Add        : 1.01 s             - 5.09GFLOPS,  2.55GB/s
Scalar Float4 - AddMul     : 1.16 s  +15.7%     - 4.40GFLOPS,  2.20GB/s
Scalar Float4 - FMA        : 1.16 s  +15.8%     - 4.40GFLOPS,  2.20GB/s
Scalar Float4 - AddMul2    : 1.17 s  +16.0%     - 4.39GFLOPS,  2.19GB/s
Scalar Float4 - MulAdd     : 1.17 s  +16.2%     - 4.38GFLOPS,  2.19GB/s
Scalar Float4 - Mul        : 1.34 s  +33.3%     - 3.82GFLOPS,  1.91GB/s
Scalar Float4 - Div        : 1.99 s  +98.2%     - 1.28GFLOPS,  1.28GB/s
Scalar Float4 - Sqrt       : 2.84 s  +182.9%    - 899.9MFLOPS, 899.9MB/s
Scalar Float4 - seq1 Add   : 4.00 s  +297.4%    - 1.28GFLOPS,  640.6MB/s
Scalar Float4 - seq1 MulAdd: 4.65 s  +362.4%    - 1.10GFLOPS,  550.6MB/s
Scalar Float4 - seq1 Div   : 6.00 s  +496.4%    - 426.9MFLOPS, 426.9MB/s

Simd Float4 - FMA          : 0.67 s             - 7.67GFLOPS,  3.83GB/s
Simd Float4 - Add          : 1.00 s  +49.7%     - 5.12GFLOPS,  2.56GB/s
Simd Float4 - AddMul2      : 1.17 s  +74.7%     - 4.39GFLOPS,  2.19GB/s
Simd Float4 - AddMul       : 1.17 s  +74.8%     - 4.38GFLOPS,  2.19GB/s
Simd Float4 - MulAdd       : 1.18 s  +76.0%     - 4.36GFLOPS,  2.18GB/s
Simd Float4 - Mul          : 1.34 s  +100.3%    - 3.83GFLOPS,  1.91GB/s
Simd Float4 - Sqrt         : 1.92 s  +186.9%    - 1.34GFLOPS,  1.34GB/s
Simd Float4 - Div          : 1.99 s  +197.3%    - 1.29GFLOPS,  1.29GB/s
Simd Float4 - seq1 Add     : 4.01 s  +500.6%    - 1.28GFLOPS,  638.2MB/s
Simd Float4 - seq1 MulAdd  : 4.66 s  +597.7%    - 1.10GFLOPS,  549.4MB/s
Simd Float4 - seq1 Div     : 6.02 s  +800.7%    - 425.5MFLOPS, 425.5MB/s
```

## Double SIMD

```
Scalar Double4 - Mul        : 0.69 s             - 3.69GFLOPS,  3.69GB/s
Scalar Double4 - FMA        : 0.78 s  +11.9%     - 3.29GFLOPS,  3.29GB/s
Scalar Double2 - FMA        : 1.17 s  +68.6%     - 2.19GFLOPS,  2.19GB/s
Scalar Double2 - Mul        : 1.34 s  +93.0%     - 1.91GFLOPS,  1.91GB/s
Scalar Double2 - Add        : 1.34 s  +93.0%     - 1.91GFLOPS,  1.91GB/s
Scalar Double4 - Add        : 1.39 s  +100.5%    - 1.84GFLOPS,  1.84GB/s
Scalar Double4 - AddMul2    : 1.44 s  +107.1%    - 1.78GFLOPS,  1.78GB/s
Scalar Double4 - MulAdd     : 1.47 s  +112.1%    - 1.74GFLOPS,  1.74GB/s
Scalar Double2 - AddMul2    : 1.49 s  +114.3%    - 1.72GFLOPS,  1.72GB/s
Scalar Double2 - MulAdd     : 1.50 s  +116.3%    - 1.71GFLOPS,  1.71GB/s
Scalar Double2 - AddMul     : 1.55 s  +123.5%    - 1.65GFLOPS,  1.65GB/s
Scalar Double4 - AddMul     : 1.58 s  +127.8%    - 1.62GFLOPS,  1.62GB/s
Scalar Double4 - Div        : 1.91 s  +174.9%    - 670.8MFLOPS,  1.34GB/s
Scalar Double4 - seq1 Add   : 2.07 s  +198.4%    - 1.24GFLOPS,  1.24GB/s
Scalar Double2 - Div        : 2.16 s  +211.2%    - 592.7MFLOPS,  1.19GB/s
Scalar Double4 - seq1 MulAdd: 2.30 s  +231.5%    - 1.11GFLOPS,  1.11GB/s
Scalar Double4 - Sqrt       : 2.65 s  +282.2%    - 482.5MFLOPS,  965.0MB/s
Scalar Double2 - Sqrt       : 3.11 s  +348.3%    - 411.4MFLOPS,  822.8MB/s
Scalar Double4 - seq1 Div   : 3.33 s  +380.5%    - 383.8MFLOPS,  767.6MB/s
Scalar Double2 - seq1 Add   : 4.01 s  +477.8%    - 638.3MFLOPS,  638.3MB/s
Scalar Double2 - seq1 MulAdd: 4.64 s  +568.9%    - 551.4MFLOPS,  551.4MB/s
Scalar Double2 - seq1 Div   : 6.76 s  +873.5%    - 189.4MFLOPS,  378.9MB/s

Simd Double2 - FMA          : 0.67 s             - 3.83GFLOPS,  3.83GB/s
Simd Double2 - Add          : 1.00 s  +49.4%     - 2.56GFLOPS,  2.56GB/s
Simd Double2 - AddMul2      : 1.17 s  +74.5%     - 2.20GFLOPS,  2.20GB/s
Simd Double2 - MulAdd       : 1.17 s  +74.6%     - 2.19GFLOPS,  2.19GB/s
Simd Double2 - AddMul       : 1.17 s  +75.6%     - 2.18GFLOPS,  2.18GB/s
Simd Double2 - Mul          : 1.34 s  +99.8%     - 1.92GFLOPS,  1.92GB/s
Simd Double2 - Sqrt         : 1.90 s  +184.8%    - 672.5MFLOPS,  1.35GB/s
Simd Double2 - Div          : 2.35 s  +251.2%    - 545.4MFLOPS,  1.09GB/s
Simd Double2 - seq1 Add     : 3.99 s  +497.0%    - 641.6MFLOPS,  641.6MB/s
Simd Double2 - seq1 MulAdd  : 4.62 s  +591.9%    - 553.6MFLOPS,  553.6MB/s
Simd Double2 - seq1 Div     : 7.39 s  +1005.8%   - 173.2MFLOPS,  346.4MB/s
```


# Intel N150

* Twin Lake (2024)
* 'Skymont' LP E-cores
* Theoretical performance (FMA):
	- 2x fp32 128-bit FMA pipes
	- 16 FLOPS * 3.6 GHz = 57.6 GFLOPS per core
	- 16 FLOPS * 3.3 GHz * 2 cores = 52.8 * 2 = 106 GFLOPS per CPU
	- 16 FLOPS * 2.8 GHz * 4 cores = 44.8 * 4 = 180 GFLOPS per CPU
	- 2 FLOPS (scalar) * 3.6 GHz = 7.2 GFLOPS per core

## Float scalar (single thread)

| | FLOPS | SOL |
|------------------------------|-------------|---|
| Scalar Float4 - MulAdd       | 5.72GFLOPS  | 80%  |
| Scalar Float4 - Add          | 5.68GFLOPS  | 160% |
| Scalar Float4 - Mul          | 5.26GFLOPS  |
| Scalar Float4 - FMA          | 4.58GFLOPS  | 64%  |
| Scalar Float4 - Div          | 654.4MFLOPS |
| Scalar Float4 - Precise Sqrt | 419.2MFLOPS |
| Scalar Float4 - Fast Sqrt    | 289.5MFLOPS |

## Float SIMD (single thread)

| | FLOPS | SOL |
|-------------------------------|-------------|---|
| Simd Float8 - FMA ilp8        | 38.3GFLOPS | 66% |
| Simd Float8 - FMA ilp7        | 37.8GFLOPS |
| Simd Float8 - FMA ilp6        | 37.3GFLOPS |
| Simd Float8 - FMA ilp5        | 37.1GFLOPS |
| Simd Float8 - FMA ilp4        | 36.8GFLOPS |
| Simd Float8 - FMA ilp16       | 34.2GFLOPS |
| Simd Float8 - Add ilp7        | 28.3GFLOPS | 99% |
| Simd Float8 - MulAdd          | 28.3GFLOPS | 49% |
| Simd Float8 - Add ilp4        | 28.2GFLOPS |
| Simd Float8 - Mul             | 28.1GFLOPS | 99% |
| Simd Float8 - Add ilp6        | 28.2GFLOPS |
| Simd Float8 - Add ilp5        | 28.2GFLOPS |
| Simd Float8 - Add ilp8        | 28.1GFLOPS |
| Simd Float8 - FMA ilp8 + Div2 | 22.6GFLOPS |
| Simd Float8 - FMA ilp8 + Add  | 22.2GFLOPS |
| Simd Float8 - FMA ilp4 + Div4 | 11.3GFLOPS |
| Simd Float8 - Fast Div        | 5.64GFLOPS |
| Simd Float8 - Fast Sqrt       | 5.60GFLOPS |
| Simd Float8 - Div ilp8        | 1.41GFLOPS |
| Simd Float8 - Div ilp4        | 1.41GFLOPS |
| Simd Float8 - Precise Sqrt    | 1.18GFLOPS |

## Float SIMD (2 threads, 2 cores)

| | FLOPS per thread | SOL |
|-------------------------------|-------------|---|
| Simd Float8 - FMA ilp5       | 35.6GFLOPS | 67% |
| Simd Float8 - FMA ilp6       | 35.1GFLOPS |
| Simd Float8 - FMA ilp7       | 34.8GFLOPS |
| Simd Float8 - FMA ilp4       | 34.5GFLOPS |
| Simd Float8 - FMA ilp8       | 33.2GFLOPS |
| Simd Float8 - Add ilp7       | 26.1GFLOPS | 99% |
| Simd Float8 - Add ilp8       | 26.3GFLOPS |
| Simd Float8 - Add ilp4       | 26.2GFLOPS |
| Simd Float8 - Add ilp6       | 26.3GFLOPS |
| Simd Float8 - Add ilp5       | 26.1GFLOPS |
| Simd Float8 - FMA ilp16      | 24.3GFLOPS |

## Float SIMD (4 threads, 4 cores)

| | FLOPS per thread | SOL |
|------------------------------|-------------|---|
| Simd Float8 - FMA ilp5       | 29.5GFLOPS | 66% |
| Simd Float8 - FMA ilp7       | 29.0GFLOPS |
| Simd Float8 - FMA ilp6       | 28.7GFLOPS |
| Simd Float8 - FMA ilp4       | 28.8GFLOPS |
| Simd Float8 - FMA ilp8       | 27.0GFLOPS |
| Simd Float8 - Add ilp5       | 22.1GFLOPS | 99% |
| Simd Float8 - Add ilp6       | 22.0GFLOPS |
| Simd Float8 - Add ilp7       | 21.9GFLOPS |
| Simd Float8 - Add ilp8       | 21.8GFLOPS |
| Simd Float8 - FMA ilp16      | 20.2GFLOPS |
| Simd Float8 - FMA ilp8 + Add | 19.9GFLOPS |
| Simd Float8 - Add ilp4       | 19.0GFLOPS |


# AMD Ryzen 7 8745HS

* Theoretical performance (FMA):
	- 2x fp32 256-bit FMA pipes
	- 32 FLOPS * 4.9 GHz = 156.8 GFLOPS per core

## Float SIMD (single thread)

| | time | diff | FLOPS | SOL |
|----------------------------|----------|--------|-------------|---|
| Simd Float8 - FMA ilp=8    | 51.69 ms | +1%    | 154.8GFLOPS | 99% |
| Simd Float8 - FMA ilp=16   | 0.10 s   | +102%  | 154.4GFLOPS |
| Simd Float8 - FMA ilp=7    | 53.47 ms | +4%    | 130.9GFLOPS |
| Simd Float8 - FMA + Div    | 65.05 ms | +27%   | 123.0GFLOPS |
| Simd Float8 - FMA ilp=6    | 51.20 ms | -      | 117.2GFLOPS |
| Simd Float8 - MulAdd       | 82.21 ms | +61%   | 97.3GFLOPS  | 62% |
| Simd Float8 - FMA ilp=5    | 0.10 s   | +102%  | 96.8GFLOPS  |
| Simd Float8 - FMA + Div2   | 84.31 ms | +65%   | 94.9GFLOPS  |
| Simd Float8 - FMA + Add    | 96.49 ms | +88%   | 82.9GFLOPS  |
| Simd Float8 - Add ilp=7    | 90.62 ms | +77%   | 77.2GFLOPS  |
| Simd Float8 - Add ilp=8    | 0.10 s   | +102%  | 77.2GFLOPS  |
| Simd Float8 - FMA ilp=4    | 0.10 s   | +102%  | 77.2GFLOPS  | 49% |
| Simd Float8 - Mul          | 0.10 s   | +103%  | 77.0GFLOPS  |
| Simd Float8 - Add ilp=6    | 0.12 s   | +129%  | 76.8GFLOPS  |
| Simd Float8 - Add ilp=5    | 0.16 s   | +205%  | 64.1GFLOPS  |
| Simd Float8 - Add ilp=4    | 0.16 s   | +203%  | 51.6GFLOPS  | 66% |
| Simd Float8 - Fast Div     | 0.21 s   | +304%  | 38.7GFLOPS  |
| Simd Float8 - Fast Sqrt    | 0.21 s   | +304%  | 38.7GFLOPS  |
| Simd Float8 - Div ilp=4    | 0.62 s   | +1104% | 13.0GFLOPS  |
| Simd Float8 - Div ilp=8    | 0.62 s   | +1105% | 13.0GFLOPS  |
| Simd Float8 - Precise Sqrt | 1.03 s   | +1905% | 7.79GFLOPS  |

## Float SIMD (4 threads, 4 cores)

| | time | diff | FLOPS | SOL |
|--------------------------|----------|-------|-------------|---|
| Simd Float8 - FMA ilp=16 | 0.14 s   | +89%  | 118.0GFLOPS | 75% |
| Simd Float8 - FMA ilp=8  | 76.72 ms | +7%   | 104.3GFLOPS | 67% |
| Simd Float8 - FMA ilp=7  | 71.56 ms | -     | 97.8GFLOPS  |
| Simd Float8 - FMA ilp=6  | 71.88 ms | -     | 83.5GFLOPS  |
| Simd Float8 - FMA ilp=5  | 0.14 s   | +97%  | 70.9GFLOPS  |
| Simd Float8 - FMA + Add  | 0.13 s   | +78%  | 62.8GFLOPS  |
| Simd Float8 - Add ilp=5  | 0.17 s   | +132% | 60.2GFLOPS  | 77% |
| Simd Float8 - Add ilp=8  | 0.13 s   | +86%  | 60.1GFLOPS  | 77% |
| Simd Float8 - Add ilp=7  | 0.12 s   | +64%  | 59.6GFLOPS  |
| Simd Float8 - Add ilp=6  | 0.15 s   | +112% | 59.4GFLOPS  |
| Simd Float8 - FMA ilp=4  | 0.15 s   | +103% | 54.9GFLOPS  |
| Simd Float8 - Add ilp=4  | 0.16 s   | +130% | 48.5GFLOPS  |

## Double SIMD (single thread)

| | time | diff | delta | FLOPS | SOL |
|-----------------------------|----------|--------|-------|------------|---|
| Simd Double4 - FMA ilp=8    | 51.72 ms | +1%    | +1%   | 77.3GFLOPS | 99% |
| Simd Double4 - FMA ilp=16   | 0.10 s   | +102%  | -     | 77.3GFLOPS | 99% |
| Simd Double4 - FMA ilp=7    | 53.43 ms | +4%    | +3%   | 65.5GFLOPS |
| Simd Double4 - FMA ilp=6    | 51.25 ms | -      | -     | 58.5GFLOPS |
| Simd Double4 - MulAdd       | 82.25 ms | +60%   | +54%  | 48.6GFLOPS |
| Simd Double4 - FMA ilp=5    | 0.10 s   | +102%  | +7%   | 48.4GFLOPS |
| Simd Double4 - FMA + Div    | 84.43 ms | +65%   | +3%   | 47.4GFLOPS |
| Simd Double4 - FMA + Add    | 96.35 ms | +88%   | +6%   | 41.5GFLOPS |
| Simd Double4 - Add ilp=7    | 90.50 ms | +77%   | +7%   | 38.7GFLOPS | 99% |
| Simd Double2 - FMA ilp=16   | 0.21 s   | +304%  | -     | 38.7GFLOPS | 49% |
| Simd Double4 - FMA ilp=4    | 0.10 s   | +102%  | -     | 38.6GFLOPS |
| Simd Double4 - Mul          | 0.10 s   | +103%  | -     | 38.5GFLOPS |
| Simd Double4 - Add ilp=8    | 0.10 s   | +103%  | -     | 38.5GFLOPS | 98% |
| Simd Double4 - Add ilp=6    | 0.12 s   | +128%  | +10%  | 38.5GFLOPS | 98% |
| Simd Double2 - FMA ilp=8    | 0.10 s   | +103%  | -     | 38.5GFLOPS |
| Simd Double4 - FMA + Div2   | 0.12 s   | +130%  | +1%   | 34.0GFLOPS |
| Simd Double2 - FMA ilp=7    | 0.11 s   | +107%  | +2%   | 33.0GFLOPS |
| Simd Double4 - Add ilp=5    | 0.16 s   | +205%  | +1%   | 32.0GFLOPS |
| Simd Double2 - FMA ilp=6    | 0.10 s   | +103%  | -     | 28.8GFLOPS |
| Simd Double4 - Add ilp=4    | 0.16 s   | +203%  | +32%  | 25.8GFLOPS |
| Simd Double2 - MulAdd       | 0.16 s   | +218%  | +4%   | 24.5GFLOPS |
| Simd Double2 - FMA ilp=5    | 0.21 s   | +303%  | -     | 24.2GFLOPS |
| Simd Double2 - FMA + Div    | 0.17 s   | +229%  | +3%   | 23.7GFLOPS |
| Simd Double2 - FMA + Add    | 0.19 s   | +275%  | +6%   | 20.8GFLOPS |
| Simd Double2 - Add ilp=7    | 0.18 s   | +252%  | +7%   | 19.4GFLOPS |
| Simd Double2 - Add ilp=8    | 0.21 s   | +303%  | +7%   | 19.4GFLOPS |
| Simd Double2 - Add ilp=6    | 0.23 s   | +353%  | +9%   | 19.4GFLOPS |
| Simd Double2 - Mul          | 0.21 s   | +304%  | -     | 19.3GFLOPS |
| Simd Double2 - FMA ilp=4    | 0.21 s   | +304%  | -     | 19.3GFLOPS |
| Simd Double4 - Fast Div     | 0.21 s   | +317%  | +3%   | 18.7GFLOPS |
| Simd Double2 - FMA + Div2   | 0.24 s   | +360%  | +2%   | 17.0GFLOPS |
| Simd Double2 - Add ilp=5    | 0.31 s   | +503%  | -     | 16.2GFLOPS |
| Simd Double2 - Add ilp=4    | 0.31 s   | +503%  | +31%  | 12.9GFLOPS |
| Simd Double2 - Fast Div     | 0.42 s   | +718%  | +35%  | 9.55GFLOPS |
| Simd Double4 - Div ilp=4    | 0.93 s   | +1724% | +123% | 4.28GFLOPS |
| Simd Double4 - Div ilp=8    | 1.03 s   | +1900% | +10%  | 3.90GFLOPS |
| Simd Double4 - Precise Sqrt | 1.73 s   | +3268% | +68%  | 2.32GFLOPS |
| Simd Double2 - Div ilp=4    | 1.87 s   | +3540% | +8%   | 2.14GFLOPS |
| Simd Double2 - Div ilp=8    | 2.05 s   | +3902% | +10%  | 1.95GFLOPS |
| Simd Double2 - Precise Sqrt | 3.46 s   | +6645% | +69%  | 1.16GFLOPS |


# Intel Ultra 7 255H, P-core

* Theoretical performance (FMA):
	- 2x fp32 256-bit FMA pipes
	- 32 FLOPS * 5.1 GHz = 163 GFLOPS per core

## Float SIMD (single thread)

| | time | diff | FLOPS | SOL |
|-------------------------------|----------|------|-------------|---|
| Simd Float8 - FMA ilp8        | 64.78 ms | +2%  | 123.5GFLOPS | 76% |
| Simd Float8 - FMA ilp8 + Div2 | 69.65 ms | +8%  | 114.9GFLOPS |
| Simd Float8 - FMA ilp7        | 63.63 ms | +1%  | 110.0GFLOPS |
| Simd Float8 - MulAdd          | 77.74 ms | +12% | 102.9GFLOPS |
| Simd Float8 - FMA ilp4 + Div4 | 80.59 ms | +4%  | 99.3GFLOPS  |
| Simd Float8 - FMA ilp6        | 63.07 ms | -    | 95.1GFLOPS  |
| Simd Float8 - FMA ilp8 + Add  | 90.41 ms | -    | 88.5GFLOPS  |
| Simd Float8 - FMA ilp5        | 0.12 s   | +5%  | 80.7GFLOPS  |
| Simd Float8 - Add ilp8        | 0.10 s   | +13% | 78.2GFLOPS  | 96% |
| Simd Float8 - Add ilp7        | 90.30 ms | +12% | 77.5GFLOPS  |
| Simd Float8 - Add ilp6        | 0.12 s   | +5%  | 76.0GFLOPS  |
| Simd Float8 - Add ilp5        | 0.13 s   | +7%  | 74.7GFLOPS  |
| Simd Float8 - Mul             | 0.11 s   | +9%  | 71.9GFLOPS  |
| Simd Float8 - Add ilp4        | 0.11 s   | +1%  | 70.9GFLOPS  |
| Simd Float8 - FMA ilp16       | 0.24 s   | +80% | 66.3GFLOPS  |
| Simd Float8 - FMA ilp4        | 0.12 s   | +1%  | 64.1GFLOPS  |
| Simd Float8 - Fast Sqrt       | 0.31 s   | +28% | 25.9GFLOPS  |
| Simd Float8 - Fast Div        | 0.32 s   | +5%  | 24.7GFLOPS  |
| Simd Float8 - Div ilp8        | 0.41 s   | +27% | 19.5GFLOPS  |
| Simd Float8 - Div ilp4        | 0.45 s   | +10% | 17.7GFLOPS  |
| Simd Float8 - Precise Sqrt    | 0.71 s   | +58% | 11.2GFLOPS  |

## Float SIMD (4 threads, 4 cores)

| | time | diff | FLOPS per thread | SOL |
|------------------------------|----------|------|-------------|---|
| Simd Float8 - FMA ilp8       | 72.65 ms | +4%  | 110.1GFLOPS | 67% |
| Simd Float8 - FMA ilp7       | 69.54 ms | -    | 100.7GFLOPS |
| Simd Float8 - FMA ilp6       | 69.26 ms | -    | 86.6GFLOPS  |
| Simd Float8 - FMA ilp8 + Add | 0.10 s   | +1%  | 79.3GFLOPS  |
| Simd Float8 - FMA ilp16      | 0.21 s   | +40% | 77.3GFLOPS  |
| Simd Float8 - FMA ilp5       | 0.14 s   | -    | 73.5GFLOPS  |
| Simd Float8 - Add ilp8       | 0.11 s   | +11% | 71.4GFLOPS  | 88% |
| Simd Float8 - Add ilp7       | 0.10 s   | +38% | 69.9GFLOPS  |
| Simd Float8 - Add ilp5       | 0.15 s   | +9%  | 67.7GFLOPS  |
| Simd Float8 - Add ilp6       | 0.13 s   | +6%  | 66.7GFLOPS  |
| Simd Float8 - Add ilp4       | 0.13 s   | +13% | 63.0GFLOPS  |
| Simd Float8 - FMA ilp4       | 0.14 s   | +1%  | 58.9GFLOPS  |

## Double SIMD (single thread)

| | time | diff | FLOPS | SOL |
|--------------------------------|----------|-------|------------|---|
| Simd Double4 - FMA ilp8        | 64.67 ms | -     | 61.9GFLOPS | 76% |
| Simd Double4 - FMA ilp7        | 64.39 ms | +1%   | 54.4GFLOPS |
| Simd Double4 - MulAdd          | 77.86 ms | +20%  | 51.4GFLOPS |
| Simd Double4 - FMA ilp8 + Div2 | 79.79 ms | +2%   | 50.1GFLOPS |
| Simd Double4 - FMA ilp6        | 63.45 ms | -     | 47.3GFLOPS |
| Simd Double4 - FMA ilp8 + Add  | 90.86 ms | -     | 44.0GFLOPS |
| Simd Double4 - FMA ilp5        | 0.12 s   | -     | 40.1GFLOPS |
| Simd Double4 - FMA ilp4 + Div4 | 99.93 ms | +10%  | 40.0GFLOPS |
| Simd Double4 - Add ilp8        | 0.10 s   | +2%   | 39.1GFLOPS | 96% |
| Simd Double4 - Add ilp7        | 90.53 ms | +13%  | 38.7GFLOPS |
| Simd Double4 - Add ilp6        | 0.12 s   | +5%   | 38.1GFLOPS |
| Simd Double4 - Add ilp5        | 0.13 s   | +8%   | 37.2GFLOPS |
| Simd Double4 - Mul             | 0.11 s   | +4%   | 36.0GFLOPS |
| Simd Double4 - Add ilp4        | 0.11 s   | +1%   | 35.5GFLOPS |
| Simd Double4 - FMA ilp16       | 0.24 s   | +2%   | 33.2GFLOPS |
| Simd Double4 - FMA ilp4        | 0.12 s   | +5%   | 32.2GFLOPS |
| Simd Double4 - Div ilp8        | 0.75 s   | +172% | 5.36GFLOPS |
| Simd Double4 - Div ilp4        | 0.80 s   | +8%   | 4.97GFLOPS |
| Simd Double4 - Precise Sqrt    | 1.22 s   | +52%  | 3.27GFLOPS |

## Float SIMD GEMM

**1024x1024x1024**

|         |  time    | diff  |     FLOPS    |  bandwidth | SOL |
|---------|----------|-------|--------------|------------|---|
| AVX2 v3 | 16.93 ms | -     | 126.8GFLOPS  |  991.0MB/s |
| AVX2 v2 | 22.65 ms | +34%  | 94.8GFLOPS   |  740.7MB/s |
| AE GEMM | 22.99 ms | +36%  | 93.3GFLOPS   |  729.6MB/s |
| AVX2 v5 | 33.99 ms | +101% | 63.1GFLOPS   |  493.6MB/s |
| AVX2 v4 | 34.38 ms | +103% | 62.4GFLOPS   |  488.0MB/s |
| AVX2 v1 | 48.00 ms | +184% | 44.7GFLOPS   |  349.5MB/s |

**128x128x128**

|         |  time    | diff |     FLOPS    | bandwidth | SOL |
|---------|----------|------|--------------|-----------|---|
| AE GEMM | 28.72 us | -    | 145.5GFLOPS  |  9.13GB/s |
| AVX2 v2 | 29.50 us | +3%  | 141.6GFLOPS  |  8.89GB/s |
| AVX2 v5 | 30.09 us | +5%  | 138.9GFLOPS  |  8.71GB/s |
| AVX2 v4 | 36.83 us | +28% | 113.5GFLOPS  |  7.12GB/s |
| AVX2 v3 | 39.51 us | +38% | 105.7GFLOPS  |  6.63GB/s |
| AVX2 v1 | 50.07 us | +74% | 83.4GFLOPS   |  5.24GB/s |

## Float SIMD GEMV

**2048x2048**

|         |   time  | diff |   FLOPS     | bandwidth | SOL |
|---------|---------|------|-------------|-----------|---|
| AVX2 v3 | 0.36 ms | -    | 23.4GFLOPS  |  46.8GB/s |
| AVX2 v2 | 0.37 ms | +2%  | 22.9GFLOPS  |  45.9GB/s |
| AVX2 v4 | 0.40 ms | +10% | 21.2GFLOPS  |  42.4GB/s |
| AVX2 v1 | 0.40 ms | +10% | 21.2GFLOPS  |  42.4GB/s |
| AE GEMV | 0.41 ms | +13% | 20.7GFLOPS  |  41.3GB/s |

**256x256**

|         |  time   | diff |    FLOPS    | bandwidth | SOL |
|---------|---------|------|-------------|-----------|---|
| AVX2 v3 | 1.80 us | -    | 72.8GFLOPS  |  146.7GB/s |
| AE GEMV | 2.03 us | +13% | 64.6GFLOPS  |  130.1GB/s |
| AVX2 v2 | 2.08 us | +15% | 63.1GFLOPS  |  127.3GB/s |
| AVX2 v4 | 2.33 us | +30% | 56.2GFLOPS  |  113.2GB/s |
| AVX2 v1 | 2.34 us | +30% | 55.9GFLOPS  |  112.7GB/s |



# Intel Ultra 7 255H, E-Core

* Theoretical performance (FMA):
	- 4x fp32 128-bit FMA pipes
	- 32 FLOPS * 4.4 GHz = 141 GFLOPS per core

## Float SIMD (single thread)

| | time | diff | FLOPS | SOL |
|-------------------------------|----------|-------|-------------|---|
| Simd Float8 - FMA ilp8        | 61.22 ms | +3%   | 130.7GFLOPS | 93% |
| Simd Float8 - FMA ilp7        | 63.20 ms | +3%   | 110.8GFLOPS |
| Simd Float8 - FMA ilp6        | 59.57 ms | -     | 100.7GFLOPS |
| Simd Float8 - FMA ilp5        | 0.12 s   | +1%   | 86.7GFLOPS  |
| Simd Float8 - FMA ilp16       | 0.21 s   | +48%  | 74.9GFLOPS  |
| Simd Float8 - Add ilp7        | 99.91 ms | +58%  | 70.1GFLOPS  | 99% |
| Simd Float8 - Add ilp8        | 0.11 s   | +14%  | 70.0GFLOPS  |
| Simd Float8 - MulAdd          | 0.11 s   | -     | 70.0GFLOPS  |
| Simd Float8 - FMA ilp4        | 0.11 s   | -     | 70.0GFLOPS  |
| Simd Float8 - Mul             | 0.11 s   | -     | 69.9GFLOPS  |
| Simd Float8 - Add ilp5        | 0.14 s   | +5%   | 69.9GFLOPS  |
| Simd Float8 - Add ilp6        | 0.13 s   | +8%   | 69.8GFLOPS  |
| Simd Float8 - Add ilp4        | 0.12 s   | +3%   | 67.3GFLOPS  |
| Simd Float8 - FMA ilp8 + Add  | 0.14 s   | +6%   | 58.8GFLOPS  |
| Simd Float8 - FMA ilp8 + Div2 | 0.14 s   | +1%   | 55.5GFLOPS  |
| Simd Float8 - FMA ilp4 + Div4 | 0.29 s   | +36%  | 27.6GFLOPS  |
| Simd Float8 - Fast Div        | 0.49 s   | +68%  | 16.4GFLOPS  |
| Simd Float8 - Fast Sqrt       | 0.49 s   | -     | 16.3GFLOPS  |
| Simd Float8 - Div ilp4        | 2.28 s   | +366% | 3.50GFLOPS  |
| Simd Float8 - Div ilp8        | 2.41 s   | +6%   | 3.32GFLOPS  |
| Simd Float8 - Precise Sqrt    | 2.78 s   | +15%  | 2.88GFLOPS  |

## Float SIMD (4 threads, 4 cores)

| | time | diff | FLOPS per thread | SOL |
|------------------------------|----------|------|-------------|---|
| Simd Float8 - FMA ilp8       | 63.59 ms | +3%  | 125.8GFLOPS | 89% |
| Simd Float8 - FMA ilp7       | 64.46 ms | +1%  | 108.6GFLOPS |
| Simd Float8 - FMA ilp6       | 61.64 ms | -    | 97.3GFLOPS  |
| Simd Float8 - FMA ilp5       | 0.12 s   | +1%  | 84.6GFLOPS  |
| Simd Float8 - FMA ilp16      | 0.21 s   | +43% | 76.9GFLOPS  |
| Simd Float8 - FMA ilp4       | 0.12 s   | +14% | 68.8GFLOPS  |
| Simd Float8 - Add ilp6       | 0.13 s   | +7%  | 68.8GFLOPS  | 98% |
| Simd Float8 - Add ilp7       | 0.10 s   | +59% | 68.5GFLOPS  |
| Simd Float8 - Add ilp5       | 0.15 s   | +5%  | 68.6GFLOPS  |
| Simd Float8 - Add ilp8       | 0.12 s   | +1%  | 68.5GFLOPS  |
| Simd Float8 - Add ilp4       | 0.12 s   | +3%  | 65.7GFLOPS  |
| Simd Float8 - FMA ilp8 + Add | 0.14 s   | +6%  | 57.6GFLOPS  |

## Double SIMD (single thread)

| | time | diff | FLOPS | SOL |
|--------------------------------|----------|-------|-------------|---|
| Simd Double4 - FMA ilp8        | 61.94 ms | +4%   | 64.6GFLOPS  | 92% |
| Simd Double4 - FMA ilp7        | 63.82 ms | +3%   | 54.8GFLOPS  |
| Simd Double4 - FMA ilp6        | 59.54 ms | -     | 50.4GFLOPS  |
| Simd Double4 - FMA ilp5        | 0.12 s   | -     | 43.2GFLOPS  |
| Simd Double4 - FMA ilp16       | 0.21 s   | +10%  | 37.3GFLOPS  |
| Simd Double4 - Add ilp7        | 0.10 s   | +57%  | 35.0GFLOPS  | 99% |
| Simd Double4 - Mul             | 0.11 s   | -     | 35.0GFLOPS  |
| Simd Double4 - MulAdd          | 0.11 s   | -     | 34.9GFLOPS  |
| Simd Double4 - FMA ilp4        | 0.11 s   | -     | 34.9GFLOPS  |
| Simd Double4 - Add ilp6        | 0.13 s   | +1%   | 34.9GFLOPS  |
| Simd Double4 - Add ilp8        | 0.12 s   | +1%   | 34.7GFLOPS  |
| Simd Double4 - Add ilp5        | 0.14 s   | +5%   | 34.7GFLOPS  |
| Simd Double4 - Add ilp4        | 0.13 s   | +10%  | 31.4GFLOPS  |
| Simd Double4 - FMA ilp8 + Add  | 0.14 s   | +6%   | 29.2GFLOPS  |
| Simd Double4 - FMA ilp8 + Div2 | 0.23 s   | -     | 17.3GFLOPS  |
| Simd Double4 - FMA ilp4 + Div4 | 0.46 s   | +42%  | 8.68GFLOPS  |
| Simd Double4 - Div ilp8        | 3.69 s   | -     | 1.08GFLOPS  |
| Simd Double4 - Div ilp4        | 3.71 s   | +1%   | 1.08GFLOPS  |
| Simd Double4 - Precise Sqrt    | 5.55 s   | +1%   | 721.0MFLOPS |

## Float SIMD GEMM

**1024x1024x1024**

|         |  time    | diff  |    FLOPS    |  bandwidth | SOL |
|---------|----------|------|-------------|------------|---|
| AVX2 v3 | 26.58 ms | -    | 80.7GFLOPS  |  631.1MB/s |
| AVX2 v2 | 30.86 ms | +16% | 69.5GFLOPS  |  543.6MB/s |
| AE GEMM | 31.70 ms | +19% | 67.7GFLOPS  |  529.3MB/s |
| AVX2 v5 | 32.74 ms | +23% | 65.6GFLOPS  |  512.4MB/s |
| AVX2 v1 | 39.80 ms | +50% | 53.9GFLOPS  |  421.5MB/s |
| AVX2 v4 | 41.73 ms | +57% | 51.4GFLOPS  |  402.0MB/s |

**128x128x128**

|         |  time    | diff |    FLOPS    | bandwidth | SOL |
|---------|----------|------|-------------|-----------|---|
| AVX2 v2 | 47.20 us | -    | 88.5GFLOPS  |  5.55GB/s |
| AVX2 v5 | 47.72 us | +1%  | 87.6GFLOPS  |  5.49GB/s |
| AE GEMM | 47.98 us | +2%  | 87.1GFLOPS  |  5.46GB/s |
| AVX2 v4 | 58.52 us | +24% | 71.4GFLOPS  |  4.48GB/s |
| AVX2 v1 | 68.81 us | +46% | 60.7GFLOPS  |  3.81GB/s |
| AVX2 v3 | 73.73 us | +56% | 56.7GFLOPS  |  3.56GB/s |

## Float SIMD GEMV

**2048x2048**

|         |  time   |  diff |   FLOPS     | bandwidth | SOL |
|---------|---------|-------|-------------|-----------|---|
| AVX2 v3 | 0.32 ms | -     | 26.5GFLOPS  |  53.0GB/s |
| AVX2 v2 | 0.33 ms | +3%   | 25.7GFLOPS  |  51.5GB/s |
| AVX2 v4 | 0.34 ms | +7%   | 24.7GFLOPS  |  49.4GB/s |
| AE GEMV | 0.36 ms | +12%  | 23.6GFLOPS  |  47.2GB/s |
| AVX2 v1 | 0.71 ms | +123% | 11.9GFLOPS  |  23.8GB/s |

**256x256**

|         |  time   | diff |    FLOPS    |  bandwidth | SOL |
|---------|---------|------|-------------|------------|---|
| AVX2 v3 | 2.46 us | -    | 53.3GFLOPS  |  107.5GB/s |
| AVX2 v2 | 2.50 us | +2%  | 52.4GFLOPS  |  105.7GB/s |
| AE GEMV | 2.66 us | +8%  | 49.3GFLOPS  |  99.3GB/s  |
| AVX2 v1 | 3.30 us | +34% | 39.7GFLOPS  |  80.0GB/s  |
| AVX2 v4 | 3.32 us | +35% | 39.5GFLOPS  |  79.6GB/s  |



# Intel Ultra 7 255H, LPE-Core

* Skymont microarchitecture
* Theoretical performance (FMA):
	- 2x fp32 128-bit FMA pipes
	- 16 FLOPS * 2.5 GHz = 40 GFLOPS per core

## Float SIMD (single thread)

| | time | diff | FLOPS | SOL |
|-------------------------------|--------|-------|-------------|---|
| Simd Float8 - FMA ilp6        | 0.15 s | -     | 39.5GFLOPS  | 99% |
| Simd Float8 - FMA ilp7        | 0.18 s | +17%  | 39.3GFLOPS  |
| Simd Float8 - FMA ilp8        | 0.20 s | +15%  | 39.2GFLOPS  |
| Simd Float8 - FMA ilp16       | 0.48 s | +6%   | 33.2GFLOPS  |
| Simd Float8 - FMA ilp5        | 0.31 s | -     | 32.7GFLOPS  |
| Simd Float8 - FMA ilp4        | 0.31 s | +50%  | 26.2GFLOPS  |
| Simd Float8 - Add ilp7        | 0.35 s | +16%  | 19.8GFLOPS  | 99% |
| Simd Float8 - Add ilp4        | 0.40 s | +14%  | 19.8GFLOPS  |
| Simd Float8 - Add ilp5        | 0.51 s | -     | 19.8GFLOPS  |
| Simd Float8 - Add ilp8        | 0.40 s | -     | 19.8GFLOPS  |
| Simd Float8 - Mul             | 0.41 s | -     | 19.7GFLOPS  |
| Simd Float8 - Add ilp6        | 0.46 s | +12%  | 19.7GFLOPS  |
| Simd Float8 - MulAdd          | 0.41 s | -     | 19.6GFLOPS  |
| Simd Float8 - FMA ilp8 + Add  | 0.48 s | -     | 16.6GFLOPS  |
| Simd Float8 - FMA ilp8 + Div2 | 0.50 s | +4%   | 15.9GFLOPS  |
| Simd Float8 - FMA ilp4 + Div4 | 1.02 s | +101% | 7.87GFLOPS  |
| Simd Float8 - Fast Sqrt       | 2.02 s | +99%  | 3.96GFLOPS  |
| Simd Float8 - Fast Div        | 2.04 s | +1%   | 3.93GFLOPS  |
| Simd Float8 - Div ilp8        | 8.08 s | +297% | 989.7MFLOPS |
| Simd Float8 - Div ilp4        | 8.09 s | -     | 989.2MFLOPS |
| Simd Float8 - Precise Sqrt    | 9.69 s | +20%  | 825.5MFLOPS |

## Float SIMD (2 threads, 2 cores)

| | time | diff | FLOPS per thread | SOL |
|------------------------------|--------|------|------------|---|
| Simd Float8 - FMA ilp6       | 0.15 s | -    | 39.5GFLOPS | 99% |
| Simd Float8 - FMA ilp7       | 0.18 s | +16% | 39.6GFLOPS |
| Simd Float8 - FMA ilp8       | 0.20 s | +14% | 39.5GFLOPS |
| Simd Float8 - FMA ilp16      | 0.46 s | +2%  | 34.5GFLOPS |
| Simd Float8 - FMA ilp5       | 0.31 s | +1%  | 32.1GFLOPS |
| Simd Float8 - FMA ilp4       | 0.31 s | +53% | 25.9GFLOPS |
| Simd Float8 - Add ilp7       | 0.35 s | +14% | 19.8GFLOPS | 99% |
| Simd Float8 - Add ilp4       | 0.40 s | +14% | 19.8GFLOPS |
| Simd Float8 - Add ilp6       | 0.45 s | +11% | 19.8GFLOPS |
| Simd Float8 - Add ilp5       | 0.51 s | +6%  | 19.7GFLOPS |
| Simd Float8 - Add ilp8       | 0.41 s | +1%  | 19.5GFLOPS |
| Simd Float8 - FMA ilp8 + Add | 0.48 s | +3%  | 16.8GFLOPS |

## Double SIMD (single thread)

| | time | diff | FLOPS | SOL |
|--------------------------------|---------|-------|-------------|---|
| Simd Double4 - FMA ilp6        | 0.15 s  | -     | 19.8GFLOPS  | 99% |
| Simd Double4 - FMA ilp7        | 0.18 s  | +17%  | 19.8GFLOPS  |
| Simd Double4 - FMA ilp8        | 0.20 s  | +14%  | 19.8GFLOPS  |
| Simd Double4 - FMA ilp16       | 0.48 s  | +5%   | 16.7GFLOPS  |
| Simd Double4 - FMA ilp5        | 0.30 s  | -     | 16.4GFLOPS  |
| Simd Double4 - FMA ilp4        | 0.30 s  | +50%  | 13.2GFLOPS  |
| Simd Double4 - Add ilp7        | 0.35 s  | +3%   | 9.92GFLOPS  | 99% |
| Simd Double4 - Mul             | 0.40 s  | +7%   | 9.92GFLOPS  |
| Simd Double4 - MulAdd          | 0.40 s  | -     | 9.92GFLOPS  |
| Simd Double4 - Add ilp6        | 0.45 s  | +4%   | 9.92GFLOPS  |
| Simd Double4 - Add ilp5        | 0.50 s  | +3%   | 9.92GFLOPS  |
| Simd Double4 - Add ilp4        | 0.40 s  | -     | 9.91GFLOPS  |
| Simd Double4 - Add ilp8        | 0.40 s  | -     | 9.91GFLOPS  |
| Simd Double4 - FMA ilp8 + Add  | 0.48 s  | -     | 8.34GFLOPS  |
| Simd Double4 - FMA ilp8 + Div2 | 0.81 s  | +1%   | 4.96GFLOPS  |
| Simd Double4 - FMA ilp4 + Div4 | 1.61 s  | +96%  | 2.48GFLOPS  |
| Simd Double4 - Div ilp8        | 12.91 s | +699% | 309.8MFLOPS |
| Simd Double4 - Div ilp4        | 12.93 s | -     | 309.4MFLOPS |
| Simd Double4 - Precise Sqrt    | 19.37 s | +50%  | 206.5MFLOPS |

## Float SIMD GEMM

**1024x1024x1024**

|         |  time    | diff  |    FLOPS    |  bandwidth | SOL |
|---------|----------|-------|-------------|------------|---|
| AVX2 v3 | 84.38 ms | -     | 25.4GFLOPS  |  198.8MB/s |
| AVX2 v1 | 0.13 s   | +55%  | 16.5GFLOPS  |  128.7MB/s |
| AVX2 v2 | 0.59 s   | +600% | 3.63GFLOPS  |  28.4MB/s  |
| AE GEMM | 0.60 s   | +611% | 3.58GFLOPS  |  28.0MB/s  |
| AVX2 v4 | 0.85 s   | +903% | 2.54GFLOPS  |  19.8MB/s  |
| AVX2 v5 | 0.87 s   | +934% | 2.46GFLOPS  |  19.2MB/s  |

**128x128x128**

|         |  time   | diff |    FLOPS    | bandwidth | SOL |
|---------|---------|------|-------------|-----------|---|
| AVX2 v5 | 0.12 ms | -    | 35.0GFLOPS  |  2.20GB/s |
| AE GEMM | 0.13 ms | +5%  | 33.4GFLOPS  |  2.09GB/s |
| AVX2 v2 | 0.13 ms | +6%  | 33.0GFLOPS  |  2.07GB/s |
| AVX2 v4 | 0.16 ms | +35% | 26.0GFLOPS  |  1.63GB/s |
| AVX2 v1 | 0.17 ms | +40% | 25.0GFLOPS  |  1.57GB/s |
| AVX2 v3 | 0.22 ms | +80% | 19.4GFLOPS  |  1.22GB/s |

## Float SIMD GEMV

**2048x2048**

|         |  time   | diff  |   FLOPS     | bandwidth | SOL |
|---------|---------|-------|-------------|-----------|---|
| AE GEMV | 1.60 ms | -     | 5.26GFLOPS  |  10.5GB/s |
| AVX2 v2 | 1.82 ms | +14%  | 4.60GFLOPS  |  9.21GB/s |
| AVX2 v3 | 1.98 ms | +24%  | 4.24GFLOPS  |  8.48GB/s |
| AVX2 v4 | 3.93 ms | +146% | 2.14GFLOPS  |  4.28GB/s |
| AVX2 v1 | 3.93 ms | +146% | 2.13GFLOPS  |  4.27GB/s |

**256x256**

|         |  time   | diff |    FLOPS    | bandwidth | SOL |
|---------|---------|------|-------------|-----------|---|
| AVX2 v3 | 5.74 us | -    | 22.8GFLOPS  |  46.0GB/s |
| AE GEMV | 5.79 us | +1%  | 22.6GFLOPS  |  45.6GB/s |
| AVX2 v2 | 5.86 us | +2%  | 22.4GFLOPS  |  45.1GB/s |
| AVX2 v4 | 9.63 us | +68% | 13.6GFLOPS  |  27.4GB/s |
| AVX2 v1 | 9.99 us | +74% | 13.1GFLOPS  |  26.4GB/s |


# Intel Ultra 7 255H, per core

Simd Float8 FMA ilp8 test.

| core | type | FLOPS | SOL |
|---|---|---|---|
|  0 | P  | 104.3GFLOPS | 64% |
|  1 | P  | 111.1GFLOPS | 68% |
|  2 | E  | 128.6GFLOPS | 91% |
|  3 | E  | 127.0GFLOPS | 90% |
|  4 | E  | 128.2GFLOPS | 91% |
|  5 | E  | 127.1GFLOPS | 90% |
|  6 | E  | 129.3GFLOPS | 92% |
|  7 | E  | 129.6GFLOPS | 92% |
|  8 | E  | 129.1GFLOPS | 92% |
|  9 | E  | 129.5GFLOPS | 92% |
| 10 | P  | 115.1GFLOPS | 70% |
| 11 | P  | 111.0GFLOPS | 68% |
| 12 | P  | 115.4GFLOPS | 71% |
| 13 | P  | 113.0GFLOPS | 69% |
| 14 | LP | 39.7GFLOPS  | 99% |
| 15 | LP | 39.4GFLOPS  | 99% |



# Pico 4 Ultra P-core

* Arch: ARM Cortex A78C
* Clock: 2.36 GHz
* Cores: 4
* Theoretical performance (FMA):
	- 2 FMA/cy, SIMD size: 4, 8 FMA/cy total
	- 2.36 G * 2 * 4 = 19 GFLOPS
* Results:
	- only 1 P-core is available

## Float SIMD (single thread)

| | FLOPS | SOL |
|-------------------------------|----------------|
| Simd Float4 - FMA ilp4        | 18.7GFLOPS     | 98% |
| Simd Float4 - FMA ilp5        | 16.5GFLOPS     |
| Simd Float4 - FMA ilp16       | 16.5GFLOPS     |
| Simd Float4 - FMA ilp8        | 15.5GFLOPS     |
| Simd Float4 - MulAdd          | 15.3GFLOPS     |
| Simd Float4 - FMA ilp6        | 15.2GFLOPS     |
| Simd Float4 - FMA ilp7        | 15.0GFLOPS     |
| Simd Float4 - Fast Sqrt       | 4.68GFLOPS     |
| Simd Float4 - Fast Div        | 4.68GFLOPS     |
| Simd Float4 - Precise Sqrt    | 1.34GFLOPS     |

## Float SIMD (4 threads, 4 cores)

| | FLOPS per thread | SOL |
|------------------------------|------------|
| Simd Float4 - FMA ilp4       | 4.99GFLOPS | 25% (100% for single thread) |
| Simd Float4 - FMA ilp5       | 4.40GFLOPS |
| Simd Float4 - FMA ilp16      | 4.35GFLOPS |
| Simd Float4 - FMA ilp8       | 4.11GFLOPS |
| Simd Float4 - FMA ilp6       | 4.06GFLOPS |
| Simd Float4 - FMA ilp7       | 4.04GFLOPS |


# Pico 4 Ultra E-core

* Arch: ARM Cortex A78C
* Clock: 2.0 GHz
* Cores: 2
* Theoretical performance (FMA):
	- 2 FMA/cy, SIMD size: 4, 8 FMA/cy total
	- 2.0 G * 2 * 4 = 16 GFLOPS
* Results:
	- all 2 E-cores are available

## Float SIMD (single thread)

| | FLOPS | SOL |
|-------------------------------|----------------|
| Simd Float4 - FMA ilp4        | 15.2GFLOPS     | 95% |
| Simd Float4 - FMA ilp5        | 13.4GFLOPS     |
| Simd Float4 - FMA ilp16       | 13.4GFLOPS     |
| Simd Float4 - FMA ilp8        | 12.6GFLOPS     |
| Simd Float4 - MulAdd          | 12.4GFLOPS     |
| Simd Float4 - FMA ilp6        | 12.3GFLOPS     |
| Simd Float4 - FMA ilp7        | 12.2GFLOPS     |
| Simd Float4 - Fast Div        | 3.81GFLOPS     |
| Simd Float4 - Fast Sqrt       | 3.80GFLOPS     |
| Simd Float4 - Precise Sqrt    | 1.09GFLOPS     |

## Float SIMD (2 threads, 2 cores)

| | FLOPS per thread | SOL |
|------------------------------|------------|
| Simd Float4 - FMA ilp4       | 14.7GFLOPS | 92% |
| Simd Float4 - FMA ilp5       | 13.1GFLOPS |
| Simd Float4 - FMA ilp16      | 13.0GFLOPS |
| Simd Float4 - FMA ilp8       | 12.3GFLOPS |
| Simd Float4 - FMA ilp6       | 11.9GFLOPS |
| Simd Float4 - FMA ilp7       | 11.8GFLOPS |


# Snapdragon 8 Elite gen 5 Prime-core

* Arch: Oryon Gen 3 Prime core
* Clock: 4.6 GHz
* Cores: 2
* Theoretical performance (FMA):
	- 4x fp32 128-bit FMA pipes
	- 32 FLOPS * 4.6 G = 147 GFLOPS
	- 294 GFLOPS for all cores

## Float SIMD (single thread)

| | FLOPS | SOL |
|-------------------------------|----------------|
| Simd Float4 - Mul             | 115.8GFLOPS    |
| Simd Float4 - Add ilp6        | 38.6GFLOPS     |
| Simd Float4 - MulAdd          | 28.6GFLOPS     |
| Simd Float4 - FMA ilp16       | 27.6GFLOPS     |
| Simd Float4 - Div ilp8        | 26.0GFLOPS     |
| Simd Float4 - FMA ilp8        | 26.0GFLOPS     |
| Simd Float4 - Div ilp4        | 26.0GFLOPS     |
| Simd Float4 - FMA ilp6        | 19.5GFLOPS     |
| Simd Float4 - FMA ilp7        | 22.5GFLOPS     |
| Simd Float4 - FMA ilp4        | 13.0GFLOPS     |
| Simd Float4 - FMA ilp5        | 16.0GFLOPS     |
| Simd Float4 - Fast Sqrt       | 6.50GFLOPS     |
| Simd Float4 - Fast Div        | 5.77GFLOPS     |
| Simd Float4 - Precise Sqrt    | 3.25GFLOPS     |

## Float SIMD (2 threads, 2 cores)

| | FLOPS per thread | SOL |
|------------------------------|------------|
| Simd Float4 - Add ilp6       | 36.6GFLOPS |
| Simd Float4 - FMA ilp6       | 18.4GFLOPS |
| Simd Float4 - FMA ilp8       | 24.5GFLOPS |
| Simd Float4 - FMA ilp7       | 21.3GFLOPS |
| Simd Float4 - FMA ilp16      | 45.0GFLOPS |
| Simd Float4 - FMA ilp8 + Add | 13.3GFLOPS |
| Simd Float4 - FMA ilp4       | 12.7GFLOPS |
| Simd Float4 - FMA ilp5       | 15.6GFLOPS |

## Float SIMD GEMM

**1024x1024x1024**

|         |  time    | diff  |    FLOPS     |  bandwidth | SOL |
|---------|----------|-------|--------------|------------|---|
| NEON v1 | 21.15 ms | -     | 101.5GFLOPS  |  793.2MB/s |
| AE GEMM | 32.44 ms | +53%  | 66.2GFLOPS   |  517.2MB/s |
| NEON v3 | 45.87 ms | +117% | 46.8GFLOPS   |  365.8MB/s |

**128x128x128**

|         |  time    | diff  |    FLOPS   | bandwidth | SOL |
|---------|----------|------|-------------|-----------|---|
| AE GEMM | 64.98 us | -    | 64.3GFLOPS  |  4.03GB/s |
| NEON v1 | 65.55 us | +1%  | 63.7GFLOPS  |  4.00GB/s |
| NEON v3 | 86.06 us | +32% | 48.5GFLOPS  |  3.05GB/s |

## Float SIMD GEMV

**2048x2048**

|         |  time   | diff  |    FLOPS    | bandwidth | SOL |
|---------|---------|-------|-------------|-----------|---|
| NEON v3 | 0.25 ms | -     | 33.7GFLOPS  |  67.6GB/s |
| NEON v4 | 0.27 ms | +7%   | 31.4GFLOPS  |  62.9GB/s |
| NEON v1 | 0.28 ms | +13%  | 30.0GFLOPS  |  60.0GB/s |
| NEON v2 | 0.84 ms | +238% | 9.98GFLOPS  |  20.0GB/s |

**256x256**

|         |  time   | diff  |    FLOPS    | bandwidth | SOL |
|---------|---------|-------|-------------|-----------|---|
| NEON v1 | 2.85 us | -     | 45.9GFLOPS  |  92.6GB/s |
| NEON v3 | 5.12 us | +80%  | 25.6GFLOPS  |  51.6GB/s |
| NEON v4 | 6.32 us | +122% | 20.7GFLOPS  |  41.8GB/s |
| NEON v2 | 7.64 us | +168% | 17.2GFLOPS  |  34.6GB/s |


# Snapdragon 8 Elite gen 5 P-core

* Arch: Oryon Gen 3 Performance core
* Clock: 3.6 GHz
* Cores: 6
* Theoretical performance (FMA):
	- 4x fp32 128-bit FMA pipes ?
	- 32 FLOPS * 3.6 G = 115 GFLOPS
	- 691 GFLOPS for all cores

## Float SIMD (single thread)

| | FLOPS | SOL |
|-------------------------------|----------------|
| Simd Float4 - Mul             | 65.5GFLOPS     |
| Simd Float4 - Add ilp6        | 50.7GFLOPS     |
| Simd Float4 - Div ilp8        | 34.1GFLOPS     |
| Simd Float4 - Div ilp4        | 34.0GFLOPS     |
| Simd Float4 - FMA ilp8        | 32.1GFLOPS     |
| Simd Float4 - MulAdd          | 32.0GFLOPS     |
| Simd Float4 - FMA ilp16       | 29.5GFLOPS     |
| Simd Float4 - FMA ilp7        | 29.1GFLOPS     |
| Simd Float4 - FMA ilp6        | 25.5GFLOPS     |
| Simd Float4 - FMA ilp5        | 20.6GFLOPS     |
| Simd Float4 - FMA ilp4        | 17.0GFLOPS     |
| Simd Float4 - Fast Sqrt       | 8.25GFLOPS     |
| Simd Float4 - Fast Div        | 6.19GFLOPS     |
| Simd Float4 - Precise Sqrt    | 4.26GFLOPS     |

## Float SIMD (6 threads, 6 cores)

| | FLOPS per thread | SOL |
|------------------------------|------------|
| Simd Float4 - Add ilp6       | 45.4GFLOPS |
| Simd Float4 - FMA ilp8       | 29.5GFLOPS |
| Simd Float4 - FMA ilp7       | 26.8GFLOPS |
| Simd Float4 - FMA ilp6       | 23.3GFLOPS |
| Simd Float4 - FMA ilp16      | 28.6GFLOPS |
| Simd Float4 - FMA ilp5       | 19.3GFLOPS |
| Simd Float4 - FMA ilp8 + Add | 16.0GFLOPS |
| Simd Float4 - FMA ilp4       | 15.9GFLOPS |

## Float SIMD GEMM

**1024x1024x1024**

|         |  time    | diff |    FLOPS    |  bandwidth | SOL |
|---------|----------|------|-------------|------------|---|
| AE GEMM | 67.60 ms | -    | 31.8GFLOPS  |  248.2MB/s |
| NEON v1 | 68.92 ms | +2%  | 31.1GFLOPS  |  243.4MB/s |
| NEON v3 | 98.16 ms | +45% | 21.9GFLOPS  |  170.9MB/s |

**128x128x128**

|         |  time   | diff |    FLOPS    | bandwidth | SOL |
|---------|---------|------|-------------|-----------|---|
| AE GEMM | 0.13 ms | -    | 31.5GFLOPS  |  1.98GB/s |
| NEON v1 | 0.13 ms | -    | 31.4GFLOPS  |  1.97GB/s |
| NEON v3 | 0.19 ms | +42% | 22.1GFLOPS  |  1.39GB/s |


## Float SIMD GEMV

**2048x2048**

|         |  time   | diff  |    FLOPS    | bandwidth | SOL |
|---------|---------|-------|-------------|-----------|---|
| NEON v3 | 0.56 ms | -     | 15.0GFLOPS  |  29.9GB/s |
| NEON v4 | 0.71 ms | +27%  | 11.8GFLOPS  |  23.6GB/s |
| NEON v1 | 0.87 ms | +56%  | 9.60GFLOPS  |  19.2GB/s |
| NEON v2 | 1.41 ms | +151% | 5.95GFLOPS  |  11.9GB/s |

**256x256**

|         |  time    | diff |    FLOPS    | bandwidth | SOL |
|---------|----------|------|-------------|-----------|---|
| NEON v3 | 8.86 us  | -    | 14.8GFLOPS  |  29.8GB/s |
| NEON v1 | 8.95 us  | +1%  | 14.6GFLOPS  |  29.5GB/s |
| NEON v4 | 11.40 us | +29% | 11.5GFLOPS  |  23.2GB/s |
| NEON v2 | 17.57 us | +98% | 7.46GFLOPS  |  15.0GB/s |
