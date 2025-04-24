# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if ((TARGET "Scripting") AND (TARGET "PipelineCompiler") AND (TARGET "ResourceLoaders") AND AE_HAS_CXX_COROUTINE)

	file( GLOB_RECURSE LIB_SOURCES "*.h" "*.cpp" "*.md" )
	file( GLOB_RECURSE DATA_FILES  "_ui_data/*.*" )

	set( SOURCES ${LIB_SOURCES} ${DATA_FILES} )

	set( APP_NAME "ResourceEditor" )
	if (APPLE)
		add_executable( ${APP_NAME} MACOSX_BUNDLE ${SOURCES} )
	else()
		add_executable( ${APP_NAME} ${SOURCES} )
	endif()

	source_group( TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES} )
	set_property( TARGET ${APP_NAME} PROPERTY FOLDER "Samples" )

	target_include_directories( ${APP_NAME} PUBLIC
		".."
		"${MAIN_SOURCE_DIR}/engine/tools/res_pack/asset_packer/Utils"
		"${AE_ENGINE_SHARED_DATA}/scripts"
	)

	EnablePCH( ${APP_NAME} )
	EnableUnitBuild( ${APP_NAME} )

	if (${AE_USE_UNITY_BUILD})
		set( NON_UNITY_BUILD_SRC
			"${CMAKE_CURRENT_SOURCE_DIR}/Scripting/ScriptExe.cpp"
		)
		set_property( SOURCE ${NON_UNITY_BUILD_SRC} PROPERTY SKIP_UNITY_BUILD_INCLUSION ON )
	endif()

	target_link_libraries( ${APP_NAME} PUBLIC
		# tools
		"PipelineCompiler" "ResourceLoaders" "GeometryTools" "GraphicsTestUtils"
		# engine
		"Graphics" "Video"
	)
	if (TARGET "Profiler")
		target_link_libraries( ${APP_NAME} PUBLIC "Profiler" )
	endif()
	if (TARGET "Compressonator-lib")
		target_link_libraries( ${APP_NAME} PUBLIC "Compressonator-lib" )
	endif()
	if (TARGET "AstcEncoder-lib")
		target_link_libraries( ${APP_NAME} PUBLIC "AstcEncoder-lib" )
	endif()

	target_compile_definitions( ${APP_NAME} PUBLIC
		AE_SHARED_DATA="${AE_ENGINE_SHARED_DATA}"
		AE_LOCAL_DATA_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/_data"
	)

	if (${AE_RES_EDITOR_RELEASE})
		target_compile_definitions( ${APP_NAME} PUBLIC "AE_PUBLIC_VERSION=true" )
	else()
		target_compile_definitions( ${APP_NAME} PUBLIC "AE_PUBLIC_VERSION=false" )
	endif()
	#----------------------------------------------------------

	set( AE_TEMP_FOLDER_RESEDITOR "${AE_TEMP_FOLDER}/samples/res_editor" )

	if (TARGET "OfflinePacker")
		add_custom_target( "${APP_NAME}.PackRes"
			DEPENDS  "OfflinePacker"
			COMMAND  $<TARGET_FILE:OfflinePacker>
						-i "${CMAKE_CURRENT_SOURCE_DIR}/_ui_data/main.as"
						-o "${AE_TEMP_FOLDER_RESEDITOR}"
			COMMENT  "pack resources for '${APP_NAME}' ..."
			VERBATIM
		)
		set_property( TARGET "${APP_NAME}.PackRes" PROPERTY FOLDER "Samples" )
		add_dependencies( "PACK_RES" "${APP_NAME}.PackRes" )
	endif()

	add_subdirectory( "_data" )
endif()
