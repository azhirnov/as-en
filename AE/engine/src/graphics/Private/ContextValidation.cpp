// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Private/ContextValidation.h"
#include "graphics/GraphicsImpl.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VDevice.h"
#endif

#if AE_VALIDATE_GCTX
# include "graphics/Private/EnumUtils.h"
#endif

namespace AE::Graphics::_hidden_
{
namespace
{
	static constexpr FeatureSet::EFeature	True = FeatureSet::EFeature::RequireTrue;


	ND_ static FeatureSet const&  _GetFeatureSet () __NE___ {
		return GraphicsScheduler().GetFeatureSet();
	}

/*
=================================================
	AccelerationStructureSupported
=================================================
*/
	ND_ static bool  AccelerationStructureSupported ()	__NE___ {
		return _GetFeatureSet().accelerationStructure() == True;
	}

	ND_ static bool  RayTracingCtxSupported ()			__NE___ {
		return _GetFeatureSet().rayTracingPipeline == True;
	}

	ND_ static bool  VideoDecodeSupported ()			__NE___	{ return false; }
	ND_ static bool  VideoEncodeSupported ()			__NE___	{ return false; }

} // namespace
//-----------------------------------------------------------------------------



	void  TransferContextValidation::CtxInit (EQueueMask rtaskQueue) __Th___
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute | EQueueMask::AsyncTransfer, rtaskQueue ));
	}

	void  ComputeContextValidation::CtxInit (EQueueMask rtaskQueue) __Th___
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute, rtaskQueue ));
	}

	void  GraphicsContextValidation::CtxInit (EQueueMask rtaskQueue) __Th___
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics, rtaskQueue ));
	}

	void  RayTracingContextValidation::CtxInit (EQueueMask rtaskQueue) __Th___
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute, rtaskQueue ));
		CHECK_THROW( RayTracingCtxSupported() );
	}

	void  VideoDecodeContextValidation::CtxInit (EQueueMask rtaskQueue) __Th___
	{
		CHECK_THROW( AnyBits( EQueueMask::VideoDecode, rtaskQueue ));
		CHECK_THROW( VideoDecodeSupported() );
	}

	void  VideoEncodeContextValidation::CtxInit (EQueueMask rtaskQueue) __Th___
	{
		CHECK_THROW( AnyBits( EQueueMask::VideoEncode, rtaskQueue ));
		CHECK_THROW( VideoEncodeSupported() );
	}

	void  ASBuildContextValidation::CtxInit (EQueueMask rtaskQueue) __Th___
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute, rtaskQueue ));
		CHECK_THROW( AccelerationStructureSupported() );
	}
//-----------------------------------------------------------------------------



#if AE_VALIDATE_GCTX
namespace
{
	template <typename T>
	ND_ static bool  IsDeviceMemory (const T &desc)	__NE___	{ return EMemoryType_IsDeviceLocal( desc.memType ); }


	ND_ static bool  IsColorFormat (EPixelFormat fmt) __Th___
	{
		const auto&	info = EPixelFormat_GetInfo( fmt );
		GCTX_CHECK( info.IsValid() );
		return info.IsColor();
	}

	ND_ static bool  IsDepthOrStencilFormat (EPixelFormat fmt) __Th___
	{
		const auto&	info = EPixelFormat_GetInfo( fmt );
		GCTX_CHECK( info.IsValid() );
		return info.HasDepthOrStencil();
	}

	ND_ static uint3  MipmapDimension (const ImageDesc &desc, MipmapLevel mip)
	{
		return ImageUtils::MipmapDimension( desc.Dimension(), mip.Get(), EPixelFormat_GetInfo( desc.format ).TexBlockDim() );
	}

	static void  ValidateImageSubresourceLayers (const ImageDesc &desc, const ImageSubresourceLayers &subres, const uint3 &offset, const uint3 &extent) __Th___
	{
		const uint3	dim = MipmapDimension( desc, subres.mipLevel );

		GCTX_CHECK( All( offset < dim ));
		GCTX_CHECK( All( (offset + extent) <= dim ));

		GCTX_CHECK( subres.mipLevel < desc.mipLevels );
		GCTX_CHECK( subres.baseLayer < desc.arrayLayers );
		GCTX_CHECK( subres.aspectMask != Default );
	}

	ND_ static bool  BuildIndirectSupported ()				__NE___	{ return _GetFeatureSet().accelerationStructureIndirectBuild == True; }

	ND_ inline bool  MultiViewportSupported ()				__NE___	{ return _GetFeatureSet().multiViewport == True; }
	ND_ static bool  DepthBiasClampSupported ()				__NE___	{ return _GetFeatureSet().depthBiasClamp == True; }
	ND_ static bool  DepthBoundsSupported ()				__NE___	{ return _GetFeatureSet().depthBounds == True; }

	//ND_ inline bool  DrawIndirectFirstInstanceSupported ()	__NE___	{ return _GetFeatureSet().drawIndirectFirstInstance == True; }	// TODO
	ND_ static bool  DrawIndirectCountSupported ()			__NE___	{ return _GetFeatureSet().drawIndirectCount == True; }
	ND_ static bool  MeshShaderSupported ()					__NE___	{ return _GetFeatureSet().meshShader == True; }
	ND_ inline bool  TileShaderSupported ()					__NE___	{ return _GetFeatureSet().tileShader == True; }

	ND_ static bool  FragmentShadingRateSupported ()		__NE___
	{
		auto&	fs = _GetFeatureSet();
		return	fs.pipelineFragmentShadingRate		== True	or
				fs.primitiveFragmentShadingRate		== True	or
				fs.attachmentFragmentShadingRate	== True;
	}

	ND_ static bool  ViewportWScalingSupported ()			__NE___	{ return _GetFeatureSet().clipSpaceWScalingNV == True; }

#ifdef AE_ENABLE_VULKAN
	ND_ static auto const&  _GetDeviceExtensions () __NE___ {
		return GraphicsScheduler().GetDevice().GetVExtensions();
	}

	ND_ static auto const&  _GetVkDeviceProperties () __NE___ {
		return GraphicsScheduler().GetDevice().GetVProperties();
	}
#endif

	ND_ static auto const&  _GetRayTracingProps () __NE___ {
		return GraphicsScheduler().GetDevice().GetDeviceProperties().rayTracing;
	}

	ND_ static auto const&  _GetResourceProps () __NE___ {
		return GraphicsScheduler().GetDevice().GetDeviceProperties().res;
	}

} // namespace
//-----------------------------------------------------------------------------



#if 1
/*
=================================================
	ClearColorImage
=================================================
*/
	void  TransferContextValidation::ClearColorImage (const ImageDesc &imgDesc, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( AllBits( imgDesc.usage, EImageUsage::TransferDst ));
		GCTX_CHECK( IsDeviceMemory( imgDesc ));
		GCTX_CHECK( IsColorFormat( imgDesc.format ));

		for (auto& range : ranges)
		{
			GCTX_CHECK( range.baseMipLevel < imgDesc.mipLevels );
			GCTX_CHECK( range.baseLayer < imgDesc.arrayLayers );
		}
	}

# ifdef AE_ENABLE_VULKAN
	void  TransferContextValidation::ClearColorImage (VkImage image, ArrayView<VkImageSubresourceRange> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( image != Default );

		for (auto& range : ranges) {
			GCTX_CHECK( range.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT );
		}
	}
# endif

/*
=================================================
	ClearDepthStencilImage
=================================================
*/
	void  TransferContextValidation::ClearDepthStencilImage (const ImageDesc &imgDesc, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( AllBits( imgDesc.usage, EImageUsage::TransferDst ));
		GCTX_CHECK( IsDeviceMemory( imgDesc ));
		GCTX_CHECK( IsDepthOrStencilFormat( imgDesc.format ));

		for (auto& range : ranges)
		{
			GCTX_CHECK( range.baseMipLevel < imgDesc.mipLevels );
			GCTX_CHECK( range.baseLayer < imgDesc.arrayLayers );
		}
	}

