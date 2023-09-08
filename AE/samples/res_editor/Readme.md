Редактор шейдеров

## Возможности

 * Код пишется на скриптах
 * Быстрая перезагрузка скриптов и шейдеров
 * Слайдеры и колорпикеры для переменных в шейдере
 * Встроенная отладка и профилирование шейдеров
 * Контроллеры: 2D, FPS/FPV камеры, Flight camera (авиасимы)
 * Шаблоны геометрии:
    - Частицы
    - Сферический куб с тангенциальной проекцией (скайбокс, планеты)
 * Совместимость с shadertoy
 * Поддерживает: MeshShader, RTX
 * Асинхронная загрузка ресурсов
 * Запись видео (ffmpeg), скриншоты (dds, png, jpeg)
 * Поддержка HDR мониторов (RGBA16F формат)


## Управление

`Esc` - переключение контроллера на UI<br/>
`Backspace` - отключает рендер, остается только UI<br/>
`Tab` - скрыть/показать UI<br/>
`F1` - показать подсказку<br/>


## UI редактора

### Вкладка Scripts

Выводит дерево файлов из папки указаной в `cfg.ScriptDir()`.<br/>
При клике выполняет скрипт.

### Вкладка Editor

#### Debugger

Позволяет запустить отладку выбранного прохода (__Pass__).<br/>
Режимы (__Mode__):
 * __Trace__ - записывает лог вызовов всех функций со всеми аргументами и переменными в шейдере.
 * __FnProfiling__ - записывает текстовый файл с временем выполнения шейдерных функций.
 * __TimeHeatmap__ - пока не поддерживается.

Этап (__Stage__) - позволяет выбрать один шейдер: вершинный, фрагментный и тд.

Чекбокс __'Show debug view'__ показывает/скрывает дополнительный окна, которые добавлены в скрипте функцией `DbgView()`.

#### Capture

* Настройки формата для скриншотов.
* Настройки видео.

#### Sliders

Слайдеры, добавленные в скриптах функцией `Slider()`.

#### Statistics

Выводится некоторая статистика:
 * Положение курсора
 * Цвет экрана под курсором


### Вкладка Graphics

__Surface scale__<br/>
Позволяет менять масштаб промежуточных рендер таргетов, в скриптах функция `SurfaceSize()` возвращает реальный размер окна умноженый на масштаб.

__Surface size__<br/>
Выводит размер рендер таргета после масштабирования.

__Surface format__<br/>
Позволяет выбрать формат рендер таргета, который выводится на экран.
 * RGBA8 / BGRA8 sRGB - стандартный режим.
 * RGBA16F Extended_sRGB - HDR режим монитора, переключение займет некоторое время. После чего цвет > 1.0 будет добавлять яркость на экране, максимальное значение зависит от монитора, чаще всего после 10.0 - 50.0 цвет переходит в белый.

__Present mode__<br/>
Режим вертикальной синхронизации:
 * FIFO, FIFO_Relaxed - с вертикальной синхронизацией для каждого кадра.
 * Mailbox - с вертикальной синхронизацией, но с потерей промежуточных кадров.
 * Immediate - без синхронизации, кадры могут накладываться.

### Окна с профайлерами

 * TaskProfiler - замеряет время на ЦП для каждой задачи.
 * GraphicsProfiler - замеряет время каждого прохода.
 * HWProfiler - вендорские счетчики, если доступно.



## Скрипты

Рендер скрипт запускается один раз, чтобы настроить проходы рисования.<br/>
__Скрипт не выполняется каждый кадр!__

С помощью препроцессора можно объединять скрипт с шейдерным кодом, для этого скрипт должен быть внутри блока:
```cpp
#ifdef SCRIPT
    ...
#endif
```

Скрипт пишется на AngelScript с препроцессором, который позволяет компилировать скрипт как C++ код (линковка и выполнение не доступно), это позволяет сразу проверить корректность кода.
Все доступные функции и типы находятся в файле `<res_editor>`, для выполнения скрипта подключать его не требуется, это нужно для компиляции и работы подсказок и автодополнения.

### Ресурсы

#### Image

<details>
<summary>Конструкторы</summary>

