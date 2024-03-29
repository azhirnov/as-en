#ifdef VKFEATS_STRUCT
	struct VExtensions
	{
	// ---- instance ----
		bool  surface                           : 1;   // VK_KHR_surface 
		bool  surfaceCaps2                      : 1;   // VK_KHR_get_surface_capabilities2 
		bool  swapchainColorspace               : 1;   // VK_EXT_swapchain_colorspace 
		bool  display                           : 1;   // VK_KHR_display 
		bool  directModeDisplay                 : 1;   // VK_EXT_direct_mode_display 
		bool  displayProps2                     : 1;   // VK_KHR_get_display_properties2 
		bool  debugReport                       : 1;   // VK_EXT_debug_report 
		bool  debugMarker                       : 1;   // VK_EXT_debug_marker 
		bool  debugUtils                        : 1;   // VK_EXT_debug_utils 
		bool  validationFlags                   : 1;   // VK_EXT_validation_features 
		bool  deviceGroupCreation               : 1;   // VK_KHR_device_group_creation 
		bool  deviceProps2                      : 1;   // VK_KHR_get_physical_device_properties2 
		bool  externalMemoryCaps                : 1;   // VK_KHR_external_memory_capabilities 
		bool  externalSemaphoreCaps             : 1;   // VK_KHR_external_semaphore_capabilities 
		bool  portabilityEnum                   : 1;   // VK_KHR_portability_enumeration 

	// ---- device ----

		// ---- 1.1 ----
		bool  memRequirements2                  : 1;   // VK_KHR_get_memory_requirements2 
		bool  bindMemory2                       : 1;   // VK_KHR_bind_memory2 
		bool  dedicatedAllocation               : 1;   // VK_KHR_dedicated_allocation 
		bool  descriptorUpdateTemplate          : 1;   // VK_KHR_descriptor_update_template 
		bool  maintenance1                      : 1;   // VK_KHR_maintenance1 
		bool  maintenance2                      : 1;   // VK_KHR_maintenance2 
		bool  maintenance3                      : 1;   // VK_KHR_maintenance3 
		bool  deviceGroup                       : 1;   // VK_KHR_device_group 
		bool  storageBufferClass                : 1;   // VK_KHR_storage_buffer_storage_class 
		bool  subgroup                          : 1;   //  
		bool  relaxedBlockLayout                : 1;   // VK_KHR_relaxed_block_layout 

		// ---- ext ----
		bool  multiview                         : 1;   // VK_KHR_multiview 
		bool  variablePointers                  : 1;   // VK_KHR_variable_pointers 
		bool  samplerYcbcrConversion            : 1;   // VK_KHR_sampler_ycbcr_conversion 
		bool  shaderDrawParams                  : 1;   // VK_KHR_shader_draw_parameters 
		bool  storage16bit                      : 1;   // VK_KHR_16bit_storage 

		// ---- 1.2 ----
		bool  renderPass2                       : 1;   // VK_KHR_create_renderpass2 
		bool  shaderFloatControls               : 1;   // VK_KHR_shader_float_controls 
		bool  spirv14                           : 1;   // VK_KHR_spirv_1_4 
		bool  imageFormatList                   : 1;   // VK_KHR_image_format_list 
		bool  driverProperties                  : 1;   // VK_KHR_driver_properties 

		// ---- ext ----
		bool  storage8bits                      : 1;   // VK_KHR_8bit_storage 
		bool  shaderAtomicInt64                 : 1;   // VK_KHR_shader_atomic_int64 
		bool  shaderFloat16Int8                 : 1;   // VK_KHR_shader_float16_int8 
		bool  samplerMirrorClamp                : 1;   // VK_KHR_sampler_mirror_clamp_to_edge 
		bool  drawIndirectCount                 : 1;   // VK_KHR_draw_indirect_count 
		bool  descriptorIndexing                : 1;   // VK_EXT_descriptor_indexing 
		bool  samplerFilterMinmax               : 1;   // VK_EXT_sampler_filter_minmax 
		bool  scalarBlockLayout                 : 1;   // VK_EXT_scalar_block_layout 
		bool  subgroupExtendedTypes             : 1;   // VK_KHR_shader_subgroup_extended_types 
		bool  hostQueryReset                    : 1;   // VK_EXT_host_query_reset 
		bool  timelineSemaphore                 : 1;   // VK_KHR_timeline_semaphore 
		bool  bufferDeviceAddress               : 1;   // VK_KHR_buffer_device_address 
		bool  memoryModel                       : 1;   // VK_KHR_vulkan_memory_model 
		bool  uniformBufferStandardLayout       : 1;   // VK_KHR_uniform_buffer_standard_layout 
		bool  shaderViewportIndexLayer          : 1;   // VK_EXT_shader_viewport_index_layer 

		// ---- 1.3 ----
		bool  formatFeat2                       : 1;   // VK_KHR_format_feature_flags2 
		bool  texelBufferAlignment              : 1;   // VK_EXT_texel_buffer_alignment 

		// ---- ext ----
		bool  maintenance4                      : 1;   // VK_KHR_maintenance4 
		bool  synchronization2                  : 1;   // VK_KHR_synchronization2 
		bool  zeroInitializeWorkgroupMem        : 1;   // VK_KHR_zero_initialize_workgroup_memory 
		bool  inlineUniformBlock                : 1;   // VK_EXT_inline_uniform_block 
		bool  subgroupSizeControl               : 1;   // VK_EXT_subgroup_size_control 
		bool  astcHdr                           : 1;   // VK_EXT_texture_compression_astc_hdr 
		bool  shaderTerminateInvocation         : 1;   // VK_KHR_shader_terminate_invocation 
		bool  ycbcr2Plane444                    : 1;   // VK_EXT_ycbcr_2plane_444_formats 
		bool  shaderDemoteToHelperInvocation    : 1;   // VK_EXT_shader_demote_to_helper_invocation 
		bool  swapchain                         : 1;   // VK_KHR_swapchain 
		bool  displaySwapchain                  : 1;   // VK_KHR_display_swapchain 
		bool  depthRangeUnrestricted            : 1;   // VK_EXT_depth_range_unrestricted 
		bool  memoryPriority                    : 1;   // VK_EXT_memory_priority 
		bool  vertexDivisor                     : 1;   // VK_EXT_vertex_attribute_divisor 
		bool  depthClip                         : 1;   // VK_EXT_depth_clip_enable 
		bool  portabilitySubset                 : 1;   // VK_KHR_portability_subset 
		bool  loadOpNone                        : 1;   // VK_EXT_load_store_op_none 
		bool  pagebleDeviceLocalMemory          : 1;   // VK_EXT_pageable_device_local_memory 
		bool  sampleLocations                   : 1;   // VK_EXT_sample_locations 
		bool  fragmentBarycentric               : 1;   // VK_KHR_fragment_shader_barycentric 
		bool  meshShader                        : 1;   // VK_EXT_mesh_shader 
		bool  rasterOrderAttachment             : 1;   // VK_EXT_rasterization_order_attachment_access 
		bool  cooperativeMatrix                 : 1;   // VK_KHR_cooperative_matrix 
		bool  nonSeamlessCubeMap                : 1;   // VK_EXT_non_seamless_cube_map 
		bool  fragShaderInterlock               : 1;   // VK_EXT_fragment_shader_interlock 
		bool  shaderClock                       : 1;   // VK_KHR_shader_clock 
		bool  shaderAtomicFloat                 : 1;   // VK_EXT_shader_atomic_float 
		bool  shaderAtomicFloat2                : 1;   // VK_EXT_shader_atomic_float2 
		bool  shaderSubgroupUniformControlFlow  : 1;   // VK_KHR_shader_subgroup_uniform_control_flow 
		bool  workgroupMemExplicitLayout        : 1;   // VK_KHR_workgroup_memory_explicit_layout 
		bool  shaderImageAtomicInt64            : 1;   // VK_EXT_shader_image_atomic_int64 
		bool  pplnExecProps                     : 1;   // VK_KHR_pipeline_executable_properties 
		bool  queueGlobalPriority               : 1;   // VK_KHR_global_priority 
		bool  externalMemory                    : 1;   // VK_KHR_external_memory 
		bool  externalSemaphore                 : 1;   // VK_KHR_external_semaphore 
		bool  externalSemaphoreFd               : 1;   // VK_KHR_external_semaphore_fd 
		bool  queueFamilyForeign                : 1;   // VK_EXT_queue_family_foreign 
		bool  androidExternalMemoryHwBuf        : 1;   // VK_ANDROID_external_memory_android_hardware_buffer 
		bool  performanceQuery                  : 1;   // VK_KHR_performance_query 
		bool  calibratedTimestamps              : 1;   // VK_EXT_calibrated_timestamps 
		bool  toolingInfo                       : 1;   // VK_EXT_tooling_info 
		bool  memoryBudget                      : 1;   // VK_EXT_memory_budget 
		bool  memoryReport                      : 1;   // VK_EXT_device_memory_report 
		bool  fragShadingRate                   : 1;   // VK_KHR_fragment_shading_rate 
		bool  pipelineLibrary                   : 1;   // VK_KHR_pipeline_library 
		bool  deferredHostOps                   : 1;   // VK_KHR_deferred_host_operations 
		bool  accelerationStructure             : 1;   // VK_KHR_acceleration_structure 
		bool  rayTracingPipeline                : 1;   // VK_KHR_ray_tracing_pipeline 
		bool  rayQuery                          : 1;   // VK_KHR_ray_query 
		bool  rayTracingMaintenance1            : 1;   // VK_KHR_ray_tracing_maintenance1 
		bool  rayTracingPositionFetch           : 1;   // VK_KHR_ray_tracing_position_fetch 
		bool  astcDecodeMode                    : 1;   // VK_EXT_astc_decode_mode 
		bool  imageCompressionCtrl              : 1;   // VK_EXT_image_compression_control 
		bool  swapchainCompressionCtrl          : 1;   // VK_EXT_image_compression_control_swapchain 
		bool  videoQueue                        : 1;   // VK_KHR_video_queue 
		bool  videoEncodeQueue                  : 1;   // VK_KHR_video_encode_queue 
		bool  videoDecodeQueue                  : 1;   // VK_KHR_video_decode_queue 
		bool  videoDecodeH264                   : 1;   // VK_KHR_video_decode_h264 
		bool  videoDecodeH265                   : 1;   // VK_KHR_video_decode_h265 
		bool  videoEncodeH264                   : 1;   // VK_KHR_video_encode_h264 
		bool  videoEncodeH265                   : 1;   // VK_KHR_video_encode_h265 
		bool  imageFootprintNV                  : 1;   // VK_NV_shader_image_footprint 
		bool  deviceGeneratedCmdsNV             : 1;   // VK_NV_device_generated_commands 
		bool  shaderSMBuiltinsNV                : 1;   // VK_NV_shader_sm_builtins 
		bool  shaderCorePropsAMD                : 1;   // VK_AMD_shader_core_properties 
		bool  shaderCorePropsAMD2               : 1;   // VK_AMD_shader_core_properties2 
		bool  shaderCoreBuiltinsARM             : 1;   // VK_ARM_shader_core_builtins 
		bool  subpassShadingHW                  : 1;   // VK_HUAWEI_subpass_shading 
		bool  incrementalPresent                : 1;   // VK_KHR_incremental_present 
		bool  presentId                         : 1;   // VK_KHR_present_id 
		bool  presentWait                       : 1;   // VK_KHR_present_wait 

		VExtensions () { ZeroMem( OUT this, Sizeof(*this) ); }
	};


	struct VProperties
	{
		VkPhysicalDeviceProperties         properties;
		VkPhysicalDeviceFeatures           features;
		VkPhysicalDeviceMemoryProperties   memoryProperties;
		VkPhysicalDeviceSubgroupProperties subgroupProperties;

		// VK_KHR_maintenance3
		VkPhysicalDeviceMaintenance3PropertiesKHR  maintenance3Props;

		// VK_KHR_multiview
		VkPhysicalDeviceMultiviewFeaturesKHR  multiviewFeats;
		VkPhysicalDeviceMultiviewPropertiesKHR  multiviewProps;

		// VK_KHR_variable_pointers
		VkPhysicalDeviceVariablePointersFeaturesKHR  variablePointersFeats;

		// VK_KHR_sampler_ycbcr_conversion
		VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR  samplerYcbcrConversionFeats;

		// VK_KHR_16bit_storage
		VkPhysicalDevice16BitStorageFeaturesKHR  storage16bitFeats;

		// VK_KHR_driver_properties
		VkPhysicalDeviceDriverPropertiesKHR  driverPropertiesProps;

		// VK_KHR_8bit_storage
		VkPhysicalDevice8BitStorageFeaturesKHR  storage8bitsFeats;

		// VK_KHR_shader_atomic_int64
		VkPhysicalDeviceShaderAtomicInt64FeaturesKHR  shaderAtomicInt64Feats;

		// VK_KHR_shader_float16_int8
		VkPhysicalDeviceShaderFloat16Int8FeaturesKHR  shaderFloat16Int8Feats;

		// VK_EXT_descriptor_indexing
		VkPhysicalDeviceDescriptorIndexingFeaturesEXT  descriptorIndexingFeats;
		VkPhysicalDeviceDescriptorIndexingPropertiesEXT  descriptorIndexingProps;

		// VK_EXT_sampler_filter_minmax
		VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT  samplerFilterMinmaxProps;

		// VK_EXT_scalar_block_layout
		VkPhysicalDeviceScalarBlockLayoutFeaturesEXT  scalarBlockLayoutFeats;

		// VK_KHR_shader_subgroup_extended_types
		VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR  subgroupExtendedTypesFeats;

		// VK_EXT_host_query_reset
		VkPhysicalDeviceHostQueryResetFeaturesEXT  hostQueryResetFeats;

		// VK_KHR_timeline_semaphore
		VkPhysicalDeviceTimelineSemaphoreFeaturesKHR  timelineSemaphoreFeats;
		VkPhysicalDeviceTimelineSemaphorePropertiesKHR  timelineSemaphoreProps;

		// VK_KHR_buffer_device_address
		VkPhysicalDeviceBufferDeviceAddressFeaturesKHR  bufferDeviceAddressFeats;

		// VK_KHR_vulkan_memory_model
		VkPhysicalDeviceVulkanMemoryModelFeaturesKHR  memoryModelFeats;

		// VK_KHR_uniform_buffer_standard_layout
		VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR  uniformBufferStandardLayoutFeats;

		// VK_EXT_texel_buffer_alignment
		VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT  texelBufferAlignmentFeats;
		VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT  texelBufferAlignmentProps;

		// VK_KHR_maintenance4
		VkPhysicalDeviceMaintenance4FeaturesKHR  maintenance4Feats;
		VkPhysicalDeviceMaintenance4PropertiesKHR  maintenance4Props;

		// VK_KHR_synchronization2
		VkPhysicalDeviceSynchronization2FeaturesKHR  synchronization2Feats;

		// VK_KHR_zero_initialize_workgroup_memory
		VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR  zeroInitializeWorkgroupMemFeats;

		// VK_EXT_inline_uniform_block
		VkPhysicalDeviceInlineUniformBlockFeaturesEXT  inlineUniformBlockFeats;
		VkPhysicalDeviceInlineUniformBlockPropertiesEXT  inlineUniformBlockProps;

		// VK_EXT_subgroup_size_control
		VkPhysicalDeviceSubgroupSizeControlFeaturesEXT  subgroupSizeControlFeats;
		VkPhysicalDeviceSubgroupSizeControlPropertiesEXT  subgroupSizeControlProps;

		// VK_EXT_texture_compression_astc_hdr
		VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT  astcHdrFeats;

		// VK_KHR_shader_terminate_invocation
		VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR  shaderTerminateInvocationFeats;

		// VK_EXT_ycbcr_2plane_444_formats
		VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT  ycbcr2Plane444Feats;

		// VK_EXT_shader_demote_to_helper_invocation
		VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT  shaderDemoteToHelperInvocationFeats;

		// VK_EXT_memory_priority
		VkPhysicalDeviceMemoryPriorityFeaturesEXT  memoryPriorityFeats;

		// VK_EXT_vertex_attribute_divisor
		VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT  vertexDivisorFeats;
		VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT  vertexDivisorProps;

		// VK_EXT_depth_clip_enable
		VkPhysicalDeviceDepthClipEnableFeaturesEXT  depthClipFeats;

		// VK_KHR_portability_subset
		VkPhysicalDevicePortabilitySubsetFeaturesKHR  portabilitySubsetFeats;
		VkPhysicalDevicePortabilitySubsetPropertiesKHR  portabilitySubsetProps;

		// VK_EXT_pageable_device_local_memory
		VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT  pagebleDeviceLocalMemoryFeats;

		// VK_EXT_sample_locations
		VkPhysicalDeviceSampleLocationsPropertiesEXT  sampleLocationsProps;

		// VK_KHR_fragment_shader_barycentric
		VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR  fragmentBarycentricFeats;
		VkPhysicalDeviceFragmentShaderBarycentricPropertiesKHR  fragmentBarycentricProps;

		// VK_EXT_mesh_shader
		VkPhysicalDeviceMeshShaderFeaturesEXT  meshShaderFeats;
		VkPhysicalDeviceMeshShaderPropertiesEXT  meshShaderProps;

		// VK_KHR_cooperative_matrix
		VkPhysicalDeviceCooperativeMatrixFeaturesKHR  cooperativeMatrixFeats;
		VkPhysicalDeviceCooperativeMatrixPropertiesKHR  cooperativeMatrixProps;

		// VK_EXT_non_seamless_cube_map
		VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT  nonSeamlessCubeMapFeats;

		// VK_EXT_fragment_shader_interlock
		VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT  fragShaderInterlockFeats;

		// VK_KHR_shader_clock
		VkPhysicalDeviceShaderClockFeaturesKHR  shaderClockFeats;

		// VK_EXT_shader_atomic_float
		VkPhysicalDeviceShaderAtomicFloatFeaturesEXT  shaderAtomicFloatFeats;

		// VK_EXT_shader_atomic_float2
		VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT  shaderAtomicFloat2Feats;

		// VK_KHR_shader_subgroup_uniform_control_flow
		VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR  shaderSubgroupUniformControlFlowFeats;

		// VK_KHR_workgroup_memory_explicit_layout
		VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR  workgroupMemExplicitLayoutFeats;

		// VK_EXT_shader_image_atomic_int64
		VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT  shaderImageAtomicInt64Feats;

		// VK_KHR_pipeline_executable_properties
		VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR  pplnExecPropsFeats;

		// VK_KHR_performance_query
		VkPhysicalDevicePerformanceQueryFeaturesKHR  performanceQueryFeats;
		VkPhysicalDevicePerformanceQueryPropertiesKHR  performanceQueryProps;

		// VK_EXT_memory_budget
		VkPhysicalDeviceMemoryBudgetPropertiesEXT  memoryBudgetProps;

		// VK_EXT_device_memory_report
		VkPhysicalDeviceDeviceMemoryReportFeaturesEXT  memoryReportFeats;

		// VK_KHR_fragment_shading_rate
		VkPhysicalDeviceFragmentShadingRateFeaturesKHR  fragShadingRateFeats;
		VkPhysicalDeviceFragmentShadingRatePropertiesKHR  fragShadingRateProps;

		// VK_KHR_acceleration_structure
		VkPhysicalDeviceAccelerationStructureFeaturesKHR  accelerationStructureFeats;
		VkPhysicalDeviceAccelerationStructurePropertiesKHR  accelerationStructureProps;

		// VK_KHR_ray_tracing_pipeline
		VkPhysicalDeviceRayTracingPipelineFeaturesKHR  rayTracingPipelineFeats;
		VkPhysicalDeviceRayTracingPipelinePropertiesKHR  rayTracingPipelineProps;

		// VK_KHR_ray_query
		VkPhysicalDeviceRayQueryFeaturesKHR  rayQueryFeats;

		// VK_KHR_ray_tracing_maintenance1
		VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR  rayTracingMaintenance1Feats;

		// VK_KHR_ray_tracing_position_fetch
		VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR  rayTracingPositionFetchFeats;

		// VK_EXT_image_compression_control
		VkPhysicalDeviceImageCompressionControlFeaturesEXT  imageCompressionCtrlFeats;

		// VK_EXT_image_compression_control_swapchain
		VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT  swapchainCompressionCtrlFeats;

		// VK_NV_shader_image_footprint
		VkPhysicalDeviceShaderImageFootprintFeaturesNV  imageFootprintNVFeats;

		// VK_NV_device_generated_commands
		VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV  deviceGeneratedCmdsNVFeats;
		VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV  deviceGeneratedCmdsNVProps;

		// VK_NV_shader_sm_builtins
		VkPhysicalDeviceShaderSMBuiltinsFeaturesNV  shaderSMBuiltinsNVFeats;
		VkPhysicalDeviceShaderSMBuiltinsPropertiesNV  shaderSMBuiltinsNVProps;

		// VK_AMD_shader_core_properties
		VkPhysicalDeviceShaderCorePropertiesAMD  shaderCorePropsAMDProps;

		// VK_ARM_shader_core_builtins
		VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM  shaderCoreBuiltinsARMFeats;
		VkPhysicalDeviceShaderCoreBuiltinsPropertiesARM  shaderCoreBuiltinsARMProps;

		// VK_HUAWEI_subpass_shading
		VkPhysicalDeviceSubpassShadingFeaturesHUAWEI  subpassShadingHWFeats;
		VkPhysicalDeviceSubpassShadingPropertiesHUAWEI  subpassShadingHWProps;

		// VK_KHR_present_id
		VkPhysicalDevicePresentIdFeaturesKHR  presentIdFeats;

		// VK_KHR_present_wait
		VkPhysicalDevicePresentWaitFeaturesKHR  presentWaitFeats;

		VProperties () { ZeroMem( OUT this, Sizeof(*this) ); }
	};
