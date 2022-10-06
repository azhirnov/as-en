// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// Ray Tracing Geometry Options
	//
	enum class ERTGeometryOpt : uint
	{
		Opaque						= 1 << 0,	// indicates that this geometry does not invoke the any-hit shaders even if present in a hit group
		NoDuplicateAnyHitInvocation	= 1 << 1,
		_Last,
		Unknown						= 0,
	};
	AE_BIT_OPERATORS( ERTGeometryOpt );
	

	
	//
	// Ray Tracing Instance Options
	//
	enum class ERTInstanceOpt : ubyte
	{
		TriangleCullDisable		= 1 << 0,
		TriangleFrontCCW		= 1 << 1,
		ForceOpaque				= 1 << 2,	// enable  ERTGeometryOpt::Opaque flag
		ForceNonOpaque			= 1 << 3,	// disable ERTGeometryOpt::Opaque flag
		_Last,
		Unknown					= 0,
	};
	AE_BIT_OPERATORS( ERTInstanceOpt );



	//
	// Ray Tracing Acceleration Structure Options
	//
	enum class ERTASOptions : uint
	{
		AllowUpdate				= 1 << 0,
		AllowCompaction			= 1 << 1,
		PreferFastTrace			= 1 << 2,
		PreferFastBuild			= 1 << 3,
		LowMemory				= 1 << 4,
		//MotionNV
		_Last,
		Unknown					= 0,
	};
	AE_BIT_OPERATORS( ERTASOptions );



	//
	// Ray Tracing Acceleration Structure Copy Mode
	//
	enum class ERTASCopyMode : uint
	{
		Clone		= 0,
		Compation	= 1,
		Unknown		= Clone
	};


}	// AE::Graphics
