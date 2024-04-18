# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# Abseil (Apache 2.0)

set( ABSEIL_PATH "${AE_DST_PATH}/Abseil" )

add_library( "Abseil-lib" INTERFACE )
target_include_directories( "Abseil-lib" INTERFACE "${ABSEIL_PATH}" )
target_compile_definitions( "Abseil-lib" INTERFACE "AE_ENABLE_ABSEIL" "AE_LICENSE_APACHE_2" )
install( FILES "${ABSEIL_PATH}/LICENSE" DESTINATION "3party_license" RENAME "Abseil-LICENSE" )

if (DEFINED ANDROID)
	set_property( TARGET "Abseil-lib" PROPERTY INTERFACE_LINK_LIBRARIES
		"${ABSEIL_PATH}/lib/${ANDROID_ABI}/libAbseil.a" )
else()
	set_property( TARGET "Abseil-lib" PROPERTY INTERFACE_LINK_LIBRARIES
		$<$<CONFIG:Release>: "${ABSEIL_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}Abseil${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		$<$<CONFIG:Profile>: "${ABSEIL_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}Abseil${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		$<$<CONFIG:Develop>: "${ABSEIL_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}Abseil${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		$<$<CONFIG:Debug>:   "${ABSEIL_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}Abseild${CMAKE_STATIC_LIBRARY_SUFFIX}" > )
endif()

if (${AE_ENABLE_EXTERNAL_TESTS})
	if (DEFINED ANDROID)
		add_library( "Tests.Abseil" STATIC "tests/main.cpp" )
		target_compile_definitions( "Tests.Abseil" PUBLIC "ANDROID" )
		target_include_directories( "Tests.Abseil" PRIVATE "${AE_TEST_DIR}" )
	else()
		add_executable( "Tests.Abseil" "tests/main.cpp" )
		#add_test( NAME "Tests.Abseil" COMMAND "Tests.Abseil" )
	endif()
	set_target_properties( "Tests.Abseil" PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES )
	target_compile_features( "Tests.Abseil" PUBLIC cxx_std_17 )
	set_property( TARGET "Tests.Abseil" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.Abseil" PUBLIC "Abseil-lib" )
endif ()
