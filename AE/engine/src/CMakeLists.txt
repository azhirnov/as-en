# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
#  Base    Threading    Serializing    Scripting    GraphicsRHI    Graphics    Networking    VFS    Platform    ECS    Profiler    Video    Audio
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
add_subdirectory( "serializing" )

add_subdirectory( "networking" )
add_subdirectory( "graphics_rhi" )
add_subdirectory( "vfs" )
add_subdirectory( "ecs-st" )

add_subdirectory( "audio" )
add_subdirectory( "platform" )
add_subdirectory( "video" )

add_subdirectory( "graphics" )
add_subdirectory( "profiler" )