# ifdef AE_ENABLE_VULKAN
	void  TransferContextValidation::ClearDepthStencilImage (VkImage image, ArrayView<VkImageSubresourceRange> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( image != Default );

		for (auto& range : ranges) {
			GCTX_CHECK( NoBits( range.aspectMask, ~(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT) ));
		}
	}
# endif

/*
=================================================
	FillBuffer
=================================================
*/
	void  TransferContextValidation::FillBuffer (const BufferDesc &bufDesc, Bytes offset, Bytes size) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( bufDesc ));
		GCTX_CHECK( AllBits( bufDesc.usage, EBufferUsage::TransferDst ));
		GCTX_CHECK( IsMultipleOf( offset, 4 ));
		GCTX_CHECK( offset < bufDesc.size );
		GCTX_CHECK( size == UMax or (offset + size <= bufDesc.size) );
		GCTX_CHECK( size == UMax or IsMultipleOf( size, 4 ));
	}

# ifdef AE_ENABLE_VULKAN
	void  TransferContextValidation::FillBuffer (VkBuffer buffer, Bytes offset, Bytes size) __Th___
	{
		GCTX_CHECK( buffer != Default );
		GCTX_CHECK( IsMultipleOf( offset, 4 ));
		GCTX_CHECK( IsMultipleOf( size, 4 ));
	}
# endif

/*
=================================================
	UpdateBuffer
=================================================
*/
	void  TransferContextValidation::UpdateBuffer (const BufferDesc &bufDesc, Bytes offset, Bytes size, const void* data) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( bufDesc ));
		GCTX_CHECK( AllBits( bufDesc.usage, EBufferUsage::TransferDst ));
		GCTX_CHECK( size > 0_b );
		GCTX_CHECK( data != null );
		GCTX_CHECK( IsMultipleOf( offset, 4 ));
		GCTX_CHECK( offset < bufDesc.size );
		GCTX_CHECK( size == UMax or (offset + size <= bufDesc.size) );
		GCTX_CHECK( size == UMax or IsMultipleOf( size, 4 ));
	}

# ifdef AE_ENABLE_VULKAN
	void  TransferContextValidation::UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data) __Th___
	{
		GCTX_CHECK( buffer != Default );
		GCTX_CHECK( data != null );
		GCTX_CHECK( IsMultipleOf( offset, 4 ));
		GCTX_CHECK( IsMultipleOf( size, 4 ));
		GCTX_CHECK( size <= 65536 );
	}
# endif

/*
=================================================
	UploadBuffer
=================================================
*/
	void  TransferContextValidation::UploadBuffer (const BufferDesc &bufDesc, Bytes offset, Bytes size, BufferMemView &memView) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( bufDesc ));
		GCTX_CHECK( AllBits( bufDesc.usage, EBufferUsage::TransferDst ));

		GCTX_CHECK( offset < bufDesc.size );
		GCTX_CHECK( size == UMax or (offset + size <= bufDesc.size) );

		ASSERT( memView.Empty() );
		Unused( memView );
	}

/*
=================================================
	UploadImage
=================================================
*/
	void  TransferContextValidation::UploadImage (const ImageDesc &imgDesc) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( imgDesc ));
		GCTX_CHECK( AllBits( imgDesc.usage, EImageUsage::TransferDst ));

		// TODO: more checks
	}

/*
=================================================
	UploadImage
=================================================
*/
	void  TransferContextValidation::UploadImage (const VideoImageDesc &imgDesc) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( imgDesc ));
		GCTX_CHECK( AllBits( imgDesc.usage, EImageUsage::TransferDst ));

		// TODO: more checks
	}

/*
=================================================
	ReadbackBuffer
=================================================
*/
	void  TransferContextValidation::ReadbackBuffer (const BufferDesc &bufDesc, Bytes offset, Bytes size) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( bufDesc ));
		GCTX_CHECK( AllBits( bufDesc.usage, EBufferUsage::TransferSrc ));

		GCTX_CHECK( offset < bufDesc.size );
		GCTX_CHECK( size == UMax or (offset + size <= bufDesc.size) );
	}

/*
=================================================
	ReadbackImage
=================================================
*/
	void  TransferContextValidation::ReadbackImage (const ImageDesc &imgDesc) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( imgDesc ));
		GCTX_CHECK( AllBits( imgDesc.usage, EImageUsage::TransferSrc ));

		// TODO: more checks
	}

/*
=================================================
	ReadbackImage
=================================================
*/
	void  TransferContextValidation::ReadbackImage (const VideoImageDesc &imgDesc) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( imgDesc ));
		GCTX_CHECK( AllBits( imgDesc.usage, EImageUsage::TransferSrc ));

		// TODO: more checks
	}

/*
=================================================
	MapHostBuffer
=================================================
*/
	void  TransferContextValidation::MapHostBuffer (const BufferDesc &bufDesc, Bytes offset, Bytes size) __Th___
	{
		GCTX_CHECK( EMemoryType_IsHostVisible( bufDesc.memType ));
		GCTX_CHECK( offset < bufDesc.size );
		GCTX_CHECK( size == UMax or (offset + size <= bufDesc.size) );
	}

/*
=================================================
	CopyBuffer
=================================================
*/
	void  TransferContextValidation::CopyBuffer (const BufferDesc &srcBufferDesc, const BufferDesc &dstBufferDesc, ArrayView<BufferCopy> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( AllBits( srcBufferDesc.usage, EBufferUsage::TransferSrc ));
		GCTX_CHECK( AllBits( dstBufferDesc.usage, EBufferUsage::TransferDst ));

		for (auto& range : ranges)
		{
			GCTX_CHECK( All( range.srcOffset < srcBufferDesc.size ));
			GCTX_CHECK( All( range.dstOffset < dstBufferDesc.size ));
			GCTX_CHECK( All( (range.srcOffset + range.size) <= srcBufferDesc.size ));
			GCTX_CHECK( All( (range.dstOffset + range.size) <= dstBufferDesc.size ));
		}
	}

# ifdef AE_ENABLE_VULKAN
	void  TransferContextValidation::CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		Unused( ranges );
		GCTX_CHECK( srcBuffer != Default );
		GCTX_CHECK( dstBuffer != Default );
	}
# endif

/*
=================================================
	CopyImage
=================================================
*/
	void  TransferContextValidation::CopyImage (const ImageDesc &srcImageDesc, const ImageDesc &dstImageDesc, ArrayView<ImageCopy> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( AllBits( srcImageDesc.usage, EImageUsage::TransferSrc ));
		GCTX_CHECK( AllBits( dstImageDesc.usage, EImageUsage::TransferDst ));

		// TODO: queue transfer granularity

		for (auto& range : ranges)
		{
			ValidateImageSubresourceLayers( srcImageDesc, range.srcSubres, range.srcOffset, range.extent );
			ValidateImageSubresourceLayers( dstImageDesc, range.dstSubres, range.dstOffset, range.extent );

			GCTX_CHECK( range.srcSubres.aspectMask == range.dstSubres.aspectMask );	// TODO: multi-planar format

			uint2	src_gran {1};
			uint2	dst_gran {1};

			GCTX_CHECK( EPixelFormat_GetCopyGranularity( srcImageDesc.format, OUT src_gran, dstImageDesc.format, OUT dst_gran ));

			// TODO
		}
	}

# ifdef AE_ENABLE_VULKAN
	void  TransferContextValidation::CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		Unused( ranges );
		GCTX_CHECK( srcImage != Default );
		GCTX_CHECK( dstImage != Default );
	}
# endif

