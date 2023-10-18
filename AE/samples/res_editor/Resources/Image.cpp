// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Possible problems:

    * '_id' and '_view' are atomics but some processes may happens when '_id' has new image and '_view' has previous view.
    * '_id' must be updated before '_view'.
    * Add imageId to the RG state tracker before updating '_id' and '_view'.
*/

#include "res_editor/Resources/Image.h"
#include "res_editor/Core/RenderGraph.h"
#include "res_editor/Passes/Renderer.h"

#include "res_pack/asset_packer/Packer/ImagePacker.h"

namespace AE::ResEditor
{
namespace {
#   define AE_BUILD_ASSET_PACKER
#   include "res_pack/asset_packer/Packer/ImagePackerUtils.cpp.h"
#   undef AE_BUILD_ASSET_PACKER
}

/*
=================================================
    constructor
=================================================
*/
    Image::Image (Renderer&     renderer,
                  StringView    dbgName) :
        IResource{ renderer },
        _dbgName{ dbgName }
    {
    }

    Image::Image (Strong<ImageID>       id,
                  Strong<ImageViewID>   view,
                  ArrayView<LoadOp>     loadOps,
                  Renderer &            renderer,
                  bool                  isDummy,
                  const ImageDesc &     desc,
                  const ImageViewDesc&  viewDesc,
                  RC<DynamicDim>        inDynSize,
                  RC<DynamicDim>        outDynSize,
                  StringView            dbgName) :
        IResource{ renderer },
        _id{ RVRef(id) },
        _view{ RVRef(view) },
        _isDummy{ isDummy },
        _inDynSize{ RVRef(inDynSize) },
        _outDynSize{ RVRef(outDynSize) },
        _loadOps{ loadOps.begin(), loadOps.end() },
        _dbgName{ dbgName }
    {
        _imageDesc.Write( desc );
        _imageDesc.Write( viewDesc );

        if ( isDummy )
        {
            CHECK_THROW( not _inDynSize );

            _uploadStatus.store( EUploadStatus::InProgress );
        }
        else
        {
            _uploadStatus.store( EUploadStatus::Completed );

            RenderGraph().GetStateTracker().AddResource( _id.Get() );
        }

        if ( not _loadOps.empty() )
        {
            _uploadStatus.store( EUploadStatus::InProgress );

            for (auto& op : _loadOps)
            {
                CHECK_THROW( GetVFS().Open( OUT op.file, op.filename ));

                auto    req = op.file->ReadRemaining( 0_b );    // TODO: read by blocks
                CHECK_THROW( req );

                op.loaded = req->AsPromise().Then( [fmt = op.imgFormat] (const AsyncDSRequestResult &in) { return _Load( in, fmt ); });
            }

            _DtTrQueue().EnqueueForUpload( GetRC() );
        }
    }

/*
=================================================
    destructor
=================================================
*/
    Image::~Image ()
    {
        Cancel();

        {
            auto    view    = _view.Release();
            auto    id      = _id.Release();
            RenderGraph().GetStateTracker().ReleaseResources( view, id );
        }

        if ( _base )
            _base->_Remove( this );
    }

/*
=================================================
    CreateDummy2D
=================================================
*/
    RC<Image>  Image::CreateDummy2D (Renderer &renderer, StringView dbgName) __Th___
    {
        ImageDesc   desc;
        desc.imageDim   = EImageDim_2D;

        auto        img_and_view = renderer.GetDummyImage( desc );
        CHECK_THROW( img_and_view );

        auto&       res_mngr    = RenderTaskScheduler().GetResourceManager();
        RC<Image>   result      {new Image{ renderer, dbgName }};

        desc            = res_mngr.GetDescription( img_and_view.image );
        desc.usage      = EImageUsage::Transfer | EImageUsage::Sampled;
        desc.options    = EImageOpt::BlitSrc | EImageOpt::BlitDst;

        result->_imageDesc.Write( desc );
        // don't change image view descriptor
        result->_isDummy.store( true );

        Unused( result->_id.Attach( RVRef(img_and_view.image) ));
        Unused( result->_view.Attach( RVRef(img_and_view.view) ));

        return result;
    }

/*
=================================================
    CreateAndLoad
=================================================
*/
    RC<Image>  Image::CreateAndLoad (Renderer &renderer, const Path &inPath, StringView dbgName, ELoadOpFlags flags, ArrayView<Path> texSearchDirs) __Th___
    {
        Path    path = inPath;

        if ( not FileSystem::IsFile( path ))
        {
            for (auto& dir : texSearchDirs)
            {
                path = dir / inPath;
                if ( FileSystem::IsFile( path ))
                    break;
            }
        }
        CHECK_THROW_MSG( FileSystem::IsFile( path ),
            "Image file '"s << ToString(path) << "' is not exists" );

        RC<Image>   result = CreateDummy2D( renderer, (dbgName.empty() ? StringView{path.filename().replace_extension().string()} : dbgName) );

        // load from filesystem instead of VFS
        {
            result->_uploadStatus.store( EUploadStatus::InProgress );

            auto&   load_op     = result->_loadOps.emplace_back();

            load_op.flags       = flags;
            load_op.file        = MakeRC< Threading::WinAsyncRDataSource >( path );
            load_op.imgFormat   = ResLoader::PathToImageFileFormat( path );
            CHECK_THROW( load_op.file->IsOpen() );

            auto    req         = load_op.file->ReadRemaining( 0_b );   // TODO: read by blocks
            CHECK_THROW( req );

            load_op.loaded  = req->AsPromise().Then( [fmt = load_op.imgFormat] (const AsyncDSRequestResult &in) { return _Load( in, fmt ); });

            result->_DtTrQueue().EnqueueForUpload( result );
        }

        return result;
    }

/*
=================================================
    CreateAndLoad
=================================================
*/
    RC<Image>  Image::CreateAndLoad (Renderer &renderer, IntermImageRC imageData, StringView dbgName, ELoadOpFlags flags, ArrayView<Path> texSearchDirs) __Th___
    {
        CHECK_THROW( imageData );

        if ( imageData->IsEmpty() )
            return CreateAndLoad( renderer, imageData->GetPath(), dbgName, flags, texSearchDirs );

        RC<Image>   result = CreateDummy2D( renderer, dbgName );

        // upload from RAM
        {
            result->_uploadStatus.store( EUploadStatus::InProgress );

            auto&   load_op = result->_loadOps.emplace_back();

            load_op.flags   = flags;
            load_op.loaded  = Threading::MakePromiseFromValue( RVRef(imageData) );

            result->_DtTrQueue().EnqueueForUpload( result );
        }

        return result;
    }

/*
=================================================
    RequireResize
=================================================
*/
    bool  Image::RequireResize () C_Th___
    {
        if ( _base )
            return _base->RequireResize();

        if ( not _inDynSize )
            return false;

        ImageDesc   desc = GetImageDesc();

        if ( not _inDynSize->IsChanged_NonZero( INOUT desc.dimension ))
            return false;

        return true;
    }

/*
=================================================
    Resize
=================================================
*/
    bool  Image::Resize (TransferCtx_t &ctx)
    {
        if ( _base )
            return _base->Resize( ctx );

        if ( not _inDynSize )
            return false;

        ImageDesc   desc = GetImageDesc();

        if ( not _inDynSize->IsChanged_NonZero( INOUT desc.dimension ))
            return false;

        return _ResizeImage( ctx, desc, GetViewDesc() );
    }

/*
=================================================
    _ResizeImage
=================================================
*/
    bool  Image::_ResizeImage (TransferCtx_t &ctx, const ImageDesc &imageDesc, const ImageViewDesc &viewDesc)
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        {
            CHECK_ERR_MSG( res_mngr.IsSupported( imageDesc ),
                "Image '"s << _dbgName << "' description is not supported by GPU device" );

            auto    image = res_mngr.CreateImage( imageDesc, _dbgName, _GfxDynamicAllocator() );
            CHECK_ERR( image );

            RenderGraph().GetStateTracker().AddResource( image.Get(),
                                                        EResourceState::_InvalidState,  // current is not used
                                                        EResourceState::General,        // default
                                                        ctx.GetCommandBatchRC() );
            ctx.ResourceState( image, EResourceState::Invalidate );

            _imageDesc.Write( res_mngr.GetDescription( image ));

            auto    old_img = _id.Attach( RVRef(image) );
            res_mngr.ReleaseResource( old_img );    // release previous resource
        }{
            auto    view = res_mngr.CreateImageView( viewDesc, _id.Get(), _dbgName );
            CHECK_ERR( view );

            _imageDesc.Write( res_mngr.GetDescription( view ));

            auto    old_view = _view.Attach( RVRef(view) );
            res_mngr.ReleaseResource( old_view );   // release previous resource
        }

