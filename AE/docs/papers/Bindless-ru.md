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
* NVidia Kepler ?
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

<details><summary>Опции *NonUniformIndexing поддерживается начиная с:</summary>

* Adreno 600 *(все опции, включая Native)*
* AMD GCN1 *(все опции, кроме InputAttachment)*
* Apple A9 *(все опции и shaderSampledImageArrayNonUniformIndexingNative)*
* Intel gen9 ? *(все опции)*
* Mali Valhall gen1 *(все опции и shaderStorageBufferArrayNonUniformIndexingNative)*
* Maleoon 9xx *(все опции, кроме InputAttachment)*
* NVidia Kepler ? *(все опции, включая Native)*
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

Какие данные являются однородными:
* Данные из uniform buffer и push constant.
* При чтении массивов из uniform buffer индекс должен быть однородным.
* `gl_DrawID`.

Неоднородные данные:
* `gl_VertexIndex`, `gl_PrimitiveID`, вершинные аттрибуты и тд.
* `gl_InstanceIndex` на TBDR архитектуре, так как фрагментные шейдеры примитивов из разных инстансов могут попасть в один варп.


### Device Address

Расширение `VK_KHR_buffer_device_address` позволяет использовать указатели на память буфера. Адрес получается из `ulong` млм `uint2` типа.
[Пример](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/samples-2d/BufferReference.as) с бинарным деревом.

<details><summary>Поддерживается начиная с:</summary>

* Adreno 600
* AMD GCN1
* Intel gen9 ?
* Mali Bifrost gen1
* Maleoon 9xx
* NVidia Kepler ?
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
Пример: [PrefixScan-3](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/samples-compute/PrefixScan-3.as).


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
* NVidia Kepler ?
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



# Тесты производительности

1. Nonuniform.<br/>
Разница в производительности между использованием `nonuniform()` и выбором слоя из Texture2DArray.
Чтобы в варп попадали разные индексы используется хэш от `gl_FragCoord` с двумя режимами: квадрат 2х2 и попиксельно.<br/>
Вариант per object больше приближен к реальному использованию, тогда как per quad и per pixel это стресс-тест, но могут возникнуть: per quad для микротреугольников, per pixel в visibility buffer.<br/>

Тест сравнивает производительность разного доступа к ресурсам при низкой нагрузке на другие системы, но не показывает влияния bindless на производительность в целом, поэтому тест будет заменен на новый.

Исходники: [скрипт](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/NonUniform-Tex.as), [шейдер](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipeline_inc/NonUniformTex-shared.as).


