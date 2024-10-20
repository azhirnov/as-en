// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/RenderState.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/VertexEnums.h"
#include "graphics/Public/GraphicsCreateInfo.h"

namespace AE::Graphics
{

	//
	// Pipeline Options
	//
	enum class EPipelineOpt : ushort
	{
		Optimize						= 1 << 0,

		// compute only //
		CS_DispatchBase					= 1 << 1,		// Vulkan only

		// ray tracing only //
		RT_NoNullAnyHitShaders			= 1 << 2,
		RT_NoNullClosestHitShaders		= 1 << 3,
		RT_NoNullMissShaders			= 1 << 4,
		RT_NoNullIntersectionShaders	= 1 << 5,
		RT_SkipTriangles				= 1 << 6,
		RT_SkipAABBs					= 1 << 7,
		//RT_AllowMotion
		//RT_OpacityMicromap
		//RT_DisplacementMicromap

		// cache //
		//	Pipeline creation will fail if it is not exists in cache.
		DontCompile						= 1 << 8,

		// debug / profile //
		//	When a pipeline is created, its state and shaders are compiled into zero or more device-specific executables,
		//	which are used when executing commands against that pipeline.
		CaptureStatistics				= 1 << 9,

		//	May include the final shader assembly, a binary form of the compiled shader,
		//	or the shader compiler’s internal representation at any number of intermediate compile steps.
		CaptureInternalRepresentation	= 1 << 10,

		_Last,
		All								= ((_Last - 1) << 1) - 1,
		Unknown							= 0,
	};
	AE_BIT_OPERATORS( EPipelineOpt );



	//
	// Pipeline Pack Options
	//
	enum class EPipelinePackOpt : ubyte
	{
		Unknown			= 0,
		FeatureSets		= 1 << 0,
		Samplers		= 1 << 1,
		RenderPasses	= 1 << 2,
		Pipelines		= 1 << 3,	// layouts, pipelines, shaders, render techniques
		// TODO: override pipeline optimization flags
		_Last,
		All				= ((_Last - 1) << 1) - 1,
	};
	AE_BIT_OPERATORS( EPipelinePackOpt );



	//
	// Shading Rate (VRS)
	//
	enum class EShadingRate : ubyte
	{
		Size1x1				= 0x10,
		Size1x2				= 0x20,
		Size1x4				= 0x30,
		Size2x1				= 0x40,
		Size2x2				= 0x50,
		Size2x4				= 0x60,
		Size4x1				= 0x70,
		Size4x2				= 0x80,
		Size4x4				= 0x90,
		_SizeCount			= 9,
		_SizeMask			= 0xF0,

		Samples1			= 0x00,
		Samples2			= 0x01,
		Samples4			= 0x02,
		Samples8			= 0x04,
		Samples16			= 0x08,
		Samples1_2			= Samples1 | Samples2,
		Samples1_2_4		= Samples1 | Samples2 | Samples4,
		Samples1_2_4_8		= Samples1 | Samples2 | Samples4 | Samples8,
		Samples1_2_4_8_16	= Samples1 | Samples2 | Samples4 | Samples8 | Samples16,
		_SamplesMask		= 0x0F,
	};

	ND_ constexpr EShadingRate  operator | (EShadingRate lhs, EShadingRate rhs) __NE___
	{
		ASSERT( AnyBits( lhs, EShadingRate::_SizeMask ) != AnyBits( rhs, EShadingRate::_SizeMask ));	// can not combine size with size
		return EShadingRate(uint(lhs) | uint(rhs));
	}

	ND_ inline EShadingRate	EShadingRate_FromSize (uint2 size)				__NE___	{ return EShadingRate( (IntLog2(size.x)*3 + IntLog2(size.y) + 1) << 4 ); }
	ND_ inline EShadingRate	EShadingRate_FromSampleBits (uint bits)			__NE___	{ return EShadingRate( (bits >> 1) & 0x0F ); }
	ND_ inline uint2		EShadingRate_Size (EShadingRate size)			__NE___	{ uint s = ((uint(size) & 0xF0) >> 4) - 1;  return uint2{ 1u<<(s/3), 1u<<(s%3) }; }
	ND_ inline uint			EShadingRate_SampleBits (EShadingRate samples)	__NE___	{ return (uint(samples) & 0x0F); }



	//
	// Shading Rate Combiner Ops (VRS)
	//
	enum class EShadingRateCombinerOp : ubyte
	{
		// S - original rate
		// D - new rate
		Keep		= 0,	// S
		Replace,			// D
		Min,				// min( S, D )
		Max,				// max( S, D )
		Sum,				// S + D		// \__ check 'fragmentShadingRateStrictMultiplyCombiner' feature
		Mul,				// S * D		// /
		_Count,
		Unknown		= 0xFF,
	};


	//
	// Pipeline Pack description
	//
	struct PipelinePackDesc
	{
		RC<RStream>			stream;
		PipelinePackID		parentPackId;
		EPipelinePackOpt	options			= EPipelinePackOpt::All;
		EPixelFormat		surfaceFormat	= Default;		// TODO: color space
		StringView			dbgName;
		GfxMemAllocatorPtr	sbtAllocator;	// used to allocate SBT with unified memory (Vulkan only)
	};



