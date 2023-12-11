// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/Resources/LoadableImage.h"
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
        if ( _imageId )
            GraphicsScheduler().GetResourceManager().DelayedReleaseResources( _imageId, _viewId );
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

        ImageAtlasPacker    unpacker;
        {
            Serializing::Deserializer   des{ MakeRC<BufferedRStream>( stream )};
            CHECK_ERR( unpacker.Deserialize( des ));
        }

        auto    atlas       = MakeRC<StaticImageAtlas>();
        auto&   res_mngr    = GraphicsScheduler().GetResourceManager();

        atlas->_imageId = res_mngr.CreateImage( unpacker.Header().ToDesc().SetUsage( EImageUsage::Sampled | EImageUsage::Transfer ), Default, alloc );
        CHECK_ERR( atlas->_imageId );

        atlas->_viewId = res_mngr.CreateImageView( unpacker.Header().ToViewDesc(), atlas->_imageId, Default );
        CHECK_ERR( atlas->_viewId );

        CHECK_ERR( LoadableImage::Loader::_Load( *stream, atlas->_imageId, &unpacker.Header(), ctx ));

        atlas->_invImgSize   = 1.0f / float2{unpacker.Header().dimension};
        atlas->_nameToIdx    = RVRef(unpacker.map);
        atlas->_imageRects   = RVRef(unpacker.rects);

        return atlas;
    }


} // AE::Graphics
