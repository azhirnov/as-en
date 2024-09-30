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
- [x] VK_EXT_vertex_attribute_divisor, VK_KHR_vertex_attribute_divisor
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
- [x] VK_EXT_device_memory_report
- [x] VK_NV_ray_tracing_validation
- [x] VK_NV_clip_space_w_scaling
- [x] VK_EXT_subgroup_size_control

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

**Pending extensions with high priority:**
- [ ] VK_EXT_pageable_device_local_memory

**Pending extensions:**
- [ ] VK_KHR_multiview
- [ ] VK_KHR_uniform_buffer_standard_layout
- [ ] VK_KHR_format_feature_flags2
- [ ] VK_EXT_inline_uniform_block
- [ ] VK_EXT_depth_range_unrestricted
- [ ] VK_EXT_sample_locations
- [ ] VK_EXT_rasterization_order_attachment_access
- [ ] VK_EXT_astc_decode_mode
- [ ] VK_EXT_image_compression_control, VK_EXT_image_compression_control_swapchain
- [ ] VK_NV_shader_image_footprint
- [ ] VK_NV_device_generated_commands, VK_NV_device_generated_commands_compute
- [ ] VK_ARM_rasterization_order_attachment_access
- [ ] VK_KHR_incremental_present
- [ ] VK_KHR_present_id
- [ ] VK_KHR_present_wait
- [ ] VK_EXT_full_screen_exclusive
- [ ] VK_EXT_host_image_copy
- [ ] VK_KHR_maintenance5
- [ ] VK_NV_memory_decompression
- [ ] VK_EXT_memory_priority
- [ ] VK_NV_external_memory_rdma
- [ ] VK_EXT_direct_mode_display
- [ ] VK_EXT_hdr_metadata
- [ ] VK_EXT_headless_surface ???
- [ ] VK_EXT_metal_objects
- [ ] VK_EXT_multisampled_render_to_single_sampled
- [ ] VK_EXT_attachment_feedback_loop_layout
- [ ] VK_EXT_depth_clamp_zero_one
- [ ] VK_EXT_subpass_merge_feedback
- [ ] VK_NV_representative_fragment_test
- [ ] VK_QCOM_tile_properties
- [ ] VK_EXT_display_control
- [ ] VK_EXT_display_surface_counter
- [ ] VK_EXT_opacity_micromap, GL_EXT_opacity_micromap
- [ ] VK_EXT_pipeline_protected_access
- [ ] VK_NV_present_barrier
- [ ] VK_NV_shader_subgroup_partitioned
- [ ] VK_EXT_descriptor_buffer
- [ ] VK_EXT_swapchain_maintenance1
- [ ] VK_EXT_surface_maintenance1
- [ ] VK_ARM_scheduling_controls
- [ ] VK_KHR_ray_tracing_position_fetch, GL_EXT_ray_tracing_position_fetch
- [ ] VK_EXT_discard_rectangles
- [ ] VK_EXT_image_sliced_view_of_3d
- [ ] VK_EXT_image_2d_view_of_3d
- [ ] VK_EXT_image_view_min_lod
- [ ] VK_EXT_line_rasterization
- [ ] VK_EXT_nested_command_buffer
- [ ] VK_EXT_post_depth_coverage
- [ ] VK_ARM_render_pass_striped
- [ ] VK_NV_dedicated_allocation_image_aliasing
- [ ] VK_NV_extended_sparse_address_space
- [ ] VK_ARM_shader_core_properties
- [ ] VK_KHR_dynamic_rendering_local_read
- [ ] VK_KHR_line_rasterization
- [ ] VK_KHR_maintenance6
- [ ] VK_KHR_shader_expect_assume
- [ ] VK_KHR_shader_float_controls2
- [ ] VK_KHR_shader_subgroup_rotate
- [ ] VK_KHR_video_maintenance1
- [ ] VK_NV_extended_sparse_address_space
- [ ] VK_EXT_shader_tile_image
- [ ] VK_EXT_primitive_topology_list_restart
- [ ] VK_NV_viewport_array2, GL_NV_viewport_array2 - add gl_ViewportMask
- [ ] VK_NV_ray_tracing_invocation_reorder, GL_NV_shader_invocation_reorder
- [ ] VK_NV_displacement_micromap, GL_NV_displacement_micromap
- [ ] VK_KHR_push_descriptor
- [ ] VK_KHR_shader_integer_dot_product - ML
- [ ] VK_KHR_pipeline_binary

