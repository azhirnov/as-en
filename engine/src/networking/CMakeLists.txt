file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
    file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "Networking" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "Networking" PROPERTY FOLDER "Engine" )
target_link_libraries( "Networking" PUBLIC "Threading" )
target_compile_definitions( "Networking" PUBLIC "AE_ENABLE_NETWORKING" )

EnablePCH( "Networking" )
EnablePrebuild( "Networking" )

install( TARGETS "Networking" ARCHIVE DESTINATION "lib" )
