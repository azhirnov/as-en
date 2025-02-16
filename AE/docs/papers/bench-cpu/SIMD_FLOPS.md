Performance per thread.


# AMD Ryzen 3900X, 4.2 GHz, 7nm (Zen2)

Theoretical performance:<br/>
32 FLOPS/cy * 4.2 GHz = 134 GFLOPS

## Float SIMD

```
Scalar Float4 - FMA        : 0.85 s             - 6.0GFLOPS
Scalar Float4 - MulAdd     : 0.85 s  +0.1%      - 6.0GFLOPS
Scalar Float4 - par Add    : 0.99 s  +16.5%     - 5.2GFLOPS
Scalar Float4 - MulAdd2    : 1.00 s  +17.3%     - 5.1GFLOPS
Scalar Float4 - Mul        : 1.04 s  +22.6%     - 4.9GFLOPS
Scalar Float4 - seq2 Add   : 1.21 s  +42.7%     - 4.2GFLOPS
Scalar Float4 - seq1 Add   : 1.56 s  +83.2%     - 3.3GFLOPS
Scalar Float4 - seq1 MulAdd: 1.68 s  +97.3%     - 3.1GFLOPS
Scalar Float4 - Div        : 2.07 s  +144.2%    - 1.2GFLOPS
Scalar Float4 - seq1 Div   : 2.21 s  +160.3%    - 1.2GFLOPS
Scalar Float4 - Sqrt       : 4.57 s  +438.4%    - 0.6GFLOPS

Simd Float8 - FMA        : 0.12 s             - 41.9GFLOPS
Simd Float8 - Mul        : 0.14 s  +17.7%     - 35.6GFLOPS
Simd Float8 - AddMul2    : 0.14 s  +17.7%     - 35.6GFLOPS
Simd Float8 - MulAdd     : 0.14 s  +17.8%     - 35.6GFLOPS
Simd Float8 - AddMul     : 0.14 s  +17.9%     - 35.6GFLOPS
Simd Float8 - par Add    : 0.15 s  +19.0%     - 35.2GFLOPS
Simd Float4 - FMA        : 0.20 s  +61.4%     - 26.0GFLOPS
Simd Float8 - Sqrt       : 0.22 s  +78.4%     - 11.8GFLOPS
Simd Float8 - seq2 Add   : 0.23 s  +89.4%     - 22.1GFLOPS
Simd Float4 - AddMul2    : 0.24 s  +94.0%     - 21.6GFLOPS
Simd Float4 - MulAdd     : 0.24 s  +94.5%     - 21.6GFLOPS
Simd Float4 - AddMul     : 0.24 s  +94.5%     - 21.6GFLOPS
Simd Float4 - par Add    : 0.24 s  +95.1%     - 21.5GFLOPS
Simd Float4 - Mul        : 0.24 s  +97.3%     - 21.3GFLOPS
Simd Float8 - Div        : 0.27 s  +119.1%    - 9.6GFLOPS
Simd Float4 - Sqrt       : 0.42 s  +246.8%    - 6.0GFLOPS
Simd Float8 - seq1 MulAdd: 0.45 s  +266.0%    - 11.5GFLOPS
Simd Float8 - seq1 Add   : 0.45 s  +267.0%    - 11.4GFLOPS
Simd Float4 - seq2 Add   : 0.45 s  +271.6%    - 11.3GFLOPS
Simd Float4 - Div        : 0.53 s  +333.3%    - 4.8GFLOPS
Simd Float8 - seq1 Div   : 0.74 s  +504.4%    - 3.5GFLOPS
Simd Float4 - seq1 Add   : 0.89 s  +631.2%    - 5.7GFLOPS
Simd Float4 - seq1 MulAdd: 0.90 s  +635.1%    - 5.7GFLOPS
Simd Float4 - seq1 Div   : 1.49 s  +1118.6%   - 1.7GFLOPS
```

## Double SIMD

