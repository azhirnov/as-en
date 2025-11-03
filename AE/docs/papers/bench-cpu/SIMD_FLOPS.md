Performance per thread.

[source](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_SIMD.cpp)

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
* [ARM Cortex A76 (MediaTek Helio G96)](#ARM-Cortex-A76-MediaTek-Helio-G96)
* [ARM Cortex A55 (MediaTek Helio G96)](#ARM-Cortex-A55-MediaTek-Helio-G96)
* [ARM Cortex A55 (MediaTek Dimensity 7020)](#ARM-Cortex-A55-MediaTek-Dimensity-7020)
* [ARM Cortex A55 (Snapdragon 888)](#ARM-Cortex-A55-Snapdragon-888)
* [ARM Cortex A53 (Samsung Exynos 7870)](#ARM-Cortex-A53-Samsung-Exynos-7870)
* [ARM Cortex A53, P-core (Snapdragon 439)](#ARM-Cortex-A53-P-core-Snapdragon-439)
* [ARM Cortex A53, E-core (Snapdragon 439)](#ARM-Cortex-A53-E-core-Snapdragon-439)

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

| | time | diff | delta | FLOPS | SOL |
|----------------------------|----------|--------|-------|-------------|---|
| Simd Float8 - FMA ilp=8    | 72.73 ms | +1%    | +1%   | 110.0GFLOPS | 82% |
| Simd Float8 - FMA ilp=7    | 75.63 ms | +5%    | +4%   | 92.6GFLOPS  | 
| Simd Float8 - MulAdd       | 87.15 ms | +21%   | +15%  | 91.8GFLOPS  | 
| Simd Float8 - FMA ilp=6    | 72.00 ms | -      | -     | 83.3GFLOPS  | 
| Simd Float8 - FMA ilp=16   | 0.20 s   | +182%  | +8%   | 78.7GFLOPS  | 
| Simd Float8 - FMA ilp=5    | 0.14 s   | +100%  | -     | 69.5GFLOPS  | 
| Simd Float8 - Mul          | 0.12 s   | +60%   | +14%  | 69.3GFLOPS  | 103% |
| Simd Float8 - Add ilp=6    | 0.13 s   | +81%   | +1%   | 69.2GFLOPS  | 103% |
| Simd Float8 - Add ilp=7    | 0.10 s   | +41%   | +16%  | 69.1GFLOPS  | 
| Simd Float8 - Add ilp=8    | 0.12 s   | +61%   | -     | 69.1GFLOPS  | 
| Simd Float8 - FMA + Div    | 0.12 s   | +72%   | +7%   | 64.8GFLOPS  | 
| Simd Float8 - FMA + Add    | 0.13 s   | +80%   | +5%   | 61.9GFLOPS  | 
| Simd Float8 - FMA + Div2   | 0.14 s   | +99%   | +10%  | 55.7GFLOPS  | 
| Simd Float8 - FMA ilp=4    | 0.14 s   | +99%   | -     | 55.7GFLOPS  | 
| Simd Float8 - Add ilp=5    | 0.19 s   | +162%  | +9%   | 53.0GFLOPS  | 
| Simd Float8 - Add ilp=4    | 0.17 s   | +140%  | +20%  | 46.3GFLOPS  | 
| Simd Float8 - Fast Div     | 0.25 s   | +251%  | +24%  | 31.6GFLOPS  | 
| Simd Float8 - Fast Sqrt    | 0.26 s   | +263%  | +3%   | 30.6GFLOPS  | 
| Simd Float8 - Div ilp=8    | 0.80 s   | +1009% | +206% | 10.0GFLOPS  | 89% |
| Simd Float8 - Div ilp=4    | 0.80 s   | +1009% | -     | 10.0GFLOPS  | 89% |
| Simd Float8 - Precise Sqrt | 1.25 s   | +1642% | +57%  | 6.38GFLOPS  |

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
* Performance: 43.2 GFLOPS per CPU [ref](https://gadgetversus.com/processor/amd-phenom-ii-x4-945-specs/)
* Theoretical performance (FMA):
	- 2 FLOPS * 4 Op/cy * 3 GHz = 24 GFLOPS per core
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
* Performance: 163 GFLOPS per CPU in SGEMM [ref](https://gadgetversus.com/processor/intel-core-i5-8250u-specs/)
* Theoretical performance (FMA):
	- 2 FLOPS * 2*8 Op/cy * 3.39 GHz = 108.5 GFLOPS per core
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
* Performance: 144 GFLOPS per CPU [ref](https://gadgetversus.com/processor/qualcomm-sm8350-snapdragon-888-5g-specs/)
* Notes:
	- 4x128b NEON [ref](https://www.anandtech.com/show/15813/arm-cortex-a78-cortex-x1-cpu-ip-diverging/3)
* Theoretical performance (FMA):
	- 2 FLOPS * 4*2 Op/cy * 2.8 GHz = 44.8 GFLOPS per core

## Float SIMD

```
Scalar Float4 - seq1 MulAdd: 33.01 ms           - 38.8GFLOPS,  19.4GB/s
Scalar Float4 - Mul        : 58.01 ms  +75.8%   - 22.1GFLOPS,  11.0GB/s
Scalar Float4 - AddMul     : 77.54 ms  +134.9%  - 16.5GFLOPS,  8.3GB/s
Scalar Float4 - Add        : 82.08 ms  +148.7%  - 15.6GFLOPS,  7.8GB/s
Scalar Float4 - AddMul2    : 95.38 ms  +189.0%  - 13.4GFLOPS,  6.7GB/s
Scalar Float4 - MulAdd     : 0.12 s  +266.3%    - 10.6GFLOPS,  5.3GB/s
Scalar Float4 - FMA        : 0.13 s  +283.4%    - 10.1GFLOPS,  5.1GB/s
Scalar Float4 - seq1 Add   : 0.13 s  +294.7%    - 9.8GFLOPS,   4.9GB/s
Scalar Float4 - Div        : 0.20 s  +492.9%    - 3.3GFLOPS,   3.3GB/s
Scalar Float4 - Sqrt       : 0.22 s  +577.3%    - 2.9GFLOPS,   2.9GB/s
Scalar Float4 - seq1 Div   : 0.42 s  +1162.4%   - 1.5GFLOPS,   1.5GB/s

Simd Float4 - Add        : 33.19 ms             - 38.6GFLOPS,  19.3GB/s
Simd Float4 - FMA        : 42.80 ms  +29.0%     - 29.9GFLOPS,  15.0GB/s
Simd Float4 - Mul        : 43.92 ms  +32.3%     - 29.1GFLOPS,  14.6GB/s
Simd Float4 - seq1 MulAdd: 52.74 ms  +58.9%     - 24.3GFLOPS,  12.1GB/s
Simd Float4 - AddMul2    : 85.00 ms  +156.1%    - 15.1GFLOPS,  7.5GB/s
Simd Float4 - AddMul     : 97.69 ms  +194.4%    - 13.1GFLOPS,  6.6GB/s
Simd Float4 - MulAdd     : 0.13 s  +288.8%      - 9.9GFLOPS,   5.0GB/s
Simd Float4 - seq1 Add   : 0.15 s  +349.7%      - 8.6GFLOPS,   4.3GB/s
Simd Float4 - Sqrt       : 0.17 s  +405.3%      - 3.8GFLOPS,   3.8GB/s
Simd Float4 - Div        : 0.19 s  +482.1%      - 3.3GFLOPS,   3.3GB/s
Simd Float4 - seq1 Div   : 0.40 s  +1094.3%     - 1.6GFLOPS,   1.6GB/s
```

## Double SIMD

```
Scalar Double2 - seq1 MulAdd: 32.31 ms             - 19.8GFLOPS,  19.8GB/s
Scalar Double4 - MulAdd     : 35.04 ms  +8.4%      - 18.3GFLOPS,  18.3GB/s
Scalar Double2 - Add        : 40.72 ms  +26.0%     - 15.7GFLOPS,  15.7GB/s
Scalar Double4 - seq1 MulAdd: 40.76 ms  +26.1%     - 15.7GFLOPS,  15.7GB/s
Scalar Double4 - seq1 Add   : 57.33 ms  +77.4%     - 11.2GFLOPS,  11.2GB/s
Scalar Double2 - Mul        : 58.07 ms  +79.7%     - 11.0GFLOPS,  11.0GB/s
Scalar Double4 - AddMul2    : 65.01 ms  +101.2%    - 9.8GFLOPS,  9.8GB/s
Scalar Double2 - MulAdd     : 66.10 ms  +104.6%    - 9.7GFLOPS,  9.7GB/s
Scalar Double4 - Add        : 68.88 ms  +113.2%    - 9.3GFLOPS,  9.3GB/s
Scalar Double4 - Mul        : 80.68 ms  +149.7%    - 7.9GFLOPS,  7.9GB/s
Scalar Double4 - FMA        : 96.49 ms  +198.6%    - 6.6GFLOPS,  6.6GB/s
Scalar Double2 - AddMul2    : 99.52 ms  +208.0%    - 6.4GFLOPS,  6.4GB/s
Scalar Double2 - AddMul     : 99.76 ms  +208.7%    - 6.4GFLOPS,  6.4GB/s
Scalar Double4 - AddMul     : 0.10 s  +220.1%      - 6.2GFLOPS,  6.2GB/s
Scalar Double2 - FMA        : 0.13 s  +296.8%      - 5.0GFLOPS,  5.0GB/s
Scalar Double2 - seq1 Add   : 0.13 s  +312.7%      - 4.8GFLOPS,  4.8GB/s
Scalar Double4 - seq1 Div   : 0.20 s  +527.0%      - 1.6GFLOPS,  3.2GB/s
Scalar Double4 - Div        : 0.27 s  +747.1%      - 1.2GFLOPS,  2.3GB/s
Scalar Double2 - Div        : 0.29 s  +787.0%      - 1.1GFLOPS,  2.2GB/s
Scalar Double4 - Sqrt       : 0.42 s  +1210.4%     - 0.8GFLOPS,  1.5GB/s
Scalar Double2 - Sqrt       : 0.42 s  +1211.9%     - 0.8GFLOPS,  1.5GB/s
Scalar Double2 - seq1 Div   : 0.42 s  +1212.8%     - 0.8GFLOPS,  1.5GB/s

Simd Double2 - Add        : 38.06 ms             - 16.8GFLOPS,  16.8GB/s
Simd Double2 - seq1 MulAdd: 43.18 ms  +13.5%     - 14.8GFLOPS,  14.8GB/s
Simd Double2 - FMA        : 62.19 ms  +63.4%     - 10.3GFLOPS,  10.3GB/s
Simd Double2 - AddMul     : 71.79 ms  +88.6%     - 8.9GFLOPS,  8.9GB/s
Simd Double2 - Mul        : 77.54 ms  +103.8%    - 8.3GFLOPS,  8.3GB/s
Simd Double2 - AddMul2    : 0.11 s  +192.9%    - 5.7GFLOPS,  5.7GB/s
Simd Double2 - MulAdd     : 0.12 s  +205.4%    - 5.5GFLOPS,  5.5GB/s
Simd Double2 - seq1 Add   : 0.15 s  +284.5%    - 4.4GFLOPS,  4.4GB/s
Simd Double2 - Sqrt       : 0.16 s  +321.7%    - 2.0GFLOPS,  4.0GB/s
Simd Double2 - Div        : 0.29 s  +652.8%    - 1.1GFLOPS,  2.2GB/s
Simd Double2 - seq1 Div   : 0.40 s  +941.5%    - 0.8GFLOPS,  1.6GB/s
```


# ARM Cortex A78 (Snapdragon 888)

* Kryo 680 Gold, 2419 MHz, 5nm, Asus ROG Phone 5
* Notes:
	- 2x128b NEON [ref](https://www.anandtech.com/show/15813/arm-cortex-a78-cortex-x1-cpu-ip-diverging/3)
* Theoretical performance (FMA):
	- 2 FLOPS * 2*4 Op/cy * 2.4 GHz = 38.4 GFLOPS per core

## Float SIMD

```
Scalar Float4 - seq1 MulAdd: 58.87 ms           - 21.7GFLOPS,  10.9GB/s
Scalar Float4 - Mul        : 82.22 ms  +39.7%   - 15.6GFLOPS,  7.8GB/s
Scalar Float4 - AddMul     : 84.04 ms  +42.7%   - 15.2GFLOPS,  7.6GB/s
Scalar Float4 - AddMul2    : 84.08 ms  +42.8%   - 15.2GFLOPS,  7.6GB/s
Scalar Float4 - Add        : 92.37 ms  +56.9%   - 13.9GFLOPS,  6.9GB/s
Scalar Float4 - FMA        : 0.12 s  +99.3%     - 10.9GFLOPS,  5.5GB/s
Scalar Float4 - MulAdd     : 0.12 s  +108.2%    - 10.4GFLOPS,  5.2GB/s
Scalar Float4 - seq1 Add   : 0.13 s  +126.7%    - 9.6GFLOPS,  4.8GB/s
Scalar Float4 - Div        : 0.40 s  +578.6%    - 1.6GFLOPS,  1.6GB/s
Scalar Float4 - seq1 Div   : 0.47 s  +691.6%    - 1.4GFLOPS,  1.4GB/s
Scalar Float4 - Sqrt       : 0.47 s  +691.9%    - 1.4GFLOPS,  1.4GB/s

Simd Float4 - FMA        : 48.00 ms             - 26.7GFLOPS,  13.3GB/s
Simd Float4 - seq1 MulAdd: 51.31 ms  +6.9%      - 24.9GFLOPS,  12.5GB/s
Simd Float4 - Mul        : 52.92 ms  +10.3%     - 24.2GFLOPS,  12.1GB/s
Simd Float4 - Add        : 59.83 ms  +24.7%     - 21.4GFLOPS,  10.7GB/s
Simd Float4 - AddMul     : 83.87 ms  +74.7%     - 15.3GFLOPS,  7.6GB/s
Simd Float4 - AddMul2    : 83.89 ms  +74.8%     - 15.3GFLOPS,  7.6GB/s
Simd Float4 - MulAdd     : 0.12 s  +155.5%      - 10.4GFLOPS,  5.2GB/s
Simd Float4 - seq1 Add   : 0.13 s  +178.0%      - 9.6GFLOPS,  4.8GB/s
Simd Float4 - Sqrt       : 0.19 s  +298.3%      - 3.3GFLOPS,  3.3GB/s
Simd Float4 - Div        : 0.40 s  +732.4%      - 1.6GFLOPS,  1.6GB/s
Simd Float4 - seq1 Div   : 0.47 s  +870.9%      - 1.4GFLOPS,  1.4GB/s
```

## Double SIMD

```
Scalar Double2 - seq1 MulAdd: 40.67 ms           - 15.7GFLOPS,  15.7GB/s
Scalar Double4 - seq1 MulAdd: 40.78 ms  +0.3%    - 15.7GFLOPS,  15.7GB/s
Scalar Double4 - MulAdd     : 67.64 ms  +66.3%   - 9.5GFLOPS,   9.5GB/s
Scalar Double4 - seq1 Add   : 67.78 ms  +66.6%   - 9.4GFLOPS,   9.4GB/s
Scalar Double2 - Mul        : 77.98 ms  +91.7%   - 8.2GFLOPS,   8.2GB/s
Scalar Double2 - Add        : 81.28 ms  +99.8%   - 7.9GFLOPS,   7.9GB/s
Scalar Double4 - Mul        : 82.30 ms  +102.4%  - 7.8GFLOPS,   7.8GB/s
Scalar Double2 - MulAdd     : 83.62 ms  +105.6%  - 7.7GFLOPS,   7.7GB/s
Scalar Double4 - Add        : 92.19 ms  +126.6%  - 6.9GFLOPS,   6.9GB/s
Scalar Double4 - FMA        : 0.10 s  +147.5%    - 6.4GFLOPS,   6.4GB/s
Scalar Double2 - FMA        : 0.13 s  +215.5%    - 5.0GFLOPS,   5.0GB/s
Scalar Double2 - seq1 Add   : 0.13 s  +228.1%    - 4.8GFLOPS,   4.8GB/s
Scalar Double4 - AddMul     : 0.14 s  +249.0%    - 4.5GFLOPS,   4.5GB/s
Scalar Double4 - AddMul2    : 0.14 s  +249.3%    - 4.5GFLOPS,   4.5GB/s
Scalar Double2 - AddMul     : 0.15 s  +269.3%    - 4.3GFLOPS,   4.3GB/s
Scalar Double2 - AddMul2    : 0.15 s  +269.5%    - 4.3GFLOPS,   4.3GB/s
Scalar Double4 - seq1 Div   : 0.40 s  +882.4%    - 0.8GFLOPS,   1.6GB/s
Scalar Double2 - seq1 Div   : 0.47 s  +1046.0%   - 0.7GFLOPS,   1.4GB/s
Scalar Double4 - Div        : 0.53 s  +1211.3%   - 0.6GFLOPS,   1.2GB/s
Scalar Double2 - Div        : 0.53 s  +1213.0%   - 0.6GFLOPS,   1.2GB/s
Scalar Double2 - Sqrt       : 0.93 s  +2191.7%   - 343.3MFLOPS, 0.7GB/s
Scalar Double4 - Sqrt       : 0.93 s  +2192.3%   - 343.2MFLOPS, 0.7GB/s

Simd Double2 - FMA        : 47.95 ms             - 13.3GFLOPS,  13.3GB/s
Simd Double2 - seq1 MulAdd: 51.48 ms  +7.3%      - 12.4GFLOPS,  12.4GB/s
Simd Double2 - Mul        : 53.06 ms  +10.7%     - 12.1GFLOPS,  12.1GB/s
Simd Double2 - Add        : 59.54 ms  +24.2%     - 10.7GFLOPS,  10.7GB/s
Simd Double2 - AddMul     : 83.93 ms  +75.0%     - 7.6GFLOPS,   7.6GB/s
Simd Double2 - AddMul2    : 83.97 ms  +75.1%     - 7.6GFLOPS,   7.6GB/s
Simd Double2 - MulAdd     : 0.12 s  +156.4%      - 5.2GFLOPS,   5.2GB/s
Simd Double2 - seq1 Add   : 0.13 s  +178.6%      - 4.8GFLOPS,   4.8GB/s
Simd Double2 - Sqrt       : 0.15 s  +213.3%      - 2.1GFLOPS,   4.3GB/s
Simd Double2 - seq1 Div   : 0.47 s  +872.0%      - 0.7GFLOPS,   1.4GB/s
Simd Double2 - Div        : 0.53 s  +1011.6%     - 0.6GFLOPS,   1.2GB/s
```

# ARM Cortex A78 (MediaTek Dimensity 7020)

* 2200 MHz, 6nm, Motorola G54
* Notes:
	- 2x128b NEON [ref](https://www.anandtech.com/show/15813/arm-cortex-a78-cortex-x1-cpu-ip-diverging/3)
* Performance: 87.9 GFLOPS per CPU [ref](https://gadgetversus.com/processor/mediatek-dimensity-7020-specs/)
* Theoretical performance (FMA):
	- 2 FLOPS * 2*4 Op/cy * 2.2 GHz = 35.2 GFLOPS per core

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


# ARM Cortex A76 (MediaTek Helio G96)

* 2050 MHz, 12nm, Realme 8i
* Notes:
	- 2x128b NEON ?
* Performance: 66.6 GFLOPS per CPU [ref](https://gadgetversus.com/processor/mediatek-mt6781-helio-g96-specs/)
* Theoretical performance (FMA):
	- 2 FLOPS * 2*4 Op/cy * 2 GHz = 32 GFLOPS per core

## Float SIMD

```
Scalar Float4 - seq1 Add   : 46.44 ms             - 27.6GFLOPS,  13.8GB/s
Scalar Float4 - seq2 Add   : 47.64 ms  +2.6%      - 26.9GFLOPS,  13.4GB/s
Scalar Float4 - seq1 MulAdd: 48.08 ms  +3.5%      - 26.6GFLOPS,  13.3GB/s
Scalar Float4 - Mul        : 53.22 ms  +14.6%     - 24.1GFLOPS,  12.0GB/s
Scalar Float4 - seq1 Div   : 56.55 ms  +21.8%     - 11.3GFLOPS,  11.3GB/s
Scalar Float4 - Add        : 71.41 ms  +53.8%     - 17.9GFLOPS,  9.0GB/s
Scalar Float4 - MulAdd     : 80.11 ms  +72.5%     - 16.0GFLOPS,  8.0GB/s
Scalar Float4 - FMA        : 80.60 ms  +73.6%     - 15.9GFLOPS,  7.9GB/s
Scalar Float4 - AddMul     : 87.92 ms  +89.3%     - 14.6GFLOPS,  7.3GB/s
Scalar Float4 - AddMul2    : 88.32 ms  +90.2%     - 14.5GFLOPS,  7.2GB/s
Scalar Float4 - Div        : 0.47 s  +922.2%      - 1.3GFLOPS,  1.3GB/s
Scalar Float4 - Sqrt       : 0.56 s  +1098.3%     - 1.1GFLOPS,  1.1GB/s

Simd Float4 - seq1 Add   : 44.54 ms             - 28.7GFLOPS,  14.4GB/s
Simd Float4 - FMA        : 47.21 ms  +6.0%      - 27.1GFLOPS,  13.6GB/s
Simd Float4 - seq1 MulAdd: 48.03 ms  +7.8%      - 26.7GFLOPS,  13.3GB/s
Simd Float4 - Mul        : 48.61 ms  +9.1%      - 26.3GFLOPS,  13.2GB/s
Simd Float4 - seq2 Add   : 53.28 ms  +19.6%     - 24.0GFLOPS,  12.0GB/s
Simd Float4 - seq1 Div   : 55.59 ms  +24.8%     - 11.5GFLOPS,  11.5GB/s
Simd Float4 - Add        : 57.66 ms  +29.5%     - 22.2GFLOPS,  11.1GB/s
Simd Float4 - MulAdd     : 79.95 ms  +79.5%     - 16.0GFLOPS,  8.0GB/s
Simd Float4 - AddMul2    : 85.23 ms  +91.4%     - 15.0GFLOPS,  7.5GB/s
Simd Float4 - AddMul     : 97.80 ms  +119.6%    - 13.1GFLOPS,  6.5GB/s
Simd Float4 - Sqrt       : 0.18 s  +311.5%      - 3.5GFLOPS,  3.5GB/s
Simd Float4 - Div        : 0.50 s  +1021.6%     - 1.3GFLOPS,  1.3GB/s
```

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
	- 2 FLOPS * 4 Op/cy * 2 GHz = 16 GFLOPS per core

## Float SIMD

```
Scalar Float4 - seq1 MulAdd: 0.13 s             - 9.9GFLOPS,  4.9GB/s
Scalar Float4 - seq2 Add   : 0.15 s  +16.7%     - 8.5GFLOPS,  4.2GB/s
Scalar Float4 - seq1 Add   : 0.16 s  +21.7%     - 8.1GFLOPS,  4.1GB/s
Scalar Float4 - seq1 Div   : 0.16 s  +21.8%     - 4.1GFLOPS,  4.1GB/s
Scalar Float4 - FMA        : 0.21 s  +63.9%     - 6.0GFLOPS,  3.0GB/s
Scalar Float4 - MulAdd     : 0.27 s  +106.2%    - 4.8GFLOPS,  2.4GB/s
Scalar Float4 - AddMul2    : 0.29 s  +120.3%    - 4.5GFLOPS,  2.2GB/s
Scalar Float4 - AddMul     : 0.29 s  +121.4%    - 4.5GFLOPS,  2.2GB/s
Scalar Float4 - Add        : 0.32 s  +150.3%    - 3.9GFLOPS,  2.0GB/s
Scalar Float4 - Mul        : 0.33 s  +152.5%    - 3.9GFLOPS,  2.0GB/s
Scalar Float4 - Div        : 0.69 s  +432.6%    - 0.9GFLOPS,  0.9GB/s
Scalar Float4 - Sqrt       : 0.75 s  +478.7%    - 0.9GFLOPS,  0.9GB/s

Simd Float4 - seq1 MulAdd: 0.12 s             - 10.3GFLOPS,  5.1GB/s
Simd Float4 - Add        : 0.13 s  +6.5%      - 9.6GFLOPS,  4.8GB/s
Simd Float4 - seq2 Add   : 0.14 s  +9.9%      - 9.3GFLOPS,  4.7GB/s
Simd Float4 - FMA        : 0.14 s  +14.1%     - 9.0GFLOPS,  4.5GB/s
Simd Float4 - Mul        : 0.14 s  +14.2%     - 9.0GFLOPS,  4.5GB/s
Simd Float4 - seq1 Add   : 0.16 s  +24.9%     - 8.2GFLOPS,  4.1GB/s
Simd Float4 - seq1 Div   : 0.16 s  +29.1%     - 4.0GFLOPS,  4.0GB/s
Simd Float4 - MulAdd     : 0.21 s  +67.1%     - 6.1GFLOPS,  3.1GB/s
Simd Float4 - AddMul     : 0.22 s  +75.0%     - 5.9GFLOPS,  2.9GB/s
Simd Float4 - AddMul2    : 0.22 s  +75.0%     - 5.9GFLOPS,  2.9GB/s
Simd Float4 - Sqrt       : 0.30 s  +138.0%    - 2.2GFLOPS,  2.2GB/s
Simd Float4 - Div        : 0.67 s  +433.6%    - 1.0GFLOPS,  1.0GB/s
```

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
	- 2 FLOPS * 4 Op/cy * 2 GHz = 16 GFLOPS per core

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
	- 2 FLOPS * 4 Op/cy * 1.8 GHz = 14.4 GFLOPS per core

## Float SIMD

```
Scalar Float4 - seq1 MulAdd: 0.21 s             - 6.1GFLOPS,  3.1GB/s
Scalar Float4 - Mul        : 0.24 s  +13.5%     - 5.4GFLOPS,  2.7GB/s
Scalar Float4 - Add        : 0.29 s  +37.6%     - 4.4GFLOPS,  2.2GB/s
Scalar Float4 - AddMul     : 0.31 s  +49.3%     - 4.1GFLOPS,  2.0GB/s
Scalar Float4 - AddMul2    : 0.31 s  +49.9%     - 4.1GFLOPS,  2.0GB/s
Scalar Float4 - FMA        : 0.36 s  +74.1%     - 3.5GFLOPS,  1.8GB/s
Scalar Float4 - MulAdd     : 0.43 s  +103.3%    - 3.0GFLOPS,  1.5GB/s
Scalar Float4 - seq1 Add   : 0.54 s  +157.9%    - 2.4GFLOPS,  1.2GB/s
Scalar Float4 - seq1 Div   : 0.84 s  +299.5%    - 0.8GFLOPS,  0.8GB/s
Scalar Float4 - Div        : 0.84 s  +299.6%    - 0.8GFLOPS,  0.8GB/s
Scalar Float4 - Sqrt       : 0.93 s  +342.9%    - 0.7GFLOPS,  0.7GB/s

Simd Float4 - FMA        : 0.22 s             - 5.8GFLOPS,  2.9GB/s
Simd Float4 - AddMul     : 0.31 s  +39.8%     - 4.1GFLOPS,  2.1GB/s
Simd Float4 - AddMul2    : 0.31 s  +39.9%     - 4.1GFLOPS,  2.1GB/s
Simd Float4 - Add        : 0.33 s  +49.8%     - 3.9GFLOPS,  1.9GB/s
Simd Float4 - Mul        : 0.33 s  +50.0%     - 3.8GFLOPS,  1.9GB/s
Simd Float4 - seq1 MulAdd: 0.36 s  +62.6%     - 3.6GFLOPS,  1.8GB/s
Simd Float4 - MulAdd     : 0.42 s  +90.4%     - 3.0GFLOPS,  1.5GB/s
Simd Float4 - Sqrt       : 0.49 s  +121.3%    - 1.3GFLOPS,  1.3GB/s
Simd Float4 - seq1 Add   : 0.54 s  +141.6%    - 2.4GFLOPS,  1.2GB/s
Simd Float4 - Div        : 0.83 s  +274.2%    - 0.8GFLOPS,  0.8GB/s
Simd Float4 - seq1 Div   : 0.83 s  +274.6%    - 0.8GFLOPS,  0.8GB/s
```

## Double SIMD

```
Scalar Double4 - seq1 MulAdd: 0.18 s             - 3.5GFLOPS,    3.5GB/s
Scalar Double2 - seq1 MulAdd: 0.21 s  +13.5%     - 3.1GFLOPS,    3.1GB/s
Scalar Double2 - MulAdd     : 0.22 s  +20.5%     - 2.9GFLOPS,    2.9GB/s
Scalar Double2 - Mul        : 0.24 s  +30.2%     - 2.7GFLOPS,    2.7GB/s
Scalar Double4 - Mul        : 0.24 s  +30.4%     - 2.7GFLOPS,    2.7GB/s
Scalar Double4 - MulAdd     : 0.24 s  +31.2%     - 2.7GFLOPS,    2.7GB/s
Scalar Double2 - AddMul     : 0.29 s  +56.7%     - 2.2GFLOPS,    2.2GB/s
Scalar Double2 - AddMul2    : 0.29 s  +56.8%     - 2.2GFLOPS,    2.2GB/s
Scalar Double2 - Add        : 0.29 s  +57.6%     - 2.2GFLOPS,    2.2GB/s
Scalar Double4 - Add        : 0.29 s  +59.0%     - 2.2GFLOPS,    2.2GB/s
Scalar Double4 - AddMul     : 0.31 s  +70.1%     - 2.1GFLOPS,    2.1GB/s
Scalar Double4 - AddMul2    : 0.31 s  +70.3%     - 2.1GFLOPS,    2.1GB/s
Scalar Double4 - seq1 Add   : 0.34 s  +87.2%     - 1.9GFLOPS,    1.9GB/s
Scalar Double4 - FMA        : 0.35 s  +89.6%     - 1.8GFLOPS,    1.8GB/s
Scalar Double2 - FMA        : 0.39 s  +115.1%    - 1.6GFLOPS,    1.6GB/s
Scalar Double2 - seq1 Add   : 0.54 s  +193.5%    - 1.2GFLOPS,    1.2GB/s
Scalar Double2 - seq1 Div   : 1.10 s  +502.4%    - 290.9MFLOPS,  0.6GB/s
Scalar Double4 - seq1 Div   : 1.14 s  +523.1%    - 281.2MFLOPS,  0.6GB/s
Scalar Double2 - Div        : 2.37 s  +1197.2%   - 135.1MFLOPS,  270.2MB/s
Scalar Double4 - Div        : 2.39 s  +1210.9%   - 133.7MFLOPS,  267.3MB/s
Scalar Double2 - Sqrt       : 3.54 s  +1836.0%   - 90.5MFLOPS,   181.0MB/s
Scalar Double4 - Sqrt       : 3.59 s  +1865.1%   - 89.2MFLOPS,   178.3MB/s

Simd Double2 - FMA        : 0.22 s             - 2.9GFLOPS,    2.9GB/s
Simd Double2 - AddMul2    : 0.31 s  +40.3%     - 2.1GFLOPS,    2.1GB/s
Simd Double2 - AddMul     : 0.31 s  +41.6%     - 2.0GFLOPS,    2.0GB/s
Simd Double2 - Add        : 0.34 s  +51.5%     - 1.9GFLOPS,    1.9GB/s
Simd Double2 - Mul        : 0.34 s  +51.7%     - 1.9GFLOPS,    1.9GB/s
Simd Double2 - seq1 MulAdd: 0.36 s  +62.7%     - 1.8GFLOPS,    1.8GB/s
Simd Double2 - MulAdd     : 0.43 s  +92.5%     - 1.5GFLOPS,    1.5GB/s
Simd Double2 - Sqrt       : 0.49 s  +121.6%    - 0.7GFLOPS,    1.3GB/s
Simd Double2 - seq1 Add   : 0.54 s  +144.4%    - 1.2GFLOPS,    1.2GB/s
Simd Double2 - seq1 Div   : 0.83 s  +274.6%    - 385.7MFLOPS,  0.8GB/s
Simd Double2 - Div        : 1.24 s  +458.7%    - 258.7MFLOPS,  0.5GB/s
```

# ARM Cortex A53 (Samsung Exynos 7870)

* 1586 MHz, 14nm, Samsung J7 Neo
* Theoretical performance (FMA):
	- 2 FLOPS * 4 Op/cy * 1.6 GHz = 12.7 GFLOPS per core

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

9.4 GFLOPS per CPU [ref](https://gadgetversus.com/processor/samsung-exynos-7-octa-7870-specs/)


# ARM Cortex A53, P-core (Snapdragon 439)

* 2016 MHz, 12nm, Xiaomi Redmi 7A
* Theoretical performance (FMA):
	- 2 FLOPS * 4 Op/cy * 2 GHz = 16 GFLOPS per core

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

24 GFLOPS per CPU [ref](https://gadgetversus.com/processor/qualcomm-sdm439-snapdragon-439-specs/)


# ARM Cortex A53, E-core (Snapdragon 439)

* 1459 MHz, 12nm, Xiaomi Redmi 7A
* Theoretical performance (FMA):
	- 2 FLOPS * 4 Op/cy * 1.46 GHz = 11.7 GFLOPS per core

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

203.4 GFLOPS [ref](https://gadgetversus.com/processor/apple-m1-specs/)


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

* 3.6 GHz
* Theoretical performance (FMA):
	- 16 FLOPS * 3.6 GHz = 57.6 GFLOPS per core
	- 57.6 GFLOPS * 4 cores  = 230 GFLOPS per CPU
	- 2 FLOPS (scalar) * 3.6 GHz = 7.2 GFLOPS per core

## Float SIMD, single thread
```
Simd Float8 - FMA ilp=6   : 0.16 s             - 38.0GFLOPS (67%)
Simd Float8 - FMA ilp=8   : 0.21 s  +33.4%     - 38.0GFLOPS
Simd Float8 - FMA ilp=7   : 0.19 s  +18.9%     - 37.3GFLOPS
Simd Float8 - FMA ilp=5   : 0.27 s  +72.2%     - 36.8GFLOPS
Simd Float8 - FMA ilp=4   : 0.22 s  +38.0%     - 36.7GFLOPS
Simd Float8 - FMA ilp=16  : 0.49 s  +212.2%    - 32.5GFLOPS
Simd Float8 - Add ilp=4   : 0.28 s  +79.4%     - 28.3GFLOPS (50%)
Simd Float8 - Add ilp=7   : 0.25 s  +57.1%     - 28.2GFLOPS
Simd Float8 - Add ilp=8   : 0.28 s  +80.3%     - 28.1GFLOPS
Simd Float8 - Add ilp=6   : 0.32 s  +103.7%    - 28.0GFLOPS
Simd Float8 - Add ilp=5   : 0.36 s  +126.0%    - 28.0GFLOPS
Simd Float8 - MulAdd      : 0.29 s  +83.9%     - 27.6GFLOPS
Simd Float8 - Mul         : 0.30 s  +90.1%     - 26.7GFLOPS
Simd Float8 - FMA + Div   : 0.35 s  +124.4%    - 22.6GFLOPS
Simd Float8 - FMA + Add   : 0.39 s  +149.7%    - 20.3GFLOPS
Simd Float8 - FMA + Div2  : 0.71 s  +352.3%    - 11.2GFLOPS
Simd Float8 - Fast Sqrt   : 1.41 s  +795.9%    - 5.66GFLOPS
Simd Float8 - Fast Div    : 1.43 s  +803.2%    - 5.61GFLOPS
Simd Float8 - Div ilp=8   : 5.69 s  +3503.0%   - 1.41GFLOPS
Simd Float8 - Precise Sqrt: 6.78 s  +4195.9%   - 1.18GFLOPS
Simd Float8 - Div ilp=4   : 6.90 s  +4269.5%   - 1.16GFLOPS
```

## Float SIMD, 4 threads
```
Simd Float8 - FMA ilp=7 : 0.24 s  +14.7%     - 29.2GFLOPS (50%) - 8 FLOPS/cy on FMA
Simd Float8 - FMA ilp=8 : 0.27 s  +29.9%     - 29.5GFLOPS
Simd Float8 - FMA ilp=4 : 0.28 s  +32.4%     - 28.9GFLOPS
Simd Float8 - FMA ilp=6 : 0.21 s             - 28.7GFLOPS
Simd Float8 - FMA ilp=5 : 0.35 s  +65.3%     - 28.9GFLOPS
Simd Float8 - FMA ilp=16: 0.61 s  +191.7%    - 26.2GFLOPS
Simd Float8 - Add ilp=7 : 0.32 s  +53.1%     - 21.9GFLOPS
Simd Float8 - Add ilp=6 : 0.41 s  +96.4%     - 21.9GFLOPS
Simd Float8 - Add ilp=8 : 0.37 s  +75.5%     - 21.8GFLOPS
Simd Float8 - Add ilp=5 : 0.46 s  +122.3%    - 21.5GFLOPS
Simd Float8 - Add ilp=4 : 0.39 s  +86.8%     - 20.5GFLOPS
Simd Float8 - FMA + Add : 0.46 s  +120.3%    - 17.4GFLOPS
```

# AMD Ryzen 7 8745HS

* Theoretical performance (FMA):
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

| | time | diff | FLOPS | SOL |
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

| | time | diff | FLOPS | SOL |
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


# Intel Ultra 7 255H, LPE-Core

* Theoretical performance (FMA):
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

| | time | diff | FLOPS | SOL |
|------------------------------|----------|------|-------------|---|
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
