// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/Renderer.h"
#include "res_editor/Passes/OtherPasses.h"
#include "res_editor/Core/EditorUI.h"
#include "res_editor/Core/EditorCore.h"

#include "res_loaders/AllImages/AllImageSavers.h"

#include "res_editor/_ui_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	Present::Present (Array<RC<Image>> src, StringView dbgName, RC<DynamicDim> dynSize) __NE___ :
		IPass{dbgName}, _src{RVRef(src)}, _dynSize{dynSize}, _filterMode{UIInteraction::Instance().GetFilterMode()}
	{
		_dbgColor = RGBA8u{150};
	}

/*
=================================================
	PresentAsync
=================================================
*/
	AsyncTask  Present::PresentAsync (const PresentPassData &pd) __Th___
	{
		const auto	infos = pd.surface->GetTargetInfo();

		if ( _dynSize and not infos.empty() )
			_dynSize->Resize( infos[0].Dimension() );

		{
			auto&		ui		= UIInteraction::Instance();
			const auto	capture	= ui.capture.Read();
			auto		encoder	= _videoEncoder.load();

			if ( capture.video != bool{encoder} )
			{
				if ( encoder )
					CHECK( encoder->End() );

				_videoEncoder.reset();
				encoder = null;

				if ( capture.video )
				{
					encoder = _CreateEncoder( capture.bitrate, capture.videoFormat, capture.videoCodec, capture.colorPreset );

					if ( encoder )
						_videoEncoder.store( RVRef(encoder) );
					else
						ui.capture->video = false;
				}
			}
		}

		return pd.batch.Task( _Blit( GetRC<Present>(), *pd.surface ), {"Present", HtmlColor::Violet} )
						.Run( Tuple{pd.DepsRef()} );
	}

