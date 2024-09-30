GPUs:
* [Adreno 660](bench/Adreno_660.md)
* [Adreno 505](bench/Adreno_505.md)
* [AMD RX 570](bench/AMD_RX570.md)
* [Apple M1](bench/Apple_M1.md)
* [Intel UHD 620](bench/Intel_UHD620.md)
* [Mali G57](bench/ARM_Mali_G57.md)
* [Mali T830](bench/ARM_Mali_T830.md)
* [NVidia RTX 2080](bench/NVidia_RTX2080.md)
* [PowerVR BXM-8-256](bench/PowerVR_BXM.md)

Other:
* [Comparison of Results](#Comparison-of-Results)
* [Test Sources](#Test-Sources)


# Comparison of Results
	
## Render target compression

**block** - compare compression between 1x1 noise and block size (4x4 or 8x8) noise.<br/>
**max** - compare compression between 1x1 noise and solid color.<br/>

| GPU | block size | block RGBA8_UNorm | max RGBA8_UNorm | block RGBA16_UNorm | max RGBA16_UNorm | method | comments |
|---|---|---|---|
| NV RTX 20xx          | 4x4   | 3   | 3.2 | 4.1 | 4.1 | exec time   |
| Adreno 6xx           | 16x16 | 1.9 | 6.9 | ?   | 3.3 | exec time   |
| Adreno 5xx           | 4x4   | 2.5 | 2.7 | ?   | ?   | exec time   |
| AMD GCN4             | 4x4   | 2.3 | 3   | 2.3 | 3   | exec time   |
| Intel UHD 6xx 9.5gen | 8x8   | 1.6 | 1.8 | ?   | ?   | exec time   |
| ARM Mali Valhall     | 4x4   | 6.9 | 60  | -   | -   | mem traffic | only 32bit formats |
| PowerVR B-Series     | 8x8   | 23  | 134 | 24  | 134 | mem traffic |


# Test Sources

### 1. fp16 instruction performance
[code](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/Inst-fp16.as)

### 2. fp32 instruction performance
[code](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/Inst-fp32.as)

### 3. Render target compression
[code](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/RTCompression.as)

### 4. Shader instruction benchmark


### 5. Texture lookup performance

* sequential access - UV coordinates multiplied by scale and added bias.
	- scale < 1 has better texture cache usage.
	- scale > 1 has high cache misses.
	- scale > 1 in practice used for noise texture in procedural generation.
* 'noise NxN' - screen divided into blocks with NxN size, each block has unique offset for texture lookup, each pixel in block has 1px offset from nearest pixels.
	- offset with 1px used to find case where nearest warp can not use cached texel.
	- in practice this method is used for packed 2D sprites and textures for meshes.

[code](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/TexLookup.as)

### 6. Subgroups

* [Subgroups in fullscreen triangle](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/Subgroups-1.as)
* [Subgroups with multiple triangles](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/Subgroups-2.as)

### 7. Buffer/Image storage access

* [Image/Buffer common cases](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/Storage.as)
* [Buffer with variable data size](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/BufferStorage.as)
* [Image with thread/group reorder](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/ImageStorage-Reorder.as)
* [Image with RT compression, 4xRGBA8](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/ImageStorage-1.as)
* [Image with RT compression, 2xRGBA16](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/ImageStorage-2.as)
* [Image with RT compression, 1xR32](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/ImageStorage-3.as)

### 9. Texture cache

Find texture size where performance has near to 2x degradation this indicates a lot of cache misses and bottleneck in high level cache or external memory (RAM/VRAM).<br/>
Expected hierarchy:
* texture cache (L1)
* L2 cache
* RAM / VRAM

[code](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/TexCache.as)