/*
=================================================
	CopyBufferToImage
=================================================
*/
	void  TransferContextValidation::CopyBufferToImage (const BufferDesc &srcBufferDesc, const ImageDesc &dstImageDesc, ArrayView<BufferImageCopy> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( AllBits( srcBufferDesc.usage, EBufferUsage::TransferSrc ));
		GCTX_CHECK( AllBits( dstImageDesc.usage, EImageUsage::TransferDst ));

		for (auto& range : ranges)
		{
			ValidateImageSubresourceLayers( dstImageDesc, range.imageSubres, range.imageOffset, range.imageExtent );

			GCTX_CHECK( range.bufferRowLength == 0 or range.bufferRowLength >= range.imageExtent.x );
			GCTX_CHECK( range.bufferImageHeight == 0 or range.bufferImageHeight >= range.imageExtent.y );
		}
	}

	void  TransferContextValidation::CopyBufferToImage (const BufferDesc &srcBufferDesc, const ImageDesc &dstImageDesc, ArrayView<BufferImageCopy2> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( AllBits( srcBufferDesc.usage, EBufferUsage::TransferSrc ));
		GCTX_CHECK( AllBits( dstImageDesc.usage, EImageUsage::TransferDst ));

		for (auto& range : ranges)
		{
			ValidateImageSubresourceLayers( dstImageDesc, range.imageSubres, range.imageOffset, range.imageExtent );

			GCTX_CHECK( range.rowPitch != 0_b );
			GCTX_CHECK( range.slicePitch > 0_b and IsMultipleOf( range.slicePitch, range.rowPitch ));
		}
	}

# ifdef AE_ENABLE_VULKAN
	void  TransferContextValidation::CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		Unused( ranges );
		GCTX_CHECK( srcBuffer != Default );
		GCTX_CHECK( dstImage != Default );
	}
# endif

/*
=================================================
	CopyImageToBuffer
=================================================
*/
	void  TransferContextValidation::CopyImageToBuffer (const ImageDesc &srcImageDesc, const BufferDesc &dstBufferDesc, ArrayView<BufferImageCopy> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( AllBits( srcImageDesc.usage, EImageUsage::TransferSrc ));
		GCTX_CHECK( AllBits( dstBufferDesc.usage, EBufferUsage::TransferDst ));

		for (auto& range : ranges)
		{
			ValidateImageSubresourceLayers( srcImageDesc, range.imageSubres, range.imageOffset, range.imageExtent );

			GCTX_CHECK( range.bufferRowLength == 0 or range.bufferRowLength >= range.imageExtent.x );
			GCTX_CHECK( range.bufferImageHeight == 0 or range.bufferImageHeight >= range.imageExtent.y );
		}
	}

	void  TransferContextValidation::CopyImageToBuffer (const ImageDesc &srcImageDesc, const BufferDesc &dstBufferDesc, ArrayView<BufferImageCopy2> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( AllBits( srcImageDesc.usage, EImageUsage::TransferSrc ));
		GCTX_CHECK( AllBits( dstBufferDesc.usage, EBufferUsage::TransferDst ));

		for (auto& range : ranges)
		{
			ValidateImageSubresourceLayers( srcImageDesc, range.imageSubres, range.imageOffset, range.imageExtent );

			GCTX_CHECK( range.rowPitch != 0_b );
			GCTX_CHECK( range.slicePitch > 0_b and IsMultipleOf( range.slicePitch, range.rowPitch ));
		}
	}

# ifdef AE_ENABLE_VULKAN
	void  TransferContextValidation::CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		Unused( ranges );
		GCTX_CHECK( srcImage != Default );
		GCTX_CHECK( dstBuffer != Default );
	}
# endif

/*
=================================================
	BlitImage
=================================================
*/
	void  TransferContextValidation::BlitImage (const ImageDesc &srcImageDesc, const ImageDesc &dstImageDesc, EBlitFilter blitFilter, ArrayView<ImageBlit> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( IsDeviceMemory( srcImageDesc ));
		GCTX_CHECK( IsDeviceMemory( dstImageDesc ));

		GCTX_CHECK( AllBits( srcImageDesc.usage, EImageUsage::TransferSrc ));
		GCTX_CHECK( AllBits( dstImageDesc.usage, EImageUsage::TransferDst ));
		GCTX_CHECK( AllBits( srcImageDesc.options, EImageOpt::BlitSrc ));
		GCTX_CHECK( AllBits( dstImageDesc.options, EImageOpt::BlitDst ));
		GCTX_CHECK( not srcImageDesc.samples.IsEnabled() );
		GCTX_CHECK( not dstImageDesc.samples.IsEnabled() );
		GCTX_CHECK( EPixelFormat_IsBlitSupported( srcImageDesc.format, dstImageDesc.format, blitFilter ));

		//if ( blitFilter == EBlitFilter::Linear )
		//	GCTX_CHECK( AllBits( srcImageDesc.options, EImageOpt::SampledLinear ));

		for (auto& range : ranges)
		{
			const uint3	src_dim = MipmapDimension( srcImageDesc, range.srcSubres.mipLevel );
			const uint3	dst_dim = MipmapDimension( dstImageDesc, range.dstSubres.mipLevel );

			GCTX_CHECK( All( range.srcOffset0 <= src_dim ));
			GCTX_CHECK( All( range.srcOffset1 <= src_dim ));

			GCTX_CHECK( All( range.dstOffset0 <= dst_dim ));
			GCTX_CHECK( All( range.dstOffset1 <= dst_dim ));
		}
	}

# ifdef AE_ENABLE_VULKAN
	void  TransferContextValidation::BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions) __Th___
	{
		ASSERT( not regions.empty() );
		Unused( regions );
		GCTX_CHECK( srcImage != Default );
		GCTX_CHECK( dstImage != Default );
		GCTX_CHECK( AnyEqual( filter, VK_FILTER_NEAREST, VK_FILTER_LINEAR ));
	}
# endif

/*
=================================================
	ResolveImage
=================================================
*/
	void  TransferContextValidation::ResolveImage (const ImageDesc &srcImageDesc, const ImageDesc &dstImageDesc, ArrayView<ImageResolve> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( IsDeviceMemory( srcImageDesc ));
		GCTX_CHECK( IsDeviceMemory( dstImageDesc ));

		GCTX_CHECK( AllBits( srcImageDesc.usage, EImageUsage::TransferSrc ));
		GCTX_CHECK( AllBits( dstImageDesc.usage, EImageUsage::TransferDst ));
		GCTX_CHECK( not srcImageDesc.samples.IsEnabled() );
		GCTX_CHECK( dstImageDesc.samples.IsEnabled() );
		GCTX_CHECK( srcImageDesc.format == dstImageDesc.format );

		for (auto& range : ranges)
		{
			const uint3		src_dim	= MipmapDimension( srcImageDesc, range.srcSubres.mipLevel );
			const uint3		dst_dim	= MipmapDimension( dstImageDesc, range.dstSubres.mipLevel );

			GCTX_CHECK( All( range.srcOffset < src_dim ));
			GCTX_CHECK( All( range.srcOffset + range.extent <= src_dim ));

			GCTX_CHECK( All( range.dstOffset < dst_dim ));
			GCTX_CHECK( All( range.dstOffset + range.extent <= dst_dim ));
		}
	}

# ifdef AE_ENABLE_VULKAN
	void  TransferContextValidation::ResolveImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageResolve> regions) __Th___
	{
		ASSERT( not regions.empty() );
		Unused( regions );
		GCTX_CHECK( srcImage != Default );
		GCTX_CHECK( dstImage != Default );
	}
# endif

/*
=================================================
	GenerateMipmaps
=================================================
*/
	void  TransferContextValidation::GenerateMipmaps (const ImageDesc &imgDesc, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		GCTX_CHECK( IsDeviceMemory( imgDesc ));
		GCTX_CHECK( AllBits( imgDesc.usage, EImageUsage::Transfer ));

		for (auto& range : ranges)
		{
			GCTX_CHECK( AllBits( imgDesc.options, EImageOpt::BlitSrc | EImageOpt::BlitDst ));
			GCTX_CHECK( range.aspectMask == EPixelFormat_ToImageAspect( imgDesc.format ));
			GCTX_CHECK( range.baseLayer.Get() < imgDesc.arrayLayers.Get() );
			GCTX_CHECK( range.layerCount + range.baseLayer.Get() <= imgDesc.arrayLayers.Get() );
			GCTX_CHECK( range.baseMipLevel.Get() < imgDesc.mipLevels.Get() );
			GCTX_CHECK( range.baseMipLevel.Get() + range.mipmapCount <= imgDesc.mipLevels.Get() );
			GCTX_CHECK( range.mipmapCount > 1 );
		}
	}

