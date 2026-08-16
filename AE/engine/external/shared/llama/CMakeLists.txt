# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#
# llama-cpp (MIT license)

set( LLAMA_PATH "${AE_DST_SHARED_PATH}/llama" )

if (NOT EXISTS "${LLAMA_PATH}/lib")
	message( FATAL_ERROR "'${LLAMA_PATH}/lib' path is not found" )
endif()
#------------------------------------------------

add_library( "llama-lib" INTERFACE )
target_include_directories( "llama-lib" INTERFACE "${LLAMA_PATH}/include" )
target_compile_definitions( "llama-lib" INTERFACE "AE_ENABLE_LLAMA" "AE_LICENSE_MIT" )	# don't add LLAMA_SHARED for dynamic loading

add_custom_target( "llama-copy-dll"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${LLAMA_PATH}/lib" "${MAIN_BINARY_DIR}${AE_RUNTIME_CONFIG}"
	COMMENT "Copying llama DLL..." )

set_property( TARGET "llama-copy-dll" PROPERTY FOLDER "Engine/External" )
add_dependencies( "llama-lib" "llama-copy-dll" )

install( FILES "${LLAMA_PATH}/llama-LICENSE.txt" DESTINATION "3party_license" RENAME "llama-LICENSE" )
#------------------------------------------------

#[[
if (${AE_ENABLE_EXTERNAL_TESTS})
	add_executable( "Tests.llama" "tests/main.cpp" )
	set_property( TARGET "Tests.llama" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.llama" PUBLIC "llama-lib" )
	#add_test( NAME "Tests.llama" COMMAND "Tests.llama" )
	set_target_properties( "Tests.llama" PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES )
endif ()
]]
