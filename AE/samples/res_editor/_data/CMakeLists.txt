# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.18 FATAL_ERROR )
project( "ResourceEditor" )

file( GLOB_RECURSE DATA_FILES  "*.as" "*.glsl" "*.msl" )

file( GLOB_RECURSE SHADERS_SHARED "${AE_ENGINE_SHARED_DATA}/shaders/*.glsl" )
file( GLOB_RECURSE SHADERS_3PARTY "${AE_ENGINE_SHARED_DATA}/3party_shaders/*.glsl" "${AE_ENGINE_SHARED_DATA}/experimental/*.glsl" )

if (WIN32)
	file( GLOB CONFIGS "${MAIN_BINARY_DIR}/*/res_editor_cfg.as" )
else()
	file( GLOB CONFIGS "${MAIN_BINARY_DIR}/res_editor_cfg.as" )
endif()
set( SOURCES ${DATA_FILES} ${SHADERS_SHARED} ${SHADERS_3PARTY} ${CONFIGS} )

add_library( "ResourceEditor.Data" STATIC EXCLUDE_FROM_ALL ${SOURCES} ${PREBUILD_CPP_FILE} )
set_property( TARGET "ResourceEditor.Data" PROPERTY LINKER_LANGUAGE CXX )

if (TARGET "ResourceEditor")
	set_property( TARGET "ResourceEditor.Data" PROPERTY FOLDER "Samples" )
else()
	if (WIN32)
		add_custom_target( "ResourceEditor"
			COMMAND  "../ResourceEditor.exe"
		)
	else()
		add_custom_target( "ResourceEditor"
			COMMAND  "../ResourceEditor"
		)
	endif()
endif()

source_group( TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${DATA_FILES} )
source_group( "shaders/shared" FILES ${SHADERS_SHARED} )
source_group( "shaders/3party" FILES ${SHADERS_3PARTY} )
source_group( "config" FILES ${CONFIGS} )

target_include_directories( "ResourceEditor.Data" PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/shaders"
	"${CMAKE_CURRENT_SOURCE_DIR}/script_inc"
	"${CMAKE_CURRENT_SOURCE_DIR}/pipeline_inc"
	"${AE_ENGINE_SHARED_DATA}/scripts"
	"${AE_ENGINE_SHARED_DATA}/shaders"
)
