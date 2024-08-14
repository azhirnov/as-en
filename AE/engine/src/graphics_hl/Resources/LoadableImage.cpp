// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/Resources/LoadableImage.h"
#include "AssetPackerImpl.h"

namespace AE::Graphics
{
	using namespace AE::AssetPacker;

namespace {
#	include "Packer/ImagePacker.cpp.h"
}

/*
=================================================
	destructor
=================================================
*/
	LoadableImage::~LoadableImage () __NE___
	{
		if ( _imageId )
			GraphicsScheduler().GetResourceManager().ReleaseResource( _imageId );
	}

/*
=================================================
	ReleaseImageAndView
=================================================
*/
	auto  LoadableImage::ReleaseImageAndView () __NE___ -> StrongImageAndViewID
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		auto	view_id	= res_mngr.CreateImageView( ImageViewDesc{ _viewType }, _imageId, Default );

		return StrongImageAndViewID( RVRef(_imageId), RVRef(view_id) );
	}

/*
=================================================
	Loader::Load
=================================================
*/
	RC<LoadableImage>  LoadableImage::Loader::Load (RC<RStream> stream, ITransferContext &ctx, GfxMemAllocatorPtr alloc) __NE___
	{
		// TODO: async
		//	- read header in FileIO thread
		//	- create image in special thread for GPU allocations
		//	- read image data to RAM
		//	- copy from RAM to staging buffer

		CHECK_ERR( stream and stream->IsOpen() );

		auto	image		= MakeRC<LoadableImage>();
		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();

		ImagePacker::FileHeader	header;
		CHECK_ERR( ImagePacker_ReadHeader( *stream, OUT header ));

		image->_imageId = res_mngr.CreateImage( header.hdr.ToDesc().SetUsage( EImageUsage::Sampled | EImageUsage::Transfer ), Default, RVRef(alloc) );
		CHECK_ERR( image->_imageId );

		CHECK_ERR( _Load( *stream, image->_imageId, &header.hdr, ctx ));

		image->_viewType = header.hdr.viewType;
		return image;
	}

/*
=================================================
	Loader::_Load
=================================================
*/
	bool  LoadableImage::Loader::_Load (RStream &stream, ImageID imageId, const void* hdr, ITransferContext &ctx) __NE___
	{
		auto&			header		= *Cast<ImagePacker::Header>(hdr);
		const Bytes		base_off	= stream.Position();

		RC<SharedMem>	tmp			= SharedMem::Create( AE::GetDefaultAllocator(), ImagePacker_MaxSliceSize( header ));
		CHECK_ERR( tmp );

		// copy to staging buffer
		ctx.ImageBarrier( imageId, EResourceState::Unknown, EResourceState::CopyDst );
		ctx.CommitBarriers();

		UploadImageDesc	upload;
		upload.heapType	= EStagingHeapType::Dynamic;

		for (uint mip = 0, mip_cnt = header.mipmaps; mip < mip_cnt; ++mip)
		{
			for (uint layer = 0, layer_cnt = header.arrayLayers; layer < layer_cnt; ++layer)
			{
				upload.arrayLayer	= ImageLayer{layer};
				upload.mipLevel		= MipmapLevel{mip};

				Bytes	off;
				ImagePacker_GetOffset( header, upload.arrayLayer, upload.mipLevel, uint3{0},
										OUT upload.imageDim, OUT off, OUT upload.dataRowPitch, OUT upload.dataSlicePitch );

				const Bytes	size = upload.dataSlicePitch * upload.imageDim.z;

				CHECK_ERR( stream.Position() == off + base_off );
				CHECK_ERR( size <= tmp->Size() );
				CHECK_ERR( stream.Read( OUT tmp->Data(), size ));

				ImageMemView	dst_mem;
				ctx.UploadImage( imageId, upload, OUT dst_mem );

				ImageMemView	src_mem { tmp->Data(), size, uint3{}, upload.imageDim, upload.dataRowPitch,
										  upload.dataSlicePitch, header.format, EImageAspect::Color };
				CHECK_ERR( dst_mem.CopyFrom( src_mem ));
			}
		}
		return true;
	}


} // AE::Graphics
