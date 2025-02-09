
# AMD Ryzen 3900X (Zen2)

* Cache:
	- L1I: 12x 32KB 8-way
	- L1D: 12x 32KB 8-way
	- L2: 12x 512KB 8-way
	- L3: 64MB: 4x 16MB 16-way
* Memory: DDR4, 3GHz, 24GB/s

```
block 4 Gb:
  MemCopy32 - avx v6    : 0.23 s             - 18.3GB/s
  MemCopy64 - non-cached: 0.23 s  +0.0%      - 18.3GB/s		<<<<
  MemCopy32 - avx v1    : 0.23 s  +0.2%      - 18.3GB/s
  std::memcpy - align256: 0.23 s  +0.2%      - 18.3GB/s		<<<<
  MemCopy32 - avx v4    : 0.24 s  +0.4%      - 18.2GB/s
  std::memcpy - align64 : 0.24 s  +0.7%      - 18.2GB/s
  MemCopy32 - avx v5    : 0.24 s  +1.7%      - 18.0GB/s
  MemCopy16 - sse v5    : 0.24 s  +3.0%      - 17.8GB/s
  MemCopy16 - sse v4    : 0.24 s  +3.0%      - 17.8GB/s
  MemCopy16 - sse v6    : 0.24 s  +3.3%      - 17.7GB/s
  MemCopy16 - sse v7    : 0.24 s  +3.4%      - 17.7GB/s
  MemCopy16 - sse v3    : 0.24 s  +3.5%      - 17.7GB/s
  std::memcpy -         : 0.28 s  +20.8%     - 15.2GB/s
  MemCopy16 - sse v2    : 0.41 s  +73.4%     - 10.6GB/s
  MemCopy64 - cached    : 0.41 s  +76.9%     - 10.4GB/s
  MemCopy32 - avx v2    : 0.42 s  +77.4%     - 10.3GB/s
  MemCopy16 - sse v1    : 0.42 s  +78.5%     - 10.3GB/s
  MemCopy32 - avx v3    : 0.42 s  +79.0%     - 10.2GB/s
  MemCopy64 - movsb     : 0.49 s  +109.2%    - 8.8GB/s

block 4 Mb:
  MemCopy32 - avx v2    : 8.58 ms             - 48.9GB/s
  MemCopy32 - avx v3    : 8.68 ms  +1.2%      - 48.3GB/s
  MemCopy64 - cached    : 8.69 ms  +1.3%      - 48.3GB/s	<<<<<<
  MemCopy16 - sse v2    : 8.97 ms  +4.6%      - 46.8GB/s
  MemCopy16 - sse v1    : 12.24 ms  +42.7%     - 34.3GB/s
  MemCopy64 - non-cached: 22.49 ms  +162.1%    - 18.7GB/s
  MemCopy32 - avx v6    : 22.58 ms  +163.2%    - 18.6GB/s
  std::memcpy -         : 22.58 ms  +163.2%    - 18.6GB/s	<<<<<
  std::memcpy - align64 : 22.65 ms  +164.0%    - 18.5GB/s
  std::memcpy - align256: 22.75 ms  +165.2%    - 18.4GB/s
  MemCopy32 - avx v1    : 22.82 ms  +166.0%    - 18.4GB/s
  MemCopy16 - sse v3    : 23.08 ms  +169.0%    - 18.2GB/s
  MemCopy32 - avx v4    : 23.13 ms  +169.7%    - 18.1GB/s
  MemCopy16 - sse v4    : 23.17 ms  +170.1%    - 18.1GB/s
  MemCopy16 - sse v6    : 23.17 ms  +170.1%    - 18.1GB/s
  MemCopy16 - sse v5    : 23.18 ms  +170.2%    - 18.1GB/s
  MemCopy16 - sse v7    : 23.19 ms  +170.3%    - 18.1GB/s
  MemCopy32 - avx v5    : 23.26 ms  +171.1%    - 18.0GB/s
  MemCopy64 - movsb     : 34.82 ms  +305.9%    - 12.0GB/s

block 4 Kb:
  MemCopy32 - avx v2    : 0.30 ms             - 138.0GB/s
  MemCopy32 - avx v3    : 0.30 ms  +0.5%      - 137.3GB/s
  std::memcpy - align64 : 0.30 ms  +1.3%      - 136.2GB/s	<<<<<
  MemCopy64 - cached    : 0.30 ms  +1.4%      - 136.1GB/s	<<<<<
  std::memcpy - align256: 0.30 ms  +2.2%      - 135.1GB/s
  std::memcpy -         : 0.37 ms  +24.5%     - 110.9GB/s
  MemCopy64 - movsb     : 0.44 ms  +49.0%     - 92.7GB/s
  MemCopy16 - sse v2    : 0.60 ms  +103.6%    - 67.8GB/s
  MemCopy16 - sse v1    : 1.24 ms  +317.4%    - 33.1GB/s
  MemCopy32 - avx v5    : 2.80 ms  +844.1%    - 14.6GB/s
  MemCopy32 - avx v4    : 2.83 ms  +853.6%    - 14.5GB/s
  MemCopy64 - non-cached: 2.88 ms  +871.3%    - 14.2GB/s
  MemCopy32 - avx v6    : 3.09 ms  +941.6%    - 13.3GB/s
  MemCopy32 - avx v1    : 3.43 ms  +1055.7%   - 11.9GB/s
  MemCopy16 - sse v7    : 3.70 ms  +1146.9%   - 11.1GB/s
  MemCopy16 - sse v6    : 3.75 ms  +1163.5%   - 10.9GB/s
  MemCopy16 - sse v3    : 3.77 ms  +1171.3%   - 10.9GB/s
  MemCopy16 - sse v4    : 3.79 ms  +1176.5%   - 10.8GB/s
  MemCopy16 - sse v5    : 4.00 ms  +1247.3%   - 10.2GB/s
```

```
block 1024 Mb:
  memset avx v1       : 0.18 s             - 23.7GB/s
  ZeroMem256_NonCached: 0.18 s  +0.0%      - 23.7GB/s
  memset avx v3       : 0.18 s  +0.0%      - 23.7GB/s
  memset sse v3       : 0.18 s  +0.1%      - 23.7GB/s
  memset sse v1       : 0.18 s  +0.4%      - 23.6GB/s
  std::memset         : 0.19 s  +3.9%      - 22.8GB/s
  memset sse v2       : 0.28 s  +54.9%     - 15.3GB/s
  memset sse v4       : 0.28 s  +55.1%     - 15.3GB/s
  memset avx v2       : 0.28 s  +55.6%     - 15.2GB/s
  memset avx v4       : 0.28 s  +57.1%     - 15.1GB/s
  ZeroMem256_Cached   : 0.29 s  +57.7%     - 15.0GB/s

block 4 Mb:
  ZeroMem256_Cached   : 4.39 ms             - 95.6GB/s
  std::memset         : 4.41 ms  +0.4%      - 95.1GB/s
  memset avx v4       : 4.44 ms  +1.0%      - 94.6GB/s
  memset avx v2       : 4.49 ms  +2.3%      - 93.4GB/s
  memset sse v4       : 6.18 ms  +40.7%     - 67.9GB/s
  memset sse v2       : 6.21 ms  +41.4%     - 67.6GB/s
  memset sse v1       : 17.73 ms  +303.9%   - 23.7GB/s
  memset avx v3       : 17.74 ms  +304.1%   - 23.6GB/s
  memset avx v1       : 17.74 ms  +304.2%   - 23.6GB/s
  ZeroMem256_NonCached: 17.76 ms  +304.5%   - 23.6GB/s
  memset sse v3       : 17.77 ms  +304.8%   - 23.6GB/s

block 4 Kb:
  ZeroMem256_Cached   : 0.30 ms             - 138.1GB/s
  std::memset         : 0.31 ms  +3.6%      - 133.3GB/s
  memset avx v4       : 0.32 ms  +6.3%      - 129.9GB/s
  memset avx v2       : 0.35 ms  +16.4%     - 118.6GB/s
  memset sse v4       : 0.58 ms  +95.9%     - 70.5GB/s
  memset sse v2       : 0.65 ms  +119.9%    - 62.8GB/s
  memset avx v1       : 2.63 ms  +787.9%    - 15.6GB/s
  memset sse v3       : 2.63 ms  +788.6%    - 15.5GB/s
  memset avx v3       : 2.64 ms  +789.9%    - 15.5GB/s
  ZeroMem256_NonCached: 2.66 ms  +796.8%    - 15.4GB/s
  memset sse v1       : 2.66 ms  +798.1%    - 15.4GB/s
```

