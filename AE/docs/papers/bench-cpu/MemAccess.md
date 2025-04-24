
Variable block size test: [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_CacheSize.cpp)<br/>
Different memcpy implementations: [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/base/Perf_MemCopy.cpp)

* [AMD Ryzen 3900X](#AMD-Ryzen-3900X)
* [AMD Phenom II X4 945](#AMD-Phenom-II-X4-945)
* [AMD Ryzen 7 8745HS](#Ryzen-7-8745HS)
* [Intel i5 8250U](#Intel-i5-8250U)
* [Intel N150](#Intel-N150)
* [MediaTek Helio G96](#MediaTek-Helio-G96)
* [MediaTek Dimensity 7020](#MediaTek-Dimensity-7020)
* [MediaTek Dimensity 8200](#MediaTek-Dimensity-8200)
* [Snapdragon 888](#Snapdragon-888)
* [Snapdragon 439](#Snapdragon-439)
* [Apple Mac M1](#Apple-Mac-M1)
* [Samsung Exynos 7870](#Samsung-Exynos-7870)

temp
```
<details><summary><b>memcpy, single thread</b></summary>
</details>

<details><summary><b>memcpy, multithreading (4 threads bound to physical cores)</b></summary>
</details>

<details><summary><b>memset, single thread</b></summary>
</details>

<details><summary><b>memset, multithreading (4 threads bound to physical cores)</b></summary>
</details>
```

# AMD Ryzen 3900X

* Arch: Zen2
* Cache:
	- L1: 12x 32KB 8-way
	- L2: 12x 512KB 8-way
	- L3: 64MB: 4x CCX with 16MB 16-way, 4 slices of 4MB per CCX
	- write to L1: 32B/cy
	- read L1: 64B/cy
	- L2 to L1: 32B/cy
* Memory: DDR4, 3GHz, 24GB/s
* Theoretical performance:<br/>
  32B/cy write to L1 * 4.2GHz = 134.4 GB/s - max speed of memset<br/>
  64B/cy read from L1 * 4.2GHz = 268.8 GB/s - max speed of search<br/>

**Results**

| | range | single thread bandwidth (GB/s) | multithreading, per core bandwidth (GB/s) | comment |
|---|---|---|---|---|
| cached copy     | <= 16 KiB      | 130 | 95 - 115 |
| cached copy     | 32 KiB - 2 MiB |     | 45 - 65  |
| non-cached copy | >= 64 KiB      | 19  | 19.5     |
| non-cached copy | >= 8 KiB       | -   | 19.5     |
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
| 4 MiB   | 0.14 s   | +627% | +1%   | 18.8GB/s   |              | 
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
| 4 MiB   | 54.32 ms | +177%  | -     | 49.4GB/s   |              | 
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
| 4 MiB   | 0.14 s | +4%    | +1%  | 18.7GB/s  |              | 
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
| 4 MiB   | 0.55 s   | +2607% | +1%   | 19.6GB/s   |              | 
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
| 4 MiB   | 0.60 s   | +2481% | +937% | 17.8GB/s   |              | 
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
| 4 MiB   | 0.54 s | -     | -    | 19.7GB/s  |              | 
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
| 4 MiB   | 28.05 ms | +43%  | +1%   | 95.7GB/s   |              | 
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
| 4 MiB   | 28.07 ms | +45%  | +1%   | 95.6GB/s   |              | 
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
| 4 MiB   | 0.11 s | -     | -    | 23.7GB/s  |              | 
| 8 MiB   | 0.11 s | -     | -    | 23.7GB/s  |              | 
| 16 MiB  | 0.11 s | -     | -    | 23.7GB/s  |  L3, 16 MiB  | 
| 32 MiB  | 0.11 s | -     | -    | 23.7GB/s  |              | 
| 64 MiB  | 0.11 s | -     | -    | 23.7GB/s  |              | 
| 128 MiB | 0.11 s | -     | -    | 23.7GB/s  |              |

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
| 4 MiB   | 0.38 s   | +1798% | +970% | 28.0GB/s   |              | 
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
| 4 MiB   | 0.42 s   | +1799% | +1138% | 25.6GB/s   |              | 
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
| 4 MiB   | 0.45 s | -     | -    | 23.8GB/s  |              | 
| 8 MiB   | 0.45 s | -     | -    | 23.8GB/s  |              | 
| 16 MiB  | 0.45 s | -     | -    | 23.7GB/s  |  L3, 16 MiB  | 
| 32 MiB  | 0.45 s | -     | -    | 23.8GB/s  |              | 
| 64 MiB  | 0.45 s | -     | -    | 23.8GB/s  |              | 
| 128 MiB | 0.45 s | -     | -    | 23.7GB/s  |              |

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
	
<details>

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

# MediaTek Helio G96

* Device: Realme 8i
* block size: 1GB
* Memory: 4GB, LPDDR4X, DC 16bit, 2133 MHz, **17.07** GB/s

<details>
</details>


# Snapdragon 888

* Device: Asus ROG Phone 5
* Memory: 8 GB, LPDDR5-6400, QC 16bit, 3200MHz, 51.2 GB/s

<details>
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

<details>

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
  512 B  : 0.67 s    +1453.5%   - 4.00GB/s
  256 B  : 0.70 s    +1516.8%   - 3.85GB/s
  1 KiB  : 0.34 s    +678.5%    - 7.99GB/s
  2 KiB  : 0.17 s    +291.0%    - 15.9GB/s
  4 KiB  : 0.11 s    +146.8%    - 25.2GB/s
  8 KiB  : 59.67 ms  +38.3%     - 45.0GB/s
  16 KiB : 43.15 ms             - 62.2GB/s - page size
  32 KiB : 62.87 ms  +45.7%     - 42.7GB/s
  64 KiB : 73.91 ms  +71.3%     - 36.3GB/s
  128 KiB: 73.26 ms  +69.8%     - 36.6GB/s
  256 KiB: 76.67 ms  +77.7%     - 35.0GB/s
  512 KiB: 74.67 ms  +73.0%     - 35.9GB/s
  1 MiB  : 74.90 ms  +73.6%     - 35.8GB/s
  2 MiB  : 74.65 ms  +73.0%     - 36.0GB/s
  4 MiB  : 76.74 ms  +77.8%     - 35.0GB/s
  8 MiB  : 75.98 ms  +76.1%     - 35.3GB/s
  16 MiB : 74.32 ms  +72.2%     - 36.1GB/s
  32 MiB : 74.17 ms  +71.9%     - 36.2GB/s
  64 MiB : 76.24 ms  +76.7%     - 35.2GB/s
  128 MiB: 75.77 ms  +75.6%     - 35.4GB/s

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

SIMD non-cached fill, single thread:
  256 B  : 1.98 s  +1099.2%   - 1.35GB/s
  512 B  : 1.01 s  +512.0%    - 2.65GB/s
  1 KiB  : 0.51 s  +205.6%    - 5.31GB/s
  2 KiB  : 0.19 s  +14.7%     - 14.1GB/s
  4 KiB  : 0.17 s  +2.4%      - 15.8GB/s
  8 KiB  : 0.17 s  +1.0%      - 16.1GB/s
  16 KiB : 0.17 s  +0.3%      - 16.2GB/s
  32 KiB : 0.17 s             - 16.2GB/s
  64 KiB : 0.17 s  +0.7%      - 16.1GB/s
  128 KiB: 0.17 s  +0.6%      - 16.1GB/s
  256 KiB: 0.17 s  +1.1%      - 16.1GB/s
  512 KiB: 0.17 s  +0.5%      - 16.1GB/s
  1 MiB  : 0.17 s  +2.3%      - 15.9GB/s
  2 MiB  : 0.17 s  +2.2%      - 15.9GB/s
  4 MiB  : 0.17 s  +1.5%      - 16.0GB/s
  8 MiB  : 0.17 s  +2.7%      - 15.8GB/s
  16 MiB : 0.17 s  +2.3%      - 15.9GB/s
  32 MiB : 0.18 s  +7.1%      - 15.1GB/s
  64 MiB : 0.17 s  +3.5%      - 15.7GB/s
  128 MiB: 0.18 s  +7.8%      - 15.0GB/s

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

</details>

# Intel N150

* CPU Clock: 3.6 GHz
* L1: 32B/cy, 115 GB/s
* Cache:
	- L1D: 64KB per 2 cores, 32KB per core
	- L2: 2MB shared
	- L3: 6MB shared
* Memory: 16GB DDR4-3200, single channel, 20GB/s

<details>

## memset

* cached fill: 256B - 4KiB on 110 GB/s
* non-cached fill: >= 16 KiB on 20 GB/s

```
memset, single thread:
  256 B  : 48.39 ms  +81.7%   - 55.5GB/s
  512 B  : 31.84 ms  +19.5%   - 84.3GB/s
  1 KiB  : 32.10 ms  +20.5%   - 83.6GB/s
  2 KiB  : 28.50 ms  +7.0%    - 94.2GB/s
  4 KiB  : 26.64 ms           - 100.8GB/s  - page size
  8 KiB  : 50.24 ms  +88.6%   - 53.4GB/s
  16 KiB : 56.64 ms  +112.6%  - 47.4GB/s
  32 KiB : 60.59 ms  +127.5%  - 44.3GB/s  - L1 per core
  64 KiB : 76.95 ms  +188.9%  - 34.9GB/s  - L1 per 2 cores
  128 KiB: 82.12 ms  +208.3%  - 32.7GB/s
  256 KiB: 83.89 ms  +214.9%  - 32.0GB/s
  512 KiB: 87.19 ms  +227.3%  - 30.8GB/s
  1 MiB  : 84.73 ms  +218.1%  - 31.7GB/s
  2 MiB  : 88.83 ms  +233.5%  - 30.2GB/s  - L2
  4 MiB  : 0.10 s  +286.0%    - 26.1GB/s
  8 MiB  : 0.20 s  +641.1%    - 13.6GB/s  - L3 6MB
  16 MiB : 0.32 s  +1088.1%   - 8.48GB/s
  32 MiB : 0.31 s  +1081.1%   - 8.53GB/s
  64 MiB : 0.31 s  +1072.2%   - 8.60GB/s
  128 MiB: 0.32 s  +1083.4%   - 8.52GB/s
  
SIMD cached fill, single thread:
  256 B  : 23.96 ms  +0.6%    - 112.0GB/s  <<
  512 B  : 23.81 ms           - 112.8GB/s  << faster than memset
  1 KiB  : 23.89 ms  +0.3%    - 112.4GB/s  <<
  2 KiB  : 24.31 ms  +2.1%    - 110.4GB/s
  4 KiB  : 24.25 ms  +1.9%    - 110.7GB/s  - page size
  8 KiB  : 47.03 ms  +97.5%   - 57.1GB/s
  16 KiB : 56.18 ms  +136.0%  - 47.8GB/s
  32 KiB : 61.21 ms  +157.1%  - 43.9GB/s  - L1 per core
  64 KiB : 67.93 ms  +185.3%  - 39.5GB/s  - L1 per 2 cores
  128 KiB: 75.76 ms  +218.2%  - 35.4GB/s
  256 KiB: 81.70 ms  +243.2%  - 32.9GB/s
  512 KiB: 86.20 ms  +262.1%  - 31.1GB/s
  1 MiB  : 85.67 ms  +259.8%  - 31.3GB/s
  2 MiB  : 87.92 ms  +269.3%  - 30.5GB/s  - L2
  4 MiB  : 0.10 s  +337.5%    - 25.8GB/s
  8 MiB  : 0.20 s  +731.1%    - 13.6GB/s  - L3 6MB
  16 MiB : 0.31 s  +1220.2%   - 8.54GB/s
  32 MiB : 0.34 s  +1347.5%   - 7.79GB/s
  64 MiB : 0.31 s  +1210.7%   - 8.60GB/s
  128 MiB: 0.31 s  +1207.5%   - 8.62GB/s
  
SIMD non-cached fill, single thread:
  256 B  : 0.70 s  +426.8%    - 3.84GB/s
  512 B  : 0.42 s  +214.9%    - 6.43GB/s
  1 KiB  : 0.28 s  +107.8%    - 9.74GB/s
  2 KiB  : 0.20 s  +51.0%     - 13.4GB/s
  4 KiB  : 0.17 s  +28.7%     - 15.7GB/s  - page size
  8 KiB  : 0.15 s  +15.0%     - 17.6GB/s
  16 KiB : 0.14 s  +7.4%      - 18.9GB/s
  32 KiB : 0.14 s  +6.8%      - 19.0GB/s  - L1 per core
  64 KiB : 0.14 s  +3.1%      - 19.6GB/s  - L1 per 2 cores
  128 KiB: 0.13 s  +0.7%      - 20.1GB/s
  256 KiB: 0.14 s  +2.7%      - 19.7GB/s
  512 KiB: 0.13 s  +1.2%      - 20.0GB/s
  1 MiB  : 0.13 s             - 20.2GB/s
  2 MiB  : 0.13 s  +1.0%      - 20.0GB/s  - L2
  4 MiB  : 0.13 s  +0.9%      - 20.1GB/s
  8 MiB  : 0.13 s  +0.1%      - 20.2GB/s  - L3 6MB  <<
  16 MiB : 0.13 s  +0.6%      - 20.1GB/s            <<
  32 MiB : 0.13 s  +0.6%      - 20.1GB/s            << faster than memset
  64 MiB : 0.13 s  +0.9%      - 20.1GB/s            <<
  128 MiB: 0.13 s  +0.2%      - 20.2GB/s            << 
```

## memcpy

* cached copy: 256B - 16KiB on 60-80 GB/s
* non-cached copy: >= 128 KiB on 10 GB/s

```
memcpy, single thread:
  256 B  : 54.49 ms  +88.5%     - 49.3GB/s
  512 B  : 43.56 ms  +50.6%     - 61.6GB/s
  1 KiB  : 35.86 ms  +24.0%     - 74.8GB/s
  2 KiB  : 32.16 ms  +11.2%     - 83.5GB/s
  4 KiB  : 29.84 ms  +3.2%      - 90.0GB/s  - page size
  8 KiB  : 28.91 ms             - 92.8GB/s
  16 KiB : 51.97 ms  +79.7%     - 51.7GB/s
  32 KiB : 83.65 ms  +189.3%    - 32.1GB/s  - L1 per core
  64 KiB : 88.88 ms  +207.4%    - 30.2GB/s  - L1 per 2 cores
  128 KiB: 85.90 ms  +197.1%    - 31.2GB/s
  256 KiB: 85.41 ms  +195.4%    - 31.4GB/s
  512 KiB: 85.94 ms  +197.2%    - 31.2GB/s
  1 MiB  : 0.12 s    +305.9%    - 22.9GB/s
  2 MiB  : 0.27 s    +847.5%    - 9.80GB/s  - L2
  4 MiB  : 0.27 s    +822.9%    - 10.1GB/s
  8 MiB  : 0.27 s    +836.1%    - 9.92GB/s  - L3 6MB
  16 MiB : 0.27 s    +833.2%    - 9.95GB/s
  32 MiB : 0.27 s    +831.3%    - 9.97GB/s
  64 MiB : 0.27 s    +842.4%    - 9.85GB/s
  128 MiB: 0.27 s    +835.4%    - 9.92GB/s
  
SIMD cached copy, single thread:
  256 B  : 44.07 ms  +36.9%     - 60.9GB/s  << faster than memcpy
  512 B  : 36.87 ms  +14.6%     - 72.8GB/s  <<
  1 KiB  : 34.54 ms  +7.3%      - 77.7GB/s
  2 KiB  : 32.95 ms  +2.4%      - 81.5GB/s
  4 KiB  : 32.18 ms             - 83.4GB/s  - page size
  8 KiB  : 32.68 ms  +1.6%      - 82.1GB/s
  16 KiB : 34.63 ms  +7.6%      - 77.5GB/s
  32 KiB : 84.39 ms  +162.2%    - 31.8GB/s  - L1 per core
  64 KiB : 83.94 ms  +160.8%    - 32.0GB/s  - L1 per 2 cores
  128 KiB: 83.94 ms  +160.8%    - 32.0GB/s
  256 KiB: 83.29 ms  +158.8%    - 32.2GB/s
  512 KiB: 86.07 ms  +167.5%    - 31.2GB/s
  1 MiB  : 0.12 s    +257.8%    - 23.3GB/s
  2 MiB  : 0.14 s    +331.9%    - 19.3GB/s  - L2   << faster than memcpy
  4 MiB  : 0.30 s    +824.9%    - 9.02GB/s
  8 MiB  : 0.43 s    +1248.2%   - 6.19GB/s  - L3 6MB
  16 MiB : 0.43 s    +1246.5%   - 6.19GB/s
  32 MiB : 0.43 s    +1242.0%   - 6.22GB/s
  64 MiB : 0.43 s    +1241.7%   - 6.22GB/s
  128 MiB: 0.43 s    +1239.8%   - 6.23GB/s
  
SIMD non-cached copy, single thread:
  256 B  : 1.59 s  +505.0%    - 1.69GB/s
  512 B  : 0.95 s  +261.5%    - 2.83GB/s
  1 KiB  : 0.58 s  +122.9%    - 4.59GB/s
  2 KiB  : 0.53 s  +100.6%    - 5.11GB/s
  4 KiB  : 0.51 s  +94.7%     - 5.26GB/s
  8 KiB  : 0.50 s  +90.9%     - 5.37GB/s
  16 KiB : 0.49 s  +87.8%     - 5.46GB/s
  32 KiB : 0.50 s  +89.8%     - 5.40GB/s
  64 KiB : 0.49 s  +88.0%     - 5.45GB/s
  128 KiB: 0.26 s  +1.1%      - 10.1GB/s
  256 KiB: 0.26 s  +1.1%      - 10.1GB/s
  512 KiB: 0.27 s  +2.7%      - 9.98GB/s
  1 MiB  : 0.27 s  +1.3%      - 10.1GB/s
  2 MiB  : 0.26 s             - 10.2GB/s  - L2
  4 MiB  : 0.27 s  +1.5%      - 10.1GB/s
  8 MiB  : 0.27 s  +2.3%      - 10.0GB/s
  16 MiB : 0.27 s  +2.3%      - 10.0GB/s
  32 MiB : 0.27 s  +2.7%      - 9.98GB/s
  64 MiB : 0.27 s  +3.3%      - 9.91GB/s
  128 MiB: 0.27 s  +3.2%      - 9.93GB/s
  
SIMD cached load, non-cached store, single thread:
  256 B  : 1.54 s  +485.5%    - 1.74GB/s
  512 B  : 0.79 s  +197.8%    - 3.42GB/s
  1 KiB  : 0.58 s  +119.7%    - 4.63GB/s
  2 KiB  : 0.53 s  +99.6%     - 5.10GB/s
  4 KiB  : 0.50 s  +89.4%     - 5.38GB/s
  8 KiB  : 0.49 s  +87.5%     - 5.43GB/s
  16 KiB : 0.49 s  +84.4%     - 5.52GB/s
  32 KiB : 0.49 s  +85.1%     - 5.50GB/s
  64 KiB : 0.49 s  +84.5%     - 5.52GB/s
  128 KiB: 0.27 s  +1.2%      - 10.1GB/s
  256 KiB: 0.27 s  +1.7%      - 10.0GB/s
  512 KiB: 0.27 s  +0.9%      - 10.1GB/s
  1 MiB  : 0.27 s  +1.5%      - 10.0GB/s
  2 MiB  : 0.26 s             - 10.2GB/s - L2
  4 MiB  : 0.26 s  +0.3%      - 10.2GB/s
  8 MiB  : 0.27 s  +4.1%      - 9.78GB/s
  32 MiB : 0.27 s  +0.5%      - 10.1GB/s
  16 MiB : 0.27 s  +1.9%      - 9.99GB/s
  64 MiB : 0.27 s  +3.2%      - 9.87GB/s
  128 MiB: 0.27 s  +0.5%      - 10.1GB/s
```

## memset, multithreading

* cached fill: <= 16 KiB on 120 - 300 GB/s
* non-cached fill: >= 1 KiB on 22.5 GB/s

```
memset, multithreading, 4T:
  256 B  : 76.98 ms  +84.8%   - 139.5GB/s
  512 B  : 41.66 ms           - 257.7GB/s
  1 KiB  : 78.57 ms  +88.6%   - 136.7GB/s
  2 KiB  : 68.27 ms  +63.9%   - 157.3GB/s
  4 KiB  : 76.16 ms  +82.8%   - 141.0GB/s - page size
  8 KiB  : 90.49 ms  +117.2%  - 118.7GB/s
  16 KiB : 0.15 s  +268.4%    - 70.0GB/s
  32 KiB : 0.21 s  +412.8%    - 50.3GB/s  - L1 per core
  64 KiB : 0.30 s  +629.6%    - 35.3GB/s  - L1 per 2 cores
  128 KiB: 0.33 s  +683.8%    - 32.9GB/s
  256 KiB: 0.34 s  +726.9%    - 31.2GB/s
  512 KiB: 0.43 s  +937.4%    - 24.8GB/s
  1 MiB  : 0.45 s  +991.1%    - 23.6GB/s
  2 MiB  : 1.11 s  +2561.3%   - 9.68GB/s  - L2
  4 MiB  : 1.13 s  +2619.7%   - 9.48GB/s
  8 MiB  : 1.14 s  +2647.4%   - 9.38GB/s  - L3 6MB
  16 MiB : 1.11 s  +2573.0%   - 9.64GB/s
  32 MiB : 1.17 s  +2697.5%   - 9.21GB/s
  64 MiB : 1.15 s  +2664.4%   - 9.32GB/s
  128 MiB: 1.11 s  +2553.4%   - 9.71GB/s
  
SIMD cached fill, multithreading, 4T:
  512 B  : 34.89 ms           - 307.7GB/s << faster than memset (x3 faster than single thread)
  256 B  : 40.39 ms  +15.8%   - 265.8GB/s
  1 KiB  : 89.63 ms  +156.9%  - 119.8GB/s
  2 KiB  : 57.03 ms  +63.4%   - 188.3GB/s
  4 KiB  : 52.63 ms  +50.9%   - 204.0GB/s - page size (x2 faster than single thread)
  8 KiB  : 73.44 ms  +110.5%  - 146.2GB/s
  16 KiB : 89.09 ms  +155.3%  - 120.5GB/s
  32 KiB : 0.21 s  +496.3%    - 51.6GB/s  - L1 per core (+18% to single thread)
  64 KiB : 0.28 s  +695.8%    - 38.7GB/s  - L1 per 2 cores
  128 KiB: 0.31 s  +778.4%    - 35.0GB/s
  256 KiB: 0.34 s  +869.0%    - 31.8GB/s
  512 KiB: 0.42 s  +1101.1%   - 25.6GB/s
  1 MiB  : 0.44 s  +1154.4%   - 24.5GB/s
  2 MiB  : 1.08 s  +3008.1%   - 9.90GB/s  - L2
  4 MiB  : 1.10 s  +3045.3%   - 9.78GB/s
  8 MiB  : 1.13 s  +3150.8%   - 9.47GB/s  - L3 6MB
  16 MiB : 1.10 s  +3057.4%   - 9.75GB/s
  32 MiB : 1.10 s  +3055.2%   - 9.75GB/s
  64 MiB : 1.10 s  +3060.6%   - 9.74GB/s
  128 MiB: 1.10 s  +3062.0%   - 9.73GB/s  (+13% to single thread)

SIMD non-cached fill, multithreading, 4T:
  256 B  : 0.91 s  +117.6%    - 11.8GB/s
  512 B  : 0.58 s  +37.8%     - 18.7GB/s
  1 KiB  : 0.42 s             - 25.7GB/s
  2 KiB  : 0.45 s  +7.9%      - 23.8GB/s
  4 KiB  : 0.47 s  +11.4%     - 23.1GB/s - page size
  8 KiB  : 0.46 s  +11.2%     - 23.1GB/s
  16 KiB : 0.47 s  +11.7%     - 23.0GB/s
  32 KiB : 0.47 s  +12.3%     - 22.9GB/s - L1 per core
  64 KiB : 0.47 s  +12.7%     - 22.8GB/s - L1 per 2 cores
  128 KiB: 0.47 s  +13.2%     - 22.7GB/s
  256 KiB: 0.48 s  +13.9%     - 22.6GB/s
  512 KiB: 0.47 s  +13.2%     - 22.7GB/s
  1 MiB  : 0.47 s  +13.6%     - 22.6GB/s
  2 MiB  : 0.48 s  +14.4%     - 22.5GB/s - L2      <<
  4 MiB  : 0.48 s  +13.9%     - 22.6GB/s           <<
  8 MiB  : 0.47 s  +13.5%     - 22.6GB/s - L3 6MB  <<
  16 MiB : 0.48 s  +13.9%     - 22.6GB/s           << faster than memset
  32 MiB : 0.47 s  +13.4%     - 22.7GB/s           <<
  64 MiB : 0.48 s  +14.2%     - 22.5GB/s           <<
  128 MiB: 0.48 s  +14.4%     - 22.5GB/s           << (+11% to single thread)
```

## memcpy, multithreading

* cached copy: 256B - 16KiB on 200-240 GB/s
* non-cached copy:  >= 1 KiB on 10 GB/s

```
memcpy, multithreading, 4T:
  256 B  : 74.27 ms  +78.8%   - 144.6GB/s
  512 B  : 57.86 ms  +39.3%   - 185.6GB/s
  1 KiB  : 49.72 ms  +19.7%   - 216.0GB/s
  2 KiB  : 44.59 ms  +7.4%    - 240.8GB/s
  4 KiB  : 41.53 ms           - 258.5GB/s  - page size
  8 KiB  : 41.97 ms  +1.1%    - 255.8GB/s
  16 KiB : 69.15 ms  +66.5%   - 155.3GB/s
  32 KiB : 0.36 s  +756.7%    - 30.2GB/s   - L1 per core
  64 KiB : 0.36 s  +763.5%    - 29.9GB/s   - L1 per 2 cores
  128 KiB: 0.36 s  +758.4%    - 30.1GB/s
  256 KiB: 0.43 s  +938.8%    - 24.9GB/s
  512 KiB: 0.55 s  +1217.6%   - 19.6GB/s
  1 MiB  : 0.63 s  +1413.4%   - 17.1GB/s
  2 MiB  : 1.01 s  +2340.0%   - 10.6GB/s   - L2
  4 MiB  : 1.02 s  +2357.4%   - 10.5GB/s
  8 MiB  : 1.02 s  +2351.3%   - 10.5GB/s   - L3 6MB
  16 MiB : 1.02 s  +2350.6%   - 10.5GB/s
  32 MiB : 1.02 s  +2349.3%   - 10.6GB/s
  64 MiB : 1.02 s  +2355.9%   - 10.5GB/s
  128 MiB: 1.02 s  +2349.3%   - 10.6GB/s
  
SIMD cached copy, multithreading, 4T:
  256 B  : 58.60 ms  +32.5%   - 183.2GB/s  << faster than memcpy (x3 faster than single thread)
  512 B  : 53.25 ms  +20.4%   - 201.6GB/s  <<
  1 KiB  : 49.84 ms  +12.7%   - 215.5GB/s
  2 KiB  : 48.37 ms  +9.3%    - 222.0GB/s
  4 KiB  : 48.11 ms  +8.8%    - 223.2GB/s  - page size (x2.7 faster than single thread)
  8 KiB  : 44.24 ms           - 242.7GB/s
  16 KiB : 51.51 ms  +16.4%   - 208.5GB/s  (x2.7)
  32 KiB : 0.36 s  +709.1%    - 30.0GB/s   - L1 per core (+6%)
  64 KiB : 0.36 s  +724.8%    - 29.4GB/s   - L1 per 2 cores
  128 KiB: 0.36 s  +713.3%    - 29.8GB/s
  256 KiB: 0.39 s  +781.3%    - 27.5GB/s
  512 KiB: 0.55 s  +1143.3%   - 19.5GB/s
  1 MiB  : 0.64 s  +1337.0%   - 16.9GB/s
  2 MiB  : 1.57 s  +3449.4%   - 6.84GB/s   - L2
  4 MiB  : 1.58 s  +3472.7%   - 6.79GB/s
  8 MiB  : 1.58 s  +3480.5%   - 6.78GB/s   - L3 6MB
  16 MiB : 1.59 s  +3494.1%   - 6.75GB/s
  64 MiB : 1.58 s  +3476.5%   - 6.79GB/s
  32 MiB : 1.58 s  +3477.0%   - 6.79GB/s
  128 MiB: 1.58 s  +3479.7%   - 6.78GB/s  (+9%)
  
SIMD non-cached copy, multithreading, 4T:
  256 B  : 1.90 s  +98.4%     - 5.66GB/s
  512 B  : 1.11 s  +16.3%     - 9.66GB/s
  1 KiB  : 0.96 s             - 11.2GB/s
  2 KiB  : 1.00 s  +4.5%      - 10.7GB/s
  4 KiB  : 1.06 s  +10.5%     - 10.2GB/s  - page size
  8 KiB  : 1.07 s  +11.5%     - 10.1GB/s
  16 KiB : 1.08 s  +12.5%     - 9.99GB/s
  32 KiB : 1.00 s  +4.8%      - 10.7GB/s  - L1 per core
  64 KiB : 1.00 s  +4.5%      - 10.7GB/s  - L1 per 2 cores
  128 KiB: 0.99 s  +3.9%      - 10.8GB/s
  256 KiB: 1.00 s  +4.1%      - 10.8GB/s
  512 KiB: 0.99 s  +3.9%      - 10.8GB/s
  1 MiB  : 1.00 s  +4.7%      - 10.7GB/s
  2 MiB  : 1.01 s  +5.6%      - 10.6GB/s  - L2  (+4%)
  4 MiB  : 1.01 s  +5.8%      - 10.6GB/s
  8 MiB  : 1.01 s  +5.9%      - 10.6GB/s  - L3 6MB
  16 MiB : 1.04 s  +8.4%      - 10.4GB/s
  32 MiB : 1.01 s  +6.0%      - 10.6GB/s
  64 MiB : 1.01 s  +5.9%      - 10.6GB/s
  128 MiB: 1.01 s  +6.0%      - 10.6GB/s
  
SIMD cached load, non-cached store, multithreading, 4T:
  256 B  : 1.89 s  +94.9%     - 5.67GB/s 
  512 B  : 1.10 s  +12.9%     - 9.79GB/s
  1 KiB  : 0.97 s             - 11.0GB/s
  2 KiB  : 1.07 s  +9.9%      - 10.1GB/s
  4 KiB  : 1.36 s  +40.4%     - 7.87GB/s  - page size
  8 KiB  : 1.39 s  +43.0%     - 7.73GB/s
  16 KiB : 1.17 s  +20.8%     - 9.14GB/s
  32 KiB : 1.06 s  +8.9%      - 10.1GB/s  - L1 per core
  64 KiB : 1.08 s  +11.0%     - 9.95GB/s  - L1 per 2 cores
  128 KiB: 1.08 s  +11.0%     - 9.96GB/s
  256 KiB: 1.04 s  +6.9%      - 10.3GB/s
  512 KiB: 1.05 s  +8.5%      - 10.2GB/s
  1 MiB  : 1.03 s  +5.6%      - 10.5GB/s
  2 MiB  : 1.10 s  +12.7%     - 9.80GB/s  - L2
  4 MiB  : 1.12 s  +15.7%     - 9.55GB/s
  8 MiB  : 1.12 s  +14.7%     - 9.63GB/s  - L3 6MB
  16 MiB : 1.12 s  +15.1%     - 9.59GB/s
  32 MiB : 1.11 s  +13.7%     - 9.71GB/s
  64 MiB : 1.10 s  +13.1%     - 9.77GB/s
  128 MiB: 1.10 s  +13.3%     - 9.76GB/s
```

</details>


# Samsung Exynos 7870

* Device: Samsung J7 Neo
* Memory: 2GB, LPDDR3, DC 32bit, 933MHz, 14.9GB/s

<details>
</details>


# MediaTek Dimensity 8200

* Device: Infinix GT 20 Pro
* Memory: 8GB, LPDDR5-6400, 3200 MHz, QC 16bit, 51.2 GB/s

<details>
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

memcpy:
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
| 4 MiB   | 43.61 ms | +141% | -     | 61.6GB/s   |             | 
| 8 MiB   | 71.57 ms | +296% | +64%  | 37.5GB/s   |             | 
| 16 MiB  | 0.16 s   | +778% | +122% | 16.9GB/s   | L3, 16 MiB  | 
| 32 MiB  | 0.16 s   | +782% | -     | 16.8GB/s   |             | 
| 64 MiB  | 0.16 s   | +800% | +2%   | 16.5GB/s   |             | 
| 128 MiB | 0.16 s   | +803% | -     | 16.4GB/s   |             |

SIMD cached copy:
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
| 4 MiB   | 43.84 ms | +148% | -    | 61.2GB/s   |             | 
| 8 MiB   | 72.02 ms | +308% | +64% | 37.3GB/s   |             | 
| 16 MiB  | 0.11 s   | +527% | +54% | 24.2GB/s   | L3, 16 MiB  | 
| 32 MiB  | 0.13 s   | +664% | +22% | 19.9GB/s   |             | 
| 64 MiB  | 0.15 s   | +739% | +10% | 18.1GB/s   |             | 
| 128 MiB | 0.15 s   | +742% | -    | 18.0GB/s   |             |

SIMD non-cached copy,:
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
| 4 MiB   | 91.23 ms | -      | -    | 29.4GB/s   |             | 
| 8 MiB   | 95.45 ms | +5%    | +5%  | 28.1GB/s   |             | 
| 16 MiB  | 0.10 s   | +10%   | +5%  | 26.7GB/s   | L3, 16 MiB  | 
| 32 MiB  | 0.10 s   | +11%   | +1%  | 26.6GB/s   |             | 
| 64 MiB  | 0.10 s   | +12%   | +1%  | 26.2GB/s   |             | 
| 128 MiB | 0.10 s   | +12%   | -    | 26.3GB/s   |             |

</details>
<details><summary><b>memcpy, multithreading (4 threads bound to physical cores)</b></summary>

memcpy:
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
| 4 MiB   | 0.34 s   | +1126% | +267% | 31.7GB/s   | 7.94GB/s  |             | 
| 8 MiB   | 0.49 s   | +1669% | +44%  | 22.0GB/s   | 5.50GB/s  |             | 
| 16 MiB  | 0.37 s   | +1225% | +25%  | 29.4GB/s   | 7.34GB/s  | L3, 16 MiB  | 
| 32 MiB  | 0.37 s   | +1230% | -     | 29.3GB/s   | 7.32GB/s  |             | 
| 64 MiB  | 0.37 s   | +1224% | -     | 29.4GB/s   | 7.35GB/s  |             | 
| 128 MiB | 0.37 s   | +1231% | +1%   | 29.2GB/s   | 7.31GB/s  |             |

SIMD cached copy:
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
| 4 MiB   | 0.34 s   | +1079% | +260% | 31.9GB/s   | 7.98GB/s  |             | 
| 8 MiB   | 0.48 s   | +1580% | +43%  | 22.4GB/s   | 5.60GB/s  |             | 
| 16 MiB  | 0.57 s   | +1890% | +18%  | 18.9GB/s   | 4.73GB/s  | L3, 16 MiB  | 
| 32 MiB  | 0.57 s   | +1895% | -     | 18.9GB/s   | 4.72GB/s  |             | 
| 64 MiB  | 0.57 s   | +1898% | -     | 18.8GB/s   | 4.71GB/s  |             | 
| 128 MiB | 0.57 s   | +1897% | -     | 18.8GB/s   | 4.71GB/s  |             |

SIMD non-cached copy:
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
| 4 MiB   | 0.35 s  | -      | +75% | 31.1GB/s  | 7.77GB/s   |             | 
| 8 MiB   | 0.34 s  | -      | -    | 31.2GB/s  | 7.81GB/s   |             | 
| 16 MiB  | 0.35 s  | +1%    | +1%  | 31.1GB/s  | 7.76GB/s   | L3, 16 MiB  | 
| 32 MiB  | 0.35 s  | +2%    | +2%  | 30.5GB/s  | 7.63GB/s   |             | 
| 64 MiB  | 0.35 s  | +1%    | +1%  | 30.9GB/s  | 7.72GB/s   |             | 
| 128 MiB | 0.35 s  | +1%    | -    | 30.9GB/s  | 7.72GB/s   |             |

</details>
<details><summary><b>memset, single thread</b></summary>

memset:
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
| 4 MiB   | 20.93 ms | +18%  | +1%  | 128.3GB/s  |             | 
| 8 MiB   | 21.62 ms | +22%  | +3%  | 124.2GB/s  |             | 
| 16 MiB  | 39.09 ms | +120% | +81% | 68.7GB/s   | L3, 16 MiB  | 
| 32 MiB  | 64.42 ms | +263% | +65% | 41.7GB/s   |             | 
| 64 MiB  | 78.87 ms | +344% | +22% | 34.0GB/s   |             | 
| 128 MiB | 87.76 ms | +395% | +11% | 30.6GB/s   |             |

SIMD cached fill:
| block | time | diff | delta | bandwidth | comment |
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
| 4 MiB   | 22.48 ms | +28%  | +1%  | 119.4GB/s  |             | 
| 8 MiB   | 22.98 ms | +31%  | +2%  | 116.8GB/s  |             | 
| 16 MiB  | 39.70 ms | +126% | +73% | 67.6GB/s   | L3, 16 MiB  | 
| 32 MiB  | 65.35 ms | +271% | +65% | 41.1GB/s   |             | 
| 64 MiB  | 82.27 ms | +368% | +26% | 32.6GB/s   |             | 
| 128 MiB | 93.34 ms | +431% | +13% | 28.8GB/s   |             |

SIMD non-cached fill:
| block | time | diff | delta | bandwidth | comment |
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
| 4 MiB   | 34.47 ms | -      | -    | 77.9GB/s  |             | 
| 8 MiB   | 34.40 ms | -      | -    | 78.0GB/s  |             | 
| 16 MiB  | 35.49 ms | +3%    | +3%  | 75.6GB/s  | L3, 16 MiB  | 
| 32 MiB  | 34.58 ms | +1%    | +3%  | 77.6GB/s  |             | 
| 64 MiB  | 34.55 ms | -      | -    | 77.7GB/s  |             | 
| 128 MiB | 34.49 ms | -      | -    | 77.8GB/s  |             |

</details>
<details><summary><b>memset, multithreading (4 threads bound to physical cores)</b></summary>

memset:
| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
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
| 4 MiB   | 0.23 s   | +707%  | +347% | 47.3GB/s   | 11.8GB/s  |             | 
| 8 MiB   | 0.32 s   | +1029% | +40%  | 33.8GB/s   | 8.45GB/s  |             | 
| 16 MiB  | 0.36 s   | +1177% | +13%  | 29.9GB/s   | 7.47GB/s  | L3, 16 MiB  | 
| 32 MiB  | 0.38 s   | +1250% | +6%   | 28.3GB/s   | 7.07GB/s  |             | 
| 64 MiB  | 0.38 s   | +1264% | +1%   | 28.0GB/s   | 6.99GB/s  |             | 
| 128 MiB | 0.38 s   | +1266% | -     | 27.9GB/s   | 6.98GB/s  |             |

SIMD cached fill:
| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
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
| 4 MiB   | 0.22 s   | +755%  | +299% | 47.8GB/s   | 12.0GB/s   |             | 
| 8 MiB   | 0.30 s   | +1045% | +34%  | 35.7GB/s   | 8.92GB/s   |             | 
| 16 MiB  | 0.35 s   | +1234% | +16%  | 30.6GB/s   | 7.66GB/s   | L3, 16 MiB  | 
| 32 MiB  | 0.37 s   | +1320% | +6%   | 28.8GB/s   | 7.20GB/s   |             | 
| 64 MiB  | 0.38 s   | +1348% | +2%   | 28.2GB/s   | 7.05GB/s   |             | 
| 128 MiB | 0.38 s   | +1350% | -     | 28.2GB/s   | 7.04GB/s   |             |

SIMD non-cached fill:
| block | time | diff | delta | bandwidth | bandwidth per thread | comment |
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
| 4 MiB   | 0.16 s | +9%    | +71% | 65.5GB/s  | 16.4GB/s  |             | 
| 8 MiB   | 0.15 s | -      | +8%  | 71.1GB/s  | 17.8GB/s  |             | 
| 16 MiB  | 0.17 s | +11%   | +11% | 64.1GB/s  | 16.0GB/s  | L3, 16 MiB  | 
| 32 MiB  | 0.16 s | +4%    | +6%  | 68.3GB/s  | 17.1GB/s  |             | 
| 64 MiB  | 0.15 s | +2%    | +2%  | 69.9GB/s  | 17.5GB/s  |             | 
| 128 MiB | 0.16 s | +3%    | +1%  | 69.1GB/s  | 17.3GB/s  |             |

</details>
