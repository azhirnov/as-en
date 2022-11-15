// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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

// beta extension in old versions
# define AE_METAL_3_0_BETA		1


namespace AE::Graphics
{
	using AE::Threading::Mutex;
	using AE::Threading::SharedMutex;
	using AE::Threading::SpinLock;
	using AE::Threading::RWSpinLock;
	using AE::Threading::RecursiveMutex;
	using AE::Threading::EMemoryOrder;
	using AE::Threading::ThreadFence;
	using AE::Threading::AsyncTask;
	using AE::Threading::GlobalLinearAllocatorRef;
	using AE::Threading::GraphicsFrameAllocatorRef;
	using AE::Threading::BitAtomic;
	using AE::Threading::FAtomic;
	using AE::Threading::EThread;
	
#	if AE_ENABLE_DATA_RACE_CHECK
	using AE::Threading::RWDataRaceCheck;
#	endif

	class MDevice;
	class MResourceManager;
	class MCommandBatch;
	class MRenderPass;
	struct MPixFormatInfo;
	
	DEBUG_ONLY(
		using DebugName_t = FixedString<64>;
	)

	// Used for temporary allocations.
	// thread-safe: no
	using MTempLinearAllocator	= LinearAllocator< UntypedAllocator, 8, false >;	// TODO: use fast block allocator
	using MTempStackAllocator	= StackAllocator< UntypedAllocator, 8, false >;
	

	enum class MTextureIndex		: ubyte {};		// [[ texture(x) ]]
	enum class MBufferIndex			: ubyte {};		// [[ buffer(x) ]]
	enum class MSamplerIndex		: ubyte {};		// [[ sampler(x) ]]
	enum class MThreadgroupIndex	: ubyte {};		// [[ threadgroup(x) ]]
	enum class MCounterIndex		: ubyte {};

	
	using MSamplerID			= HandleTmpl< 16, 16, Graphics::_hidden_::MetalIDs_Start + 1 >;
	using MPipelineLayoutID		= HandleTmpl< 16, 16, Graphics::_hidden_::MetalIDs_Start + 2 >;
	using MRenderPassID			= HandleTmpl< 16, 16, Graphics::_hidden_::MetalIDs_Start + 4 >;
	using MMemoryID				= HandleTmpl< 32, 32, Graphics::_hidden_::MetalIDs_Start + 6 >;

	
	//
	// MTLBarrierScope
	//
	enum class MtlBarrierScope : ubyte
	{
		Buffers			= 1 << 0,
		RenderTargets	= 1 << 1,
		Textures		= 1 << 2,
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
	struct MConfig final : Noninstancable
	{
		static constexpr uint	MaxQueues			= 2;	// TODO ?
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

		MViewport () {}
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

		MScissorRect () {}
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

		MClearColor () {}
		MClearColor (float r, float g, float b, float a) : red{r}, green{g}, blue{b}, alpha{a} {}
		MClearColor (const RGBA32f &col) : MClearColor{col.r, col.g, col.b, col.a} {}
		MClearColor (const RGBA8u &col) : MClearColor{RGBA32f{col}} {}
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