/*
=================================================
	_Blit
=================================================
*/
	RenderTaskCoro  Present::_Blit (RC<Present> self, IOutputSurface &surface)
	{
		IOutputSurface::RenderTargets_t		targets;
		CHECK_CE( surface.GetTargets( OUT targets ));

		auto&		src			= self->_src[0];
		const auto	src_desc	= src->GetImageDesc();
		const auto	src_view	= src->GetViewDesc();
		const uint2	src_dim		= src_view.Dimension2();

		auto&		dst			= targets[0];
		RenderTask&	rtask		= co_await RenderTask_GetRef;
		const auto	filter		= self->_filterMode->Get() == 0 ? EBlitFilter::Nearest : EBlitFilter::Linear;
		const bool	copy		= self->_filterMode->Get() == 2 and
								  EPixelFormat_IsCopySupported( src_desc.format, dst.format ) and
								  All( src_dim == dst.RegionSize() );

		DirectCtx::Transfer		ctx{ rtask, Default, DebugLabel{ self->_dbgName, self->_dbgColor }};

		ctx.AddSurfaceTargets( targets );

		if ( copy )
		{
			ImageCopy	range;
			range.extent				= uint3{src_dim, 1};
			range.srcSubres.aspectMask	= EImageAspect::Color;
			range.dstSubres.aspectMask	= EImageAspect::Color;

			ctx.CopyImage( src->GetImageId(), dst.imageId, {range} );
		}
		else
		{
			ImageBlit	blit;
			blit.srcSubres	= { EImageAspect::Color, src_view.baseMipmap, src_view.baseLayer, 1u };
			blit.srcOffset0	= uint3{ 0 };
			blit.srcOffset1	= uint3{ src_dim.x, src_dim.y, 1u };
			blit.dstSubres	= { EImageAspect::Color, 0_mipmap, 0_layer, 1u };
			blit.dstOffset0	= int3{ dst.region.left,  dst.region.top,    0 };
			blit.dstOffset1	= int3{ dst.region.right, dst.region.bottom, 1 };

			ctx.BlitImage( src->GetImageId(), dst.imageId, filter, {blit} );
		}

		// read pixel color for debugging
		{
			float2	unorm_pos = UIInteraction::Instance().selectedPixel.ConstPtr()->pendingPos;

			ReadbackImageDesc		readback;
			readback.heapType		= EStagingHeapType::Static;
			readback.imageDim		= uint3{1};
			readback.imageOffset	= uint3{float3{ unorm_pos * float2{src_dim}, 0.f }};
			readback.imageOffset	= Min( readback.imageOffset, uint3{src_dim - 1u, 0u} );

			ctx.ReadbackImage( src->GetImageId(), readback )
				.Then(	[fid = ctx.GetFrameId()] (const ImageMemView &inView)
						{
							RWImageMemView	view {inView};
							RGBA32f			color;
							view.Load( uint3{}, OUT color );

							auto	sp	= UIInteraction::Instance().selectedPixel.WriteLock();
							sp->frame	= fid;
							sp->pos		= uint2{view.Offset()};
							sp->color	= color;
						},
						"Read color under cursor",
						ETaskQueue::PerFrame
					);
		}

		// read image for screenshot / video
		{
			UIInteraction::Capture	capture;
			{
				auto	capture_wr	= UIInteraction::Instance().capture.WriteLock();
						capture		= *capture_wr;

				capture_wr->screenshot		= false;
				capture_wr->testScreenshot	= false;
			}

			if ( capture.video )
			{
				auto&		vi		= self->_videoInfo;
				auto		dt		= Min( GraphicsScheduler().GetFrameTimeDelta(), secondsf{vi.frameTime} );
				secondsd	exp_dur	{vi.frameTime * (vi.frameCount+1)};

				vi.duration += dt;

				if ( exp_dur < vi.duration )
					vi.frameCount ++;
				else
					capture.video = false;
			}

			if ( capture.screenshot or capture.testScreenshot or capture.video )
			{
				ReadbackImageDesc	readback;
				readback.heapType	= EStagingHeapType::Dynamic;
				readback.imageDim	= uint3{src_dim, 1u};

				ctx.ReadbackImage( src->GetImageId(), readback )
					.Then(	[self, capture, encoder = self->_videoEncoder.load()] (const ImageMemView &inView)
							{
								if ( capture.screenshot or capture.testScreenshot )
								{
									auto	image = MakeRC<ResLoader::IntermImage>();
									CHECK_ERRV( image->Copy( inView ));

									Scheduler().Run(
										ETaskQueue::Background,
										_SaveScreenshot( RVRef(image), capture.imageFormat, capture.testScreenshot ));
								}

								if ( capture.video and encoder )
									Unused( encoder->AddFrame( inView, True{"end encoding on error"} ));
							},
							"Screen capture",
							ETaskQueue::PerFrame
						);
			}
		}

		co_await RenderTask_Execute( ctx );
	}