```cpp
Image ()
Image (EImageType, string vfsFileName)
Image (EPixelFormat, uint2)
Image (EPixelFormat, uint3)
Image (EPixelFormat, uint2, ImageLayer)
Image (EPixelFormat, uint2, MipmapLevel)
Image (EPixelFormat, uint3, MipmapLevel)
Image (EPixelFormat, uint2, ImageLayer, MipmapLevel)
Image (EPixelFormat, RC<DynamicDim>)
Image (EPixelFormat, RC<DynamicDim>, ImageLayer)
Image (EPixelFormat, RC<DynamicDim>, MipmapLevel)
Image (EPixelFormat, RC<DynamicDim>, ImageLayer, MipmapLevel)
```
</details>

<details>
<summary>Методы</summary>

```cpp
void  Name (string)

void  LoadLayer (string vfsFileName, uint layer)
void  LoadLayer (string vfsFileName, uint layer, ImageLoadOpFlags)

bool  IsMutableDimension ()

uint2  Dimension2 ()
uint3  Dimension2_Layers ()
uint3  Dimension3 ()
RC<DynamicDim>  Dimension ()
uint  ArrayLayers ()
uint  MipmapCount ()

RC<Image>  CreateView (EImage, MipmapLevel, uint mipsCount, ImageLayer, uint arrayLayers)
RC<Image>  CreateView (EImage)
RC<Image>  CreateView (EImage, MipmapLevel, uint mipsCount)
RC<Image>  CreateView (EImage, ImageLayer, uint arrayLayers)
```

`vfsFileName` - принимает только путь из папок, указаных в `cfg.VFSPath()`.
</details>

#### Buffer

<details>
<summary>Конструкторы</summary>

```cpp
Buffer ()
Buffer (uint sizeInBytes)
Buffer (string vfsFileName)
```
</details>

<details>
<summary>Методы</summary>

```cpp
void  Name (string)

void  LayoutAndSize (string typeName, uint64 dataSize)
void  Layout (string typeName)
void  Layout (string typeName, string source)
void  LayoutAndCount (string typeName, uint arraySize)
void  LayoutAndCount (string typeName, RC<DynamicUInt> arraySize)
void  LayoutAndCount (string typeName, string source, uint arraySize)
void  LayoutAndCount (string typeName, string source, RC<DynamicUInt> arraySize)

void  Float (string name, float)
void  Float2 (string name, float, float)
void  Float3 (string name, float, float, float)
void  Float4 (string name, float, float, float, float)
void  Float2 (string name, float2)
void  Float3 (string name, float3)
void  Float4 (string name, float4)

void  Float2x2 (string name, float2x2)
void  Float2x3 (string name, float2x3)
void  Float2x4 (string name, float2x4)
void  Float3x2 (string name, float3x2)
void  Float3x3 (string name, float3x3)
void  Float3x4 (string name, float3x4)
void  Float4x2 (string name, float4x2)
void  Float4x3 (string name, float4x3)
void  Float4x4 (string name, float4x4)

void  Int (string name, int)
void  Int2 (string name, int, int)
void  Int3 (string name, int, int, int)
void  Int4 (string name, int, int, int, int)
void  Int2 (string name, int2)
void  Int3 (string name, int3)
void  Int4 (string name, int4)
void  Uint (string name, uint)
void  Uint2 (string name, uint, uint)
void  Uint3 (string name, uint, uint, uint)
void  Uint4 (string name, uint, uint, uint, uint)
void  Uint2 (string name, uint2)
void  Uint3 (string name, uint3)
void  Uint4 (string name, uint4)

void  Float1Array (string name, array<float>)
void  Float2Array (string name, array<float2>)
void  Float3Array (string name, array<float3>)
void  Float4Array (string name, array<float4>)

void  Float2x2Array (string name, array<float2x2>)
void  Float2x3Array (string name, array<float2x3>)
void  Float2x4Array (string name, array<float2x4>)
void  Float3x2Array (string name, array<float3x2>)
void  Float3x3Array (string name, array<float3x3>)
void  Float3x4Array (string name, array<float3x4>)
void  Float4x2Array (string name, array<float4x2>)
void  Float4x3Array (string name, array<float4x3>)
void  Float4x4Array (string name, array<float4x4>)

void  Int1Array (string name, array<int>)
void  Int2Array (string name, array<int2>)
void  Int3Array (string name, array<int3>)
void  Int4Array (string name, array<int4>)
void  UInt1Array (string name, array<uint>)
void  UInt2Array (string name, array<uint2>)
void  UInt3Array (string name, array<uint3>)
void  UInt4Array (string name, array<uint4>)
```

