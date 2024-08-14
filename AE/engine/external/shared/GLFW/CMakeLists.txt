# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download GLFW (ZLib license)

set( GLFW_PATH "${AE_DST_PATH}/GLFW" )

if (NOT EXISTS "${GLFW_PATH}/lib")
	message( FATAL_ERROR "'${GLFW_PATH}/lib' path is not found" )
endif()
#------------------------------------------------

add_library( "GLFW-lib" INTERFACE )
target_include_directories( "GLFW-lib" INTERFACE "${GLFW_PATH}/include" )
target_compile_definitions( "GLFW-lib" INTERFACE "AE_ENABLE_GLFW" "AE_LICENSE_ZLIB" )
install( FILES "${GLFW_PATH}/LICENSE.md" DESTINATION "3party_license" RENAME "GLFW-LICENSE.md" )

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set_property( TARGET "GLFW-lib" PROPERTY INTERFACE_LINK_LIBRARIES
		"${GLFW_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX}"
		"-framework Cocoa"
		"-framework IOKit"
		"-framework CoreFoundation"
		"-framework CoreVideo" )

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set_property( TARGET "GLFW-lib" PROPERTY INTERFACE_LINK_LIBRARIES
		"${GLFW_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX}"
		"dl" "m" "X11" )

else()
	set_property( TARGET "GLFW-lib" PROPERTY INTERFACE_LINK_LIBRARIES
		"${GLFW_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX}" )
endif()
#------------------------------------------------

if (${AE_ENABLE_EXTERNAL_TESTS})
	add_executable( "Tests.GLFW" "tests/main.cpp" )
	set_property( TARGET "Tests.GLFW" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.GLFW" PUBLIC "GLFW-lib" )
	#add_test( NAME "Tests.GLFW" COMMAND "Tests.GLFW" )
endif ()
