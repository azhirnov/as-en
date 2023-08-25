file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
    file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "Threading" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "Threading" PROPERTY FOLDER "Engine" )
target_link_libraries( "Threading" PUBLIC "Base" )

EnablePCH( "Threading" )
EnablePrebuild( "Threading" )

install( TARGETS "Threading" ARCHIVE DESTINATION "lib" )
