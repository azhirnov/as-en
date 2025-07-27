Оглавление:
* [В чем преимущество](#В-чем-преимущество)
* [Bindless](#Bindless)
	- [Bindless в Vulkan](#Bindless-в-Vulkan)
	- [Bindless в Metal](#Bindless-в-Metal)
* [GPU Driven Rendering](#GPU-Driven-Rendering)
* [Тесты производительности](#Тесты-производительности)
* [Итоги](#итоги)


# В чем преимущество

Для начала нужно понять как работает рисование.

В TBR и TBDR архитектурах один варп может закрашивать несколько треугольников, это позволяет уменьшить количество простаивающих потоков.
Но варп может выполнять только один шейдер и один набор состояний.
Так два вызова рисования с одинаковым Pipeline и DescriptorSet могут попасть в один варп в фрагментном шейдере, но если забиндить другой DescriptorSet, то уже нет, а значит в некоторых случаях потребуется в 2 раза больше варпов.
Чем больше плотность геометрии, тем важнее чтобы фрагментные шейдеры полностью заполняли варп.
В этом помогает bindless подход, когда биндится один DescriptorSet, а нужный ресурс выбирается в шейдере по ID.

В дотайловой архитектуре другие особенности.
Часть деталей работы графического пайплайна есть у AMD в статье [Understanding GPU context rolls](https://gpuopen.com/learn/understanding-gpu-context-rolls/).
В железе поддерживается 7 контекстов, которые выполняют команды рисования параллельно, но каждая смена состояний занимает один контекст.
Так bindless вариант выставит состояние один раз и в 6 контекстов запустит параллельное рисование, а выставляя состояния для каждого рисования мы получим 3 состояния и 3 параллельных рисования, то есть в 2 раза меньше работы.


# Bindless

Идея в том, чтобы забиндить все ресурсы один раз, а в шейдере выбирать нужный буфер и текстуры.

Есть старая модель bindless, когда для каждого рисования задается индекс ресурсов, и более новая, когда индекс ресурса меняется в пределах вызова рисования.

Новая модель bindless позволяет использовать GPU Driven Rendering с сортировкой и отсечением невидимой геометрии на стороне ГП.
Также это позволяет использовать техники типа Deferred Texturing и Visibility Buffer, где чтение текстур идет пост-процессом.


## Производительность

Даже старые мобилки, которые поддерживают Vulkan, хорошо справляются с bindless, но есть нюансы.

Так для Adreno пишут:
> It is recommended to use VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER because of how the Adreno GPU works with Bindless mode. When using a combined image sampler, the GPU can use Bindless mode which is more performant. When using separate samplers, it will fall back to a slower mode. Performance deltas have shown a decrease by 2-5% in the fill rate for separate samplers.

И для Mali:
> A bindless descriptor set will work well with combining resources, allowing indexing into texture and buffer arrays.

То есть сэмплеры надо выставить заранее, нельзя использовать конструкции вида `sampler2D( un_Textures[tex_id], un_Samplers[samp_id] )`.



## Bindless в Vulkan

`shaderStorageBufferArrayDynamicIndexing` и другие доступны в ядре Vulkan 1.0, определяет разрешена ли динамическая индексация массива ресурсов. Но все индексы в пределах варпа должны совпадать (uniform), иначе это неопределенное поведение. Если не поддерживается, то доступ к массиву разрешен только по константным значениям.<br/>
Кроме этого можно по-старинке выбирать слой из текстурного массива (sampler2DArray) и слой может быть неоднородным.

<details><summary>Опции shaderSampledImageArrayDynamicIndexing и shaderStorageBufferArrayDynamicIndexing поддерживаются начиная с:</summary>

* Adreno 500
* AMD GCN1 ?
* Apple A9
* Intel gen9
* Mali Midgard Gen3
* NVidia Kepler/GTX600 ?
* PowerVR Series 8

</details>

### Descriptor Indexing

Расширение `VK_EXT_descriptor_indexing` (добавлено в 1.x.72) позволяет использовать bindless-техники. Но кроме поддержки расширения есть различные опции, которые могут не поддерживаться.

`shaderSampledImageArrayNonUniformIndexing` и другие определяет разрешена ли динамическая индексация массива ресурсов, когда индекс в вределах варпа не совпадает (non-uniform).
В шейдере обязательно помечать индекс как [nonuniformEXT](https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GL_EXT_nonuniform_qualifier.txt): `resource[ nonuniformEXT(index) ]`.
Минимальный набор опций, который доступен на большинстве ГП можно посмотреть в [min_nonuniform_desc_idx](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/feature_set/parts/min_nonuniform_desc_idx.as).
Старые ГП поддерживают только `shaderSampledImageArrayNonUniformIndexing`, поэтому для буферов придется использовать RGBA32F текстуры, этот формат поддерживается у большинства ГП, хоть и без линейной фильтрации.

В Vulkan 1.4 расширение `VK_EXT_descriptor_indexing` сделали обязательным в ядре, до этого с 1.2 оно было опционально. Минимально должны поддерживаться `shaderUniformTexelBufferArrayDynamicIndexing` и `shaderStorageTexelBufferArrayDynamicIndexing`.

`shaderSampledImageArrayNonUniformIndexingNative` и другие определяет как будет реализован доступ к ресурсам в случае, когда индекс внутри варпа не совпадает. Если нет поддержки в железе, то код компилируется в waterfall loop - цикл по всем уникальным значениям индекса в пределах варпа.

Для ускоряющих структур (ray tracing acceleration structure) всегда разрешен неоднородный доступ.

<details><summary>Опции *NonUniformIndexing поддерживается начиная с:</summary>

* Adreno 600 *(все опции, включая Native)*
* AMD GCN1 *(все опции, кроме InputAttachment)*
* Apple A9 *(все опции и shaderSampledImageArrayNonUniformIndexingNative)*
* Intel gen9 ? *(все опции)*
* Mali Valhall gen1 *(все опции и shaderStorageBufferArrayNonUniformIndexingNative)*
* Maleoon 9xx *(все опции, кроме InputAttachment)*
* NVidia Kepler/GTX600 ? *(все опции, включая Native)*
* PowerVR B-Series *(все опции, включая Native)*

</details>

Расширение `VK_EXT_descriptor_indexing` также добавляет полезные флаги `VkDescriptorBindingFlags` :
* `VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT` - требует опцию `descriptorBindingPartiallyBound`, помечает дескрипторы, которые __не будут динамически индексироваться__.
	- Позволяет хранить невалидные дескрипторы, если к ним нет статичных обращений из шейдера.
	- Без этого флага драйвер считает, что все дескрипторы валидны.
	- Если есть динамическая индексация, то все элементы массива должны быть валидны. *(В старых примерах флаг используется неправильно, сейчас слои валидации выдают ошибку)*.
* `VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT` - требует опцию `descriptorBindingVariableDescriptorCount`, позволяет сделать последний дескриптор переменного размера. Размер устанавливается при создании дескриптор сета.
* `VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT` - требует опции `descriptorBindingSampledImageUpdateAfterBind` и другие для каждого типа ресурсов. Позволяет обновлять дескрипторы после вызова vkBindDescriptorSet.
	- Обновление должно быть до отправки командного буфера на ГП (сабмита).
	- Будет использоваться последний установленый дескриптор.
	- Дескрипторы могут обновляться из разных потоков, синхронизация нужна только при одновременном обновлении одного дескриптора.
* `VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT` - требует опцию `descriptorBindingUpdateUnusedWhilePending`. Позволяет обновлять неиспользуемые дескрипторы параллельно с выполнением команд на ГП, которые используют этот дескриптор сет.
	- Дескрипторы могут обновляться из разных потоков, синхронизация нужна только при одновременном обновлении дескриптора.
	- Вместе с `VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT` разрешается обновлять дескрипторы, которые не индексируются динамически.
	

### Nonuniform

Однородными являются данные, которые не меняются в пределах вызова команды рисования `vkCmdDraw***`.

Какие данные являются однородными:
* Данные из uniform buffer и push constant.
* `gl_DrawID`.
* Для компьют шейдера: одинаковые значения в пределах воркгруппы, например `gl_WorkGroupID`.

Неоднородные данные:
* `gl_VertexIndex`, `gl_PrimitiveID`, вершинные аттрибуты и тд.
* `gl_LocalInvocationID` и `gl_GlobalInvocationID`.
* `gl_InstanceIndex` на TBDR архитектуре, так как фрагментные шейдеры примитивов из разных инстансов могут попасть в один варп.
* `gl_BaseInstance`, `gl_BaseVertex`, `gl_ViewIndex` ???

При использовании `nonuniform()` компилятор может добавить дополнительные инструкции, но если компилятор знает, что переменная только `uniform`, то проигнорирует `nonuniform()` и лишних инструкций не будет.<br/>
Пример [UniqueIDs](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/samples-compute/UniqueIDs-1.as) показывает как компилятор превращает неоднородный доступ к ресурсам в однородный.

Пример [BrokenNonuniform](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/tests/BrokenNonuniform.as) показывает, что будет если не использовать `nonuniform()`.
Почти на всех протестированных ГП драйвер сам обнаруживает неоднородность и `nonuniform()` ни на что не влияет, поэтому такие ошибки сложно отловить. Только на AMD GCN берется один индекс на варп и ошибки сразу проявляются.

Подробнее можно почитать в [Vulkan Samples: descriptor indexing](https://github.com/KhronosGroup/Vulkan-Samples/tree/main/samples/extensions/descriptor_indexing#non-uniform-indexing-enabling-advanced-algorithms).

Также есть параметр `quadDivergentImplicitLod`, который показывает может ли драйвер рассчитать LOD для текстуры, когда индекс меняется в пределах квадрата.

> If the image or sampler object used by an implicit derivative image instruction is not uniform across the quad and quadDivergentImplicitLod is not supported, then the derivative and LOD values are undefined.

Проблем не возникает при одинаковых индексах на треугольник, так как даже на мобилках при объединении нескольких треугольников в один варп, всегда закрашивание идет квадратами.
Для visibility buffer производные и так считаются попиксельно.
Но остаются рельефное текстурирование и постпроцессы с трассировкой, в которых возможно попиксельное вырождение.

Если все же нужно менять индекс попиксельно, то требуется явно посчитать производные:
```
float2 dx = dFdx(uv) * Exp2(bias);
float2 dy = dFdy(uv) * Exp2(bias);
textureGrad( un_Textures[nonuniform(tex_id)], dx, dy );
```

Поддержка `quadDivergentImplicitLod` зависит от производителя, а не версии архитектуры.
Так параметр поддерживается на Adreno, Intel, NVidia, PowerVR и не поддерживается на AMD, Apple, Mali, VideoCore, Maleoon.

Пример [QuadDivergentImplicitLod](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/tests/QuadDivergentImplicitLod.as) покажет появляется ли ошибка, если не использовать `textureGrad()`.
На AMD RX570 разница только в `textureQueryLod().x`.
NVidia, Intel и PowerVR показали небольшое отличие между `texture()` и `textureGrad()`, это может быть связано с меньшей точностью при неявном расчете дериватив.

С Mali оказалось сложнее - `textureQueryLod( nonuniform(...))` не работает вовсе, а `textureGrad( nonuniform(...), dFdx(), dFdy() )` показывает худшую фультрацию вдали. В рекомендациях по оптимизации для Mali даже не советуют использовать `textureGrad` если есть такая возможность.

Более старые Mali Midgard не поддерживают `nonuniform()`, но работают также как более новые Mali Valhall.
На старом Adreno 500 неоднородный доступ не работает вовсе - чтение текстуры возвращает черный цвет.


### Device Address

Расширение `VK_KHR_buffer_device_address` позволяет использовать указатели на память буфера. Адрес получается из `ulong` или `uint2` типа.<br/>
[Пример](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/samples-2d/BufferReference.as) с бинарным деревом.

<details><summary>Поддерживается начиная с:</summary>

* Adreno 600
* AMD GCN1
* Intel gen9 ?
* Mali Bifrost gen1
* Maleoon 9xx
* NVidia Kepler/GTX600 ?
* PowerVR Series 8

</details>


### Descriptor Buffer

Расширение `VK_EXT_descriptor_buffer` упрощает работу с дескрипторами, теперь вместо абстрактных дескриптор сетов и пулов будет буфер, который хранит дескрипторы.
Подробнее в [proposal](https://github.com/KhronosGroup/Vulkan-Docs/blob/main/proposals/VK_EXT_descriptor_buffer.adoc) и [блоге](https://www.khronos.org/blog/vk-ext-descriptor-buffer).

Обновление данных.<br/>
Теперь обновление дескрипторов аналогично обновлению буфера.
Чтение дескрипторов происходит в шейдере, поэтому обновление должно быть синхронизированно с ними, например:
```
dstStage = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT
dstAccess = VK_ACCESS_2_DESCRIPTOR_BUFFER_READ_BIT_EXT 
```
Аналогично, перед обновлением нужно дождаться пока завершится шейдер.

Так же как с дескриптор сетами все дескрипторы, которые используются динамически должны быть валидны.

В расширении `VK_EXT_robustness2` появилась возможность использовать нулевые дескрипторы, для этого требуется опция `nullDescriptor`.
Говорят, что именно `nullDescriptor` на производительность [не влияет](https://github.com/KhronosGroup/Vulkan-Docs/issues/1971#issuecomment-1308974805).
Тогда как другие опции из robustness расширений могут сильно влиять на производительность.

<details><summary>Поддерживается начиная с</summary>

* Adreno 800/X1 (начиная с 512.800.0 драйвера)
* Adreno Turnip 600 (открытый драйвер)
* AMD GCN4
* Intel Xe-HP
* Intel Xe-LP
* Mali Valhall gen3 (начиная с 53.0.0 драйвера)
* NVidia Kepler/GTX7xx

</details>


### Лимиты

Более новые ГП поддерживают сотни текстур на шейдер и часто такие ГП хорошо совместимы с bindless подходом.
Но встречаются еще старые модели, где ограничение в 16-32 текстуры.

<details><summary>Сотни текстур поддерживаются:</summary>

* Adreno 500 (128 текстур, 158 ресурсов всего)
* Adreno 600 (по 524'288 каждого ресурса, 1'572'864 в сумме)
* AMD GCN1 (по 4'294'967'295 каждого ресурса)
* Apple M1 (128 текстур, 159 ресурсов всего) ???
* Intel gen9 (200 текстур, 200 ресурсов всего)
* Intel Xe-HP, Xe+ LP (по 33'554'432 каждого ресурса)
* Mali Bifrost gen1 (256 текстур, 361 ресурсов всего)
* Mali Valhall gen1 (по 500'000 каждого ресурса, 500'000 в сумме)
* Maleoon 9xx (по 500'000 каждого ресурса, 2'000'016 в сумме)
* NVidia Kepler/GTX600 (по 1'048'576 каждого ресурса)
* PowerVR Series 9 (48 текстур, 224 ресурсов всего)
* PowerVR B Series (по 4'294'967'295 каждого ресурса)

</details>


## Bindless в Metal


# GPU Driven Rendering

Bindless техники позволяют перенести больше логики на сторону ГП.

Главный минус такого подхода - сложно искать ошибки. При чтении/записи за пределы массива не всегда происходит падение, падение происходит при обращении за пределы страницы памяти, что добавляет случайности.


## Prefix Scan

Один из этапов GPU Driven подхода - проверка видимости объектов и их удаление из очереди рисования.
Проверка видимости выполняется через frustum culling, [HiZ](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/GeometryCulling-ru.md#hierarchy-z-buffer-hzb-hiz), [Raster occlusion](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/GeometryCulling-ru.md#raster-occlusion) и тд.
После проверки видимости получаем массив из ID объектов и пустые элементы, чтобы сгруппировать ID используется prefix scan / prefix sum алгоритм.
Примеры: [PrefixScan-1](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/samples-compute/PrefixScan-1.as), [PrefixScan-2](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/samples-compute/PrefixScan-2.as).

Если порядок ID не важен, то используется более простой алгоритм с атомиком. 
Пример: [PrefixScan-3](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/samples-compute/PrefixScan-3.as), мелькание возникает из-за перемешивания ID при использовании атомика.


## Radix Sort

Одной из важных оптимизаций рисования является сортировка по расстоянию от камеры, это позволяет отсечь большинство невидимых пикселей на earlyZS этапе.
Эффективнее всего производить сортировку после проверки видимости, так как остается меньше объектов и сортировка займет меньше итераций.

С другой стороны при отправке сцены на рисование объекты идут уже частично отсортированными. Например это может быть quad-tree или octree, каждый узел содержит набор геометрии, это может быть тысячи мешлетов по 64 треугольника.
При обходе дерева от камеры, узлы уже добавляются в нужном порядке, остается только сделать сортировку внутри узла и на границе.

Для сортировки лучше всего нарезать экран на тайлы и произвести сортировку мешлетов внутри тайла.

Примеры: ...


## Multi Draw Indirect

Расширение `VK_KHR_draw_indirect_count` (добавлено в ядро 1.2) не так распространено и код получается не универсальным, поэтому лучше не использовать.<br/>

<details><summary>Доступно начиная с:</summary>

* AMD GCN1
* Adreno 600
* Samsung Xclipse 530
* PowerVR Series 8
* NVidia Kepler/GTX600 ?
* Intel gen9 ?
* Mali Valhall gen3

Не поддерживается на Apple, так как в Metal сделан другой механизм.

</details>

В ядре Vulkan 1.0 достпна опция `drawIndirectFirstInstance`, которая позволяет использовать поле `firstInstance` структуры `VkDrawIndexedIndirectCommand`.<br/>

<details><summary>Опция не поддерживается на небольшом количестве устройств:</summary>

* Adreno 500
* PowerVR Series 6

</details>

Также есть опция `multiDrawIndirect` и лимит `maxDrawIndirectCount`, которые влияют на аргумент `drawCount` функций `vkCmdDrawIndexedIndirect()`.
Часто опция поддерживается, но `maxDrawIndirectCount=1`, что равноценно отсутствию поддержки.<br/>

<details><summary>Не поддерживается:</summary>

* Mali до Valhall gen2 включительно (T880, G71, G72, G76, G77, G78)
* Mali Panfrost драйвер под Linux
* Adreno 500

</details>

Когда `maxDrawIndirectCount=1`, то остается вариант использовать инстансинг с фиксированным количеством индексов.
Так геометрия разбивается на мешлеты одинакового размера, если нужно меньше вершин, то лишние вершины пишут NaN в позицию.


## Per Instance Vertex Rate

Также известный как Vertex Attribute Divisor.
Позволяет передавать данные инстанса через вершинный буфер, что может быть быстрее на старом железе, где медленно работает storage buffer.

Такой подход описан в [Optimizing the Graphics Pipeline with Compute](https://ubm-twvideo01.s3.amazonaws.com/o1/vault/gdc2016/Presentations/Wihlidal_Graham_OptimizingTheGraphics.pdf) (слайд 23).


# Тесты производительности

<details><summary>Старые тесты</summary>

1.1. Nonuniform stress test v2<br/>
Разница в производительности между использованием `nonuniform()` и выбором слоя из Texture2DArray.
Чтобы в варп попадали разные индексы используется хэш от `gl_FragCoord` с двумя режимами: квадрат 2х2 и попиксельно.<br/>
Вариант per object больше приближен к реальному использованию, тогда как per quad и per pixel это стресс-тест, но могут возникнуть: per quad для микротреугольников, per pixel в visibility buffer.<br/>
Тест сравнивает производительность разного доступа к ресурсам при низкой нагрузке на другие системы, но не показывает влияния bindless на производительность в целом.<br/>
Исходники: [скрипт](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/NonUniform-Stress.as), [шейдер](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipeline_inc/NonUniform-Stress-shared.as).

1.2. Nonuniform with depth pre-pass<br/>
Сделаны примитивные объекты в виде повернутых прямоугольников, вытянутые формы приводят к тому, что больше треугольников попадают в варп и сильнее проявляется неоднородность индексов.
Показывает разницу в производительности между использованием `nonuniform()` и выбором слоя из Texture2DArray.
Можно менять детализацию текстур, чтобы определить насколько bindless влияет на производительность при нормальной нагрузке на память и при пониженой, когда читаются нижние мип-уровни.<br/>
Исходники: [скрипт](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/NonUniform-DPP.as), [шейдер](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipeline_inc/NonUniform-shared.as).

1.3. Nonuniform with visibility buffer<br/>
Аналогично depth pre-pass, но вызывается меньше фрагментных шейдеров и больше уникальных индексов в варпе.
На слабом железе сильно нагружается ALU из-за чего нагрузка на текстуры оказалась минимальной.<br/>
Исходники: [скрипт](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/NonUniform-VB.as), [шейдер](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipeline_inc/NonUniform-VB-shared.as).

Все три теста оказались не достаточно информативными, хватило данных чтобы разбить ГП по группам, но сложно оценить как влияет увеличение общего количества текстур, увеличение количества чтений текстур для PBR, parallax mapping, ландшафта, где нагрузка увеличивается в 4 раза.

</details>

1.1. Nonuniform PBR<br/>
Разница в производительности между использованием `nonuniform()` и выбором слоя из Texture2DArray.
Чтобы в варп попадали разные индексы используется хэш от `gl_FragCoord` с двумя режимами: квадрат 2х2 и попиксельно.<br/>
Для имитации PBR используются LODы с разным количеством текстур.

1.2. Nonuniform parallax<br/>
Разница в производительности между использованием `nonuniform()` и выбором слоя из Texture2DArray.
Чтобы в варп попадали разные индексы используется хэш от `gl_FragCoord` с двумя режимами: квадрат 2х2 и попиксельно.<br/>
Используется цикл до 64 шагов по одной текстуре для имитации рельефного текстурирования.


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


# Итоги

Внезапно, только Adreno 660 плохо справился с bindless.
У Intel gen9.5 возникли проблемы с bindless texture + immutable sampler, но на аналогичной по производительности модели N150 (gen12 ?) эту проблему исправили.

Получилось 3 группы:
1. Когда bindless texture + imutable sampler быстрее. Это AMD 780M RADV.
2. Когда bindless texture + bindless sampler быстрее. Это Intel UHD620, PowerVR BXM, Adreno 660, Mali G57.
3. Одинаковая производительность у bindless. Это Intel N150, Mali G610, AMD 780M PRO и VLK, AMD RX570, NV RTX 2080.

Тесты показали, что `*NonUniformIndexingNative` параметры ни на что не влияет: производительность всегда снижается, `nonuniform()` нужен только для AMD GCN.

На старых устройствах Mali T830 и Adreno 505 нет поддержки `nonuniform()` и забиндить можно всего 16 текстур.
Зато даже старый Mali T830 поддерживает неоднородный доступ к текстурам.
