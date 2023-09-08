// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Core/RenderGraph.h"
#include "res_editor/Resources/VideoImage.h"
#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/Image.h"
#include "res_editor/Resources/RTScene.h"
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
        IResource{ renderer },
        _outDynSize{ RVRef(outDynSize) },
        _dbgName{ dbgName }
    {
        _decoder = Video::VideoFactory::CreateFFmpegDecoder();
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
        auto&   rstate      = RenderGraph().GetStateTracker();

        CHECK_THROW_MSG( res_mngr.IsSupported( desc ),
            "VideoImage '"s << _dbgName << "' description is not supported by GPU device" );

        for (uint i = 0; i < _MaxImages; ++i)
        {
            _ids[i] = res_mngr.CreateImage( desc, _dbgName + ToString(i), _GfxAllocator() );
            CHECK_THROW( _ids[i] );

            _views[i] = res_mngr.CreateImageView( ImageViewDesc{desc}, _ids[i], _dbgName + ToString(i) );
            CHECK_THROW( _views[i] );

            rstate.AddResource( _ids[i].Get(),
                                EResourceState::Invalidate,                                 // current is not used
                                EResourceState::ShaderSample | EResourceState::AllShaders,  // default
                                EQueueType::Graphics );
        }
        _uploadStatus.store( EUploadStatus::InProgress );

        _ResQueue().EnqueueImageTransition( _ids[0] );
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

        auto&   rstate = RenderGraph().GetStateTracker();
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
        using FrameInfo = Video::IVideoDecoder::FrameInfo;

        if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
            return stat;

        ImageMemView    src_mem;
        FrameInfo       info;
        const uint      idx     = (_imageIdx.load()+1) % _MaxImages;

        // init image stream
        {
            UploadImageDesc     upload;
            upload.imageSize    = uint3{ _dimension, 1u };
            upload.heapType     = EStagingHeapType::Dynamic;
            upload.aspectMask   = EImageAspect::Color;
            _stream             = ImageStream{ _ids[idx], upload };
        }

        // get video frame and upload it to GPU
        if ( _decoder->GetFrame( OUT src_mem, OUT info ))
        {
            _frameTimes[ idx ] = info.timestamp;

            ImageMemView    dst_mem;
            ctx.ResourceState( _ids[idx], EResourceState::Invalidate );
            ctx.UploadImage( _stream, OUT dst_mem );

            if ( not dst_mem.Empty() )
            {
                CHECK( dst_mem.Copy( uint3{}, dst_mem.Offset(), src_mem, dst_mem.Dimension() ));
            }

            CHECK( _stream.IsCompleted() );
            _imageIdx.store( idx );
        }
        else
        {
            // restart
            if ( not _decoder->SeekTo( 0 ))
                _SetUploadStatus( EUploadStatus::Canceled );
        }

        return _uploadStatus.load();
    }


} // AE::ResEditor
