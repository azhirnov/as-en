# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
	file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
	file( GLOB_RECURSE OBJC_SOURCES "*.mm" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} ${OBJC_SOURCES} )

add_library( "Threading" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "Threading" PROPERTY FOLDER "Engine" )
target_link_libraries( "Threading" PUBLIC "Base" )

if (NOT APPLE)
	set_source_files_properties( ${OBJC_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set( AE_ENABLE_IO_URING ON CACHE BOOL "use io_uring instead of Linux AIO, io_uring may be unsupported" )
endif()

if (APPLE)
	# true async
	target_compile_definitions( "Threading" PUBLIC AE_ASYNCIO_USE_POSIX_AIO )

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
	target_compile_definitions( "Threading" PUBLIC AE_ASYNCIO_USE_LINUX_AIO )

elseif ((${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD") OR
		(${CMAKE_SYSTEM_NAME} STREQUAL "DragonFly"))
	target_compile_definitions( "Threading" PUBLIC AE_ASYNCIO_USE_BSD_AIO )

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	if (${AE_ENABLE_IO_URING})
		target_compile_definitions( "Threading" PUBLIC AE_ASYNCIO_USE_IO_URING )
		target_link_libraries( "Threading" PUBLIC "uring" )
	else()
		# true async
		target_compile_definitions( "Threading" PUBLIC AE_ASYNCIO_USE_LINUX_AIO )

		# impl dependent, may have thread pool inside
		#target_compile_definitions( "Threading" PUBLIC AE_ASYNCIO_USE_POSIX_AIO )
	endif()
endif()

EnablePCH( "Threading" )
EnablePrebuild( "Threading" )
EnableUnitBuild( "Threading" )

install( TARGETS "Threading" ARCHIVE DESTINATION "lib" )
