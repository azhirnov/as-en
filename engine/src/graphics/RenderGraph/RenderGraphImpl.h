// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/RenderGraph/ResStateTracker.h"
#include "graphics/RenderGraph/RenderGraph.h"
#include "graphics/RenderGraph/RGCommandContext.h"

namespace AE::RG
{
	struct DirectCtx
	{
		using CommandBuffer	= AE::Graphics::DirectCtx::CommandBuffer;
		using Transfer		= RG::_hidden_::TransferContext< AE::Graphics::DirectCtx::Transfer >;
		using Compute		= RG::_hidden_::ComputeContext< AE::Graphics::DirectCtx::Compute >;
		using Draw			= AE::Graphics::DirectCtx::Draw;	// TODO
		using Graphics		= RG::_hidden_::GraphicsContext< AE::Graphics::DirectCtx::Graphics >;
		using ASBuild		= RG::_hidden_::ASBuildContext< AE::Graphics::DirectCtx::ASBuild >;
		using RayTracing	= RG::_hidden_::RayTracingContext< AE::Graphics::DirectCtx::RayTracing >;
	};
	
	struct IndirectCtx
	{
		using CommandBuffer	= AE::Graphics::IndirectCtx::CommandBuffer;
		using Transfer		= RG::_hidden_::TransferContext< AE::Graphics::IndirectCtx::Transfer >;
		using Compute		= RG::_hidden_::ComputeContext< AE::Graphics::IndirectCtx::Compute >;
		using Draw			= AE::Graphics::IndirectCtx::Draw;	// TODO
		using Graphics		= RG::_hidden_::GraphicsContext< AE::Graphics::IndirectCtx::Graphics >;
		using ASBuild		= RG::_hidden_::ASBuildContext< AE::Graphics::IndirectCtx::ASBuild >;
		using RayTracing	= RG::_hidden_::RayTracingContext< AE::Graphics::IndirectCtx::RayTracing >;
	};

	using RenderGraph		= RG::_hidden_::RenderGraph;
	using CommandBatchPtr	= RG::_hidden_::RGCommandBatchPtr;

} // AE::RG
