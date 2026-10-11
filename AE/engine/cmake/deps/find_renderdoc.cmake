# find RenderDoc header
function(ParseRenderDocLocation jsonFile outRDPath)
	if (CMAKE_VERSION VERSION_LESS 3.19)
		message( FATAL_ERROR "ParseRenderDocLocation requires CMake 3.19+ for string(JSON)" )
	endif()

	if (NOT EXISTS "${jsonFile}")
		message( FATAL_ERROR "JSON file not found: ${jsonFile}" )
	endif()

	file( READ "${jsonFile}" JSON_DATA )

	string( JSON LIBRARY_PATH ERROR_VARIABLE JSON_ERR GET "${JSON_DATA}" layer "library_path" )
	if (JSON_ERR)
		message( FATAL_ERROR "Failed to read library_path from ${jsonFile}: ${JSON_ERR}" )
	endif()

	# library_path is often a filename or relative to the JSON file
	if (NOT IS_ABSOLUTE "${LIBRARY_PATH}")
		get_filename_component( LAYER_DIR "${jsonFile}" DIRECTORY )
		get_filename_component( LIBRARY_PATH "${LAYER_DIR}/${LIBRARY_PATH}" ABSOLUTE )
	endif()
	get_filename_component( RD_DIR "${LIBRARY_PATH}" DIRECTORY )

	set( ${outRDPath} "${RD_DIR}/../include" PARENT_SCOPE )
endfunction()

if ( (${CMAKE_SYSTEM_NAME} STREQUAL "Windows") OR (${CMAKE_SYSTEM_NAME} STREQUAL "Linux") )

	if (WIN32)
		set( AE_RENDERDOC_PATH "C:/Program Files/RenderDoc" CACHE PATH "Path to RenderDoc installation folder" )
	else ()
		set( AE_RENDERDOC_PATH_INIT "" )
		set( RD_JSON_FILE "/etc/vulkan/implicit_layer.d/renderdoc_capture.json")
		if (EXISTS ${RD_JSON_FILE})
			ParseRenderDocLocation( ${RD_JSON_FILE} AE_RENDERDOC_PATH_INIT )
		endif()

		set( AE_RENDERDOC_PATH "${AE_RENDERDOC_PATH_INIT}" CACHE PATH "Path to RenderDoc installation folder" )
	endif()

	if (EXISTS "${AE_RENDERDOC_PATH}/renderdoc_app.h")
		message( STATUS "Found RenderDoc in '${AE_RENDERDOC_PATH}'" )

		add_library( "RenderDoc-lib" INTERFACE )

		target_include_directories( "RenderDoc-lib" INTERFACE "${AE_RENDERDOC_PATH}" )
		target_compile_definitions( "RenderDoc-lib" INTERFACE "AE_ENABLE_RENDERDOC" )
	else()
		message( STATUS "RenderDoc is NOT found in '${AE_RENDERDOC_PATH}'" )
	endif()
endif()
