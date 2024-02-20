Source folder: [Vulkan backend](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/graphics/Vulkan/)

**Supported extensions:**
- [x] VK_EXT_swapchain_colorspace
- [x] VK_EXT_validation_features
- [x] VK_KHR_descriptor_update_template
- [x] VK_KHR_sampler_ycbcr_conversion
- [x] VK_KHR_draw_indirect_count
- [x] VK_EXT_descriptor_indexing
- [x] VK_KHR_timeline_semaphore
- [x] VK_KHR_buffer_device_address
- [x] VK_EXT_sampler_filter_minmax
- [x] VK_EXT_host_query_reset
- [x] VK_KHR_image_format_list
- [x] VK_KHR_synchronization2
- [x] VK_EXT_load_store_op_none
- [x] VK_EXT_mesh_shader
- [x] VK_KHR_global_priority
- [x] VK_EXT_calibrated_timestamps
- [x] VK_KHR_acceleration_structure
- [x] VK_KHR_ray_tracing_pipeline
- [x] VK_KHR_ray_query
- [x] VK_KHR_ray_tracing_maintenance1
- [x] VK_EXT_vertex_attribute_divisor
- [x] VK_EXT_memory_budget
- [x] VK_KHR_fragment_shading_rate
- [x] VK_KHR_get_surface_capabilities2
- [x] VK_EXT_swapchain_colorspace
- [x] VK_EXT_debug_utils
- [x] VK_EXT_validation_features
- [x] VK_KHR_dedicated_allocation
- [x] VK_KHR_create_renderpass2
- [x] VK_KHR_sampler_mirror_clamp_to_edge
- [x] VK_EXT_texture_compression_astc_hdr
- [x] VK_EXT_texel_buffer_alignment
- [x] VK_KHR_portability_subset
- [x] VK_AMD_shader_core_properties, VK_AMD_shader_core_properties2
- [x] VK_KHR_pipeline_executable_properties

**In progress:**
- [ ] VK_KHR_video_queue
- [ ] VK_KHR_video_encode_queue
- [ ] VK_KHR_video_decode_queue
- [ ] VK_KHR_video_decode_h264
- [ ] VK_KHR_video_decode_h265
- [ ] VK_EXT_video_encode_h264
- [ ] VK_EXT_video_encode_h265
- [ ] VK_KHR_external_memory, VK_KHR_external_memory_capabilities
- [ ] VK_KHR_external_semaphore, VK_KHR_external_semaphore_fd, VK_KHR_external_semaphore_capabilities
- [ ] VK_EXT_queue_family_foreign
- [ ] VK_ANDROID_external_memory_android_hardware_buffer (for NdkCamera)


**Supported shader extensions:**
- [x] VK_KHR_shader_clock, GL_EXT_shader_realtime_clock, GL_ARB_shader_clock
- [x] GL_EXT_control_flow_attributes
- [x] GL_EXT_debug_printf
- [x] GL_EXT_samplerless_texture_functions
- [x] GL_KHR_shader_subgroup_*
- [x] VK_KHR_shader_subgroup_extended_types, GL_EXT_shader_subgroup_extended_types_*
- [x] VK_KHR_8bit_storage, GL_EXT_shader_8bit_storage
- [x] VK_KHR_shader_float16_int8, GL_EXT_shader_explicit_arithmetic_types_*
- [x] VK_KHR_16bit_storage, GL_EXT_shader_16bit_storage
- [x] GL_ARB_gpu_shader_int64
- [x] GL_ARB_gpu_shader_fp64
- [x] GL_EXT_buffer_reference, GL_EXT_buffer_reference2, GL_EXT_buffer_reference_uvec2
- [x] VK_KHR_shader_atomic_int64, GL_EXT_shader_atomic_int64
- [x] GL_EXT_shader_image_int64
- [x] VK_EXT_shader_atomic_float, GL_EXT_shader_atomic_float
- [x] VK_EXT_shader_atomic_float2, GL_EXT_shader_atomic_float2
- [x] GL_EXT_mesh_shader
- [x] GL_EXT_ray_tracing, GL_EXT_ray_flags_primitive_culling
- [x] GL_EXT_ray_query
- [x] VK_EXT_shader_viewport_index_layer, GL_ARB_shader_viewport_layer_array
- [x] GL_ARB_sparse_texture_clamp
- [x] VK_NV_shader_sm_builtins, GL_NV_shader_sm_builtins
- [x] VK_ARM_shader_core_builtins, GL_ARM_shader_core_builtins
- [x] GL_EXT_nonuniform_qualifier
- [x] VK_KHR_vulkan_memory_model, GL_KHR_memory_scope_semantics
- [x] VK_EXT_shader_demote_to_helper_invocation, GL_EXT_demote_to_helper_invocation
- [x] GL_EXT_fragment_shading_rate
- [x] VK_EXT_fragment_shader_interlock, GL_ARB_fragment_shader_interlock
- [x] VK_KHR_shader_draw_parameters, GL_ARB_shader_draw_parameters
- [x] VK_KHR_fragment_shader_barycentric, GL_EXT_fragment_shader_barycentric
- [x] VK_KHR_cooperative_matrix, GL_KHR_cooperative_matrix (deprecated: VK_NV_cooperative_matrix, GL_NV_cooperative_matrix, GL_NV_integer_cooperative_matrix)
- [x] VK_KHR_shader_subgroup_uniform_control_flow, GL_EXT_subgroupuniform_qualifier


**Features:**
- [x] Async compute and transfer queues
- [x] Async queues with concurrent sharing mode
- [x] Async queues with exclusive sharing mode
- [x] Push constants (compatible with Metal)
- [x] Custom allocator for GPU memory
- [x] Shader subgroup
- [x] Secondary command buffers (async draw)
- [x] Immutable samplers