memcopy vs SIMD
```
std::memcpy, single thread:
  256 b  : 78.14 ms  +100.3%  - 68.7GB/s
  512 b  : 53.55 ms  +37.3%   - 100.2GB/s
  1024 b : 40.43 ms  +3.6%    - 132.8GB/s
  2048 b : 39.46 ms  +1.1%    - 136.1GB/s
  4 Kb   : 39.02 ms           - 137.6GB/s - page size
  8 Kb   : 39.02 ms  +0.0%    - 137.6GB/s
  16 Kb  : 40.78 ms  +4.5%    - 131.7GB/s
  32 Kb  : 77.36 ms  +98.3%   - 69.4GB/s  - L1
  64 Kb  : 77.01 ms  +97.4%   - 69.7GB/s
  128 Kb : 76.92 ms  +97.1%   - 69.8GB/s
  256 Kb : 80.80 ms  +107.1%  - 66.4GB/s
  512 Kb : 0.10 s  +160.2%    - 52.9GB/s  - L2
  1024 Kb: 0.10 s  +165.7%    - 51.8GB/s
  2048 Kb: 0.29 s  +642.0%    - 18.5GB/s  - used non-cached copy
  4 Mb   : 0.29 s  +643.6%    - 18.5GB/s
  8 Mb   : 0.29 s  +646.4%    - 18.4GB/s
  16 Mb  : 0.29 s  +650.6%    - 18.3GB/s  - L3 bank
  32 Mb  : 0.30 s  +656.2%    - 18.2GB/s
  64 Mb  : 0.29 s  +655.1%    - 18.2GB/s  - L3
  128 Mb : 0.29 s  +654.8%    - 18.2GB/s
  256 Mb : 0.30 s  +656.7%    - 18.2GB/s  - 7% slower than in 4 threads

SIMD cached copy, single thread:
  256 b  : 43.95 ms  +13.9%   - 122.2GB/s  <<
  512 b  : 38.99 ms  +1.1%    - 137.7GB/s
  1024 b : 38.92 ms  +0.9%    - 137.9GB/s
  2048 b : 38.77 ms  +0.5%    - 138.5GB/s
  4 Kb   : 38.58 ms  +0.0%    - 139.2GB/s - page size
  8 Kb   : 38.57 ms           - 139.2GB/s
  16 Kb  : 39.10 ms  +1.4%    - 137.3GB/s
  32 Kb  : 77.01 ms  +99.7%   - 69.7GB/s  - L1
  64 Kb  : 77.04 ms  +99.7%   - 69.7GB/s
  128 Kb : 76.94 ms  +99.5%   - 69.8GB/s
  256 Kb : 81.46 ms  +111.2%  - 65.9GB/s
  512 Kb : 0.10 s  +163.8%    - 52.8GB/s  - L2
  1024 Kb: 0.10 s  +168.4%    - 51.9GB/s
  2048 Kb: 0.10 s  +171.7%    - 51.2GB/s  <<
  4 Mb   : 0.10 s  +170.0%    - 51.6GB/s  << faster than memcpy
  8 Mb   : 0.21 s  +441.5%    - 25.7GB/s  <<
  16 Mb  : 0.52 s  +1247.1%   - 10.3GB/s  - L3 bank
  32 Mb  : 0.52 s  +1251.6%   - 10.3GB/s
  64 Mb  : 0.52 s  +1242.9%   - 10.4GB/s  - L3
  128 Mb : 0.52 s  +1244.5%   - 10.4GB/s
  256 Mb : 0.52 s  +1241.4%   - 10.4GB/s
```

Single thread
```
memset, single thread:
  256 b  : 39.28 ms  +103.7%  - 68.3GB/s
  512 b  : 21.22 ms  +10.1%   - 126.5GB/s
  1024 b : 20.17 ms  +4.6%    - 133.1GB/s
  2048 b : 19.80 ms  +2.7%    - 135.6GB/s
  4 Kb   : 19.56 ms  +1.4%    - 137.3GB/s - page size
  8 Kb   : 19.34 ms  +0.3%    - 138.8GB/s
  16 Kb  : 19.28 ms           - 139.2GB/s
  32 Kb  : 19.42 ms  +0.7%    - 138.2GB/s - L1
  64 Kb  : 20.00 ms  +3.7%    - 134.2GB/s
  128 Kb : 20.16 ms  +4.5%    - 133.2GB/s
  256 Kb : 19.95 ms  +3.5%    - 134.5GB/s
  512 Kb : 20.88 ms  +8.3%    - 128.5GB/s - L2
  1024 Kb: 28.06 ms  +45.5%   - 95.7GB/s
  2048 Kb: 28.48 ms  +47.7%   - 94.3GB/s
  4 Mb   : 28.76 ms  +49.2%   - 93.3GB/s
  8 Mb   : 29.04 ms  +50.6%   - 92.4GB/s
  16 Mb  : 80.95 ms  +319.8%  - 33.2GB/s  - L3 bank
  32 Mb  : 0.18 s  +844.9%    - 14.7GB/s
  64 Mb  : 0.11 s  +491.3%    - 23.5GB/s  - L3
  128 Mb : 0.11 s  +494.1%    - 23.4GB/s


SIMD cached fill, single thread:
  256 b  : 19.41 ms  +0.6%    - 138.3GB/s
  512 b  : 19.48 ms  +0.9%    - 137.8GB/s
  1024 b : 19.67 ms  +1.9%    - 136.5GB/s
  2048 b : 19.71 ms  +2.1%    - 136.2GB/s
  4 Kb   : 19.30 ms           - 139.1GB/s - page size
  8 Kb   : 19.40 ms  +0.5%    - 138.4GB/s
  16 Kb  : 19.52 ms  +1.1%    - 137.5GB/s
  32 Kb  : 20.07 ms  +3.9%    - 133.8GB/s - L1 (unused?)
  64 Kb  : 20.60 ms  +6.7%    - 130.3GB/s
  128 Kb : 20.16 ms  +4.5%    - 133.1GB/s
  256 Kb : 20.25 ms  +4.9%    - 132.6GB/s
  512 Kb : 21.09 ms  +9.2%    - 127.3GB/s - L2
  1024 Kb: 27.98 ms  +44.9%   - 96.0GB/s
  2048 Kb: 28.47 ms  +47.5%   - 94.3GB/s
  4 Mb   : 28.67 ms  +48.5%   - 93.6GB/s
  8 Mb   : 28.94 ms  +49.9%   - 92.8GB/s
  16 Mb  : 80.89 ms  +319.1%  - 33.2GB/s  - L3 bank
  32 Mb  : 0.18 s  +828.6%    - 15.0GB/s
  64 Mb  : 0.18 s  +838.2%    - 14.8GB/s  - L3
  128 Mb : 0.18 s  +827.7%    - 15.0GB/s


SIMD non-cached fill, single thread:
  256 b  : 1.07 s  +845.9%    - 2.5GB/s
  512 b  : 0.59 s  +418.7%    - 4.6GB/s
  1024 b : 0.35 s  +208.6%    - 7.7GB/s
  2048 b : 0.23 s  +103.8%    - 11.6GB/s
  4 Kb   : 0.17 s  +52.1%     - 15.6GB/s - page size
  8 Kb   : 0.14 s  +26.3%     - 18.7GB/s
  16 Kb  : 0.13 s  +13.0%     - 20.9GB/s
  32 Kb  : 0.12 s  +6.9%      - 22.1GB/s - L1
  64 Kb  : 0.12 s  +3.7%      - 22.8GB/s
  128 Kb : 0.12 s  +1.8%      - 23.2GB/s
  256 Kb : 0.11 s  +1.2%      - 23.4GB/s
  512 Kb : 0.11 s  +0.7%      - 23.5GB/s
  1024 Kb: 0.11 s  +0.3%      - 23.6GB/s
  2048 Kb: 0.11 s  +0.2%      - 23.6GB/s
  4 Mb   : 0.11 s  +0.2%      - 23.6GB/s
  8 Mb   : 0.11 s  +0.2%      - 23.6GB/s
  16 Mb  : 0.11 s             - 23.7GB/s
  32 Mb  : 0.11 s  +0.1%      - 23.6GB/s
  64 Mb  : 0.11 s  +0.1%      - 23.6GB/s
  128 Mb : 0.11 s  +0.3%      - 23.6GB/s
```

