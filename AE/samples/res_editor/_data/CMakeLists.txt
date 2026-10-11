# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

file( GLOB_RECURSE DATA_FILES  "*.as" "*.glsl" "*.msl" "*.ppln" "*.md" )

if (NOT DEFINED AE_ENGINE_SHARED_DATA)
	set( AE_ENGINE_SHARED_DATA "${CMAKE_CURRENT_SOURCE_DIR}/../shared_data" )
endif()

file( GLOB SHADERS_SHARED "${AE_ENGINE_SHARED_DATA}/shaders/*.glsl" "${AE_ENGINE_SHARED_DATA}/shaders/*.h" "${AE_ENGINE_SHARED_DATA}/experimental/*.glsl" )
file( GLOB_RECURSE SHADERS_3PARTY "${AE_ENGINE_SHARED_DATA}/3party_shaders/*.glsl" )


#----------------------------------------------------------
if (DEFINED MAIN_BINARY_DIR)
	if (WIN32)
		file( GLOB LOCAL_CONFIGS "${MAIN_BINARY_DIR}/*/res_editor_cfg.as" )
	else()
		file( GLOB LOCAL_CONFIGS "${MAIN_BINARY_DIR}/res_editor_cfg.as" )
	endif()
else()
	file( GLOB LOCAL_CONFIGS "../res_editor_cfg.as" )
endif()

if (WIN32)
	execute_process( COMMAND powershell -NoProfile -Command "[Environment]::GetFolderPath('MyDocuments')"
					 OUTPUT_VARIABLE USER_DOCUMENTS
					 OUTPUT_STRIP_TRAILING_WHITESPACE
					)
elseif (APPLE)
	set( USER_DOCUMENTS "$ENV{HOME}/Documents" )

else(UNIX)
	execute_process( COMMAND xdg-user-dir DOCUMENTS
					 OUTPUT_VARIABLE USER_DOCUMENTS
					 OUTPUT_STRIP_TRAILING_WHITESPACE
					 ERROR_QUIET
					)
	if (NOT USER_DOCUMENTS)
		set( USER_DOCUMENTS "$ENV{HOME}/Documents" )
	endif()
endif()

# Optional: normalize to CMake-style paths (forward slashes)
file( TO_CMAKE_PATH "${USER_DOCUMENTS}" USER_DOCUMENTS )

if (${AE_ENABLE_REMOTE_GRAPHICS})
	file( GLOB SHARED_CONFIGS "${USER_DOCUMENTS}/as-en/rmg_res_editor_cfg.as" )
else()
	file( GLOB SHARED_CONFIGS "${USER_DOCUMENTS}/as-en/res_editor_cfg.as" )
endif()
set( CONFIGS "${LOCAL_CONFIGS}" "${SHARED_CONFIGS}" )
#----------------------------------------------------------


set( SOURCES ${DATA_FILES} ${SHADERS_SHARED} ${SHADERS_3PARTY} ${CONFIGS} )

add_library( "ResourceEditor.Data" STATIC EXCLUDE_FROM_ALL ${SOURCES} )
set_property( TARGET "ResourceEditor.Data" PROPERTY LINKER_LANGUAGE CXX )

if (TARGET "ResourceEditor")
	set_property( TARGET "ResourceEditor.Data" PROPERTY FOLDER "Samples" )

	# much faster startup
	add_custom_target( "ResourceEditor.App"
		COMMAND  "${MAIN_BINARY_DIR}/$<CONFIG>/ResourceEditor${AE_EXECUTABLE_SUFFIX}"
	)
	set_property( TARGET "ResourceEditor.App" PROPERTY FOLDER "Samples" )
else()
	if (WIN32)
		add_custom_target( "ResourceEditor.App"
			COMMAND  "../ResourceEditor-dev.exe"
		)
	else()
		add_custom_target( "ResourceEditor.App"
			COMMAND  "../ResourceEditor"
		)
	endif()
endif()

source_group( TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${DATA_FILES} )
source_group( "shaders/shared" FILES ${SHADERS_SHARED} )
source_group( "shaders/3party" FILES ${SHADERS_3PARTY} )
source_group( "config" FILES ${LOCAL_CONFIGS} )
source_group( "config/shared" FILES ${SHARED_CONFIGS} )

target_include_directories( "ResourceEditor.Data" PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/shaders"
	"${CMAKE_CURRENT_SOURCE_DIR}/script_inc"
	"${CMAKE_CURRENT_SOURCE_DIR}/pipeline_inc"
	"${AE_ENGINE_SHARED_DATA}/scripts"
	"${AE_ENGINE_SHARED_DATA}/shaders"
)

# optional: allow to compile scripts & shaders
if (TRUE)
	set_source_files_properties( ${SOURCES} PROPERTIES LANGUAGE CXX )
	set_target_properties( "ResourceEditor.Data" PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES )
	target_compile_features( "ResourceEditor.Data" PUBLIC cxx_std_20 )

	target_compile_definitions( "ResourceEditor.Data" PUBLIC
		"SCRIPT"
		"AE_LICENSE_MIT"
		"AE_LICENSE_BSD2"
		"AE_LICENSE_BSD3"
		"AE_LICENSE_APACHE_2"
		"AE_LICENSE_UNLICENSE"
		"AE_LICENSE_CC0"
		"AE_LICENSE_CC_BY_NC_SA_3"
		"AE_ENABLE_UNKNOWN_LICENSE"
	)
endif()