# ifdef AE_ENABLE_VULKAN
	void  TransferContextValidation::GenerateMipmaps (VkImage image, const uint3 &dimension, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		ASSERT( not ranges.empty() );
		Unused( ranges );
		GCTX_CHECK( image != Default );
		GCTX_CHECK( All( dimension > 0u ));
	}
# endif

#endif
//-----------------------------------------------------------------------------



#if 1
/*
=================================================
	PushConstant
=================================================
*/
	void  ComputeContextValidation::PushConstant (const PushConstantIndex &idx, Bytes size, ShaderStructName::Ref typeName) __Th___
	{
	#ifdef AE_DEBUG
		GCTX_CHECK( typeName == Default or idx.dbgTypeName == Default or idx.dbgTypeName == typeName );
		GCTX_CHECK( Bytes{idx.dbgDataSize} == size or idx.dbgDataSize == 0 );
	#else
		Unused( idx, size, typeName );
	#endif
	}

# ifdef AE_ENABLE_VULKAN
	void  ComputeContextValidation::PushConstant (VkPipelineLayout layout, Bytes offset, Bytes size, const void* values, EShaderStages stages) __Th___
	{
		GCTX_CHECK( size > 0 );
		GCTX_CHECK( IsMultipleOf( size, 4 ));
		GCTX_CHECK( IsMultipleOf( offset, 4 ));
		GCTX_CHECK( values != null );
		GCTX_CHECK( stages != Default );
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( AnyBits( stages, EShaderStages::Compute ));
		GCTX_CHECK( NoBits( stages, ~EShaderStages::Compute ));
	}
# endif

/*
=================================================
	BindDescriptorSet
=================================================
*/
# ifdef AE_ENABLE_VULKAN
	void  ComputeContextValidation::BindDescriptorSet (VkPipelineLayout layout, DescSetBinding index, VkDescriptorSet ds) __Th___
	{
		GCTX_CHECK( ds != Default );
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( index.vkIndex < _GetFeatureSet().maxDescriptorSets );
	}
# endif

/*
=================================================
	Dispatch
=================================================
*/
# ifdef AE_ENABLE_VULKAN
	void  ComputeContextValidation::Dispatch (VkPipelineLayout layout, const uint3 &groupCount) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( All( groupCount >= 1u ));
	}
# endif

/*
=================================================
	DispatchBase
=================================================
*/
# ifdef AE_ENABLE_VULKAN
	void  ComputeContextValidation::DispatchBase (VkPipelineLayout layout, const uint3 &baseGroup, const uint3 &groupCount) __Th___
	{
		Unused( baseGroup );
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( All( groupCount >= 1u ));
		GCTX_CHECK( _GetDeviceExtensions().deviceGroup );
	}
# endif

/*
=================================================
	DispatchIndirect
=================================================
*/
	void  ComputeContextValidation::DispatchIndirect (const BufferDesc &indirectBufferDesc, Bytes offset) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( indirectBufferDesc ));
		GCTX_CHECK( AllBits( indirectBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( offset, 4 ));
		GCTX_CHECK( (offset + sizeof(DispatchIndirectCommand)) <= indirectBufferDesc.size );
	}

# ifdef AE_ENABLE_VULKAN
	void  ComputeContextValidation::DispatchIndirect (VkPipelineLayout layout, VkBuffer buffer) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( buffer != Default );
	}
# endif

#endif
//-----------------------------------------------------------------------------



#if 1
/*
=================================================
	PushConstant
=================================================
*/
	void  DrawContextValidation::PushConstant (const PushConstantIndex &idx, Bytes size, ShaderStructName::Ref typeName) __Th___
	{
	#ifdef AE_DEBUG
		GCTX_CHECK( typeName == Default or idx.dbgTypeName == Default or idx.dbgTypeName == typeName );
		GCTX_CHECK( Bytes{idx.dbgDataSize} == size or idx.dbgDataSize == 0 );
	#else
		Unused( idx, size, typeName );
	#endif
	}

# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::PushConstant (VkPipelineLayout layout, Bytes offset, Bytes size, const void* values, EShaderStages stages) __Th___
	{
		GCTX_CHECK( size > 0 );
		GCTX_CHECK( IsMultipleOf( size, 4 ));
		GCTX_CHECK( IsMultipleOf( offset, 4 ));
		GCTX_CHECK( values != null );
		GCTX_CHECK( stages != Default );
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( AnyBits( stages, EShaderStages::AllGraphics ));
		GCTX_CHECK( NoBits( stages, ~EShaderStages::AllGraphics ));
	}
# endif

/*
=================================================
	BindDescriptorSet
=================================================
*/
# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::BindDescriptorSet (VkPipelineLayout layout, DescSetBinding index, VkDescriptorSet ds) __Th___
	{
		GCTX_CHECK( ds != Default );
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( index.vkIndex < _GetFeatureSet().maxDescriptorSets );
	}
# endif

/*
=================================================
	SetViewports
=================================================
*/
# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::SetViewport (uint first, ArrayView<VkViewport> viewports) __Th___
	{
		ASSERT( not viewports.empty() );
		GCTX_CHECK( MultiViewportSupported() or (first + viewports.size() == 1) );
	}
# endif

/*
=================================================
	SetScissors
=================================================
*/
# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::SetScissor (uint first, ArrayView<VkRect2D> scissors) __Th___
	{
		ASSERT( not scissors.empty() );
		GCTX_CHECK( MultiViewportSupported() or (first + scissors.size() == 1) );
	}
# endif

/*
=================================================
	BindVertexBuffers
=================================================
*/
	void  DrawContextValidation::BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets) __Th___
	{
		const uint		max_vb	= _GetFeatureSet().maxVertexBuffers;
		const Bytes		align	= _GetResourceProps().minVertexBufferOffsetAlign;

		GCTX_CHECK( not buffers.empty() and not offsets.empty() );
		GCTX_CHECK( buffers.size() == offsets.size() );
		GCTX_CHECK( firstBinding < max_vb );
		GCTX_CHECK( (firstBinding + buffers.size()) <= max_vb );

		for (auto off : offsets) {
			GCTX_CHECK( IsMultipleOf( off, align ));
		}
	}

# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<Bytes> offsets) __Th___
	{
		const uint		max_vb	= _GetFeatureSet().maxVertexBuffers;
		const Bytes		align	= _GetResourceProps().minVertexBufferOffsetAlign;

		GCTX_CHECK( not buffers.empty() and not offsets.empty() );
		GCTX_CHECK( buffers.size() == offsets.size() );
		GCTX_CHECK( firstBinding < max_vb );
		GCTX_CHECK( (firstBinding + buffers.size()) <= max_vb );

		for (auto buf : buffers) {
			GCTX_CHECK( buf != Default );
		}
		for (auto off : offsets) {
			GCTX_CHECK( IsMultipleOf( off, align ));
		}
	}
# endif

/*
=================================================
	BindIndexBuffer
=================================================
*/
# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::BindIndexBuffer (VkBuffer buffer, EIndex indexType) __Th___
	{
		GCTX_CHECK( buffer != Default );
		GCTX_CHECK( indexType < EIndex::_Count );
	}
# endif

/*
=================================================
	ClearAttachment
=================================================
*/
	void  DrawContextValidation::ClearAttachment (const RectI &rect) __Th___
	{
		GCTX_CHECK( rect.IsValid() );
	}

/*
=================================================
	Draw / DrawIndexed
=================================================
*/
# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::Draw (VkPipelineLayout layout) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
	}

	void  DrawContextValidation::DrawIndexed (VkPipelineLayout layout) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
	}
# endif

