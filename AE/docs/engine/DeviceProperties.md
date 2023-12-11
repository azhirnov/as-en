Source: [DeviceProperties.h](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/graphics/Public/DeviceProperties.h)

Use **DeviceProperties** from `DeviceProperties const&  GraphicsScheduler().GetDeviceProperties();` to get runtime limits for the current GPU.

Use **DeviceLimits** from `static constexpr DeviceProperties  DeviceLimits;` to get compile time limits which is compatible with most GPUs.


## ResourceAlignment

These constants are used to calculate data alignment for uniform, storage and vertex buffers.
Constants are used in C++ code to store data for accessing them in the GPU.

* `minUniformBufferOffsetAlign`</br>
`minStorageBufferOffsetAlign`</br>
`minThreadgroupMemoryLengthAlign`</br>
`minUniformTexelBufferOffsetAlign`</br>
`minStorageTexelBufferOffsetAlign`</br>
__Vulkan__: defined in VkPhysicalDeviceLimits:</br>
			minUniformBufferOffsetAlignment, minStorageBufferOffsetAlignment, minTexelBufferOffsetAlignment, minTexelBufferOffsetAlignment.</br>
__Metal__:  from [Feature Set Tables](https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf):</br>
"*Threadgroup memory length alignment*",</br>
"*Minimum constant buffer offset alignment*"</br>
and from [specs](https://developer.apple.com/documentation/metal/mtlcomputecommandencoder/1443134-setbuffers):</br>
"*For buffers in the device address space, align the offset to the data type consumed by the
compute function (which is always less than or equal to 16 bytes).
For buffers in the constant address space, align the offset to 256 bytes in macOS. In iOS, align
the offset to the maximum of either the data type consumed by the compute function, or 4 bytes.
A 16-byte alignment is safe in iOS if you don't need to consider the data type.*"

* `maxVerticesPerRenderPass`</br>
__Mali GPU__: from [ARM blog](https://community.arm.com/arm-community-blogs/b/graphics-gaming-and-vr-blog/posts/memory-limits-with-vulkan-on-mali-gpus): "*has fixed-sized buffer to store vertices after transformations and before per-tile execution,
this is not documented in Vulkan API and should be manually updated.*"</br>
__Vulkan__: *not defined*</br>
__Metal__:  *not defined*

* `minVertexBufferOffsetAlign`</br>
__Vulkan__: from [specs](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#fxvertex-input-address-calculation): "*If format is a packed format, attribAddress must be a multiple of the size in bytes of the whole attribute data type as described in Packed Formats. Otherwise, attribAddress must be a multiple of the size in bytes of the component type indicated by format.*"</br>
__Metal__:  from [specs](https://developer.apple.com/documentation/metal/mtlvertexattributedescriptor/1515785-offset?language=objc): "*must be a multiple of 4 bytes*"

* `minVertexBufferElementsAlign`</br>
__Vulkan__: not defined in docs</br>
__Mali GPU__: hardware process vertices by 4 elements, so vertex buffer must have enough size or robust buffer access must be enabled.</br>
__Metal__:  *not defined*		// TODO: check

* `maxUniformBufferRange`</br>
__Vulkan__: defined in VkPhysicalDeviceLimits::maxUniformBufferRange</br>
__Metal__:  from [Feature Set Tables](https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf):
"*Maximum length of constant buffer arguments in vertex, fragment, tile, or kernel function*"

* `maxBoundDescriptorSets`</br>
__Vulkan__: defined in VkPhysicalDeviceLimits::maxBoundDescriptorSets</br>
__Metal__:  used as ArgumentBuffer, which is bound as buffer, maximum number of buffers defined as "*Maximum number of entries in the buffer argument
table, per graphics or kernel function*" in [Feature Set Tables](https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf), minus number of vertex buffers.

* `minMemoryMapAlign`</br>
__Vulkan__: defined in VkPhysicalDeviceLimits::minMemoryMapAlignment</br>
__Metal__:	// TODO

* `minNonCoherentAtomSize`</br>
__Vulkan__: defined in VkPhysicalDeviceLimits::nonCoherentAtomSize</br>
__Metal__:	// TODO

* `minBufferCopyOffsetAlign`</br>
__Vulkan__: defined in VkPhysicalDeviceLimits::optimalBufferCopyOffsetAlignment</br>
__Metal__:  from [specs](https://developer.apple.com/documentation/metal/mtlblitcommandencoder/1400767-copyfrombuffer/):
"*In macOS, offset needs to be a multiple of 4, but can be any value in iOS and tvOS.*"

* `minBufferCopyRowPitchAlign`</br>
__Vulkan__: defined in VkPhysicalDeviceLimits::optimalBufferCopyRowPitchAlignment</br>
__Metal__:  from [Feature Set Tables](https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf):
"*Buffer alignment for copying an existing texture to a buffer*"


## RayTracingProperties

This constants is used to calculate data alignment for acceleration structure input data: vertices, indices, transformations, AABBs, instances.
Constants used in C++ code to store data for GPU usage.

* `vertexDataAlign`<br/>
__Vulkan__: from specs: "*vertexData.deviceAddress must be aligned to the size in bytes of the smallest component of the format in vertexFormat*", max 4 bytes.<br/>
__Metal__:  from [specs](https://developer.apple.com/documentation/metal/mtlaccelerationstructuretrianglegeometrydescriptor/3553877-vertexbufferoffset?language=objc): "*The vertex must be a multiple of the vertex stride and must be a multiple of 4 bytes*".

* `vertexStrideAlign`<br/>
__Vulkan__: from specs: "*must be a multiple of the size in bytes of the smallest component of vertexFormat*", max 4 bytes.<br/>
__Metal__:  from [specs](https://developer.apple.com/documentation/metal/mtlaccelerationstructuretrianglegeometrydescriptor/3553878-vertexstride?language=objc): "*must be a multiple of 4 bytes*".

* `indexDataAlign`<br/>
__Vulkan__: from specs: "*indexData.deviceAddress must be aligned to the size in bytes of the type in indexType*", max 4 bytes.<br/>
__Metal__:  from [specs](https://developer.apple.com/documentation/metal/mtlaccelerationstructuretrianglegeometrydescriptor/3553871-indexbufferoffset?language=objc): "*multiple of the index data type size and a multiple of the platform’s buffer offset alignment*".

* `aabbDataAlign`<br/>
__Vulkan__: from specs: "*must be aligned to 8 bytes*".<br/>
__Metal__:  from [specs](https://developer.apple.com/documentation/metal/mtlaccelerationstructureboundingboxgeometrydescriptor/3553861-boundingboxbufferoffset?language=objc): "*The offset must be a multiple of boundingBoxStride, and must be aligned to the platform’s buffer offset alignment*", min stride: 8 bytes.

* `aabbStrideAlign`<br/>
__Vulkan__: from specs: "*must be a multiple of 8*".<br/>
__Metal__:  from [specs](https://developer.apple.com/documentation/metal/mtlaccelerationstructureboundingboxgeometrydescriptor/3553863-boundingboxstride?language=objc): "*The stride must be at least 24 bytes, and must be a multiple of 4 bytes*".

* `transformDataAlign`<br/>
__Vulkan__: from specs: "*must be aligned to 16 bytes*"<br/>
__Metal__:  in [specs](https://developer.apple.com/documentation/metal/mtlaccelerationstructuretrianglegeometrydescriptor/3929834-transformationmatrixbufferoffset?language=objc): *not defined*.<br/>

* `instanceDataAlign`<br/>
__Vulkan__: from specs: if arrayOfPointers: "*must be aligned to 8 bytes*", otherwise: "*must be aligned to 16 bytes*". For array elements: "*each element of ... in device memory must be aligned to 16 bytes*"<br/>
__Metal__:  from [specs](https://developer.apple.com/documentation/metal/mtlinstanceaccelerationstructuredescriptor/3553887-instancedescriptorbufferoffset?language=objc): "*Specify an offset that is a multiple of 4 bytes and a multiple of the platform’s buffer offset alignment*".

* `instanceStrideAlign`<br/>
__Vulkan__: custom stride is not supported, stride must be 64.<br/>
__Metal__:  from [specs](https://developer.apple.com/documentation/metal/mtlinstanceaccelerationstructuredescriptor/3553888-instancedescriptorstride?language=objc): "*The stride must be at least 64 bytes and must be a multiple of 4 bytes*".

* `scratchBufferAlign`<br/>
__Vulkan__: defined in VkPhysicalDeviceAccelerationStructurePropertiesKHR::minAccelerationStructureScratchOffsetAlignment.<br/>
__Metal__:  *not defined*.<br/>

* `maxGeometries`<br/>
__Vulkan__: defined in VkPhysicalDeviceAccelerationStructurePropertiesKHR::maxGeometryCount.<br/>
__Metal__:  from [specs](https://developer.apple.com/documentation/metal/mtlaccelerationstructureusage/mtlaccelerationstructureusageextendedlimits): 2^24^ (can be extended to 2^30^)<br/>

* `maxInstances`<br/>
__Vulkan__: defined in VkPhysicalDeviceAccelerationStructurePropertiesKHR::maxInstanceCount.<br/>
__Metal__:  from [specs](https://developer.apple.com/documentation/metal/mtlaccelerationstructureusage/mtlaccelerationstructureusageextendedlimits): 2^24^ (can be extended to 2^30^)<br/>

* `maxPrimitives`<br/>
__Vulkan__: defined in VkPhysicalDeviceAccelerationStructurePropertiesKHR::maxPrimitiveCount.<br/>
__Metal__:  from [specs](https://developer.apple.com/documentation/metal/mtlaccelerationstructureusage/mtlaccelerationstructureusageextendedlimits): 2^28^ (can be extended to 2^30^)<br/>

* `maxRecursion`<br/>
__Vulkan__: defined in VkPhysicalDeviceRayTracingPipelinePropertiesKHR::maxRayRecursionDepth.<br/>
__Metal__:  *not defined*.<br/>

* `maxDispatchInvocations`<br/>
__Vulkan__: defined in VkPhysicalDeviceRayTracingPipelinePropertiesKHR::maxRayDispatchInvocationCount.<br/>
__Metal__:  *not defined*.<br/>


## ShaderHWProperties

* `cores`<br/>
StreamingMultiprocessor/Core count per GPU.

* `warpsPerCore`<br/>
Maximum number of simultaneously executing warps on an StreamingMultiprocessor/Core.

* `threadsPerWarp`<br/>
Number of thread in warp, same as maximum size of subgroup.

