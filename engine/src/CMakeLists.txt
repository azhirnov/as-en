# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
#  Base    Threading    Serializing    Scripting    Graphics    GraphicsHL    Networking    VFS    Platform    ECS    Profiler    Video    Audio
#    o-->>-----o                                                                                                                                 |
#    o-->>----------->>------o                                                                                                                   |
#    o-->>----------->>------------->>-----o                                                                                                     |
#    o-->>-----o----->>------o------>>----------->>----o----->>------o                                                                           |
#    o-->>-----o----->>------------->>----------->>---------->>------------>>-----o                                                              |
#    o-->>-----o----->>------------->>----------->>---------->>------------>>-----o------>>--o                                                   |
#    o-->>-----o----->>------------->>----------->>----o----->>------------>>-----o------>>--o-->>----o                                          |
#    o-->>-----o----->>------o------>>----------->>---------->>------------>>------------>>----->>---------->>--o                                |
#    o-->>-----o----->>------------->>----------->>----o----->>------------>>-----o------>>----->>---------->>----->>----o                       |
#    o-->>-----o----->>------o------>>----------->>----o----->>------------>>------------>>----->>---------->>----->>---------->>---o            |
#    o-->>----------->>------------->>----------->>---------->>------------>>------------>>----->>---------->>----->>---------->>------->>---o   |

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

add_subdirectory( "base" )
add_subdirectory( "scripting" )
add_subdirectory( "threading" )
add_subdirectory( "networking" )
add_subdirectory( "serializing" )
add_subdirectory( "graphics" )
add_subdirectory( "graphics_hl" )
add_subdirectory( "platform" )
add_subdirectory( "vfs" )
add_subdirectory( "ecs-st" )
add_subdirectory( "profiler" )
add_subdirectory( "video" )
add_subdirectory( "hulang" )