```
Scalar Double2 - FMA    : 0.40 s             - 6.5GFLOPS
Scalar Double2 - MulAdd : 0.40 s  +0.2%      - 6.5GFLOPS
Scalar Double2 - Mul    : 0.43 s  +9.1%      - 5.9GFLOPS
Scalar Double2 - par Add: 0.44 s  +10.4%     - 5.9GFLOPS
Scalar Double2 - seq Add: 0.94 s  +137.9%    - 2.7GFLOPS
Scalar Double4 - MulAdd : 1.94 s  +391.6%    - 1.3GFLOPS
Scalar Double4 - Mul    : 1.95 s  +391.9%    - 1.3GFLOPS
Scalar Double4 - par Add: 1.95 s  +392.4%    - 1.3GFLOPS
Scalar Double4 - FMA    : 1.95 s  +393.6%    - 1.3GFLOPS
Scalar Double4 - seq Add: 2.47 s  +524.2%    - 1.0GFLOPS
Scalar Double2 - Div    : 1.51 s  +281.2%    - 0.8GFLOPS
Scalar Double4 - Div    : 2.21 s  +458.9%    - 0.6GFLOPS
Scalar Double4 - Sqrt   : 6.32 s  +1498.6%   - 202.5MFLOPS
Scalar Double2 - Sqrt   : 6.41 s  +1521.6%   - 199.6MFLOPS

Simd Double2 - FMA    : 0.21 s             - 12.2GFLOPS
Simd Double2 - MulAdd : 0.25 s  +17.3%     - 10.4GFLOPS
Simd Double2 - par Add: 0.25 s  +17.9%     - 10.4GFLOPS
Simd Double2 - Mul    : 0.25 s  +19.2%     - 10.2GFLOPS
Simd Double4 - FMA    : 0.28 s  +35.5%     - 9.0GFLOPS
Simd Double4 - MulAdd : 0.30 s  +44.1%     - 8.5GFLOPS
Simd Double4 - Mul    : 0.30 s  +44.6%     - 8.4GFLOPS
Simd Double4 - par Add: 0.31 s  +46.4%     - 8.3GFLOPS
Simd Double4 - seq Add: 0.49 s  +133.4%    - 5.2GFLOPS
Simd Double4 - Div    : 0.50 s  +138.9%    - 2.6GFLOPS
Simd Double2 - seq Add: 0.97 s  +361.0%    - 2.6GFLOPS
Simd Double4 - Sqrt   : 0.66 s  +216.8%    - 1.9GFLOPS
Simd Double2 - Div    : 0.77 s  +267.6%    - 1.7GFLOPS
Simd Double2 - Sqrt   : 1.32 s  +527.9%    - 1.0GFLOPS
```

Compiling with AVX2 is 1.5 faster for non-simd vector.

