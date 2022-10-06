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
		EImageUsage		usage			= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;	// TODO: keep ColorAttachment only
		EPresentMode	presentMode		= EPresentMode::FIFO;
		uint			minImageCount	= 2;

		SwapchainDesc () {}
		SwapchainDesc (const SwapchainDesc &) = default;

		SwapchainDesc (const GraphicsCreateInfo &info) :
			format{ info.swapchain.format },
			colorSpace{ info.swapchain.colorSpace },
			usage{ info.swapchain.usage },
			presentMode{ info.swapchain.presentMode },
			minImageCount{ info.maxFrames }
		{}
	};


}	// AE::Graphics