	//
	// Base Pipeline description
	//
	struct BasePipelineDesc
	{
		using SpecValues_t	= FixedMap< SpecializationName::Optimized_t, /*bool/int/uint/float*/uint, 8 >;

		static constexpr ushort	UndefinedLocalSize = UMax;

		SpecValues_t			specialization;
		EPipelineOpt			options				= Default;
		EPipelineDynamicState	dynamicState		= Default;
		StringView				dbgName;
	};



	//
	// Graphics Pipeline description
	//
	struct GraphicsPipelineDesc : BasePipelineDesc
	{
	// types
		struct VertexInput
		{
			EVertexType		type			= Default;
			Bytes16u		offset;
			ubyte			index			= UMax;		// attrib index in shader
			ubyte			bufferBinding	= UMax;		// index in 'vertexBuffers'
		};

		struct VertexBuffer
		{
			VertexBufferName::Optimized_t	name;
			ShaderStructName::Optimized_t	typeName;
			EVertexInputRate				rate		= Default;
			ubyte							index		= UMax;			// for 'ctx.BindVertexBuffer()'
			Bytes16u						stride;
			uint							divisor		= 0;
		};
		StaticAssert( sizeof(VertexInput) == 6 );
		StaticAssert( sizeof(VertexBuffer) == 16 );

	// variables
		Ptr<const RenderState>				renderStatePtr;
		CompatRenderPassName::Optimized_t	renderPass;
		SubpassName::Optimized_t			subpass;
		ArrayView<VertexInput>				vertexInput;
		ArrayView<VertexBuffer>				vertexBuffers;
		ubyte								viewportCount	= 1;

	// methods
		GraphicsPipelineDesc () __NE___ {}

		void  SetRenderPass (CompatRenderPassName::Ref rpName, SubpassName::Ref spName) __NE___	{ renderPass = rpName;  subpass = spName; }
	};



	//
	// Mesh Pipeline description
	//
	struct MeshPipelineDesc : BasePipelineDesc
	{
	// variables
		Ptr<const RenderState>				renderStatePtr;
		CompatRenderPassName::Optimized_t	renderPass;
		SubpassName::Optimized_t			subpass;
		ubyte								viewportCount	= 1;
		packed_ushort3						taskLocalSize	{UndefinedLocalSize};
		packed_ushort3						meshLocalSize	{UndefinedLocalSize};

	// methods
		MeshPipelineDesc () __NE___ {}
	};



	//
	// Compute Pipeline description
	//
	struct ComputePipelineDesc : BasePipelineDesc
	{
	// variables
		packed_ushort3		localSize		{UndefinedLocalSize};
		ushort				subgroupSize	= 0;

	// methods
		ComputePipelineDesc () __NE___ {}
	};



	//
	// Ray Tracing Pipeline description
	//
	struct RayTracingPipelineDesc : BasePipelineDesc
	{
	// variables
		uint		maxRecursionDepth			= 1;
		Bytes32u	maxPipelineRayPayloadSize;
		Bytes32u	maxPipelineRayHitAttributeSize;

	// methods
		RayTracingPipelineDesc () __NE___ {}
	};



	//
	// Tile Pipeline description
	//
	struct TilePipelineDesc : BasePipelineDesc
	{
	// variables
		CompatRenderPassName::Optimized_t	renderPass;
		SubpassName::Optimized_t			subpass;
		packed_ushort2						localSize	{UndefinedLocalSize};

	// methods
		TilePipelineDesc () __NE___ {}
	};


} // AE::Graphics

namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::GraphicsPipelineDesc >			: CT_True {};
	template <> struct TTriviallySerializable< AE::Graphics::GraphicsPipelineDesc >		: CT_True {};

	template <> struct TMemCopyAvailable< AE::Graphics::MeshPipelineDesc >				: CT_True {};
	template <> struct TTriviallySerializable< AE::Graphics::MeshPipelineDesc >			: CT_True {};

	template <> struct TMemCopyAvailable< AE::Graphics::ComputePipelineDesc >			: CT_True {};
	template <> struct TTriviallySerializable< AE::Graphics::ComputePipelineDesc >		: CT_True {};

	template <> struct TMemCopyAvailable< AE::Graphics::RayTracingPipelineDesc >		: CT_True {};
	template <> struct TTriviallySerializable< AE::Graphics::RayTracingPipelineDesc >	: CT_True {};

	template <> struct TMemCopyAvailable< AE::Graphics::TilePipelineDesc >				: CT_True {};
	template <> struct TTriviallySerializable< AE::Graphics::TilePipelineDesc >			: CT_True {};

	template <>	struct TTriviallySerializable< AE::Graphics::GraphicsPipelineDesc::VertexInput >	: CT_True {};
	template <>	struct TTriviallySerializable< AE::Graphics::GraphicsPipelineDesc::VertexBuffer >	: CT_True {};

} // AE::Base

