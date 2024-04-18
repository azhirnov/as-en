# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

find_package( Git QUIET REQUIRED )

if (DEFINED GIT_EXECUTABLE)
	execute_process(
		COMMAND "${GIT_EXECUTABLE}" describe --dirty=+ --tags --always
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE AE_COMMIT_HASH
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	set_property( DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/.git/index" )
endif()
