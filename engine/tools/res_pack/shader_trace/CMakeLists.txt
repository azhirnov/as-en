cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

file( GLOB_RECURSE SOURCES "*.*" )
add_library( "ShaderTrace" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

set_property( TARGET "ShaderTrace" PROPERTY FOLDER "Engine/ResPack" )

target_include_directories( "ShaderTrace" PUBLIC "Public" )

target_link_libraries( "ShaderTrace" PUBLIC "Base" "Serializing" )

if (TARGET "GLSLang-lib")
	target_link_libraries( "ShaderTrace" PUBLIC "GLSLang-lib" )
else()
	message( FATAL_ERROR "GLSLang is required" )
endif()

target_compile_definitions( "ShaderTrace" PUBLIC "AE_ENABLE_GLSL_TRACE" )
