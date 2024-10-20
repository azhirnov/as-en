// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "graphics_hl/ImGui/ImGuiRenderer.h"
# include "graphics/Private/EnumToString.h"

# include "imgui.h"
# include "imgui_internal.h"

# if IMGUI_VERSION_NUM != 19100
#	error required ImGui version 1.91.0
# endif

namespace AE::Graphics
{
	using namespace AE::App;

	INTERNAL_LINKAGE( const float	DefaultScale = 0.25f );

/*
=================================================
	Initialize
=================================================
*/
	bool  ImGuiRenderer::Initialize (GfxMemAllocatorPtr			gfxAlloc,
									 RenderTechPipelinesPtr		rtech,
									 PipelineInfo_t				pplnInfo,
									 const DescriptorSetName	&dsName,
									 UniformName::Ref			unTexture) __NE___
	{
		CHECK_ERR( not pplnInfo.empty() );
		CHECK_ERR( _Initialize( RVRef(gfxAlloc), RVRef(rtech) ));

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		for (auto& [fmt, rp, p] : pplnInfo)
		{
			auto	ppln = _rtech->GetGraphicsPipeline( p );
			CHECK_ERR( ppln );

			_pplnMap.insert_or_assign( fmt, PipelineSet{ RenderTechPassName::Optimized_t{rp}, ppln });
		}

		// create DS
		{
			auto [ds, idx] = res_mngr.CreateDescriptorSet( _pplnMap.begin()->second.ppln, dsName );
			CHECK_ERR( ds and idx == _dsIndex );
			_descSet = RVRef(ds);
		}{
			DescriptorUpdater	updater;
			CHECK( updater.Set( _descSet, EDescUpdateMode::Partialy ));
			updater.BindImage( unTexture, _font.view );
			CHECK( updater.Flush() );
		}
		return true;
	}

/*
=================================================
	_Initialize
=================================================
*/
	bool  ImGuiRenderer::_Initialize (GfxMemAllocatorPtr gfxAlloc, RenderTechPipelinesPtr rtech)
	{
		CHECK_ERR( rtech );
		_rtech = RVRef(rtech);

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		// init ImGUI context
		if ( _imguiCtx == null )
		{
			IMGUI_CHECKVERSION();

			_imguiCtx = ImGui::CreateContext();
			CHECK_ERR( _imguiCtx != null );

			ImGui::StyleColorsDark();
		}

		// initialize font atlas
		{
			uint8_t*	pixels;
			int			width, height;
			_imguiCtx->IO.Fonts->GetTexDataAsRGBA32( OUT &pixels, OUT &width, OUT &height );	// TODO: GetTexDataAsAlpha8 ?

			_font.image = res_mngr.CreateImage( ImageDesc{}.SetDimension( width, height )
													.SetFormat( EPixelFormat::RGBA8_UNorm )
													.SetUsage( EImageUsage::Sampled | EImageUsage::TransferDst ),
												"Imgui font image", RVRef(gfxAlloc) );
			CHECK_ERR( _font.image );

			_font.view = res_mngr.CreateImageView( ImageViewDesc{}, _font.image, "Imgui font image view" );
			CHECK_ERR( _font.view );
		}
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  ImGuiRenderer::Deinitialize () __NE___
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		res_mngr.DelayedReleaseResources( _descSet, _font.image, _font.view );

		if ( _imguiCtx != null )
		{
			ImGui::DestroyContext( _imguiCtx );
			_imguiCtx = null;
		}

		_pplnMap.clear();
		_rtech = null;

		_fontInitialized = false;
	}

/*
=================================================
	SetScale
=================================================
*/
	void  ImGuiRenderer::SetScale (float scale) __NE___
	{
		_scale = scale * DefaultScale;
	}