        if ( _outDynSize )
        {
            ASSERT( imageDesc.imageDim == _outDynSize->NumDimensions() );
            _outDynSize->Resize( imageDesc.dimension );
        }

        auto    derived = _derived.ReadNoLock();
        SHAREDLOCK( derived );

        for (auto* img : *derived) {
            CHECK( img->_OnResize( img->GetViewDesc() ));
        }

        return true;
    }

/*
=================================================
    Upload
=================================================
*/
    IResource::EUploadStatus  Image::Upload (TransferCtx_t &ctx) __Th___
    {
        if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
            return stat;

        ASSERT( not _loadOps.empty() );

        bool    all_complete    = true;
        bool    failed          = false;

        auto&   fmt_info    = EPixelFormat_GetInfo( _imageDesc.ConstPtr<0>()->format );

        for (auto& op : _loadOps)
        {
            if ( op.IsCompleted() )
                continue;

            op.loaded.WithResult(
                [this, &ctx, &op, &failed, &fmt_info] (const IntermImageRC &imageData)
                {
                    if ( not imageData )
                    {
                        failed = true;
                        return;
                    }

                    if_unlikely( _isDummy.load() )
                    {
                        CHECK_THROW( _CreateImage( *imageData, op.mipmap, op.layer, AllBits( op.flags, ELoadOpFlags::GenMipmaps ), ctx ));
                        _isDummy.store( false );
                    }

                    for (;;)
                    {
                        if_unlikely( not op.stream.IsInitialized() )
                        {
                            UploadImageDesc     upload;
                            upload.imageSize    = ImageUtils::MipmapDimension( imageData->Dimension(), op.curMipmap.Get(), fmt_info.TexBlockDim() );
                            upload.arrayLayer   = op.layer + op.curLayer;
                            upload.mipLevel     = op.mipmap + op.curMipmap;
                            upload.heapType     = EStagingHeapType::Dynamic;
                            upload.aspectMask   = EImageAspect::Color;
                            op.stream           = ImageStream{ _id, upload };
                        }

                        ASSERT( op.stream.Image() == _id );

                        ImageMemView    src_mem = imageData->ToView( op.curMipmap, op.curLayer );
                        ImageMemView    dst_mem;
                        ctx.UploadImage( op.stream, OUT dst_mem );

                        if ( dst_mem.Empty() )
                            break;

                        CHECK( dst_mem.CopyFrom( uint3{}, dst_mem.Offset(), src_mem, dst_mem.Dimension() ));

                        if ( op.stream.IsCompleted() )
                        {
                            // invalidate
                            op.stream = Default;

                            if ( ++op.curMipmap < MipmapLevel{imageData->MipLevels()} )
                                continue;

                            if ( ++op.curLayer < ImageLayer{imageData->ArrayLayers()} )
                            {
                                op.curMipmap = MipmapLevel{};
                                continue;
                            }

                            op.complete = true;
                            break;
                        }
                    }
                });

            all_complete &= op.IsCompleted();
        }

        if ( failed )
        {
            _loadOps.clear();
            _SetUploadStatus( EUploadStatus::Canceled );
        }

        if ( all_complete )
        {
            _GenMipmaps( ctx );

            _loadOps.clear();
            _SetUploadStatus( EUploadStatus::Completed );
        }

        return _uploadStatus.load();
    }

