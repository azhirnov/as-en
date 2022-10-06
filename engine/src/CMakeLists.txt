# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# GraphicsHL	<-- Graphics	<-- { Threading, Serializing, Scripting } <-- Base
# ECS			<-- Threading	<-- Base
# Platform		<-- Graphics	<-- ...

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

add_subdirectory( "base" )
add_subdirectory( "scripting" )
add_subdirectory( "threading" )
add_subdirectory( "networking" )
add_subdirectory( "serializing" )
add_subdirectory( "graphics" )
add_subdirectory( "graphics_hl" )
add_subdirectory( "platform" )
add_subdirectory( "ecs-st" )