`Layout()` - создает неинициализированный буфер с заданным типом - должен быть заранее определен.
`LayoutAndCount()` - создает неинициализированный буфер с статическим/динамическим массивом.
`<type><count> (name, data)` - создает константный буфер и добавляет в него поле `name` инициализированное `data`, можно вызывать множество раз.
</details>

#### VideoImage

<details>
<summary>Конструкторы</summary>

```cpp
VideoImage ()
VideoImage (string path)
VideoImage (EPixelFormat, string path)
```
`path` - путь к видео файлу, без VFS.
</details>

<details>
<summary>Методы</summary>

```cpp
void  Name (string)
RC<DynamicDim>  Dimension ()
```
</details>

#### RTGeometry

<details>
<summary>Конструкторы</summary>

```cpp
RTGeometry ()
```
</details>

<details>
<summary>Методы</summary>

```cpp
void  Name (string)

void  AddTriangles (RC<Buffer> vb)
void  AddTriangles (RC<Buffer> vb, uint, uint)

void  AddIndexedTriangles (RC<Buffer> vb, RC<Buffer> ib)
void  AddIndexedTriangles (RC<Buffer> vb, uint maxVertex, uint maxPrimitives, RC<Buffer> ib, EIndex)
```
</details>

#### RTScene

<details>
<summary>Конструкторы</summary>

```cpp
RTScene ()
```
</details>

<details>
<summary>Методы</summary>

```cpp
void  Name (string)

void  AddInstance (RC<RTGeometry>)
void  AddInstance (RC<RTGeometry>, RTInstanceCustomIndex)
void  AddInstance (RC<RTGeometry>, RTInstanceMask)
void  AddInstance (RC<RTGeometry>, RTInstanceSBTOffset)
void  AddInstance (RC<RTGeometry>, RTInstanceCustomIndex, RTInstanceMask)
void  AddInstance (RC<RTGeometry>, RTInstanceCustomIndex, RTInstanceSBTOffset)
void  AddInstance (RC<RTGeometry>, RTInstanceCustomIndex, RTInstanceMask, RTInstanceSBTOffset)
void  AddInstance (RC<RTGeometry>, float3 pos)
void  AddInstance (RC<RTGeometry>, float3 pos, RTInstanceCustomIndex)
void  AddInstance (RC<RTGeometry>, float3 pos, RTInstanceMask)
void  AddInstance (RC<RTGeometry>, float3 pos, RTInstanceSBTOffset)
void  AddInstance (RC<RTGeometry>, float3 pos, RTInstanceCustomIndex, RTInstanceMask)
void  AddInstance (RC<RTGeometry>, float3 pos, RTInstanceCustomIndex, RTInstanceSBTOffset)
void  AddInstance (RC<RTGeometry>, float3 pos, RTInstanceCustomIndex, RTInstanceMask, RTInstanceSBTOffset)
void  AddInstance (RC<RTGeometry>, RTInstanceRotation, float3 pos)
void  AddInstance (RC<RTGeometry>, RTInstanceRotation, float3 pos, RTInstanceCustomIndex)
void  AddInstance (RC<RTGeometry>, RTInstanceRotation, float3 pos, RTInstanceMask)
void  AddInstance (RC<RTGeometry>, RTInstanceRotation, float3 pos, RTInstanceSBTOffset)
void  AddInstance (RC<RTGeometry>, RTInstanceRotation, float3 pos, RTInstanceCustomIndex, RTInstanceMask)
void  AddInstance (RC<RTGeometry>, RTInstanceRotation, float3 pos, RTInstanceCustomIndex, RTInstanceSBTOffset)
void  AddInstance (RC<RTGeometry>, RTInstanceRotation, float3 pos, RTInstanceCustomIndex, RTInstanceMask, RTInstanceSBTOffset)

RC<Buffer>  InstanceBuffer ()
uint  InstanceCount ()
```
</details>

#### UnifiedGeometry : GeomSource

Реализует интерфейс `GeomSource`.

Унифицированная геометрия (меш, модель и тд). Поддерживат множественные вызовы `Draw()`.

<details>
<summary>Конструкторы</summary>

```cpp
UnifiedGeometry ()
```
</details>

<details>
<summary>Методы</summary>

