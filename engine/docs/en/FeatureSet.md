source: [FeatureSet.h](../../src/graphics/Public/FeatureSet.h)

## Render States

* alphaToOne
* depthBiasClamp
* depthBounds
* depthClamp
* dualSrcBlend
* fillModeNonSolid
* independentBlend
* logicOp
* sampleRateShading
* constantAlphaColorBlendFactors
* pointPolygons
* separateStencilMaskRef
* triangleFans


## Shader variable types and functions

### Subgroup

* subgroupOperations
* subgroupTypes
* subgroupStages
* subgroupQuadStages
* requiredSubgroupSizeStages
* subgroup
* subgroupBroadcastDynamicId
* subgroupSizeControl
* shaderSubgroupUniformControlFlow
* minSubgroupSize
* maxSubgroupSize

### Types

* shaderInt8
* shaderInt16
* shaderInt64
* shaderFloat16
* shaderFloat64

### Uniform / Storage buffer

* storageBuffer16BitAccess
* uniformAndStorageBuffer16BitAccess
* storageInputOutput16
* storageBuffer8BitAccess
* uniformAndStorageBuffer8BitAccess
* uniformBufferStandardLayout
* scalarBlockLayout
* bufferDeviceAddress

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

* rayQuery
* rayQueryStages

### Ray tracing (GL_EXT_ray_tracing)

* rayTracingPipeline
* rayTraversalPrimitiveCulling
* minRayRecursionDepth

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

* tessellationIsolines
* tessellationPointMode

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

* minComputeSharedMemorySize
* minComputeWorkGroupInvocations
* minComputeWorkGroupSize [X,Y,Z]

### Mesh shader (GL_EXT_mesh_shader)

* taskShader
* meshShader
* minTaskWorkGroupSize
* minMeshWorkGroupSize
* minMeshOutputVertices
* minMeshOutputPrimitives
* maxMeshOutputPerVertexGranularity
* maxMeshOutputPerPrimitiveGranularity
* minTaskPayloadSize
* minTaskSharedMemorySize
* minTaskPayloadAndSharedMemorySize
* minMeshSharedMemorySize
* minMeshPayloadAndSharedMemorySize
* minMeshOutputMemorySize
* minMeshPayloadAndOutputMemorySize
* minMeshMultiviewViewCount
* minPreferredTaskWorkGroupInvocations
* minPreferredMeshWorkGroupInvocations

### Raster order group

* minRasterOrderGroups

### Shaders

* geometryShader
* tessellationShader
* computeShader
* tileShader

### Vertex buffer

* minVertexAttributes
* minVertexBuffers

__- VK_EXT_vertex_attribute_divisor__:
* vertexDivisor
* minVertexAttribDivisor


## Buffer

* vertexFormats
* uniformTexBufferFormats
* storageTexBufferFormats
* storageTexBufferAtomicFormats
* accelStructVertexFormats


## Image

* imageCubeArray
* textureCompressionASTC_LDR
* textureCompressionASTC_HDR
* textureCompressionBC
* textureCompressionETC2
* imageViewMinLod
* multisampleArrayImage
* minImageArrayLayers
* storageImageAtomicFormats
* storageImageFormats
* attachmentBlendFormats
* attachmentFormats
* linearSampledFormats
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

