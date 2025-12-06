
# Тесты производительности

1.1. Nonuniform PBR<br/>
Разница в производительности между использованием `nonuniform()` и выбором слоя из Texture2DArray.
Чтобы в варп попадали разные индексы используется хэш от `gl_FragCoord` с двумя режимами: квадрат 2х2 и попиксельно.<br/>
Для имитации PBR используются LODы с разным количеством текстур.

1.2. Nonuniform parallax<br/>
Разница в производительности между использованием `nonuniform()` и выбором слоя из Texture2DArray.
Чтобы в варп попадали разные индексы используется хэш от `gl_FragCoord` с двумя режимами: квадрат 2х2 и попиксельно.<br/>
Используется цикл до 64 шагов по одной текстуре для имитации рельефного текстурирования.


2. Triangles per draw call.<br/>
Как влияет на производительность количество треугольников в одном вызове рисования.

3. Meshlets as instances.<br/>


**Результаты**
* [AMD RX570](#AMD-RX570)
* [AMD Radeon 780M, AMDPRO](#AMD-Radeon-780M-AMDPRO)
* [AMD Radeon 780M, AMDVLK](#AMD-Radeon-780M-AMDVLK)
* [AMD Radeon 780M, RADV](#AMD-Radeon-780M-RADV)
* [Nvidia RTX 2080](#Nvidia-RTX-2080)
* [ARM Mali G57](#ARM-Mali-G57)
* [ARM Mali G610](#ARM-Mali-G610)
* [Adreno 660](#Adreno-660)
* [Apple M1](#Apple-M1)
* [PowerVR BXM-8-256](#PowerVR-BXM-8-256)
* [Intel UHD 620](#Intel-UHD-620)
* [Intel N150](#Intel-N150)

## Nvidia RTX 2080

**Nonuniform, depth pre-pass**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0      | 1.0       |
| texture index           | 1.0        | 1.014    | 1.13     | 1.99      |
| texture & sampler index | 1.0        | 1.018    | 1.11     | 1.97      |

**Nonuniform, visibility buffer**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0      | 1.0       |
| texture index           | 1.007      | 1.007    | 1.02     | 1.43      |
| texture & sampler index | 1.007      | 1.007    | 1.03     | 1.41      |

[Подробные результаты](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-gpu/NVidia_RTX2080.md#Nonuniform)


## AMD RX570

Из-за бага в драйвере nonuniform работает через раз. Первые тесты делались на текстурах с низким разрешением (64х64) и видимо они попадали в кэш, поэтому проблема не проявлялась и разница в производительности оказалась небольшой.
В новом тесте используются текстуры 1024х1024 и это приводит к некорректным данным при чтении, а также потере производительности.
Когда драйвер работал корректно вариант с visibility buffer оказался намного быстрее с bindless подходом.

**Nonuniform, depth pre-pass**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0      | 1.0       |
| texture index           | 2.24       | 2.03     | 2.47     | 2.52      |
| texture & sampler index | 2.24       | 2.03     | 2.47     | 2.52      |

**Nonuniform, visibility buffer**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0      | 1.0       |
| texture index           | 1.017      | 1.013    | 1.21     | 1.79      |
| texture & sampler index | 1.026      | 1.022    | 1.21     | 1.8       |

[Подробные результаты](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-gpu/AMD_RX570.md#Nonuniform)


## AMD Radeon 780M, AMDPRO

Хоть и нет нативной поддержки неоднородных индексов, но производительность меняется незначительно.

**Nonuniform, depth pre-pass**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       |
| texture index           |
| texture & sampler index |

**Nonuniform, visibility buffer**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       |
| texture index           |
| texture & sampler index |

[Подробные результаты](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-gpu/AMD_780M.md#Nonuniform-AMDPRO)


## AMD Radeon 780M, AMDVLK

**Nonuniform, depth pre-pass**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       |
| texture index           |
| texture & sampler index |

**Nonuniform, visibility buffer**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       |
| texture index           |
| texture & sampler index |

[Подробные результаты](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-gpu/AMD_780M.md#Nonuniform-AMDVLK)


## AMD Radeon 780M, RADV

RADV драйвер оказался быстрее других, но bindless сильнее влияет на производительность.

**Nonuniform, depth pre-pass**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       |
| texture index           |
| texture & sampler index |

**Nonuniform, visibility buffer**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       |
| texture index           |
| texture & sampler index |

[Подробные результаты](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-gpu/AMD_780M.md#Nonuniform-RADV)


## ARM Mali G57

**Nonuniform, depth pre-pass**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0      | 1.0       |
| texture index           | 1.034      | 0.94     | 1.35     | 2.68      |
| texture & sampler index | 1.034      | 0.92     | 1.39     | 2.6       |

[Подробные результаты](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-gpu/ARM_Mali_G57.md#Nonuniform)


## ARM Mali G610

Valhall gen3 архитектура уже лучше справляется с bindless по сравнению с gen1.

**Nonuniform, depth pre-pass**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       |
| texture index           |
| texture & sampler index |

**Nonuniform, visibility buffer**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       |
| texture index           |
| texture & sampler index |


[Подробные результаты](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-gpu/ARM_Mali_G610.md#Nonuniform)


## Adreno 660

**Nonuniform, depth pre-pass**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       |
| texture index           |
| texture & sampler index |

**Nonuniform, visibility buffer**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       |
| texture index           |
| texture & sampler index |

[Подробные результаты](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-gpu/Adreno_660.md#Nonuniform)


## Apple M1

**Nonuniform, depth pre-pass**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0      | 1.0       |
| texture index           | 1.03       | 0.98     | 0.99     | 0.99      |
| texture & sampler index | 1.06       | 0.96     | 0.95     | 0.96      |

**Nonuniform, visibility buffer**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0      | 1.0       |
| texture index           | 1.06       | 0.99     | 1.005    | 1.07      |
| texture & sampler index | 1.09       | 1.005    | 1.016    | 1.07      |

[Подробные результаты](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-gpu/Apple_M1.md#Nonuniform)


## PowerVR BXM-8-256

**Nonuniform, depth pre-pass**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       |
| texture index           |
| texture & sampler index |

**Nonuniform, visibility buffer**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       |
| texture index           |
| texture & sampler index |

[Подробные результаты](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-gpu/PowerVR_BXM.md#Nonuniform)


## Intel UHD 620

Вариант с bindless texture в разы медленее, скорее всего компилятор сопоставлял immutable sampler с динамической индексацией и получилось очень плохо.

**Nonuniform, depth pre-pass**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0      | 1.0       |
| texture index           | 1.0        | 2.2      | 7.5      | 9.7       |
| texture & sampler index | 1.03       | 1.0      | 1.86     | 1.94      |

**Nonuniform, visibility buffer**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0      | 1.0       |
| texture index           | 1.14       | 1.08     | 3.7      | 8.7       |
| texture & sampler index | 1.14       | 1.04     | 1.57     | 2.6       |

[Подробные результаты](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-gpu/Intel_UHD620.md#Nonuniform)


## Intel N150

**Nonuniform, depth pre-pass**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0      | 1.0       |
| texture index           | 1.19       | 0.82     | 1.48     | 1.6       |
| texture & sampler index | 1.19       | 0.82     | 1.48     | 1.6       |

**Nonuniform, visibility buffer**

| nonuniform              | per object | per warp | per quad | per pixel |
|-------------------------|------------|----------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0      | 1.0       |
| texture index           | 1.17       | 1.02     | 1.56     | 2.4       |
| texture & sampler index | 1.15       | 1.02     | 1.56     | 2.4       |

[Подробные результаты](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-gpu/Intel_N150.md#Nonuniform)