4 threads, bound to physical cores
```
mem copy, 4 threads bound to physical cores:
  256 b  : 41.98 ms  +110.4%  - 255.8GB/s
  512 b  : 25.46 ms  +27.6%   - 421.8GB/s
  1024 b : 20.82 ms  +4.3%    - 0.5TB/s
  2048 b : 20.74 ms  +4.0%    - 0.5TB/s
  4 Kb   : 20.16 ms  +1.1%    - 0.5TB/s    - page size
  8 Kb   : 19.95 ms           - 0.5TB/s
  16 Kb  : 22.54 ms  +13.0%   - 476.3GB/s
  32 Kb  : 40.61 ms  +103.6%  - 264.4GB/s  - L1
  64 Kb  : 40.01 ms  +100.5%  - 268.4GB/s
  128 Kb : 39.95 ms  +100.2%  - 268.8GB/s
  256 Kb : 46.97 ms  +135.4%  - 228.6GB/s
  512 Kb : 54.50 ms  +173.2%  - 197.0GB/s  - L2
  1024 Kb: 54.09 ms  +171.1%  - 198.5GB/s
  2048 Kb: 0.54 s  +2620.3%   - 19.8GB/s   - used non-cached copy
  4 Mb   : 0.54 s  +2625.1%   - 19.7GB/s
  8 Mb   : 0.54 s  +2631.5%   - 19.7GB/s
  16 Mb  : 0.55 s  +2642.1%   - 19.6GB/s   - L3 bank
  32 Mb  : 0.55 s  +2642.6%   - 19.6GB/s
  64 Mb  : 0.55 s  +2641.7%   - 19.6GB/s   - L3
  128 Mb : 0.55 s  +2643.3%   - 19.6GB/s   - 75% of memory bandwidth


SIMD cached copy, 4 threads bound to physical cores:
  256 b  : 28.30 ms  +43.3%   - 379.4GB/s
  512 b  : 19.94 ms  +1.0%    - 0.5TB/s
  1024 b : 19.83 ms  +0.4%    - 0.5TB/s
  2048 b : 19.98 ms  +1.2%    - 0.5TB/s
  4 Kb   : 19.75 ms           - 0.5TB/s
  8 Kb   : 19.84 ms  +0.4%    - 0.5TB/s
  16 Kb  : 20.10 ms  +1.8%    - 0.5TB/s
  32 Kb  : 39.49 ms  +100.0%  - 271.9GB/s  - L1
  64 Kb  : 39.35 ms  +99.3%   - 272.9GB/s
  128 Kb : 39.43 ms  +99.7%   - 272.3GB/s
  256 Kb : 45.35 ms  +129.7%  - 236.7GB/s
  512 Kb : 53.98 ms  +173.3%  - 198.9GB/s  - L2
  1024 Kb: 55.61 ms  +181.6%  - 193.1GB/s
  2048 Kb: 56.70 ms  +187.1%  - 189.4GB/s  << faster than memcpy
  4 Mb   : 0.62 s  +3021.6%   - 17.4GB/s
  8 Mb   : 0.76 s  +3732.0%   - 14.2GB/s
  16 Mb  : 0.91 s  +4521.5%   - 11.8GB/s   - L3 bank
  32 Mb  : 0.91 s  +4516.8%   - 11.8GB/s
  64 Mb  : 0.92 s  +4544.9%   - 11.7GB/s   - L3
  128 Mb : 0.91 s  +4532.6%   - 11.7GB/s


SIMD non-cached copy, 4 threads bound to physical cores:
  256 B  : 3.09 s  +467.2%    - 3.47GB/s
  512 B  : 1.62 s  +196.8%    - 6.63GB/s
  1 KiB  : 0.87 s  +60.1%     - 12.3GB/s
  2 KiB  : 0.67 s  +22.0%     - 16.1GB/s
  4 KiB  : 0.61 s  +11.5%     - 17.7GB/s  - page size
  8 KiB  : 0.56 s  +2.2%      - 19.3GB/s
  16 KiB : 0.55 s  +1.0%      - 19.5GB/s
  32 KiB : 0.55 s  +0.2%      - 19.7GB/s  - L1
  64 KiB : 0.55 s  +0.4%      - 19.6GB/s
  128 KiB: 0.55 s  +0.4%      - 19.6GB/s
  256 KiB: 0.55 s  +0.4%      - 19.6GB/s
  512 KiB: 0.55 s  +0.5%      - 19.6GB/s
  1 MiB  : 0.55 s  +0.2%      - 19.6GB/s
  2 MiB  : 0.55 s  +0.7%      - 19.6GB/s
  4 MiB  : 0.55 s             - 19.7GB/s
  8 MiB  : 0.55 s  +0.1%      - 19.7GB/s
  16 MiB : 0.55 s  +0.0%      - 19.7GB/s
  32 MiB : 0.55 s  +0.3%      - 19.6GB/s
  64 MiB : 0.55 s  +1.5%      - 19.4GB/s
  128 MiB: 0.56 s  +2.0%      - 19.3GB/s


SIMD cached load, non-cached store, 4 threads bound to physical cores:
  256 B  : 2.51 s  +361.1%    - 4.28GB/s  <<
  512 B  : 1.44 s  +164.2%    - 7.47GB/s  <<
  1 KiB  : 0.76 s  +39.7%     - 14.1GB/s  <<
  2 KiB  : 0.60 s  +9.9%      - 18.0GB/s  << faster than non-cached load
  4 KiB  : 0.55 s  +1.1%      - 19.5GB/s  << - page size
  8 KiB  : 0.55 s  +1.3%      - 19.5GB/s
  16 KiB : 0.54 s             - 19.7GB/s
  32 KiB : 0.55 s  +0.6%      - 19.6GB/s
  64 KiB : 0.55 s  +0.2%      - 19.7GB/s
  128 KiB: 0.55 s  +0.4%      - 19.7GB/s
  256 KiB: 0.54 s  +0.2%      - 19.7GB/s
  512 KiB: 0.55 s  +1.6%      - 19.4GB/s
  1 MiB  : 0.55 s  +0.5%      - 19.6GB/s
  2 MiB  : 0.55 s  +0.5%      - 19.6GB/s
  4 MiB  : 0.55 s  +0.4%      - 19.7GB/s
  8 MiB  : 0.55 s  +0.6%      - 19.6GB/s
  16 MiB : 0.55 s  +0.8%      - 19.6GB/s
  32 MiB : 0.55 s  +1.1%      - 19.5GB/s
  64 MiB : 0.56 s  +3.3%      - 19.1GB/s
  128 MiB: 0.55 s  +1.3%      - 19.5GB/s
```

# AMD Phenom II X4 945, 3 GHz, 45nm (Deneb, 2009)

* Memory: DDR3, 1333 MT/s, 10.6GB/s
* Cache:
	- L1I: 4x 64KB
	- L1D: 4x 64KB
	- L2: 4x 512KB
	- L3: 6MB, 4x 1.5MB

```
std::memset, single thread:
  256 b  : 76.17 ms  +131.3%  - 17.6GB/s
  512 b  : 57.69 ms  +75.2%   - 23.3GB/s
  1024 b : 48.77 ms  +48.1%   - 27.5GB/s
  2048 b : 43.03 ms  +30.7%   - 31.2GB/s
  4 Kb   : 37.28 ms  +13.2%   - 36.0GB/s
  8 Kb   : 34.68 ms  +5.3%    - 38.7GB/s
  16 Kb  : 39.36 ms  +19.5%   - 34.1GB/s
  32 Kb  : 38.15 ms  +15.9%   - 35.2GB/s
  64 Kb  : 32.93 ms           - 40.8GB/s  - L1
  128 Kb : 95.97 ms  +191.4%  - 14.0GB/s
  256 Kb : 0.13 s  +287.0%    - 10.5GB/s
  512 Kb : 0.10 s  +211.0%    - 13.1GB/s  - L2
  1024 Kb: 0.16 s  +391.9%    - 8.3GB/s
  2048 Kb: 0.17 s  +405.0%    - 8.1GB/s   - L3 1.5MB bank
  4 Mb   : 0.19 s  +478.1%    - 7.0GB/s
  8 Mb   : 0.35 s  +951.6%    - 3.9GB/s   - L3 6MB
  16 Mb  : 0.36 s  +1001.5%   - 3.7GB/s
  32 Mb  : 0.36 s  +1000.7%   - 3.7GB/s
  64 Mb  : 0.36 s  +1008.2%   - 3.7GB/s

std::memcpy, single thread:
  256 b  : 0.10 s  +88.8%     - 13.0GB/s
  512 b  : 74.67 ms  +37.0%   - 18.0GB/s
  1024 b : 66.58 ms  +22.1%   - 20.2GB/s
  2048 b : 62.47 ms  +14.6%   - 21.5GB/s
  4 Kb   : 55.15 ms  +1.2%    - 24.3GB/s
  8 Kb   : 54.51 ms           - 24.6GB/s
  16 Kb  : 57.49 ms  +5.5%    - 23.3GB/s
  32 Kb  : 61.11 ms  +12.1%   - 22.0GB/s
  64 Kb  : 0.15 s  +183.6%    - 8.7GB/s  - L1
  128 Kb : 0.15 s  +167.6%    - 9.2GB/s
  256 Kb : 0.16 s  +199.9%    - 8.2GB/s
  512 Kb : 0.32 s  +495.5%    - 4.1GB/s  - L2
  1024 Kb: 0.33 s  +500.3%    - 4.1GB/s
  2048 Kb: 0.36 s  +552.2%    - 3.8GB/s  - L3 1.5MB bank
  4 Mb   : 0.55 s  +906.5%    - 2.4GB/s
  8 Mb   : 0.57 s  +940.0%    - 2.4GB/s  - L3 6MB
  16 Mb  : 0.56 s  +923.3%    - 2.4GB/s
  32 Mb  : 0.57 s  +939.4%    - 2.4GB/s
  64 Mb  : 0.57 s  +946.5%    - 2.4GB/s

SIMD cached copy, single thread:
  256 b  : 68.44 ms  +12.7%   - 19.6GB/s
  512 b  : 68.59 ms  +12.9%   - 19.6GB/s
  1024 b : 61.15 ms  +0.7%    - 21.9GB/s
  2048 b : 62.69 ms  +3.2%    - 21.4GB/s
  4 Kb   : 64.69 ms  +6.5%    - 20.7GB/s
  8 Kb   : 67.25 ms  +10.8%   - 20.0GB/s
  16 Kb  : 60.72 ms           - 22.1GB/s
  32 Kb  : 66.18 ms  +9.0%    - 20.3GB/s
  64 Kb  : 0.14 s  +126.0%    - 9.8GB/s  - L1
  128 Kb : 0.16 s  +161.7%    - 8.4GB/s
  256 Kb : 0.15 s  +154.5%    - 8.7GB/s
  512 Kb : 0.32 s  +432.4%    - 4.2GB/s  - L2
  1024 Kb: 0.32 s  +434.2%    - 4.1GB/s
  2048 Kb: 0.36 s  +491.4%    - 3.7GB/s  - L3 1.5MB bank
  4 Mb   : 0.56 s  +823.4%    - 2.4GB/s
  8 Mb   : 0.58 s  +858.3%    - 2.3GB/s  - L3 6MB
  16 Mb  : 0.58 s  +856.4%    - 2.3GB/s
  32 Mb  : 0.58 s  +852.7%    - 2.3GB/s
  64 Mb  : 0.58 s  +862.6%    - 2.3GB/s

SIMD non-cached copy, single thread:
  256 b  : 0.85 s  +328.0%    - 0.8GB/s
  512 b  : 0.45 s  +127.1%    - 1.5GB/s
  1024 b : 0.39 s  +94.5%     - 1.7GB/s
  2048 b : 0.34 s  +70.9%     - 2.0GB/s
  4 Kb   : 0.32 s  +62.5%     - 2.1GB/s
  8 Kb   : 0.33 s  +65.7%     - 2.0GB/s
  16 Kb  : 0.32 s  +62.2%     - 2.1GB/s
  32 Kb  : 0.32 s  +62.1%     - 2.1GB/s
  64 Kb  : 0.32 s  +62.9%     - 2.1GB/s  - L1
  128 Kb : 0.31 s  +55.8%     - 2.2GB/s
  256 Kb : 0.31 s  +53.3%     - 2.2GB/s
  512 Kb : 0.31 s  +55.2%     - 2.2GB/s  - L2
  1024 Kb: 0.20 s  +0.6%      - 3.4GB/s
  2048 Kb: 0.20 s             - 3.4GB/s
  4 Mb   : 0.20 s  +0.1%      - 3.4GB/s  << faster than memcpy
  8 Mb   : 0.20 s  +2.0%      - 3.3GB/s  << L3 6MB
  16 Mb  : 0.22 s  +10.5%     - 3.1GB/s  <<
  32 Mb  : 0.22 s  +8.4%      - 3.1GB/s  <<
```

