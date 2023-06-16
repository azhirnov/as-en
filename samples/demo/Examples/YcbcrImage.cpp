// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Examples/YcbcrImage.h"

namespace AE::Samples::Demo
{
	
	//
	// Process Input Task
	//
	class YcbcrImageSample::ProcessInputTask final : public Threading::IAsyncTask
	{
	public:
		RC<YcbcrImageSample>	t;
		ActionQueueReader		reader;

		ProcessInputTask (YcbcrImageSample* p, ActionQueueReader reader) :
			IAsyncTask{ ETaskQueue::PerFrame },
			t{ p }, reader{ RVRef(reader) }
		{}
		
		void  Run () __Th_OV;

		StringView	DbgName ()	C_NE_OV	{ return "YcbcrImage::ProcessInput"; }
	};
	
/*
=================================================
	ProcessInputTask::Run
=================================================
*/
	void  YcbcrImageSample::ProcessInputTask::Run ()
	{
	}
//-----------------------------------------------------------------------------
	


	//
	// Draw Task
	//
	class YcbcrImageSample::DrawTask final : public RenderTask
	{
	public:
		RC<YcbcrImageSample>	t;
		IOutputSurface &		surface;

		DrawTask (YcbcrImageSample* p, IOutputSurface &surf, CommandBatchPtr batch, DebugLabel) :
			RenderTask{ batch, {"YcbcrImage::DrawTask"} },
			t{ p }, surface{ surf }
		{}
		
		void  Run () __Th_OV;
	};
	
/*
=================================================
	DrawTask::Run
=================================================
*/
	void  YcbcrImageSample::DrawTask::Run ()
	{
		IOutputSurface::RenderTargets_t		targets;
		CHECK_TE( surface.GetTargets( OUT targets ));

		auto&	rt = targets[0];
		
		DirectCtx::Transfer		tctx{ *this };
		{
			tctx.AccumBarriers()
				.ImageBarrier( t->ycbcrImageId, EResourceState::Invalidate, EResourceState::CopyDst );

			const ubyte3	yuv		= ubyte3{ 0xFF, 0, 0 };
			ubyte			g_pixels  [ dim[0] * dim[1] ];
			ubyte			rb_pixels [ (dim[0] * dim[1] * 2) / 4 ];
			UploadImageDesc	upload;

			for (ubyte& c : g_pixels) {
				c = yuv[0];
			}
			for (usize i = 0; i < CountOf(rb_pixels)/2; i += 2) {
				rb_pixels[i+0] = yuv[1];
				rb_pixels[i+1] = yuv[2];
			}

			upload.imageSize		= uint3{ dim[0], dim[1], 1u };
			upload.dataRowPitch		= 1_b * dim[0];
			upload.dataSlicePitch	= Bytes{CountOf(g_pixels)};
			upload.aspectMask		= EImageAspect::Plane_0;

			Unused( tctx.UploadImage( t->ycbcrImageId, upload, ArrayView<ubyte>{g_pixels} ));
			
			upload.imageSize		= uint3{ dim[0]/2, dim[1]/2, 1u };
			upload.dataRowPitch		= 2_b * (dim[0]/2);
			upload.dataSlicePitch	= Bytes{CountOf(rb_pixels)};
			upload.aspectMask		= EImageAspect::Plane_1;

			Unused( tctx.UploadImage( t->ycbcrImageId, upload, ArrayView<ubyte>{rb_pixels} ));
		}

		DirectCtx::Graphics		gctx{ *this, tctx.ReleaseCommandBuffer() };
			
		gctx.AccumBarriers()
			.MemoryBarrier( EResourceState::Host_Write, EResourceState::VertexBuffer )
			.MemoryBarrier( EResourceState::Host_Write, EResourceState::IndexBuffer )
			.ImageBarrier( t->ycbcrImageId, EResourceState::ClearDst, EResourceState::ShaderSample | EResourceState::FragmentShader );
		
		const auto	rp_desc = RenderPassDesc{ t->rtech, RenderTechPassName{"Main"}, rt.RegionSize() }
								.AddViewport( rt.RegionSize() )
								.AddTarget( AttachmentName{"Color"}, rt.viewId, RGBA32f{ 0.f, 0.f, 0.f, 1.f }, rt.initialState | EResourceState::Invalidate, rt.finalState );

		auto	dctx = gctx.BeginRenderPass( rp_desc );
		{
			dctx.BindPipeline( t->ppln );
			dctx.BindDescriptorSet( t->dsIndex, t->descSet );

			dctx.Draw( 3 );
		}
		gctx.EndRenderPass( dctx, rp_desc );

		Execute( gctx );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Init
=================================================
*/
	bool  YcbcrImageSample::Init (PipelinePackID pack)
	{
		auto&	res_mngr = RenderTaskScheduler().GetResourceManager();
		
		rtech = res_mngr.LoadRenderTech( pack, RenderTechName{"Ycbcr.RTech"}, Default );
		CHECK_ERR( rtech );
		
		ppln	= rtech->GetGraphicsPipeline( PipelineName{"ycbcr.draw1"} );
		CHECK_ERR( ppln );
		
		{
			auto [ds, idx] = res_mngr.CreateDescriptorSet( ppln, DescriptorSetName{"ycbcr.ds0"} );
			CHECK_ERR( ds and idx == dsIndex );
			descSet = RVRef(ds);
		}
		{
			gfxAlloc = MakeRC<GfxLinearMemAllocator>();

			ycbcrImg = res_mngr.CreateVideoImage( VideoImageDesc{}.SetDimension( dim[0], dim[1] )
													.SetFormat( EPixelFormat::G8_B8R8_420_UNorm )
													.SetUsage( EImageUsage::Sampled | EImageUsage::TransferDst )
													.SetYcbcrConversion( SamplerName{ "NearestClamp|ycbcr|G8_B8R8_420_UNorm" }),
												  "", gfxAlloc );
			CHECK_ERR( ycbcrImg );

			ycbcrImageId = res_mngr.GetResource( ycbcrImg )->GetImageID();
		}
		{
			DescriptorUpdater	updater;
			
			CHECK( updater.Set( descSet, EDescUpdateMode::Partialy ));
			updater.BindVideoImage( UniformName{"un_YcbcrTexture"}, ycbcrImg );
			CHECK( updater.Flush() );
		}

		return true;
	}
	
/*
=================================================
	Update
=================================================
*/
	AsyncTask  YcbcrImageSample::Update (const ActionQueueReader &reader, ArrayView<AsyncTask> deps)
	{
		return Scheduler().Run< ProcessInputTask >( Tuple{ this, RVRef(reader) }, Tuple{ deps });
	}
	
/*
=================================================
	Draw
=================================================
*/
	AsyncTask  YcbcrImageSample::Draw (RenderGraph &rg, ArrayView<AsyncTask> deps)
	{
		auto	batch = rg.Render( "Canvas2D pass" );
		CHECK_ERR( batch );

		auto	surf_acquire = rg.BeginOnSurface( batch, deps );
		CHECK_ERR( surf_acquire );

		return batch->Run< DrawTask >( Tuple{ this, rg.GetSurfaceArg() }, Tuple{surf_acquire}, True{"Last"}, Default );
	}
	
/*
=================================================
	destructor
=================================================
*/
	YcbcrImageSample::~YcbcrImageSample ()
	{
		auto&	res_mngr = RenderTaskScheduler().GetResourceManager();

		res_mngr.DelayedReleaseResources( descSet, ycbcrImg );
	}


} // AE::Samples::Demo
