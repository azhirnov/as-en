# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if ((TARGET "Scripting") AND (TARGET "PipelineCompiler") AND (TARGET "ResourceLoaders") AND AE_HAS_CXX_COROUTINE)

	file( GLOB_RECURSE LIB_SOURCES "*.h" "*.cpp" "*.md" )
	file( GLOB_RECURSE DATA_FILES  "_ui_data/*.*" )

	set( SOURCES ${LIB_SOURCES} ${DATA_FILES} )

	if (APPLE)
		add_executable( "ResourceEditor" MACOSX_BUNDLE ${SOURCES} )
	else()
		add_executable( "ResourceEditor" ${SOURCES} )
	endif()

	source_group( TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES} )
	set_property( TARGET "ResourceEditor" PROPERTY FOLDER "Samples" )

	target_include_directories( "ResourceEditor" PUBLIC
		".."
		"${MAIN_SOURCE_DIR}/engine/tools/res_pack/asset_packer/Utils"
	)

	EnablePCH( "ResourceEditor" )

	target_link_libraries( "ResourceEditor" PUBLIC
		# tools
		"PipelineCompiler" "ResourceLoaders" "GeometryTools"
		# engine
		"GraphicsHL" "Video"
	)
	if (TARGET "Profiler")
		target_link_libraries( "ResourceEditor" PUBLIC "Profiler" )
	endif()
	if (TARGET "Compressonator-lib")
		target_link_libraries( "ResourceEditor" PUBLIC "Compressonator-lib" )
	endif()
	if (TARGET "AstcEncoder-lib")
		target_link_libraries( "ResourceEditor" PUBLIC "AstcEncoder-lib" )
	endif()

	target_compile_definitions( "ResourceEditor" PUBLIC
		AE_SHARED_DATA="${AE_ENGINE_SHARED_DATA}"
		AE_LOCAL_DATA_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/_data"
	)

	if (${AE_RES_EDITOR_RELEASE})
		target_compile_definitions( "ResourceEditor" PUBLIC "AE_PUBLIC_VERSION=true" )
	else()
		target_compile_definitions( "ResourceEditor" PUBLIC "AE_PUBLIC_VERSION=false" )
	endif()
	#----------------------------------------------------------

	set( AE_TEMP_FOLDER_RESEDITOR "${AE_TEMP_FOLDER}/samples/res_editor" )

	if (TARGET "OfflinePacker")
		add_custom_target( "ResourceEditor.PackRes"
			DEPENDS  "OfflinePacker"
			COMMAND  $<TARGET_FILE:OfflinePacker>
						-i "${CMAKE_CURRENT_SOURCE_DIR}/_ui_data/main.as"
						-o "${AE_TEMP_FOLDER_RESEDITOR}"
			COMMENT  "pack resources for 'ResourceEditor' ..."
			VERBATIM
		)
		set_property( TARGET "ResourceEditor.PackRes" PROPERTY FOLDER "Samples" )
		add_dependencies( "PACK_RES" "ResourceEditor.PackRes" )
	endif()

	add_subdirectory( "_data" )
endif()