```
mem copy, 4 threads:
  256 b  : 0.12 s  +114.8%    - 43.7GB/s
  512 b  : 78.47 ms  +37.1%   - 68.4GB/s
  1024 b : 65.91 ms  +15.1%   - 81.5GB/s
  2048 b : 66.56 ms  +16.3%   - 80.7GB/s
  4 Kb   : 60.09 ms  +5.0%    - 89.3GB/s
  8 Kb   : 57.25 ms           - 93.8GB/s
  16 Kb  : 62.44 ms  +9.1%    - 86.0GB/s
  32 Kb  : 60.78 ms  +6.2%    - 88.3GB/s
  64 Kb  : 0.14 s  +151.2%    - 37.3GB/s - L1
  128 Kb : 0.15 s  +165.1%    - 35.4GB/s
  256 Kb : 0.15 s  +169.1%    - 34.9GB/s
  512 Kb : 0.33 s  +474.8%    - 16.3GB/s - L2
  1024 Kb: 0.37 s  +537.8%    - 14.7GB/s
  2048 Kb: 1.34 s  +2234.9%   - 4.0GB/s
  4 Mb   : 1.34 s  +2237.7%   - 4.0GB/s  - L3 6MB
  8 Mb   : 1.34 s  +2233.8%   - 4.0GB/s
  16 Mb  : 1.34 s  +2233.6%   - 4.0GB/s
  32 Mb  : 1.32 s  +2207.5%   - 4.1GB/s
  64 Mb  : 1.32 s  +2207.9%   - 4.1GB/s


SIMD cached copy, 4 threads:
  256 b  : 0.11 s  +49.2%     - 49.7GB/s
  512 b  : 75.57 ms  +4.4%    - 71.0GB/s
  1024 b : 72.41 ms           - 74.1GB/s
  2048 b : 77.74 ms  +7.4%    - 69.1GB/s
  4 Kb   : 81.24 ms  +12.2%   - 66.1GB/s
  8 Kb   : 78.90 ms  +9.0%    - 68.0GB/s
  16 Kb  : 77.27 ms  +6.7%    - 69.5GB/s
  32 Kb  : 78.05 ms  +7.8%    - 68.8GB/s
  64 Kb  : 0.20 s  +176.0%    - 26.9GB/s - L1
  128 Kb : 0.22 s  +205.8%    - 24.2GB/s
  256 Kb : 0.24 s  +234.7%    - 22.1GB/s
  512 Kb : 0.44 s  +514.1%    - 12.1GB/s - L2
  1024 Kb: 0.48 s  +565.4%    - 11.1GB/s
  2048 Kb: 1.47 s  +1927.1%   - 3.7GB/s  - L3 1.5MB bank?
  4 Mb   : 1.47 s  +1935.3%   - 3.6GB/s  - L3 6MB
  8 Mb   : 1.50 s  +1969.7%   - 3.6GB/s
  16 Mb  : 1.51 s  +1985.6%   - 3.6GB/s
  32 Mb  : 1.51 s  +1986.2%   - 3.6GB/s
  64 Mb  : 1.46 s  +1914.7%   - 3.7GB/s


SIMD non-cached copy, 4 threads:
  256 b  : 0.93 s  +106.5%    - 2.9GB/s
  512 b  : 0.72 s  +60.8%     - 3.7GB/s
  1024 b : 0.52 s  +15.4%     - 5.2GB/s
  2048 b : 0.48 s  +7.0%      - 5.6GB/s
  4 Kb   : 0.48 s  +6.4%      - 5.6GB/s
  8 Kb   : 0.47 s  +4.0%      - 5.7GB/s
  16 Kb  : 0.49 s  +8.5%      - 5.5GB/s
  32 Kb  : 0.49 s  +8.2%      - 5.5GB/s
  64 Kb  : 0.49 s  +8.8%      - 5.5GB/s  - L1
  128 Kb : 0.50 s  +10.2%     - 5.4GB/s
  256 Kb : 0.49 s  +9.3%      - 5.5GB/s
  512 Kb : 0.48 s  +7.2%      - 5.6GB/s  - L2
  1024 Kb: 0.45 s             - 6.0GB/s
  2048 Kb: 0.46 s  +3.2%      - 5.8GB/s  <<
  4 Mb   : 0.47 s  +3.6%      - 5.8GB/s  << faster than memcpy
  8 Mb   : 0.47 s  +3.8%      - 5.8GB/s  << L3 6MB
  16 Mb  : 0.47 s  +5.5%      - 5.7GB/s  <<
  32 Mb  : 0.48 s  +6.0%      - 5.6GB/s  <<
```

# Intel i5-8250U, 3.39 GHz

https://en.wikichip.org/wiki/intel/core_i5/i5-8250u

* Cache:
	- L1I: 4x 32KB
	- L1D: 4x 32KB
	- L2: 4x 256KB
	- L3: 6MB: 4x 1.5MB
* Memory: 8GB DDR3, 1867MHz, dual channel, 29.8 GB/s

```
std::memset, single thread:
  256 b  : 91.40 ms  +260.9%  - 29.4GB/s
  512 b  : 43.51 ms  +71.8%   - 61.7GB/s
  1024 b : 34.31 ms  +35.5%   - 78.2GB/s
  2048 b : 26.73 ms  +5.5%    - 100.4GB/s
  4 Kb   : 25.46 ms  +0.5%    - 105.4GB/s
  8 Kb   : 27.40 ms  +8.2%    - 98.0GB/s
  16 Kb  : 25.32 ms           - 106.0GB/s
  32 Kb  : 32.04 ms  +26.5%   - 83.8GB/s  - L1
  64 Kb  : 49.81 ms  +96.7%   - 53.9GB/s
  128 Kb : 52.80 ms  +108.5%  - 50.8GB/s
  256 Kb : 56.50 ms  +123.1%  - 47.5GB/s  - L2
  512 Kb : 55.27 ms  +118.3%  - 48.6GB/s
  1024 Kb: 57.39 ms  +126.6%  - 46.8GB/s
  2048 Kb: 59.84 ms  +136.3%  - 44.9GB/s  - L3 1.5MB bank
  4 Mb   : 62.84 ms  +148.2%  - 42.7GB/s
  8 Mb   : 97.71 ms  +285.9%  - 27.5GB/s  - L3 6MB
  16 Mb  : 0.10 s  +308.0%    - 26.0GB/s
  32 Mb  : 0.10 s  +308.1%    - 26.0GB/s
  64 Mb  : 0.11 s  +322.8%    - 25.1GB/s
  128 Mb : 0.10 s  +313.4%    - 25.6GB/s


std::memcpy, single thread:
  256 b  : 57.52 ms  +121.5%  - 46.7GB/s
  512 b  : 44.39 ms  +70.9%   - 60.5GB/s
  1024 b : 32.34 ms  +24.5%   - 83.0GB/s
  2048 b : 27.00 ms  +4.0%    - 99.4GB/s
  4 Kb   : 26.05 ms  +0.3%    - 103.1GB/s - page size
  8 Kb   : 25.97 ms           - 103.4GB/s
  16 Kb  : 28.90 ms  +11.3%   - 92.9GB/s
  32 Kb  : 59.87 ms  +130.5%  - 44.8GB/s  - L1
  64 Kb  : 59.52 ms  +129.2%  - 45.1GB/s
  128 Kb : 62.50 ms  +140.7%  - 42.9GB/s
  256 Kb : 91.09 ms  +250.7%  - 29.5GB/s  - L2
  512 Kb : 95.40 ms  +267.3%  - 28.1GB/s
  1024 Kb: 96.08 ms  +269.9%  - 27.9GB/s
  2048 Kb: 0.26 s  +892.7%    - 10.4GB/s  - L3 1.5MB bank
  4 Mb   : 0.27 s  +944.7%    - 9.9GB/s
  8 Mb   : 0.27 s  +934.5%    - 10.0GB/s  - L3 6MB
  16 Mb  : 0.27 s  +935.9%    - 10.0GB/s
  32 Mb  : 0.27 s  +937.0%    - 10.0GB/s
  64 Mb  : 0.27 s  +936.5%    - 10.0GB/s
  128 Mb : 0.27 s  +939.3%    - 9.9GB/s


SIMD cached copy, single thread:
  256 b  : 28.34 ms  +13.4%   - 94.7GB/s
  512 b  : 25.00 ms  +0.0%    - 107.4GB/s
  1024 b : 25.07 ms  +0.3%    - 107.1GB/s
  2048 b : 25.39 ms  +1.6%    - 105.7GB/s
  4 Kb   : 24.99 ms  +0.0%    - 107.4GB/s
  8 Kb   : 24.99 ms           - 107.4GB/s
  16 Kb  : 25.62 ms  +2.5%    - 104.8GB/s
  32 Kb  : 64.46 ms  +157.9%  - 41.6GB/s  - L1
  64 Kb  : 64.07 ms  +156.4%  - 41.9GB/s
  128 Kb : 69.62 ms  +178.6%  - 38.6GB/s
  256 Kb : 0.12 s  +365.7%    - 23.1GB/s  - L2
  512 Kb : 0.12 s  +367.2%    - 23.0GB/s
  1024 Kb: 0.12 s  +370.5%    - 22.8GB/s
  2048 Kb: 0.13 s  +409.6%    - 21.1GB/s  - L3 1.5MB bank
  4 Mb   : 0.25 s  +890.1%    - 10.8GB/s
  8 Mb   : 0.30 s  +1112.4%   - 8.9GB/s   - L3 6MB
  16 Mb  : 0.34 s  +1257.9%   - 7.9GB/s
  32 Mb  : 0.36 s  +1343.4%   - 7.4GB/s
  64 Mb  : 0.37 s  +1380.3%   - 7.3GB/s
  128 Mb : 0.37 s  +1376.4%   - 7.3GB/s
```

