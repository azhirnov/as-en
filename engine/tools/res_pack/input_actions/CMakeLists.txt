
set( PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/platform" )
set( PLATFORM_SRC
	"${PLATFORM_DIR}/Private/SerializableInputActions.h"
	#"${PLATFORM_DIR}/Private/SerializableInputActions.cpp"
	"${PLATFORM_DIR}/GLFW/SerializableInputActionsGLFW.h"
	#"${PLATFORM_DIR}/GLFW/SerializableInputActionsGLFW.cpp"
	"${PLATFORM_DIR}/WinAPI/SerializableInputActionsWinAPI.h"
	#"${PLATFORM_DIR}/WinAPI/SerializableInputActionsWinAPI.cpp"
	"${PLATFORM_DIR}/Android/SerializableInputActionsAndroid.h"
	#"${PLATFORM_DIR}/Android/SerializableInputActionsAndroid.cpp"
	"${PLATFORM_DIR}/OpenVR/SerializableInputActionsOpenVR.h"
	#"${PLATFORM_DIR}/OpenVR/SerializableInputActionsOpenVR.cpp"
)
file( GLOB_RECURSE SOURCES "*.*" )

add_library( "InputActionsBinding" STATIC ${SOURCES} ${PLATFORM_SRC} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
source_group( "external/platform" FILES ${PLATFORM_SRC} )

target_link_libraries( "InputActionsBinding" PUBLIC "Serializing" "Scripting" )
target_compile_definitions( "InputActionsBinding" PRIVATE "AE_BUILD_INPUT_ACTIONS_BINDING" )
target_include_directories( "InputActionsBinding" PUBLIC "." "${PLATFORM_SRC}/.." )

add_library( "InputActionsBinding-shared" SHARED
	"InputActionsBinding.h"
	#"Lib/main.cpp"
	#"Lib/InputActionsBinding.def"
)
target_link_libraries( "InputActionsBinding-shared" PUBLIC "InputActionsBinding" )

set_property( TARGET "InputActionsBinding" PROPERTY FOLDER "Engine/ResPack" )
set_property( TARGET "InputActionsBinding-shared" PROPERTY FOLDER "Engine/ResPack" )

# generated cpp file used as dependency to trigger project rebuild
set( InputActionsBinding.trigger "${CMAKE_CURRENT_SOURCE_DIR}/InputActionsBinding.trigger.cpp" CACHE INTERNAL "" FORCE )
add_custom_command( TARGET "InputActionsBinding-shared" POST_BUILD COMMAND ${CMAKE_COMMAND} -E touch "${InputActionsBinding.trigger}" )
