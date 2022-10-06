// Copyright (c)  Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/ObjC/NS.inl.h"
#include "graphics/Public/MetalTypes.h"
#include "graphics/Public/ImageUtils.h"
#include "graphics/Metal/MCommon.h"

#undef null
#ifdef AE_PLATFORM_MACOS
#include <Cocoa/Cocoa.h>
#endif
#include <QuartzCore/QuartzCore.h>
#include <Metal/MTLDevice.h>
#include <Metal/MTLCommandQueue.h>
#include <Metal/MTLCommandBuffer.h>
#include <Metal/MTLBuffer.h>
#include <Metal/MTLTexture.h>
#include <Metal/MTLAccelerationStructure.h>
#include <Metal/MTLComputePipeline.h>
#include <Metal/MTLSampler.h>
#include <Metal/MTLArgument.h>
#include <Metal/MTLLibrary.h>
#include <Metal/MTLIndirectCommandBuffer.h>
#include <Metal/MTLIndirectCommandEncoder.h>
#include <Metal/MTLIntersectionFunctionTable.h>
#define null	nullptr


namespace AE::Graphics
{
	using AE::NS::NSCastRetain;
	using AE::NS::NSCastAttach;
	

	//
	// Metal Pixel Format Info
	//
	struct MPixFormatInfo
	{
		ushort		bitsPerBlock	= 0;		// for color and depth
		ubyte2		blockSize		{1,1};

		MPixFormatInfo () {}

		MPixFormatInfo (uint bpp, const uint2 &block) :
			bpp{ CheckCast<ushort>( bpp )},
			blockSize{ CheckCast<ubyte2>(block)}
		{}

		ND_ uint2	TexBlockSize ()	const	{ return uint2{blockSize}; }

		ND_ static MPixFormatInfo  GetFormatInfo (MTLPixelFormat fmt);
	};


/*
=================================================
	MetalObject::Cast
=================================================
*/
# ifdef AE_PLATFORM_MACOS
	template <>	auto  MetalObject< EMetalObjType::NSView						>::Cast () const	{ return (__bridge NSView *)									Ptr(); }
# endif
	template <>	auto  MetalObject< EMetalObjType::CALayer						>::Cast () const	{ return (__bridge CAMetalLayer *)								Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::CADrawable					>::Cast () const	{ return (__bridge id<CAMetalDrawable>)							Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::Drawable						>::Cast () const	{ return (__bridge id<MTLDrawable>)								Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::CaptureScope					>::Cast () const	{ return (__bridge id<MTLCaptureScope>)							Ptr(); }

	template <>	auto  MetalObject< EMetalObjType::Device						>::Cast () const	{ return (__bridge id<MTLDevice>)								Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::CommandQueue					>::Cast () const	{ return (__bridge id<MTLCommandQueue>)							Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::IOCommandQueue				>::Cast () const	{ return (__bridge id<MTLIOCommandQueue>)						Ptr(); }

	template <>	auto  MetalObject< EMetalObjType::CommandBuffer					>::Cast () const	{ return (__bridge id<MTLCommandBuffer>)						Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::CommandEncoder				>::Cast () const	{ return (__bridge id<MTLCommandEncoder>)						Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::ComputeCommandEncoder			>::Cast () const	{ return (__bridge id<MTLComputeCommandEncoder>)				Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::RenderCommandEncoder			>::Cast () const	{ return (__bridge id<MTLRenderCommandEncoder>)					Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::ParallelRenderCommandEncoder	>::Cast () const	{ return (__bridge id<MTLParallelRenderCommandEncoder>)			Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::AccelStructCommandEncoder		>::Cast () const	{ return (__bridge id<MTLAccelerationStructureCommandEncoder>)	Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::BlitCommandEncoder			>::Cast () const	{ return (__bridge id<MTLBlitCommandEncoder>)					Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::ResourceStateCommandEncoder	>::Cast () const	{ return (__bridge id<MTLResourceStateCommandEncoder>)			Ptr(); }
	
	template <>	auto  MetalObject< EMetalObjType::ArrayOfArgumentDescriptor		>::Cast () const	{ return (__bridge NSArray<MTLArgumentDescriptor *>*)			Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::ArgumentEncoder				>::Cast () const	{ return (__bridge id<MTLArgumentEncoder>)						Ptr(); }

	template <>	auto  MetalObject< EMetalObjType::ShaderFunction				>::Cast () const	{ return (__bridge id<MTLFunction>)								Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::Library						>::Cast () const	{ return (__bridge id<MTLLibrary>)								Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::DepthStencilState				>::Cast () const	{ return (__bridge id<MTLDepthStencilState>)					Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::RenderPipeline				>::Cast () const	{ return (__bridge id<MTLRenderPipelineState>)					Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::ComputePipeline				>::Cast () const	{ return (__bridge id<MTLComputePipelineState>)					Ptr(); }

	template <>	auto  MetalObject< EMetalObjType::Memory						>::Cast () const	{ return (__bridge id<MTLHeap>)									Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::Sampler						>::Cast () const	{ return (__bridge id<MTLSamplerState>)							Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::RasterizationRateMap			>::Cast () const	{ return (__bridge id<MTLRasterizationRateMap>)					Ptr(); }

	template <>	auto  MetalObject< EMetalObjType::Fence							>::Cast () const	{ return (__bridge id<MTLFence>)								Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::Event							>::Cast () const	{ return (__bridge id<MTLEvent>)								Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::SharedEvent					>::Cast () const	{ return (__bridge id<MTLSharedEvent>)							Ptr(); }

	// resources
	template <>	auto  MetalObject< EMetalObjType::BaseResource					>::Cast () const	{ return (__bridge id<MTLResource>)								Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::Buffer						>::Cast () const	{ return (__bridge id<MTLBuffer>)								Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::Image							>::Cast () const	{ return (__bridge id<MTLTexture>)								Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::AccelStruct					>::Cast () const	{ return (__bridge id<MTLAccelerationStructure>)				Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::IndirectCmdBuf				>::Cast () const	{ return (__bridge id<MTLIndirectCommandBuffer>)				Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::IntersectionFnTable			>::Cast () const	{ return (__bridge id<MTLIntersectionFunctionTable>)			Ptr(); }
	template <>	auto  MetalObject< EMetalObjType::VisibleFnTable				>::Cast () const	{ return (__bridge id<MTLVisibleFunctionTable>)					Ptr(); }
	
/*
=================================================
	MetalCast
=================================================
*/
	ND_ MTLViewport  MetalCast (const RenderPassDesc::Viewport &vp)
	{
		MTLViewport	res;
		res.originX	= vp.rect.left;
		res.originY	= vp.rect.top;
		res.width	= vp.rect.Width();
		res.height	= vp.rect.Height();
		res.znear	= vp.minDepth;
		res.zfar	= vp.maxDepth;
		return res;
	}
	
/*
=================================================
	MetalCast
=================================================
*/
	ND_ MTLScissorRect  MetalCast (const RectI &sc)
	{
		MTLScissorRect	res;
		res.x		= sc.left;
		res.y		= sc.top;
		res.width	= sc.Width();
		res.height	= sc.Height();
		return res;
	}


} // AE::Graphics
