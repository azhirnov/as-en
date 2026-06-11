
add_custom_target( RESCAN
    COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}
    COMMENT "Rescanning source folders and regenerating build system"
)

set_property( TARGET RESCAN PROPERTY FOLDER "CMakePredefinedTargets" )

