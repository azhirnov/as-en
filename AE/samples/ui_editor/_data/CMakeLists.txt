# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.22 FATAL_ERROR )
project( "UIEditor" )

file( GLOB_RECURSE DATA_FILES  "*.as" "*.glsl" "*.msl" "*.vs" )

set( SOURCES ${DATA_FILES} )

add_library( "UIEditor.Data" STATIC EXCLUDE_FROM_ALL ${SOURCES} )
set_property( TARGET "UIEditor.Data" PROPERTY LINKER_LANGUAGE CXX )

if (TARGET "UIEditor")
	set_property( TARGET "UIEditor.Data" PROPERTY FOLDER "Samples" )

	# much faster startup
	add_custom_target( "UIEditor.App"
		COMMAND  "${MAIN_BINARY_DIR}/$<CONFIG>/UIEditor${AE_EXECUTABLE_SUFFIX}"
	)
	set_property( TARGET "UIEditor.App" PROPERTY FOLDER "Samples" )
else()
	if (WIN32)
		add_custom_target( "UIEditor"
			COMMAND  "../UIEditor.exe"
		)
	else()
		add_custom_target( "UIEditor"
			COMMAND  "../UIEditor"
		)
	endif()
endif()

source_group( TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${DATA_FILES} )

target_include_directories( "UIEditor.Data" PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/ui_styles"
	"${CMAKE_CURRENT_SOURCE_DIR}/ui"
	"${AE_ENGINE_SHARED_DATA}/scripts"
)

# optional: allow to compile scripts & shaders
if (TRUE)
	set_source_files_properties( ${SOURCES} PROPERTIES LANGUAGE CXX )
	set_target_properties( "UIEditor.Data" PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES )
	target_compile_features( "UIEditor.Data" PUBLIC cxx_std_20 )
endif()
