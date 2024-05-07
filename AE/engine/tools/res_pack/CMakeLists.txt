# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (${AE_DESKTOP} AND (TARGET "Scripting") AND ${AE_ENABLE_EXCEPTIONS})
	add_subdirectory( "pipeline_compiler" )
	add_subdirectory( "input_actions" )
	add_subdirectory( "asset_packer" )

	# Executable
	add_subdirectory( "offline" )
endif()