```
mem copy, 4 threads bound to physical cores:
  256 b  : 50.27 ms  +149.7%  - 106.8GB/s
  512 b  : 43.65 ms  +116.9%  - 123.0GB/s
  1024 b : 24.49 ms  +21.7%   - 219.2GB/s
  2048 b : 34.15 ms  +69.6%   - 157.2GB/s
  4 Kb   : 20.13 ms           - 266.7GB/s  - page size
  8 Kb   : 21.50 ms  +6.8%    - 249.7GB/s
  16 Kb  : 29.81 ms  +48.1%   - 180.1GB/s
  32 Kb  : 44.71 ms  +122.1%  - 120.1GB/s  - L1
  64 Kb  : 38.49 ms  +91.2%   - 139.5GB/s
  128 Kb : 42.74 ms  +112.3%  - 125.6GB/s
  256 Kb : 58.59 ms  +191.1%  - 91.6GB/s   - L2
  512 Kb : 71.84 ms  +256.9%  - 74.7GB/s
  1024 Kb: 0.28 s  +1276.3%   - 19.4GB/s
  2048 Kb: 0.49 s  +2311.5%   - 11.1GB/s   - L3 1.5MB bank
  4 Mb   : 0.49 s  +2329.4%   - 11.0GB/s
  8 Mb   : 0.49 s  +2313.0%   - 11.1GB/s   - L3 6MB
  16 Mb  : 0.49 s  +2333.6%   - 11.0GB/s
  32 Mb  : 0.49 s  +2351.8%   - 10.9GB/s
  64 Mb  : 0.49 s  +2313.5%   - 11.1GB/s


mem copy, 4 threads bound to logical cores:
  256 b  : 81.88 ms  +187.3%  - 65.6GB/s
  512 b  : 51.30 ms  +80.0%   - 104.7GB/s
  1024 b : 35.09 ms  +23.1%   - 153.0GB/s
  2048 b : 30.09 ms  +5.6%    - 178.4GB/s
  4 Kb   : 28.50 ms           - 188.4GB/s - page size
  8 Kb   : 33.25 ms  +16.7%   - 161.5GB/s
  16 Kb  : 63.86 ms  +124.1%  - 84.1GB/s  - L1 shared between 2 threads
  32 Kb  : 61.93 ms  +117.3%  - 86.7GB/s  - L1
  64 Kb  : 68.09 ms  +138.9%  - 78.9GB/s
  128 Kb : 94.11 ms  +230.2%  - 57.0GB/s
  256 Kb : 0.10 s  +252.4%    - 53.5GB/s  - L2
  512 Kb : 0.12 s  +306.3%    - 46.4GB/s
  1024 Kb: 0.31 s  +992.7%    - 17.2GB/s
  2048 Kb: 0.47 s  +1564.5%   - 11.3GB/s  - L3 1.5MB bank
  4 Mb   : 0.48 s  +1569.9%   - 11.3GB/s
  8 Mb   : 0.48 s  +1572.6%   - 11.3GB/s  - L3 6MB
  16 Mb  : 0.48 s  +1574.9%   - 11.2GB/s
  32 Mb  : 0.48 s  +1567.6%   - 11.3GB/s
  64 Mb  : 0.48 s  +1567.8%   - 11.3GB/s


SIMD cached copy, 4 threads bound to physical cores:
  256 b  : 41.82 ms  +192.3%  - 128.4GB/s
  512 b  : 20.84 ms  +45.7%   - 257.6GB/s
  1024 b : 21.65 ms  +51.3%   - 248.0GB/s
  2048 b : 16.70 ms  +16.8%   - 321.4GB/s
  4 Kb   : 19.08 ms  +33.4%   - 281.3GB/s
  8 Kb   : 14.31 ms           - 375.3GB/s
  16 Kb  : 18.13 ms  +26.7%   - 296.2GB/s
  32 Kb  : 42.84 ms  +199.5%  - 125.3GB/s  - L1
  64 Kb  : 45.41 ms  +217.4%  - 118.2GB/s
  128 Kb : 52.29 ms  +265.5%  - 102.7GB/s
  256 Kb : 72.37 ms  +405.9%  - 74.2GB/s   - L2
  512 Kb : 78.35 ms  +447.7%  - 68.5GB/s
  1024 Kb: 0.21 s  +1364.0%   - 25.6GB/s
  2048 Kb: 0.57 s  +3898.4%   - 9.4GB/s    - L3 1.5MB bank
  4 Mb   : 0.65 s  +4463.7%   - 8.2GB/s
  8 Mb   : 0.68 s  +4681.6%   - 7.8GB/s    - L3 6MB
  16 Mb  : 0.69 s  +4744.7%   - 7.7GB/s
  32 Mb  : 0.69 s  +4745.3%   - 7.7GB/s
  64 Mb  : 0.70 s  +4767.8%   - 7.7GB/s


SIMD non-cached copy, 4 threads bound to physical cores:
  256 b  : 1.12 s  +232.0%    - 4.8GB/s
  512 b  : 0.60 s  +79.4%     - 8.9GB/s
  1024 b : 0.38 s  +13.3%     - 14.1GB/s
  2048 b : 0.34 s             - 16.0GB/s
  4 Kb   : 0.42 s  +24.5%     - 12.8GB/s  - page size
  8 Kb   : 0.45 s  +32.6%     - 12.0GB/s
  16 Kb  : 0.46 s  +35.5%     - 11.8GB/s
  32 Kb  : 0.45 s  +34.9%     - 11.8GB/s  - L1
  64 Kb  : 0.45 s  +35.1%     - 11.8GB/s
  128 Kb : 0.46 s  +37.4%     - 11.6GB/s
  256 Kb : 0.46 s  +37.1%     - 11.6GB/s
  512 Kb : 0.46 s  +37.4%     - 11.6GB/s
  1024 Kb: 0.46 s  +37.9%     - 11.6GB/s
  2048 Kb: 0.47 s  +38.5%     - 11.5GB/s
  4 Mb   : 0.47 s  +38.6%     - 11.5GB/s
  8 Mb   : 0.47 s  +38.5%     - 11.5GB/s
  16 Mb  : 0.47 s  +38.8%     - 11.5GB/s
  32 Mb  : 0.47 s  +39.1%     - 11.5GB/s
  64 Mb  : 0.47 s  +40.4%     - 11.4GB/s
```

# Realme 8i, MediaTek Helio G96, LPDDR4

* block size: 1GB
* Memory: 4GB, LPDDR4X, DC 16bit, 2133 MHz, **17.07** GB/s

### Performance (Cortex A76, 2050 MHz, 12nm)

```
MemCopy32 -        : 0.18 s             - 6.1GB/s
MemCopy64 -        : 0.18 s  +1.9%      - 6.0GB/s
MemCopy16 -        : 0.19 s  +8.8%      - 5.6GB/s
MemCopy16 - neon v1: 0.19 s  +9.1%      - 5.6GB/s
MemCopy16 - neon v2: 0.19 s  +10.9%     - 5.5GB/s
std::memset        : 0.23 s  +30.6%     - 4.7GB/s
std::memcpy        : 0.30 s  +72.5%     - 3.5GB/s
```

### EnergyEfficient (Cortex A55, 2000 MHz, 12nm)

```
std::memset        : 0.13 s             - 8.6GB/s
MemCopy16 -        : 0.34 s  +171.9%    - 3.1GB/s
MemCopy16 - neon v2: 0.35 s  +175.3%    - 3.1GB/s
MemCopy16 - neon v1: 0.35 s  +181.5%    - 3.0GB/s
MemCopy32 -        : 0.37 s  +191.3%    - 2.9GB/s
MemCopy64 -        : 0.37 s  +197.4%    - 2.9GB/s
std::memcpy        : 0.47 s  +272.1%    - 2.3GB/s
```


# Asus ROG Phone 5, Snapdragon 888

2GB block

### HighPerformance

```
std::memset        : 97.74 ms           - 22.0GB/s
MemCopy16 - neon v1: 0.13 s  +33.7%     - 16.4GB/s
MemCopy16 - neon v2: 0.13 s  +34.9%     - 16.3GB/s
MemCopy32 -        : 0.14 s  +38.4%     - 15.9GB/s
MemCopy16 -        : 0.14 s  +38.5%     - 15.9GB/s
MemCopy64 -        : 0.14 s  +44.9%     - 15.2GB/s
std::memcpy        : 0.27 s  +179.9%    - 7.8GB/s
```

### Performance

```
std::memset        : 63.33 ms           - 33.9GB/s
MemCopy16 - neon v1: 0.16 s  +147.7%    - 13.7GB/s
MemCopy16 - neon v2: 0.16 s  +147.7%    - 13.7GB/s
MemCopy32 -        : 0.16 s  +152.2%    - 13.4GB/s
MemCopy64 -        : 0.16 s  +152.2%    - 13.4GB/s
MemCopy16 -        : 0.16 s  +152.3%    - 13.4GB/s
std::memcpy        : 0.16 s  +152.5%    - 13.4GB/s
```

### EnergyEfficient

```
std::memset        : 0.24 s             - 9.1GB/s
MemCopy16 - neon v1: 0.63 s  +164.7%    - 3.4GB/s
std::memcpy        : 0.63 s  +166.5%    - 3.4GB/s
MemCopy16 -        : 0.63 s  +166.7%    - 3.4GB/s
MemCopy64 -        : 0.64 s  +168.2%    - 3.4GB/s
MemCopy32 -        : 0.64 s  +168.4%    - 3.4GB/s
MemCopy16 - neon v2: 0.64 s  +169.0%    - 3.4GB/s
```


# Motorola G54, MediaTek Dimensity 7020

2GB block

### Performance

```
std::memset        : 0.22 s             - 9.9GB/s
MemCopy16 - neon v2: 0.28 s  +29.3%     - 7.7GB/s
MemCopy16 -        : 0.28 s  +29.7%     - 7.6GB/s
MemCopy16 - neon v1: 0.28 s  +29.8%     - 7.6GB/s
MemCopy64 -        : 0.28 s  +29.9%     - 7.6GB/s
MemCopy32 -        : 0.28 s  +31.0%     - 7.6GB/s
std::memcpy        : 0.36 s  +68.1%     - 5.9GB/s
```

