// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "base/ObjC/NS.h"
# include "graphics/Public/IDs.h"

namespace AE::Graphics
{

	enum class EMetalObjType
	{
		NSView,			// NSView
		CALayer,		// CAMetalLayer
		CADrawable,		// CAMetalDrawable
		Drawable,		// MTLDrawable
		CaptureScope,	// MTLCaptureScope

		Device,
		CommandQueue,
		IOCommandQueue,					// macOS 13+, iOS 16.0+

		CommandBuffer,
		CommandEncoder,
		ComputeCommandEncoder,
		RenderCommandEncoder,
		ParallelRenderCommandEncoder,	// macOS 10.11+, iOS 8.0+
		AccelStructCommandEncoder,		// macOS 11.0+, iOS 14.0+
		BlitCommandEncoder,
		ResourceStateCommandEncoder,	// macOS 11.0+, iOS 14.0+

		ArrayOfArgumentDescriptor,		// macOS 10.13+, iOS 11.0+
		ArgumentEncoder,				// macOS 10.13+, iOS 11.0+

		Memory,
		Sampler,
		RasterizationRateMap,			// macOS 10.15.4+, iOS 13.0+
		
		Fence,
		Event,							// macOS 10.14+, iOS 12.0+
		SharedEvent,					// macOS 10.14+, iOS 12.0+

		ShaderFunction,
		Library,
		DepthStencilState,
		RenderPipeline,
		ComputePipeline,
		BinaryArchive,					// macOS 11.0+, iOS 14.0+

		_ResourceBegin,
		BaseResource,					// MTLResource
		Buffer,
		Image,
		IndirectCmdBuf,					// macOS 10.14+, iOS 12.0+
		AccelStruct,					// macOS 11.0+, iOS 14.0+
		IntersectionFnTable,			// macOS 11.0+, iOS 14.0+
		VisibleFnTable,					// macOS 11.0+, iOS 14.0+
		_ResourceEnd,
	};


	//
	// Metal Object (without RC)
	//
	template <EMetalObjType TypeID>
	class MetalObject final : public NS::ObjectRef
	{
	// types
	public:
		using Self = MetalObject< TypeID >;


	// methods
	public:
		MetalObject () {}
		MetalObject (const Self &) = default;
		MetalObject (std::nullptr_t) {}
		explicit MetalObject (const void* ptr)	{ Attach( ptr ); }

		Self&  operator = (const Self &) = default;

		ND_ explicit operator bool () const	{ return ObjectRef::operator bool(); }

		ND_ auto  Cast () const;
	};
	
	using MetalNSView							= MetalObject< EMetalObjType::NSView >;
	using MetalCALayer							= MetalObject< EMetalObjType::CALayer >;
	using MetalCADrawable						= MetalObject< EMetalObjType::CADrawable >;
	using MetalDrawable							= MetalObject< EMetalObjType::Drawable >;
	using MetalCaptureScope						= MetalObject< EMetalObjType::CaptureScope >;
	using MetalDevice							= MetalObject< EMetalObjType::Device >;
	using MetalQueue							= MetalObject< EMetalObjType::CommandQueue >;
	using MetalIOQueue							= MetalObject< EMetalObjType::IOCommandQueue >;
	using MetalCommandBuffer					= MetalObject< EMetalObjType::CommandBuffer >;
	using MetalCommandEncoder					= MetalObject< EMetalObjType::CommandEncoder >;
	using MetalComputeCommandEncoder			= MetalObject< EMetalObjType::ComputeCommandEncoder >;
	using MetalRenderCommandEncoder				= MetalObject< EMetalObjType::RenderCommandEncoder >;
	using MetalParallelRenderCommandEncoder		= MetalObject< EMetalObjType::ParallelRenderCommandEncoder >;
	using MetalAccelStructCommandEncoder		= MetalObject< EMetalObjType::AccelStructCommandEncoder >;
	using MetalBlitCommandEncoder				= MetalObject< EMetalObjType::BlitCommandEncoder >;
	using MetalResourceStateCommandEncoder		= MetalObject< EMetalObjType::ResourceStateCommandEncoder >;
	using MetalArrayOfArgumentDescriptor		= MetalObject< EMetalObjType::ArrayOfArgumentDescriptor >;
	using MetalArgumentEncoder					= MetalObject< EMetalObjType::ArgumentEncoder >;
	using MetalMemory							= MetalObject< EMetalObjType::Memory >;
	using MetalFence							= MetalObject< EMetalObjType::Fence >;
	using MetalEvent							= MetalObject< EMetalObjType::Event >;
	using MetalSharedEvent						= MetalObject< EMetalObjType::SharedEvent >;
	using MetalSampler							= MetalObject< EMetalObjType::Sampler >;
	using MetalRasterizationRateMap				= MetalObject< EMetalObjType::RasterizationRateMap >;
	using MetalShaderFunction					= MetalObject< EMetalObjType::ShaderFunction >;
	using MetalLibrary							= MetalObject< EMetalObjType::Library >;
	using MetalDepthStencilState				= MetalObject< EMetalObjType::DepthStencilState >;
	using MetalRenderPipeline					= MetalObject< EMetalObjType::RenderPipeline >;
	using MetalComputePipeline					= MetalObject< EMetalObjType::ComputePipeline >;
	using MetalBinaryArchive					= MetalObject< EMetalObjType::BinaryArchive >;
	using MetalResource							= MetalObject< EMetalObjType::BaseResource >;
	using MetalBuffer							= MetalObject< EMetalObjType::Buffer >;
	using MetalImage							= MetalObject< EMetalObjType::Image >;
	using MetalAccelStruct						= MetalObject< EMetalObjType::AccelStruct >;
	using MetalIndirectCmdBuf					= MetalObject< EMetalObjType::IndirectCmdBuf >;
	using MetalIntersectionFnTable				= MetalObject< EMetalObjType::IntersectionFnTable >;
	using MetalVisibleFnTable					= MetalObject< EMetalObjType::VisibleFnTable >;
	

