# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Scripting")
	add_executable( "OfflinePacker" "main.cpp" )
	source_group( "" FILES "main.cpp" )

	set_property( TARGET "OfflinePacker" PROPERTY FOLDER "Engine/ResPack" )

	target_link_libraries( "OfflinePacker" PRIVATE "Scripting" "VFS" )
	target_include_directories( "OfflinePacker" PRIVATE ".." )

	set( CANVAS_VERTS_PATH	"${MAIN_SOURCE_DIR}/engine/src/graphics_hl/Canvas/VertexTypes.as" )
	set( UI_BINDINGS_PATH	"${MAIN_SOURCE_DIR}/engine/src/graphics_hl/UI" )

	if (NOT EXISTS ${CANVAS_VERTS_PATH})
		message( FATAL_ERROR "Path '${CANVAS_VERTS_PATH}' is not exists" )
	endif()
	if (NOT EXISTS ${UI_BINDINGS_PATH})
		message( FATAL_ERROR "Path '${UI_BINDINGS_PATH}' is not exists" )
	endif()

	target_compile_definitions( "OfflinePacker" PRIVATE
		AE_SHARED_DATA="${AE_ENGINE_SHARED_DATA}"
		AE_CANVAS_VERTS="${CANVAS_VERTS_PATH}"
		AE_UI_BINDINGS="${UI_BINDINGS_PATH}"
		AE_BUILD_ASSET_PACKER
		AE_BUILD_INPUT_ACTIONS_BINDING
		AE_BUILD_PIPELINE_COMPILER
	)

	set( OFFLINE_PACKER_USE_STATIC_LIBS ON CACHE BOOL "use static libs instead of dynamic" FORCE )
	if (${OFFLINE_PACKER_USE_STATIC_LIBS})
		target_link_libraries( "OfflinePacker" PRIVATE "PipelineCompiler" "InputActionsBinding" "AssetPacker" )
		target_compile_definitions( "OfflinePacker" PRIVATE AE_OFFLINE_PACKER_USE_STATIC_LIBS )
	else()
		if (TARGET "PipelineCompiler-shared")
			target_compile_definitions( "OfflinePacker" PRIVATE AE_PIPELINE_COMPILER_LIBRARY="$<TARGET_FILE:PipelineCompiler-shared>" )
			add_dependencies( "OfflinePacker" "PipelineCompiler-shared" )
			target_sources( "OfflinePacker" PRIVATE "${PipelineCompiler.trigger}" )
			source_group( "_gen_" FILES "${PipelineCompiler.trigger}" )
			set_property( SOURCE "${PipelineCompiler.trigger}" PROPERTY GENERATED 1 )
		endif()

		if (TARGET "AssetPacker-shared")
			target_compile_definitions( "OfflinePacker" PRIVATE AE_ASSET_PACKER_LIBRARY="$<TARGET_FILE:AssetPacker-shared>" )
			add_dependencies( "OfflinePacker" "AssetPacker-shared" )
			target_sources( "OfflinePacker" PRIVATE "${AssetPacker.trigger}" )
			source_group( "_gen_" FILES "${AssetPacker.trigger}" )
			set_property( SOURCE "${AssetPacker.trigger}" PROPERTY GENERATED 1 )
		endif()

		if (TARGET "InputActionsBinding-shared")
			target_compile_definitions( "OfflinePacker" PRIVATE AE_INPUT_ACTIONS_BINDING_LIBRARY="$<TARGET_FILE:InputActionsBinding-shared>" )
			add_dependencies( "OfflinePacker" "InputActionsBinding-shared" )
			target_sources( "OfflinePacker" PRIVATE "${InputActionsBinding.trigger}" )
			source_group( "_gen_" FILES "${InputActionsBinding.trigger}" )
			set_property( SOURCE "${InputActionsBinding.trigger}" PROPERTY GENERATED 1 )
		endif()
	endif()

	#------------------------------------------------------

	add_executable( "PACK_RES" EXCLUDE_FROM_ALL "empty.cpp" )
	set_property( TARGET "PACK_RES" PROPERTY FOLDER "CMakePredefinedTargets" )
	add_dependencies( "PACK_RES" "OfflinePacker" )
endif()
