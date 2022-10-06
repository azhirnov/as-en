// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	TransferCtx --> DirectTransferCtx   --> BarrierMngr --> Metal device 
				\-> IndirectTransferCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MBaseIndirectContext.h"
# include "graphics/Metal/Commands/MBaseDirectContext.h"
# include "graphics/Metal/Commands/MAccumBarriers.h"

namespace AE::Graphics::_hidden_
{

	//
	// Metal Direct Transfer Context implementation
	//
	
	class _MDirectTransferCtx : public MBaseDirectContext
	{
	// variables
	private:
		MetalBlitCommandEncoderRC	_encoder;
		
		
	// methods
	public:
		void  FillBuffer (MetalBuffer buffer, Bytes offset, Bytes size, uint data);

		void  CopyBuffer (MetalBuffer srcBuffer, MetalBuffer dstBuffer, ArrayView<BufferCopy> ranges);
		void  CopyImage (MetalImage srcImage, MetalImage dstImage, ArrayView<ImageCopy> ranges);
		
		void  CopyBufferToImage (MetalBuffer srcBuffer, MetalImage dstImage, ArrayView<BufferImageCopy> ranges);
		void  CopyBufferToImage (MetalBuffer srcBuffer, MetalImage dstImage, ArrayView<BufferImageCopy2> ranges);

		void  CopyImageToBuffer (MetalImage srcImage, MetalBuffer dstBuffer, ArrayView<BufferImageCopy> ranges);
		void  CopyImageToBuffer (MetalImage srcImage, MetalBuffer dstBuffer, ArrayView<BufferImageCopy2> ranges);

	protected:
		_MDirectTransferCtx (Ptr<MCommandBatch> batch);
		_MDirectTransferCtx (Ptr<MCommandBatch> batch, MCommandBuffer cmdbuf);

		ND_ MetalCommandEncoder  _BaseEncoder ();
		
		void  _CommitBarriers ();
		void  _GenerateMipmaps (MetalImage image);
		
		void  _DebugMarker (NtStringView text, RGBA8u);
		void  _PushDebugGroup (NtStringView text, RGBA8u);
		void  _PopDebugGroup ();

	private:
		void  _CopyBufferToImage (MetalBuffer srcBuffer, MetalImage dstImage, const MPixFormatInfo &, ArrayView<BufferImageCopy> ranges);
		void  _CopyImageToBuffer (MetalImage srcImage, MetalBuffer dstBuffer, const MPixFormatInfo &, ArrayView<BufferImageCopy> ranges);
	};
	
	
	
	//
	// Metal Indirect Transfer Context implementation
	//
	
	class _MIndirectTransferCtx : public MBaseIndirectContext
	{
	// methods
	public:
		void  FillBuffer (MetalBuffer buffer, Bytes offset, Bytes size, uint data);

		void  CopyBuffer (MetalBuffer srcBuffer, MetalBuffer dstBuffer, ArrayView<BufferCopy> ranges);
		void  CopyImage (MetalImage srcImage, MetalImage dstImage, ArrayView<ImageCopy> ranges);
		
		void  CopyBufferToImage (MetalBuffer srcBuffer, MetalImage dstImage, ArrayView<BufferImageCopy> ranges);
		void  CopyBufferToImage (MetalBuffer srcBuffer, MetalImage dstImage, ArrayView<BufferImageCopy2> ranges);

		void  CopyImageToBuffer (MetalImage srcImage, MetalBuffer dstBuffer, ArrayView<BufferImageCopy> ranges);
		void  CopyImageToBuffer (MetalImage srcImage, MetalBuffer dstBuffer, ArrayView<BufferImageCopy2> ranges);

	protected:
		_MIndirectTransferCtx (Ptr<MCommandBatch> batch);
		_MIndirectTransferCtx (Ptr<MCommandBatch> batch, MSoftwareCmdBufPtr cmdbuf);
		
		void  _CommitBarriers ();
		void  _GenerateMipmaps (MetalImage image);
	};


	
	//
	// Metal Transfer Context implementation
	//

	template <typename CtxImpl>
	class _MTransferContextImpl : public CtxImpl, public ITransferContext
	{
	// types
	public:
		static constexpr bool	IsTransferContext		= true;
		static constexpr bool	IsMetalTransferContext	= true;
	private:
		static constexpr uint	_LocalArraySize			= 16;
		static constexpr Bytes	_StagingBufOffsetAlign	= 4_b;

		using RawCtx	= CtxImpl;
		using AccumBar	= MAccumBarriers< _MTransferContextImpl< CtxImpl >>;


	// methods
	public:
		explicit _MTransferContextImpl (Ptr<MCommandBatch> batch) : RawCtx{ batch } {}
		
		template <typename RawCmdBufType>
		_MTransferContextImpl (Ptr<MCommandBatch> batch, RawCmdBufType cmdbuf) : RawCtx{ batch, RVRef(cmdbuf) } {}

		_MTransferContextImpl () = delete;
		_MTransferContextImpl (const _MTransferContextImpl &) = delete;

		using RawCtx::FillBuffer;
		void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data) override final;

		using RawCtx::CopyBuffer;
		using RawCtx::CopyImage;

		void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges) override final;
		void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges) override final;

		using RawCtx::CopyBufferToImage;
		using RawCtx::CopyImageToBuffer;

		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges) override final;
		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges) override final;

		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges) override final;
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges) override final;

		void  UploadBuffer (BufferID buffer, Bytes offset, Bytes size, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static) override final;
		void  UploadImage  (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView) override final;

		void  UploadBuffer (BufferStream &stream, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static) override final;
		void  UploadImage (ImageStream &stream, OUT ImageMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static) override final;

		ND_ Promise<BufferMemView>	ReadbackBuffer (BufferID buffer, Bytes offset, Bytes size, EStagingHeapType heapType = EStagingHeapType::Static) override final;
		ND_ Promise<ImageMemView>   ReadbackImage (ImageID image, const ReadbackImageDesc &desc) override final;
		
		ND_ bool  MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped) override final;
		ND_ bool  UpdateHostBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data) override final;

		ND_ Promise<BufferMemView>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size) override final;
		
		void  GenerateMipmaps (ImageID srcImage) override final;

		void  CommitBarriers ()									override final	{ RawCtx::_CommitBarriers(); }
		
		void  DebugMarker (NtStringView text, RGBA8u color)		override final	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)	override final	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()									override final	{ RawCtx::_PopDebugGroup(); }
		
		using ITransferContext::UpdateHostBuffer;
		using ITransferContext::UploadBuffer;
		using ITransferContext::UploadImage;

		ND_ AccumBar  AccumBarriers ()							{ return AccumBar{ *this }; }

		MBARRIERMNGR_INHERIT_BARRIERS
			
	protected:
		ND_ bool  _IsHostMemory (BufferID buffer);
		ND_ bool  _IsHostMemory (ImageID image);

		ND_ bool  _IsDeviceMemory (BufferID buffer);
		ND_ bool  _IsDeviceMemory (ImageID image);
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using MDirectTransferContext	= _hidden_::_MTransferContextImpl< _hidden_::_MDirectTransferCtx >;
	using MIndirectTransferContext	= _hidden_::_MTransferContextImpl< _hidden_::_MIndirectTransferCtx >;

} // AE::Graphics

#endif // AE_ENABLE_METAL