/*
=================================================
    _GenMipmaps
=================================================
*/
    void  Image::_GenMipmaps (TransferCtx_t &ctx) const
    {
        const auto  desc = GetImageDesc();

        for (auto& op : _loadOps)
        {
            ASSERT( op.IsCompleted() );

            if ( AllBits( op.flags, ELoadOpFlags::GenMipmaps ))
            {
                ImageSubresourceRange   range;
                range.aspectMask    = EImageAspect::Color;
                range.baseLayer     = op.layer;
                range.layerCount    = 1;
                range.baseMipLevel  = op.mipmap;
                range.mipmapCount   = desc.maxLevel.Get() - op.mipmap.Get();

                if ( range.mipmapCount > 1 )
                    ctx.GenerateMipmaps( _id, {range} );
            }
        }
    }

/*
=================================================
    CreateAndStore
=================================================
*/
    RC<Image>  Image::CreateAndStore (const Image           &src,
                                      ArrayView<StoreOp>    storeOps,
                                      StringView            dbgName) __Th___
    {
        CHECK_ERR( not src._isDummy.load() );
        CHECK_ERR( not storeOps.empty() );

        auto&       res_mngr    = RenderTaskScheduler().GetResourceManager();
        RC<Image>   result      {new Image{ src._Renderer(), dbgName }};

        Unused( result->_id.Attach( res_mngr.AcquireResource( src._id.Get() )));
        Unused( result->_view.Attach( res_mngr.AcquireResource( src._view.Get() )));

        const auto  img_desc    = res_mngr.GetDescription( result->_id.Get() );
        const auto  view_desc   = res_mngr.GetDescription( result->_view.Get() );

        result->_imageDesc.Write( img_desc );
        result->_imageDesc.Write( view_desc );

        result->_uploadStatus.store( EUploadStatus::InProgress );

        for (auto& op : storeOps) {
            result->_storeOps.emplace_back( StoreOp2{ op });
        }

        for (auto& op : result->_storeOps)
        {
            CHECK_ERR( op.file and op.file->IsOpen() );

            using Header = AssetPacker::ImagePacker::Header2;

            auto    mem = op.file->Alloc( SizeAndAlignOf<Header> );
            CHECK_ERR( mem );

            auto&   hdr     = PlacementNew<Header>( mem->Data() )->hdr;
            hdr.dimension   = packed_ushort3{img_desc.dimension};
            hdr.arrayLayers = ushort(img_desc.arrayLayers.Get());
            hdr.mipmaps     = ushort(img_desc.maxLevel.Get());
            hdr.viewType    = view_desc.viewType;
            hdr.format      = img_desc.format;

            CHECK_ERR( ImagePacker_IsValid( hdr ));

            auto    req = op.file->WriteBlock( 0_b, SizeOf<Header>, RVRef(mem) );
            CHECK_ERR( req );
        }

        result->_DtTrQueue().EnqueueForReadback( result );
        return result;
    }

