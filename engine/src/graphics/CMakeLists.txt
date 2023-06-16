if (${AE_ENABLE_VULKAN} OR ${AE_ENABLE_METAL} OR ${AE_ENABLE_REMOTE_GRAPHICS})

	file( GLOB_RECURSE PRIVATE_SOURCES "Private/*.*" )
	file( GLOB_RECURSE PUBLIC_SOURCES "Public/*.*" )
	file( GLOB_RECURSE RG_SOURCES "RenderGraph/*.*" )

	if (TARGET "Scripting")
		file( GLOB_RECURSE SCRIPTING_SOURCES "Scripting/*.*" )
	endif()

	if (APPLE AND NOT ${AE_ENABLE_METAL})
		message( FATAL_ERROR "AE_ENABLE_METAL must be enabled" )
	endif()

	if (${AE_ENABLE_METAL} OR (MSVC AND NOT ${AE_ENABLE_REMOTE_GRAPHICS}))
		file( GLOB_RECURSE METAL_CPP_SOURCES "Metal/*.h" "Metal/*.cpp" )
		file( GLOB_RECURSE METAL_OBJC_SOURCES "Metal/*.mm" )
	endif()
	if (MSVC OR ${AE_ENABLE_REMOTE_GRAPHICS})
		file( GLOB_RECURSE REMOTE_SOURCES "Remote/*.h" "Remote/*.cpp" )
	endif()

	set( PIPELINE_COMPILER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/res_pack/pipeline_compiler" )
	set( PIPELINE_COMPILER_SRC
		"${PIPELINE_COMPILER_DIR}/Packer/PipelinePack.h"
		#"${PIPELINE_COMPILER_DIR}/Packer/PipelinePackDeserializer.cpp"
		"${PIPELINE_COMPILER_DIR}/Packer/RenderPassPack.h"
		#"${PIPELINE_COMPILER_DIR}/Packer/RenderPassPack.cpp"
		"${PIPELINE_COMPILER_DIR}/Packer/SamplerPack.h"
		#"${PIPELINE_COMPILER_DIR}/Packer/SamplerPack.cpp"
		"${PIPELINE_COMPILER_DIR}/Packer/FeatureSetPack.h"
		#"${PIPELINE_COMPILER_DIR}/Packer/FeatureSetPack.cpp"
	)
	
	if (${AE_ENABLE_VULKAN})
		file( GLOB_RECURSE VULKAN_SOURCES "Vulkan/*.*" )
		set( SHADER_TRACE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/res_pack/shader_trace" )
		set( SHADER_TRACE_SRC
			"${SHADER_TRACE_DIR}/Public/ShaderTrace.h"
			"${SHADER_TRACE_DIR}/Impl/ShaderTrace.cpp"
			"${SHADER_TRACE_DIR}/Impl/ParseShaderTrace.cpp" )
	else()
		set( SHADER_TRACE_SRC "" )
	endif()

	set( SOURCES
		${PRIVATE_SOURCES} ${PUBLIC_SOURCES} ${RG_SOURCES} ${SCRIPTING_SOURCES}
		${VULKAN_SOURCES}
		${METAL_CPP_SOURCES} ${METAL_OBJC_SOURCES}
		${REMOTE_SOURCES}
	)

	add_library( "Graphics" STATIC ${SOURCES} ${PIPELINE_COMPILER_SRC} ${SHADER_TRACE_SRC} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	source_group( "external/pipeline_compiler" FILES ${PIPELINE_COMPILER_SRC} )

	set_property( TARGET "Graphics" PROPERTY FOLDER "Engine" )
	target_link_libraries( "Graphics" PUBLIC "Threading" "Serializing" )

	target_include_directories( "Graphics" PUBLIC "${PIPELINE_COMPILER_DIR}" )

	if (${AE_ENABLE_VULKAN})
		source_group( "external/shader_trace" FILES ${SHADER_TRACE_SRC} )
		target_link_libraries( "Graphics" PUBLIC "Vulkan-lib" "VulkanExtEmulation" )
		target_include_directories( "Graphics" PUBLIC "${SHADER_TRACE_DIR}/Public" )
		target_compile_definitions( "Graphics" PRIVATE AE_ENABLE_GLSL_TRACE )

		if (${AE_ENABLE_VMA})
			target_link_libraries( "Graphics" PUBLIC "VMA-lib" )
		endif()

		if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
			set( AE_VK_TIMELINE_SEMAPHORE OFF CACHE BOOL "Use Vulkan timeline semaphore" )
		else()
			set( AE_VK_TIMELINE_SEMAPHORE ON CACHE BOOL "Use Vulkan timeline semaphore" )
		endif()
		mark_as_advanced( AE_VK_TIMELINE_SEMAPHORE )
		if (${AE_VK_TIMELINE_SEMAPHORE})
			target_compile_definitions( "Graphics" PUBLIC AE_VK_TIMELINE_SEMAPHORE=1 )
		else()
			target_compile_definitions( "Graphics" PUBLIC AE_VK_TIMELINE_SEMAPHORE=0 )
		endif()
	endif()
	
	if (${AE_ENABLE_METAL} OR MSVC)
		if ( (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin") OR (${CMAKE_SYSTEM_NAME} STREQUAL "iOS") )
			find_library( METAL_FRAMEWORK Metal REQUIRED )
			target_link_libraries( "Graphics" PUBLIC ${METAL_FRAMEWORK} )
			set_target_properties( "Graphics" PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC NO )
		else()
			set_source_files_properties( ${METAL_OBJC_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
		endif()
	endif()

	if (${AE_ENABLE_REMOTE_GRAPHICS})
		target_link_libraries( "Graphics" PUBLIC "Networking" )
		target_compile_definitions( "Graphics" PRIVATE AE_ENABLE_REMOTE_GRAPHICS )
	endif()

	if (WIN32)
		target_link_libraries( "Graphics" PUBLIC "NVPerf-lib" )
	endif()

	install( TARGETS "Graphics" ARCHIVE DESTINATION "lib" )

endif()
