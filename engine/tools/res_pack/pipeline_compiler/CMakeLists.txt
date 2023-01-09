
if (TARGET "GLSLang-lib")
	set( GRAPHICS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/graphics" )
	set( GRAPHICS_SRC
		"${GRAPHICS_DIR}/Scripting/GraphicsBindings.h"
		"${GRAPHICS_DIR}/Scripting/GraphicsBindings.cpp"
		"${GRAPHICS_DIR}/Public/FeatureSet.h"
		"${GRAPHICS_DIR}/Private/FeatureSet.cpp"
		"${GRAPHICS_DIR}/Public/RenderState.h"
		"${GRAPHICS_DIR}/Private/RenderState.cpp"
		"${GRAPHICS_DIR}/Public/PipelineDesc.h"
		"${GRAPHICS_DIR}/Public/SamplerDesc.h"
		"${GRAPHICS_DIR}/Private/EnumUtils.cpp"
		"${GRAPHICS_DIR}/Private/EnumUtils.h"
		"${GRAPHICS_DIR}/Private/EnumToString.h"
		"${GRAPHICS_DIR}/Public/IDs.h" )
	file( GLOB_RECURSE SOURCES "*.*" )

	add_library( "PipelineCompiler" STATIC ${SOURCES} ${GRAPHICS_SRC} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	source_group( "external/graphics" FILES ${GRAPHICS_SRC} )

	target_link_libraries( "PipelineCompiler" PUBLIC "Serializing" "Scripting" )
	target_compile_definitions( "PipelineCompiler" PUBLIC "AE_BUILD_PIPELINE_COMPILER" "AE_TEST_PIPELINE_COMPILER" )
	target_include_directories( "PipelineCompiler" PUBLIC "." )

	if (WIN32)
		set( METAL_TOOLS "${AE_EXTERNAL_REP_PATH}/external/macos/MetalTools/bin" )
		if ( EXISTS "${METAL_TOOLS}" )
			target_compile_definitions( "PipelineCompiler" PUBLIC "AE_METAL_TOOLS=\"${METAL_TOOLS}\"" )
		else()
			message( STATUS "Metal tools '${METAL_TOOLS}' is not exists" )
		endif()
	endif()

	if (TARGET "Vulkan-lib")
		target_link_libraries( "PipelineCompiler" PUBLIC "Vulkan-lib" )
	endif()

	if (TARGET "ShaderTrace")
		target_link_libraries( "PipelineCompiler" PUBLIC "ShaderTrace" )
	endif()

	target_link_libraries( "PipelineCompiler" PUBLIC "GLSLang-lib" )

	if (TARGET "SPIRV-Cross-lib")
		target_link_libraries( "PipelineCompiler" PUBLIC "SPIRV-Cross-lib" )
	endif()

	add_library( "PipelineCompiler-shared" SHARED
		"PipelineCompiler.h"
		"Lib/main.cpp"
		"Lib/PipelineCompiler.def"
	)
	target_link_libraries( "PipelineCompiler-shared" PUBLIC "PipelineCompiler" )

	set_property( TARGET "PipelineCompiler" PROPERTY FOLDER "Engine/ResPack" )
	set_property( TARGET "PipelineCompiler-shared" PROPERTY FOLDER "Engine/ResPack" )

	# generated cpp file used as dependency to trigger project rebuild
	set( PipelineCompiler.trigger "${CMAKE_CURRENT_SOURCE_DIR}/PipelineCompiler.trigger.cpp" CACHE INTERNAL "" FORCE )
	add_custom_command( TARGET "PipelineCompiler-shared" POST_BUILD COMMAND ${CMAKE_COMMAND} -E touch "${PipelineCompiler.trigger}" )
endif()