/*
=================================================
	_SaveScreenshot
=================================================
*/
	CoroTask  Present::_SaveScreenshot (RC<ResLoader::IntermImage> image, EImageFormat fmt, const bool testScreenshot)
	{
		CHECK_CE( image );

		const auto&	cfg = ResEditorAppConfig::Get();

		if ( testScreenshot )
		{
			if ( image->PixelFormat() != EPixelFormat::RGBA8_UNorm )
				fmt = EImageFormat::DDS;

			const auto	fname = (cfg.testOutput / cfg.screenshotPrefix.Read()).replace_extension( ImageFileFormatToExt( fmt ));

			FileWStream		file { fname };
			CHECK_CE( file.IsOpen() );

			ResLoader::AllImageSavers	saver;
			CHECK_CE( saver.SaveImage( file, *image, fmt ));

			AE_LOGI( "Save screenshot to '"s << ToString(fname) << "'" );
			// TODO: compare with previous
		}
		else
		{
			const auto&	screenshot_folder	= cfg.screenshotFolder;
			auto		prefix				= cfg.screenshotPrefix.Read();

			if ( prefix.empty() )
				prefix = "screenshot_";

			const auto	BuildName = [&screenshot_folder, &prefix, fmt] (OUT Path &fname, usize index)
			{{
				fname = screenshot_folder / (String{prefix} << ToString(index) << '.' << ImageFileFormatToExt( fmt ));
			}};

			const auto	WriteToFile = [image, fmt] (const Path &fname) -> bool
			{{
				FileWStream		file {fname};
				if ( file.IsOpen() )
				{
					ResLoader::AllImageSavers	saver;
					if ( saver.SaveImage( file, *image, fmt ))
					{
						AE_LOGI( "Save screenshot to '"s << ToString(fname) << "'" );
						return true; // exit
					}
				}
				return true; // exit
			}};

			FileSystem::FindUnusedFilename( BuildName, WriteToFile );
		}
		co_return;
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

		const auto	desc	= _src[0]->GetImageDesc();

		IVideoEncoder::Config	cfg;
		cfg.srcFormat		= desc.format;
		cfg.dstFormat		= videoFormat;
		cfg.srcDim			= uint2{desc.dimension};
		cfg.dstDim			= cfg.srcDim;
		cfg.codec			= videoCodec;
		cfg.colorPreset		= preset;
		cfg.filter			= Video::EFilter::Bilinear;
		cfg.quality			= 0.9f;
		cfg.framerate		= FractionalI{ int(_videoInfo.frameRate) };
		cfg.bitrate			= Bitrate{ ulong(double(bitrate) * 1024.0) * 1024 };	// TODO
		cfg.hwAccelerated	= EHwAcceleration::Optional;
		cfg.targetCPU		= CpuArchInfo::Get().cpu.vendor;

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
	  # if RmG_UI_ON_HOST
		if ( auto glib = GraphicsScheduler().GetDevice().GetGraphicsLib() )
			cfg.targetGPU = glib->GetResourceManager()->GetFeatureSet().devicesIds.include.First();
	  # endif
	  #else
		cfg.targetGPU		= GraphicsScheduler().GetFeatureSet().devicesIds.include.First();
	  #endif

		auto		result		 = VideoFactory::CreateFFmpegEncoder();
		const auto&	video_folder = ResEditorAppConfig::Get().videoFolder;
		StringView	ext			 = result->GetFileExtension( cfg.codec );

		const auto	BuildName = [&video_folder, ext] (OUT Path &fname, usize index)
		{{
			fname = video_folder / ("video_"s << ToString(index) << '.' << ext);
		}};

		const auto	WriteToFile = [result, &cfg] (const Path &fname) -> bool
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
	bool  DebugView::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		{
			auto&	ui = UIInteraction::Instance();
			ui.SetDbgView( _index, _view );
		}

		// TODO: optimize - skip if dbg view disabled
		switch_enum( _flags )
		{
			case EFlags::Copy :
				return _CopyImage( pd );

			case EFlags::NoCopy :
				return true;

			case EFlags::Histogram :
			{
				CHECK_ERR( _src != _copy );
				CHECK_ERR( _pass );
				return _pass->Execute( *_src, *_copy, pd );
			}

			case EFlags::LinearDepth :
			{
				CHECK_ERR( _src != _copy );
				CHECK_ERR( _pass );
				return _pass->Execute( *_src, *_copy, pd );
			}

			case EFlags::Stencil :
			{
				CHECK_ERR( _src != _copy );
				CHECK_ERR( _pass );
				return _pass->Execute( *_src, *_copy, pd );
			}

			case EFlags::_Count :
			default :
				RETURN_ERR( "unsupported dbg view mode" );
		}
		switch_end
	}

