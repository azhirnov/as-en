# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# fameta-counter source code (unlicense)

add_library( "FametaCounter-lib" INTERFACE )

target_compile_definitions( "FametaCounter-lib" INTERFACE "AE_ENABLE_FAMETA_COUNTER" "AE_LICENSE_UNLICENSE" )

install( FILES "${FREETYPE_PATH}/LICENSE" DESTINATION "3party_license" RENAME "FametaCounter-LICENSE" )
