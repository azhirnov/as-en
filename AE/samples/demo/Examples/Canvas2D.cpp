// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Examples/Canvas2D.h"

namespace AE::Samples::Demo
{
	INTERNAL_LINKAGE( constexpr auto&	RTech	= RenderTechs::Canvas_RTech );
	INTERNAL_LINKAGE( constexpr auto&	IA		= InputActions::Canvas2D );


/*
=================================================
	_DrawTask
=================================================
*/
	RenderCoro  Canvas2DSample::_DrawTask (RC<Canvas2DSample> t, IOutputSurface &surface) __NE___
	{
		IOutputSurface::RenderTargets_t		targets;
		CHECK_CE( surface.GetTargets( OUT targets ));

		auto&	rt = targets[0];

		DirectCtx::Transfer		copy_ctx{ RenderCoro_Get() };

		// update
		{
			t->angle = Wrap( t->angle + 0.005f, -float(Rad::Pi()), float(Rad::Pi()) );

			const auto	rot		= float2x2::Rotate( Rad{t->angle} );
			const auto	iden	= float2x2::Identity();

			ShaderTypes::sdf_font_ublock	ublock_data;
			ublock_data.sdfScale	= t->sdfFont->GetSDFConfig().scale;
			ublock_data.sdfBias		= t->sdfFont->GetSDFConfig().bias;
		//	ublock_data.pxRange		= t->sdfFont->GetSDFConfig().pixRange2D;
			ublock_data.bgColor		= float4{0.f};
			ublock_data.rotation0	= iden[0];
			ublock_data.rotation1	= iden[1];

			copy_ctx.UpdateBuffer( t->ublock, 0_b, Sizeof(ublock_data), &ublock_data );

			ublock_data.rotation0	= rot[0];
			ublock_data.rotation1	= rot[1];

			copy_ctx.UpdateBuffer( t->ublock, AlignUp( SizeOf<ShaderTypes::sdf_font_ublock>, DeviceLimits.res.minUniformBufferOffsetAlign ), Sizeof(ublock_data), &ublock_data );
		}

		DirectCtx::Graphics		gfx_ctx{ RenderCoro_Get(), copy_ctx.ReleaseCommandBuffer() };

		gfx_ctx.AccumBarriers()
			.MemoryBarrier( EResourceState::CopyDst, EResourceState::UniformRead | EResourceState::PreRasterizationShaders | EResourceState::FragmentShader );

		Canvas	canvas;
		canvas.SetDimensions( rt );
		canvas.NextFrame( RenderCoro_Get().FrameId() );

		// draw
		{
			constexpr auto&	rtech_pass = RTech.Main;
			StaticAssert( rtech_pass.attachmentsCount == 1 );

			auto	dctx = gfx_ctx.BeginRenderPass( RenderPassDesc{ *t->rtech, rtech_pass, rt.RegionSize() }
									.AddViewport( rt.RegionSize() )
									.AddTarget( rtech_pass.att_Color, rt.viewId, RGBA32f{HtmlColor::Black}, rt.initialState | EResourceState::Invalidate, rt.finalState ));

			//{
			//	canvas.Draw( Circle2D{ 16, RectF{-0.9f, -0.5f, 0.2f, 0.2f}, HtmlColor::Blue });
			//	canvas.Flush( dctx, EPrimitive::LineList );
			//}

			// UV color
			{
				canvas.Draw( Rectangle2D{ RectF{-0.9f, -0.2f, -0.5f, 0.2f}, t->enter ? HtmlColor::Red : HtmlColor::BlueViolet });
				canvas.Draw( FilledCircle2D{ 16, RectF{0.5f, -0.2f, 0.9f, 0.2f}, RectF{0.f, 0.f, 1.f, 1.f}, HtmlColor::Green });

				canvas.Draw( NinePatch2D{ RectF{-0.4f, -0.9f, 0.4f, -0.1f},	RectF{0.2f, 0.2f, 0.2f, 0.2f},
										  RectF{0.f, 0.f, 1.f, 1.f},		RectF{0.25f, 0.25f, 0.25f, 0.25f},
										  HtmlColor::Orange });


				dctx.BindPipeline( t->ppln1 );
				canvas.Flush( dctx, EPrimitive::TriangleList );
			}

			// wireframe
			if ( t->ppln2_wire )
			{
				canvas.Draw( NinePatch2D{ RectF{-0.4f, 0.1f, 0.4f, 0.9f},	RectF{0.2f, 0.2f, 0.2f, 0.2f},
										  RectF{0.f, 0.f, 1.f, 1.f},		RectF{0.25f, 0.25f, 0.25f, 0.25f},
										  HtmlColor::Orange });
				dctx.BindPipeline( t->ppln2_wire );
				canvas.Flush( dctx, EPrimitive::TriangleList );
			}

			// cursor (UV color)
			{
				const float2	pos = ToSNorm( t->cursorPos / float2(rt.RegionSize()) );
				canvas.Draw( Rectangle2D{ RectF{-0.05f, -0.05f, 0.05f, 0.05f} + pos, HtmlColor::White });

				dctx.BindPipeline( t->ppln1 );
				canvas.Flush( dctx, EPrimitive::TriangleList );
			}

			// textured
			{
				canvas.Draw( Rectangle2D{ RectF{float2{ 0.2f }} + float2{0.5f, -0.9f}, RectF{0.f, 0.f, 1.f, 1.f}, HtmlColor::White });
				dctx.BindPipeline( t->ppln3 );
				dctx.BindDescriptorSet( t->dsIndex, t->ppln3_ds1 );
				canvas.Flush( dctx, EPrimitive::TriangleList );
			}{
				RectF	rect_uv;
				CHECK_CE( t->atlas->Get( ImageInAtlasName{"Blue"}, OUT rect_uv ));
				canvas.Draw( Rectangle2D{ RectF{float2{ 0.2f }} + float2{0.5f, -0.6f}, rect_uv, HtmlColor::White });

				dctx.BindPipeline( t->ppln3 );
				dctx.BindDescriptorSet( t->dsIndex, t->ppln3_ds2 );
				canvas.Flush( dctx, EPrimitive::TriangleList );
			}

			// text regions
			//const RectF	txt_region{ -0.38f, -0.7f, -0.15f, -0.45f };
			const RectF		txt_region{ -0.38f, -0.7f, -0.0f, -0.3f };
			{
				canvas.Draw( Rectangle2D{ txt_region, t->enter ? HtmlColor::Red : HtmlColor::DarkGray });
				dctx.BindPipeline( t->ppln2 );
				canvas.Flush( dctx, EPrimitive::TriangleList );
			}

			// text
			Canvas::FontParams	fnt_params;
			fnt_params.heightInPx	= t->fontHeight;

			const U8StringView	text {Cast<CharUtf8>( u8"111 223\n456\ngL^.-Я" )};

			fnt_params.color = HtmlColor::LightCoral;
			{
				canvas.DrawText( text, *t->font, fnt_params, txt_region );

				dctx.BindPipeline( t->ppln4 );
				dctx.BindDescriptorSet( t->dsIndex, t->ppln4_ds );
				canvas.Flush( dctx, EPrimitive::TriangleList );
			}


			fnt_params.color = HtmlColor::Lime;
			fnt_params.bold  = FloatToUNormShort(0.05f);
			{
				canvas.DrawText( text, *t->sdfFont, fnt_params, txt_region );

				dctx.BindPipeline( t->ppln5 );
				dctx.BindDescriptorSet( t->dsIndex, t->ppln5_ds, {0u} );
				canvas.Flush( dctx, EPrimitive::TriangleList );
			}

			fnt_params.color = HtmlColor::Lime;
			fnt_params.bold  = FloatToUNormShort(0.f);
			{
				canvas.DrawText( text, *t->sdfFont, fnt_params, RectF{0.2f, -0.5f, 1.f, 1.f} );

				dctx.BindPipeline( t->ppln5 );
				//dctx.BindDescriptorSet( t->dsIndex, t->ppln5_ds, {0u} );
				dctx.BindDescriptorSet( t->dsIndex, t->ppln5_ds, {uint(AlignUp( SizeOf<ShaderTypes::sdf_font_ublock>, DeviceLimits.res.minUniformBufferOffsetAlign ))} );
				canvas.Flush( dctx, EPrimitive::TriangleList );
			}

			t->profiler.Draw( canvas );

			gfx_ctx.EndRenderPass( dctx );
		}

		RenderCoro_Execute( gfx_ctx );
	}

/*
=================================================
	Init
=================================================
*/
	bool  Canvas2DSample::Init (PipelinePackID pack, IApplicationTS) __NE___
	{
		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();
				gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();

		rtech = res_mngr.LoadRenderTech( pack, RTech );
		CHECK_ERR( rtech );

		ppln1		= rtech->GetGraphicsPipeline( RTech.Main.canvas2d_draw1 );
		ppln2		= rtech->GetGraphicsPipeline( RTech.Main.canvas2d_draw2 );
	//	ppln2_wire	= rtech->GetGraphicsPipeline( RTech.Main.canvas2d_draw2_wire );
		ppln3		= rtech->GetGraphicsPipeline( RTech.Main.canvas2d_draw3 );
		ppln4		= rtech->GetGraphicsPipeline( RTech.Main.font_draw );
		ppln5		= rtech->GetGraphicsPipeline( RTech.Main.sdf_font_draw );
		CHECK_ERR( ppln1 and ppln2 and ppln3 and ppln4 and ppln5 );

		uploadMngr = MakeRC<ResourceUploadManager>();

		// For better performance should be used in memory meta data file.
		// This implementation will read file in current thread and then delay image data uploading.

		font	= RasterFont::Loader::Load( VFS::FileName{"canvas2d.font"}, gfxAlloc, *uploadMngr );
		tex		= LoadableImage::Loader::Load( VFS::FileName{"canvas2d.tex1"}, gfxAlloc, *uploadMngr );
		sdfFont	= RasterFont::Loader::Load( VFS::FileName{"canvas2d.sdf_font"}, gfxAlloc, *uploadMngr );
		atlas	= StaticImageAtlas::Loader::Load( VFS::FileName{"canvas2d.atlas"}, gfxAlloc, *uploadMngr );

		CHECK_ERR( font and sdfFont and tex and atlas );

		{
			auto [ds, idx] = res_mngr.CreateDescriptorSet( ppln3, DescriptorSetName{"canvas2d.ds0"} );
			CHECK_ERR( ds and idx == dsIndex );
			ppln3_ds1 = RVRef(ds);
		}{
			auto [ds, idx] = res_mngr.CreateDescriptorSet( ppln3, DescriptorSetName{"canvas2d.ds0"} );
			CHECK_ERR( ds and idx == dsIndex );
			ppln3_ds2 = RVRef(ds);
		}{
			auto [ds, idx] = res_mngr.CreateDescriptorSet( ppln4, DescriptorSetName{"font.ds0"} );
			CHECK_ERR( ds and idx == dsIndex );
			ppln4_ds = RVRef(ds);
		}{
			auto [ds, idx] = res_mngr.CreateDescriptorSet( ppln5, DescriptorSetName{"sdf_font.ds0"} );
			CHECK_ERR( ds and idx == dsIndex );
			ppln5_ds = RVRef(ds);
		}
		{
			ublock = res_mngr.CreateBuffer( BufferDesc{ AlignUp( SizeOf<ShaderTypes::sdf_font_ublock>, DeviceLimits.res.minUniformBufferOffsetAlign ) * 2,
														EBufferUsage::Uniform | EBufferUsage::Transfer },
											"Canvas2D UB", gfxAlloc );
			CHECK_ERR( ublock );
		}
		{
			DescriptorUpdater	updater;

			CHECK_ERR( updater.Set( ppln5_ds, EDescUpdateMode::Partialy ));
			updater.BindBuffer< ShaderTypes::sdf_font_ublock >( UniformName{"drawUB"}, ublock, 0_b );

			CHECK_ERR( updater.Set( ppln3_ds2, EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_Texture"}, atlas->ViewId() );

			CHECK_ERR( updater.Set( ppln4_ds, EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_Texture"}, font->ViewId() );

			CHECK_ERR( updater.Set( ppln5_ds, EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_Texture"}, sdfFont->ViewId() );

			CHECK_ERR( updater.Set( ppln3_ds1, EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_Texture"}, tex->ViewId() );

			CHECK_ERR( updater.Flush() );
		}

		CHECK( profiler.Initialize( null ));

		return true;
	}

/*
=================================================
	Update
=================================================
*/
	AsyncTask  Canvas2DSample::Update (const IInputActions::ActionQueueReader &reader, ArrayView<AsyncTask> deps) __NE___
	{
		return Scheduler().Run(
					ETaskQueue::PerFrame,
					_ProcessInputTask( GetRC<Canvas2DSample>(), RVRef(reader) ),
					Tuple{ deps },
					"Canvas2D::ProcessInput"
				);
	}

/*
=================================================
	_ProcessInputTask
=================================================
*/
	AsyncCoro  Canvas2DSample::_ProcessInputTask (RC<Canvas2DSample> t, ActionQueueReader reader) __NE___
	{
		t->enter = false;

		ActionQueueReader::Header	hdr;
		for (; reader.ReadHeader( OUT hdr );)
		{
			switch_IA( hdr.name )
			{
				case IA.Cursor :
					t->cursorPos = reader.Data<packed_float2>( hdr.offset );	break;

				case IA.Enter :
					t->enter = true;											break;
			}
			switch_end
		}
		co_return;
	}

/*
=================================================
	Draw
=================================================
*/
	AsyncTask  Canvas2DSample::Draw (RenderGraph &rg, ArrayView<AsyncTask> deps) __NE___
	{
		auto	batch = rg.Render( "Canvas2D pass" );
		CHECK_ERR( batch );

		auto	surf_acquire = rg.BeginOnSurface( batch, deps );
		CHECK_ERR( surf_acquire );

		auto	upload	= uploadMngr->UploadAsync( *batch, 1 );
		auto	draw	= batch->Run( _DrawTask( GetRC<Canvas2DSample>(), rg.GetSurface() ), Tuple{surf_acquire} );

		return batch->SubmitAsTask( Tuple{ upload, draw });
	}

/*
=================================================
	GetInputMode
=================================================
*/
	InputModeName  Canvas2DSample::GetInputMode () C_NE___
	{
		return IA;
	}

/*
=================================================
	destructor
=================================================
*/
	Canvas2DSample::~Canvas2DSample () __NE___
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		res_mngr.DelayedReleaseResources( ppln3_ds1, ppln3_ds2, ppln4_ds, ppln5_ds, ublock );
		profiler.Deinitialize();
	}


} // AE::Samples::Demo