/*
=================================================
	_CopyImage
=================================================
*/
	bool  DebugView::_CopyImage (SyncPassData &pd) const
	{
		CHECK_ERR( _src != _copy );
		const auto	src_desc = _src->GetImageDesc();
		const auto	dst_desc = _copy->GetImageDesc();

		DirectCtx::Transfer  ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName, HtmlColor::Blue} };

		ImageBlit	blit;
		blit.srcSubres	= { EImageAspect::Color, 0_mipmap, 0_layer, 1u };
		blit.srcOffset0	= { 0u, 0u, 0u };
		blit.srcOffset1	= { src_desc.dimension.x, src_desc.dimension.y, 1u };
		blit.dstSubres	= { EImageAspect::Color, 0_mipmap, 0_layer, 1u };
		blit.dstOffset0	= { 0u, 0u, 0u };
		blit.dstOffset1	= { dst_desc.dimension.x, dst_desc.dimension.y, 1u };

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
		IPass{dbgName}, _index{idx}, _flags{flags}
	{
		CHECK_THROW( src )
		CHECK_THROW( renderer != null );

		ImageViewDesc	view_desc;
		view_desc.baseLayer		= layer;
		view_desc.baseMipmap	= mipmap;

		String	dbg_name = "DbgView: "s << src->GetName();

		_src = src->CreateView( view_desc, dbg_name );
		CHECK_THROW( _src );

		ImageDesc	img_desc	= _src->GetImageDesc();
					view_desc	= Default;
		bool		make_copy	= true;

		switch_enum( flags )
		{
			case EFlags::Copy :
				img_desc.usage		= EImageUsage::Transfer | EImageUsage::Sampled;
				img_desc.options	|= EImageOpt::BlitDst;
				break;

			case EFlags::Histogram :
				img_desc.dimension	= ImageDim_t{ 1024, 1024, 1 };
				img_desc.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled;
				img_desc.format		= EPixelFormat::RGBA8_UNorm;	// defined in 'histogram.as'
				img_desc.options	= Default;
				break;

			case EFlags::LinearDepth :
				img_desc.format		= EPixelFormat::R32F;
				img_desc.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled;
				img_desc.options	= Default;
				view_desc.swizzle	= "RRR1"_swizzle;
				break;

			case EFlags::NoCopy :
				make_copy = false;
				break;

			case EFlags::Stencil :
				img_desc.format		= EPixelFormat::RGBA8_UNorm;
				img_desc.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled;
				img_desc.options	= Default;
				break;

			case EFlags::_Count :
				break;
		}
		switch_end

		if ( make_copy )
		{
			dbg_name << "-Copy";

			auto&	res_mngr	= GraphicsScheduler().GetResourceManager();
			auto	image_id	= res_mngr.CreateImage( img_desc, dbg_name, renderer->ChooseAllocator( False{"static"}, img_desc ));
			CHECK_THROW( image_id );

			auto	view_id		= res_mngr.CreateImageView( ImageViewDesc{img_desc}, image_id, dbg_name );
			CHECK_THROW( view_id );

			renderer->GetDataTransferQueue().EnqueueImageTransition( image_id );

			_copy = MakeRCTh<Image>( RVRef(image_id), RVRef(view_id), Default, *renderer, false,
									 img_desc, ImageViewDesc{img_desc}, null, null, dbg_name );
		}else{
			_copy = _src;
		}

		if ( view_desc != Default ){
			_view = _copy->CreateView( view_desc, dbg_name );
			CHECK_THROW( _view );
		}else{
			_view = _copy;
		}
		CHECK_THROW( _copy and _view );

		switch_enum( _flags )
		{
			case EFlags::Copy :
				CHECK_THROW( _src != _copy );
				break;

			case EFlags::NoCopy :
				CHECK_THROW( _src == _copy );
				break;

			case EFlags::Histogram :
				_pass.reset( new Histogram{ renderer, *_src, *_copy });	// throw
				break;

			case EFlags::LinearDepth :
				_pass.reset( new LinearDepth{ *_src, *_copy });			// throw
				break;

			case EFlags::Stencil :
				_pass.reset( new StencilView{ *_src, *_copy });			// throw
				break;

			case EFlags::_Count :
			default :
				CHECK_THROW_MSG( false, "unsupported dbg view mode" );
		}
		switch_end
	}

