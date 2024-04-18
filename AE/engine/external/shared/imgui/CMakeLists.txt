# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download ImGUI (MIT license)

set( IMGUI_PATH "${AE_DST_PATH}/imgui" )

add_library( "ImGUI-lib" INTERFACE )
target_include_directories( "ImGUI-lib" INTERFACE "${IMGUI_PATH}/include" )
target_compile_definitions( "ImGUI-lib" INTERFACE "AE_ENABLE_IMGUI" "AE_LICENSE_MIT" )
install( FILES "${IMGUI_PATH}/LICENSE" DESTINATION "3party_license" RENAME "IMGUI-LICENSE" )

if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
	set_property( TARGET "ImGUI-lib" PROPERTY INTERFACE_LINK_LIBRARIES
		"${IMGUI_PATH}/lib/${ANDROID_ABI}/${CMAKE_STATIC_LIBRARY_PREFIX}imgui${CMAKE_STATIC_LIBRARY_SUFFIX}" )

elseif (APPLE) # Darwin & iOS
	set_property( TARGET "ImGUI-lib" PROPERTY INTERFACE_LINK_LIBRARIES
		"${IMGUI_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}imgui${CMAKE_STATIC_LIBRARY_SUFFIX}"
		"-framework ApplicationServices" )

else()
	set_property( TARGET "ImGUI-lib" PROPERTY INTERFACE_LINK_LIBRARIES
		"${IMGUI_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}imgui${CMAKE_STATIC_LIBRARY_SUFFIX}" )
endif()

if (${AE_ENABLE_EXTERNAL_TESTS})
	if (DEFINED ANDROID)
		add_library( "Tests.ImGUI" STATIC "tests/main.cpp" )
		target_compile_definitions( "Tests.ImGUI" PUBLIC "ANDROID" )
		target_include_directories( "Tests.ImGUI" PRIVATE "${AE_TEST_DIR}" )
	else()
		add_executable( "Tests.ImGUI" "tests/main.cpp" )
		#add_test( NAME "Tests.ImGUI" COMMAND "Tests.ImGUI" )
	endif()
	set_property( TARGET "Tests.ImGUI" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.ImGUI" PUBLIC "ImGUI-lib" )
	set_target_properties( "Tests.ImGUI" PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES )
	target_compile_features( "Tests.ImGUI" PUBLIC cxx_std_17 )
endif ()