### EnergyEfficient

```
std::memset        : 0.21 s             - 10.4GB/s
MemCopy16 - neon v1: 0.62 s  +200.0%    - 3.5GB/s
MemCopy16 - neon v2: 0.62 s  +201.2%    - 3.5GB/s
MemCopy16 -        : 0.62 s  +202.7%    - 3.4GB/s
MemCopy32 -        : 0.63 s  +203.7%    - 3.4GB/s
std::memcpy        : 0.63 s  +205.2%    - 3.4GB/s
MemCopy64 -        : 0.63 s  +206.0%    - 3.4GB/s
```


# Redmi 7A

256 MB block

### Performance

```
std::memset        : 0.11 s             - 2.4GB/s
MemCopy64 -        : 0.12 s  +6.9%      - 2.2GB/s
MemCopy16 - neon v2: 0.13 s  +13.8%     - 2.1GB/s
MemCopy16 - neon v1: 0.13 s  +14.8%     - 2.1GB/s
MemCopy32 -        : 0.14 s  +18.9%     - 2.0GB/s
MemCopy16 -        : 0.14 s  +20.5%     - 2.0GB/s
std::memcpy        : 0.17 s  +53.7%     - 1.5GB/s
```

### EnergyEfficient

```
std::memset        : 45.84 ms           - 5.9GB/s
MemCopy64 -        : 0.12 s  +164.7%    - 2.2GB/s
MemCopy32 -        : 0.12 s  +165.3%    - 2.2GB/s
std::memcpy        : 0.12 s  +166.2%    - 2.2GB/s
MemCopy16 -        : 0.12 s  +167.4%    - 2.2GB/s
MemCopy16 - neon v2: 0.14 s  +202.7%    - 1.9GB/s
MemCopy16 - neon v1: 0.14 s  +203.6%    - 1.9GB/s
```


# Apple Mac M1

### Performance

* Cache:
	- L1D: 4x 128 KB
	- L2:  4x 3MB,  12 MB
* Memory: 16 GB, LPDDR4X-4266 DC 16bit, 2133 MHz, 128bit Bus, 68.25 GB/s

```
memset, single thread:
  256 B  : 32.41 ms  +20.9%     - 82.8GB/s
  512 B  : 30.03 ms  +12.0%     - 89.4GB/s
  1 KiB  : 28.35 ms  +5.8%      - 94.7GB/s
  2 KiB  : 27.71 ms  +3.4%      - 96.9GB/s
  4 KiB  : 26.80 ms             - 100.2GB/s
  8 KiB  : 29.09 ms  +8.5%      - 92.3GB/s
  16 KiB : 26.96 ms  +0.6%      - 99.6GB/s  - page size
  32 KiB : 41.03 ms  +53.1%     - 65.4GB/s
  64 KiB : 58.21 ms  +117.2%    - 46.1GB/s
  128 KiB: 66.70 ms  +148.9%    - 40.2GB/s  - L1
  256 KiB: 70.25 ms  +162.1%    - 38.2GB/s
  512 KiB: 69.97 ms  +161.1%    - 38.4GB/s
  1 MiB  : 73.07 ms  +172.6%    - 36.7GB/s
  2 MiB  : 71.23 ms  +165.8%    - 37.7GB/s
  4 MiB  : 73.35 ms  +173.7%    - 36.6GB/s  - L2 bank 3MB
  8 MiB  : 71.20 ms  +165.7%    - 37.7GB/s
  16 MiB : 73.15 ms  +172.9%    - 36.7GB/s  - L2 12MB
  32 MiB : 74.54 ms  +178.1%    - 36.0GB/s
  64 MiB : 69.78 ms  +160.4%    - 38.5GB/s
  128 MiB: 70.12 ms  +161.6%    - 38.3GB/s

SIMD cached fill, single thread:
  256 B  : 29.53 ms  +11.5%     - 90.9GB/s
  512 B  : 28.06 ms  +5.9%      - 95.7GB/s
  1 KiB  : 27.18 ms  +2.6%      - 98.8GB/s
  2 KiB  : 26.75 ms  +1.0%      - 100.4GB/s
  4 KiB  : 26.49 ms             - 101.3GB/s
  8 KiB  : 27.84 ms  +5.1%      - 96.4GB/s
  16 KiB : 26.58 ms  +0.4%      - 101.0GB/s - page size
  32 KiB : 30.18 ms  +13.9%     - 89.0GB/s  <<
  64 KiB : 35.59 ms  +34.4%     - 75.4GB/s  <<
  128 KiB: 38.22 ms  +44.3%     - 70.2GB/s  << - L1
  256 KiB: 35.55 ms  +34.2%     - 75.5GB/s  <<
  512 KiB: 38.73 ms  +46.2%     - 69.3GB/s  << faster than memset
  1 MiB  : 36.31 ms  +37.1%     - 73.9GB/s  <<
  2 MiB  : 39.13 ms  +47.7%     - 68.6GB/s  <<
  4 MiB  : 38.29 ms  +44.6%     - 70.1GB/s  << - L2 bank 3MB
  8 MiB  : 40.63 ms  +53.4%     - 66.1GB/s  <<
  16 MiB : 42.86 ms  +61.8%     - 62.6GB/s  << - L2 12MB
  32 MiB : 42.65 ms  +61.0%     - 62.9GB/s  <<
  64 MiB : 42.84 ms  +61.7%     - 62.7GB/s  <<
  128 MiB: 42.92 ms  +62.0%     - 62.5GB/s  <<

SIMD non-cached fill, single thread:
  16 KiB : 43.15 ms             - 62.2GB/s
  8 KiB  : 59.67 ms  +38.3%     - 45.0GB/s
  32 KiB : 62.87 ms  +45.7%     - 42.7GB/s
  128 KiB: 73.26 ms  +69.8%     - 36.6GB/s
  64 KiB : 73.91 ms  +71.3%     - 36.3GB/s
  32 MiB : 74.17 ms  +71.9%     - 36.2GB/s
  16 MiB : 74.32 ms  +72.2%     - 36.1GB/s
  2 MiB  : 74.65 ms  +73.0%     - 36.0GB/s
  512 KiB: 74.67 ms  +73.0%     - 35.9GB/s
  1 MiB  : 74.90 ms  +73.6%     - 35.8GB/s
  128 MiB: 75.77 ms  +75.6%     - 35.4GB/s
  8 MiB  : 75.98 ms  +76.1%     - 35.3GB/s
  64 MiB : 76.24 ms  +76.7%     - 35.2GB/s
  256 KiB: 76.67 ms  +77.7%     - 35.0GB/s
  4 MiB  : 76.74 ms  +77.8%     - 35.0GB/s
  4 KiB  : 0.11 s    +146.8%    - 25.2GB/s
  2 KiB  : 0.17 s    +291.0%    - 15.9GB/s
  1 KiB  : 0.34 s    +678.5%    - 7.99GB/s
  512 B  : 0.67 s    +1453.5%   - 4.00GB/s
  256 B  : 0.70 s    +1516.8%   - 3.85GB/s

memcpy, single thread:
  256 B  : 45.94 ms  +34.2%     - 58.4GB/s
  512 B  : 36.07 ms  +5.4%      - 74.4GB/s
  1 KiB  : 36.18 ms  +5.7%      - 74.2GB/s
  2 KiB  : 35.80 ms  +4.6%      - 75.0GB/s
  4 KiB  : 34.23 ms             - 78.4GB/s
  8 KiB  : 34.68 ms  +1.3%      - 77.4GB/s
  16 KiB : 34.95 ms  +2.1%      - 76.8GB/s  - page size
  32 KiB : 35.08 ms  +2.5%      - 76.5GB/s
  64 KiB : 36.18 ms  +5.7%      - 74.2GB/s
  128 KiB: 85.52 ms  +149.8%    - 31.4GB/s  - L1
  256 KiB: 38.35 ms  +12.0%     - 70.0GB/s
  512 KiB: 59.68 ms  +74.3%     - 45.0GB/s
  1 MiB  : 66.46 ms  +94.1%     - 40.4GB/s
  2 MiB  : 68.73 ms  +100.8%    - 39.1GB/s
  4 MiB  : 68.85 ms  +101.1%    - 39.0GB/s  - L2 bank 3MB
  8 MiB  : 90.66 ms  +164.8%    - 29.6GB/s
  16 MiB : 92.71 ms  +170.8%    - 29.0GB/s  - L2 12MB
  32 MiB : 92.87 ms  +171.3%    - 28.9GB/s
  64 MiB : 93.03 ms  +171.8%    - 28.9GB/s
  128 MiB: 93.21 ms  +172.3%    - 28.8GB/s

SIMD cached copy, single thread:
  256 B  : 37.44 ms  +7.7%      - 71.7GB/s  << faster than memcpy
  512 B  : 36.12 ms  +3.9%      - 74.3GB/s
  1 KiB  : 35.51 ms  +2.2%      - 75.6GB/s
  2 KiB  : 35.45 ms  +2.0%      - 75.7GB/s
  4 KiB  : 35.46 ms  +2.0%      - 75.7GB/s
  8 KiB  : 34.75 ms             - 77.3GB/s
  16 KiB : 35.20 ms  +1.3%      - 76.3GB/s  - page size
  32 KiB : 35.34 ms  +1.7%      - 76.0GB/s
  64 KiB : 36.17 ms  +4.1%      - 74.2GB/s
  128 KiB: 63.26 ms  +82.1%     - 42.4GB/s  - L1
  256 KiB: 56.27 ms  +61.9%     - 47.7GB/s
  512 KiB: 56.21 ms  +61.7%     - 47.8GB/s
  1 MiB  : 56.05 ms  +61.3%     - 47.9GB/s  <<
  2 MiB  : 56.16 ms  +61.6%     - 47.8GB/s  <<
  4 MiB  : 59.22 ms  +70.4%     - 45.3GB/s  << - L2 bank 3MB
  8 MiB  : 61.51 ms  +77.0%     - 43.6GB/s  <<
  16 MiB : 93.30 ms  +168.5%    - 28.8GB/s  - L2 12MB
  32 MiB : 92.61 ms  +166.5%    - 29.0GB/s
  64 MiB : 92.50 ms  +166.2%    - 29.0GB/s
  128 MiB: 92.72 ms  +166.8%    - 29.0GB/s

SIMD non-cached copy, single thread:
  256 B  : 92.27 ms  +177.7%    - 29.1GB/s
  512 B  : 35.31 ms  +6.3%      - 76.0GB/s
  1 KiB  : 34.14 ms  +2.8%      - 78.6GB/s
  2 KiB  : 33.75 ms  +1.6%      - 79.5GB/s
  8 KiB  : 34.46 ms  +3.7%      - 77.9GB/s
  4 KiB  : 33.61 ms  +1.2%      - 79.9GB/s
  16 KiB : 33.62 ms  +1.2%      - 79.8GB/s  - page size
  32 KiB : 33.22 ms             - 80.8GB/s
  64 KiB : 34.38 ms  +3.5%      - 78.1GB/s
  128 KiB: 62.18 ms  +87.2%     - 43.2GB/s  - L1
  256 KiB: 55.94 ms  +68.4%     - 48.0GB/s
  512 KiB: 55.81 ms  +68.0%     - 48.1GB/s
  1 MiB  : 55.68 ms  +67.6%     - 48.2GB/s  << faster than memcpy
  2 MiB  : 56.04 ms  +68.7%     - 47.9GB/s  <<
  4 MiB  : 58.64 ms  +76.5%     - 45.8GB/s  << - L2 bank 3MB
  8 MiB  : 62.12 ms  +87.0%     - 43.2GB/s  <<
  16 MiB : 94.02 ms  +183.0%    - 28.6GB/s  - L2 12MB
  32 MiB : 92.56 ms  +178.6%    - 29.0GB/s
  64 MiB : 92.64 ms  +178.9%    - 29.0GB/s
  128 MiB: 92.56 ms  +178.6%    - 29.0GB/s
```

