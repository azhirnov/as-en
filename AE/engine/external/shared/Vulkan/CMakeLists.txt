# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download Vulkan headers (Apache 2.0 license)

set( VULKAN_PATH "${AE_EXTERNAL_SHARED_PATH}/Vulkan" )

add_library( "Vulkan-lib" INTERFACE )
target_include_directories( "Vulkan-lib" INTERFACE "${VULKAN_PATH}" )
target_compile_definitions( "Vulkan-lib" INTERFACE "AE_LICENSE_APACHE_2" )
install( FILES "${VULKAN_PATH}/LICENSE.txt" DESTINATION "3party_license" RENAME "VulkanHeader-LICENSE" )
#------------------------------------------------

if (ANDROID)
	add_custom_target( "Android-Vulkan-copy-so"
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${AE_DST_SHARED_PATH}/Vulkan/${ANDROID_ABI}" "${AE_ANDROID_JNILIB_PATH}/${ANDROID_ABI}"
		COMMENT "Copying Android Vulkan shared libs..." )
	set_property( TARGET "Android-Vulkan-copy-so" PROPERTY FOLDER "Engine/External" )
	add_dependencies( "Vulkan-lib" "Android-Vulkan-copy-so" )
endif()
#------------------------------------------------

if (${AE_ENABLE_EXTERNAL_TESTS})
	if (ANDROID)
		add_library( "Tests.Vulkan" STATIC "tests/main.cpp" )
		target_compile_definitions( "Tests.Vulkan" PUBLIC "ANDROID" )
		target_include_directories( "Tests.Vulkan" PRIVATE "${AE_TEST_DIR}" )
	else ()
		add_executable( "Tests.Vulkan" "tests/main.cpp" )
		#add_test( NAME "Tests.Vulkan" COMMAND "Tests.Vulkan" )
	endif ()
	set_property( TARGET "Tests.Vulkan" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.Vulkan" PUBLIC "Vulkan-lib" )
endif ()
