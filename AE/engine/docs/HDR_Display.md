## General information

__Non-HDR (RGBA8 / RGB10A2 formats)__

* Color values above 1.0 are clamped to 1.0.

__HDR (RGBA16F format)__

* Display uses internal tonemapping to convert color values above 1.0 to another color, more often the color tends to white.
* The transition to white depends on luminance, green is the brightest and turns white earlier.
* There is no way to programmatically find out the characteristics of the built-in tone mapping, so it's better to use your own and adjust the brightness of the screen with values exceeding 1.0.


## Monitor

Samsung monitor with VA matrix with a brightness of 1000 nit. Surface with `RGBA16F_Extended_sRGB_linear` mode.

* An analog of ACES tonemapping is used, so blue turns into pink, and only then into white.

![](ru/img/HDR-monitor.jpg)


## Smartphone

ASUS smartphone with AMOLED screen with a brightness of 800 nit. Surface with `RGBA16F_Extended_sRGB_linear` mode.

* Color value range 0..100 on monitor is approximately the same as range 0..24 on the smartphone.
* Smartphone uses much simpler tonemapping which doesn't change the blue color.

![](ru/img/HDR-smartphone.jpg)


## Surface format in Engine

Sources: [FeatureSetEnums.h - ESurfaceFormat](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/graphics/Public/FeatureSetEnums.h#L26)

### `RGBA16F_Extended_sRGB_linear`

Backward compatible with sRGB in range [0..1] but allows to use values above 1.0.
Color (1,1,1) has brightness 80 nit. Allowed values up to 125.0 which has 10 000 nit brightness.

### `BGRA8_sRGB_nonlinear` and `RGBA8_sRGB_nonlinear`

sRGB mode, RGB or BGR depends on platform.

### `RGB10A2_sRGB_nonlinear`

sRGB mode with increased precision to 10bit per channel.
