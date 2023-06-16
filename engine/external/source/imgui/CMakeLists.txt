# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download ImGUI (MIT license)

set( IMGUI_PATH "${AE_EXTERNAL_SHARED_PATH}/imgui" )

add_library( "ImGUI-lib" INTERFACE )
target_include_directories( "ImGUI-lib" INTERFACE "${IMGUI_PATH}/include" )
target_compile_definitions( "ImGUI-lib" INTERFACE "AE_ENABLE_IMGUI" "AE_LICENSE_MIT" )
install( FILES "${IMGUI_PATH}/LICENSE" DESTINATION "3party_license" RENAME "IMGUI-LICENSE" )
