
#----------------------------------------------------------
# PCH

function( EnablePCH2 projName headerName )
	if (${AE_USE_PCH})
		if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${headerName}.pch.h" )
			message( FATAL_ERROR "PCH header '${CMAKE_CURRENT_SOURCE_DIR}/${headerName}.pch.h' is not exists" )
		endif()

		source_group( "_pch_" FILES
			"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${projName}.dir/cmake_pch.cxx"
			"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${projName}.dir/Release/cmake_pch.hxx"
			"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${projName}.dir/Profile/cmake_pch.hxx"
			"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${projName}.dir/Develop/cmake_pch.hxx"
			"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${projName}.dir/Debug/cmake_pch.hxx"
		)
		target_precompile_headers( ${projName} PRIVATE "${headerName}.pch.h" )
	endif()
endfunction()

function( EnablePCH projName )
	EnablePCH2( ${projName} ${projName} )
endfunction()

#----------------------------------------------------------

function( EnablePrebuild projName )
if (DEFINED ENGINE_LIBS_PATH)
	if (ANDROID)
		set( LIB_PATH "${ENGINE_LIBS_PATH}/${ANDROID_ABI}/lib${projName}.a" )
		if (NOT EXISTS "${LIB_PATH}")
			message( FATAL_ERROR "Lib '${LIB_PATH}' is not exists" )
		endif()
		target_link_libraries( "${projName}" PUBLIC "${LIB_PATH}" )
	else()
		foreach( CFG CMAKE_CONFIGURATION_TYPES )
			set( LIB_PATH "${ENGINE_LIBS_PATH}/${CFG}/${CMAKE_STATIC_LIBRARY_PREFIX}${projName}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
			if (NOT EXISTS "${LIB_PATH}")
				message( FATAL_ERROR "Lib '${LIB_PATH}' is not exists" )
			endif()
		endforeach()
		target_link_libraries( "${projName}" PUBLIC
			$<$<CONFIG:Release>: "${ENGINE_LIBS_PATH}/Release/${CMAKE_STATIC_LIBRARY_PREFIX}${projName}${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Profile>: "${ENGINE_LIBS_PATH}/Profile/${CMAKE_STATIC_LIBRARY_PREFIX}${projName}${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Develop>: "${ENGINE_LIBS_PATH}/Develop/${CMAKE_STATIC_LIBRARY_PREFIX}${projName}${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Debug>:   "${ENGINE_LIBS_PATH}/Debug/${CMAKE_STATIC_LIBRARY_PREFIX}${projName}${CMAKE_STATIC_LIBRARY_SUFFIX}" > )
	endif()
endif()
endfunction()

#----------------------------------------------------------

function( EnableUnitBuild projName )
if (${AE_USE_UNITY_BUILD})
    set_target_properties( "${projName}" PROPERTIES
		UNITY_BUILD				ON
		UNITY_BUILD_MODE		BATCH	# BATCH or GROUP, default is BATCH
		UNITY_BUILD_BATCH_SIZE	8		# defult 8
	)
endif()
endfunction()