/*
=================================================
	DrawIndirect
=================================================
*/
	void  DrawContextValidation::DrawIndirect (const BufferDesc &	indirectBufferDesc,
											   Bytes				indirectBufferOffset,
											   uint					drawCount,
											   Bytes				stride) __Th___
	{
		DrawIndirect( drawCount, stride );

		GCTX_CHECK( IsDeviceMemory( indirectBufferDesc ));
		GCTX_CHECK( AllBits( indirectBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( indirectBufferOffset, 4 ));
		GCTX_CHECK( indirectBufferOffset < indirectBufferDesc.size );
		GCTX_CHECK( (indirectBufferOffset + drawCount * stride) <= indirectBufferDesc.size );
	}

	void  DrawContextValidation::DrawIndirect (uint drawCount, Bytes stride) __Th___
	{
		ASSERT( drawCount > 0 );
		Unused( drawCount );
		GCTX_CHECK( stride >= SizeOf<DrawIndirectCommand> );
		GCTX_CHECK( IsMultipleOf( stride, 4 ));
	}

# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::DrawIndirect (VkPipelineLayout	layout,
											   VkBuffer			indirectBuffer,
											   uint				drawCount,
											   Bytes			stride) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( indirectBuffer != Default );

		DrawIndirect( drawCount, stride );
	}
# endif

/*
=================================================
	DrawIndexedIndirect
=================================================
*/
	void  DrawContextValidation::DrawIndexedIndirect (const BufferDesc &	indirectBufferDesc,
													  Bytes					indirectBufferOffset,
													  uint					drawCount,
													  Bytes					stride) __Th___
	{
		DrawIndexedIndirect( drawCount, stride );

		GCTX_CHECK( IsDeviceMemory( indirectBufferDesc ));
		GCTX_CHECK( AllBits( indirectBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( indirectBufferOffset, 4 ));
		GCTX_CHECK( indirectBufferOffset < indirectBufferDesc.size );
		GCTX_CHECK( (indirectBufferOffset + drawCount * stride) <= indirectBufferDesc.size );
	}

	void  DrawContextValidation::DrawIndexedIndirect (uint drawCount, Bytes stride) __Th___
	{
		ASSERT( drawCount > 0 );
		Unused( drawCount );
		GCTX_CHECK( stride >= SizeOf<DrawIndexedIndirectCommand> );
		GCTX_CHECK( IsMultipleOf( stride, 4 ));
	}

# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::DrawIndexedIndirect (VkPipelineLayout	layout,
													  VkBuffer			indirectBuffer,
													  uint				drawCount,
													  Bytes				stride) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( indirectBuffer != Default );

		DrawIndexedIndirect( drawCount, stride );
	}
# endif

/*
=================================================
	DrawIndirectCount
=================================================
*/
	void  DrawContextValidation::DrawIndirectCount (const BufferDesc &	indirectBufferDesc,
													Bytes				indirectBufferOffset,
													const BufferDesc &	countBufferDesc,
													Bytes				countBufferOffset,
													uint				maxDrawCount,
													Bytes				stride) __Th___
	{
		DrawIndirectCount( maxDrawCount, stride );

		GCTX_CHECK( IsDeviceMemory( indirectBufferDesc ));
		GCTX_CHECK( AllBits( indirectBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( indirectBufferOffset, 4 ));
		GCTX_CHECK( indirectBufferOffset < indirectBufferDesc.size );
		GCTX_CHECK( (indirectBufferOffset + maxDrawCount * stride) <= indirectBufferDesc.size );

		GCTX_CHECK( IsDeviceMemory( countBufferDesc ));
		GCTX_CHECK( AllBits( countBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( countBufferOffset, 4 ));
		GCTX_CHECK( countBufferOffset < countBufferDesc.size );
		GCTX_CHECK( (countBufferOffset + SizeOf<uint>) <= countBufferDesc.size );
	}

	void  DrawContextValidation::DrawIndirectCount (uint maxDrawCount, Bytes stride) __Th___
	{
		ASSERT( maxDrawCount > 0 );
		Unused( maxDrawCount );
		GCTX_CHECK( DrawIndirectCountSupported() );
		GCTX_CHECK( stride >= SizeOf<DrawIndirectCommand> );
		GCTX_CHECK( IsMultipleOf( stride, 4 ));
	}

# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::DrawIndirectCount (VkPipelineLayout	layout,
													VkBuffer			indirectBuffer,
													VkBuffer			countBuffer,
													uint				maxDrawCount,
													Bytes				stride) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( indirectBuffer != Default );
		GCTX_CHECK( countBuffer != Default );
		DrawIndirectCount( maxDrawCount, stride );
	}
# endif

/*
=================================================
	DrawIndexedIndirectCount
=================================================
*/
	void  DrawContextValidation::DrawIndexedIndirectCount (const BufferDesc &	indirectBufferDesc,
														   Bytes				indirectBufferOffset,
														   const BufferDesc &	countBufferDesc,
														   Bytes				countBufferOffset,
														   uint					maxDrawCount,
														   Bytes				stride) __Th___
	{
		DrawIndexedIndirectCount( maxDrawCount, stride );

		GCTX_CHECK( IsDeviceMemory( indirectBufferDesc ));
		GCTX_CHECK( AllBits( indirectBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( indirectBufferOffset, 4 ));
		GCTX_CHECK( indirectBufferOffset < indirectBufferDesc.size );
		GCTX_CHECK( (indirectBufferOffset + maxDrawCount * stride) <= indirectBufferDesc.size );

		GCTX_CHECK( IsDeviceMemory( countBufferDesc ));
		GCTX_CHECK( AllBits( countBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( countBufferOffset, 4 ));
		GCTX_CHECK( countBufferOffset < countBufferDesc.size );
		GCTX_CHECK( (countBufferOffset + SizeOf<uint>) <= countBufferDesc.size );
	}

	void  DrawContextValidation::DrawIndexedIndirectCount (uint maxDrawCount, Bytes stride) __Th___
	{
		ASSERT( maxDrawCount > 0 );
		Unused( maxDrawCount );
		GCTX_CHECK( DrawIndirectCountSupported() );
		GCTX_CHECK( stride >= SizeOf<DrawIndexedIndirectCommand> );
		GCTX_CHECK( IsMultipleOf( stride, 4 ));
	}

# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::DrawIndexedIndirectCount (VkPipelineLayout	layout,
														   VkBuffer			indirectBuffer,
														   VkBuffer			countBuffer,
														   uint				maxDrawCount,
														   Bytes			stride) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( indirectBuffer != Default );
		GCTX_CHECK( countBuffer != Default );

		DrawIndexedIndirectCount( maxDrawCount, stride );
	}
# endif

/*
=================================================
	DrawMeshTasks
=================================================
*/
	void  DrawContextValidation::DrawMeshTasks (const uint3 &taskCount) __Th___
	{
		ASSERT( All( taskCount >= 1u ));
		Unused( taskCount );
		GCTX_CHECK( MeshShaderSupported() );
	}

# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::DrawMeshTasks (VkPipelineLayout layout, const uint3 &taskCount) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		DrawMeshTasks( taskCount );
	}
# endif

/*
=================================================
	DrawMeshTasksIndirect
=================================================
*/
	void  DrawContextValidation::DrawMeshTasksIndirect (const BufferDesc &	indirectBufferDesc,
														Bytes				indirectBufferOffset,
														uint				drawCount,
														Bytes				stride) __Th___
	{
		DrawMeshTasksIndirect( drawCount, stride );

		GCTX_CHECK( IsDeviceMemory( indirectBufferDesc ));
		GCTX_CHECK( AllBits( indirectBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( indirectBufferOffset, 4 ));
		GCTX_CHECK( indirectBufferOffset < indirectBufferDesc.size );
		GCTX_CHECK( (indirectBufferOffset + drawCount * stride) <= indirectBufferDesc.size );
	}

	void  DrawContextValidation::DrawMeshTasksIndirect (uint drawCount, Bytes stride) __Th___
	{
		ASSERT( drawCount > 0 );
		Unused( drawCount );
		GCTX_CHECK( MeshShaderSupported() );
		GCTX_CHECK( stride >= SizeOf<DrawMeshTasksIndirectCommand> );
		GCTX_CHECK( IsMultipleOf( stride, 4 ));
	}

# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::DrawMeshTasksIndirect (VkPipelineLayout layout,
														VkBuffer		 indirectBuffer,
														uint			 drawCount,
														Bytes			 stride) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( indirectBuffer != Default );

		DrawMeshTasksIndirect( drawCount, stride );
	}