### EnergyEfficient

* Cache:
	- L1D: 4x 64 KB
	- L2:  4x 1MB,  4 MB
* Memory: 16 GB, LPDDR4X-4266 DC 16bit, 2133 MHz, 128bit Bus, 68.25 GB/s

```
memset, single thread:
  256 B  : 0.15 s             - 17.4GB/s
  512 B  : 0.17 s  +11.3%     - 15.7GB/s
  1 KiB  : 0.16 s  +5.2%      - 16.6GB/s
  2 KiB  : 0.17 s  +9.4%      - 15.9GB/s
  4 KiB  : 0.17 s  +7.5%      - 16.2GB/s
  8 KiB  : 0.17 s  +9.6%      - 15.9GB/s
  16 KiB : 0.17 s  +11.0%     - 15.7GB/s  - page size
  32 KiB : 0.16 s  +1.9%      - 17.1GB/s
  64 KiB : 0.16 s  +1.9%      - 17.1GB/s  - L1
  128 KiB: 0.16 s  +3.0%      - 16.9GB/s
  256 KiB: 0.16 s  +1.9%      - 17.1GB/s
  512 KiB: 0.16 s  +4.2%      - 16.7GB/s
  1 MiB  : 0.16 s  +2.2%      - 17.0GB/s  - L2 bank
  2 MiB  : 0.16 s  +1.3%      - 17.2GB/s
  4 MiB  : 0.16 s  +2.6%      - 17.0GB/s  - L2
  8 MiB  : 0.16 s  +5.0%      - 16.6GB/s
  16 MiB : 0.16 s  +5.4%      - 16.5GB/s
  32 MiB : 0.16 s  +6.2%      - 16.4GB/s
  64 MiB : 0.17 s  +8.4%      - 16.1GB/s
  128 MiB: 0.17 s  +7.0%      - 16.3GB/s

SIMD cached fill, single thread:
  256 B  : 0.17 s  +225.0%    - 15.9GB/s
  512 B  : 0.17 s  +219.6%    - 16.2GB/s
  1 KiB  : 0.17 s  +229.8%    - 15.7GB/s
  2 KiB  : 0.17 s  +223.0%    - 16.0GB/s
  4 KiB  : 0.17 s  +234.4%    - 15.5GB/s
  8 KiB  : 0.17 s  +229.8%    - 15.7GB/s
  16 KiB : 0.17 s  +225.9%    - 15.9GB/s  - page size
  32 KiB : 55.41 ms  +6.9%    - 48.4GB/s <<
  64 KiB : 54.09 ms  +4.4%    - 49.6GB/s << - L1
  128 KiB: 53.51 ms  +3.2%    - 50.2GB/s <<
  256 KiB: 54.57 ms  +5.3%    - 49.2GB/s <<
  512 KiB: 51.83 ms           - 51.8GB/s <<
  1 MiB  : 52.86 ms  +2.0%    - 50.8GB/s << - L2 bank
  2 MiB  : 53.11 ms  +2.5%    - 50.5GB/s <<
  4 MiB  : 52.90 ms  +2.1%    - 50.7GB/s << - L2
  8 MiB  : 52.87 ms  +2.0%    - 50.8GB/s <<
  16 MiB : 54.40 ms  +5.0%    - 49.3GB/s <<
  32 MiB : 57.51 ms  +11.0%   - 46.7GB/s << faster than memset ????
  64 MiB : 57.24 ms  +10.4%   - 46.9GB/s <<
  128 MiB: 55.51 ms  +7.1%    - 48.4GB/s <<

SIMD cached fill, single thread:
  256 B  : 0.16 s             - 16.6GB/s
  512 B  : 0.17 s  +5.9%      - 15.7GB/s
  1 KiB  : 0.17 s  +3.2%      - 16.1GB/s
  2 KiB  : 0.17 s  +3.1%      - 16.1GB/s
  4 KiB  : 0.17 s  +4.2%      - 16.0GB/s
  8 KiB  : 0.17 s  +3.6%      - 16.1GB/s
  16 KiB : 0.16 s  +1.9%      - 16.3GB/s
  32 KiB : 0.16 s  +2.1%      - 16.3GB/s
  64 KiB : 0.17 s  +5.4%      - 15.8GB/s
  128 KiB: 0.19 s  +19.8%     - 13.9GB/s
  256 KiB: 0.19 s  +20.1%     - 13.9GB/s
  512 KiB: 0.20 s  +22.1%     - 13.6GB/s
  1 MiB  : 0.19 s  +20.9%     - 13.8GB/s
  4 MiB  : 0.20 s  +21.0%     - 13.8GB/s
  2 MiB  : 0.20 s  +21.6%     - 13.7GB/s
  8 MiB  : 0.21 s  +29.1%     - 12.9GB/s
  64 MiB : 0.22 s  +37.2%     - 12.1GB/s
  16 MiB : 0.22 s  +37.9%     - 12.1GB/s
  32 MiB : 0.22 s  +38.7%     - 12.0GB/s
  128 MiB: 0.22 s  +36.9%     - 12.2GB/s

SIMD non-cached fill, single thread:
  16 KiB : 0.17 s  +0.3%      - 16.2GB/s
  32 KiB : 0.17 s             - 16.2GB/s
  512 KiB: 0.17 s  +0.5%      - 16.1GB/s
  128 KiB: 0.17 s  +0.6%      - 16.1GB/s
  64 KiB : 0.17 s  +0.7%      - 16.1GB/s
  8 KiB  : 0.17 s  +1.0%      - 16.1GB/s
  256 KiB: 0.17 s  +1.1%      - 16.1GB/s
  4 MiB  : 0.17 s  +1.5%      - 16.0GB/s
  2 MiB  : 0.17 s  +2.2%      - 15.9GB/s
  1 MiB  : 0.17 s  +2.3%      - 15.9GB/s
  16 MiB : 0.17 s  +2.3%      - 15.9GB/s
  4 KiB  : 0.17 s  +2.4%      - 15.8GB/s
  8 MiB  : 0.17 s  +2.7%      - 15.8GB/s
  64 MiB : 0.17 s  +3.5%      - 15.7GB/s
  32 MiB : 0.18 s  +7.1%      - 15.1GB/s
  128 MiB: 0.18 s  +7.8%      - 15.0GB/s
  2 KiB  : 0.19 s  +14.7%     - 14.1GB/s
  1 KiB  : 0.51 s  +205.6%    - 5.31GB/s
  512 B  : 1.01 s  +512.0%    - 2.65GB/s
  256 B  : 1.98 s  +1099.2%   - 1.35GB/s

memcpy, single thread:
  256 B  : 0.23 s  +32.5%     - 11.7GB/s
  512 B  : 0.20 s  +13.3%     - 13.7GB/s
  1 KiB  : 0.18 s  +5.6%      - 14.7GB/s
  2 KiB  : 0.18 s  +3.8%      - 15.0GB/s
  4 KiB  : 0.18 s  +4.5%      - 14.9GB/s
  8 KiB  : 0.18 s  +2.0%      - 15.3GB/s
  16 KiB : 0.17 s             - 15.6GB/s  - page size
  32 KiB : 0.18 s  +3.4%      - 15.0GB/s
  64 KiB : 0.23 s  +34.8%     - 11.5GB/s  - L1
  128 KiB: 0.22 s  +27.4%     - 12.2GB/s
  256 KiB: 0.22 s  +29.0%     - 12.1GB/s
  512 KiB: 0.24 s  +40.4%     - 11.1GB/s
  1 MiB  : 0.22 s  +28.7%     - 12.1GB/s  - L2 bank
  2 MiB  : 0.23 s  +33.4%     - 11.7GB/s
  4 MiB  : 0.21 s  +24.6%     - 12.5GB/s  - L2
  8 MiB  : 0.23 s  +31.2%     - 11.9GB/s
  16 MiB : 0.23 s  +32.8%     - 11.7GB/s
  32 MiB : 0.23 s  +32.6%     - 11.7GB/s
  64 MiB : 0.23 s  +32.7%     - 11.7GB/s
  128 MiB: 0.23 s  +33.2%     - 11.7GB/s

SIMD cached copy, single thread:
  256 B  : 0.19 s  +11.6%     - 14.2GB/s
  512 B  : 0.18 s  +6.3%      - 14.9GB/s
  1 KiB  : 0.17 s  +3.1%      - 15.4GB/s
  2 KiB  : 0.17 s  +2.0%      - 15.6GB/s
  4 KiB  : 0.18 s  +8.1%      - 14.7GB/s
  8 KiB  : 0.18 s  +4.9%      - 15.1GB/s
  16 KiB : 0.17 s             - 15.9GB/s  - page size
  32 KiB : 0.20 s  +15.9%     - 13.7GB/s
  64 KiB : 0.31 s  +84.9%     - 8.59GB/s  - L1
  128 KiB: 0.31 s  +84.3%     - 8.62GB/s
  256 KiB: 0.31 s  +81.3%     - 8.76GB/s
  512 KiB: 0.31 s  +86.3%     - 8.53GB/s
  1 MiB  : 0.31 s  +85.7%     - 8.55GB/s  - L2 bank
  2 MiB  : 0.26 s  +51.6%     - 10.5GB/s
  4 MiB  : 0.30 s  +79.3%     - 8.86GB/s  - L2
  8 MiB  : 0.32 s  +88.5%     - 8.43GB/s
  16 MiB : 0.32 s  +87.2%     - 8.49GB/s
  32 MiB : 0.32 s  +89.2%     - 8.40GB/s
  64 MiB : 0.32 s  +88.3%     - 8.44GB/s
  128 MiB: 0.32 s  +88.4%     - 8.43GB/s

SIMD non-cached copy, single thread:
  256 B  : 0.27 s  +25.6%     - 9.96GB/s
  512 B  : 0.23 s  +6.2%      - 11.8GB/s
  1 KiB  : 0.22 s  +3.1%      - 12.1GB/s
  2 KiB  : 0.21 s             - 12.5GB/s
  4 KiB  : 0.22 s  +3.4%      - 12.1GB/s
  8 KiB  : 0.22 s  +2.8%      - 12.2GB/s
  16 KiB : 0.22 s  +1.2%      - 12.4GB/s
  32 KiB : 0.22 s  +1.7%      - 12.3GB/s
  64 KiB : 0.23 s  +4.9%      - 11.9GB/s
  128 KiB: 0.23 s  +7.7%      - 11.6GB/s
  256 KiB: 0.23 s  +9.2%      - 11.5GB/s
  512 KiB: 0.23 s  +6.5%      - 11.7GB/s
  1 MiB  : 0.23 s  +6.0%      - 11.8GB/s
  2 MiB  : 0.25 s  +14.8%     - 10.9GB/s
  4 MiB  : 0.25 s  +15.9%     - 10.8GB/s
  8 MiB  : 0.25 s  +15.3%     - 10.8GB/s
  16 MiB : 0.25 s  +16.6%     - 10.7GB/s
  32 MiB : 0.25 s  +15.5%     - 10.8GB/s
  64 MiB : 0.25 s  +16.5%     - 10.7GB/s
  128 MiB: 0.25 s  +15.3%     - 10.9GB/s
```

