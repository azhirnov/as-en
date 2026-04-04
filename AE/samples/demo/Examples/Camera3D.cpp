// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Examples/Camera3D.h"

namespace AE::Samples::Demo
{
	StaticAssert( sizeof(ShaderTypes::CubeVertex) == sizeof(GeometryTools::CubeRenderer::Vertex) );
	StaticAssert( sizeof(ShaderTypes::SphericalCubeVertex) == sizeof(GeometryTools::SphericalCubeRenderer::Vertex) );

	INTERNAL_LINKAGE( constexpr auto&	RTech	= RenderTechs::Scene3D_RTech );
	INTERNAL_LINKAGE( constexpr auto&	IA		= InputActions::Camera3D );

/*
=================================================
	_DrawTask
=================================================
*/
	RenderCoro  Camera3DSample::_DrawTask (RC<Camera3DSample> t, IOutputSurface &surface) __NE___
	{
		IOutputSurface::RenderTargets_t		targets;
		CHECK_CE( surface.GetTargets( OUT targets ));  // compatible with VR

		const uint2		view_size	= targets[0].RegionSize();
		auto&			res_mngr	= GraphicsScheduler().GetResourceManager();

		// resize depth buffer
		if_unlikely( not t->depthBuf.image or Any( uint2{res_mngr.GetDescription( t->depthBuf.image ).dimension} != view_size ))
		{
			// delayed destruction
			res_mngr.DelayedReleaseResources( t->depthBuf.image, t->depthBuf.view );

			t->depthBuf.image = res_mngr.CreateImage( ImageDesc::CreateDepthAttachment( view_size, EPixelFormat::Depth16 ), "Sample3D depth" );
			CHECK_CE( t->depthBuf.image );

			t->depthBuf.view = res_mngr.CreateImageView( ImageViewDesc{}, t->depthBuf.image, "Sample3D depth view" );
			CHECK_CE( t->depthBuf.view );

			t->camera.SetPerspective( 90_deg, float(view_size.x) / view_size.y, 0.1f, 100.0f );
		}


		DirectCtx::Transfer		copy_ctx{ RenderCoro_Get() };

		// create cube
		if_unlikely( not t->cube1.IsCreated() and not t->cube2.IsCreated() )
		{
			CHECK_CE( t->cube1.Create( res_mngr, copy_ctx, True{"cubeMap"}, t->gfxAlloc ));
			CHECK_CE( t->cube2.Create( res_mngr, copy_ctx, t->lod, t->lod, False{"tris"}, True{"cubeMap"}, Default, t->gfxAlloc ));

			copy_ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::CopyDst, EResourceState::VertexBuffer )
				.MemoryBarrier( EResourceState::CopyDst, EResourceState::IndexBuffer );
		}

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
			CHECK_CE( copy_ctx.UploadBuffer( t->uniformBuf, off, Sizeof(ub), &ub ));

			copy_ctx.AccumBarriers()
				.ResourceBarrier( t->uniformBuf, EResourceState::CopyDst, EResourceState::ShaderUniform | EResourceState::PreRasterizationShaders );
		}


		DirectCtx::Graphics		gfx_ctx{ RenderCoro_Get(), copy_ctx.ReleaseCommandBuffer() };

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

		RenderCoro_Execute( gfx_ctx );
	}

/*
=================================================
	Init
=================================================
*/
	bool  Camera3DSample::Init (PipelinePackID pack, IApplicationTS) __NE___
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
				gfxAlloc = res_mngr.CreateLinearGfxMemAllocator();

		uploadMngr = MakeRC<ResourceUploadManager>();

		rtech = res_mngr.LoadRenderTech( pack, RTech );
		CHECK_ERR( rtech );

		ppln = rtech->GetGraphicsPipeline( use_cube1 ? RTech.Main.camera3d_draw1 : RTech.Main.camera3d_draw2 );
		CHECK_ERR( ppln );

		uniformBuf = res_mngr.CreateBuffer( BufferDesc{ AlignUp( SizeOf<ShaderTypes::camera3d_ub>, DeviceLimits.res.minUniformBufferOffsetAlign ) * 2,
														EBufferUsage::Uniform | EBufferUsage::TransferDst },
											"Sample3D uniforms" );
		CHECK_ERR( uniformBuf );

		cubeMap = LoadableImage::Loader::Load( VFS::FileName{"camera3d.cube"}, gfxAlloc, *uploadMngr );
		CHECK_ERR( cubeMap );

		// update descriptors
		{
			auto [ds, idx] = res_mngr.CreateDescriptorSet( ppln, DescriptorSetName{"camera3d.ds0"} );
			CHECK_ERR( ds and idx == dsIndex );
			descSet = RVRef(ds);

			DescriptorUpdater	updater;

			CHECK_ERR( updater.Set( descSet, EDescUpdateMode::Partialy ));
			updater.BindBuffer( UniformName{"drawUB"}, uniformBuf, 0_b, SizeOf<ShaderTypes::camera3d_ub> );
			updater.BindImage( UniformName{"un_ColorTexture"}, cubeMap->ViewId() );

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
		return Scheduler().Run(
					ETaskQueue::PerFrame,
					_ProcessInputTask( GetRC<Camera3DSample>(), RVRef(reader) ),
					Tuple{deps},
					"Camera3D::ProcessInput"
				);
	}

/*
=================================================
	_ProcessInputTask
=================================================
*/
	AsyncCoro   Camera3DSample::_ProcessInputTask (RC<Camera3DSample> t, ActionQueueReader reader) __NE___
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
		co_return;
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
	AsyncTask  Camera3DSample::Draw (RenderGraph &rg, ArrayView<AsyncTask> deps) __NE___
	{
		auto	batch = rg.Render( "3D pass" );
		CHECK_ERR( batch );

		auto	surf_acquire = rg.BeginOnSurface( batch, deps );
		CHECK_ERR( surf_acquire );

		auto	upload	= uploadMngr->UploadAsync( *batch, 1 );
		auto	draw	= batch->Run( _DrawTask( GetRC<Camera3DSample>(), rg.GetSurface() ), Tuple{surf_acquire} );

		return batch->SubmitAsTask( Tuple{ upload, draw });
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
		res_mngr.DelayedReleaseResources( uniformBuf, descSet, depthBuf.image, depthBuf.view );
	}


} // AE::Samples::Demo
