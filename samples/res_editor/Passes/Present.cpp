// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Algorithms/StringUtils.h"
#include "base/DataSource/FileStream.h"

#include "res_editor/Passes/Renderer.h"
#include "res_editor/Passes/Present.h"
#include "res_editor/EditorUI.h"
#include "res_editor/EditorCore.h"

#include "res_loaders/All/AllImageSavers.h"

#include "res_editor/_ui_data/cpp/types.h"

#include "video/FFmpeg/FFmpegVideoEncoder.h"

namespace AE::ResEditor
{

/*
=================================================
    PresentAsync
=================================================
*/
    AsyncTask  Present::PresentAsync (const PresentPassData &pd) __NE___
    {
        const auto  sizes = pd.surface->GetTargetSizes();

        if ( _dynSize and not sizes.empty() )
            _dynSize->Resize( sizes[0] );

        {
            auto&       ui      = UIInteraction::Instance();
            const auto  capture = ui.capture.Read();

            if ( capture.video != bool{_videoEncoder} )
            {
                if ( _videoEncoder )
                    CHECK( _videoEncoder->End() );

                _videoEncoder = null;

                if ( capture.video )
                    _videoEncoder = _CreateEncoder( capture.bitrate, capture.videoFormat, capture.videoCodec, capture.colorPreset );
            }
        }

        return pd.batch.Task( _Blit( GetRC<Present>(), *pd.surface ), {"Present", HtmlColor::Violet} )
                        .Run( Tuple{pd.deps} );
    }

/*
=================================================
    _Blit
=================================================
*/
    RenderTaskCoro  Present::_Blit (RC<Present> self, IOutputSurface &surface)
    {
        IOutputSurface::RenderTargets_t     targets;
        CHECK_CE( surface.GetTargets( OUT targets ));

        auto&       src     = self->_src[0];
        const auto  desc    = src->GetImageDesc();
        auto&       dst     = targets[0];
        RenderTask& rtask = co_await RenderTask_GetRef;

        DirectCtx::Transfer ctx {rtask, Default, DebugLabel{self->_dbgName, HtmlColor::Blue}};

        ctx.AddSurfaceTargets( targets );

        ImageBlit   blit;
        blit.srcSubres  = { EImageAspect::Color, 0_mipmap, 0_layer, 1u };
        blit.srcOffset0 = { 0u, 0u, 0u };
        blit.srcOffset1 = { desc.dimension.x, desc.dimension.y, 1u };
        blit.dstSubres  = { EImageAspect::Color, 0_mipmap, 0_layer, 1u };
        blit.dstOffset0 = { dst.region.left,  dst.region.top,    0 };
        blit.dstOffset1 = { dst.region.right, dst.region.bottom, 1 };

        ctx.BlitImage( src->GetImageId(), dst.imageId, EBlitFilter::Linear, ArrayView<ImageBlit>{ &blit, 1 });


        // read pixel color for debugging
        {
            ReadbackImageDesc       readback;
            readback.heapType       = EStagingHeapType::Static;
            readback.imageSize      = uint3{1};
            readback.imageOffset    = uint3{ UIInteraction::Instance().selectedPixel.ConstPtr()->pendingPos, 0 };
            readback.imageOffset    = Min( readback.imageOffset, desc.dimension-1u );

            auto    task = ctx.ReadbackImage( src->GetImageId(), readback )
                            .Then(  [fid = ctx.GetFrameId()] (const ImageMemView &inView)
                                    {
                                        RWImageMemView  view {inView};
                                        RGBA32f         color;
                                        view.Load( uint3{}, OUT color );

                                        auto    sp = UIInteraction::Instance().selectedPixel.WriteNoLock();
                                        EXLOCK( sp );

                                        sp->frame   = fid;
                                        sp->pos     = uint2{view.Offset()};
                                        sp->color   = color;
                                    });
            //RenderTaskScheduler().AddNextCycleDeps( AsyncTask{task} );
        }

        // read image for screenshot / video
        {
            auto&       ui      = UIInteraction::Instance();
            const auto  capture = ui.capture.Read();

            ui.capture->screenshot = false;

            if ( capture.screenshot or capture.video )
            {
                ReadbackImageDesc   readback;
                readback.heapType   = EStagingHeapType::Dynamic;
                readback.imageSize  = desc.dimension;

                auto    task = ctx.ReadbackImage( src->GetImageId(), readback )
                                .Then(  [self, capture, encoder = self->_videoEncoder] (const ImageMemView &inView)
                                        {
                                            if ( capture.screenshot )
                                                _SaveScreenshot( inView, capture.imageFormat );

                                            if ( capture.video and encoder )
                                                Unused( encoder->AddFrame( inView, True{"end encoding on error"} ));
                                        });
                //RenderTaskScheduler().AddNextCycleDeps( AsyncTask{task} );
            }
        }

        co_await RenderTask_Execute( ctx );
    }

/*
=================================================
    _SaveScreenshot
=================================================
*/
    void  Present::_SaveScreenshot (const ImageMemView &inView, EImageFormat fmt)
    {
        ResLoader::IntermImage  image;
        CHECK_ERRV( image.Copy( inView ));

        const auto& screenshot_folder   = ResEditorAppConfig::Get().screenshotFolder;

        const auto  BuildName = [&screenshot_folder, fmt] (OUT Path &fname, usize index)
        {{
            fname = screenshot_folder / ("screenshot_"s << ToString(index) << '.' << ImageFileFormatToExt( fmt ));
        }};

        const auto  WriteToFile = [&image, fmt] (const Path &fname) -> bool
        {{
            FileWStream     file {fname};
            if ( file.IsOpen() )
            {
                ResLoader::AllImageSavers   saver;
                if ( saver.SaveImage( file, image, fmt ))
                {
                    AE_LOGI( "Save screenshot to '"s << ToString(fname) << "'" );
                    return true;
                }
            }
            return true; // exit
        }};

        FileSystem::FindUnusedFilename( BuildName, WriteToFile );
    }

/*
=================================================
    _CreateEncoder
=================================================
*/
    RC<IVideoEncoder>  Present::_CreateEncoder (float bitrate, EVideoFormat videoFormat, EVideoCodec videoCodec, EVideoColorPreset preset) const
    {
        using namespace AE::Video;
        CHECK_ERR( _src.size() == 1 );

        const auto  desc    = _src[0]->GetImageDesc();
        const auto& fs      = RenderTaskScheduler().GetResourceManager().GetFeatureSet();

        IVideoEncoder::Config   cfg;
        cfg.srcFormat       = desc.format;
        cfg.dstFormat       = videoFormat;
        cfg.srcSize         = uint2{desc.dimension};
        cfg.dstSize         = cfg.srcSize;
        cfg.codec           = videoCodec;
        cfg.colorPreset     = preset;
        cfg.filter          = Video::EFilter::Bilinear;
        cfg.quality         = 0.9f;
        cfg.framerate       = FractionalI{ 60 };
        cfg.bitrate         = IVideoEncoder::Bitrate_t{ ulong(double(bitrate) * 1024.0) * 1024 };
        cfg.hwAccelerated   = true;
        cfg.targetGPU       = fs.devicesIds.include.First();
        cfg.targetCPU       = CpuArchInfo::Get().cpu.vendor;


        auto        result       = IVideoEncoder::CreateFFmpegEncoder();
        const auto& video_folder = ResEditorAppConfig::Get().videoFolder;
        StringView  ext          = result->GetFileExtension( cfg.codec );

        const auto  BuildName = [&video_folder, ext] (OUT Path &fname, usize index)
        {{
            fname = video_folder / ("video_"s << ToString(index) << '.' << ext);
        }};

        const auto  WriteToFile = [result, &cfg] (const Path &fname) -> bool
        {{
            CHECK( result->Begin( cfg, fname ));
            return true; // exit
        }};

        FileSystem::FindUnusedFilename( BuildName, WriteToFile );
        CHECK_ERR( result->IsEncoding() );

        return result;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Execute
=================================================
*/
    bool  DebugView::Execute (SyncPassData &pd) __NE___
    {
        {
            auto&   ui = UIInteraction::Instance();
            ui.SetDbgView( _index, _copy );
        }

        // TODO: optimize - skip if dbg view disabled
        BEGIN_ENUM_CHECKS();
        switch ( _flags )
        {
            case EFlags::Unknown :
                return _CopyImage( pd );

            case EFlags::NoCopy :
                return true;

            case EFlags::Histogram :
            {
                CHECK_ERR( _src != _copy );
                CHECK_ERR( _pass );
                return _pass->Execute( *_src, *_copy, pd );
            }

            case EFlags::_Count :
            default :
                RETURN_ERR( "unsupported dbg view mode" );
        }
        END_ENUM_CHECKS();
    }

/*
=================================================
    _CopyImage
=================================================
*/
    bool  DebugView::_CopyImage (SyncPassData &pd) const
    {
        CHECK_ERR( _src != _copy );
        const auto  src_desc = _src->GetImageDesc();
        const auto  dst_desc = _copy->GetImageDesc();

        DirectCtx::Transfer  ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName, HtmlColor::Blue} };