/*
=================================================
	destructor
=================================================
*/
	DebugView::~DebugView ()
	{
		auto&	ui = UIInteraction::Instance();
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
		constexpr auto&	RTech = RenderTechs::Histogram_RTech;

		CHECK_THROW( &src != &copy );
		CHECK_THROW( copy.GetImageDesc().format == EPixelFormat::RGBA8_UNorm );

		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();

		_rtech = res_mngr.LoadRenderTech( Default, RTech, Default );
		CHECK_THROW( _rtech );

		_ppln1 = _rtech->GetComputePipeline( RTech.Compute.Histogram_CSPass1 );
		_ppln2 = _rtech->GetComputePipeline( RTech.Compute.Histogram_CSPass2 );
		_ppln3 = _rtech->GetGraphicsPipeline( RTech.Graphics.Histogram_draw );
		CHECK_THROW( _ppln1 and _ppln2 and _ppln3 );

		_ppln1LS = uint2{res_mngr.GetResource( _ppln1 )->LocalSize()};
		_ppln2LS = res_mngr.GetResource( _ppln2 )->LocalSize().x;

		_ssb = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ShaderTypes::Histogram_ssb>, EBufferUsage::Transfer | EBufferUsage::Storage},
									  "Histogram SSB", renderer->GetStaticAllocator() );
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
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		res_mngr.ReleaseResourceArray( _ppln1DS );
		res_mngr.ReleaseResourceArray( _ppln3DS );
		res_mngr.ReleaseResource( _ssb );
	}

