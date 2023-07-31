source: [FeatureSet.h](../../src/graphics/Public/FeatureSet.h)

Some kind of [Vulkan Profiles](https://github.com/KhronosGroup/Vulkan-Profiles) and [Metal Feature Set Tables](https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf).

## Render States

* __alphaToOne__<br/>
Same as `VkPhysicalDeviceFeatures::alphaToOne`.
* __depthBiasClamp__<br/>
Same as `VkPhysicalDeviceFeatures::depthBiasClamp`.
* __depthBounds__<br/>
Same as `VkPhysicalDeviceFeatures::depthBounds`.
* __depthClamp__<br/>
Same as `VkPhysicalDeviceFeatures::depthClamp`.
* __dualSrcBlend__<br/>
Same as `VkPhysicalDeviceFeatures::dualSrcBlend`.
* __fillModeNonSolid__<br/>
Same as `VkPhysicalDeviceFeatures::fillModeNonSolid`.
* __independentBlend__<br/>
Same as `VkPhysicalDeviceFeatures::independentBlend`.
* __logicOp__<br/>
Same as `VkPhysicalDeviceFeatures::logicOp`.
* __sampleRateShading__<br/>
Same as `VkPhysicalDeviceFeatures::sampleRateShading`.
* __constantAlphaColorBlendFactors__<br/>
Same as `VkPhysicalDevicePortabilitySubsetFeaturesKHR::constantAlphaColorBlendFactors`.
* __pointPolygons__<br/>
Same as `VkPhysicalDevicePortabilitySubsetFeaturesKHR::pointPolygons`.
* __separateStencilMaskRef__<br/>
Same as `VkPhysicalDevicePortabilitySubsetFeaturesKHR::separateStencilMaskRef`.
* __triangleFans__<br/>
Same as `VkPhysicalDevicePortabilitySubsetFeaturesKHR::triangleFans`.


## Shader variable types and functions

### Subgroup

* __subgroupOperations__
* __subgroupTypes__
* __subgroupStages__
* __subgroupQuadStages__
* __requiredSubgroupSizeStages__
* __subgroup__
* __subgroupBroadcastDynamicId__
* __subgroupSizeControl__
Same as `VkPhysicalDeviceSubgroupSizeControlFeatures::subgroupSizeControl`.<br/>
Metal: require SIMD feature.
* __shaderSubgroupUniformControlFlow__<br/>
Same as `VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR::shaderSubgroupUniformControlFlow`.<br/>
Metal: TODO
* __minSubgroupSize__<br/>
Maximal value of `VkPhysicalDeviceSubgroupSizeControlProperties::minSubgroupSize`.<br/>
Metal: 4.
* __maxSubgroupSize__<br/>
Minimal value of `VkPhysicalDeviceSubgroupSizeControlProperties::maxSubgroupSize`.<br/>
Metal: 32 or 64.

### Types

* __shaderInt8__<br/>
Same as `VkPhysicalDeviceShaderFloat16Int8FeaturesKHR::shaderInt8`.<br/>
Metal: always supported.
* __shaderInt16__<br/>
Same as `VkPhysicalDeviceFeatures::shaderInt16`.<br/>
Metal: always supported.
* __shaderInt64__<br/>
Same as `VkPhysicalDeviceFeatures::shaderInt64`.<br/>
Metal: v2.3+.
* __shaderFloat16__<br/>
Same as `VkPhysicalDeviceShaderFloat16Int8FeaturesKHR::shaderFloat16`.<br/>
Metal: always supported.
* __shaderFloat64__<br/>
Same as `VkPhysicalDeviceFeatures::shaderFloat64`.<br/>
Metal: not supported.

### Uniform / Storage buffer

* __storageBuffer16BitAccess__
* __uniformAndStorageBuffer16BitAccess__
* __storageInputOutput16__
* __storageBuffer8BitAccess__
* __uniformAndStorageBuffer8BitAccess__
* __uniformBufferStandardLayout__
* __scalarBlockLayout__
* __bufferDeviceAddress__

### Push constant

* storagePushConstant8
* storagePushConstant16

### Atomic

* fragmentStoresAndAtomics
* vertexPipelineStoresAndAtomics
* shaderImageInt64Atomics

__- GL_EXT_shader_atomic_int64__:
* shaderBufferInt64Atomics
* shaderSharedInt64Atomics

__- GL_EXT_shader_atomic_float__:
* shaderBufferFloat32Atomics
* shaderBufferFloat32AtomicAdd
* shaderBufferFloat64Atomics
* shaderBufferFloat64AtomicAdd
* shaderSharedFloat32Atomics
* shaderSharedFloat32AtomicAdd
* shaderSharedFloat64Atomics
* shaderSharedFloat64AtomicAdd
* shaderImageFloat32Atomics
* shaderImageFloat32AtomicAdd

__- GL_EXT_shader_atomic_float2__:
* shaderBufferFloat16Atomics
* shaderBufferFloat16AtomicAdd
* shaderBufferFloat16AtomicMinMax
* shaderBufferFloat32AtomicMinMax
* shaderBufferFloat64AtomicMinMax
* shaderSharedFloat16Atomics
* shaderSharedFloat16AtomicAdd
* shaderSharedFloat16AtomicMinMax
* shaderSharedFloat32AtomicMinMax
* shaderSharedFloat64AtomicMinMax
* shaderImageFloat32AtomicMinMax
* sparseImageFloat32AtomicMinMax

### Output

* shaderOutputViewportIndex
* shaderOutputLayer

### Clock

* shaderSubgroupClock
* shaderDeviceClock

### nVidia specific

* cooperativeMatrixNV


## Shader features and limits

* shaderClipDistance
* shaderCullDistance
* shaderResourceMinLod
* shaderDrawParameters
* runtimeDescriptorArray
* shaderSMBuiltinsNV
* shaderCoreBuiltinsARM
* shaderSampleRateInterpolationFunctions
* shaderDemoteToHelperInvocation
* shaderTerminateInvocation
* shaderZeroInitializeWorkgroupMemory
* shaderIntegerDotProduct
* minShaderVersion

### Array dynamic indexing

* shaderSampledImageArrayDynamicIndexing
* shaderStorageBufferArrayDynamicIndexing
* shaderStorageImageArrayDynamicIndexing
* shaderUniformBufferArrayDynamicIndexing
* shaderInputAttachmentArrayDynamicIndexing
* shaderUniformTexelBufferArrayDynamicIndexing
* shaderStorageTexelBufferArrayDynamicIndexing

### Non uniform indexing (GL_EXT_nonuniform_qualifier)

* shaderUniformBufferArrayNonUniformIndexing
* shaderSampledImageArrayNonUniformIndexing
* shaderStorageBufferArrayNonUniformIndexing
* shaderStorageImageArrayNonUniformIndexing
* shaderInputAttachmentArrayNonUniformIndexing
* shaderUniformTexelBufferArrayNonUniformIndexing
* shaderStorageTexelBufferArrayNonUniformIndexing

### Storage image format

* shaderStorageImageMultisample
* shaderStorageImageReadWithoutFormat
* shaderStorageImageWriteWithoutFormat

### Memory model (GL_KHR_memory_scope_semantics)

* vulkanMemoryModel
* vulkanMemoryModelDeviceScope
* vulkanMemoryModelAvailabilityVisibilityChains

### Fragment shader interlock (GL_ARB_fragment_shader_interlock)

* fragmentShaderSampleInterlock
* fragmentShaderPixelInterlock
* fragmentShaderShadingRateInterlock

### Fragment shading rate (GL_EXT_fragment_shading_rate)

* pipelineFragmentShadingRate
* primitiveFragmentShadingRate
* attachmentFragmentShadingRate
* primitiveFragmentShadingRateWithMultipleViewports
* layeredShadingRateAttachments
* fragmentShadingRateWithShaderDepthStencilWrites
* fragmentShadingRateWithSampleMask
* fragmentShadingRateWithShaderSampleMask
* fragmentShadingRateWithFragmentShaderInterlock
* fragmentShadingRateWithCustomSampleLocations
* fragmentShadingRateTexelSize
* fragmentShadingRates

### Inline ray tracing (GL_EXT_ray_query)

* __rayQuery__<br/>
Same as `VkPhysicalDeviceRayQueryFeaturesKHR::rayQuery`.
* __rayQueryStages__<br/>
Vulkan: all supported stages.<br/>
Metal: separate features for compute and graphics stages.

### Ray tracing (GL_EXT_ray_tracing)

* __rayTracingPipeline__<br/>
Same as `VkPhysicalDeviceRayTracingPipelineFeaturesKHR::rayTracingPipeline`.<br/>
Metal: not supported (can be emulated).
* __rayTraversalPrimitiveCulling__<br/>
Same as `VkPhysicalDeviceRayTracingPipelineFeaturesKHR::rayTraversalPrimitiveCulling`.<br/>
Metal: not supported.
* __minRayRecursionDepth__<br/>
Minimal value of `VkPhysicalDeviceRayTracingPipelinePropertiesKHR::maxRayRecursionDepth`.<br/>
Metal: not supported.

### Multiview

* multiview
* multiviewGeometryShader
* multiviewTessellationShader
* minMultiviewViewCount

### Multi viewport

* multiViewport
* minViewports

### Sample locations

* sampleLocations
* variableSampleLocations

### Tessellation

* __tessellationShader__<br/>
Same as `VkPhysicalDeviceFeatures::tessellationShader`.
* __tessellationIsolines__<br/>
Same as `VkPhysicalDevicePortabilitySubsetFeaturesKHR::tessellationIsolines`.
* __tessellationPointMode__<br/>
Same as `VkPhysicalDevicePortabilitySubsetFeaturesKHR::tessellationPointMode`.

### Shader limits

* minTexelBufferElements
* minUniformBufferSize
* minStorageBufferSize
* perDescrSet
* perStage
* minDescriptorSets
* minTexelOffset
* minTexelGatherOffset
* minFragmentOutputAttachments
* minFragmentDualSrcAttachments
* minFragmentCombinedOutputResources
* minPushConstantsSize
* minTotalThreadgroupSize
* minTotalTileMemory
* minVertAmplification

### Compute shader 

* __computeShader__<br/>
Vulkan: always supported.<br/>
Metal: always supported.
* __minComputeSharedMemorySize__<br/>
Minimal value of `VkPhysicalDeviceLimits::maxComputeSharedMemorySize`.<br/>
Metal: constant.
* __minComputeWorkGroupInvocations__<br/>
Minimal value of `VkPhysicalDeviceLimits::maxComputeWorkGroupInvocations`.<br/>
Metal: constant.
* __minComputeWorkGroupSize [X,Y,Z]__<br/>
Minimal value of `VkPhysicalDeviceLimits::maxComputeWorkGroupCount`.<br/>
Metal: `[MTLDevice maxThreadsPerThreadgroup]`.

### Mesh shader (GL_EXT_mesh_shader)

* __taskShader__<br/>
Same as `VkPhysicalDeviceMeshShaderFeaturesEXT::taskShader`.
* __meshShader__<br/>
Same as `VkPhysicalDeviceMeshShaderFeaturesEXT::meshShader`.
* __minTaskWorkGroupSize__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxTaskWorkGroupInvocations` and `maxTaskWorkGroupSize`. In current implementations this values are same and equal to 32.
* __minMeshWorkGroupSize__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshWorkGroupInvocations` and `maxMeshWorkGroupSize`. In current implementations this values are same and equal to 32.
* __minMeshOutputVertices__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshOutputVertices`.
* __minMeshOutputPrimitives__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshOutputPrimitives`.
* __maxMeshOutputPerVertexGranularity__<br/>
Maximal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::meshOutputPerVertexGranularity`.
* __maxMeshOutputPerPrimitiveGranularity__<br/>
Maximal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::meshOutputPerPrimitiveGranularity`.
* __minTaskPayloadSize__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxTaskPayloadSize`.
* __minTaskSharedMemorySize__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxTaskSharedMemorySize`.
* __minTaskPayloadAndSharedMemorySize__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxTaskPayloadAndSharedMemorySize`.
* __minMeshSharedMemorySize__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshSharedMemorySize`.
* __minMeshPayloadAndSharedMemorySize__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshPayloadAndSharedMemorySize`.
* __minMeshOutputMemorySize__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshOutputMemorySize`.
* __minMeshPayloadAndOutputMemorySize__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshPayloadAndOutputMemorySize`.
* __minMeshMultiviewViewCount__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshMultiviewViewCount`.
* __minPreferredTaskWorkGroupInvocations__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxPreferredTaskWorkGroupInvocations`.
* __minPreferredMeshWorkGroupInvocations__<br/>
Minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxPreferredMeshWorkGroupInvocations`.

### Raster order group

* minRasterOrderGroups

### Shaders

* __geometryShader__<br/>
Same as `VkPhysicalDeviceFeatures::geometryShader`.<br/>
Metal: not supported.
* __tileShader__<br/>
Vulkan: not supported.<br/>
Metal: requires tile shader feature.

### Vertex buffer

* minVertexAttributes
* minVertexBuffers

__- VK_EXT_vertex_attribute_divisor__:
* vertexDivisor
* minVertexAttribDivisor


## Buffer

* __vertexFormats__<br/>
Formats which supports `VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT` in `vkGetPhysicalDeviceFormatProperties()`.
* __uniformTexBufferFormats__<br/>
Formats which supports `VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT` in `vkGetPhysicalDeviceFormatProperties()`.
* __storageTexBufferFormats__<br/>
Formats which supports `VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT` in `vkGetPhysicalDeviceFormatProperties()`.
* __storageTexBufferAtomicFormats__<br/>
Formats which supports `VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT` in `vkGetPhysicalDeviceFormatProperties()`.
* __accelStructVertexFormats__<br/>
Formats which supports `VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR` in `vkGetPhysicalDeviceFormatProperties()`.


## Image

* imageCubeArray
* textureCompressionASTC_LDR
* textureCompressionASTC_HDR
* textureCompressionBC
* textureCompressionETC2
* imageViewMinLod
* multisampleArrayImage
* minImageArrayLayers
* __storageImageAtomicFormats__<br/>
Formats which supports `VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT` in `vkGetPhysicalDeviceFormatProperties()`.
* __storageImageFormats__<br/>
Formats which supports `VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT` in `vkGetPhysicalDeviceFormatProperties()`.
* __attachmentBlendFormats__<br/>
Formats which supports `VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT` in `vkGetPhysicalDeviceFormatProperties()`.
* __attachmentFormats__<br/>
Formats which supports `VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT` in `vkGetPhysicalDeviceFormatProperties()`.
* __linearSampledFormats__<br/>
Formats which supports `VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT` in `vkGetPhysicalDeviceFormatProperties()`.
* surfaceFormats
* hwCompressedAttachmentFormats
* lossyCompressedAttachmentFormats


## Sampler

* samplerAnisotropy
* samplerMirrorClampToEdge
* samplerFilterMinmax
* filterMinmaxImageComponentMapping
* samplerMipLodBias
* samplerYcbcrConversion
* minSamplerAnisotropy
* minSamplerLodBias


## Framebuffer

* framebufferColorSampleCounts
* framebufferDepthSampleCounts
* minFramebufferLayers


## Render pass

* variableMultisampleRate


## Metal specific

* metalArgBufferTier


## Hardware info

* queues
* vendorIds
* devicesIds

