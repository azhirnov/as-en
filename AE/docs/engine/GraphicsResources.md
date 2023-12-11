
## Graphics IDs

ID is a [trivial type](https://learn.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types?view=msvc-170) which contains index in pool and generation counter.<br/>
Source code: [HandleTmpl](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/base/Utils/HandleTmpl.h), [Graphics IDs](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/graphics/Public/IDs.h).

Supports strong and weak references:<br/>
`BufferID` - weak reference.<br/>
`Strong<BufferID>` - strong reference.<br/>
Conversion from strong ref to weak ref has zero cost.<br/>
Weak reference can be converted to the strong ref by using `IResourceManager::AcquireResource()`.

### IDs in command context

Inside command context resource internal data will be retrieved by `ResourceManager::GetResourcesOrThrow()` which throws exception if ID is not valid - resource is deleted.

### IDs in ResourceManager

Exceptions are not used here. Error will be generated if ID is not valid and method will return `false`.


## ResourceManager

### Resource creation

`Create*()` methods doesn't check if resource description is supported by the GPU. Vulkan implementation may create resource even if it is not supported by the current GPU.

If the engine is compiled with `AE_GRAPHICS_STRONG_VALIDATION=ON` then `Create*()` methods will use the `IsSupported()` method to check whether the resource description is supported by the GPU. If the description is not supported, the resource will not be created and an error will be generated.

### Resource description validation

Use `IsSupported()` method before creating new resource to check if the resource description is supported by the GPU.

