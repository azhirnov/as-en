# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download stb (MIT license)

set( STB_PATH "${AE_EXTERNAL_SHARED_PATH}/stb" )

add_library( "STB-lib" INTERFACE )
target_include_directories( "STB-lib" INTERFACE "${STB_PATH}" )
target_compile_definitions( "STB-lib" INTERFACE "AE_ENABLE_STB" "AE_LICENSE_MIT" )
install( FILES "${STB_PATH}/LICENSE" DESTINATION "3party_license" RENAME "STB-LICENSE.md" )

if (${AE_ENABLE_EXTERNAL_TESTS})
	if (DEFINED ANDROID)
		add_library( "Tests.STB" STATIC "tests/main.cpp" )
		target_compile_definitions( "Tests.STB" PUBLIC "ANDROID" )
		target_include_directories( "Tests.STB" PRIVATE "${AE_TEST_DIR}" )
	else()
		add_executable( "Tests.STB" "tests/main.cpp" )
		#add_test( NAME "Tests.STB" COMMAND "Tests.STB" )
	endif()
	set_property( TARGET "Tests.STB" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.STB" PUBLIC "STB-lib" )
endif ()