	void  ImGuiRenderer::_UpdateScale (float pixToMm)
	{
		if ( _scale < 0.f )
		{
			_uiToPix = 1.f;
			_pixToUI = 1.f;
		}
		else
		{
			_uiToPix = _scale / pixToMm;
			_pixToUI = pixToMm / _scale;
		}
	}

/*
=================================================
	Upload
=================================================
*/
	bool  ImGuiRenderer::Upload (DirectCtx::Transfer &ctx) __Th___
	{
		CHECK_ERR( IsInitialized() );

		if_unlikely( not _fontInitialized )
		{
			_fontInitialized = true;
			return _Upload( ctx );
		}
		return true;
	}

/*
=================================================
	Render
=================================================
*/
	bool  ImGuiRenderer::Render (DirectCtx::Draw					&ctx,
								 const IOutputSurface::RenderTarget	&rt,
								 const Function<void()>				&updateUI) __Th___
	{
		CHECK_ERR( _fontInitialized );
		CHECK_ERR( _Update( rt, updateUI ));

		PipelineSet	ps;
		{
			auto	it = _pplnMap.find( rt.format );
			if ( it == _pplnMap.end() )
				it = _pplnMap.find( EPixelFormat::SwapchainColor );

			CHECK_ERR_MSG( it != _pplnMap.end(),
				"Failed to find pipeline for surface format "s << ToString(rt.format) );
			ps = it->second;
		}

		// same as ImGui::GetDrawData()
		auto*	viewport = _imguiCtx->Viewports[0];

		if_likely( viewport->DrawDataP.Valid )
			_DrawUI( ctx, viewport->DrawDataP, ps.ppln );

		return true;
	}

/*
=================================================
	Draw
=================================================
*/
	bool  ImGuiRenderer::Draw (RenderTask									&rtask,
							   App::IOutputSurface							&surface,
							   const Function< void () >					&updateUI,
							   const Function< void (DirectCtx::Draw &) >	&drawBefore,
							   const RenderPassDesc::ClearValue_t			&clearValue) __Th___
	{
		IOutputSurface::RenderTargets_t		targets;
		CHECK_ERR( surface.GetTargets( OUT targets ));

		auto&	rt = targets[0];
		rt.initialState |= EResourceState::Invalidate;

		return Draw( rtask, DirectCtx::CommandBuffer{}, rt, updateUI, drawBefore, clearValue );
	}

