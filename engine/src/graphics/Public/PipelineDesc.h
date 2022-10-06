// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/RenderState.h"
#include "graphics/Public/ResourceEnums.h"

namespace AE::Graphics
{

	//
	// Pipeline Options
	//
	enum class EPipelineOpt : ubyte
	{
		Optimize					= 1 << 0,

		// compute only
		CS_DispatchBase				= 1 << 1,		// Vulkan only

		// ray tracing only
		RT_NoNullAnyHitShaders		= 1 << 2,
		RT_NoNullClosestHitShaders	= 1 << 3,
		RT_NoNullMissShaders		= 1 << 4,
		RT_NoNullIntersectionShaders= 1 << 5,
		RT_SkipTriangles			= 1 << 6,
		RT_SkipAABBs				= 1 << 7,

		_Last,
		All							= ((_Last - 1) << 1) - 1,
		Unknown						= 0,
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
		_Last,
		All				= ((_Last - 1) << 1) - 1,
	};
	AE_BIT_OPERATORS( EPipelinePackOpt );



	//
	// Pipeline Pack description
	//

	struct PipelinePackDesc
	{
		RC<RStream>			stream;
		PipelinePackID		parentPackId;
		EPipelinePackOpt	options			= EPipelinePackOpt::All;
		EPixelFormat		swapchainFmt	= Default;
		StringView			dbgName;
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
		Ptr<const RenderState>				renderStatePtr;
		CompatRenderPassName::Optimized_t	renderPass;
		SubpassName::Optimized_t			subpass;
		ubyte								viewportCount	= 1;
	};



	//
	// Mesh Pipeline description
	//

	struct MeshPipelineDesc : BasePipelineDesc
	{
		Ptr<const RenderState>				renderStatePtr;
		CompatRenderPassName::Optimized_t	renderPass;
		SubpassName::Optimized_t			subpass;
		ubyte								viewportCount	= 1;
		ushort3								taskLocalSize	{UndefinedLocalSize};	// Vulkan: must be {x,1,1}
		ushort3								meshLocalSize	{UndefinedLocalSize};	// Vulkan: must be {x,1,1}
	};



	//
	// Compute Pipeline description
	//

	struct ComputePipelineDesc : BasePipelineDesc
	{
		ushort3		localSize	{UndefinedLocalSize};
	};



	//
	// Ray Tracing Pipeline description
	//

	struct RayTracingPipelineDesc : BasePipelineDesc
	{
		uint		maxRecursionDepth			= 1;
		Bytes32u	maxPipelineRayPayloadSize;
		Bytes32u	maxPipelineRayHitAttributeSize;
	};



	//
	// Tile Pipeline description
	//

	struct TilePipelineDesc : BasePipelineDesc
	{
		CompatRenderPassName::Optimized_t	renderPass;
		SubpassName::Optimized_t			subpass;
		ushort2								localSize	{UndefinedLocalSize};
	};


}	// AE::Graphics

namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::GraphicsPipelineDesc >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::GraphicsPipelineDesc >	{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::MeshPipelineDesc >			{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::MeshPipelineDesc >		{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::ComputePipelineDesc >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::ComputePipelineDesc >	{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::RayTracingPipelineDesc >	{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::RayTracingPipelineDesc >{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::TilePipelineDesc >			{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::TilePipelineDesc >		{ static constexpr bool  value = true; };

}	// AE::Base

