set( DATA_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/res_pack/shared_data" )
file( GLOB_RECURSE SOURCES "*.h" "*.cpp" "*.as" "*.glsl" )
file( GLOB_RECURSE SHARED_DATA "${DATA_FOLDER}/*.*" )

if (DEFINED ANDROID)
	add_library( "Tests.GraphicsHL" STATIC ${SOURCES} ${SHARED_DATA} )
elseif (APPLE)
	add_executable( "Tests.GraphicsHL" MACOSX_BUNDLE ${SOURCES} ${SHARED_DATA} )
else()
	add_executable( "Tests.GraphicsHL" ${SOURCES} ${SHARED_DATA} )
endif()
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
source_group( TREE ${DATA_FOLDER} FILES ${SHARED_DATA} )

set_property( TARGET "Tests.GraphicsHL" PROPERTY FOLDER "Engine/Tests" )
target_link_libraries( "Tests.GraphicsHL" PUBLIC "Platform" "GraphicsHL" "GraphicsTestUtils" )
target_compile_definitions( "Tests.GraphicsHL" PUBLIC AE_TEST_GRAPHICS_HL )

target_compile_definitions( "Tests.GraphicsHL" PUBLIC
	AE_TEST_GRAPHICS
	AE_PIPELINE_COMPILER_LIBRARY="${MAIN_BINARY_DIR}"
	AE_SHARED_DATA="${DATA_FOLDER}"
	AE_CURRENT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
	AE_CANVAS_VERTS="${CMAKE_CURRENT_SOURCE_DIR}/../../src/graphics_hl/Canvas/VertexTypes.as"
)

add_dependencies( "Tests.GraphicsHL" "PipelineCompiler-shared" )

if (TARGET "VulkanSyncLog")
	target_link_libraries( "Tests.GraphicsHL" PUBLIC "VulkanSyncLog" )
endif ()

if (${AE_ENABLE_VULKAN})
	target_compile_definitions( "Tests.GraphicsHL" PUBLIC AE_REF_IMG_PATH="${AE_EXTERNAL_REP_PATH}/tests/graphics_hl/vulkan" )
elseif (${AE_ENABLE_METAL})
	target_compile_definitions( "Tests.GraphicsHL" PUBLIC AE_REF_IMG_PATH="${AE_EXTERNAL_REP_PATH}/tests/graphics_hl/metal" )
endif()

add_test( NAME "Tests.GraphicsHL" COMMAND "Tests.GraphicsHL" )
