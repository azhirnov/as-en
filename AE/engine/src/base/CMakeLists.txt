# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

set( EXP_DIR "${MAIN_SOURCE_DIR}/engine/_experimental/base" )

file( GLOB_RECURSE HPP_SOURCES	"*.h" )
file( GLOB_RECURSE HPP_EXP		"${EXP_DIR}/*.h" )
file( GLOB_RECURSE CPP_SOURCES	"*.cpp" )
file( GLOB_RECURSE CPP_EXP		"${EXP_DIR}/*.cpp" )

if (MSVC OR APPLE)
	file( GLOB_RECURSE OBJC_SOURCES "*.mm" )
	file( GLOB_RECURSE OBJC_EXP		"${EXP_DIR}/*.mm" )
endif()

set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} ${OBJC_SOURCES} )
set( EXP_SOURCES ${HPP_EXP} ${CPP_EXP} ${OBJC_EXP} )

add_library( "Base" STATIC ${SOURCES} ${EXP_SOURCES} )

source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
source_group( TREE ${EXP_DIR} FILES ${EXP_SOURCES} )

set_property( TARGET "Base" PROPERTY FOLDER "Engine" )


# for IDE
if (FALSE) #WIN32)
	target_include_directories( "Base" PRIVATE
		"${ANDROID_NDK_INCLUDE}"
		"${ANDROID_NDK_INCLUDE}/lib/clang/17.0.2/include" )	# for IDE
endif()

if (MSVC)
	set_source_files_properties( ${OBJC_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
endif()

UseEnginePCH( "Base" )
EnableUnityBuild( "Base" )

install( TARGETS "Base" ARCHIVE DESTINATION "lib" )