```cpp
void  Buffer (string uniform, RC<Buffer>)
void  Texture (string uniform, RC<Image>)

void  Draw (Draw)
void  Draw (DrawIndexed)
void  Draw (DrawIndirect)
void  Draw (DrawIndexedIndirect)
void  Draw (DrawMeshTasks)
void  Draw (DrawMeshTasksIndirect)
void  Draw (DrawIndirectCount)
void  Draw (DrawIndexedIndirectCount)
void  Draw (DrawMeshTasksIndirectCount)
```
</details>

#### SphericalCube : GeomSource

Реализует интерфейс `GeomSource`.

Куб с тангенциальной сферической проекцией. Сфера состоит из треугольников одинакового размера.

<details>
<summary>Конструкторы</summary>

```cpp
SphericalCube ()
```
</details>

<details>
<summary>Методы</summary>

```cpp
void  AddTexture (string uniform, RC<Image>)
void  AddTexture (string uniform, RC<Image>, string sampler)

void  DetailLevel (uint)
void  DetailLevel (uint min, uint max)

void  TessLevel (float)
void  TessLevel (RC<DynamicFloat>)
```
</details>

#### Scene

<details>
<summary>Конструкторы</summary>

```cpp
Scene ()
```
</details>

<details>
<summary>Методы</summary>

```cpp
void  Input (RC<GeomSource>, float3 pos)
void  Input (RC<GeomSource>)
void  Input (RC<BaseController>)

RC<SceneGraphicsPass>  AddGraphicsPass (string name)
```
</details>

#### Controller2D : BaseController

Реализует интерфейс `BaseController`.

#### FlightCamera : BaseController

Реализует интерфейс `BaseController`.

<details>
<summary>Методы</summary>

```cpp
void  FovY (float angleInRadians)
void  ClipPlanes (float near, float far)

void  RotationScale (float)
void  RotationScale (float, float, float)

void  Position (float3)

void  EngineThrust (float min, float max)
```
</details>

#### FPSCamera / FPVCamera : BaseController

Реализует интерфейс `BaseController`.

<details>
<summary>Методы</summary>

```cpp
void  FovY (float angleInRadians)
void  ClipPlanes (float near, float far)

void  ForwardBackwardScale (float)
void  ForwardBackwardScale (float forward, float backward)

void  UpDownScale (float)
void  UpDownScale (float up, float down)

void  SideMovementScale (float)

void  RotationScale (float)
void  RotationScale (float x, float y)

void  Position (float3)
```
</details>

### Проходы

<details>
<summary>EPassFlags</summary>

* `Unknown` - без дополнительных функций.
* `Enable_ShaderTrace` - шейдер поддерживает отладку.
* `Enable_ShaderFnProf` - шейдер поддерживает профилирование.
* `Enable_ShaderTmProf` - шейдер поддерживает профилирование в режиме heatmap.
* `Enable_AllShaderDbg` - шейдер поддерживает все виды отладки и профилирования.
</details>

#### ComputePass

Вычислительный шейдер, поддерживает множественные вызовы `Dispatch()`.

<details>
<summary>Конструкторы</summary>

```cpp
ComputePass  ()
ComputePass (string shaderPathOrEmpty)
ComputePass (string shaderPathOrEmpty, string defines)
ComputePass (string shaderPathOrEmpty, EPassFlags)
ComputePass (string shaderPathOrEmpty, string defines, EPassFlags)
```
</details>

<details>
<summary>Методы</summary>