        ImageBlit   blit;
        blit.srcSubres  = { EImageAspect::Color, 0_mipmap, 0_layer, 1u };
        blit.srcOffset0 = { 0u, 0u, 0u };
        blit.srcOffset1 = { src_desc.dimension.x, src_desc.dimension.y, 1u };
        blit.dstSubres  = { EImageAspect::Color, 0_mipmap, 0_layer, 1u };
        blit.dstOffset0 = { 0u, 0u, 0u };
        blit.dstOffset1 = { dst_desc.dimension.x, dst_desc.dimension.y, 1u };

        ctx.BlitImage( _src->GetImageId(), _copy->GetImageId(), EBlitFilter::Linear, ArrayView<ImageBlit>{ &blit, 1 });

        pd.cmdbuf = ctx.ReleaseCommandBuffer();
        return true;
    }

/*
=================================================
    constructor
=================================================
*/
    DebugView::DebugView (RC<Image> src, uint idx, EFlags flags, ImageLayer layer, MipmapLevel mipmap, Renderer* renderer, StringView dbgName) :
        _index{idx}, _flags{flags}, _dbgName{dbgName}
    {
        CHECK_THROW( src )
        CHECK_THROW( renderer != null );

        ImageViewDesc   view;
        view.baseLayer  = layer;
        view.baseMipmap = mipmap;

        const String    dbg_name = "DbgView: "s << src->GetName();

        _src = src->CreateView( view, dbg_name );
        CHECK_THROW( _src );

        ImageDesc   img_desc    = _src->GetImageDesc();
        const bool  make_copy   = flags != DebugView::EFlags::NoCopy;

        BEGIN_ENUM_CHECKS();
        switch ( flags )
        {
            case DebugView::EFlags::Unknown :
                img_desc.usage = EImageUsage::Transfer | EImageUsage::Sampled;
                break;

            case DebugView::EFlags::Histogram :
                img_desc.dimension  = uint3{ 1024, 1024, 1 };
                img_desc.usage      = EImageUsage::ColorAttachment | EImageUsage::Sampled;
                img_desc.format     = EPixelFormat::RGBA8_UNorm;    // defined in 'histogram.as'
                break;

            case DebugView::EFlags::NoCopy :
            case DebugView::EFlags::_Count :
                break;
        }
        END_ENUM_CHECKS();

        if ( make_copy )
        {
            auto&   res_mngr    = RenderTaskScheduler().GetResourceManager();
            auto    image_id    = res_mngr.CreateImage( img_desc, dbg_name, renderer->GetAllocator() );
            CHECK_THROW( image_id );

            auto    view_id     = res_mngr.CreateImageView( ImageViewDesc{img_desc}, image_id, dbg_name );
            CHECK_THROW( view_id );

            renderer->GetResourceQueue().EnqueueImageTransition( image_id );

            _copy = MakeRC<Image>( RVRef(image_id), RVRef(view_id), Default, *renderer, false, img_desc, null, null, dbg_name );
        }
        else
            _copy = _src;

        CHECK_THROW( _copy );

        BEGIN_ENUM_CHECKS();
        switch ( _flags )
        {
            case EFlags::Unknown :
                CHECK_THROW( _src != _copy );
                break;

            case EFlags::NoCopy :
                CHECK_THROW( _src == _copy );
                break;

            case EFlags::Histogram :
                _pass.reset( new Histogram{ renderer, *_src, *_copy }); // throw
                break;

            case EFlags::_Count :
            default :
                CHECK_THROW_MSG( false, "unsupported dbg view mode" );
        }
        END_ENUM_CHECKS();
    }