**Результаты**
* [AMD RX570](#AMD-RX570)
* [AMD Radeon 780M, AMDPRO](#AMD-Radeon-780M-AMDPRO)
* [AMD Radeon 780M, AMDVLK](#AMD-Radeon-780M-AMDVLK)
* [AMD Radeon 780M, RADV](#AMD-Radeon-780M-RADV)
* [Nvidia RTX 2080](#Nvidia-RTX-2080)
* [ARM Mali G57](#ARM-Mali-G57)
* [ARM Mali G610](#ARM-Mali-G610)
* [Adreno 660](#Adreno-660)
* [PowerVR BXM-8-256](#PowerVR-BXM-8-256)
* [Intel UHD 620](#Intel-UHD-620)
* [Intel N150](#Intel-N150)

## Nvidia RTX 2080

| nonuniform              | per object | per quad | per pixel |
|-------------------------|------------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0       |
| texture index           | 1.0        | 1.06     | 2.3       |
| texture & sampler index | 1.01       | 1.06     | 2.3       |

<details><summary><b>Подробные результаты</b></summary>

Тестируется в 8К разрешении, 4К в 4 раза быстрее, значит все упирается в FS.

| nonuniform              | per object (ms) | per quad (ms) | per pixel (ms) |
|-------------------------|-----------------|---------------|----------------|
| texture layer           | 1.55            | 1.59          | 1.71           |
| texture index           | 1.55            | 1.69          | 3.98           |
| texture & sampler index | 1.57            | 1.69          | 3.93           |

</details>

## AMD RX570

| nonuniform              | per object | per quad | per pixel |
|-------------------------|------------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0       |
| texture index           | 1.0        | 1.04     | 1.42      |
| texture & sampler index | 1.0        | 1.05     | 1.42      |

<details><summary><b>Подробные результаты</b></summary>

| nonuniform              | per object (ms) | per quad (ms) | per pixel (ms) |
|-------------------------|-----------------|---------------|----------------|
| texture layer           | 6.65            | 6.65          | 8.3            |
| texture index           | 6.65            | 6.94          | 11.8           |
| texture & sampler index | 6.65            | 6.95          | 11.8           |

</details>

## AMD Radeon 780M, AMDPRO

Хоть и нет нативной поддержки неоднородных индексов, но производительность меняется незначительно.

| nonuniform              | per object | per quad | per pixel |
|-------------------------|------------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0       |
| texture index           | 1.0        | 0.99     | 1.19      |
| texture & sampler index | 1.0        | 0.99     | 1.19      |

<details><summary><b>Подробные результаты</b></summary>

GPU Clock: 2600MHz

| nonuniform              | per object (ms) | per quad (ms) | per pixel (ms) |
|-------------------------|-----------------|---------------|----------------|
| texture layer           | 9.1             | 10.0          | 10.0           |
| texture index           | 9.1             | 9.9           | 11.9           |
| texture & sampler index | 9.1             | 9.9           | 11.9           |

</details>

## AMD Radeon 780M, AMDVLK

| nonuniform              | per object | per quad | per pixel |
|-------------------------|------------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0       |
| texture index           | 1.0        | 0.99     | 1.21      |
| texture & sampler index | 1.0        | 0.99     | 1.21      |

<details><summary><b>Подробные результаты</b></summary>

GPU Clock: 2600MHz

| nonuniform              | per object (ms) | per quad (ms) | per pixel (ms) |
|-------------------------|-----------------|---------------|----------------|
| texture layer           | 9.7             | 10.1          | 10.1           |
| texture index           | 9.7             | 10.0          | 12.2           |
| texture & sampler index | 9.7             | 10.0          | 12.2           |

</details>

## AMD Radeon 780M, RADV

RADV драйвер оказался быстрее других, но bindless сильнее влияет на производительность.

| nonuniform              | per object | per quad | per pixel |
|-------------------------|------------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0       |
| texture index           | 1.0        | 1.04     | 1.18      |
| texture & sampler index | 1.0        | 1.07     | 1.26      |

<details><summary><b>Подробные результаты</b></summary>

GPU Clock: 2500MHz

| nonuniform              | per object (ms) | per quad (ms) | per pixel (ms) |
|-------------------------|-----------------|---------------|----------------|
| texture layer           | 5.9             | 7.4           | 8.9            |
| texture index           | 5.9             | 7.7           | 10.5           |
| texture & sampler index | 5.9             | 7.9           | 11.2           |

</details>

## ARM Mali G57

| nonuniform              | per object | per quad | per pixel |
|-------------------------|------------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0       |
| texture index           | 1.0        | 1.6      | 2.3       |
| texture & sampler index | 1.0        | 1.64     | 2         |

<details><summary><b>Подробные результаты</b></summary>

| nonuniform              | per object (ms) | per quad (ms) | per pixel (ms) |
|-------------------------|-----------------|---------------|----------------|
| texture layer           | 4.87            | 5.4           | 6.2            |
| texture index           | 4.9             | 8.65          | 14.4           |
| texture & sampler index | 4.9             | 8.85          | 12.4           |

</details>

## ARM Mali G610

Valhall gen3 архитектура уже лучше справляется с bindless по сравнению с gen1.

| nonuniform              | per object | per quad | per pixel |
|-------------------------|------------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0       |
| texture index           | 1.0        | 1.27     | 1.52      |
| texture & sampler index | 1.0        | 1.33     | 1.54      |

<details><summary><b>Подробные результаты</b></summary>

| nonuniform              | per object (ms) | per quad (ms) | per pixel (ms) |
|-------------------------|-----------------|---------------|----------------|
| texture layer           | 6.5             | 6.7           | 6.9            |
| texture index           | 6.5             | 8.5           | 10.5           |
| texture & sampler index | 6.5             | 8.9           | 10.6           |

</details>

## Adreno 660

В per object режиме texture layer оказывается в 1.7 раз быстрее, но при переходе к per quad разница минимальна. Скорее всего связано с общей просадкой производительности.

Разница между per object и per quad в 2.1 раза, а между per object и per pixel аж 4.5 раза, что влияет на подход к рисования в целом.
Возможно нужна большая локальность текселей к которым обращается варп, так будет меньше потерь.

| nonuniform              | per object | per quad | per pixel |
|-------------------------|------------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0       |
| texture index           | 1.7        | 1.05     | 1.3       |
| texture & sampler index | 1.7        | 1.04     | 1.28      |

<details><summary><b>Подробные результаты</b></summary>

| nonuniform              | per object (ms) | per quad (ms) | per pixel (ms) |
|-------------------------|-----------------|---------------|----------------|
| texture layer           | 3.8             | 8.1           | 17.2           |
| texture index           | 6.6             | 8.5           | 22.3           |
| texture & sampler index | 6.6             | 8.4           | 22.1           |

</details>

## PowerVR BXM-8-256

| nonuniform              | per object | per quad | per pixel | per pixel 4K |
|-------------------------|------------|----------|-----------|--------------|
| **texture layer**       | 1.0        | 1.0      | 1.0       | 1.0          |
| texture index           | 1.0        | 1.0      | 1.17      | 1.5          |
| texture & sampler index | 1.0        | 1.0      | 1.14      | 1.35         |

<details><summary><b>Подробные результаты</b></summary>

| nonuniform              | per object 2K (ms) | per quad 2K (ms) | per pixel 2K (ms) | per pixel 4K (ms) |
|-------------------------|--------------------|------------------|-------------------|-------------------|
| texture layer           | 3.53               | 3.59             | 3.86              | 10.1              |
| texture index           | 3.53               | 3.59             | 4.5               | 15.1              |
| texture & sampler index | 3.53               | 3.59             | 4.4               | 13.7              |

</details>

## Intel UHD 620

Вариант с bindless texture в разы медленее, скорее всего компилятор сопоставлял immutable sampler с динамической индексацией и получилось очень плохо.

| nonuniform              | per object | per quad | per pixel |
|-------------------------|------------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0       |
| texture index           | 1.0        | 4.9      | 10.8      |
| texture & sampler index | 1.0        | 1.4      | 2.4       |

<details><summary><b>Подробные результаты</b></summary>

| nonuniform              | per object (ms) | per quad (ms) | per pixel (ms) |
|-------------------------|-----------------|---------------|----------------|
| texture layer           | 8.8             | 9.0           | 9.3            |
| texture index           | 8.8             | 44.0          | 100            |
| texture & sampler index | 8.8             | 13.0          | 22.0           |

</details>

## Intel N150

| nonuniform              | per object | per quad | per pixel |
|-------------------------|------------|----------|-----------|
| **texture layer**       | 1.0        | 1.0      | 1.0       |
| texture index           | 1.04       | 1.21     | 1.5       |
| texture & sampler index | 1.04       | 1.21     | 1.5       |

<details><summary><b>Подробные результаты</b></summary>

| nonuniform              | per object (ms) | per quad (ms) | per pixel (ms) |
|-------------------------|-----------------|---------------|----------------|
| texture layer           | 12.0            | 12.4          | 12.7           |
| texture index           | 12.5            | 15.0          | 19.0           |
| texture & sampler index | 12.5            | 15.0          | 19.0           |

</details>


# Итоги

Внезапно, только Adreno 660 плохо справился с bindless.
У Intel gen9.5 возникли проблемы с bindless texture + immutable sampler, но на аналогичной по производительности модели N150 (gen12 ?) эту проблему исправили.

Получилось 3 группы:
1. Когда bindless texture + imutable sampler быстрее. Это AMD 780M RADV.
2. Когда bindless texture + bindless sampler быстрее. Это Intel UHD620, PowerVR BXM, Adreno 660, Mali G57.
3. Одинаковая производительность у bindless. Это Intel N150, Mali G610, AMD 780M PRO и VLK, AMD RX570, NV RTX 2080.

`shaderSampledImageArrayNonUniformIndexingNative` флаг не дает информации о производительности.

На старых устройствах Mali T830 и Adreno 505 нет поддержки nonuniform и забиндить можно всего 16 текстур.
