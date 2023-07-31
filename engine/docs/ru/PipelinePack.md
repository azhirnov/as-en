__Особенности движка__:
* Предкомпилированные ресурсы хранятся в `PipelinePack`.
* Все шейдеры в движке должны быть предкомпилированы.
* Все рендер пассы заданы заранее, есть переменный формат `EPixelFormat::SwapchainColor` который определяется на этапе загрузки `PipelinePack`.
* Для каждого ресурса должен быть задан `FeatureSet`, он нужен для валидации.
* Если ресурсы скомпилировались с заданным `FeatureSet`, то они будут работать на совместимых ГПУ.

Все ресурсы определяются в скриптах (.as файлы). Макрос `SCRIPT` позволяет выделить только часть файла под скрипты, а остальное, например, под код шейдеров.


## CompatibleRenderPass

Совместимый рендер пасс используется для создания пайплайнов и фрейбуферов в Vulkan.

`CompatibleRenderPass (string name)`<br/>
Конструктор, принимает название рендер пасса.

`void  AddFeatureSet (string name)`<br/>
Добавляет `FeatureSet`, который задает лимиты для рендер пасса.

`RC<Attachment>  AddAttachment (string name)`<br/>
Добавляет аттачмент (текстура для рендеринга, либо маска для shading rate).

`void  AddSubpass (string name)`<br/>
Добавляет сабпасс.

`void  Print () const`<br/>
Печатает подробную информацию о рендер пасса, нужен для отладки.

`RC<RenderPass>  AddSpecialization (string name)`<br/>
Создает специализацию рендер пасса.


### RenderPass

Специализация рендер пасса. Используется в технике рендеринга.

Пайплайны создаются с совместимым рендер пассам и могут использоваться со всеми его специализациями.

`RC<AttachmentSpec>  AddAttachment (string name)`<br/>
Добавляет специализацию для аттачмента.

`void  GenOptimalLayouts ()`<br/>
Вызывается вместо `AddAttachment()` и автоматически задает лейауты для всех аттачментов.

`void  Print () const`<br/>
Печатает подробную информацию о рендер пасса, нужен для отладки.


### Attachment

`EPixelFormat format`<br/>
Задает формат аттачмента.

`MultiSamples samples`<br/>
Задает количество сэмплов.

`void  Usage (string subpass, EAttachment usage)`<br/>
`void  Usage (string subpass, EAttachment usage, ShaderIO inOrOut)`<br/>
`void  Usage (string subpass, EAttachment usage, ShaderIO in, ShaderIO out)`<br/>
`void  Usage (string subpass, EAttachment usage, uint2 tileSize)`<br/>
Задает как аттачмент будет использоваться в каждом сабпассе.

`void  Print () const`<br/>
Печатает подробную информацию об аттачменте, нужен для отладки.


### AttachmentSpec

`EAttachmentLoadOp loadOp`<br/>
Задает функцию загрузки содержимого аттачмента.

`EAttachmentStoreOp storeOp`<br/>
Задает функцию сохранения содержимого аттачмента.

`void  Layout (string subpass, EResourceState state)`<br/>
Задает лейаут аттачмента для сабпасса.

`void  GenOptimalLayouts ()`<br/>
`void  GenOptimalLayouts (EResourceState initialState, EResourceState finalState)`<br/>
Вызывается вместо `Layout()` и автоматически задает лейауты для сабпассов.

`void  Print () const`<br/>
Печатает подробную информацию об аттачменте, нужен для отладки.


## RenderTechnique

`RenderTechnique (string name)`<br/>
Конструктор, принимает название техники.

`void  AddFeatureSet (string name)`<br/>
Добавляет `FeatureSet`, который задает лимиты для пайплайна.

`RC<GraphicsPass>  AddGraphicsPass (string name)`<br/>
`RC<ComputePass>  AddComputePass (string name)`<br/>
Добавляет проход рендер техники.

`RC<GraphicsPass>  CopyGraphicsPass (string newName, string rtech, string pass)`<br/>
`RC<ComputePass>  CopyComputePass (string newName, string rtech, string pass)`<br/>
Копирует специализации пайплайнов из другой техники рендеринга.


### GraphicsPass

