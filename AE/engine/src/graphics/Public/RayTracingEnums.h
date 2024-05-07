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
		TriangleCullDisable			= 1 << 0,	// otherwise cull back
		TriangleFrontCCW			= 1 << 1,	// otherwise CW
		ForceOpaque					= 1 << 2,	// enable  ERTGeometryOpt::Opaque flag
		ForceNonOpaque				= 1 << 3,	// disable ERTGeometryOpt::Opaque flag
		_Last,
		All							= ((_Last - 1) << 1) - 1,
		Unknown						= 0,
		TriangleCullBack			= 0,		// default
		TriangleFrontCW				= 0,		// default
	};
	AE_BIT_OPERATORS( ERTInstanceOpt );



	//
	// Ray Tracing Acceleration Structure Options
	//
	enum class ERTASOptions : uint
	{
		AllowUpdate					= 1 << 0,
		AllowCompaction				= 1 << 1,
		PreferFastTrace				= 1 << 2,
		PreferFastBuild				= 1 << 3,
		LowMemory					= 1 << 4,
		AllowDataAccess				= 1 << 5,
		//MotionNV
		_Last,
		Unknown						= 0,
	};
	AE_BIT_OPERATORS( ERTASOptions );



	//
	// Ray Tracing Acceleration Structure Copy Mode
	//
	enum class ERTASCopyMode : uint
	{
		Clone,
		Compaction,
		_Count,
	};



	//
	// Ray Tracing Acceleration Structure Property
	//
	enum class ERTASProperty : uint
	{
		CompactedSize,
		SerializationSize,
		Size,								// require 'VK_KHR_ray_tracing_maintenance1'
	//	SerializationBottomLevelPointers,	// require 'VK_KHR_ray_tracing_maintenance1'
		_Count,
	};


	//
	// Ray Tracing Shader Group Type
	//
	enum class ERTShaderGroup : ubyte
	{
		General,
		ClosestHit,
		AnyHit,
		Intersection,

		_Count,
		Unknown	= 0xFF
	};


} // AE::Graphics