	//
	// Metal Object with RC
	//
	
	using MetalNSViewRC							= NS::ObjStrongPtr< MetalNSView >;
	using MetalCALayerRC						= NS::ObjStrongPtr< MetalCALayer >;
	using MetalCADrawableRC						= NS::ObjStrongPtr< MetalCADrawable >;
	using MetalDrawableRC						= NS::ObjStrongPtr< MetalDrawable >;
	using MetalCaptureScopeRC					= NS::ObjStrongPtr< MetalCaptureScope >;
	using MetalDeviceRC							= NS::ObjStrongPtr< MetalDevice >;
	using MetalQueueRC							= NS::ObjStrongPtr< MetalQueue >;
	using MetalIOQueueRC						= NS::ObjStrongPtr< MetalIOQueue >;
	using MetalCommandBufferRC					= NS::ObjStrongPtr< MetalCommandBuffer >;
	using MetalCommandEncoderRC					= NS::ObjStrongPtr< MetalCommandEncoder >;
	using MetalComputeCommandEncoderRC			= NS::ObjStrongPtr< MetalComputeCommandEncoder >;
	using MetalRenderCommandEncoderRC			= NS::ObjStrongPtr< MetalRenderCommandEncoder >;
	using MetalParallelRenderCommandEncoderRC	= NS::ObjStrongPtr< MetalParallelRenderCommandEncoder >;
	using MetalAccelStructCommandEncoderRC		= NS::ObjStrongPtr< MetalAccelStructCommandEncoder >;
	using MetalBlitCommandEncoderRC				= NS::ObjStrongPtr< MetalBlitCommandEncoder >;
	using MetalResourceStateCommandEncoderRC	= NS::ObjStrongPtr< MetalResourceStateCommandEncoder >;
	using MetalArrayOfArgumentDescriptorRC		= NS::ObjStrongPtr< MetalArrayOfArgumentDescriptor >;
	using MetalArgumentEncoderRC				= NS::ObjStrongPtr< MetalArgumentEncoder >;
	using MetalMemoryRC							= NS::ObjStrongPtr< MetalMemory >;
	using MetalFenceRC							= NS::ObjStrongPtr< MetalFence >;
	using MetalEventRC							= NS::ObjStrongPtr< MetalEvent >;
	using MetalSharedEventRC					= NS::ObjStrongPtr< MetalSharedEvent >;
	using MetalSamplerRC						= NS::ObjStrongPtr< MetalSampler >;
	using MetalRasterizationRateMapRC			= NS::ObjStrongPtr< MetalRasterizationRateMap >;
	using MetalShaderFunctionRC					= NS::ObjStrongPtr< MetalShaderFunction >;
	using MetalLibraryRC						= NS::ObjStrongPtr< MetalLibrary >;
	using MetalDepthStencilStateRC				= NS::ObjStrongPtr< MetalDepthStencilState >;
	using MetalRenderPipelineRC					= NS::ObjStrongPtr< MetalRenderPipeline >;
	using MetalComputePipelineRC				= NS::ObjStrongPtr< MetalComputePipeline >;
	using MetalBinaryArchiveRC					= NS::ObjStrongPtr< MetalBinaryArchive >;
	using MetalResourceRC						= NS::ObjStrongPtr< MetalResource >;
	using MetalBufferRC							= NS::ObjStrongPtr< MetalBuffer >;
	using MetalImageRC							= NS::ObjStrongPtr< MetalImage >;
	using MetalAccelStructRC					= NS::ObjStrongPtr< MetalAccelStruct >;
	using MetalIndirectCmdBufRC					= NS::ObjStrongPtr< MetalIndirectCmdBuf >;
	using MetalIntersectionFnTableRC			= NS::ObjStrongPtr< MetalIntersectionFnTable >;
	using MetalVisibleFnTableRC					= NS::ObjStrongPtr< MetalVisibleFnTable >;



	//
	// Metal Memory Object Info
	//
	struct MetalMemoryObjInfo
	{
		MetalMemory		mem;					// can be null if MTLHeap is not used
		Bytes			offset;
		Bytes			size;
		void *			mappedPtr	= null;		// include 'offset'
	};


	//
	// Metal Image description
	//
	struct MetalImageDesc
	{
		MetalImage		image;
		bool			canBeDestroyed	= true;
	};


	//
	// Metal Buffer desciption
	//
	struct MetalBufferDesc
	{
		MetalBuffer		buffer;
		bool			canBeDestroyed	= true;
	};


	//
	// Metal Command Batch Dependency
	//
	struct MetalCmdBatchDependency
	{
		MetalSharedEvent	event;
		ulong				value	= 0;
	};

	
	enum class MDeviceAddress : ulong { Unknown = 0 };
	
	ND_ inline MDeviceAddress  operator + (MDeviceAddress addr, Bytes offset) {
		return MDeviceAddress(ulong(addr) + ulong(offset));
	}


} // AE::Graphics

#endif // AE_ENABLE_METAL
