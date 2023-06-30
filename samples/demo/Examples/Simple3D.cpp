// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Examples/Simple3D.h"

namespace AE::Samples::Demo
{
    STATIC_ASSERT( sizeof(ShaderTypes::CubeVertex) == sizeof(GeometryTools::CubeRenderer::Vertex) );
    STATIC_ASSERT( sizeof(ShaderTypes::SphericalCubeVertex) == sizeof(GeometryTools::SphericalCubeRenderer::Vertex) );


    //
    // Upload Texture Task
    //
    class Simple3DSample::UploadTextureTask final : public RenderTask
    {
    public:
        RC<Simple3DSample>  t;

        UploadTextureTask (Simple3DSample* p, CommandBatchPtr batch, DebugLabel) :
            RenderTask{ batch, {"Simple3D::UploadTexture"} },
            t{ p }
        {}

        void  Run () __Th_OV;
    };

/*
=================================================
    UploadTextureTask::Run
=================================================
*/
    void  Simple3DSample::UploadTextureTask::Run ()
    {
        DirectCtx::Transfer     ctx{ *this };

        // load texture
        {
            auto    file = GetVFS().OpenAsStream( VFS::FileName{"simple3d.cube"} );
            CHECK_TE( file );

            LoadableImage::Loader   loader;
            auto    image = loader.Load( file, ctx, t->gfxAlloc );
            CHECK_TE( image );

            t->cubeMap = image->ReleaseImageAndView();

            ctx.AccumBarriers()
                .ImageBarrier( t->cubeMap.image, EResourceState::CopyDst, EResourceState::ShaderSample | EResourceState::FragmentShader );
        }

        // create cube
        {
            auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

            CHECK_TE( t->cube1.Create( res_mngr, ctx, t->gfxAlloc ));
            CHECK_TE( t->cube2.Create( res_mngr, ctx, t->lod, t->lod, false, t->gfxAlloc ));

            ctx.AccumBarriers()
                .MemoryBarrier( EResourceState::CopyDst, EResourceState::VertexBuffer )
                .MemoryBarrier( EResourceState::CopyDst, EResourceState::IndexBuffer );
        }

        // update DS
        {
            DescriptorUpdater   updater;
            CHECK_TE( updater.Set( t->descSet, EDescUpdateMode::Partialy ));
            updater.BindImage( UniformName{"un_ColorTexture"}, t->cubeMap.view );
            CHECK_TE( updater.Flush() );
        }

        Execute( ctx );
    }
//-----------------------------------------------------------------------------



    //
    // Process Input Task
    //
    class Simple3DSample::ProcessInputTask final : public Threading::IAsyncTask
    {
    public:
        RC<Simple3DSample>  t;
        ActionQueueReader   reader;

        ProcessInputTask (Simple3DSample* p, ActionQueueReader reader) :
            IAsyncTask{ ETaskQueue::PerFrame },
            t{ p }, reader{ RVRef(reader) }
        {}

        void  Run () __Th_OV
        {
            packed_float3   move;
            packed_float2   rotation;

            ActionQueueReader::Header   hdr;
            for (; reader.ReadHeader( OUT hdr );)
            {
                if_unlikely( hdr.name == InputActionName{"Camera.Rotate"} )
                    rotation += reader.Data<packed_float2>( hdr.offset );

                if_unlikely( hdr.name == InputActionName{"Camera.Move"} )
                    move += reader.Data<packed_float3>( hdr.offset );
            }

            t->camera.Rotate( Rad{rotation.x}, Rad{rotation.y} );
            t->camera.Move3D( move );
        }

        StringView  DbgName ()  C_NE_OV { return "Simple3D::ProcessInput"; }
    };
//-----------------------------------------------------------------------------



    //
    // Draw Task
    //
    class Simple3DSample::DrawTask final : public RenderTask
    {
    public:
        RC<Simple3DSample>  t;
        IOutputSurface &    surface;

        DrawTask (Simple3DSample* p, IOutputSurface &surf, CommandBatchPtr batch, DebugLabel) :
            RenderTask{ batch, {"Simple3D::Draw"} },
            t{ p }, surface{ surf }
        {}