```cpp
void  SetDebugLabel (string)
void  SetDebugLabel (string, RGBA8u)

void  SliderI (string name)
void  Slider (string name, int min, int max)
void  Slider (string name, int2 min, int2 max)
void  Slider (string name, int3 min, int3 max)
void  Slider (string name, int4 min, int4 max)
void  Slider (string name)
void  Slider (string name, float min, float max)
void  Slider (string name, float2 min, float2 max)
void  Slider (string name, float3 min, float3 max)
void  Slider (string name, float4 min, float4 max)
void  ColorSelector (string name)

void  Constant (string name, RC<DynamicFloat4>)
void  Constant (string name, RC<DynamicInt4>)

void  LocalSize (uint)
void  LocalSize (uint, uint)
void  LocalSize (uint, uint, uint)
void  LocalSize (uint2)
void  LocalSize (uint3)

void  DispatchGroups (uint)
void  DispatchGroups (uint, uint)
void  DispatchGroups (uint, uint, uint)
void  DispatchGroups (uint2)
void  DispatchGroups (uint3)
void  DispatchGroups (RC<DynamicDim>)
void  DispatchGroups (RC<DynamicUInt>)

void  DispatchThreads (uint)
void  DispatchThreads (uint, uint)
void  DispatchThreads (uint, uint, uint)
void  DispatchThreads (uint2)
void  DispatchThreads (uint3)
void  DispatchThreads (RC<DynamicDim>)
void  DispatchThreads (RC<DynamicUInt>)

void  ArgIn (string uniform, RC<RTScene>)
void  ArgIn (string uniform, RC<Buffer>)
void  ArgOut (string uniform, RC<Buffer>)
void  ArgInOut (string uniform, RC<Buffer>)
void  ArgIn (string uniform, RC<Image>)
void  ArgOut (string uniform, RC<Image>)
void  ArgInOut (string uniform, RC<Image>)
void  ArgIn (string uniform, RC<Image>, string sampler)
void  ArgIn (string uniform, RC<VideoImage>, string sampler)
void  ArgIn (RC<BaseController>)
```
</details>

#### Postprocess

Фрагментный шейдер, поддерживает константы и функции из shadertoy.

<details>
<summary>EPostprocess</summary>

* `None` - обычный фрагментный шейдер, стартовая функция `Main`.
* `Shadertoy` - копирует шейдеры shadertoy, стартовая функция `mainImage`.
* `ShadertoyVR` - копирует шейдеры shadertoy, стартовая функция `mainVR`.
* `ShadertoyVR_180` - для записи видео в формате VR180, стартовая функция `mainVR`.
* `ShadertoyVR_360` - для записи видео в формате VR360, стартовая функция `mainVR`.
* `Shadertoy_360` - для записи видео в формате 360, стартовая функция `mainVR`.
</details>

<details>
<summary>Конструкторы</summary>

```cpp
Postprocess ()
Postprocess (string shaderPath)
Postprocess (string shaderPath, EPostprocess)
Postprocess (EPostprocess)
Postprocess (EPostprocess, string defines)
Postprocess (EPostprocess, EPassFlags)
Postprocess (EPostprocess, string defines, EPassFlags)
Postprocess (string shaderPath, EPostprocess, EPassFlags)
Postprocess (string shaderPath, EPostprocess, string defines, EPassFlags)
```
</details>

<details>
<summary>Методы</summary>