`void  SetRenderPass (string renderPass, string subpass)`<br/>
Задает рендер пасс. Каждый проход техники рендеринга соответствует одному сабпасу рендер пасса. Следующие проходы должны использовать следующие сабпасы.

`void  SetRenderState (RenderState)`<br/>
`void  SetRenderState (string name)`<br/>
Задает общие рендер стейты для пайплайнов.

`void  SetMutableStates (EMutableRenderState)`<br/>
Задает состояния, которые могут отличаться в пайплайнах.

`void  SetDSLayout (string name)`<br/>
Задает общий дескриптор сет для прохода.


### ComputePass

`void  SetDSLayout (string name)`<br/>
Задает общий дескриптор сет для прохода.


## Shader

`string source`<br/>
Код шейдера можно задать из скрипта.

`string file`<br/>
Файл с исходным кодом шейдера.

`EShaderVersion version`<br/>
Версия шейдера.

`EShaderOpt options`<br/>
Дополнительный флаги.

`EShader type`<br/>
Тип шейдера. Опционально, за исключением шейдеров для трассировки лучей.

`void  AddSpec (EValueType type, string constName)`<br/>
Добавить константу специализации.

`void  Define (string)`<br/>
Добавить макрос в виде `MACRO=<value>`.

`void  LoadSelf ()`<br/>
Загрузить текущий файл как исходный код шейдера.


### Шейдеры на GLSL

Совместимы с Vulkan и Metal бэкэндом. Для Metal используется SPIRV-Cross для конвертации в msl, а затем все компилируется в байткод.

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


### Шейдеры на MSL


## ComputePipeline

`ComputePipeline (string name)`<br/>
Конструктор, принимает название пайплайна.

`void  Define (string)`<br/>
Добавить макрос в виде `MACRO=<value>`.

`void  SetShader (RC<Shader>)`<br/>
Задает шейдер, вызывается один раз.

`void  AddFeatureSet (string name)`<br/>
Добавляет `FeatureSet`, который задает лимиты для пайплайна.

`void  SetLayout (string name)`<br/>
`void  SetLayout (RC<PipelineLayout>)`<br/>
Задает `PipelineLayout` для пайплайна, при смене лейаута необходимо заново биндить дескриптор сеты (TODO: ссылка на рекомендации по оптимизации).

`RC<ComputePipelineSpec>  AddSpecialization (string name)`<br/>
Создает специализацию пайплайна.


### ComputePipelineSpec

Специализация пайплайна использует одинаковый лейаут, одинаковый бинарник шейдера и тд, различия только в некоторых параметрах при создании пайплайна.
Тем же способом может быть создан пайплайн в рантайме. Специализация пайплайнов загружается сразу при загрузке связанных с ними техник рендеринга.

`void  SetSpecValue (string specName, uint)`<br/>
`void  SetSpecValue (string specName, int)`<br/>
`void  SetSpecValue (string specName, float)`<br/>
Задает константу специализации.

`void  SetDynamicState (EPipelineDynamicState)`<br/>
Задает динамические состояния пайплайна. Для компьют пайплайна нет флагов.

`void  SetOptions (EPipelineOpt)`<br/>
Задает дополнительные опции для пайплайна. Поддерживаются: Optimize, CS_DispatchBase.

`void  SetLocalSize (uint x)`<br/>
`void  SetLocalSize (uint x, uint y)`<br/>
`void  SetLocalSize (uint x, uint y, uint z)`<br/>
Задается размер рабочей группы (local size).

`void  AddToRenderTech (string rtech, string pass)`<br/>
Добавить пайплайн к проходу техники рендеринга. Проход должен быть `ComputePass`.


### Shader

`void  ComputeSpec1 ()`<br/>
`void  ComputeSpec2 ()`<br/>
`void  ComputeSpec3 ()`<br/>
Задаются константы специализации для размера рабочей группы (local size).

`void  ComputeSpecAndDefault (uint x)`<br/>
`void  ComputeSpecAndDefault (uint x, uint y)`<br/>
`void  ComputeSpecAndDefault (uint x, uint y, uint z)`<br/>
Задаются константы специализации для размера рабочей группы (local size) и значения по умолчанию.

