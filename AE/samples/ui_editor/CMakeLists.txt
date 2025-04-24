# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if ((TARGET "Scripting") AND (TARGET "AssetPacker") AND AE_HAS_CXX_COROUTINE AND (NOT AE_ENABLE_REMOTE_GRAPHICS))

	file( GLOB_RECURSE CPP_SOURCES "*.h" "*.cpp" "*.md" )
	file( GLOB_RECURSE DATA_FILES  "_ui_data/*.*" )

	set( APP_NAME "UIEditor" )
	if (APPLE)
		add_executable( ${APP_NAME} MACOSX_BUNDLE ${CPP_SOURCES} ${DATA_FILES} )
	else()
		add_executable( ${APP_NAME} ${CPP_SOURCES} ${DATA_FILES} )
	endif()

	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPP_SOURCES} ${DATA_FILES} )
	set_property( TARGET ${APP_NAME} PROPERTY FOLDER "Samples" )
	target_include_directories( ${APP_NAME} PUBLIC ".." )

	target_include_directories( ${APP_NAME} PUBLIC
		"${AE_ENGINE_SHARED_DATA}/scripts"
		"${AE_ENGINE_SHARED_DATA}/shaders"
		"${CMAKE_CURRENT_SOURCE_DIR}/_data/ui_styles"
	)

	target_link_libraries( ${APP_NAME} PUBLIC
		"Platform" "Graphics"
		"ResourceLoaders"
	)

	add_dependencies( ${APP_NAME} "PipelineCompiler-shared" "AssetPacker-shared" )
	target_sources( ${APP_NAME} PRIVATE "${PipelineCompiler.trigger}" "${AssetPacker.trigger}" )
	source_group( "_gen_" FILES "${PipelineCompiler.trigger}" "${AssetPacker.trigger}" )
	set_property( SOURCE "${PipelineCompiler.trigger}" PROPERTY GENERATED 1 )
	set_property( SOURCE "${AssetPacker.trigger}" PROPERTY GENERATED 1 )

	EnablePCH( ${APP_NAME} )
	EnableUnitBuild( ${APP_NAME} )

	set( AE_TEMP_FOLDER_UIEDITOR "${AE_TEMP_FOLDER}/samples/ui_editor/ui" )
	target_compile_definitions( ${APP_NAME} PUBLIC
		AE_RES_FOLDER="${AE_TEMP_FOLDER_UIEDITOR}"
		AE_UI_SCRIPT_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/_data/ui"
		AE_CANVAS_VERTS="${MAIN_SOURCE_DIR}/engine/src/graphics/Canvas/VertexTypes.as"
		AE_SHARED_DATA="${AE_ENGINE_SHARED_DATA}"
		AE_LIBRARY_FOLDER="${MAIN_BINARY_DIR}"
	)

	#----------------------------------------------------------

	if (TARGET "OfflinePacker")
		add_custom_target( "${APP_NAME}.PackRes"
			DEPENDS  "OfflinePacker"
			COMMAND  $<TARGET_FILE:OfflinePacker>
						-i "${CMAKE_CURRENT_SOURCE_DIR}/_ui_data/main.as"
						-o "${AE_TEMP_FOLDER_UIEDITOR}"
			COMMENT  "pack resources for '${APP_NAME}' ..."
			VERBATIM
		)
		set_property( TARGET "${APP_NAME}.PackRes" PROPERTY FOLDER "Samples" )
		add_dependencies( "PACK_RES" "${APP_NAME}.PackRes" )
	endif()

	add_subdirectory( "_data" )
endif()
