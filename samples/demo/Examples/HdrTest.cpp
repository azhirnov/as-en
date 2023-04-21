// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "demo/Examples/HdrTest.h"

# include "imgui.h"
# include "imgui_internal.h"

namespace AE::Samples::Demo
{

	//
	// Process Input Task
	//
	class HdrTestSample::ProcessInputTask final : public Threading::IAsyncTask
	{
	public:
		HdrTestSample&		t;
		ActionQueueReader	reader;

		ProcessInputTask (HdrTestSample* t, ActionQueueReader reader) :
			IAsyncTask{ ETaskQueue::Worker },
			t{ *t },
			reader{ RVRef(reader) }
		{}

		void  Run () override
		{
			t.imgui.mouseLBDown	= false;
			t.imgui.mouseWheel	= {};

			ActionQueueReader::Header	hdr;
			for (; reader.ReadHeader( OUT hdr );)
			{
				if ( hdr.name == InputActionName{"MousePos"} )
					t.imgui.mousePos = reader.Data<packed_float2>( hdr.offset );

				if ( hdr.name == InputActionName{"MouseLBDown"} )
					t.imgui.mouseLBDown = true;

				if ( hdr.name == InputActionName{"Touch"} ){
					t.imgui.mousePos    = reader.Data<packed_float2>( hdr.offset );
					t.imgui.touchActive = hdr.state != EGestureState::End;
				}
				if ( hdr.name == InputActionName{"Test.Click"} ){
					t.imgui.mousePos    = reader.Data<packed_float2>( hdr.offset );
					t.imgui.mouseLBDown = true;
				}
			}
		}

		StringView	DbgName ()	C_NE_OV	{ return "HDR::ProcessInput"; }
	};
//-----------------------------------------------------------------------------



	//
	// Draw Task
	//
	class HdrTestSample::DrawTask final : public RenderTask
	{
	// variables
	private:
		HdrTestSample &		t;
		IOutputSurface &	surface;


	// methods
	public:
		DrawTask (HdrTestSample* t, IOutputSurface &surf, CommandBatchPtr batch, DebugLabel) :
			RenderTask{ batch, {"HDR::Draw"} },
			t{ *t }, surface{ surf }
		{}
		
		void  Run () override
		{
			CHECK_TE( t.imgui.Draw( *this, surface,
									[] () {
										// draw profiler
									},
									[this] (DirectCtx::Draw &dctx)
									{
										dctx.BindPipeline( t.hdrPpln );
										dctx.Draw( 3 );
									}));
		}
	};
//-----------------------------------------------------------------------------



	//
	// Upload Task
	//
	class HdrTestSample::UploadTask final : public RenderTask
	{
	public:
		HdrTestSample&	t;

		UploadTask (HdrTestSample* t, CommandBatchPtr batch, DebugLabel) :
			RenderTask{ batch, {"HDR::Upload"} },
			t{ *t }
		{}
		
		void  Run () override
		{
			DirectCtx::Transfer	copy_ctx{ *this };
		
			CHECK_TE( t.imgui.Upload( copy_ctx ));
		
			Execute( copy_ctx );
		}
	};
//-----------------------------------------------------------------------------



/*
=================================================
	Init
=================================================
*/
	bool  HdrTestSample::Init (PipelinePackID pack)
	{
		auto&	res_mngr	= RenderTaskScheduler().GetResourceManager();
		auto	gfx_alloc	= MakeRC<GfxLinearMemAllocator>();
		auto	rtech		= res_mngr.LoadRenderTech( pack, RenderTechName{"HDR.RTech"}, Default );

		CHECK_ERR( imgui.Init( gfx_alloc, rtech ));
		
		hdrPpln = rtech->GetGraphicsPipeline( PipelineName{"hdr_test"} );
		CHECK_ERR( hdrPpln );
		
		return true;
	}
	
/*
=================================================
	Update
=================================================
*/
	AsyncTask  HdrTestSample::Update (const IInputActions::ActionQueueReader &reader, ArrayView<AsyncTask> deps)
	{
		return Scheduler().Run< ProcessInputTask >( Tuple{ this, RVRef(reader) }, Tuple{deps} );
	}

/*
=================================================
	Draw
=================================================
*/
	AsyncTask  HdrTestSample::Draw (RenderGraph &rg, ArrayView<AsyncTask> inDeps)
	{
		auto	batch = rg.Render( "HDR pass" );
		CHECK_ERR( batch );
		
		ArrayView<AsyncTask>	deps = inDeps;
		AsyncTask				upload;

		if ( not uploaded.load() )
		{
			uploaded.store( true );
			upload	= batch->Run< UploadTask >( Tuple{this}, Tuple{deps} );
			deps	= ArrayView<AsyncTask>{ upload };
		}
		
		auto	surf_acquire = rg.BeginOnSurface( batch, deps );
		CHECK_ERR( surf_acquire );

		return batch->Run< DrawTask >( Tuple{ this, rg.GetSurfaceArg() }, Tuple{surf_acquire}, True{"Last"} );
	}


} // AE::Samples::Demo

#endif // AE_ENABLE_IMGUI
