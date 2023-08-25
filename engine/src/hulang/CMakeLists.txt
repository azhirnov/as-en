file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
    file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "HuLang" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "HuLang" PROPERTY FOLDER "Engine" )
target_include_directories( "HuLang" PUBLIC ".." )
target_link_libraries( "HuLang" PUBLIC "Base" "Threading" )

EnablePCH( "HuLang" )
EnablePrebuild( "HuLang" )

install( TARGETS "HuLang" ARCHIVE DESTINATION "lib" )