/*
=================================================
    destructor
=================================================
*/
    DebugView::~DebugView ()
    {
        auto&   ui = UIInteraction::Instance();
        ui.ResetDbgView( _index );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Histogram ctor
=================================================
*/
    DebugView::Histogram::Histogram (Renderer* renderer, const Image &src, const Image &copy) __Th___
    {
        CHECK_THROW( &src != &copy );
        CHECK_THROW( copy.GetImageDesc().format == EPixelFormat::RGBA8_UNorm );

        auto&       res_mngr    = RenderTaskScheduler().GetResourceManager();
        const auto  max_frames  = RenderTaskScheduler().GetMaxFrames();

        _rtech = res_mngr.LoadRenderTech( Default, RenderTechName{"Histogram.RTech"}, Default );
        CHECK_THROW( _rtech );

        _ppln1 = _rtech->GetComputePipeline( PipelineName{"Histogram.CSPass1.def"} );
        _ppln2 = _rtech->GetComputePipeline( PipelineName{"Histogram.CSPass2.def"} );
        _ppln3 = _rtech->GetGraphicsPipeline( PipelineName{"Histogram.draw"} );
        CHECK_THROW( _ppln1 and _ppln2 and _ppln3 );

        _ppln1LS = uint2{res_mngr.GetResource( _ppln1 )->LocalSize()};
        _ppln2LS = res_mngr.GetResource( _ppln2 )->LocalSize().x;

        _ssb = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ShaderTypes::Histogram_ssb>, EBufferUsage::Transfer | EBufferUsage::Storage},
                                      "Histogram SSB", renderer->GetAllocator() );
        CHECK_THROW( _ssb );

        CHECK_THROW( res_mngr.CreateDescriptorSets( OUT _ppln1DSIdx, OUT _ppln1DS.data(), max_frames, _ppln1, DescriptorSetName{"ds0"} ));
        CHECK_THROW( res_mngr.CreateDescriptorSets( OUT _ppln3DSIdx, OUT _ppln3DS.data(), max_frames, _ppln3, DescriptorSetName{"ds0"} ));
    }

