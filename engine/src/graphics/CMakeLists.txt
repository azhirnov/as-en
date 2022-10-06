file( GLOB_RECURSE PRIVATE_SOURCES "Private/*.*" )
file( GLOB_RECURSE PUBLIC_SOURCES "Public/*.*" )

if (TARGET "Scripting")
	file( GLOB_RECURSE SCRIPTING_SOURCES "Scripting/*.*" )
endif()

if (NOT DEFINED AE_ENABLE_VULKAN AND NOT DEFINED AE_ENABLE_METAL)
	message( FATAL_ERROR "AE_ENABLE_VULKAN or AE_ENABLE_METAL must be enabled" )
endif()

if (APPLE AND NOT ${AE_ENABLE_METAL})
	message( FATAL_ERROR "AE_ENABLE_METAL must be enabled for Apple" )
endif()

if (${AE_ENABLE_METAL})
	file( GLOB_RECURSE METAL_CPP_SOURCES "Metal/*.h" "Metal/*.cpp" )
	file( GLOB_RECURSE METAL_OBJC_SOURCES "Metal/*.mm" )
endif()

set( PIPELINE_COMPILER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/res_pack/pipeline_compiler" )
set( PIPELINE_COMPILER_SRC )
#	"${PIPELINE_COMPILER_DIR}/Packer/PipelinePackDeserializer.cpp"
#	"${PIPELINE_COMPILER_DIR}/Packer/RenderPassPack.cpp"
#	"${PIPELINE_COMPILER_DIR}/Packer/SamplerPack.cpp"
#	"${PIPELINE_COMPILER_DIR}/Packer/FeatureSetPack.cpp" )
	
if (${AE_ENABLE_VULKAN})
	file( GLOB_RECURSE VULKAN_SOURCES "Vulkan/*.*" )
	set( SHADER_TRACE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/res_pack/shader_trace" )
	set( SHADER_TRACE_SRC )
	#	"${SHADER_TRACE_DIR}/include/ShaderTrace.h"
	#	"${SHADER_TRACE_DIR}/source/ShaderTrace.cpp"
	#	"${SHADER_TRACE_DIR}/source/ParseShaderTrace.cpp" )
else()
	set( SHADER_TRACE_SRC "" )
endif()

set( SOURCES
	${PRIVATE_SOURCES} ${PUBLIC_SOURCES} ${SCRIPTING_SOURCES}
	${VULKAN_SOURCES}
	${METAL_CPP_SOURCES} ${METAL_OBJC_SOURCES}
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
	target_include_directories( "Graphics" PUBLIC "${SHADER_TRACE_DIR}/include" )
	target_compile_definitions( "Graphics" PRIVATE AE_ENABLE_GLSL_TRACE )

	if (${AE_ENABLE_VMA})
		target_link_libraries( "Graphics" PUBLIC "VMA-lib" )
	endif ()
endif ()

if (${AE_ENABLE_METAL})
	target_compile_definitions( "Graphics" PUBLIC AE_ENABLE_METAL )
	find_library( METAL_FRAMEWORK Metal REQUIRED )
	target_link_libraries( "Graphics" PUBLIC ${METAL_FRAMEWORK} )
	set_target_properties( "Graphics" PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC NO )
endif()

if (${AE_ENABLE_OPENGL})
	target_link_libraries( "Graphics" PUBLIC "OpenGL-lib" )
endif ()
