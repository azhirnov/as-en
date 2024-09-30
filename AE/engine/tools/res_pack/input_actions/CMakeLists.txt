# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

set( PLATFORM_DIR "${MAIN_SOURCE_DIR}/engine/src/platform" )
set( PLATFORM_HPP_SRC
	"${PLATFORM_DIR}/Private/SerializableInputActions.h"
	"${PLATFORM_DIR}/GLFW/SerializableInputActionsGLFW.h"
	"${PLATFORM_DIR}/WinAPI/SerializableInputActionsWinAPI.h"
	"${PLATFORM_DIR}/Android/SerializableInputActionsAndroid.h"
	"${PLATFORM_DIR}/OpenVR/SerializableInputActionsOpenVR.h" )

set( PLATFORM_CPP_SRC
	"${PLATFORM_DIR}/Private/SerializableInputActions.cpp"
	"${PLATFORM_DIR}/GLFW/SerializableInputActionsGLFW.cpp"
	"${PLATFORM_DIR}/WinAPI/SerializableInputActionsWinAPI.cpp"
	"${PLATFORM_DIR}/Android/SerializableInputActionsAndroid.cpp"
	"${PLATFORM_DIR}/OpenVR/SerializableInputActionsOpenVR.cpp" )

file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
	file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "InputActionsBinding" STATIC ${SOURCES} ${PLATFORM_HPP_SRC} ${PLATFORM_CPP_SRC} ${PREBUILD_CPP_FILE} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
source_group( "external/platform" FILES ${PLATFORM_HPP_SRC} ${PLATFORM_CPP_SRC} )

target_link_libraries( "InputActionsBinding" PUBLIC "Serializing" "Scripting" )
target_compile_definitions( "InputActionsBinding" PRIVATE "AE_BUILD_INPUT_ACTIONS_BINDING" )
target_include_directories( "InputActionsBinding" PUBLIC "." )

EnablePCH( "InputActionsBinding" )
EnablePrebuild( "InputActionsBinding" )
EnableUnitBuild( "InputActionsBinding" )

add_library( "InputActionsBinding-shared" SHARED
	"InputActionsBinding.h"
	"Lib/Export.cpp"
	"Lib/InputActionsBinding.def"
)
target_link_libraries( "InputActionsBinding-shared" PUBLIC "InputActionsBinding" )

set_property( TARGET "InputActionsBinding" PROPERTY FOLDER "Engine/ResPack" )
set_property( TARGET "InputActionsBinding-shared" PROPERTY FOLDER "Engine/ResPack" )

# generated cpp file used as dependency to trigger project rebuild
set( InputActionsBinding.trigger "${CMAKE_CURRENT_SOURCE_DIR}/InputActionsBinding.trigger.cpp" CACHE INTERNAL "" FORCE )
add_custom_command( TARGET "InputActionsBinding-shared" POST_BUILD COMMAND ${CMAKE_COMMAND} -E touch "${InputActionsBinding.trigger}" )
set_source_files_properties( "${InputActionsBinding.trigger}" PROPERTIES GENERATED TRUE )