/*
=================================================
	Histogram::Execute
=================================================
*/
	bool  DebugView::Histogram::Execute (const Image &srcImage, const Image &dstImage, SyncPassData &pd) const
	{
		constexpr auto&		RTech		= RenderTechs::Histogram_RTech;

		const uint2			src_dim		= srcImage.GetImageDesc().Dimension2();		// TODO: GetViewDimension ?
		const uint2			dst_dim		= dstImage.GetImageDesc().Dimension2();

		DirectCtx::Transfer	copy_ctx	{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{"Histogram pass1", HtmlColor::Blue} };
		DescriptorSetID		comp_ds		= _ppln1DS[ copy_ctx.GetFrameId().Index() ];
		DescriptorSetID		gfx_ds		= _ppln3DS[ copy_ctx.GetFrameId().Index() ];
		const uint			hg_size		= uint(CountOf( &ShaderTypes::Histogram_ssb::RGBLuma ));

		// update
		{
			DescriptorUpdater	updater;

			CHECK_ERR( updater.Set( comp_ds, EDescUpdateMode::Partialy ));
			CHECK_ERR( updater.BindBuffer< ShaderTypes::Histogram_ssb >( UniformName{"un_Histogram"}, _ssb ));
			CHECK_ERR( updater.BindImage( UniformName{"un_Texture"}, srcImage.GetViewId() ));
			CHECK_ERR( updater.Flush() );

			CHECK_ERR( updater.Set( gfx_ds, EDescUpdateMode::Partialy ));
			CHECK_ERR( updater.BindBuffer< ShaderTypes::Histogram_ssb >( UniformName{"un_Histogram"}, _ssb ));
			CHECK_ERR( updater.Flush() );
		}

		// clear
		{
			copy_ctx.FillBuffer( _ssb, 0_b, SizeOf<ShaderTypes::Histogram_ssb>, 0 );
		}

		DirectCtx::Compute	comp_ctx	{ pd.rtask, copy_ctx.ReleaseCommandBuffer(), DebugLabel{"Histogram pass2", HtmlColor::Blue} };

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


		DirectCtx::Graphics	gfx_ctx		{ pd.rtask, comp_ctx.ReleaseCommandBuffer(), DebugLabel{"Histogram pass3", HtmlColor::Blue} };
		const uint			num_verts	= uint(CountOf( &ShaderTypes::Histogram_ssb::RGBLuma )) * 2 * 4;

		// pass 3
		{
			gfx_ctx.MemoryBarrier( EResourceState::CopyDst, EResourceState::ShaderStorage_Read | EResourceState::VertexProcessingShaders );
			gfx_ctx.MemoryBarrier( EResourceState::ShaderStorage_RW | EResourceState::ComputeShader, EResourceState::ShaderStorage_Read | EResourceState::VertexProcessingShaders );

			StaticAssert( RTech.Graphics.attachmentsCount == 1 );

			auto	dctx = gfx_ctx.BeginRenderPass( RenderPassDesc{ *_rtech, RTech.Graphics, dst_dim }
									.AddTarget( RTech.Graphics.att_Color, dstImage.GetViewId(), RGBA32f{0.f} )
									.DefaultViewport() );

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
	LinearDepth ctor
=================================================
*/
	DebugView::LinearDepth::LinearDepth (const Image &src, const Image &copy) __Th___
	{
		constexpr auto&	RTech = RenderTechs::LinearDepth_RTech;

		CHECK_THROW( &src != &copy );
		CHECK_THROW( copy.GetImageDesc().format == EPixelFormat::R32F );

		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();

		_rtech = res_mngr.LoadRenderTech( Default, RTech, Default );
		CHECK_THROW( _rtech );

		_ppln = _rtech->GetGraphicsPipeline( RTech.Graphics.LinearDepth_draw );
		CHECK_THROW( _ppln );

		CHECK_THROW( res_mngr.CreateDescriptorSets( OUT _pplnDSIdx, OUT _pplnDS.data(), max_frames, _ppln, DescriptorSetName{"ds0"} ));

		_pcIdx = res_mngr.GetPushConstantIndex< ShaderTypes::LinearDepth_draw_pc >( _ppln, PushConstantName{"pc"} );
		CHECK_THROW( _pcIdx );
	}

/*
=================================================
	LinearDepth dtor
=================================================
*/
	DebugView::LinearDepth::~LinearDepth ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		res_mngr.ReleaseResourceArray( _pplnDS );
	}

/*
=================================================
	LinearDepth::Execute
=================================================
*/
	bool  DebugView::LinearDepth::Execute (const Image &srcImage, const Image &dstImage, SyncPassData &pd) const
	{
		constexpr auto&	RTech = RenderTechs::LinearDepth_RTech;

		const uint2			dst_dim		= dstImage.GetImageDesc().Dimension2();		// TODO: GetViewDimension ?
		DirectCtx::Graphics	ctx			{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{"ToLinearDepth", HtmlColor::Blue} };
		DescriptorSetID		ds			= _pplnDS[ ctx.GetFrameId().Index() ];

		// update
		{
			DescriptorUpdater	updater;
			CHECK_ERR( updater.Set( ds, EDescUpdateMode::Partialy ));
			CHECK_ERR( updater.BindImage( UniformName{"un_Depth"}, srcImage.GetViewId() ));
			CHECK_ERR( updater.Flush() );
		}

		// convert depth to linear space
		{
			ctx.ResourceState( srcImage.GetViewId(), EResourceState::ShaderSample | EResourceState::FragmentShader );

			StaticAssert( RTech.Graphics.attachmentsCount == 1 );

			auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ *_rtech, RTech.Graphics, dst_dim }
								.AddTarget( RTech.Graphics.att_Color, dstImage.GetViewId() )
								.DefaultViewport() );

			dctx.BindPipeline( _ppln );
			dctx.BindDescriptorSet( _pplnDSIdx, ds );
			dctx.PushConstant( _pcIdx, ShaderTypes::LinearDepth_draw_pc{float2{ 0.1f, 1.1f }} );
			dctx.Draw( 3 );

			ctx.EndRenderPass( dctx );
		}

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	StencilView ctor
=================================================
*/
	DebugView::StencilView::StencilView (const Image &src, const Image &copy) __Th___
	{
		constexpr auto&	RTech = RenderTechs::StencilView_RTech;

		CHECK_THROW( &src != &copy );
		CHECK_THROW( copy.GetImageDesc().format == EPixelFormat::RGBA8_UNorm );

		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();

		_rtech = res_mngr.LoadRenderTech( Default, RTech, Default );
		CHECK_THROW( _rtech );

		_ppln = _rtech->GetGraphicsPipeline( RTech.Graphics.StencilView_draw );
		CHECK_THROW( _ppln );

		CHECK_THROW( res_mngr.CreateDescriptorSets( OUT _pplnDSIdx, OUT _pplnDS.data(), max_frames, _ppln, DescriptorSetName{"ds0"} ));
	}

/*
=================================================
	StencilView dtor
=================================================
*/
	DebugView::StencilView::~StencilView ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		res_mngr.ReleaseResourceArray( _pplnDS );
	}

