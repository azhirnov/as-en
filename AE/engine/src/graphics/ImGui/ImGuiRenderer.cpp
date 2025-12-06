// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "graphics/ImGui/ImGuiRenderer.h"
# include "graphics_rhi/Private/EnumToString.h"

# include "imgui.h"
# include "imgui_internal.h"

# if IMGUI_VERSION_NUM != 19210
#	pragma message( "required ImGui version 1.92.1" )
# endif

namespace AE::Graphics
{
	using namespace AE::App;

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

		auto&	rts		 = GraphicsScheduler();
		auto&	res_mngr = rts.GetResourceManager();

		for (auto& [fmt, rp, p] : pplnInfo)
		{
			auto	ppln = _rtech->GetGraphicsPipeline( p );
			CHECK_ERR( ppln );

			_pplnMap.insert_or_assign( fmt, PipelineSet{ RenderTechPassName::Optimized_t{rp}, ppln });
		}

		auto	default_ppln = _pplnMap.begin()->second.ppln;

		_texUniform = unTexture;

		// create DS
		{
			_descSets.resize( rts.GetMaxFrames() );
			CHECK_ERR( res_mngr.CreateDescriptorSets( OUT _dsIndex, OUT _descSets.data(), _descSets.size(), default_ppln, dsName ));

			_vsPCIndex = res_mngr.GetPushConstantIndex<imgui_vs_pc>( default_ppln, PushConstantName{"imguiVSpc"} );
			_fsPCIndex = res_mngr.GetPushConstantIndex<imgui_fs_pc>( default_ppln, PushConstantName{"imguiFSpc"} );
			CHECK_ERR( _vsPCIndex and _fsPCIndex );
		}{
			StaticArray< ImageViewID, TextureCount >	textures;
			textures.fill( _font.view );

			DescriptorUpdater	updater;
			for (auto& ds : _descSets)
			{
				CHECK_ERR( updater.Set( ds, EDescUpdateMode::Partialy ));
				CHECK_ERR( updater.BindImages( unTexture, textures ));
				CHECK_ERR( updater.Flush() );
			}
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

			_imguiCtx->IO.Fonts->SetTexID( BitCast<ImTextureID>( 0ull ));
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

		res_mngr.DelayedReleaseResources( _font.image, _font.view );
		res_mngr.ReleaseResourceArray( _descSets );

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
		_scale = scale;
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
			// TODO
			_pixToUI = ( pixToMm * _scale );

		//	auto	bits = BitCast<Float32Bits>( _pixToUI );
		//	bits.m = 0;
		//	_pixToUI = BitCast<float>( bits );		// round to power of 2

			_uiToPix = 1.f / _pixToUI;
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

		if_unlikely( NeedUpload() )
		{
			_fontInitialized = true;
			return _Upload( ctx );
		}
		return true;
	}

/*
=================================================
	BindTextures
=================================================
*/
	bool  ImGuiRenderer::BindTextures (FrameUID currentFrameId, ArrayView<ImageViewID> ids) __NE___
	{
		CHECK( ids.size() < TextureCount-1 );

		StaticArray< ImageViewID, TextureCount >	textures;
		textures.fill( _font.view );

		for (usize i = 0, cnt = Min( ids.size(), textures.size()-1 ); i < cnt; ++i)
		{
			if ( ids[i] == Default )
				continue;

			textures[i+1] = ids[i];
		}

		DescriptorUpdater	updater;

		CHECK_ERR( updater.Set( _descSets[ currentFrameId.Index() ], EDescUpdateMode::Partialy ));
		CHECK_ERR( updater.BindImages( UniformName{_texUniform}, textures ));
		CHECK_ERR( updater.Flush() );

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
			_DrawUI( ctx, viewport->DrawDataP, ps.ppln, rt.transform );

		return true;
	}

/*
=================================================
	Render2
=================================================
*/
	bool  ImGuiRenderer::Render2 (DirectCtx::Graphics					&gfxCtx,
								  App::IOutputSurface					&surface,
								  const Function<void()>				&updateUI,
								  const RenderPassDesc::ClearValue_t	&clearValue) __Th___
	{
		IOutputSurface::RenderTargets_t		targets;
		CHECK_ERR( surface.GetTargets( OUT targets ));
		CHECK_Eq( targets.size(), 1 );

		auto&	rt = targets[0];
		rt.initialState |= EResourceState::Invalidate;

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

		auto	dctx = gfxCtx.BeginRenderPass( RenderPassDesc{ *_rtech, RenderTechPassName{ps.pass}, rt.RegionSize() }
													.AddViewport( rt.RegionSize() )
													.AddTarget( AttachmentName{"Color"}, rt.viewId, clearValue, rt.initialState, rt.finalState ),
												DebugLabel{"ImGui", HtmlColor::Yellow} );

		// same as ImGui::GetDrawData()
		auto*	viewport = _imguiCtx->Viewports[0];

		if_likely( viewport->DrawDataP.Valid )
			_DrawUI( dctx, viewport->DrawDataP, ps.ppln, rt.transform );

		gfxCtx.EndRenderPass( dctx );
		return true;
	}

/*
=================================================
	Draw
=================================================
*/
	bool  ImGuiRenderer::Draw (RenderTaskRef								rtask,
							   App::IOutputSurface							&surface,
							   const Function< void () >					&updateUI,
							   const Function< void (DirectCtx::Draw &) >	&drawBefore,
							   const RenderPassDesc::ClearValue_t			&clearValue) __Th___
	{
		IOutputSurface::RenderTargets_t		targets;
		CHECK_ERR( surface.GetTargets( OUT targets ));
		CHECK_Eq( targets.size(), 1 );

		auto&	rt = targets[0];
		rt.initialState |= EResourceState::Invalidate;

		return Draw( rtask, DirectCtx::CommandBuffer{}, rt, updateUI, drawBefore, clearValue );
	}

