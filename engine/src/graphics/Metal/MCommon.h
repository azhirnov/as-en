// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
	https://developer.apple.com/metal/Metal-Shading-Language-Specification.pdf
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "base/Algorithms/Cast.h"
# include "base/Containers/Ptr.h"
# include "base/Containers/FixedArray.h"
# include "base/Containers/FixedString.h"
# include "base/Containers/FixedTupleArray.h"
# include "base/Containers/StructView.h"
# include "base/Utils/Helpers.h"

# include "threading/Primitives/DataRaceCheck.h"
# include "threading/Primitives/Atomic.h"
# include "threading/TaskSystem/TaskScheduler.h"
# include "threading/TaskSystem/Promise.h"
# include "threading/Memory/GlobalLinearAllocator.h"
# include "threading/Memory/FrameAllocator.h"

# include "graphics/Public/Common.h"
# include "graphics/Public/IDs.h"
# include "graphics/Public/FrameUID.h"
# include "graphics/Public/MetalTypes.h"

// Xcode frame debugger doesn't support sample counters
# define AE_METAL_NATIVE_DEBUGGER	0


namespace AE::Graphics
{
	using AE::Threading::SpinLock;
	using AE::Threading::RWSpinLock;
	using AE::Threading::AsyncTask;
	using AE::Threading::GlobalLinearAllocatorRef;
	using AE::Threading::GraphicsFrameAllocatorRef;
	using AE::Threading::BitAtomic;
	using AE::Threading::FAtomic;
	using AE::Threading::ETaskQueue;
	
#	if AE_ENABLE_DATA_RACE_CHECK
	using AE::Threading::RWDataRaceCheck;
#	endif

	class MDevice;
	class MCommandBuffer;
	class MResourceManager;
	class MCommandBatch;
	class MRenderPass;
	struct MPixFormatInfo;
	class MRenderTaskScheduler;
	
	DEBUG_ONLY(
		using DebugName_t = FixedString<64>;
	)

	// Used for temporary allocations.
	// thread-safe: no
	using MTempLinearAllocator	= LinearAllocator< UntypedAllocator, 8, false >;	// TODO: use fast block allocator
	using MTempStackAllocator	= StackAllocator<  UntypedAllocator, 8, false >;
	
													// in MSL:
	enum class MTextureIndex		: ubyte {};		// [[ texture(x) ]]
	enum class MBufferIndex			: ubyte {};		// [[ buffer(x) ]]
	enum class MSamplerIndex		: ubyte {};		// [[ sampler(x) ]]
	enum class MThreadgroupIndex	: ubyte {};		// [[ threadgroup(x) ]]

	
	//
	// MTLBarrierScope
	//
	enum class MtlBarrierScope : ubyte
	{
		Buffers			= 1 << 0,
		Textures		= 1 << 1,
		RenderTargets	= 1 << 2,
		_Last,
		All				= ((_Last - 1) << 1) - 1,
		Unknown			= 0,
	};
	AE_BIT_OPERATORS( MtlBarrierScope );
	

	//
	// MTLRenderStages
	//
	enum class MtlRenderStages  : ubyte
	{
		Vertex				= 1 << 0,
		Fragment			= 1 << 1,
		Tile				= 1 << 2,
		MeshTask			= 1 << 3,
		Mesh				= 1 << 4,
		_Last,
		All					= ((_Last - 1) << 1) - 1,
		PreRasterization	= Vertex | MeshTask | Mesh,
		PostRasterization	= Fragment | Tile,
		Unknown				= 0,
	};
	AE_BIT_OPERATORS( MtlRenderStages );


	//
	// Metal Config
	//
	struct MConfig final : Noninstanceable
	{
		static constexpr uint	MaxQueues			= 4;
		static constexpr uint	VertexBufferOffset	= 31 - GraphicsConfig::MaxVertexBuffers;
	};
	

	//
	// MTLViewport
	//
	struct MViewport
	{
		double	originX		= 0.0;
		double	originY		= 0.0;
		double	width		= 0.0;
		double	height		= 0.0;
		double	znear		= 0.0;
		double	zfar		= 1.0;

		MViewport ()	__NE___	{}
	};


	//
	// MTLScissorRect
	//
	struct MScissorRect
	{
		NS::UInteger	x		= 0;
		NS::UInteger	y		= 0;
		NS::UInteger	height	= 0;
		NS::UInteger	width	= 0;

		MScissorRect ()	__NE___	{}
	};


	//
	// MTLClearColor
	//
	struct MClearColor
	{
		float	red		= 0.0f;
		float	green	= 0.0f;
		float	blue	= 0.0f;
		float	alpha	= 0.0f;

		MClearColor ()										__NE___	{}
		MClearColor (float r, float g, float b, float a)	__NE___	: red{r}, green{g}, blue{b}, alpha{a} {}
		MClearColor (const RGBA32f &col)					__NE___	: MClearColor{col.r, col.g, col.b, col.a} {}
		MClearColor (const RGBA8u &col)						__NE___	: MClearColor{RGBA32f{col}} {}
	};

	
	//
	// Dependency Info
	//
	struct MDependencyInfo
	{
		MtlBarrierScope		scope			= Default;
		MtlRenderStages		beforeStages	= Default;
		MtlRenderStages		afterStages		= Default;

		//Array<MetalResource>	resources;	// TODO
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL


# ifdef AE_CPP_DETECT_MISMATCH

#  ifdef AE_ENABLE_METAL
#	pragma detect_mismatch( "AE_ENABLE_METAL", "1" )
#  else
#	pragma detect_mismatch( "AE_ENABLE_METAL", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH
