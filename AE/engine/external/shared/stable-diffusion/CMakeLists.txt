# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# stable-diffusion.cpp (MIT license)

set( STABLE_DIFFUSION_PATH "${AE_DST_SHARED_PATH}/stable-diffusion" )

if (NOT EXISTS "${STABLE_DIFFUSION_PATH}/lib")
	message( FATAL_ERROR "'${STABLE_DIFFUSION_PATH}/lib' path is not found" )
endif()
#------------------------------------------------

add_library( "StableDiffusion-lib" INTERFACE )
target_include_directories( "StableDiffusion-lib" INTERFACE "${STABLE_DIFFUSION_PATH}/include" )
target_compile_definitions( "StableDiffusion-lib" INTERFACE "AE_ENABLE_STABLE_DIFFUSION" "AE_LICENSE_MIT" )

add_custom_target( "StableDiffusion-copy-dll"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${STABLE_DIFFUSION_PATH}/lib" "${MAIN_BINARY_DIR}${AE_RUNTIME_CONFIG}"
	COMMENT "Copying StableDiffusion DLL..." )

set_property( TARGET "StableDiffusion-copy-dll" PROPERTY FOLDER "Engine/External" )
add_dependencies( "StableDiffusion-lib" "StableDiffusion-copy-dll" )

install( FILES "${STABLE_DIFFUSION_PATH}/stable-diffusion-LICENSE.txt" DESTINATION "3party_license" RENAME "stable-diffusion-LICENSE" )
#------------------------------------------------

#[[
if (${AE_ENABLE_EXTERNAL_TESTS})
	add_executable( "Tests.StableDiffusion" "tests/main.cpp" )
	set_property( TARGET "Tests.StableDiffusion" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.StableDiffusion" PUBLIC "StableDiffusion-lib" )
	#add_test( NAME "Tests.StableDiffusion" COMMAND "Tests.StableDiffusion" )
	set_target_properties( "Tests.StableDiffusion" PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES )
endif ()
]]
