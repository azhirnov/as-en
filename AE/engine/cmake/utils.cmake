
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

function( EnableUnityBuild projName )
if (${AE_USE_UNITY_BUILD})
	set_target_properties( "${projName}" PROPERTIES
		UNITY_BUILD				ON
		UNITY_BUILD_MODE		BATCH	# BATCH or GROUP, default is BATCH
		UNITY_BUILD_BATCH_SIZE	8		# defult 8
	)
endif()
endfunction()