`void  ComputeLocalSize (uint x)`<br/>
`void  ComputeLocalSize (uint x, uint y)`<br/>
`void  ComputeLocalSize (uint x, uint y, uint z)`<br/>
Задается размер рабочей группы (local size).


## GraphicsPipeline

`GraphicsPipeline (string name)`<br/>
Конструктор, принимает название пайплайна.

`void  Define (string)`<br/>
Добавить макрос в виде `MACRO=<value>`.

`void  SetVertexShader (RC<Shader>)`<br/>
`void  SetTessControlShader (RC<Shader>)`<br/>
`void  SetTessEvalShader (RC<Shader>)`<br/>
`void  SetGeometryShader (RC<Shader>)`<br/>
`void  SetFragmentShader (RC<Shader>)`<br/>
Задает шейдер.

`void  TestRenderPass (string renderPass, string subpass)`<br/>
Проверяет, что вывод фрагментного шейдера совместим с рендер пассом.

`void  SetFragmentOutputFromRenderTech (string rtech, string pass)`<br/>
`void  SetFragmentOutputFromRenderPass (string renderPass, string subpass)`<br/>
Задает вывод фрагментного шейдера совместимый с рендер пассом.

`void  AddFeatureSet (string name)`<br/>
Добавляет `FeatureSet`, который задает лимиты для пайплайна.

`void  SetLayout (string name)`<br/>
`void  SetLayout (RC<PipelineLayout>)`<br/>
Задает `PipelineLayout` для пайплайна, при смене лейаута необходимо заново биндить дескриптор сеты (TODO: ссылка на рекомендации по оптимизации).

`void  SetVertexInput (string name)`<br/>
`void  SetVertexInput (RC<VertexBufferInput>)`<br/>
Задает вершинные аттрибуты.

`void  SetShaderIO (EShader in, EShader out, string structType)`<br/>
Задает тип структуры, в которой передаются данные между шейдерами.

`RC<GraphicsPipelineSpec>  AddSpecialization (string name)`<br/>
Создает специализацию пайплайна.


### GraphicsPipelineSpec

Специализация пайплайна использует одинаковый лейаут, одинаковый бинарник шейдера и тд, различия только в некоторых параметрах при создании пайплайна.
Тем же способом может быть создан пайплайн в рантайме. Специализация пайплайнов загружается сразу при загрузке связанных с ними техник рендеринга.

`void  SetSpecValue (string specName, uint)`<br/>
`void  SetSpecValue (string specName, int)`<br/>
`void  SetSpecValue (string specName, float)`<br/>
Задает константу специализации.

`void  SetDynamicState (EPipelineDynamicState)`<br/>
Задает динамические состояния пайплайна. Поддерживается: StencilCompareMask, StencilWriteMask, StencilReference, DepthBias, BlendConstants, FragmentShadingRate.

`void  SetOptions (EPipelineOpt)`<br/>
Задает дополнительные опции для пайплайна. Поддерживается: Optimize.

`void  SetRenderState (string name)`<br/>
`void  SetRenderState (RenderState)`<br/>
Задает состояние фиксированного рендер пайплайна.

`void  SetVertexInput (string name)`<br/>
`void  SetVertexInput (RC<VertexBufferInput>)`<br/>
Задает вершинные аттрибуты.

`void  SetViewportCount (uint)`<br/>
Задает количество виюпортов, если поддерживается multiviewport.

`void  AddToRenderTech (string rtech, string pass)`<br/>
Добавить пайплайн к проходу техники рендеринга. Проход должен быть `GraphicsPass`.


### Shader

`void  TessPatchSize (uint)`<br/>
Задается количество вершин в патче при использовании тесселяции.

`void  TessPatchMode (ETessPatch, ETessSpacing, bool ccw)`<br/>
Задается режим тесселяции.


## MeshPipeline

`MeshPipeline (string name)`<br/>
Конструктор, принимает название пайплайна.

`void  Define (string)`<br/>
Добавить макрос в виде `MACRO=<value>`.

`void  SetTaskShader (RC<Shader>)`<br/>
`void  SetMeshShader (RC<Shader>)`<br/>
`void  SetFragmentShader (RC<Shader>)`<br/>
Задает шейдер.

