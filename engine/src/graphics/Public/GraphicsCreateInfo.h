// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/Queue.h"

namespace AE::Graphics
{

	class IGfxMemAllocator;
	using GfxMemAllocatorPtr = RC< IGfxMemAllocator >;
	
	class IDescriptorAllocator;
	using DescriptorAllocatorPtr = RC< IDescriptorAllocator >;


	//
	// Graphics Create Info
	//
	struct GraphicsCreateInfo final
	{
		uint	maxFrames	= 2;

		// staging buffers //
		using SizePerQueue_t = StaticArray< Bytes32u, 3 >;
		struct {
			// static staging buffers allocated at engine start
			SizePerQueue_t	writeStaticSize			= {};
			SizePerQueue_t	readStaticSize			= {};

			// dynamic buffers will be allocated when needed and will be released after,
			// but total size can be limited
			Bytes			maxWriteDynamicSize		= 1_Gb;
			Bytes			maxReadDynamicSize		= 1_Gb;

			// granularity of the dynamic staging buffers
			Bytes			dynamicBlockSize		= 32_Mb;

			// wait X frames before release dynamic buffer
			uint			maxFramesToRelease		= 1 << 10;
			
			// vertex & index buffer size for single frame
			Bytes32u		vstreamSize				= 4_Mb;

			// total size of staging memory is:
			//   (writeStaticSize + readStaticSize) * maxFrames + (maxWriteDynamicSize + maxReadDynamicSize)
		}	staging;
		

		GfxMemAllocatorPtr		defaultGfxAllocator;
		DescriptorAllocatorPtr	defaultDescAllocator;
		
		struct
		{
			String			appName			= "Test";
			String			deviceName;		// keep empty for auto-detect
			EQueueMask		requiredQueues	= EQueueMask::Graphics;
			EQueueMask		optionalQueues	= Default;
			bool			debuggable		= true;
		}	device;

		struct
		{
			EPixelFormat	format			= EPixelFormat::RGBA8_UNorm;
			EColorSpace		colorSpace		= EColorSpace::sRGB_nonlinear;
			EImageUsage		usage			= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;	// TODO: keep ColorAttachment only
			EPresentMode	presentMode		= EPresentMode::FIFO;
		}	swapchain;
	};

} // AE::Graphics
