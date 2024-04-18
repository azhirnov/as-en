
#----------------------------------------------------------
# PCH

function( EnablePCH2 projName headerName )
	if (${AE_USE_PCH})
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
	if (DEFINED ANDROID)
		target_link_libraries( "${projName}" PUBLIC "${ENGINE_LIBS_PATH}/${ANDROID_ABI}/lib${projName}.a" )
	else()
		target_link_libraries( "${projName}" PUBLIC
			$<$<CONFIG:Release>: "${ENGINE_LIBS_PATH}/Release/${CMAKE_STATIC_LIBRARY_PREFIX}${projName}${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Profile>: "${ENGINE_LIBS_PATH}/Profile/${CMAKE_STATIC_LIBRARY_PREFIX}${projName}${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Develop>: "${ENGINE_LIBS_PATH}/Develop/${CMAKE_STATIC_LIBRARY_PREFIX}${projName}${CMAKE_STATIC_LIBRARY_SUFFIX}" >
			$<$<CONFIG:Debug>:   "${ENGINE_LIBS_PATH}/Debug/${CMAKE_STATIC_LIBRARY_PREFIX}${projName}${CMAKE_STATIC_LIBRARY_SUFFIX}" > )
	endif()
endif()
endfunction()