`void  TestRenderPass (string renderPass, string subpass)`<br/>
Проверяет, что вывод фрагментного шейдера совместим с рендер пассом.

`void  SetFragmentOutputFromRenderTech (string rtech, string pass)`<br/>
`void  SetFragmentOutputFromRenderPass (string renderPass, string subpass)`<br/>
Задает вывод фрагментного шейдера совместимый с рендер пассом.

`void  AddFeatureSet (string name)`<br/>
Добавляет `FeatureSet`, который задает лимиты для пайплайна.

`void  SetLayout (string name)`<br/>
`void  SetLayout (RC<PipelineLayout>)`<br/>
Задает `PipelineLayout` для пайплайна, при смене лейаута необходимо заново биндить дескриптор сеты (TODO: ссылка на рекомендации по оптимизации).

`void  SetShaderIO (EShader in, EShader out, string structType)`<br/>
Задает тип структуры, в которой передаются данные между шейдерами.

`RC<MeshPipelineSpec>  AddSpecialization (string name)`<br/>
Создает специализацию пайплайна.

### MeshPipelineSpec

Специализация пайплайна использует одинаковый лейаут, одинаковый бинарник шейдера и тд, различия только в некоторых параметрах при создании пайплайна.
Тем же способом может быть создан пайплайн в рантайме. Специализация пайплайнов загружается сразу при загрузке связанных с ними техник рендеринга.

`void  SetSpecValue (string specName, uint)`<br/>
`void  SetSpecValue (string specName, int)`<br/>
`void  SetSpecValue (string specName, float)`<br/>
Задает константу специализации.

`void  SetDynamicState (EPipelineDynamicState)`<br/>
Задает динамические состояния пайплайна. Поддерживается: StencilCompareMask, StencilWriteMask, StencilReference, DepthBias, BlendConstants, FragmentShadingRate.

`void  SetOptions (EPipelineOpt)`<br/>
Задает дополнительные опции для пайплайна. Поддерживается: Optimize.

`void  SetRenderState (RenderState)`<br/>
`void  SetRenderState (string name)`<br/>
Задает состояние фиксированного рендер пайплайна.

`void  SetViewportCount (uint)`<br/>
Задает количество виюпортов, если поддерживается multiviewport.

`void  AddToRenderTech (string rtech, string pass)`<br/>
Добавить пайплайн к проходу техники рендеринга. Проход должен быть `GraphicsPass`.

`void  SetTaskLocalSize (uint x)`<br/>
`void  SetTaskLocalSize (uint x, uint y)`<br/>
`void  SetTaskLocalSize (uint x, uint y, uint z)`<br/>
Задается размер рабочей группы (local size) для task шейдера.

`void  SetMeshLocalSize (uint x)`<br/>
`void  SetMeshLocalSize (uint x, uint y)`<br/>
`void  SetMeshLocalSize (uint x, uint y, uint z)`<br/>
Задается размер рабочей группы (local size) для mesh шейдера.


### Shader

`void  MeshSpec1 ()`<br/>
`void  MeshSpec2 ()`<br/>
`void  MeshSpec3 ()`<br/>
Задаются константы специализации для размера рабочей группы (local size).

`void  MeshSpecAndDefault (uint x)`<br/>
`void  MeshSpecAndDefault (uint x, uint y)`<br/>
`void  MeshSpecAndDefault (uint x, uint y, uint z)`<br/>
Задаются константы специализации для размера рабочей группы (local size) и значения по умолчанию.

`void  MeshLocalSize (uint x)`<br/>
`void  MeshLocalSize (uint x, uint y)`<br/>
`void  MeshLocalSize (uint x, uint y, uint z)`<br/>
Задается размер рабочей группы (local size).

`void  MeshOutput (uint maxVertices, uint maxPrimitives, EPrimitive topology)`<br/>
Задается размер и топология выходных данных для меш шейдера.


## RayTracingPipeline

`RayTracingPipeline (string name)`<br/>
Конструктор, принимает название пайплайна.

`void  Define (string)`<br/>
Добавить макрос в виде `MACRO=<value>`.

`void  AddFeatureSet (string name)`<br/>
Добавляет `FeatureSet`, который задает лимиты для пайплайна.

