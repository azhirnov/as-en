// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_rhi/Public/ResourceEnums.h"
#include "graphics_rhi/Public/Queue.h"
#include "graphics_rhi/Public/SurfaceTransform.h"

namespace AE::Graphics
{

	//
	// Surface Format
	//

	struct SurfaceFormat
	{
		EPixelFormat	colorFormat	= Default;
		EColorSpace		colorSpace	= Default;

		SurfaceFormat ()												__NE___ = default;
		explicit SurfaceFormat (EPixelFormat fmt, EColorSpace space)	__NE___	: colorFormat{fmt}, colorSpace{space} {}

		ND_ bool  operator == (const SurfaceFormat &rhs)				C_NE___	{ return colorFormat == rhs.colorFormat and colorSpace == rhs.colorSpace; }
	};


	//
	// Swapchain description
	//

	struct SwapchainDesc : SurfaceFormat
	{
		EPresentMode	presentMode		= EPresentMode::FIFO;
		ubyte			minImageCount	= 2;
		bool			usePreTransform	= true;							// a bit faster on smartphones
		EPresentScaling	scaling			= EPresentScaling::OneToOne;	// requires 'swapchainMaintenance1'
		EImageUsage		usage			= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;	// TODO: keep ColorAttachment only
		EImageOpt		options			= EImageOpt::BlitDst;
		float			scale			= 1.f;							// if 'scaling = Stretch*'

		SwapchainDesc () __NE___ : SurfaceFormat{ Default, EColorSpace::sRGB_nonlinear } {}
	};


} // AE::Graphics


namespace AE::Base
{
	template <> struct TTriviallySerializable< Graphics::SurfaceFormat > : CT_True {};
	template <> struct TTriviallySerializable< Graphics::SwapchainDesc > : CT_True {};
}