```cpp
void  SetDebugLabel (string)
void  SetDebugLabel (string, RGBA8u)

void  SliderI (string name)
void  Slider (string name, int min, int max)
void  Slider (string name, int2 min, int2 max)
void  Slider (string name, int3 min, int3 max)
void  Slider (string name, int4 min, int4 max)
void  Slider (string name)
void  Slider (string name, float min, float max)
void  Slider (string name, float2 min, float2 max)
void  Slider (string name, float3 min, float3 max)
void  Slider (string name, float4 min, float4 max)
void  ColorSelector (string name)

void  Constant (string name, RC<DynamicFloat4>)
void  Constant (string name, RC<DynamicInt4>)

void  Output (RC<Image>)
void  Output (RC<Image>, MipmapLevel)
void  Output (RC<Image>, ImageLayer)
void  Output (RC<Image>, ImageLayer, MipmapLevel)
void  Output (RC<Image>, RGBA32f clear)
void  Output (RC<Image>, MipmapLevel, RGBA32f clear)
void  Output (RC<Image>, ImageLayer, RGBA32f clear)
void  Output (RC<Image>, ImageLayer, MipmapLevel, RGBA32f clear)
void  Output (RC<Image>, RGBA32u clear)
void  Output (RC<Image>, MipmapLevel, RGBA32u clear)
void  Output (RC<Image>, ImageLayer, RGBA32u clear)
void  Output (RC<Image>, ImageLayer, MipmapLevel, RGBA32u clear)
void  Output (RC<Image>, RGBA32i clear)
void  Output (RC<Image>, MipmapLevel, RGBA32i clear)
void  Output (RC<Image>, ImageLayer, RGBA32i clear)
void  Output (RC<Image>, ImageLayer, MipmapLevel, RGBA32i clear)
void  Output (RC<Image>, DepthStencil clear)
void  Output (RC<Image>, MipmapLevel, DepthStencil clear)
void  Output (RC<Image>, ImageLayer, DepthStencil clear)
void  Output (RC<Image>, ImageLayer, MipmapLevel, DepthStencil clear)

void  Output (string attachment, RC<Image>)
void  Output (string attachment, RC<Image>, MipmapLevel)
void  Output (string attachment, RC<Image>, ImageLayer)
void  Output (string attachment, RC<Image>, ImageLayer, MipmapLevel)
void  Output (string attachment, RC<Image>, RGBA32f clear)
void  Output (string attachment, RC<Image>, MipmapLevel, RGBA32f clear)
void  Output (string attachment, RC<Image>, ImageLayer, RGBA32f clear)
void  Output (string attachment, RC<Image>, ImageLayer, MipmapLevel, RGBA32f clear)
void  Output (string attachment, RC<Image>, RGBA32u clear)
void  Output (string attachment, RC<Image>, MipmapLevel, RGBA32u clear)
void  Output (string attachment, RC<Image>, ImageLayer, RGBA32u clear)
void  Output (string attachment, RC<Image>, ImageLayer, MipmapLevel, RGBA32u clear)
void  Output (string attachment, RC<Image>, RGBA32i clear)
void  Output (string attachment, RC<Image>, MipmapLevel, RGBA32i clear)
void  Output (string attachment, RC<Image>, ImageLayer, RGBA32i clear)
void  Output (string attachment, RC<Image>, ImageLayer, MipmapLevel, RGBA32i clear)
void  Output (string attachment, RC<Image>, DepthStencil clear)
void  Output (string attachment, RC<Image>, MipmapLevel, DepthStencil clear)
void  Output (string attachment, RC<Image>, ImageLayer, DepthStencil clear)
void  Output (string attachment, RC<Image>, ImageLayer, MipmapLevel, DepthStencil clear)

void  OutputBlend (RC<Image>, EBlendFactor src, EBlendFactor dst, EBlendOp)
void  OutputBlend (RC<Image>, MipmapLevel, EBlendFactor src, EBlendFactor dst, EBlendOp)
void  OutputBlend (RC<Image>, ImageLayer, EBlendFactor src, EBlendFactor dst, EBlendOp)
void  OutputBlend (RC<Image>, ImageLayer, MipmapLevel, EBlendFactor src, EBlendFactor dst, EBlendOp)
void  OutputBlend (RC<Image>, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)
void  OutputBlend (RC<Image>, MipmapLevel, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)
void  OutputBlend (RC<Image>, ImageLayer, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)
void  OutputBlend (RC<Image>, ImageLayer, MipmapLevel, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)

void  OutputBlend (string attachment, RC<Image>, EBlendFactor, EBlendFactor, EBlendOp)
void  OutputBlend (string attachment, RC<Image>, MipmapLevel, EBlendFactor, EBlendFactor, EBlendOp)
void  OutputBlend (string attachment, RC<Image>, ImageLayer, EBlendFactor, EBlendFactor, EBlendOp)
void  OutputBlend (string attachment, RC<Image>, ImageLayer, MipmapLevel, EBlendFactor, EBlendFactor, EBlendOp)
void  OutputBlend (string attachment, RC<Image>, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)
void  OutputBlend (string attachment, RC<Image>, MipmapLevel, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)
void  OutputBlend (string attachment, RC<Image>, ImageLayer, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)
void  OutputBlend (string attachment, RC<Image>, ImageLayer, MipmapLevel, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)

void  Input (string uniform, RC<Buffer>)
void  Input (string uniform, RC<Image>, string sampler)
void  Input (string uniform, RC<VideoImage>, string sampler)
void  Input (RC<BaseController>)
```
</details>

#### SceneGraphicsPass

Рисование сцены (класс `Scene`) с помощью графических пайплайнов.

<details>
<summary>Методы</summary>

