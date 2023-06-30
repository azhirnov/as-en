// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/VideoImage.h"
#include "res_editor/Passes/FrameGraph.h"
#include "res_editor/Passes/Renderer.h"

namespace AE::ResEditor
{

/*
=================================================
    constructor
=================================================
*/
    VideoImage::VideoImage (Renderer &          renderer,
                            const ImageDesc &   inDesc,
                            const Path &        path,
                            RC<DynamicDim>      outDynSize,
                            StringView          dbgName) __Th___ :
        IImageResource{ renderer },
        _outDynSize{ RVRef(outDynSize) },
        _dbgName{ dbgName }
    {
        _decoder = Video::IVideoDecoder::CreateFFmpegDecoder();
        // TODO: other decoders
        CHECK_THROW( _decoder );

        Video::IVideoDecoder::Config    in_cfg;
        in_cfg.dstFormat    = inDesc.format;
        in_cfg.filter       = Video::EFilter::Bilinear;

        CHECK_THROW( _decoder->Begin( in_cfg, path ));

        const auto  config  = _decoder->GetConfig();
        const auto  props   = _decoder->GetProperties();
        const auto* vstream = props.GetStream( config.videoStreamIdx );
        CHECK_THROW( vstream != null );

        ImageDesc   desc    = inDesc;
        desc.dimension  = uint3{ vstream->size, 1u };
        _dimension      = vstream->size;

        auto&   res_mngr    = RenderTaskScheduler().GetResourceManager();
        auto&   rstate      = FrameGraph().GetStateTracker();

        for (uint i = 0; i < _MaxImages; ++i)
        {
            _ids[i] = res_mngr.CreateImage( desc, _dbgName, _GfxAllocator() );
            CHECK_THROW( _ids[i] );

            _views[i] = res_mngr.CreateImageView( ImageViewDesc{desc}, _ids[i], _dbgName );
            CHECK_THROW( _views[i] );

            rstate.AddResource( _ids[i].Get(),
                                EResourceState::_InvalidState,                              // current is not used
                                EResourceState::ShaderSample | EResourceState::AllShaders,  // default
                                EQueueType::Graphics );
        }

        _uploadStatus.store( EUploadStatus::InProgress );
        _ResQueue().EnqueueForUpload( GetRC() );
    }

/*
=================================================
    destructor
=================================================
*/
    VideoImage::~VideoImage ()
    {
        if ( _decoder )
            CHECK( _decoder->End() );

        auto&   rstate = FrameGraph().GetStateTracker();
        rstate.ReleaseResourceArray( _ids );
        rstate.ReleaseResourceArray( _views );
    }

/*
=================================================
    Upload
=================================================
*/
    IResource::EUploadStatus  VideoImage::Upload (TransferCtx_t &ctx) __Th___
    {
        if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
            return stat;

        EXLOCK( _loadOpGuard );

        ImageMemView                    src_mem;
        Video::IVideoDecoder::FrameInfo info;
        const uint                      idx     = _NextIdx();
        ImageStream                     img_stream;

        // init image stream
        {
            UploadImageDesc upload;
            upload.imageSize    = uint3{ _dimension, 1u };
            upload.heapType     = EStagingHeapType::Dynamic;
            upload.aspectMask   = EImageAspect::Color;
            img_stream          = ImageStream{ _ids[idx], upload };
        }

        // get video frame and upload it to GPU
        if ( _decoder->GetFrame( OUT src_mem, OUT info ))
        {
            _frameTimes[ idx ] = info.timestamp;

            ImageMemView    dst_mem;
            ctx.ResourceState( _ids[idx], EResourceState::Invalidate );
            ctx.UploadImage( img_stream, OUT dst_mem );

            if ( not dst_mem.Empty() )
            {
                CHECK( dst_mem.Copy( uint3{}, dst_mem.Offset(), src_mem, dst_mem.Dimension() ));
            }

            ASSERT( img_stream.IsCompleted() );
            _imageIdx.store( idx );

            return EUploadStatus::NoMemory;
        }
        else
        {
            if ( not _decoder->SeekTo( 0 ))
                _uploadStatus.store( EUploadStatus::Canceled );

            return _uploadStatus.load();
        }
    }

/*
=================================================
    Cancel
=================================================
*/
    void  VideoImage::Cancel ()
    {
        EXLOCK( _loadOpGuard );

        if ( _uploadStatus.load() != EUploadStatus::Complete )
            _uploadStatus.store( EUploadStatus::Canceled );
    }


} // AE::ResEditor