	bool  ImGuiRenderer::Draw (RenderTask									&rtask,
							   DirectCtx::CommandBuffer						cmdbuf,
							   const IOutputSurface::RenderTarget			&rt,
							   const Function< void () >					&updateUI,
							   const Function< void (DirectCtx::Draw &) >	&drawBefore,
							   const RenderPassDesc::ClearValue_t			&clearValue) __Th___
	{
		CHECK_ERR( _Update( rt, updateUI ));

		// same as ImGui::GetDrawData()
		auto*	viewport = _imguiCtx->Viewports[0];

		if_unlikely( not _fontInitialized )
		{
			DirectCtx::Transfer		copy_ctx{ rtask, RVRef(cmdbuf) };

			_fontInitialized = true;
			Unused( _Upload( copy_ctx ));

			cmdbuf = copy_ctx.ReleaseCommandBuffer();
		}

		PipelineSet	ps;
		{
			auto	it = _pplnMap.find( rt.format );
			if ( it == _pplnMap.end() )
				it = _pplnMap.find( EPixelFormat::SwapchainColor );

			CHECK_ERR_MSG( it != _pplnMap.end(),
				"Failed to find pipeline for surface format "s << ToString(rt.format) );
			ps = it->second;
		}

		DirectCtx::Graphics		gfx_ctx{ rtask, RVRef(cmdbuf) };

		gfx_ctx.AccumBarriers()
			.MemoryBarrier( EResourceState::CopyDst, EResourceState::ShaderUniform | EResourceState::FragmentShader | EResourceState::PreRasterizationShaders );

		auto	dctx = gfx_ctx.BeginRenderPass( RenderPassDesc{ *_rtech, RenderTechPassName{ps.pass}, rt.RegionSize() }
													.AddViewport( rt.RegionSize() )
													.AddTarget( AttachmentName{"Color"}, rt.viewId, clearValue, rt.initialState, rt.finalState ),
												DebugLabel{"ImGui", HtmlColor::Yellow} );
		if ( drawBefore )
			drawBefore( dctx );

		if_likely( viewport->DrawDataP.Valid )
			_DrawUI( dctx, viewport->DrawDataP, ps.ppln );

		gfx_ctx.EndRenderPass( dctx );

		rtask.Execute( gfx_ctx );
		return true;
	}

/*
=================================================
	_Update
=================================================
*/
	bool  ImGuiRenderer::_Update (const IOutputSurface::RenderTarget &rt, const Function<void()> &updateUI)
	{
		_UpdateScale( rt.pixToMm );

		ImGui::SetCurrentContext( _imguiCtx );

		const float		dt		= GraphicsScheduler().GetFrameTimeDelta().count();
		ImGuiIO &		io		= ImGui::GetIO();
		const float2	size	= rt.RegionSizePxf() * _pixToUI;
		const float2	pos		= mousePos * _pixToUI;

		io.DisplaySize	= ImVec2{ size.x, size.y };
		io.DeltaTime	= dt;

		// update input
		{
			ZeroMem( OUT io.MouseDown );
			ZeroMem( OUT io.NavInputs );

			io.MouseDown[0]	= mouseLBDown or touchActive;
			io.MousePos		= ImVec2{ pos.x, pos.y };
			io.MouseWheel	= mouseWheel.y;
			io.MouseWheelH	= mouseWheel.x;
		}

		ImGui::NewFrame();

		if ( updateUI )
			updateUI();

		ImGui::Render();

		return true;
	}

/*
=================================================
	_UploadVB
=================================================
*/
	bool  ImGuiRenderer::_UploadVB (DirectCtx::Draw &dctx, const ImDrawData &drawData)
	{
		// allocate
		Bytes	vtx_size;
		Bytes	idx_size;

		for (int i = 0; i < drawData.CmdListsCount; ++i)
		{
			ImDrawList const&	cmd_list = *drawData.CmdLists[i];

			vtx_size += cmd_list.VtxBuffer.Size * SizeOf<ImDrawVert>;
			idx_size += cmd_list.IdxBuffer.Size * SizeOf<ImDrawIdx>;
		}

		VertexStream	vstream;
		VertexStream	istream;
		CHECK_ERR( dctx.AllocVStream( vtx_size, OUT vstream ));
		CHECK_ERR( dctx.AllocVStream( idx_size, OUT istream ));


		// upload
		Bytes	vtx_offset;
		Bytes	idx_offset;

		for (int i = 0; i < drawData.CmdListsCount; ++i)
		{
			ImDrawList const&	cmd_list = *drawData.CmdLists[i];

			MemCopy( OUT vstream.mappedPtr + vtx_offset, cmd_list.VtxBuffer.Data, cmd_list.VtxBuffer.Size * SizeOf<ImDrawVert> );
			MemCopy( OUT istream.mappedPtr + idx_offset, cmd_list.IdxBuffer.Data, cmd_list.IdxBuffer.Size * SizeOf<ImDrawIdx> );

			vtx_offset += cmd_list.VtxBuffer.Size * SizeOf<ImDrawVert>;
			idx_offset += cmd_list.IdxBuffer.Size * SizeOf<ImDrawIdx>;
		}

		ASSERT( vtx_size == vtx_offset );
		ASSERT( idx_size == idx_offset );


		// bind
		dctx.BindVertexBuffer( 0, vstream.bufferHandle, vstream.offset );
		dctx.BindIndexBuffer( istream.bufferHandle, istream.offset, IndexDesc<ImDrawIdx>::value );

		return true;
	}

/*
=================================================
	_DrawUI
=================================================
*/
	bool  ImGuiRenderer::_DrawUI (DirectCtx::Draw &dctx, const ImDrawData &drawData, GraphicsPipelineID ppln)
	{
		const bool	is_minimized = (drawData.DisplaySize.x <= 0.0f or drawData.DisplaySize.y <= 0.0f);

		if_unlikely( drawData.TotalVtxCount <= 0 or is_minimized )
			return false;

		dctx.BindPipeline( ppln );
		dctx.BindDescriptorSet( _dsIndex, _descSet );

		{
			ImVec2 const&	scale	= _imguiCtx->IO.DisplayFramebufferScale;
			imgui_ub		ub_data;

			ub_data.scale.x		= 2.0f / (drawData.DisplaySize.x * scale.x);
			ub_data.scale.y		= 2.0f / (drawData.DisplaySize.y * scale.y);

			ub_data.translate.x	= -1.0f - (drawData.DisplayPos.x * ub_data.scale.x);
			ub_data.translate.y	= -1.0f - (drawData.DisplayPos.y * ub_data.scale.y);

			dctx.PushConstant( _pcIndex, ub_data );
		}

		CHECK_ERR( _UploadVB( dctx, drawData ));

		uint	idx_offset	= 0;
		uint	vtx_offset	= 0;

		for (int i = 0; i < drawData.CmdListsCount; ++i)
		{
			ImDrawList const&	cmd_list = *drawData.CmdLists[i];

			for (int j = 0; j < cmd_list.CmdBuffer.Size; ++j)
			{
				ImDrawCmd const&	cmd = cmd_list.CmdBuffer[j];

				if_likely( cmd.UserCallback == null )
				{
					RectI	scissor;
					scissor.left	= int(cmd.ClipRect.x * _uiToPix + 0.5f);
					scissor.top		= int(cmd.ClipRect.y * _uiToPix + 0.5f);
					scissor.right	= int(cmd.ClipRect.z * _uiToPix + 0.5f);
					scissor.bottom	= int(cmd.ClipRect.w * _uiToPix + 0.5f);

					dctx.SetScissor( scissor );

					dctx.DrawIndexed( cmd.ElemCount, 1, cmd.IdxOffset + idx_offset, cmd.VtxOffset + vtx_offset, 0 );
				}
				else
				{
					cmd.UserCallback( &cmd_list, &cmd );
				}
			}
			idx_offset += cmd_list.IdxBuffer.Size;
			vtx_offset += cmd_list.VtxBuffer.Size;
		}
		return true;
	}

/*
=================================================
	_Upload
=================================================
*/
	bool  ImGuiRenderer::_Upload (DirectCtx::Transfer &copyCtx)
	{
		ubyte*	pixels;
		int		width, height;
		_imguiCtx->IO.Fonts->GetTexDataAsRGBA32( OUT &pixels, OUT &width, OUT &height );

		copyCtx.AccumBarriers()
			.ImageBarrier( _font.image, EResourceState::Unknown, EResourceState::CopyDst );

		UploadImageDesc		upload;
		upload.aspectMask	= EImageAspect::Color;
		upload.heapType		= EStagingHeapType::Dynamic;
		upload.imageDim		= int3{ width, height, 1 };
		upload.dataRowPitch	= Bytes{width * 4 * sizeof(ubyte)};

		const Bytes	size	{width * height * 4 * sizeof(ubyte)};
		const bool	result	= (copyCtx.UploadImage( _font.image, upload, ArrayView<ubyte>{ pixels, usize(size) }) == size);

		copyCtx.AccumBarriers()
			.ImageBarrier( _font.image, EResourceState::CopyDst, EResourceState::ShaderSample | EResourceState::FragmentShader );

		return result;
	}

/*
=================================================
	constructor
=================================================
*/
	ImGuiRenderer::ImGuiRenderer (ImGuiContext* ctx) __NE___ :
		_imguiCtx{ctx}, _scale{DefaultScale}
	{}

/*
=================================================
	destructor
=================================================
*/
	ImGuiRenderer::~ImGuiRenderer () __NE___
	{
		Deinitialize();
	}

/*
=================================================
	StyleScope ctor / dtor
=================================================
*/
	ImGuiRenderer::StyleScope::StyleScope (ImGuiContext* ctx) __NE___ :
		_imguiCtx{ ctx },
		_stackSize{ _imguiCtx->ColorStack.size() }
	{}