# endif

/*
=================================================
	DrawMeshTasksIndirectCount
=================================================
*/
	void  DrawContextValidation::DrawMeshTasksIndirectCount (const BufferDesc &	indirectBufferDesc,
															 Bytes				indirectBufferOffset,
															 const BufferDesc &	countBufferDesc,
															 Bytes				countBufferOffset,
															 uint				maxDrawCount,
															 Bytes				stride) __Th___
	{
		DrawMeshTasksIndirectCount( maxDrawCount, stride );

		GCTX_CHECK( IsDeviceMemory( indirectBufferDesc ));
		GCTX_CHECK( AllBits( indirectBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( indirectBufferOffset, 4 ));
		GCTX_CHECK( indirectBufferOffset < indirectBufferDesc.size );
		GCTX_CHECK( (indirectBufferOffset + maxDrawCount * stride) <= indirectBufferDesc.size );

		GCTX_CHECK( IsDeviceMemory( countBufferDesc ));
		GCTX_CHECK( AllBits( countBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( countBufferOffset, 4 ));
		GCTX_CHECK( countBufferOffset < countBufferDesc.size );
		GCTX_CHECK( (countBufferOffset + SizeOf<uint>) <= countBufferDesc.size );
	}

	void  DrawContextValidation::DrawMeshTasksIndirectCount (uint maxDrawCount, Bytes stride) __Th___
	{
		ASSERT( maxDrawCount > 0 );
		Unused( maxDrawCount );
		GCTX_CHECK( MeshShaderSupported() );
		GCTX_CHECK( stride >= SizeOf<DrawMeshTasksIndirectCommand> );
		GCTX_CHECK( IsMultipleOf( stride, 4 ));
	}

# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::DrawMeshTasksIndirectCount (VkPipelineLayout	layout,
															 VkBuffer			indirectBuffer,
															 VkBuffer			countBuffer,
															 uint				maxDrawCount,
															 Bytes				stride) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( indirectBuffer != Default );
		GCTX_CHECK( countBuffer != Default );

		DrawMeshTasksIndirectCount( maxDrawCount, stride );
	}
# endif

/*
=================================================
	DispatchTile
=================================================
*/
# ifdef AE_ENABLE_VULKAN
	void  DrawContextValidation::DispatchTile (VkPipelineLayout layout) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( TileShaderSupported() );
	}
# endif

/*
=================================================
	SetDepthBias
=================================================
*/
	void  DrawContextValidation::SetDepthBias (EPipelineDynamicState dynState, float depthBiasClamp) __Th___
	{
		GCTX_CHECK( AllBits( dynState, EPipelineDynamicState::DepthBias ));
		GCTX_CHECK( DepthBiasClampSupported() or IsZero( depthBiasClamp ));
	}

/*
=================================================
	SetDepthBounds
=================================================
*/
	void  DrawContextValidation::SetDepthBounds (EPipelineDynamicState) __Th___
	{
		GCTX_CHECK( DepthBoundsSupported() );
		//GCTX_CHECK( AllBits( dynState, EPipelineDynamicState::DepthBounds ));		// TODO
	}

/*
=================================================
	SetStencilCompareMask
=================================================
*/
	void  DrawContextValidation::SetStencilCompareMask (EPipelineDynamicState dynState) __Th___
	{
		GCTX_CHECK( AllBits( dynState, EPipelineDynamicState::StencilCompareMask ));
	}

/*
=================================================
	SetStencilWriteMask
=================================================
*/
	void  DrawContextValidation::SetStencilWriteMask (EPipelineDynamicState dynState) __Th___
	{
		GCTX_CHECK( AllBits( dynState, EPipelineDynamicState::StencilWriteMask ));
	}

/*
=================================================
	SetStencilReference
=================================================
*/
	void  DrawContextValidation::SetStencilReference (EPipelineDynamicState dynState) __Th___
	{
		GCTX_CHECK( AllBits( dynState, EPipelineDynamicState::StencilReference ));
	}

/*
=================================================
	SetBlendConstants
=================================================
*/
	void  DrawContextValidation::SetBlendConstants (EPipelineDynamicState dynState) __Th___
	{
		GCTX_CHECK( AllBits( dynState, EPipelineDynamicState::BlendConstants ));
	}

/*
=================================================
	SetFragmentShadingRate
=================================================
*/
	void  DrawContextValidation::SetFragmentShadingRate (EPipelineDynamicState dynState, EShadingRate rate,
														 EShadingRateCombinerOp primitiveOp, EShadingRateCombinerOp textureOp) __Th___
	{
		Unused( primitiveOp, textureOp );	// TODO
		GCTX_CHECK( FragmentShadingRateSupported() );

	//	GCTX_CHECK( AllBits( dynState, EPipelineDynamicState::FragmentShadingRate ));
		GCTX_CHECK( NoBits( rate, ~uint(EShadingRate::_SizeMask) ));	// only size
	}

/*
=================================================
	SetViewportWScaling
=================================================
*/
	void  DrawContextValidation::SetViewportWScaling (EPipelineDynamicState dynState, ArrayView<packed_float2> scaling) __Th___
	{
		GCTX_CHECK( ViewportWScalingSupported() );
		//GCTX_CHECK( AllBits( dynState, EPipelineDynamicState::ViewportWScaling ));

		Unused( scaling );	// TODO
	}

#endif
//-----------------------------------------------------------------------------



#if 1
/*
=================================================
	Copy
=================================================
*/
	void  ASBuildContextValidation::Copy (const RTGeometryDesc &srcGeometryDesc, const RTGeometryDesc &dstGeometryDesc, ERTASCopyMode mode) __Th___
	{
		GCTX_CHECK_MSG( &srcGeometryDesc != &dstGeometryDesc, "'src' and 'dst' memory must not overlap" );
		switch_enum( mode )
		{
			case ERTASCopyMode::Clone :			break;
			case ERTASCopyMode::Compaction :	GCTX_CHECK( AllBits( srcGeometryDesc.options, ERTASOptions::AllowCompaction ));		break;
			case ERTASCopyMode::_Count :
			default :							GCTX_CHECK_MSG( false, "unknown ERTASCopyMode" );	break;
		}
		switch_end
	}

	void  ASBuildContextValidation::Copy (const RTSceneDesc &srcSceneDesc, const RTSceneDesc &dstSceneDesc, ERTASCopyMode mode) __Th___
	{
		GCTX_CHECK_MSG( &srcSceneDesc != &dstSceneDesc, "'src' and 'dst' memory must not overlap" );
		switch_enum( mode )
		{
			case ERTASCopyMode::Clone :			break;
			case ERTASCopyMode::Compaction :	GCTX_CHECK( AllBits( srcSceneDesc.options, ERTASOptions::AllowCompaction ));		break;
			case ERTASCopyMode::_Count :
			default :							GCTX_CHECK_MSG( false, "unknown ERTASCopyMode" );	break;
		}
		switch_end
	}

/*
=================================================
	Build
=================================================
*/
	void  ASBuildContextValidation::Build (const RTGeometryDesc &, const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( scratchBufDesc ));
		GCTX_CHECK( AllBits( scratchBufDesc.usage, EBufferUsage::ASBuild_Scratch ));
		GCTX_CHECK( IsMultipleOf( scratchBufferOffset, _GetRayTracingProps().scratchBufferAlign ));
		GCTX_CHECK( scratchBufferOffset < scratchBufDesc.size );
	}

	void  ASBuildContextValidation::Build (const RTSceneDesc &,
										   const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset,
										   const BufferDesc &instanceBufDesc, Bytes instanceBufferOffset) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( scratchBufDesc ));
		GCTX_CHECK( AllBits( scratchBufDesc.usage, EBufferUsage::ASBuild_Scratch ));
		GCTX_CHECK( IsMultipleOf( scratchBufferOffset, _GetRayTracingProps().scratchBufferAlign ));
		GCTX_CHECK( scratchBufferOffset < scratchBufDesc.size );

		GCTX_CHECK( IsDeviceMemory( instanceBufDesc ));
		GCTX_CHECK( AllBits( instanceBufDesc.usage, EBufferUsage::ASBuild_ReadOnly ));
		GCTX_CHECK( IsMultipleOf( instanceBufferOffset, _GetRayTracingProps().instanceDataAlign ));
		GCTX_CHECK( instanceBufferOffset < instanceBufDesc.size );
	}

