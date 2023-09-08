// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/Resources/StaticImageAtlas.h"
#include "AssetPackerImpl.h"

namespace AE::Graphics
{
    using namespace AE::Serializing;
    using namespace AE::AssetPacker;

/*
=================================================
    destructor
=================================================
*/
    StaticImageAtlas::~StaticImageAtlas () __NE___
    {
        if ( _imageAndView )
            RenderTaskScheduler().GetResourceManager().DelayedReleaseResources( _imageAndView.image, _imageAndView.view );
    }

/*
=================================================
    Get
=================================================
*/
    bool  StaticImageAtlas::Get (const ImageInAtlasName &name, OUT RectI &region) C_NE___
    {
        auto    it = _nameToIdx.find( name );
        if_likely( it != _nameToIdx.end() )
        {
            region = RectI{ _imageRects[ it->second ]};
            return true;
        }
        return false;
    }

    bool  StaticImageAtlas::Get (const ImageInAtlasName &name, OUT RectF &region) C_NE___
    {
        auto    it = _nameToIdx.find( name );
        if_likely( it != _nameToIdx.end() )
        {
            region = RectF{ _imageRects[ it->second ]} * _invImgSize;
            return true;
        }
        return false;
    }

/*
=================================================
    Loader::Load
=================================================
*/
    RC<StaticImageAtlas>  StaticImageAtlas::Loader::Load (RC<RStream> stream, ITransferContext &ctx, GfxMemAllocatorPtr alloc) __NE___
    {
        CHECK_ERR( stream and stream->IsOpen() );

        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        ImageAtlasPacker    unpacker;
        {
            Serializing::Deserializer   des{ MakeRC<BufferedRStream>( stream )};
            CHECK_ERR( unpacker.Deserialize( des ));
        }
        ASSERT( unpacker.header.viewType == EImage_2D );

        auto    img_id = res_mngr.CreateImage( ImageDesc{}
                            .SetDimension( uint3{unpacker.header.dimension} ).SetArrayLayers( unpacker.header.arrayLayers ).SetMaxMipmaps( unpacker.header.mipmaps )
                            .SetType( unpacker.header.viewType ).SetFormat( unpacker.header.format ).SetUsage( EImageUsage::Sampled | EImageUsage::Transfer ),
                            Default, alloc );
        CHECK_ERR( img_id );

        auto    view_id = res_mngr.CreateImageView( ImageViewDesc{ unpacker.header.viewType }, img_id, Default );
        CHECK_ERR( view_id );

        ImagePacker::ImageData  img_data;
        CHECK_ERR( unpacker.ReadImage( *stream, INOUT img_data ));

        // copy to staging buffer
        {
            ctx.ImageBarrier( img_id, EResourceState::Unknown, EResourceState::CopyDst );
            ctx.CommitBarriers();

            UploadImageDesc desc;
            desc.heapType   = EStagingHeapType::Dynamic;

            ImageMemView    dst_mem;
            ctx.UploadImage( img_id, desc, OUT dst_mem );

            CHECK_ERR( dst_mem.Copy( img_data.memView ));
        }

        auto    atlas = MakeRC<StaticImageAtlas>();

        atlas->_imageAndView = StrongImageAndViewID{ RVRef(img_id), RVRef(view_id) };
        atlas->_invImgSize   = 1.0f / float2{unpacker.header.dimension};
        atlas->_nameToIdx    = RVRef(unpacker.map);
        atlas->_imageRects   = RVRef(unpacker.rects);

        return atlas;
    }


} // AE::Graphics