	ImGuiRenderer::StyleScope::~StyleScope () __NE___
	{
		ImGui::PopStyleColor(int( _imguiCtx->ColorStack.size() - _stackSize ));
	}

/*
=================================================
	AEStyleScope ctor
=================================================
*/
	ImGuiRenderer::AEStyleScope::AEStyleScope (ImGuiContext* ctx, Bool sRGB) __NE___ :
		StyleScope{ ctx }
	{
		const auto  PushStyleColor = [sRGB] (ImGuiCol idx, RGBA8u color) __NE___
		{{
			ImU32	c = Base::BitCast<ImU32>(color);
			if_unlikely( not sRGB )
				c = Base::BitCast<ImU32>( RGBA8u{ RemoveSRGBCurve( RGBA32f{color} )});

			ImGui::PushStyleColor( idx, c );
		}};

		// window / frame
		PushStyleColor( ImGuiCol_WindowBg,				RGBA8u{ 20, 0,  60, 255} );
		PushStyleColor( ImGuiCol_ChildBg,				RGBA8u{ 40, 0, 100, 255} );

		// window title
		PushStyleColor( ImGuiCol_TitleBg,				RGBA8u{ 30, 0,  80, 255} );
		PushStyleColor( ImGuiCol_TitleBgActive,			RGBA8u{ 30, 0,  80, 255} );
		PushStyleColor( ImGuiCol_TitleBgCollapsed,		RGBA8u{ 30, 0,  80, 255} );

		// background (checkbox, radio button, plot, slider, text input)
		PushStyleColor( ImGuiCol_FrameBg,				RGBA8u{ 40, 0, 100, 255} );
		PushStyleColor( ImGuiCol_FrameBgHovered,		RGBA8u{ 90, 0, 180, 255} );
		PushStyleColor( ImGuiCol_FrameBgActive,			RGBA8u{120, 0, 220, 255} );

		// tabs
		PushStyleColor( ImGuiCol_Tab,					RGBA8u{ 90, 0, 180, 255} );
		PushStyleColor( ImGuiCol_TabUnfocused,			RGBA8u{ 90, 0, 180, 255} );
		PushStyleColor( ImGuiCol_TabHovered,			RGBA8u{120, 0, 220, 255} );
		PushStyleColor( ImGuiCol_TabActive,				RGBA8u{120, 0, 220, 255} );
		PushStyleColor( ImGuiCol_TabUnfocusedActive,	RGBA8u{120, 0, 220, 255} );

		// tree view
		PushStyleColor( ImGuiCol_Header,				RGBA8u{ 90, 0, 180, 255} );
		PushStyleColor( ImGuiCol_HeaderActive,			RGBA8u{120, 0, 220, 255} );
		PushStyleColor( ImGuiCol_HeaderHovered,			RGBA8u{ 90, 0, 180, 255} );

		// button
		PushStyleColor( ImGuiCol_Button,				RGBA8u{ 90, 0, 180, 255} );
		PushStyleColor( ImGuiCol_ButtonHovered,			RGBA8u{120, 0, 220, 255} );
		PushStyleColor( ImGuiCol_ButtonActive,			RGBA8u{140, 0, 240, 255} );	// pressed

		// scrollbar
		PushStyleColor( ImGuiCol_ScrollbarBg,			RGBA8u{ 30, 0,  80, 255} );
		PushStyleColor( ImGuiCol_ScrollbarGrab,			RGBA8u{ 90, 0, 180, 255} );
		PushStyleColor( ImGuiCol_ScrollbarGrabHovered,	RGBA8u{120, 0, 220, 255} );
		PushStyleColor( ImGuiCol_ScrollbarGrabActive,	RGBA8u{120, 0, 220, 255} );

		// checkbox
		PushStyleColor( ImGuiCol_CheckMark,				RGBA8u{230, 0, 255, 255} );

		// slider
		PushStyleColor( ImGuiCol_SliderGrab,			RGBA8u{120, 0, 220, 255} );
		PushStyleColor( ImGuiCol_SliderGrabActive,		RGBA8u{230, 0, 255, 255} );
	}


} // AE::Graphics

#endif // AE_ENABLE_IMGUI
