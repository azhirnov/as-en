# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download astc-encoder (Apache-2.0)

set( ASTC_ENCODER_PATH "${AE_DST_PATH}/AstcEncoder" )

if (NOT EXISTS "${ASTC_ENCODER_PATH}/lib")
	message( FATAL_ERROR "'${ASTC_ENCODER_PATH}/lib' path is not found" )
endif()
#------------------------------------------------

add_library( "AstcEncoder-lib" INTERFACE )
target_include_directories( "AstcEncoder-lib" INTERFACE "${ASTC_ENCODER_PATH}/include" )
target_compile_definitions( "AstcEncoder-lib" INTERFACE "AE_ENABLE_ASTC_ENCODER" "AE_LICENSE_APACHE_2" )
install( FILES "${ASTC_ENCODER_PATH}/LICENSE.txt" DESTINATION "3party_license" RENAME "ASTC_ENCODER-LICENSE.txt" )

if (AE_CPU_ARCH_X64 OR AE_CPU_ARCH_X86)
	if (${AE_SIMD_AVX} GREATER_EQUAL 2)
		set_property( TARGET "AstcEncoder-lib" PROPERTY INTERFACE_LINK_LIBRARIES
			$<$<CONFIG:Release>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-avx2-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Profile>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-avx2-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Develop>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-avx2-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Debug>:   "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-avx2-staticd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		)
			
	elseif ( (${AE_SIMD_AVX} GREATER 0) OR (${AE_SIMD_SSE} GREATER_EQUAL 41) )
		set_property( TARGET "AstcEncoder-lib" PROPERTY INTERFACE_LINK_LIBRARIES
			$<$<CONFIG:Release>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-sse4.1-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Profile>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-sse4.1-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Develop>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-sse4.1-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Debug>:   "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-sse4.1-staticd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		)
			
	elseif (${AE_SIMD_SSE} GREATER_EQUAL 20)
		set_property( TARGET "AstcEncoder-lib" PROPERTY INTERFACE_LINK_LIBRARIES
			$<$<CONFIG:Release>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-sse2-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Profile>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-sse2-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Develop>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-sse2-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Debug>:   "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-sse2-staticd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		)
			
	else()
		set_property( TARGET "AstcEncoder-lib" PROPERTY INTERFACE_LINK_LIBRARIES
			$<$<CONFIG:Release>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-none-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Profile>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-none-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Develop>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-none-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Debug>:   "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-none-staticd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		)
	endif()

elseif (AE_CPU_ARCH_ARM64)
	set_property( TARGET "AstcEncoder-lib" PROPERTY INTERFACE_LINK_LIBRARIES
		$<$<CONFIG:Release>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-neon-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		$<$<CONFIG:Profile>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-neon-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		$<$<CONFIG:Develop>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-neon-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		$<$<CONFIG:Debug>:   "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-neon-staticd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	)

else()
	set_property( TARGET "AstcEncoder-lib" PROPERTY INTERFACE_LINK_LIBRARIES
		$<$<CONFIG:Release>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-none-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		$<$<CONFIG:Profile>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-none-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		$<$<CONFIG:Develop>: "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-none-static${CMAKE_STATIC_LIBRARY_SUFFIX}" >
		$<$<CONFIG:Debug>:   "${ASTC_ENCODER_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}astcenc-none-staticd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	)
endif()
#------------------------------------------------
					
if (${AE_ENABLE_EXTERNAL_TESTS})
	add_executable( "Tests.AstcEncoder" "tests/main.cpp" )
	#add_test( NAME "Tests.AstcEncoder" COMMAND "Tests.AstcEncoder" )
	set_property( TARGET "Tests.AstcEncoder" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.AstcEncoder" PUBLIC "AstcEncoder-lib" )
endif ()