```cpp
void  SetDebugLabel (string)
void  SetDebugLabel (string, RGBA8u)

void  SliderI (string name)
void  Slider (string name, int min, int max)
void  Slider (string name, int2 min, int2 max)
void  Slider (string name, int3 min, int3 max)
void  Slider (string name, int4 min, int4 max)
void  Slider (string name)
void  Slider (string name, float min, float max)
void  Slider (string name, float2 min, float2 max)
void  Slider (string name, float3 min, float3 max)
void  Slider (string name, float4 min, float4 max)
void  ColorSelector (string name)

void  Constant (string name, RC<DynamicFloat4>)
void  Constant (string name, RC<DynamicInt4>)

void  Output (RC<Image>)
void  Output (RC<Image>, MipmapLevel)
void  Output (RC<Image>, ImageLayer)
void  Output (RC<Image>, ImageLayer, MipmapLevel)
void  Output (RC<Image>, RGBA32f clear)
void  Output (RC<Image>, MipmapLevel, RGBA32f clear)
void  Output (RC<Image>, ImageLayer, RGBA32f clear)
void  Output (RC<Image>, ImageLayer, MipmapLevel, RGBA32f clear)
void  Output (RC<Image>, RGBA32u clear)
void  Output (RC<Image>, MipmapLevel, RGBA32u clear)
void  Output (RC<Image>, ImageLayer, RGBA32u clear)
void  Output (RC<Image>, ImageLayer, MipmapLevel, RGBA32u clear)
void  Output (RC<Image>, RGBA32i clear)
void  Output (RC<Image>, MipmapLevel, RGBA32i clear)
void  Output (RC<Image>, ImageLayer, RGBA32i clear)
void  Output (RC<Image>, ImageLayer, MipmapLevel, RGBA32i clear)
void  Output (RC<Image>, DepthStencil clear)
void  Output (RC<Image>, MipmapLevel, DepthStencil clear)
void  Output (RC<Image>, ImageLayer, DepthStencil clear)
void  Output (RC<Image>, ImageLayer, MipmapLevel, DepthStencil clear)

void  Output (string attachment, RC<Image>)
void  Output (string attachment, RC<Image>, MipmapLevel)
void  Output (string attachment, RC<Image>, ImageLayer)
void  Output (string attachment, RC<Image>, ImageLayer, MipmapLevel)
void  Output (string attachment, RC<Image>, RGBA32f clear)
void  Output (string attachment, RC<Image>, MipmapLevel, RGBA32f clear)
void  Output (string attachment, RC<Image>, ImageLayer, RGBA32f clear)
void  Output (string attachment, RC<Image>, ImageLayer, MipmapLevel, RGBA32f clear)
void  Output (string attachment, RC<Image>, RGBA32u clear)
void  Output (string attachment, RC<Image>, MipmapLevel, RGBA32u clear)
void  Output (string attachment, RC<Image>, ImageLayer, RGBA32u clear)
void  Output (string attachment, RC<Image>, ImageLayer, MipmapLevel, RGBA32u clear)
void  Output (string attachment, RC<Image>, RGBA32i clear)
void  Output (string attachment, RC<Image>, MipmapLevel, RGBA32i clear)
void  Output (string attachment, RC<Image>, ImageLayer, RGBA32i clear)
void  Output (string attachment, RC<Image>, ImageLayer, MipmapLevel, RGBA32i clear)
void  Output (string attachment, RC<Image>, DepthStencil clear)
void  Output (string attachment, RC<Image>, MipmapLevel, DepthStencil clear)
void  Output (string attachment, RC<Image>, ImageLayer, DepthStencil clear)
void  Output (string attachment, RC<Image>, ImageLayer, MipmapLevel, DepthStencil clear)

void  OutputBlend (RC<Image>, EBlendFactor src, EBlendFactor dst, EBlendOp)
void  OutputBlend (RC<Image>, MipmapLevel, EBlendFactor src, EBlendFactor dst, EBlendOp)
void  OutputBlend (RC<Image>, ImageLayer, EBlendFactor src, EBlendFactor dst, EBlendOp)
void  OutputBlend (RC<Image>, ImageLayer, MipmapLevel, EBlendFactor src, EBlendFactor dst, EBlendOp)
void  OutputBlend (RC<Image>, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)
void  OutputBlend (RC<Image>, MipmapLevel, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)
void  OutputBlend (RC<Image>, ImageLayer, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)
void  OutputBlend (RC<Image>, ImageLayer, MipmapLevel, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)

void  OutputBlend (string attachment, RC<Image>, EBlendFactor, EBlendFactor, EBlendOp)
void  OutputBlend (string attachment, RC<Image>, MipmapLevel, EBlendFactor, EBlendFactor, EBlendOp)
void  OutputBlend (string attachment, RC<Image>, ImageLayer, EBlendFactor, EBlendFactor, EBlendOp)
void  OutputBlend (string attachment, RC<Image>, ImageLayer, MipmapLevel, EBlendFactor, EBlendFactor, EBlendOp)
void  OutputBlend (string attachment, RC<Image>, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)
void  OutputBlend (string attachment, RC<Image>, MipmapLevel, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)
void  OutputBlend (string attachment, RC<Image>, ImageLayer, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)
void  OutputBlend (string attachment, RC<Image>, ImageLayer, MipmapLevel, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA)

void  Input (RC<BaseController>)

void  AddPipeline (string file)
void  AddPipelines (string folder)
```