`void  SetLayout (string name)`<br/>
`void  SetLayout (RC<PipelineLayout>)`<br/>
Задает `PipelineLayout` для пайплайна, при смене лейаута необходимо заново биндить дескриптор сеты (TODO: ссылка на рекомендации по оптимизации).

`void  AddGeneralShader (string name, RC<Shader>)`<br/>
Добавляет RayGen, RayCallable и RayMiss шейдеры. Тип шейдера должен быть определен.

`void  AddTriangleHitGroup (string name, RC<Shader> closestHit, RC<Shader> anyHit)`<br/>
Добавляет хит-группу с RayClosestHit шейдером и, опционально, с RayAnyHit шейдером.

`void  AddProceduralHitGroup (string name, RC<Shader> intersection, RC<Shader> closestHit, RC<Shader> anyHit)`<br/>
Добавляет хит-группу с RayIntersection шейдером и, опционально, с RayClosestHit и RayAnyHit шейдерами.

`RC<RayTracingPipelineSpec>  AddSpecialization (string name)`<br/>
Создает специализацию пайплайна.


### RayTracingPipelineSpec

Специализация пайплайна использует одинаковый лейаут, одинаковый бинарник шейдера и тд, различия только в некоторых параметрах при создании пайплайна.
Тем же способом может быть создан пайплайн в рантайме. Специализация пайплайнов загружается сразу при загрузке связанных с ними техник рендеринга.

`void  SetSpecValue (string specName, uint)`<br/>
`void  SetSpecValue (string specName, int)`<br/>
`void  SetSpecValue (string specName, float)`<br/>
Задает константу специализации.

`void  SetDynamicState (EPipelineDynamicState)`<br/>
Задает динамические состояния пайплайна. Поддерживается: RTStackSize.

`void  SetOptions (EPipelineOpt)`<br/>
Задает дополнительные опции для пайплайна. Поддерживается: Optimize.

`void  AddToRenderTech (string rtech, string pass)`<br/>
Добавить пайплайн к проходу техники рендеринга. Проход должен быть `ComputePass`.

`void  MaxRecursionDepth (uint)`<br/>
Задает глубину рекурсии.


## RayTracingShaderBinding

При компиляции пайплайнов можно задать таблицу шейдеров для трассировки лучей. Для этого в скриптах есть класс `RayTracingShaderBinding`.


`RayTracingShaderBinding (RC<RayTracingPipelineSpec>, string sbtName)`<br/>
Конструктор.

`void  HitGroupStride (uint)`<br/>
Задает размер таблицы для хит-групп.

`void  BindRayGen (string name)`<br/>
Задает RayGen шейдер, который добавлен через `RayTracingPipeline::AddGeneralShader()`.

`void  BindMiss (string name, MissIndex)`<br/>
Задает miss шейдер, который добавлен через `RayTracingPipeline::AddGeneralShader()`.<br/>
Вызывается, если нет пересечений.

`void  BindHitGroup (string name, InstanceIndex, RayIndex)`<br/>
Задает хит-группу для инстанса и типа луча.<br/>
Хит-группа добавляет через `RayTracingPipeline::AddTriangleHitGroup()` и `RayTracingPipeline::AddProceduralHitGroup()`.<br/>
`RayIndex` должен быть меньше значения переданного в `HitGroupStride()`.

`void  BindCallable (string name, CallableIndex)`<br/>
Задает callable шейдер, который добавлен через `RayTracingPipeline::AddGeneralShader()`.


Пример:

```cpp
RC<RayTracingShaderBinding>  sbt;

sbt.BindRayGen( "Main" );

sbt.HitGroupStride( 2 );

sbt.BindMiss( "PrimaryMiss", MissIndex(0) );  // traceRays() with missIndex = 0
sbt.BindMiss( "ShadowMiss",  MissIndex(1) );  // traceRays() with missIndex = 1

sbt.BindHitGroup( "Primary", InstanceIndex(0), RayIndex(0) );  // traceRays() with sbtRecordOffset = 0
sbt.BindHitGroup( "Shadow",  InstanceIndex(0), RayIndex(1) );  // traceRays() with sbtRecordOffset = 1
```