/*
=================================================
    Readback
=================================================
*/
    IResource::EUploadStatus  Image::Readback (TransferCtx_t &ctx) __Th___
    {
        if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
            return stat;

        ASSERT( not _storeOps.empty() );

        bool    all_complete    = true;
        bool    failed          = false;

        for (auto& op : _storeOps)
        {
            if ( op.IsCompleted() )
                continue;

            if_unlikely( not op.stream.IsInitialized() )
            {
                const auto      img_desc    = _imageDesc.Read<ImageDesc>();
                const auto&     fmt_info    = EPixelFormat_GetInfo( img_desc.format );

                ReadbackImageDesc   read;
                read.imageSize  = ImageUtils::MipmapDimension( img_desc.dimension, op.curMipmap.Get(), fmt_info.TexBlockDim() );
                read.arrayLayer = op.curLayer;
                read.mipLevel   = op.curMipmap;
                read.heapType   = EStagingHeapType::Dynamic;
                read.aspectMask = EImageAspect::Color;
                op.stream       = ImageStream{ _id, read };
            }

            ctx.ReadbackImage( INOUT op.stream )
                .Then(  [self = GetRC<Image>(), layer = op.curLayer, mipmap = op.curMipmap, file = op.file] (const ImageMemView &memView)
                        {
                            const auto      img_desc    = self->_imageDesc.Read<ImageDesc>();
                            const auto      view_type   = self->_imageDesc.ConstPtr<1>()->viewType;
                            const auto&     fmt_info    = EPixelFormat_GetInfo( img_desc.format );
                            const uint3     mip_dim     = ImageUtils::MipmapDimension( img_desc.dimension, mipmap.Get(), fmt_info.TexBlockDim() );

                            AssetPacker::ImagePacker::Header    header;
                            header.dimension    = packed_ushort3{mip_dim};
                            header.arrayLayers  = ushort(img_desc.arrayLayers.Get());
                            header.mipmaps      = ushort(img_desc.maxLevel.Get());
                            header.viewType     = view_type;
                            header.format       = img_desc.format;

                            uint3   dim;
                            Bytes   off, size, row_size, slice_size;
                            ImagePacker_GetOffset( header, layer, mipmap, memView.Offset(),
                                                   OUT dim, OUT off, OUT size, OUT row_size, OUT slice_size );

                            CHECK( All( dim == mip_dim ));
                            CHECK( size >= memView.ContentSize() );
                            CHECK( row_size == memView.RowPitch() );
                            CHECK( slice_size == memView.SlicePitch() );

                            auto    mem = file->Alloc( memView.ContentSize() );
                            CHECK_THROW( mem );

                            CHECK( memView.CopyTo( mem->Data(), memView.ContentSize() ));

                            Unused( file->WriteBlock( off + SizeOf<AssetPacker::ImagePacker::Header2>, memView.ContentSize(), RVRef(mem) ));
                        });

            if ( op.stream.IsCompleted() )
            {
                const auto  img_desc = _imageDesc.Read<ImageDesc>();

                // invalidate
                op.stream = Default;

                if ( ++op.curLayer < img_desc.arrayLayers )
                    continue;

                if ( ++op.curMipmap < img_desc.maxLevel )
                {
                    op.curLayer = ImageLayer{};
                    continue;
                }

                op.complete = true;
            }

            all_complete &= op.IsCompleted();
        }

        if ( failed )
        {
            _storeOps.clear();
            _SetUploadStatus( EUploadStatus::Canceled );
        }

        if ( all_complete )
        {
            _storeOps.clear();
            _SetUploadStatus( EUploadStatus::Completed );
        }

        return _uploadStatus.load();
    }

