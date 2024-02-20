# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# MeshOptimizer (MIT license)

set( MESHOPT_PATH "${AE_DST_PATH}/MeshOptimizer" )

add_library( "MeshOptimizer-lib" INTERFACE )
target_include_directories( "MeshOptimizer-lib" INTERFACE "${MESHOPT_PATH}/include" )
target_compile_definitions( "MeshOptimizer-lib" INTERFACE "AE_ENABLE_MESH_OPTIMIZER" "AE_LICENSE_MIT" )
install( FILES "${MESHOPT_PATH}/LICENSE.md" DESTINATION "3party_license" RENAME "MeshOptimizer-LICENSE" )

set_property( TARGET "MeshOptimizer-lib" PROPERTY INTERFACE_LINK_LIBRARIES
    $<$<CONFIG:Release>: "${MESHOPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}meshoptimizer${CMAKE_STATIC_LIBRARY_SUFFIX}" >
    $<$<CONFIG:Profile>: "${MESHOPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}meshoptimizer${CMAKE_STATIC_LIBRARY_SUFFIX}" >
    $<$<CONFIG:Develop>: "${MESHOPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}meshoptimizer${CMAKE_STATIC_LIBRARY_SUFFIX}" >
    $<$<CONFIG:Debug>:   "${MESHOPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}meshoptimizerd${CMAKE_STATIC_LIBRARY_SUFFIX}" >)

if (${AE_ENABLE_EXTERNAL_TESTS})
    add_executable( "Tests.MeshOptimizer" "tests/main.cpp" )
    #add_test( NAME "Tests.MeshOptimizer" COMMAND "Tests.MeshOptimizer" )
    set_target_properties( "Tests.MeshOptimizer" PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES )
    target_compile_features( "Tests.MeshOptimizer" PUBLIC cxx_std_17 )
    set_property( TARGET "Tests.MeshOptimizer" PROPERTY FOLDER "Engine/External/Tests" )
    target_link_libraries( "Tests.MeshOptimizer" PUBLIC "MeshOptimizer-lib" )
endif ()
