# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download xxHash (BSD-2 license)

set( XXHASH_PATH "${AE_EXTERNAL_SHARED_PATH}/xxHash" )

add_library( "xxHash-lib" INTERFACE )
target_include_directories( "xxHash-lib" INTERFACE "${XXHASH_PATH}" )
target_compile_definitions( "xxHash-lib" INTERFACE "AE_ENABLE_XXHASH" "AE_LICENSE_BSD_2" "XXH_INLINE_ALL" )
install( FILES "${XXHASH_PATH}/LICENSE.txt" DESTINATION "3party_license" RENAME "xxHash-LICENSE.txt" )
#------------------------------------------------

if (${AE_ENABLE_EXTERNAL_TESTS})
	add_executable( "Tests.xxHash" "tests/main.cpp" )
	#add_test( NAME "Tests.xxHash" COMMAND "Tests.xxHash" )
	set_property( TARGET "Tests.xxHash" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.xxHash" PUBLIC "xxHash-lib" )
endif ()