/*
=================================================
    Cancel
=================================================
*/
    void  Image::Cancel () __NE___
    {
        IResource::Cancel();

        for (auto& op : _loadOps)
        {
            if ( op.file )      op.file->CancelAllRequests();
            if ( op.loaded )    op.loaded.Cancel();
        }
        _loadOps.clear();

        for (auto& op : _storeOps)
        {
            if ( op.file )      CHECK( not op.file->CancelAllRequests() );  // all write requests must complete
        }
        _storeOps.clear();
    }

/*
=================================================
    CreateView
=================================================
*/
    RC<Image>  Image::CreateView (const ImageViewDesc &viewDesc, StringView dbgName) __NE___
    {
        //CHECK_ERR( _uploadStatus == EUploadStatus::Completed );

        RC<Image>   result  {new Image{ _Renderer(), dbgName }};

        result->_inDynSize  = _inDynSize;
        result->_base       = GetRC();
        result->_uploadStatus.store( EUploadStatus::Completed );

        _derived->insert( result.get() );

        CHECK_ERR( result->_OnResize( viewDesc ));
        return result;
    }

/*
=================================================
    _OnResize
=================================================
*/
    bool  Image::_OnResize (const ImageViewDesc &viewDesc)
    {
        auto&   res_mngr    = RenderTaskScheduler().GetResourceManager();
        auto&   base        = *_base;

        auto    image       = res_mngr.AcquireResource( base._id.Get() );
        CHECK_ERR( image );

        auto    old_img     = _id.Attach( RVRef(image) );
        res_mngr.ReleaseResource( old_img );

        auto    view        = res_mngr.CreateImageView( viewDesc, _id.Get(), _dbgName );
        CHECK_ERR( view );

        auto    old_view    = _view.Attach( RVRef(view) );
        res_mngr.ReleaseResource( old_view );

        _imageDesc.WriteAll(
            res_mngr.GetDescription( _id ),
            res_mngr.GetDescription( _view ));

        return true;
    }

