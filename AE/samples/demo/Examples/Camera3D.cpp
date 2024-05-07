// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Examples/Camera3D.h"

namespace AE::Samples::Demo
{
	StaticAssert( sizeof(ShaderTypes::CubeVertex) == sizeof(GeometryTools::CubeRenderer::Vertex) );
	StaticAssert( sizeof(ShaderTypes::SphericalCubeVertex) == sizeof(GeometryTools::SphericalCubeRenderer::Vertex) );

	INTERNAL_LINKAGE( constexpr auto&	RTech	= RenderTechs::Scene3D_RTech );
	INTERNAL_LINKAGE( constexpr auto&	IA		= InputActions::Camera3D );


	//
	// Upload Texture Task
	//
	class Camera3DSample::UploadTextureTask final : public RenderTask
	{
	public:
		RC<Camera3DSample>	t;

		UploadTextureTask (Camera3DSample* p, CommandBatchPtr batch, DebugLabel) __NE___ :
			RenderTask{ batch, {"Camera3D::UploadTexture"} },
			t{ p }
		{}

		void  Run () __Th_OV;
	};

/*
=================================================
	UploadTextureTask::Run
=================================================
*/
	void  Camera3DSample::UploadTextureTask::Run ()
	{
		DirectCtx::Transfer		ctx{ *this };

		// load texture
		{
			auto	file = GetVFS().Open<RStream>( VFS::FileName{"camera3d.cube"} );
			CHECK_TE( file );

			LoadableImage::Loader	loader;
			auto	image = loader.Load( file, ctx, t->gfxAlloc );
			CHECK_TE( image );

			t->cubeMap = image->ReleaseImageAndView();

			ctx.AccumBarriers()
				.ImageBarrier( t->cubeMap.image, EResourceState::CopyDst, EResourceState::ShaderSample | EResourceState::FragmentShader );
		}

		// create cube
		{
			auto&	res_mngr = GraphicsScheduler().GetResourceManager();

			CHECK_TE( t->cube1.Create( res_mngr, ctx, True{"cubeMap"}, t->gfxAlloc ));
			CHECK_TE( t->cube2.Create( res_mngr, ctx, t->lod, t->lod, False{"tris"}, True{"cubeMap"}, Default, t->gfxAlloc ));

			ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::CopyDst, EResourceState::VertexBuffer )
				.MemoryBarrier( EResourceState::CopyDst, EResourceState::IndexBuffer );
		}

