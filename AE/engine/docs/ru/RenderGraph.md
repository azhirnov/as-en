

## Синхронизации без рендер графа

Когда рендер кадра состоит из заранее известных проходов, то управлять состояниями ресурсов можно и вручную.

__Первым проходом должно быть обновление данных на ГП.__<br/>
Синхронизация `Host_Write -> CopySrc|IndexBuffer|VertexBuffer|UniformBuffer` нужна при записи данных на стороне ЦП.
Все копирования и обновление юниформ должно быть в этом проходе, чтоб избавиться от лишних синхронизаций в других проходах.

__Далее идут проходы рисования и вычислений.__<br/>
Предполагается, что все неизменяемые ресурсы уже находятся в том состоянии, в котором они используются в дескрипторах.
Остаются синхронизации для изменяемых ресурсов (Attachment, StorageImage, StorageBuffer), их легко отслеживать и синхронизировать вручную, для проверки корректности [есть способы](RenderGraph.md#проверка-на-корректность-синхронизаций).

__Синхронизации между очередями.__<br/>
Есть 2 подхода:
1. Сделать ресурсы общими для всех очередей (`VK_SHARING_MODE_CONCURRENT`), тогда достаточно сделать синхронизации семафорами, чтобы избежать одновременной записи или чтения и записи (data race), в движке это делается через `CommandBatch::AddInputDependency (CommandBatch &)`. Минус этого подхода - на AMD на общих ресурсах не включается компрессия рендер таргетов (DCC), что снижает производительность.<br/>
Пример: [Test_RG_AsyncCompute1.cpp](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/Test_RG_AsyncCompute1.cpp)
2. Явно передавать ресурсы между очередями (queue ownership transfer). Внутри рендер таска это сложнее отслеживать, поэтому такие барьеры удобнее вынести в интерфейс `CommandBatch`, так появился метод `CommandBatch::DeferredBarriers()` и `initial, final` параметры при создании рендер таска. Теперь управление перемещением ресурсов происходит на этапе планирования батчей команд.<br/>
Пример: [Test_RG_AsyncCompute2.cpp](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/Test_RG_AsyncCompute2.cpp)


## Синхронизации с помощью рендер графа

#### Контекст для записи команд.
Работает поверх существующих контекстов.
Добавлено только отслеживание состояний ресурсов в пределах `RenderTask` и автоматическое перемещение их в нужное состояние.
Начальное и конечное состояние ресурса это либо дефолтное, либо задается вручную на этапе планирования рендер графа, иногда это добавляет ненужные синхронизации, но потери на них минимальные, если не приводят к декомпресии рендер таргетов (TODO: ссылка).


#### Этап планирования.
Точно также создаются батчи, но теперь через builder паттерн, где можно указать какие ресурсы будут использоваться в батче и их начальное/конечное состояние. Если ресурс используется только в одной ГП-очереди (VkQueue), то указывать его не обязательно. Но если ресурс используется в нескольких очередях, то требуется явно добавить его в батч, тогда внутри вставятся все необходимые синхронизации.
Для каждого рендер таска также можно указать начальное и конечное состояние ресурса, это позволит оптимизировать синхронизации между тасками.

```cpp
AsyncTask begin = rg.BeginFrame();

// создаем батч, в нем будем использовать 'image' для чтения в фрагментном шейдере
auto batch_gfx = rg.CmdBatch( EQueueType::Graphics, {"graphics batch"} )
                     .UseResource( image, EResourceState::ShaderSample | EResourceState::FragmentShader )
                     .Begin();

// создаем второй батч, 'image' используется для чтения/записи в вычислительном шейдере
auto batch_ac = rg.CmdBatch( EQueueType::AsyncCompute, {"compute batch"} )
                    .UseResource( image, EResourceState::ShaderStorage_RW | EResourceState::ComputeShader )
                    .Begin();

// при вызове 'UseResource()' неявно устанавливается зависимость 'batch_gfx -> batch_ac'

AsyncTask gfx_task  = batch_gfx.Task<GraphicsTask>( Tuple{...}, {"graphics task"}      ).SubmitBatch().Run( Tuple{begin} );
AsyncTask comp_task = batch_ac .Task<ComputeTask >( Tuple{...}, {"async compute task"} ).SubmitBatch().Run( Tuple{gfx_task} );

// 'SubmitBatch()' помечает задачу как последнюю, тогда вызов 'RenderTask::Execute(cmdbuf)' также добавит батч в очередь на отправку на ГП (submit),
// иначе отправку батча нужно сделать через отдельный таск - 'CommandBatch::SubmitAsTask()'.

AsyncTask end = rg.EndFrame( Tuple{ gfx_task, comp_task });
```

В рендер графе порядок создания батчей имеет значение, поэтому желательно создать все батчи в одном потоке.

Пример: [Test_RG_AsyncCompute3.cpp](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/Test_RG_AsyncCompute3.cpp)


#### Производительность

Текущая реализация рендер графа не делает перестановку и оптимизацию, только расставляет синхронизации.
С помощью профилировщиков можно обнаружить медленные места и вручную расставить синхронизации и переставить команды для лучшего распараллеливания.

Потери производительности на стороне ЦП минимальны, но добавляет редкие кэш-промахи при доступе к состояниям ресурсов.

Исходники: [RGCommandBatch.h](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/graphics/RenderGraph/RGCommandBatch.h), [RenderGraph.h](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/graphics/RenderGraph/RenderGraph.h)


## Проверка на корректность синхронизаций

Для этого в движке есть логирование команд (проект `VulkanSyncLog`), который выдает читаемый лог вызовов Vulkan комманд и его результат не меняется в зависимости от запусков, что позволяет следить за изменениями.
Но все синхронизации придется один раз вручную проверить на корректность.<br/>
[Пример лога](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/Vulkan/ref/NVIDIA%20GeForce%20RTX%202080/Test_AsyncCompute2.txt)

Другой вариант - запустить vkconfig и включить полную валидацию синхронизаций - Synchronization preset.<br/>
[Guide to Vulkan Synchronization Validation](https://www.lunarg.com/wp-content/uploads/2020/09/Final_LunarG_Guide_to_Vulkan-Synchronization_Validation_08_20.pdf)

Либо из кода, при инициализации движка передать флаг `EDeviceValidation::SynchronizationPreset`.
