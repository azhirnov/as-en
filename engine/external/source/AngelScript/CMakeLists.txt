# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download AngelScript SDK (ZLib license)

set( ANGELSCRIPT_PATH "${AE_EXTERNAL_SHARED_PATH}/AngelScript" )

add_library( "AngelScript-lib" INTERFACE )
target_include_directories( "AngelScript-lib" INTERFACE "${ANGELSCRIPT_PATH}/include" )
target_compile_definitions( "AngelScript-lib" INTERFACE "AE_ENABLE_ANGELSCRIPT" "AS_USE_NAMESPACE=1" "AS_USE_STLNAMES=1" "AE_LICENSE_ZLIB" )
install( FILES "${ANGELSCRIPT_PATH}/LICENSE.txt" DESTINATION "3party_license" RENAME "AngelScript-LICENSE" )