/*
=================================================
	StencilView::Execute
=================================================
*/
	bool  DebugView::StencilView::Execute (const Image &srcImage, const Image &dstImage, SyncPassData &pd) const
	{
		constexpr auto&	RTech = RenderTechs::StencilView_RTech;

		const uint2			dst_dim		= dstImage.GetImageDesc().Dimension2();		// TODO: GetViewDimension ?
		DirectCtx::Graphics	ctx			{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{"StencilView", HtmlColor::Blue} };
		DescriptorSetID		ds			= _pplnDS[ ctx.GetFrameId().Index() ];

		// update
		{
			DescriptorUpdater	updater;
			CHECK_ERR( updater.Set( ds, EDescUpdateMode::Partialy ));
			CHECK_ERR( updater.BindImage( UniformName{"un_Stencil"}, srcImage.GetViewId() ));
			CHECK_ERR( updater.Flush() );
		}

		// convert depth to linear space
		{
			ctx.ResourceState( srcImage.GetViewId(), EResourceState::ShaderSample | EResourceState::FragmentShader );

			StaticAssert( RTech.Graphics.attachmentsCount == 2 );

			auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ *_rtech, RTech.Graphics, dst_dim }
								.AddTarget( RTech.Graphics.att_Color, dstImage.GetViewId() )
								.AddTarget( RTech.Graphics.att_Stencil, srcImage.GetViewId() )
								.DefaultViewport() );

			dctx.BindPipeline( _ppln );
			dctx.BindDescriptorSet( _pplnDSIdx, ds );
			dctx.Draw( 3 );

			ctx.EndRenderPass( dctx );
		}

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Execute
=================================================
*/
	bool  GenerateMipmapsPass::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		DirectCtx::Transfer		ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{"GenerateMipmaps", HtmlColor::Blue} };

		ctx.GenerateMipmaps( _image->GetImageId() );

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	CopyImagePass::CopyImagePass (RC<Image> src, RC<Image> dst, StringView dbgName) __Th___ :
		IPass{dbgName}, _srcImage{RVRef(src)}, _dstImage{RVRef(dst)}
	{
		const auto&	src_desc = _srcImage->GetViewDesc();
		const auto&	dst_desc = _dstImage->GetViewDesc();

		CHECK_THROW( All( src_desc.dimension == dst_desc.dimension ));
		CHECK_THROW( All( src_desc.format == dst_desc.format ));
		CHECK_THROW( src_desc.layerCount == dst_desc.layerCount );

		_aspect = EPixelFormat_GetInfo( src_desc.format ).aspectMask;
	}

/*
=================================================
	Execute
=================================================
*/
	bool  CopyImagePass::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		DirectCtx::Transfer		ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{"CopyImage", HtmlColor::Blue} };

		const auto&	src_desc = _srcImage->GetViewDesc();
		const auto&	dst_desc = _dstImage->GetViewDesc();
		CHECK_THROW( All( src_desc.dimension == dst_desc.dimension ));
		CHECK_THROW( src_desc.layerCount == dst_desc.layerCount );

		ImageCopy	copy;
		copy.srcOffset	= {};
		copy.srcSubres	= { _aspect, src_desc.baseMipmap, src_desc.baseLayer, src_desc.layerCount };
		copy.dstOffset	= {};
		copy.dstSubres	= { _aspect, dst_desc.baseMipmap, dst_desc.baseLayer, dst_desc.layerCount };
		copy.extent		= src_desc.Dimension();

		ctx.CopyImage( _srcImage->GetImageId(), _dstImage->GetImageId(), {copy} );

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	BlitImagePass::BlitImagePass (RC<Image> src, RC<Image> dst, StringView dbgName) __Th___ :
		IPass{dbgName}, _srcImage{RVRef(src)}, _dstImage{RVRef(dst)}
	{
		const auto&	src_desc = _srcImage->GetViewDesc();
		const auto&	dst_desc = _dstImage->GetViewDesc();

		CHECK_THROW( EPixelFormat_IsBlitSupported( src_desc.format, dst_desc.format, EBlitFilter::Linear ));

		_aspect = EPixelFormat_GetInfo( src_desc.format ).aspectMask;
	}

