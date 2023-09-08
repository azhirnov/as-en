
## Идентификаторы

Используются тривиальные типы на 32 бита, например: `BufferID`, `ImageID`.

Поддерживаются сильные и слабые ссылки.<br/>
`Strong<BufferID>` - сильная ссылка, `BufferID` - слабая ссылка.<br/>
Сильные ссылки копируются через `IResourceManager::AcquireResource()`, слабые ссылки - через обычное копирование.

Движок возвращает ошибку, если используются уже удаленные ресурсы.

При записи командного буфера использование несуществующего ресурса приводит к бросанию исключения, если его не перехватить, то это произойдет снаружи `RenderTask::Run()` и таск пометится как отмененный.


## Проверки при создании ресурсов

**Некоторые реализации Vulkan позволяет создать ресурс с неподдерживаемыми параметрами, что приводит к неопределенному поведению, поэтому необходимо проверять описание ресурса до его создания**.

`IResourceManager::IsSupported (EMemoryType memType)` <br/>
Проверяет, поддерживается ли выбранный тип памяти.

`IResourceManager::IsSupported (const BufferDesc &desc)` <br/>
Проверяет, поддерживается ли буфер с выбранными параметрами.
При вызове `IResourceManager::CreateBuffer()` это проверка не производится.

`IResourceManager::IsSupported (const ImageDesc &desc)` <br/>
Проверяет, поддерживается ли текстура с выбранными параметрами.
При вызове `IResourceManager::CreateImage()` это проверка не производится.

`IResourceManager::IsSupported (const VideoImageDesc &desc)` <br/>
Проверяет, поддерживается ли текстура для работы с видео с выбранными параметрами.
При вызове `IResourceManager::CreateVideoImage()` это проверка не производится.

`IResourceManager::IsSupported (const VideoBufferDesc &desc)` <br/>
Проверяет, поддерживается ли буфер для работы с видео с выбранными параметрами.
При вызове `IResourceManager::CreateVideoBuffer()` это проверка не производится.

`IResourceManager::IsSupported (const VideoSessionDesc &desc)` <br/>
Проверяет, поддерживается ли видео сессия с выбранными параметрами.
При вызове `IResourceManager::CreateVideoSession()` это проверка не производится.

`IResourceManager::IsSupported (BufferID buffer, const BufferViewDesc &desc)` <br/>
Проверяет, поддерживается ли вию для буфер с выбранными параметрами.
При вызове `IResourceManager::CreateBufferView()` это проверка не производится.

`IResourceManager::IsSupported (ImageID image, const ImageViewDesc &desc)` <br/>
Проверяет, поддерживается ли вию для текстуры с выбранными параметрами.
При вызове `IResourceManager::CreateImageView()` это проверка не производится.

`IResourceManager::IsSupported (const RTGeometryDesc &desc)` <br/>
Проверяет, поддерживается ли ускоряющая структура с выбранными параметрами.
При вызове `IResourceManager::CreateRTGeometry()` это проверка не производится.

`IResourceManager::IsSupported (const RTSceneDesc &desc)` <br/>
Проверяет, поддерживается ли ускоряющая структура с выбранными параметрами.
При вызове `IResourceManager::CreateRTScene()` это проверка не производится.

