set( DATA_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/res_pack/shared_data" )
file( GLOB_RECURSE SOURCES "*.h" "*.cpp" "*.as" "*.glsl" )
file( GLOB_RECURSE SHARED_DATA "${DATA_FOLDER}/*.*" )

if (DEFINED ANDROID)
	add_library( "Tests.Graphics" STATIC ${SOURCES} ${SHARED_DATA} )
elseif (APPLE)
	add_executable( "Tests.Graphics" MACOSX_BUNDLE ${SOURCES} ${SHARED_DATA} )
else()
	add_executable( "Tests.Graphics" ${SOURCES} ${SHARED_DATA} )
endif()
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
source_group( TREE ${DATA_FOLDER} FILES ${SHARED_DATA} )

set_property( TARGET "Tests.Graphics" PROPERTY FOLDER "Engine/Tests" )
target_link_libraries( "Tests.Graphics" PUBLIC "Graphics" "Platform" "GraphicsTestUtils" )

target_include_directories( "Tests.Graphics" PUBLIC "." )

target_compile_definitions( "Tests.Graphics" PUBLIC
	AE_TEST_GRAPHICS
	AE_VULKAN_REF_IMG_PATH="${AE_EXTERNAL_REP_PATH}/tests/graphics/vulkan"
	AE_PIPELINE_COMPILER_LIBRARY="${AE_RESPACK_BINARY_DIR}"
	AE_SHARED_DATA="${DATA_FOLDER}"
	AE_CURRENT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

if (TARGET "ResourcePacker-lib")
	target_link_libraries( "Tests.Graphics" PUBLIC "ResourcePacker-lib" )
endif ()

if (TARGET "VulkanSyncLog")
	target_link_libraries( "Tests.Graphics" PUBLIC "VulkanSyncLog" )
endif ()

add_test( NAME "Tests.Graphics" COMMAND "Tests.Graphics" )
