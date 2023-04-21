// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/CommandBuffer.h"

# ifdef AE_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4100)
#	pragma warning (disable: 4189)
# endif

namespace AE::Graphics::_hidden_
{

	//
	// Transfer Context Validation
	//
	class TransferContextValidation final : Noninstancable
	{
	public:
		template <typename ImgType>
		static void  ClearColorImage (ImgType &img, ArrayView<ImageSubresourceRange> ranges)
		{
			ASSERT( ranges.size() );
			ASSERT( AllBits( img.Description().usage, EImageUsage::TransferDst ));
			ASSERT( _IsDeviceMemory( img ));
		}

		template <typename ImgType>
		static void  ClearDepthStencilImage (ImgType &img, ArrayView<ImageSubresourceRange> ranges)
		{
			ASSERT( ranges.size() );
			ASSERT( AllBits( img.Description().usage, EImageUsage::TransferDst ));
			ASSERT( _IsDeviceMemory( img ));
		}

		template <typename BufType>
		static void  FillBuffer (BufType &buf, Bytes offset, Bytes size)
		{
			ASSERT( _IsDeviceMemory( buf ));
			ASSERT( AllBits( buf.Description().usage, EBufferUsage::TransferDst ));
			ASSERT( offset < buf.Size() );
			ASSERT( size == UMax or (offset + size <= buf.Size()) );
		}

		template <typename BufType>
		static void  UpdateBuffer (BufType &buf, Bytes offset, Bytes size, const void* data)
		{
			ASSERT( _IsDeviceMemory( buf ));
			ASSERT( AllBits( buf.Description().usage, EBufferUsage::TransferDst ));
			ASSERT( size > 0_b );
			ASSERT( data != null );
			ASSERT( offset < buf.Size() );
			ASSERT( size == UMax or (offset + size <= buf.Size()) );
		}

		template <typename BufType>
		static void  UploadBuffer (BufType &buf, Bytes offset, Bytes size, OUT BufferMemView &memView)
		{
			ASSERT( _IsDeviceMemory( buf ));
			ASSERT( AllBits( buf.Description().usage, EBufferUsage::TransferDst ));
		
			ASSERT( offset < buf.Size() );
			ASSERT( size == UMax or (offset + size <= buf.Size()) );
			ASSERT( memView.Empty() );
		}

		template <typename ImgType>
		static void  UploadImage (ImgType &img)
		{
			ASSERT( _IsDeviceMemory( img ));
			ASSERT( AllBits( img.Description().usage, EImageUsage::TransferDst ));
		}
		
		template <typename BufType>
		static void  ReadbackBuffer (BufType &buf, Bytes offset, Bytes size)
		{
			ASSERT( _IsDeviceMemory( buf ));
			ASSERT( AllBits( buf.Description().usage, EBufferUsage::TransferSrc ));

			ASSERT( offset < buf.Size() );
			ASSERT( size == UMax or (offset + size <= buf.Size()) );
		}
		
		template <typename ImgType>
		static void  ReadbackImage (ImgType &img)
		{
			ASSERT( _IsDeviceMemory( img ));
			ASSERT( AllBits( img.Description().usage, EImageUsage::TransferSrc ));
		}

		template <typename BufType>
		static void  MapHostBuffer (BufType &buf, Bytes offset, Bytes size)
		{
			ASSERT( _IsHostMemory( buf ));
			ASSERT( offset < buf.Size() );
			ASSERT( size == UMax or (offset + size <= buf.Size()) );
		}

		template <typename BufType>
		static void  CopyBuffer (BufType &src_buf, BufType &dst_buf, ArrayView<BufferCopy> ranges)
		{
		#ifdef AE_DEBUG
			ASSERT( ranges.size() );
			ASSERT( AllBits( src_buf.Description().usage, EBufferUsage::TransferSrc ));
			ASSERT( AllBits( dst_buf.Description().usage, EBufferUsage::TransferDst ));
			
			const Bytes		src_size	= src_buf.Size();
			const Bytes		dst_size	= dst_buf.Size();

			for (auto& range : ranges)
			{
				ASSERT( All( range.srcOffset < src_size ));
				ASSERT( All( range.dstOffset < dst_size ));
				ASSERT( All( (range.srcOffset + range.size) <= src_size ));
				ASSERT( All( (range.dstOffset + range.size) <= dst_size ));
			}
		#endif
		}
		