#endif // VKFEATS_STRUCT


#ifdef VKFEATS_FN_DECL
	ND_ static Array<const char*>  _GetInstanceExtensions (InstanceVersion ver);
	ND_ static Array<const char*>  _GetDeviceExtensions (DeviceVersion ver);
	ND_ String  _GetVulkanExtensionsString () const;
	void  _InitFeaturesAndProperties (void** nextFeat);
	void  _CheckInstanceExtensions ();
	void  _CheckDeviceExtensions ();
#endif // VKFEATS_FN_DECL


#ifdef VKFEATS_FN_IMPL
	Array<const char*>  VDeviceInitializer::_GetInstanceExtensions (InstanceVersion ver)
	{
		static const char* core_1_1_ext_1_0[] =
		{
			VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
			VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
			VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
			VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
		};
		static const char* ext_1_0[] =
		{
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
			VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME,
			VK_KHR_DISPLAY_EXTENSION_NAME,
			VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME,
			VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
			VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
			VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME,
			VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
		};
		CHECK_ERR(( ver >= InstanceVersion{1,0} ));

		Array<const char*> result;
		if ( ver >= InstanceVersion{1,0} and ver < InstanceVersion{1,1} )
			result.insert( result.end(), std::begin(core_1_1_ext_1_0), std::end(core_1_1_ext_1_0) );
		if ( ver >= InstanceVersion{1,0} )
			result.insert( result.end(), std::begin(ext_1_0), std::end(ext_1_0) );

		return result;
	}

	Array<const char*>  VDeviceInitializer::_GetDeviceExtensions (DeviceVersion ver)
	{
		static const char* core_1_1_ext_1_0[] =
		{
			VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
			VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
			VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
			VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME,
			VK_KHR_MAINTENANCE1_EXTENSION_NAME,
			VK_KHR_MAINTENANCE2_EXTENSION_NAME,
			VK_KHR_MAINTENANCE3_EXTENSION_NAME,
			VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
			VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
			VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME,
		};
		static const char* core_1_2_ext_1_0[] =
		{
			VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
			VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
			VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME,
			VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME,
		};
		static const char* core_1_2_ext_1_1[] =
		{
			VK_KHR_SPIRV_1_4_EXTENSION_NAME,
		};
		static const char* core_1_3_ext_1_0[] =
		{
			VK_KHR_FORMAT_FEATURE_FLAGS_2_EXTENSION_NAME,
			VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME,
		};
		static const char* ext_1_0[] =
		{
			VK_KHR_MULTIVIEW_EXTENSION_NAME,
			VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME,
			VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
			VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
			VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
			VK_KHR_8BIT_STORAGE_EXTENSION_NAME,
			VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME,
			VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME,
			VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME,
			VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME,
			VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
			VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME,
			VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME,
			VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME,
			VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
			VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME,
			VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME,
			VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME,
			VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
			VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME,
			VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME,
			VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME,
			VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME,
			VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME,
			VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME,
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME,
			VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME,
			VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,
			VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME,
			VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME,
			VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
			VK_EXT_LOAD_STORE_OP_NONE_EXTENSION_NAME,
			VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME,
			VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME,
			VK_KHR_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME,
			VK_EXT_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_EXTENSION_NAME,
			VK_KHR_COOPERATIVE_MATRIX_EXTENSION_NAME,
			VK_EXT_NON_SEAMLESS_CUBE_MAP_EXTENSION_NAME,
			VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME,
			VK_KHR_SHADER_CLOCK_EXTENSION_NAME,
			VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
			VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME,
			VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME,
			VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME,
			VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME,
			VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME,
			VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
			VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
			VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME,
			VK_EXT_QUEUE_FAMILY_FOREIGN_EXTENSION_NAME,
			#ifdef VK_ANDROID_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER_EXTENSION_NAME
				VK_ANDROID_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER_EXTENSION_NAME,
			#endif
			VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME,
			VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME,
			VK_EXT_TOOLING_INFO_EXTENSION_NAME,
			VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
			VK_EXT_DEVICE_MEMORY_REPORT_EXTENSION_NAME,
			VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
			VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
			VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME,
			VK_EXT_IMAGE_COMPRESSION_CONTROL_EXTENSION_NAME,
			VK_EXT_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_EXTENSION_NAME,
			VK_NV_SHADER_IMAGE_FOOTPRINT_EXTENSION_NAME,
			VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME,
			VK_AMD_SHADER_CORE_PROPERTIES_2_EXTENSION_NAME,
			VK_ARM_SHADER_CORE_BUILTINS_EXTENSION_NAME,
			VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME,
			VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME,
			VK_KHR_PRESENT_ID_EXTENSION_NAME,
			VK_KHR_PRESENT_WAIT_EXTENSION_NAME,
		};
		static const char* ext_1_1[] =
		{
			VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME,
			VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
			VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
			VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME,
			VK_EXT_MESH_SHADER_EXTENSION_NAME,
			VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME,
			VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
			VK_KHR_RAY_QUERY_EXTENSION_NAME,
			VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME,
			VK_KHR_RAY_TRACING_POSITION_FETCH_EXTENSION_NAME,
			VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,
			VK_KHR_VIDEO_ENCODE_QUEUE_EXTENSION_NAME,
			VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME,
			VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME,
			VK_KHR_VIDEO_DECODE_H265_EXTENSION_NAME,
			VK_KHR_VIDEO_ENCODE_H264_EXTENSION_NAME,
			VK_KHR_VIDEO_ENCODE_H265_EXTENSION_NAME,
			VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME,
			VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME,
		};
		CHECK_ERR(( ver >= DeviceVersion{1,0} ));

		Array<const char*> result;
		if ( ver >= DeviceVersion{1,0} and ver < DeviceVersion{1,1} )
			result.insert( result.end(), std::begin(core_1_1_ext_1_0), std::end(core_1_1_ext_1_0) );
		if ( ver >= DeviceVersion{1,0} and ver < DeviceVersion{1,2} )
			result.insert( result.end(), std::begin(core_1_2_ext_1_0), std::end(core_1_2_ext_1_0) );
		if ( ver >= DeviceVersion{1,1} and ver < DeviceVersion{1,2} )
			result.insert( result.end(), std::begin(core_1_2_ext_1_1), std::end(core_1_2_ext_1_1) );
		if ( ver >= DeviceVersion{1,0} and ver < DeviceVersion{1,3} )
			result.insert( result.end(), std::begin(core_1_3_ext_1_0), std::end(core_1_3_ext_1_0) );
		if ( ver >= DeviceVersion{1,0} )
			result.insert( result.end(), std::begin(ext_1_0), std::end(ext_1_0) );
		if ( ver >= DeviceVersion{1,1} )
			result.insert( result.end(), std::begin(ext_1_1), std::end(ext_1_1) );

		return result;
	}

	void  VDeviceInitializer::_CheckInstanceExtensions ()
	{
		CHECK_ERRV(( GetInstanceVersion() >= InstanceVersion{1,0} ));

		_extensions.surface                           = (HasInstanceExtension( VK_KHR_SURFACE_EXTENSION_NAME ));
		_extensions.surfaceCaps2                      = (HasInstanceExtension( VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME ));
		_extensions.swapchainColorspace               = (HasInstanceExtension( VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME ));
		_extensions.display                           = (HasInstanceExtension( VK_KHR_DISPLAY_EXTENSION_NAME ));
		_extensions.directModeDisplay                 = (HasInstanceExtension( VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME ));
		_extensions.displayProps2                     = (HasInstanceExtension( VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME ));
		_extensions.debugReport                       = (HasInstanceExtension( VK_EXT_DEBUG_REPORT_EXTENSION_NAME ));
		_extensions.debugMarker                       = (HasInstanceExtension( VK_EXT_DEBUG_MARKER_EXTENSION_NAME ));
		_extensions.debugUtils                        = (HasInstanceExtension( VK_EXT_DEBUG_UTILS_EXTENSION_NAME ));
		_extensions.validationFlags                   = (HasInstanceExtension( VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME ));
		_extensions.deviceGroupCreation               = (GetInstanceVersion() >= InstanceVersion{1,1}) or (HasInstanceExtension( VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME ));
		_extensions.deviceProps2                      = (GetInstanceVersion() >= InstanceVersion{1,1}) or (HasInstanceExtension( VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME ));
		_extensions.externalMemoryCaps                = (GetInstanceVersion() >= InstanceVersion{1,1}) or (HasInstanceExtension( VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME ));
		_extensions.externalSemaphoreCaps             = (GetInstanceVersion() >= InstanceVersion{1,1}) or (HasInstanceExtension( VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME ));
		_extensions.portabilityEnum                   = (HasInstanceExtension( VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME ));
	}

	void  VDeviceInitializer::_CheckDeviceExtensions ()
	{
		CHECK_ERRV(( GetDeviceVersion() >= DeviceVersion{1,0} ));

		_extensions.memRequirements2                  = (GetDeviceVersion() >= DeviceVersion{1,1}) or (HasDeviceExtension( VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME ));
		_extensions.bindMemory2                       = (GetDeviceVersion() >= DeviceVersion{1,1}) or (HasDeviceExtension( VK_KHR_BIND_MEMORY_2_EXTENSION_NAME ));
		_extensions.dedicatedAllocation               = (GetDeviceVersion() >= DeviceVersion{1,1}) or (HasDeviceExtension( VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME ));
		_extensions.descriptorUpdateTemplate          = (GetDeviceVersion() >= DeviceVersion{1,1}) or (HasDeviceExtension( VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME ));
		_extensions.maintenance1                      = (GetDeviceVersion() >= DeviceVersion{1,1}) or (HasDeviceExtension( VK_KHR_MAINTENANCE1_EXTENSION_NAME ));
		_extensions.maintenance2                      = (GetDeviceVersion() >= DeviceVersion{1,1}) or (HasDeviceExtension( VK_KHR_MAINTENANCE2_EXTENSION_NAME ));
		_extensions.maintenance3                      = (GetDeviceVersion() >= DeviceVersion{1,1}) or (HasDeviceExtension( VK_KHR_MAINTENANCE3_EXTENSION_NAME ));
		_extensions.deviceGroup                       = (GetDeviceVersion() >= DeviceVersion{1,1}) or (HasDeviceExtension( VK_KHR_DEVICE_GROUP_EXTENSION_NAME ));
		_extensions.storageBufferClass                = (GetDeviceVersion() >= DeviceVersion{1,1}) or (HasDeviceExtension( VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME ));
		_extensions.subgroup                          = (GetDeviceVersion() >= DeviceVersion{1,1});
		_extensions.relaxedBlockLayout                = (GetDeviceVersion() >= DeviceVersion{1,1}) or (HasDeviceExtension( VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME ));
		_extensions.multiview                         = (HasDeviceExtension( VK_KHR_MULTIVIEW_EXTENSION_NAME ));
		_extensions.variablePointers                  = (HasDeviceExtension( VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME ));
		_extensions.samplerYcbcrConversion            = (HasDeviceExtension( VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME ));
		_extensions.shaderDrawParams                  = (HasDeviceExtension( VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME ));
		_extensions.storage16bit                      = (HasDeviceExtension( VK_KHR_16BIT_STORAGE_EXTENSION_NAME ));
		_extensions.renderPass2                       = (GetDeviceVersion() >= DeviceVersion{1,2}) or (HasDeviceExtension( VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME ));
		_extensions.shaderFloatControls               = (GetDeviceVersion() >= DeviceVersion{1,2}) or (HasDeviceExtension( VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME ));
		_extensions.spirv14                           = (GetDeviceVersion() >= DeviceVersion{1,2}) or (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_SPIRV_1_4_EXTENSION_NAME ));
		_extensions.imageFormatList                   = (GetDeviceVersion() >= DeviceVersion{1,2}) or (HasDeviceExtension( VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME ));
		_extensions.driverProperties                  = (GetDeviceVersion() >= DeviceVersion{1,2}) or (HasDeviceExtension( VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME ));
		_extensions.storage8bits                      = (HasDeviceExtension( VK_KHR_8BIT_STORAGE_EXTENSION_NAME ));
		_extensions.shaderAtomicInt64                 = (HasDeviceExtension( VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME ));
		_extensions.shaderFloat16Int8                 = (HasDeviceExtension( VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME ));
		_extensions.samplerMirrorClamp                = (HasDeviceExtension( VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME ));
		_extensions.drawIndirectCount                 = (HasDeviceExtension( VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME ));
		_extensions.descriptorIndexing                = (HasDeviceExtension( VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME ));
		_extensions.samplerFilterMinmax               = (HasDeviceExtension( VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME ));
		_extensions.scalarBlockLayout                 = (HasDeviceExtension( VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME ));
		_extensions.subgroupExtendedTypes             = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME ));
		_extensions.hostQueryReset                    = (HasDeviceExtension( VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME ));
		_extensions.timelineSemaphore                 = (HasDeviceExtension( VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME ));
		_extensions.bufferDeviceAddress               = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME ));
		_extensions.memoryModel                       = (HasDeviceExtension( VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME ));
		_extensions.uniformBufferStandardLayout       = (HasDeviceExtension( VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME ));
		_extensions.shaderViewportIndexLayer          = (HasDeviceExtension( VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME ));
		_extensions.formatFeat2                       = (GetDeviceVersion() >= DeviceVersion{1,3}) or (HasDeviceExtension( VK_KHR_FORMAT_FEATURE_FLAGS_2_EXTENSION_NAME ));
		_extensions.texelBufferAlignment              = (GetDeviceVersion() >= DeviceVersion{1,3}) or (HasDeviceExtension( VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME ));
		_extensions.maintenance4                      = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_MAINTENANCE_4_EXTENSION_NAME ));
		_extensions.synchronization2                  = (HasDeviceExtension( VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME ));
		_extensions.zeroInitializeWorkgroupMem        = (HasDeviceExtension( VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME ));
		_extensions.inlineUniformBlock                = (HasDeviceExtension( VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME ));
		_extensions.subgroupSizeControl               = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME ));
		_extensions.astcHdr                           = (HasDeviceExtension( VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME ));
		_extensions.shaderTerminateInvocation         = (HasDeviceExtension( VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME ));
		_extensions.ycbcr2Plane444                    = (HasDeviceExtension( VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME ));
		_extensions.shaderDemoteToHelperInvocation    = (HasDeviceExtension( VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME ));
		_extensions.swapchain                         = (HasDeviceExtension( VK_KHR_SWAPCHAIN_EXTENSION_NAME ));
		_extensions.displaySwapchain                  = (HasDeviceExtension( VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME ));
		_extensions.depthRangeUnrestricted            = (HasDeviceExtension( VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME ));
		_extensions.memoryPriority                    = (HasDeviceExtension( VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME ));
		_extensions.vertexDivisor                     = (HasDeviceExtension( VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME ));
		_extensions.depthClip                         = (HasDeviceExtension( VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME ));
		_extensions.portabilitySubset                 = (HasDeviceExtension( VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME ));
		_extensions.loadOpNone                        = (HasDeviceExtension( VK_EXT_LOAD_STORE_OP_NONE_EXTENSION_NAME ));
		_extensions.pagebleDeviceLocalMemory          = (HasDeviceExtension( VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME ));
		_extensions.sampleLocations                   = (HasDeviceExtension( VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME ));
		_extensions.fragmentBarycentric               = (HasDeviceExtension( VK_KHR_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME ));
		_extensions.meshShader                        = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_EXT_MESH_SHADER_EXTENSION_NAME ));
		_extensions.rasterOrderAttachment             = (HasDeviceExtension( VK_EXT_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_EXTENSION_NAME ));
		_extensions.cooperativeMatrix                 = (HasDeviceExtension( VK_KHR_COOPERATIVE_MATRIX_EXTENSION_NAME ));
		_extensions.nonSeamlessCubeMap                = (HasDeviceExtension( VK_EXT_NON_SEAMLESS_CUBE_MAP_EXTENSION_NAME ));
		_extensions.fragShaderInterlock               = (HasDeviceExtension( VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME ));
		_extensions.shaderClock                       = (HasDeviceExtension( VK_KHR_SHADER_CLOCK_EXTENSION_NAME ));
		_extensions.shaderAtomicFloat                 = (HasDeviceExtension( VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME ));
		_extensions.shaderAtomicFloat2                = (HasDeviceExtension( VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME ));
		_extensions.shaderSubgroupUniformControlFlow  = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME ));
		_extensions.workgroupMemExplicitLayout        = (HasDeviceExtension( VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME ));
		_extensions.shaderImageAtomicInt64            = (HasDeviceExtension( VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME ));
		_extensions.pplnExecProps                     = (HasDeviceExtension( VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME ));
		_extensions.queueGlobalPriority               = (HasDeviceExtension( VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME ));
		_extensions.externalMemory                    = (HasDeviceExtension( VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME ));
		_extensions.externalSemaphore                 = (HasDeviceExtension( VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME ));
		_extensions.externalSemaphoreFd               = (HasDeviceExtension( VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME ));
		_extensions.queueFamilyForeign                = (HasDeviceExtension( VK_EXT_QUEUE_FAMILY_FOREIGN_EXTENSION_NAME ));
		#if defined(VK_ANDROID_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER_EXTENSION_NAME)
		_extensions.androidExternalMemoryHwBuf        = (HasDeviceExtension( VK_ANDROID_EXTERNAL_MEMORY_ANDROID_HARDWARE_BUFFER_EXTENSION_NAME ));
		#endif
		_extensions.performanceQuery                  = (HasDeviceExtension( VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME ));
		_extensions.calibratedTimestamps              = (HasDeviceExtension( VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME ));
		_extensions.toolingInfo                       = (HasDeviceExtension( VK_EXT_TOOLING_INFO_EXTENSION_NAME ));
		_extensions.memoryBudget                      = (HasDeviceExtension( VK_EXT_MEMORY_BUDGET_EXTENSION_NAME ));
		_extensions.memoryReport                      = (HasDeviceExtension( VK_EXT_DEVICE_MEMORY_REPORT_EXTENSION_NAME ));
		_extensions.fragShadingRate                   = (HasDeviceExtension( VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME ));
		_extensions.pipelineLibrary                   = (HasDeviceExtension( VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME ));
		_extensions.deferredHostOps                   = (HasDeviceExtension( VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME ));
		_extensions.accelerationStructure             = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME ));
		_extensions.rayTracingPipeline                = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME ));
		_extensions.rayQuery                          = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_RAY_QUERY_EXTENSION_NAME ));
		_extensions.rayTracingMaintenance1            = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME ));
		_extensions.rayTracingPositionFetch           = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_RAY_TRACING_POSITION_FETCH_EXTENSION_NAME ));
		_extensions.astcDecodeMode                    = (HasDeviceExtension( VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME ));
		_extensions.imageCompressionCtrl              = (HasDeviceExtension( VK_EXT_IMAGE_COMPRESSION_CONTROL_EXTENSION_NAME ));
		_extensions.swapchainCompressionCtrl          = (HasDeviceExtension( VK_EXT_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_EXTENSION_NAME ));
		_extensions.videoQueue                        = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_VIDEO_QUEUE_EXTENSION_NAME ));
		_extensions.videoEncodeQueue                  = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_VIDEO_ENCODE_QUEUE_EXTENSION_NAME ));
		_extensions.videoDecodeQueue                  = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME ));
		_extensions.videoDecodeH264                   = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME ));
		_extensions.videoDecodeH265                   = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_VIDEO_DECODE_H265_EXTENSION_NAME ));
		_extensions.videoEncodeH264                   = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_VIDEO_ENCODE_H264_EXTENSION_NAME ));
		_extensions.videoEncodeH265                   = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_KHR_VIDEO_ENCODE_H265_EXTENSION_NAME ));
		_extensions.imageFootprintNV                  = (HasDeviceExtension( VK_NV_SHADER_IMAGE_FOOTPRINT_EXTENSION_NAME ));
		_extensions.deviceGeneratedCmdsNV             = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME ));
		_extensions.shaderSMBuiltinsNV                = (GetDeviceVersion() >= DeviceVersion{1,1} and HasDeviceExtension( VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME ));
		_extensions.shaderCorePropsAMD                = (HasDeviceExtension( VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME ));
		_extensions.shaderCorePropsAMD2               = (HasDeviceExtension( VK_AMD_SHADER_CORE_PROPERTIES_2_EXTENSION_NAME ));
		_extensions.shaderCoreBuiltinsARM             = (HasDeviceExtension( VK_ARM_SHADER_CORE_BUILTINS_EXTENSION_NAME ));
		_extensions.subpassShadingHW                  = (HasDeviceExtension( VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME ));
		_extensions.incrementalPresent                = (HasDeviceExtension( VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME ));
		_extensions.presentId                         = (HasDeviceExtension( VK_KHR_PRESENT_ID_EXTENSION_NAME ));
		_extensions.presentWait                       = (HasDeviceExtension( VK_KHR_PRESENT_WAIT_EXTENSION_NAME ));
	}

	void  VDeviceInitializer::_InitFeaturesAndProperties (void** nextFeat)
	{
		vkGetPhysicalDeviceFeatures( GetVkPhysicalDevice(), OUT &_properties.features );
		vkGetPhysicalDeviceProperties( GetVkPhysicalDevice(), OUT &_properties.properties );
		vkGetPhysicalDeviceMemoryProperties( GetVkPhysicalDevice(), OUT &_properties.memoryProperties );

		if ( GetInstanceVersion() >= InstanceVersion{1,1} or HasInstanceExtension( VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME ))
		{
			VkPhysicalDeviceFeatures2         feat2      = {};
			VkPhysicalDeviceProperties2       props2     = {};
			VkPhysicalDeviceMemoryProperties2 mem_props2 = {};

			void **  next_feat  = &feat2.pNext;
			void **  next_props = &props2.pNext;
			void **  next_mem   = &mem_props2.pNext;

			feat2.sType      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			props2.sType     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			mem_props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;

			if ( _extensions.subgroup )
			{
				*next_props = &_properties.subgroupProperties;
				next_props  = &_properties.subgroupProperties.pNext;
				_properties.subgroupProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES;
			}
			if ( _extensions.maintenance3 )
			{
				*next_props = &_properties.maintenance3Props;
				next_props  = &_properties.maintenance3Props.pNext;
				_properties.maintenance3Props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR;
			}
			if ( _extensions.multiview )
			{
				*next_feat = &_properties.multiviewFeats;
				next_feat  = &_properties.multiviewFeats.pNext;
				_properties.multiviewFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;
				*next_props = &_properties.multiviewProps;
				next_props  = &_properties.multiviewProps.pNext;
				_properties.multiviewProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;
			}
			if ( _extensions.variablePointers )
			{
				*next_feat = &_properties.variablePointersFeats;
				next_feat  = &_properties.variablePointersFeats.pNext;
				_properties.variablePointersFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR;
			}
			if ( _extensions.samplerYcbcrConversion )
			{
				*next_feat = &_properties.samplerYcbcrConversionFeats;
				next_feat  = &_properties.samplerYcbcrConversionFeats.pNext;
				_properties.samplerYcbcrConversionFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR;
			}
			if ( _extensions.storage16bit )
			{
				*next_feat = &_properties.storage16bitFeats;
				next_feat  = &_properties.storage16bitFeats.pNext;
				_properties.storage16bitFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR;
			}
			if ( _extensions.driverProperties )
			{
				*next_props = &_properties.driverPropertiesProps;
				next_props  = &_properties.driverPropertiesProps.pNext;
				_properties.driverPropertiesProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR;
			}
			if ( _extensions.storage8bits )
			{
				*next_feat = &_properties.storage8bitsFeats;
				next_feat  = &_properties.storage8bitsFeats.pNext;
				_properties.storage8bitsFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR;
			}
			if ( _extensions.shaderAtomicInt64 )
			{
				*next_feat = &_properties.shaderAtomicInt64Feats;
				next_feat  = &_properties.shaderAtomicInt64Feats.pNext;
				_properties.shaderAtomicInt64Feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR;
			}
			if ( _extensions.shaderFloat16Int8 )
			{
				*next_feat = &_properties.shaderFloat16Int8Feats;
				next_feat  = &_properties.shaderFloat16Int8Feats.pNext;
				_properties.shaderFloat16Int8Feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;
			}
			if ( _extensions.descriptorIndexing )
			{
				*next_feat = &_properties.descriptorIndexingFeats;
				next_feat  = &_properties.descriptorIndexingFeats.pNext;
				_properties.descriptorIndexingFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
				*next_props = &_properties.descriptorIndexingProps;
				next_props  = &_properties.descriptorIndexingProps.pNext;
				_properties.descriptorIndexingProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT;
			}
			if ( _extensions.samplerFilterMinmax )
			{
				*next_props = &_properties.samplerFilterMinmaxProps;
				next_props  = &_properties.samplerFilterMinmaxProps.pNext;
				_properties.samplerFilterMinmaxProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT;
			}
			if ( _extensions.scalarBlockLayout )
			{
				*next_feat = &_properties.scalarBlockLayoutFeats;
				next_feat  = &_properties.scalarBlockLayoutFeats.pNext;
				_properties.scalarBlockLayoutFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT;
			}
			if ( _extensions.subgroupExtendedTypes )
			{
				*next_feat = &_properties.subgroupExtendedTypesFeats;
				next_feat  = &_properties.subgroupExtendedTypesFeats.pNext;
				_properties.subgroupExtendedTypesFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR;
			}
			if ( _extensions.hostQueryReset )
			{
				*next_feat = &_properties.hostQueryResetFeats;
				next_feat  = &_properties.hostQueryResetFeats.pNext;
				_properties.hostQueryResetFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT;
			}
			if ( _extensions.timelineSemaphore )
			{
				*next_feat = &_properties.timelineSemaphoreFeats;
				next_feat  = &_properties.timelineSemaphoreFeats.pNext;
				_properties.timelineSemaphoreFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;
				*next_props = &_properties.timelineSemaphoreProps;
				next_props  = &_properties.timelineSemaphoreProps.pNext;
				_properties.timelineSemaphoreProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR;
			}
			if ( _extensions.bufferDeviceAddress )
			{
				*next_feat = &_properties.bufferDeviceAddressFeats;
				next_feat  = &_properties.bufferDeviceAddressFeats.pNext;
				_properties.bufferDeviceAddressFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;
			}
			if ( _extensions.memoryModel )
			{
				*next_feat = &_properties.memoryModelFeats;
				next_feat  = &_properties.memoryModelFeats.pNext;
				_properties.memoryModelFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR;
			}
			if ( _extensions.uniformBufferStandardLayout )
			{
				*next_feat = &_properties.uniformBufferStandardLayoutFeats;
				next_feat  = &_properties.uniformBufferStandardLayoutFeats.pNext;
				_properties.uniformBufferStandardLayoutFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR;
			}
			if ( _extensions.texelBufferAlignment )
			{
				*next_feat = &_properties.texelBufferAlignmentFeats;
				next_feat  = &_properties.texelBufferAlignmentFeats.pNext;
				_properties.texelBufferAlignmentFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT;
				*next_props = &_properties.texelBufferAlignmentProps;
				next_props  = &_properties.texelBufferAlignmentProps.pNext;
				_properties.texelBufferAlignmentProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT;
			}
			if ( _extensions.maintenance4 )
			{
				*next_feat = &_properties.maintenance4Feats;
				next_feat  = &_properties.maintenance4Feats.pNext;
				_properties.maintenance4Feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES_KHR;
				*next_props = &_properties.maintenance4Props;
				next_props  = &_properties.maintenance4Props.pNext;
				_properties.maintenance4Props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES_KHR;
			}
			if ( _extensions.synchronization2 )
			{
				*next_feat = &_properties.synchronization2Feats;
				next_feat  = &_properties.synchronization2Feats.pNext;
				_properties.synchronization2Feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
			}
			if ( _extensions.zeroInitializeWorkgroupMem )
			{
				*next_feat = &_properties.zeroInitializeWorkgroupMemFeats;
				next_feat  = &_properties.zeroInitializeWorkgroupMemFeats.pNext;
				_properties.zeroInitializeWorkgroupMemFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR;
			}
			if ( _extensions.inlineUniformBlock )
			{
				*next_feat = &_properties.inlineUniformBlockFeats;
				next_feat  = &_properties.inlineUniformBlockFeats.pNext;
				_properties.inlineUniformBlockFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
				*next_props = &_properties.inlineUniformBlockProps;
				next_props  = &_properties.inlineUniformBlockProps.pNext;
				_properties.inlineUniformBlockProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT;
			}
			if ( _extensions.subgroupSizeControl )
			{
				*next_feat = &_properties.subgroupSizeControlFeats;
				next_feat  = &_properties.subgroupSizeControlFeats.pNext;
				_properties.subgroupSizeControlFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT;
				*next_props = &_properties.subgroupSizeControlProps;
				next_props  = &_properties.subgroupSizeControlProps.pNext;
				_properties.subgroupSizeControlProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT;
			}
			if ( _extensions.astcHdr )
			{
				*next_feat = &_properties.astcHdrFeats;
				next_feat  = &_properties.astcHdrFeats.pNext;
				_properties.astcHdrFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT;
			}
			if ( _extensions.shaderTerminateInvocation )
			{
				*next_feat = &_properties.shaderTerminateInvocationFeats;
				next_feat  = &_properties.shaderTerminateInvocationFeats.pNext;
				_properties.shaderTerminateInvocationFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR;
			}
			if ( _extensions.ycbcr2Plane444 )
			{
				*next_feat = &_properties.ycbcr2Plane444Feats;
				next_feat  = &_properties.ycbcr2Plane444Feats.pNext;
				_properties.ycbcr2Plane444Feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT;
			}
			if ( _extensions.shaderDemoteToHelperInvocation )
			{
				*next_feat = &_properties.shaderDemoteToHelperInvocationFeats;
				next_feat  = &_properties.shaderDemoteToHelperInvocationFeats.pNext;
				_properties.shaderDemoteToHelperInvocationFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT;
			}
			if ( _extensions.memoryPriority )
			{
				*next_feat = &_properties.memoryPriorityFeats;
				next_feat  = &_properties.memoryPriorityFeats.pNext;
				_properties.memoryPriorityFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT;
			}
			if ( _extensions.vertexDivisor )
			{
				*next_feat = &_properties.vertexDivisorFeats;
				next_feat  = &_properties.vertexDivisorFeats.pNext;
				_properties.vertexDivisorFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT;
				*next_props = &_properties.vertexDivisorProps;
				next_props  = &_properties.vertexDivisorProps.pNext;
				_properties.vertexDivisorProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT;
			}
			if ( _extensions.depthClip )
			{
				*next_feat = &_properties.depthClipFeats;
				next_feat  = &_properties.depthClipFeats.pNext;
				_properties.depthClipFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT;
			}
			if ( _extensions.portabilitySubset )
			{
				*next_feat = &_properties.portabilitySubsetFeats;
				next_feat  = &_properties.portabilitySubsetFeats.pNext;
				_properties.portabilitySubsetFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR;
				*next_props = &_properties.portabilitySubsetProps;
				next_props  = &_properties.portabilitySubsetProps.pNext;
				_properties.portabilitySubsetProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR;
			}
			if ( _extensions.pagebleDeviceLocalMemory )
			{
				*next_feat = &_properties.pagebleDeviceLocalMemoryFeats;
				next_feat  = &_properties.pagebleDeviceLocalMemoryFeats.pNext;
				_properties.pagebleDeviceLocalMemoryFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT;
			}
			if ( _extensions.sampleLocations )
			{
				*next_props = &_properties.sampleLocationsProps;
				next_props  = &_properties.sampleLocationsProps.pNext;
				_properties.sampleLocationsProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT;
			}
			if ( _extensions.fragmentBarycentric )
			{
				*next_feat = &_properties.fragmentBarycentricFeats;
				next_feat  = &_properties.fragmentBarycentricFeats.pNext;
				_properties.fragmentBarycentricFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_KHR;
				*next_props = &_properties.fragmentBarycentricProps;
				next_props  = &_properties.fragmentBarycentricProps.pNext;
				_properties.fragmentBarycentricProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_PROPERTIES_KHR;
			}
			if ( _extensions.meshShader )
			{
				*next_feat = &_properties.meshShaderFeats;
				next_feat  = &_properties.meshShaderFeats.pNext;
				_properties.meshShaderFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
				*next_props = &_properties.meshShaderProps;
				next_props  = &_properties.meshShaderProps.pNext;
				_properties.meshShaderProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;
			}
			if ( _extensions.cooperativeMatrix )
			{
				*next_feat = &_properties.cooperativeMatrixFeats;
				next_feat  = &_properties.cooperativeMatrixFeats.pNext;
				_properties.cooperativeMatrixFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_KHR;
				*next_props = &_properties.cooperativeMatrixProps;
				next_props  = &_properties.cooperativeMatrixProps.pNext;
				_properties.cooperativeMatrixProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_KHR;
			}
			if ( _extensions.nonSeamlessCubeMap )
			{
				*next_feat = &_properties.nonSeamlessCubeMapFeats;
				next_feat  = &_properties.nonSeamlessCubeMapFeats.pNext;
				_properties.nonSeamlessCubeMapFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NON_SEAMLESS_CUBE_MAP_FEATURES_EXT;
			}
			if ( _extensions.fragShaderInterlock )
			{
				*next_feat = &_properties.fragShaderInterlockFeats;
				next_feat  = &_properties.fragShaderInterlockFeats.pNext;
				_properties.fragShaderInterlockFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT;
			}
			if ( _extensions.shaderClock )
			{
				*next_feat = &_properties.shaderClockFeats;
				next_feat  = &_properties.shaderClockFeats.pNext;
				_properties.shaderClockFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR;
			}
			if ( _extensions.shaderAtomicFloat )
			{
				*next_feat = &_properties.shaderAtomicFloatFeats;
				next_feat  = &_properties.shaderAtomicFloatFeats.pNext;
				_properties.shaderAtomicFloatFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
			}
			if ( _extensions.shaderAtomicFloat2 )
			{
				*next_feat = &_properties.shaderAtomicFloat2Feats;
				next_feat  = &_properties.shaderAtomicFloat2Feats.pNext;
				_properties.shaderAtomicFloat2Feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT;
			}
			if ( _extensions.shaderSubgroupUniformControlFlow )
			{
				*next_feat = &_properties.shaderSubgroupUniformControlFlowFeats;
				next_feat  = &_properties.shaderSubgroupUniformControlFlowFeats.pNext;
				_properties.shaderSubgroupUniformControlFlowFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR;
			}
			if ( _extensions.workgroupMemExplicitLayout )
			{
				*next_feat = &_properties.workgroupMemExplicitLayoutFeats;
				next_feat  = &_properties.workgroupMemExplicitLayoutFeats.pNext;
				_properties.workgroupMemExplicitLayoutFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR;
			}
			if ( _extensions.shaderImageAtomicInt64 )
			{
				*next_feat = &_properties.shaderImageAtomicInt64Feats;
				next_feat  = &_properties.shaderImageAtomicInt64Feats.pNext;
				_properties.shaderImageAtomicInt64Feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT;
			}
			if ( _extensions.pplnExecProps )
			{
				*next_feat = &_properties.pplnExecPropsFeats;
				next_feat  = &_properties.pplnExecPropsFeats.pNext;
				_properties.pplnExecPropsFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR;
			}
			if ( _extensions.performanceQuery )
			{
				*next_feat = &_properties.performanceQueryFeats;
				next_feat  = &_properties.performanceQueryFeats.pNext;
				_properties.performanceQueryFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR;
				*next_props = &_properties.performanceQueryProps;
				next_props  = &_properties.performanceQueryProps.pNext;
				_properties.performanceQueryProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR;
			}
			if ( _extensions.memoryBudget )
			{
				*next_mem = &_properties.memoryBudgetProps;
				next_mem  = &_properties.memoryBudgetProps.pNext;
				_properties.memoryBudgetProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
			}
			if ( _extensions.memoryReport )
			{
				*next_feat = &_properties.memoryReportFeats;
				next_feat  = &_properties.memoryReportFeats.pNext;
				_properties.memoryReportFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT;
			}
			if ( _extensions.fragShadingRate )
			{
				*next_feat = &_properties.fragShadingRateFeats;
				next_feat  = &_properties.fragShadingRateFeats.pNext;
				_properties.fragShadingRateFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
				*next_props = &_properties.fragShadingRateProps;
				next_props  = &_properties.fragShadingRateProps.pNext;
				_properties.fragShadingRateProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
			}
			if ( _extensions.accelerationStructure )
			{
				*next_feat = &_properties.accelerationStructureFeats;
				next_feat  = &_properties.accelerationStructureFeats.pNext;
				_properties.accelerationStructureFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
				*next_props = &_properties.accelerationStructureProps;
				next_props  = &_properties.accelerationStructureProps.pNext;
				_properties.accelerationStructureProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
			}
			if ( _extensions.rayTracingPipeline )
			{
				*next_feat = &_properties.rayTracingPipelineFeats;
				next_feat  = &_properties.rayTracingPipelineFeats.pNext;
				_properties.rayTracingPipelineFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
				*next_props = &_properties.rayTracingPipelineProps;
				next_props  = &_properties.rayTracingPipelineProps.pNext;
				_properties.rayTracingPipelineProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
			}
			if ( _extensions.rayQuery )
			{
				*next_feat = &_properties.rayQueryFeats;
				next_feat  = &_properties.rayQueryFeats.pNext;
				_properties.rayQueryFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
			}
			if ( _extensions.rayTracingMaintenance1 )
			{
				*next_feat = &_properties.rayTracingMaintenance1Feats;
				next_feat  = &_properties.rayTracingMaintenance1Feats.pNext;
				_properties.rayTracingMaintenance1Feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MAINTENANCE_1_FEATURES_KHR;
			}
			if ( _extensions.rayTracingPositionFetch )
			{
				*next_feat = &_properties.rayTracingPositionFetchFeats;
				next_feat  = &_properties.rayTracingPositionFetchFeats.pNext;
				_properties.rayTracingPositionFetchFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_POSITION_FETCH_FEATURES_KHR;
			}
			if ( _extensions.imageCompressionCtrl )
			{
				*next_feat = &_properties.imageCompressionCtrlFeats;
				next_feat  = &_properties.imageCompressionCtrlFeats.pNext;
				_properties.imageCompressionCtrlFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_FEATURES_EXT;
			}
			if ( _extensions.swapchainCompressionCtrl )
			{
				*next_feat = &_properties.swapchainCompressionCtrlFeats;
				next_feat  = &_properties.swapchainCompressionCtrlFeats.pNext;
				_properties.swapchainCompressionCtrlFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_FEATURES_EXT;
			}
			if ( _extensions.imageFootprintNV )
			{
				*next_feat = &_properties.imageFootprintNVFeats;
				next_feat  = &_properties.imageFootprintNVFeats.pNext;
				_properties.imageFootprintNVFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV;
			}
			if ( _extensions.deviceGeneratedCmdsNV )
			{
				*next_feat = &_properties.deviceGeneratedCmdsNVFeats;
				next_feat  = &_properties.deviceGeneratedCmdsNVFeats.pNext;
				_properties.deviceGeneratedCmdsNVFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV;
				*next_props = &_properties.deviceGeneratedCmdsNVProps;
				next_props  = &_properties.deviceGeneratedCmdsNVProps.pNext;
				_properties.deviceGeneratedCmdsNVProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV;
			}
			if ( _extensions.shaderSMBuiltinsNV )
			{
				*next_feat = &_properties.shaderSMBuiltinsNVFeats;
				next_feat  = &_properties.shaderSMBuiltinsNVFeats.pNext;
				_properties.shaderSMBuiltinsNVFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV;
				*next_props = &_properties.shaderSMBuiltinsNVProps;
				next_props  = &_properties.shaderSMBuiltinsNVProps.pNext;
				_properties.shaderSMBuiltinsNVProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV;
			}
			if ( _extensions.shaderCorePropsAMD )
			{
				*next_props = &_properties.shaderCorePropsAMDProps;
				next_props  = &_properties.shaderCorePropsAMDProps.pNext;
				_properties.shaderCorePropsAMDProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD;
			}
			if ( _extensions.shaderCoreBuiltinsARM )
			{
				*next_feat = &_properties.shaderCoreBuiltinsARMFeats;
				next_feat  = &_properties.shaderCoreBuiltinsARMFeats.pNext;
				_properties.shaderCoreBuiltinsARMFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_BUILTINS_FEATURES_ARM;
				*next_props = &_properties.shaderCoreBuiltinsARMProps;
				next_props  = &_properties.shaderCoreBuiltinsARMProps.pNext;
				_properties.shaderCoreBuiltinsARMProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_BUILTINS_PROPERTIES_ARM;
			}
			if ( _extensions.subpassShadingHW )
			{
				*next_feat = &_properties.subpassShadingHWFeats;
				next_feat  = &_properties.subpassShadingHWFeats.pNext;
				_properties.subpassShadingHWFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI;
				*next_props = &_properties.subpassShadingHWProps;
				next_props  = &_properties.subpassShadingHWProps.pNext;
				_properties.subpassShadingHWProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_PROPERTIES_HUAWEI;
			}
			if ( _extensions.presentId )
			{
				*next_feat = &_properties.presentIdFeats;
				next_feat  = &_properties.presentIdFeats.pNext;
				_properties.presentIdFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR;
			}
			if ( _extensions.presentWait )
			{
				*next_feat = &_properties.presentWaitFeats;
				next_feat  = &_properties.presentWaitFeats.pNext;
				_properties.presentWaitFeats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR;
			}

			*next_feat  = null;
			*next_props = null;
			*next_mem   = null;

			vkGetPhysicalDeviceFeatures2KHR( GetVkPhysicalDevice(), OUT &feat2 );
			vkGetPhysicalDeviceProperties2KHR( GetVkPhysicalDevice(), OUT &props2 );
			vkGetPhysicalDeviceMemoryProperties2KHR( GetVkPhysicalDevice(), OUT &mem_props2 );
			*nextFeat = feat2.pNext;
		}else{
			*nextFeat = null;
		}
	}

	String  VDeviceInitializer::_GetVulkanExtensionsString () const
	{
		String src;
		src << "\n  ---- instance ----"
			<< "\n  surface:. . . . . . . . . . . . . " << ToString( _extensions.surface )
			<< "\n  surfaceCaps2:                     " << ToString( _extensions.surfaceCaps2 )
			<< "\n  swapchainColorspace:. . . . . . . " << ToString( _extensions.swapchainColorspace )
			<< "\n  display:                          " << ToString( _extensions.display )
			<< "\n  directModeDisplay:. . . . . . . . " << ToString( _extensions.directModeDisplay )
			<< "\n  displayProps2:                    " << ToString( _extensions.displayProps2 )
			<< "\n  debugReport:. . . . . . . . . . . " << ToString( _extensions.debugReport )
			<< "\n  debugMarker:                      " << ToString( _extensions.debugMarker )
			<< "\n  debugUtils: . . . . . . . . . . . " << ToString( _extensions.debugUtils )
			<< "\n  validationFlags:                  " << ToString( _extensions.validationFlags )
			<< "\n  deviceGroupCreation:. . . . . . . " << ToString( _extensions.deviceGroupCreation )
			<< "\n  deviceProps2:                     " << ToString( _extensions.deviceProps2 )
			<< "\n  externalMemoryCaps: . . . . . . . " << ToString( _extensions.externalMemoryCaps )
			<< "\n  externalSemaphoreCaps:            " << ToString( _extensions.externalSemaphoreCaps )
			<< "\n  portabilityEnum:. . . . . . . . . " << ToString( _extensions.portabilityEnum )
			<< "\n  ---- 1.1 ----"
			<< "\n  memRequirements2:                 " << ToString( _extensions.memRequirements2 )
			<< "\n  bindMemory2:. . . . . . . . . . . " << ToString( _extensions.bindMemory2 )
			<< "\n  dedicatedAllocation:              " << ToString( _extensions.dedicatedAllocation )
			<< "\n  descriptorUpdateTemplate: . . . . " << ToString( _extensions.descriptorUpdateTemplate )
			<< "\n  maintenance1:                     " << ToString( _extensions.maintenance1 )
			<< "\n  maintenance2: . . . . . . . . . . " << ToString( _extensions.maintenance2 )
			<< "\n  maintenance3:                     " << ToString( _extensions.maintenance3 )
			<< "\n  deviceGroup:. . . . . . . . . . . " << ToString( _extensions.deviceGroup )
			<< "\n  storageBufferClass:               " << ToString( _extensions.storageBufferClass )
			<< "\n  subgroup: . . . . . . . . . . . . " << ToString( _extensions.subgroup )
			<< "\n  relaxedBlockLayout:               " << ToString( _extensions.relaxedBlockLayout )
			<< "\n  ---- ext ----"
			<< "\n  multiview:. . . . . . . . . . . . " << ToString( _extensions.multiview )
			<< "\n  variablePointers:                 " << ToString( _extensions.variablePointers )
			<< "\n  samplerYcbcrConversion: . . . . . " << ToString( _extensions.samplerYcbcrConversion )
			<< "\n  shaderDrawParams:                 " << ToString( _extensions.shaderDrawParams )
			<< "\n  storage16bit: . . . . . . . . . . " << ToString( _extensions.storage16bit )
			<< "\n  ---- 1.2 ----"
			<< "\n  renderPass2:                      " << ToString( _extensions.renderPass2 )
			<< "\n  shaderFloatControls:. . . . . . . " << ToString( _extensions.shaderFloatControls )
			<< "\n  spirv14:                          " << ToString( _extensions.spirv14 )
			<< "\n  imageFormatList:. . . . . . . . . " << ToString( _extensions.imageFormatList )
			<< "\n  driverProperties:                 " << ToString( _extensions.driverProperties )
			<< "\n  ---- ext ----"
			<< "\n  storage8bits: . . . . . . . . . . " << ToString( _extensions.storage8bits )
			<< "\n  shaderAtomicInt64:                " << ToString( _extensions.shaderAtomicInt64 )
			<< "\n  shaderFloat16Int8:. . . . . . . . " << ToString( _extensions.shaderFloat16Int8 )
			<< "\n  samplerMirrorClamp:               " << ToString( _extensions.samplerMirrorClamp )
			<< "\n  drawIndirectCount:. . . . . . . . " << ToString( _extensions.drawIndirectCount )
			<< "\n  descriptorIndexing:               " << ToString( _extensions.descriptorIndexing )
			<< "\n  samplerFilterMinmax:. . . . . . . " << ToString( _extensions.samplerFilterMinmax )
			<< "\n  scalarBlockLayout:                " << ToString( _extensions.scalarBlockLayout )
			<< "\n  subgroupExtendedTypes:. . . . . . " << ToString( _extensions.subgroupExtendedTypes )
			<< "\n  hostQueryReset:                   " << ToString( _extensions.hostQueryReset )
			<< "\n  timelineSemaphore:. . . . . . . . " << ToString( _extensions.timelineSemaphore )
			<< "\n  bufferDeviceAddress:              " << ToString( _extensions.bufferDeviceAddress )
			<< "\n  memoryModel:. . . . . . . . . . . " << ToString( _extensions.memoryModel )
			<< "\n  uniformBufferStandardLayout:      " << ToString( _extensions.uniformBufferStandardLayout )
			<< "\n  shaderViewportIndexLayer: . . . . " << ToString( _extensions.shaderViewportIndexLayer )
			<< "\n  ---- 1.3 ----"
			<< "\n  formatFeat2:                      " << ToString( _extensions.formatFeat2 )
			<< "\n  texelBufferAlignment: . . . . . . " << ToString( _extensions.texelBufferAlignment )
			<< "\n  ---- ext ----"
			<< "\n  maintenance4:                     " << ToString( _extensions.maintenance4 )
			<< "\n  synchronization2: . . . . . . . . " << ToString( _extensions.synchronization2 )
			<< "\n  zeroInitializeWorkgroupMem:       " << ToString( _extensions.zeroInitializeWorkgroupMem )
			<< "\n  inlineUniformBlock: . . . . . . . " << ToString( _extensions.inlineUniformBlock )
			<< "\n  subgroupSizeControl:              " << ToString( _extensions.subgroupSizeControl )
			<< "\n  astcHdr:. . . . . . . . . . . . . " << ToString( _extensions.astcHdr )
			<< "\n  shaderTerminateInvocation:        " << ToString( _extensions.shaderTerminateInvocation )
			<< "\n  ycbcr2Plane444: . . . . . . . . . " << ToString( _extensions.ycbcr2Plane444 )
			<< "\n  shaderDemoteToHelperInvocation:   " << ToString( _extensions.shaderDemoteToHelperInvocation )
			<< "\n  swapchain:. . . . . . . . . . . . " << ToString( _extensions.swapchain )
			<< "\n  displaySwapchain:                 " << ToString( _extensions.displaySwapchain )
			<< "\n  depthRangeUnrestricted: . . . . . " << ToString( _extensions.depthRangeUnrestricted )
			<< "\n  memoryPriority:                   " << ToString( _extensions.memoryPriority )
			<< "\n  vertexDivisor:. . . . . . . . . . " << ToString( _extensions.vertexDivisor )
			<< "\n  depthClip:                        " << ToString( _extensions.depthClip )
			<< "\n  portabilitySubset:. . . . . . . . " << ToString( _extensions.portabilitySubset )
			<< "\n  loadOpNone:                       " << ToString( _extensions.loadOpNone )
			<< "\n  pagebleDeviceLocalMemory: . . . . " << ToString( _extensions.pagebleDeviceLocalMemory )
			<< "\n  sampleLocations:                  " << ToString( _extensions.sampleLocations )
			<< "\n  fragmentBarycentric:. . . . . . . " << ToString( _extensions.fragmentBarycentric )
			<< "\n  meshShader:                       " << ToString( _extensions.meshShader )
			<< "\n  rasterOrderAttachment:. . . . . . " << ToString( _extensions.rasterOrderAttachment )
			<< "\n  cooperativeMatrix:                " << ToString( _extensions.cooperativeMatrix )
			<< "\n  nonSeamlessCubeMap: . . . . . . . " << ToString( _extensions.nonSeamlessCubeMap )
			<< "\n  fragShaderInterlock:              " << ToString( _extensions.fragShaderInterlock )
			<< "\n  shaderClock:. . . . . . . . . . . " << ToString( _extensions.shaderClock )
			<< "\n  shaderAtomicFloat:                " << ToString( _extensions.shaderAtomicFloat )
			<< "\n  shaderAtomicFloat2: . . . . . . . " << ToString( _extensions.shaderAtomicFloat2 )
			<< "\n  shaderSubgroupUniformControlFlow: " << ToString( _extensions.shaderSubgroupUniformControlFlow )
			<< "\n  workgroupMemExplicitLayout: . . . " << ToString( _extensions.workgroupMemExplicitLayout )
			<< "\n  shaderImageAtomicInt64:           " << ToString( _extensions.shaderImageAtomicInt64 )
			<< "\n  pplnExecProps:. . . . . . . . . . " << ToString( _extensions.pplnExecProps )
			<< "\n  queueGlobalPriority:              " << ToString( _extensions.queueGlobalPriority )
			<< "\n  externalMemory: . . . . . . . . . " << ToString( _extensions.externalMemory )
			<< "\n  externalSemaphore:                " << ToString( _extensions.externalSemaphore )
			<< "\n  externalSemaphoreFd:. . . . . . . " << ToString( _extensions.externalSemaphoreFd )
			<< "\n  queueFamilyForeign:               " << ToString( _extensions.queueFamilyForeign )
			<< "\n  androidExternalMemoryHwBuf: . . . " << ToString( _extensions.androidExternalMemoryHwBuf )
			<< "\n  performanceQuery:                 " << ToString( _extensions.performanceQuery )
			<< "\n  calibratedTimestamps: . . . . . . " << ToString( _extensions.calibratedTimestamps )
			<< "\n  toolingInfo:                      " << ToString( _extensions.toolingInfo )
			<< "\n  memoryBudget: . . . . . . . . . . " << ToString( _extensions.memoryBudget )
			<< "\n  memoryReport:                     " << ToString( _extensions.memoryReport )
			<< "\n  fragShadingRate:. . . . . . . . . " << ToString( _extensions.fragShadingRate )
			<< "\n  pipelineLibrary:                  " << ToString( _extensions.pipelineLibrary )
			<< "\n  deferredHostOps:. . . . . . . . . " << ToString( _extensions.deferredHostOps )
			<< "\n  accelerationStructure:            " << ToString( _extensions.accelerationStructure )
			<< "\n  rayTracingPipeline: . . . . . . . " << ToString( _extensions.rayTracingPipeline )
			<< "\n  rayQuery:                         " << ToString( _extensions.rayQuery )
			<< "\n  rayTracingMaintenance1: . . . . . " << ToString( _extensions.rayTracingMaintenance1 )
			<< "\n  rayTracingPositionFetch:          " << ToString( _extensions.rayTracingPositionFetch )
			<< "\n  astcDecodeMode: . . . . . . . . . " << ToString( _extensions.astcDecodeMode )
			<< "\n  imageCompressionCtrl:             " << ToString( _extensions.imageCompressionCtrl )
			<< "\n  swapchainCompressionCtrl: . . . . " << ToString( _extensions.swapchainCompressionCtrl )
			<< "\n  videoQueue:                       " << ToString( _extensions.videoQueue )
			<< "\n  videoEncodeQueue: . . . . . . . . " << ToString( _extensions.videoEncodeQueue )
			<< "\n  videoDecodeQueue:                 " << ToString( _extensions.videoDecodeQueue )
			<< "\n  videoDecodeH264:. . . . . . . . . " << ToString( _extensions.videoDecodeH264 )
			<< "\n  videoDecodeH265:                  " << ToString( _extensions.videoDecodeH265 )
			<< "\n  videoEncodeH264:. . . . . . . . . " << ToString( _extensions.videoEncodeH264 )
			<< "\n  videoEncodeH265:                  " << ToString( _extensions.videoEncodeH265 )
			<< "\n  imageFootprintNV: . . . . . . . . " << ToString( _extensions.imageFootprintNV )
			<< "\n  deviceGeneratedCmdsNV:            " << ToString( _extensions.deviceGeneratedCmdsNV )
			<< "\n  shaderSMBuiltinsNV: . . . . . . . " << ToString( _extensions.shaderSMBuiltinsNV )
			<< "\n  shaderCorePropsAMD:               " << ToString( _extensions.shaderCorePropsAMD )
			<< "\n  shaderCorePropsAMD2:. . . . . . . " << ToString( _extensions.shaderCorePropsAMD2 )
			<< "\n  shaderCoreBuiltinsARM:            " << ToString( _extensions.shaderCoreBuiltinsARM )
			<< "\n  subpassShadingHW: . . . . . . . . " << ToString( _extensions.subpassShadingHW )
			<< "\n  incrementalPresent:               " << ToString( _extensions.incrementalPresent )
			<< "\n  presentId:. . . . . . . . . . . . " << ToString( _extensions.presentId )
			<< "\n  presentWait:                      " << ToString( _extensions.presentWait );
		return src;
	}
#endif // VKFEATS_FN_IMPL

