// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/ResourceEnums.h"

namespace AE::PipelineCompiler
{

	enum class EAttachment : uint
	{
		Invalidate,
		Color,
		ColorResolve,
		ReadWrite,		// read from input attachment, write to color/depth attachment	// TODO: self dependency
		Input,
		Depth,
		Preserve,
		ShadingRate,
		// TODO
		//	FragmentDensity,
		//	DepthStencilResolve,
		//	Depth_ShaderRead,
		_Count,
		DepthStencil	= Depth,
		Unknown			= ~0u,
	};

	enum class EAttachmentLoadOp : ubyte
	{
		Invalidate,
		Load,
		Clear,
		None,
		_Count,
		Unknown		= 0xFF,
	};

	enum class EAttachmentStoreOp : ubyte
	{
		Invalidate,
		Store,
		None,
		StoreCustomSamplePositions,	// MTLStoreActionOptionCustomSamplePositions
		_Count,
		Unknown		= 0xFF,
	};


} // AE::PipelineCompiler
