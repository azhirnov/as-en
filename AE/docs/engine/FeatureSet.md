Source: [FeatureSet.h](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/graphics_rhi/Public/FeatureSet.h)

This is some kind of [Vulkan Profiles](https://github.com/KhronosGroup/Vulkan-Profiles) and [Metal Feature Set Tables](https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf).

Use [FeatureSetGen](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/feature_set_gen/Readme.md) application to generate feature sets from Vulkan device info JSON and from Metal feature set table.<br/>
Predefined FeatureSet's located in the [feature_set folder](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/feature_set), use them to get GPU limits and features in resource compilation stage.

Use `FeatureSet const&  GraphicsScheduler().GetFeatureSet()` to get cross-platform limits and features of the current GPU in runtime.

*Docs are partially LLM-generated and may be incorrect.*


## Render States

* __alphaToOne__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::alphaToOne`.<br/>
Metal: same as `MTLRenderPipelineDescriptor::alphaToOneEnabled`.
* __depthBiasClamp__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::depthBiasClamp`.<br/>
Metal: supported by clamped depth bias state in `MTLRenderCommandEncoder::setDepthBias:slopeScale:clamp:`.
* __depthBounds__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::depthBounds`.<br/>
Metal: not supported.
* __depthClamp__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::depthClamp`.<br/>
Metal: same as `MTLRenderPipelineDescriptor::depthClipMode` with `MTLDepthClipModeClamp`.
* __dualSrcBlend__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::dualSrcBlend`.<br/>
Metal: supported by dual-source blending with fragment outputs using `[[color(n), index(i)]]`.
* __fillModeNonSolid__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::fillModeNonSolid`.<br/>
Metal: same as `MTLRenderCommandEncoder::setTriangleFillMode`.
* __independentBlend__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::independentBlend`.<br/>
Metal: same as per-attachment `MTLRenderPipelineColorAttachmentDescriptor` blend state.
* __logicOp__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::logicOp`.<br/>
Metal: not supported.
* __sampleRateShading__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::sampleRateShading`.<br/>
Metal: same as sample-rate fragment shader execution using the `[[sample_id]]` attribute.
* __constantAlphaColorBlendFactors__<br/>
Vulkan: same as `VkPhysicalDevicePortabilitySubsetFeaturesKHR::constantAlphaColorBlendFactors`.<br/>
Metal: same as blend factors `MTLBlendFactorBlendAlpha` and `MTLBlendFactorOneMinusBlendAlpha`.
* __pointPolygons__<br/>
Vulkan: same as `VkPhysicalDevicePortabilitySubsetFeaturesKHR::pointPolygons`.<br/>
Metal: same as point primitive rendering with `MTLPrimitiveTypePoint`.
* __triangleFans__<br/>
Vulkan: same as `VkPhysicalDevicePortabilitySubsetFeaturesKHR::triangleFans`.<br/>
Metal: not supported.
* __largePoints__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::largePoints`.<br/>
Metal: same as shader point size output `[[point_size]]`.
* __wideLines__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::wideLines`.<br/>
Metal: not supported.
* __conservativeRasterization__<br/>
Vulkan: supported by `VK_EXT_conservative_rasterization`.<br/>
Metal: not supported.

## Shader variable types and functions

### Subgroup

* __subgroupOperations__<br/>
Vulkan: supported subgroup operation bits from `VkPhysicalDeviceSubgroupProperties::supportedOperations`.<br/>
Metal: same as SIMD-group functions in the Metal Shading Language.
* __subgroupTypes__<br/>
Vulkan: subgroup operation types supported by subgroup extended type features such as 8-bit, 16-bit, 64-bit, and 16-bit floating-point support.<br/>
Metal: same as data types accepted by SIMD-group functions in the Metal Shading Language.
* __subgroupStages__<br/>
Vulkan: shader stages from `VkPhysicalDeviceSubgroupProperties::supportedStages`.<br/>
Metal: shader stages where SIMD-group functions are available.
* __subgroupQuadStages__<br/>
Vulkan: shader stages where quad subgroup operations are supported.<br/>
Metal: shader stages where quadgroup functions are available.
* __requiredSubgroupSizeStages__<br/>
Vulkan: same as `VkPhysicalDeviceSubgroupSizeControlProperties::requiredSubgroupSizeStages`.<br/>
Metal: not supported.
* __minSubgroupSize__<br/>
Vulkan: maximal value of `VkPhysicalDeviceSubgroupSizeControlProperties::minSubgroupSize` across devices.<br/>
Metal: minimum SIMD-group width supported by the selected GPU family.
* __maxSubgroupSize__<br/>
Vulkan: minimal value of `VkPhysicalDeviceSubgroupSizeControlProperties::maxSubgroupSize` across devices.<br/>
Metal: maximum SIMD-group width supported by the selected GPU family.
* __subgroup__<br/>
Vulkan: supported when subgroup stages and operations are reported by `VkPhysicalDeviceSubgroupProperties`.<br/>
Metal: same as SIMD-group function support in the Metal Shading Language.
* __subgroupBroadcastDynamicId__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::subgroupBroadcastDynamicId`.<br/>
Metal: same as SIMD-group broadcast with a runtime lane index.
* __subgroupSizeControl__<br/>
Vulkan: same as `VkPhysicalDeviceSubgroupSizeControlFeatures::subgroupSizeControl`.<br/>
Metal: not supported.
* __shaderSubgroupUniformControlFlow__<br/>
Vulkan: same as `VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR::shaderSubgroupUniformControlFlow`.<br/>
Metal: not supported.
* __shaderMaximalReconvergence__<br/>
Vulkan: same as `VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR::shaderMaximalReconvergence`.<br/>
Metal: not supported.
* __shaderQuadControl__<br/>
Vulkan: same as `VkPhysicalDeviceShaderQuadControlFeaturesKHR::shaderQuadControl`.<br/>
Metal: not supported.

### Types

* __shaderInt8__<br/>
Vulkan: same as `VkPhysicalDeviceShaderFloat16Int8FeaturesKHR::shaderInt8`.<br/>
Metal: same as 8-bit integer scalar and vector types in the Metal Shading Language.
* __shaderInt16__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderInt16`.<br/>
Metal: same as 16-bit integer scalar and vector types in the Metal Shading Language.
* __shaderInt64__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderInt64`.<br/>
Metal: same as 64-bit integer scalar and vector types in Metal 2.3+.
* __shaderFloat16__<br/>
Vulkan: same as `VkPhysicalDeviceShaderFloat16Int8FeaturesKHR::shaderFloat16`.<br/>
Metal: same as `half` scalar and vector types in the Metal Shading Language.
* __shaderFloat64__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderFloat64`.<br/>
Metal: not supported.
* __shaderBFloat16Type__<br/>
Vulkan: bfloat16 shader type support from the bfloat16 shader extension.<br/>
Metal: same as `bfloat` types in Metal 3.1+.
* __shaderBFloat16DotProduct__<br/>
Vulkan: bfloat16 dot-product support from the bfloat16 shader extension.<br/>
Metal: same as bfloat dot-product functions in Metal 3.1+.
* __shaderBFloat16CooperativeMatrix__<br/>
Vulkan: bfloat16 cooperative matrix support when both bfloat16 and cooperative matrix support are available.<br/>
Metal: not supported.
* __shaderFloat8__<br/>
Vulkan: 8-bit floating-point shader type support from float8 extensions such as E4M3 and E5M2.<br/>
Metal: not supported.
* __shaderFloat8CooperativeMatrix__<br/>
Vulkan: 8-bit floating-point cooperative matrix support when both float8 and cooperative matrix support are available.<br/>
Metal: not supported.

### Uniform / Storage buffer

* __storageBuffer16BitAccess__<br/>
Vulkan: same as `VkPhysicalDevice16BitStorageFeatures::storageBuffer16BitAccess`.<br/>
Metal: same as 16-bit types in device address space.
* __uniformAndStorageBuffer16BitAccess__<br/>
Vulkan: same as `VkPhysicalDevice16BitStorageFeatures::uniformAndStorageBuffer16BitAccess`.<br/>
Metal: same as 16-bit types in constant and device address spaces.
* __storageInputOutput16__<br/>
Vulkan: same as `VkPhysicalDevice16BitStorageFeatures::storageInputOutput16`.<br/>
Metal: same as 16-bit shader inputs and outputs where supported by the stage ABI.
* __storageBuffer8BitAccess__<br/>
Vulkan: same as `VkPhysicalDevice8BitStorageFeatures::storageBuffer8BitAccess`.<br/>
Metal: same as 8-bit types in device address space.
* __uniformAndStorageBuffer8BitAccess__<br/>
Vulkan: same as `VkPhysicalDevice8BitStorageFeatures::uniformAndStorageBuffer8BitAccess`.<br/>
Metal: same as 8-bit types in constant and device address spaces.
* __uniformBufferStandardLayout__<br/>
Vulkan: same as `VkPhysicalDeviceUniformBufferStandardLayoutFeatures::uniformBufferStandardLayout`.<br/>
Metal: supported by Metal buffer layout rules used by the backend.
* __scalarBlockLayout__<br/>
Vulkan: same as `VkPhysicalDeviceScalarBlockLayoutFeatures::scalarBlockLayout`.<br/>
Metal: supported by Metal buffer layout rules used by the backend.
* __bufferDeviceAddress__<br/>
Vulkan: same as `VkPhysicalDeviceBufferDeviceAddressFeatures::bufferDeviceAddress`.<br/>
Metal: same as shader-visible buffer pointers in Metal 2.1+.

### Push constant

* __storagePushConstant8__<br/>
Vulkan: same as `VkPhysicalDevice8BitStorageFeatures::storagePushConstant8`.<br/>
Metal: not supported.
* __storagePushConstant16__<br/>
Vulkan: same as `VkPhysicalDevice16BitStorageFeatures::storagePushConstant16`.<br/>
Metal: not supported.

### Atomic

* __fragmentStoresAndAtomics__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::fragmentStoresAndAtomics`.<br/>
Metal: same as writable resources and atomic operations in fragment functions.
* __vertexPipelineStoresAndAtomics__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::vertexPipelineStoresAndAtomics`.<br/>
Metal: same as writable resources and atomic operations in vertex-stage functions.
* __shaderImageInt64Atomics__<br/>
Vulkan: 64-bit integer image atomics from `VK_EXT_shader_image_atomic_int64`.<br/>
Metal: not supported.
* __shaderBufferInt64Atomics__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicInt64Features::shaderBufferInt64Atomics`.<br/>
Metal: same as 64-bit atomics in device address space in Metal 2.4+.
* __shaderSharedInt64Atomics__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicInt64Features::shaderSharedInt64Atomics`.<br/>
Metal: same as 64-bit atomics in threadgroup address space in Metal 2.4+.
* __shaderBufferFloat32Atomics__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderBufferFloat32Atomics`.<br/>
Metal: same as `atomic_float` in device address space.
* __shaderBufferFloat32AtomicAdd__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderBufferFloat32AtomicAdd`.<br/>
Metal: same as atomic add on `atomic_float` in device address space.
* __shaderBufferFloat64Atomics__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderBufferFloat64Atomics`.<br/>
Metal: not supported.
* __shaderBufferFloat64AtomicAdd__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderBufferFloat64AtomicAdd`.<br/>
Metal: not supported.
* __shaderSharedFloat32Atomics__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderSharedFloat32Atomics`.<br/>
Metal: same as `atomic_float` in threadgroup address space.
* __shaderSharedFloat32AtomicAdd__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderSharedFloat32AtomicAdd`.<br/>
Metal: same as atomic add on `atomic_float` in threadgroup address space.
* __shaderSharedFloat64Atomics__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderSharedFloat64Atomics`.<br/>
Metal: not supported.
* __shaderSharedFloat64AtomicAdd__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderSharedFloat64AtomicAdd`.<br/>
Metal: not supported.
* __shaderImageFloat32Atomics__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderImageFloat32Atomics`.<br/>
Metal: not supported.
* __shaderImageFloat32AtomicAdd__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloatFeaturesEXT::shaderImageFloat32AtomicAdd`.<br/>
Metal: not supported.
* __shaderBufferFloat16Atomics__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderBufferFloat16Atomics`.<br/>
Metal: same as half-precision atomics in device address space where supported.
* __shaderBufferFloat16AtomicAdd__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderBufferFloat16AtomicAdd`.<br/>
Metal: same as half-precision atomic add in device address space where supported.
* __shaderBufferFloat16AtomicMinMax__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderBufferFloat16AtomicMinMax`.<br/>
Metal: not supported.
* __shaderBufferFloat32AtomicMinMax__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderBufferFloat32AtomicMinMax`.<br/>
Metal: not supported.
* __shaderBufferFloat64AtomicMinMax__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderBufferFloat64AtomicMinMax`.<br/>
Metal: not supported.
* __shaderSharedFloat16Atomics__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderSharedFloat16Atomics`.<br/>
Metal: same as half-precision atomics in threadgroup address space where supported.
* __shaderSharedFloat16AtomicAdd__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderSharedFloat16AtomicAdd`.<br/>
Metal: same as half-precision atomic add in threadgroup address space where supported.
* __shaderSharedFloat16AtomicMinMax__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderSharedFloat16AtomicMinMax`.<br/>
Metal: not supported.
* __shaderSharedFloat32AtomicMinMax__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderSharedFloat32AtomicMinMax`.<br/>
Metal: not supported.
* __shaderSharedFloat64AtomicMinMax__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderSharedFloat64AtomicMinMax`.<br/>
Metal: not supported.
* __shaderImageFloat32AtomicMinMax__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::shaderImageFloat32AtomicMinMax`.<br/>
Metal: not supported.
* __sparseImageFloat32AtomicMinMax__<br/>
Vulkan: same as `VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT::sparseImageFloat32AtomicMinMax`.<br/>
Metal: not supported.
* __shaderAtomicPackedFp16__<br/>
Vulkan: packed FP16 vector atomic support from `VK_NV_shader_atomic_float16_vector`.<br/>
Metal: not supported.

### Output

* __shaderOutputViewportIndex__<br/>
Vulkan: same as `VkPhysicalDeviceShaderDrawParametersFeatures::shaderOutputViewportIndex` or equivalent shader viewport-index output support.<br/>
Metal: same as vertex shader output `[[viewport_array_index]]`.
* __shaderOutputLayer__<br/>
Vulkan: same as `VkPhysicalDeviceShaderDrawParametersFeatures::shaderOutputLayer` or equivalent shader layer output support.<br/>
Metal: same as vertex shader output `[[render_target_array_index]]`.

### Clock

* __shaderSubgroupClock__<br/>
Vulkan: same as `VkPhysicalDeviceShaderClockFeaturesKHR::shaderSubgroupClock`.<br/>
Metal: not supported.
* __shaderDeviceClock__<br/>
Vulkan: same as `VkPhysicalDeviceShaderClockFeaturesKHR::shaderDeviceClock`.<br/>
Metal: not supported.

### Cooperative matrix / vector

* __cooperativeMatrix__<br/>
Vulkan: same as `VkPhysicalDeviceCooperativeMatrixFeaturesKHR::cooperativeMatrix`.<br/>
Metal: not supported.
* __cooperativeMatrixStages__<br/>
Vulkan: shader stages from `VkPhysicalDeviceCooperativeMatrixPropertiesKHR::cooperativeMatrixSupportedStages`.<br/>
Metal: not supported.
* __cooperativeMatrixConfig__<br/>
Vulkan: set of configurations reported by `vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR`. Can be empty if no default/common configuration is supported.<br/>
Metal: not supported.
* __cooperativeVector__<br/>
Vulkan: cooperative vector shader support from the cooperative vector extension.<br/>
Metal: not supported.
* __cooperativeVectorTraining__<br/>
Vulkan: cooperative vector training support from the cooperative vector extension.<br/>
Metal: not supported.
* __cooperativeVectorConfig__<br/>
Vulkan: set of supported cooperative vector configurations. Can be empty if no default/common configuration is supported.<br/>
Metal: not supported.
* __shaderIntegerDotProduct__<br/>
Vulkan: same as `VkPhysicalDeviceShaderIntegerDotProductFeatures::shaderIntegerDotProduct`.<br/>
Metal: same as integer dot-product functions in Metal 2.3+.
* __integerDotProductFeatures__<br/>
Vulkan: integer dot-product acceleration and capability flags from `VkPhysicalDeviceShaderIntegerDotProductProperties`.<br/>
Metal: same as integer dot-product function capabilities in Metal 2.3+.

## Shader features and limits

* __shaderClipDistance__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderClipDistance`.<br/>
Metal: same as clip distance output `[[clip_distance]]`.
* __shaderCullDistance__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderCullDistance`.<br/>
Metal: not supported.
* __shaderResourceMinLod__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderResourceMinLod`.<br/>
Metal: same as minimum LOD clamp in texture sampling.
* __shaderDrawParameters__<br/>
Vulkan: same as `VkPhysicalDeviceShaderDrawParametersFeatures::shaderDrawParameters`.<br/>
Metal: same as shader inputs such as `[[vertex_id]]`, `[[instance_id]]`, base vertex, and base instance where available.
* __runtimeDescriptorArray__<br/>
Vulkan: same as `VkPhysicalDeviceDescriptorIndexingFeatures::runtimeDescriptorArray`.<br/>
Metal: same as runtime-sized resource arrays in argument buffers.
* __shaderSMBuiltinsNV__<br/>
Vulkan: same as `VkPhysicalDeviceShaderSMBuiltinsFeaturesNV::shaderSMBuiltins`.<br/>
Metal: not supported.
* __shaderCoreBuiltinsARM__<br/>
Vulkan: same as `VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM::shaderCoreBuiltins`.<br/>
Metal: not supported.
* __shaderSampleRateInterpolationFunctions__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::sampleRateShading` plus sample-rate interpolation function support.<br/>
Metal: same as interpolation sampling functions using sample IDs / sample positions.
* __shaderStencilExport__<br/>
Vulkan: supported by `VK_EXT_shader_stencil_export`.<br/>
Metal: not supported.
* __shaderExpectAssume__<br/>
Vulkan: same as `VkPhysicalDeviceShaderExpectAssumeFeaturesKHR::shaderExpectAssume`.<br/>
Metal: not supported.
* __quadDivergentImplicitLod__<br/>
Vulkan: same as `VkPhysicalDeviceDescriptorIndexingProperties::quadDivergentImplicitLod`.<br/>
Metal: same as Metal texture sampling derivative and LOD rules.
* __descriptorHeap__<br/>
Vulkan: descriptor heap style access through descriptor indexing or descriptor buffer based implementation.<br/>
Metal: same as argument buffer resource indexing.
* __shaderStorageImageMultisample__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderStorageImageMultisample`.<br/>
Metal: same as read/write multisample textures where supported.
* __shaderStorageImageReadWithoutFormat__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderStorageImageReadWithoutFormat`.<br/>
Metal: same as reading from writable textures with the declared texture format.
* __shaderStorageImageWriteWithoutFormat__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderStorageImageWriteWithoutFormat`.<br/>
Metal: same as writing to writable textures with the declared texture format.
* __shaderDemoteToHelperInvocation__<br/>
Vulkan: same as `VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures::shaderDemoteToHelperInvocation`.<br/>
Metal: same as `discard_fragment`.
* __shaderTerminateInvocation__<br/>
Vulkan: same as `VkPhysicalDeviceShaderTerminateInvocationFeatures::shaderTerminateInvocation`.<br/>
Metal: same as `discard_fragment`.
* __shaderZeroInitializeWorkgroupMemory__<br/>
Vulkan: same as `VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures::shaderZeroInitializeWorkgroupMemory`.<br/>
Metal: not supported.
* __fragmentShaderBarycentric__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR::fragmentShaderBarycentric`.<br/>
Metal: same as barycentric coordinate attributes in Metal 2.2+.

### Array dynamic indexing

* __shaderSampledImageArrayDynamicIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderSampledImageArrayDynamicIndexing`.<br/>
Metal: same as dynamic indexing of texture arrays or argument-buffer texture arrays.
* __shaderStorageBufferArrayDynamicIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderStorageBufferArrayDynamicIndexing`.<br/>
Metal: same as dynamic indexing of device-buffer arrays or argument-buffer buffer arrays.
* __shaderStorageImageArrayDynamicIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderStorageImageArrayDynamicIndexing`.<br/>
Metal: same as dynamic indexing of writable texture arrays or argument-buffer writable texture arrays.
* __shaderUniformBufferArrayDynamicIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::shaderUniformBufferArrayDynamicIndexing`.<br/>
Metal: same as dynamic indexing of constant-buffer arrays or argument-buffer buffer arrays.
* __shaderInputAttachmentArrayDynamicIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceDescriptorIndexingFeatures::shaderInputAttachmentArrayDynamicIndexing`.<br/>
Metal: not supported.
* __shaderUniformTexelBufferArrayDynamicIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceDescriptorIndexingFeatures::shaderUniformTexelBufferArrayDynamicIndexing`.<br/>
Metal: same as dynamic indexing of buffer-backed texture arrays.
* __shaderStorageTexelBufferArrayDynamicIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceDescriptorIndexingFeatures::shaderStorageTexelBufferArrayDynamicIndexing`.<br/>
Metal: same as dynamic indexing of writable buffer-backed texture arrays.

### Non uniform indexing

* __shaderUniformBufferArrayNonUniformIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceDescriptorIndexingFeatures::shaderUniformBufferArrayNonUniformIndexing`.<br/>
Metal: same as non-uniform indexing of argument-buffer buffer arrays.
* __shaderSampledImageArrayNonUniformIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceDescriptorIndexingFeatures::shaderSampledImageArrayNonUniformIndexing`.<br/>
Metal: same as non-uniform indexing of argument-buffer texture arrays.
* __shaderStorageBufferArrayNonUniformIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceDescriptorIndexingFeatures::shaderStorageBufferArrayNonUniformIndexing`.<br/>
Metal: same as non-uniform indexing of argument-buffer device-buffer arrays.
* __shaderStorageImageArrayNonUniformIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceDescriptorIndexingFeatures::shaderStorageImageArrayNonUniformIndexing`.<br/>
Metal: same as non-uniform indexing of argument-buffer writable texture arrays.
* __shaderInputAttachmentArrayNonUniformIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceDescriptorIndexingFeatures::shaderInputAttachmentArrayNonUniformIndexing`.<br/>
Metal: not supported.
* __shaderUniformTexelBufferArrayNonUniformIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceDescriptorIndexingFeatures::shaderUniformTexelBufferArrayNonUniformIndexing`.<br/>
Metal: same as non-uniform indexing of argument-buffer buffer-backed texture arrays.
* __shaderStorageTexelBufferArrayNonUniformIndexing__<br/>
Vulkan: same as `VkPhysicalDeviceDescriptorIndexingFeatures::shaderStorageTexelBufferArrayNonUniformIndexing`.<br/>
Metal: same as non-uniform indexing of argument-buffer writable buffer-backed texture arrays.

### Memory model

* __vulkanMemoryModel__<br/>
Vulkan: same as `VkPhysicalDeviceVulkanMemoryModelFeatures::vulkanMemoryModel`.<br/>
Metal: not supported.
* __vulkanMemoryModelDeviceScope__<br/>
Vulkan: same as `VkPhysicalDeviceVulkanMemoryModelFeatures::vulkanMemoryModelDeviceScope`.<br/>
Metal: not supported.
* __vulkanMemoryModelAvailabilityVisibilityChains__<br/>
Vulkan: same as `VkPhysicalDeviceVulkanMemoryModelFeatures::vulkanMemoryModelAvailabilityVisibilityChains`.<br/>
Metal: not supported.

### Fragment shader interlock

* __fragmentShaderSampleInterlock__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT::fragmentShaderSampleInterlock`.<br/>
Metal: not supported.
* __fragmentShaderPixelInterlock__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT::fragmentShaderPixelInterlock`.<br/>
Metal: same as raster order groups for pixel/fragment ordering.
* __fragmentShaderShadingRateInterlock__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT::fragmentShaderShadingRateInterlock`.<br/>
Metal: not supported.

### Fragment shading rate

* __pipelineFragmentShadingRate__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShadingRateFeaturesKHR::pipelineFragmentShadingRate`.<br/>
Metal: same as `MTLRasterizationRateMap`.
* __primitiveFragmentShadingRate__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShadingRateFeaturesKHR::primitiveFragmentShadingRate`.<br/>
Metal: not supported.
* __attachmentFragmentShadingRate__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShadingRateFeaturesKHR::attachmentFragmentShadingRate`.<br/>
Metal: same as `MTLRasterizationRateMap`.
* __primitiveFragmentShadingRateWithMultipleViewports__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShadingRatePropertiesKHR::primitiveFragmentShadingRateWithMultipleViewports`.<br/>
Metal: not supported.
* __layeredShadingRateAttachments__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShadingRatePropertiesKHR::layeredShadingRateAttachments`.<br/>
Metal: same as layered `MTLRasterizationRateMap` where supported.
* __fragmentShadingRateWithShaderDepthStencilWrites__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShadingRatePropertiesKHR::fragmentShadingRateWithShaderDepthStencilWrites`.<br/>
Metal: same as `MTLRasterizationRateMap` with depth/stencil writes.
* __fragmentShadingRateWithSampleMask__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShadingRatePropertiesKHR::fragmentShadingRateWithSampleMask`.<br/>
Metal: same as `MTLRasterizationRateMap` with fixed-function sample mask.
* __fragmentShadingRateWithShaderSampleMask__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShadingRatePropertiesKHR::fragmentShadingRateWithShaderSampleMask`.<br/>
Metal: same as `MTLRasterizationRateMap` with shader sample-mask output where supported.
* __fragmentShadingRateWithFragmentShaderInterlock__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShadingRatePropertiesKHR::fragmentShadingRateWithFragmentShaderInterlock`.<br/>
Metal: not supported.
* __fragmentShadingRateWithCustomSampleLocations__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentShadingRatePropertiesKHR::fragmentShadingRateWithCustomSampleLocations`.<br/>
Metal: not supported.
* __fragmentShadingRateTexelSize__<br/>
Vulkan: supported fragment shading rate attachment texel-size range, derived from `minFragmentShadingRateAttachmentTexelSize` and `maxFragmentShadingRateAttachmentTexelSize`.<br/>
Metal: same as `MTLRasterizationRateMapDescriptor` screen size and layer limits.
* __fragmentShadingRates__<br/>
Vulkan: set of supported fragment shading rates from `vkGetPhysicalDeviceFragmentShadingRatesKHR`.<br/>
Metal: same as supported rates in `MTLRasterizationRateMapDescriptor`.

### Fragment density map

* __fragmentDensityMap__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentDensityMapFeaturesEXT::fragmentDensityMap`.<br/>
Metal: same as `MTLRasterizationRateMap`.
* __fragmentDensityMapDynamic__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentDensityMapFeaturesEXT::fragmentDensityMapDynamic`.<br/>
Metal: same as selecting `MTLRasterizationRateMap` at render pass time.
* __fragmentDensityMapNonSubsampledImages__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentDensityMapFeaturesEXT::fragmentDensityMapNonSubsampledImages`.<br/>
Metal: not supported.
* __fragmentDensityInvocations__<br/>
Vulkan: same as `VkPhysicalDeviceFragmentDensityMap2FeaturesEXT::fragmentDensityMapDeferred`.<br/>
Metal: not supported.
* __subsampledLoads__<br/>
Vulkan: subsampled image load support for fragment density map images.<br/>
Metal: not supported.
* __maxSubsampledArrayLayers__<br/>
Vulkan: minimal value of `VkPhysicalDeviceFragmentDensityMap2PropertiesEXT::maxSubsampledArrayLayers`.<br/>
Metal: not supported.
* __perPipeline_maxSubsampledSamplers__<br/>
Vulkan: minimal value of `VkPhysicalDeviceFragmentDensityMap2PropertiesEXT::maxDescriptorSetSubsampledSamplers`.<br/>
Metal: not supported.

### Acceleration structure

* __accelerationStructureIndirectBuild__<br/>
Vulkan: same as `VkPhysicalDeviceAccelerationStructureFeaturesKHR::accelerationStructureIndirectBuild`.<br/>
Metal: same as acceleration structure build commands with indirect parameters in Metal 3.0+ where supported.
* __clusterAccelerationStructure__<br/>
Vulkan: supported by `VK_NV_cluster_acceleration_structure`.<br/>
Metal: not supported.
* __partitionedAccelerationStructure__<br/>
Vulkan: supported by `VK_NV_partitioned_acceleration_structure`.<br/>
Metal: not supported.

### Inline ray tracing

* __rayQuery__<br/>
Vulkan: same as `VkPhysicalDeviceRayQueryFeaturesKHR::rayQuery`.<br/>
Metal: same as intersection query functions in Metal 3.0+.
* __rayQueryStages__<br/>
Vulkan: shader stages where ray query is supported.<br/>
Metal: shader stages where intersection query functions are available.

### Ray tracing

* __rayTracingPipeline__<br/>
Vulkan: same as `VkPhysicalDeviceRayTracingPipelineFeaturesKHR::rayTracingPipeline`.<br/>
Metal: not supported.
* __rayTraversalPrimitiveCulling__<br/>
Vulkan: same as `VkPhysicalDeviceRayTracingPipelineFeaturesKHR::rayTraversalPrimitiveCulling`.<br/>
Metal: not supported.
* __maxRayRecursionDepth__<br/>
Vulkan: minimal value of `VkPhysicalDeviceRayTracingPipelinePropertiesKHR::maxRayRecursionDepth`.<br/>
Metal: not supported.

### Opacity and displacement micromap

* __opacityMicromap__<br/>
Vulkan: same as `VkPhysicalDeviceOpacityMicromapFeaturesEXT::micromap`.<br/>
Metal: not supported.
* __maxOpacity2StateSubdivisionLevel__<br/>
Vulkan: minimal value of `VkPhysicalDeviceOpacityMicromapPropertiesEXT::maxOpacity2StateSubdivisionLevel`.<br/>
Metal: not supported.
* __maxOpacity4StateSubdivisionLevel__<br/>
Vulkan: minimal value of `VkPhysicalDeviceOpacityMicromapPropertiesEXT::maxOpacity4StateSubdivisionLevel`.<br/>
Metal: not supported.
* __displacementMicromap__<br/>
Vulkan: supported by `VK_NV_displacement_micromap`.<br/>
Metal: not supported.
* __maxDisplacementMicromapSubdivisionLevel__<br/>
Vulkan: minimal value of the maximum displacement micromap subdivision level reported by `VK_NV_displacement_micromap`.<br/>
Metal: not supported.

### Shader version

* __maxShaderVersion.spirv__<br/>
Metal: used SPIRV version from emulation layer.
* __maxShaderVersion.metal__<br/>
Maximum shader language or intermediate representation version supported by all merged devices.

### Draw indirect

* __drawIndirectFirstInstance__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::drawIndirectFirstInstance`.<br/>
Metal: same as indirect draw arguments with base instance.
* __drawIndirectCount__<br/>
Vulkan: same as `VkPhysicalDeviceVulkan12Features::drawIndirectCount`, or `VK_KHR_draw_indirect_count`.<br/>
Metal: same as indirect command buffers in Metal 2.0+ where command count is GPU-controlled.
* __maxDrawIndirectCount__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxDrawIndirectCount`.<br/>
Metal: maximum command count for indirect command buffers.

### Multiview

* __multiview__<br/>
Vulkan: same as `VkPhysicalDeviceMultiviewFeatures::multiview`.<br/>
Metal: same as vertex amplification / layered rendering.
* __multiviewGeometryShader__<br/>
Vulkan: same as `VkPhysicalDeviceMultiviewFeatures::multiviewGeometryShader`.<br/>
Metal: not supported.
* __multiviewTessellationShader__<br/>
Vulkan: same as `VkPhysicalDeviceMultiviewFeatures::multiviewTessellationShader`.<br/>
Metal: same as tessellation with vertex amplification / layered rendering where supported.
* __maxMultiviewViewCount__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMultiviewProperties::maxMultiviewViewCount`.<br/>
Metal: maximum vertex amplification count / render target array layer count used for multiview.

### Multi viewport

* __multiViewport__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::multiViewport`.<br/>
Metal: same as `MTLRenderCommandEncoder::setViewports:count:`.
* __maxViewports__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxViewports`.<br/>
Metal: maximum count accepted by `MTLRenderCommandEncoder::setViewports:count:`.

### Sample locations

* __sampleLocations__<br/>
Vulkan: same as `VkPhysicalDeviceSampleLocationsFeaturesEXT::sampleLocations`.<br/>
Metal: same as programmable sample positions in `MTLRenderPassDescriptor`.
* __variableSampleLocations__<br/>
Vulkan: same as `VkPhysicalDeviceSampleLocationsFeaturesEXT::variableSampleLocations`.<br/>
Metal: same as programmable sample positions that may vary between render passes.

### Tessellation

* __tessellationIsolines__<br/>
Vulkan: same as `VkPhysicalDevicePortabilitySubsetFeaturesKHR::tessellationIsolines`.<br/>
Metal: same as `MTLTessellationPartitionMode` / tessellation pipeline support for isolines where available.
* __tessellationPointMode__<br/>
Vulkan: same as `VkPhysicalDevicePortabilitySubsetFeaturesKHR::tessellationPointMode`.<br/>
Metal: same as tessellation point-mode output where available.

### Shader limits

* __maxTexelBufferElements__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxTexelBufferElements`.<br/>
Metal: maximum element count for buffer-backed textures.
* __maxUniformBufferSize__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxUniformBufferRange`.<br/>
Metal: maximum constant-buffer binding size.
* __maxStorageBufferSize__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxStorageBufferRange`.<br/>
Metal: maximum device-buffer binding size.
* __perPipeline_maxUniformBuffersDynamic__<br/>
Vulkan: same as `VkPhysicalDeviceLimits::maxDescriptorSetUniformBuffersDynamic`, or `VkPhysicalDeviceMaintenance7PropertiesKHR::maxDescriptorSetTotalUniformBuffersDynamic`.<br/>
Metal: not supported.
* __perPipeline_maxStorageBuffersDynamic__<br/>
Vulkan: same as `VkPhysicalDeviceLimits::maxDescriptorSetStorageBuffersDynamic`, or `VkPhysicalDeviceMaintenance7PropertiesKHR::maxDescriptorSetTotalStorageBuffersDynamic`.<br/>
Metal: not supported.
* __perPipeline_maxTotalBuffersDynamic__<br/>
Vulkan: same as `VkPhysicalDeviceMaintenance7PropertiesKHR::maxDescriptorSetTotalBuffersDynamic`.<br/>
Metal: not supported.
* __perPipeline__<br/>
Vulkan: per-pipeline descriptor/resource limits derived from descriptor set limits such as `maxDescriptorSet*`.<br/>
Metal: per-pipeline resource limits from Metal buffer, texture, sampler, and argument-buffer binding limits.
* __perStage__<br/>
Vulkan: per-stage descriptor/resource limits derived from `VkPhysicalDeviceLimits::maxPerStageDescriptor*` and related limits.<br/>
Metal: per-stage resource limits from Metal buffer, texture, sampler, and argument-buffer binding limits.
* __maxDescriptorSets__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxBoundDescriptorSets`.<br/>
Metal: not supported.
* __maxTexelOffset__<br/>
Vulkan: minimal absolute texel offset range derived from `minTexelOffset` and `maxTexelOffset`.<br/>
Metal: maximum texture offset supported by sample/read operations with offsets.
* __maxTexelGatherOffset__<br/>
Vulkan: minimal absolute texel gather offset range derived from `minTexelGatherOffset` and `maxTexelGatherOffset`.<br/>
Metal: maximum texture gather offset supported by gather operations.
* __maxFragmentOutputAttachments__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxFragmentOutputAttachments` and color attachment limits.<br/>
Metal: maximum number of color attachments in `MTLRenderPipelineDescriptor`.
* __maxFragmentDualSrcAttachments__<br/>
Vulkan: maximum number of dual-source color attachments supported by all merged devices.<br/>
Metal: maximum number of dual-source color attachments supported by the render pipeline.
* __maxFragmentCombinedOutputResources__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxFragmentCombinedOutputResources`.<br/>
Metal: maximum combined fragment outputs and writable resources supported by the render pipeline.
* __maxPushConstantsSize__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxPushConstantsSize`.<br/>
Metal: not supported.
* __maxVertAmplification__<br/>
Vulkan: not supported.<br/>
Metal: same as maximum vertex amplification count.
* __maxTotalThreadgroupSize__<br/>
Vulkan: not supported.<br/>
Metal: same as maximum total threadgroup memory size.
* __maxTotalTileMemory__<br/>
Vulkan: not supported.<br/>
Metal: same as maximum tile memory size.

### Compute shader

* __maxComputeSharedMemorySize__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxComputeSharedMemorySize`.<br/>
Metal: same as `MTLDevice::maxThreadgroupMemoryLength`.
* __maxComputeWorkGroupInvocations__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxComputeWorkGroupInvocations`.<br/>
Metal: same as `MTLComputePipelineState::maxTotalThreadsPerThreadgroup`.
* __maxComputeWorkGroupSizeX__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxComputeWorkGroupSize[0]`.<br/>
Metal: same as `MTLComputePipelineState::threadExecutionWidth` / `maxTotalThreadsPerThreadgroup` constraints for X dimension.
* __maxComputeWorkGroupSizeY__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxComputeWorkGroupSize[1]`.<br/>
Metal: same as `MTLComputePipelineState::maxTotalThreadsPerThreadgroup` constraints for Y dimension.
* __maxComputeWorkGroupSizeZ__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxComputeWorkGroupSize[2]`.<br/>
Metal: same as `MTLComputePipelineState::maxTotalThreadsPerThreadgroup` constraints for Z dimension.

### Mesh shader

* __taskShader__<br/>
Vulkan: same as `VkPhysicalDeviceMeshShaderFeaturesEXT::taskShader`.<br/>
Metal: not supported.
* __meshShader__<br/>
Vulkan: same as `VkPhysicalDeviceMeshShaderFeaturesEXT::meshShader`.<br/>
Metal: not supported.
* __maxTaskWorkGroupSize__<br/>
Vulkan: minimal task shader workgroup size/invocation limit from `VkPhysicalDeviceMeshShaderPropertiesEXT`.<br/>
Metal: not supported.
* __maxMeshWorkGroupSize__<br/>
Vulkan: minimal mesh shader workgroup size/invocation limit from `VkPhysicalDeviceMeshShaderPropertiesEXT`.<br/>
Metal: not supported.
* __maxMeshOutputVertices__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshOutputVertices`.<br/>
Metal: not supported.
* __maxMeshOutputPrimitives__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshOutputPrimitives`.<br/>
Metal: not supported.
* __maxMeshOutputPerVertexGranularity__<br/>
Vulkan: maximal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::meshOutputPerVertexGranularity`.<br/>
Metal: not supported.
* __maxMeshOutputPerPrimitiveGranularity__<br/>
Vulkan: maximal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::meshOutputPerPrimitiveGranularity`.<br/>
Metal: not supported.
* __maxTaskPayloadSize__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxTaskPayloadSize`.<br/>
Metal: not supported.
* __maxTaskSharedMemorySize__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxTaskSharedMemorySize`.<br/>
Metal: not supported.
* __maxMeshSharedMemorySize__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshSharedMemorySize`.<br/>
Metal: not supported.
* __maxMeshOutputMemorySize__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshOutputMemorySize`.<br/>
Metal: not supported.
* __maxTaskPayloadAndSharedMemorySize__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxTaskPayloadAndSharedMemorySize`.<br/>
Metal: not supported.
* __maxMeshPayloadAndSharedMemorySize__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshPayloadAndSharedMemorySize`.<br/>
Metal: not supported.
* __maxMeshPayloadAndOutputMemorySize__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshPayloadAndOutputMemorySize`.<br/>
Metal: not supported.
* __maxMeshMultiviewViewCount__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxMeshMultiviewViewCount`.<br/>
Metal: not supported.
* __maxPreferredTaskWorkGroupInvocations__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxPreferredTaskWorkGroupInvocations`.<br/>
Metal: not supported.
* __maxPreferredMeshWorkGroupInvocations__<br/>
Vulkan: minimal value of `VkPhysicalDeviceMeshShaderPropertiesEXT::maxPreferredMeshWorkGroupInvocations`.<br/>
Metal: not supported.

### Raster order group

* __maxRasterOrderGroups__<br/>
Vulkan: not supported.<br/>
Metal: maximum raster order group index count.

### Shaders

* __geometryShader__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::geometryShader`.<br/>
Metal: not supported.
* __tessellationShader__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::tessellationShader`.<br/>
Metal: same as Metal tessellation pipeline support.
* __computeShader__<br/>
Supported by both Vulkan and Metal.
* __tileShader__<br/>
Vulkan: supported with subpass shading, such as `VK_HUAWEI_subpass_shading`.<br/>
Metal: same as tile functions / imageblocks.

### Vertex buffer

* __vertexDivisor__<br/>
Vulkan: same as `VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT::vertexAttributeInstanceRateDivisor`.<br/>
Metal: same as `MTLVertexBufferLayoutDescriptor::stepFunction` and `stepRate`.
* __maxVertexAttribDivisor__<br/>
Vulkan: minimal value of `VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT::maxVertexAttribDivisor`.<br/>
Metal: maximum supported `MTLVertexBufferLayoutDescriptor::stepRate`.
* __maxVertexAttributes__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxVertexInputAttributes`.<br/>
Metal: maximum number of `MTLVertexDescriptor::attributes`.
* __maxVertexBuffers__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxVertexInputBindings`.<br/>
Metal: maximum number of vertex buffer bindings.

### Rasterization order attachment access

* __rasterizationOrderColorAttachmentAccess__<br/>
Vulkan: same as `VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT::rasterizationOrderColorAttachmentAccess`.<br/>
Metal: same as raster order groups for color attachment access.
* __rasterizationOrderDepthAttachmentAccess__<br/>
Vulkan: same as `VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT::rasterizationOrderDepthAttachmentAccess`.<br/>
Metal: same as raster order groups for depth attachment access where available.
* __rasterizationOrderStencilAttachmentAccess__<br/>
Vulkan: same as `VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT::rasterizationOrderStencilAttachmentAccess`.<br/>
Metal: same as raster order groups for stencil attachment access where available.

## Buffer

* __vertexFormats__<br/>
Vulkan: formats that support `VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT` in `vkGetPhysicalDeviceFormatProperties`.<br/>
Metal: same as supported `MTLVertexFormat` values.
* __uniformTexBufferFormats__<br/>
Vulkan: formats that support `VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT` in `vkGetPhysicalDeviceFormatProperties`.<br/>
Metal: same as supported `MTLPixelFormat` values for buffer-backed texture reads.
* __storageTexBufferFormats__<br/>
Vulkan: formats that support `VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT` in `vkGetPhysicalDeviceFormatProperties`.<br/>
Metal: same as supported `MTLPixelFormat` values for buffer-backed writable textures.
* __storageTexBufferAtomicFormats__<br/>
Vulkan: formats that support `VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT` in `vkGetPhysicalDeviceFormatProperties`.<br/>
Metal: same as supported `MTLPixelFormat` values for atomic buffer-backed writable textures.
* __accelStructVertexFormats__<br/>
Vulkan: formats that support `VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR` in `vkGetPhysicalDeviceFormatProperties`.<br/>
Metal: same as vertex formats supported by Metal acceleration structure geometry descriptors.

## Image

* __imageCubeArray__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::imageCubeArray`.<br/>
Metal: same as `MTLTextureTypeCubeArray`.
* __textureCompressionASTC_LDR__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::textureCompressionASTC_LDR`.<br/>
Metal: same as ASTC LDR `MTLPixelFormat` support.
* __textureCompressionASTC_HDR__<br/>
Vulkan: same as `VkPhysicalDeviceTextureCompressionASTCHDRFeatures::textureCompressionASTC_HDR`.<br/>
Metal: same as ASTC HDR `MTLPixelFormat` support.
* __textureCompressionBC__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::textureCompressionBC`.<br/>
Metal: same as BC `MTLPixelFormat` support.
* __textureCompressionETC2__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::textureCompressionETC2`.<br/>
Metal: same as ETC2/EAC `MTLPixelFormat` support.
* __multisampleArrayImage__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::multisampleArrayImage`.<br/>
Metal: same as `MTLTextureType2DMultisampleArray`.
* __imageViewFormatList__<br/>
Vulkan: same as `VkPhysicalDeviceImageFormatListFeatures::imageFormatList`.<br/>
Metal: same as texture views created by `MTLTexture::newTextureViewWithPixelFormat`.
* __imageViewExtendedUsage__<br/>
Vulkan: same as `VkPhysicalDeviceMaintenance2Features::imageViewExtendedUsage`.<br/>
Metal: same as `MTLTextureUsage` and texture view usage compatibility.
* __surfaceFormats__<br/>
Vulkan: supported presentation surface formats from `vkGetPhysicalDeviceSurfaceFormatsKHR`.<br/>
Metal: same as supported `CAMetalLayer::pixelFormat` drawable formats.
* __maxImageDimension1D__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxImageDimension1D`.<br/>
Metal: maximum width for `MTLTextureType1D`.
* __maxImageDimension2D__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxImageDimension2D`.<br/>
Metal: maximum width/height for `MTLTextureType2D`.
* __maxImageDimension3D__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxImageDimension3D`.<br/>
Metal: maximum width/height/depth for `MTLTextureType3D`.
* __maxImageDimensionCube__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxImageDimensionCube`.<br/>
Metal: maximum width/height for `MTLTextureTypeCube`.
* __maxImageArrayLayers__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxImageArrayLayers`.<br/>
Metal: maximum array length for array texture types.
* __storageImageAtomicFormats__<br/>
Vulkan: formats that support `VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT` in `vkGetPhysicalDeviceFormatProperties`.<br/>
Metal: same as `MTLPixelFormat` values that support atomics on writable textures.
* __storageImageFormats__<br/>
Vulkan: formats that support `VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT` in `vkGetPhysicalDeviceFormatProperties`.<br/>
Metal: same as `MTLPixelFormat` values that support `MTLTextureUsageShaderWrite`.
* __attachmentBlendFormats__<br/>
Vulkan: formats that support `VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT` in `vkGetPhysicalDeviceFormatProperties`.<br/>
Metal: same as color-renderable `MTLPixelFormat` values that support blending.
* __attachmentFormats__<br/>
Vulkan: formats that support `VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT` or `VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT` in `vkGetPhysicalDeviceFormatProperties`.<br/>
Metal: same as color/depth/stencil renderable `MTLPixelFormat` values.
* __linearSampledFormats__<br/>
Vulkan: formats that support `VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT` in `vkGetPhysicalDeviceFormatProperties`.<br/>
Metal: same as filterable `MTLPixelFormat` values.
* __minmaxFilterFormats__<br/>
Vulkan: formats that support `VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT` in `vkGetPhysicalDeviceFormatProperties`.<br/>
Metal: not supported.
* __hwCompressedAttachmentFormats__<br/>
Attachment formats compatible with lossless hardware compression on all merged devices.
* __lossyCompressedAttachmentFormats__<br/>
Attachment formats compatible with lossy hardware compression on all merged devices.

## Sampler

* __samplerAnisotropy__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::samplerAnisotropy`.<br/>
Metal: same as `MTLSamplerDescriptor::maxAnisotropy`.
* __samplerMirrorClampToEdge__<br/>
Vulkan: supported by `VK_KHR_sampler_mirror_clamp_to_edge`.<br/>
Metal: same as `MTLSamplerAddressModeMirrorClampToEdge`.
* __samplerFilterMinmax__<br/>
Vulkan: same as `VkPhysicalDeviceSamplerFilterMinmaxFeatures::samplerFilterMinmax`.<br/>
Metal: not supported.
* __filterMinmaxImageComponentMapping__<br/>
Vulkan: same as `VkPhysicalDeviceSamplerFilterMinmaxProperties::filterMinmaxImageComponentMapping`.<br/>
Metal: not supported.
* __samplerMipLodBias__<br/>
Vulkan: same as `VkPhysicalDevicePortabilitySubsetFeaturesKHR::samplerMipLodBias`.<br/>
Metal: same as `MTLSamplerDescriptor::lodBias`.
* __samplerYcbcrConversion__<br/>
Vulkan: same as `VkPhysicalDeviceSamplerYcbcrConversionFeatures::samplerYcbcrConversion`.<br/>
Metal: same as YCbCr / YUV texture sampling support.
* __ycbcr2Plane444__<br/>
Vulkan: supported by `VK_EXT_ycbcr_2plane_444_formats`.<br/>
Metal: same as two-plane 4:4:4 YCbCr pixel format support.
* __nonSeamlessCubeMap__<br/>
Vulkan: same as `VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT::nonSeamlessCubeMap`.<br/>
Metal: not supported.
* __maxSamplerAnisotropy__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxSamplerAnisotropy`.<br/>
Metal: maximum supported `MTLSamplerDescriptor::maxAnisotropy`.
* __maxSamplerLodBias__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxSamplerLodBias`.<br/>
Metal: maximum supported `MTLSamplerDescriptor::lodBias`.

## Framebuffer

* __framebufferColorSampleCounts__<br/>
Vulkan: sample counts from `VkPhysicalDeviceLimits::framebufferColorSampleCounts`.<br/>
Metal: same as color-renderable sample counts supported by `MTLDevice`.
* __framebufferDepthSampleCounts__<br/>
Vulkan: sample counts from `VkPhysicalDeviceLimits::framebufferDepthSampleCounts`.<br/>
Metal: same as depth-renderable sample counts supported by `MTLDevice`.
* __maxFramebufferLayers__<br/>
Vulkan: minimal value of `VkPhysicalDeviceLimits::maxFramebufferLayers`.<br/>
Metal: maximum render target array length.

## Render pass

* __variableMultisampleRate__<br/>
Vulkan: same as `VkPhysicalDeviceFeatures::variableMultisampleRate`.<br/>
Metal: supported by Metal render pass and pipeline sample-count rules.
* __separateDepthStencilRW__<br/>
Vulkan: separate depth/stencil read/write layout support from Vulkan 1.1 or `VK_KHR_maintenance2`.<br/>
Metal: same as separate depth and stencil texture usage / attachment state.

## Indirect command buffer

* __deviceGeneratedCommands__<br/>
Vulkan: same as `VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT::deviceGeneratedCommands`.<br/>
Metal: same as `MTLIndirectCommandBuffer` in Metal 2.0+.
* __deviceGeneratedCommandsMultiDrawIndirectCount__<br/>
Vulkan: same as `VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT::deviceGeneratedCommandsMultiDrawIndirectCount`.<br/>
Metal: same as GPU-controlled indirect command execution with `MTLIndirectCommandBuffer` in Metal 2.0+ where available.
* __supportedIndirectCommandsShaderStages__<br/>
Vulkan: shader stages supported for device-generated command preprocessing or generation.<br/>
Metal: shader stages that can write or execute commands through `MTLIndirectCommandBuffer`.
* __supportedIndirectCommandsShaderStagesPipelineBinding__<br/>
Vulkan: shader stages supported for device-generated command pipeline binding.<br/>
Metal: shader stages that can bind pipelines through `MTLIndirectCommandBuffer`.
* __maxIndirectPipelineCount__<br/>
Vulkan: maximum number of pipelines usable by device-generated commands.<br/>
Metal: maximum number of pipelines usable by `MTLIndirectCommandBuffer`.

## Android

* __externalFormatAndroid__<br/>
Vulkan: supported by `VK_ANDROID_external_memory_android_hardware_buffer` external formats.<br/>
Metal: not supported.

## Metal specific

* __metalArgBufferTier__<br/>
Vulkan: not supported.<br/>
Metal: same as `MTLDevice::argumentBuffersSupport`.

## Hardware info

* __queues__<br/>
Vulkan: queue capabilities exposed by queue families, such as graphics, compute, transfer, sparse binding, video, or presentation support.<br/>
Metal: same as command queue and encoder capabilities supported by `MTLDevice`.
* __vendorIds__<br/>
Vulkan: physical device vendor IDs from `VkPhysicalDeviceProperties::vendorID`.<br/>
Metal: same as vendor identification available from the selected `MTLDevice`.
* __devicesIds__<br/>
Vulkan: physical device IDs from `VkPhysicalDeviceProperties::deviceID`.<br/>
Metal: same as device identification available from the selected `MTLDevice`.
