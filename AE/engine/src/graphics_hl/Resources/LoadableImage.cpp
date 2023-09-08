// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/Resources/LoadableImage.h"
#include "AssetPackerImpl.h"

namespace AE::Graphics
{
    using namespace AE::AssetPacker;

/*
=================================================
    destructor
=================================================
*/
    LoadableImage::~LoadableImage () __NE___
    {
        if ( _imageId )
            RenderTaskScheduler().GetResourceManager().ReleaseResource( _imageId );
    }

/*
=================================================
    ReleaseImageAndView
=================================================
*/
    auto  LoadableImage::ReleaseImageAndView () __NE___ -> StrongImageAndViewID
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        auto    view_id = res_mngr.CreateImageView( ImageViewDesc{ _viewType }, _imageId, Default );

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
        //  - read header in FileIO thread
        //  - create image in special thread for GPU allocations
        //  - read image data to RAM
        //  - copy from RAM to staging buffer

        CHECK_ERR( stream and stream->IsOpen() );

        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        ImagePacker unpacker;
        CHECK_ERR( unpacker.ReadHeader( *stream ));

        ImageDesc   desc;
        desc.SetDimension( uint3{unpacker.header.dimension} )
            .SetArrayLayers( unpacker.header.arrayLayers )
            .SetMaxMipmaps( unpacker.header.mipmaps )
            .SetType( unpacker.header.viewType )
            .SetFormat( unpacker.header.format )
            .SetUsage( EImageUsage::Sampled | EImageUsage::Transfer );

        switch ( unpacker.header.viewType ) {
            case EImage::Cube :
            case EImage::CubeArray :    desc.options |= EImageOpt::CubeCompatible;  break;
        }

        auto img_id = res_mngr.CreateImage( desc, Default, alloc );
        CHECK_ERR( img_id );

        // copy to staging buffer
        {
            ctx.ImageBarrier( img_id, EResourceState::Unknown, EResourceState::CopyDst );
            ctx.CommitBarriers();

            UploadImageDesc upload;
            upload.heapType = EStagingHeapType::Dynamic;

            for (uint mip = 0, mip_cnt = unpacker.header.mipmaps; mip < mip_cnt; ++mip)
            {
                for (uint layer = 0, layer_cnt = unpacker.header.arrayLayers; layer < layer_cnt; ++layer)
                {
                    ImagePacker::ImageData  img_data;
                    CHECK_ERR( unpacker.ReadImage( *stream, INOUT img_data ));

                    upload.arrayLayer   = ImageLayer{layer};
                    upload.mipLevel     = MipmapLevel{mip};

                    ImageMemView    dst_mem;
                    ctx.UploadImage( img_id, upload, OUT dst_mem );

                    CHECK_ERR( dst_mem.Copy( img_data.memView ));
                }
            }
        }

        auto    image = MakeRC<LoadableImage>();
        image->_imageId     = RVRef(img_id);
        image->_viewType    = unpacker.header.viewType;
        return image;
    }


} // AE::Graphics