/*
=================================================
	Execute
=================================================
*/
	bool  BlitImagePass::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		DirectCtx::Transfer		ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{"BlitImage", HtmlColor::Blue} };

		const auto&	src_desc = _srcImage->GetViewDesc();
		const auto&	dst_desc = _dstImage->GetViewDesc();
		CHECK_THROW( src_desc.layerCount == dst_desc.layerCount );

		ImageBlit	blit;
		blit.srcSubres	= { _aspect, src_desc.baseMipmap, src_desc.baseLayer, src_desc.layerCount };
		blit.srcOffset0	= {};
		blit.srcOffset1	= src_desc.Dimension();
		blit.dstSubres	= { _aspect, dst_desc.baseMipmap, dst_desc.baseLayer, dst_desc.layerCount };
		blit.dstOffset0	= {};
		blit.dstOffset1	= dst_desc.Dimension();

		EBlitFilter	filter = EBlitFilter::Linear;
		if ( All( src_desc.Dimension() == dst_desc.Dimension() ))
			filter = EBlitFilter::Nearest;

		ctx.BlitImage( _srcImage->GetImageId(), _dstImage->GetImageId(), filter, {blit} );

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ResolveImagePass::ResolveImagePass (RC<Image> src, RC<Image> dst, StringView dbgName) __Th___ :
		IPass{dbgName}, _srcImage{RVRef(src)}, _dstImage{RVRef(dst)}
	{
		const auto&	src_desc = _srcImage->GetViewDesc();
		const auto&	dst_desc = _dstImage->GetViewDesc();

		CHECK_THROW( All( src_desc.dimension == dst_desc.dimension ));
		CHECK_THROW( All( src_desc.format == dst_desc.format ));
		CHECK_THROW( src_desc.layerCount == dst_desc.layerCount );

		_aspect = EPixelFormat_GetInfo( src_desc.format ).aspectMask;
	}

/*
=================================================
	Execute
=================================================
*/
	bool  ResolveImagePass::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		DirectCtx::Transfer		ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{"CopyImage", HtmlColor::Blue} };

		const auto&	src_desc = _srcImage->GetViewDesc();
		const auto&	dst_desc = _dstImage->GetViewDesc();
		CHECK_THROW( All( src_desc.dimension == dst_desc.dimension ));
		CHECK_THROW( src_desc.layerCount == dst_desc.layerCount );

		ImageResolve	copy;
		copy.srcOffset	= {};
		copy.srcSubres	= { _aspect, src_desc.baseMipmap, src_desc.baseLayer, src_desc.layerCount };
		copy.dstOffset	= {};
		copy.dstSubres	= { _aspect, dst_desc.baseMipmap, dst_desc.baseLayer, dst_desc.layerCount };
		copy.extent		= src_desc.Dimension();

		ctx.ResolveImage( _srcImage->GetImageId(), _dstImage->GetImageId(), {copy} );

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Execute
=================================================
*/
	bool  ClearImagePass::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		DirectCtx::Transfer		ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{"ClearImage", HtmlColor::Blue} };
		ImageSubresourceRange	range{ EImageAspect::Color, 0_mipmap, UMax, 0_layer, UMax };
		ImageID					id = _image->GetImageId();

		Visit( _value,
			[&] (const RGBA32f &col) { ctx.ClearColorImage( id, col, {range} ); },
			[&] (const RGBA32u &col) { ctx.ClearColorImage( id, col, {range} ); },
			[&] (const RGBA32i &col) { ctx.ClearColorImage( id, col, {range} ); }
		);

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Execute
=================================================
*/
	bool  ClearBufferPass::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		DirectCtx::Transfer		ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{"ClearBuffer", HtmlColor::Blue} };

		ctx.FillBuffer( _buffer->GetBufferId( ctx.GetFrameId() ), _offset, _size, _value );

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}
//-----------------------------------------------------------------------------


} // AE::ResEditor