/*
=================================================
    Histogram dtor
=================================================
*/
    DebugView::Histogram::~Histogram ()
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        res_mngr.ReleaseResourceArray( _ppln1DS );
        res_mngr.ReleaseResourceArray( _ppln3DS );
        res_mngr.ReleaseResource( _ssb );
    }

/*
=================================================
    Execute
=================================================
*/
    bool  DebugView::Histogram::Execute (const Image &srcImage, const Image &dstImage, SyncPassData &pd) const
    {
        const uint2         src_dim     = uint2{srcImage.GetImageDesc().dimension};
        const uint2         dst_dim     = uint2{dstImage.GetImageDesc().dimension};

        DirectCtx::Transfer copy_ctx    { pd.rtask, RVRef(pd.cmdbuf), DebugLabel{"Histogram pass1", HtmlColor::Blue} };
        DescriptorSetID     comp_ds     = _ppln1DS[ copy_ctx.GetFrameId().Index() ];
        DescriptorSetID     gfx_ds      = _ppln3DS[ copy_ctx.GetFrameId().Index() ];
        const uint          hg_size     = uint(CountOf( &ShaderTypes::Histogram_ssb::RGBLuma ));

        // update
        {
            DescriptorUpdater   updater;

            CHECK_ERR( updater.Set( comp_ds, EDescUpdateMode::Partialy ));
            CHECK_ERR( updater.BindBuffer< ShaderTypes::Histogram_ssb >( UniformName{"un_Histogram"}, _ssb ));
            CHECK_ERR( updater.BindImage(  UniformName{"un_Texture"},   srcImage.GetViewId() ));
            CHECK_ERR( updater.Flush() );

            CHECK_ERR( updater.Set( gfx_ds, EDescUpdateMode::Partialy ));
            CHECK_ERR( updater.BindBuffer< ShaderTypes::Histogram_ssb >( UniformName{"un_Histogram"}, _ssb ));
            CHECK_ERR( updater.Flush() );
        }

        // clear
        {
            copy_ctx.FillBuffer( _ssb, 0_b, SizeOf<ShaderTypes::Histogram_ssb>, 0 );
        }

        DirectCtx::Compute  comp_ctx    { pd.rtask, copy_ctx.ReleaseCommandBuffer(), DebugLabel{"Histogram pass2", HtmlColor::Blue} };

        // pass 1
        {
            comp_ctx.ResourceState( srcImage.GetImageId(), EResourceState::ShaderSample | EResourceState::ComputeShader );
            comp_ctx.ResourceState( _ssb, EResourceState::ShaderStorage_RW | EResourceState::ComputeShader );
            comp_ctx.CommitBarriers();

            comp_ctx.BindPipeline( _ppln1 );
            comp_ctx.BindDescriptorSet( _ppln1DSIdx, comp_ds );
            comp_ctx.Dispatch( DivCeil( src_dim, _ppln1LS ));
        }

        // pass 2
        {
            comp_ctx.ResourceState( _ssb, EResourceState::ShaderStorage_RW | EResourceState::ComputeShader );
            comp_ctx.CommitBarriers();

            comp_ctx.BindPipeline( _ppln2 );
            comp_ctx.BindDescriptorSet( _ppln1DSIdx, comp_ds );
            comp_ctx.Dispatch( DivCeil( hg_size, _ppln2LS ));
        }


        DirectCtx::Graphics gfx_ctx     { pd.rtask, comp_ctx.ReleaseCommandBuffer(), DebugLabel{"Histogram pass3", HtmlColor::Blue} };
        const uint          num_verts   = uint(CountOf( &ShaderTypes::Histogram_ssb::RGBLuma )) * 2 * 4;

        // pass 3
        {
            RenderPassDesc  rp_desc{ _rtech, RenderTechPassName{"Graphics"}, dst_dim };
            rp_desc.AddTarget( AttachmentName{"Color"}, dstImage.GetViewId(), RGBA32f{0.f} );
            rp_desc.DefaultViewport();

            auto    dctx = gfx_ctx.BeginRenderPass( rp_desc );

            dctx.BindPipeline( _ppln3 );
            dctx.BindDescriptorSet( _ppln3DSIdx, gfx_ds );
            dctx.Draw( num_verts );

            gfx_ctx.EndRenderPass( dctx );
        }

        pd.cmdbuf = gfx_ctx.ReleaseCommandBuffer();
        return true;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Execute
=================================================
*/
    bool  GenerateMipmaps::Execute (SyncPassData &pd) __NE___
    {
        DirectCtx::Transfer     ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{"GenerateMipmaps", HtmlColor::Blue} };

        ctx.GenerateMipmaps( _image->GetImageId() );

        pd.cmdbuf = ctx.ReleaseCommandBuffer();
        return true;
    }


} // AE::ResEditor