**Pending for debugging:**
- [ ] VK_EXT_device_fault
- [ ] VK_EXT_robustness2
- [ ] VK_EXT_device_address_binding_report
- [ ] VK_EXT_frame_boundary
- [ ] VK_EXT_layer_settings
- [ ] VK_EXT_pipeline_robustness
- [ ] VK_NV_device_diagnostics_config
- [ ] VK_NV_device_diagnostic_checkpoints

**Pending for profiling:**
- [ ] VK_EXT_pipeline_creation_feedback
- [ ] VK_EXT_primitives_generated_query
- [ ] VK_KHR_performance_query

**Will not be supported**
- VK_NV_linear_color_attachment
- VK_EXT_non_seamless_cube_map
- VK_NV_geometry_shader_passthrough
- VK_NV_compute_shader_derivatives, GL_NV_compute_shader_derivatives
- VK_KHR_pipeline_library
- VK_NV_raw_access_chains - for HLSL
- VK_KHR_index_type_uint8
- VK_NV_copy_memory_indirect

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
- [x] GL_EXT_ray_tracing, GL_EXT_ray_flags_primitive_culling, GL_EXT_ray_cull_mask
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
- [x] VK_KHR_shader_subgroup_uniform_control_flow, GL_EXT_subgroup_uniform_control_flow
- [x] GL_EXT_control_flow_attributes2
- [x] VK_KHR_shader_quad_control, GL_EXT_shader_quad
- [x] VK_KHR_shader_maximal_reconvergence, GL_EXT_maximal_reconvergence

**Pending shader extensions:**
- [ ] GL_EXT_multiview
- [ ] GL_EXT_null_initializer
- [ ] VK_KHR_zero_initialize_workgroup_memory
- [ ] VK_EXT_scalar_block_layout, GL_EXT_scalar_block_layout
- [ ] VK_HUAWEI_subpass_shading, GL_HUAWEI_subpass_shading ???
- [ ] VK_KHR_shader_terminate_invocation
- [ ] VK_KHR_workgroup_memory_explicit_layout
- [ ] VK_EXT_shader_image_atomic_int64
- [ ] VK_HUAWEI_cluster_culling_shader, GL_HUAWEI_cluster_culling_shader
- [ ] GL_EXT_expect_assume - depends on compiler?
- [ ] VK_NV_shader_atomic_float16_vector
- [ ] VK_KHR_shader_float_controls, SPV_KHR_float_controls - not supported by glslang
- [ ] VK_KHR_shader_float_controls2, SPV_KHR_float_controls2 - not supported by glslang
- [ ] GL_EXT_spirv_intrinsics
- [ ] GL_EXT_shared_memory_block
- [ ] GL_EXT_fragment_invocation_density
- [ ] GL_EXT_shader_tile_image
- [ ] GL_EXT_subgroupuniform_qualifier - not supported by glslang
- [ ] GL_EXT_terminate_invocation
- [ ] GL_NV_ray_tracing_motion_blur
- [ ] GL_NV_shader_texture_footprint
- [ ] GL_NV_shader_subgroup_partitioned

**Will not be supported**
- GL_EXT_vulkan_glsl_relaxed
- GL_NV_fragment_shader_barycentric
- GL_NV_mesh_shader
- GL_NV_primitive_shading_rate
- GL_NV_ray_tracing
- GL_NV_shading_rate_image
- GL_NVX_multiview_per_view_attributes
- GL_QCOM_image_processing

**Features:**
- [x] Async compute and transfer queues
- [x] Async queues with concurrent sharing mode
- [x] Async queues with exclusive sharing mode
- [x] Push constants (compatible with Metal)
- [x] Custom allocator for GPU memory (fast linear allocator, block allocator)
- [x] Shader subgroup
- [x] Secondary command buffers for async recording of draw commands
- [x] Immutable samplers

**TODO:**
- [ ] sparse memory
- [ ] reuse device memory immediately, destroy object with delay - memory on GPU side will be synchronized
- [ ] unbounded array descriptors
- [ ] push descriptors