469 GFLOPS per CPU [ref](https://www.pugetsystems.com/labs/hpc/amd-3900x-brief-compute-performance-linpack-and-namd-1540/)
820 GFLOPS per CPU [ref](https://gadgetversus.com/processor/amd-ryzen-9-3900x-specs/)


# AMD Phenom II X4 945, 3 GHz, 45nm (Deneb, K10)

Theoretical performance:<br/>
8 FLOPS/cy * 3 GHz = 24 GFLOPS

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

43.2 GFLOPS per CPU [ref](https://gadgetversus.com/processor/amd-phenom-ii-x4-945-specs/)


# Intel i5-8250U, 3.39 GHz, 14nm

## Float SIMD

```
Scalar Float4 - Mul    : 1.77 s             - 2.9GFLOPS
Scalar Float4 - MulAdd : 1.77 s  +0.2%      - 2.9GFLOPS
Scalar Float4 - FMA    : 1.78 s  +0.3%      - 2.9GFLOPS
Scalar Float4 - par Add: 1.93 s  +9.0%      - 2.7GFLOPS
Scalar Float4 - seq Add: 2.10 s  +18.5%     - 2.4GFLOPS
Scalar Float4 - Div    : 2.37 s  +33.6%     - 1.1GFLOPS
Scalar Float4 - Sqrt   : 5.76 s  +225.5%    - 444.2MFLOPS

Simd Float8 - FMA     : 0.18 s             - 29.0GFLOPS
Simd Float4 - FMA     : 0.25 s  +40.6%     - 20.6GFLOPS
Simd Float8 - Mul     : 0.25 s  +41.1%     - 20.5GFLOPS
Simd Float8 - MulAdd  : 0.25 s  +41.2%     - 20.5GFLOPS
Simd Float8 - par Add : 0.26 s  +45.7%     - 19.9GFLOPS
Simd Float8 - Sqrt    : 0.34 s  +91.6%     - 7.6GFLOPS
Simd Float8 - seq2 Add: 0.42 s  +139.6%    - 12.1GFLOPS
Simd Float4 - Mul     : 0.43 s  +143.9%    - 11.9GFLOPS
Simd Float4 - MulAdd  : 0.43 s  +145.0%    - 11.8GFLOPS
Simd Float4 - par Add : 0.44 s  +147.1%    - 11.7GFLOPS
Simd Float8 - Div     : 0.51 s  +188.3%    - 5.0GFLOPS
Simd Float4 - Sqrt    : 0.62 s  +249.7%    - 4.1GFLOPS
Simd Float4 - Div     : 0.75 s  +322.9%    - 3.4GFLOPS
Simd Float8 - seq1 Add: 0.78 s  +339.2%    - 6.6GFLOPS
Simd Float4 - seq2 Add: 0.80 s  +349.7%    - 6.4GFLOPS
Simd Float8 - seq1 Div: 1.04 s  +489.1%    - 2.5GFLOPS
Simd Float4 - seq1 Add: 1.53 s  +764.5%    - 3.3GFLOPS
Simd Float4 - seq1 Div: 2.09 s  +1080.1%   - 1.2GFLOPS
```

## Double SIMD

```
Scalar Double2 - FMA    : 0.60 s             - 4.3GFLOPS
Scalar Double2 - MulAdd : 0.60 s  +0.0%      - 4.3GFLOPS
Scalar Double2 - Mul    : 0.60 s  +0.1%      - 4.3GFLOPS
Scalar Double2 - par Add: 0.63 s  +5.1%      - 4.1GFLOPS
Scalar Double2 - Div    : 1.58 s  +163.1%    - 0.8GFLOPS
Scalar Double2 - seq Add: 1.58 s  +164.6%    - 1.6GFLOPS
Scalar Double4 - par Add: 2.30 s  +283.6%    - 1.1GFLOPS
Scalar Double4 - MulAdd : 2.35 s  +292.9%    - 1.1GFLOPS
Scalar Double4 - FMA    : 2.35 s  +293.3%    - 1.1GFLOPS
Scalar Double4 - Mul    : 2.38 s  +297.3%    - 1.1GFLOPS
Scalar Double4 - Div    : 2.42 s  +303.8%    - 0.5GFLOPS
Scalar Double4 - seq Add: 2.49 s  +315.8%    - 1.0GFLOPS
Scalar Double4 - Sqrt   : 6.88 s  +1049.3%   - 186.1MFLOPS
Scalar Double2 - Sqrt   : 7.20 s  +1102.1%   - 177.9MFLOPS

Simd Double4 - FMA     : 0.18 s             - 14.4GFLOPS
Simd Double4 - par Add : 0.25 s  +40.6%     - 10.3GFLOPS
Simd Double2 - FMA     : 0.25 s  +40.6%     - 10.3GFLOPS
Simd Double4 - Mul     : 0.25 s  +40.6%     - 10.3GFLOPS
Simd Double4 - MulAdd  : 0.25 s  +41.2%     - 10.2GFLOPS
Simd Double4 - seq2 Add: 0.42 s  +137.8%    - 6.1GFLOPS
Simd Double2 - par Add : 0.43 s  +142.6%    - 5.9GFLOPS
Simd Double2 - MulAdd  : 0.43 s  +142.6%    - 5.9GFLOPS
Simd Double2 - Mul     : 0.43 s  +142.8%    - 5.9GFLOPS
Simd Double4 - Div     : 0.77 s  +336.8%    - 1.7GFLOPS
Simd Double4 - seq1 Add: 0.78 s  +338.8%    - 3.3GFLOPS
Simd Double2 - seq2 Add: 0.79 s  +347.7%    - 3.2GFLOPS
Simd Double2 - Div     : 0.89 s  +400.5%    - 1.4GFLOPS
Simd Double4 - Sqrt    : 1.14 s  +544.3%    - 1.1GFLOPS
Simd Double2 - Sqrt    : 1.16 s  +554.2%    - 1.1GFLOPS
Simd Double4 - seq1 Div: 1.23 s  +593.4%    - 1.0GFLOPS
Simd Double2 - seq1 Add: 1.53 s  +761.0%    - 1.7GFLOPS
Simd Double2 - seq1 Div: 2.46 s  +1286.7%   - 0.5GFLOPS
```

163 GFLOPS per CPU [ref](https://gadgetversus.com/processor/intel-core-i5-8250u-specs/)


# ARM Cortex X1 (Kryo 680 Prime), 2841 MHz, 5nm (Asus ROG Phone 5, Snapdragon 888)

* Memory: 8 GB, LPDDR5-6400, QC 16bit, 3200MHz, 51.2 GB/s

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

144 GFLOPS per CPU [ref](https://gadgetversus.com/processor/qualcomm-sm8350-snapdragon-888-5g-specs/)


# ARM Cortex A78 (Kryo 680 Gold), 2419 MHz, 5nm (Asus ROG Phone 5, Snapdragon 888)

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

# ARM Cortex A78, 2200 MHz, 6nm (Motorola G54, MediaTek Dimensity 7020)

* Memory: 8 GB, LPDDR5, QC 16bit, 3200 MHz, 51.2 GB/s
*
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

87.9 GFLOPS per CPU [ref](https://gadgetversus.com/processor/mediatek-dimensity-7020-specs/)


# ARM Cortex A76, 2050 MHz, 12nm (Realme 8i, MediaTek Helio G96)

* Memory: 4GB, LPDDR4X, DC 16bit, 2133 MHz, 17.07 GB/s

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

66.6 GFLOPS per CPU [ref](https://gadgetversus.com/processor/mediatek-mt6781-helio-g96-specs/)


# ARM Cortex A55, 2000 MHz, 12nm (Realme 8i, MediaTek Helio G96)

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

# ARM Cortex A55, 2000 MHz, 6nm (Motorola G54, MediaTek Dimensity 7020)

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


# ARM Cortex A55 (Kryo 680 Silver), 1804 MHz, 5nm (Asus ROG Phone 5, Snapdragon 888)

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

# ARM Cortex A53, 1586 MHz, 14nm (Samsung J7 Neo, Samsung Exynos 7870)

* Memory: 2GB, LPDDR3, DC 32bit, 933MHz, 14.9 GB/s

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


# ARM Cortex A53, 2016 MHz, 12nm (Xiaomi Redmi 7A, Snapdragon 439)

* Memory v1: 2GB, LPDDR3, 933 MHz, 7.4GB/s
* Memory v2: 2GB, LPDDR3-1600, SC 32bit, 800MHz, 6.4 GB/s

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


# ARM Cortex A53, 1459 MHz, 12nm (Xiaomi Redmi 7A, Snapdragon 439)

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


# Apple M1, P-core, 3.2 GHz, 5nm (Mac Mini M1 16GB, Firestorm)

Theoretical performance:<br/>
8 FLOPS/cy * 3.2 GHz = 25.6 GFLOPS

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


# Apple M1, E-core, 2.06 GHz, 5nm (Mac Mini M1 16GB, Icestorm)

Theoretical performance:<br/>
4 FLOPS/cy * 2.06 GHz = 8.2 GFLOPS

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

