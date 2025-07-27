// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_rhi/Public/Common.h"

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
		_BITOPS_,
		Unknown						= 0
	};



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



	//
	// Ray Tracing Acceleration Structure Options
	//
	enum class ERTASOptions : ubyte
	{
		AllowUpdate					= 1 << 0,
		AllowCompaction				= 1 << 1,
		PreferFastTrace				= 1 << 2,
		PreferFastBuild				= 1 << 3,
		LowMemory					= 1 << 4,
		AllowDataAccess				= 1 << 5,
		//MotionNV
		_Last,
		All							= ((_Last-1) << 1) - 1,
		Unknown						= 0,
	};



	//
	// Ray Tracing Acceleration Structure Copy Mode
	//
	enum class ERTASCopyMode : ubyte
	{
		Clone,
		Compaction,
		_Count,
	};



	//
	// Ray Tracing Acceleration Structure Property
	//
	enum class ERTASProperty : ubyte
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
//-----------------------------------------------------------------------------



	//
	// Ray Tracing Partitioned Scene Instance Options
	//
	enum class EPartitionedInstanceOpt : uint
	{
		TriangleCullDisable			= 1 << 0,	// otherwise cull back
		TriangleFrontCCW			= 1 << 1,	// otherwise CW
		ForceOpaque					= 1 << 2,	// enable  ERTGeometryOpt::Opaque flag
		ForceNonOpaque				= 1 << 3,	// disable ERTGeometryOpt::Opaque flag
		EnableExplicitAABB			= 1 << 4,	// enables use of 'WriteInstanceVk::explicitAABB'
		_Last,
		All							= ((_Last - 1) << 1) - 1,
		Unknown						= 0,
		TriangleCullBack			= 0,		// default
		TriangleFrontCW				= 0,		// default
	};


	//
	// Ray Tracing Partitioned Scene Instance Indirect Command Type
	//
	enum class EPartitionedCmd : uint
	{
		WriteInstance				= 0,
		UpdateInstance				= 1,
		WritePartitionTranslation	= 2,
		_Count,
		Unknown						= ~0u,
	};
//-----------------------------------------------------------------------------



	//
	// Ray Tracing Cluster Type
	//
	enum class ERTClusterType : ubyte
	{
		Unknown		= 0,

		Geometry,			// replacement for RTGeometry (BLAS)
		TriangleCluster,
		TriangleTemplate,
	};


	//
	// Ray Tracing Cluster Op Type
	//
	enum class ERTClusterOpType : ubyte
	{
		Unknown		= 0,

		MoveObjects,
		BuildClustersGeometry,
		BuildTriangleCluster,
		BuildTriangleClusterTemplate,
		InstantiateTriangleCluster,
	};


	//
	// Ray Tracing Cluster Op Mode
	//
	enum class ERTClusterOpMode : ubyte
	{
		Unknown		= 0,

		ImplicitDst,	// indicates that the build or move operation will implicitly distribute built/moved structures
						// in the user specified buffer 'RTClusterBuild::dstImplicitData'.

		ExplicitDst,	// indicates that the build or move operation will explicitly write built/moved acceleration structures
						// to the addresses specified in user specified buffer 'RTClusterBuild::dstAddressesArray'.

		ComputeSizes,	// indicates that computed cluster acceleration structure’s sizes will be written
						// to user specified buffer 'RTClusterBuild::dstSizesArray'.
	};


	//
	// Ray Tracing Cluster Address Resolution
	//
	enum class ERTClusterAddressResolution : ubyte
	{
		Unknown				= 0,
		DstImplicitData		= 1 << 0,
		ScratchData			= 1 << 1,
		DstAddressArray		= 1 << 2,
		DstSizesArray		= 1 << 3,
		SrcInfosArray		= 1 << 4,
		SrcInfosCount		= 1 << 5,
		_Last,
		All					= ((_Last - 1) << 1) - 1,
	};


	//
	// Cluster Flags for Ray Tracing Cluster
	//
	enum class EClusterFlags : uint
	{
		Unknown							= 0,
		AllowDisableOpacityMicromaps	= 1 << 0,
		_BITOPS_
	};



	enum class EClusterIndexType : uint
	{
		UByte	= 1,	// same as in Vulkan
		UShort	= 2,
		UInt	= 4,
	};
//-----------------------------------------------------------------------------



} // AE::Graphics