		template <typename ImgType>
		static void  CopyImage (ImgType &src_img, ImgType &dst_img, ArrayView<ImageCopy> ranges)
		{
		#ifdef AE_DEBUG
			ASSERT( ranges.size() );

			const ImageDesc &	src_desc	= src_img.Description();
			const ImageDesc &	dst_desc	= dst_img.Description();

			ASSERT( AllBits( src_desc.usage, EImageUsage::TransferSrc ));
			ASSERT( AllBits( dst_desc.usage, EImageUsage::TransferDst ));

			for (auto& range : ranges)
			{
				ASSERT( All( range.srcOffset < src_img.Dimension() ));
				ASSERT( All( (range.srcOffset + range.extent) <= src_img.Dimension() ));
				ASSERT( All( range.dstOffset < dst_img.Dimension() ));
				ASSERT( All( (range.dstOffset + range.extent) <= dst_img.Dimension() ));
			}
		#endif
		}
		
		template <typename BufType, typename ImgType>
		static void  CopyBufferToImage (BufType &src_buf, ImgType &dst_img, ArrayView<BufferImageCopy> ranges)
		{
			ASSERT( ranges.size() );
			ASSERT( AllBits( src_buf.Description().usage, EBufferUsage::TransferSrc ));
			ASSERT( AllBits( dst_img.Description().usage, EImageUsage::TransferDst ));
		}

		template <typename BufType, typename ImgType>
		static void  CopyBufferToImage (BufType &src_buf, ImgType &dst_img, ArrayView<BufferImageCopy2> ranges)
		{
			ASSERT( ranges.size() );
			ASSERT( AllBits( src_buf.Description().usage, EBufferUsage::TransferSrc ));
			ASSERT( AllBits( dst_img.Description().usage, EImageUsage::TransferDst ));
		}

		template <typename BufType, typename ImgType>
		static void  CopyImageToBuffer (BufType &src_img, ImgType &dst_buf, ArrayView<BufferImageCopy> ranges)
		{
			ASSERT( ranges.size() );
			ASSERT( AllBits( src_img.Description().usage, EImageUsage::TransferSrc ));
			ASSERT( AllBits( dst_buf.Description().usage, EBufferUsage::TransferDst ));
		}

		template <typename BufType, typename ImgType>
		static void  CopyImageToBuffer (BufType &src_img, ImgType &dst_buf, ArrayView<BufferImageCopy2> ranges)
		{
			ASSERT( ranges.size() );
			ASSERT( AllBits( src_img.Description().usage, EImageUsage::TransferSrc ));
			ASSERT( AllBits( dst_buf.Description().usage, EBufferUsage::TransferDst ));
		}
		
		template <typename ImgType>
		static void  BlitImage (ImgType &src_img, ImgType &dst_img, ArrayView<ImageBlit> ranges)
		{
		#ifdef AE_DEBUG
			ASSERT( ranges.size() );
			ASSERT( _IsDeviceMemory( src_img ));
			ASSERT( _IsDeviceMemory( dst_img ));

			const ImageDesc &	src_desc	= src_img.Description();
			const ImageDesc &	dst_desc	= dst_img.Description();

			ASSERT( AllBits( src_desc.usage, EImageUsage::TransferSrc ));
			ASSERT( AllBits( dst_desc.usage, EImageUsage::TransferDst ));
			ASSERT( AllBits( src_desc.options, EImageOpt::BlitSrc ));
			ASSERT( AllBits( dst_desc.options, EImageOpt::BlitDst ));
			ASSERT( not src_desc.samples.IsEnabled() );
			ASSERT( not dst_desc.samples.IsEnabled() );

			for (auto& range : ranges)
			{
				ASSERT( All( range.srcOffset0 <= src_desc.dimension ));
				ASSERT( All( range.srcOffset1 <= src_desc.dimension ));
				ASSERT( All( range.dstOffset0 <= dst_desc.dimension ));
				ASSERT( All( range.dstOffset1 <= dst_desc.dimension ));
			}
		#endif
		}

		template <typename ImgType>
		static void  ResolveImage (ImgType &src_img, ImgType &dst_img, ArrayView<ImageResolve> ranges)
		{
		#ifdef AE_DEBUG
			ASSERT( ranges.size() );
			ASSERT( _IsDeviceMemory( src_img ));
			ASSERT( _IsDeviceMemory( dst_img ));

			const ImageDesc &	src_desc	= src_img.Description();
			const ImageDesc &	dst_desc	= dst_img.Description();
			
			ASSERT( AllBits( src_desc.usage, EImageUsage::TransferSrc ));
			ASSERT( AllBits( dst_desc.usage, EImageUsage::TransferDst ));
			ASSERT( not src_desc.samples.IsEnabled() );
			ASSERT( dst_desc.samples.IsEnabled() );
			ASSERT( src_desc.format == dst_desc.format );

			for (auto& range : ranges)
			{
				ASSERT( All( range.srcOffset < src_desc.dimension ));
				ASSERT( All( range.srcOffset + range.extent <= src_desc.dimension ));
				ASSERT( All( range.dstOffset < dst_desc.dimension ));
				ASSERT( All( range.dstOffset + range.extent <= dst_desc.dimension ));
			}
		#endif
		}
		
