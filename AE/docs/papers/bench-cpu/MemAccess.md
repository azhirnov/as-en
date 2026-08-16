
Variable block size test: [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_CacheSize.cpp)<br/>
Different memcpy implementations: [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_MemCopy.cpp)

* [AMD Ryzen 3900X](#AMD-Ryzen-3900X)
* [AMD Phenom II X4 945](#AMD-Phenom-II-X4-945)
* [AMD Ryzen 7 8745HS](#AMD-Ryzen-7-8745HS)
* [Intel i5 8250U](#Intel-i5-8250U)
* [Intel N150](#Intel-N150)
* [Intel Ultra 7 255H](#Intel-Ultra-7-255H)
* [Apple Mac M1](#Apple-Mac-M1)
* [MediaTek Helio G96](#MediaTek-Helio-G96)
* [MediaTek Dimensity 7020](#MediaTek-Dimensity-7020)
* [MediaTek Dimensity 8200](#MediaTek-Dimensity-8200)
* [Snapdragon 888](#Snapdragon-888)
* [Snapdragon 439](#Snapdragon-439)
* [Samsung Exynos 7870](#Samsung-Exynos-7870)
* [Pico 4 Ultra](#Pico-4-Ultra)
* [Snapdragon 8 Elite gen 5](#Snapdragon-8-Elite-gen-5)


# AMD Ryzen 3900X

* Arch: Zen2
* Cache:
    - L1: 12x 32KB 8-way
    - L2: 12x 512KB 8-way
    - L3: 64MB: 4x CCX with 16MB 16-way, 4 slices of 4MB per CCX
    - write to L1: 32B/cy
    - read L1: 64B/cy
    - L2 to L1: 32B/cy
    - L3 to L2: 32B/cy
* Memory: DDR4, 3GHz, 24GB/s *(must be 48GB/s in dual channel mode)*
* Theoretical performance:<br/>
  32B/cy write to L1 * 4.2GHz = 134.4 GB/s - max speed of memset<br/>
  64B/cy read from L1 * 4.2GHz = 268.8 GB/s - max speed of sum/hash<br/>

**Results**

| | range | single thread bandwidth (GB/s) | multithreading, per core bandwidth (GB/s) | comment |
|---|---|---|---|---|
| cached copy     | <= 16 KiB      | 130 | 95 - 115 |
| cached copy     | 32 KiB - 2 MiB |     | 45 - 65  |
| non-cached copy | => 64 KiB      | 19  | 19.5     |
| non-cached copy | => 8 KiB       | -   | 19.5     |
| cached fill     | <= 512 KiB     | 130 |

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|------------|--------------|
| 256 B   | 39.18 ms | +100% | -     | 68.5GB/s   |              |
| 512 B   | 23.36 ms | +19%  | +40%  | 114.9GB/s  |              |
| 1 KiB   | 20.22 ms | +3%   | +13%  | 132.7GB/s  |              |
| 2 KiB   | 19.89 ms | +1%   | +2%   | 134.9GB/s  |              |
| 4 KiB   | 19.67 ms | -     | +1%   | 136.4GB/s  |  page, 4 KiB |
| 8 KiB   | 19.60 ms | -     | -     | 137.0GB/s  |              |
| 16 KiB  | 20.37 ms | +4%   | +4%   | 131.8GB/s  |              |
| 32 KiB  | 38.84 ms | +98%  | +91%  | 69.1GB/s   |  L1D, 32 KiB |
| 64 KiB  | 38.78 ms | +98%  | -     | 69.2GB/s   |              |
| 128 KiB | 38.68 ms | +97%  | -     | 69.4GB/s   |              |
| 256 KiB | 41.84 ms | +113% | +8%   | 64.2GB/s   |              |
| 512 KiB | 52.01 ms | +165% | +24%  | 51.6GB/s   |  L2, 512 KiB |
| 1 MiB   | 53.85 ms | +175% | +4%   | 49.8GB/s   |              |
| 2 MiB   | 0.14 s   | +635% | +167% | 18.6GB/s   |              |
| 4 MiB   | 0.14 s   | +627% | +1%   | 18.8GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 0.14 s   | +628% | -     | 18.8GB/s   |              |
| 16 MiB  | 0.14 s   | +628% | -     | 18.8GB/s   |  L3, 16 MiB  |
| 32 MiB  | 0.14 s   | +638% | +1%   | 18.6GB/s   |              |
| 64 MiB  | 0.14 s   | +629% | +1%   | 18.8GB/s   |              |
| 128 MiB | 0.14 s   | +629% | -     | 18.8GB/s   |              |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|-------|------------|--------------|
| 256 B   | 22.13 ms | +13%   | -     | 121.3GB/s  |              |
| 512 B   | 19.67 ms | -      | +11%  | 136.4GB/s  |              |
| 1 KiB   | 19.70 ms | -      | -     | 136.3GB/s  |              |
| 2 KiB   | 19.71 ms | -      | -     | 136.2GB/s  |              |
| 4 KiB   | 19.62 ms | -      | -     | 136.8GB/s  |  page, 4 KiB |
| 8 KiB   | 19.65 ms | -      | -     | 136.6GB/s  |              |
| 16 KiB  | 20.85 ms | +6%    | +6%   | 128.7GB/s  |              |
| 32 KiB  | 38.93 ms | +98%   | +87%  | 69.0GB/s   |  L1D, 32 KiB |
| 64 KiB  | 38.83 ms | +98%   | -     | 69.1GB/s   |              |
| 128 KiB | 38.80 ms | +98%   | -     | 69.2GB/s   |              |
| 256 KiB | 42.19 ms | +115%  | +9%   | 63.6GB/s   |              |
| 512 KiB | 51.79 ms | +164%  | +23%  | 51.8GB/s   |  L2, 512 KiB |
| 1 MiB   | 53.74 ms | +174%  | +4%   | 49.9GB/s   |              |
| 2 MiB   | 54.09 ms | +176%  | +1%   | 49.6GB/s   |              |
| 4 MiB   | 54.32 ms | +177%  | -     | 49.4GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 90.27 ms | +360%  | +66%  | 29.7GB/s   |              |
| 16 MiB  | 0.25 s   | +1174% | +177% | 10.7GB/s   |  L3, 16 MiB  |
| 32 MiB  | 0.25 s   | +1192% | +1%   | 10.6GB/s   |              |
| 64 MiB  | 0.25 s   | +1190% | -     | 10.6GB/s   |              |
| 128 MiB | 0.25 s   | +1191% | -     | 10.6GB/s   |              |

**SIMD non-cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|--------|------|-----------|--------------|
| 256 B   | 2.39 s | +1629% | -    | 1.12GB/s  |              |
| 512 B   | 1.19 s | +763%  | +50% | 2.25GB/s  |              |
| 1 KiB   | 0.52 s | +274%  | +57% | 5.21GB/s  |              |
| 2 KiB   | 0.30 s | +121%  | +41% | 8.82GB/s  |              |
| 4 KiB   | 0.23 s | +64%   | +26% | 11.9GB/s  |  page, 4 KiB |
| 8 KiB   | 0.21 s | +50%   | +8%  | 12.9GB/s  |              |
| 16 KiB  | 0.19 s | +35%   | +10% | 14.4GB/s  |              |
| 32 KiB  | 0.15 s | +10%   | +19% | 17.7GB/s  |  L1D, 32 KiB |
| 64 KiB  | 0.15 s | +8%    | +1%  | 18.0GB/s  |              |
| 128 KiB | 0.14 s | +5%    | +3%  | 18.6GB/s  |              |
| 256 KiB | 0.14 s | +1%    | +4%  | 19.3GB/s  |              |
| 512 KiB | 0.14 s | -      | +1%  | 19.5GB/s  |  L2, 512 KiB |
| 1 MiB   | 0.15 s | +7%    | +7%  | 18.1GB/s  |              |
| 2 MiB   | 0.15 s | +5%    | +2%  | 18.5GB/s  |              |
| 4 MiB   | 0.14 s | +4%    | +1%  | 18.7GB/s  | L3 slice, 4 MiB |
| 8 MiB   | 0.14 s | +4%    | -    | 18.7GB/s  |              |
| 16 MiB  | 0.14 s | +4%    | -    | 18.7GB/s  |  L3, 16 MiB  |
| 32 MiB  | 0.14 s | +4%    | +1%  | 18.8GB/s  |              |
| 64 MiB  | 0.14 s | +3%    | -    | 18.9GB/s  |              |
| 128 MiB | 0.14 s | +3%    | -    | 18.9GB/s  |              |

</details>
<details><summary><b>memcpy, multithreading (4 threads bound to physical cores)</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|-------|------------|--------------|
| 256 B   | 41.05 ms | +103%  | -     | 261.5GB/s  |              |
| 512 B   | 28.84 ms | +42%   | +30%  | 372.4GB/s  |              |
| 1 KiB   | 22.87 ms | +13%   | +21%  | 469.4GB/s  |              |
| 2 KiB   | 26.35 ms | +30%   | +15%  | 407.5GB/s  |              |
| 4 KiB   | 20.26 ms | -      | +23%  | 529.9GB/s  |  page, 4 KiB |
| 8 KiB   | 26.20 ms | +29%   | +29%  | 409.8GB/s  |              |
| 16 KiB  | 25.32 ms | +25%   | +3%   | 424.0GB/s  |              |
| 32 KiB  | 40.11 ms | +98%   | +58%  | 267.7GB/s  |  L1D, 32 KiB |
| 64 KiB  | 40.06 ms | +98%   | -     | 268.0GB/s  |              |
| 128 KiB | 39.94 ms | +97%   | -     | 268.8GB/s  |              |
| 256 KiB | 48.08 ms | +137%  | +20%  | 223.3GB/s  |              |
| 512 KiB | 55.52 ms | +174%  | +15%  | 193.4GB/s  |  L2, 512 KiB |
| 1 MiB   | 59.16 ms | +192%  | +7%   | 181.5GB/s  |              |
| 2 MiB   | 0.55 s   | +2639% | +838% | 19.3GB/s   |              |
| 4 MiB   | 0.55 s   | +2607% | +1%   | 19.6GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 0.55 s   | +2602% | -     | 19.6GB/s   |              |
| 16 MiB  | 0.55 s   | +2605% | -     | 19.6GB/s   |  L3, 16 MiB  |
| 32 MiB  | 0.55 s   | +2612% | -     | 19.5GB/s   |              |
| 64 MiB  | 0.55 s   | +2616% | -     | 19.5GB/s   |              |
| 128 MiB | 0.55 s   | +2635% | +1%   | 19.4GB/s   |              |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|-------|------------|--------------|
| 256 B   | 29.14 ms | +24%   | -     | 368.5GB/s  |              |
| 512 B   | 27.99 ms | +19%   | +4%   | 383.7GB/s  |              |
| 1 KiB   | 27.02 ms | +15%   | +3%   | 397.4GB/s  |              |
| 2 KiB   | 25.23 ms | +8%    | +7%   | 425.6GB/s  |              |
| 4 KiB   | 26.39 ms | +13%   | +5%   | 406.8GB/s  |  page, 4 KiB |
| 8 KiB   | 24.34 ms | +4%    | +8%   | 441.1GB/s  |              |
| 16 KiB  | 23.42 ms | -      | +4%   | 458.4GB/s  |              |
| 32 KiB  | 43.45 ms | +85%   | +85%  | 247.1GB/s  |  L1D, 32 KiB |
| 64 KiB  | 40.15 ms | +71%   | +8%   | 267.5GB/s  |              |
| 128 KiB | 40.31 ms | +72%   | -     | 266.4GB/s  |              |
| 256 KiB | 45.60 ms | +95%   | +13%  | 235.5GB/s  |              |
| 512 KiB | 55.69 ms | +138%  | +22%  | 192.8GB/s  |  L2, 512 KiB |
| 1 MiB   | 58.26 ms | +149%  | +5%   | 184.3GB/s  |              |
| 2 MiB   | 58.32 ms | +149%  | -     | 184.1GB/s  |              |
| 4 MiB   | 0.60 s   | +2481% | +937% | 17.8GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 0.72 s   | +2953% | +18%  | 15.0GB/s   |              |
| 16 MiB  | 0.89 s   | +3698% | +24%  | 12.1GB/s   |  L3, 16 MiB  |
| 32 MiB  | 0.89 s   | +3697% | -     | 12.1GB/s   |              |
| 64 MiB  | 0.89 s   | +3690% | -     | 12.1GB/s   |              |
| 128 MiB | 0.89 s   | +3692% | -     | 12.1GB/s   |              |

**SIMD non-cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|-------|------|-----------|--------------|
| 256 B   | 2.81 s | +419% | -    | 3.82GB/s  |              |
| 512 B   | 1.24 s | +129% | +56% | 8.66GB/s  |              |
| 1 KiB   | 0.76 s | +40%  | +39% | 14.2GB/s  |              |
| 2 KiB   | 0.59 s | +9%   | +22% | 18.2GB/s  |              |
| 4 KiB   | 0.55 s | +2%   | +6%  | 19.4GB/s  |  page, 4 KiB |
| 8 KiB   | 0.54 s | -     | +2%  | 19.8GB/s  |              |
| 16 KiB  | 0.55 s | +1%   | +1%  | 19.7GB/s  |              |
| 32 KiB  | 0.56 s | +2%   | +2%  | 19.3GB/s  |  L1D, 32 KiB |
| 64 KiB  | 0.58 s | +7%   | +4%  | 18.5GB/s  |              |
| 128 KiB | 0.56 s | +3%   | +4%  | 19.3GB/s  |              |
| 256 KiB | 0.59 s | +8%   | +6%  | 18.3GB/s  |              |
| 512 KiB | 0.57 s | +6%   | +2%  | 18.7GB/s  |  L2, 512 KiB |
| 1 MiB   | 0.56 s | +3%   | +3%  | 19.3GB/s  |              |
| 2 MiB   | 0.54 s | -     | +3%  | 19.8GB/s  |              |
| 4 MiB   | 0.54 s | -     | -    | 19.7GB/s  | L3 slice, 4 MiB |
| 8 MiB   | 0.54 s | +1%   | -    | 19.7GB/s  |              |
| 16 MiB  | 0.55 s | +1%   | -    | 19.7GB/s  |  L3, 16 MiB  |
| 32 MiB  | 0.54 s | -     | -    | 19.7GB/s  |              |
| 64 MiB  | 0.54 s | -     | -    | 19.8GB/s  |              |
| 128 MiB | 0.54 s | -     | -    | 19.8GB/s  |              |

</details>
<details><summary><b>memset, single thread</b></summary>

**memset**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|------------|--------------|
| 256 B   | 32.38 ms | +65%  | -     | 82.9GB/s   |              |
| 512 B   | 21.17 ms | +8%   | +35%  | 126.8GB/s  |              |
| 1 KiB   | 20.54 ms | +4%   | +3%   | 130.7GB/s  |              |
| 2 KiB   | 19.93 ms | +1%   | +3%   | 134.7GB/s  |              |
| 4 KiB   | 19.66 ms | -     | +1%   | 136.5GB/s  |  page, 4 KiB |
| 8 KiB   | 19.66 ms | -     | -     | 136.5GB/s  |              |
| 16 KiB  | 19.89 ms | +1%   | +1%   | 135.0GB/s  |              |
| 32 KiB  | 20.32 ms | +3%   | +2%   | 132.1GB/s  |  L1D, 32 KiB |
| 64 KiB  | 20.36 ms | +4%   | -     | 131.8GB/s  |              |
| 128 KiB | 20.11 ms | +2%   | +1%   | 133.5GB/s  |              |
| 256 KiB | 20.01 ms | +2%   | -     | 134.1GB/s  |              |
| 512 KiB | 22.09 ms | +12%  | +10%  | 121.5GB/s  |  L2, 512 KiB |
| 1 MiB   | 27.75 ms | +41%  | +26%  | 96.7GB/s   |              |
| 2 MiB   | 27.87 ms | +42%  | -     | 96.3GB/s   |              |
| 4 MiB   | 28.05 ms | +43%  | +1%   | 95.7GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 28.09 ms | +43%  | -     | 95.6GB/s   |              |
| 16 MiB  | 59.35 ms | +202% | +111% | 45.2GB/s   |  L3, 16 MiB  |
| 32 MiB  | 0.17 s   | +757% | +184% | 15.9GB/s   |              |
| 64 MiB  | 0.11 s   | +477% | +33%  | 23.7GB/s   |              |
| 128 MiB | 0.11 s   | +479% | -     | 23.6GB/s   |              |

**SIMD cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|------------|--------------|
| 256 B   | 19.45 ms | +1%   | -     | 138.0GB/s  |              |
| 512 B   | 19.56 ms | +1%   | +1%   | 137.2GB/s  |              |
| 1 KiB   | 19.40 ms | -     | +1%   | 138.4GB/s  |              |
| 2 KiB   | 19.34 ms | -     | -     | 138.8GB/s  |              |
| 4 KiB   | 19.33 ms | -     | -     | 138.8GB/s  |  page, 4 KiB |
| 8 KiB   | 19.48 ms | +1%   | +1%   | 137.8GB/s  |              |
| 16 KiB  | 19.51 ms | +1%   | -     | 137.6GB/s  |              |
| 32 KiB  | 19.68 ms | +2%   | +1%   | 136.4GB/s  |  L1D, 32 KiB |
| 64 KiB  | 20.19 ms | +4%   | +3%   | 132.9GB/s  |              |
| 128 KiB | 20.25 ms | +5%   | -     | 132.5GB/s  |              |
| 256 KiB | 20.27 ms | +5%   | -     | 132.4GB/s  |              |
| 512 KiB | 22.13 ms | +14%  | +9%   | 121.3GB/s  |  L2, 512 KiB |
| 1 MiB   | 27.47 ms | +42%  | +24%  | 97.7GB/s   |              |
| 2 MiB   | 27.84 ms | +44%  | +1%   | 96.4GB/s   |              |
| 4 MiB   | 28.07 ms | +45%  | +1%   | 95.6GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 27.99 ms | +45%  | -     | 95.9GB/s   |              |
| 16 MiB  | 59.52 ms | +208% | +113% | 45.1GB/s   |  L3, 16 MiB  |
| 32 MiB  | 0.17 s   | +769% | +182% | 16.0GB/s   |              |
| 64 MiB  | 0.17 s   | +780% | +1%   | 15.8GB/s   |              |
| 128 MiB | 0.17 s   | +780% | -     | 15.8GB/s   |              |

**SIMD non-cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|-------|------|-----------|--------------|
| 256 B   | 1.06 s | +839% | -    | 2.52GB/s  |              |
| 512 B   | 0.59 s | +417% | +45% | 4.58GB/s  |              |
| 1 KiB   | 0.35 s | +208% | +40% | 7.68GB/s  |              |
| 2 KiB   | 0.23 s | +104% | +34% | 11.6GB/s  |              |
| 4 KiB   | 0.17 s | +52%  | +25% | 15.6GB/s  |  page, 4 KiB |
| 8 KiB   | 0.14 s | +26%  | +17% | 18.8GB/s  |              |
| 16 KiB  | 0.13 s | +13%  | +10% | 20.9GB/s  |              |
| 32 KiB  | 0.12 s | +7%   | +6%  | 22.2GB/s  |  L1D, 32 KiB |
| 64 KiB  | 0.12 s | +3%   | +3%  | 22.9GB/s  |              |
| 128 KiB | 0.12 s | +2%   | +2%  | 23.3GB/s  |              |
| 256 KiB | 0.11 s | +1%   | +1%  | 23.4GB/s  |              |
| 512 KiB | 0.11 s | +1%   | -    | 23.5GB/s  |  L2, 512 KiB |
| 1 MiB   | 0.11 s | -     | -    | 23.6GB/s  |              |
| 2 MiB   | 0.11 s | -     | -    | 23.6GB/s  |              |
| 4 MiB   | 0.11 s | -     | -    | 23.7GB/s  | L3 slice, 4 MiB |
| 8 MiB   | 0.11 s | -     | -    | 23.7GB/s  |              |
| 16 MiB  | 0.11 s | -     | -    | 23.7GB/s  |  L3, 16 MiB  |
| 32 MiB  | 0.11 s | -     | -    | 23.7GB/s  |              |
| 64 MiB  | 0.11 s | -     | -    | 23.7GB/s  |              |
| 128 MiB | 0.11 s | -     | -    | 23.7GB/s  |              |

</details>
<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|-------|------------|-------------|
| 256 B   | 30.04 ms | +182%  | -     | 89.4GB/s   |
| 512 B   | 15.21 ms | +43%   | +49%  | 176.5GB/s  |
| 1 KiB   | 12.81 ms | +20%   | +16%  | 209.6GB/s  |
| 2 KiB   | 11.22 ms | +5%    | +12%  | 239.2GB/s  |
| 4 KiB   | 10.80 ms | +1%    | +4%   | 248.5GB/s  | page, 4 KiB |
| 8 KiB   | 10.65 ms | -      | +1%   | 252.2GB/s  |
| 16 KiB  | 11.49 ms | +8%    | +8%   | 233.6GB/s  |
| 32 KiB  | 26.11 ms | +145%  | +127% | 102.8GB/s  | L1D, 32 KiB |
| 64 KiB  | 30.03 ms | +182%  | +15%  | 89.4GB/s   |
| 128 KiB | 30.03 ms | +182%  | -     | 89.4GB/s   |
| 256 KiB | 31.32 ms | +194%  | +4%   | 85.7GB/s   |
| 512 KiB | 36.88 ms | +246%  | +18%  | 72.8GB/s   | L2, 512 KiB |
| 1 MiB   | 37.91 ms | +256%  | +3%   | 70.8GB/s   |
| 2 MiB   | 38.01 ms | +257%  | -     | 70.6GB/s   |
| 4 MiB   | 38.17 ms | +259%  | -     | 70.3GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 51.76 ms | +386%  | +36%  | 51.9GB/s   |
| 16 MiB  | 0.16 s   | +1449% | +219% | 16.3GB/s   | L3, 16 MiB  |
| 32 MiB  | 0.18 s   | +1575% | +8%   | 15.1GB/s   |
| 64 MiB  | 0.18 s   | +1567% | -     | 15.1GB/s   |
| 128 MiB | 0.18 s   | +1558% | +1%   | 15.2GB/s   |

**read from cache with loop dependency (SIMD xor)**

| block | diff | bandwidth | comment |
|---------|-------|------------|-------------|
| 256 B   | +208% | 85.7GB/s   |
| 512 B   | +80%  | 146.9GB/s  |
| 1 KiB   | +17%  | 225.4GB/s  |
| 2 KiB   | +4%   | 253.4GB/s  |
| 4 KiB   | +3%   | 256.4GB/s  | page, 4 KiB |
| 8 KiB   | -     | 263.8GB/s  |
| 16 KiB  | +5%   | 250.1GB/s  |
| 32 KiB  | +1%   | 260.5GB/s  | L1D, 32 KiB |
| 64 KiB  | +93%  | 136.8GB/s  |
| 128 KiB | +94%  | 136.2GB/s  |
| 256 KiB | +92%  | 137.6GB/s  |
| 512 KiB | +95%  | 135.1GB/s  | L2, 512 KiB |
| 1 MiB   | +172% | 97.0GB/s   |
| 2 MiB   | +176% | 95.6GB/s   |
| 4 MiB   | +180% | 94.4GB/s   |
| 8 MiB   | +184% | 92.9GB/s   |
| 16 MiB  | +403% | 52.4GB/s   | L3, 16 MiB  |
| 32 MiB  | +964% | 24.8GB/s   |
| 64 MiB  | +967% | 24.7GB/s   |
| 128 MiB | +981% | 24.4GB/s   |

**read from cache (SIMD xor v2)**

| block | diff | bandwidth | comment |
|---------|-------|------------|-------------|
| 256 B   | +207% | 85.5GB/s   |
| 512 B   | +79%  | 146.7GB/s  |
| 1 KiB   | +17%  | 225.4GB/s  |
| 2 KiB   | +4%   | 252.9GB/s  |
| 4 KiB   | +1%   | 259.8GB/s  | page, 4 KiB |
| 8 KiB   | -     | 262.9GB/s  |
| 16 KiB  | +5%   | 250.8GB/s  |
| 32 KiB  | +5%   | 249.6GB/s  | L1D, 32 KiB |
| 64 KiB  | +97%  | 133.7GB/s  |
| 128 KiB | +93%  | 136.2GB/s  |
| 256 KiB | +90%  | 138.1GB/s  |
| 512 KiB | +95%  | 134.5GB/s  | L2, 512 KiB |
| 1 MiB   | +159% | 101.6GB/s  |
| 2 MiB   | +158% | 102.1GB/s  |
| 4 MiB   | +159% | 101.7GB/s  |
| 8 MiB   | +160% | 101.2GB/s  |
| 16 MiB  | +377% | 55.1GB/s   | L3, 16 MiB  |
| 32 MiB  | +933% | 25.4GB/s   |
| 64 MiB  | +900% | 26.3GB/s   |
| 128 MiB | +890% | 26.6GB/s   |

</details>
<details><summary><b>memset, multithreading (4 threads bound to physical cores)</b></summary>

**memset**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|-------|------------|--------------|
| 256 B   | 35.55 ms | +76%   | -     | 302.0GB/s  |              |
| 512 B   | 27.76 ms | +37%   | +22%  | 386.8GB/s  |              |
| 1 KiB   | 24.82 ms | +23%   | +11%  | 432.5GB/s  |              |
| 2 KiB   | 26.22 ms | +30%   | +6%   | 409.5GB/s  |              |
| 4 KiB   | 24.16 ms | +19%   | +8%   | 444.5GB/s  |  page, 4 KiB |
| 8 KiB   | 20.22 ms | -      | +16%  | 531.1GB/s  |              |
| 16 KiB  | 25.98 ms | +29%   | +29%  | 413.3GB/s  |              |
| 32 KiB  | 23.17 ms | +15%   | +11%  | 463.5GB/s  |  L1D, 32 KiB |
| 64 KiB  | 20.92 ms | +3%    | +10%  | 513.2GB/s  |              |
| 128 KiB | 22.91 ms | +13%   | +9%   | 468.7GB/s  |              |
| 256 KiB | 30.48 ms | +51%   | +33%  | 352.3GB/s  |              |
| 512 KiB | 29.64 ms | +47%   | +3%   | 362.3GB/s  |  L2, 512 KiB |
| 1 MiB   | 35.76 ms | +77%   | +21%  | 300.3GB/s  |              |
| 2 MiB   | 35.87 ms | +77%   | -     | 299.3GB/s  |              |
| 4 MiB   | 0.38 s   | +1798% | +970% | 28.0GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 0.47 s   | +2246% | +24%  | 22.6GB/s   |              |
| 16 MiB  | 0.61 s   | +2918% | +29%  | 17.6GB/s   |  L3, 16 MiB  |
| 32 MiB  | 0.61 s   | +2918% | -     | 17.6GB/s   |              |
| 64 MiB  | 0.45 s   | +2134% | +26%  | 23.8GB/s   |              |
| 128 MiB | 0.45 s   | +2135% | -     | 23.8GB/s   |              |

**SIMD cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|--------|------------|--------------|
| 256 B   | 27.58 ms | +25%   | -      | 389.3GB/s  |              |
| 512 B   | 22.11 ms | -      | +20%   | 485.7GB/s  |              |
| 1 KiB   | 27.93 ms | +27%   | +26%   | 384.4GB/s  |              |
| 2 KiB   | 22.08 ms | -      | +21%   | 486.4GB/s  |              |
| 4 KiB   | 24.06 ms | +9%    | +9%    | 446.2GB/s  |  page, 4 KiB |
| 8 KiB   | 23.91 ms | +8%    | +1%    | 449.1GB/s  |              |
| 16 KiB  | 26.83 ms | +22%   | +12%   | 400.2GB/s  |              |
| 32 KiB  | 25.18 ms | +14%   | +6%    | 426.4GB/s  |  L1D, 32 KiB |
| 64 KiB  | 22.93 ms | +4%    | +9%    | 468.3GB/s  |              |
| 128 KiB | 27.32 ms | +24%   | +19%   | 393.0GB/s  |              |
| 256 KiB | 27.32 ms | +24%   | -      | 393.0GB/s  |              |
| 512 KiB | 32.65 ms | +48%   | +19%   | 328.9GB/s  |  L2, 512 KiB |
| 1 MiB   | 35.72 ms | +62%   | +9%    | 300.6GB/s  |              |
| 2 MiB   | 33.86 ms | +53%   | +5%    | 317.1GB/s  |              |
| 4 MiB   | 0.42 s   | +1799% | +1138% | 25.6GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 0.49 s   | +2111% | +16%   | 22.0GB/s   |              |
| 16 MiB  | 0.61 s   | +2672% | +25%   | 17.5GB/s   |  L3, 16 MiB  |
| 32 MiB  | 0.61 s   | +2667% | -      | 17.6GB/s   |              |
| 64 MiB  | 0.61 s   | +2665% | -      | 17.6GB/s   |              |
| 128 MiB | 0.61 s   | +2673% | -      | 17.5GB/s   |              |

**SIMD non-cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|-------|------|-----------|--------------|
| 256 B   | 1.11 s | +146% | -    | 9.66GB/s  |              |
| 512 B   | 0.63 s | +40%  | +43% | 17.0GB/s  |              |
| 1 KiB   | 0.46 s | +1%   | +28% | 23.6GB/s  |              |
| 2 KiB   | 0.45 s | -     | -    | 23.7GB/s  |              |
| 4 KiB   | 0.45 s | -     | -    | 23.7GB/s  |  page, 4 KiB |
| 8 KiB   | 0.46 s | +1%   | +1%  | 23.6GB/s  |              |
| 16 KiB  | 0.45 s | -     | +1%  | 23.7GB/s  |              |
| 32 KiB  | 0.45 s | -     | -    | 23.8GB/s  |  L1D, 32 KiB |
| 64 KiB  | 0.45 s | -     | -    | 23.7GB/s  |              |
| 128 KiB | 0.45 s | -     | -    | 23.8GB/s  |              |
| 256 KiB | 0.45 s | -     | -    | 23.8GB/s  |              |
| 512 KiB | 0.45 s | -     | -    | 23.7GB/s  |  L2, 512 KiB |
| 1 MiB   | 0.45 s | +1%   | -    | 23.6GB/s  |              |
| 2 MiB   | 0.45 s | -     | +1%  | 23.7GB/s  |              |
| 4 MiB   | 0.45 s | -     | -    | 23.8GB/s  | L3 slice, 4 MiB |
| 8 MiB   | 0.45 s | -     | -    | 23.8GB/s  |              |
| 16 MiB  | 0.45 s | -     | -    | 23.7GB/s  |  L3, 16 MiB  |
| 32 MiB  | 0.45 s | -     | -    | 23.8GB/s  |              |
| 64 MiB  | 0.45 s | -     | -    | 23.8GB/s  |              |
| 128 MiB | 0.45 s | -     | -    | 23.7GB/s  |              |

</details>

<details><summary><b>other, multithreading (4 threads, 4 cores)</b></summary>

**read from cache with loop dependency (SIMD xor)**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|--------|------------|------------|-------------|
| 256 B   | +65%   | 402.0GB/s  | 100.5GB/s  |
| 512 B   | +26%   | 525.0GB/s  | 131.3GB/s  |
| 1 KiB   | +8%    | 612.7GB/s  | 153.2GB/s  |
| 2 KiB   | +11%   | 598.7GB/s  | 149.7GB/s  |
| 4 KiB   | +15%   | 577.7GB/s  | 144.4GB/s  | page, 4 KiB |
| 8 KiB   | -      | 662.9GB/s  | 165.7GB/s  |
| 16 KiB  | +8%    | 615.4GB/s  | 153.9GB/s  |
| 32 KiB  | +49%   | 445.4GB/s  | 111.4GB/s  | L1D, 32 KiB |
| 64 KiB  | +73%   | 382.8GB/s  | 95.7GB/s   |
| 128 KiB | +60%   | 414.6GB/s  | 103.6GB/s  |
| 256 KiB | +40%   | 472.2GB/s  | 118.1GB/s  |
| 512 KiB | +112%  | 312.4GB/s  | 78.1GB/s   | L2, 512 KiB |
| 1 MiB   | +98%   | 334.9GB/s  | 83.7GB/s   |
| 2 MiB   | +102%  | 327.5GB/s  | 81.9GB/s   |
| 4 MiB   | +408%  | 130.4GB/s  | 32.6GB/s   |
| 8 MiB   | +1274% | 48.2GB/s   | 12.1GB/s   |
| 16 MiB  | +1588% | 39.3GB/s   | 9.82GB/s   | L3, 16 MiB  |
| 32 MiB  | +1596% | 39.1GB/s   | 9.77GB/s   |
| 64 MiB  | +1610% | 38.8GB/s   | 9.69GB/s   |
| 128 MiB | +1594% | 39.1GB/s   | 9.78GB/s   |

**read from cache (SIMD xor v2)**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|--------|------------|------------|-------------|
| 256 B   | +145%  | 328.7GB/s  | 82.2GB/s   |
| 512 B   | +67%   | 482.0GB/s  | 120.5GB/s  |
| 1 KiB   | -      | 801.6GB/s  | 200.4GB/s  |
| 2 KiB   | +15%   | 701.2GB/s  | 175.3GB/s  |
| 4 KiB   | -      | 805.3GB/s  | 201.3GB/s  | page, 4 KiB |
| 8 KiB   | +1%    | 796.1GB/s  | 199.0GB/s  |
| 16 KiB  | +9%    | 736.6GB/s  | 184.1GB/s  |
| 32 KiB  | +68%   | 480.6GB/s  | 120.2GB/s  | L1D, 32 KiB |
| 64 KiB  | +111%  | 382.1GB/s  | 95.5GB/s   |
| 128 KiB | +111%  | 381.4GB/s  | 95.3GB/s   |
| 256 KiB | +131%  | 348.9GB/s  | 87.2GB/s   |
| 512 KiB | +128%  | 353.0GB/s  | 88.2GB/s   | L2, 512 KiB |
| 1 MiB   | +170%  | 298.5GB/s  | 74.6GB/s   |
| 2 MiB   | +153%  | 317.9GB/s  | 79.5GB/s   |
| 4 MiB   | +1075% | 68.5GB/s   | 17.1GB/s   |
| 8 MiB   | +1557% | 48.6GB/s   | 12.1GB/s   |
| 16 MiB  | +1966% | 39.0GB/s   | 9.74GB/s   | L3, 16 MiB  |
| 32 MiB  | +1998% | 38.4GB/s   | 9.60GB/s   |
| 64 MiB  | +2103% | 36.6GB/s   | 9.14GB/s   |
| 128 MiB | +1964% | 39.0GB/s   | 9.75GB/s   |

</details>


# AMD Phenom II X4 945

* Clock: 3 GHz
* Arch: Deneb, 2009
* Memory: DDR3-1333, dual channel ?, 10.6GB/s
* Cache:
    - L1I: 4x 64KB
    - L1D: 4x 64KB
    - L2: 4x 512KB
    - L3: 6MB, 4x 1.5MB

<details><summary><b>memset, single thread</b></summary>

**memset**

| block | time | diff | bandwidth | comment |
|------|------|----|-----------|-------|
| 256  b | 76.17 ms | +131.3 % | 17.6 GB/s | |
| 512  b | 57.69 ms | +75.2 % | 23.3 GB/s | |
| 1  kB | 48.77 ms | +48.1 % | 27.5 GB/s | |
| 2  kB | 43.03 ms | +30.7 % | 31.2 GB/s | |
| 4  kB | 37.28 ms | +13.2 % | 36.0 GB/s | |
| 8  kB | 34.68 ms | +5.3 % | 38.7 GB/s | |
| 16  kB | 39.36 ms | +19.5 % | 34.1 GB/s | |
| 32  kB | 38.15 ms | +15.9 % | 35.2 GB/s | |
| 64  kB | 32.93 ms |  | 40.8 GB/s |  L1 |
| 128  kB | 95.97 ms | +191.4 % | 14.0 GB/s | |
| 256  kB | 0.13 s | +287.0 % | 10.5 GB/s | |
| 512  kB | 0.10 s | +211.0 % | 13.1 GB/s |  L2 |
| 1  MB | 0.16 s | +391.9 % | 8.3 GB/s | |
| 2  MB | 0.17 s | +405.0 % | 8.1 GB/s | L3  1.5 MB bank |
| 4  MB | 0.19 s | +478.1 % | 7.0 GB/s | |
| 8  MB | 0.35 s | +951.6 % | 3.9 GB/s |  L3  6 MB |
| 16  MB | 0.36 s | +1001.5 % | 3.7 GB/s | |
| 32  MB | 0.36 s | +1000.7 % | 3.7 GB/s | |
| 64  MB | 0.36 s | +1008.2 % | 3.7 GB/s | |

</details>

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block | time | diff | bandwidth | comment |
|------|------|----|-----------|-------|
| 256  b | 0.10 s | +88.8 % | 13.0 GB/s | |
| 512  b | 74.67 ms | +37.0 % | 18.0 GB/s | |
| 1  kB | 66.58 ms | +22.1 % | 20.2 GB/s | |
| 2  kB | 62.47 ms | +14.6 % | 21.5 GB/s | |
| 4  kB | 55.15 ms | +1.2 % | 24.3 GB/s | |
| 8  kB | 54.51 ms |  | 24.6 GB/s | |
| 16  kB | 57.49 ms | +5.5 % | 23.3 GB/s | |
| 32  kB | 61.11 ms | +12.1 % | 22.0 GB/s | |
| 64  kB | 0.15 s | +183.6 % | 8.7 GB/s |  L1 |
| 128  kB | 0.15 s | +167.6 % | 9.2 GB/s | |
| 256  kB | 0.16 s | +199.9 % | 8.2 GB/s | |
| 512  kB | 0.32 s | +495.5 % | 4.1 GB/s |  L2 |
| 1  MB | 0.33 s | +500.3 % | 4.1 GB/s | |
| 2  MB | 0.36 s | +552.2 % | 3.8 GB/s | L3  1.5 MB bank |
| 4  MB | 0.55 s | +906.5 % | 2.4 GB/s | |
| 8  MB | 0.57 s | +940.0 % | 2.4 GB/s |  L3  6 MB |
| 16  MB | 0.56 s | +923.3 % | 2.4 GB/s | |
| 32  MB | 0.57 s | +939.4 % | 2.4 GB/s | |
| 64  MB | 0.57 s | +946.5 % | 2.4 GB/s | |

**SIMD cached copy**

| block | time | diff | bandwidth | comment |
|------|------|----|-----------|-------|
| 256  b | 68.44 ms | +12.7 % | 19.6 GB/s | |
| 512  b | 68.59 ms | +12.9 % | 19.6 GB/s | |
| 1  kB | 61.15 ms | +0.7 % | 21.9 GB/s | |
| 2  kB | 62.69 ms | +3.2 % | 21.4 GB/s | |
| 4  kB | 64.69 ms | +6.5 % | 20.7 GB/s | |
| 8  kB | 67.25 ms | +10.8 % | 20.0 GB/s | |
| 16  kB | 60.72 ms |  | 22.1 GB/s | |
| 32  kB | 66.18 ms | +9.0 % | 20.3 GB/s | |
| 64  kB | 0.14 s | +126.0 % | 9.8 GB/s |  L1 |
| 128  kB | 0.16 s | +161.7 % | 8.4 GB/s | |
| 256  kB | 0.15 s | +154.5 % | 8.7 GB/s | |
| 512  kB | 0.32 s | +432.4 % | 4.2 GB/s |  L2 |
| 1  MB | 0.32 s | +434.2 % | 4.1 GB/s | |
| 2  MB | 0.36 s | +491.4 % | 3.7 GB/s | L3  1.5 MB bank |
| 4  MB | 0.56 s | +823.4 % | 2.4 GB/s | |
| 8  MB | 0.58 s | +858.3 % | 2.3 GB/s |  L3  6 MB |
| 16  MB | 0.58 s | +856.4 % | 2.3 GB/s | |
| 32  MB | 0.58 s | +852.7 % | 2.3 GB/s | |
| 64  MB | 0.58 s | +862.6 % | 2.3 GB/s | |

**SIMD non‑cached copy**

| block | time | diff | bandwidth | comment |
|------|------|----|-----------|-------|
| 256  b | 0.85 s | +328.0 % | 0.8 GB/s | |
| 512  b | 0.45 s | +127.1 % | 1.5 GB/s | |
| 1  kB | 0.39 s | +94.5 % | 1.7 GB/s | |
| 2  kB | 0.34 s | +70.9 % | 2.0 GB/s | |
| 4  kB | 0.32 s | +62.5 % | 2.1 GB/s | |
| 8  kB | 0.33 s | +65.7 % | 2.0 GB/s | |
| 16  kB | 0.32 s | +62.2 % | 2.1 GB/s | |
| 32  kB | 0.32 s | +62.1 % | 2.1 GB/s | |
| 64  kB | 0.32 s | +62.9 % | 2.1 GB/s |  L1 |
| 128  kB | 0.31 s | +55.8 % | 2.2 GB/s | |
| 256  kB | 0.31 s | +53.3 % | 2.2 GB/s | |
| 512  kB | 0.31 s | +55.2 % | 2.2 GB/s |  L2 |
| 1  MB | 0.20 s | +0.6 % | 3.4 GB/s | |
| 2  MB | 0.20 s |  | 3.4 GB/s | |
| 4  MB | 0.20 s | +0.1 % | 3.4 GB/s |  |
| 8  MB | 0.20 s | +2.0 % | 3.3 GB/s | L3  6 MB |
| 16  MB | 0.22 s | +10.5 % | 3.1 GB/s |  |
| 32  MB | 0.22 s | +8.4 % | 3.1 GB/s |  |

</details>

<details><summary><b>memcpy, multithreading (4 threads, 4 cores)</b></summary>

**memcpy**

| block | time | diff | bandwidth | comment |
|------|------|----|-----------|-------|
| 256  b | 0.12 s | +114.8 % | 43.7 GB/s | |
| 512  b | 78.47 ms | +37.1 % | 68.4 GB/s | |
| 1  kB | 65.91 ms | +15.1 % | 81.5 GB/s | |
| 2  kB | 66.56 ms | +16.3 % | 80.7 GB/s | |
| 4  kB | 60.09 ms | +5.0 % | 89.3 GB/s | |
| 8  kB | 57.25 ms |  | 93.8 GB/s | |
| 16  kB | 62.44 ms | +9.1 % | 86.0 GB/s | |
| 32  kB | 60.78 ms | +6.2 % | 88.3 GB/s | |
| 64  kB | 0.14 s | +151.2 % | 37.3 GB/s |  L1 |
| 128  kB | 0.15 s | +165.1 % | 35.4 GB/s | |
| 256  kB | 0.15 s | +169.1 % | 34.9 GB/s | |
| 512  kB | 0.33 s | +474.8 % | 16.3 GB/s | L2 |
| 1  MB | 0.37 s | +537.8 % | 14.7 GB/s | |
| 2  MB | 1.34 s | +2234.9 % | 4.0 GB/s |  L3  6 MB |
| 4  MB | 1.34 s | +2237.7 % | 4.0 GB/s | |
| 8  MB | 1.34 s | +2233.8 % | 4.0 GB/s | |
| 16  MB | 1.34 s | +2233.6 % | 4.0 GB/s | |
| 32  MB | 1.32 s | +2207.5 % | 4.1 GB/s | |
| 64  MB | 1.32 s | +2207.9 % | 4.1 GB/s | |

**SIMD cached copy**

| block | time | diff | bandwidth | comment |
|------|------|----|-----------|-------|
| 256  b | 0.11 s | +49.2 % | 49.7 GB/s | |
| 512  b | 75.57 ms | +4.4 % | 71.0 GB/s | |
| 1  kB | 72.41 ms |  | 74.1 GB/s | |
| 2  kB | 77.74 ms | +7.4 % | 69.1 GB/s | |
| 4  kB | 81.24 ms | +12.2 % | 66.1 GB/s | |
| 8  kB | 78.90 ms | +9.0 % | 68.0 GB/s | |
| 16  kB | 77.27 ms | +6.7 % | 69.5 GB/s | |
| 32  kB | 78.05 ms | +7.8 % | 68.8 GB/s | |
| 64  kB | 0.20 s | +176.0 % | 26.9 GB/s |  L1 |
| 128  kB | 0.22 s | +205.8 % | 24.2 GB/s | |
| 256  kB | 0.24 s | +234.7 % | 22.1 GB/s | |
| 512  kB | 0.44 s | +514.1 % | 12.1 GB/s |  L2 |
| 1  MB | 0.48 s | +565.4 % | 11.1 GB/s | |
| 2  MB | 1.47 s | +1927.1 % | 3.7 GB/s |  L3  1.5 MB bank? |
| 4  MB | 1.47 s | +1935.3 % | 3.6 GB/s |  L3  6 MB |
| 8  MB | 1.50 s | +1969.7 % | 3.6 GB/s | |
| 16  MB | 1.51 s | +1985.6 % | 3.6 GB/s | |
| 32  MB | 1.51 s | +1986.2 % | 3.6 GB/s | |
| 64  MB | 1.46 s | +1914.7 % | 3.7 GB/s | |

**SIMD non‑cached copy**

| block | time | diff | bandwidth | comment |
|------|------|----|-----------|-------|
| 256  b | 0.93 s | +106.5 % | 2.9 GB/s | |
| 512  b | 0.72 s | +60.8 % | 3.7 GB/s | |
| 1  kB | 0.52 s | +15.4 % | 5.2 GB/s | |
| 2  kB | 0.48 s | +7.0 % | 5.6 GB/s | |
| 4  kB | 0.48 s | +6.4 % | 5.6 GB/s | |
| 8  kB | 0.47 s | +4.0 % | 5.7 GB/s | |
| 16  kB | 0.49 s | +8.5 % | 5.5 GB/s | |
| 32  kB | 0.49 s | +8.2 % | 5.5 GB/s | |
| 64  kB | 0.49 s | +8.8 % | 5.5 GB/s |  L1 |
| 128  kB | 0.50 s | +10.2 % | 5.4 GB/s | |
| 256  kB | 0.49 s | +9.3 % | 5.5 GB/s | |
| 512  kB | 0.48 s | +7.2 % | 5.6 GB/s |  L2 |
| 1  MB | 0.45 s |  | 6.0 GB/s | |
| 2  MB | 0.46 s | +3.2 % | 5.8 GB/s |  |
| 4  MB | 0.47 s | +3.6 % | 5.8 GB/s | |
| 8  MB | 0.47 s | +3.8 % | 5.8 GB/s | L3  6 MB |
| 16  MB | 0.47 s | +5.5 % | 5.7 GB/s |  |
| 32  MB | 0.48 s | +6.0 % | 5.6 GB/s |  |

</details>


# Intel i5 8250U

* Clock: 3.39 GHz
* Cache:
    - L1I: 4x 32KB
    - L1D: 4x 32KB
    - L2: 4x 256KB
    - L3: 6MB: 4x 1.5MB
* Memory: 8GB DDR3, 1867MHz, dual channel, 29.8 GB/s

<details>

```
std::memset, single thread:
  256 b  : 91.40 ms  +260.9%  - 29.4GB/s
  512 b  : 43.51 ms  +71.8%   - 61.7GB/s
  1024 b : 34.31 ms  +35.5%   - 78.2GB/s
  2048 b : 26.73 ms  +5.5%    - 100.4GB/s
  4 Kb   : 25.46 ms  +0.5%    - 105.4GB/s - page size
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
  4 Kb   : 24.99 ms  +0.0%    - 107.4GB/s - page size
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
  4 Kb   : 19.08 ms  +33.4%   - 281.3GB/s - page size
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

</details>

# Intel Ultra 7 255H

<details><summary><b>Cache hierarchy from cpuid</b></summary>

```
Performance
    base clock: 3700 MHz
    max clock:  5100 MHz
    threads:    6 / 6
    IDs:        (0-1, 10-13)
    -----
    L1I.lineSize:      64 B
    L1I.associativity: 16
    L1I.cores          2
    L1I.size:  . . . . 64 KiB
    L1I.total:         192 KiB
    -----
    L1D.lineSize:      64 B
    L1D.associativity: 12
    L1D.cores          2
    L1D.size:  . . . . 48 KiB
    L1D.total:         144 KiB
    -----
    L2 .lineSize:      64 B
    L2 .associativity: 12
    L2 .cores          8
    L2 .size:  . . . . 3 MiB
    L2 .total:         3 MiB
    -----
    L3 .lineSize:      64 B
    L3 .associativity: 12
    L3 .cores          64
    L3 .size:  . . . . 24 MiB
    L3 .total:         24 MiB
    ----------

EnergyEfficient
    base clock: 3700 MHz
    max clock:  4400 MHz
    threads:    8 / 8
    IDs:        (2-9)
    -----
    L1I.lineSize:      64 B
    L1I.associativity: 8
    L1I.cores          1
    L1I.size:  . . . . 64 KiB
    L1I.total:         512 KiB
    -----
    L1D.lineSize:      64 B
    L1D.associativity: 8
    L1D.cores          1
    L1D.size:  . . . . 32 KiB
    L1D.total:         256 KiB
    -----
    L2 .lineSize:      64 B
    L2 .associativity: 16
    L2 .cores          8
    L2 .size:  . . . . 4 MiB
    L2 .total:         4 MiB
    -----
    L3 .lineSize:      64 B
    L3 .associativity: 12
    L3 .cores          64
    L3 .size:  . . . . 24 MiB
    L3 .total:         24 MiB
    ----------

LowPower
    base clock: 3700 MHz
    max clock:  4400 MHz
    threads:    2 / 2
    IDs:        (14-15)
    -----
    L1I.lineSize:      64 B
    L1I.associativity: 8
    L1I.cores          1
    L1I.size:  . . . . 64 KiB
    L1I.total:         128 KiB
    -----
    L1D.lineSize:      64 B
    L1D.associativity: 8
    L1D.cores          1
    L1D.size:  . . . . 32 KiB
    L1D.total:         64 KiB
    -----
    L2 .lineSize:      64 B
    L2 .associativity: 16
    L2 .cores          8
    L2 .size:  . . . . 2 MiB
    L2 .total:         2 MiB
    ----------
```

</details>

## P-core

* Theoretical performance:<br/>
  64B/cy write to L1 * 5.1GHz = 326 GB/s - max speed of memset<br/>
  128B/cy read from L1 * 5.1GHz = 652 GB/s - max speed of sum/hash<br/>
  32B/cy write to L2 * 5.1GHz = 163 GB/s - max speed of memset to L2<br/>

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|-------|------------|--------------|
| 256 B   | 33.82 ms | +278%  | -     | 79.4GB/s   |
| 512 B   | 26.64 ms | +198%  | +21%  | 100.7GB/s  |
| 1 KiB   | 18.54 ms | +107%  | +30%  | 144.8GB/s  |
| 2 KiB   | 11.13 ms | +24%   | +40%  | 241.3GB/s  |
| 4 KiB   | 8.94 ms  | -      | +20%  | 300.2GB/s  | page, 4 KiB  |
| 8 KiB   | 8.94 ms  | -      | -     | 300.1GB/s  |
| 16 KiB  | 9.18 ms  | +3%    | +3%   | 292.5GB/s  |
| 32 KiB  | 31.48 ms | +252%  | +243% | 85.3GB/s   |
| 64 KiB  | 33.58 ms | +275%  | +7%   | 79.9GB/s   | L1D, 48 KiB  |
| 128 KiB | 34.09 ms | +281%  | +2%   | 78.8GB/s   |
| 256 KiB | 36.07 ms | +303%  | +6%   | 74.4GB/s   | L1.5D, 192 KB |
| 512 KiB | 37.31 ms | +317%  | +3%   | 71.9GB/s   |
| 1 MiB   | 36.96 ms | +313%  | +1%   | 72.6GB/s   |
| 2 MiB   | 0.11 s   | +1125% | +196% | 24.5GB/s   |
| 4 MiB   | 0.11 s   | +1125% | -     | 24.5GB/s   | L2, 3 MiB |
| 8 MiB   | 0.11 s   | +1123% | -     | 24.5GB/s   |
| 16 MiB  | 0.11 s   | +1128% | -     | 24.5GB/s   |
| 32 MiB  | 0.11 s   | +1147% | +2%   | 24.1GB/s   | L3, 24 MiB   |
| 64 MiB  | 0.11 s   | +1136% | +1%   | 24.3GB/s   |
| 128 MiB | 0.12 s   | +1191% | +4%   | 23.3GB/s   |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|-------|------------|--------------|
| 256 B   | 34.23 ms | +293%  | -     | 78.4GB/s   |
| 512 B   | 20.40 ms | +134%  | +40%  | 131.6GB/s  |
| 1 KiB   | 16.07 ms | +85%   | +21%  | 167.1GB/s  |
| 2 KiB   | 10.40 ms | +20%   | +35%  | 258.0GB/s  |
| 4 KiB   | 8.71 ms  | -      | +16%  | 308.1GB/s  | page, 4 KiB  |
| 8 KiB   | 8.71 ms  | -      | -     | 308.4GB/s  |
| 16 KiB  | 8.71 ms  | -      | -     | 308.2GB/s  |
| 32 KiB  | 35.82 ms | +312%  | +311% | 74.9GB/s   |
| 64 KiB  | 35.30 ms | +305%  | +1%   | 76.0GB/s   | L1D, 48 KiB  |
| 128 KiB | 47.89 ms | +450%  | +36%  | 56.1GB/s   |
| 256 KiB | 46.40 ms | +433%  | +3%   | 57.9GB/s   | L1.5D, 192 KB |
| 512 KiB | 45.82 ms | +426%  | +1%   | 58.6GB/s   |
| 1 MiB   | 46.27 ms | +431%  | +1%   | 58.0GB/s   |
| 2 MiB   | 0.10 s   | +1065% | +119% | 26.5GB/s   |
| 4 MiB   | 0.12 s   | +1285% | +19%  | 22.3GB/s   | L2, 3 MiB |
| 8 MiB   | 0.12 s   | +1268% | +1%   | 22.5GB/s   |
| 16 MiB  | 0.13 s   | +1433% | +12%  | 20.1GB/s   |
| 32 MiB  | 0.16 s   | +1763% | +22%  | 16.5GB/s   | L3, 24 MiB   |
| 64 MiB  | 0.18 s   | +1973% | +11%  | 14.9GB/s   |
| 128 MiB | 0.19 s   | +2055% | +4%   | 14.3GB/s   |

**SIMD non-cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|--------|------|-----------|--------------|
| 256 B   | 2.28 s | +1571% | -    | 1.18GB/s  |
| 512 B   | 1.16 s | +749%  | +49% | 2.31GB/s  |
| 1 KiB   | 0.61 s | +345%  | +48% | 4.41GB/s  |
| 2 KiB   | 0.34 s | +149%  | +44% | 7.89GB/s  |
| 4 KiB   | 0.31 s | +127%  | +9%  | 8.66GB/s  | page, 4 KiB  |
| 8 KiB   | 0.24 s | +74%   | +23% | 11.3GB/s  |
| 16 KiB  | 0.23 s | +67%   | +4%  | 11.7GB/s  |
| 32 KiB  | 0.23 s | +66%   | +1%  | 11.8GB/s  |
| 64 KiB  | 0.22 s | +62%   | +3%  | 12.1GB/s  | L1D, 48 KiB  |
| 128 KiB | 0.16 s | +16%   | +28% | 17.0GB/s  |
| 256 KiB | 0.14 s | +3%    | +11% | 19.1GB/s  |
| 512 KiB | 0.14 s | +3%    | -    | 19.1GB/s  |
| 1 MiB   | 0.14 s | +3%    | -    | 19.1GB/s  |
| 2 MiB   | 0.14 s | -      | +3%  | 19.6GB/s  |
| 4 MiB   | 0.14 s | +2%    | +2%  | 19.3GB/s  | L2, 3 MiB |
| 8 MiB   | 0.14 s | +4%    | +2%  | 19.0GB/s  |
| 16 MiB  | 0.14 s | +4%    | +1%  | 18.8GB/s  |
| 32 MiB  | 0.14 s | +5%    | +1%  | 18.7GB/s  | L3, 24 MiB   |
| 64 MiB  | 0.14 s | +5%    | -    | 18.7GB/s  |
| 128 MiB | 0.14 s | +5%    | -    | 18.7GB/s  |

</details>

<details><summary><b>memcpy, multithreading (4 threads, 4 cores)</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|------------|--------------|
| 256 B   | 40.13 ms | +214%  | -     | 267.6GB/s  | 66.9GB/s   |
| 512 B   | 30.11 ms | +136%  | +25%  | 356.6GB/s  | 89.2GB/s   |
| 1 KiB   | 22.72 ms | +78%   | +25%  | 472.7GB/s  | 118.2GB/s  |
| 2 KiB   | 14.99 ms | +17%   | +34%  | 716.2GB/s  | 179.0GB/s  |
| 4 KiB   | 13.92 ms | +9%    | +7%   | 771.5GB/s  | 192.9GB/s  | page, 4 KiB  |
| 8 KiB   | 13.25 ms | +4%    | +5%   | 810.6GB/s  | 202.6GB/s  |
| 16 KiB  | 12.76 ms | -      | +4%   | 841.4GB/s  | 210.4GB/s  |
| 32 KiB  | 36.33 ms | +185%  | +185% | 295.6GB/s  | 73.9GB/s   |
| 64 KiB  | 39.35 ms | +208%  | +8%   | 272.8GB/s  | 68.2GB/s   | L1D, 48 KiB  |
| 128 KiB | 39.78 ms | +212%  | +1%   | 269.9GB/s  | 67.5GB/s   |
| 256 KiB | 42.28 ms | +231%  | +6%   | 254.0GB/s  | 63.5GB/s   | L1.5D, 192 KB |
| 512 KiB | 42.91 ms | +236%  | +1%   | 250.3GB/s  | 62.6GB/s   |
| 1 MiB   | 53.36 ms | +318%  | +24%  | 201.2GB/s  | 50.3GB/s   |
| 2 MiB   | 0.30 s   | +2273% | +468% | 35.5GB/s   | 8.86GB/s   |
| 4 MiB   | 0.30 s   | +2266% | -     | 35.6GB/s   | 8.89GB/s   | L2, 3 MiB |
| 8 MiB   | 0.30 s   | +2275% | -     | 35.4GB/s   | 8.86GB/s   |
| 16 MiB  | 0.30 s   | +2265% | -     | 35.6GB/s   | 8.89GB/s   |
| 32 MiB  | 0.30 s   | +2283% | +1%   | 35.3GB/s   | 8.83GB/s   | L3, 24 MiB   |
| 64 MiB  | 0.30 s   | +2285% | -     | 35.3GB/s   | 8.82GB/s   |
| 128 MiB | 0.31 s   | +2290% | -     | 35.2GB/s   | 8.80GB/s   |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|------------|--------------|
| 256 B   | 39.43 ms | +206%  | -     | 272.3GB/s  | 68.1GB/s   |
| 512 B   | 23.45 ms | +82%   | +41%  | 457.8GB/s  | 114.4GB/s  |
| 1 KiB   | 22.20 ms | +72%   | +5%   | 483.7GB/s  | 120.9GB/s  |
| 2 KiB   | 15.68 ms | +22%   | +29%  | 684.7GB/s  | 171.2GB/s  |
| 4 KiB   | 12.90 ms | -      | +18%  | 832.1GB/s  | 208.0GB/s  | page, 4 KiB  |
| 8 KiB   | 13.28 ms | +3%    | +3%   | 808.6GB/s  | 202.1GB/s  |
| 16 KiB  | 13.31 ms | +3%    | -     | 806.9GB/s  | 201.7GB/s  |
| 32 KiB  | 42.26 ms | +227%  | +218% | 254.1GB/s  | 63.5GB/s   |
| 64 KiB  | 42.38 ms | +228%  | -     | 253.3GB/s  | 63.3GB/s   | L1D, 48 KiB  |
| 128 KiB | 55.70 ms | +332%  | +31%  | 192.8GB/s  | 48.2GB/s   |
| 256 KiB | 55.69 ms | +332%  | -     | 192.8GB/s  | 48.2GB/s   | L1.5D, 192 KB |
| 512 KiB | 53.04 ms | +311%  | +5%   | 202.4GB/s  | 50.6GB/s   |
| 1 MiB   | 54.67 ms | +324%  | +3%   | 196.4GB/s  | 49.1GB/s   |
| 2 MiB   | 0.11 s   | +741%  | +98%  | 99.0GB/s   | 24.7GB/s   |
| 4 MiB   | 0.14 s   | +986%  | +29%  | 76.6GB/s   | 19.2GB/s   | L2, 3 MiB |
| 8 MiB   | 0.30 s   | +2209% | +113% | 36.0GB/s   | 9.01GB/s   |
| 16 MiB  | 0.39 s   | +2888% | +29%  | 27.8GB/s   | 6.96GB/s   |
| 32 MiB  | 0.42 s   | +3143% | +9%   | 25.7GB/s   | 6.41GB/s   | L3, 24 MiB   |
| 64 MiB  | 0.43 s   | +3251% | +3%   | 24.8GB/s   | 6.21GB/s   |
| 128 MiB | 0.44 s   | +3300% | +1%   | 24.5GB/s   | 6.12GB/s   |

**SIMD non-cached copy**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|--------|--------|------|-----------|-----------|--------------|
| 256 B   | 9.54 s | +3000% | -    | 4.50GB/s  | 1.13GB/s  |
| 512 B   | 4.97 s | +1515% | +48% | 8.64GB/s  | 2.16GB/s  |
| 1 KiB   | 2.65 s | +761%  | +47% | 16.2GB/s  | 4.05GB/s  |
| 2 KiB   | 1.62 s | +428%  | +39% | 26.4GB/s  | 6.61GB/s  |
| 4 KiB   | 1.37 s | +344%  | +16% | 31.4GB/s  | 7.86GB/s  | page, 4 KiB  |
| 8 KiB   | 1.22 s | +295%  | +11% | 35.3GB/s  | 8.84GB/s  |
| 16 KiB  | 1.29 s | +320%  | +6%  | 33.2GB/s  | 8.30GB/s  |
| 32 KiB  | 1.29 s | +320%  | -    | 33.2GB/s  | 8.31GB/s  |
| 64 KiB  | 1.29 s | +319%  | -    | 33.3GB/s  | 8.32GB/s  | L1D, 48 KiB  |
| 128 KiB | 1.23 s | +301%  | +4%  | 34.8GB/s  | 8.71GB/s  |
| 256 KiB | 1.23 s | +298%  | +1%  | 35.1GB/s  | 8.76GB/s  |
| 512 KiB | 1.22 s | +298%  | -    | 35.1GB/s  | 8.77GB/s  |
| 1 MiB   | 1.22 s | +298%  | -    | 35.1GB/s  | 8.77GB/s  |
| 2 MiB   | 1.22 s | +298%  | -    | 35.1GB/s  | 8.77GB/s  |
| 4 MiB   | 0.31 s | -      | +75% | 34.9GB/s  | 8.72GB/s  | L2, 3 MiB |
| 8 MiB   | 0.31 s | +1%    | +1%  | 34.7GB/s  | 8.67GB/s  |
| 16 MiB  | 0.31 s | +1%    | -    | 34.7GB/s  | 8.67GB/s  |
| 32 MiB  | 0.31 s | +1%    | +1%  | 34.5GB/s  | 8.62GB/s  | L3, 24 MiB   |
| 64 MiB  | 0.31 s | +2%    | -    | 34.3GB/s  | 8.59GB/s  |
| 128 MiB | 0.32 s | +3%    | +2%  | 33.8GB/s  | 8.45GB/s  |

</details>

<details><summary><b>memset, single thread</b></summary>

**memset**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|-------|------------|--------------|
| 256 B   | 18.36 ms | +106%  | -     | 146.2GB/s  |
| 512 B   | 11.51 ms | +29%   | +37%  | 233.3GB/s  |
| 1 KiB   | 10.04 ms | +12%   | +13%  | 267.5GB/s  |
| 2 KiB   | 9.71 ms  | +9%    | +3%   | 276.5GB/s  |
| 4 KiB   | 9.33 ms  | +5%    | +4%   | 287.7GB/s  | page, 4 KiB  |
| 8 KiB   | 9.16 ms  | +3%    | +2%   | 293.2GB/s  |
| 16 KiB  | 9.01 ms  | +1%    | +2%   | 298.0GB/s  |
| 32 KiB  | 8.92 ms  | -      | +1%   | 300.8GB/s  |
| 64 KiB  | 27.08 ms | +203%  | +203% | 99.1GB/s   | L1D, 48 KiB  |
| 128 KiB | 28.00 ms | +214%  | +3%   | 95.9GB/s   |
| 256 KiB | 26.53 ms | +197%  | +5%   | 101.2GB/s  | L1.5D, 192 KB |
| 512 KiB | 33.47 ms | +275%  | +26%  | 80.2GB/s   |
| 1 MiB   | 33.97 ms | +281%  | +1%   | 79.0GB/s   |
| 2 MiB   | 34.34 ms | +285%  | +1%   | 78.2GB/s   |
| 4 MiB   | 54.13 ms | +507%  | +58%  | 49.6GB/s   | L2, 3 MiB |
| 8 MiB   | 65.06 ms | +629%  | +20%  | 41.3GB/s   |
| 16 MiB  | 64.01 ms | +617%  | +2%   | 41.9GB/s   |
| 32 MiB  | 87.92 ms | +885%  | +37%  | 30.5GB/s   | L3, 24 MiB   |
| 64 MiB  | 0.13 s   | +1380% | +50%  | 20.3GB/s   |
| 128 MiB | 0.14 s   | +1496% | +8%   | 18.8GB/s   |

**SIMD cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|-------|------------|--------------|
| 256 B   | 8.46 ms  | -      | -     | 317.4GB/s  |
| 512 B   | 8.72 ms  | +3%    | +3%   | 307.9GB/s  |
| 1 KiB   | 8.86 ms  | +5%    | +2%   | 303.0GB/s  |
| 2 KiB   | 8.73 ms  | +3%    | +1%   | 307.4GB/s  |
| 4 KiB   | 9.02 ms  | +7%    | +3%   | 297.6GB/s  | page, 4 KiB  |
| 8 KiB   | 8.79 ms  | +4%    | +3%   | 305.4GB/s  |
| 16 KiB  | 9.17 ms  | +8%    | +4%   | 292.7GB/s  |
| 32 KiB  | 8.71 ms  | +3%    | +5%   | 308.2GB/s  |
| 64 KiB  | 26.09 ms | +209%  | +200% | 102.9GB/s  | L1D, 48 KiB  |
| 128 KiB | 26.47 ms | +213%  | +1%   | 101.4GB/s  |
| 256 KiB | 47.17 ms | +458%  | +78%  | 56.9GB/s   | L1.5D, 192 KB |
| 512 KiB | 34.91 ms | +313%  | +26%  | 76.9GB/s   |
| 1 MiB   | 35.08 ms | +315%  | -     | 76.5GB/s   |
| 2 MiB   | 35.84 ms | +324%  | +2%   | 74.9GB/s   |
| 4 MiB   | 54.81 ms | +548%  | +53%  | 49.0GB/s   | L2, 3 MiB |
| 8 MiB   | 63.47 ms | +651%  | +16%  | 42.3GB/s   |
| 16 MiB  | 63.20 ms | +647%  | -     | 42.5GB/s   |
| 32 MiB  | 92.71 ms | +996%  | +47%  | 29.0GB/s   | L3, 24 MiB   |
| 64 MiB  | 0.13 s   | +1408% | +38%  | 21.1GB/s   |
| 128 MiB | 0.14 s   | +1606% | +13%  | 18.6GB/s   |

**SIMD non-cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|------|-----------|--------------|
| 256 B   | 1.29 s   | +3529% | -    | 2.07GB/s  |
| 512 B   | 0.66 s   | +1751% | +49% | 4.07GB/s  |
| 1 KiB   | 0.34 s   | +840%  | +49% | 8.01GB/s  |
| 2 KiB   | 0.18 s   | +415%  | +45% | 14.6GB/s  |
| 4 KiB   | 0.10 s   | +193%  | +43% | 25.7GB/s  | page, 4 KiB  |
| 8 KiB   | 68.05 ms | +91%   | +35% | 39.4GB/s  |
| 16 KiB  | 48.97 ms | +37%   | +28% | 54.8GB/s  |
| 32 KiB  | 39.26 ms | +10%   | +20% | 68.4GB/s  |
| 64 KiB  | 35.66 ms | -      | +9%  | 75.3GB/s  | L1D, 48 KiB  |
| 128 KiB | 40.60 ms | +14%   | +14% | 66.1GB/s  |
| 256 KiB | 44.53 ms | +25%   | +10% | 60.3GB/s  |
| 512 KiB | 44.38 ms | +24%   | -    | 60.5GB/s  |
| 1 MiB   | 42.89 ms | +20%   | +3%  | 62.6GB/s  |
| 2 MiB   | 41.20 ms | +16%   | +4%  | 65.1GB/s  |
| 4 MiB   | 42.13 ms | +18%   | +2%  | 63.7GB/s  | L2, 3 MiB |
| 8 MiB   | 41.50 ms | +16%   | +1%  | 64.7GB/s  |
| 16 MiB  | 40.96 ms | +15%   | +1%  | 65.5GB/s  |
| 32 MiB  | 40.02 ms | +12%   | +2%  | 67.1GB/s  | L3, 24 MiB   |
| 64 MiB  | 41.49 ms | +16%   | +4%  | 64.7GB/s  |
| 128 MiB | 40.69 ms | +14%   | +2%  | 66.0GB/s  |

</details>

<details><summary><b>memset, multithreading (4 threads, 4 cores)</b></summary>

**memset**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|------------|--------------|
| 256 B   | 22.48 ms | +82%   | -     | 477.6GB/s  | 119.4GB/s  |
| 512 B   | 15.17 ms | +23%   | +33%  | 708.0GB/s  | 177.0GB/s  |
| 1 KiB   | 14.77 ms | +20%   | +3%   | 727.1GB/s  | 181.8GB/s  |
| 2 KiB   | 13.07 ms | +6%    | +11%  | 821.3GB/s  | 205.3GB/s  |
| 4 KiB   | 12.63 ms | +2%    | +3%   | 850.2GB/s  | 212.5GB/s  | page, 4 KiB  |
| 8 KiB   | 13.02 ms | +6%    | +3%   | 824.8GB/s  | 206.2GB/s  |
| 16 KiB  | 12.33 ms | -      | +5%   | 870.9GB/s  | 217.7GB/s  |
| 32 KiB  | 34.95 ms | +183%  | +183% | 307.2GB/s  | 76.8GB/s   |
| 64 KiB  | 31.96 ms | +159%  | +9%   | 336.0GB/s  | 84.0GB/s   | L1D, 48 KiB  |
| 128 KiB | 33.79 ms | +174%  | +6%   | 317.8GB/s  | 79.4GB/s   |
| 256 KiB | 37.61 ms | +205%  | +11%  | 285.5GB/s  | 71.4GB/s   | L1.5D, 192 KB |
| 512 KiB | 38.33 ms | +211%  | +2%   | 280.2GB/s  | 70.0GB/s   |
| 1 MiB   | 38.16 ms | +209%  | -     | 281.4GB/s  | 70.4GB/s   |
| 2 MiB   | 56.76 ms | +360%  | +49%  | 189.2GB/s  | 47.3GB/s   |
| 4 MiB   | 89.28 ms | +624%  | +57%  | 120.3GB/s  | 30.1GB/s   | L2, 3 MiB |
| 8 MiB   | 0.14 s   | +1048% | +59%  | 75.8GB/s   | 19.0GB/s   |
| 16 MiB  | 0.15 s   | +1132% | +7%   | 70.7GB/s   | 17.7GB/s   |
| 32 MiB  | 0.15 s   | +1151% | +2%   | 69.6GB/s   | 17.4GB/s   | L3, 24 MiB   |
| 64 MiB  | 0.16 s   | +1202% | +4%   | 66.9GB/s   | 16.7GB/s   |
| 128 MiB | 0.16 s   | +1198% | -     | 67.1GB/s   | 16.8GB/s   |

**SIMD cached fill**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|------------|--------------|
| 256 B   | 12.48 ms | +4%    | -     | 860.1GB/s  | 215.0GB/s  |
| 512 B   | 11.95 ms | -      | +4%   | 898.5GB/s  | 224.6GB/s  |
| 1 KiB   | 13.14 ms | +10%   | +10%  | 817.2GB/s  | 204.3GB/s  |
| 2 KiB   | 13.61 ms | +14%   | +4%   | 789.2GB/s  | 197.3GB/s  |
| 4 KiB   | 13.00 ms | +9%    | +4%   | 826.3GB/s  | 206.6GB/s  | page, 4 KiB  |
| 8 KiB   | 12.73 ms | +7%    | +2%   | 843.6GB/s  | 210.9GB/s  |
| 16 KiB  | 12.55 ms | +5%    | +1%   | 855.3GB/s  | 213.8GB/s  |
| 32 KiB  | 30.60 ms | +156%  | +144% | 350.9GB/s  | 87.7GB/s   |
| 64 KiB  | 30.84 ms | +158%  | +1%   | 348.1GB/s  | 87.0GB/s   | L1D, 48 KiB  |
| 128 KiB | 52.01 ms | +335%  | +69%  | 206.5GB/s  | 51.6GB/s   |
| 256 KiB | 40.26 ms | +237%  | +23%  | 266.7GB/s  | 66.7GB/s   | L1.5D, 192 KB |
| 512 KiB | 40.98 ms | +243%  | +2%   | 262.0GB/s  | 65.5GB/s   |
| 1 MiB   | 40.68 ms | +240%  | +1%   | 263.9GB/s  | 66.0GB/s   |
| 2 MiB   | 60.34 ms | +405%  | +48%  | 177.9GB/s  | 44.5GB/s   |
| 4 MiB   | 97.91 ms | +719%  | +62%  | 109.7GB/s  | 27.4GB/s   | L2, 3 MiB |
| 8 MiB   | 0.16 s   | +1270% | +67%  | 65.6GB/s   | 16.4GB/s   |
| 16 MiB  | 0.21 s   | +1658% | +28%  | 51.1GB/s   | 12.8GB/s   |
| 32 MiB  | 0.18 s   | +1415% | +14%  | 59.3GB/s   | 14.8GB/s   | L3, 24 MiB   |
| 64 MiB  | 0.17 s   | +1361% | +4%   | 61.5GB/s   | 15.4GB/s   |
| 128 MiB | 0.18 s   | +1397% | +2%   | 60.0GB/s   | 15.0GB/s   |

**SIMD non-cached fill**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|--------|--------|------|-----------|-----------|--------------|
| 256 B   | 5.72 s | +3816% | -    | 7.51GB/s  | 1.88GB/s  |
| 512 B   | 2.66 s | +1719% | +54% | 16.2GB/s  | 4.04GB/s  |
| 1 KiB   | 1.41 s | +865%  | +47% | 30.5GB/s  | 7.63GB/s  |
| 2 KiB   | 0.77 s | +430%  | +45% | 55.5GB/s  | 13.9GB/s  |
| 4 KiB   | 0.55 s | +275%  | +29% | 78.4GB/s  | 19.6GB/s  | page, 4 KiB  |
| 8 KiB   | 0.52 s | +255%  | +5%  | 82.9GB/s  | 20.7GB/s  |
| 16 KiB  | 0.54 s | +268%  | +4%  | 79.9GB/s  | 20.0GB/s  |
| 32 KiB  | 0.56 s | +283%  | +4%  | 76.8GB/s  | 19.2GB/s  |
| 64 KiB  | 0.56 s | +284%  | -    | 76.6GB/s  | 19.2GB/s  | L1D, 48 KiB  |
| 128 KiB | 0.56 s | +284%  | -    | 76.7GB/s  | 19.2GB/s  |
| 256 KiB | 0.57 s | +292%  | +2%  | 75.0GB/s  | 18.8GB/s  |
| 512 KiB | 0.58 s | +297%  | +1%  | 74.1GB/s  | 18.5GB/s  |
| 1 MiB   | 0.58 s | +294%  | +1%  | 74.6GB/s  | 18.7GB/s  |
| 2 MiB   | 0.58 s | +297%  | +1%  | 74.2GB/s  | 18.5GB/s  |
| 4 MiB   | 0.15 s | -      | +75% | 73.6GB/s  | 18.4GB/s  | L2, 3 MiB |
| 8 MiB   | 0.15 s | -      | -    | 73.4GB/s  | 18.3GB/s  |
| 16 MiB  | 0.15 s | +1%    | -    | 73.0GB/s  | 18.3GB/s  |
| 32 MiB  | 0.15 s | +1%    | -    | 72.9GB/s  | 18.2GB/s  | L3, 24 MiB   |
| 64 MiB  | 0.15 s | +2%    | +1%  | 72.0GB/s  | 18.0GB/s  |
| 128 MiB | 0.15 s | +2%    | -    | 72.4GB/s  | 18.1GB/s  |

</details>


## E-core

* Theoretical performance:<br/>
    - 32B/cy write to L1 * 4.4GHz = 141 GB/s - max speed of memset
    - 3x 16B/cy read from L1 * 4.4GHz = 211 GB/s - max speed of sum/hash
    - 64B/cy per core read from L2 * 4.4GHz = 282 GB/s
    - 128B/cy per 4 cores read from L2 * 4.4GHz = 563 GB/s

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|------------|-------------|
| 256 B   | 30.22 ms | +54%  | -     | 88.8GB/s   |
| 512 B   | 20.17 ms | +3%   | +33%  | 133.1GB/s  |
| 1 KiB   | 20.65 ms | +5%   | +2%   | 130.0GB/s  |
| 2 KiB   | 19.73 ms | +1%   | +4%   | 136.1GB/s  |
| 4 KiB   | 19.84 ms | +1%   | +1%   | 135.3GB/s  | page, 4 KiB |
| 8 KiB   | 19.58 ms | -     | +1%   | 137.1GB/s  |
| 16 KiB  | 40.94 ms | +109% | +109% | 65.6GB/s   |
| 32 KiB  | 41.92 ms | +114% | +2%   | 64.0GB/s   | L1D, 32 KiB |
| 64 KiB  | 41.35 ms | +111% | +1%   | 64.9GB/s   |
| 128 KiB | 41.96 ms | +114% | +1%   | 64.0GB/s   |
| 256 KiB | 41.74 ms | +113% | +1%   | 64.3GB/s   |
| 512 KiB | 41.73 ms | +113% | -     | 64.3GB/s   |
| 1 MiB   | 41.82 ms | +114% | -     | 64.2GB/s   |
| 2 MiB   | 0.13 s   | +546% | +202% | 21.2GB/s   |
| 4 MiB   | 0.13 s   | +545% | -     | 21.2GB/s   | L2, 4 MiB   |
| 8 MiB   | 0.13 s   | +561% | +2%   | 20.7GB/s   |
| 16 MiB  | 0.13 s   | +557% | +1%   | 20.9GB/s   |
| 32 MiB  | 0.13 s   | +563% | +1%   | 20.7GB/s   | L3, 24 MiB  |
| 64 MiB  | 0.13 s   | +569% | +1%   | 20.5GB/s   |
| 128 MiB | 0.13 s   | +578% | +1%   | 20.2GB/s   |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|-------|------------|-------------|
| 256 B   | 23.85 ms | +22%   | -     | 112.5GB/s  |
| 512 B   | 19.71 ms | +1%    | +17%  | 136.2GB/s  |
| 1 KiB   | 19.80 ms | +1%    | -     | 135.6GB/s  |
| 2 KiB   | 20.07 ms | +3%    | +1%   | 133.8GB/s  |
| 4 KiB   | 19.92 ms | +2%    | +1%   | 134.7GB/s  | page, 4 KiB |
| 8 KiB   | 19.51 ms | -      | +2%   | 137.6GB/s  |
| 16 KiB  | 20.04 ms | +3%    | +3%   | 133.9GB/s  |
| 32 KiB  | 40.56 ms | +108%  | +102% | 66.2GB/s   | L1D, 32 KiB |
| 64 KiB  | 40.38 ms | +107%  | -     | 66.5GB/s   |
| 128 KiB | 41.15 ms | +111%  | +2%   | 65.2GB/s   |
| 256 KiB | 41.32 ms | +112%  | -     | 65.0GB/s   |
| 512 KiB | 41.63 ms | +113%  | +1%   | 64.5GB/s   |
| 1 MiB   | 41.38 ms | +112%  | +1%   | 64.9GB/s   |
| 2 MiB   | 48.24 ms | +147%  | +17%  | 55.6GB/s   |
| 4 MiB   | 81.25 ms | +316%  | +68%  | 33.0GB/s   | L2, 4 MiB   |
| 8 MiB   | 85.67 ms | +339%  | +5%   | 31.3GB/s   |
| 16 MiB  | 0.23 s   | +1076% | +168% | 11.7GB/s   |
| 32 MiB  | 0.21 s   | +975%  | +9%   | 12.8GB/s   | L3, 24 MiB  |
| 64 MiB  | 0.21 s   | +984%  | +1%   | 12.7GB/s   |
| 128 MiB | 0.22 s   | +1012% | +3%   | 12.4GB/s   |

**SIMD non-cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|--------|------|-----------|-------------|
| 256 B   | 2.29 s | +1585% | -    | 1.17GB/s  |
| 512 B   | 1.35 s | +893%  | +41% | 1.99GB/s  |
| 1 KiB   | 0.72 s | +434%  | +46% | 3.71GB/s  |
| 2 KiB   | 0.54 s | +301%  | +25% | 4.93GB/s  |
| 4 KiB   | 0.49 s | +261%  | +10% | 5.48GB/s  | page, 4 KiB |
| 8 KiB   | 0.48 s | +253%  | +2%  | 5.61GB/s  |
| 16 KiB  | 0.46 s | +242%  | +3%  | 5.78GB/s  |
| 32 KiB  | 0.46 s | +236%  | +2%  | 5.89GB/s  | L1D, 32 KiB |
| 64 KiB  | 0.19 s | +41%   | +58% | 14.0GB/s  |
| 128 KiB | 0.14 s | +6%    | +25% | 18.6GB/s  |
| 256 KiB | 0.14 s | +2%    | +4%  | 19.3GB/s  |
| 512 KiB | 0.14 s | +6%    | +4%  | 18.6GB/s  |
| 1 MiB   | 0.14 s | +6%    | -    | 18.7GB/s  |
| 2 MiB   | 0.14 s | +1%    | +4%  | 19.5GB/s  |
| 4 MiB   | 0.14 s | +1%    | +1%  | 19.7GB/s  | L2, 4 MiB   |
| 8 MiB   | 0.14 s | -      | +1%  | 19.8GB/s  |
| 16 MiB  | 0.14 s | +1%    | +1%  | 19.5GB/s  |
| 32 MiB  | 0.14 s | +3%    | +2%  | 19.2GB/s  | L3, 24 MiB  |
| 64 MiB  | 0.14 s | +4%    | -    | 19.1GB/s  |
| 128 MiB | 0.14 s | +5%    | +1%  | 18.8GB/s  |

</details>

<details><summary><b>memcpy, multithreading (4 threads, 4 cores)</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|------------|-------------|
| 256 B   | 36.88 ms | +53%   | -     | 291.1GB/s  | 72.8GB/s   |
| 512 B   | 25.88 ms | +7%    | +30%  | 414.9GB/s  | 103.7GB/s  |
| 1 KiB   | 25.49 ms | +6%    | +2%   | 421.3GB/s  | 105.3GB/s  |
| 2 KiB   | 24.68 ms | +2%    | +3%   | 435.0GB/s  | 108.8GB/s  |
| 4 KiB   | 24.25 ms | +1%    | +2%   | 442.8GB/s  | 110.7GB/s  | page, 4 KiB |
| 8 KiB   | 24.09 ms | -      | +1%   | 445.7GB/s  | 111.4GB/s  |
| 16 KiB  | 46.29 ms | +92%   | +92%  | 231.9GB/s  | 58.0GB/s   |
| 32 KiB  | 86.92 ms | +261%  | +88%  | 123.5GB/s  | 30.9GB/s   | L1D, 32 KiB |
| 64 KiB  | 86.64 ms | +260%  | -     | 123.9GB/s  | 31.0GB/s   |
| 128 KiB | 86.67 ms | +260%  | -     | 123.9GB/s  | 31.0GB/s   |
| 256 KiB | 86.13 ms | +258%  | +1%   | 124.7GB/s  | 31.2GB/s   |
| 512 KiB | 99.31 ms | +312%  | +15%  | 108.1GB/s  | 27.0GB/s   |
| 1 MiB   | 0.29 s   | +1085% | +188% | 37.6GB/s   | 9.40GB/s   |
| 2 MiB   | 0.39 s   | +1534% | +38%  | 27.3GB/s   | 6.82GB/s   |
| 4 MiB   | 0.39 s   | +1529% | -     | 27.4GB/s   | 6.84GB/s   | L2, 4 MiB   |
| 8 MiB   | 0.39 s   | +1534% | -     | 27.3GB/s   | 6.82GB/s   |
| 16 MiB  | 0.39 s   | +1529% | -     | 27.4GB/s   | 6.84GB/s   |
| 32 MiB  | 0.39 s   | +1532% | -     | 27.3GB/s   | 6.83GB/s   | L3, 24 MiB  |
| 64 MiB  | 0.40 s   | +1541% | +1%   | 27.2GB/s   | 6.79GB/s   |
| 128 MiB | 0.40 s   | +1549% | +1%   | 27.0GB/s   | 6.76GB/s   |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|------------|-------------|
| 256 B   | 28.15 ms | +18%   | -     | 381.4GB/s  | 95.3GB/s   |
| 512 B   | 24.18 ms | +1%    | +14%  | 444.1GB/s  | 111.0GB/s  |
| 1 KiB   | 24.30 ms | +2%    | +1%   | 441.9GB/s  | 110.5GB/s  |
| 2 KiB   | 23.83 ms | -      | +2%   | 450.6GB/s  | 112.6GB/s  |
| 4 KiB   | 24.04 ms | +1%    | +1%   | 446.7GB/s  | 111.7GB/s  | page, 4 KiB |
| 8 KiB   | 24.81 ms | +4%    | +3%   | 432.7GB/s  | 108.2GB/s  |
| 16 KiB  | 31.52 ms | +32%   | +27%  | 340.6GB/s  | 85.2GB/s   |
| 32 KiB  | 91.14 ms | +282%  | +189% | 117.8GB/s  | 29.5GB/s   | L1D, 32 KiB |
| 64 KiB  | 87.53 ms | +267%  | +4%   | 122.7GB/s  | 30.7GB/s   |
| 128 KiB | 88.33 ms | +271%  | +1%   | 121.6GB/s  | 30.4GB/s   |
| 256 KiB | 86.89 ms | +265%  | +2%   | 123.6GB/s  | 30.9GB/s   |
| 512 KiB | 0.10 s   | +323%  | +16%  | 106.6GB/s  | 26.6GB/s   |
| 1 MiB   | 0.28 s   | +1081% | +179% | 38.2GB/s   | 9.54GB/s   |
| 2 MiB   | 0.28 s   | +1071% | +1%   | 38.5GB/s   | 9.62GB/s   |
| 4 MiB   | 0.30 s   | +1156% | +7%   | 35.9GB/s   | 8.97GB/s   | L2, 4 MiB   |
| 8 MiB   | 0.56 s   | +2238% | +86%  | 19.3GB/s   | 4.82GB/s   |
| 16 MiB  | 0.64 s   | +2567% | +14%  | 16.9GB/s   | 4.22GB/s   |
| 32 MiB  | 0.69 s   | +2803% | +9%   | 15.5GB/s   | 3.88GB/s   | L3, 24 MiB  |
| 64 MiB  | 0.69 s   | +2801% | -     | 15.5GB/s   | 3.88GB/s   |
| 128 MiB | 0.68 s   | +2760% | +1%   | 15.8GB/s   | 3.94GB/s   |

**SIMD non-cached copy**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|--------|--------|------|-----------|-----------|-------------|
| 256 B   | 9.22 s | +2244% | -    | 4.66GB/s  | 1.16GB/s  |
| 512 B   | 4.86 s | +1135% | +47% | 8.84GB/s  | 2.21GB/s  |
| 1 KiB   | 2.90 s | +638%  | +40% | 14.8GB/s  | 3.70GB/s  |
| 2 KiB   | 2.51 s | +538%  | +14% | 17.1GB/s  | 4.28GB/s  |
| 4 KiB   | 2.39 s | +506%  | +5%  | 18.0GB/s  | 4.50GB/s  | page, 4 KiB |
| 8 KiB   | 2.21 s | +461%  | +7%  | 19.4GB/s  | 4.86GB/s  |
| 16 KiB  | 1.96 s | +399%  | +11% | 21.9GB/s  | 5.47GB/s  |
| 32 KiB  | 1.74 s | +343%  | +11% | 24.7GB/s  | 6.16GB/s  | L1D, 32 KiB |
| 64 KiB  | 1.64 s | +316%  | +6%  | 26.2GB/s  | 6.56GB/s  |
| 128 KiB | 1.63 s | +314%  | -    | 26.4GB/s  | 6.59GB/s  |
| 256 KiB | 1.61 s | +309%  | +1%  | 26.7GB/s  | 6.68GB/s  |
| 512 KiB | 1.59 s | +305%  | +1%  | 27.0GB/s  | 6.74GB/s  |
| 1 MiB   | 1.60 s | +308%  | +1%  | 26.8GB/s  | 6.69GB/s  |
| 2 MiB   | 1.58 s | +302%  | +1%  | 27.1GB/s  | 6.78GB/s  |
| 4 MiB   | 0.39 s | -      | +75% | 27.2GB/s  | 6.80GB/s  | L2, 4 MiB   |
| 8 MiB   | 0.40 s | +1%    | -    | 27.1GB/s  | 6.77GB/s  |
| 16 MiB  | 0.39 s | -      | +1%  | 27.3GB/s  | 6.82GB/s  |
| 32 MiB  | 0.40 s | +1%    | +1%  | 27.0GB/s  | 6.74GB/s  | L3, 24 MiB  |
| 64 MiB  | 0.40 s | +1%    | -    | 27.0GB/s  | 6.75GB/s  |
| 128 MiB | 0.40 s | +1%    | -    | 27.0GB/s  | 6.76GB/s  |

</details>

<details><summary><b>memset, single thread</b></summary>

**memset**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|------------|-------------|
| 256 B   | 22.27 ms | +16%  | -     | 120.6GB/s  |
| 512 B   | 19.81 ms | +3%   | +11%  | 135.5GB/s  |
| 1 KiB   | 19.56 ms | +2%   | +1%   | 137.2GB/s  |
| 2 KiB   | 19.36 ms | +1%   | +1%   | 138.7GB/s  |
| 4 KiB   | 19.43 ms | +1%   | -     | 138.1GB/s  | page, 4 KiB |
| 8 KiB   | 19.29 ms | -     | +1%   | 139.2GB/s  |
| 16 KiB  | 19.20 ms | -     | -     | 139.8GB/s  |
| 32 KiB  | 20.43 ms | +6%   | +6%   | 131.4GB/s  | L1D, 32 KiB |
| 64 KiB  | 39.08 ms | +104% | +91%  | 68.7GB/s   |
| 128 KiB | 38.80 ms | +102% | +1%   | 69.2GB/s   |
| 256 KiB | 38.68 ms | +101% | -     | 69.4GB/s   |
| 512 KiB | 38.50 ms | +101% | -     | 69.7GB/s   |
| 1 MiB   | 39.31 ms | +105% | +2%   | 68.3GB/s   |
| 2 MiB   | 38.48 ms | +100% | +2%   | 69.8GB/s   |
| 4 MiB   | 39.76 ms | +107% | +3%   | 67.5GB/s   | L2, 4 MiB   |
| 8 MiB   | 52.89 ms | +175% | +33%  | 50.8GB/s   |
| 16 MiB  | 55.73 ms | +190% | +5%   | 48.2GB/s   |
| 32 MiB  | 0.13 s   | +555% | +126% | 21.4GB/s   | L3, 24 MiB  |
| 64 MiB  | 0.18 s   | +840% | +44%  | 14.9GB/s   |
| 128 MiB | 0.19 s   | +871% | +3%   | 14.4GB/s   |

**SIMD cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|------------|-------------|
| 256 B   | 19.24 ms | -     | -     | 139.5GB/s  |
| 512 B   | 19.20 ms | -     | -     | 139.8GB/s  |
| 1 KiB   | 19.26 ms | -     | -     | 139.4GB/s  |
| 2 KiB   | 19.29 ms | -     | -     | 139.2GB/s  |
| 4 KiB   | 19.32 ms | +1%   | -     | 138.9GB/s  | page, 4 KiB |
| 8 KiB   | 19.19 ms | -     | +1%   | 139.8GB/s  |
| 16 KiB  | 19.29 ms | -     | -     | 139.2GB/s  |
| 32 KiB  | 19.33 ms | +1%   | -     | 138.8GB/s  | L1D, 32 KiB |
| 64 KiB  | 28.83 ms | +50%  | +49%  | 93.1GB/s   |
| 128 KiB | 28.98 ms | +51%  | +1%   | 92.6GB/s   |
| 256 KiB | 29.47 ms | +54%  | +2%   | 91.1GB/s   |
| 512 KiB | 29.48 ms | +54%  | -     | 91.1GB/s   |
| 1 MiB   | 29.50 ms | +54%  | -     | 91.0GB/s   |
| 2 MiB   | 29.56 ms | +54%  | -     | 90.8GB/s   |
| 4 MiB   | 34.35 ms | +79%  | +16%  | 78.1GB/s   | L2, 4 MiB   |
| 8 MiB   | 52.69 ms | +175% | +53%  | 50.9GB/s   |
| 16 MiB  | 56.22 ms | +193% | +7%   | 47.7GB/s   |
| 32 MiB  | 0.12 s   | +550% | +122% | 21.5GB/s   | L3, 24 MiB  |
| 64 MiB  | 0.18 s   | +846% | +46%  | 14.8GB/s   |
| 128 MiB | 0.19 s   | +888% | +4%   | 14.2GB/s   |

**SIMD non-cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|------|-----------|-------------|
| 256 B   | 1.36 s   | +1313% | -    | 1.98GB/s  |
| 512 B   | 0.71 s   | +636%  | +48% | 3.80GB/s  |
| 1 KiB   | 0.41 s   | +325%  | +42% | 6.57GB/s  |
| 2 KiB   | 0.25 s   | +156%  | +40% | 10.9GB/s  |
| 4 KiB   | 0.17 s   | +73%   | +32% | 16.1GB/s  | page, 4 KiB |
| 8 KiB   | 0.13 s   | +39%   | +20% | 20.1GB/s  |
| 16 KiB  | 0.12 s   | +20%   | +13% | 23.2GB/s  |
| 32 KiB  | 0.11 s   | +10%   | +9%  | 25.5GB/s  | L1D, 32 KiB |
| 64 KiB  | 0.10 s   | +5%    | +5%  | 26.7GB/s  |
| 128 KiB | 98.74 ms | +3%    | +2%  | 27.2GB/s  |
| 256 KiB | 97.17 ms | +1%    | +2%  | 27.6GB/s  |
| 512 KiB | 96.91 ms | +1%    | -    | 27.7GB/s  |
| 1 MiB   | 96.51 ms | -      | -    | 27.8GB/s  |
| 2 MiB   | 96.06 ms | -      | -    | 27.9GB/s  |
| 4 MiB   | 96.54 ms | +1%    | -    | 27.8GB/s  | L2, 4 MiB   |
| 8 MiB   | 96.07 ms | -      | -    | 27.9GB/s  |
| 16 MiB  | 96.21 ms | -      | -    | 27.9GB/s  |
| 32 MiB  | 96.53 ms | -      | -    | 27.8GB/s  | L3, 24 MiB  |
| 64 MiB  | 96.05 ms | -      | -    | 27.9GB/s  |
| 128 MiB | 96.28 ms | -      | -    | 27.9GB/s  |

</details>

<details><summary><b>memset, multithreading (4 threads, 4 cores)</b></summary>

**memset**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|------------|-------------|
| 256 B   | 26.39 ms | +14%   | -     | 406.9GB/s  | 101.7GB/s  |
| 512 B   | 32.62 ms | +40%   | +24%  | 329.2GB/s  | 82.3GB/s   |
| 1 KiB   | 23.50 ms | +1%    | +28%  | 456.8GB/s  | 114.2GB/s  |
| 2 KiB   | 23.93 ms | +3%    | +2%   | 448.6GB/s  | 112.2GB/s  |
| 4 KiB   | 23.42 ms | +1%    | +2%   | 458.4GB/s  | 114.6GB/s  | page, 4 KiB |
| 8 KiB   | 23.22 ms | -      | +1%   | 462.3GB/s  | 115.6GB/s  |
| 16 KiB  | 23.27 ms | -      | -     | 461.5GB/s  | 115.4GB/s  |
| 32 KiB  | 63.99 ms | +176%  | +175% | 167.8GB/s  | 41.9GB/s   | L1D, 32 KiB |
| 64 KiB  | 63.21 ms | +172%  | +1%   | 169.9GB/s  | 42.5GB/s   |
| 128 KiB | 63.64 ms | +174%  | +1%   | 168.7GB/s  | 42.2GB/s   |
| 256 KiB | 63.24 ms | +172%  | +1%   | 169.8GB/s  | 42.4GB/s   |
| 512 KiB | 70.79 ms | +205%  | +12%  | 151.7GB/s  | 37.9GB/s   |
| 1 MiB   | 0.13 s   | +474%  | +88%  | 80.6GB/s   | 20.1GB/s   |
| 2 MiB   | 0.13 s   | +479%  | +1%   | 79.9GB/s   | 20.0GB/s   |
| 4 MiB   | 0.14 s   | +522%  | +8%   | 74.3GB/s   | 18.6GB/s   | L2, 4 MiB   |
| 8 MiB   | 0.31 s   | +1253% | +117% | 34.2GB/s   | 8.54GB/s   |
| 16 MiB  | 0.38 s   | +1540% | +21%  | 28.2GB/s   | 7.05GB/s   |
| 32 MiB  | 0.39 s   | +1592% | +3%   | 27.3GB/s   | 6.83GB/s   | L3, 24 MiB  |
| 64 MiB  | 0.40 s   | +1638% | +3%   | 26.6GB/s   | 6.65GB/s   |
| 128 MiB | 0.40 s   | +1623% | +1%   | 26.8GB/s   | 6.71GB/s   |

**SIMD cached fill**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|------------|-------------|
| 256 B   | 23.68 ms | +3%    | -     | 453.4GB/s  | 113.3GB/s  |
| 512 B   | 23.28 ms | +2%    | +2%   | 461.3GB/s  | 115.3GB/s  |
| 1 KiB   | 23.67 ms | +3%    | +2%   | 453.6GB/s  | 113.4GB/s  |
| 2 KiB   | 23.54 ms | +3%    | +1%   | 456.1GB/s  | 114.0GB/s  |
| 4 KiB   | 22.91 ms | -      | +3%   | 468.7GB/s  | 117.2GB/s  | page, 4 KiB |
| 8 KiB   | 23.71 ms | +3%    | +3%   | 452.9GB/s  | 113.2GB/s  |
| 16 KiB  | 24.17 ms | +5%    | +2%   | 444.3GB/s  | 111.1GB/s  |
| 32 KiB  | 63.48 ms | +177%  | +163% | 169.1GB/s  | 42.3GB/s   | L1D, 32 KiB |
| 64 KiB  | 63.68 ms | +178%  | -     | 168.6GB/s  | 42.2GB/s   |
| 128 KiB | 64.19 ms | +180%  | +1%   | 167.3GB/s  | 41.8GB/s   |
| 256 KiB | 63.19 ms | +176%  | +2%   | 169.9GB/s  | 42.5GB/s   |
| 512 KiB | 73.48 ms | +221%  | +16%  | 146.1GB/s  | 36.5GB/s   |
| 1 MiB   | 0.13 s   | +481%  | +81%  | 80.7GB/s   | 20.2GB/s   |
| 2 MiB   | 0.13 s   | +480%  | -     | 80.8GB/s   | 20.2GB/s   |
| 4 MiB   | 0.14 s   | +526%  | +8%   | 74.9GB/s   | 18.7GB/s   | L2, 4 MiB   |
| 8 MiB   | 0.30 s   | +1229% | +112% | 35.3GB/s   | 8.82GB/s   |
| 16 MiB  | 0.36 s   | +1490% | +20%  | 29.5GB/s   | 7.37GB/s   |
| 32 MiB  | 0.39 s   | +1589% | +6%   | 27.8GB/s   | 6.94GB/s   | L3, 24 MiB  |
| 64 MiB  | 0.39 s   | +1621% | +2%   | 27.2GB/s   | 6.81GB/s   |
| 128 MiB | 0.40 s   | +1633% | +1%   | 27.0GB/s   | 6.76GB/s   |

**SIMD non-cached fill**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|--------|--------|------|-----------|-----------|-------------|
| 256 B   | 5.93 s | +3941% | -    | 7.25GB/s  | 1.81GB/s  |
| 512 B   | 2.85 s | +1843% | +52% | 15.1GB/s  | 3.77GB/s  |
| 1 KiB   | 1.66 s | +1032% | +42% | 25.9GB/s  | 6.47GB/s  |
| 2 KiB   | 1.00 s | +582%  | +40% | 42.9GB/s  | 10.7GB/s  |
| 4 KiB   | 0.76 s | +417%  | +24% | 56.6GB/s  | 14.1GB/s  | page, 4 KiB |
| 8 KiB   | 0.62 s | +325%  | +18% | 68.9GB/s  | 17.2GB/s  |
| 16 KiB  | 0.61 s | +316%  | +2%  | 70.4GB/s  | 17.6GB/s  |
| 32 KiB  | 0.58 s | +294%  | +5%  | 74.4GB/s  | 18.6GB/s  | L1D, 32 KiB |
| 64 KiB  | 0.56 s | +285%  | +2%  | 76.1GB/s  | 19.0GB/s  |
| 128 KiB | 0.57 s | +290%  | +1%  | 75.0GB/s  | 18.8GB/s  |
| 256 KiB | 0.58 s | +297%  | +2%  | 73.7GB/s  | 18.4GB/s  |
| 512 KiB | 0.59 s | +301%  | +1%  | 72.9GB/s  | 18.2GB/s  |
| 1 MiB   | 0.58 s | +296%  | +1%  | 73.9GB/s  | 18.5GB/s  |
| 2 MiB   | 0.58 s | +298%  | +1%  | 73.5GB/s  | 18.4GB/s  |
| 4 MiB   | 0.15 s | -      | +75% | 73.0GB/s  | 18.3GB/s  | L2, 4 MiB   |
| 8 MiB   | 0.15 s | -      | -    | 73.2GB/s  | 18.3GB/s  |
| 16 MiB  | 0.15 s | +3%    | +3%  | 70.8GB/s  | 17.7GB/s  |
| 32 MiB  | 0.15 s | +4%    | +1%  | 70.4GB/s  | 17.6GB/s  | L3, 24 MiB  |
| 64 MiB  | 0.15 s | +3%    | +1%  | 71.2GB/s  | 17.8GB/s  |
| 128 MiB | 0.15 s | +6%    | +3%  | 69.3GB/s  | 17.3GB/s  |

</details>


## LP E-core

* Theoretical performance:<br/>
  32B/cy write to L1 * 2.5GHz = 80 GB/s - max speed of memset<br/>
  64B/cy read from L1 * 2.5GHz = 160 GB/s - max speed of search<br/>

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|-------|-----------|-------------|
| 256 B   | 69.10 ms | +89%   | -     | 38.8GB/s  |
| 512 B   | 47.18 ms | +29%   | +32%  | 56.9GB/s  |
| 1 KiB   | 43.04 ms | +18%   | +9%   | 62.4GB/s  |
| 2 KiB   | 39.60 ms | +8%    | +8%   | 67.8GB/s  |
| 4 KiB   | 37.11 ms | +1%    | +6%   | 72.3GB/s  | page, 4 KiB |
| 8 KiB   | 36.60 ms | -      | +1%   | 73.3GB/s  |
| 16 KiB  | 71.97 ms | +97%   | +97%  | 37.3GB/s  |
| 32 KiB  | 0.10 s   | +183%  | +44%  | 25.9GB/s  | L1D, 32 KiB |
| 64 KiB  | 0.10 s   | +182%  | -     | 26.0GB/s  |
| 128 KiB | 0.10 s   | +180%  | +1%   | 26.2GB/s  |
| 256 KiB | 0.10 s   | +179%  | -     | 26.3GB/s  |
| 512 KiB | 0.10 s   | +179%  | -     | 26.3GB/s  |
| 1 MiB   | 0.23 s   | +529%  | +126% | 11.7GB/s  |
| 2 MiB   | 0.40 s   | +992%  | +74%  | 6.72GB/s  | L2, 2 MiB   |
| 4 MiB   | 0.41 s   | +1027% | +3%   | 6.51GB/s  |
| 8 MiB   | 0.43 s   | +1084% | +5%   | 6.20GB/s  |
| 16 MiB  | 0.43 s   | +1061% | +2%   | 6.32GB/s  |
| 32 MiB  | 0.42 s   | +1038% | +2%   | 6.45GB/s  |
| 64 MiB  | 0.43 s   | +1086% | +4%   | 6.18GB/s  |
| 128 MiB | 0.42 s   | +1044% | +4%   | 6.41GB/s  |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|-------|-----------|-------------|
| 256 B   | 38.13 ms | +10%   | -     | 70.4GB/s  |
| 512 B   | 36.71 ms | +6%    | +4%   | 73.1GB/s  |
| 1 KiB   | 35.55 ms | +3%    | +3%   | 75.5GB/s  |
| 2 KiB   | 34.63 ms | -      | +3%   | 77.5GB/s  |
| 4 KiB   | 34.83 ms | +1%    | +1%   | 77.1GB/s  | page, 4 KiB |
| 8 KiB   | 35.13 ms | +1%    | +1%   | 76.4GB/s  |
| 16 KiB  | 39.65 ms | +14%   | +13%  | 67.7GB/s  |
| 32 KiB  | 0.10 s   | +196%  | +159% | 26.2GB/s  | L1D, 32 KiB |
| 64 KiB  | 0.10 s   | +195%  | -     | 26.2GB/s  |
| 128 KiB | 0.10 s   | +195%  | -     | 26.2GB/s  |
| 256 KiB | 0.10 s   | +195%  | -     | 26.2GB/s  |
| 512 KiB | 0.10 s   | +195%  | -     | 26.3GB/s  |
| 1 MiB   | 0.23 s   | +570%  | +127% | 11.6GB/s  |
| 2 MiB   | 0.46 s   | +1227% | +98%  | 5.84GB/s  | L2, 2 MiB   |
| 4 MiB   | 0.44 s   | +1156% | +5%   | 6.17GB/s  |
| 8 MiB   | 0.51 s   | +1377% | +18%  | 5.25GB/s  |
| 16 MiB  | 0.49 s   | +1302% | +5%   | 5.53GB/s  |
| 32 MiB  | 0.48 s   | +1299% | -     | 5.54GB/s  |
| 64 MiB  | 0.48 s   | +1292% | -     | 5.57GB/s  |
| 128 MiB | 0.49 s   | +1316% | +2%   | 5.47GB/s  |

**SIMD non-cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|-------|------|------------|-------------|
| 256 B   | 3.66 s | +859% | -    | 733.0MB/s  |
| 512 B   | 1.88 s | +393% | +49% | 1.43GB/s   |
| 1 KiB   | 1.21 s | +217% | +36% | 2.22GB/s   |
| 2 KiB   | 1.03 s | +169% | +15% | 2.61GB/s   |
| 4 KiB   | 1.04 s | +172% | +1%  | 2.58GB/s   | page, 4 KiB |
| 8 KiB   | 0.98 s | +157% | +6%  | 2.74GB/s   |
| 16 KiB  | 0.96 s | +153% | +2%  | 2.78GB/s   |
| 32 KiB  | 0.93 s | +143% | +4%  | 2.89GB/s   | L1D, 32 KiB |
| 64 KiB  | 0.92 s | +141% | +1%  | 2.92GB/s   |
| 128 KiB | 0.39 s | +2%   | +58% | 6.90GB/s   |
| 256 KiB | 0.41 s | +6%   | +4%  | 6.63GB/s   |
| 512 KiB | 0.41 s | +8%   | +2%  | 6.49GB/s   |
| 1 MiB   | 0.41 s | +9%   | -    | 6.47GB/s   |
| 2 MiB   | 0.41 s | +8%   | +1%  | 6.53GB/s   | L2, 2 MiB   |
| 4 MiB   | 0.40 s | +6%   | +2%  | 6.64GB/s   |
| 8 MiB   | 0.41 s | +7%   | +1%  | 6.58GB/s   |
| 16 MiB  | 0.39 s | +3%   | +4%  | 6.84GB/s   |
| 32 MiB  | 0.41 s | +7%   | +4%  | 6.58GB/s   |
| 64 MiB  | 0.38 s | -     | +6%  | 7.03GB/s   |
| 128 MiB | 0.40 s | +5%   | +5%  | 6.66GB/s   |

</details>

<details><summary><b>memcpy, multithreading (2 threads, 2 cores)</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|-----------|-------------|
| 256 B   | 87.97 ms | +128%  | -     | 61.0GB/s   | 30.5GB/s  |
| 512 B   | 70.43 ms | +82%   | +20%  | 76.2GB/s   | 38.1GB/s  |
| 1 KiB   | 50.56 ms | +31%   | +28%  | 106.2GB/s  | 53.1GB/s  |
| 2 KiB   | 56.93 ms | +47%   | +13%  | 94.3GB/s   | 47.1GB/s  |
| 4 KiB   | 41.31 ms | +7%    | +27%  | 130.0GB/s  | 65.0GB/s  | page, 4 KiB |
| 8 KiB   | 38.61 ms | -      | +7%   | 139.1GB/s  | 69.5GB/s  |
| 16 KiB  | 75.80 ms | +96%   | +96%  | 70.8GB/s   | 35.4GB/s  |
| 32 KiB  | 0.16 s   | +309%  | +108% | 34.0GB/s   | 17.0GB/s  | L1D, 32 KiB |
| 64 KiB  | 0.16 s   | +319%  | +2%   | 33.2GB/s   | 16.6GB/s  |
| 128 KiB | 0.16 s   | +309%  | +2%   | 34.0GB/s   | 17.0GB/s  |
| 256 KiB | 0.16 s   | +311%  | +1%   | 33.8GB/s   | 16.9GB/s  |
| 512 KiB | 0.22 s   | +461%  | +36%  | 24.8GB/s   | 12.4GB/s  |
| 1 MiB   | 0.57 s   | +1382% | +164% | 9.38GB/s   | 4.69GB/s  |
| 2 MiB   | 0.37 s   | +871%  | +35%  | 14.3GB/s   | 7.16GB/s  | L2, 2 MiB   |
| 4 MiB   | 0.37 s   | +866%  | +1%   | 14.4GB/s   | 7.20GB/s  |
| 8 MiB   | 0.38 s   | +890%  | +2%   | 14.1GB/s   | 7.03GB/s  |
| 16 MiB  | 0.38 s   | +882%  | +1%   | 14.2GB/s   | 7.08GB/s  |
| 32 MiB  | 0.39 s   | +905%  | +2%   | 13.8GB/s   | 6.92GB/s  |
| 64 MiB  | 0.38 s   | +887%  | +2%   | 14.1GB/s   | 7.05GB/s  |
| 128 MiB | 0.39 s   | +911%  | +2%   | 13.8GB/s   | 6.88GB/s  |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|-----------|-------------|
| 256 B   | 56.33 ms | +47%   | -     | 95.3GB/s   | 47.7GB/s  |
| 512 B   | 47.25 ms | +23%   | +16%  | 113.6GB/s  | 56.8GB/s  |
| 1 KiB   | 41.68 ms | +9%    | +12%  | 128.8GB/s  | 64.4GB/s  |
| 2 KiB   | 38.37 ms | -      | +8%   | 139.9GB/s  | 70.0GB/s  |
| 4 KiB   | 39.80 ms | +4%    | +4%   | 134.9GB/s  | 67.5GB/s  | page, 4 KiB |
| 8 KiB   | 38.29 ms | -      | +4%   | 140.2GB/s  | 70.1GB/s  |
| 16 KiB  | 47.97 ms | +25%   | +25%  | 111.9GB/s  | 56.0GB/s  |
| 32 KiB  | 0.16 s   | +323%  | +237% | 33.2GB/s   | 16.6GB/s  | L1D, 32 KiB |
| 64 KiB  | 0.16 s   | +314%  | +2%   | 33.9GB/s   | 16.9GB/s  |
| 128 KiB | 0.16 s   | +316%  | +1%   | 33.7GB/s   | 16.8GB/s  |
| 256 KiB | 0.16 s   | +315%  | -     | 33.8GB/s   | 16.9GB/s  |
| 512 KiB | 0.29 s   | +660%  | +83%  | 18.5GB/s   | 9.23GB/s  |
| 1 MiB   | 0.59 s   | +1429% | +101% | 9.17GB/s   | 4.59GB/s  |
| 2 MiB   | 0.61 s   | +1497% | +4%   | 8.78GB/s   | 4.39GB/s  | L2, 2 MiB   |
| 4 MiB   | 0.61 s   | +1502% | -     | 8.75GB/s   | 4.38GB/s  |
| 8 MiB   | 0.61 s   | +1503% | -     | 8.75GB/s   | 4.37GB/s  |
| 16 MiB  | 0.61 s   | +1491% | +1%   | 8.81GB/s   | 4.41GB/s  |
| 32 MiB  | 0.61 s   | +1504% | +1%   | 8.74GB/s   | 4.37GB/s  |
| 64 MiB  | 0.61 s   | +1504% | -     | 8.74GB/s   | 4.37GB/s  |
| 128 MiB | 0.61 s   | +1505% | -     | 8.74GB/s   | 4.37GB/s  |

**SIMD non-cached copy**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|---------|--------|------|-----------|------------|-------------|
| 256 B   | 14.56 s | +3867% | -    | 1.47GB/s  | 737.4MB/s  |
| 512 B   | 8.10 s  | +2105% | +44% | 2.65GB/s  | 1.33GB/s   |
| 1 KiB   | 5.24 s  | +1328% | +35% | 4.10GB/s  | 2.05GB/s   |
| 2 KiB   | 4.25 s  | +1057% | +19% | 5.06GB/s  | 2.53GB/s   |
| 4 KiB   | 4.52 s  | +1130% | +6%  | 4.75GB/s  | 2.38GB/s   | page, 4 KiB |
| 8 KiB   | 4.07 s  | +1009% | +10% | 5.27GB/s  | 2.64GB/s   |
| 16 KiB  | 4.56 s  | +1141% | +12% | 4.71GB/s  | 2.36GB/s   |
| 32 KiB  | 4.16 s  | +1033% | +9%  | 5.16GB/s  | 2.58GB/s   | L1D, 32 KiB |
| 64 KiB  | 1.63 s  | +345%  | +61% | 13.1GB/s  | 6.57GB/s   |
| 128 KiB | 1.61 s  | +340%  | +1%  | 13.3GB/s  | 6.65GB/s   |
| 256 KiB | 1.61 s  | +339%  | -    | 13.3GB/s  | 6.66GB/s   |
| 512 KiB | 1.62 s  | +342%  | +1%  | 13.2GB/s  | 6.61GB/s   |
| 1 MiB   | 1.57 s  | +328%  | +3%  | 13.7GB/s  | 6.83GB/s   |
| 2 MiB   | 1.50 s  | +308%  | +5%  | 14.4GB/s  | 7.18GB/s   | L2, 2 MiB   |
| 4 MiB   | 0.38 s  | +2%    | +75% | 14.3GB/s  | 7.14GB/s   |
| 8 MiB   | 0.37 s  | -      | +2%  | 14.6GB/s  | 7.31GB/s   |
| 16 MiB  | 0.38 s  | +4%    | +4%  | 14.1GB/s  | 7.04GB/s   |
| 32 MiB  | 0.37 s  | +1%    | +3%  | 14.5GB/s  | 7.26GB/s   |
| 64 MiB  | 0.37 s  | +1%    | +1%  | 14.4GB/s  | 7.21GB/s   |
| 128 MiB | 0.37 s  | +1%    | +1%  | 14.5GB/s  | 7.25GB/s   |

</details>

<details><summary><b>memset, single thread</b></summary>

**memset**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|-----------|-------------|
| 256 B   | 45.42 ms | +32%  | -     | 59.1GB/s  |
| 512 B   | 41.99 ms | +22%  | +8%   | 63.9GB/s  |
| 1 KiB   | 41.48 ms | +21%  | +1%   | 64.7GB/s  |
| 2 KiB   | 38.03 ms | +11%  | +8%   | 70.6GB/s  |
| 4 KiB   | 35.98 ms | +5%   | +5%   | 74.6GB/s  | page, 4 KiB |
| 8 KiB   | 34.99 ms | +2%   | +3%   | 76.7GB/s  |
| 16 KiB  | 34.39 ms | -     | +2%   | 78.1GB/s  |
| 32 KiB  | 37.63 ms | +9%   | +9%   | 71.3GB/s  | L1D, 32 KiB |
| 64 KiB  | 82.44 ms | +140% | +119% | 32.6GB/s  |
| 128 KiB | 82.36 ms | +140% | -     | 32.6GB/s  |
| 256 KiB | 82.18 ms | +139% | -     | 32.7GB/s  |
| 512 KiB | 82.17 ms | +139% | -     | 32.7GB/s  |
| 1 MiB   | 82.57 ms | +140% | -     | 32.5GB/s  |
| 2 MiB   | 0.12 s   | +238% | +41%  | 23.1GB/s  | L2, 2 MiB   |
| 4 MiB   | 0.32 s   | +832% | +175% | 8.38GB/s  |
| 8 MiB   | 0.33 s   | +857% | +3%   | 8.16GB/s  |
| 16 MiB  | 0.34 s   | +889% | +3%   | 7.89GB/s  |
| 32 MiB  | 0.33 s   | +856% | +3%   | 8.17GB/s  |
| 64 MiB  | 0.33 s   | +864% | +1%   | 8.10GB/s  |
| 128 MiB | 0.33 s   | +848% | +2%   | 8.24GB/s  |

**SIMD cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|-----------|-------------|
| 256 B   | 33.79 ms | -     | -     | 79.4GB/s  |
| 512 B   | 33.81 ms | -     | -     | 79.4GB/s  |
| 1 KiB   | 33.81 ms | -     | -     | 79.4GB/s  |
| 2 KiB   | 33.84 ms | -     | -     | 79.3GB/s  |
| 4 KiB   | 33.81 ms | -     | -     | 79.4GB/s  | page, 4 KiB |
| 8 KiB   | 34.60 ms | +2%   | +2%   | 77.6GB/s  |
| 16 KiB  | 34.66 ms | +3%   | -     | 77.4GB/s  |
| 32 KiB  | 34.19 ms | +1%   | +1%   | 78.5GB/s  | L1D, 32 KiB |
| 64 KiB  | 81.99 ms | +143% | +140% | 32.7GB/s  |
| 128 KiB | 82.04 ms | +143% | -     | 32.7GB/s  |
| 256 KiB | 82.08 ms | +143% | -     | 32.7GB/s  |
| 512 KiB | 82.03 ms | +143% | -     | 32.7GB/s  |
| 1 MiB   | 82.46 ms | +144% | +1%   | 32.6GB/s  |
| 2 MiB   | 0.11 s   | +233% | +36%  | 23.9GB/s  | L2, 2 MiB   |
| 4 MiB   | 0.32 s   | +862% | +189% | 8.26GB/s  |
| 8 MiB   | 0.33 s   | +868% | +1%   | 8.20GB/s  |
| 16 MiB  | 0.34 s   | +905% | +4%   | 7.90GB/s  |
| 32 MiB  | 0.33 s   | +878% | +3%   | 8.12GB/s  |
| 64 MiB  | 0.35 s   | +924% | +5%   | 7.76GB/s  |
| 128 MiB | 0.35 s   | +924% | -     | 7.76GB/s  |

**SIMD non-cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|-------|------|-----------|-------------|
| 256 B   | 1.97 s | +959% | -    | 1.36GB/s  |
| 512 B   | 0.83 s | +348% | +58% | 3.23GB/s  |
| 1 KiB   | 0.50 s | +171% | +40% | 5.34GB/s  |
| 2 KiB   | 0.42 s | +127% | +16% | 6.37GB/s  |
| 4 KiB   | 0.28 s | +52%  | +33% | 9.54GB/s  | page, 4 KiB |
| 8 KiB   | 0.24 s | +31%  | +14% | 11.0GB/s  |
| 16 KiB  | 0.21 s | +14%  | +13% | 12.6GB/s  |
| 32 KiB  | 0.20 s | +6%   | +7%  | 13.6GB/s  | L1D, 32 KiB |
| 64 KiB  | 0.19 s | +3%   | +3%  | 14.0GB/s  |
| 128 KiB | 0.19 s | +2%   | +2%  | 14.2GB/s  |
| 256 KiB | 0.19 s | +1%   | +1%  | 14.3GB/s  |
| 512 KiB | 0.19 s | -     | -    | 14.4GB/s  |
| 1 MiB   | 0.19 s | -     | -    | 14.4GB/s  |
| 2 MiB   | 0.19 s | -     | -    | 14.4GB/s  | L2, 2 MiB   |
| 4 MiB   | 0.19 s | -     | -    | 14.5GB/s  |
| 8 MiB   | 0.19 s | -     | -    | 14.5GB/s  |
| 16 MiB  | 0.19 s | -     | -    | 14.4GB/s  |
| 32 MiB  | 0.19 s | -     | -    | 14.4GB/s  |
| 64 MiB  | 0.19 s | -     | -    | 14.4GB/s  |
| 128 MiB | 0.19 s | -     | -    | 14.4GB/s  |

</details>

<details><summary><b>memset, multithreading (2 threads, 2 cores)</b></summary>

**memset**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|-------|-------|------------|-----------|-------------|
| 256 B   | 53.25 ms | +32%  | -     | 100.8GB/s  | 50.4GB/s  |
| 512 B   | 44.51 ms | +10%  | +16%  | 120.6GB/s  | 60.3GB/s  |
| 1 KiB   | 41.36 ms | +3%   | +7%   | 129.8GB/s  | 64.9GB/s  |
| 2 KiB   | 41.27 ms | +2%   | -     | 130.1GB/s  | 65.0GB/s  |
| 4 KiB   | 40.83 ms | +1%   | +1%   | 131.5GB/s  | 65.8GB/s  | page, 4 KiB |
| 8 KiB   | 42.72 ms | +6%   | +5%   | 125.7GB/s  | 62.8GB/s  |
| 16 KiB  | 40.32 ms | -     | +6%   | 133.2GB/s  | 66.6GB/s  |
| 32 KiB  | 0.14 s   | +239% | +239% | 39.3GB/s   | 19.6GB/s  | L1D, 32 KiB |
| 64 KiB  | 0.13 s   | +233% | +2%   | 40.0GB/s   | 20.0GB/s  |
| 128 KiB | 0.13 s   | +234% | -     | 39.9GB/s   | 19.9GB/s  |
| 256 KiB | 0.13 s   | +230% | +1%   | 40.3GB/s   | 20.2GB/s  |
| 512 KiB | 0.15 s   | +279% | +15%  | 35.1GB/s   | 17.5GB/s  |
| 1 MiB   | 0.31 s   | +675% | +104% | 17.2GB/s   | 8.59GB/s  |
| 2 MiB   | 0.33 s   | +712% | +5%   | 16.4GB/s   | 8.19GB/s  | L2, 2 MiB   |
| 4 MiB   | 0.33 s   | +707% | +1%   | 16.5GB/s   | 8.25GB/s  |
| 8 MiB   | 0.33 s   | +712% | +1%   | 16.4GB/s   | 8.20GB/s  |
| 16 MiB  | 0.33 s   | +722% | +1%   | 16.2GB/s   | 8.10GB/s  |
| 32 MiB  | 0.34 s   | +733% | +1%   | 16.0GB/s   | 7.99GB/s  |
| 64 MiB  | 0.34 s   | +735% | -     | 16.0GB/s   | 7.98GB/s  |
| 128 MiB | 0.34 s   | +734% | -     | 16.0GB/s   | 7.98GB/s  |

**SIMD cached fill**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|-------|-------|------------|-----------|-------------|
| 256 B   | 36.55 ms | -     | -     | 146.9GB/s  | 73.5GB/s  |
| 512 B   | 37.40 ms | +2%   | +2%   | 143.6GB/s  | 71.8GB/s  |
| 1 KiB   | 46.33 ms | +27%  | +24%  | 115.9GB/s  | 57.9GB/s  |
| 2 KiB   | 41.28 ms | +13%  | +11%  | 130.0GB/s  | 65.0GB/s  |
| 4 KiB   | 36.71 ms | -     | +11%  | 146.2GB/s  | 73.1GB/s  | page, 4 KiB |
| 8 KiB   | 38.10 ms | +4%   | +4%   | 140.9GB/s  | 70.5GB/s  |
| 16 KiB  | 38.57 ms | +6%   | +1%   | 139.2GB/s  | 69.6GB/s  |
| 32 KiB  | 0.14 s   | +272% | +253% | 39.4GB/s   | 19.7GB/s  | L1D, 32 KiB |
| 64 KiB  | 0.13 s   | +269% | +1%   | 39.8GB/s   | 19.9GB/s  |
| 128 KiB | 0.13 s   | +269% | -     | 39.8GB/s   | 19.9GB/s  |
| 256 KiB | 0.14 s   | +278% | +2%   | 38.8GB/s   | 19.4GB/s  |
| 512 KiB | 0.19 s   | +407% | +34%  | 29.0GB/s   | 14.5GB/s  |
| 1 MiB   | 0.31 s   | +744% | +67%  | 17.4GB/s   | 8.70GB/s  |
| 2 MiB   | 0.34 s   | +825% | +10%  | 15.9GB/s   | 7.94GB/s  | L2, 2 MiB   |
| 4 MiB   | 0.34 s   | +820% | +1%   | 16.0GB/s   | 7.99GB/s  |
| 8 MiB   | 0.33 s   | +812% | +1%   | 16.1GB/s   | 8.05GB/s  |
| 16 MiB  | 0.34 s   | +829% | +2%   | 15.8GB/s   | 7.91GB/s  |
| 32 MiB  | 0.34 s   | +832% | -     | 15.8GB/s   | 7.88GB/s  |
| 64 MiB  | 0.36 s   | +880% | +5%   | 15.0GB/s   | 7.50GB/s  |
| 128 MiB | 0.34 s   | +819% | +6%   | 16.0GB/s   | 8.00GB/s  |

**SIMD non-cached fill**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|--------|--------|------|-----------|-----------|-------------|
| 256 B   | 8.60 s | +4399% | -    | 2.50GB/s  | 1.25GB/s  |
| 512 B   | 4.54 s | +2274% | +47% | 4.73GB/s  | 2.37GB/s  |
| 1 KiB   | 2.77 s | +1351% | +39% | 7.75GB/s  | 3.87GB/s  |
| 2 KiB   | 1.65 s | +763%  | +40% | 13.0GB/s  | 6.51GB/s  |
| 4 KiB   | 1.10 s | +476%  | +33% | 19.5GB/s  | 9.75GB/s  | page, 4 KiB |
| 8 KiB   | 0.90 s | +371%  | +18% | 23.9GB/s  | 11.9GB/s  |
| 16 KiB  | 0.83 s | +332%  | +8%  | 26.0GB/s  | 13.0GB/s  |
| 32 KiB  | 0.79 s | +313%  | +4%  | 27.2GB/s  | 13.6GB/s  | L1D, 32 KiB |
| 64 KiB  | 0.77 s | +303%  | +3%  | 27.9GB/s  | 13.9GB/s  |
| 128 KiB | 0.76 s | +299%  | +1%  | 28.2GB/s  | 14.1GB/s  |
| 256 KiB | 0.75 s | +294%  | +1%  | 28.5GB/s  | 14.3GB/s  |
| 512 KiB | 0.75 s | +294%  | -    | 28.6GB/s  | 14.3GB/s  |
| 1 MiB   | 0.75 s | +291%  | +1%  | 28.8GB/s  | 14.4GB/s  |
| 2 MiB   | 0.75 s | +290%  | -    | 28.8GB/s  | 14.4GB/s  | L2, 2 MiB   |
| 4 MiB   | 0.19 s | -      | +74% | 28.1GB/s  | 14.0GB/s  |
| 8 MiB   | 0.19 s | -      | -    | 28.1GB/s  | 14.0GB/s  |
| 16 MiB  | 0.20 s | +5%    | +4%  | 26.9GB/s  | 13.4GB/s  |
| 32 MiB  | 0.19 s | -      | +4%  | 28.1GB/s  | 14.0GB/s  |
| 64 MiB  | 0.19 s | +2%    | +2%  | 27.6GB/s  | 13.8GB/s  |
| 128 MiB | 0.20 s | +4%    | +2%  | 27.0GB/s  | 13.5GB/s  |

</details>


# MediaTek Helio G96

* Device: Realme 8i
* block size: 1GB
* Memory: 4GB, LPDDR4X, DC 16bit, 2133 MHz, **8.5** GB/s

## Performance core

* Cache:
	- L1D: 64 KB (per core)
	- L2: 512 KB (per core)
	- L3: 1 MB

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +25%  | 26.1GB/s  |
| 512 B   | +1%   | 32.3GB/s  |
| 1 KiB   | -     | 32.7GB/s  |
| 2 KiB   | -     | 32.6GB/s  |
| 4 KiB   | -     | 32.7GB/s  | page, 4 KiB |
| 8 KiB   | -     | 32.7GB/s  |
| 16 KiB  | +19%  | 27.4GB/s  |
| 32 KiB  | +2%   | 32.0GB/s  |
| 64 KiB  | +10%  | 29.6GB/s  |
| 128 KiB | +25%  | 26.2GB/s  |
| 256 KiB | +63%  | 20.0GB/s  |
| 512 KiB | +124% | 14.6GB/s  |
| 1 MiB   | +197% | 11.0GB/s  |
| 2 MiB   | +288% | 8.42GB/s  |
| 4 MiB   | +356% | 7.17GB/s  |
| 8 MiB   | +341% | 7.41GB/s  |
| 16 MiB  | +357% | 7.15GB/s  |
| 32 MiB  | +350% | 7.27GB/s  |
| 64 MiB  | +353% | 7.21GB/s  |
| 128 MiB | +343% | 7.38GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +4%   | 31.5GB/s  |
| 512 B   | +1%   | 32.5GB/s  |
| 1 KiB   | -     | 32.6GB/s  |
| 2 KiB   | -     | 32.6GB/s  |
| 4 KiB   | -     | 32.7GB/s  | page, 4 KiB |
| 8 KiB   | -     | 32.6GB/s  |
| 16 KiB  | -     | 32.6GB/s  |
| 32 KiB  | +1%   | 32.4GB/s  |
| 64 KiB  | +7%   | 30.6GB/s  |
| 128 KiB | +42%  | 23.0GB/s  |
| 256 KiB | +61%  | 20.3GB/s  |
| 512 KiB | +113% | 15.4GB/s  |
| 1 MiB   | +196% | 11.0GB/s  |
| 2 MiB   | +288% | 8.42GB/s  |
| 4 MiB   | +348% | 7.30GB/s  |
| 8 MiB   | +352% | 7.23GB/s  |
| 16 MiB  | +338% | 7.47GB/s  |
| 32 MiB  | +357% | 7.16GB/s  |
| 64 MiB  | +341% | 7.41GB/s  |
| 128 MiB | +342% | 7.39GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +13%  | 28.8GB/s  |
| 512 B   | +6%   | 30.7GB/s  |
| 1 KiB   | +4%   | 31.4GB/s  |
| 2 KiB   | +2%   | 32.0GB/s  |
| 4 KiB   | +1%   | 32.4GB/s  | page, 4 KiB |
| 8 KiB   | -     | 32.5GB/s  |
| 16 KiB  | -     | 32.6GB/s  |
| 32 KiB  | +2%   | 32.1GB/s  |
| 64 KiB  | +13%  | 28.8GB/s  |
| 128 KiB | +27%  | 25.6GB/s  |
| 256 KiB | +73%  | 18.9GB/s  |
| 512 KiB | +105% | 15.9GB/s  |
| 1 MiB   | +235% | 9.75GB/s  |
| 2 MiB   | +300% | 8.15GB/s  |
| 4 MiB   | +355% | 7.16GB/s  |
| 8 MiB   | +333% | 7.53GB/s  |
| 16 MiB  | +339% | 7.42GB/s  |
| 32 MiB  | +343% | 7.36GB/s  |
| 64 MiB  | +345% | 7.33GB/s  |
| 128 MiB | +348% | 7.28GB/s  |

</details>
<details><summary><b>memcpy, multithreading (2 threads, 2 cores)</b></summary>

**memcpy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +8%   | 33.9GB/s  | 16.9GB/s  |
| 512 B   | +32%  | 27.8GB/s  | 13.9GB/s  |
| 1 KiB   | +16%  | 31.5GB/s  | 15.8GB/s  |
| 2 KiB   | +13%  | 32.5GB/s  | 16.3GB/s  |
| 4 KiB   | -     | 36.6GB/s  | 18.3GB/s  | page, 4 KiB |
| 8 KiB   | +5%   | 34.8GB/s  | 17.4GB/s  |
| 16 KiB  | +15%  | 32.0GB/s  | 16.0GB/s  |
| 32 KiB  | +5%   | 34.9GB/s  | 17.4GB/s  |
| 64 KiB  | +16%  | 31.5GB/s  | 15.7GB/s  |
| 128 KiB | +44%  | 25.5GB/s  | 12.7GB/s  |
| 256 KiB | +53%  | 24.0GB/s  | 12.0GB/s  |
| 512 KiB | +183% | 13.0GB/s  | 6.48GB/s  |
| 1 MiB   | +347% | 8.19GB/s  | 4.09GB/s  |
| 2 MiB   | +415% | 7.11GB/s  | 3.55GB/s  |
| 4 MiB   | +436% | 6.83GB/s  | 3.42GB/s  |
| 8 MiB   | +438% | 6.80GB/s  | 3.40GB/s  |
| 16 MiB  | +448% | 6.68GB/s  | 3.34GB/s  |
| 32 MiB  | +441% | 6.77GB/s  | 3.39GB/s  |
| 64 MiB  | +445% | 6.71GB/s  | 3.36GB/s  |
| 128 MiB | +443% | 6.74GB/s  | 3.37GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +8%   | 32.9GB/s  | 16.5GB/s  |
| 512 B   | +10%  | 32.4GB/s  | 16.2GB/s  |
| 1 KiB   | +4%   | 34.2GB/s  | 17.1GB/s  |
| 2 KiB   | +11%  | 31.9GB/s  | 16.0GB/s  |
| 4 KiB   | -     | 35.6GB/s  | 17.8GB/s  | page, 4 KiB |
| 8 KiB   | +6%   | 33.5GB/s  | 16.7GB/s  |
| 16 KiB  | +7%   | 33.3GB/s  | 16.6GB/s  |
| 32 KiB  | +8%   | 33.1GB/s  | 16.5GB/s  |
| 64 KiB  | +12%  | 31.9GB/s  | 15.9GB/s  |
| 128 KiB | +1%   | 35.1GB/s  | 17.5GB/s  |
| 256 KiB | +46%  | 24.4GB/s  | 12.2GB/s  |
| 512 KiB | +169% | 13.2GB/s  | 6.60GB/s  |
| 1 MiB   | +333% | 8.21GB/s  | 4.11GB/s  |
| 2 MiB   | +403% | 7.07GB/s  | 3.54GB/s  |
| 4 MiB   | +422% | 6.82GB/s  | 3.41GB/s  |
| 8 MiB   | +422% | 6.81GB/s  | 3.41GB/s  |
| 16 MiB  | +421% | 6.82GB/s  | 3.41GB/s  |
| 32 MiB  | +431% | 6.70GB/s  | 3.35GB/s  |
| 64 MiB  | +431% | 6.70GB/s  | 3.35GB/s  |
| 128 MiB | +417% | 6.88GB/s  | 3.44GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +20%  | 46.7GB/s  | 23.4GB/s  |
| 512 B   | +20%  | 46.9GB/s  | 23.5GB/s  |
| 1 KiB   | +7%   | 52.8GB/s  | 26.4GB/s  |
| 2 KiB   | +10%  | 51.2GB/s  | 25.6GB/s  |
| 4 KiB   | +5%   | 53.6GB/s  | 26.8GB/s  | page, 4 KiB |
| 8 KiB   | +5%   | 53.6GB/s  | 26.8GB/s  |
| 16 KiB  | +3%   | 54.4GB/s  | 27.2GB/s  |
| 32 KiB  | -     | 56.3GB/s  | 28.1GB/s  |
| 64 KiB  | +12%  | 50.4GB/s  | 25.2GB/s  |
| 128 KiB | +27%  | 44.3GB/s  | 22.1GB/s  |
| 256 KiB | +114% | 26.3GB/s  | 13.2GB/s  |
| 512 KiB | +349% | 12.5GB/s  | 6.27GB/s  |
| 1 MiB   | +590% | 8.16GB/s  | 4.08GB/s  |
| 2 MiB   | +669% | 7.32GB/s  | 3.66GB/s  |
| 4 MiB   | +101% | 6.99GB/s  | 3.49GB/s  |
| 8 MiB   | +109% | 6.73GB/s  | 3.36GB/s  |
| 16 MiB  | +106% | 6.82GB/s  | 3.41GB/s  |
| 32 MiB  | +106% | 6.84GB/s  | 3.42GB/s  |
| 64 MiB  | +109% | 6.74GB/s  | 3.37GB/s  |
| 128 MiB | +108% | 6.77GB/s  | 3.38GB/s  |

</details>
<details><summary><b>memset, single thread</b></summary>

**memset**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +135% | 14.6GB/s  |
| 512 B   | +17%  | 29.2GB/s  |
| 1 KiB   | +14%  | 30.1GB/s  |
| 2 KiB   | +8%   | 31.8GB/s  |
| 4 KiB   | +4%   | 33.0GB/s  | page, 4 KiB |
| 8 KiB   | +10%  | 31.3GB/s  |
| 16 KiB  | +19%  | 28.7GB/s  |
| 32 KiB  | +6%   | 32.5GB/s  |
| 64 KiB  | -     | 34.3GB/s  |
| 128 KiB | +2%   | 33.6GB/s  |
| 256 KiB | +11%  | 30.9GB/s  |
| 512 KiB | +99%  | 17.2GB/s  |
| 1 MiB   | +88%  | 18.2GB/s  |
| 2 MiB   | +92%  | 17.9GB/s  |
| 4 MiB   | +113% | 16.1GB/s  |
| 8 MiB   | +119% | 15.7GB/s  |
| 16 MiB  | +122% | 15.4GB/s  |
| 32 MiB  | +123% | 15.4GB/s  |
| 64 MiB  | +136% | 14.5GB/s  |
| 128 MiB | +160% | 13.2GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | -     | 32.7GB/s  |
| 512 B   | -     | 32.7GB/s  |
| 1 KiB   | +10%  | 29.8GB/s  |
| 2 KiB   | -     | 32.7GB/s  |
| 4 KiB   | -     | 32.7GB/s  | page, 4 KiB |
| 8 KiB   | -     | 32.7GB/s  |
| 16 KiB  | -     | 32.7GB/s  |
| 32 KiB  | -     | 32.7GB/s  |
| 64 KiB  | +6%   | 30.9GB/s  |
| 128 KiB | +7%   | 30.4GB/s  |
| 256 KiB | +16%  | 28.2GB/s  |
| 512 KiB | +23%  | 26.7GB/s  |
| 1 MiB   | +61%  | 20.4GB/s  |
| 2 MiB   | +101% | 16.3GB/s  |
| 4 MiB   | +101% | 16.3GB/s  |
| 8 MiB   | +139% | 13.7GB/s  |
| 16 MiB  | +110% | 15.6GB/s  |
| 32 MiB  | +112% | 15.4GB/s  |
| 64 MiB  | +112% | 15.4GB/s  |
| 128 MiB | +113% | 15.4GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | -     | 32.6GB/s  |
| 512 B   | -     | 32.7GB/s  |
| 1 KiB   | -     | 32.6GB/s  |
| 2 KiB   | -     | 32.7GB/s  |
| 4 KiB   | -     | 32.7GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 32.4GB/s  |
| 16 KiB  | -     | 32.7GB/s  |
| 32 KiB  | -     | 32.7GB/s  |
| 64 KiB  | +1%   | 32.5GB/s  |
| 128 KiB | +1%   | 32.5GB/s  |
| 256 KiB | +6%   | 30.9GB/s  |
| 512 KiB | +26%  | 26.0GB/s  |
| 1 MiB   | +61%  | 20.4GB/s  |
| 2 MiB   | +83%  | 17.8GB/s  |
| 4 MiB   | +109% | 15.6GB/s  |
| 8 MiB   | +128% | 14.4GB/s  |
| 16 MiB  | +136% | 13.9GB/s  |
| 32 MiB  | +131% | 14.2GB/s  |
| 64 MiB  | +121% | 14.8GB/s  |
| 128 MiB | +137% | 13.8GB/s  |

</details>
<details><summary><b>memset, multithreading (2 threads, 2 cores)</b></summary>

**memset**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +11%  | 31.2GB/s  | 15.6GB/s  |
| 512 B   | +4%   | 33.2GB/s  | 16.6GB/s  |
| 1 KiB   | +2%   | 33.9GB/s  | 16.9GB/s  |
| 2 KiB   | +35%  | 25.7GB/s  | 12.8GB/s  |
| 4 KiB   | +6%   | 32.7GB/s  | 16.4GB/s  | page, 4 KiB |
| 8 KiB   | +17%  | 29.7GB/s  | 14.8GB/s  |
| 16 KiB  | -     | 34.6GB/s  | 17.3GB/s  |
| 32 KiB  | +20%  | 28.8GB/s  | 14.4GB/s  |
| 64 KiB  | +45%  | 23.8GB/s  | 11.9GB/s  |
| 128 KiB | +7%   | 32.4GB/s  | 16.2GB/s  |
| 256 KiB | +68%  | 20.6GB/s  | 10.3GB/s  |
| 512 KiB | +87%  | 18.5GB/s  | 9.24GB/s  |
| 1 MiB   | +117% | 16.0GB/s  | 7.99GB/s  |
| 2 MiB   | +126% | 15.3GB/s  | 7.66GB/s  |
| 4 MiB   | +131% | 15.0GB/s  | 7.48GB/s  |
| 8 MiB   | +151% | 13.8GB/s  | 6.89GB/s  |
| 16 MiB  | +144% | 14.2GB/s  | 7.09GB/s  |
| 32 MiB  | +140% | 14.4GB/s  | 7.20GB/s  |
| 64 MiB  | +151% | 13.8GB/s  | 6.88GB/s  |
| 128 MiB | +145% | 14.1GB/s  | 7.06GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +1%   | 34.6GB/s  | 17.3GB/s  |
| 512 B   | +8%   | 32.4GB/s  | 16.2GB/s  |
| 1 KiB   | +4%   | 33.4GB/s  | 16.7GB/s  |
| 2 KiB   | +7%   | 32.7GB/s  | 16.3GB/s  |
| 4 KiB   | +6%   | 33.0GB/s  | 16.5GB/s  | page, 4 KiB |
| 8 KiB   | -     | 34.8GB/s  | 17.4GB/s  |
| 16 KiB  | +5%   | 33.1GB/s  | 16.6GB/s  |
| 32 KiB  | +32%  | 26.4GB/s  | 13.2GB/s  |
| 64 KiB  | +30%  | 26.8GB/s  | 13.4GB/s  |
| 128 KiB | +45%  | 24.0GB/s  | 12.0GB/s  |
| 256 KiB | +57%  | 22.1GB/s  | 11.1GB/s  |
| 512 KiB | +91%  | 18.3GB/s  | 9.13GB/s  |
| 1 MiB   | +116% | 16.1GB/s  | 8.05GB/s  |
| 2 MiB   | +128% | 15.3GB/s  | 7.63GB/s  |
| 4 MiB   | +134% | 14.9GB/s  | 7.44GB/s  |
| 8 MiB   | +152% | 13.8GB/s  | 6.92GB/s  |
| 16 MiB  | +142% | 14.4GB/s  | 7.21GB/s  |
| 32 MiB  | +149% | 14.0GB/s  | 7.01GB/s  |
| 64 MiB  | +149% | 14.0GB/s  | 6.99GB/s  |
| 128 MiB | +144% | 14.3GB/s  | 7.14GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +15%  | 52.1GB/s  | 26.0GB/s  |
| 512 B   | +12%  | 53.6GB/s  | 26.8GB/s  |
| 1 KiB   | +9%   | 55.1GB/s  | 27.5GB/s  |
| 2 KiB   | +12%  | 53.6GB/s  | 26.8GB/s  |
| 4 KiB   | +11%  | 54.2GB/s  | 27.1GB/s  | page, 4 KiB |
| 8 KiB   | +12%  | 53.8GB/s  | 26.9GB/s  |
| 16 KiB  | +11%  | 53.8GB/s  | 26.9GB/s  |
| 32 KiB  | +12%  | 53.4GB/s  | 26.7GB/s  |
| 64 KiB  | +23%  | 48.9GB/s  | 24.5GB/s  |
| 128 KiB | +32%  | 45.4GB/s  | 22.7GB/s  |
| 256 KiB | +135% | 25.5GB/s  | 12.8GB/s  |
| 512 KiB | +208% | 19.5GB/s  | 9.74GB/s  |
| 1 MiB   | +270% | 16.2GB/s  | 8.10GB/s  |
| 2 MiB   | +289% | 15.4GB/s  | 7.71GB/s  |
| 4 MiB   | -     | 15.0GB/s  | 7.50GB/s  |
| 8 MiB   | +8%   | 13.9GB/s  | 6.96GB/s  |
| 16 MiB  | +4%   | 14.4GB/s  | 7.20GB/s  |
| 32 MiB  | +5%   | 14.3GB/s  | 7.16GB/s  |
| 64 MiB  | +11%  | 13.6GB/s  | 6.78GB/s  |
| 128 MiB | +5%   | 14.3GB/s  | 7.15GB/s  |

</details>
<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +45%  | 29.3GB/s  |
| 512 B   | +10%  | 38.3GB/s  |
| 1 KiB   | +4%   | 40.5GB/s  |
| 2 KiB   | +2%   | 41.6GB/s  |
| 4 KiB   | +1%   | 41.8GB/s  | page, 4 KiB |
| 8 KiB   | -     | 42.3GB/s  |
| 16 KiB  | +1%   | 41.8GB/s  |
| 32 KiB  | +4%   | 40.5GB/s  |
| 64 KiB  | +24%  | 34.1GB/s  |
| 128 KiB | +68%  | 25.1GB/s  |
| 256 KiB | +101% | 21.1GB/s  |
| 512 KiB | +135% | 18.0GB/s  |
| 1 MiB   | +242% | 12.4GB/s  |
| 2 MiB   | +277% | 11.2GB/s  |
| 4 MiB   | +321% | 10.1GB/s  |
| 8 MiB   | +340% | 9.62GB/s  |
| 16 MiB  | +342% | 9.58GB/s  |
| 32 MiB  | +340% | 9.61GB/s  |
| 64 MiB  | +343% | 9.56GB/s  |
| 128 MiB | +372% | 8.96GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +179% | 15.0GB/s  |
| 512 B   | +37%  | 30.5GB/s  |
| 1 KiB   | +10%  | 38.2GB/s  |
| 2 KiB   | +5%   | 40.0GB/s  |
| 4 KiB   | +1%   | 41.4GB/s  | page, 4 KiB |
| 8 KiB   | -     | 41.9GB/s  |
| 16 KiB  | +188% | 14.5GB/s  |
| 32 KiB  | +148% | 16.9GB/s  |
| 64 KiB  | +177% | 15.1GB/s  |
| 128 KiB | +159% | 16.2GB/s  |
| 256 KiB | +161% | 16.0GB/s  |
| 512 KiB | +165% | 15.8GB/s  |
| 1 MiB   | +163% | 15.9GB/s  |
| 2 MiB   | +163% | 15.9GB/s  |
| 4 MiB   | +163% | 15.9GB/s  |
| 8 MiB   | +167% | 15.7GB/s  |
| 16 MiB  | +164% | 15.9GB/s  |
| 32 MiB  | +164% | 15.9GB/s  |
| 64 MiB  | +168% | 15.6GB/s  |
| 128 MiB | +166% | 15.8GB/s  |

**read from cache (SIMD xor v2)**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +67%  | 26.1GB/s  |
| 512 B   | +18%  | 37.1GB/s  |
| 1 KiB   | +7%   | 40.8GB/s  |
| 2 KiB   | +2%   | 42.7GB/s  |
| 4 KiB   | +1%   | 43.3GB/s  | page, 4 KiB |
| 8 KiB   | -     | 43.6GB/s  |
| 16 KiB  | +192% | 14.9GB/s  |
| 32 KiB  | +158% | 16.9GB/s  |
| 64 KiB  | +167% | 16.3GB/s  |
| 128 KiB | +175% | 15.8GB/s  |
| 256 KiB | +176% | 15.8GB/s  |
| 512 KiB | +176% | 15.8GB/s  |
| 1 MiB   | +179% | 15.6GB/s  |
| 2 MiB   | +177% | 15.7GB/s  |
| 4 MiB   | +177% | 15.7GB/s  |
| 8 MiB   | +178% | 15.7GB/s  |
| 16 MiB  | +180% | 15.5GB/s  |
| 32 MiB  | +178% | 15.7GB/s  |
| 64 MiB  | +178% | 15.7GB/s  |
| 128 MiB | +181% | 15.5GB/s  |

</details>
<details><summary><b>other, multithreading (2 threads, 2 cores)</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +14%  | 35.7GB/s  | 17.9GB/s  |
| 512 B   | +9%   | 37.4GB/s  | 18.7GB/s  |
| 1 KiB   | +14%  | 35.6GB/s  | 17.8GB/s  |
| 2 KiB   | +14%  | 35.5GB/s  | 17.8GB/s  |
| 4 KiB   | +3%   | 39.4GB/s  | 19.7GB/s  | page, 4 KiB |
| 8 KiB   | -     | 40.6GB/s  | 20.3GB/s  |
| 16 KiB  | +7%   | 38.0GB/s  | 19.0GB/s  |
| 32 KiB  | +9%   | 37.2GB/s  | 18.6GB/s  |
| 64 KiB  | +23%  | 33.1GB/s  | 16.6GB/s  |
| 128 KiB | +27%  | 32.0GB/s  | 16.0GB/s  |
| 256 KiB | +71%  | 23.7GB/s  | 11.8GB/s  |
| 512 KiB | +140% | 16.9GB/s  | 8.45GB/s  |
| 1 MiB   | +256% | 11.4GB/s  | 5.71GB/s  |
| 2 MiB   | +313% | 9.83GB/s  | 4.91GB/s  |
| 4 MiB   | +341% | 9.21GB/s  | 4.60GB/s  |
| 8 MiB   | +347% | 9.09GB/s  | 4.55GB/s  |
| 16 MiB  | +348% | 9.06GB/s  | 4.53GB/s  |
| 32 MiB  | +366% | 8.72GB/s  | 4.36GB/s  |
| 64 MiB  | +356% | 8.91GB/s  | 4.45GB/s  |
| 128 MiB | +364% | 8.75GB/s  | 4.38GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +30%  | 36.2GB/s  | 18.1GB/s  |
| 512 B   | -     | 47.0GB/s  | 23.5GB/s  |
| 1 KiB   | +12%  | 41.8GB/s  | 20.9GB/s  |
| 2 KiB   | +26%  | 37.4GB/s  | 18.7GB/s  |
| 4 KiB   | +3%   | 45.5GB/s  | 22.7GB/s  | page, 4 KiB |
| 8 KiB   | +19%  | 39.5GB/s  | 19.8GB/s  |
| 16 KiB  | +68%  | 27.9GB/s  | 14.0GB/s  |
| 32 KiB  | +47%  | 32.0GB/s  | 16.0GB/s  |
| 64 KiB  | +48%  | 31.8GB/s  | 15.9GB/s  |
| 128 KiB | +48%  | 31.8GB/s  | 15.9GB/s  |
| 256 KiB | +74%  | 27.1GB/s  | 13.5GB/s  |
| 512 KiB | +72%  | 27.3GB/s  | 13.7GB/s  |
| 1 MiB   | +157% | 18.3GB/s  | 9.14GB/s  |
| 2 MiB   | +145% | 19.2GB/s  | 9.58GB/s  |
| 4 MiB   | +142% | 19.4GB/s  | 9.71GB/s  |
| 8 MiB   | +175% | 17.1GB/s  | 8.55GB/s  |
| 16 MiB  | +164% | 17.8GB/s  | 8.89GB/s  |
| 32 MiB  | +147% | 19.0GB/s  | 9.51GB/s  |
| 64 MiB  | +162% | 17.9GB/s  | 8.97GB/s  |
| 128 MiB | +160% | 18.1GB/s  | 9.03GB/s  |

**read from cache (SIMD xor v2)**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +54%  | 32.1GB/s  | 16.0GB/s  |
| 512 B   | +26%  | 39.2GB/s  | 19.6GB/s  |
| 1 KiB   | +6%   | 46.8GB/s  | 23.4GB/s  |
| 2 KiB   | +13%  | 43.8GB/s  | 21.9GB/s  |
| 4 KiB   | +10%  | 45.1GB/s  | 22.6GB/s  | page, 4 KiB |
| 8 KiB   | -     | 49.4GB/s  | 24.7GB/s  |
| 16 KiB  | +51%  | 32.7GB/s  | 16.3GB/s  |
| 32 KiB  | +52%  | 32.6GB/s  | 16.3GB/s  |
| 64 KiB  | +53%  | 32.3GB/s  | 16.2GB/s  |
| 128 KiB | +61%  | 30.6GB/s  | 15.3GB/s  |
| 256 KiB | +57%  | 31.5GB/s  | 15.7GB/s  |
| 512 KiB | +56%  | 31.7GB/s  | 15.8GB/s  |
| 1 MiB   | +138% | 20.8GB/s  | 10.4GB/s  |
| 2 MiB   | +147% | 20.0GB/s  | 10.0GB/s  |
| 4 MiB   | +191% | 17.0GB/s  | 8.50GB/s  |
| 8 MiB   | +185% | 17.4GB/s  | 8.68GB/s  |
| 16 MiB  | +176% | 17.9GB/s  | 8.96GB/s  |
| 32 MiB  | +170% | 18.3GB/s  | 9.17GB/s  |
| 64 MiB  | +141% | 20.5GB/s  | 10.3GB/s  |
| 128 MiB | +154% | 19.5GB/s  | 9.74GB/s  |

</details>

## EnergyEfficient core

* Cache:
	- L1D: 32 KB (per core)
	- L2: 256 KB (per A55 cluster - 6 cores)
	- L3: 1 MB

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +14% | 7.78GB/s  |
| 512 B   | +4%  | 8.54GB/s  |
| 1 KiB   | -    | 8.85GB/s  |
| 2 KiB   | +7%  | 8.28GB/s  |
| 4 KiB   | +4%  | 8.50GB/s  | page, 4 KiB |
| 8 KiB   | +9%  | 8.12GB/s  |
| 16 KiB  | +24% | 7.12GB/s  |
| 32 KiB  | +18% | 7.50GB/s  |
| 64 KiB  | +27% | 6.96GB/s  |
| 128 KiB | +24% | 7.13GB/s  |
| 256 KiB | +25% | 7.06GB/s  |
| 512 KiB | +45% | 6.10GB/s  |
| 1 MiB   | +59% | 5.58GB/s  |
| 2 MiB   | +70% | 5.20GB/s  |
| 4 MiB   | +81% | 4.89GB/s  |
| 8 MiB   | +78% | 4.98GB/s  |
| 16 MiB  | +74% | 5.08GB/s  |
| 32 MiB  | +71% | 5.18GB/s  |
| 64 MiB  | +72% | 5.15GB/s  |
| 128 MiB | +71% | 5.18GB/s  |

**SIMD cached copy**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +9%  | 6.72GB/s  |
| 512 B   | +7%  | 6.83GB/s  |
| 1 KiB   | +3%  | 7.10GB/s  |
| 2 KiB   | +1%  | 7.23GB/s  |
| 4 KiB   | -    | 7.30GB/s  | page, 4 KiB |
| 8 KiB   | -    | 7.33GB/s  |
| 16 KiB  | +4%  | 7.04GB/s  |
| 32 KiB  | +13% | 6.47GB/s  |
| 64 KiB  | +8%  | 6.77GB/s  |
| 128 KiB | +8%  | 6.80GB/s  |
| 256 KiB | +9%  | 6.73GB/s  |
| 512 KiB | +27% | 5.77GB/s  |
| 1 MiB   | +43% | 5.14GB/s  |
| 2 MiB   | +38% | 5.32GB/s  |
| 4 MiB   | +51% | 4.86GB/s  |
| 8 MiB   | +43% | 5.14GB/s  |
| 16 MiB  | +44% | 5.10GB/s  |
| 32 MiB  | +42% | 5.15GB/s  |
| 64 MiB  | +43% | 5.14GB/s  |
| 128 MiB | +42% | 5.18GB/s  |

**SIMD non-cached copy**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +7%  | 6.90GB/s  |
| 512 B   | +10% | 6.72GB/s  |
| 1 KiB   | +3%  | 7.20GB/s  |
| 2 KiB   | +1%  | 7.30GB/s  |
| 4 KiB   | -    | 7.38GB/s  | page, 4 KiB |
| 8 KiB   | +7%  | 6.91GB/s  |
| 16 KiB  | +4%  | 7.10GB/s  |
| 32 KiB  | +9%  | 6.79GB/s  |
| 64 KiB  | +12% | 6.58GB/s  |
| 128 KiB | +16% | 6.37GB/s  |
| 256 KiB | +16% | 6.35GB/s  |
| 512 KiB | +39% | 5.31GB/s  |
| 1 MiB   | +48% | 4.98GB/s  |
| 2 MiB   | +73% | 4.27GB/s  |
| 4 MiB   | +67% | 4.42GB/s  |
| 8 MiB   | +57% | 4.71GB/s  |
| 16 MiB  | +60% | 4.63GB/s  |
| 32 MiB  | +59% | 4.65GB/s  |
| 64 MiB  | +66% | 4.45GB/s  |
| 128 MiB | +60% | 4.61GB/s  |

</details>
<details><summary><b>memcpy, multithreading (4 threads, 4 cores)</b></summary>

**memcpy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +10%  | 22.9GB/s  | 5.73GB/s  |
| 512 B   | +6%   | 23.7GB/s  | 5.93GB/s  |
| 1 KiB   | +19%  | 21.3GB/s  | 5.32GB/s  |
| 2 KiB   | -     | 25.3GB/s  | 6.32GB/s  |
| 4 KiB   | +4%   | 24.2GB/s  | 6.06GB/s  | page, 4 KiB |
| 8 KiB   | +4%   | 24.2GB/s  | 6.04GB/s  |
| 16 KiB  | +25%  | 20.1GB/s  | 5.03GB/s  |
| 32 KiB  | +11%  | 22.8GB/s  | 5.71GB/s  |
| 64 KiB  | +15%  | 22.0GB/s  | 5.50GB/s  |
| 128 KiB | +22%  | 20.8GB/s  | 5.19GB/s  |
| 256 KiB | +218% | 7.93GB/s  | 1.98GB/s  |
| 512 KiB | +328% | 5.90GB/s  | 1.47GB/s  |
| 1 MiB   | +315% | 6.09GB/s  | 1.52GB/s  |
| 2 MiB   | +312% | 6.13GB/s  | 1.53GB/s  |
| 4 MiB   | +319% | 6.03GB/s  | 1.51GB/s  |
| 8 MiB   | +321% | 6.00GB/s  | 1.50GB/s  |
| 16 MiB  | +319% | 6.03GB/s  | 1.51GB/s  |
| 32 MiB  | +318% | 6.04GB/s  | 1.51GB/s  |
| 64 MiB  | +325% | 5.95GB/s  | 1.49GB/s  |
| 128 MiB | +327% | 5.92GB/s  | 1.48GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +10%  | 20.0GB/s  | 4.99GB/s  |
| 512 B   | +5%   | 20.9GB/s  | 5.23GB/s  |
| 1 KiB   | +10%  | 19.9GB/s  | 4.99GB/s  |
| 2 KiB   | +12%  | 19.5GB/s  | 4.87GB/s  |
| 4 KiB   | +4%   | 21.0GB/s  | 5.25GB/s  | page, 4 KiB |
| 8 KiB   | -     | 21.9GB/s  | 5.47GB/s  |
| 16 KiB  | +9%   | 20.1GB/s  | 5.02GB/s  |
| 32 KiB  | +7%   | 20.4GB/s  | 5.09GB/s  |
| 64 KiB  | +5%   | 20.9GB/s  | 5.22GB/s  |
| 128 KiB | +14%  | 19.2GB/s  | 4.79GB/s  |
| 256 KiB | +194% | 7.43GB/s  | 1.86GB/s  |
| 512 KiB | +266% | 5.98GB/s  | 1.49GB/s  |
| 1 MiB   | +250% | 6.25GB/s  | 1.56GB/s  |
| 2 MiB   | +258% | 6.11GB/s  | 1.53GB/s  |
| 4 MiB   | +263% | 6.02GB/s  | 1.51GB/s  |
| 8 MiB   | +272% | 5.87GB/s  | 1.47GB/s  |
| 16 MiB  | +269% | 5.93GB/s  | 1.48GB/s  |
| 32 MiB  | +266% | 5.97GB/s  | 1.49GB/s  |
| 64 MiB  | +269% | 5.92GB/s  | 1.48GB/s  |
| 128 MiB | +270% | 5.90GB/s  | 1.48GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +12%  | 21.3GB/s  | 5.33GB/s  |
| 512 B   | +8%   | 22.2GB/s  | 5.56GB/s  |
| 1 KiB   | +4%   | 22.9GB/s  | 5.73GB/s  |
| 2 KiB   | +1%   | 23.6GB/s  | 5.90GB/s  |
| 4 KiB   | -     | 23.9GB/s  | 5.99GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 23.8GB/s  | 5.95GB/s  |
| 16 KiB  | +2%   | 23.5GB/s  | 5.87GB/s  |
| 32 KiB  | +7%   | 22.4GB/s  | 5.60GB/s  |
| 64 KiB  | +10%  | 21.8GB/s  | 5.45GB/s  |
| 128 KiB | +64%  | 14.6GB/s  | 3.65GB/s  |
| 256 KiB | +269% | 6.49GB/s  | 1.62GB/s  |
| 512 KiB | +348% | 5.35GB/s  | 1.34GB/s  |
| 1 MiB   | +347% | 5.36GB/s  | 1.34GB/s  |
| 2 MiB   | +345% | 5.38GB/s  | 1.34GB/s  |
| 4 MiB   | +15%  | 5.23GB/s  | 1.31GB/s  |
| 8 MiB   | +14%  | 5.26GB/s  | 1.31GB/s  |
| 16 MiB  | +14%  | 5.24GB/s  | 1.31GB/s  |
| 32 MiB  | +14%  | 5.23GB/s  | 1.31GB/s  |
| 64 MiB  | +15%  | 5.22GB/s  | 1.30GB/s  |
| 128 MiB | +16%  | 5.18GB/s  | 1.29GB/s  |

</details>
<details><summary><b>memset, single thread</b></summary>

**memset**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +72% | 14.3GB/s  |
| 512 B   | +51% | 16.3GB/s  |
| 1 KiB   | +23% | 20.0GB/s  |
| 2 KiB   | +10% | 22.5GB/s  |
| 4 KiB   | +9%  | 22.5GB/s  | page, 4 KiB |
| 8 KiB   | +4%  | 23.8GB/s  |
| 16 KiB  | +1%  | 24.3GB/s  |
| 32 KiB  | -    | 24.6GB/s  |
| 64 KiB  | +4%  | 23.7GB/s  |
| 128 KiB | +15% | 21.4GB/s  |
| 256 KiB | +14% | 21.6GB/s  |
| 512 KiB | +9%  | 22.5GB/s  |
| 1 MiB   | +33% | 18.5GB/s  |
| 2 MiB   | +47% | 16.8GB/s  |
| 4 MiB   | +66% | 14.8GB/s  |
| 8 MiB   | +74% | 14.1GB/s  |
| 16 MiB  | +69% | 14.6GB/s  |
| 32 MiB  | +71% | 14.4GB/s  |
| 64 MiB  | +78% | 13.8GB/s  |
| 128 MiB | +89% | 13.0GB/s  |

**SIMD cached fill**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +13% | 18.6GB/s  |
| 512 B   | +8%  | 19.5GB/s  |
| 1 KiB   | +5%  | 20.0GB/s  |
| 2 KiB   | +3%  | 20.4GB/s  |
| 4 KiB   | +4%  | 20.3GB/s  | page, 4 KiB |
| 8 KiB   | +1%  | 20.7GB/s  |
| 16 KiB  | +1%  | 20.9GB/s  |
| 32 KiB  | +3%  | 20.4GB/s  |
| 64 KiB  | -    | 21.0GB/s  |
| 128 KiB | +2%  | 20.7GB/s  |
| 256 KiB | +5%  | 20.1GB/s  |
| 512 KiB | +8%  | 19.5GB/s  |
| 1 MiB   | +24% | 17.0GB/s  |
| 2 MiB   | +45% | 14.5GB/s  |
| 4 MiB   | +30% | 16.1GB/s  |
| 8 MiB   | +37% | 15.3GB/s  |
| 16 MiB  | +44% | 14.6GB/s  |
| 32 MiB  | +45% | 14.5GB/s  |
| 64 MiB  | +54% | 13.7GB/s  |
| 128 MiB | +58% | 13.3GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +14%  | 20.1GB/s  |
| 512 B   | +7%   | 21.4GB/s  |
| 1 KiB   | +16%  | 19.7GB/s  |
| 2 KiB   | +3%   | 22.2GB/s  |
| 4 KiB   | +1%   | 22.6GB/s  | page, 4 KiB |
| 8 KiB   | +2%   | 22.5GB/s  |
| 16 KiB  | +1%   | 22.7GB/s  |
| 32 KiB  | +1%   | 22.8GB/s  |
| 64 KiB  | -     | 22.9GB/s  |
| 128 KiB | +13%  | 20.3GB/s  |
| 256 KiB | +42%  | 16.2GB/s  |
| 512 KiB | +46%  | 15.7GB/s  |
| 1 MiB   | +48%  | 15.4GB/s  |
| 2 MiB   | +98%  | 11.6GB/s  |
| 4 MiB   | +91%  | 12.0GB/s  |
| 8 MiB   | +93%  | 11.8GB/s  |
| 16 MiB  | +116% | 10.6GB/s  |
| 32 MiB  | +125% | 10.2GB/s  |
| 64 MiB  | +98%  | 11.6GB/s  |
| 128 MiB | +98%  | 11.6GB/s  |

</details>
<details><summary><b>memset, multithreading (4 threads, 4 cores)</b></summary>

**memset**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +72%  | 35.4GB/s  | 8.86GB/s  |
| 512 B   | +30%  | 46.8GB/s  | 11.7GB/s  |
| 1 KiB   | +10%  | 55.1GB/s  | 13.8GB/s  |
| 2 KiB   | +12%  | 54.2GB/s  | 13.6GB/s  |
| 4 KiB   | -     | 60.8GB/s  | 15.2GB/s  | page, 4 KiB |
| 8 KiB   | +4%   | 58.6GB/s  | 14.6GB/s  |
| 16 KiB  | +5%   | 57.7GB/s  | 14.4GB/s  |
| 32 KiB  | +8%   | 56.5GB/s  | 14.1GB/s  |
| 64 KiB  | +45%  | 42.0GB/s  | 10.5GB/s  |
| 128 KiB | +104% | 29.8GB/s  | 7.45GB/s  |
| 256 KiB | +216% | 19.3GB/s  | 4.82GB/s  |
| 512 KiB | +276% | 16.2GB/s  | 4.04GB/s  |
| 1 MiB   | +320% | 14.5GB/s  | 3.62GB/s  |
| 2 MiB   | +337% | 13.9GB/s  | 3.48GB/s  |
| 4 MiB   | +349% | 13.5GB/s  | 3.39GB/s  |
| 8 MiB   | +359% | 13.3GB/s  | 3.31GB/s  |
| 16 MiB  | +355% | 13.4GB/s  | 3.34GB/s  |
| 32 MiB  | +352% | 13.4GB/s  | 3.36GB/s  |
| 64 MiB  | +353% | 13.4GB/s  | 3.36GB/s  |
| 128 MiB | +359% | 13.2GB/s  | 3.31GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +30%  | 41.9GB/s  | 10.5GB/s  |
| 512 B   | +48%  | 36.7GB/s  | 9.18GB/s  |
| 1 KiB   | +29%  | 41.9GB/s  | 10.5GB/s  |
| 2 KiB   | +27%  | 42.7GB/s  | 10.7GB/s  |
| 4 KiB   | -     | 54.2GB/s  | 13.6GB/s  | page, 4 KiB |
| 8 KiB   | +2%   | 53.3GB/s  | 13.3GB/s  |
| 16 KiB  | +3%   | 52.7GB/s  | 13.2GB/s  |
| 32 KiB  | +5%   | 51.6GB/s  | 12.9GB/s  |
| 64 KiB  | +24%  | 43.7GB/s  | 10.9GB/s  |
| 128 KiB | +89%  | 28.6GB/s  | 7.15GB/s  |
| 256 KiB | +182% | 19.2GB/s  | 4.81GB/s  |
| 512 KiB | +238% | 16.0GB/s  | 4.01GB/s  |
| 1 MiB   | +278% | 14.3GB/s  | 3.58GB/s  |
| 2 MiB   | +285% | 14.1GB/s  | 3.52GB/s  |
| 4 MiB   | +300% | 13.6GB/s  | 3.39GB/s  |
| 8 MiB   | +302% | 13.5GB/s  | 3.37GB/s  |
| 16 MiB  | +304% | 13.4GB/s  | 3.35GB/s  |
| 32 MiB  | +303% | 13.5GB/s  | 3.36GB/s  |
| 64 MiB  | +312% | 13.2GB/s  | 3.29GB/s  |
| 128 MiB | +316% | 13.0GB/s  | 3.26GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +45%  | 49.3GB/s  | 12.3GB/s  |
| 512 B   | +24%  | 57.9GB/s  | 14.5GB/s  |
| 1 KiB   | +12%  | 64.1GB/s  | 16.0GB/s  |
| 2 KiB   | +9%   | 65.8GB/s  | 16.5GB/s  |
| 4 KiB   | +3%   | 69.5GB/s  | 17.4GB/s  | page, 4 KiB |
| 8 KiB   | +6%   | 67.3GB/s  | 16.8GB/s  |
| 16 KiB  | -     | 71.6GB/s  | 17.9GB/s  |
| 32 KiB  | +4%   | 68.8GB/s  | 17.2GB/s  |
| 64 KiB  | +44%  | 49.8GB/s  | 12.4GB/s  |
| 128 KiB | +140% | 29.9GB/s  | 7.47GB/s  |
| 256 KiB | +456% | 12.9GB/s  | 3.22GB/s  |
| 512 KiB | +651% | 9.53GB/s  | 2.38GB/s  |
| 1 MiB   | +622% | 9.92GB/s  | 2.48GB/s  |
| 2 MiB   | +607% | 10.1GB/s  | 2.53GB/s  |
| 4 MiB   | +81%  | 9.92GB/s  | 2.48GB/s  |
| 8 MiB   | +96%  | 9.12GB/s  | 2.28GB/s  |
| 16 MiB  | +103% | 8.83GB/s  | 2.21GB/s  |
| 32 MiB  | +100% | 8.95GB/s  | 2.24GB/s  |
| 64 MiB  | +84%  | 9.73GB/s  | 2.43GB/s  |
| 128 MiB | +87%  | 9.56GB/s  | 2.39GB/s  |

</details>
<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +4%  | 7.77GB/s  |
| 512 B   | +3%  | 7.83GB/s  |
| 1 KiB   | +2%  | 7.95GB/s  |
| 2 KiB   | +1%  | 8.03GB/s  |
| 4 KiB   | -    | 8.09GB/s  | page, 4 KiB |
| 8 KiB   | -    | 8.06GB/s  |
| 16 KiB  | +3%  | 7.87GB/s  |
| 32 KiB  | +14% | 7.12GB/s  |
| 64 KiB  | +21% | 6.66GB/s  |
| 128 KiB | +16% | 6.96GB/s  |
| 256 KiB | +15% | 7.03GB/s  |
| 512 KiB | +30% | 6.21GB/s  |
| 1 MiB   | +47% | 5.50GB/s  |
| 2 MiB   | +45% | 5.58GB/s  |
| 4 MiB   | +51% | 5.34GB/s  |
| 8 MiB   | +39% | 5.80GB/s  |
| 16 MiB  | +46% | 5.54GB/s  |
| 32 MiB  | +37% | 5.89GB/s  |
| 64 MiB  | +43% | 5.66GB/s  |
| 128 MiB | +45% | 5.59GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +23% | 8.18GB/s  |
| 512 B   | +8%  | 9.32GB/s  |
| 1 KiB   | +4%  | 9.72GB/s  |
| 2 KiB   | +2%  | 9.84GB/s  |
| 4 KiB   | +2%  | 9.89GB/s  | page, 4 KiB |
| 8 KiB   | -    | 10.1GB/s  |
| 16 KiB  | +41% | 7.11GB/s  |
| 32 KiB  | +38% | 7.30GB/s  |
| 64 KiB  | +32% | 7.63GB/s  |
| 128 KiB | +35% | 7.47GB/s  |
| 256 KiB | +38% | 7.31GB/s  |
| 512 KiB | +45% | 6.94GB/s  |
| 1 MiB   | +35% | 7.43GB/s  |
| 2 MiB   | +35% | 7.44GB/s  |
| 4 MiB   | +36% | 7.41GB/s  |
| 8 MiB   | +37% | 7.37GB/s  |
| 16 MiB  | +36% | 7.38GB/s  |
| 32 MiB  | +37% | 7.34GB/s  |
| 64 MiB  | +42% | 7.09GB/s  |
| 128 MiB | +40% | 7.18GB/s  |

**read from cache (SIMD xor v2)**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +17% | 8.27GB/s  |
| 512 B   | +22% | 7.91GB/s  |
| 1 KiB   | +4%  | 9.22GB/s  |
| 2 KiB   | +2%  | 9.46GB/s  |
| 4 KiB   | +3%  | 9.33GB/s  | page, 4 KiB |
| 8 KiB   | -    | 9.64GB/s  |
| 16 KiB  | +41% | 6.82GB/s  |
| 32 KiB  | +40% | 6.91GB/s  |
| 64 KiB  | +31% | 7.33GB/s  |
| 128 KiB | +39% | 6.92GB/s  |
| 256 KiB | +35% | 7.14GB/s  |
| 512 KiB | +35% | 7.12GB/s  |
| 1 MiB   | +37% | 7.03GB/s  |
| 2 MiB   | +35% | 7.13GB/s  |
| 4 MiB   | +35% | 7.12GB/s  |
| 8 MiB   | +61% | 5.97GB/s  |
| 16 MiB  | +36% | 7.11GB/s  |
| 32 MiB  | +37% | 7.04GB/s  |
| 64 MiB  | +39% | 6.95GB/s  |
| 128 MiB | +39% | 6.95GB/s  |


</details>
<details><summary><b>other, multithreading (4 threads, 4 cores)</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +13%  | 21.3GB/s  | 5.31GB/s  |
| 512 B   | +10%  | 21.7GB/s  | 5.43GB/s  |
| 1 KiB   | +6%   | 22.6GB/s  | 5.64GB/s  |
| 2 KiB   | +9%   | 21.9GB/s  | 5.47GB/s  |
| 4 KiB   | +1%   | 23.7GB/s  | 5.92GB/s  | page, 4 KiB |
| 8 KiB   | -     | 23.9GB/s  | 5.98GB/s  |
| 16 KiB  | +5%   | 22.9GB/s  | 5.72GB/s  |
| 32 KiB  | +10%  | 21.7GB/s  | 5.42GB/s  |
| 64 KiB  | +11%  | 21.6GB/s  | 5.39GB/s  |
| 128 KiB | +22%  | 19.7GB/s  | 4.91GB/s  |
| 256 KiB | +101% | 11.9GB/s  | 2.98GB/s  |
| 512 KiB | +187% | 8.35GB/s  | 2.09GB/s  |
| 1 MiB   | +183% | 8.45GB/s  | 2.11GB/s  |
| 2 MiB   | +193% | 8.18GB/s  | 2.05GB/s  |
| 4 MiB   | +194% | 8.16GB/s  | 2.04GB/s  |
| 8 MiB   | +199% | 8.00GB/s  | 2.00GB/s  |
| 16 MiB  | +194% | 8.14GB/s  | 2.04GB/s  |
| 32 MiB  | +196% | 8.09GB/s  | 2.02GB/s  |
| 64 MiB  | +209% | 7.75GB/s  | 1.94GB/s  |
| 128 MiB | +204% | 7.88GB/s  | 1.97GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|------|-----------|-----------|-------------|
| 256 B   | +26% | 26.2GB/s  | 6.56GB/s  |
| 512 B   | +10% | 30.0GB/s  | 7.49GB/s  |
| 1 KiB   | +3%  | 32.1GB/s  | 8.02GB/s  |
| 2 KiB   | +9%  | 30.4GB/s  | 7.60GB/s  |
| 4 KiB   | -    | 33.0GB/s  | 8.26GB/s  | page, 4 KiB |
| 8 KiB   | +11% | 29.8GB/s  | 7.44GB/s  |
| 16 KiB  | +20% | 27.5GB/s  | 6.88GB/s  |
| 32 KiB  | +25% | 26.5GB/s  | 6.62GB/s  |
| 64 KiB  | +25% | 26.3GB/s  | 6.59GB/s  |
| 128 KiB | +28% | 25.7GB/s  | 6.44GB/s  |
| 256 KiB | +27% | 25.9GB/s  | 6.48GB/s  |
| 512 KiB | +63% | 20.3GB/s  | 5.07GB/s  |
| 1 MiB   | +70% | 19.4GB/s  | 4.85GB/s  |
| 2 MiB   | +72% | 19.1GB/s  | 4.79GB/s  |
| 4 MiB   | +70% | 19.4GB/s  | 4.85GB/s  |
| 8 MiB   | +65% | 20.0GB/s  | 5.01GB/s  |
| 16 MiB  | +77% | 18.7GB/s  | 4.67GB/s  |
| 32 MiB  | +82% | 18.1GB/s  | 4.53GB/s  |
| 64 MiB  | +91% | 17.3GB/s  | 4.33GB/s  |
| 128 MiB | +92% | 17.2GB/s  | 4.30GB/s  |

**read from cache (SIMD xor v2)**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +18%  | 26.1GB/s  | 6.53GB/s  |
| 512 B   | +7%   | 28.9GB/s  | 7.21GB/s  |
| 1 KiB   | +3%   | 30.1GB/s  | 7.52GB/s  |
| 2 KiB   | +4%   | 29.8GB/s  | 7.44GB/s  |
| 4 KiB   | -     | 30.8GB/s  | 7.71GB/s  | page, 4 KiB |
| 8 KiB   | +6%   | 29.2GB/s  | 7.29GB/s  |
| 16 KiB  | -     | 30.8GB/s  | 7.71GB/s  |
| 32 KiB  | +19%  | 26.0GB/s  | 6.50GB/s  |
| 64 KiB  | +40%  | 22.1GB/s  | 5.53GB/s  |
| 128 KiB | +29%  | 23.9GB/s  | 5.97GB/s  |
| 256 KiB | +77%  | 17.5GB/s  | 4.37GB/s  |
| 512 KiB | +141% | 12.8GB/s  | 3.20GB/s  |
| 1 MiB   | +148% | 12.4GB/s  | 3.11GB/s  |
| 2 MiB   | +159% | 11.9GB/s  | 2.98GB/s  |
| 4 MiB   | +154% | 12.1GB/s  | 3.04GB/s  |
| 8 MiB   | +153% | 12.2GB/s  | 3.05GB/s  |
| 16 MiB  | +158% | 11.9GB/s  | 2.99GB/s  |
| 32 MiB  | +155% | 12.1GB/s  | 3.02GB/s  |
| 64 MiB  | +170% | 11.4GB/s  | 2.86GB/s  |
| 128 MiB | +157% | 12.0GB/s  | 3.00GB/s  |

</details>


# Snapdragon 888

* Device: Asus ROG Phone 5
* Memory: 8 GB, LPDDR5-6400, QC 16bit, 3200MHz, 51.2 GB/s

## High Performance core

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +25%  | 26.8GB/s  |
| 512 B   | +18%  | 28.6GB/s  |
| 1 KiB   | +10%  | 30.7GB/s  |
| 2 KiB   | +1%   | 33.2GB/s  |
| 4 KiB   | -     | 33.6GB/s  | page, 4 KiB |
| 8 KiB   | +3%   | 32.8GB/s  |
| 16 KiB  | +5%   | 32.1GB/s  |
| 32 KiB  | +11%  | 30.3GB/s  |
| 64 KiB  | +16%  | 28.9GB/s  |
| 128 KiB | +14%  | 29.6GB/s  |
| 256 KiB | +14%  | 29.5GB/s  |
| 512 KiB | +14%  | 29.4GB/s  |
| 1 MiB   | +43%  | 23.6GB/s  |
| 2 MiB   | +82%  | 18.4GB/s  |
| 4 MiB   | +139% | 14.1GB/s  |
| 8 MiB   | +161% | 12.9GB/s  |
| 16 MiB  | +141% | 14.0GB/s  |
| 32 MiB  | +143% | 13.9GB/s  |
| 64 MiB  | +133% | 14.4GB/s  |
| 128 MiB | +126% | 14.9GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +2%   | 46.6GB/s  |
| 512 B   | +5%   | 45.2GB/s  |
| 1 KiB   | -     | 47.3GB/s  |
| 2 KiB   | +3%   | 46.1GB/s  |
| 4 KiB   | +3%   | 45.8GB/s  | page, 4 KiB |
| 8 KiB   | +5%   | 45.1GB/s  |
| 16 KiB  | +6%   | 44.8GB/s  |
| 32 KiB  | +3%   | 46.0GB/s  |
| 64 KiB  | +20%  | 39.5GB/s  |
| 128 KiB | +10%  | 43.0GB/s  |
| 256 KiB | +8%   | 43.9GB/s  |
| 512 KiB | +6%   | 44.8GB/s  |
| 1 MiB   | +95%  | 24.2GB/s  |
| 2 MiB   | +145% | 19.3GB/s  |
| 4 MiB   | +221% | 14.7GB/s  |
| 8 MiB   | +264% | 13.0GB/s  |
| 16 MiB  | +234% | 14.2GB/s  |
| 32 MiB  | +235% | 14.1GB/s  |
| 64 MiB  | +214% | 15.1GB/s  |
| 128 MiB | +211% | 15.2GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | -     | 42.1GB/s  |
| 512 B   | +2%   | 41.2GB/s  |
| 1 KiB   | +2%   | 41.1GB/s  |
| 2 KiB   | +4%   | 40.5GB/s  |
| 4 KiB   | +4%   | 40.4GB/s  | page, 4 KiB |
| 8 KiB   | +2%   | 41.2GB/s  |
| 16 KiB  | +3%   | 40.9GB/s  |
| 32 KiB  | +5%   | 40.0GB/s  |
| 64 KiB  | +9%   | 38.5GB/s  |
| 128 KiB | +7%   | 39.4GB/s  |
| 256 KiB | +7%   | 39.3GB/s  |
| 512 KiB | +14%  | 36.8GB/s  |
| 1 MiB   | +76%  | 23.9GB/s  |
| 2 MiB   | +129% | 18.3GB/s  |
| 4 MiB   | +193% | 14.4GB/s  |
| 8 MiB   | +222% | 13.1GB/s  |
| 16 MiB  | +193% | 14.3GB/s  |
| 32 MiB  | +197% | 14.2GB/s  |
| 64 MiB  | +171% | 15.5GB/s  |
| 128 MiB | +178% | 15.2GB/s  |

</details>

<details><summary><b>memset, single thread</b></summary>

**memset**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +12%  | 76.5GB/s  |
| 512 B   | +4%   | 82.3GB/s  |
| 1 KiB   | +25%  | 68.9GB/s  |
| 2 KiB   | +17%  | 73.2GB/s  |
| 4 KiB   | +20%  | 71.3GB/s  | page, 4 KiB |
| 8 KiB   | +26%  | 68.4GB/s  |
| 16 KiB  | +22%  | 70.6GB/s  |
| 32 KiB  | +32%  | 65.0GB/s  |
| 64 KiB  | +21%  | 70.9GB/s  |
| 128 KiB | +14%  | 75.5GB/s  |
| 256 KiB | -     | 85.8GB/s  |
| 512 KiB | +9%   | 79.1GB/s  |
| 1 MiB   | +11%  | 77.1GB/s  |
| 2 MiB   | +57%  | 54.5GB/s  |
| 4 MiB   | +95%  | 43.9GB/s  |
| 8 MiB   | +124% | 38.4GB/s  |
| 16 MiB  | +172% | 31.6GB/s  |
| 32 MiB  | +153% | 34.0GB/s  |
| 64 MiB  | +156% | 33.5GB/s  |
| 128 MiB | +152% | 34.1GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | -     | 75.4GB/s  |
| 512 B   | -     | 75.7GB/s  |
| 1 KiB   | -     | 75.7GB/s  |
| 2 KiB   | -     | 75.7GB/s  |
| 4 KiB   | -     | 75.5GB/s  | page, 4 KiB |
| 8 KiB   | -     | 75.7GB/s  |
| 16 KiB  | -     | 75.7GB/s  |
| 32 KiB  | -     | 75.5GB/s  |
| 64 KiB  | -     | 75.4GB/s  |
| 128 KiB | -     | 75.4GB/s  |
| 256 KiB | +1%   | 75.1GB/s  |
| 512 KiB | +25%  | 60.4GB/s  |
| 1 MiB   | +38%  | 54.9GB/s  |
| 2 MiB   | +117% | 34.8GB/s  |
| 4 MiB   | +141% | 31.4GB/s  |
| 8 MiB   | +133% | 32.5GB/s  |
| 16 MiB  | +128% | 33.3GB/s  |
| 32 MiB  | +130% | 33.0GB/s  |
| 64 MiB  | +142% | 31.3GB/s  |
| 128 MiB | +139% | 31.7GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +8%   | 59.5GB/s  |
| 512 B   | +6%   | 60.7GB/s  |
| 1 KiB   | +2%   | 62.7GB/s  |
| 2 KiB   | -     | 64.0GB/s  |
| 4 KiB   | +3%   | 62.6GB/s  | page, 4 KiB |
| 8 KiB   | -     | 64.2GB/s  |
| 16 KiB  | +4%   | 61.5GB/s  |
| 32 KiB  | -     | 63.9GB/s  |
| 64 KiB  | +6%   | 60.4GB/s  |
| 128 KiB | +3%   | 62.1GB/s  |
| 256 KiB | +11%  | 57.8GB/s  |
| 512 KiB | +11%  | 57.8GB/s  |
| 1 MiB   | +46%  | 44.0GB/s  |
| 2 MiB   | +101% | 31.9GB/s  |
| 4 MiB   | +116% | 29.8GB/s  |
| 8 MiB   | +98%  | 32.5GB/s  |
| 16 MiB  | +112% | 30.3GB/s  |
| 32 MiB  | +103% | 31.7GB/s  |
| 64 MiB  | +101% | 32.0GB/s  |
| 128 MiB | +103% | 31.6GB/s  |

</details>

<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +71%  | 43.4GB/s  |
| 512 B   | +27%  | 58.5GB/s  |
| 1 KiB   | +1%   | 73.1GB/s  |
| 2 KiB   | -     | 74.1GB/s  |
| 4 KiB   | +3%   | 72.1GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 73.4GB/s  |
| 16 KiB  | +2%   | 72.6GB/s  |
| 32 KiB  | +16%  | 63.8GB/s  |
| 64 KiB  | +57%  | 47.3GB/s  |
| 128 KiB | +54%  | 48.2GB/s  |
| 256 KiB | +61%  | 45.9GB/s  |
| 512 KiB | +102% | 36.6GB/s  |
| 1 MiB   | +161% | 28.4GB/s  |
| 2 MiB   | +228% | 22.6GB/s  |
| 4 MiB   | +332% | 17.2GB/s  |
| 8 MiB   | +434% | 13.9GB/s  |
| 16 MiB  | +416% | 14.4GB/s  |
| 32 MiB  | +398% | 14.9GB/s  |
| 64 MiB  | +353% | 16.4GB/s  |
| 128 MiB | +346% | 16.6GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +38% | 15.6GB/s  |
| 512 B   | +21% | 17.7GB/s  |
| 1 KiB   | +14% | 18.8GB/s  |
| 2 KiB   | -    | 21.4GB/s  |
| 4 KiB   | +8%  | 19.9GB/s  | page, 4 KiB |
| 8 KiB   | +1%  | 21.1GB/s  |
| 16 KiB  | +1%  | 21.3GB/s  |
| 32 KiB  | +6%  | 20.2GB/s  |
| 64 KiB  | +3%  | 20.7GB/s  |
| 128 KiB | +5%  | 20.4GB/s  |
| 256 KiB | +6%  | 20.2GB/s  |
| 512 KiB | +4%  | 20.5GB/s  |
| 1 MiB   | +7%  | 19.9GB/s  |
| 2 MiB   | +27% | 16.8GB/s  |
| 4 MiB   | +4%  | 20.5GB/s  |
| 8 MiB   | +9%  | 19.5GB/s  |
| 16 MiB  | +15% | 18.6GB/s  |
| 32 MiB  | +11% | 19.2GB/s  |
| 64 MiB  | +11% | 19.3GB/s  |
| 128 MiB | +11% | 19.3GB/s  |

</details>


## Performance core

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +29%  | 28.2GB/s  |
| 512 B   | +14%  | 31.8GB/s  |
| 1 KiB   | +5%   | 34.4GB/s  |
| 2 KiB   | +2%   | 35.6GB/s  |
| 4 KiB   | +1%   | 35.9GB/s  | page, 4 KiB |
| 8 KiB   | -     | 36.2GB/s  |
| 16 KiB  | +10%  | 33.0GB/s  |
| 32 KiB  | +31%  | 27.6GB/s  |
| 64 KiB  | +10%  | 32.8GB/s  |
| 128 KiB | +10%  | 33.0GB/s  |
| 256 KiB | +13%  | 31.9GB/s  |
| 512 KiB | +32%  | 27.5GB/s  |
| 1 MiB   | +70%  | 21.3GB/s  |
| 2 MiB   | +97%  | 18.4GB/s  |
| 4 MiB   | +148% | 14.6GB/s  |
| 8 MiB   | +188% | 12.6GB/s  |
| 16 MiB  | +206% | 11.8GB/s  |
| 32 MiB  | +182% | 12.8GB/s  |
| 64 MiB  | +178% | 13.0GB/s  |
| 128 MiB | +168% | 13.5GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +1%   | 54.0GB/s  |
| 512 B   | -     | 54.7GB/s  |
| 1 KiB   | -     | 54.7GB/s  |
| 2 KiB   | -     | 54.7GB/s  |
| 4 KiB   | -     | 54.6GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 54.4GB/s  |
| 16 KiB  | +16%  | 47.0GB/s  |
| 32 KiB  | +52%  | 36.1GB/s  |
| 64 KiB  | +34%  | 40.9GB/s  |
| 128 KiB | +33%  | 41.2GB/s  |
| 256 KiB | +45%  | 37.8GB/s  |
| 512 KiB | +98%  | 27.6GB/s  |
| 1 MiB   | +159% | 21.1GB/s  |
| 2 MiB   | +197% | 18.4GB/s  |
| 4 MiB   | +271% | 14.7GB/s  |
| 8 MiB   | +336% | 12.6GB/s  |
| 16 MiB  | +360% | 11.9GB/s  |
| 32 MiB  | +321% | 13.0GB/s  |
| 64 MiB  | +315% | 13.2GB/s  |
| 128 MiB | +301% | 13.7GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | -     | 38.4GB/s  |
| 512 B   | -     | 38.4GB/s  |
| 1 KiB   | -     | 38.4GB/s  |
| 2 KiB   | -     | 38.3GB/s  |
| 4 KiB   | -     | 38.3GB/s  | page, 4 KiB |
| 8 KiB   | -     | 38.3GB/s  |
| 16 KiB  | +1%   | 37.9GB/s  |
| 32 KiB  | +9%   | 35.1GB/s  |
| 64 KiB  | +1%   | 37.8GB/s  |
| 128 KiB | +1%   | 37.9GB/s  |
| 256 KiB | +7%   | 36.0GB/s  |
| 512 KiB | +38%  | 27.8GB/s  |
| 1 MiB   | +80%  | 21.3GB/s  |
| 2 MiB   | +109% | 18.4GB/s  |
| 4 MiB   | +161% | 14.7GB/s  |
| 8 MiB   | +204% | 12.6GB/s  |
| 16 MiB  | +222% | 11.9GB/s  |
| 32 MiB  | +194% | 13.0GB/s  |
| 64 MiB  | +209% | 12.4GB/s  |
| 128 MiB | +185% | 13.5GB/s  |

</details>

<details><summary><b>memcpy, multithreading (3 threads, 3 cores)</b></summary>

**memcpy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +11%  | 71.0GB/s  | 23.7GB/s  |
| 512 B   | +4%   | 76.2GB/s  | 25.4GB/s  |
| 1 KiB   | -     | 78.8GB/s  | 26.3GB/s  |
| 2 KiB   | +6%   | 74.7GB/s  | 24.9GB/s  |
| 4 KiB   | +2%   | 77.4GB/s  | 25.8GB/s  | page, 4 KiB |
| 8 KiB   | +5%   | 75.2GB/s  | 25.1GB/s  |
| 16 KiB  | +4%   | 75.5GB/s  | 25.2GB/s  |
| 32 KiB  | +20%  | 65.7GB/s  | 21.9GB/s  |
| 64 KiB  | +28%  | 61.8GB/s  | 20.6GB/s  |
| 128 KiB | +7%   | 73.9GB/s  | 24.6GB/s  |
| 256 KiB | +29%  | 61.0GB/s  | 20.3GB/s  |
| 512 KiB | +94%  | 40.6GB/s  | 13.5GB/s  |
| 1 MiB   | +182% | 28.0GB/s  | 9.33GB/s  |
| 2 MiB   | +345% | 17.7GB/s  | 5.91GB/s  |
| 4 MiB   | +414% | 15.3GB/s  | 5.11GB/s  |
| 8 MiB   | +423% | 15.1GB/s  | 5.02GB/s  |
| 16 MiB  | +418% | 15.2GB/s  | 5.07GB/s  |
| 32 MiB  | +422% | 15.1GB/s  | 5.04GB/s  |
| 64 MiB  | +416% | 15.3GB/s  | 5.09GB/s  |
| 128 MiB | +428% | 14.9GB/s  | 4.98GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +4%   | 108.5GB/s  | 36.2GB/s  |
| 512 B   | +4%   | 108.6GB/s  | 36.2GB/s  |
| 1 KiB   | +16%  | 97.8GB/s   | 32.6GB/s  |
| 2 KiB   | +3%   | 109.2GB/s  | 36.4GB/s  |
| 4 KiB   | +1%   | 112.1GB/s  | 37.4GB/s  | page, 4 KiB |
| 8 KiB   | -     | 113.0GB/s  | 37.7GB/s  |
| 16 KiB  | -     | 112.9GB/s  | 37.6GB/s  |
| 32 KiB  | +66%  | 68.2GB/s   | 22.7GB/s  |
| 64 KiB  | +21%  | 93.5GB/s   | 31.2GB/s  |
| 128 KiB | +57%  | 71.8GB/s   | 23.9GB/s  |
| 256 KiB | +52%  | 74.6GB/s   | 24.9GB/s  |
| 512 KiB | +197% | 38.0GB/s   | 12.7GB/s  |
| 1 MiB   | +323% | 26.7GB/s   | 8.91GB/s  |
| 2 MiB   | +610% | 15.9GB/s   | 5.30GB/s  |
| 4 MiB   | +637% | 15.3GB/s   | 5.11GB/s  |
| 8 MiB   | +664% | 14.8GB/s   | 4.93GB/s  |
| 16 MiB  | +650% | 15.1GB/s   | 5.02GB/s  |
| 32 MiB  | +654% | 15.0GB/s   | 5.00GB/s  |
| 64 MiB  | +653% | 15.0GB/s   | 5.01GB/s  |
| 128 MiB | +654% | 15.0GB/s   | 5.00GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +17%  | 82.6GB/s  | 27.5GB/s  |
| 512 B   | -     | 96.3GB/s  | 32.1GB/s  |
| 1 KiB   | +13%  | 85.1GB/s  | 28.4GB/s  |
| 2 KiB   | +11%  | 86.8GB/s  | 28.9GB/s  |
| 4 KiB   | +2%   | 94.5GB/s  | 31.5GB/s  | page, 4 KiB |
| 8 KiB   | +9%   | 88.6GB/s  | 29.5GB/s  |
| 16 KiB  | +16%  | 82.7GB/s  | 27.6GB/s  |
| 32 KiB  | +29%  | 74.7GB/s  | 24.9GB/s  |
| 64 KiB  | +15%  | 83.8GB/s  | 27.9GB/s  |
| 128 KiB | +26%  | 76.3GB/s  | 25.4GB/s  |
| 256 KiB | +53%  | 63.0GB/s  | 21.0GB/s  |
| 512 KiB | +169% | 35.8GB/s  | 11.9GB/s  |
| 1 MiB   | +287% | 24.9GB/s  | 8.30GB/s  |
| 2 MiB   | +503% | 16.0GB/s  | 5.32GB/s  |
| 4 MiB   | +67%  | 14.4GB/s  | 4.81GB/s  |
| 8 MiB   | +66%  | 14.5GB/s  | 4.83GB/s  |
| 16 MiB  | +62%  | 14.9GB/s  | 4.96GB/s  |
| 32 MiB  | +62%  | 14.9GB/s  | 4.97GB/s  |
| 64 MiB  | +60%  | 15.0GB/s  | 5.02GB/s  |
| 128 MiB | +62%  | 14.9GB/s  | 4.95GB/s  |

</details>

<details><summary><b>memset, single thread</b></summary>

**memset**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +21%  | 65.2GB/s  |
| 512 B   | +7%   | 73.7GB/s  |
| 1 KiB   | +2%   | 76.9GB/s  |
| 2 KiB   | -     | 78.5GB/s  |
| 4 KiB   | -     | 78.8GB/s  | page, 4 KiB |
| 8 KiB   | -     | 78.8GB/s  |
| 16 KiB  | -     | 78.8GB/s  |
| 32 KiB  | +1%   | 78.3GB/s  |
| 64 KiB  | +1%   | 78.3GB/s  |
| 128 KiB | +1%   | 78.4GB/s  |
| 256 KiB | +1%   | 78.4GB/s  |
| 512 KiB | +110% | 37.5GB/s  |
| 1 MiB   | +165% | 29.7GB/s  |
| 2 MiB   | +192% | 27.0GB/s  |
| 4 MiB   | +223% | 24.4GB/s  |
| 8 MiB   | +238% | 23.3GB/s  |
| 16 MiB  | +159% | 30.4GB/s  |
| 32 MiB  | +124% | 35.3GB/s  |
| 64 MiB  | +124% | 35.1GB/s  |
| 128 MiB | +124% | 35.2GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +1%   | 76.0GB/s  |
| 512 B   | -     | 76.4GB/s  |
| 1 KiB   | -     | 76.6GB/s  |
| 2 KiB   | -     | 76.6GB/s  |
| 4 KiB   | -     | 76.7GB/s  | page, 4 KiB |
| 8 KiB   | -     | 76.7GB/s  |
| 16 KiB  | -     | 76.7GB/s  |
| 32 KiB  | +1%   | 75.8GB/s  |
| 64 KiB  | +1%   | 75.8GB/s  |
| 128 KiB | +1%   | 75.7GB/s  |
| 256 KiB | +2%   | 75.5GB/s  |
| 512 KiB | +118% | 35.2GB/s  |
| 1 MiB   | +306% | 18.9GB/s  |
| 2 MiB   | +245% | 22.3GB/s  |
| 4 MiB   | +214% | 24.4GB/s  |
| 8 MiB   | +239% | 22.6GB/s  |
| 16 MiB  | +232% | 23.1GB/s  |
| 32 MiB  | +196% | 25.9GB/s  |
| 64 MiB  | +117% | 35.3GB/s  |
| 128 MiB | +118% | 35.2GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +2%   | 75.2GB/s  |
| 512 B   | -     | 76.7GB/s  |
| 1 KiB   | -     | 76.7GB/s  |
| 2 KiB   | -     | 76.7GB/s  |
| 4 KiB   | -     | 76.7GB/s  | page, 4 KiB |
| 8 KiB   | -     | 76.7GB/s  |
| 16 KiB  | -     | 76.7GB/s  |
| 32 KiB  | +1%   | 75.6GB/s  |
| 64 KiB  | +1%   | 75.7GB/s  |
| 128 KiB | +1%   | 75.7GB/s  |
| 256 KiB | +5%   | 73.4GB/s  |
| 512 KiB | +112% | 36.3GB/s  |
| 1 MiB   | +336% | 17.6GB/s  |
| 2 MiB   | +364% | 16.5GB/s  |
| 4 MiB   | +219% | 24.0GB/s  |
| 8 MiB   | +251% | 21.8GB/s  |
| 16 MiB  | +226% | 23.5GB/s  |
| 32 MiB  | +207% | 25.0GB/s  |
| 64 MiB  | +117% | 35.3GB/s  |
| 128 MiB | +118% | 35.2GB/s  |

</details>

<details><summary><b>memset, multithreading (3 threads, 3 cores)</b></summary>

**memset**

| block   | diff  | bandwidth  | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +39%  | 124.4GB/s  | 41.5GB/s  |
| 512 B   | +42%  | 121.7GB/s  | 40.6GB/s  |
| 1 KiB   | +30%  | 132.6GB/s  | 44.2GB/s  |
| 2 KiB   | +10%  | 156.7GB/s  | 52.2GB/s  |
| 4 KiB   | +1%   | 170.5GB/s  | 56.8GB/s  | page, 4 KiB |
| 8 KiB   | +4%   | 165.9GB/s  | 55.3GB/s  |
| 16 KiB  | +6%   | 163.7GB/s  | 54.6GB/s  |
| 32 KiB  | -     | 172.1GB/s  | 57.4GB/s  |
| 64 KiB  | -     | 172.8GB/s  | 57.6GB/s  |
| 128 KiB | +3%   | 167.7GB/s  | 55.9GB/s  |
| 256 KiB | +125% | 76.9GB/s   | 25.6GB/s  |
| 512 KiB | +247% | 49.8GB/s   | 16.6GB/s  |
| 1 MiB   | +260% | 48.0GB/s   | 16.0GB/s  |
| 2 MiB   | +289% | 44.4GB/s   | 14.8GB/s  |
| 4 MiB   | +308% | 42.4GB/s   | 14.1GB/s  |
| 8 MiB   | +350% | 38.4GB/s   | 12.8GB/s  |
| 16 MiB  | +363% | 37.3GB/s   | 12.4GB/s  |
| 32 MiB  | +401% | 34.5GB/s   | 11.5GB/s  |
| 64 MiB  | +397% | 34.8GB/s   | 11.6GB/s  |
| 128 MiB | +412% | 33.7GB/s   | 11.2GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth  | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +3%   | 169.2GB/s  | 56.4GB/s  |
| 512 B   | +1%   | 172.5GB/s  | 57.5GB/s  |
| 1 KiB   | +6%   | 164.6GB/s  | 54.9GB/s  |
| 2 KiB   | +5%   | 166.4GB/s  | 55.5GB/s  |
| 4 KiB   | +1%   | 173.2GB/s  | 57.7GB/s  | page, 4 KiB |
| 8 KiB   | +2%   | 172.1GB/s  | 57.4GB/s  |
| 16 KiB  | +2%   | 170.7GB/s  | 56.9GB/s  |
| 32 KiB  | +4%   | 167.7GB/s  | 55.9GB/s  |
| 64 KiB  | -     | 175.0GB/s  | 58.3GB/s  |
| 128 KiB | +6%   | 164.9GB/s  | 55.0GB/s  |
| 256 KiB | +166% | 65.7GB/s   | 21.9GB/s  |
| 512 KiB | +260% | 48.6GB/s   | 16.2GB/s  |
| 1 MiB   | +256% | 49.1GB/s   | 16.4GB/s  |
| 2 MiB   | +369% | 37.3GB/s   | 12.4GB/s  |
| 4 MiB   | +321% | 41.5GB/s   | 13.8GB/s  |
| 8 MiB   | +337% | 40.1GB/s   | 13.4GB/s  |
| 16 MiB  | +396% | 35.3GB/s   | 11.8GB/s  |
| 32 MiB  | +393% | 35.5GB/s   | 11.8GB/s  |
| 64 MiB  | +414% | 34.0GB/s   | 11.3GB/s  |
| 128 MiB | +400% | 35.0GB/s   | 11.7GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth  | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +25%  | 163.1GB/s  | 54.4GB/s  |
| 512 B   | +10%  | 184.3GB/s  | 61.4GB/s  |
| 1 KiB   | +11%  | 183.2GB/s  | 61.1GB/s  |
| 2 KiB   | +11%  | 182.7GB/s  | 60.9GB/s  |
| 4 KiB   | +13%  | 180.1GB/s  | 60.0GB/s  | page, 4 KiB |
| 8 KiB   | +6%   | 191.3GB/s  | 63.8GB/s  |
| 16 KiB  | +12%  | 182.1GB/s  | 60.7GB/s  |
| 32 KiB  | -     | 203.6GB/s  | 67.9GB/s  |
| 64 KiB  | +21%  | 168.0GB/s  | 56.0GB/s  |
| 128 KiB | +14%  | 178.9GB/s  | 59.6GB/s  |
| 256 KiB | +118% | 93.3GB/s   | 31.1GB/s  |
| 512 KiB | +288% | 52.5GB/s   | 17.5GB/s  |
| 1 MiB   | +355% | 44.8GB/s   | 14.9GB/s  |
| 2 MiB   | +445% | 37.3GB/s   | 12.4GB/s  |
| 4 MiB   | +41%  | 36.1GB/s   | 12.0GB/s  |
| 8 MiB   | +29%  | 39.6GB/s   | 13.2GB/s  |
| 16 MiB  | +36%  | 37.4GB/s   | 12.5GB/s  |
| 32 MiB  | +43%  | 35.5GB/s   | 11.8GB/s  |
| 64 MiB  | +49%  | 34.1GB/s   | 11.4GB/s  |
| 128 MiB | +43%  | 35.6GB/s   | 11.9GB/s  |

</details>

<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +39%  | 48.8GB/s  |
| 512 B   | +4%   | 65.3GB/s  |
| 1 KiB   | +1%   | 67.2GB/s  |
| 2 KiB   | -     | 67.5GB/s  |
| 4 KiB   | -     | 67.6GB/s  | page, 4 KiB |
| 8 KiB   | -     | 67.6GB/s  |
| 16 KiB  | +21%  | 56.0GB/s  |
| 32 KiB  | +53%  | 44.1GB/s  |
| 64 KiB  | +42%  | 47.5GB/s  |
| 128 KiB | +41%  | 48.1GB/s  |
| 256 KiB | +89%  | 35.9GB/s  |
| 512 KiB | +121% | 30.6GB/s  |
| 1 MiB   | +162% | 25.8GB/s  |
| 2 MiB   | +187% | 23.5GB/s  |
| 4 MiB   | +302% | 16.8GB/s  |
| 8 MiB   | +395% | 13.7GB/s  |
| 16 MiB  | +402% | 13.5GB/s  |
| 32 MiB  | +411% | 13.2GB/s  |
| 64 MiB  | +361% | 14.7GB/s  |
| 128 MiB | +341% | 15.3GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +25% | 20.5GB/s  |
| 512 B   | +12% | 22.7GB/s  |
| 1 KiB   | +6%  | 24.1GB/s  |
| 2 KiB   | +3%  | 24.8GB/s  |
| 4 KiB   | +1%  | 25.2GB/s  | page, 4 KiB |
| 8 KiB   | +1%  | 25.4GB/s  |
| 16 KiB  | -    | 25.5GB/s  |
| 32 KiB  | -    | 25.5GB/s  |
| 64 KiB  | -    | 25.5GB/s  |
| 128 KiB | -    | 25.5GB/s  |
| 256 KiB | -    | 25.5GB/s  |
| 512 KiB | +1%  | 25.4GB/s  |
| 1 MiB   | +1%  | 25.4GB/s  |
| 2 MiB   | +4%  | 24.6GB/s  |
| 4 MiB   | +1%  | 25.4GB/s  |
| 8 MiB   | +15% | 22.2GB/s  |
| 16 MiB  | +22% | 20.9GB/s  |
| 32 MiB  | +24% | 20.6GB/s  |
| 64 MiB  | +25% | 20.5GB/s  |
| 128 MiB | +25% | 20.4GB/s  |

</details>

<details><summary><b>other, multithreading (3 threads, 3 cores)</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth  | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +128% | 73.6GB/s   | 24.5GB/s  |
| 512 B   | +45%  | 115.8GB/s  | 38.6GB/s  |
| 1 KiB   | +15%  | 146.4GB/s  | 48.8GB/s  |
| 2 KiB   | -     | 167.7GB/s  | 55.9GB/s  |
| 4 KiB   | +1%   | 166.8GB/s  | 55.6GB/s  | page, 4 KiB |
| 8 KiB   | +12%  | 150.3GB/s  | 50.1GB/s  |
| 16 KiB  | +100% | 83.7GB/s   | 27.9GB/s  |
| 32 KiB  | +127% | 74.0GB/s   | 24.7GB/s  |
| 64 KiB  | +183% | 59.2GB/s   | 19.7GB/s  |
| 128 KiB | +123% | 75.0GB/s   | 25.0GB/s  |
| 256 KiB | +227% | 51.3GB/s   | 17.1GB/s  |
| 512 KiB | +256% | 47.1GB/s   | 15.7GB/s  |
| 1 MiB   | +310% | 40.9GB/s   | 13.6GB/s  |
| 2 MiB   | +510% | 27.5GB/s   | 9.17GB/s  |
| 4 MiB   | +689% | 21.2GB/s   | 7.08GB/s  |
| 8 MiB   | +739% | 20.0GB/s   | 6.66GB/s  |
| 16 MiB  | +719% | 20.5GB/s   | 6.82GB/s  |
| 32 MiB  | +728% | 20.3GB/s   | 6.75GB/s  |
| 64 MiB  | +736% | 20.0GB/s   | 6.68GB/s  |
| 128 MiB | +759% | 19.5GB/s   | 6.51GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff  | bandwidth  | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +9%   | 104.3GB/s  | 34.8GB/s  |
| 512 B   | -     | 113.9GB/s  | 38.0GB/s  |
| 1 KiB   | +46%  | 78.2GB/s   | 26.1GB/s  |
| 2 KiB   | +68%  | 67.8GB/s   | 22.6GB/s  |
| 4 KiB   | +84%  | 61.8GB/s   | 20.6GB/s  | page, 4 KiB |
| 8 KiB   | +71%  | 66.7GB/s   | 22.2GB/s  |
| 16 KiB  | +132% | 49.0GB/s   | 16.3GB/s  |
| 32 KiB  | +124% | 50.9GB/s   | 17.0GB/s  |
| 64 KiB  | +131% | 49.4GB/s   | 16.5GB/s  |
| 128 KiB | +125% | 50.8GB/s   | 16.9GB/s  |
| 256 KiB | +136% | 48.3GB/s   | 16.1GB/s  |
| 512 KiB | +149% | 45.7GB/s   | 15.2GB/s  |
| 1 MiB   | +150% | 45.6GB/s   | 15.2GB/s  |
| 2 MiB   | +160% | 43.8GB/s   | 14.6GB/s  |
| 4 MiB   | +231% | 34.5GB/s   | 11.5GB/s  |
| 8 MiB   | +255% | 32.1GB/s   | 10.7GB/s  |
| 16 MiB  | +210% | 36.7GB/s   | 12.2GB/s  |
| 32 MiB  | +226% | 35.0GB/s   | 11.7GB/s  |
| 64 MiB  | +233% | 34.2GB/s   | 11.4GB/s  |
| 128 MiB | +249% | 32.6GB/s   | 10.9GB/s  |

</details>


## Energy Efficient core

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +36%  | 7.08GB/s  |
| 512 B   | +16%  | 8.35GB/s  |
| 1 KiB   | +5%   | 9.22GB/s  |
| 2 KiB   | -     | 9.62GB/s  |
| 4 KiB   | +1%   | 9.57GB/s  | page, 4 KiB |
| 8 KiB   | -     | 9.66GB/s  |
| 16 KiB  | +33%  | 7.26GB/s  |
| 32 KiB  | +28%  | 7.52GB/s  |
| 64 KiB  | +28%  | 7.56GB/s  |
| 128 KiB | +29%  | 7.47GB/s  |
| 256 KiB | +29%  | 7.46GB/s  |
| 512 KiB | +28%  | 7.53GB/s  |
| 1 MiB   | +28%  | 7.52GB/s  |
| 2 MiB   | +30%  | 7.44GB/s  |
| 4 MiB   | +129% | 4.22GB/s  |
| 8 MiB   | +173% | 3.54GB/s  |
| 16 MiB  | +182% | 3.43GB/s  |
| 32 MiB  | +183% | 3.41GB/s  |
| 64 MiB  | +184% | 3.40GB/s  |
| 128 MiB | +184% | 3.40GB/s  |

**SIMD cached copy**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +6%  | 6.10GB/s  |
| 512 B   | +1%  | 6.36GB/s  |
| 1 KiB   | -    | 6.44GB/s  |
| 2 KiB   | +3%  | 6.24GB/s  |
| 4 KiB   | +6%  | 6.08GB/s  | page, 4 KiB |
| 8 KiB   | +4%  | 6.18GB/s  |
| 16 KiB  | +5%  | 6.16GB/s  |
| 32 KiB  | +8%  | 5.98GB/s  |
| 64 KiB  | +8%  | 5.95GB/s  |
| 128 KiB | +7%  | 6.00GB/s  |
| 256 KiB | +6%  | 6.09GB/s  |
| 512 KiB | +5%  | 6.14GB/s  |
| 1 MiB   | +5%  | 6.14GB/s  |
| 2 MiB   | +9%  | 5.91GB/s  |
| 4 MiB   | +58% | 4.07GB/s  |
| 8 MiB   | +83% | 3.51GB/s  |
| 16 MiB  | +90% | 3.40GB/s  |
| 32 MiB  | +91% | 3.38GB/s  |
| 64 MiB  | +91% | 3.37GB/s  |
| 128 MiB | +91% | 3.37GB/s  |

**SIMD non-cached copy**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +7%  | 6.19GB/s  |
| 512 B   | +3%  | 6.41GB/s  |
| 1 KiB   | +2%  | 6.45GB/s  |
| 2 KiB   | +1%  | 6.55GB/s  |
| 4 KiB   | -    | 6.59GB/s  | page, 4 KiB |
| 8 KiB   | -    | 6.59GB/s  |
| 16 KiB  | -    | 6.59GB/s  |
| 32 KiB  | +10% | 6.00GB/s  |
| 64 KiB  | +12% | 5.91GB/s  |
| 128 KiB | +17% | 5.61GB/s  |
| 256 KiB | +18% | 5.57GB/s  |
| 512 KiB | +17% | 5.62GB/s  |
| 1 MiB   | +17% | 5.64GB/s  |
| 2 MiB   | +18% | 5.60GB/s  |
| 4 MiB   | +77% | 3.73GB/s  |
| 8 MiB   | +90% | 3.46GB/s  |
| 16 MiB  | +97% | 3.35GB/s  |
| 32 MiB  | +98% | 3.34GB/s  |
| 64 MiB  | +98% | 3.33GB/s  |
| 128 MiB | +98% | 3.33GB/s  |

</details>

<details><summary><b>memcpy, multithreading (4 threads, 4 cores)</b></summary>

**memcpy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +39%  | 26.6GB/s  | 6.65GB/s  |
| 512 B   | +18%  | 31.3GB/s  | 7.82GB/s  |
| 1 KiB   | +6%   | 35.0GB/s  | 8.75GB/s  |
| 2 KiB   | +1%   | 36.7GB/s  | 9.18GB/s  |
| 4 KiB   | -     | 36.8GB/s  | 9.21GB/s  | page, 4 KiB |
| 8 KiB   | -     | 36.9GB/s  | 9.23GB/s  |
| 16 KiB  | +31%  | 28.1GB/s  | 7.03GB/s  |
| 32 KiB  | +27%  | 29.0GB/s  | 7.26GB/s  |
| 64 KiB  | +26%  | 29.3GB/s  | 7.33GB/s  |
| 128 KiB | +29%  | 28.5GB/s  | 7.13GB/s  |
| 256 KiB | +56%  | 23.7GB/s  | 5.93GB/s  |
| 512 KiB | +69%  | 21.8GB/s  | 5.45GB/s  |
| 1 MiB   | +182% | 13.1GB/s  | 3.27GB/s  |
| 2 MiB   | +220% | 11.5GB/s  | 2.88GB/s  |
| 4 MiB   | +207% | 12.0GB/s  | 3.01GB/s  |
| 8 MiB   | +209% | 12.0GB/s  | 2.99GB/s  |
| 16 MiB  | +210% | 11.9GB/s  | 2.98GB/s  |
| 32 MiB  | +210% | 11.9GB/s  | 2.97GB/s  |
| 64 MiB  | +212% | 11.8GB/s  | 2.96GB/s  |
| 128 MiB | +216% | 11.7GB/s  | 2.92GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +7%   | 23.2GB/s  | 5.80GB/s  |
| 512 B   | +1%   | 24.5GB/s  | 6.12GB/s  |
| 1 KiB   | -     | 24.8GB/s  | 6.21GB/s  |
| 2 KiB   | +2%   | 24.4GB/s  | 6.09GB/s  |
| 4 KiB   | +2%   | 24.4GB/s  | 6.11GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 24.5GB/s  | 6.13GB/s  |
| 16 KiB  | +5%   | 23.6GB/s  | 5.90GB/s  |
| 32 KiB  | +6%   | 23.4GB/s  | 5.86GB/s  |
| 64 KiB  | +6%   | 23.3GB/s  | 5.84GB/s  |
| 128 KiB | +6%   | 23.5GB/s  | 5.86GB/s  |
| 256 KiB | +9%   | 22.8GB/s  | 5.69GB/s  |
| 512 KiB | +17%  | 21.3GB/s  | 5.33GB/s  |
| 1 MiB   | +82%  | 13.6GB/s  | 3.41GB/s  |
| 2 MiB   | +130% | 10.8GB/s  | 2.70GB/s  |
| 4 MiB   | +131% | 10.7GB/s  | 2.68GB/s  |
| 8 MiB   | +132% | 10.7GB/s  | 2.68GB/s  |
| 16 MiB  | +130% | 10.8GB/s  | 2.70GB/s  |
| 32 MiB  | +112% | 11.7GB/s  | 2.93GB/s  |
| 64 MiB  | +109% | 11.9GB/s  | 2.97GB/s  |
| 128 MiB | +110% | 11.8GB/s  | 2.96GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +75%  | 24.2GB/s  | 6.05GB/s  |
| 512 B   | +69%  | 25.0GB/s  | 6.25GB/s  |
| 1 KiB   | +66%  | 25.5GB/s  | 6.37GB/s  |
| 2 KiB   | +65%  | 25.7GB/s  | 6.43GB/s  |
| 4 KiB   | +63%  | 25.9GB/s  | 6.48GB/s  | page, 4 KiB |
| 8 KiB   | +64%  | 25.8GB/s  | 6.44GB/s  |
| 16 KiB  | +63%  | 26.0GB/s  | 6.50GB/s  |
| 32 KiB  | +79%  | 23.6GB/s  | 5.91GB/s  |
| 64 KiB  | +83%  | 23.1GB/s  | 5.76GB/s  |
| 128 KiB | +103% | 20.9GB/s  | 5.22GB/s  |
| 256 KiB | +138% | 17.8GB/s  | 4.45GB/s  |
| 512 KiB | +150% | 16.9GB/s  | 4.22GB/s  |
| 1 MiB   | +263% | 11.6GB/s  | 2.91GB/s  |
| 2 MiB   | +293% | 10.8GB/s  | 2.69GB/s  |
| 4 MiB   | -     | 10.6GB/s  | 2.64GB/s  |
| 8 MiB   | +1%   | 10.4GB/s  | 2.61GB/s  |
| 16 MiB  | +1%   | 10.5GB/s  | 2.62GB/s  |
| 32 MiB  | +1%   | 10.4GB/s  | 2.61GB/s  |
| 64 MiB  | +1%   | 10.5GB/s  | 2.62GB/s  |
| 128 MiB | +1%   | 10.4GB/s  | 2.61GB/s  |

</details>

<details><summary><b>memset, single thread</b></summary>

**memset**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +66%  | 13.0GB/s  |
| 512 B   | +31%  | 16.4GB/s  |
| 1 KiB   | +20%  | 17.9GB/s  |
| 2 KiB   | +7%   | 20.1GB/s  |
| 4 KiB   | +2%   | 21.1GB/s  | page, 4 KiB |
| 8 KiB   | -     | 21.5GB/s  |
| 16 KiB  | +9%   | 19.8GB/s  |
| 32 KiB  | +15%  | 18.7GB/s  |
| 64 KiB  | +30%  | 16.5GB/s  |
| 128 KiB | +51%  | 14.2GB/s  |
| 256 KiB | +78%  | 12.1GB/s  |
| 512 KiB | +95%  | 11.0GB/s  |
| 1 MiB   | +105% | 10.5GB/s  |
| 2 MiB   | +110% | 10.2GB/s  |
| 4 MiB   | +109% | 10.3GB/s  |
| 8 MiB   | +113% | 10.1GB/s  |
| 16 MiB  | +112% | 10.1GB/s  |
| 32 MiB  | +118% | 9.86GB/s  |
| 64 MiB  | +113% | 10.1GB/s  |
| 128 MiB | +99%  | 10.8GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +11% | 16.7GB/s  |
| 512 B   | +5%  | 17.6GB/s  |
| 1 KiB   | +4%  | 18.0GB/s  |
| 2 KiB   | +1%  | 18.4GB/s  |
| 4 KiB   | -    | 18.6GB/s  | page, 4 KiB |
| 8 KiB   | -    | 18.5GB/s  |
| 16 KiB  | +6%  | 17.5GB/s  |
| 32 KiB  | +11% | 16.8GB/s  |
| 64 KiB  | +21% | 15.4GB/s  |
| 128 KiB | +40% | 13.3GB/s  |
| 256 KiB | +58% | 11.8GB/s  |
| 512 KiB | +73% | 10.8GB/s  |
| 1 MiB   | +68% | 11.1GB/s  |
| 2 MiB   | +85% | 10.0GB/s  |
| 4 MiB   | +69% | 11.0GB/s  |
| 8 MiB   | +78% | 10.5GB/s  |
| 16 MiB  | +80% | 10.3GB/s  |
| 32 MiB  | +88% | 9.91GB/s  |
| 64 MiB  | +79% | 10.4GB/s  |
| 128 MiB | +88% | 9.87GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +14%  | 18.0GB/s  |
| 512 B   | +7%   | 19.1GB/s  |
| 1 KiB   | +5%   | 19.4GB/s  |
| 2 KiB   | +2%   | 19.9GB/s  |
| 4 KiB   | +1%   | 20.3GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 20.1GB/s  |
| 16 KiB  | -     | 20.3GB/s  |
| 32 KiB  | -     | 20.4GB/s  |
| 64 KiB  | -     | 20.3GB/s  |
| 128 KiB | +92%  | 10.6GB/s  |
| 256 KiB | +291% | 5.22GB/s  |
| 512 KiB | +355% | 4.48GB/s  |
| 1 MiB   | +348% | 4.55GB/s  |
| 2 MiB   | +345% | 4.59GB/s  |
| 4 MiB   | +282% | 5.35GB/s  |
| 8 MiB   | +323% | 4.83GB/s  |
| 16 MiB  | +336% | 4.68GB/s  |
| 32 MiB  | +348% | 4.55GB/s  |
| 64 MiB  | +343% | 4.61GB/s  |
| 128 MiB | +361% | 4.42GB/s  |

</details>

<details><summary><b>memset, multithreading (4 threads, 4 cores)</b></summary>

**memset**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +61%  | 45.0GB/s  | 11.2GB/s  |
| 512 B   | +24%  | 58.0GB/s  | 14.5GB/s  |
| 1 KiB   | +7%   | 67.2GB/s  | 16.8GB/s  |
| 2 KiB   | -     | 72.2GB/s  | 18.1GB/s  |
| 4 KiB   | +4%   | 69.7GB/s  | 17.4GB/s  | page, 4 KiB |
| 8 KiB   | +20%  | 60.4GB/s  | 15.1GB/s  |
| 16 KiB  | +44%  | 50.2GB/s  | 12.6GB/s  |
| 32 KiB  | +102% | 35.7GB/s  | 8.93GB/s  |
| 64 KiB  | +159% | 27.9GB/s  | 6.97GB/s  |
| 128 KiB | +190% | 24.9GB/s  | 6.21GB/s  |
| 256 KiB | +216% | 22.8GB/s  | 5.71GB/s  |
| 512 KiB | +228% | 22.0GB/s  | 5.51GB/s  |
| 1 MiB   | +266% | 19.7GB/s  | 4.93GB/s  |
| 2 MiB   | +265% | 19.8GB/s  | 4.95GB/s  |
| 4 MiB   | +241% | 21.2GB/s  | 5.29GB/s  |
| 8 MiB   | +226% | 22.2GB/s  | 5.54GB/s  |
| 16 MiB  | +222% | 22.4GB/s  | 5.60GB/s  |
| 32 MiB  | +220% | 22.6GB/s  | 5.64GB/s  |
| 64 MiB  | +218% | 22.7GB/s  | 5.68GB/s  |
| 128 MiB | +226% | 22.2GB/s  | 5.54GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +26%  | 54.3GB/s  | 13.6GB/s  |
| 512 B   | +11%  | 61.4GB/s  | 15.4GB/s  |
| 1 KiB   | +16%  | 58.7GB/s  | 14.7GB/s  |
| 2 KiB   | +10%  | 62.2GB/s  | 15.6GB/s  |
| 4 KiB   | -     | 68.3GB/s  | 17.1GB/s  | page, 4 KiB |
| 8 KiB   | +18%  | 58.0GB/s  | 14.5GB/s  |
| 16 KiB  | +48%  | 46.1GB/s  | 11.5GB/s  |
| 32 KiB  | +87%  | 36.4GB/s  | 9.10GB/s  |
| 64 KiB  | +126% | 30.1GB/s  | 7.54GB/s  |
| 128 KiB | +194% | 23.2GB/s  | 5.81GB/s  |
| 256 KiB | +233% | 20.5GB/s  | 5.12GB/s  |
| 512 KiB | +227% | 20.9GB/s  | 5.22GB/s  |
| 1 MiB   | +261% | 18.9GB/s  | 4.73GB/s  |
| 2 MiB   | +301% | 17.0GB/s  | 4.25GB/s  |
| 4 MiB   | +304% | 16.9GB/s  | 4.23GB/s  |
| 8 MiB   | +278% | 18.1GB/s  | 4.52GB/s  |
| 16 MiB  | +292% | 17.4GB/s  | 4.35GB/s  |
| 32 MiB  | +289% | 17.6GB/s  | 4.39GB/s  |
| 64 MiB  | +289% | 17.6GB/s  | 4.39GB/s  |
| 128 MiB | +294% | 17.3GB/s  | 4.33GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +35%  | 58.2GB/s  | 14.5GB/s  |
| 512 B   | +17%  | 67.3GB/s  | 16.8GB/s  |
| 1 KiB   | +10%  | 71.5GB/s  | 17.9GB/s  |
| 2 KiB   | +4%   | 75.7GB/s  | 18.9GB/s  |
| 4 KiB   | +1%   | 78.2GB/s  | 19.5GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 78.2GB/s  | 19.5GB/s  |
| 16 KiB  | -     | 78.8GB/s  | 19.7GB/s  |
| 32 KiB  | +94%  | 40.5GB/s  | 10.1GB/s  |
| 64 KiB  | +289% | 20.2GB/s  | 5.06GB/s  |
| 128 KiB | +423% | 15.1GB/s  | 3.76GB/s  |
| 256 KiB | +369% | 16.8GB/s  | 4.20GB/s  |
| 512 KiB | +354% | 17.3GB/s  | 4.34GB/s  |
| 1 MiB   | +472% | 13.8GB/s  | 3.44GB/s  |
| 2 MiB   | +446% | 14.4GB/s  | 3.60GB/s  |
| 4 MiB   | +27%  | 15.5GB/s  | 3.88GB/s  |
| 8 MiB   | +30%  | 15.1GB/s  | 3.78GB/s  |
| 16 MiB  | +29%  | 15.3GB/s  | 3.82GB/s  |
| 32 MiB  | +30%  | 15.1GB/s  | 3.78GB/s  |
| 64 MiB  | +28%  | 15.3GB/s  | 3.84GB/s  |
| 128 MiB | +30%  | 15.1GB/s  | 3.78GB/s  |

</details>

<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +3%  | 6.93GB/s  |
| 512 B   | +2%  | 7.00GB/s  |
| 1 KiB   | -    | 7.15GB/s  |
| 2 KiB   | -    | 7.18GB/s  |
| 4 KiB   | -    | 7.14GB/s  | page, 4 KiB |
| 8 KiB   | +3%  | 6.95GB/s  |
| 16 KiB  | +4%  | 6.91GB/s  |
| 32 KiB  | +13% | 6.36GB/s  |
| 64 KiB  | +16% | 6.21GB/s  |
| 128 KiB | +20% | 5.96GB/s  |
| 256 KiB | +22% | 5.90GB/s  |
| 512 KiB | +21% | 5.91GB/s  |
| 1 MiB   | +21% | 5.91GB/s  |
| 2 MiB   | +22% | 5.88GB/s  |
| 4 MiB   | +49% | 4.82GB/s  |
| 8 MiB   | +69% | 4.24GB/s  |
| 16 MiB  | +77% | 4.06GB/s  |
| 32 MiB  | +77% | 4.04GB/s  |
| 64 MiB  | +78% | 4.03GB/s  |
| 128 MiB | +79% | 4.02GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +6%  | 8.18GB/s  |
| 512 B   | +2%  | 8.43GB/s  |
| 1 KiB   | +1%  | 8.52GB/s  |
| 2 KiB   | -    | 8.60GB/s  |
| 4 KiB   | -    | 8.63GB/s  | page, 4 KiB |
| 8 KiB   | -    | 8.61GB/s  |
| 16 KiB  | -    | 8.64GB/s  |
| 32 KiB  | -    | 8.64GB/s  |
| 64 KiB  | +29% | 6.68GB/s  |
| 128 KiB | +34% | 6.44GB/s  |
| 256 KiB | +38% | 6.24GB/s  |
| 512 KiB | +42% | 6.08GB/s  |
| 1 MiB   | +42% | 6.09GB/s  |
| 2 MiB   | +42% | 6.09GB/s  |
| 4 MiB   | +41% | 6.11GB/s  |
| 8 MiB   | +65% | 5.23GB/s  |
| 16 MiB  | +86% | 4.65GB/s  |
| 32 MiB  | +90% | 4.54GB/s  |
| 64 MiB  | +91% | 4.51GB/s  |
| 128 MiB | +92% | 4.49GB/s  |

</details>

<details><summary><b>other, multithreading (4 threads, 4 cores)</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +6%   | 26.2GB/s  | 6.54GB/s  |
| 512 B   | +3%   | 27.0GB/s  | 6.74GB/s  |
| 1 KiB   | -     | 27.7GB/s  | 6.92GB/s  |
| 2 KiB   | -     | 27.7GB/s  | 6.92GB/s  |
| 4 KiB   | +1%   | 27.4GB/s  | 6.84GB/s  | page, 4 KiB |
| 8 KiB   | -     | 27.6GB/s  | 6.90GB/s  |
| 16 KiB  | +11%  | 25.0GB/s  | 6.26GB/s  |
| 32 KiB  | +15%  | 24.2GB/s  | 6.04GB/s  |
| 64 KiB  | +15%  | 24.1GB/s  | 6.01GB/s  |
| 128 KiB | +18%  | 23.4GB/s  | 5.86GB/s  |
| 256 KiB | +21%  | 22.9GB/s  | 5.72GB/s  |
| 512 KiB | +21%  | 22.9GB/s  | 5.73GB/s  |
| 1 MiB   | +37%  | 20.3GB/s  | 5.06GB/s  |
| 2 MiB   | +117% | 12.7GB/s  | 3.18GB/s  |
| 4 MiB   | +130% | 12.0GB/s  | 3.01GB/s  |
| 8 MiB   | +129% | 12.1GB/s  | 3.03GB/s  |
| 16 MiB  | +116% | 12.8GB/s  | 3.21GB/s  |
| 32 MiB  | +115% | 12.9GB/s  | 3.22GB/s  |
| 64 MiB  | +111% | 13.1GB/s  | 3.28GB/s  |
| 128 MiB | +100% | 13.8GB/s  | 3.46GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff | bandwidth | bandwidth per thread | comment |
|---------|------|-----------|-----------|-------------|
| 256 B   | +17% | 28.4GB/s  | 7.09GB/s  |
| 512 B   | +8%  | 30.7GB/s  | 7.69GB/s  |
| 1 KiB   | +4%  | 32.0GB/s  | 8.00GB/s  |
| 2 KiB   | +2%  | 32.7GB/s  | 8.17GB/s  |
| 4 KiB   | -    | 33.1GB/s  | 8.27GB/s  | page, 4 KiB |
| 8 KiB   | -    | 33.2GB/s  | 8.29GB/s  |
| 16 KiB  | +15% | 29.0GB/s  | 7.24GB/s  |
| 32 KiB  | +25% | 26.5GB/s  | 6.62GB/s  |
| 64 KiB  | +32% | 25.1GB/s  | 6.28GB/s  |
| 128 KiB | +35% | 24.6GB/s  | 6.16GB/s  |
| 256 KiB | +37% | 24.3GB/s  | 6.07GB/s  |
| 512 KiB | +38% | 24.1GB/s  | 6.01GB/s  |
| 1 MiB   | +37% | 24.1GB/s  | 6.03GB/s  |
| 2 MiB   | +49% | 22.3GB/s  | 5.56GB/s  |
| 4 MiB   | +61% | 20.6GB/s  | 5.16GB/s  |
| 8 MiB   | +65% | 20.1GB/s  | 5.03GB/s  |
| 16 MiB  | +66% | 20.0GB/s  | 5.01GB/s  |
| 32 MiB  | +71% | 19.4GB/s  | 4.86GB/s  |
| 64 MiB  | +72% | 19.3GB/s  | 4.83GB/s  |
| 128 MiB | +75% | 18.9GB/s  | 4.73GB/s  |

</details>



# MediaTek Dimensity 7020

* Device: Motorola G54
* Memory: 8 GB, LPDDR5, QC 16bit, 3200 MHz, 51.2 GB/s

<details>
</details>


# Snapdragon 439

* Device: Redmi 7A
* Memory v1: 2GB, LPDDR3, 933 MHz, 7.4GB/s
* Memory v2: 2GB, LPDDR3-1600, SC 32bit, 800MHz, 6.4 GB/s

<details>
</details>


# Apple Mac M1

## Performance core

* Cache:
    - L1D: 4x 128 KB
    - L2:  4x 3MB,  12 MB
* Memory: 16 GB, LPDDR4X-4266 DC 16bit, 2133 MHz, 128bit Bus, 68.25 GB/s

<details><summary><b>memset, single thread</b></summary>

**memset**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|------|------------|--------------|
| 256 B   | 33.41 ms | +26%  | -    | 80.4GB/s   |              |
| 512 B   | 27.92 ms | +5%   | +16% | 96.1GB/s   |              |
| 1 KiB   | 27.06 ms | +2%   | +3%  | 99.2GB/s   |              |
| 2 KiB   | 26.65 ms | -     | +2%  | 100.7GB/s  |              |
| 4 KiB   | 26.52 ms | -     | -    | 101.2GB/s  |              |
| 8 KiB   | 27.37 ms | +3%   | +3%  | 98.1GB/s   |              |
| 16 KiB  | 26.98 ms | +2%   | +1%  | 99.5GB/s   | page, 16 KiB |
| 32 KiB  | 37.26 ms | +40%  | +38% | 72.0GB/s   |              |
| 64 KiB  | 47.71 ms | +80%  | +28% | 56.3GB/s   |              |
| 128 KiB | 52.03 ms | +96%  | +9%  | 51.6GB/s   | L1D, 128 KiB |
| 256 KiB | 54.27 ms | +105% | +4%  | 49.5GB/s   |              |
| 512 KiB | 55.39 ms | +109% | +2%  | 48.5GB/s   |              |
| 1 MiB   | 56.02 ms | +111% | +1%  | 47.9GB/s   |              |
| 2 MiB   | 56.08 ms | +111% | -    | 47.9GB/s   |              |
| 4 MiB   | 55.19 ms | +108% | +2%  | 48.6GB/s   |              |
| 8 MiB   | 56.78 ms | +114% | +3%  | 47.3GB/s   |              |
| 16 MiB  | 57.45 ms | +117% | +1%  | 46.7GB/s   | L2, 12 MiB   |
| 32 MiB  | 57.93 ms | +118% | +1%  | 46.3GB/s   |              |
| 64 MiB  | 58.87 ms | +122% | +2%  | 45.6GB/s   |              |
| 128 MiB | 58.27 ms | +120% | +1%  | 46.1GB/s   |              |

**SIMD cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|------|------------|--------------|
| 256 B   | 26.25 ms | -     | -    | 102.3GB/s  |              |
| 512 B   | 26.26 ms | -     | -    | 102.2GB/s  |              |
| 1 KiB   | 26.27 ms | -     | -    | 102.2GB/s  |              |
| 2 KiB   | 26.24 ms | -     | -    | 102.3GB/s  |              |
| 4 KiB   | 26.25 ms | -     | -    | 102.3GB/s  |              |
| 8 KiB   | 26.45 ms | +1%   | +1%  | 101.5GB/s  |              |
| 16 KiB  | 27.25 ms | +4%   | +3%  | 98.5GB/s   | page, 16 KiB |
| 32 KiB  | 29.47 ms | +12%  | +8%  | 91.1GB/s   |              |
| 64 KiB  | 32.73 ms | +25%  | +11% | 82.0GB/s   |              |
| 128 KiB | 34.04 ms | +30%  | +4%  | 78.9GB/s   | L1D, 128 KiB |
| 256 KiB | 32.14 ms | +22%  | +6%  | 83.5GB/s   |              |
| 512 KiB | 33.37 ms | +27%  | +4%  | 80.4GB/s   |              |
| 1 MiB   | 39.16 ms | +49%  | +17% | 68.5GB/s   |              |
| 2 MiB   | 44.60 ms | +70%  | +14% | 60.2GB/s   |              |
| 4 MiB   | 48.30 ms | +84%  | +8%  | 55.6GB/s   |              |
| 8 MiB   | 54.52 ms | +108% | +13% | 49.2GB/s   |              |
| 16 MiB  | 56.19 ms | +114% | +3%  | 47.8GB/s   | L2, 12 MiB   |
| 32 MiB  | 58.57 ms | +123% | +4%  | 45.8GB/s   |              |
| 64 MiB  | 59.02 ms | +125% | +1%  | 45.5GB/s   |              |
| 128 MiB | 59.46 ms | +127% | +1%  | 45.1GB/s   |              |

**SIMD non-cached fill** (if supported)

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|------|-----------|--------------|
| 256 B   | 0.61 s   | +937% | -    | 4.40GB/s  |              |
| 512 B   | 0.55 s   | +830% | +10% | 4.91GB/s  |              |
| 1 KiB   | 0.28 s   | +371% | +49% | 9.68GB/s  |              |
| 2 KiB   | 0.14 s   | +137% | +50% | 19.3GB/s  |              |
| 4 KiB   | 87.39 ms | +49%  | +37% | 30.7GB/s  |              |
| 8 KiB   | 61.76 ms | +5%   | +29% | 43.5GB/s  |              |
| 16 KiB  | 59.50 ms | +1%   | +4%  | 45.1GB/s  | page, 16 KiB |
| 32 KiB  | 59.51 ms | +1%   | -    | 45.1GB/s  |              |
| 64 KiB  | 59.49 ms | +1%   | -    | 45.1GB/s  |              |
| 128 KiB | 59.50 ms | +1%   | -    | 45.1GB/s  | L1D, 128 KiB |
| 256 KiB | 59.48 ms | +1%   | -    | 45.1GB/s  |              |
| 512 KiB | 58.82 ms | -     | +1%  | 45.6GB/s  |              |
| 1 MiB   | 59.49 ms | +1%   | +1%  | 45.1GB/s  |              |
| 2 MiB   | 59.50 ms | +1%   | -    | 45.1GB/s  |              |
| 4 MiB   | 59.56 ms | +1%   | -    | 45.1GB/s  |              |
| 8 MiB   | 59.53 ms | +1%   | -    | 45.1GB/s  |              |
| 16 MiB  | 59.61 ms | +1%   | -    | 45.0GB/s  | L2, 12 MiB   |
| 32 MiB  | 59.56 ms | +1%   | -    | 45.1GB/s  |              |
| 64 MiB  | 59.56 ms | +1%   | -    | 45.1GB/s  |              |
| 128 MiB | 59.55 ms | +1%   | -    | 45.1GB/s  |              |

</details>
<details><summary><b>memset, multithreading (4 threads, 4 cores)</b></summary>

**memset**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|------------|-----------|--------------|
| 256 B   | 61.37 ms | +5%   | -     | 175.0GB/s  | 43.7GB/s  |              |
| 512 B   | 59.69 ms | +2%   | +3%   | 179.9GB/s  | 45.0GB/s  |              |
| 1 KiB   | 59.22 ms | +2%   | +1%   | 181.3GB/s  | 45.3GB/s  |              |
| 2 KiB   | 59.89 ms | +3%   | +1%   | 179.3GB/s  | 44.8GB/s  |              |
| 4 KiB   | 60.44 ms | +4%   | +1%   | 177.7GB/s  | 44.4GB/s  |              |
| 8 KiB   | 58.30 ms | -     | +4%   | 184.2GB/s  | 46.0GB/s  |              |
| 16 KiB  | 67.38 ms | +16%  | +16%  | 159.4GB/s  | 39.8GB/s  | page, 16 KiB |
| 32 KiB  | 0.23 s   | +302% | +248% | 45.8GB/s   | 11.5GB/s  |              |
| 64 KiB  | 0.24 s   | +307% | +1%   | 45.2GB/s   | 11.3GB/s  |              |
| 128 KiB | 0.24 s   | +307% | -     | 45.2GB/s   | 11.3GB/s  | L1D, 128 KiB |
| 256 KiB | 0.24 s   | +307% | -     | 45.2GB/s   | 11.3GB/s  |              |
| 512 KiB | 0.24 s   | +308% | -     | 45.2GB/s   | 11.3GB/s  |              |
| 1 MiB   | 0.24 s   | +308% | -     | 45.2GB/s   | 11.3GB/s  |              |
| 2 MiB   | 0.24 s   | +308% | -     | 45.1GB/s   | 11.3GB/s  |              |
| 4 MiB   | 0.24 s   | +308% | -     | 45.1GB/s   | 11.3GB/s  |              |
| 8 MiB   | 0.24 s   | +309% | -     | 45.1GB/s   | 11.3GB/s  |              |
| 16 MiB  | 0.24 s   | +309% | -     | 45.0GB/s   | 11.3GB/s  | L2, 12 MiB   |
| 32 MiB  | 0.24 s   | +309% | -     | 45.1GB/s   | 11.3GB/s  |              |
| 64 MiB  | 0.24 s   | +307% | -     | 45.2GB/s   | 11.3GB/s  |              |
| 128 MiB | 0.24 s   | +305% | +1%   | 45.5GB/s   | 11.4GB/s  |              |

**SIMD cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|------|------------|-----------|--------------|
| 256 B   | 63.95 ms | +17%  | -    | 167.9GB/s  | 42.0GB/s  |              |
| 512 B   | 54.46 ms | -     | +15% | 197.2GB/s  | 49.3GB/s  |              |
| 1 KiB   | 59.82 ms | +10%  | +10% | 179.5GB/s  | 44.9GB/s  |              |
| 2 KiB   | 57.38 ms | +5%   | +4%  | 187.1GB/s  | 46.8GB/s  |              |
| 4 KiB   | 57.46 ms | +6%   | -    | 186.9GB/s  | 46.7GB/s  |              |
| 8 KiB   | 56.36 ms | +3%   | +2%  | 190.5GB/s  | 47.6GB/s  |              |
| 16 KiB  | 86.49 ms | +59%  | +53% | 124.1GB/s  | 31.0GB/s  | page, 16 KiB |
| 32 KiB  | 0.11 s   | +109% | +31% | 94.5GB/s   | 23.6GB/s  |              |
| 64 KiB  | 0.14 s   | +165% | +27% | 74.3GB/s   | 18.6GB/s  |              |
| 128 KiB | 0.17 s   | +211% | +17% | 63.4GB/s   | 15.8GB/s  | L1D, 128 KiB |
| 256 KiB | 0.20 s   | +268% | +18% | 53.6GB/s   | 13.4GB/s  |              |
| 512 KiB | 0.22 s   | +301% | +9%  | 49.2GB/s   | 12.3GB/s  |              |
| 1 MiB   | 0.23 s   | +318% | +4%  | 47.2GB/s   | 11.8GB/s  |              |
| 2 MiB   | 0.23 s   | +328% | +2%  | 46.1GB/s   | 11.5GB/s  |              |
| 4 MiB   | 0.24 s   | +336% | +2%  | 45.3GB/s   | 11.3GB/s  |              |
| 8 MiB   | 0.24 s   | +343% | +2%  | 44.5GB/s   | 11.1GB/s  |              |
| 16 MiB  | 0.24 s   | +339% | +1%  | 44.9GB/s   | 11.2GB/s  | L2, 12 MiB   |
| 32 MiB  | 0.24 s   | +341% | -    | 44.8GB/s   | 11.2GB/s  |              |
| 64 MiB  | 0.24 s   | +344% | +1%  | 44.4GB/s   | 11.1GB/s  |              |
| 128 MiB | 0.24 s   | +345% | -    | 44.3GB/s   | 11.1GB/s  |              |

**SIMD non-cached fill** (if supported)

| block | time | diff | delta | bandwidth | comment |
|---------|--------|-------|------|-----------|-----------|--------------|
| 256 B   | 2.27 s | +853% | -    | 18.9GB/s  | 4.73GB/s  |              |
| 512 B   | 1.26 s | +427% | +45% | 34.2GB/s  | 8.54GB/s  |              |
| 1 KiB   | 0.97 s | +306% | +23% | 44.3GB/s  | 11.1GB/s  |              |
| 2 KiB   | 0.95 s | +300% | +1%  | 45.0GB/s  | 11.2GB/s  |              |
| 4 KiB   | 0.95 s | +300% | -    | 45.0GB/s  | 11.2GB/s  |              |
| 8 KiB   | 0.95 s | +300% | -    | 45.0GB/s  | 11.2GB/s  |              |
| 16 KiB  | 0.95 s | +300% | -    | 45.0GB/s  | 11.3GB/s  | page, 16 KiB |
| 32 KiB  | 0.96 s | +300% | -    | 45.0GB/s  | 11.2GB/s  |              |
| 64 KiB  | 0.96 s | +301% | -    | 44.9GB/s  | 11.2GB/s  |              |
| 128 KiB | 0.96 s | +301% | -    | 44.9GB/s  | 11.2GB/s  | L1D, 128 KiB |
| 256 KiB | 0.96 s | +301% | -    | 44.9GB/s  | 11.2GB/s  |              |
| 512 KiB | 0.96 s | +301% | -    | 44.9GB/s  | 11.2GB/s  |              |
| 1 MiB   | 0.96 s | +301% | -    | 44.9GB/s  | 11.2GB/s  |              |
| 2 MiB   | 0.96 s | +301% | -    | 44.9GB/s  | 11.2GB/s  |              |
| 4 MiB   | 0.24 s | -     | +75% | 45.0GB/s  | 11.3GB/s  |              |
| 8 MiB   | 0.24 s | -     | -    | 45.0GB/s  | 11.2GB/s  |              |
| 16 MiB  | 0.24 s | -     | -    | 44.9GB/s  | 11.2GB/s  | L2, 12 MiB   |
| 32 MiB  | 0.24 s | -     | -    | 44.9GB/s  | 11.2GB/s  |              |
| 64 MiB  | 0.24 s | -     | -    | 44.8GB/s  | 11.2GB/s  |              |
| 128 MiB | 0.24 s | +1%   | -    | 44.8GB/s  | 11.2GB/s  |              |

</details>
<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|-----------|--------------|
| 256 B   | 45.93 ms | +35%  | -     | 58.4GB/s  |              |
| 512 B   | 36.08 ms | +6%   | +21%  | 74.4GB/s  |              |
| 1 KiB   | 36.20 ms | +6%   | -     | 74.2GB/s  |              |
| 2 KiB   | 35.77 ms | +5%   | +1%   | 75.0GB/s  |              |
| 4 KiB   | 34.07 ms | -     | +5%   | 78.8GB/s  |              |
| 8 KiB   | 34.71 ms | +2%   | +2%   | 77.3GB/s  |              |
| 16 KiB  | 34.98 ms | +3%   | +1%   | 76.7GB/s  | page, 16 KiB |
| 32 KiB  | 35.14 ms | +3%   | -     | 76.4GB/s  |              |
| 64 KiB  | 36.39 ms | +7%   | +4%   | 73.8GB/s  |              |
| 128 KiB | 82.27 ms | +142% | +126% | 32.6GB/s  | L1D, 128 KiB |
| 256 KiB | 38.05 ms | +12%  | +54%  | 70.5GB/s  |              |
| 512 KiB | 54.30 ms | +59%  | +43%  | 49.4GB/s  |              |
| 1 MiB   | 63.70 ms | +87%  | +17%  | 42.1GB/s  |              |
| 2 MiB   | 69.53 ms | +104% | +9%   | 38.6GB/s  |              |
| 4 MiB   | 68.69 ms | +102% | +1%   | 39.1GB/s  |              |
| 8 MiB   | 87.48 ms | +157% | +27%  | 30.7GB/s  |              |
| 16 MiB  | 90.57 ms | +166% | +4%   | 29.6GB/s  | L2, 12 MiB   |
| 32 MiB  | 90.67 ms | +166% | -     | 29.6GB/s  |              |
| 64 MiB  | 90.80 ms | +167% | -     | 29.6GB/s  |              |
| 128 MiB | 91.04 ms | +167% | -     | 29.5GB/s  |              |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|------|-----------|--------------|
| 256 B   | 37.59 ms | +8%   | -    | 71.4GB/s  |              |
| 512 B   | 36.09 ms | +4%   | +4%  | 74.4GB/s  |              |
| 1 KiB   | 35.47 ms | +2%   | +2%  | 75.7GB/s  |              |
| 2 KiB   | 35.47 ms | +2%   | -    | 75.7GB/s  |              |
| 4 KiB   | 35.43 ms | +2%   | -    | 75.8GB/s  |              |
| 8 KiB   | 34.77 ms | -     | +2%  | 77.2GB/s  |              |
| 16 KiB  | 35.22 ms | +1%   | +1%  | 76.2GB/s  | page, 16 KiB |
| 32 KiB  | 35.32 ms | +2%   | -    | 76.0GB/s  |              |
| 64 KiB  | 36.13 ms | +4%   | +2%  | 74.3GB/s  |              |
| 128 KiB | 62.28 ms | +79%  | +72% | 43.1GB/s  | L1D, 128 KiB |
| 256 KiB | 56.09 ms | +61%  | +10% | 47.9GB/s  |              |
| 512 KiB | 55.78 ms | +60%  | +1%  | 48.1GB/s  |              |
| 1 MiB   | 55.73 ms | +60%  | -    | 48.2GB/s  |              |
| 2 MiB   | 56.09 ms | +61%  | +1%  | 47.9GB/s  |              |
| 4 MiB   | 57.30 ms | +65%  | +2%  | 46.8GB/s  |              |
| 8 MiB   | 60.69 ms | +75%  | +6%  | 44.2GB/s  |              |
| 16 MiB  | 91.13 ms | +162% | +50% | 29.5GB/s  | L2, 12 MiB   |
| 32 MiB  | 90.33 ms | +160% | +1%  | 29.7GB/s  |              |
| 64 MiB  | 90.33 ms | +160% | -    | 29.7GB/s  |              |
| 128 MiB | 90.41 ms | +160% | -    | 29.7GB/s  |              |

**SIMD non-cached copy** (if supported)

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|------|-----------|--------------|
| 256 B   | 92.30 ms | +178% | -    | 29.1GB/s  |              |
| 512 B   | 35.25 ms | +6%   | +62% | 76.1GB/s  |              |
| 1 KiB   | 34.24 ms | +3%   | +3%  | 78.4GB/s  |              |
| 2 KiB   | 33.95 ms | +2%   | +1%  | 79.1GB/s  |              |
| 4 KiB   | 33.91 ms | +2%   | -    | 79.2GB/s  |              |
| 8 KiB   | 34.46 ms | +4%   | +2%  | 77.9GB/s  |              |
| 16 KiB  | 33.58 ms | +1%   | +3%  | 79.9GB/s  | page, 16 KiB |
| 32 KiB  | 33.20 ms | -     | +1%  | 80.9GB/s  |              |
| 64 KiB  | 34.41 ms | +4%   | +4%  | 78.0GB/s  |              |
| 128 KiB | 62.11 ms | +87%  | +81% | 43.2GB/s  | L1D, 128 KiB |
| 256 KiB | 55.95 ms | +69%  | +10% | 48.0GB/s  |              |
| 512 KiB | 56.02 ms | +69%  | -    | 47.9GB/s  |              |
| 1 MiB   | 55.97 ms | +69%  | -    | 48.0GB/s  |              |
| 2 MiB   | 56.26 ms | +69%  | +1%  | 47.7GB/s  |              |
| 4 MiB   | 57.61 ms | +74%  | +2%  | 46.6GB/s  |              |
| 8 MiB   | 60.39 ms | +82%  | +5%  | 44.5GB/s  |              |
| 16 MiB  | 91.15 ms | +175% | +51% | 29.4GB/s  | L2, 12 MiB   |
| 32 MiB  | 90.30 ms | +172% | +1%  | 29.7GB/s  |              |
| 64 MiB  | 90.41 ms | +172% | -    | 29.7GB/s  |              |
| 128 MiB | 90.47 ms | +172% | -    | 29.7GB/s  |              |

**SIMD cached load, non-cached store** (if supported)

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|------|-----------|--------------|
| 256 B   | 89.42 ms | +169% | -    | 30.0GB/s  |              |
| 512 B   | 35.25 ms | +6%   | +61% | 76.1GB/s  |              |
| 1 KiB   | 34.48 ms | +4%   | +2%  | 77.9GB/s  |              |
| 2 KiB   | 34.61 ms | +4%   | -    | 77.6GB/s  |              |
| 4 KiB   | 34.98 ms | +5%   | +1%  | 76.7GB/s  |              |
| 8 KiB   | 34.51 ms | +4%   | +1%  | 77.8GB/s  |              |
| 16 KiB  | 33.60 ms | +1%   | +3%  | 79.9GB/s  | page, 16 KiB |
| 32 KiB  | 33.21 ms | -     | +1%  | 80.8GB/s  |              |
| 64 KiB  | 35.26 ms | +6%   | +6%  | 76.1GB/s  |              |
| 128 KiB | 63.69 ms | +92%  | +81% | 42.1GB/s  | L1D, 128 KiB |
| 256 KiB | 56.45 ms | +70%  | +11% | 47.6GB/s  |              |
| 512 KiB | 56.32 ms | +70%  | -    | 47.7GB/s  |              |
| 1 MiB   | 56.18 ms | +69%  | -    | 47.8GB/s  |              |
| 2 MiB   | 56.46 ms | +70%  | -    | 47.5GB/s  |              |
| 4 MiB   | 57.59 ms | +73%  | +2%  | 46.6GB/s  |              |
| 8 MiB   | 60.96 ms | +84%  | +6%  | 44.0GB/s  |              |
| 16 MiB  | 91.38 ms | +175% | +50% | 29.4GB/s  | L2, 12 MiB   |
| 32 MiB  | 90.41 ms | +172% | +1%  | 29.7GB/s  |              |
| 64 MiB  | 90.34 ms | +172% | -    | 29.7GB/s  |              |
| 128 MiB | 90.26 ms | +172% | -    | 29.7GB/s  |              |

</details>
<details><summary><b>memcpy, multithreading (4 threads, 4 cores)</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|------------|-----------|--------------|
| 256 B   | 81.32 ms | +24%  | -     | 132.0GB/s  | 33.0GB/s  |              |
| 512 B   | 73.68 ms | +12%  | +9%   | 145.7GB/s  | 36.4GB/s  |              |
| 1 KiB   | 68.70 ms | +5%   | +7%   | 156.3GB/s  | 39.1GB/s  |              |
| 2 KiB   | 68.13 ms | +4%   | +1%   | 157.6GB/s  | 39.4GB/s  |              |
| 4 KiB   | 66.42 ms | +1%   | +3%   | 161.7GB/s  | 40.4GB/s  |              |
| 8 KiB   | 65.60 ms | -     | +1%   | 163.7GB/s  | 40.9GB/s  |              |
| 16 KiB  | 67.25 ms | +3%   | +3%   | 159.7GB/s  | 39.9GB/s  | page, 16 KiB |
| 32 KiB  | 72.00 ms | +10%  | +7%   | 149.1GB/s  | 37.3GB/s  |              |
| 64 KiB  | 70.41 ms | +7%   | +2%   | 152.5GB/s  | 38.1GB/s  |              |
| 128 KiB | 0.12 s   | +87%  | +74%  | 87.7GB/s   | 21.9GB/s  | L1D, 128 KiB |
| 256 KiB | 96.73 ms | +47%  | +21%  | 111.0GB/s  | 27.8GB/s  |              |
| 512 KiB | 0.30 s   | +360% | +212% | 35.6GB/s   | 8.89GB/s  |              |
| 1 MiB   | 0.29 s   | +347% | +3%   | 36.6GB/s   | 9.15GB/s  |              |
| 2 MiB   | 0.31 s   | +367% | +4%   | 35.0GB/s   | 8.76GB/s  |              |
| 4 MiB   | 0.37 s   | +463% | +21%  | 29.1GB/s   | 7.27GB/s  |              |
| 8 MiB   | 0.37 s   | +461% | -     | 29.2GB/s   | 7.29GB/s  |              |
| 16 MiB  | 0.37 s   | +462% | -     | 29.2GB/s   | 7.29GB/s  | L2, 12 MiB   |
| 32 MiB  | 0.37 s   | +463% | -     | 29.1GB/s   | 7.27GB/s  |              |
| 64 MiB  | 0.37 s   | +462% | -     | 29.1GB/s   | 7.28GB/s  |              |
| 128 MiB | 0.37 s   | +462% | -     | 29.1GB/s   | 7.28GB/s  |              |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|------------|-----------|--------------|
| 256 B   | 69.53 ms | +13%  | -     | 154.4GB/s  | 38.6GB/s  |              |
| 512 B   | 70.58 ms | +15%  | +2%   | 152.1GB/s  | 38.0GB/s  |              |
| 1 KiB   | 61.31 ms | -     | +13%  | 175.1GB/s  | 43.8GB/s  |              |
| 2 KiB   | 69.44 ms | +13%  | +13%  | 154.6GB/s  | 38.7GB/s  |              |
| 4 KiB   | 67.61 ms | +10%  | +3%   | 158.8GB/s  | 39.7GB/s  |              |
| 8 KiB   | 66.06 ms | +8%   | +2%   | 162.5GB/s  | 40.6GB/s  |              |
| 16 KiB  | 68.17 ms | +11%  | +3%   | 157.5GB/s  | 39.4GB/s  | page, 16 KiB |
| 32 KiB  | 69.88 ms | +14%  | +2%   | 153.7GB/s  | 38.4GB/s  |              |
| 64 KiB  | 67.82 ms | +11%  | +3%   | 158.3GB/s  | 39.6GB/s  |              |
| 128 KiB | 0.10 s   | +69%  | +53%  | 103.8GB/s  | 25.9GB/s  | L1D, 128 KiB |
| 256 KiB | 96.64 ms | +58%  | +7%   | 111.1GB/s  | 27.8GB/s  |              |
| 512 KiB | 84.59 ms | +38%  | +12%  | 126.9GB/s  | 31.7GB/s  |              |
| 1 MiB   | 90.15 ms | +47%  | +7%   | 119.1GB/s  | 29.8GB/s  |              |
| 2 MiB   | 0.16 s   | +168% | +82%  | 65.5GB/s   | 16.4GB/s  |              |
| 4 MiB   | 0.36 s   | +492% | +121% | 29.6GB/s   | 7.40GB/s  |              |
| 8 MiB   | 0.37 s   | +498% | +1%   | 29.3GB/s   | 7.32GB/s  |              |
| 16 MiB  | 0.37 s   | +496% | -     | 29.4GB/s   | 7.35GB/s  | L2, 12 MiB   |
| 32 MiB  | 0.37 s   | +496% | -     | 29.4GB/s   | 7.34GB/s  |              |
| 64 MiB  | 0.37 s   | +496% | -     | 29.4GB/s   | 7.35GB/s  |              |
| 128 MiB | 0.37 s   | +498% | -     | 29.3GB/s   | 7.33GB/s  |              |

**SIMD non-cached copy** (if supported)

| block | time | diff | delta | bandwidth | comment |
|---------|--------|-------|-------|------------|-----------|--------------|
| 256 B   | 0.42 s | +151% | -     | 102.7GB/s  | 25.7GB/s  |              |
| 512 B   | 0.18 s | +8%   | +57%  | 239.5GB/s  | 59.9GB/s  |              |
| 1 KiB   | 0.17 s | +5%   | +3%   | 245.9GB/s  | 61.5GB/s  |              |
| 2 KiB   | 0.18 s | +6%   | +1%   | 244.2GB/s  | 61.1GB/s  |              |
| 4 KiB   | 0.18 s | +7%   | +1%   | 241.0GB/s  | 60.3GB/s  |              |
| 8 KiB   | 0.18 s | +6%   | +1%   | 244.0GB/s  | 61.0GB/s  |              |
| 16 KiB  | 0.17 s | -     | +6%   | 258.2GB/s  | 64.6GB/s  | page, 16 KiB |
| 32 KiB  | 0.17 s | +2%   | +2%   | 252.1GB/s  | 63.0GB/s  |              |
| 64 KiB  | 0.18 s | +9%   | +6%   | 237.7GB/s  | 59.4GB/s  |              |
| 128 KiB | 0.31 s | +87%  | +72%  | 138.4GB/s  | 34.6GB/s  | L1D, 128 KiB |
| 256 KiB | 0.29 s | +72%  | +8%   | 150.4GB/s  | 37.6GB/s  |              |
| 512 KiB | 0.28 s | +67%  | +3%   | 154.9GB/s  | 38.7GB/s  |              |
| 1 MiB   | 0.29 s | +77%  | +6%   | 145.6GB/s  | 36.4GB/s  |              |
| 2 MiB   | 0.67 s | +301% | +126% | 64.4GB/s   | 16.1GB/s  |              |
| 4 MiB   | 0.36 s | +118% | +46%  | 29.6GB/s   | 7.41GB/s  |              |
| 8 MiB   | 0.36 s | +119% | -     | 29.5GB/s   | 7.38GB/s  |              |
| 16 MiB  | 0.36 s | +118% | -     | 29.6GB/s   | 7.40GB/s  | L2, 12 MiB   |
| 32 MiB  | 0.36 s | +118% | -     | 29.5GB/s   | 7.39GB/s  |              |
| 64 MiB  | 0.36 s | +119% | -     | 29.4GB/s   | 7.36GB/s  |              |
| 128 MiB | 0.36 s | +119% | -     | 29.5GB/s   | 7.37GB/s  |              |

</details>

## EnergyEfficient core

* Cache:
    - L1D: 4x 64 KB
    - L2:  4x 1MB,  4 MB
* Memory: 16 GB, LPDDR4X-4266 DC 16bit, 2133 MHz, 128bit Bus, 68.25 GB/s

<details><summary><b>memset, single thread</b></summary>

**memset**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|-----|-----|-----------|--------------|
| 256 B   | 0.17 s | +8% | -   | 16.2GB/s  |              |
| 512 B   | 0.17 s | +8% | -   | 16.1GB/s  |              |
| 1 KiB   | 0.17 s | +8% | -   | 16.2GB/s  |              |
| 2 KiB   | 0.17 s | +8% | -   | 16.1GB/s  |              |
| 4 KiB   | 0.16 s | +7% | +1% | 16.3GB/s  |              |
| 8 KiB   | 0.17 s | +8% | +1% | 16.1GB/s  |              |
| 16 KiB  | 0.17 s | +9% | +1% | 16.0GB/s  | page, 16 KiB |
| 32 KiB  | 0.16 s | +1% | +8% | 17.3GB/s  |              |
| 64 KiB  | 0.16 s | +1% | -   | 17.2GB/s  | L1D, 64 KiB  |
| 128 KiB | 0.15 s | -   | +1% | 17.4GB/s  |              |
| 256 KiB | 0.16 s | +1% | +1% | 17.2GB/s  |              |
| 512 KiB | 0.15 s | -   | +1% | 17.4GB/s  |              |
| 1 MiB   | 0.16 s | +1% | +1% | 17.2GB/s  |              |
| 2 MiB   | 0.15 s | -   | +1% | 17.4GB/s  |              |
| 4 MiB   | 0.16 s | +3% | +3% | 17.0GB/s  | L2, 4 MiB    |
| 8 MiB   | 0.16 s | +1% | +2% | 17.3GB/s  |              |
| 16 MiB  | 0.16 s | +1% | +1% | 17.2GB/s  |              |
| 32 MiB  | 0.16 s | +5% | +4% | 16.6GB/s  |              |
| 64 MiB  | 0.16 s | +4% | +1% | 16.7GB/s  |              |
| 128 MiB | 0.16 s | +5% | -   | 16.7GB/s  |              |

**SIMD cached fill**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|------|------|-----------|--------------|
| 256 B   | 0.16 s | -    | -    | 16.4GB/s  |              |
| 512 B   | 0.17 s | +2%  | +2%  | 16.2GB/s  |              |
| 1 KiB   | 0.17 s | +2%  | -    | 16.2GB/s  |              |
| 2 KiB   | 0.17 s | +1%  | -    | 16.2GB/s  |              |
| 4 KiB   | 0.17 s | +3%  | +1%  | 16.0GB/s  |              |
| 8 KiB   | 0.17 s | +1%  | +1%  | 16.2GB/s  |              |
| 16 KiB  | 0.17 s | +1%  | -    | 16.2GB/s  | page, 16 KiB |
| 32 KiB  | 0.17 s | +2%  | -    | 16.2GB/s  |              |
| 64 KiB  | 0.17 s | +4%  | +3%  | 15.8GB/s  | L1D, 64 KiB  |
| 128 KiB | 0.19 s | +17% | +12% | 14.1GB/s  |              |
| 256 KiB | 0.19 s | +17% | -    | 14.0GB/s  |              |
| 512 KiB | 0.19 s | +17% | -    | 14.1GB/s  |              |
| 1 MiB   | 0.19 s | +16% | +1%  | 14.2GB/s  |              |
| 2 MiB   | 0.19 s | +16% | -    | 14.1GB/s  |              |
| 4 MiB   | 0.20 s | +23% | +5%  | 13.4GB/s  | L2, 4 MiB    |
| 8 MiB   | 0.20 s | +24% | +1%  | 13.3GB/s  |              |
| 16 MiB  | 0.21 s | +28% | +3%  | 12.8GB/s  |              |
| 32 MiB  | 0.21 s | +28% | -    | 12.9GB/s  |              |
| 64 MiB  | 0.21 s | +28% | -    | 12.8GB/s  |              |
| 128 MiB | 0.21 s | +29% | +1%  | 12.8GB/s  |              |

**SIMD non-cached fill** (if supported)

| block | time | diff | delta | bandwidth | comment |
|---------|--------|-------|------|-----------|--------------|
| 256 B   | 1.68 s | +930% | -    | 1.60GB/s  |              |
| 512 B   | 0.85 s | +419% | +50% | 3.17GB/s  |              |
| 1 KiB   | 0.42 s | +159% | +50% | 6.34GB/s  |              |
| 2 KiB   | 0.17 s | +4%   | +60% | 15.8GB/s  |              |
| 4 KiB   | 0.17 s | +2%   | +2%  | 16.2GB/s  |              |
| 8 KiB   | 0.17 s | +3%   | +1%  | 16.0GB/s  |              |
| 16 KiB  | 0.17 s | +2%   | -    | 16.1GB/s  | page, 16 KiB |
| 32 KiB  | 0.16 s | -     | +2%  | 16.4GB/s  |              |
| 64 KiB  | 0.17 s | +1%   | +1%  | 16.2GB/s  | L1D, 64 KiB  |
| 128 KiB | 0.17 s | +2%   | +1%  | 16.1GB/s  |              |
| 256 KiB | 0.16 s | +1%   | +1%  | 16.3GB/s  |              |
| 512 KiB | 0.17 s | +2%   | +1%  | 16.2GB/s  |              |
| 1 MiB   | 0.17 s | +2%   | -    | 16.2GB/s  |              |
| 2 MiB   | 0.17 s | +1%   | -    | 16.3GB/s  |              |
| 4 MiB   | 0.17 s | +2%   | +1%  | 16.1GB/s  | L2, 4 MiB    |
| 8 MiB   | 0.17 s | +1%   | +1%  | 16.2GB/s  |              |
| 16 MiB  | 0.17 s | +2%   | +1%  | 16.1GB/s  |              |
| 32 MiB  | 0.17 s | +6%   | +4%  | 15.5GB/s  |              |
| 64 MiB  | 0.17 s | +4%   | +2%  | 15.8GB/s  |              |
| 128 MiB | 0.17 s | +5%   | -    | 15.7GB/s  |              |

</details>
<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|------|------|-----------|--------------|
| 256 B   | 0.22 s | +33% | -    | 12.0GB/s  |              |
| 512 B   | 0.19 s | +15% | +13% | 13.9GB/s  |              |
| 1 KiB   | 0.18 s | +7%  | +7%  | 14.9GB/s  |              |
| 2 KiB   | 0.17 s | +4%  | +3%  | 15.4GB/s  |              |
| 4 KiB   | 0.18 s | +5%  | +2%  | 15.2GB/s  |              |
| 8 KiB   | 0.17 s | +2%  | +3%  | 15.6GB/s  |              |
| 16 KiB  | 0.17 s | -    | +2%  | 16.0GB/s  | page, 16 KiB |
| 32 KiB  | 0.18 s | +7%  | +7%  | 15.0GB/s  |              |
| 64 KiB  | 0.23 s | +36% | +28% | 11.8GB/s  | L1D, 64 KiB  |
| 128 KiB | 0.21 s | +26% | +7%  | 12.7GB/s  |              |
| 256 KiB | 0.21 s | +25% | +1%  | 12.8GB/s  |              |
| 512 KiB | 0.21 s | +25% | -    | 12.8GB/s  |              |
| 1 MiB   | 0.21 s | +24% | +1%  | 12.9GB/s  |              |
| 2 MiB   | 0.21 s | +25% | -    | 12.8GB/s  |              |
| 4 MiB   | 0.21 s | +27% | +2%  | 12.6GB/s  | L2, 4 MiB    |
| 8 MiB   | 0.21 s | +27% | -    | 12.6GB/s  |              |
| 16 MiB  | 0.22 s | +29% | +2%  | 12.4GB/s  |              |
| 32 MiB  | 0.22 s | +29% | -    | 12.4GB/s  |              |
| 64 MiB  | 0.22 s | +28% | -    | 12.5GB/s  |              |
| 128 MiB | 0.22 s | +29% | -    | 12.4GB/s  |              |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|------|------|-----------|--------------|
| 256 B   | 0.18 s | +8%  | -    | 15.1GB/s  |              |
| 512 B   | 0.17 s | +3%  | +4%  | 15.8GB/s  |              |
| 1 KiB   | 0.17 s | +3%  | -    | 15.8GB/s  |              |
| 2 KiB   | 0.16 s | -    | +3%  | 16.3GB/s  |              |
| 4 KiB   | 0.17 s | +6%  | +6%  | 15.4GB/s  |              |
| 8 KiB   | 0.17 s | +3%  | +3%  | 15.9GB/s  |              |
| 16 KiB  | 0.17 s | +3%  | -    | 15.8GB/s  | page, 16 KiB |
| 32 KiB  | 0.19 s | +16% | +13% | 14.0GB/s  |              |
| 64 KiB  | 0.30 s | +84% | +58% | 8.87GB/s  | L1D, 64 KiB  |
| 128 KiB | 0.28 s | +71% | +7%  | 9.56GB/s  |              |
| 256 KiB | 0.30 s | +84% | +8%  | 8.88GB/s  |              |
| 512 KiB | 0.30 s | +84% | -    | 8.89GB/s  |              |
| 1 MiB   | 0.31 s | +86% | +1%  | 8.76GB/s  |              |
| 2 MiB   | 0.30 s | +83% | +1%  | 8.89GB/s  |              |
| 4 MiB   | 0.30 s | +80% | +2%  | 9.06GB/s  | L2, 4 MiB    |
| 8 MiB   | 0.30 s | +82% | +1%  | 8.94GB/s  |              |
| 16 MiB  | 0.30 s | +82% | -    | 8.96GB/s  |              |
| 32 MiB  | 0.30 s | +85% | +2%  | 8.83GB/s  |              |
| 64 MiB  | 0.30 s | +85% | -    | 8.81GB/s  |              |
| 128 MiB | 0.31 s | +89% | +2%  | 8.61GB/s  |              |

**SIMD non-cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|------|------|-----------|--------------|
| 256 B   | 0.26 s | +22% | -    | 10.3GB/s  |              |
| 512 B   | 0.23 s | +7%  | +13% | 11.9GB/s  |              |
| 1 KiB   | 0.22 s | +1%  | +5%  | 12.5GB/s  |              |
| 2 KiB   | 0.21 s | -    | +1%  | 12.6GB/s  |              |
| 4 KiB   | 0.22 s | +4%  | +4%  | 12.1GB/s  |              |
| 8 KiB   | 0.21 s | +1%  | +3%  | 12.5GB/s  |              |
| 16 KiB  | 0.21 s | -    | +1%  | 12.6GB/s  | page, 16 KiB |
| 32 KiB  | 0.22 s | +2%  | +2%  | 12.4GB/s  |              |
| 64 KiB  | 0.22 s | +5%  | +3%  | 12.1GB/s  | L1D, 64 KiB  |
| 128 KiB | 0.22 s | +6%  | +1%  | 11.9GB/s  |              |
| 256 KiB | 0.22 s | +6%  | -    | 12.0GB/s  |              |
| 512 KiB | 0.22 s | +5%  | +1%  | 12.0GB/s  |              |
| 1 MiB   | 0.23 s | +9%  | +4%  | 11.6GB/s  |              |
| 2 MiB   | 0.23 s | +10% | +1%  | 11.5GB/s  |              |
| 4 MiB   | 0.23 s | +8%  | +2%  | 11.7GB/s  | L2, 4 MiB    |
| 8 MiB   | 0.23 s | +9%  | +1%  | 11.6GB/s  |              |
| 16 MiB  | 0.23 s | +9%  | -    | 11.6GB/s  |              |
| 32 MiB  | 0.23 s | +10% | +1%  | 11.5GB/s  |              |
| 64 MiB  | 0.24 s | +11% | +1%  | 11.4GB/s  |              |
| 128 MiB | 0.26 s | +21% | +9%  | 10.4GB/s  |              |

**SIMD cached load, non-cached store**

| block | time | diff | delta | bandwidth | comment |
|---------|--------|------|------|-----------|--------------|
| 256 B   | 0.27 s | +27% | -    | 9.96GB/s  |              |
| 512 B   | 0.22 s | +5%  | +17% | 12.1GB/s  |              |
| 1 KiB   | 0.22 s | +3%  | +2%  | 12.3GB/s  |              |
| 2 KiB   | 0.21 s | -    | +3%  | 12.6GB/s  |              |
| 4 KiB   | 0.22 s | +4%  | +4%  | 12.1GB/s  |              |
| 8 KiB   | 0.21 s | +1%  | +4%  | 12.6GB/s  |              |
| 16 KiB  | 0.21 s | -    | +1%  | 12.7GB/s  | page, 16 KiB |
| 32 KiB  | 0.22 s | +3%  | +3%  | 12.3GB/s  |              |
| 64 KiB  | 0.24 s | +13% | +10% | 11.2GB/s  | L1D, 64 KiB  |
| 128 KiB | 0.24 s | +13% | -    | 11.2GB/s  |              |
| 256 KiB | 0.24 s | +12% | +1%  | 11.3GB/s  |              |
| 512 KiB | 0.23 s | +10% | +2%  | 11.5GB/s  |              |
| 1 MiB   | 0.24 s | +15% | +4%  | 11.0GB/s  |              |
| 2 MiB   | 0.25 s | +17% | +2%  | 10.9GB/s  |              |
| 4 MiB   | 0.24 s | +13% | +3%  | 11.2GB/s  | L2, 4 MiB    |
| 8 MiB   | 0.24 s | +15% | +2%  | 11.0GB/s  |              |
| 16 MiB  | 0.25 s | +17% | +2%  | 10.8GB/s  |              |
| 32 MiB  | 0.25 s | +18% | -    | 10.8GB/s  |              |
| 64 MiB  | 0.25 s | +17% | -    | 10.8GB/s  |              |
| 128 MiB | 0.25 s | +17% | -    | 10.8GB/s  |              |

</details>

# Intel N150

* CPU Clock: 3.6 GHz
* L1: 32B/cy, 115 GB/s
* Cache:
    - L1D: 64KB per 2 cores (32KB per core)
    - L2: 2MB shared
    - L3: 6MB shared
* Memory: 16GB DDR4-3200, single channel, 20GB/s

**Results**

| | range | single thread bandwidth (GB/s) | multithreading, per core bandwidth (GB/s) | comment |
|---|---|---|---|---|
| cached copy     | 256B - 16KiB | 60-80 GB/s | 200-240 GB/s   | |
| non-cached copy | => 128 KiB   | 10 GB/s    |                | |
| non-cached copy | => 1 KiB     |            | 10 GB/s        |
| cached fill     | 256B - 4KiB  | 110 GB/s   |                | |
| cached fill     | <= 16 KiB    |            | 120 - 300 GB/s |
| non-cached fill | => 16 KiB    | 20 GB/s    |                | |
| non-cached fill | => 1 KiB     |            | 22.5 GB/s      |

<details><summary><b>memset, single thread</b></summary>

**memset**

| block   |  diff  | bandwidth | comment     |
|---------|--------|-----------|-------------|
| 256 B   | +219%  | 30.7GB/s  |
| 512 B   | +16%   | 84.5GB/s  |
| 1 KiB   | +24%   | 79.2GB/s  |
| 2 KiB   | +16%   | 84.4GB/s  |
| 4 KiB   | -      | 97.9GB/s  | page, 4 KiB |
| 8 KiB   | +67%   | 58.5GB/s  |
| 16 KiB  | +101%  | 48.6GB/s  |
| 32 KiB  | +117%  | 45.2GB/s  | L1D, 32 KiB |
| 64 KiB  | +185%  | 34.3GB/s  |
| 128 KiB | +205%  | 32.1GB/s  |
| 256 KiB | +228%  | 29.9GB/s  |
| 512 KiB | +225%  | 30.1GB/s  |
| 1 MiB   | +229%  | 29.7GB/s  |
| 2 MiB   | +245%  | 28.4GB/s  | L2, 2 MiB   |
| 4 MiB   | +316%  | 23.5GB/s  |
| 8 MiB   | +621%  | 13.6GB/s  | L3, 6 MiB   |
| 16 MiB  | +1108% | 8.10GB/s  |
| 32 MiB  | +1105% | 8.12GB/s  |
| 64 MiB  | +1109% | 8.10GB/s  |
| 128 MiB | +1113% | 8.07GB/s  |

**SIMD cached fill**

| block   |  diff  | bandwidth  | comment     |
|---------|--------|------------|-------------|
| 256 B   | -      | 106.9GB/s  |
| 512 B   | +1%    | 106.5GB/s  |
| 1 KiB   | -      | 107.2GB/s  |
| 2 KiB   | +3%    | 104.4GB/s  |
| 4 KiB   | +6%    | 100.7GB/s  | page, 4 KiB |
| 8 KiB   | +84%   | 58.4GB/s   |
| 16 KiB  | +98%   | 54.2GB/s   |
| 32 KiB  | +144%  | 44.0GB/s   | L1D, 32 KiB |
| 64 KiB  | +180%  | 38.3GB/s   |
| 128 KiB | +209%  | 34.7GB/s   |
| 256 KiB | +239%  | 31.6GB/s   |
| 512 KiB | +261%  | 29.7GB/s   |
| 1 MiB   | +260%  | 29.8GB/s   |
| 2 MiB   | +280%  | 28.2GB/s   | L2, 2 MiB   |
| 4 MiB   | +361%  | 23.3GB/s   |
| 8 MiB   | +719%  | 13.1GB/s   | L3, 6 MiB   |
| 16 MiB  | +1248% | 7.95GB/s   |
| 32 MiB  | +1231% | 8.05GB/s   |
| 64 MiB  | +1233% | 8.04GB/s   |
| 128 MiB | +1240% | 8.00GB/s   |

**SIMD non-cached fill**

| block   |  diff | bandwidth | comment     |
|---------|-------|-----------|-------------|
| 256 B   | +431% | 3.53GB/s  |
| 512 B   | +211% | 6.02GB/s  |
| 1 KiB   | +103% | 9.23GB/s  |
| 2 KiB   | +49%  | 12.5GB/s  |
| 4 KiB   | +26%  | 14.9GB/s  | page, 4 KiB |
| 8 KiB   | +14%  | 16.4GB/s  |
| 16 KiB  | +8%   | 17.3GB/s  |
| 32 KiB  | +5%   | 17.9GB/s  | L1D, 32 KiB |
| 64 KiB  | +2%   | 18.3GB/s  |
| 128 KiB | -     | 18.7GB/s  |
| 256 KiB | +1%   | 18.6GB/s  |
| 512 KiB | +1%   | 18.6GB/s  |
| 1 MiB   | -     | 18.7GB/s  |
| 2 MiB   | -     | 18.7GB/s  | L2, 2 MiB   |
| 4 MiB   | +1%   | 18.6GB/s  |
| 8 MiB   | -     | 18.7GB/s  | L3, 6 MiB   |
| 16 MiB  | -     | 18.7GB/s  |
| 32 MiB  | +1%   | 18.6GB/s  |
| 64 MiB  | +1%   | 18.6GB/s  |
| 128 MiB | -     | 18.7GB/s  |

</details>

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block   |  diff | bandwidth | comment     |
|---------|-------|-----------|-------------|
| 256 B   | +92%  | 45.2GB/s  |
| 512 B   | +53%  | 56.7GB/s  |
| 1 KiB   | +19%  | 72.7GB/s  |
| 2 KiB   | +12%  | 77.3GB/s  |
| 4 KiB   | +4%   | 83.4GB/s  | page, 4 KiB |
| 8 KiB   | -     | 86.6GB/s  |
| 16 KiB  | +81%  | 48.0GB/s  |
| 32 KiB  | +187% | 30.2GB/s  | L1D, 32 KiB |
| 64 KiB  | +197% | 29.1GB/s  |
| 128 KiB | +210% | 28.0GB/s  |
| 256 KiB | +204% | 28.5GB/s  |
| 512 KiB | +205% | 28.4GB/s  |
| 1 MiB   | +276% | 23.1GB/s  |
| 2 MiB   | +866% | 8.97GB/s  | L2, 2 MiB   |
| 4 MiB   | +852% | 9.10GB/s  |
| 8 MiB   | +859% | 9.04GB/s  | L3, 6 MiB   |
| 16 MiB  | +860% | 9.03GB/s  |
| 32 MiB  | +862% | 9.01GB/s  |
| 64 MiB  | +873% | 8.91GB/s  |
| 128 MiB | +877% | 8.87GB/s  |

**SIMD cached copy**

| block   |  diff  | bandwidth | comment     |
|---------|--------|-----------|-------------|
| 256 B   | +35%   | 59.7GB/s  |
| 512 B   | +20%   | 67.5GB/s  |
| 1 KiB   | +6%    | 75.8GB/s  |
| 2 KiB   | +10%   | 73.4GB/s  |
| 4 KiB   | +6%    | 76.3GB/s  | page, 4 KiB |
| 8 KiB   | -      | 80.7GB/s  |
| 16 KiB  | +12%   | 72.2GB/s  |
| 32 KiB  | +180%  | 28.8GB/s  | L1D, 32 KiB |
| 64 KiB  | +170%  | 29.9GB/s  |
| 128 KiB | +173%  | 29.6GB/s  |
| 256 KiB | +178%  | 29.1GB/s  |
| 512 KiB | +177%  | 29.2GB/s  |
| 1 MiB   | +257%  | 22.6GB/s  |
| 2 MiB   | +361%  | 17.5GB/s  | L2, 2 MiB   |
| 4 MiB   | +939%  | 7.77GB/s  |
| 8 MiB   | +1321% | 5.68GB/s  | L3, 6 MiB   |
| 16 MiB  | +1357% | 5.54GB/s  |
| 32 MiB  | +1670% | 4.56GB/s  |
| 64 MiB  | +1389% | 5.42GB/s  |
| 128 MiB | +1336% | 5.62GB/s  |

**SIMD non-cached copy**

| block   |  diff | bandwidth | comment     |
|---------|-------|-----------|-------------|
| 256 B   | +470% | 1.59GB/s  |
| 512 B   | +187% | 3.15GB/s  |
| 1 KiB   | +114% | 4.23GB/s  |
| 2 KiB   | +93%  | 4.69GB/s  |
| 4 KiB   | +86%  | 4.86GB/s  | page, 4 KiB |
| 8 KiB   | +86%  | 4.87GB/s  |
| 16 KiB  | +86%  | 4.87GB/s  |
| 32 KiB  | +100% | 4.52GB/s  | L1D, 32 KiB |
| 64 KiB  | +66%  | 5.47GB/s  |
| 128 KiB | +2%   | 8.86GB/s  |
| 256 KiB | -     | 9.06GB/s  |
| 512 KiB | +2%   | 8.84GB/s  |
| 1 MiB   | -     | 9.02GB/s  |
| 2 MiB   | +3%   | 8.77GB/s  | L2, 2 MiB   |
| 4 MiB   | +2%   | 8.88GB/s  |
| 8 MiB   | +1%   | 8.94GB/s  | L3, 6 MiB   |
| 16 MiB  | +5%   | 8.61GB/s  |
| 32 MiB  | +6%   | 8.54GB/s  |
| 64 MiB  | +4%   | 8.70GB/s  |
| 128 MiB | +6%   | 8.53GB/s  |

</details>

<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   |  diff  | bandwidth | comment     |
|---------|--------|-----------|-------------|
| 256 B   | +36%   | 71.7GB/s  |
| 512 B   | +20%   | 81.5GB/s  |
| 1 KiB   | +18%   | 82.8GB/s  |
| 2 KiB   | +14%   | 85.5GB/s  |
| 4 KiB   | -      | 97.6GB/s  | page, 4 KiB |
| 8 KiB   | +8%    | 90.4GB/s  |
| 16 KiB  | +4%    | 93.8GB/s  |
| 32 KiB  | +136%  | 41.3GB/s  | L1D, 32 KiB |
| 64 KiB  | +134%  | 41.6GB/s  |
| 128 KiB | +131%  | 42.2GB/s  |
| 256 KiB | +144%  | 40.0GB/s  |
| 512 KiB | +137%  | 41.2GB/s  |
| 1 MiB   | +162%  | 37.2GB/s  |
| 2 MiB   | +298%  | 24.5GB/s  | L2, 2 MiB   |
| 4 MiB   | +612%  | 13.7GB/s  |
| 8 MiB   | +1004% | 8.83GB/s  | L3, 6 MiB   |
| 16 MiB  | +1081% | 8.26GB/s  |
| 32 MiB  | +1091% | 8.19GB/s  |
| 64 MiB  | +1085% | 8.24GB/s  |
| 128 MiB | +1276% | 7.09GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   |  diff | bandwidth  | comment     |
|---------|-------|------------|-------------|
| 256 B   | +77%  | 56.5GB/s   |
| 512 B   | +20%  | 83.6GB/s   |
| 1 KiB   | +5%   | 95.7GB/s   |
| 2 KiB   | +7%   | 93.5GB/s   |
| 4 KiB   | +10%  | 91.5GB/s   | page, 4 KiB |
| 8 KiB   | -     | 100.2GB/s  |
| 16 KiB  | +1%   | 98.9GB/s   |
| 32 KiB  | +8%   | 92.7GB/s   | L1D, 32 KiB |
| 64 KiB  | +29%  | 77.6GB/s   |
| 128 KiB | +41%  | 70.9GB/s   |
| 256 KiB | +43%  | 70.3GB/s   |
| 512 KiB | +43%  | 70.2GB/s   |
| 1 MiB   | +41%  | 70.9GB/s   |
| 2 MiB   | +69%  | 59.3GB/s   | L2, 2 MiB   |
| 4 MiB   | +175% | 36.4GB/s   |
| 8 MiB   | +668% | 13.0GB/s   | L3, 6 MiB   |
| 16 MiB  | +849% | 10.6GB/s   |
| 32 MiB  | +755% | 11.7GB/s   |
| 64 MiB  | +716% | 12.3GB/s   |
| 128 MiB | +696% | 12.6GB/s   |

</details>

<details><summary><b>memset, multithreading (4 threads, 4 cores)</b></summary>

**memset**

| block   | diff   | bandwidth  | bandwidth per thread | comment |
|---------|--------|------------|-----------|-------------|
| 256 B   | +69%   | 141.6GB/s  | 35.4GB/s  |
| 512 B   | -      | 239.5GB/s  | 59.9GB/s  |
| 1 KiB   | +106%  | 116.4GB/s  | 29.1GB/s  |
| 2 KiB   | +42%   | 168.2GB/s  | 42.0GB/s  |
| 4 KiB   | +45%   | 164.7GB/s  | 41.2GB/s  | page, 4 KiB |
| 8 KiB   | +74%   | 137.5GB/s  | 34.4GB/s  |
| 16 KiB  | +225%  | 73.7GB/s   | 18.4GB/s  |
| 32 KiB  | +390%  | 48.8GB/s   | 12.2GB/s  | L1D, 32 KiB |
| 64 KiB  | +595%  | 34.5GB/s   | 8.62GB/s  |
| 128 KiB | +633%  | 32.7GB/s   | 8.17GB/s  |
| 256 KiB | +688%  | 30.4GB/s   | 7.60GB/s  |
| 512 KiB | +875%  | 24.6GB/s   | 6.14GB/s  |
| 1 MiB   | +940%  | 23.0GB/s   | 5.76GB/s  |
| 2 MiB   | +2433% | 9.46GB/s   | 2.36GB/s  | L2, 2 MiB   |
| 4 MiB   | +2516% | 9.16GB/s   | 2.29GB/s  |
| 8 MiB   | +2523% | 9.13GB/s   | 2.28GB/s  | L3, 6 MiB   |
| 16 MiB  | +2535% | 9.09GB/s   | 2.27GB/s  |
| 32 MiB  | +2668% | 8.65GB/s   | 2.16GB/s  |
| 64 MiB  | +2541% | 9.07GB/s   | 2.27GB/s  |
| 128 MiB | +2537% | 9.08GB/s   | 2.27GB/s  |

**SIMD cached fill**

| block   | diff   | bandwidth  | bandwidth per thread | comment |
|---------|--------|------------|-----------|-------------|
| 256 B   | +14%   | 240.5GB/s  | 60.1GB/s  |
| 512 B   | -      | 273.5GB/s  | 68.4GB/s  |
| 1 KiB   | +153%  | 108.2GB/s  | 27.1GB/s  |
| 2 KiB   | +60%   | 171.1GB/s  | 42.8GB/s  |
| 4 KiB   | +67%   | 164.2GB/s  | 41.0GB/s  | page, 4 KiB |
| 8 KiB   | +110%  | 130.2GB/s  | 32.6GB/s  |
| 16 KiB  | +145%  | 111.5GB/s  | 27.9GB/s  |
| 32 KiB  | +456%  | 49.2GB/s   | 12.3GB/s  | L1D, 32 KiB |
| 64 KiB  | +635%  | 37.2GB/s   | 9.30GB/s  |
| 128 KiB | +719%  | 33.4GB/s   | 8.35GB/s  |
| 256 KiB | +775%  | 31.3GB/s   | 7.81GB/s  |
| 512 KiB | +999%  | 24.9GB/s   | 6.22GB/s  |
| 1 MiB   | +1082% | 23.1GB/s   | 5.78GB/s  |
| 2 MiB   | +2794% | 9.45GB/s   | 2.36GB/s  | L2, 2 MiB   |
| 4 MiB   | +2891% | 9.14GB/s   | 2.29GB/s  |
| 8 MiB   | +2891% | 9.14GB/s   | 2.29GB/s  | L3, 6 MiB   |
| 16 MiB  | +2914% | 9.07GB/s   | 2.27GB/s  |
| 32 MiB  | +2913% | 9.08GB/s   | 2.27GB/s  |
| 64 MiB  | +2911% | 9.08GB/s   | 2.27GB/s  |
| 128 MiB | +2924% | 9.04GB/s   | 2.26GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +653% | 11.1GB/s  | 2.76GB/s  |
| 512 B   | +374% | 17.6GB/s  | 4.39GB/s  |
| 1 KiB   | +249% | 23.8GB/s  | 5.96GB/s  |
| 2 KiB   | +267% | 22.7GB/s  | 5.67GB/s  |
| 4 KiB   | +279% | 22.0GB/s  | 5.49GB/s  | page, 4 KiB |
| 8 KiB   | +290% | 21.3GB/s  | 5.33GB/s  |
| 16 KiB  | +289% | 21.4GB/s  | 5.35GB/s  |
| 32 KiB  | +290% | 21.4GB/s  | 5.34GB/s  | L1D, 32 KiB |
| 64 KiB  | +293% | 21.2GB/s  | 5.29GB/s  |
| 128 KiB | +290% | 21.3GB/s  | 5.34GB/s  |
| 256 KiB | +290% | 21.3GB/s  | 5.33GB/s  |
| 512 KiB | +293% | 21.2GB/s  | 5.29GB/s  |
| 1 MiB   | +293% | 21.2GB/s  | 5.30GB/s  |
| 2 MiB   | +293% | 21.2GB/s  | 5.29GB/s  | L2, 2 MiB   |
| 4 MiB   | -     | 20.8GB/s  | 5.20GB/s  |
| 8 MiB   | +1%   | 20.7GB/s  | 5.17GB/s  | L3, 6 MiB   |
| 16 MiB  | -     | 20.8GB/s  | 5.20GB/s  |
| 32 MiB  | -     | 20.8GB/s  | 5.20GB/s  |
| 64 MiB  | +1%   | 20.5GB/s  | 5.14GB/s  |
| 128 MiB | +1%   | 20.6GB/s  | 5.16GB/s  |

</details>

<details><summary><b>memcpy, multithreading (4 threads, 4 cores)</b></summary>

**memcpy**

| block   | diff   | bandwidth  | bandwidth per thread | comment |
|---------|--------|------------|-----------|-------------|
| 256 B   | +102%  | 113.6GB/s  | 28.4GB/s  |
| 512 B   | +52%   | 150.8GB/s  | 37.7GB/s  |
| 1 KiB   | +37%   | 167.6GB/s  | 41.9GB/s  |
| 2 KiB   | +7%    | 215.2GB/s  | 53.8GB/s  |
| 4 KiB   | -      | 229.9GB/s  | 57.5GB/s  | page, 4 KiB |
| 8 KiB   | -      | 229.5GB/s  | 57.4GB/s  |
| 16 KiB  | +75%   | 131.2GB/s  | 32.8GB/s  |
| 32 KiB  | +712%  | 28.3GB/s   | 7.07GB/s  | L1D, 32 KiB |
| 64 KiB  | +714%  | 28.2GB/s   | 7.06GB/s  |
| 128 KiB | +718%  | 28.1GB/s   | 7.03GB/s  |
| 256 KiB | +826%  | 24.8GB/s   | 6.21GB/s  |
| 512 KiB | +1217% | 17.5GB/s   | 4.36GB/s  |
| 1 MiB   | +1450% | 14.8GB/s   | 3.71GB/s  |
| 2 MiB   | +2248% | 9.79GB/s   | 2.45GB/s  | L2, 2 MiB   |
| 4 MiB   | +2244% | 9.81GB/s   | 2.45GB/s  |
| 8 MiB   | +2247% | 9.80GB/s   | 2.45GB/s  | L3, 6 MiB   |
| 16 MiB  | +2237% | 9.84GB/s   | 2.46GB/s  |
| 32 MiB  | +2231% | 9.86GB/s   | 2.47GB/s  |
| 64 MiB  | +2239% | 9.83GB/s   | 2.46GB/s  |
| 128 MiB | +2224% | 9.89GB/s   | 2.47GB/s  |

**SIMD cached copy**

| block   | diff   | bandwidth  | bandwidth per thread | comment |
|---------|--------|------------|-----------|-------------|
| 256 B   | +30%   | 179.9GB/s  | 45.0GB/s  |
| 512 B   | +24%   | 187.7GB/s  | 46.9GB/s  |
| 1 KiB   | +9%    | 214.5GB/s  | 53.6GB/s  |
| 2 KiB   | +8%    | 215.3GB/s  | 53.8GB/s  |
| 4 KiB   | -      | 233.1GB/s  | 58.3GB/s  | page, 4 KiB |
| 8 KiB   | +5%    | 222.3GB/s  | 55.6GB/s  |
| 16 KiB  | +23%   | 189.4GB/s  | 47.4GB/s  |
| 32 KiB  | +691%  | 29.5GB/s   | 7.37GB/s  | L1D, 32 KiB |
| 64 KiB  | +692%  | 29.4GB/s   | 7.36GB/s  |
| 128 KiB | +702%  | 29.1GB/s   | 7.26GB/s  |
| 256 KiB | +775%  | 26.7GB/s   | 6.66GB/s  |
| 512 KiB | +1129% | 19.0GB/s   | 4.74GB/s  |
| 1 MiB   | +1360% | 16.0GB/s   | 3.99GB/s  |
| 2 MiB   | +3459% | 6.55GB/s   | 1.64GB/s  | L2, 2 MiB   |
| 4 MiB   | +3541% | 6.40GB/s   | 1.60GB/s  |
| 8 MiB   | +3565% | 6.36GB/s   | 1.59GB/s  | L3, 6 MiB   |
| 16 MiB  | +3578% | 6.34GB/s   | 1.58GB/s  |
| 32 MiB  | +3567% | 6.36GB/s   | 1.59GB/s  |
| 64 MiB  | +3523% | 6.43GB/s   | 1.61GB/s  |
| 128 MiB | +3570% | 6.35GB/s   | 1.59GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth  | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +636% | 5.43GB/s   | 1.36GB/s  |
| 512 B   | +404% | 7.93GB/s   | 1.98GB/s  |
| 1 KiB   | +311% | 9.74GB/s   | 2.43GB/s  |
| 2 KiB   | +314% | 9.67GB/s   | 2.42GB/s  |
| 4 KiB   | +331% | 9.28GB/s   | 2.32GB/s  | page, 4 KiB |
| 8 KiB   | +321% | 9.51GB/s   | 2.38GB/s  |
| 16 KiB  | +315% | 9.63GB/s   | 2.41GB/s  |
| 32 KiB  | +295% | 10.1GB/s   | 2.53GB/s  | L1D, 32 KiB |
| 64 KiB  | +291% | 10.2GB/s   | 2.55GB/s  |
| 128 KiB | +288% | 10.3GB/s   | 2.58GB/s  |
| 256 KiB | +289% | 10.3GB/s   | 2.57GB/s  |
| 512 KiB | +291% | 10.2GB/s   | 2.56GB/s  |
| 1 MiB   | +293% | 10.2GB/s   | 2.54GB/s  |
| 2 MiB   | +298% | 10.1GB/s   | 2.51GB/s  | L2, 2 MiB   |
| 4 MiB   | -     | 10.00GB/s  | 2.50GB/s  |
| 8 MiB   | +1%   | 9.93GB/s   | 2.48GB/s  | L3, 6 MiB   |
| 16 MiB  | +1%   | 9.92GB/s   | 2.48GB/s  |
| 32 MiB  | +3%   | 9.68GB/s   | 2.42GB/s  |
| 64 MiB  | +2%   | 9.80GB/s   | 2.45GB/s  |
| 128 MiB | +1%   | 9.89GB/s   | 2.47GB/s  |

</details>

<details><summary><b>other, multithreading (4 threads, 4 cores)</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff   | bandwidth  | bandwidth per thread | comment |
|---------|--------|------------|-----------|-------------|
| 256 B   | +29%   | 221.6GB/s  | 55.4GB/s  |
| 512 B   | +32%   | 216.4GB/s  | 54.1GB/s  |
| 1 KiB   | +28%   | 223.7GB/s  | 55.9GB/s  |
| 2 KiB   | +3%    | 277.0GB/s  | 69.3GB/s  |
| 4 KiB   | +20%   | 238.3GB/s  | 59.6GB/s  | page, 4 KiB |
| 8 KiB   | -      | 285.5GB/s  | 71.4GB/s  |
| 16 KiB  | +8%    | 265.0GB/s  | 66.2GB/s  |
| 32 KiB  | +457%  | 51.2GB/s   | 12.8GB/s  | L1D, 32 KiB |
| 64 KiB  | +461%  | 50.9GB/s   | 12.7GB/s  |
| 128 KiB | +457%  | 51.3GB/s   | 12.8GB/s  |
| 256 KiB | +506%  | 47.1GB/s   | 11.8GB/s  |
| 512 KiB | +815%  | 31.2GB/s   | 7.80GB/s  |
| 1 MiB   | +1162% | 22.6GB/s   | 5.66GB/s  |
| 2 MiB   | +2527% | 10.9GB/s   | 2.72GB/s  | L2, 2 MiB   |
| 4 MiB   | +2833% | 9.74GB/s   | 2.43GB/s  |
| 8 MiB   | +2864% | 9.63GB/s   | 2.41GB/s  | L3, 6 MiB   |
| 16 MiB  | +2852% | 9.67GB/s   | 2.42GB/s  |
| 32 MiB  | +3001% | 9.21GB/s   | 2.30GB/s  |
| 64 MiB  | +2880% | 9.58GB/s   | 2.40GB/s  |
| 128 MiB | +2899% | 9.52GB/s   | 2.38GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff   | bandwidth  | bandwidth per thread | comment |
|---------|--------|------------|-----------|-------------|
| 256 B   | +36%   | 203.6GB/s  | 50.9GB/s  |
| 512 B   | +26%   | 220.1GB/s  | 55.0GB/s  |
| 1 KiB   | +10%   | 252.1GB/s  | 63.0GB/s  |
| 2 KiB   | -      | 276.4GB/s  | 69.1GB/s  |
| 4 KiB   | +9%    | 254.7GB/s  | 63.7GB/s  | page, 4 KiB |
| 8 KiB   | +9%    | 254.2GB/s  | 63.5GB/s  |
| 16 KiB  | +5%    | 263.3GB/s  | 65.8GB/s  |
| 32 KiB  | +121%  | 125.2GB/s  | 31.3GB/s  | L1D, 32 KiB |
| 64 KiB  | +116%  | 127.9GB/s  | 32.0GB/s  |
| 128 KiB | +109%  | 132.1GB/s  | 33.0GB/s  |
| 256 KiB | +180%  | 98.9GB/s   | 24.7GB/s  |
| 512 KiB | +508%  | 45.4GB/s   | 11.4GB/s  |
| 1 MiB   | +658%  | 36.5GB/s   | 9.12GB/s  |
| 2 MiB   | +1223% | 20.9GB/s   | 5.22GB/s  | L2, 2 MiB   |
| 4 MiB   | +1244% | 20.6GB/s   | 5.14GB/s  |
| 8 MiB   | +1252% | 20.4GB/s   | 5.11GB/s  | L3, 6 MiB   |
| 16 MiB  | +1256% | 20.4GB/s   | 5.10GB/s  |
| 32 MiB  | +1251% | 20.5GB/s   | 5.12GB/s  |
| 64 MiB  | +1237% | 20.7GB/s   | 5.17GB/s  |
| 128 MiB | +1248% | 20.5GB/s   | 5.13GB/s  |

</details>



# Samsung Exynos 7870

* Device: Samsung J7 Neo
* Memory: 2GB, LPDDR3, DC 32bit, 933MHz, 14.9GB/s

<details>
</details>


# MediaTek Dimensity 8200

* Device: Infinix GT 20 Pro
* Memory: 8GB, LPDDR5-6400, 3200 MHz, QC 16bit, **51.2 GB/s**

## HighPerformance core

* A78 Core:
	- 1x 3.1 GHz
	- SIMD: 2x 128b (8 scalar op/cy)
	- max copy: 99.2 GB/s
* Cache:
	- L1D: 64 KB (per core)
	- L2: 512 KB (per core)
	- L3: 4 MB (shared)

<details><summary><b>memset, single thread</b></summary>

**memset**

| block   | diff  | bandwidth | comment |
|---------|-------|------------|-------------|
| 256 B   | +15%  | 89.3GB/s   |
| 512 B   | +6%   | 96.7GB/s   |
| 1 KiB   | +2%   | 100.5GB/s  |
| 2 KiB   | -     | 102.6GB/s  |
| 4 KiB   | -     | 102.7GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 102.1GB/s  |
| 16 KiB  | -     | 102.4GB/s  |
| 32 KiB  | +1%   | 101.7GB/s  |
| 64 KiB  | +1%   | 101.6GB/s  |
| 128 KiB | +1%   | 101.4GB/s  |
| 256 KiB | +2%   | 101.0GB/s  |
| 512 KiB | +15%  | 89.2GB/s   |
| 1 MiB   | +67%  | 61.3GB/s   |
| 2 MiB   | +114% | 47.9GB/s   |
| 4 MiB   | +173% | 37.6GB/s   |
| 8 MiB   | +249% | 29.5GB/s   |
| 16 MiB  | +294% | 26.1GB/s   |
| 32 MiB  | +317% | 24.6GB/s   |
| 64 MiB  | +332% | 23.8GB/s   |
| 128 MiB | +352% | 22.7GB/s   |

**SIMD cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | -     | 99.3GB/s  |
| 512 B   | -     | 99.4GB/s  |
| 1 KiB   | -     | 99.3GB/s  |
| 2 KiB   | -     | 99.4GB/s  |
| 4 KiB   | -     | 99.3GB/s  | page, 4 KiB |
| 8 KiB   | -     | 99.3GB/s  |
| 16 KiB  | -     | 99.2GB/s  |
| 32 KiB  | +1%   | 98.4GB/s  |
| 64 KiB  | +1%   | 98.1GB/s  |
| 128 KiB | +1%   | 97.9GB/s  |
| 256 KiB | +2%   | 97.9GB/s  |
| 512 KiB | +6%   | 94.1GB/s  |
| 1 MiB   | +59%  | 62.4GB/s  |
| 2 MiB   | +109% | 47.5GB/s  |
| 4 MiB   | +181% | 35.4GB/s  |
| 8 MiB   | +245% | 28.8GB/s  |
| 16 MiB  | +288% | 25.6GB/s  |
| 32 MiB  | +315% | 23.9GB/s  |
| 64 MiB  | +323% | 23.5GB/s  |
| 128 MiB | +325% | 23.4GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | -     | 99.1GB/s  |
| 512 B   | -     | 99.4GB/s  |
| 1 KiB   | -     | 99.4GB/s  |
| 2 KiB   | -     | 99.3GB/s  |
| 4 KiB   | -     | 99.2GB/s  | page, 4 KiB |
| 8 KiB   | -     | 99.1GB/s  |
| 16 KiB  | -     | 99.4GB/s  |
| 32 KiB  | +2%   | 97.9GB/s  |
| 64 KiB  | +1%   | 98.2GB/s  |
| 128 KiB | +1%   | 98.3GB/s  |
| 256 KiB | +4%   | 95.6GB/s  |
| 512 KiB | +9%   | 91.5GB/s  |
| 1 MiB   | +61%  | 61.8GB/s  |
| 2 MiB   | +109% | 47.6GB/s  |
| 4 MiB   | +171% | 36.6GB/s  |
| 8 MiB   | +242% | 29.1GB/s  |
| 16 MiB  | +289% | 25.6GB/s  |
| 32 MiB  | +313% | 24.1GB/s  |
| 64 MiB  | +327% | 23.3GB/s  |
| 128 MiB | +327% | 23.3GB/s  |

</details>
<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +14%  | 61.8GB/s  |
| 512 B   | +5%   | 67.2GB/s  |
| 1 KiB   | +2%   | 69.2GB/s  |
| 2 KiB   | +1%   | 69.9GB/s  |
| 4 KiB   | -     | 70.4GB/s  | page, 4 KiB |
| 8 KiB   | -     | 70.4GB/s  |
| 16 KiB  | +4%   | 67.8GB/s  |
| 32 KiB  | +7%   | 66.0GB/s  |
| 64 KiB  | +52%  | 46.2GB/s  |
| 128 KiB | +31%  | 53.8GB/s  |
| 256 KiB | +59%  | 44.2GB/s  |
| 512 KiB | +131% | 30.4GB/s  |
| 1 MiB   | +205% | 23.1GB/s  |
| 2 MiB   | +235% | 21.0GB/s  |
| 4 MiB   | +278% | 18.6GB/s  |
| 8 MiB   | +314% | 17.0GB/s  |
| 16 MiB  | +341% | 16.0GB/s  |
| 32 MiB  | +352% | 15.6GB/s  |
| 64 MiB  | +361% | 15.3GB/s  |
| 128 MiB | +366% | 15.1GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +2%   | 69.6GB/s  |
| 512 B   | -     | 70.7GB/s  |
| 1 KiB   | -     | 70.4GB/s  |
| 2 KiB   | -     | 70.6GB/s  |
| 4 KiB   | -     | 70.6GB/s  | page, 4 KiB |
| 8 KiB   | -     | 70.4GB/s  |
| 16 KiB  | +4%   | 67.7GB/s  |
| 32 KiB  | +10%  | 64.3GB/s  |
| 64 KiB  | +52%  | 46.6GB/s  |
| 128 KiB | +32%  | 53.5GB/s  |
| 256 KiB | +63%  | 43.5GB/s  |
| 512 KiB | +133% | 30.4GB/s  |
| 1 MiB   | +208% | 23.0GB/s  |
| 2 MiB   | +238% | 20.9GB/s  |
| 4 MiB   | +283% | 18.5GB/s  |
| 8 MiB   | +323% | 16.7GB/s  |
| 16 MiB  | +343% | 16.0GB/s  |
| 32 MiB  | +353% | 15.6GB/s  |
| 64 MiB  | +362% | 15.3GB/s  |
| 128 MiB | +365% | 15.2GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | -     | 49.8GB/s  |
| 512 B   | -     | 49.7GB/s  |
| 1 KiB   | -     | 49.6GB/s  |
| 2 KiB   | -     | 49.6GB/s  |
| 4 KiB   | -     | 49.6GB/s  | page, 4 KiB |
| 8 KiB   | -     | 49.6GB/s  |
| 16 KiB  | -     | 49.6GB/s  |
| 32 KiB  | +1%   | 49.1GB/s  |
| 64 KiB  | +11%  | 44.6GB/s  |
| 128 KiB | +1%   | 49.2GB/s  |
| 256 KiB | +16%  | 42.9GB/s  |
| 512 KiB | +63%  | 30.4GB/s  |
| 1 MiB   | +120% | 22.7GB/s  |
| 2 MiB   | +186% | 17.4GB/s  |
| 4 MiB   | +183% | 17.6GB/s  |
| 8 MiB   | +198% | 16.7GB/s  |
| 16 MiB  | +213% | 15.9GB/s  |
| 32 MiB  | +222% | 15.5GB/s  |
| 64 MiB  | +232% | 15.0GB/s  |
| 128 MiB | +230% | 15.1GB/s  |

</details>
<details><summary><b>other, single thread</b></summary>

**SIMD fp32 sum cached**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +37%  | 63.6GB/s  |
| 512 B   | +3%   | 84.8GB/s  |
| 1 KiB   | +1%   | 86.5GB/s  |
| 2 KiB   | -     | 86.8GB/s  |
| 4 KiB   | -     | 86.9GB/s  | page, 4 KiB |
| 8 KiB   | -     | 87.0GB/s  |
| 16 KiB  | +5%   | 82.8GB/s  |
| 32 KiB  | +13%  | 76.9GB/s  |
| 64 KiB  | +46%  | 59.6GB/s  |
| 128 KiB | +40%  | 62.3GB/s  |
| 256 KiB | +135% | 37.1GB/s  |
| 512 KiB | +163% | 33.1GB/s  |
| 1 MiB   | +238% | 25.7GB/s  |
| 2 MiB   | +330% | 20.3GB/s  |
| 4 MiB   | +340% | 19.8GB/s  |
| 8 MiB   | +346% | 19.5GB/s  |
| 16 MiB  | +351% | 19.3GB/s  |
| 32 MiB  | +365% | 18.7GB/s  |
| 64 MiB  | +371% | 18.5GB/s  |
| 128 MiB | +376% | 18.3GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff  | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +25% | 26.5GB/s  |
| 512 B   | +13% | 29.5GB/s  |
| 1 KiB   | +6%  | 31.2GB/s  |
| 2 KiB   | +3%  | 32.1GB/s  |
| 4 KiB   | +2%  | 32.6GB/s  | page, 4 KiB |
| 8 KiB   | +1%  | 32.9GB/s  |
| 16 KiB  | +1%  | 33.0GB/s  |
| 32 KiB  | -    | 33.1GB/s  |
| 64 KiB  | -    | 33.1GB/s  |
| 128 KiB | -    | 33.2GB/s  |
| 256 KiB | -    | 33.2GB/s  |
| 512 KiB | +2%  | 32.5GB/s  |
| 1 MiB   | +1%  | 32.9GB/s  |
| 2 MiB   | +12% | 29.5GB/s  |
| 4 MiB   | +77% | 18.7GB/s  |
| 8 MiB   | +37% | 24.2GB/s  |
| 16 MiB  | +40% | 23.7GB/s  |
| 32 MiB  | +41% | 23.5GB/s  |
| 64 MiB  | +45% | 22.9GB/s  |
| 128 MiB | +44% | 23.0GB/s  |

</details>

## Performance core

* A78 Core:
	- 3x 3 GHz
	- SIMD: 2x 128b (8 scalar op/cy)
	- max copy: 96 GB/s per core
* Cache:
	- L1D: 64 KB (per core)
	- L2: 512 KB (per core)
	- L3: 4 MB (shared)

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +23%  | 48.3GB/s  |
| 512 B   | +5%   | 56.8GB/s  |
| 1 KiB   | +5%   | 56.6GB/s  |
| 2 KiB   | +3%   | 57.5GB/s  |
| 4 KiB   | +1%   | 58.7GB/s  | page, 4 KiB |
| 8 KiB   | -     | 59.5GB/s  |
| 16 KiB  | +7%   | 55.4GB/s  |
| 32 KiB  | +18%  | 50.2GB/s  |
| 64 KiB  | +57%  | 37.9GB/s  |
| 128 KiB | +38%  | 43.2GB/s  |
| 256 KiB | +74%  | 34.2GB/s  |
| 512 KiB | +183% | 21.0GB/s  |
| 1 MiB   | +255% | 16.7GB/s  |
| 2 MiB   | +291% | 15.2GB/s  |
| 4 MiB   | +338% | 13.6GB/s  |
| 8 MiB   | +374% | 12.5GB/s  |
| 16 MiB  | +374% | 12.5GB/s  |
| 32 MiB  | +396% | 12.0GB/s  |
| 64 MiB  | +377% | 12.5GB/s  |
| 128 MiB | +376% | 12.5GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +8%   | 54.9GB/s  |
| 512 B   | +1%   | 58.7GB/s  |
| 1 KiB   | +1%   | 59.2GB/s  |
| 2 KiB   | -     | 59.5GB/s  |
| 4 KiB   | +2%   | 58.3GB/s  | page, 4 KiB |
| 8 KiB   | +2%   | 58.1GB/s  |
| 16 KiB  | +4%   | 57.2GB/s  |
| 32 KiB  | +15%  | 51.9GB/s  |
| 64 KiB  | +57%  | 37.9GB/s  |
| 128 KiB | +39%  | 42.8GB/s  |
| 256 KiB | +86%  | 32.1GB/s  |
| 512 KiB | +184% | 20.9GB/s  |
| 1 MiB   | +246% | 17.2GB/s  |
| 2 MiB   | +292% | 15.2GB/s  |
| 4 MiB   | +347% | 13.3GB/s  |
| 8 MiB   | +375% | 12.5GB/s  |
| 16 MiB  | +390% | 12.2GB/s  |
| 32 MiB  | +382% | 12.3GB/s  |
| 64 MiB  | +392% | 12.1GB/s  |
| 128 MiB | +380% | 12.4GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | -     | 42.6GB/s  |
| 512 B   | +3%   | 41.5GB/s  |
| 1 KiB   | +2%   | 41.7GB/s  |
| 2 KiB   | +2%   | 41.9GB/s  |
| 4 KiB   | +4%   | 40.9GB/s  | page, 4 KiB |
| 8 KiB   | +3%   | 41.2GB/s  |
| 16 KiB  | +6%   | 40.3GB/s  |
| 32 KiB  | +2%   | 41.7GB/s  |
| 64 KiB  | +17%  | 36.3GB/s  |
| 128 KiB | +4%   | 40.8GB/s  |
| 256 KiB | +32%  | 32.3GB/s  |
| 512 KiB | +82%  | 23.4GB/s  |
| 1 MiB   | +152% | 16.9GB/s  |
| 2 MiB   | +199% | 14.2GB/s  |
| 4 MiB   | +225% | 13.1GB/s  |
| 8 MiB   | +248% | 12.3GB/s  |
| 16 MiB  | +255% | 12.0GB/s  |
| 32 MiB  | +260% | 11.8GB/s  |
| 64 MiB  | +258% | 11.9GB/s  |
| 128 MiB | +250% | 12.2GB/s  |

</details>
<details><summary><b>memcpy, multithreading (3 threads, 3 cores)</b></summary>

**memcpy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +24%  | 93.4GB/s   | 31.1GB/s  |
| 512 B   | +30%  | 89.3GB/s   | 29.8GB/s  |
| 1 KiB   | +20%  | 96.1GB/s   | 32.0GB/s  |
| 2 KiB   | +20%  | 96.5GB/s   | 32.2GB/s  |
| 4 KiB   | -     | 115.6GB/s  | 38.5GB/s  | page, 4 KiB |
| 8 KiB   | +12%  | 103.3GB/s  | 34.4GB/s  |
| 16 KiB  | +23%  | 93.6GB/s   | 31.2GB/s  |
| 32 KiB  | +23%  | 94.2GB/s   | 31.4GB/s  |
| 64 KiB  | +76%  | 65.8GB/s   | 21.9GB/s  |
| 128 KiB | +58%  | 73.0GB/s   | 24.3GB/s  |
| 256 KiB | +101% | 57.6GB/s   | 19.2GB/s  |
| 512 KiB | +256% | 32.5GB/s   | 10.8GB/s  |
| 1 MiB   | +370% | 24.6GB/s   | 8.20GB/s  |
| 2 MiB   | +502% | 19.2GB/s   | 6.41GB/s  |
| 4 MiB   | +599% | 16.5GB/s   | 5.51GB/s  |
| 8 MiB   | +646% | 15.5GB/s   | 5.16GB/s  |
| 16 MiB  | +657% | 15.3GB/s   | 5.09GB/s  |
| 32 MiB  | +663% | 15.1GB/s   | 5.05GB/s  |
| 64 MiB  | +660% | 15.2GB/s   | 5.07GB/s  |
| 128 MiB | +671% | 15.0GB/s   | 5.00GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +7%   | 98.1GB/s   | 32.7GB/s  |
| 512 B   | -     | 105.2GB/s  | 35.1GB/s  |
| 1 KiB   | +6%   | 99.6GB/s   | 33.2GB/s  |
| 2 KiB   | +1%   | 104.8GB/s  | 34.9GB/s  |
| 4 KiB   | -     | 105.5GB/s  | 35.2GB/s  | page, 4 KiB |
| 8 KiB   | +3%   | 102.1GB/s  | 34.0GB/s  |
| 16 KiB  | +8%   | 98.1GB/s   | 32.7GB/s  |
| 32 KiB  | +10%  | 95.7GB/s   | 31.9GB/s  |
| 64 KiB  | +58%  | 66.8GB/s   | 22.3GB/s  |
| 128 KiB | +44%  | 73.0GB/s   | 24.3GB/s  |
| 256 KiB | +94%  | 54.3GB/s   | 18.1GB/s  |
| 512 KiB | +226% | 32.3GB/s   | 10.8GB/s  |
| 1 MiB   | +319% | 25.1GB/s   | 8.38GB/s  |
| 2 MiB   | +439% | 19.6GB/s   | 6.53GB/s  |
| 4 MiB   | +535% | 16.6GB/s   | 5.53GB/s  |
| 8 MiB   | +577% | 15.6GB/s   | 5.19GB/s  |
| 16 MiB  | +595% | 15.2GB/s   | 5.06GB/s  |
| 32 MiB  | +593% | 15.2GB/s   | 5.07GB/s  |
| 64 MiB  | +598% | 15.1GB/s   | 5.03GB/s  |
| 128 MiB | +598% | 15.1GB/s   | 5.04GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +1%   | 92.0GB/s  | 30.7GB/s  |
| 512 B   | -     | 92.8GB/s  | 30.9GB/s  |
| 1 KiB   | +2%   | 90.7GB/s  | 30.2GB/s  |
| 2 KiB   | +4%   | 89.4GB/s  | 29.8GB/s  |
| 4 KiB   | +4%   | 88.9GB/s  | 29.6GB/s  | page, 4 KiB |
| 8 KiB   | +3%   | 89.8GB/s  | 29.9GB/s  |
| 16 KiB  | +3%   | 90.5GB/s  | 30.2GB/s  |
| 32 KiB  | +6%   | 87.7GB/s  | 29.2GB/s  |
| 64 KiB  | +20%  | 77.5GB/s  | 25.8GB/s  |
| 128 KiB | +12%  | 83.0GB/s  | 27.7GB/s  |
| 256 KiB | +39%  | 66.8GB/s  | 22.3GB/s  |
| 512 KiB | +179% | 33.3GB/s  | 11.1GB/s  |
| 1 MiB   | +263% | 25.6GB/s  | 8.53GB/s  |
| 2 MiB   | +380% | 19.3GB/s  | 6.44GB/s  |
| 4 MiB   | +44%  | 16.1GB/s  | 5.38GB/s  |
| 8 MiB   | +51%  | 15.4GB/s  | 5.14GB/s  |
| 16 MiB  | +54%  | 15.1GB/s  | 5.02GB/s  |
| 32 MiB  | +56%  | 14.9GB/s  | 4.96GB/s  |
| 64 MiB  | +54%  | 15.0GB/s  | 5.01GB/s  |
| 128 MiB | +55%  | 15.0GB/s  | 4.99GB/s  |

</details>
<details><summary><b>memset, single thread</b></summary>

**memset**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +25%  | 68.6GB/s  |
| 512 B   | +9%   | 78.6GB/s  |
| 1 KiB   | +2%   | 84.0GB/s  |
| 2 KiB   | +3%   | 82.8GB/s  |
| 4 KiB   | +4%   | 82.5GB/s  | page, 4 KiB |
| 8 KiB   | +4%   | 82.3GB/s  |
| 16 KiB  | -     | 85.6GB/s  |
| 32 KiB  | +3%   | 82.7GB/s  |
| 64 KiB  | +2%   | 83.5GB/s  |
| 128 KiB | +4%   | 82.1GB/s  |
| 256 KiB | +7%   | 79.9GB/s  |
| 512 KiB | +73%  | 49.4GB/s  |
| 1 MiB   | +111% | 40.5GB/s  |
| 2 MiB   | +146% | 34.8GB/s  |
| 4 MiB   | +187% | 29.8GB/s  |
| 8 MiB   | +249% | 24.5GB/s  |
| 16 MiB  | +300% | 21.4GB/s  |
| 32 MiB  | +329% | 19.9GB/s  |
| 64 MiB  | +334% | 19.7GB/s  |
| 128 MiB | +330% | 19.9GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +5%   | 79.8GB/s  |
| 512 B   | -     | 83.6GB/s  |
| 1 KiB   | +7%   | 78.0GB/s  |
| 2 KiB   | +9%   | 76.9GB/s  |
| 4 KiB   | +3%   | 81.0GB/s  | page, 4 KiB |
| 8 KiB   | -     | 83.3GB/s  |
| 16 KiB  | +4%   | 80.6GB/s  |
| 32 KiB  | +1%   | 82.5GB/s  |
| 64 KiB  | +2%   | 81.6GB/s  |
| 128 KiB | +10%  | 76.3GB/s  |
| 256 KiB | +12%  | 74.6GB/s  |
| 512 KiB | +73%  | 48.3GB/s  |
| 1 MiB   | +106% | 40.6GB/s  |
| 2 MiB   | +141% | 34.7GB/s  |
| 4 MiB   | +177% | 30.2GB/s  |
| 8 MiB   | +242% | 24.4GB/s  |
| 16 MiB  | +287% | 21.6GB/s  |
| 32 MiB  | +317% | 20.0GB/s  |
| 64 MiB  | +329% | 19.5GB/s  |
| 128 MiB | +320% | 19.9GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +4%   | 82.0GB/s  |
| 512 B   | +4%   | 82.0GB/s  |
| 1 KiB   | +3%   | 82.7GB/s  |
| 2 KiB   | -     | 85.3GB/s  |
| 4 KiB   | +3%   | 82.6GB/s  | page, 4 KiB |
| 8 KiB   | +2%   | 83.5GB/s  |
| 16 KiB  | +8%   | 78.7GB/s  |
| 32 KiB  | +11%  | 76.6GB/s  |
| 64 KiB  | +9%   | 78.4GB/s  |
| 128 KiB | +6%   | 80.1GB/s  |
| 256 KiB | +14%  | 74.5GB/s  |
| 512 KiB | +76%  | 48.4GB/s  |
| 1 MiB   | +121% | 38.6GB/s  |
| 2 MiB   | +141% | 35.4GB/s  |
| 4 MiB   | +187% | 29.7GB/s  |
| 8 MiB   | +248% | 24.5GB/s  |
| 16 MiB  | +300% | 21.3GB/s  |
| 32 MiB  | +327% | 20.0GB/s  |
| 64 MiB  | +340% | 19.4GB/s  |
| 128 MiB | +342% | 19.3GB/s  |

</details>
<details><summary><b>memset, multithreading (3 threads, 3 cores)</b></summary>

**memset**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +15%  | 111.7GB/s  | 37.2GB/s  |
| 512 B   | -     | 128.4GB/s  | 42.8GB/s  |
| 1 KiB   | +9%   | 118.3GB/s  | 39.4GB/s  |
| 2 KiB   | +6%   | 120.6GB/s  | 40.2GB/s  |
| 4 KiB   | +8%   | 119.0GB/s  | 39.7GB/s  | page, 4 KiB |
| 8 KiB   | +3%   | 124.7GB/s  | 41.6GB/s  |
| 16 KiB  | +1%   | 127.7GB/s  | 42.6GB/s  |
| 32 KiB  | +4%   | 122.9GB/s  | 41.0GB/s  |
| 64 KiB  | +1%   | 127.7GB/s  | 42.6GB/s  |
| 128 KiB | +18%  | 108.9GB/s  | 36.3GB/s  |
| 256 KiB | +74%  | 73.6GB/s   | 24.5GB/s  |
| 512 KiB | +125% | 57.1GB/s   | 19.0GB/s  |
| 1 MiB   | +202% | 42.5GB/s   | 14.2GB/s  |
| 2 MiB   | +361% | 27.8GB/s   | 9.28GB/s  |
| 4 MiB   | +444% | 23.6GB/s   | 7.86GB/s  |
| 8 MiB   | +475% | 22.3GB/s   | 7.44GB/s  |
| 16 MiB  | +487% | 21.9GB/s   | 7.29GB/s  |
| 32 MiB  | +496% | 21.5GB/s   | 7.17GB/s  |
| 64 MiB  | +497% | 21.5GB/s   | 7.16GB/s  |
| 128 MiB | +493% | 21.6GB/s   | 7.22GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +6%   | 122.3GB/s  | 40.8GB/s  |
| 512 B   | +4%   | 124.7GB/s  | 41.6GB/s  |
| 1 KiB   | +1%   | 128.1GB/s  | 42.7GB/s  |
| 2 KiB   | +1%   | 129.1GB/s  | 43.0GB/s  |
| 4 KiB   | +16%  | 112.5GB/s  | 37.5GB/s  | page, 4 KiB |
| 8 KiB   | +10%  | 118.5GB/s  | 39.5GB/s  |
| 16 KiB  | -     | 129.9GB/s  | 43.3GB/s  |
| 32 KiB  | +13%  | 115.2GB/s  | 38.4GB/s  |
| 64 KiB  | +7%   | 121.2GB/s  | 40.4GB/s  |
| 128 KiB | +13%  | 115.1GB/s  | 38.4GB/s  |
| 256 KiB | +82%  | 71.5GB/s   | 23.8GB/s  |
| 512 KiB | +147% | 52.6GB/s   | 17.5GB/s  |
| 1 MiB   | +203% | 42.9GB/s   | 14.3GB/s  |
| 2 MiB   | +360% | 28.2GB/s   | 9.42GB/s  |
| 4 MiB   | +455% | 23.4GB/s   | 7.81GB/s  |
| 8 MiB   | +489% | 22.1GB/s   | 7.35GB/s  |
| 16 MiB  | +493% | 21.9GB/s   | 7.30GB/s  |
| 32 MiB  | +501% | 21.6GB/s   | 7.21GB/s  |
| 64 MiB  | +501% | 21.6GB/s   | 7.21GB/s  |
| 128 MiB | +503% | 21.5GB/s   | 7.18GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +2%   | 171.1GB/s  | 57.0GB/s  |
| 512 B   | +2%   | 170.9GB/s  | 57.0GB/s  |
| 1 KiB   | +1%   | 172.6GB/s  | 57.5GB/s  |
| 2 KiB   | -     | 174.3GB/s  | 58.1GB/s  |
| 4 KiB   | +2%   | 171.6GB/s  | 57.2GB/s  | page, 4 KiB |
| 8 KiB   | +2%   | 170.4GB/s  | 56.8GB/s  |
| 16 KiB  | +1%   | 172.4GB/s  | 57.5GB/s  |
| 32 KiB  | +4%   | 167.9GB/s  | 56.0GB/s  |
| 64 KiB  | +7%   | 162.7GB/s  | 54.2GB/s  |
| 128 KiB | +13%  | 154.8GB/s  | 51.6GB/s  |
| 256 KiB | +78%  | 97.7GB/s   | 32.6GB/s  |
| 512 KiB | +147% | 70.4GB/s   | 23.5GB/s  |
| 1 MiB   | +276% | 46.3GB/s   | 15.4GB/s  |
| 2 MiB   | +481% | 30.0GB/s   | 10.0GB/s  |
| 4 MiB   | +81%  | 24.0GB/s   | 8.01GB/s  |
| 8 MiB   | +97%  | 22.1GB/s   | 7.37GB/s  |
| 16 MiB  | +100% | 21.8GB/s   | 7.27GB/s  |
| 32 MiB  | +101% | 21.6GB/s   | 7.21GB/s  |
| 64 MiB  | +102% | 21.6GB/s   | 7.20GB/s  |
| 128 MiB | +102% | 21.6GB/s   | 7.19GB/s  |

</details>
<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +43%  | 51.5GB/s  |
| 512 B   | +7%   | 69.2GB/s  |
| 1 KiB   | +1%   | 72.9GB/s  |
| 2 KiB   | +3%   | 71.7GB/s  |
| 4 KiB   | +4%   | 71.0GB/s  | page, 4 KiB |
| 8 KiB   | -     | 73.8GB/s  |
| 16 KiB  | +7%   | 69.2GB/s  |
| 32 KiB  | +15%  | 64.0GB/s  |
| 64 KiB  | +54%  | 47.9GB/s  |
| 128 KiB | +48%  | 49.8GB/s  |
| 256 KiB | +142% | 30.5GB/s  |
| 512 KiB | +213% | 23.6GB/s  |
| 1 MiB   | +277% | 19.6GB/s  |
| 2 MiB   | +329% | 17.2GB/s  |
| 4 MiB   | +382% | 15.3GB/s  |
| 8 MiB   | +418% | 14.3GB/s  |
| 16 MiB  | +435% | 13.8GB/s  |
| 32 MiB  | +427% | 14.0GB/s  |
| 64 MiB  | +430% | 13.9GB/s  |
| 128 MiB | +435% | 13.8GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff  | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +28% | 21.8GB/s  |
| 512 B   | +12% | 24.9GB/s  |
| 1 KiB   | +10% | 25.4GB/s  |
| 2 KiB   | +4%  | 26.8GB/s  |
| 4 KiB   | +4%  | 26.7GB/s  | page, 4 KiB |
| 8 KiB   | +5%  | 26.7GB/s  |
| 16 KiB  | +4%  | 27.0GB/s  |
| 32 KiB  | -    | 27.9GB/s  |
| 64 KiB  | +1%  | 27.8GB/s  |
| 128 KiB | +8%  | 25.8GB/s  |
| 256 KiB | +8%  | 26.0GB/s  |
| 512 KiB | +8%  | 25.8GB/s  |
| 1 MiB   | +8%  | 25.8GB/s  |
| 2 MiB   | +26% | 22.1GB/s  |
| 4 MiB   | +58% | 17.7GB/s  |
| 8 MiB   | +64% | 17.1GB/s  |
| 16 MiB  | +54% | 18.1GB/s  |
| 32 MiB  | +55% | 18.0GB/s  |
| 64 MiB  | +59% | 17.6GB/s  |
| 128 MiB | +61% | 17.3GB/s  |

</details>
<details><summary><b>other, multithreading (3 threads, 3 cores)</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth  | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +45%  | 82.4GB/s   | 27.5GB/s  |
| 512 B   | +14%  | 104.9GB/s  | 35.0GB/s  |
| 1 KiB   | +5%   | 112.9GB/s  | 37.6GB/s  |
| 2 KiB   | +14%  | 104.3GB/s  | 34.8GB/s  |
| 4 KiB   | -     | 119.1GB/s  | 39.7GB/s  | page, 4 KiB |
| 8 KiB   | +11%  | 107.6GB/s  | 35.9GB/s  |
| 16 KiB  | +79%  | 66.4GB/s   | 22.1GB/s  |
| 32 KiB  | +73%  | 69.0GB/s   | 23.0GB/s  |
| 64 KiB  | +99%  | 59.9GB/s   | 20.0GB/s  |
| 128 KiB | +109% | 56.9GB/s   | 19.0GB/s  |
| 256 KiB | +124% | 53.3GB/s   | 17.8GB/s  |
| 512 KiB | +180% | 42.6GB/s   | 14.2GB/s  |
| 1 MiB   | +248% | 34.2GB/s   | 11.4GB/s  |
| 2 MiB   | +324% | 28.1GB/s   | 9.35GB/s  |
| 4 MiB   | +391% | 24.2GB/s   | 8.08GB/s  |
| 8 MiB   | +422% | 22.8GB/s   | 7.60GB/s  |
| 16 MiB  | +433% | 22.3GB/s   | 7.45GB/s  |
| 32 MiB  | +438% | 22.2GB/s   | 7.38GB/s  |
| 64 MiB  | +440% | 22.0GB/s   | 7.35GB/s  |
| 128 MiB | +442% | 22.0GB/s   | 7.33GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +8%   | 72.4GB/s  | 24.1GB/s  |
| 512 B   | -     | 77.8GB/s  | 25.9GB/s  |
| 1 KiB   | +24%  | 62.7GB/s  | 20.9GB/s  |
| 2 KiB   | +38%  | 56.3GB/s  | 18.8GB/s  |
| 4 KiB   | +35%  | 57.5GB/s  | 19.2GB/s  | page, 4 KiB |
| 8 KiB   | +57%  | 49.5GB/s  | 16.5GB/s  |
| 16 KiB  | +58%  | 49.1GB/s  | 16.4GB/s  |
| 32 KiB  | +58%  | 49.3GB/s  | 16.4GB/s  |
| 64 KiB  | +67%  | 46.7GB/s  | 15.6GB/s  |
| 128 KiB | +63%  | 47.9GB/s  | 16.0GB/s  |
| 256 KiB | +74%  | 44.6GB/s  | 14.9GB/s  |
| 512 KiB | +84%  | 42.3GB/s  | 14.1GB/s  |
| 1 MiB   | +92%  | 40.5GB/s  | 13.5GB/s  |
| 2 MiB   | +106% | 37.7GB/s  | 12.6GB/s  |
| 4 MiB   | +113% | 36.5GB/s  | 12.2GB/s  |
| 8 MiB   | +109% | 37.3GB/s  | 12.4GB/s  |
| 16 MiB  | +113% | 36.4GB/s  | 12.1GB/s  |
| 32 MiB  | +111% | 36.8GB/s  | 12.3GB/s  |
| 64 MiB  | +111% | 36.9GB/s  | 12.3GB/s  |
| 128 MiB | +114% | 36.4GB/s  | 12.1GB/s  |

</details>


## EnergyEfficient core

* A55 Core:
	- 4x 2 GHz
	- SIMD: 128b (4 scalar op/cy)
	- max copy: 32 GB/s per core
* Cache:
	- L1D: 64 KB (per core)
	- L2: 256 KB (per core)
	- L3: 4 MB (shared)

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +21%  | 7.06GB/s  |
| 512 B   | +9%   | 7.83GB/s  |
| 1 KiB   | +2%   | 8.30GB/s  |
| 2 KiB   | -     | 8.51GB/s  |
| 4 KiB   | -     | 8.51GB/s  | page, 4 KiB |
| 8 KiB   | +2%   | 8.34GB/s  |
| 16 KiB  | +19%  | 7.13GB/s  |
| 32 KiB  | +20%  | 7.11GB/s  |
| 64 KiB  | +22%  | 6.95GB/s  |
| 128 KiB | +24%  | 6.89GB/s  |
| 256 KiB | +23%  | 6.93GB/s  |
| 512 KiB | +22%  | 6.95GB/s  |
| 1 MiB   | +28%  | 6.67GB/s  |
| 2 MiB   | +54%  | 5.54GB/s  |
| 4 MiB   | +98%  | 4.29GB/s  |
| 8 MiB   | +110% | 4.05GB/s  |
| 16 MiB  | +113% | 4.00GB/s  |
| 32 MiB  | +108% | 4.09GB/s  |
| 64 MiB  | +116% | 3.94GB/s  |
| 128 MiB | +108% | 4.09GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +7%  | 6.68GB/s  |
| 512 B   | +3%  | 6.95GB/s  |
| 1 KiB   | +1%  | 7.11GB/s  |
| 2 KiB   | -    | 7.16GB/s  |
| 4 KiB   | -    | 7.14GB/s  | page, 4 KiB |
| 8 KiB   | +4%  | 6.85GB/s  |
| 16 KiB  | +10% | 6.48GB/s  |
| 32 KiB  | +10% | 6.50GB/s  |
| 64 KiB  | +10% | 6.52GB/s  |
| 128 KiB | +11% | 6.46GB/s  |
| 256 KiB | +11% | 6.47GB/s  |
| 512 KiB | +11% | 6.44GB/s  |
| 1 MiB   | +15% | 6.21GB/s  |
| 2 MiB   | +32% | 5.42GB/s  |
| 4 MiB   | +63% | 4.40GB/s  |
| 8 MiB   | +67% | 4.29GB/s  |
| 16 MiB  | +78% | 4.03GB/s  |
| 32 MiB  | +77% | 4.05GB/s  |
| 64 MiB  | +72% | 4.16GB/s  |
| 128 MiB | +79% | 4.00GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +7%  | 6.76GB/s  |
| 512 B   | +3%  | 7.01GB/s  |
| 1 KiB   | +2%  | 7.09GB/s  |
| 2 KiB   | +1%  | 7.17GB/s  |
| 4 KiB   | -    | 7.22GB/s  | page, 4 KiB |
| 8 KiB   | -    | 7.20GB/s  |
| 16 KiB  | +1%  | 7.16GB/s  |
| 32 KiB  | +10% | 6.54GB/s  |
| 64 KiB  | +13% | 6.41GB/s  |
| 128 KiB | +15% | 6.25GB/s  |
| 256 KiB | +17% | 6.19GB/s  |
| 512 KiB | +17% | 6.15GB/s  |
| 1 MiB   | +19% | 6.06GB/s  |
| 2 MiB   | +46% | 4.95GB/s  |
| 4 MiB   | +77% | 4.07GB/s  |
| 8 MiB   | +80% | 4.02GB/s  |
| 16 MiB  | +86% | 3.87GB/s  |
| 32 MiB  | +80% | 4.00GB/s  |
| 64 MiB  | +86% | 3.87GB/s  |
| 128 MiB | +86% | 3.88GB/s  |

</details>
<details><summary><b>memcpy, multithreading (4 threads, 4 cores)</b></summary>

**memcpy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +16%  | 25.2GB/s  | 6.31GB/s  |
| 512 B   | +4%   | 28.3GB/s  | 7.08GB/s  |
| 1 KiB   | -     | 29.4GB/s  | 7.35GB/s  |
| 2 KiB   | +3%   | 28.6GB/s  | 7.14GB/s  |
| 4 KiB   | +4%   | 28.3GB/s  | 7.08GB/s  | page, 4 KiB |
| 8 KiB   | +3%   | 28.6GB/s  | 7.15GB/s  |
| 16 KiB  | +50%  | 19.5GB/s  | 4.88GB/s  |
| 32 KiB  | +28%  | 23.0GB/s  | 5.74GB/s  |
| 64 KiB  | +19%  | 24.6GB/s  | 6.16GB/s  |
| 128 KiB | +37%  | 21.4GB/s  | 5.35GB/s  |
| 256 KiB | +60%  | 18.3GB/s  | 4.59GB/s  |
| 512 KiB | +67%  | 17.5GB/s  | 4.39GB/s  |
| 1 MiB   | +128% | 12.9GB/s  | 3.22GB/s  |
| 2 MiB   | +122% | 13.2GB/s  | 3.30GB/s  |
| 4 MiB   | +119% | 13.4GB/s  | 3.35GB/s  |
| 8 MiB   | +121% | 13.3GB/s  | 3.33GB/s  |
| 16 MiB  | +121% | 13.3GB/s  | 3.33GB/s  |
| 32 MiB  | +121% | 13.3GB/s  | 3.32GB/s  |
| 64 MiB  | +120% | 13.4GB/s  | 3.34GB/s  |
| 128 MiB | +124% | 13.1GB/s  | 3.28GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|------|-----------|-----------|-------------|
| 256 B   | +5%  | 24.1GB/s  | 6.03GB/s  |
| 512 B   | +1%  | 25.1GB/s  | 6.27GB/s  |
| 1 KiB   | -    | 25.3GB/s  | 6.34GB/s  |
| 2 KiB   | +2%  | 24.9GB/s  | 6.23GB/s  |
| 4 KiB   | +5%  | 24.1GB/s  | 6.02GB/s  | page, 4 KiB |
| 8 KiB   | +17% | 21.7GB/s  | 5.42GB/s  |
| 16 KiB  | +33% | 19.1GB/s  | 4.78GB/s  |
| 32 KiB  | +25% | 20.3GB/s  | 5.09GB/s  |
| 64 KiB  | +12% | 22.6GB/s  | 5.65GB/s  |
| 128 KiB | +30% | 19.5GB/s  | 4.88GB/s  |
| 256 KiB | +39% | 18.2GB/s  | 4.55GB/s  |
| 512 KiB | +44% | 17.6GB/s  | 4.41GB/s  |
| 1 MiB   | +97% | 12.9GB/s  | 3.22GB/s  |
| 2 MiB   | +89% | 13.4GB/s  | 3.35GB/s  |
| 4 MiB   | +90% | 13.3GB/s  | 3.34GB/s  |
| 8 MiB   | +89% | 13.4GB/s  | 3.35GB/s  |
| 16 MiB  | +89% | 13.4GB/s  | 3.36GB/s  |
| 32 MiB  | +90% | 13.3GB/s  | 3.34GB/s  |
| 64 MiB  | +90% | 13.4GB/s  | 3.34GB/s  |
| 128 MiB | +91% | 13.3GB/s  | 3.32GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +59%  | 25.1GB/s  | 6.29GB/s  |
| 512 B   | +53%  | 26.2GB/s  | 6.56GB/s  |
| 1 KiB   | +51%  | 26.5GB/s  | 6.63GB/s  |
| 2 KiB   | +49%  | 26.9GB/s  | 6.71GB/s  |
| 4 KiB   | +48%  | 27.2GB/s  | 6.79GB/s  | page, 4 KiB |
| 8 KiB   | +47%  | 27.2GB/s  | 6.79GB/s  |
| 16 KiB  | +48%  | 27.1GB/s  | 6.77GB/s  |
| 32 KiB  | +64%  | 24.4GB/s  | 6.11GB/s  |
| 64 KiB  | +76%  | 22.8GB/s  | 5.70GB/s  |
| 128 KiB | +138% | 16.8GB/s  | 4.20GB/s  |
| 256 KiB | +186% | 14.0GB/s  | 3.50GB/s  |
| 512 KiB | +203% | 13.2GB/s  | 3.30GB/s  |
| 1 MiB   | +285% | 10.4GB/s  | 2.60GB/s  |
| 2 MiB   | +297% | 10.1GB/s  | 2.52GB/s  |
| 4 MiB   | +1%   | 9.95GB/s  | 2.49GB/s  |
| 8 MiB   | +1%   | 9.92GB/s  | 2.48GB/s  |
| 16 MiB  | +1%   | 9.89GB/s  | 2.47GB/s  |
| 32 MiB  | +1%   | 9.93GB/s  | 2.48GB/s  |
| 64 MiB  | +1%   | 9.94GB/s  | 2.49GB/s  |
| 128 MiB | -     | 10.0GB/s  | 2.50GB/s  |

</details>
<details><summary><b>memset, single thread</b></summary>

**memset**

| block   | diff  | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +66% | 14.2GB/s  |
| 512 B   | +32% | 17.8GB/s  |
| 1 KiB   | +15% | 20.4GB/s  |
| 2 KiB   | +7%  | 21.9GB/s  |
| 4 KiB   | +3%  | 22.8GB/s  | page, 4 KiB |
| 8 KiB   | +2%  | 23.0GB/s  |
| 16 KiB  | +1%  | 23.4GB/s  |
| 32 KiB  | -    | 23.5GB/s  |
| 64 KiB  | +1%  | 23.3GB/s  |
| 128 KiB | +3%  | 22.9GB/s  |
| 256 KiB | +8%  | 21.7GB/s  |
| 512 KiB | +5%  | 22.3GB/s  |
| 1 MiB   | +3%  | 22.9GB/s  |
| 2 MiB   | +15% | 20.5GB/s  |
| 4 MiB   | +27% | 18.5GB/s  |
| 8 MiB   | +27% | 18.6GB/s  |
| 16 MiB  | +32% | 17.8GB/s  |
| 32 MiB  | +22% | 19.3GB/s  |
| 64 MiB  | +22% | 19.3GB/s  |
| 128 MiB | +23% | 19.1GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +12% | 18.3GB/s  |
| 512 B   | +6%  | 19.3GB/s  |
| 1 KiB   | +2%  | 20.0GB/s  |
| 2 KiB   | +2%  | 20.2GB/s  |
| 4 KiB   | -    | 20.4GB/s  | page, 4 KiB |
| 8 KiB   | +1%  | 20.2GB/s  |
| 16 KiB  | -    | 20.4GB/s  |
| 32 KiB  | -    | 20.5GB/s  |
| 64 KiB  | -    | 20.5GB/s  |
| 128 KiB | +1%  | 20.3GB/s  |
| 256 KiB | +2%  | 20.2GB/s  |
| 512 KiB | +3%  | 20.0GB/s  |
| 1 MiB   | +2%  | 20.1GB/s  |
| 2 MiB   | +11% | 18.5GB/s  |
| 4 MiB   | +17% | 17.5GB/s  |
| 8 MiB   | +44% | 14.2GB/s  |
| 16 MiB  | +14% | 18.0GB/s  |
| 32 MiB  | +14% | 17.9GB/s  |
| 64 MiB  | +13% | 18.1GB/s  |
| 128 MiB | +16% | 17.7GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +13% | 19.8GB/s  |
| 512 B   | +6%  | 21.1GB/s  |
| 1 KiB   | +3%  | 21.7GB/s  |
| 2 KiB   | +1%  | 22.0GB/s  |
| 4 KiB   | -    | 22.3GB/s  | page, 4 KiB |
| 8 KiB   | +2%  | 21.8GB/s  |
| 16 KiB  | -    | 22.2GB/s  |
| 32 KiB  | -    | 22.3GB/s  |
| 64 KiB  | -    | 22.3GB/s  |
| 128 KiB | +15% | 19.4GB/s  |
| 256 KiB | +50% | 14.9GB/s  |
| 512 KiB | +50% | 14.8GB/s  |
| 1 MiB   | +51% | 14.8GB/s  |
| 2 MiB   | +50% | 14.9GB/s  |
| 4 MiB   | +67% | 13.4GB/s  |
| 8 MiB   | +63% | 13.7GB/s  |
| 16 MiB  | +63% | 13.7GB/s  |
| 32 MiB  | +63% | 13.7GB/s  |
| 64 MiB  | +72% | 13.0GB/s  |
| 128 MiB | +79% | 12.5GB/s  |

</details>
<details><summary><b>memset, multithreading (4 threads, 4 cores)</b></summary>

**memset**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +52%  | 46.6GB/s  | 11.6GB/s  |
| 512 B   | +20%  | 59.1GB/s  | 14.8GB/s  |
| 1 KiB   | +6%   | 66.8GB/s  | 16.7GB/s  |
| 2 KiB   | +4%   | 68.0GB/s  | 17.0GB/s  |
| 4 KiB   | -     | 70.7GB/s  | 17.7GB/s  | page, 4 KiB |
| 8 KiB   | +3%   | 68.7GB/s  | 17.2GB/s  |
| 16 KiB  | +4%   | 68.1GB/s  | 17.0GB/s  |
| 32 KiB  | +7%   | 66.4GB/s  | 16.6GB/s  |
| 64 KiB  | +14%  | 61.8GB/s  | 15.5GB/s  |
| 128 KiB | +26%  | 56.3GB/s  | 14.1GB/s  |
| 256 KiB | +71%  | 41.4GB/s  | 10.3GB/s  |
| 512 KiB | +129% | 30.9GB/s  | 7.73GB/s  |
| 1 MiB   | +210% | 22.8GB/s  | 5.70GB/s  |
| 2 MiB   | +244% | 20.5GB/s  | 5.13GB/s  |
| 4 MiB   | +276% | 18.8GB/s  | 4.71GB/s  |
| 8 MiB   | +290% | 18.1GB/s  | 4.53GB/s  |
| 16 MiB  | +292% | 18.0GB/s  | 4.51GB/s  |
| 32 MiB  | +291% | 18.1GB/s  | 4.52GB/s  |
| 64 MiB  | +288% | 18.2GB/s  | 4.56GB/s  |
| 128 MiB | +299% | 17.7GB/s  | 4.44GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +28%  | 51.9GB/s  | 13.0GB/s  |
| 512 B   | +8%   | 61.8GB/s  | 15.4GB/s  |
| 1 KiB   | +5%   | 63.5GB/s  | 15.9GB/s  |
| 2 KiB   | +3%   | 64.2GB/s  | 16.1GB/s  |
| 4 KiB   | -     | 66.4GB/s  | 16.6GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 65.8GB/s  | 16.5GB/s  |
| 16 KiB  | +2%   | 64.8GB/s  | 16.2GB/s  |
| 32 KiB  | +15%  | 57.7GB/s  | 14.4GB/s  |
| 64 KiB  | +16%  | 57.4GB/s  | 14.4GB/s  |
| 128 KiB | +27%  | 52.4GB/s  | 13.1GB/s  |
| 256 KiB | +52%  | 43.6GB/s  | 10.9GB/s  |
| 512 KiB | +113% | 31.2GB/s  | 7.80GB/s  |
| 1 MiB   | +183% | 23.5GB/s  | 5.87GB/s  |
| 2 MiB   | +227% | 20.3GB/s  | 5.07GB/s  |
| 4 MiB   | +251% | 18.9GB/s  | 4.73GB/s  |
| 8 MiB   | +265% | 18.2GB/s  | 4.55GB/s  |
| 16 MiB  | +259% | 18.5GB/s  | 4.62GB/s  |
| 32 MiB  | +274% | 17.8GB/s  | 4.44GB/s  |
| 64 MiB  | +279% | 17.5GB/s  | 4.38GB/s  |
| 128 MiB | +273% | 17.8GB/s  | 4.45GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +34%  | 61.3GB/s  | 15.3GB/s  |
| 512 B   | +18%  | 69.5GB/s  | 17.4GB/s  |
| 1 KiB   | +10%  | 74.4GB/s  | 18.6GB/s  |
| 2 KiB   | +4%   | 79.0GB/s  | 19.8GB/s  |
| 4 KiB   | +1%   | 81.0GB/s  | 20.2GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 81.1GB/s  | 20.3GB/s  |
| 16 KiB  | -     | 82.1GB/s  | 20.5GB/s  |
| 32 KiB  | +9%   | 75.3GB/s  | 18.8GB/s  |
| 64 KiB  | +34%  | 61.4GB/s  | 15.4GB/s  |
| 128 KiB | +155% | 32.3GB/s  | 8.07GB/s  |
| 256 KiB | +174% | 30.0GB/s  | 7.50GB/s  |
| 512 KiB | +194% | 27.9GB/s  | 6.98GB/s  |
| 1 MiB   | +312% | 20.0GB/s  | 4.99GB/s  |
| 2 MiB   | +337% | 18.8GB/s  | 4.69GB/s  |
| 4 MiB   | +17%  | 17.6GB/s  | 4.40GB/s  |
| 8 MiB   | +16%  | 17.7GB/s  | 4.42GB/s  |
| 16 MiB  | +16%  | 17.7GB/s  | 4.43GB/s  |
| 32 MiB  | +16%  | 17.7GB/s  | 4.41GB/s  |
| 64 MiB  | +16%  | 17.7GB/s  | 4.43GB/s  |
| 128 MiB | +18%  | 17.4GB/s  | 4.36GB/s  |

</details>
<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +4%  | 7.60GB/s  |
| 512 B   | +3%  | 7.65GB/s  |
| 1 KiB   | +1%  | 7.81GB/s  |
| 2 KiB   | -    | 7.88GB/s  |
| 4 KiB   | -    | 7.90GB/s  | page, 4 KiB |
| 8 KiB   | +2%  | 7.77GB/s  |
| 16 KiB  | +6%  | 7.48GB/s  |
| 32 KiB  | +15% | 6.85GB/s  |
| 64 KiB  | +18% | 6.72GB/s  |
| 128 KiB | +18% | 6.71GB/s  |
| 256 KiB | +19% | 6.61GB/s  |
| 512 KiB | +19% | 6.63GB/s  |
| 1 MiB   | +22% | 6.46GB/s  |
| 2 MiB   | +39% | 5.69GB/s  |
| 4 MiB   | +74% | 4.53GB/s  |
| 8 MiB   | +88% | 4.21GB/s  |
| 16 MiB  | +95% | 4.05GB/s  |
| 32 MiB  | +93% | 4.09GB/s  |
| 64 MiB  | +92% | 4.12GB/s  |
| 128 MiB | +96% | 4.02GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +5%   | 8.99GB/s  |
| 512 B   | +3%   | 9.22GB/s  |
| 1 KiB   | +2%   | 9.31GB/s  |
| 2 KiB   | -     | 9.44GB/s  |
| 4 KiB   | -     | 9.46GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 9.37GB/s  |
| 16 KiB  | -     | 9.43GB/s  |
| 32 KiB  | +1%   | 9.37GB/s  |
| 64 KiB  | +30%  | 7.28GB/s  |
| 128 KiB | +36%  | 6.95GB/s  |
| 256 KiB | +37%  | 6.90GB/s  |
| 512 KiB | +38%  | 6.88GB/s  |
| 1 MiB   | +37%  | 6.92GB/s  |
| 2 MiB   | +41%  | 6.70GB/s  |
| 4 MiB   | +66%  | 5.68GB/s  |
| 8 MiB   | +86%  | 5.10GB/s  |
| 16 MiB  | +96%  | 4.84GB/s  |
| 32 MiB  | +105% | 4.62GB/s  |
| 64 MiB  | +109% | 4.52GB/s  |
| 128 MiB | +107% | 4.58GB/s  |

</details>
<details><summary><b>other, multithreading (4 threads, 4 cores)</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|------|-----------|-----------|-------------|
| 256 B   | +4%  | 27.0GB/s  | 6.75GB/s  |
| 512 B   | +2%  | 27.6GB/s  | 6.90GB/s  |
| 1 KiB   | -    | 28.0GB/s  | 7.00GB/s  |
| 2 KiB   | -    | 28.1GB/s  | 7.02GB/s  |
| 4 KiB   | +2%  | 27.6GB/s  | 6.90GB/s  | page, 4 KiB |
| 8 KiB   | +4%  | 27.1GB/s  | 6.77GB/s  |
| 16 KiB  | +30% | 21.6GB/s  | 5.41GB/s  |
| 32 KiB  | +19% | 23.6GB/s  | 5.89GB/s  |
| 64 KiB  | +19% | 23.5GB/s  | 5.88GB/s  |
| 128 KiB | +27% | 22.1GB/s  | 5.53GB/s  |
| 256 KiB | +36% | 20.7GB/s  | 5.16GB/s  |
| 512 KiB | +33% | 21.1GB/s  | 5.29GB/s  |
| 1 MiB   | +51% | 18.6GB/s  | 4.65GB/s  |
| 2 MiB   | +62% | 17.3GB/s  | 4.34GB/s  |
| 4 MiB   | +68% | 16.7GB/s  | 4.18GB/s  |
| 8 MiB   | +67% | 16.8GB/s  | 4.19GB/s  |
| 16 MiB  | +71% | 16.5GB/s  | 4.11GB/s  |
| 32 MiB  | +71% | 16.4GB/s  | 4.11GB/s  |
| 64 MiB  | +70% | 16.5GB/s  | 4.13GB/s  |
| 128 MiB | +72% | 16.3GB/s  | 4.08GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|------|-----------|-----------|-------------|
| 256 B   | +17% | 29.3GB/s  | 7.32GB/s  |
| 512 B   | +8%  | 31.6GB/s  | 7.90GB/s  |
| 1 KiB   | +3%  | 33.2GB/s  | 8.30GB/s  |
| 2 KiB   | +1%  | 33.7GB/s  | 8.43GB/s  |
| 4 KiB   | -    | 34.1GB/s  | 8.53GB/s  | page, 4 KiB |
| 8 KiB   | -    | 34.1GB/s  | 8.52GB/s  |
| 16 KiB  | +26% | 27.2GB/s  | 6.80GB/s  |
| 32 KiB  | +35% | 25.3GB/s  | 6.33GB/s  |
| 64 KiB  | +44% | 23.7GB/s  | 5.92GB/s  |
| 128 KiB | +53% | 22.3GB/s  | 5.58GB/s  |
| 256 KiB | +54% | 22.2GB/s  | 5.55GB/s  |
| 512 KiB | +50% | 22.8GB/s  | 5.70GB/s  |
| 1 MiB   | +53% | 22.3GB/s  | 5.58GB/s  |
| 2 MiB   | +64% | 20.8GB/s  | 5.20GB/s  |
| 4 MiB   | +67% | 20.5GB/s  | 5.12GB/s  |
| 8 MiB   | +66% | 20.6GB/s  | 5.15GB/s  |
| 16 MiB  | +63% | 21.0GB/s  | 5.24GB/s  |
| 32 MiB  | +70% | 20.1GB/s  | 5.01GB/s  |
| 64 MiB  | +70% | 20.1GB/s  | 5.03GB/s  |
| 128 MiB | +68% | 20.3GB/s  | 5.08GB/s  |

</details>


# AMD Ryzen 7 8745HS

* Arch: Zen4
* Clock: 5.1 GHz
* Memory: 32GB DDR5-5600 DC, 69GB/s
* Cache:
    - L1: 8x 32KB 8-way
    - L2: 8x 1MB 8-way
    - L3: CCX with 16MB 16-way, 4 slices of 4MB per CCX
    - write to L1: 32B/cy
    - read L1: 64B/cy
    - L2 to L1: 32B/cy
* Theoretical performance:<br/>
  32B/cy write to L1 * 5.1GHz = 163.2 GB/s - max speed of memset<br/>
  64B/cy read from L1 * 5.1GHz = 326.4 GB/s - max speed of search<br/>

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|-------|------------|-------------|
| 256 B   | 27.39 ms | +51%  | -     | 98.0GB/s   |             |
| 512 B   | 18.79 ms | +4%   | +31%  | 142.8GB/s  |             |
| 1 KiB   | 19.62 ms | +8%   | +4%   | 136.8GB/s  |             |
| 2 KiB   | 18.56 ms | +3%   | +5%   | 144.6GB/s  |             |
| 4 KiB   | 18.09 ms | -     | +3%   | 148.4GB/s  | page, 4 KiB |
| 8 KiB   | 18.87 ms | +4%   | +4%   | 142.3GB/s  |             |
| 16 KiB  | 34.28 ms | +89%  | +82%  | 78.3GB/s   |             |
| 32 KiB  | 30.73 ms | +70%  | +10%  | 87.4GB/s   | L1D, 32 KiB |
| 64 KiB  | 35.40 ms | +96%  | +15%  | 75.8GB/s   |             |
| 128 KiB | 35.44 ms | +96%  | -     | 75.8GB/s   |             |
| 256 KiB | 35.78 ms | +98%  | +1%   | 75.0GB/s   |             |
| 512 KiB | 42.86 ms | +137% | +20%  | 62.6GB/s   |             |
| 1 MiB   | 43.66 ms | +141% | +2%   | 61.5GB/s   | L2, 1 MiB   |
| 2 MiB   | 43.51 ms | +141% | -     | 61.7GB/s   |             |
| 4 MiB   | 43.61 ms | +141% | -     | 61.6GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 71.57 ms | +296% | +64%  | 37.5GB/s   |             |
| 16 MiB  | 0.16 s   | +778% | +122% | 16.9GB/s   | L3, 16 MiB  |
| 32 MiB  | 0.16 s   | +782% | -     | 16.8GB/s   |             |
| 64 MiB  | 0.16 s   | +800% | +2%   | 16.5GB/s   |             |
| 128 MiB | 0.16 s   | +803% | -     | 16.4GB/s   |             |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|------|------------|-------------|
| 256 B   | 27.36 ms | +55%  | -    | 98.1GB/s   |             |
| 512 B   | 17.67 ms | -     | +35% | 151.9GB/s  |             |
| 1 KiB   | 17.74 ms | -     | -    | 151.3GB/s  |             |
| 2 KiB   | 17.68 ms | -     | -    | 151.8GB/s  |             |
| 4 KiB   | 17.71 ms | -     | -    | 151.6GB/s  | page, 4 KiB |
| 8 KiB   | 18.36 ms | +4%   | +4%  | 146.2GB/s  |             |
| 16 KiB  | 34.71 ms | +96%  | +89% | 77.3GB/s   |             |
| 32 KiB  | 30.81 ms | +74%  | +11% | 87.1GB/s   | L1D, 32 KiB |
| 64 KiB  | 35.45 ms | +101% | +15% | 75.7GB/s   |             |
| 128 KiB | 35.53 ms | +101% | -    | 75.6GB/s   |             |
| 256 KiB | 35.55 ms | +101% | -    | 75.5GB/s   |             |
| 512 KiB | 42.97 ms | +143% | +21% | 62.5GB/s   |             |
| 1 MiB   | 43.92 ms | +149% | +2%  | 61.1GB/s   | L2, 1 MiB   |
| 2 MiB   | 43.63 ms | +147% | +1%  | 61.5GB/s   |             |
| 4 MiB   | 43.84 ms | +148% | -    | 61.2GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 72.02 ms | +308% | +64% | 37.3GB/s   |             |
| 16 MiB  | 0.11 s   | +527% | +54% | 24.2GB/s   | L3, 16 MiB  |
| 32 MiB  | 0.13 s   | +664% | +22% | 19.9GB/s   |             |
| 64 MiB  | 0.15 s   | +739% | +10% | 18.1GB/s   |             |
| 128 MiB | 0.15 s   | +742% | -    | 18.0GB/s   |             |

**SIMD non-cached copy**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|--------|------|------------|-------------|
| 256 B   | 2.94 s   | +3125% | -    | 912.3MB/s  |             |
| 512 B   | 1.48 s   | +1520% | +50% | 1.82GB/s   |             |
| 1 KiB   | 0.65 s   | +615%  | +56% | 4.12GB/s   |             |
| 2 KiB   | 0.35 s   | +285%  | +46% | 7.65GB/s   |             |
| 4 KiB   | 0.24 s   | +167%  | +31% | 11.0GB/s   | page, 4 KiB |
| 8 KiB   | 0.24 s   | +166%  | -    | 11.1GB/s   |             |
| 16 KiB  | 0.22 s   | +143%  | +9%  | 12.1GB/s   |             |
| 32 KiB  | 0.17 s   | +89%   | +22% | 15.6GB/s   | L1D, 32 KiB |
| 64 KiB  | 0.13 s   | +37%   | +27% | 21.4GB/s   |             |
| 128 KiB | 0.11 s   | +18%   | +14% | 24.9GB/s   |             |
| 256 KiB | 0.10 s   | +10%   | +7%  | 26.8GB/s   |             |
| 512 KiB | 96.81 ms | +6%    | +3%  | 27.7GB/s   |             |
| 1 MiB   | 93.06 ms | +2%    | +4%  | 28.8GB/s   | L2, 1 MiB   |
| 2 MiB   | 91.31 ms | -      | +2%  | 29.4GB/s   |             |
| 4 MiB   | 91.23 ms | -      | -    | 29.4GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 95.45 ms | +5%    | +5%  | 28.1GB/s   |             |
| 16 MiB  | 0.10 s   | +10%   | +5%  | 26.7GB/s   | L3, 16 MiB  |
| 32 MiB  | 0.10 s   | +11%   | +1%  | 26.6GB/s   |             |
| 64 MiB  | 0.10 s   | +12%   | +1%  | 26.2GB/s   |             |
| 128 MiB | 0.10 s   | +12%   | -    | 26.3GB/s   |             |

</details>
<details><summary><b>memcpy, multithreading (4 threads bound to physical cores)</b></summary>

**memcpy**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|-----------|-------------|
| 256 B   | 41.31 ms | +50%   | -     | 259.9GB/s  | 65.0GB/s  |             |
| 512 B   | 30.03 ms | +9%    | +27%  | 357.5GB/s  | 89.4GB/s  |             |
| 1 KiB   | 29.99 ms | +9%    | -     | 358.1GB/s  | 89.5GB/s  |             |
| 2 KiB   | 27.59 ms | -      | +8%   | 389.2GB/s  | 97.3GB/s  |             |
| 4 KiB   | 27.64 ms | -      | -     | 388.4GB/s  | 97.1GB/s  | page, 4 KiB |
| 8 KiB   | 35.34 ms | +28%   | +28%  | 303.8GB/s  | 75.9GB/s  |             |
| 16 KiB  | 42.38 ms | +54%   | +20%  | 253.4GB/s  | 63.3GB/s  |             |
| 32 KiB  | 42.76 ms | +55%   | +1%   | 251.1GB/s  | 62.8GB/s  | L1D, 32 KiB |
| 64 KiB  | 48.26 ms | +75%   | +13%  | 222.5GB/s  | 55.6GB/s  |             |
| 128 KiB | 52.25 ms | +89%   | +8%   | 205.5GB/s  | 51.4GB/s  |             |
| 256 KiB | 49.99 ms | +81%   | +4%   | 214.8GB/s  | 53.7GB/s  |             |
| 512 KiB | 61.60 ms | +123%  | +23%  | 174.3GB/s  | 43.6GB/s  |             |
| 1 MiB   | 63.76 ms | +131%  | +4%   | 168.4GB/s  | 42.1GB/s  | L2, 1 MiB   |
| 2 MiB   | 92.07 ms | +234%  | +44%  | 116.6GB/s  | 29.2GB/s  |             |
| 4 MiB   | 0.34 s   | +1126% | +267% | 31.7GB/s   | 7.94GB/s  | L3 slice, 4 MiB |
| 8 MiB   | 0.49 s   | +1669% | +44%  | 22.0GB/s   | 5.50GB/s  |             |
| 16 MiB  | 0.37 s   | +1225% | +25%  | 29.4GB/s   | 7.34GB/s  | L3, 16 MiB  |
| 32 MiB  | 0.37 s   | +1230% | -     | 29.3GB/s   | 7.32GB/s  |             |
| 64 MiB  | 0.37 s   | +1224% | -     | 29.4GB/s   | 7.35GB/s  |             |
| 128 MiB | 0.37 s   | +1231% | +1%   | 29.2GB/s   | 7.31GB/s  |             |

**SIMD cached copy**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|-----------|-------------|
| 256 B   | 43.45 ms | +52%   | -     | 247.1GB/s  | 61.8GB/s  |             |
| 512 B   | 30.58 ms | +7%    | +30%  | 351.1GB/s  | 87.8GB/s  |             |
| 1 KiB   | 29.01 ms | +2%    | +5%   | 370.1GB/s  | 92.5GB/s  |             |
| 2 KiB   | 30.50 ms | +7%    | +5%   | 352.0GB/s  | 88.0GB/s  |             |
| 4 KiB   | 28.53 ms | -      | +6%   | 376.3GB/s  | 94.1GB/s  | page, 4 KiB |
| 8 KiB   | 36.11 ms | +27%   | +27%  | 297.4GB/s  | 74.3GB/s  |             |
| 16 KiB  | 40.39 ms | +42%   | +12%  | 265.8GB/s  | 66.5GB/s  |             |
| 32 KiB  | 44.76 ms | +57%   | +11%  | 239.9GB/s  | 60.0GB/s  | L1D, 32 KiB |
| 64 KiB  | 51.76 ms | +81%   | +16%  | 207.4GB/s  | 51.9GB/s  |             |
| 128 KiB | 55.56 ms | +95%   | +7%   | 193.3GB/s  | 48.3GB/s  |             |
| 256 KiB | 48.28 ms | +69%   | +13%  | 222.4GB/s  | 55.6GB/s  |             |
| 512 KiB | 63.88 ms | +124%  | +32%  | 168.1GB/s  | 42.0GB/s  |             |
| 1 MiB   | 69.78 ms | +145%  | +9%   | 153.9GB/s  | 38.5GB/s  | L2, 1 MiB   |
| 2 MiB   | 93.29 ms | +227%  | +34%  | 115.1GB/s  | 28.8GB/s  |             |
| 4 MiB   | 0.34 s   | +1079% | +260% | 31.9GB/s   | 7.98GB/s  | L3 slice, 4 MiB |
| 8 MiB   | 0.48 s   | +1580% | +43%  | 22.4GB/s   | 5.60GB/s  |             |
| 16 MiB  | 0.57 s   | +1890% | +18%  | 18.9GB/s   | 4.73GB/s  | L3, 16 MiB  |
| 32 MiB  | 0.57 s   | +1895% | -     | 18.9GB/s   | 4.72GB/s  |             |
| 64 MiB  | 0.57 s   | +1898% | -     | 18.8GB/s   | 4.71GB/s  |             |
| 128 MiB | 0.57 s   | +1897% | -     | 18.8GB/s   | 4.71GB/s  |             |

**SIMD non-cached copy**

| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
|---------|---------|--------|------|-----------|------------|-------------|
| 256 B   | 13.62 s | +3860% | -    | 3.15GB/s  | 788.6MB/s  |             |
| 512 B   | 6.62 s  | +1826% | +51% | 6.49GB/s  | 1.62GB/s   |             |
| 1 KiB   | 3.01 s  | +776%  | +54% | 14.3GB/s  | 3.56GB/s   |             |
| 2 KiB   | 2.35 s  | +583%  | +22% | 18.3GB/s  | 4.57GB/s   |             |
| 4 KiB   | 1.98 s  | +476%  | +16% | 21.7GB/s  | 5.42GB/s   | page, 4 KiB |
| 8 KiB   | 1.52 s  | +341%  | +23% | 28.3GB/s  | 7.08GB/s   |             |
| 16 KiB  | 1.46 s  | +325%  | +4%  | 29.4GB/s  | 7.36GB/s   |             |
| 32 KiB  | 1.30 s  | +279%  | +11% | 32.9GB/s  | 8.24GB/s   | L1D, 32 KiB |
| 64 KiB  | 1.32 s  | +284%  | +1%  | 32.5GB/s  | 8.13GB/s   |             |
| 128 KiB | 1.34 s  | +291%  | +2%  | 32.0GB/s  | 8.00GB/s   |             |
| 256 KiB | 1.33 s  | +288%  | +1%  | 32.2GB/s  | 8.05GB/s   |             |
| 512 KiB | 1.34 s  | +291%  | +1%  | 32.0GB/s  | 7.99GB/s   |             |
| 1 MiB   | 1.36 s  | +295%  | +1%  | 31.6GB/s  | 7.90GB/s   | L2, 1 MiB   |
| 2 MiB   | 1.36 s  | +295%  | -    | 31.6GB/s  | 7.90GB/s   |             |
| 4 MiB   | 0.35 s  | -      | +75% | 31.1GB/s  | 7.77GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 0.34 s  | -      | -    | 31.2GB/s  | 7.81GB/s   |             |
| 16 MiB  | 0.35 s  | +1%    | +1%  | 31.1GB/s  | 7.76GB/s   | L3, 16 MiB  |
| 32 MiB  | 0.35 s  | +2%    | +2%  | 30.5GB/s  | 7.63GB/s   |             |
| 64 MiB  | 0.35 s  | +1%    | +1%  | 30.9GB/s  | 7.72GB/s   |             |
| 128 MiB | 0.35 s  | +1%    | -    | 30.9GB/s  | 7.72GB/s   |             |

</details>
<details><summary><b>memset, single thread</b></summary>

**memset**

| block | time | diff | delta | bandwidth | comment |
|---------|----------|-------|------|------------|-------------|
| 256 B   | 22.87 ms | +29%  | -    | 117.4GB/s  |             |
| 512 B   | 18.82 ms | +6%   | +18% | 142.7GB/s  |             |
| 1 KiB   | 18.37 ms | +4%   | +2%  | 146.1GB/s  |             |
| 2 KiB   | 18.03 ms | +2%   | +2%  | 148.9GB/s  |             |
| 4 KiB   | 17.87 ms | +1%   | +1%  | 150.2GB/s  | page, 4 KiB |
| 8 KiB   | 17.75 ms | -     | +1%  | 151.3GB/s  |             |
| 16 KiB  | 18.35 ms | +3%   | +3%  | 146.3GB/s  |             |
| 32 KiB  | 18.01 ms | +1%   | +2%  | 149.0GB/s  | L1D, 32 KiB |
| 64 KiB  | 18.40 ms | +4%   | +2%  | 145.9GB/s  |             |
| 128 KiB | 18.28 ms | +3%   | +1%  | 146.9GB/s  |             |
| 256 KiB | 18.29 ms | +3%   | -    | 146.7GB/s  |             |
| 512 KiB | 18.29 ms | +3%   | -    | 146.7GB/s  |             |
| 1 MiB   | 21.86 ms | +23%  | +20% | 122.8GB/s  | L2, 1 MiB   |
| 2 MiB   | 21.19 ms | +19%  | +3%  | 126.7GB/s  |             |
| 4 MiB   | 20.93 ms | +18%  | +1%  | 128.3GB/s  | L3 slice, 4 MiB |
| 8 MiB   | 21.62 ms | +22%  | +3%  | 124.2GB/s  |             |
| 16 MiB  | 39.09 ms | +120% | +81% | 68.7GB/s   | L3, 16 MiB  |
| 32 MiB  | 64.42 ms | +263% | +65% | 41.7GB/s   |             |
| 64 MiB  | 78.87 ms | +344% | +22% | 34.0GB/s   |             |
| 128 MiB | 87.76 ms | +395% | +11% | 30.6GB/s   |             |

**SIMD cached fill**

| block   | time     | diff  | delta | bandwidth | comment |
|---------|----------|-------|------|------------|-------------|
| 256 B   | 17.75 ms | +1%   | -    | 151.2GB/s  |             |
| 512 B   | 17.79 ms | +1%   | -    | 150.9GB/s  |             |
| 1 KiB   | 17.69 ms | +1%   | +1%  | 151.8GB/s  |             |
| 2 KiB   | 17.59 ms | -     | +1%  | 152.6GB/s  |             |
| 4 KiB   | 17.64 ms | -     | -    | 152.1GB/s  | page, 4 KiB |
| 8 KiB   | 17.77 ms | +1%   | +1%  | 151.0GB/s  |             |
| 16 KiB  | 18.20 ms | +3%   | +2%  | 147.5GB/s  |             |
| 32 KiB  | 18.05 ms | +3%   | +1%  | 148.7GB/s  | L1D, 32 KiB |
| 64 KiB  | 18.24 ms | +4%   | +1%  | 147.2GB/s  |             |
| 128 KiB | 18.22 ms | +4%   | -    | 147.3GB/s  |             |
| 256 KiB | 18.18 ms | +3%   | -    | 147.6GB/s  |             |
| 512 KiB | 18.57 ms | +6%   | +2%  | 144.6GB/s  |             |
| 1 MiB   | 21.72 ms | +23%  | +17% | 123.6GB/s  | L2, 1 MiB   |
| 2 MiB   | 22.77 ms | +29%  | +5%  | 117.9GB/s  |             |
| 4 MiB   | 22.48 ms | +28%  | +1%  | 119.4GB/s  | L3 slice, 4 MiB |
| 8 MiB   | 22.98 ms | +31%  | +2%  | 116.8GB/s  |             |
| 16 MiB  | 39.70 ms | +126% | +73% | 67.6GB/s   | L3, 16 MiB  |
| 32 MiB  | 65.35 ms | +271% | +65% | 41.1GB/s   |             |
| 64 MiB  | 82.27 ms | +368% | +26% | 32.6GB/s   |             |
| 128 MiB | 93.34 ms | +431% | +13% | 28.8GB/s   |             |

**SIMD non-cached fill**

| block   | time     | diff   | delta | bandwidth | comment |
|---------|----------|--------|------|-----------|-------------|
| 256 B   | 0.79 s   | +2197% | -    | 3.40GB/s  |             |
| 512 B   | 0.45 s   | +1215% | +43% | 5.93GB/s  |             |
| 1 KiB   | 0.24 s   | +602%  | +47% | 11.1GB/s  |             |
| 2 KiB   | 0.13 s   | +288%  | +45% | 20.1GB/s  |             |
| 4 KiB   | 73.01 ms | +112%  | +45% | 36.8GB/s  | page, 4 KiB |
| 8 KiB   | 46.47 ms | +35%   | +36% | 57.8GB/s  |             |
| 16 KiB  | 42.99 ms | +25%   | +7%  | 62.4GB/s  |             |
| 32 KiB  | 39.98 ms | +16%   | +7%  | 67.1GB/s  | L1D, 32 KiB |
| 64 KiB  | 35.59 ms | +3%    | +11% | 75.4GB/s  |             |
| 128 KiB | 34.74 ms | +1%    | +2%  | 77.3GB/s  |             |
| 256 KiB | 35.67 ms | +4%    | +3%  | 75.3GB/s  |             |
| 512 KiB | 35.17 ms | +2%    | +1%  | 76.3GB/s  |             |
| 1 MiB   | 34.66 ms | +1%    | +1%  | 77.5GB/s  | L2, 1 MiB   |
| 2 MiB   | 34.50 ms | -      | -    | 77.8GB/s  |             |
| 4 MiB   | 34.47 ms | -      | -    | 77.9GB/s  | L3 slice, 4 MiB |
| 8 MiB   | 34.40 ms | -      | -    | 78.0GB/s  |             |
| 16 MiB  | 35.49 ms | +3%    | +3%  | 75.6GB/s  | L3, 16 MiB  |
| 32 MiB  | 34.58 ms | +1%    | +3%  | 77.6GB/s  |             |
| 64 MiB  | 34.55 ms | -      | -    | 77.7GB/s  |             |
| 128 MiB | 34.49 ms | -      | -    | 77.8GB/s  |             |

</details>
<details><summary><b>memset, multithreading (4 threads bound to physical cores)</b></summary>

**memset**

| block   | time     | diff   | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|-----------|-------------|
| 256 B   | 39.40 ms | +40%   | -     | 272.6GB/s  | 68.1GB/s  |             |
| 512 B   | 29.70 ms | +6%    | +25%  | 361.5GB/s  | 90.4GB/s  |             |
| 1 KiB   | 28.90 ms | +3%    | +3%   | 371.6GB/s  | 92.9GB/s  |             |
| 2 KiB   | 29.54 ms | +5%    | +2%   | 363.5GB/s  | 90.9GB/s  |             |
| 4 KiB   | 29.63 ms | +5%    | -     | 362.3GB/s  | 90.6GB/s  | page, 4 KiB |
| 8 KiB   | 28.60 ms | +2%    | +3%   | 375.4GB/s  | 93.9GB/s  |             |
| 16 KiB  | 29.01 ms | +3%    | +1%   | 370.1GB/s  | 92.5GB/s  |             |
| 32 KiB  | 30.33 ms | +8%    | +5%   | 354.0GB/s  | 88.5GB/s  | L1D, 32 KiB |
| 64 KiB  | 29.12 ms | +3%    | +4%   | 368.7GB/s  | 92.2GB/s  |             |
| 128 KiB | 28.14 ms | -      | +3%   | 381.6GB/s  | 95.4GB/s  |             |
| 256 KiB | 28.70 ms | +2%    | +2%   | 374.1GB/s  | 93.5GB/s  |             |
| 512 KiB | 34.74 ms | +23%   | +21%  | 309.1GB/s  | 77.3GB/s  |             |
| 1 MiB   | 36.79 ms | +31%   | +6%   | 291.9GB/s  | 73.0GB/s  | L2, 1 MiB   |
| 2 MiB   | 50.85 ms | +81%   | +38%  | 211.2GB/s  | 52.8GB/s  |             |
| 4 MiB   | 0.23 s   | +707%  | +347% | 47.3GB/s   | 11.8GB/s  | L3 slice, 4 MiB |
| 8 MiB   | 0.32 s   | +1029% | +40%  | 33.8GB/s   | 8.45GB/s  |             |
| 16 MiB  | 0.36 s   | +1177% | +13%  | 29.9GB/s   | 7.47GB/s  | L3, 16 MiB  |
| 32 MiB  | 0.38 s   | +1250% | +6%   | 28.3GB/s   | 7.07GB/s  |             |
| 64 MiB  | 0.38 s   | +1264% | +1%   | 28.0GB/s   | 6.99GB/s  |             |
| 128 MiB | 0.38 s   | +1266% | -     | 27.9GB/s   | 6.98GB/s  |             |

**SIMD cached fill**

| block   | time     | diff   | delta | bandwidth | bandwidth per thread | comment |
|---------|----------|--------|-------|------------|------------|-------------|
| 256 B   | 28.93 ms | +10%   | -     | 371.1GB/s  | 92.8GB/s   |             |
| 512 B   | 29.03 ms | +10%   | -     | 369.9GB/s  | 92.5GB/s   |             |
| 1 KiB   | 29.81 ms | +13%   | +3%   | 360.2GB/s  | 90.1GB/s   |             |
| 2 KiB   | 29.36 ms | +12%   | +1%   | 365.7GB/s  | 91.4GB/s   |             |
| 4 KiB   | 26.28 ms | -      | +11%  | 408.6GB/s  | 102.2GB/s  | page, 4 KiB |
| 8 KiB   | 30.68 ms | +17%   | +17%  | 349.9GB/s  | 87.5GB/s   |             |
| 16 KiB  | 30.15 ms | +15%   | +2%   | 356.1GB/s  | 89.0GB/s   |             |
| 32 KiB  | 32.19 ms | +23%   | +7%   | 333.5GB/s  | 83.4GB/s   | L1D, 32 KiB |
| 64 KiB  | 27.53 ms | +5%    | +14%  | 390.1GB/s  | 97.5GB/s   |             |
| 128 KiB | 30.06 ms | +14%   | +9%   | 357.2GB/s  | 89.3GB/s   |             |
| 256 KiB | 30.62 ms | +17%   | +2%   | 350.7GB/s  | 87.7GB/s   |             |
| 512 KiB | 29.44 ms | +12%   | +4%   | 364.8GB/s  | 91.2GB/s   |             |
| 1 MiB   | 38.48 ms | +46%   | +31%  | 279.0GB/s  | 69.8GB/s   | L2, 1 MiB   |
| 2 MiB   | 56.25 ms | +114%  | +46%  | 190.9GB/s  | 47.7GB/s   |             |
| 4 MiB   | 0.22 s   | +755%  | +299% | 47.8GB/s   | 12.0GB/s   | L3 slice, 4 MiB |
| 8 MiB   | 0.30 s   | +1045% | +34%  | 35.7GB/s   | 8.92GB/s   |             |
| 16 MiB  | 0.35 s   | +1234% | +16%  | 30.6GB/s   | 7.66GB/s   | L3, 16 MiB  |
| 32 MiB  | 0.37 s   | +1320% | +6%   | 28.8GB/s   | 7.20GB/s   |             |
| 64 MiB  | 0.38 s   | +1348% | +2%   | 28.2GB/s   | 7.05GB/s   |             |
| 128 MiB | 0.38 s   | +1350% | -     | 28.2GB/s   | 7.04GB/s   |             |

**SIMD non-cached fill**

| block   | time   | diff   | delta | bandwidth | bandwidth per thread | comment |
|---------|--------|--------|------|-----------|-----------|-------------|
| 256 B   | 3.26 s | +2062% | -    | 13.2GB/s  | 3.29GB/s  |             |
| 512 B   | 1.82 s | +1105% | +44% | 23.6GB/s  | 5.90GB/s  |             |
| 1 KiB   | 0.85 s | +461%  | +53% | 50.7GB/s  | 12.7GB/s  |             |
| 2 KiB   | 0.59 s | +291%  | +30% | 72.7GB/s  | 18.2GB/s  |             |
| 4 KiB   | 0.58 s | +282%  | +2%  | 74.5GB/s  | 18.6GB/s  | page, 4 KiB |
| 8 KiB   | 0.57 s | +275%  | +2%  | 75.9GB/s  | 19.0GB/s  |             |
| 16 KiB  | 0.57 s | +277%  | +1%  | 75.5GB/s  | 18.9GB/s  |             |
| 32 KiB  | 0.55 s | +267%  | +3%  | 77.6GB/s  | 19.4GB/s  | L1D, 32 KiB |
| 64 KiB  | 0.56 s | +272%  | +2%  | 76.4GB/s  | 19.1GB/s  |             |
| 128 KiB | 0.56 s | +270%  | +1%  | 76.9GB/s  | 19.2GB/s  |             |
| 256 KiB | 0.56 s | +270%  | -    | 76.9GB/s  | 19.2GB/s  |             |
| 512 KiB | 0.56 s | +272%  | -    | 76.5GB/s  | 19.1GB/s  |             |
| 1 MiB   | 0.56 s | +272%  | -    | 76.5GB/s  | 19.1GB/s  | L2, 1 MiB   |
| 2 MiB   | 0.56 s | +271%  | -    | 76.7GB/s  | 19.2GB/s  |             |
| 4 MiB   | 0.16 s | +9%    | +71% | 65.5GB/s  | 16.4GB/s  | L3 slice, 4 MiB |
| 8 MiB   | 0.15 s | -      | +8%  | 71.1GB/s  | 17.8GB/s  |             |
| 16 MiB  | 0.17 s | +11%   | +11% | 64.1GB/s  | 16.0GB/s  | L3, 16 MiB  |
| 32 MiB  | 0.16 s | +4%    | +6%  | 68.3GB/s  | 17.1GB/s  |             |
| 64 MiB  | 0.15 s | +2%    | +2%  | 69.9GB/s  | 17.5GB/s  |             |
| 128 MiB | 0.16 s | +3%    | +1%  | 69.1GB/s  | 17.3GB/s  |             |

</details>


# Pico 4 Ultra

* CPU: Snapdragon XR2 Gen 2
* Memory: LPDDR5X quad-channel, 64bit, 63 GB/s (?)
* Arch: ARM Cortex A78C

## Performance core

* Clock: 2.36 GHz
* Cores: 4
* L1 Cache: ?

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +16%  | 46.1GB/s  |
| 512 B   | +5%   | 50.9GB/s  |
| 1 KiB   | +2%   | 52.4GB/s  |
| 2 KiB   | +1%   | 52.9GB/s  |
| 4 KiB   | -     | 53.3GB/s  | page, 4 KiB |
| 8 KiB   | -     | 53.3GB/s  |
| 16 KiB  | +9%   | 48.9GB/s  |
| 32 KiB  | +52%  | 35.0GB/s  |
| 64 KiB  | +32%  | 40.3GB/s  |
| 128 KiB | +31%  | 40.7GB/s  |
| 256 KiB | +48%  | 36.0GB/s  |
| 512 KiB | +84%  | 29.1GB/s  |
| 1 MiB   | +138% | 22.5GB/s  |
| 2 MiB   | +164% | 20.2GB/s  |
| 4 MiB   | +181% | 19.0GB/s  |
| 8 MiB   | +215% | 16.9GB/s  |
| 16 MiB  | +237% | 15.8GB/s  |
| 32 MiB  | +247% | 15.4GB/s  |
| 64 MiB  | +254% | 15.1GB/s  |
| 128 MiB | +260% | 14.8GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +2%   | 52.5GB/s  |
| 512 B   | -     | 53.6GB/s  |
| 1 KiB   | -     | 53.6GB/s  |
| 2 KiB   | -     | 53.6GB/s  |
| 4 KiB   | -     | 53.5GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 53.2GB/s  |
| 16 KiB  | +16%  | 46.1GB/s  |
| 32 KiB  | +51%  | 35.4GB/s  |
| 64 KiB  | +33%  | 40.2GB/s  |
| 128 KiB | +32%  | 40.6GB/s  |
| 256 KiB | +49%  | 35.9GB/s  |
| 512 KiB | +85%  | 29.0GB/s  |
| 1 MiB   | +140% | 22.4GB/s  |
| 2 MiB   | +166% | 20.1GB/s  |
| 4 MiB   | +184% | 18.9GB/s  |
| 8 MiB   | +209% | 17.3GB/s  |
| 16 MiB  | +238% | 15.9GB/s  |
| 32 MiB  | +247% | 15.4GB/s  |
| 64 MiB  | +252% | 15.2GB/s  |
| 128 MiB | +253% | 15.2GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | -     | 37.6GB/s  |
| 512 B   | -     | 37.6GB/s  |
| 1 KiB   | -     | 37.6GB/s  |
| 2 KiB   | -     | 37.6GB/s  |
| 4 KiB   | -     | 37.6GB/s  | page, 4 KiB |
| 8 KiB   | -     | 37.6GB/s  |
| 16 KiB  | +1%   | 37.1GB/s  |
| 32 KiB  | +9%   | 34.5GB/s  |
| 64 KiB  | +1%   | 37.1GB/s  |
| 128 KiB | +1%   | 37.3GB/s  |
| 256 KiB | +9%   | 34.5GB/s  |
| 512 KiB | +30%  | 28.9GB/s  |
| 1 MiB   | +67%  | 22.4GB/s  |
| 2 MiB   | +86%  | 20.2GB/s  |
| 4 MiB   | +98%  | 19.0GB/s  |
| 8 MiB   | +116% | 17.4GB/s  |
| 16 MiB  | +136% | 15.9GB/s  |
| 32 MiB  | +145% | 15.3GB/s  |
| 64 MiB  | +149% | 15.1GB/s  |
| 128 MiB | +150% | 15.1GB/s  |

</details>

<details><summary><b>memcpy, multithreading (4 threads, 4 cores)</b></summary>

**memcpy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +17%  | 45.5GB/s  | 11.4GB/s  |
| 512 B   | +6%   | 50.0GB/s  | 12.5GB/s  |
| 1 KiB   | +3%   | 51.6GB/s  | 12.9GB/s  |
| 2 KiB   | +1%   | 52.6GB/s  | 13.2GB/s  |
| 4 KiB   | -     | 53.1GB/s  | 13.3GB/s  | page, 4 KiB |
| 8 KiB   | -     | 53.1GB/s  | 13.3GB/s  |
| 16 KiB  | +10%  | 48.4GB/s  | 12.1GB/s  |
| 32 KiB  | +31%  | 40.5GB/s  | 10.1GB/s  |
| 64 KiB  | +31%  | 40.6GB/s  | 10.1GB/s  |
| 128 KiB | +36%  | 39.1GB/s  | 9.79GB/s  |
| 256 KiB | +83%  | 29.0GB/s  | 7.26GB/s  |
| 512 KiB | +137% | 22.4GB/s  | 5.61GB/s  |
| 1 MiB   | +160% | 20.4GB/s  | 5.11GB/s  |
| 2 MiB   | +181% | 18.9GB/s  | 4.73GB/s  |
| 4 MiB   | +202% | 17.6GB/s  | 4.39GB/s  |
| 8 MiB   | +224% | 16.4GB/s  | 4.09GB/s  |
| 16 MiB  | +238% | 15.7GB/s  | 3.93GB/s  |
| 32 MiB  | +246% | 15.3GB/s  | 3.83GB/s  |
| 64 MiB  | +252% | 15.1GB/s  | 3.77GB/s  |
| 128 MiB | +256% | 14.9GB/s  | 3.73GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +4%   | 50.9GB/s  | 12.7GB/s  |
| 512 B   | +3%   | 51.2GB/s  | 12.8GB/s  |
| 1 KiB   | +1%   | 52.5GB/s  | 13.1GB/s  |
| 2 KiB   | +1%   | 52.5GB/s  | 13.1GB/s  |
| 4 KiB   | -     | 52.8GB/s  | 13.2GB/s  | page, 4 KiB |
| 8 KiB   | -     | 52.6GB/s  | 13.2GB/s  |
| 16 KiB  | +11%  | 47.7GB/s  | 11.9GB/s  |
| 32 KiB  | +33%  | 39.8GB/s  | 9.96GB/s  |
| 64 KiB  | +33%  | 39.7GB/s  | 9.92GB/s  |
| 128 KiB | +39%  | 38.0GB/s  | 9.50GB/s  |
| 256 KiB | +85%  | 28.5GB/s  | 7.13GB/s  |
| 512 KiB | +136% | 22.4GB/s  | 5.59GB/s  |
| 1 MiB   | +160% | 20.3GB/s  | 5.08GB/s  |
| 2 MiB   | +182% | 18.8GB/s  | 4.69GB/s  |
| 4 MiB   | +201% | 17.6GB/s  | 4.39GB/s  |
| 8 MiB   | +223% | 16.4GB/s  | 4.09GB/s  |
| 16 MiB  | +236% | 15.7GB/s  | 3.93GB/s  |
| 32 MiB  | +243% | 15.4GB/s  | 3.85GB/s  |
| 64 MiB  | +249% | 15.1GB/s  | 3.79GB/s  |
| 128 MiB | +249% | 15.1GB/s  | 3.79GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +75%  | 39.9GB/s  | 9.98GB/s  |
| 512 B   | +75%  | 40.0GB/s  | 10.0GB/s  |
| 1 KiB   | +76%  | 39.9GB/s  | 9.98GB/s  |
| 2 KiB   | +76%  | 39.8GB/s  | 9.95GB/s  |
| 4 KiB   | +76%  | 39.9GB/s  | 9.96GB/s  | page, 4 KiB |
| 8 KiB   | +76%  | 39.8GB/s  | 9.96GB/s  |
| 16 KiB  | +77%  | 39.6GB/s  | 9.90GB/s  |
| 32 KiB  | +79%  | 39.1GB/s  | 9.77GB/s  |
| 64 KiB  | +79%  | 39.2GB/s  | 9.81GB/s  |
| 128 KiB | +85%  | 37.9GB/s  | 9.48GB/s  |
| 256 KiB | +135% | 29.8GB/s  | 7.46GB/s  |
| 512 KiB | +204% | 23.1GB/s  | 5.77GB/s  |
| 1 MiB   | +235% | 20.9GB/s  | 5.22GB/s  |
| 2 MiB   | +263% | 19.3GB/s  | 4.83GB/s  |
| 4 MiB   | -     | 17.5GB/s  | 4.38GB/s  |
| 8 MiB   | +7%   | 16.3GB/s  | 4.08GB/s  |
| 16 MiB  | +12%  | 15.7GB/s  | 3.93GB/s  |
| 32 MiB  | +14%  | 15.3GB/s  | 3.84GB/s  |
| 64 MiB  | +16%  | 15.1GB/s  | 3.78GB/s  |
| 128 MiB | +16%  | 15.1GB/s  | 3.77GB/s  |

</details>

<details><summary><b>memset, single thread</b></summary>

**memset**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +27%  | 60.8GB/s  |
| 512 B   | +8%   | 71.6GB/s  |
| 1 KiB   | +3%   | 74.9GB/s  |
| 2 KiB   | +1%   | 76.1GB/s  |
| 4 KiB   | +1%   | 76.7GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 76.7GB/s  |
| 16 KiB  | +1%   | 76.6GB/s  |
| 32 KiB  | +1%   | 76.3GB/s  |
| 64 KiB  | +1%   | 76.3GB/s  |
| 128 KiB | +1%   | 76.6GB/s  |
| 256 KiB | -     | 77.2GB/s  |
| 512 KiB | +17%  | 65.9GB/s  |
| 1 MiB   | +59%  | 48.5GB/s  |
| 2 MiB   | +89%  | 40.8GB/s  |
| 4 MiB   | +104% | 37.8GB/s  |
| 8 MiB   | +116% | 35.7GB/s  |
| 16 MiB  | +116% | 35.7GB/s  |
| 32 MiB  | +108% | 37.2GB/s  |
| 64 MiB  | +111% | 36.7GB/s  |
| 128 MiB | +114% | 36.1GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +17%  | 64.4GB/s  |
| 512 B   | +2%   | 73.4GB/s  |
| 1 KiB   | -     | 74.7GB/s  |
| 2 KiB   | -     | 75.0GB/s  |
| 4 KiB   | -     | 74.8GB/s  | page, 4 KiB |
| 8 KiB   | -     | 75.0GB/s  |
| 16 KiB  | -     | 74.8GB/s  |
| 32 KiB  | +1%   | 74.1GB/s  |
| 64 KiB  | +1%   | 73.9GB/s  |
| 128 KiB | +2%   | 73.8GB/s  |
| 256 KiB | +1%   | 74.2GB/s  |
| 512 KiB | +13%  | 66.5GB/s  |
| 1 MiB   | +58%  | 47.4GB/s  |
| 2 MiB   | +87%  | 40.1GB/s  |
| 4 MiB   | +99%  | 37.6GB/s  |
| 8 MiB   | +115% | 34.9GB/s  |
| 16 MiB  | +119% | 34.2GB/s  |
| 32 MiB  | +114% | 35.0GB/s  |
| 64 MiB  | +101% | 37.2GB/s  |
| 128 MiB | +102% | 37.1GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +24%  | 60.8GB/s  |
| 512 B   | -     | 75.0GB/s  |
| 1 KiB   | -     | 74.9GB/s  |
| 2 KiB   | -     | 74.9GB/s  |
| 4 KiB   | -     | 75.1GB/s  | page, 4 KiB |
| 8 KiB   | -     | 74.9GB/s  |
| 16 KiB  | -     | 75.0GB/s  |
| 32 KiB  | +2%   | 73.5GB/s  |
| 64 KiB  | +2%   | 73.9GB/s  |
| 128 KiB | +2%   | 73.9GB/s  |
| 256 KiB | +4%   | 72.1GB/s  |
| 512 KiB | +14%  | 65.6GB/s  |
| 1 MiB   | +57%  | 47.9GB/s  |
| 2 MiB   | +91%  | 39.4GB/s  |
| 4 MiB   | +108% | 36.0GB/s  |
| 8 MiB   | +117% | 34.7GB/s  |
| 16 MiB  | +122% | 33.9GB/s  |
| 32 MiB  | +121% | 33.9GB/s  |
| 64 MiB  | +102% | 37.2GB/s  |
| 128 MiB | +102% | 37.2GB/s  |

</details>

<details><summary><b>memset, multithreading (4 threads, 4 cores)</b></summary>

**memset**

| block   | diff | bandwidth | bandwidth per thread | comment |
|---------|------|-----------|-----------|-------------|
| 256 B   | +7%  | 61.3GB/s  | 15.3GB/s  |
| 512 B   | +3%  | 64.0GB/s  | 16.0GB/s  |
| 1 KiB   | -    | 65.8GB/s  | 16.4GB/s  |
| 2 KiB   | +5%  | 62.6GB/s  | 15.7GB/s  |
| 4 KiB   | +7%  | 61.5GB/s  | 15.4GB/s  | page, 4 KiB |
| 8 KiB   | -    | 65.6GB/s  | 16.4GB/s  |
| 16 KiB  | +8%  | 60.7GB/s  | 15.2GB/s  |
| 32 KiB  | +6%  | 62.0GB/s  | 15.5GB/s  |
| 64 KiB  | +8%  | 60.6GB/s  | 15.2GB/s  |
| 128 KiB | +2%  | 64.7GB/s  | 16.2GB/s  |
| 256 KiB | +41% | 46.6GB/s  | 11.6GB/s  |
| 512 KiB | +62% | 40.6GB/s  | 10.1GB/s  |
| 1 MiB   | +73% | 37.9GB/s  | 9.48GB/s  |
| 2 MiB   | +79% | 36.8GB/s  | 9.20GB/s  |
| 4 MiB   | +87% | 35.2GB/s  | 8.79GB/s  |
| 8 MiB   | +89% | 34.8GB/s  | 8.70GB/s  |
| 16 MiB  | +90% | 34.6GB/s  | 8.64GB/s  |
| 32 MiB  | +91% | 34.4GB/s  | 8.61GB/s  |
| 64 MiB  | +93% | 34.1GB/s  | 8.52GB/s  |
| 128 MiB | +95% | 33.8GB/s  | 8.45GB/s  |

**SIMD cached fill**

| block   | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +2%   | 67.2GB/s  | 16.8GB/s  |
| 512 B   | +11%  | 61.5GB/s  | 15.4GB/s  |
| 1 KiB   | -     | 68.5GB/s  | 17.1GB/s  |
| 2 KiB   | +5%   | 65.0GB/s  | 16.3GB/s  |
| 4 KiB   | +7%   | 64.2GB/s  | 16.1GB/s  | page, 4 KiB |
| 8 KiB   | +6%   | 64.5GB/s  | 16.1GB/s  |
| 16 KiB  | +4%   | 65.8GB/s  | 16.4GB/s  |
| 32 KiB  | +4%   | 65.8GB/s  | 16.4GB/s  |
| 64 KiB  | +1%   | 68.1GB/s  | 17.0GB/s  |
| 128 KiB | +10%  | 62.4GB/s  | 15.6GB/s  |
| 256 KiB | +48%  | 46.4GB/s  | 11.6GB/s  |
| 512 KiB | +74%  | 39.2GB/s  | 9.81GB/s  |
| 1 MiB   | +88%  | 36.5GB/s  | 9.11GB/s  |
| 2 MiB   | +97%  | 34.8GB/s  | 8.70GB/s  |
| 4 MiB   | +101% | 34.2GB/s  | 8.54GB/s  |
| 8 MiB   | +99%  | 34.4GB/s  | 8.60GB/s  |
| 16 MiB  | +101% | 34.0GB/s  | 8.51GB/s  |
| 32 MiB  | +99%  | 34.4GB/s  | 8.60GB/s  |
| 64 MiB  | +102% | 33.9GB/s  | 8.47GB/s  |
| 128 MiB | +102% | 34.0GB/s  | 8.49GB/s  |

**SIMD non-cached fill**

| block   | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +74%  | 78.7GB/s  | 19.7GB/s  |
| 512 B   | +73%  | 78.8GB/s  | 19.7GB/s  |
| 1 KiB   | +74%  | 78.7GB/s  | 19.7GB/s  |
| 2 KiB   | +73%  | 78.8GB/s  | 19.7GB/s  |
| 4 KiB   | +73%  | 78.7GB/s  | 19.7GB/s  | page, 4 KiB |
| 8 KiB   | +74%  | 78.5GB/s  | 19.6GB/s  |
| 16 KiB  | +76%  | 77.7GB/s  | 19.4GB/s  |
| 32 KiB  | +77%  | 77.4GB/s  | 19.3GB/s  |
| 64 KiB  | +80%  | 76.1GB/s  | 19.0GB/s  |
| 128 KiB | +92%  | 71.2GB/s  | 17.8GB/s  |
| 256 KiB | +182% | 48.4GB/s  | 12.1GB/s  |
| 512 KiB | +236% | 40.6GB/s  | 10.2GB/s  |
| 1 MiB   | +266% | 37.3GB/s  | 9.33GB/s  |
| 2 MiB   | +285% | 35.5GB/s  | 8.87GB/s  |
| 4 MiB   | +2%   | 33.5GB/s  | 8.38GB/s  |
| 8 MiB   | +1%   | 33.9GB/s  | 8.48GB/s  |
| 16 MiB  | -     | 34.1GB/s  | 8.52GB/s  |
| 32 MiB  | -     | 34.1GB/s  | 8.52GB/s  |
| 64 MiB  | -     | 34.1GB/s  | 8.54GB/s  |
| 128 MiB | +1%   | 33.9GB/s  | 8.48GB/s  |

</details>

<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +38%  | 47.9GB/s  |
| 512 B   | +3%   | 64.1GB/s  |
| 1 KiB   | +1%   | 65.7GB/s  |
| 2 KiB   | -     | 65.9GB/s  |
| 4 KiB   | -     | 66.0GB/s  | page, 4 KiB |
| 8 KiB   | -     | 66.1GB/s  |
| 16 KiB  | +20%  | 54.9GB/s  |
| 32 KiB  | +53%  | 43.3GB/s  |
| 64 KiB  | +42%  | 46.7GB/s  |
| 128 KiB | +40%  | 47.4GB/s  |
| 256 KiB | +79%  | 36.9GB/s  |
| 512 KiB | +103% | 32.6GB/s  |
| 1 MiB   | +132% | 28.5GB/s  |
| 2 MiB   | +154% | 26.0GB/s  |
| 4 MiB   | +182% | 23.4GB/s  |
| 8 MiB   | +241% | 19.4GB/s  |
| 16 MiB  | +252% | 18.7GB/s  |
| 32 MiB  | +255% | 18.6GB/s  |
| 64 MiB  | +258% | 18.4GB/s  |
| 128 MiB | +264% | 18.2GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff  | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +25% | 20.0GB/s  |
| 512 B   | +13% | 22.3GB/s  |
| 1 KiB   | +6%  | 23.6GB/s  |
| 2 KiB   | +3%  | 24.3GB/s  |
| 4 KiB   | +2%  | 24.7GB/s  | page, 4 KiB |
| 8 KiB   | +1%  | 24.9GB/s  |
| 16 KiB  | +1%  | 24.9GB/s  |
| 32 KiB  | -    | 25.1GB/s  |
| 64 KiB  | -    | 25.1GB/s  |
| 128 KiB | -    | 25.1GB/s  |
| 256 KiB | -    | 25.1GB/s  |
| 512 KiB | -    | 25.0GB/s  |
| 1 MiB   | -    | 25.1GB/s  |
| 2 MiB   | -    | 25.0GB/s  |
| 4 MiB   | +2%  | 24.7GB/s  |
| 8 MiB   | +6%  | 23.7GB/s  |
| 16 MiB  | +9%  | 22.9GB/s  |
| 32 MiB  | +7%  | 23.4GB/s  |
| 64 MiB  | +5%  | 23.9GB/s  |
| 128 MiB | +5%  | 23.9GB/s  |

</details>

<details><summary><b>other, multithreading (4 threads, 4 cores)</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +32%  | 47.9GB/s  | 12.0GB/s  |
| 512 B   | +8%   | 58.6GB/s  | 14.6GB/s  |
| 1 KiB   | +6%   | 59.4GB/s  | 14.8GB/s  |
| 2 KiB   | +2%   | 61.6GB/s  | 15.4GB/s  |
| 4 KiB   | +2%   | 61.6GB/s  | 15.4GB/s  | page, 4 KiB |
| 8 KiB   | -     | 63.1GB/s  | 15.8GB/s  |
| 16 KiB  | +11%  | 56.8GB/s  | 14.2GB/s  |
| 32 KiB  | +40%  | 45.0GB/s  | 11.3GB/s  |
| 64 KiB  | +36%  | 46.3GB/s  | 11.6GB/s  |
| 128 KiB | +80%  | 35.1GB/s  | 8.77GB/s  |
| 256 KiB | +96%  | 32.1GB/s  | 8.03GB/s  |
| 512 KiB | +122% | 28.4GB/s  | 7.09GB/s  |
| 1 MiB   | +144% | 25.9GB/s  | 6.47GB/s  |
| 2 MiB   | +179% | 22.6GB/s  | 5.66GB/s  |
| 4 MiB   | +199% | 21.1GB/s  | 5.28GB/s  |
| 8 MiB   | +221% | 19.7GB/s  | 4.91GB/s  |
| 16 MiB  | +234% | 18.9GB/s  | 4.72GB/s  |
| 32 MiB  | +239% | 18.6GB/s  | 4.65GB/s  |
| 64 MiB  | +244% | 18.3GB/s  | 4.59GB/s  |
| 128 MiB | +246% | 18.2GB/s  | 4.56GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff | bandwidth | bandwidth per thread | comment |
|---------|------|-----------|-----------|-------------|
| 256 B   | +8%  | 40.6GB/s  | 10.1GB/s  |
| 512 B   | -    | 43.6GB/s  | 10.9GB/s  |
| 1 KiB   | +15% | 38.0GB/s  | 9.49GB/s  |
| 2 KiB   | +28% | 34.2GB/s  | 8.54GB/s  |
| 4 KiB   | +49% | 29.3GB/s  | 7.33GB/s  | page, 4 KiB |
| 8 KiB   | +58% | 27.6GB/s  | 6.91GB/s  |
| 16 KiB  | +64% | 26.7GB/s  | 6.67GB/s  |
| 32 KiB  | +67% | 26.2GB/s  | 6.55GB/s  |
| 64 KiB  | +67% | 26.1GB/s  | 6.52GB/s  |
| 128 KiB | +68% | 25.9GB/s  | 6.48GB/s  |
| 256 KiB | +69% | 25.9GB/s  | 6.48GB/s  |
| 512 KiB | +73% | 25.3GB/s  | 6.32GB/s  |
| 1 MiB   | +74% | 25.1GB/s  | 6.26GB/s  |
| 2 MiB   | +87% | 23.3GB/s  | 5.82GB/s  |
| 4 MiB   | +96% | 22.2GB/s  | 5.56GB/s  |
| 8 MiB   | +92% | 22.8GB/s  | 5.69GB/s  |
| 16 MiB  | +91% | 22.8GB/s  | 5.70GB/s  |
| 32 MiB  | +91% | 22.9GB/s  | 5.71GB/s  |
| 64 MiB  | +91% | 22.9GB/s  | 5.71GB/s  |
| 128 MiB | +92% | 22.7GB/s  | 5.68GB/s  |

</details>


## EnergyEfficient core

* Clock: 2.0 GHz
* Cores: 2
* L1 Cache: ?

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +16%  | 37.5GB/s  |
| 512 B   | +5%   | 41.4GB/s  |
| 1 KiB   | +2%   | 42.6GB/s  |
| 2 KiB   | +1%   | 43.0GB/s  |
| 4 KiB   | -     | 43.3GB/s  | page, 4 KiB |
| 8 KiB   | -     | 43.4GB/s  |
| 16 KiB  | +16%  | 37.4GB/s  |
| 32 KiB  | +51%  | 28.8GB/s  |
| 64 KiB  | +32%  | 32.7GB/s  |
| 128 KiB | +50%  | 28.9GB/s  |
| 256 KiB | +90%  | 22.8GB/s  |
| 512 KiB | +144% | 17.7GB/s  |
| 1 MiB   | +169% | 16.1GB/s  |
| 2 MiB   | +178% | 15.6GB/s  |
| 4 MiB   | +183% | 15.3GB/s  |
| 8 MiB   | +187% | 15.1GB/s  |
| 16 MiB  | +206% | 14.2GB/s  |
| 32 MiB  | +216% | 13.7GB/s  |
| 64 MiB  | +223% | 13.4GB/s  |
| 128 MiB | +224% | 13.4GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +1%   | 43.2GB/s  |
| 512 B   | -     | 43.5GB/s  |
| 1 KiB   | -     | 43.6GB/s  |
| 2 KiB   | -     | 43.5GB/s  |
| 4 KiB   | -     | 43.5GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 43.3GB/s  |
| 16 KiB  | +16%  | 37.4GB/s  |
| 32 KiB  | +52%  | 28.7GB/s  |
| 64 KiB  | +34%  | 32.6GB/s  |
| 128 KiB | +50%  | 29.0GB/s  |
| 256 KiB | +91%  | 22.8GB/s  |
| 512 KiB | +145% | 17.8GB/s  |
| 1 MiB   | +170% | 16.1GB/s  |
| 2 MiB   | +179% | 15.6GB/s  |
| 4 MiB   | +185% | 15.3GB/s  |
| 8 MiB   | +189% | 15.1GB/s  |
| 16 MiB  | +206% | 14.2GB/s  |
| 32 MiB  | +216% | 13.8GB/s  |
| 64 MiB  | +223% | 13.5GB/s  |
| 128 MiB | +225% | 13.4GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | -     | 30.5GB/s  |
| 512 B   | -     | 30.5GB/s  |
| 1 KiB   | -     | 30.5GB/s  |
| 2 KiB   | -     | 30.5GB/s  |
| 4 KiB   | -     | 30.5GB/s  | page, 4 KiB |
| 8 KiB   | -     | 30.5GB/s  |
| 16 KiB  | +1%   | 30.1GB/s  |
| 32 KiB  | +9%   | 28.0GB/s  |
| 64 KiB  | +1%   | 30.1GB/s  |
| 128 KiB | +8%   | 28.3GB/s  |
| 256 KiB | +34%  | 22.8GB/s  |
| 512 KiB | +72%  | 17.8GB/s  |
| 1 MiB   | +90%  | 16.1GB/s  |
| 2 MiB   | +95%  | 15.6GB/s  |
| 4 MiB   | +99%  | 15.3GB/s  |
| 8 MiB   | +105% | 14.9GB/s  |
| 16 MiB  | +115% | 14.2GB/s  |
| 32 MiB  | +123% | 13.7GB/s  |
| 64 MiB  | +128% | 13.4GB/s  |
| 128 MiB | +129% | 13.3GB/s  |

</details>

<details><summary><b>memcpy, multithreading (2 threads, 2 cores)</b></summary>

**memcpy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +19%  | 52.6GB/s  | 26.3GB/s  |
| 512 B   | -     | 62.8GB/s  | 31.4GB/s  |
| 1 KiB   | +2%   | 61.6GB/s  | 30.8GB/s  |
| 2 KiB   | +6%   | 59.0GB/s  | 29.5GB/s  |
| 4 KiB   | +5%   | 59.6GB/s  | 29.8GB/s  | page, 4 KiB |
| 8 KiB   | +5%   | 59.8GB/s  | 29.9GB/s  |
| 16 KiB  | +8%   | 58.2GB/s  | 29.1GB/s  |
| 32 KiB  | +50%  | 41.7GB/s  | 20.9GB/s  |
| 64 KiB  | +54%  | 40.8GB/s  | 20.4GB/s  |
| 128 KiB | +56%  | 40.1GB/s  | 20.1GB/s  |
| 256 KiB | +96%  | 32.0GB/s  | 16.0GB/s  |
| 512 KiB | +142% | 25.9GB/s  | 12.9GB/s  |
| 1 MiB   | +153% | 24.8GB/s  | 12.4GB/s  |
| 2 MiB   | +202% | 20.8GB/s  | 10.4GB/s  |
| 4 MiB   | +237% | 18.6GB/s  | 9.30GB/s  |
| 8 MiB   | +276% | 16.7GB/s  | 8.35GB/s  |
| 16 MiB  | +297% | 15.8GB/s  | 7.90GB/s  |
| 32 MiB  | +310% | 15.3GB/s  | 7.65GB/s  |
| 64 MiB  | +317% | 15.0GB/s  | 7.52GB/s  |
| 128 MiB | +319% | 15.0GB/s  | 7.49GB/s  |

**SIMD cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +1%   | 61.8GB/s  | 30.9GB/s  |
| 512 B   | +10%  | 56.9GB/s  | 28.4GB/s  |
| 1 KiB   | +6%   | 58.9GB/s  | 29.4GB/s  |
| 2 KiB   | -     | 62.3GB/s  | 31.2GB/s  |
| 4 KiB   | +1%   | 61.7GB/s  | 30.8GB/s  | page, 4 KiB |
| 8 KiB   | +3%   | 60.7GB/s  | 30.4GB/s  |
| 16 KiB  | +5%   | 59.5GB/s  | 29.7GB/s  |
| 32 KiB  | +50%  | 41.5GB/s  | 20.7GB/s  |
| 64 KiB  | +39%  | 44.9GB/s  | 22.4GB/s  |
| 128 KiB | +53%  | 40.7GB/s  | 20.4GB/s  |
| 256 KiB | +95%  | 31.9GB/s  | 16.0GB/s  |
| 512 KiB | +137% | 26.3GB/s  | 13.2GB/s  |
| 1 MiB   | +152% | 24.8GB/s  | 12.4GB/s  |
| 2 MiB   | +202% | 20.7GB/s  | 10.3GB/s  |
| 4 MiB   | +238% | 18.5GB/s  | 9.23GB/s  |
| 8 MiB   | +273% | 16.7GB/s  | 8.36GB/s  |
| 16 MiB  | +296% | 15.7GB/s  | 7.86GB/s  |
| 32 MiB  | +308% | 15.3GB/s  | 7.64GB/s  |
| 64 MiB  | +314% | 15.1GB/s  | 7.53GB/s  |
| 128 MiB | +316% | 15.0GB/s  | 7.49GB/s  |

**SIMD non-cached copy**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +27%  | 58.2GB/s  | 29.1GB/s  |
| 512 B   | +26%  | 58.6GB/s  | 29.3GB/s  |
| 1 KiB   | +27%  | 58.1GB/s  | 29.0GB/s  |
| 2 KiB   | +27%  | 57.9GB/s  | 29.0GB/s  |
| 4 KiB   | +26%  | 58.5GB/s  | 29.3GB/s  | page, 4 KiB |
| 8 KiB   | +27%  | 58.2GB/s  | 29.1GB/s  |
| 16 KiB  | +28%  | 57.9GB/s  | 28.9GB/s  |
| 32 KiB  | +29%  | 57.3GB/s  | 28.6GB/s  |
| 64 KiB  | +28%  | 57.6GB/s  | 28.8GB/s  |
| 128 KiB | +35%  | 54.8GB/s  | 27.4GB/s  |
| 256 KiB | +97%  | 37.5GB/s  | 18.8GB/s  |
| 512 KiB | +164% | 27.9GB/s  | 14.0GB/s  |
| 1 MiB   | +178% | 26.5GB/s  | 13.3GB/s  |
| 2 MiB   | +239% | 21.8GB/s  | 10.9GB/s  |
| 4 MiB   | -     | 18.5GB/s  | 9.23GB/s  |
| 8 MiB   | +10%  | 16.7GB/s  | 8.36GB/s  |
| 16 MiB  | +17%  | 15.8GB/s  | 7.88GB/s  |
| 32 MiB  | +21%  | 15.2GB/s  | 7.62GB/s  |
| 64 MiB  | +23%  | 15.0GB/s  | 7.52GB/s  |
| 128 MiB | +23%  | 15.0GB/s  | 7.49GB/s  |

</details>

<details><summary><b>memset, single thread</b></summary>

**memset**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +26%  | 49.7GB/s  |
| 512 B   | +7%   | 58.8GB/s  |
| 1 KiB   | +2%   | 61.4GB/s  |
| 2 KiB   | +2%   | 61.6GB/s  |
| 4 KiB   | -     | 62.8GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 62.1GB/s  |
| 16 KiB  | -     | 62.5GB/s  |
| 32 KiB  | +1%   | 62.3GB/s  |
| 64 KiB  | -     | 62.6GB/s  |
| 128 KiB | -     | 62.7GB/s  |
| 256 KiB | +6%   | 59.5GB/s  |
| 512 KiB | +60%  | 39.1GB/s  |
| 1 MiB   | +92%  | 32.7GB/s  |
| 2 MiB   | +107% | 30.3GB/s  |
| 4 MiB   | +115% | 29.2GB/s  |
| 8 MiB   | +117% | 28.9GB/s  |
| 16 MiB  | +114% | 29.3GB/s  |
| 32 MiB  | +106% | 30.4GB/s  |
| 64 MiB  | +107% | 30.3GB/s  |
| 128 MiB | +107% | 30.3GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +19%  | 51.4GB/s  |
| 512 B   | +1%   | 60.2GB/s  |
| 1 KiB   | -     | 60.9GB/s  |
| 2 KiB   | -     | 61.1GB/s  |
| 4 KiB   | -     | 60.8GB/s  | page, 4 KiB |
| 8 KiB   | -     | 61.0GB/s  |
| 16 KiB  | -     | 61.0GB/s  |
| 32 KiB  | +1%   | 60.3GB/s  |
| 64 KiB  | +1%   | 60.3GB/s  |
| 128 KiB | +1%   | 60.2GB/s  |
| 256 KiB | +6%   | 57.8GB/s  |
| 512 KiB | +57%  | 38.9GB/s  |
| 1 MiB   | +88%  | 32.6GB/s  |
| 2 MiB   | +103% | 30.2GB/s  |
| 4 MiB   | +110% | 29.0GB/s  |
| 8 MiB   | +114% | 28.5GB/s  |
| 16 MiB  | +117% | 28.2GB/s  |
| 32 MiB  | +115% | 28.4GB/s  |
| 64 MiB  | +101% | 30.4GB/s  |
| 128 MiB | +102% | 30.3GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +18%  | 51.7GB/s  |
| 512 B   | -     | 60.9GB/s  |
| 1 KiB   | -     | 61.0GB/s  |
| 2 KiB   | -     | 61.0GB/s  |
| 4 KiB   | -     | 61.1GB/s  | page, 4 KiB |
| 8 KiB   | -     | 61.1GB/s  |
| 16 KiB  | -     | 61.1GB/s  |
| 32 KiB  | +2%   | 59.9GB/s  |
| 64 KiB  | +1%   | 60.3GB/s  |
| 128 KiB | +1%   | 60.3GB/s  |
| 256 KiB | +8%   | 56.8GB/s  |
| 512 KiB | +58%  | 38.7GB/s  |
| 1 MiB   | +88%  | 32.4GB/s  |
| 2 MiB   | +103% | 30.1GB/s  |
| 4 MiB   | +111% | 29.0GB/s  |
| 8 MiB   | +116% | 28.3GB/s  |
| 16 MiB  | +117% | 28.2GB/s  |
| 32 MiB  | +117% | 28.1GB/s  |
| 64 MiB  | +102% | 30.3GB/s  |
| 128 MiB | +102% | 30.3GB/s  |

</details>

<details><summary><b>memset, multithreading (4 threads, 4 cores)</b></summary>

**memset**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +19%  | 61.6GB/s  | 30.8GB/s  |
| 512 B   | +2%   | 71.8GB/s  | 35.9GB/s  |
| 1 KiB   | +5%   | 69.8GB/s  | 34.9GB/s  |
| 2 KiB   | +3%   | 71.1GB/s  | 35.5GB/s  |
| 4 KiB   | -     | 73.3GB/s  | 36.6GB/s  | page, 4 KiB |
| 8 KiB   | +3%   | 70.9GB/s  | 35.5GB/s  |
| 16 KiB  | +8%   | 67.5GB/s  | 33.8GB/s  |
| 32 KiB  | +11%  | 65.9GB/s  | 32.9GB/s  |
| 64 KiB  | +9%   | 67.1GB/s  | 33.5GB/s  |
| 128 KiB | +14%  | 64.3GB/s  | 32.1GB/s  |
| 256 KiB | +79%  | 41.0GB/s  | 20.5GB/s  |
| 512 KiB | +86%  | 39.4GB/s  | 19.7GB/s  |
| 1 MiB   | +84%  | 39.8GB/s  | 19.9GB/s  |
| 2 MiB   | +80%  | 40.6GB/s  | 20.3GB/s  |
| 4 MiB   | +88%  | 39.0GB/s  | 19.5GB/s  |
| 8 MiB   | +104% | 35.9GB/s  | 18.0GB/s  |
| 16 MiB  | +118% | 33.7GB/s  | 16.8GB/s  |
| 32 MiB  | +98%  | 37.1GB/s  | 18.5GB/s  |
| 64 MiB  | +105% | 35.8GB/s  | 17.9GB/s  |
| 128 MiB | +102% | 36.4GB/s  | 18.2GB/s  |

**SIMD cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | -     | 81.2GB/s  | 40.6GB/s  |
| 512 B   | +16%  | 69.7GB/s  | 34.8GB/s  |
| 1 KiB   | +23%  | 66.1GB/s  | 33.1GB/s  |
| 2 KiB   | +15%  | 70.8GB/s  | 35.4GB/s  |
| 4 KiB   | +11%  | 73.2GB/s  | 36.6GB/s  | page, 4 KiB |
| 8 KiB   | +10%  | 74.1GB/s  | 37.1GB/s  |
| 16 KiB  | +6%   | 76.2GB/s  | 38.1GB/s  |
| 32 KiB  | +5%   | 77.6GB/s  | 38.8GB/s  |
| 64 KiB  | +26%  | 64.6GB/s  | 32.3GB/s  |
| 128 KiB | +31%  | 61.9GB/s  | 30.9GB/s  |
| 256 KiB | +102% | 40.2GB/s  | 20.1GB/s  |
| 512 KiB | +100% | 40.6GB/s  | 20.3GB/s  |
| 1 MiB   | +111% | 38.4GB/s  | 19.2GB/s  |
| 2 MiB   | +110% | 38.7GB/s  | 19.4GB/s  |
| 4 MiB   | +124% | 36.2GB/s  | 18.1GB/s  |
| 8 MiB   | +132% | 34.9GB/s  | 17.5GB/s  |
| 16 MiB  | +128% | 35.6GB/s  | 17.8GB/s  |
| 32 MiB  | +130% | 35.3GB/s  | 17.6GB/s  |
| 64 MiB  | +132% | 35.0GB/s  | 17.5GB/s  |
| 128 MiB | +139% | 34.0GB/s  | 17.0GB/s  |

**SIMD non-cached fill**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +35%  | 112.0GB/s  | 56.0GB/s  |
| 512 B   | +34%  | 113.2GB/s  | 56.6GB/s  |
| 1 KiB   | +35%  | 112.5GB/s  | 56.2GB/s  |
| 2 KiB   | +36%  | 111.4GB/s  | 55.7GB/s  |
| 4 KiB   | +36%  | 111.6GB/s  | 55.8GB/s  | page, 4 KiB |
| 8 KiB   | +36%  | 111.5GB/s  | 55.7GB/s  |
| 16 KiB  | +35%  | 112.0GB/s  | 56.0GB/s  |
| 32 KiB  | +37%  | 110.4GB/s  | 55.2GB/s  |
| 64 KiB  | +41%  | 107.7GB/s  | 53.8GB/s  |
| 128 KiB | +47%  | 103.1GB/s  | 51.6GB/s  |
| 256 KiB | +160% | 58.3GB/s   | 29.2GB/s  |
| 512 KiB | +218% | 47.6GB/s   | 23.8GB/s  |
| 1 MiB   | +242% | 44.3GB/s   | 22.1GB/s  |
| 2 MiB   | +253% | 43.0GB/s   | 21.5GB/s  |
| 4 MiB   | +2%   | 37.1GB/s   | 18.5GB/s  |
| 8 MiB   | +5%   | 35.9GB/s   | 18.0GB/s  |
| 16 MiB  | +9%   | 34.9GB/s   | 17.4GB/s  |
| 32 MiB  | +5%   | 36.1GB/s   | 18.0GB/s  |
| 64 MiB  | +8%   | 35.0GB/s   | 17.5GB/s  |
| 128 MiB | -     | 37.9GB/s   | 18.9GB/s  |

</details>

<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +38%  | 38.9GB/s  |
| 512 B   | +3%   | 52.0GB/s  |
| 1 KiB   | +1%   | 53.2GB/s  |
| 2 KiB   | -     | 53.5GB/s  |
| 4 KiB   | -     | 53.7GB/s  | page, 4 KiB |
| 8 KiB   | -     | 53.6GB/s  |
| 16 KiB  | +20%  | 44.5GB/s  |
| 32 KiB  | +53%  | 35.1GB/s  |
| 64 KiB  | +42%  | 37.8GB/s  |
| 128 KiB | +81%  | 29.7GB/s  |
| 256 KiB | +111% | 25.5GB/s  |
| 512 KiB | +136% | 22.7GB/s  |
| 1 MiB   | +157% | 20.9GB/s  |
| 2 MiB   | +166% | 20.1GB/s  |
| 4 MiB   | +181% | 19.1GB/s  |
| 8 MiB   | +203% | 17.7GB/s  |
| 16 MiB  | +216% | 17.0GB/s  |
| 32 MiB  | +223% | 16.6GB/s  |
| 64 MiB  | +229% | 16.3GB/s  |
| 128 MiB | +233% | 16.1GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +25% | 16.3GB/s  |
| 512 B   | +13% | 18.1GB/s  |
| 1 KiB   | +6%  | 19.1GB/s  |
| 2 KiB   | +3%  | 19.7GB/s  |
| 4 KiB   | +2%  | 20.0GB/s  | page, 4 KiB |
| 8 KiB   | +1%  | 20.2GB/s  |
| 16 KiB  | +1%  | 20.2GB/s  |
| 32 KiB  | -    | 20.3GB/s  |
| 64 KiB  | -    | 20.4GB/s  |
| 128 KiB | -    | 20.4GB/s  |
| 256 KiB | -    | 20.4GB/s  |
| 512 KiB | -    | 20.3GB/s  |
| 1 MiB   | +1%  | 20.1GB/s  |
| 2 MiB   | +1%  | 20.1GB/s  |
| 4 MiB   | +2%  | 20.1GB/s  |
| 8 MiB   | +7%  | 19.0GB/s  |
| 16 MiB  | +8%  | 18.9GB/s  |
| 32 MiB  | +5%  | 19.4GB/s  |
| 64 MiB  | +5%  | 19.5GB/s  |
| 128 MiB | +4%  | 19.5GB/s  |

</details>

<details><summary><b>other, multithreading (4 threads, 4 cores)</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block   | diff  | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +7%   | 58.9GB/s  | 29.4GB/s  |
| 512 B   | +3%   | 60.8GB/s  | 30.4GB/s  |
| 1 KiB   | +4%   | 60.4GB/s  | 30.2GB/s  |
| 2 KiB   | -     | 62.9GB/s  | 31.5GB/s  |
| 4 KiB   | +3%   | 60.8GB/s  | 30.4GB/s  | page, 4 KiB |
| 8 KiB   | +4%   | 60.5GB/s  | 30.3GB/s  |
| 16 KiB  | +6%   | 59.1GB/s  | 29.5GB/s  |
| 32 KiB  | +44%  | 43.7GB/s  | 21.8GB/s  |
| 64 KiB  | +12%  | 56.3GB/s  | 28.2GB/s  |
| 128 KiB | +54%  | 40.8GB/s  | 20.4GB/s  |
| 256 KiB | +73%  | 36.3GB/s  | 18.2GB/s  |
| 512 KiB | +106% | 30.6GB/s  | 15.3GB/s  |
| 1 MiB   | +112% | 29.7GB/s  | 14.8GB/s  |
| 2 MiB   | +179% | 22.6GB/s  | 11.3GB/s  |
| 4 MiB   | +209% | 20.4GB/s  | 10.2GB/s  |
| 8 MiB   | +231% | 19.0GB/s  | 9.50GB/s  |
| 16 MiB  | +241% | 18.4GB/s  | 9.22GB/s  |
| 32 MiB  | +240% | 18.5GB/s  | 9.26GB/s  |
| 64 MiB  | +247% | 18.1GB/s  | 9.06GB/s  |
| 128 MiB | +248% | 18.1GB/s  | 9.03GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block   | diff | bandwidth | bandwidth per thread | comment |
|---------|------|-----------|-----------|-------------|
| 256 B   | +13% | 40.8GB/s  | 20.4GB/s  |
| 512 B   | -    | 46.0GB/s  | 23.0GB/s  |
| 1 KiB   | +11% | 41.4GB/s  | 20.7GB/s  |
| 2 KiB   | +12% | 41.1GB/s  | 20.5GB/s  |
| 4 KiB   | +11% | 41.2GB/s  | 20.6GB/s  | page, 4 KiB |
| 8 KiB   | +33% | 34.6GB/s  | 17.3GB/s  |
| 16 KiB  | +38% | 33.2GB/s  | 16.6GB/s  |
| 32 KiB  | +44% | 31.8GB/s  | 15.9GB/s  |
| 64 KiB  | +42% | 32.3GB/s  | 16.2GB/s  |
| 128 KiB | +40% | 32.9GB/s  | 16.5GB/s  |
| 256 KiB | +40% | 32.8GB/s  | 16.4GB/s  |
| 512 KiB | +43% | 32.2GB/s  | 16.1GB/s  |
| 1 MiB   | +41% | 32.6GB/s  | 16.3GB/s  |
| 2 MiB   | +80% | 25.6GB/s  | 12.8GB/s  |
| 4 MiB   | +94% | 23.7GB/s  | 11.9GB/s  |
| 8 MiB   | +85% | 24.9GB/s  | 12.4GB/s  |
| 16 MiB  | +82% | 25.3GB/s  | 12.6GB/s  |
| 32 MiB  | +85% | 24.8GB/s  | 12.4GB/s  |
| 64 MiB  | +84% | 25.0GB/s  | 12.5GB/s  |
| 128 MiB | +86% | 24.7GB/s  | 12.4GB/s  |

</details>


# Snapdragon 8 Elite gen 5

* RAM: LPDDR5X, 64-bit, 5300 MHz, 84.8 GB/s ?

## Prime core

* Clock: 4.6 GHz
* 4x 128b Vector ALUs
* L1D: 96KB ?
* L2: 12MB per 2x cores

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block | diff | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +32%  | 51.5GB/s  |
| 512 B   | +13%  | 59.9GB/s  |
| 1 KiB   | +5%   | 64.6GB/s  |
| 2 KiB   | +2%   | 66.5GB/s  |
| 4 KiB   | -     | 67.8GB/s  | page, 4 KiB |
| 8 KiB   | +5%   | 64.7GB/s  |
| 16 KiB  | +1%   | 67.3GB/s  |
| 32 KiB  | -     | 67.8GB/s  |
| 64 KiB  | +112% | 32.0GB/s  |
| 128 KiB | +55%  | 43.8GB/s  |
| 256 KiB | +35%  | 50.2GB/s  |
| 512 KiB | +33%  | 51.0GB/s  |
| 1 MiB   | +31%  | 51.7GB/s  |
| 2 MiB   | +30%  | 52.1GB/s  |
| 4 MiB   | +56%  | 43.3GB/s  |
| 8 MiB   | +78%  | 38.2GB/s  |
| 16 MiB  | +102% | 33.6GB/s  |
| 32 MiB  | +105% | 33.0GB/s  |
| 64 MiB  | +109% | 32.4GB/s  |
| 128 MiB | +114% | 31.7GB/s  |

**SIMD cached copy**

| block | diff | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +64%  | 42.7GB/s  |
| 512 B   | +2%   | 68.5GB/s  |
| 1 KiB   | +4%   | 67.6GB/s  |
| 2 KiB   | +1%   | 69.3GB/s  |
| 4 KiB   | +4%   | 67.7GB/s  | page, 4 KiB |
| 8 KiB   | -     | 70.1GB/s  |
| 16 KiB  | +6%   | 66.0GB/s  |
| 32 KiB  | +5%   | 67.0GB/s  |
| 64 KiB  | +81%  | 38.7GB/s  | L1D ? |
| 128 KiB | +83%  | 38.4GB/s  |
| 256 KiB | +82%  | 38.5GB/s  |
| 512 KiB | +84%  | 38.2GB/s  |
| 1 MiB   | +81%  | 38.7GB/s  |
| 2 MiB   | +78%  | 39.5GB/s  |
| 4 MiB   | +80%  | 39.0GB/s  |
| 8 MiB   | +98%  | 35.4GB/s  |
| 16 MiB  | +126% | 31.1GB/s  | L2, 12MB shared |
| 32 MiB  | +188% | 24.3GB/s  |
| 64 MiB  | +222% | 21.8GB/s  |
| 128 MiB | +234% | 21.0GB/s  |

**SIMD non-cached copy**

| block | diff | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +143% | 26.1GB/s  |
| 512 B   | +28%  | 49.8GB/s  |
| 1 KiB   | +3%   | 61.6GB/s  |
| 2 KiB   | +2%   | 62.6GB/s  |
| 4 KiB   | -     | 63.3GB/s  | page, 4 KiB |
| 8 KiB   | -     | 63.6GB/s  |
| 16 KiB  | +2%   | 62.1GB/s  |
| 32 KiB  | +1%   | 63.0GB/s  |
| 64 KiB  | +97%  | 32.2GB/s  |
| 128 KiB | +37%  | 46.3GB/s  |
| 256 KiB | +26%  | 50.6GB/s  |
| 512 KiB | +24%  | 51.2GB/s  |
| 1 MiB   | +23%  | 51.7GB/s  |
| 2 MiB   | +22%  | 52.2GB/s  |
| 4 MiB   | +31%  | 48.6GB/s  |
| 8 MiB   | +42%  | 44.7GB/s  |
| 16 MiB  | +62%  | 39.3GB/s  | L2, 12MB shared |
| 32 MiB  | +79%  | 35.5GB/s  |
| 64 MiB  | +89%  | 33.6GB/s  |
| 128 MiB | +95%  | 32.6GB/s  |

</details>

<details><summary><b>memcpy, multithreading (2 threads, 2 cores)</b></summary>

**memcpy**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +30%  | 60.1GB/s  | 30.0GB/s  |
| 512 B   | +28%  | 61.3GB/s  | 30.6GB/s  |
| 1 KiB   | +11%  | 70.6GB/s  | 35.3GB/s  |
| 2 KiB   | -     | 78.4GB/s  | 39.2GB/s  |
| 4 KiB   | +3%   | 76.4GB/s  | 38.2GB/s  | page, 4 KiB |
| 8 KiB   | +8%   | 72.7GB/s  | 36.4GB/s  |
| 16 KiB  | +6%   | 73.8GB/s  | 36.9GB/s  |
| 32 KiB  | +4%   | 75.0GB/s  | 37.5GB/s  |
| 64 KiB  | +107% | 37.9GB/s  | 19.0GB/s  |
| 128 KiB | +60%  | 49.1GB/s  | 24.6GB/s  |
| 256 KiB | +45%  | 54.1GB/s  | 27.0GB/s  |
| 512 KiB | +47%  | 53.4GB/s  | 26.7GB/s  |
| 1 MiB   | +48%  | 52.9GB/s  | 26.5GB/s  |
| 2 MiB   | +59%  | 49.1GB/s  | 24.6GB/s  |
| 4 MiB   | +130% | 34.1GB/s  | 17.1GB/s  |
| 8 MiB   | +154% | 30.9GB/s  | 15.4GB/s  |
| 16 MiB  | +154% | 30.9GB/s  | 15.4GB/s  | L2, 12MB shared |
| 32 MiB  | +153% | 30.9GB/s  | 15.5GB/s  |
| 64 MiB  | +161% | 30.1GB/s  | 15.0GB/s  |
| 128 MiB | +159% | 30.3GB/s  | 15.1GB/s  |

**SIMD cached copy**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +54%  | 50.4GB/s  | 25.2GB/s  |
| 512 B   | +4%   | 74.8GB/s  | 37.4GB/s  |
| 1 KiB   | +8%   | 72.0GB/s  | 36.0GB/s  |
| 2 KiB   | -     | 77.7GB/s  | 38.9GB/s  |
| 4 KiB   | +7%   | 72.4GB/s  | 36.2GB/s  | page, 4 KiB |
| 8 KiB   | +9%   | 71.0GB/s  | 35.5GB/s  |
| 16 KiB  | +9%   | 71.1GB/s  | 35.6GB/s  |
| 32 KiB  | +4%   | 75.0GB/s  | 37.5GB/s  |
| 64 KiB  | +63%  | 47.7GB/s  | 23.9GB/s  |
| 128 KiB | +70%  | 45.7GB/s  | 22.9GB/s  |
| 256 KiB | +74%  | 44.7GB/s  | 22.4GB/s  |
| 512 KiB | +78%  | 43.7GB/s  | 21.8GB/s  |
| 1 MiB   | +62%  | 48.0GB/s  | 24.0GB/s  |
| 2 MiB   | +71%  | 45.3GB/s  | 22.7GB/s  |
| 4 MiB   | +125% | 34.5GB/s  | 17.3GB/s  |
| 8 MiB   | +196% | 26.3GB/s  | 13.1GB/s  |
| 16 MiB  | +251% | 22.1GB/s  | 11.1GB/s  | L2, 12MB shared |
| 32 MiB  | +288% | 20.0GB/s  | 10.0GB/s  |
| 64 MiB  | +300% | 19.4GB/s  | 9.72GB/s  |
| 128 MiB | +316% | 18.7GB/s  | 9.34GB/s  |

**SIMD non-cached copy**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +257% | 37.3GB/s  | 18.7GB/s  |
| 512 B   | +94%  | 68.6GB/s  | 34.3GB/s  |
| 1 KiB   | +58%  | 84.4GB/s  | 42.2GB/s  |
| 2 KiB   | +54%  | 86.7GB/s  | 43.4GB/s  |
| 4 KiB   | +54%  | 86.5GB/s  | 43.2GB/s  | page, 4 KiB |
| 8 KiB   | +60%  | 83.3GB/s  | 41.6GB/s  |
| 16 KiB  | +55%  | 86.1GB/s  | 43.0GB/s  |
| 32 KiB  | +54%  | 86.4GB/s  | 43.2GB/s  |
| 64 KiB  | +193% | 45.5GB/s  | 22.7GB/s  |
| 128 KiB | +112% | 62.8GB/s  | 31.4GB/s  |
| 256 KiB | +98%  | 67.1GB/s  | 33.6GB/s  |
| 512 KiB | +96%  | 68.0GB/s  | 34.0GB/s  |
| 1 MiB   | +101% | 66.2GB/s  | 33.1GB/s  |
| 2 MiB   | +114% | 62.3GB/s  | 31.2GB/s  |
| 4 MiB   | -     | 33.3GB/s  | 16.7GB/s  |
| 8 MiB   | +5%   | 31.7GB/s  | 15.8GB/s  |
| 16 MiB  | +6%   | 31.4GB/s  | 15.7GB/s  | L2, 12MB shared |
| 32 MiB  | +9%   | 30.5GB/s  | 15.2GB/s  |
| 64 MiB  | +11%  | 30.1GB/s  | 15.1GB/s  |
| 128 MiB | +11%  | 30.0GB/s  | 15.0GB/s  |

</details>

<details><summary><b>memset, single thread</b></summary>

**memset**

| block | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +83% | 39.1GB/s  |
| 512 B   | +15% | 62.2GB/s  |
| 1 KiB   | -    | 71.5GB/s  |
| 2 KiB   | +9%  | 65.7GB/s  |
| 4 KiB   | +9%  | 65.7GB/s  | page, 4 KiB |
| 8 KiB   | +8%  | 66.0GB/s  |
| 16 KiB  | +9%  | 65.7GB/s  |
| 32 KiB  | +8%  | 66.0GB/s  |
| 64 KiB  | +10% | 65.0GB/s  |
| 128 KiB | +11% | 64.6GB/s  |
| 256 KiB | +12% | 63.7GB/s  |
| 512 KiB | +15% | 62.4GB/s  |
| 1 MiB   | +13% | 63.3GB/s  |
| 2 MiB   | +14% | 62.4GB/s  |
| 4 MiB   | +15% | 62.2GB/s  |
| 8 MiB   | +13% | 63.1GB/s  |
| 16 MiB  | +9%  | 65.6GB/s  | L2, 12MB shared |
| 32 MiB  | +4%  | 68.8GB/s  |
| 64 MiB  | +6%  | 67.1GB/s  |
| 128 MiB | +11% | 64.7GB/s  |

**SIMD cached fill**

| block | diff | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +1%   | 70.3GB/s  |
| 512 B   | -     | 70.9GB/s  |
| 1 KiB   | -     | 71.0GB/s  |
| 2 KiB   | -     | 71.0GB/s  |
| 4 KiB   | -     | 71.0GB/s  | page, 4 KiB |
| 8 KiB   | -     | 71.0GB/s  |
| 16 KiB  | -     | 71.0GB/s  |
| 32 KiB  | -     | 71.0GB/s  |
| 64 KiB  | -     | 71.0GB/s  |
| 128 KiB | +21%  | 58.6GB/s  |
| 256 KiB | +21%  | 58.8GB/s  |
| 512 KiB | +21%  | 58.6GB/s  |
| 1 MiB   | +20%  | 58.9GB/s  |
| 2 MiB   | +21%  | 58.9GB/s  |
| 4 MiB   | +20%  | 59.0GB/s  |
| 8 MiB   | +23%  | 57.6GB/s  |
| 16 MiB  | +24%  | 57.0GB/s  | L2, 12MB shared |
| 32 MiB  | +74%  | 40.9GB/s  |
| 64 MiB  | +117% | 32.7GB/s  |
| 128 MiB | +139% | 29.7GB/s  |

**SIMD non-cached fill**

| block | diff | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +293% | 18.0GB/s  |
| 512 B   | +493% | 11.9GB/s  |
| 1 KiB   | +197% | 23.9GB/s  |
| 2 KiB   | +31%  | 53.9GB/s  |
| 4 KiB   | +1%   | 69.9GB/s  | page, 4 KiB |
| 8 KiB   | +1%   | 69.9GB/s  |
| 16 KiB  | -     | 70.7GB/s  |
| 32 KiB  | +3%   | 68.7GB/s  |
| 64 KiB  | +1%   | 70.1GB/s  |
| 128 KiB | -     | 70.8GB/s  |
| 256 KiB | +1%   | 70.2GB/s  |
| 512 KiB | +1%   | 70.1GB/s  |
| 1 MiB   | -     | 70.7GB/s  |
| 2 MiB   | -     | 70.9GB/s  |
| 4 MiB   | -     | 70.6GB/s  |
| 8 MiB   | +1%   | 70.4GB/s  |
| 16 MiB  | -     | 70.8GB/s  | L2, 12MB shared |
| 32 MiB  | -     | 70.7GB/s  |
| 64 MiB  | +1%   | 70.3GB/s  |
| 128 MiB | +2%   | 69.2GB/s  |

</details>

<details><summary><b>memset, multithreading (2 threads, 2 cores)</b></summary>

**memset**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|------|-----------|-----------|-------------|
| 256 B   | +52% | 49.9GB/s  | 25.0GB/s  |
| 512 B   | +8%  | 70.0GB/s  | 35.0GB/s  |
| 1 KiB   | +7%  | 70.5GB/s  | 35.2GB/s  |
| 2 KiB   | +7%  | 70.6GB/s  | 35.3GB/s  |
| 4 KiB   | +4%  | 72.7GB/s  | 36.3GB/s  | page, 4 KiB |
| 8 KiB   | +6%  | 71.3GB/s  | 35.6GB/s  |
| 16 KiB  | +10% | 69.1GB/s  | 34.6GB/s  |
| 32 KiB  | -    | 75.7GB/s  | 37.9GB/s  |
| 64 KiB  | +27% | 59.5GB/s  | 29.8GB/s  |
| 128 KiB | +63% | 46.5GB/s  | 23.3GB/s  |
| 256 KiB | +69% | 44.9GB/s  | 22.5GB/s  |
| 512 KiB | +55% | 48.9GB/s  | 24.4GB/s  |
| 1 MiB   | +52% | 49.8GB/s  | 24.9GB/s  |
| 2 MiB   | +44% | 52.6GB/s  | 26.3GB/s  |
| 4 MiB   | +56% | 48.6GB/s  | 24.3GB/s  |
| 8 MiB   | +46% | 51.9GB/s  | 25.9GB/s  |
| 16 MiB  | +54% | 49.2GB/s  | 24.6GB/s  | L2, 12MB shared |
| 32 MiB  | +53% | 49.6GB/s  | 24.8GB/s  |
| 64 MiB  | +52% | 49.8GB/s  | 24.9GB/s  |
| 128 MiB | +52% | 49.7GB/s  | 24.8GB/s  |

**SIMD cached fill**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | -     | 76.3GB/s  | 38.2GB/s  |
| 512 B   | +3%   | 73.9GB/s  | 37.0GB/s  |
| 1 KiB   | +9%   | 70.3GB/s  | 35.2GB/s  |
| 2 KiB   | +13%  | 67.3GB/s  | 33.6GB/s  |
| 4 KiB   | +13%  | 67.5GB/s  | 33.7GB/s  | page, 4 KiB |
| 8 KiB   | +6%   | 72.3GB/s  | 36.2GB/s  |
| 16 KiB  | +9%   | 69.9GB/s  | 35.0GB/s  |
| 32 KiB  | +6%   | 72.3GB/s  | 36.1GB/s  |
| 64 KiB  | +35%  | 56.5GB/s  | 28.3GB/s  |
| 128 KiB | +31%  | 58.2GB/s  | 29.1GB/s  |
| 256 KiB | +22%  | 62.5GB/s  | 31.3GB/s  |
| 512 KiB | +36%  | 56.3GB/s  | 28.1GB/s  |
| 1 MiB   | +40%  | 54.6GB/s  | 27.3GB/s  |
| 2 MiB   | +31%  | 58.1GB/s  | 29.1GB/s  |
| 4 MiB   | +62%  | 47.1GB/s  | 23.5GB/s  |
| 8 MiB   | +127% | 33.7GB/s  | 16.8GB/s  |
| 16 MiB  | +146% | 31.1GB/s  | 15.5GB/s  | L2, 12MB shared |
| 32 MiB  | +167% | 28.6GB/s  | 14.3GB/s  |
| 64 MiB  | +176% | 27.6GB/s  | 13.8GB/s  |
| 128 MiB | +179% | 27.3GB/s  | 13.7GB/s  |

**SIMD non-cached fill**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +848% | 20.2GB/s  | 10.1GB/s  |
| 512 B   | +338% | 43.7GB/s  | 21.9GB/s  |
| 1 KiB   | +144% | 78.4GB/s  | 39.2GB/s  |
| 2 KiB   | +138% | 80.5GB/s  | 40.2GB/s  |
| 4 KiB   | +161% | 73.5GB/s  | 36.7GB/s  | page, 4 KiB |
| 8 KiB   | +237% | 56.9GB/s  | 28.4GB/s  |
| 16 KiB  | +231% | 57.9GB/s  | 28.9GB/s  |
| 32 KiB  | +218% | 60.2GB/s  | 30.1GB/s  |
| 64 KiB  | +204% | 63.0GB/s  | 31.5GB/s  |
| 128 KiB | +203% | 63.2GB/s  | 31.6GB/s  |
| 256 KiB | +200% | 63.7GB/s  | 31.9GB/s  |
| 512 KiB | +203% | 63.2GB/s  | 31.6GB/s  |
| 1 MiB   | +205% | 62.8GB/s  | 31.4GB/s  |
| 2 MiB   | +205% | 62.8GB/s  | 31.4GB/s  |
| 4 MiB   | +6%   | 45.1GB/s  | 22.5GB/s  |
| 8 MiB   | -     | 47.9GB/s  | 23.9GB/s  |
| 16 MiB  | -     | 47.8GB/s  | 23.9GB/s  | L2, 12MB shared |
| 32 MiB  | +5%   | 45.6GB/s  | 22.8GB/s  |
| 64 MiB  | -     | 47.7GB/s  | 23.8GB/s  |
| 128 MiB | -     | 47.8GB/s  | 23.9GB/s  |

</details>

<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block | diff | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +175% | 34.5GB/s  |
| 512 B   | +35%  | 70.3GB/s  |
| 1 KiB   | +1%   | 93.5GB/s  |
| 2 KiB   | -     | 94.6GB/s  |
| 4 KiB   | -     | 94.3GB/s  | page, 4 KiB |
| 8 KiB   | -     | 94.6GB/s  |
| 16 KiB  | +4%   | 90.9GB/s  |
| 32 KiB  | +2%   | 92.8GB/s  |
| 64 KiB  | +50%  | 63.1GB/s  |
| 128 KiB | +92%  | 49.2GB/s  |
| 256 KiB | +92%  | 49.4GB/s  |
| 512 KiB | +95%  | 48.5GB/s  |
| 1 MiB   | +108% | 45.4GB/s  |
| 2 MiB   | +132% | 40.8GB/s  |
| 4 MiB   | +134% | 40.4GB/s  |
| 8 MiB   | +144% | 38.8GB/s  |
| 16 MiB  | +151% | 37.7GB/s  | L2, 12MB shared |
| 32 MiB  | +168% | 35.3GB/s  |
| 64 MiB  | +192% | 32.4GB/s  |
| 128 MiB | +200% | 31.6GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +42% | 13.9GB/s  |
| 512 B   | +21% | 16.4GB/s  |
| 1 KiB   | +10% | 17.9GB/s  |
| 2 KiB   | +5%  | 18.8GB/s  |
| 4 KiB   | +3%  | 19.3GB/s  | page, 4 KiB |
| 8 KiB   | +1%  | 19.5GB/s  |
| 16 KiB  | +1%  | 19.7GB/s  |
| 32 KiB  | -    | 19.7GB/s  |
| 64 KiB  | -    | 19.8GB/s  |
| 128 KiB | -    | 19.8GB/s  |
| 256 KiB | -    | 19.8GB/s  |
| 512 KiB | -    | 19.8GB/s  |
| 1 MiB   | -    | 19.8GB/s  |
| 2 MiB   | -    | 19.8GB/s  |
| 4 MiB   | -    | 19.8GB/s  |
| 8 MiB   | -    | 19.8GB/s  |
| 16 MiB  | -    | 19.8GB/s  | L2, 12MB shared |
| 32 MiB  | -    | 19.8GB/s  |
| 64 MiB  | -    | 19.8GB/s  |
| 128 MiB | -    | 19.7GB/s  |

</details>

<details><summary><b>other, multithreading (2 threads, 2 cores)</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +136% | 38.2GB/s  | 19.1GB/s  |
| 512 B   | +22%  | 74.0GB/s  | 37.0GB/s  |
| 1 KiB   | +12%  | 80.8GB/s  | 40.4GB/s  |
| 2 KiB   | +3%   | 87.6GB/s  | 43.8GB/s  |
| 4 KiB   | +5%   | 86.1GB/s  | 43.0GB/s  | page, 4 KiB |
| 8 KiB   | +13%  | 79.9GB/s  | 39.9GB/s  |
| 16 KiB  | +4%   | 86.9GB/s  | 43.4GB/s  |
| 32 KiB  | -     | 90.4GB/s  | 45.2GB/s  |
| 64 KiB  | +49%  | 60.6GB/s  | 30.3GB/s  |
| 128 KiB | +80%  | 50.3GB/s  | 25.1GB/s  |
| 256 KiB | +76%  | 51.2GB/s  | 25.6GB/s  |
| 512 KiB | +73%  | 52.2GB/s  | 26.1GB/s  |
| 1 MiB   | +81%  | 49.9GB/s  | 24.9GB/s  |
| 2 MiB   | +85%  | 48.7GB/s  | 24.4GB/s  |
| 4 MiB   | +101% | 44.9GB/s  | 22.4GB/s  |
| 8 MiB   | +152% | 35.8GB/s  | 17.9GB/s  |
| 16 MiB  | +188% | 31.4GB/s  | 15.7GB/s  | L2, 12MB shared |
| 32 MiB  | +211% | 29.0GB/s  | 14.5GB/s  |
| 64 MiB  | +217% | 28.5GB/s  | 14.3GB/s  |
| 128 MiB | +219% | 28.4GB/s  | 14.2GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|------|-----------|-----------|-------------|
| 256 B   | +42% | 35.2GB/s  | 17.6GB/s  |
| 512 B   | +21% | 41.3GB/s  | 20.6GB/s  |
| 1 KiB   | +16% | 43.1GB/s  | 21.5GB/s  |
| 2 KiB   | -    | 49.8GB/s  | 24.9GB/s  |
| 4 KiB   | +14% | 43.9GB/s  | 21.9GB/s  | page, 4 KiB |
| 8 KiB   | +38% | 36.1GB/s  | 18.1GB/s  |
| 16 KiB  | +51% | 32.9GB/s  | 16.5GB/s  |
| 32 KiB  | +56% | 31.9GB/s  | 16.0GB/s  |
| 64 KiB  | +58% | 31.5GB/s  | 15.7GB/s  |
| 128 KiB | +60% | 31.0GB/s  | 15.5GB/s  |
| 256 KiB | +60% | 31.2GB/s  | 15.6GB/s  |
| 512 KiB | +59% | 31.3GB/s  | 15.6GB/s  |
| 1 MiB   | +61% | 30.9GB/s  | 15.5GB/s  |
| 2 MiB   | +58% | 31.5GB/s  | 15.7GB/s  |
| 4 MiB   | +58% | 31.6GB/s  | 15.8GB/s  |
| 8 MiB   | +62% | 30.7GB/s  | 15.3GB/s  |
| 16 MiB  | +62% | 30.8GB/s  | 15.4GB/s  | L2, 12MB shared |
| 32 MiB  | +59% | 31.2GB/s  | 15.6GB/s  |
| 64 MiB  | +59% | 31.3GB/s  | 15.6GB/s  |
| 128 MiB | +61% | 30.9GB/s  | 15.4GB/s  |

</details>


## Performance core

* Clock: 3.6 GHz
* 2x 128bit int pipes ???
* L1D: 64KB ?
* L2: 12MB per 6x cores

<details><summary><b>memcpy, single thread</b></summary>

**memcpy**

| block | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +46% | 22.9GB/s  |
| 512 B   | +21% | 27.5GB/s  |
| 1 KiB   | +10% | 30.5GB/s  |
| 2 KiB   | +4%  | 32.1GB/s  |
| 4 KiB   | -    | 33.3GB/s  | page, 4 KiB |
| 8 KiB   | +2%  | 32.8GB/s  |
| 16 KiB  | -    | 33.5GB/s  |
| 32 KiB  | +3%  | 32.6GB/s  |
| 64 KiB  | +10% | 30.4GB/s  |
| 128 KiB | +11% | 30.0GB/s  |
| 256 KiB | +11% | 30.3GB/s  |
| 512 KiB | +13% | 29.7GB/s  |
| 1 MiB   | +15% | 29.0GB/s  |
| 2 MiB   | +27% | 26.4GB/s  |
| 4 MiB   | +25% | 26.8GB/s  |
| 8 MiB   | +24% | 26.9GB/s  |
| 16 MiB  | +28% | 26.2GB/s  | L2, 12MB shared |
| 32 MiB  | +27% | 26.4GB/s  |
| 64 MiB  | +27% | 26.4GB/s  |
| 128 MiB | +27% | 26.4GB/s  |

**SIMD cached copy**

| block | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +6%  | 32.5GB/s  |
| 512 B   | +3%  | 33.4GB/s  |
| 1 KiB   | +2%  | 33.8GB/s  |
| 2 KiB   | -    | 34.1GB/s  |
| 4 KiB   | -    | 34.3GB/s  | page, 4 KiB |
| 8 KiB   | +3%  | 33.4GB/s  |
| 16 KiB  | +1%  | 33.9GB/s  |
| 32 KiB  | +7%  | 32.2GB/s  |
| 64 KiB  | +30% | 26.4GB/s  |
| 128 KiB | +29% | 26.5GB/s  |
| 256 KiB | +29% | 26.6GB/s  |
| 512 KiB | +29% | 26.7GB/s  |
| 1 MiB   | +29% | 26.6GB/s  |
| 2 MiB   | +28% | 26.7GB/s  |
| 4 MiB   | +31% | 26.2GB/s  |
| 8 MiB   | +34% | 25.6GB/s  |
| 16 MiB  | +52% | 22.6GB/s  | L2, 12MB shared |
| 32 MiB  | +67% | 20.5GB/s  |
| 64 MiB  | +74% | 19.7GB/s  |
| 128 MiB | +78% | 19.2GB/s  |

**SIMD non-cached copy**

| block | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +15% | 23.9GB/s  |
| 512 B   | +7%  | 25.6GB/s  |
| 1 KiB   | +4%  | 26.5GB/s  |
| 2 KiB   | +2%  | 27.0GB/s  |
| 4 KiB   | +1%  | 27.3GB/s  | page, 4 KiB |
| 8 KiB   | +2%  | 26.8GB/s  |
| 16 KiB  | +1%  | 27.1GB/s  |
| 32 KiB  | +1%  | 27.2GB/s  |
| 64 KiB  | +1%  | 27.3GB/s  |
| 128 KiB | -    | 27.4GB/s  |
| 256 KiB | -    | 27.4GB/s  |
| 512 KiB | -    | 27.4GB/s  |
| 1 MiB   | -    | 27.4GB/s  |
| 2 MiB   | +1%  | 27.3GB/s  |
| 4 MiB   | +6%  | 26.0GB/s  |
| 8 MiB   | +9%  | 25.2GB/s  |
| 16 MiB  | +7%  | 25.7GB/s  | L2, 12MB shared |
| 32 MiB  | +6%  | 25.8GB/s  |
| 64 MiB  | +6%  | 25.9GB/s  |
| 128 MiB | +6%  | 25.8GB/s  |

</details>

<details><summary><b>memcpy, multithreading (4 threads, 4 cores)</b></summary>

**memcpy**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +45%  | 68.3GB/s  | 17.1GB/s  |
| 512 B   | +22%  | 81.3GB/s  | 20.3GB/s  |
| 1 KiB   | +19%  | 83.1GB/s  | 20.8GB/s  |
| 2 KiB   | +12%  | 88.2GB/s  | 22.1GB/s  |
| 4 KiB   | -     | 99.2GB/s  | 24.8GB/s  | page, 4 KiB |
| 8 KiB   | +3%   | 96.8GB/s  | 24.2GB/s  |
| 16 KiB  | +18%  | 84.1GB/s  | 21.0GB/s  |
| 32 KiB  | +11%  | 89.8GB/s  | 22.5GB/s  |
| 64 KiB  | +21%  | 82.2GB/s  | 20.5GB/s  |
| 128 KiB | +21%  | 82.3GB/s  | 20.6GB/s  |
| 256 KiB | +18%  | 83.9GB/s  | 21.0GB/s  |
| 512 KiB | +33%  | 74.7GB/s  | 18.7GB/s  |
| 1 MiB   | +53%  | 64.9GB/s  | 16.2GB/s  |
| 2 MiB   | +197% | 33.4GB/s  | 8.35GB/s  |
| 4 MiB   | +218% | 31.2GB/s  | 7.81GB/s  |
| 8 MiB   | +213% | 31.7GB/s  | 7.93GB/s  |
| 16 MiB  | +211% | 31.9GB/s  | 7.98GB/s  | L2, 12MB shared |
| 32 MiB  | +213% | 31.7GB/s  | 7.92GB/s  |
| 64 MiB  | +216% | 31.4GB/s  | 7.86GB/s  |
| 128 MiB | +226% | 30.5GB/s  | 7.62GB/s  |

**SIMD cached copy**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +5%   | 92.7GB/s  | 23.2GB/s  |
| 512 B   | +10%  | 88.5GB/s  | 22.1GB/s  |
| 1 KiB   | -     | 97.2GB/s  | 24.3GB/s  |
| 2 KiB   | +13%  | 86.1GB/s  | 21.5GB/s  |
| 4 KiB   | +1%   | 96.4GB/s  | 24.1GB/s  | page, 4 KiB |
| 8 KiB   | -     | 97.3GB/s  | 24.3GB/s  |
| 16 KiB  | +10%  | 88.7GB/s  | 22.2GB/s  |
| 32 KiB  | +13%  | 86.2GB/s  | 21.5GB/s  |
| 64 KiB  | +33%  | 73.3GB/s  | 18.3GB/s  |
| 128 KiB | +31%  | 74.3GB/s  | 18.6GB/s  |
| 256 KiB | +25%  | 77.6GB/s  | 19.4GB/s  |
| 512 KiB | +22%  | 79.9GB/s  | 20.0GB/s  |
| 1 MiB   | +46%  | 66.7GB/s  | 16.7GB/s  |
| 2 MiB   | +113% | 45.6GB/s  | 11.4GB/s  |
| 4 MiB   | +214% | 31.0GB/s  | 7.75GB/s  |
| 8 MiB   | +360% | 21.2GB/s  | 5.29GB/s  |
| 16 MiB  | +414% | 18.9GB/s  | 4.74GB/s  | L2, 12MB shared |
| 32 MiB  | +419% | 18.8GB/s  | 4.69GB/s  |
| 64 MiB  | +418% | 18.8GB/s  | 4.69GB/s  |
| 128 MiB | +452% | 17.6GB/s  | 4.41GB/s  |

**SIMD non-cached copy**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|-----------|-----------|-------------|
| 256 B   | +45%  | 80.3GB/s  | 20.1GB/s  |
| 512 B   | +35%  | 86.2GB/s  | 21.6GB/s  |
| 1 KiB   | +28%  | 90.4GB/s  | 22.6GB/s  |
| 2 KiB   | +26%  | 91.8GB/s  | 23.0GB/s  |
| 4 KiB   | +25%  | 92.6GB/s  | 23.2GB/s  | page, 4 KiB |
| 8 KiB   | +26%  | 91.8GB/s  | 23.0GB/s  |
| 16 KiB  | +26%  | 91.8GB/s  | 23.0GB/s  |
| 32 KiB  | +25%  | 92.7GB/s  | 23.2GB/s  |
| 64 KiB  | +25%  | 92.7GB/s  | 23.2GB/s  |
| 128 KiB | +25%  | 93.0GB/s  | 23.2GB/s  |
| 256 KiB | +25%  | 92.7GB/s  | 23.2GB/s  |
| 512 KiB | +30%  | 89.7GB/s  | 22.4GB/s  |
| 1 MiB   | +41%  | 82.3GB/s  | 20.6GB/s  |
| 2 MiB   | +116% | 53.9GB/s  | 13.5GB/s  |
| 4 MiB   | -     | 29.0GB/s  | 7.26GB/s  |
| 8 MiB   | +6%   | 27.5GB/s  | 6.87GB/s  |
| 16 MiB  | +9%   | 26.7GB/s  | 6.68GB/s  | L2, 12MB shared |
| 32 MiB  | +10%  | 26.3GB/s  | 6.57GB/s  |
| 64 MiB  | +11%  | 26.1GB/s  | 6.53GB/s  |
| 128 MiB | +12%  | 26.0GB/s  | 6.51GB/s  |


</details>

<details><summary><b>memset, single thread</b></summary>

**memset**

| block | diff | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +232% | 21.0GB/s  |
| 512 B   | +97%  | 35.4GB/s  |
| 1 KiB   | +29%  | 54.1GB/s  |
| 2 KiB   | +5%   | 66.3GB/s  |
| 4 KiB   | +4%   | 66.7GB/s  | page, 4 KiB |
| 8 KiB   | +7%   | 65.3GB/s  |
| 16 KiB  | +2%   | 68.5GB/s  |
| 32 KiB  | -     | 69.7GB/s  |
| 64 KiB  | +6%   | 65.5GB/s  |
| 128 KiB | +19%  | 58.8GB/s  |
| 256 KiB | +9%   | 63.7GB/s  |
| 512 KiB | +10%  | 63.6GB/s  |
| 1 MiB   | +10%  | 63.5GB/s  |
| 2 MiB   | +10%  | 63.4GB/s  |
| 4 MiB   | +10%  | 63.3GB/s  |
| 8 MiB   | +10%  | 63.5GB/s  |
| 16 MiB  | +10%  | 63.3GB/s  | L2, 12MB shared |
| 32 MiB  | +12%  | 62.4GB/s  |
| 64 MiB  | +11%  | 62.8GB/s  |
| 128 MiB | +12%  | 62.2GB/s  |

**SIMD cached fill**

| block | diff | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +17%  | 58.1GB/s  |
| 512 B   | +9%   | 62.1GB/s  |
| 1 KiB   | +4%   | 65.1GB/s  |
| 2 KiB   | +2%   | 66.6GB/s  |
| 4 KiB   | +10%  | 61.6GB/s  | page, 4 KiB |
| 8 KiB   | +4%   | 65.1GB/s  |
| 16 KiB  | +2%   | 66.4GB/s  |
| 32 KiB  | -     | 67.7GB/s  |
| 64 KiB  | +6%   | 63.9GB/s  |
| 128 KiB | +36%  | 49.9GB/s  |
| 256 KiB | +36%  | 50.0GB/s  |
| 512 KiB | +36%  | 49.8GB/s  |
| 1 MiB   | +35%  | 50.0GB/s  |
| 2 MiB   | +35%  | 50.2GB/s  |
| 4 MiB   | +34%  | 50.4GB/s  |
| 8 MiB   | +40%  | 48.3GB/s  |
| 16 MiB  | +40%  | 48.4GB/s  | L2, 12MB shared |
| 32 MiB  | +86%  | 36.5GB/s  |
| 64 MiB  | +127% | 29.8GB/s  |
| 128 MiB | +132% | 29.2GB/s  |

**SIMD non-cached fill**

| block | diff | bandwidth | comment |
|---------|-------|-----------|-------------|
| 256 B   | +353% | 11.1GB/s  |
| 512 B   | +426% | 9.53GB/s  |
| 1 KiB   | +184% | 17.6GB/s  |
| 2 KiB   | +43%  | 35.1GB/s  |
| 4 KiB   | +1%   | 49.8GB/s  | page, 4 KiB |
| 8 KiB   | +4%   | 48.2GB/s  |
| 16 KiB  | +2%   | 49.1GB/s  |
| 32 KiB  | +1%   | 49.6GB/s  |
| 64 KiB  | +1%   | 49.8GB/s  |
| 128 KiB | -     | 50.0GB/s  |
| 256 KiB | -     | 50.1GB/s  |
| 512 KiB | -     | 50.1GB/s  |
| 1 MiB   | +1%   | 49.7GB/s  |
| 2 MiB   | -     | 49.9GB/s  |
| 4 MiB   | -     | 49.9GB/s  |
| 8 MiB   | -     | 50.0GB/s  |
| 16 MiB  | -     | 49.9GB/s  | L2, 12MB shared |
| 32 MiB  | -     | 49.9GB/s  |
| 64 MiB  | +1%   | 49.6GB/s  |
| 128 MiB | -     | 50.0GB/s  |

</details>

<details><summary><b>memset, multithreading (4 threads, 4 cores)</b></summary>

**memset**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +90%  | 88.3GB/s   | 22.1GB/s  |
| 512 B   | +53%  | 109.7GB/s  | 27.4GB/s  |
| 1 KiB   | +24%  | 135.4GB/s  | 33.8GB/s  |
| 2 KiB   | +20%  | 139.7GB/s  | 34.9GB/s  |
| 4 KiB   | -     | 167.6GB/s  | 41.9GB/s  | page, 4 KiB |
| 8 KiB   | +10%  | 151.7GB/s  | 37.9GB/s  |
| 16 KiB  | +15%  | 146.0GB/s  | 36.5GB/s  |
| 32 KiB  | +24%  | 135.3GB/s  | 33.8GB/s  |
| 64 KiB  | +57%  | 106.8GB/s  | 26.7GB/s  |
| 128 KiB | +172% | 61.7GB/s   | 15.4GB/s  |
| 256 KiB | +185% | 58.8GB/s   | 14.7GB/s  |
| 512 KiB | +193% | 57.2GB/s   | 14.3GB/s  |
| 1 MiB   | +185% | 58.9GB/s   | 14.7GB/s  |
| 2 MiB   | +188% | 58.2GB/s   | 14.6GB/s  |
| 4 MiB   | +183% | 59.3GB/s   | 14.8GB/s  |
| 8 MiB   | +180% | 59.9GB/s   | 15.0GB/s  |
| 16 MiB  | +182% | 59.4GB/s   | 14.9GB/s  | L2, 12MB shared |
| 32 MiB  | +198% | 56.2GB/s   | 14.1GB/s  |
| 64 MiB  | +202% | 55.6GB/s   | 13.9GB/s  |
| 128 MiB | +176% | 60.6GB/s   | 15.2GB/s  |

**SIMD cached fill**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +27%  | 119.8GB/s  | 30.0GB/s  |
| 512 B   | +20%  | 127.3GB/s  | 31.8GB/s  |
| 1 KiB   | +18%  | 129.3GB/s  | 32.3GB/s  |
| 2 KiB   | +19%  | 128.8GB/s  | 32.2GB/s  |
| 4 KiB   | +18%  | 129.4GB/s  | 32.4GB/s  | page, 4 KiB |
| 8 KiB   | -     | 152.8GB/s  | 38.2GB/s  |
| 16 KiB  | +2%   | 149.8GB/s  | 37.5GB/s  |
| 32 KiB  | +15%  | 133.2GB/s  | 33.3GB/s  |
| 64 KiB  | +38%  | 110.3GB/s  | 27.6GB/s  |
| 128 KiB | +37%  | 111.3GB/s  | 27.8GB/s  |
| 256 KiB | +40%  | 109.4GB/s  | 27.3GB/s  |
| 512 KiB | +39%  | 110.3GB/s  | 27.6GB/s  |
| 1 MiB   | +45%  | 105.3GB/s  | 26.3GB/s  |
| 2 MiB   | +119% | 69.9GB/s   | 17.5GB/s  |
| 4 MiB   | +304% | 37.8GB/s   | 9.45GB/s  |
| 8 MiB   | +456% | 27.5GB/s   | 6.87GB/s  |
| 16 MiB  | +487% | 26.0GB/s   | 6.51GB/s  | L2, 12MB shared |
| 32 MiB  | +482% | 26.3GB/s   | 6.57GB/s  |
| 64 MiB  | +480% | 26.3GB/s   | 6.58GB/s  |
| 128 MiB | +456% | 27.5GB/s   | 6.87GB/s  |

**SIMD non-cached fill**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|--------|-----------|-----------|-------------|
| 256 B   | +1019% | 21.3GB/s  | 5.31GB/s  |
| 512 B   | +292%  | 60.7GB/s  | 15.2GB/s  |
| 1 KiB   | +231%  | 71.9GB/s  | 18.0GB/s  |
| 2 KiB   | +230%  | 72.1GB/s  | 18.0GB/s  |
| 4 KiB   | +233%  | 71.4GB/s  | 17.9GB/s  | page, 4 KiB |
| 8 KiB   | +326%  | 55.9GB/s  | 14.0GB/s  |
| 16 KiB  | +278%  | 62.9GB/s  | 15.7GB/s  |
| 32 KiB  | +283%  | 62.1GB/s  | 15.5GB/s  |
| 64 KiB  | +309%  | 58.1GB/s  | 14.5GB/s  |
| 128 KiB | +295%  | 60.2GB/s  | 15.1GB/s  |
| 256 KiB | +292%  | 60.7GB/s  | 15.2GB/s  |
| 512 KiB | +292%  | 60.7GB/s  | 15.2GB/s  |
| 1 MiB   | +291%  | 60.9GB/s  | 15.2GB/s  |
| 2 MiB   | +292%  | 60.7GB/s  | 15.2GB/s  |
| 4 MiB   | +10%   | 53.8GB/s  | 13.5GB/s  |
| 8 MiB   | +21%   | 49.0GB/s  | 12.3GB/s  |
| 16 MiB  | -      | 59.4GB/s  | 14.9GB/s  | L2, 12MB shared |
| 32 MiB  | +22%   | 48.8GB/s  | 12.2GB/s  |
| 64 MiB  | +4%    | 57.3GB/s  | 14.3GB/s  |
| 128 MiB | +2%    | 58.3GB/s  | 14.6GB/s  |

</details>

<details><summary><b>other, single thread</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +24% | 36.5GB/s  |
| 512 B   | +11% | 40.6GB/s  |
| 1 KiB   | +5%  | 43.2GB/s  |
| 2 KiB   | +2%  | 44.5GB/s  |
| 4 KiB   | -    | 45.2GB/s  | page, 4 KiB |
| 8 KiB   | +3%  | 44.0GB/s  |
| 16 KiB  | -    | 45.0GB/s  |
| 32 KiB  | +2%  | 44.3GB/s  |
| 64 KiB  | +27% | 35.4GB/s  |
| 128 KiB | +33% | 34.0GB/s  |
| 256 KiB | +32% | 34.1GB/s  |
| 512 KiB | +32% | 34.1GB/s  |
| 1 MiB   | +32% | 34.3GB/s  |
| 2 MiB   | +32% | 34.2GB/s  |
| 4 MiB   | +34% | 33.6GB/s  |
| 8 MiB   | +35% | 33.4GB/s  |
| 16 MiB  | +45% | 31.2GB/s  | L2, 12MB shared |
| 32 MiB  | +56% | 29.0GB/s  |
| 64 MiB  | +62% | 27.9GB/s  |
| 128 MiB | +62% | 27.8GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block | diff | bandwidth | comment |
|---------|------|-----------|-------------|
| 256 B   | +42% | 16.2GB/s  |
| 512 B   | +21% | 19.0GB/s  |
| 1 KiB   | +11% | 20.7GB/s  |
| 2 KiB   | +5%  | 21.8GB/s  |
| 4 KiB   | +2%  | 22.4GB/s  | page, 4 KiB |
| 8 KiB   | +1%  | 22.6GB/s  |
| 16 KiB  | +1%  | 22.8GB/s  |
| 32 KiB  | +1%  | 22.7GB/s  |
| 64 KiB  | -    | 22.8GB/s  |
| 128 KiB | -    | 22.9GB/s  |
| 256 KiB | -    | 22.8GB/s  |
| 512 KiB | -    | 22.9GB/s  |
| 1 MiB   | -    | 22.9GB/s  |
| 2 MiB   | -    | 22.9GB/s  |
| 4 MiB   | -    | 22.9GB/s  |
| 8 MiB   | -    | 22.8GB/s  |
| 16 MiB  | -    | 22.8GB/s  | L2, 12MB shared |
| 32 MiB  | +1%  | 22.8GB/s  |
| 64 MiB  | +1%  | 22.8GB/s  |
| 128 MiB | +1%  | 22.7GB/s  |

</details>

<details><summary><b>other, multithreading (4 threads, 4 cores)</b></summary>

**4x read, 2x write (SIMD fp32 sum)**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +22%  | 87.4GB/s   | 21.8GB/s  |
| 512 B   | +12%  | 95.4GB/s   | 23.8GB/s  |
| 1 KiB   | +14%  | 94.1GB/s   | 23.5GB/s  |
| 2 KiB   | -     | 106.9GB/s  | 26.7GB/s  |
| 4 KiB   | +1%   | 105.7GB/s  | 26.4GB/s  | page, 4 KiB |
| 8 KiB   | +2%   | 105.1GB/s  | 26.3GB/s  |
| 16 KiB  | +2%   | 104.6GB/s  | 26.2GB/s  |
| 32 KiB  | +4%   | 103.1GB/s  | 25.8GB/s  |
| 64 KiB  | +23%  | 86.9GB/s   | 21.7GB/s  |
| 128 KiB | +26%  | 84.7GB/s   | 21.2GB/s  |
| 256 KiB | +31%  | 81.7GB/s   | 20.4GB/s  |
| 512 KiB | +27%  | 84.0GB/s   | 21.0GB/s  |
| 1 MiB   | +24%  | 86.3GB/s   | 21.6GB/s  |
| 2 MiB   | +70%  | 62.8GB/s   | 15.7GB/s  |
| 4 MiB   | +173% | 39.1GB/s   | 9.77GB/s  |
| 8 MiB   | +266% | 29.2GB/s   | 7.29GB/s  |
| 16 MiB  | +312% | 26.0GB/s   | 6.49GB/s  | L2, 12MB shared |
| 32 MiB  | +328% | 25.0GB/s   | 6.25GB/s  |
| 64 MiB  | +351% | 23.7GB/s   | 5.93GB/s  |
| 128 MiB | +338% | 24.4GB/s   | 6.09GB/s  |

**read from cache with loop dependency (SIMD xor)**

| block | diff | bandwidth | bandwidth per thread | comment |
|---------|-------|------------|-----------|-------------|
| 256 B   | +28%  | 80.2GB/s   | 20.1GB/s  |
| 512 B   | +14%  | 90.2GB/s   | 22.5GB/s  |
| 1 KiB   | -     | 102.5GB/s  | 25.6GB/s  |
| 2 KiB   | +18%  | 86.9GB/s   | 21.7GB/s  |
| 4 KiB   | +35%  | 75.9GB/s   | 19.0GB/s  | page, 4 KiB |
| 8 KiB   | +48%  | 69.1GB/s   | 17.3GB/s  |
| 16 KiB  | +51%  | 68.0GB/s   | 17.0GB/s  |
| 32 KiB  | +50%  | 68.5GB/s   | 17.1GB/s  |
| 64 KiB  | +66%  | 61.8GB/s   | 15.5GB/s  |
| 128 KiB | +53%  | 66.8GB/s   | 16.7GB/s  |
| 256 KiB | +56%  | 65.7GB/s   | 16.4GB/s  |
| 512 KiB | +64%  | 62.5GB/s   | 15.6GB/s  |
| 1 MiB   | +50%  | 68.3GB/s   | 17.1GB/s  |
| 2 MiB   | +60%  | 64.2GB/s   | 16.0GB/s  |
| 4 MiB   | +60%  | 63.9GB/s   | 16.0GB/s  |
| 8 MiB   | +96%  | 52.4GB/s   | 13.1GB/s  |
| 16 MiB  | +107% | 49.6GB/s   | 12.4GB/s  | L2, 12MB shared |
| 32 MiB  | +120% | 46.6GB/s   | 11.6GB/s  |
| 64 MiB  | +121% | 46.4GB/s   | 11.6GB/s  |
| 128 MiB | +123% | 45.9GB/s   | 11.5GB/s  |

</details>
