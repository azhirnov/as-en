# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

set( CMAKE_FOLDER "${MAIN_SOURCE_DIR}/engine/cmake" )
set( EXTERNAL_FOLDER "${MAIN_SOURCE_DIR}/engine/external" )

include( "${CMAKE_FOLDER}/compilers.cmake" )
include( "${CMAKE_FOLDER}/compiler_tests.cmake" )

#----------------------------------------------------------

file( GLOB_RECURSE SOURCES "*.*" )
file( GLOB_RECURSE CMAKE_SOURCES "${CMAKE_FOLDER}/*.*" )
file( GLOB_RECURSE EXERNAL_CMAKE "${EXTERNAL_FOLDER}/*/CMakeLists.txt" )

add_library( "ProjectTemplate" STATIC EXCLUDE_FROM_ALL ${SOURCES} ${CMAKE_SOURCES} ${EXERNAL_CMAKE} )
set_property( TARGET "ProjectTemplate" PROPERTY FOLDER "Engine/Tools" )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
source_group( TREE "${CMAKE_FOLDER}/.." FILES ${CMAKE_SOURCES} )
source_group( TREE "${EXTERNAL_FOLDER}/.." FILES ${EXERNAL_CMAKE} )

target_compile_definitions( "ProjectTemplate" PUBLIC ${AE_COMPILER_DEFINITIONS} )

# All Configurations
if (PROJECTS_SHARED_DEFINES)
	target_compile_definitions( "ProjectTemplate" PUBLIC ${PROJECTS_SHARED_DEFINES} )
endif()

# Debug
if (PROJECTS_SHARED_DEFINES_DEBUG)
	target_compile_definitions( "ProjectTemplate" PUBLIC $<$<CONFIG:Debug>: ${PROJECTS_SHARED_DEFINES_DEBUG}> )
endif()
if (PROJECTS_SHARED_CXX_FLAGS_DEBUG)
	target_compile_options( "ProjectTemplate" PUBLIC $<$<CONFIG:Debug>: ${PROJECTS_SHARED_CXX_FLAGS_DEBUG}> )
endif()
if (PROJECTS_SHARED_LINKER_FLAGS_DEBUG)
	set_target_properties( "ProjectTemplate" PROPERTIES LINK_FLAGS_DEBUG ${PROJECTS_SHARED_LINKER_FLAGS_DEBUG} )
endif()

# Develop
if (PROJECTS_SHARED_DEFINES_DEVELOP)
	target_compile_definitions( "ProjectTemplate" PUBLIC $<$<CONFIG:Develop>: ${PROJECTS_SHARED_DEFINES_DEVELOP}> )
endif()
if (PROJECTS_SHARED_CXX_FLAGS_DEVELOP)
	target_compile_options( "ProjectTemplate" PUBLIC $<$<CONFIG:Develop>: ${PROJECTS_SHARED_CXX_FLAGS_DEVELOP}> )
endif()
if (PROJECTS_SHARED_LINKER_FLAGS_DEVELOP)
	set_target_properties( "ProjectTemplate" PROPERTIES LINK_FLAGS_DEVELOP ${PROJECTS_SHARED_LINKER_FLAGS_DEVELOP} )
endif()

# Profile
if (PROJECTS_SHARED_DEFINES_PROFILE)
	target_compile_definitions( "ProjectTemplate" PUBLIC $<$<CONFIG:Profile>: ${PROJECTS_SHARED_DEFINES_PROFILE}> )
endif()
if (PROJECTS_SHARED_CXX_FLAGS_PROFILE)
	target_compile_options( "ProjectTemplate" PUBLIC $<$<CONFIG:Profile>: ${PROJECTS_SHARED_CXX_FLAGS_PROFILE}> )
endif()
if (PROJECTS_SHARED_LINKER_FLAGS_PROFILE)
	set_target_properties( "ProjectTemplate" PROPERTIES LINK_FLAGS_PROFILE ${PROJECTS_SHARED_LINKER_FLAGS_PROFILE} )
endif()

# Release
if (PROJECTS_SHARED_DEFINES_RELEASE)
	target_compile_definitions( "ProjectTemplate" PUBLIC $<$<CONFIG:Release>: ${PROJECTS_SHARED_DEFINES_RELEASE}> )
endif()
if (PROJECTS_SHARED_CXX_FLAGS_RELEASE)
	target_compile_options( "ProjectTemplate" PUBLIC $<$<CONFIG:Release>: ${PROJECTS_SHARED_CXX_FLAGS_RELEASE}> )
endif()
if (PROJECTS_SHARED_LINKER_FLAGS_RELEASE)
	set_target_properties( "ProjectTemplate" PROPERTIES LINK_FLAGS_RELEASE ${PROJECTS_SHARED_LINKER_FLAGS_RELEASE} )
endif()

# C++ standard
if (TRUE)
	list( FIND CMAKE_CXX_COMPILE_FEATURES "cxx_std_17" HAS_CPP17 )
	if ( ${HAS_CPP17} LESS 0 )
		message( FATAL_ERROR "C++17 is required" )
	endif()

	list( FIND CMAKE_CXX_COMPILE_FEATURES "cxx_std_20" HAS_CPP20 )
	if ( ${HAS_CPP20} LESS 0 )
		message( STATUS "C++20 is NOT supported" )
	endif()

	# use C++ 20
	if ( ${HAS_CPP20} GREATER_EQUAL 0 )
		set_target_properties( "ProjectTemplate" PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES )
		target_compile_features( "ProjectTemplate" PUBLIC cxx_std_20 )
		target_compile_definitions( "ProjectTemplate" PUBLIC "AE_CXX_20" )
	else()
	# use C++ 17
		set_target_properties( "ProjectTemplate" PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES )
		target_compile_features( "ProjectTemplate" PUBLIC cxx_std_17 )
		target_compile_definitions( "ProjectTemplate" PUBLIC "AE_CXX_17" )
	endif()
endif()

if (${AE_CI_BUILD_NO_GRAPHICS})
	target_compile_definitions( "ProjectTemplate" PUBLIC "AE_CI_BUILD" "AE_CI_BUILD_NO_GRAPHICS"  )
endif()
if (${AE_CI_BUILD})
	target_compile_definitions( "ProjectTemplate" PUBLIC "AE_CI_BUILD"  )
endif()

if (${AE_ENABLE_MEMLEAK_CHECKS})
	target_compile_definitions( "ProjectTemplate" PUBLIC "AE_ENABLE_MEMLEAK_CHECKS" )
endif()

if (NOT ${AE_STD_FILESYSTEM_SUPPORTED})
	if (${AE_ENABLE_FILESYSTEM})
		target_link_libraries( "ProjectTemplate" PUBLIC "FileSystem-lib" )
		message( STATUS "used alternative std::filesystem implementation" )
	else()
		message( FATAL_ERROR "requires 'std::filesystem' or fallback implementation" )
	endif()
endif()

target_compile_definitions( "ProjectTemplate" PUBLIC
	"AE_ENGINE_NAME=\"AsEn\""
	"AE_VERSION=AE::Base::Tuple{ ${PROJECT_VERSION_MAJOR}, ${PROJECT_VERSION_MINOR}, ${PROJECT_VERSION_PATCH}, \"${AE_COMMIT_HASH}\" }"
)

target_include_directories( "ProjectTemplate" PUBLIC "${MAIN_SOURCE_DIR}/engine" )