/*
=================================================
    _Remove
=================================================
*/
    void  Image::_Remove (Image* derived)
    {
        _derived->erase( derived );
    }

/*
=================================================
    _CreateImage
=================================================
*/
    template <typename CtxType>
    bool  Image::_CreateImage (const ResLoader::IntermImage &intermImg, MipmapLevel baseMipmap, ImageLayer baseLayer, bool allMipmaps, CtxType &ctx)
    {
        ASSERT( _isDummy.load() );
        ASSERT( not intermImg.IsEmpty() );

        auto&       res_mngr = RenderTaskScheduler().GetResourceManager();

        const auto  Create   = [&] (auto& intermImg) -> bool
        {{
            ImageDesc       desc        = GetImageDesc();
            ImageViewDesc   view_desc   = GetViewDesc();
            CHECK_ERR( CompareImageTypes( desc, intermImg ));

            desc.dimension      = intermImg.Dimension() << baseMipmap.Get();
            desc.arrayLayers    = ImageLayer{ intermImg.ArrayLayers() + baseLayer.Get() };
            desc.maxLevel       = MipmapLevel{ intermImg.MipLevels() + baseMipmap.Get() };
            desc.imageDim       = intermImg.GetImageDim();
            // keep: desc.options
            // keep: desc.usage
            desc.format         = intermImg.PixelFormat();
            // keep: desc.samples

            if ( allMipmaps )
            {
                desc.maxLevel           = MipmapLevel::Max();
                view_desc.mipmapCount   = UMax;
            }

            desc.Validate();
            view_desc.Validate( desc );

            // create image
            {
                CHECK_ERR_MSG( res_mngr.IsSupported( desc ),
                    "Image '"s << _dbgName << "' description is not supported by GPU device" );

                auto    image   = res_mngr.CreateImage( desc, _dbgName, _GfxAllocator() );
                CHECK_ERR( image );

                RenderGraph().GetStateTracker().AddResource( image,
                                                             EResourceState::_InvalidState,                             // current is not used
                                                             EResourceState::ShaderSample | EResourceState::AllShaders, // default
                                                             ctx.GetCommandBatchRC() );
                ctx.ResourceState( image, EResourceState::Invalidate );

                _imageDesc.Write( res_mngr.GetDescription( image ));

                auto    old_img = _id.Attach( RVRef(image) );
                res_mngr.ReleaseResource( old_img );    // release dummy resource
            }

            // create image view
            {
                CHECK_ERR_MSG( res_mngr.IsSupported( _id.Get(), view_desc ),
                    "Image view '"s << _dbgName << "' description is not supported by GPU device" );

                auto    view = res_mngr.CreateImageView( view_desc, _id.Get(), _dbgName );
                CHECK_ERR( view );

                _imageDesc.Write( res_mngr.GetDescription( view ));

                auto    old_view = _view.Attach( RVRef(view) );
                res_mngr.ReleaseResource( old_view );   // release dummy resource
            }
            return true;
        }};

        bool    res = Create( intermImg );

        if ( res )
        {
            if ( _outDynSize )
            {
                ASSERT( intermImg.GetImageDim() == _outDynSize->NumDimensions() );
                _outDynSize->Resize( intermImg.Dimension() );
            }
        }
        else
        {
            auto    view    = _view.Release();
            auto    id      = _id.Release();
            res_mngr.ImmediatelyReleaseResources( view, id );
        }
        return res;
    }

/*
=================================================
    _Load
=================================================
*/
    Image::IntermImageRC  Image::_Load (const AsyncDSRequestResult &in, EImageFormat fileFormat)
    {
        using namespace ResLoader;

        CHECK_ERR( in.data != null );
        CHECK_ERR( fileFormat != Default );

        IntermImageRC   result = MakeRC<IntermImage>();
        MemRefRStream   stream  { in.data, in.dataSize };
        AllImageLoaders loader;

        if ( loader.LoadImage( INOUT *result, stream, False{"no flipY"}, null, fileFormat ))
            return result;

        return null;
    }


} // AE::ResEditor