		// update DS
		{
			DescriptorUpdater	updater;
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
	class Camera3DSample::ProcessInputTask final : public IAsyncTask
	{
	public:
		RC<Camera3DSample>	t;
		ActionQueueReader	reader;

		ProcessInputTask (Camera3DSample* p, ActionQueueReader reader) __NE___ :
			IAsyncTask{ ETaskQueue::PerFrame },
			t{ p }, reader{ RVRef(reader) }
		{}

		void  Run () __Th_OV
		{
			float3	move;
			float2	rotation;
			Quat	rot_quat {Zero};
			float3	sensor3f;

			ActionQueueReader::Header	hdr;
			for (; reader.ReadHeader( OUT hdr );)
			{
				StaticAssert( IA.actionCount == 1 );
				StaticAssert( IA.Desktop.actionCount == 1 );
				StaticAssert( IA.Mobile.actionCount == 1 );

				switch ( uint{hdr.name} )
				{
					case IA.Camera_Rotate :
						rotation += reader.DataCopy<float2>( hdr.offset );		break;

					case IA.Desktop.Camera_Move :
						move += reader.DataCopy<float3>( hdr.offset );			break;

					//case IA.Mobile.Camera_Rotate3D :
					//	rot_quat = Quat{reader.DataCopy<Quat>( hdr.offset )};	break;

					case IA.Mobile.Camera_Sensor3f :
						sensor3f = reader.DataCopy<float3>( hdr.offset );		break;
				}
			}

			//AE_LOGI( "Sensor3f: "s << ToString(sensor3f) );

			if ( rot_quat.LengthSq() > 0.f )
				t->camera.SetOrientation( rot_quat );
			else
				t->camera.Rotate( Rad{rotation.x}, Rad{rotation.y} );

			t->camera.Move3D( move );
		}

		StringView	DbgName ()	C_NE_OV	{ return "Camera3D::ProcessInput"; }
	};
//-----------------------------------------------------------------------------



	//
	// Draw Task
	//
	class Camera3DSample::DrawTask final : public RenderTask
	{
	public:
		RC<Camera3DSample>	t;
		IOutputSurface &	surface;

		DrawTask (Camera3DSample* p, IOutputSurface &surf, CommandBatchPtr batch, DebugLabel) __NE___ :
			RenderTask{ batch, {"Camera3D::Draw"} },
			t{ p }, surface{ surf }
		{}

		void  Run () __Th_OV;
	};

/*
=================================================
	DrawTask::Run
=================================================
*/
	void  Camera3DSample::DrawTask::Run ()
	{
		IOutputSurface::RenderTargets_t		targets;
		CHECK_TE( surface.GetTargets( OUT targets ));

		const uint2		view_size	= targets[0].RegionSize();

		// resize depth buffer
		{
			auto&	res_mngr = GraphicsScheduler().GetResourceManager();

			if_unlikely( not t->depthBuf.image or Any( uint2{res_mngr.GetDescription( t->depthBuf.image ).dimension} != view_size ))
			{
				// delayed destruction
				res_mngr.DelayedReleaseResources( t->depthBuf.image, t->depthBuf.view );

				t->depthBuf.image = res_mngr.CreateImage( ImageDesc::CreateDepthAttachment( view_size, EPixelFormat::Depth16 ), "Sample3D depth" );
				CHECK_TE( t->depthBuf.image );

				t->depthBuf.view = res_mngr.CreateImageView( ImageViewDesc{}, t->depthBuf.image, "Sample3D depth view" );
				CHECK_TE( t->depthBuf.view );

				t->camera.SetPerspective( 90_deg, float(view_size.x) / view_size.y, 0.1f, 100.0f );
			}
		}


		DirectCtx::Transfer		copy_ctx{ *this };

		// for staging buffers
		copy_ctx.AccumBarriers()
			.MemoryBarrier( EResourceState::Host_Write, EResourceState::CopySrc );

		// update uniforms
		for (usize i = 0; i < targets.size(); ++i)
		{
			const auto&		rt	= targets[i];
			const Bytes		off	= AlignUp( SizeOf<ShaderTypes::camera3d_ub>, DeviceLimits.res.minUniformBufferOffsetAlign ) * i;

			ShaderTypes::camera3d_ub	ub;

			if ( rt.projection )
				ub.mvp = rt.projection->ViewProj() * t->camera.ToModelViewMatrix();
			else
				ub.mvp = t->camera.ToModelViewProjMatrix();

			// barrier is not needed because of semaphore
			CHECK_TE( copy_ctx.UploadBuffer( t->uniformBuf, off, Sizeof(ub), &ub ));

			copy_ctx.AccumBarriers()
				.BufferBarrier( t->uniformBuf, EResourceState::CopyDst, EResourceState::ShaderUniform | EResourceState::PreRasterizationShaders );
		}


		DirectCtx::Graphics		gfx_ctx{ *this, copy_ctx.ReleaseCommandBuffer() };

		// draw
		for (usize i = 0; i < targets.size(); ++i)
		{
			const auto&		rt	= targets[i];
			const uint		off	= uint(AlignUp( SizeOf<ShaderTypes::camera3d_ub>, DeviceLimits.res.minUniformBufferOffsetAlign ) * i);

			constexpr auto&	rtech_pass = RTech.Main;
			StaticAssert( rtech_pass.attachmentsCount == 2 );

			auto	dctx = gfx_ctx.BeginRenderPass( RenderPassDesc{ *t->rtech, rtech_pass, view_size }
								.AddViewport( view_size )
								.AddTarget( rtech_pass.att_Color, rt.viewId,		RGBA32f{HtmlColor::Black},	rt.initialState | EResourceState::Invalidate,	rt.finalState )
								.AddTarget( rtech_pass.att_Depth, t->depthBuf.view,	DepthStencil{1.0f},			EResourceState::Invalidate,						EResourceState::DepthStencilAttachment_RW | EResourceState::DSTestBeforeFS ));

			dctx.BindPipeline( t->ppln );
			dctx.BindDescriptorSet( t->dsIndex, t->descSet, {&off,1} );

			if ( t->use_cube1 )
				t->cube1.Draw( dctx );
			else
				t->cube2.Draw( dctx, t->lod );

			gfx_ctx.EndRenderPass( dctx );
		}

		Execute( gfx_ctx );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Init
=================================================
*/
	bool  Camera3DSample::Init (PipelinePackID pack, IApplicationTS) __NE___
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
				gfxAlloc = res_mngr.CreateLinearGfxMemAllocator();

		rtech = res_mngr.LoadRenderTech( pack, RTech, Default );
		CHECK_ERR( rtech );

		ppln = rtech->GetGraphicsPipeline( use_cube1 ? RTech.Main.camera3d_draw1 : RTech.Main.camera3d_draw2 );
		CHECK_ERR( ppln );

		uniformBuf = res_mngr.CreateBuffer( BufferDesc{ AlignUp( SizeOf<ShaderTypes::camera3d_ub>, DeviceLimits.res.minUniformBufferOffsetAlign ) * 2,
														EBufferUsage::Uniform | EBufferUsage::TransferDst },
											"Sample3D uniforms" );
		CHECK_ERR( uniformBuf );

		// update descriptors
		{
			auto [ds, idx] = res_mngr.CreateDescriptorSet( ppln, DescriptorSetName{"camera3d.ds0"} );
			CHECK_ERR( ds and idx == dsIndex );
			descSet = RVRef(ds);

			DescriptorUpdater	updater;

			CHECK_ERR( updater.Set( descSet, EDescUpdateMode::Partialy ));
			updater.BindBuffer( UniformName{"drawUB"}, uniformBuf, 0_b, SizeOf<ShaderTypes::camera3d_ub> );

			CHECK_ERR( updater.Flush() );
		}

		return true;
	}

/*
=================================================
	Update
=================================================
*/
	AsyncTask  Camera3DSample::Update (const IInputActions::ActionQueueReader &reader, ArrayView<AsyncTask> deps) __NE___
	{
		return Scheduler().Run< ProcessInputTask >( Tuple{ this, RVRef(reader) }, Tuple{deps} );
	}

/*
=================================================
	GetInputMode
=================================================
*/
	InputModeName  Camera3DSample::GetInputMode () C_NE___
	{
		return IA;
	}

/*
=================================================
	Draw
=================================================
*/
	AsyncTask  Camera3DSample::Draw (RenderGraph &rg, ArrayView<AsyncTask> inDeps) __NE___
	{
		auto	batch = rg.Render( "3D pass" );
		CHECK_ERR( batch );

		ArrayView<AsyncTask>	deps = inDeps;
		AsyncTask				upload [1];

		if ( not uploaded.load() )
		{
			uploaded.store( true );
			upload[0]	= batch->Run< UploadTextureTask >( Tuple{this}, Tuple{deps} );
			deps		= ArrayView<AsyncTask>{ upload };
		}

		auto	surf_acquire = rg.BeginOnSurface( batch, deps );
		CHECK_ERR( surf_acquire );

		return batch->Run< DrawTask >( Tuple{ this, rg.GetSurfaceArg() }, Tuple{surf_acquire}, True{"Last"}, Default );
	}

/*
=================================================
	destructor
=================================================
*/
	Camera3DSample::~Camera3DSample () __NE___
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		cube1.Destroy( res_mngr );
		cube2.Destroy( res_mngr );
		res_mngr.DelayedReleaseResources( uniformBuf, cubeMap.image, cubeMap.view, descSet, depthBuf.image, depthBuf.view );
	}


} // AE::Samples::Demo
