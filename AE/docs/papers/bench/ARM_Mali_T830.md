
# ARM Mali T830 (Midgard gen4)

## Specs

* Cores: 1
* ALU: 2
* L2: 64 Kb
* Clock: 1000 MHz
* Bus width: 128 bits
* Memory: 2GB, LPDDR3, DC 32bit, 933MHz, **14.9**GB/s (4GB/s from tests)
* FP16 GFLOPS: **56**
* FP32 GFLOPS: **32**
* Device: Samsung J7 Neo (Android 9, Driver 28.0.0)

## Shader

Doesn't support quad and subgroups.

### Noise performance

| name | thread count | exec time (ms) | per thread (ns) |
|---|---|---|---|---|
| SimplexNoise                  | 262K | 14.6 |   55.7 |
| ValueNoise                    | 262K | 15.4 |   58.8 |
| WaveletNoise                  | 262K | 18.1 |   69.1 |
| GradientNoise                 | 262K | 21.9 |   83.6 |
| Voronoi, 2D                   | 262K | 23.6 |   90.1 |
| PerlinNoise                   | 262K | 26.2 |  100   |
| SimplexNoiseFBM, octaves=4    | 65K  | 10.8 |  166   |
| ValueNoiseFBM, octaves=4      | 65K  | 13.5 |  208   |
| Voronoi, 3D                   | 65K  | 15.6 |  240   |
| WarleyNoise                   | 65K  | 16.7 |  257   |
| GradientNoiseFBM, octaves=4   | 65K  | 17.5 |  269   |
| PerlinNoiseFBM, octaves=4     | 65K  | 18.2 |  280   |
| VoronoiCircles                | 65K  | 19.8 |  305   |
| VoronoiContour3, 2D           | 65K  | 21.8 |  335   |
| VoronoiContour2, 2D           | 65K  | 22.2 |  341   |
| WarleyNoiseFBM, octaves=4     | 65K  | 58   |  892   |
| IQNoise                       | 65K  | 75.6 | 1 160  |
| VoronoiContour3, 3D           | 16K  | 23.1 | 1 444  |
| VoronoiContour2, 3D           | 65K  | 97.2 | 1 495  |
| IQNoiseFBM, octaves=4         | 16K  | 76.2 | 4 762  |
| VoronoiContour3FBM, octaves=4 | 4K   | 23.5 | 5 875  |
| VoronoiContourFBM, octaves=4  | 4K   | 23.9 | 5 975  |


## Render target compression

* RGBA8 16.8MPix downsample 1/2, compressed/uncompressed access rate: [[3](../GPU_Benchmarks.md#3-Render-target-compression)]
	- **no compression**

	| diff (read) | read (MB) | write (MB) | name |
	|---|---|---|------|----|
	| -   | 67.1 | 16.7 | expected |
	| 0.9 | 75   | 17   | image storage |
	| 1   | 68.5 | 17   | solid color |
	| 1   | 68.5 | 17   | gradient |


