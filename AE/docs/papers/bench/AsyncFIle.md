
Content:
* [Test description](#Test-description)
* [ASUS NUC15](#ASUS-NUC15)
* [Samsung SSD 860 EVO](#Samsung-SSD-860-EVO)


# Test description

Read file in single thread with synchronous and asynchronous (AsyncIO) mode.

Used non-cached access to disable caching in OS.
On Android/Linux: `F_GLOBAL_NOCACHE` flag, on Windows: `FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH`.

[Source code](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/threading/Perf_AsyncFile.cpp)


# ASUS NUC15

## HWINFO

```
Drive Controller:	NVMe (PCIe x4 16.0 GT/s @ x4 16.0 GT/s)
Host Controller:	Micron/Crucial Technology, Device ID: 5426
Drive Model:		CT2000P3PSSD8
```

## CrystalDiskMark

```
* MB/s = 1,000,000 bytes/s [SATA/600 = 600,000,000 bytes/s]
* KB = 1000 bytes, KiB = 1024 bytes

[Read]
  SEQ    1MiB (Q=  8, T= 1):  5024.997 MB/s [   4792.2 IOPS] <  1667.96 us>
  SEQ    1MiB (Q=  1, T= 1):  3362.562 MB/s [   3206.8 IOPS] <   311.71 us>
  RND    4KiB (Q= 32, T= 1):   836.974 MB/s [ 204339.4 IOPS] <    60.86 us>
  RND    4KiB (Q=  1, T= 1):    78.170 MB/s [  19084.5 IOPS] <    52.30 us>

[Write]
  SEQ    1MiB (Q=  8, T= 1):  4693.229 MB/s [   4475.8 IOPS] <  1782.25 us>
  SEQ    1MiB (Q=  1, T= 1):  4579.979 MB/s [   4367.8 IOPS] <   228.71 us>
  RND    4KiB (Q= 32, T= 1):   585.297 MB/s [ 142894.8 IOPS] <    20.51 us>
  RND    4KiB (Q=  1, T= 1):   275.779 MB/s [  67328.9 IOPS] <    14.77 us>
```

## AsyncFile test

### 1 GiB / 1 MiB

| test                  | time   | %    | bandwidth |
|-----------------------|--------|------|----------|
| Async Random Read     | 0.22 s | -    | 4.83GB/s |
| Async Sequential Read | 0.22 s | -    | 4.80GB/s |
| Sync Random Read      | 0.36 s | +60% | 3.01GB/s |
| Sync Sequential Read  | 0.39 s | +75% | 2.75GB/s |


### 1 GiB / 256 KiB

| test                  | time   | %     | bandwidth |
|-----------------------|--------|-------|----------|
| Async Sequential Read | 0.22 s | -     | 4.83GB/s |
| Async Random Read     | 0.22 s | -     | 4.82GB/s |
| Sync Sequential Read  | 0.57 s | +157% | 1.87GB/s |
| Sync Random Read      | 0.68 s | +207% | 1.57GB/s |


### 1 GiB / 64 KiB

| test                  | time   | %     | bandwidth |
|-----------------------|--------|-------|-----------|
| Async Sequential Read | 0.22 s | -     | 4.77GB/s  |
| Async Random Read     | 0.23 s | +1%   | 4.74GB/s  |
| Sync Sequential Read  | 0.69 s | +209% | 1.55GB/s  |
| Sync Random Read      | 1.91 s | +751% | 561.3MB/s |


### 1 GiB / 16 KiB

| test                  | time   | %      | bandwidth |
|-----------------------|--------|--------|-----------|
| Async Sequential Read | 0.37 s | -      | 2.93GB/s  |
| Async Random Read     | 0.39 s | +7%    | 2.74GB/s  |
| Sync Sequential Read  | 1.38 s | +275%  | 779.4MB/s |
| Sync Random Read      | 4.54 s | +1136% | 236.7MB/s |


### 1 GiB / 8 KiB

| test                  | time   | %      | bandwidth |
|-----------------------|--------|--------|-----------|
| Async Sequential Read | 0.69 s | -      | 1.57GB/s  |
| Async Random Read     | 0.74 s | +8%    | 1.45GB/s  |
| Sync Sequential Read  | 2.54 s | +271%  | 422.4MB/s |
| Sync Random Read      | 7.67 s | +1020% | 140.0MB/s |


### 256 MiB / 4 KiB

| test                  | time   | %     | bandwidth |
|-----------------------|--------|-------|-----------|
| Async Random Read     | 0.36 s | -     | 743.3MB/s |
| Async Sequential Read | 0.38 s | +4%   | 715.6MB/s |
| Sync Sequential Read  | 1.12 s | +211% | 238.9MB/s |
| Sync Random Read      | 3.54 s | +881% | 75.8MB/s  |



# Samsung SSD 860 EVO

## HWINFO

```
Drive Controller:	Serial ATA 6Gb/s @ 6Gb/s
Drive Model:		Samsung SSD 860 EVO M.2 1TB
```

## AsyncFile test

### 1 GiB / 1 MiB

| test                  | time   | %    | bandwidth |
|-----------------------|--------|------|-----------|
| Async Random Read     | 1.95 s | -    | 550.6MB/s |
| Async Sequential Read | 1.97 s | +1%  | 544.6MB/s |
| Sync Sequential Read  | 2.09 s | +7%  | 513.5MB/s |
| Sync Random Read      | 2.16 s | +11% | 497.8MB/s |


### 1 GiB / 256 KiB

| test                  | time   | %    | bandwidth |
|-----------------------|--------|------|-----------|
| Async Random Read     | 1.97 s | -    | 544.4MB/s |
| Async Sequential Read | 1.98 s | -    | 543.2MB/s |
| Sync Sequential Read  | 2.31 s | +17% | 464.4MB/s |
| Sync Random Read      | 2.88 s | +46% | 372.5MB/s |


### 1 GiB / 64 KiB

| test                  | time   | %    | bandwidth |
|-----------------------|--------|------|-----------|
| Async Random Read     | 2.07 s | -    | 519.8MB/s |
| Async Sequential Read | 2.12 s | +3%  | 506.8MB/s |
| Sync Sequential Read  | 3.14 s | +52% | 342.1MB/s |
| Sync Random Read      | 3.99 s | +93% | 269.2MB/s |


### 1 GiB / 16 KiB

| test                  | time   | %     | bandwidth |
|-----------------------|--------|-------|-----------|
| Async Sequential Read | 2.58 s | -     | 416.0MB/s |
| Async Random Read     | 2.69 s | +4%   | 399.3MB/s |
| Sync Sequential Read  | 6.55 s | +154% | 164.0MB/s |
| Sync Random Read      | 9.06 s | +251% | 118.6MB/s |


### 1 GiB / 8 KiB

| test                  | time    | %     | bandwidth |
|-----------------------|---------|-------|-----------|
| Async Sequential Read | 3.13 s  | -     | 343.4MB/s |
| Async Random Read     | 3.26 s  | +4%   | 329.4MB/s |
| Sync Sequential Read  | 8.88 s  | +184% | 120.9MB/s |
| Sync Random Read      | 16.97 s | +443% | 63.3MB/s  |


### 256 MiB / 4 KiB

| test                  | time   | %     | bandwidth |
|-----------------------|--------|-------|-----------|
| Async Random Read     | 0.98 s | -     | 274.2MB/s |
| Async Sequential Read | 1.04 s | +6%   | 259.3MB/s |
| Sync Sequential Read  | 3.92 s | +300% | 68.6MB/s  |
| Sync Random Read      | 7.68 s | +685% | 35.0MB/s  |


# HDD WDC 3GB

## HWINFO

```
Drive Controller:	Serial ATA 6Gb/s @ 6Gb/s
Drive Model:		WDC WD30EZRZ-00Z5HB0
```

## AsyncFile test

### 1 GiB / 1 MiB

| test                  | time    | %     | bandwidth |
|-----------------------|---------|-------|----------|
| Sync Sequential Read  | 11.01 s | -     | 97.5MB/s |
| Async Sequential Read | 11.19 s | +2%   | 96.0MB/s |
| Sync Random Read      | 27.49 s | +150% | 39.1MB/s |
| Async Random Read     | 27.98 s | +154% | 38.4MB/s |

### 1 GiB / 256 KiB

| test                  | time    | %     | bandwidth |
|-----------------------|---------|-------|----------|
| Async Sequential Read | 10.94 s | -     | 98.2MB/s |
| Sync Sequential Read  | 10.96 s | -     | 98.0MB/s |
| Async Random Read     | 46.11 s | +322% | 23.3MB/s |
| Sync Random Read      | 1.26 m  | +592% | 14.2MB/s |

### 1 GiB / 64 KiB

| test                  | time    | %      | bandwidth |
|-----------------------|---------|--------|-----------|
| Async Sequential Read | 10.53 s | -      | 102.0MB/s |
| Sync Sequential Read  | 10.63 s | +1%    | 101.0MB/s |
| Async Random Read     | 2.12 m  | +1111% | 8.42MB/s  |
| Sync Random Read      | 4.63 m  | +2541% | 3.86MB/s  |

### 1 GiB / 16 KiB

| test                  | time    | %      | bandwidth |
|-----------------------|---------|--------|----------|
| Async Sequential Read | 11.23 s | -      | 95.6MB/s |
| Sync Sequential Read  | 12.12 s | +8%    | 88.6MB/s |
| Async Random Read     | 7.64 m  | +3983% | 2.34MB/s |
| Sync Random Read      | 17.84 m | +9433% | 1.00MB/s |

### 1 GiB / 8 KiB

| test                  | time    | %       | bandwidth |
|-----------------------|---------|---------|-----------|
| Async Sequential Read | 12.63 s | -       | 85.0MB/s  |
| Sync Sequential Read  | 13.42 s | +6%     | 80.0MB/s  |
| Async Random Read     | 15.04 m | +7045%  | 1.19MB/s  |
| Sync Random Read      | 34.57 m | +16328% | 517.7KB/s |

### 256 MiB / 4 KiB

| test                  | time    | %       | bandwidth |
|-----------------------|---------|---------|-----------|
| Async Sequential Read | 5.84 s  | -       | 46.0MB/s  |
| Sync Sequential Read  | 7.63 s  | +31%    | 35.2MB/s  |
| Async Random Read     | 6.47 m  | +6551%  | 691.0KB/s |
| Sync Random Read      | 16.72 m | +17082% | 267.5KB/s |


# HDD Seagate 16TB

## HWINFO

```
Drive Controller:	Serial ATA 6Gb/s @ 6Gb/s
Drive Model:		Seagate ST16000NM000J-2TW103
```

## AsyncFile test

### 1 GiB / 1 MiB

| test                  | time    | %     | bandwidth |
|-----------------------|---------|-------|-----------|
| Sync Sequential Read  | 5.91 s  | -     | 181.7MB/s |
| Async Sequential Read | 7.33 s  | +24%  | 146.5MB/s |
| Async Random Read     | 11.29 s | +91%  | 95.1MB/s  |
| Sync Random Read      | 13.12 s | +122% | 81.8MB/s  |


### 1 GiB / 256 KiB

| test                  | time    | %     | bandwidth |
|-----------------------|---------|-------|-----------|
| Sync Sequential Read  | 6.16 s  | -     | 174.2MB/s |
| Async Sequential Read | 6.75 s  | +10%  | 159.0MB/s |
| Async Random Read     | 18.24 s | +196% | 58.9MB/s  |
| Sync Random Read      | 33.02 s | +436% | 32.5MB/s  |


### 1 GiB / 64 KiB

| test                  | time    | %      | bandwidth |
|-----------------------|---------|--------|-----------|
| Sync Sequential Read  | 6.53 s  | -      | 164.4MB/s |
| Async Sequential Read | 7.00 s  | +7%    | 153.4MB/s |
| Async Random Read     | 47.63 s | +629%  | 22.5MB/s  |
| Sync Random Read      | 1.76 m  | +1519% | 10.2MB/s  |


### 1 GiB / 16 KiB

| test                  | time   | %      | bandwidth |
|-----------------------|--------|--------|-----------|
| Sync Sequential Read  | 6.60 s | -      | 162.6MB/s |
| Async Sequential Read | 6.80 s | +3%    | 157.9MB/s |
| Async Random Read     | 3.15 m | +2764% | 5.68MB/s  |
| Sync Random Read      | 7.59 m | +6796% | 2.36MB/s  |


### 1 GiB / 8 KiB

| test                  | time    | %       | bandwidth |
|-----------------------|---------|---------|-----------|
| Async Sequential Read | 6.92 s  | -       | 155.1MB/s |
| Sync Sequential Read  | 8.27 s  | +19%    | 129.9MB/s |
| Async Random Read     | 6.22 m  | +5293%  | 2.88MB/s  |
| Sync Random Read      | 15.06 m | +12949% | 1.19MB/s  |

### 256 MiB / 4 KiB

| test                  | time   | %       | bandwidth |
|-----------------------|--------|---------|-----------|
| Async Sequential Read | 2.22 s | -       | 120.8MB/s |
| Sync Sequential Read  | 3.42 s | +54%    | 78.5MB/s  |
| Async Random Read     | 1.55 m | +4075%  | 2.89MB/s  |
| Sync Random Read      | 4.95 m | +13266% | 903.6KB/s |