`Input (RC<BaseController>)` - заменяет камеру сцены, например для рисования теней.
`AddPipeline*()` - указывает путь/пути к пайплайнам, геометрия привязывается к пайплайнам, если у них совпадают юниформы.
</details>

#### Стандартные проходы

<details>
<summary>Функции</summary>

```cpp
void  Present (RC<Image>)
void  Present (RC<Image>, MipmapLevel)
void  Present (RC<Image>, ImageLayer)
void  Present (RC<Image>, ImageLayer, MipmapLevel)

void  DbgView (RC<Image>, DbgViewFlags)
void  DbgView (RC<Image>, MipmapLevel, DbgViewFlags)
void  DbgView (RC<Image>, ImageLayer, DbgViewFlags)
void  DbgView (RC<Image>, ImageLayer, MipmapLevel, DbgViewFlags)

void  GenMipmaps (RC<Image>)

void  BuildRTGeometry (RC<RTGeometry>)
void  BuildRTGeometry (RC<RTGeometry>, RC<Buffer> indirect)

void  BuildRTScene (RC<RTScene>)
void  BuildRTScene (RC<RTScene>, RC<Buffer> indirect)

RC<IPass>  RunScript (string scriptPath, RC<Collection>)
RC<IPass>  RunScript (string scriptPath, ScriptFlags flags, RC<Collection>)
```
</details>

#### Глобальные функции

<details>
<summary>Функции</summary>

```cpp
RC<DynamicDim>  SurfaceSize ()

void  Slider (RC<DynamicInt4>, string name, int4 min, int4 max)
void  Slider (RC<DynamicUInt>, string name)
void  Slider (RC<DynamicUInt>, string name, uint min, uint max)
void  Slider (RC<DynamicUInt3>, string name, uint3 min, uint3 max)
void  Slider (RC<DynamicFloat>, string name)
void  Slider (RC<DynamicFloat>, string name, float min, float max)
void  Slider (RC<DynamicFloat4>, string name, float4 min, float4 max)
```
</details>


## Шейдеры

Шейдеры пишутся на GLSL, доступен препроцессор, в котором все функции относящиеся к GLSL начинаются с `gl.`, а типы с `gl::`. Для этого нужно подключить файл `<aestyle.glsl.h>`.

При компиляции шейдера указывается его тип макросом:
* SH_VERT - вершинный
* SH_TESS_CTRL - тесселяция, выбирается уровень детализации
* SH_TESS_EVAL - тесселяция, выполняется для каждой вершины после тесселятора
* SH_GEOM - геометрический
* SH_FRAG - фрагментный/пиксельный
* SH_COMPUTE - вычислительный
* SH_TILE - тайловый (не поддерживается)
* SH_MESH_TASK - task / amplification / object, генерация мешлетов для меш шейдеров
* SH_MESH - меш шейдер
* SH_RAY_GEN - ray generation, генерация лучей
* SH_RAY_AHIT - ray any hit, шейдер выполняется при попадании луча в полигон, используется для прозрачности
* SH_RAY_CHIT - ray closest hit, шейдер выполняется при попадании луча в полигон
* SH_RAY_MISS - ray miss, шейдер для случаев, когда пересечение не найдено
* SH_RAY_INT - ray intersection, шейдер проверка пересечения
* SH_RAY_CALL - callable, шейдер вызывается из любого шейдера трассировки лучей


## Пайплайны

Полное описание скриптов для пайплайнов: [PipelinePack](../../engine/docs/ru/PipelinePack.md).

Класс `SceneGraphicsPass` и `SceneRayTracingPass` определяют технику рендера `rtech` и проход `main`, в которые необходимо добавить специализации пайплайнов.