/*
=================================================
	Update
=================================================
*/
	void  ASBuildContextValidation::Update (const RTGeometryDesc &srcGeometryDesc, const RTGeometryDesc &dstGeometryDesc,
											const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset) __Th___
	{
		if ( &srcGeometryDesc == &dstGeometryDesc ) {
			GCTX_CHECK( AllBits( srcGeometryDesc.options, ERTASOptions::AllowUpdate ));
			GCTX_CHECK( AllBits( dstGeometryDesc.options, ERTASOptions::AllowUpdate ));
		}

		GCTX_CHECK( IsDeviceMemory( scratchBufDesc ));
		GCTX_CHECK( AllBits( scratchBufDesc.usage, EBufferUsage::ASBuild_Scratch ));
		GCTX_CHECK( IsMultipleOf( scratchBufferOffset, _GetRayTracingProps().scratchBufferAlign ));
		GCTX_CHECK( scratchBufferOffset < scratchBufDesc.size );
	}

	void  ASBuildContextValidation::Update (const RTSceneDesc &srcSceneDesc, const RTSceneDesc &dstSceneDesc,
											const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset,
											const BufferDesc &instanceBufDesc, Bytes instanceBufferOffset) __Th___
	{
		if ( &srcSceneDesc == &dstSceneDesc ) {
			GCTX_CHECK( AllBits( srcSceneDesc.options, ERTASOptions::AllowUpdate ));
			GCTX_CHECK( AllBits( dstSceneDesc.options, ERTASOptions::AllowUpdate ));
		}

		GCTX_CHECK( IsDeviceMemory( scratchBufDesc ));
		GCTX_CHECK( AllBits( scratchBufDesc.usage, EBufferUsage::ASBuild_Scratch ));
		GCTX_CHECK( IsMultipleOf( scratchBufferOffset, _GetRayTracingProps().scratchBufferAlign ));
		GCTX_CHECK( scratchBufferOffset < scratchBufDesc.size );

		GCTX_CHECK( IsDeviceMemory( instanceBufDesc ));
		GCTX_CHECK( AllBits( instanceBufDesc.usage, EBufferUsage::ASBuild_ReadOnly ));
		GCTX_CHECK( IsMultipleOf( instanceBufferOffset, _GetRayTracingProps().instanceDataAlign ));
		GCTX_CHECK( instanceBufferOffset < instanceBufDesc.size );
	}

/*
=================================================
	SerializeToMemory
=================================================
*/
	void  ASBuildContextValidation::SerializeToMemory (const BufferDesc &dstBufferDesc, Bytes dstOffset) __Th___
	{
		GCTX_CHECK( dstOffset < dstBufferDesc.size );
		GCTX_CHECK( IsMultipleOf( dstOffset, 256 ));
		GCTX_CHECK( AllBits( dstBufferDesc.usage, EBufferUsage::ShaderAddress ));
	}

	void  ASBuildContextValidation::SerializeToMemory (DeviceAddress dst) __Th___
	{
		GCTX_CHECK( dst != Default );
		GCTX_CHECK( IsMultipleOf( BitCast<ulong>(dst), 256 ));
	}

/*
=================================================
	DeserializeFromMemory
=================================================
*/
	void  ASBuildContextValidation::DeserializeFromMemory (const BufferDesc &srcBufferDesc, Bytes srcOffset) __Th___
	{
		GCTX_CHECK( srcOffset < srcBufferDesc.size );
		GCTX_CHECK( IsMultipleOf( srcOffset, 256 ));
		GCTX_CHECK( AllBits( srcBufferDesc.usage, EBufferUsage::ShaderAddress ));
	}

	void  ASBuildContextValidation::DeserializeFromMemory (DeviceAddress src) __Th___
	{
		GCTX_CHECK( src != Default );
		GCTX_CHECK( IsMultipleOf( BitCast<ulong>(src), 256 ));
	}

