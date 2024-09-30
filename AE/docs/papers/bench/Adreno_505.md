
# Qualcomm Adreno 505

## Specs

* gmem: 128 Kb + 8 Kb (128x256 tile for RGBA8)
* L2 cache: ?
* texture cache: ?
* F16 GFLOPS: **86.4**
* F32 GFLOPS: **43.2**
* F64 GFLOPS: **10.8**
* Execution units: 1
* ALUs: 48 ?
* Clock: 450 MHz
* Memory v1: 2GB, LPDDR3, 933 MHz, 7.4GB/s (5GB/s from tests)
* Memory v2: 2GB, LPDDR3-1600, SC 32bit, 800MHz, 6.4 GB/s
* Device: Redmi 7A (Android ?, driver ?)

## Render target compression

* RGBA8 67.1MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- expected read: 268MB, write: 67MB, total: 335MB per frame.
	- 8x8 noise: linear: 25ms, nearest: 40ms, fetch: 46ms.
	- image storage: load: 350ms, fetch/linear: 150ms.
	- graphics to compute: ??? ms.

	| diff | exec time (ms) | approx traffic (GB/s) | name | comments |
	|---|---|---|------|----|
	| 0.19 | 350 | 0.95 | image storage | |
	| 1    | 66  | 5.0  | 1x1 noise     | |
	| 1.6  | 42  | 8.0  | 2x2 noise     | |
	| 2.5  | 26  | 12.9 | 4x4 noise     | **same as block size** |
	| 2.6  | 25  | 13.4 | 8x8 noise     | |
	| 2.6  | 25  | 13.4 | 16x16 noise   | |
	| 2.6  | 25  | 13.4 | gradient      | |
	| 2.7  | 24  | 14   | solid color   | |