### Performance, Multithreading

* Cache:
	- L1D: 4x 128 KB
	- L2:  4x 3MB,  12 MB
* Memory: 16 GB, LPDDR4X-4266 DC 16bit, 2133 MHz, 128bit Bus, 68.25 GB/s

```
memcpy, multithreading:
  256 B  : 75.89 ms  +36.7%   - 141.5GB/s
  512 B  : 64.88 ms  +16.8%   - 165.5GB/s
  1 KiB  : 65.81 ms  +18.5%   - 163.2GB/s
  2 KiB  : 64.36 ms  +15.9%   - 166.8GB/s
  4 KiB  : 55.53 ms           - 193.3GB/s
  8 KiB  : 62.92 ms  +13.3%   - 170.7GB/s
  16 KiB : 63.29 ms  +14.0%   - 169.6GB/s  - page size
  32 KiB : 61.46 ms  +10.7%   - 174.7GB/s
  64 KiB : 66.63 ms  +20.0%   - 161.2GB/s
  128 KiB: 0.12 s  +114.7%    - 90.0GB/s   - L1
  256 KiB: 99.58 ms  +79.3%   - 107.8GB/s
  512 KiB: 0.31 s  +458.2%    - 34.6GB/s
  1 MiB  : 0.31 s  +449.2%    - 35.2GB/s
  2 MiB  : 0.36 s  +547.7%    - 29.9GB/s
  4 MiB  : 0.38 s  +580.2%    - 28.4GB/s   - L2 bank 3MB
  8 MiB  : 0.38 s  +575.4%    - 28.6GB/s
  16 MiB : 0.38 s  +575.7%    - 28.6GB/s   - L2 12MB
  32 MiB : 0.38 s  +579.5%    - 28.5GB/s
  64 MiB : 0.38 s  +575.8%    - 28.6GB/s
  128 MiB: 0.38 s  +576.5%    - 28.6GB/s

SIMD cached copy, multithreading:
  256 B  : 69.09 ms  +10.4%   - 155.4GB/s  <<
  512 B  : 66.41 ms  +6.1%    - 161.7GB/s
  1 KiB  : 62.56 ms           - 171.6GB/s
  2 KiB  : 63.39 ms  +1.3%    - 169.4GB/s
  4 KiB  : 64.94 ms  +3.8%    - 165.4GB/s
  8 KiB  : 64.92 ms  +3.8%    - 165.4GB/s
  16 KiB : 65.04 ms  +4.0%    - 165.1GB/s  - page size
  32 KiB : 67.97 ms  +8.6%    - 158.0GB/s
  64 KiB : 68.19 ms  +9.0%    - 157.5GB/s
  128 KiB: 0.10 s  +60.0%     - 107.3GB/s  - L1
  256 KiB: 89.41 ms  +42.9%   - 120.1GB/s  <<
  512 KiB: 92.94 ms  +48.5%   - 115.5GB/s  << faster than memcpy
  1 MiB  : 89.61 ms  +43.2%   - 119.8GB/s  <<
  2 MiB  : 0.17 s  +177.1%    - 61.9GB/s   <<
  4 MiB  : 0.37 s  +496.4%    - 28.8GB/s   - L2 bank 3MB
  8 MiB  : 0.37 s  +499.2%    - 28.6GB/s
  16 MiB : 0.38 s  +499.4%    - 28.6GB/s   - L2 12MB
  32 MiB : 0.37 s  +497.7%    - 28.7GB/s
  64 MiB : 0.37 s  +498.8%    - 28.7GB/s
  128 MiB: 0.38 s  +499.9%    - 28.6GB/s

SIMD cached copy, multithreading:
  256 B  : 64.70 ms  +4.1%      - 166.0GB/s
  512 B  : 63.41 ms  +2.0%      - 169.3GB/s
  1 KiB  : 63.50 ms  +2.2%      - 169.1GB/s
  2 KiB  : 67.73 ms  +9.0%      - 158.5GB/s
  4 KiB  : 64.43 ms  +3.7%      - 166.6GB/s
  8 KiB  : 62.16 ms             - 172.7GB/s
  16 KiB : 63.89 ms  +2.8%      - 168.1GB/s
  32 KiB : 66.74 ms  +7.4%      - 160.9GB/s
  64 KiB : 62.85 ms  +1.1%      - 170.9GB/s
  128 KiB: 97.10 ms  +56.2%     - 110.6GB/s
  256 KiB: 88.24 ms  +41.9%     - 121.7GB/s
  512 KiB: 82.74 ms  +33.1%     - 129.8GB/s
  1 MiB  : 90.91 ms  +46.3%     - 118.1GB/s
  2 MiB  : 0.17 s    +181.2%    - 61.4GB/s
  4 MiB  : 0.37 s    +500.8%    - 28.8GB/s
  8 MiB  : 0.38 s    +506.2%    - 28.5GB/s
  16 MiB : 0.38 s    +503.3%    - 28.6GB/s
  32 MiB : 0.38 s    +504.9%    - 28.6GB/s
  64 MiB : 0.38 s    +503.6%    - 28.6GB/s
  128 MiB: 0.38 s    +505.4%    - 28.5GB/s

SIMD non-cached copy, multithreading:
  256 B  : 0.13 s    +117.6%    - 82.4GB/s
  512 B  : 65.70 ms  +9.7%      - 163.4GB/s
  1 KiB  : 60.19 ms  +0.5%      - 178.4GB/s
  2 KiB  : 64.99 ms  +8.5%      - 165.2GB/s
  4 KiB  : 60.87 ms  +1.6%      - 176.4GB/s
  8 KiB  : 66.09 ms  +10.4%     - 162.5GB/s
  16 KiB : 59.89 ms             - 179.3GB/s
  32 KiB : 62.92 ms  +5.1%      - 170.7GB/s
  64 KiB : 63.43 ms  +5.9%      - 169.3GB/s
  128 KiB: 94.84 ms  +58.4%     - 113.2GB/s
  256 KiB: 93.77 ms  +56.6%     - 114.5GB/s
  512 KiB: 86.87 ms  +45.0%     - 123.6GB/s
  1 MiB  : 85.90 ms  +43.4%     - 125.0GB/s
  2 MiB  : 0.17 s    +179.9%    - 64.1GB/s
  4 MiB  : 0.37 s    +521.6%    - 28.8GB/s
  8 MiB  : 0.37 s    +525.2%    - 28.7GB/s
  16 MiB : 0.38 s    +526.3%    - 28.6GB/s
  32 MiB : 0.38 s    +526.3%    - 28.6GB/s
  64 MiB : 0.37 s    +523.7%    - 28.7GB/s
  128 MiB: 0.38 s    +526.9%    - 28.6GB/s
```