	bool  ImGuiRenderer::Draw (RenderTaskRef								rtask,
							   DirectCtx::CommandBuffer						cmdbuf,
							   const IOutputSurface::RenderTarget			&rt,
							   const Function< void () >					&updateUI,
							   const Function< void (DirectCtx::Draw &) >	&drawBefore,
							   const RenderPassDesc::ClearValue_t			&clearValue) __Th___
	{
		CHECK_ERR( IsInitialized() );
		CHECK_ERR( _Update( rt, updateUI ));

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

		auto	dctx = gfx_ctx.BeginRenderPass( RenderPassDesc{ *_rtech, RenderTechPassName{ps.pass}, rt.RegionSize() }
													.AddViewport( rt.RegionSize() )
													.AddTarget( AttachmentName{"Color"}, rt.viewId, clearValue, rt.initialState, rt.finalState ),
												DebugLabel{"ImGui", HtmlColor::Yellow} );
		if ( drawBefore )
			drawBefore( dctx );

		// same as ImGui::GetDrawData()
		auto*	viewport = _imguiCtx->Viewports[0];

		if_likely( viewport->DrawDataP.Valid )
			_DrawUI( dctx, viewport->DrawDataP, ps.ppln, rt.transform );

		gfx_ctx.EndRenderPass( dctx );

		return ConstCast( rtask.Raw() )->Execute( gfx_ctx );
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
		const float2	size	= SurfaceTransformUtils::Transform( rt.transform, rt.RegionSizePxf() * _pixToUI );
		const float2	pos		= mousePos * _pixToUI;

		io.DisplaySize	= ImVec2{ size.x, size.y };
		io.DeltaTime	= dt;

		// update input
		{
			ZeroMem( OUT io.MouseDown );

			io.MouseDown[0]	= mouseLBDown or touchActive;
			io.MousePos		= ImVec2{ pos.x, pos.y };
			io.MouseWheel	= mouseWheel.y;
			io.MouseWheelH	= mouseWheel.x;
		}

		if ( not inputText.empty() )
		{
			io.AddInputCharactersUTF8( Cast<char>(inputText.c_str()) );
			inputText.clear();
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
	bool  ImGuiRenderer::_DrawUI (DirectCtx::Draw &dctx, const ImDrawData &drawData, GraphicsPipelineID ppln, const ESurfaceTransform orient)
	{
		const bool	is_minimized = (drawData.DisplaySize.x <= 0.0f or drawData.DisplaySize.y <= 0.0f);

		if_unlikely( drawData.TotalVtxCount <= 0 or is_minimized )
			return false;

		dctx.BindPipeline( ppln );
		dctx.BindDescriptorSet( _dsIndex, _descSets[ dctx.GetFrameId().Index() ]);

		{
			float2		scale	{ drawData.DisplaySize.x + _imguiCtx->IO.DisplayFramebufferScale.x,
								  drawData.DisplaySize.y + _imguiCtx->IO.DisplayFramebufferScale.y };
			auto	s = float3x3{ float2x2::Scale( 2.0f / scale )};
			auto	t = float3x3{ float3x2::Translate( -1.0f - float2{drawData.DisplayPos.x * scale.x, drawData.DisplayPos.y * scale.y})};
			auto	r = float3x3{ SurfaceTransformUtils::ToInvMatrix( orient )};
			auto	m = float3x2{r * (t * s)};

			imgui_vs_pc		ub_data;
			ub_data.transform_c0	= m.get<0>();
			ub_data.transform_c1	= m.get<1>();
			ub_data.transform_c2	= m.get<2>();

			dctx.PushConstant( _vsPCIndex, ub_data );
		}

		CHECK_ERR( _UploadVB( dctx, drawData ));

		uint	idx_offset	= 0;
		uint	vtx_offset	= 0;
		uint	cur_tex		= UMax;
		auto	scr_size	= int2{float2{	drawData.DisplaySize.x * _uiToPix + 0.5f,
											drawData.DisplaySize.y * _uiToPix + 0.5f }};

		for (int i = 0; i < drawData.CmdListsCount; ++i)
		{
			ImDrawList const&	cmd_list = *drawData.CmdLists[i];

			for (int j = 0; j < cmd_list.CmdBuffer.Size; ++j)
			{
				ImDrawCmd const&	cmd = cmd_list.CmdBuffer[j];
				const uint			tex	= uint(BitCast<ulong>(cmd.TexRef.GetTexID()));

				if ( tex != cur_tex )
				{
					cur_tex = tex;
					dctx.PushConstant( _fsPCIndex, imgui_fs_pc{tex} );
				}

				if_likely( cmd.UserCallback == null )
				{
					RectI	scissor;
					scissor.left	= int(cmd.ClipRect.x * _uiToPix + 0.5f);
					scissor.top		= int(cmd.ClipRect.y * _uiToPix + 0.5f);
					scissor.right	= int(cmd.ClipRect.z * _uiToPix + 0.5f);
					scissor.bottom	= int(cmd.ClipRect.w * _uiToPix + 0.5f);

					scissor = SurfaceTransformUtils::InvTransform( orient, scissor, scr_size );

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
		upload.imageDim		= ImageDim_t{int3{ width, height, 1 }};
		upload.dataRowPitch	= Bytes{width * 4 * sizeof(ubyte)};

		const Bytes	size	{width * height * 4 * sizeof(ubyte)};
		const bool	result	= (copyCtx.UploadImage( _font.image, upload, ArrayView<ubyte>{ pixels, usize(size) }) == size);

		copyCtx.AccumBarriers()
			.ImageBarrier( _font.image, EResourceState::CopyDst, EResourceState::ShaderSample | EResourceState::FragmentShader );

		_imguiCtx->IO.Fonts->ClearTexData();
		return result;
	}

/*
=================================================
	constructor
=================================================
*/
	ImGuiRenderer::ImGuiRenderer (ImGuiContext* ctx) __NE___ :
		_imguiCtx{ctx}
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
namespace {
	static void  PushColor_sRGB (ImGuiCol idx, RGBA8u color) __NE___
	{
		ImU32	c = Base::BitCast<ImU32>(color);
		ImGui::PushStyleColor( idx, c );
	}

	static void  PushColor_RemoveSRGB (ImGuiCol idx, RGBA8u color) __NE___
	{
		ImU32	c = Base::BitCast<ImU32>( RGBA8u{ RemoveSRGBCurve( RGBA32f{color} )});
		ImGui::PushStyleColor( idx, c );
	}
}
/*
=================================================
	AEStyleScope ctor
=================================================
*/
	ImGuiRenderer::AEStyleScope::AEStyleScope (ImGuiContext* ctx, Bool sRGB) __NE___ :
		StyleScope{ ctx }
	{
		auto*	PushStyleColor = sRGB ? &PushColor_sRGB : &PushColor_RemoveSRGB;

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

/*
=================================================
	AEStyleScope_StartBtn ctor
=================================================
*/
	ImGuiRenderer::AEStyleScope_StartBtn::AEStyleScope_StartBtn (ImGuiContext* ctx, Bool sRGB) __NE___ :
		StyleScope{ ctx }
	{
		auto*	PushStyleColor = sRGB ? &PushColor_sRGB : &PushColor_RemoveSRGB;

		PushStyleColor( ImGuiCol_Button,		RGBA8u{ 80, 20, 170, 255} );
		PushStyleColor( ImGuiCol_ButtonHovered,	RGBA8u{ 95, 20, 210, 255} );
		PushStyleColor( ImGuiCol_ButtonActive,	RGBA8u{110, 20, 250, 255} );
	}

/*
=================================================
	AEStyleScope_StopBtn ctor
=================================================
*/
	ImGuiRenderer::AEStyleScope_StopBtn::AEStyleScope_StopBtn (ImGuiContext* ctx, Bool sRGB) __NE___ :
		StyleScope{ ctx }
	{
		auto*	PushStyleColor = sRGB ? &PushColor_sRGB : &PushColor_RemoveSRGB;

		PushStyleColor( ImGuiCol_Button,		RGBA8u{140, 20, 150, 255} );
		PushStyleColor( ImGuiCol_ButtonHovered,	RGBA8u{160, 20, 180, 255} );
		PushStyleColor( ImGuiCol_ButtonActive,	RGBA8u{200, 20, 220, 255} );
	}

} // AE::Graphics

#endif // AE_ENABLE_IMGUI