        void  Run () __Th_OV;
    };

/*
=================================================
    DrawTask::Run
=================================================
*/
    void  Simple3DSample::DrawTask::Run ()
    {
        IOutputSurface::RenderTargets_t     targets;
        CHECK_TE( surface.GetTargets( OUT targets ));

        auto&           rt          = targets[0];
        const uint2     view_size   = rt.RegionSize();

        // resize depth buffer
        {
            auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

            if_unlikely( not t->depthBuf.image or Any( uint2{res_mngr.GetDescription( t->depthBuf.image ).dimension} != view_size ))
            {
                // delayed destruction
                res_mngr.DelayedReleaseResources( t->depthBuf.image, t->depthBuf.view );

                t->depthBuf.image = res_mngr.CreateImage( ImageDesc::CreateDepthAttachment( view_size, EPixelFormat::Depth32F ), "Sample3D depth" );
                CHECK_TE( t->depthBuf.image );

                t->depthBuf.view = res_mngr.CreateImageView( ImageViewDesc{}, t->depthBuf.image, "Sample3D depth view" );
                CHECK_TE( t->depthBuf.view );

                t->camera.SetPerspective( 90_deg, float(view_size.x) / view_size.y, 0.1f, 100.0f );
            }
        }


        DirectCtx::Transfer     copy_ctx{ *this };

        // for staging buffers
        copy_ctx.AccumBarriers().MemoryBarrier( EResourceState::Host_Write, EResourceState::CopySrc );

        // update uniforms
        {
            ShaderTypes::simple3d_ub    ub;
            ub.mvp = t->camera.GetCamera().ToModelViewProjMatrix();

            // barrier is not needed because of semaphore
            CHECK_TE( copy_ctx.UploadBuffer( t->uniformBuf, 0_b, Sizeof(ub), &ub ));

            copy_ctx.AccumBarriers().BufferBarrier( t->uniformBuf, EResourceState::CopyDst, EResourceState::ShaderUniform | EResourceState::PreRasterizationShaders );
        }


        DirectCtx::Graphics     gctx{ *this, copy_ctx.ReleaseCommandBuffer() };

        // draw
        {
            const auto  rp_desc =
                RenderPassDesc{ t->rtech, RenderTechPassName{"Main"}, view_size }
                    .AddViewport( view_size )
                    .AddTarget( AttachmentName{"Color"}, rt.viewId,         RGBA32f{HtmlColor::Black},  rt.initialState | EResourceState::Invalidate,   rt.finalState )
                    .AddTarget( Attachment_Depth,        t->depthBuf.view,  DepthStencil{1.0f},         EResourceState::Invalidate,                     EResourceState::DepthStencilAttachment_RW | EResourceState::DSTestBeforeFS );

            auto    dctx = gctx.BeginRenderPass( rp_desc );

            dctx.BindPipeline( t->ppln );
            dctx.BindDescriptorSet( t->dsIndex, t->descSet );

            if ( t->use_cube1 )
                t->cube1.Draw( dctx );
            else
                t->cube2.Draw( dctx, t->lod );

            gctx.EndRenderPass( dctx, rp_desc );
        }

        Execute( gctx );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Init
=================================================
*/
    bool  Simple3DSample::Init (PipelinePackID pack)
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        gfxAlloc = MakeRC<GfxLinearMemAllocator>();

        rtech = res_mngr.LoadRenderTech( pack, RenderTechName{"Scene3D.RTech"}, Default );
        CHECK_ERR( rtech );

        ppln = rtech->GetGraphicsPipeline( use_cube1 ? PipelineName{"simple3d.draw1"} : PipelineName{"simple3d.draw2"} );
        CHECK_ERR( ppln );

        uniformBuf = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ShaderTypes::simple3d_ub>, EBufferUsage::Uniform | EBufferUsage::TransferDst }, "Sample3D uniforms" );
        CHECK_ERR( uniformBuf );

        // update descriptors
        {
            auto [ds, idx] = res_mngr.CreateDescriptorSet( ppln, DescriptorSetName{"simple3d.ds0"} );
            CHECK_ERR( ds and idx == dsIndex );
            descSet = RVRef(ds);

            DescriptorUpdater   updater;

            CHECK_ERR( updater.Set( descSet, EDescUpdateMode::Partialy ));
            updater.BindBuffer< ShaderTypes::simple3d_ub >( UniformName{"drawUB"}, uniformBuf );

            CHECK_ERR( updater.Flush() );
        }

        return true;
    }

/*
=================================================
    Update
=================================================
*/
    AsyncTask  Simple3DSample::Update (const IInputActions::ActionQueueReader &reader, ArrayView<AsyncTask> deps)
    {
        return Scheduler().Run< ProcessInputTask >( Tuple{ this, RVRef(reader) }, Tuple{deps} );
    }

/*
=================================================
    Draw
=================================================
*/
    AsyncTask  Simple3DSample::Draw (RenderGraph &rg, ArrayView<AsyncTask> inDeps)
    {
        auto    batch = rg.Render( "3D pass" );
        CHECK_ERR( batch );

        ArrayView<AsyncTask>    deps = inDeps;
        AsyncTask               upload [1];

        if ( not uploaded.load() )
        {
            uploaded.store( true );

            upload[0] = batch->Run< UploadTextureTask >( Tuple{this}, Tuple{deps} );
        }

        auto    surf_acquire = rg.BeginOnSurface( batch, deps );
        CHECK_ERR( surf_acquire );

        return batch->Run< DrawTask >( Tuple{ this, rg.GetSurfaceArg() }, Tuple{surf_acquire}, True{"Last"}, Default );
    }

/*
=================================================
    destructor
=================================================
*/
    Simple3DSample::~Simple3DSample ()
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        cube1.Destroy( res_mngr );
        cube2.Destroy( res_mngr );
        res_mngr.DelayedReleaseResources( uniformBuf, cubeMap.image, cubeMap.view, descSet, depthBuf.image, depthBuf.view );
    }


} // AE::Samples::Demo