/*
=================================================
	BuildIndirect
=================================================
*/
	void  ASBuildContextValidation::BuildIndirect (const RTGeometryBuild &cmd, const BufferDesc &indirectBufferDesc, Bytes indirectBufferOffset, Bytes indirectStride) __Th___
	{
		GCTX_CHECK( BuildIndirectSupported() );

		GCTX_CHECK( IsDeviceMemory( indirectBufferDesc ));
		GCTX_CHECK( AllBits( indirectBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( indirectStride, 4 ));
		GCTX_CHECK( indirectStride >= SizeOf<ASBuildIndirectCommand> );

		GCTX_CHECK( IsMultipleOf( indirectBufferOffset, 4 ));
		GCTX_CHECK( indirectBufferDesc.size > indirectBufferOffset );
		GCTX_CHECK( indirectBufferDesc.size >= indirectBufferOffset + indirectStride * cmd.GeometryCount() );
	}

	void  ASBuildContextValidation::BuildIndirect (const RTSceneBuild &, const BufferDesc &indirectBufferDesc, Bytes indirectBufferOffset) __Th___
	{
		GCTX_CHECK( BuildIndirectSupported() );

		GCTX_CHECK( IsDeviceMemory( indirectBufferDesc ));
		GCTX_CHECK( AllBits( indirectBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( indirectBufferOffset, 4 ));
		GCTX_CHECK( indirectBufferDesc.size > indirectBufferOffset );
		GCTX_CHECK( indirectBufferDesc.size >= indirectBufferOffset + sizeof(ASBuildIndirectCommand) );
	}

	void  ASBuildContextValidation::BuildIndirect (const RTGeometryDesc &,
												   const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset,
												   Bytes indirectStride) __Th___
	{
		GCTX_CHECK( BuildIndirectSupported() );

		GCTX_CHECK( IsMultipleOf( indirectStride, 4 ));
		GCTX_CHECK( indirectStride >= SizeOf<ASBuildIndirectCommand> );

		GCTX_CHECK( IsDeviceMemory( scratchBufDesc ));
		GCTX_CHECK( AllBits( scratchBufDesc.usage, EBufferUsage::ASBuild_Scratch ));
		GCTX_CHECK( IsMultipleOf( scratchBufferOffset, _GetRayTracingProps().scratchBufferAlign ));
		GCTX_CHECK( scratchBufferOffset < scratchBufDesc.size );

	}

	void  ASBuildContextValidation::BuildIndirect (const RTSceneDesc &,
												   const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset,
												   const BufferDesc &instanceBufDesc, Bytes instanceBufferOffset) __Th___
	{
		GCTX_CHECK( BuildIndirectSupported() );

		GCTX_CHECK( IsDeviceMemory( scratchBufDesc ));
		GCTX_CHECK( AllBits( scratchBufDesc.usage, EBufferUsage::ASBuild_Scratch ));
		GCTX_CHECK( IsMultipleOf( scratchBufferOffset, _GetRayTracingProps().scratchBufferAlign ));
		GCTX_CHECK( scratchBufferOffset < scratchBufDesc.size );

		GCTX_CHECK( IsDeviceMemory( instanceBufDesc ));
		GCTX_CHECK( AllBits( instanceBufDesc.usage, EBufferUsage::ASBuild_ReadOnly ));
		GCTX_CHECK( IsMultipleOf( instanceBufferOffset, _GetRayTracingProps().instanceDataAlign ));
		GCTX_CHECK( instanceBufferOffset < instanceBufDesc.size );
	}

# ifdef AE_ENABLE_VULKAN
/*
=================================================
	BuildIndirect
=================================================
*/
	void  ASBuildContextValidation::BuildIndirect (const RTGeometryDesc &geometryDesc,
													const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset,
													VkDeviceAddress indirectMem, Bytes indirectStride) __Th___
	{
		GCTX_CHECK( indirectMem != Default );
		GCTX_CHECK( IsMultipleOf( indirectMem, 4 ));
		BuildIndirect( geometryDesc, scratchBufDesc, scratchBufferOffset, indirectStride );
	}

/*
=================================================
	BuildIndirect
=================================================
*/
	void  ASBuildContextValidation::BuildIndirect (const RTSceneDesc &sceneDesc,
													const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset,
													const BufferDesc &instanceBufDesc, Bytes instanceBufferOffset,
													VkDeviceAddress indirectMem) __Th___
	{
		GCTX_CHECK( indirectMem != Default );
		GCTX_CHECK( IsMultipleOf( indirectMem, 4 ));
		BuildIndirect( sceneDesc, scratchBufDesc, scratchBufferOffset, instanceBufDesc, instanceBufferOffset );
	}

# endif

/*
=================================================
	ReadProperty / WriteProperty
=================================================
*/
# ifdef AE_ENABLE_VULKAN
	void  ASBuildContextValidation::ReadProperty (ERTASProperty property) __Th___
	{
		auto&	dev = GraphicsScheduler().GetDevice();

		switch_enum( property )
		{
			case ERTASProperty::CompactedSize :		break;
			case ERTASProperty::SerializationSize :	break;
			case ERTASProperty::Size :				GCTX_CHECK( dev.GetVExtensions().rayTracingMaintenance1 );	break;

			case ERTASProperty::_Count :
			default :								break;
		}
		switch_end
	}

	void  ASBuildContextValidation::WriteProperty (ERTASProperty property, const BufferDesc &dstBufferDesc, Bytes dstOffset, Bytes size) __Th___
	{
		auto&	dev = GraphicsScheduler().GetDevice();

		switch_enum( property )
		{
			case ERTASProperty::CompactedSize :		break;
			case ERTASProperty::SerializationSize :	break;
			case ERTASProperty::Size :				GCTX_CHECK( dev.GetVExtensions().rayTracingMaintenance1 );	break;

			case ERTASProperty::_Count :
			default :								break;
		}
		switch_end

		GCTX_CHECK( size == 8_b or size == UMax );
		GCTX_CHECK( IsMultipleOf( dstOffset, 8 ));

		GCTX_CHECK( dstOffset < dstBufferDesc.size );
		GCTX_CHECK( (dstOffset + size) <= dstBufferDesc.size );

	//	GCTX_CHECK( IsDeviceMemory( dstBufferDesc ));	// TODO: ???
		GCTX_CHECK( AllBits( dstBufferDesc.usage, EBufferUsage::TransferDst ));
	}
# endif

# ifdef AE_ENABLE_METAL
	void  ASBuildContextValidation::ReadProperty (ERTASProperty property) __Th___
	{
		GCTX_CHECK( property == ERTASProperty::CompactedSize );
	}

	void  ASBuildContextValidation::WriteProperty (ERTASProperty property, const BufferDesc &dstBufferDesc, Bytes dstOffset, Bytes size) __Th___
	{
		GCTX_CHECK( property == ERTASProperty::CompactedSize );

		GCTX_CHECK( size == 4_b or size == UMax );
		GCTX_CHECK( IsMultipleOf( dstOffset, 4 ));
		GCTX_CHECK( dstOffset < dstBufferDesc.size );
		GCTX_CHECK( (dstOffset + size) <= dstBufferDesc.size );
		GCTX_CHECK( IsDeviceMemory( dstBufferDesc ));
		GCTX_CHECK( AllBits( dstBufferDesc.usage, EBufferUsage::TransferDst ));
	}
# endif

#endif
//-----------------------------------------------------------------------------



#if 1
/*
=================================================
	PushConstant
=================================================
*/
	void  RayTracingContextValidation::PushConstant (const PushConstantIndex &idx, Bytes size, ShaderStructName::Ref typeName) __Th___
	{
	#ifdef AE_DEBUG
		GCTX_CHECK( typeName == Default or idx.dbgTypeName == Default or idx.dbgTypeName == typeName );
		GCTX_CHECK( Bytes{idx.dbgDataSize} == size or idx.dbgDataSize == 0 );
	#else
		Unused( idx, size, typeName );
	#endif
	}

# ifdef AE_ENABLE_VULKAN
	void  RayTracingContextValidation::PushConstant (VkPipelineLayout layout, Bytes offset, Bytes size, const void* values, EShaderStages stages) __Th___
	{
		GCTX_CHECK( size > 0 );
		GCTX_CHECK( IsMultipleOf( size, 4 ));
		GCTX_CHECK( IsMultipleOf( offset, 4 ));
		GCTX_CHECK( values != null );
		GCTX_CHECK( stages != Default );
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( AnyBits( stages, EShaderStages::AllRayTracing ));
		GCTX_CHECK( NoBits( stages, ~EShaderStages::AllRayTracing ));
	}
# endif

/*
=================================================
	BindDescriptorSet
=================================================
*/
# ifdef AE_ENABLE_VULKAN
	void  RayTracingContextValidation::BindDescriptorSet (VkPipelineLayout layout, DescSetBinding index, VkDescriptorSet ds) __Th___
	{
		GCTX_CHECK( ds != Default );
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( index.vkIndex < _GetFeatureSet().maxDescriptorSets );
	}
# endif

/*
=================================================
	TraceRays
=================================================
*/
	void  RayTracingContextValidation::TraceRays (const uint3 &dim) __Th___
	{
		GCTX_CHECK( All( dim >= 1u ));
	}

# ifdef AE_ENABLE_VULKAN
	void  RayTracingContextValidation::TraceRays (VkPipelineLayout layout, const uint3 &dim) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		TraceRays( dim );
	}
# endif

/*
=================================================
	TraceRaysIndirect
=================================================
*/
	void  RayTracingContextValidation::TraceRaysIndirect (const BufferDesc &indirectBufferDesc, Bytes indirectBufferOffset) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( indirectBufferDesc ));
		GCTX_CHECK( AllBits( indirectBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( indirectBufferOffset, 4 ));
		GCTX_CHECK( indirectBufferDesc.size >= indirectBufferOffset + sizeof(TraceRayIndirectCommand) );
	}

# ifdef AE_ENABLE_VULKAN
	void  RayTracingContextValidation::TraceRaysIndirect (VkPipelineLayout layout, VkDeviceAddress indirectDeviceAddress) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( indirectDeviceAddress != Default );
		GCTX_CHECK( _GetVkDeviceProperties().rayTracingPipelineFeats.rayTracingPipelineTraceRaysIndirect == VK_TRUE );	// TODO: use FS
	}
# endif

/*
=================================================
	TraceRaysIndirect2
=================================================
*/
	void  RayTracingContextValidation::TraceRaysIndirect2 (const BufferDesc &indirectBufferDesc, Bytes indirectBufferOffset) __Th___
	{
		GCTX_CHECK( IsDeviceMemory( indirectBufferDesc ));
		GCTX_CHECK( AllBits( indirectBufferDesc.usage, EBufferUsage::Indirect ));
		GCTX_CHECK( IsMultipleOf( indirectBufferOffset, 4 ));
		GCTX_CHECK( indirectBufferDesc.size >= indirectBufferOffset + sizeof(TraceRayIndirectCommand2) );
	}

# ifdef AE_ENABLE_VULKAN
	void  RayTracingContextValidation::TraceRaysIndirect2 (VkPipelineLayout layout, VkDeviceAddress indirectDeviceAddress) __Th___
	{
		GCTX_CHECK_MSG( layout != Default, "pipeline is not bound" );
		GCTX_CHECK( indirectDeviceAddress != Default );
		GCTX_CHECK( IsMultipleOf( indirectDeviceAddress, 4 ));
		GCTX_CHECK( _GetVkDeviceProperties().rayTracingMaintenance1Feats.rayTracingPipelineTraceRaysIndirect2 == VK_TRUE );	// TODO: use FS
	}
# endif

#endif
//-----------------------------------------------------------------------------


#endif // AE_VALIDATE_GCTX

} // AE::Graphics::_hidden_
