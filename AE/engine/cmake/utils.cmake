
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
