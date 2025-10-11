// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Can be used for intellisense or to compile HLSL/slang as C++ source (linking will always fail).
*/

#pragma once

// TODO

#include <../cpp/vk_types.h>


#ifdef __INTELLISENSE__
#	define SH_VERT
#	define SH_TESS_CTRL
#	define SH_TESS_EVAL
#	define SH_GEOM
#	define SH_FRAG
#	define SH_COMPUTE
#	define SH_TILE
#	define SH_MESH_TASK
#	define SH_MESH
#	define SH_RAY_GEN
#	define SH_RAY_AHIT
#	define SH_RAY_CHIT
#	define SH_RAY_MISS
#	define SH_RAY_INT
#	define SH_RAY_CALL
#	define SLANG_ENTRY_OUT	void
#	define SLANG_ENTRY_IN
#endif
