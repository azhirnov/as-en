// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/Queue.h"
#include "graphics/Public/GraphicsCreateInfo.h"

namespace AE::Graphics
{

	//
	// Swapchain description
	//

	struct SwapchainDesc
	{
		EPixelFormat	format			= Default;
		EColorSpace		colorSpace		= EColorSpace::sRGB_nonlinear;
		EPresentMode	presentMode		= EPresentMode::FIFO;
		ubyte			minImageCount	= 2;
		EImageUsage		usage			= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;	// TODO: keep ColorAttachment only
		EImageOpt		options			= EImageOpt::BlitDst;

		SwapchainDesc ()								__NE___	{}
		SwapchainDesc (const SwapchainDesc &)			__NE___	= default;

		SwapchainDesc (const GraphicsCreateInfo &info)	__NE___ :
			format{ info.swapchain.format },
			colorSpace{ info.swapchain.colorSpace },
			presentMode{ info.swapchain.presentMode },
			minImageCount{ CheckCast<ubyte>( info.maxFrames )},
			usage{ info.swapchain.usage },
			options{ info.swapchain.options }
		{}
	};
	


	//
	// Surface Format
	//

	struct SurfaceFormat
	{
		EPixelFormat	format	= Default;
		EColorSpace		space	= Default;

		ND_ bool  operator == (const SurfaceFormat &rhs)	C_NE___	{ return format == rhs.format and space == rhs.space; }
	};


} // AE::Graphics
