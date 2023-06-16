source: [DeviceProperties.h](file:///<path>/engine/src/graphics/Public/DeviceProperties.h)
 
## ResourceAlignment

This constants is used to calculate data alignment for uniform, storage and vertex buffers.
Constants used in C++ code to store data for GPU usage.

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
__Mali GPU__ has fixed-sized buffer to store vertices after transformations and before per-tile execution,
this is not documented in Vulkan API and should be manually updated.</br>
ref: [ARM blog](https://community.arm.com/arm-community-blogs/b/graphics-gaming-and-vr-blog/posts/memory-limits-with-vulkan-on-mali-gpus)</br>
__Vulkan__: *not defined*</br>
__Metal__:  *not defined*
	
* `minVertexBufferOffsetAlign`</br>
__Vulkan__: *not defined*</br>
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


## ShaderHWProperties