		template <typename ImgType>
		static void  GenerateMipmaps (ImgType &img)
		{
			ASSERT( _IsDeviceMemory( img ));
			ASSERT( AllBits( img.Description().usage, EImageUsage::Transfer ));
		}


	private:
		template <typename T>
		ND_ static bool  _IsHostMemory (const T &res)	{ return AnyBits( res.Description().memType, EMemoryType::HostCachedCocherent ); }
		
		template <typename T>
		ND_ static bool  _IsDeviceMemory (const T &res)	{ return AnyBits( res.Description().memType, EMemoryType::DeviceLocal ); }
	};



	//
	// Acceleration Structure Build Context Validation
	//
	class ASBuildContextValidation final : Noninstancable
	{
	public:
		template <typename BufType>
		static void  SerializeToMemory (BufType &dst_buf, Bytes dstOffset)
		{
			ASSERT( dst_buf.Size() > dstOffset );
			ASSERT( dst_buf.HasDeviceAddress() );	// TODO: throw?
		}
		
		template <typename BufType>
		static void  DeserializeFromMemory (BufType &src_buf, Bytes srcOffset)
		{
			ASSERT( src_buf.Size() > srcOffset );
			ASSERT( src_buf.HasDeviceAddress() );	// TODO: throw?
		}
	};



	//
	// Compute Context Validation
	//
	class ComputeContextValidation final : Noninstancable
	{
	public:
		static void  PushConstant (Bytes size)
		{
			ASSERT( IsAligned( size, sizeof(uint) ));
		}

		template <typename BufType>
		static void  DispatchIndirect (BufType &buf, Bytes offset)
		{
			ASSERT( buf.Size() >= offset + sizeof(DispatchIndirectCommand) );
		}
	};



	//
	// Draw Context Validation
	//
	class DrawContextValidation final : Noninstancable
	{
	public:
		using Viewport_t = RenderPassDesc::Viewport;

	public:
		static void  PushConstant (Bytes size)
		{
			ASSERT( IsAligned( size, sizeof(uint) ));
		}

		static void  SetViewports (ArrayView<Viewport_t> viewports)
		{
			ASSERT( not viewports.empty() );
		}

		static void  SetScissors (ArrayView<RectI> scissors)
		{
			ASSERT( not scissors.empty() );
		}
		
		static void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)
		{
			ASSERT( buffers.size() == offsets.size() );
			ASSERT( buffers.size() <= GraphicsConfig::MaxVertexBuffers );
			Unused( firstBinding );
		}

		static void  ClearAttachment (const RectI &rect)
		{
			ASSERT( rect.IsValid() );
		}
	};



	//
	// Graphics Context Validation
	//
	class GraphicsContextValidation final : Noninstancable
	{
	public:
	};



	//
	// Ray Tracing Context Validation
	//
	class RayTracingContextValidation final : Noninstancable
	{
	public:
		static void  PushConstant (Bytes size)
		{
			ASSERT( IsAligned( size, sizeof(uint) ));
		}

		static void  TraceRays (const uint3 dim)
		{
			ASSERT( All( dim > uint3{0} ));
		}
		
		template <typename BufType>
		static void  TraceRaysIndirect (BufType &buf, Bytes indirectBufferOffset)
		{
			ASSERT( buf.Size() <= indirectBufferOffset + sizeof(TraceRayIndirectCommand) );
		}

		template <typename BufType>
		static void  TraceRaysIndirect2 (BufType &buf, Bytes indirectBufferOffset)
		{
			ASSERT( buf.Size() <= indirectBufferOffset + sizeof(TraceRayIndirectCommand2) );
		}
	};



	//
	// Video Decode Context Validation
	//
	class VideoDecodeContextValidation final : Noninstancable
	{
	public:
	};



	//
	// Video Encode Context Validation
	//
	class VideoEncodeContextValidation final : Noninstancable
	{
	public:
	};



} // AE::Graphics::_hidden_

# ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
# endif
