// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "video/Private/EnumToString.cpp.h"

#include "res_editor/Core/EditorUI.h"
#include "res_editor/Core/EditorCore.h"

#include "res_editor/Resources/Image.h"

#include "res_editor/_ui_data/cpp/types.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{
/*
=================================================
	PushStyleColor
=================================================
*/
	void  PushStyleColor (ImGuiCol idx, AE::Base::RGBA8u color)
	{
		ImGui::PushStyleColor( idx, AE::Base::BitCast<ImU32>(color) );
	}

	void  PushStyleColor (ImGuiCol idx, const AE::Base::RGBA32f &color)
	{
		ImGui::PushStyleColor( idx, ImVec4{ color.r, color.g, color.b, color.a });
	}

} // ImGui
//-----------------------------------------------------------------------------


namespace AE::Base
{
/*
=================================================
	ToString (EDebugMode)
=================================================
*/
	ND_ StringView  ToString (AE::ResEditor::IPass::EDebugMode mode)
	{
		using EDebugMode = AE::ResEditor::IPass::EDebugMode;
		switch_enum( mode )
		{
			case EDebugMode::Trace :		return "Trace";
			case EDebugMode::FnProfiling :	return "FnProfiling";
			case EDebugMode::TimeHeatMap :	return "TimeHeatMap";
			case EDebugMode::Unknown :
			case EDebugMode::_Count :
			default :						return "";
		}
		switch_end
	}

/*
=================================================
	ToString (EImageFormat)
=================================================
*/
	ND_ StringView  ToString (AE::ResEditor::EImageFormat fmt)
	{
		using EImageFormat = AE::ResEditor::EImageFormat;
		switch_enum( fmt )
		{
			case EImageFormat::DDS :		return "DDS";
			case EImageFormat::BMP :		return "BMP";
			case EImageFormat::JPG :		return "JPG";
			case EImageFormat::PCX :		return "PCX";
			case EImageFormat::PNG :		return "PNG";
			case EImageFormat::TGA :		return "TGA";
			case EImageFormat::TIF :		return "TIFF";
			case EImageFormat::PSD :		return "PSD";
			case EImageFormat::RadianceHDR:	return "RadianceHDR";
			case EImageFormat::OpenEXR :	return "OpenEXR";
			case EImageFormat::KTX :		return "KTX";
			case EImageFormat::AEImg :		return "AEImg";
			case EImageFormat::Unknown :
			case EImageFormat::_Count :
			default :						return "";
		}
		switch_end
	}

} // AE::Base
//-----------------------------------------------------------------------------


namespace AE::ResEditor
{
namespace
{
/*
=================================================
	SurfaceScaleName
=================================================
*/
	ND_ const char*  SurfaceScaleName (int scaleIdx)
	{
		switch ( scaleIdx )
		{
			default :	return "=";
			case -1 :	return "1/2";
			case -2 :	return "1/4";
			case -3 :	return "1/8";
			case -4 :	return "1/16";
			case 1 :	return "x2";
			case 2 :	return "x4";
		}
	}

/*
=================================================
	SurfaceScaleToLog2
=================================================
*/
	ND_ int  SurfaceScaleToLog2 (int scale)
	{
		switch ( scale )
		{
			default :	return 0;
			case 2 :	return 1;
			case 4 :	return 2;
			case -2 :	return -1;
			case -4 :	return -2;
			case -8 :	return -3;
			case -16 :	return -4;
		}
	}

/*
=================================================
	SurfaceScaleFromLog2
=================================================
*/
	ND_ int  SurfaceScaleFromLog2 (int scaleIdx)
	{
		switch ( scaleIdx )
		{
			case -1 :	return -2;
			case -2 :	return -4;
			case -3 :	return -8;
			case -4 :	return -16;
			case 1 :	return 2;
			case 2 :	return 4;
			default :	return 1;
		}
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	Instance
=================================================
*/
	INTERNAL_LINKAGE( UIInteraction s_UIInteraction );

	UIInteraction&  UIInteraction::Instance () {
		return s_UIInteraction;
	}

/*
=================================================
	UIInteraction ctor
=================================================
*/
	UIInteraction::UIInteraction ()
	{
		graphics->dynSize		= MakeRC<DynamicDim>( uint2{1024} );
		graphics->filterMode	= MakeRC<DynamicUInt>( 0u );
	}

/*
=================================================
	AddSliders
=================================================
*/
	void  UIInteraction::AddSliders (const void* uid, PerPassSlidersInfo info)
	{
		SliderMap_t::mapped_type	val;
		{
			auto	sliders = val.Get<PerPassMutableSliders>().WriteLock();

			for (uint i = 0; i < MaxSlidersPerType; ++i) {
				sliders->intSliders[i] = info.intRange[i][2];
			}

			for (uint i = 0; i < MaxSlidersPerType; ++i) {
				sliders->floatSliders[i] = info.floatRange[i][2];
			}

			for (uint i = 0; i < MaxSlidersPerType; ++i) {
				sliders->colors[i] = RGBA32f{info.colors[i]};
			}
		}
		val.Get<PerPassSlidersInfo>() = RVRef(info);

		_sliderMap->insert_or_assign( uid, RVRef(val) );
	}

/*
=================================================
	GetSliders
=================================================
*/
	auto  UIInteraction::GetSliders (const void* uid) const -> Ptr<const PerPassMutableSliders>
	{
		auto	slider_map	= _sliderMap.ReadLock();
		auto	it			= slider_map->find( uid );

		if ( it != slider_map->end() )
			return &it->second.Get<PerPassMutableSliders>();
		else
			return null;
	}

/*
=================================================
	GetAllSliders
=================================================
*/
	auto  UIInteraction::GetAllSliders () -> AllSliders_t
	{
		AllSliders_t	result;

		auto	slider_map = _sliderMap.WriteLock();

		for (auto& [key, pass_sliders] : *slider_map)
		{
			result.emplace_back( &pass_sliders.Get<PerPassMutableSliders>(),
								 &pass_sliders.Get<PerPassSlidersInfo>() );
		}
		return result;
	}

/*
=================================================
	RemovePass
=================================================
*/
	void  UIInteraction::RemovePass (const void* uid)
	{
		_sliderMap->erase( uid );
		_passDbgMap->erase( uid );
	}

/*
=================================================
	AddPassDbgInfo
=================================================
*/
	void  UIInteraction::AddPassDbgInfo (const IPass* uid, EDebugModeBits modes, EShaderStages stages)
	{
		modes.erase( EDebugMode::Unknown );
		if ( modes.None() or stages == Default )
			return;

		auto	dbg_map = _passDbgMap.WriteLock();

		PassDebugInfo	info;
		info.name	= String{uid->GetName()};
		info.pass	= uid;
		info.modes	= modes;
		info.stages	= stages;

		dbg_map->insert_or_assign( uid, info );
	}

/*
=================================================
	GetAllPassDbgInfo
=================================================
*/
	auto  UIInteraction::GetAllPassDbgInfo () const -> AllPassDbgInfo_t
	{
		AllPassDbgInfo_t	result;

		auto	dbg_map = _passDbgMap.ReadLock();

		for (auto& [uid, info] : *dbg_map)
		{
			result.push_back( &info );
		}
		return result;
	}
/*
=================================================
	DbgView
=================================================
*/
	void  UIInteraction::SetDbgView (usize idx, RC<Image> img)
	{
		if ( idx < _dbgView.size() )
			_dbgView[idx].store( RVRef(img) );
	}

	void  UIInteraction::ResetDbgView (usize idx)
	{
		if ( idx < _dbgView.size() )
			_dbgView[idx].store( null );
	}

	RC<Image>  UIInteraction::GetDbgView (usize idx)
	{
		if ( idx < _dbgView.size() )
			return _dbgView[idx].load();
		else
			return null;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	GetResMngr
=================================================
*/
namespace
{
  #if RmG_UI_ON_HOST
	ND_ auto  GetGraphicsLib () __NE___
	{
		auto	glib	= GraphicsScheduler().GetDevice().GetGraphicsLib();
		CHECK_FATAL( glib );
		return glib;
	}
  #endif


	ND_ auto&  GetResMngr () __NE___
	{
	  #if RmG_UI_ON_HOST
		return *GetGraphicsLib()->GetResourceManager();

	  #else
		return GraphicsScheduler().GetResourceManager();
	  #endif
	}
}
//-----------------------------------------------------------------------------



	//
	// Draw Task
	//
	class EditorUI::DrawTask final : public RenderTask
	{
	// types
	private:
	  #if RmG_UI_ON_HOST
		using TransferCtx	= GraphicsLib::ITransferContext2;
		using GraphicsCtx	= GraphicsLib::IGraphicsContext2;
		using DrawCtx		= GraphicsLib::DrawContext2;
	  #else
		using CommandBuffer	= Graphics::DirectCtx::CommandBuffer;
		using TransferCtx	= Graphics::DirectCtx::Transfer;
		using GraphicsCtx	= Graphics::DirectCtx::Graphics;
		using DrawCtx		= Graphics::DirectCtx::Draw;
	  #endif


	// variables
	private:
		EditorUI &						t;
		IOutputSurface &				surface;
		uint2							rtSize;
		ImGuiDataSync::WriteNoLock_t	imgui;
		const bool						isFirst;

		inline static const float		wnd_step		= 20.f;

		// button (start)
		inline static const RGBA8u		start_btn_idle	{ 80, 20, 170, 255};
		inline static const RGBA8u		start_btn_hover	{ 95, 20, 210, 255};
		inline static const RGBA8u		start_btn_press	{110, 20, 250, 255};

		// button (stop)
		inline static const RGBA8u		stop_btn_idle	{140, 20, 150, 255};
		inline static const RGBA8u		stop_btn_hover	{160, 20, 180, 255};
		inline static const RGBA8u		stop_btn_press	{200, 20, 220, 255};


	// methods
	public:
		DrawTask (EditorUI* t, IOutputSurface &surf, bool isFirst, CommandBatchPtr batch, DebugLabel) __NE___ :
			RenderTask{ RVRef(batch), {"UI::Draw", HtmlColor::Aqua} },
			t{ *t }, surface{ surf },
			imgui{ this->t._imgui.WriteNoLock() },
			isFirst{ isFirst }
		{}

		void  Run () __Th_OV;

	private:
		ND_ bool  _Update ();
			void  _UpdateMain (OUT float2 &wnd_pos);
			void  _UpdateEditorTab ();
			void  _UpdateGraphicsTab ();
			void  _UpdateEditor_Debugger ();
			void  _UpdateEditor_Capture ();
			void  _UpdateEditor_Sliders ();
			void  _UpdateDbgView (INOUT float2 &wnd_pos);
			void  _UpdatePopups ();
			void  _ShowHelp ();

			bool  _DrawUI (DrawCtx &dctx, const ImDrawData &drawData, const PipelineSet &ppln);
		ND_ bool  _UpdateDS (FrameUID);
		ND_ bool  _UploadVB (DrawCtx &dctx, const ImDrawData &drawData);

			void  _RecursiveVisitFolder (const Path &rootPath, const ScriptFolder &);
			void  _LoadScript (const Path &rootPath);
	};

/*
=================================================
	DrawTask::Run
=================================================
*/
	void  EditorUI::DrawTask::Run ()
	{
		ASSERT( IsFirstInBatch() == isFirst );

		t._CheckScriptDir( t._scriptDir );
		EXLOCK( imgui );

	  #if RmG_UI_ON_HOST
		auto			glib = GetGraphicsLib();
		CHECK_TE( glib->BeginFrame() );

		auto			copy_ctx_rc = glib->BeginTransferContext();
		TransferCtx&	copy_ctx	= *copy_ctx_rc;
	  #else
		CommandBuffer	cmdbuf;
	  #endif

		// upload font image
		if_unlikely( not t._uploaded.load() )
		{
		  #if not RmG_UI_ON_HOST
			TransferCtx		copy_ctx{ *this };
		  #endif

			ubyte*	pixels;
			int		width, height;
			imgui->ctx->IO.Fonts->GetTexDataAsRGBA32( OUT &pixels, OUT &width, OUT &height );

			UploadImageDesc		upload;
			upload.aspectMask	= EImageAspect::Color;
			upload.heapType		= EStagingHeapType::Dynamic;
			upload.imageDim		= int3{ width, height, 1 };
			upload.dataRowPitch	= Bytes{width * 4u};

			const Bytes	data_size = width * height * 4 * SizeOf<ubyte>;

			copy_ctx.ImageBarrier( t._res.fontImg, EResourceState::Invalidate, EResourceState::CopyDst );
			copy_ctx.CommitBarriers();

			CHECK( copy_ctx.UploadImage( t._res.fontImg, upload, ArrayView<ubyte>{ pixels, usize{data_size} }) == data_size );
			t._uploaded.store( true );

			copy_ctx.ImageBarrier( t._res.fontImg, EResourceState::CopyDst, EResourceState::FragmentShader | EResourceState::ShaderSample );
			copy_ctx.CommitBarriers();

			imgui->ctx->IO.Fonts->ClearTexData();

		  #if not RmG_UI_ON_HOST
			cmdbuf = copy_ctx.ReleaseCommandBuffer();
		  #endif
		}

		IOutputSurface::RenderTargets_t		targets;

	  #if RmG_UI_ON_HOST
		const bool		clear_surf	= true;		// always clear
		auto			gfx_ctx_rc	= glib->BeginGraphicsContext( copy_ctx_rc );
		GraphicsCtx&	gfx_ctx		= *gfx_ctx_rc;

		CHECK_TE( glib->GetTargets( OUT targets ));

	  #else
		const bool		clear_surf	= isFirst;
		GraphicsCtx		gfx_ctx		{ *this, RVRef(cmdbuf) };

		CHECK_TE( surface.GetTargets( OUT targets ));
	  #endif

		const auto&		rt = targets[0];
		rtSize = rt.RegionSize();

		CHECK_TE( _Update() );

		CHECK_TE( _UpdateDS( GetFrameId() ));

		if_unlikely( auto [fmt, cs] = ESurfaceFormat_Cast( imgui->reqSurfFormat );
					 fmt != Default or cs != Default )
		{
			imgui->reqSurfFormat = Default;

			if ( fmt != targets[0].format or cs != targets[0].colorSpace )
			{
				IOutputSurface::SurfaceInfo	new_mode;
				new_mode.colorFormat	= fmt;
				new_mode.colorSpace		= cs;

				// will be changed at the next frame
				Unused( surface.SetSurfaceMode( new_mode ));
			}
		}

		PipelineSet	ps;
		{
			auto	it = t._res.pplns.find( targets[0].format );
			CHECK_TE( it != t._res.pplns.end(),
					  "Failed to find pipeline for surface format "s << ToString(targets[0].format) );
			ps = it->second;
		}

		auto	dctx = gfx_ctx.BeginRenderPass( RenderPassDesc{ *t._res.rtech, ps.pass, rt.RegionSize() }
										.AddViewport( rt.RegionSize() )
										.AddTarget( AttachmentName{"Color"}, rt.viewId,
													(clear_surf ? rt.initialState | EResourceState::Invalidate : rt.finalState),
													rt.finalState ),
									DebugLabel{ DbgName(), DbgColor() });

		// same as ImGui::GetDrawData()
		auto*	viewport = imgui->ctx->Viewports[0];

		if_likely( viewport->DrawDataP.Valid )
		{
			if ( clear_surf )
				dctx.ClearAttachment( AttachmentName{"Color"}, RGBA32f{0.f}, rt.region, rt.layer, 1 );

			_DrawUI( dctx, viewport->DrawDataP, ps );
		}

		gfx_ctx.EndRenderPass( dctx );

	  #if RmG_UI_ON_HOST
		CHECK_TE( glib->EndFrame( gfx_ctx_rc ));

		CHECK_TE( surface.GetTargets( OUT targets ));

		// clear screen on host
		Graphics::DirectCtx::Transfer	tctx {*this};
		if ( isFirst )
		{
		#if 1
			// for profiling
			tctx.ImageBarrier( rt.imageId, rt.initialState | EResourceState::Invalidate, rt.finalState );
			tctx.CommitBarriers();
		#else
			tctx.ImageBarrier( rt.imageId, rt.initialState | EResourceState::Invalidate, EResourceState::ClearDst );
			tctx.CommitBarriers();

			tctx.ClearColorImage( rt.imageId, RGBA8u{20, 0, 60, 255}, {ImageSubresourceRange{ EImageAspect::Color }} );

			tctx.ImageBarrier( rt.imageId, EResourceState::ClearDst, rt.finalState );
			tctx.CommitBarriers();
		#endif
		}
		Execute( tctx );

	  #else
		Execute( gfx_ctx );
	  #endif
	}

/*
=================================================
	DrawTask::_Update
=================================================
*/
	bool  EditorUI::DrawTask::_Update ()
	{
		ImGui::SetCurrentContext( imgui->ctx );

		const float	dt = GraphicsScheduler().GetFrameTimeDelta().count();
		ImGuiIO &	io = ImGui::GetIO();

		io.DisplaySize	= ImVec2{float(rtSize.x), float(rtSize.y)};
		io.DeltaTime	= dt;

		// update input
		{
			ZeroMem( OUT io.MouseDown );
			ZeroMem( OUT io.NavInputs );

			io.MouseDown[0]	= imgui->mouseLBDown;
			io.MousePos		= { imgui->mousePos.x, imgui->mousePos.y };
			io.MouseWheel	= Clamp( imgui->mouseWheel.y, -1.f, 1.f );
			io.MouseWheelH	= Clamp( imgui->mouseWheel.x, -1.f, 1.f );

			s_UIInteraction.selectedPixel->pendingPos = (imgui->mousePos + 0.5f) / float2{rtSize};
		}

		ImGui::NewFrame();

		{
			ImGuiRenderer::AEStyleScope		ae_style {imgui->ctx};
			ImGuiRenderer::StyleScope		style	 {imgui->ctx};
			ImGui::PushStyleColor( ImGuiCol_Header, BitCast<uint>(RGBA8u{ 170, 0, 200, 255 }) );

			float2		wnd_pos	{0.f};
			_UpdateMain( OUT wnd_pos );

			_UpdateDbgView( INOUT wnd_pos );
			_UpdatePopups();
			_ShowHelp();
		}

		if ( imgui->reloadScript )
		{
			imgui->reloadScript = false;
			_LoadScript( imgui->scriptName );
		}

		t._profiler.DrawImGUI();

		ImGui::Render();
		return true;
	}

/*
=================================================
	DrawTask::_UpdateMain
=================================================
*/
	void  EditorUI::DrawTask::_UpdateMain (OUT float2 &wnd_pos)
	{
		ImGui::SetNextWindowPos( ImVec2{10.f, 10.f}, ImGuiCond_Once );
		ImGui::SetNextWindowSizeConstraints( ImVec2{370.f, 450.f}, ImGui::GetIO().DisplaySize );

		const auto	wnd_flags	= ImGuiWindowFlags_NoSavedSettings;

		if ( ImGui::Begin( ("ResEditor: "s << imgui->scriptName << " ##ResEditor").c_str(), null, wnd_flags ))
		{
			wnd_pos.x = ImGui::GetWindowPos().x + ImGui::GetWindowWidth() + wnd_step;
			wnd_pos.y = ImGui::GetWindowPos().y;

			if ( ImGui::BeginTabBar( "ResEditorTabs", ImGuiTabBarFlags_None ))
			{
				const ImGuiTabItemFlags		tab_scripts_flags	= (imgui->activeTab == 0 ? ImGuiTabItemFlags_SetSelected : 0);
				const ImGuiTabItemFlags		tab_editor_flags	= (imgui->activeTab == 1 ? ImGuiTabItemFlags_SetSelected : 0);
				const ImGuiTabItemFlags		tab_graphics_flags	= (imgui->activeTab == 2 ? ImGuiTabItemFlags_SetSelected : 0);
				imgui->activeTab = UMax;

				// script files tree view
				if ( ImGui::BeginTabItem( "Scripts", null, tab_scripts_flags ))
				{
					ImGui::BeginChild( "Scripts", ImVec2{ImGui::GetContentRegionAvail().x, 0.f}, true, ImGuiWindowFlags_HorizontalScrollbar );

					_RecursiveVisitFolder( Path{}, t._scriptDir.rootInfo );

					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				if ( ImGui::BeginTabItem( "Editor", null, tab_editor_flags ))
				{
					_UpdateEditorTab();
					ImGui::EndTabItem();
				}

				if ( ImGui::BeginTabItem( "Graphics", null, tab_graphics_flags ))
				{
					_UpdateGraphicsTab();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
		ImGui::End();
	}

/*
=================================================
	DrawTask::_UpdateGraphicsTab
=================================================
*/
	void  EditorUI::DrawTask::_UpdateGraphicsTab ()
	{
		auto	g_mode	= s_UIInteraction.graphics.WriteLock();
		auto	g_data	= t._graphics.ReadLock();

		if ( s_UIInteraction.capture->video )
		{
			ImGui::Text( "Stop video capture to change surface parameters" );
			return;
		}

		{
			int		scale = SurfaceScaleToLog2( g_mode->dynSize->Scale().x );
			ImGui::Text( "Surface scale" );
			if ( ImGui::SliderInt( "##SurfaceScaleSlider", INOUT &scale, -4, 2, SurfaceScaleName( scale )) )
				g_mode->dynSize->SetScale( int3{SurfaceScaleFromLog2( scale )} );

			uint2	dim = g_mode->dynSize->Dimension2();
			ImGui::Text( "Surface size: %ix%i", dim.x, dim.y );
			ImGui::Text( "Mega pix: %0.2f", double(dim.x * dim.y) * 1.0e-6 );

			const uint	prev_fm	= g_mode->filterMode->Get();

			bool	linear	= HasBit( prev_fm, 0 );
			if ( ImGui::Checkbox( "Linear filter", INOUT &linear ))
				g_mode->filterMode->Set( SetBit( prev_fm, linear, 0 ));

			bool	copy = HasBit( prev_fm, 1 );
			if ( ImGui::Checkbox( "Copy instead of blit (if possible)", INOUT &copy ))
				g_mode->filterMode->Set( SetBit( prev_fm, copy, 1 ));

			ImGui::Separator();
		}

		auto		mode			= g_data->output->GetSurfaceInfo();
		uint		color_mode		= int(FindArrayElementIndex( ArrayView<SurfaceFormat>{g_data->surfaceFormats}, SurfaceFormat{mode} ));
		uint		present_mode	= int(FindArrayElementIndex( ArrayView<EPresentMode>{g_data->presentModes}, mode.presentMode ));

		g_mode->colorModeIdx		= color_mode;
		g_mode->presentModeIdx		= present_mode;

		const auto	cur_color_mode	= color_mode < g_data->surfaceFormats.size() ?
										String{ToString( g_data->surfaceFormats[color_mode].colorFormat )} << " | " <<
										ToString( g_data->surfaceFormats[color_mode].colorSpace ) :
										"";
		const auto	cur_present		= present_mode < g_data->presentModes.size() ?
										ToString( g_data->presentModes[present_mode] ) : "";

		if ( ImGui::BeginCombo( "Surface format", cur_color_mode.c_str() ))
		{
			for (usize i = 0; i < g_data->surfaceFormats.size(); ++i)
			{
				const auto&	item = g_data->surfaceFormats[i];
				if ( ImGui::Selectable( (String{ToString( item.colorFormat )} << " | " << ToString( item.colorSpace )).c_str(), color_mode == i ))
					color_mode = uint(i);
			}
			ImGui::EndCombo();
		}

		if ( ImGui::BeginCombo( "Present mode", cur_present.data() ))
		{
			for (usize i = 0; i < g_data->presentModes.size(); ++i)
			{
				const auto&	item = g_data->presentModes[i];
				if ( ImGui::Selectable( ToString( item ).data(), present_mode == i ))
					present_mode = uint(i);
			}
			ImGui::EndCombo();
		}

		if ( color_mode		!= g_mode->colorModeIdx	or
			 present_mode	!= g_mode->presentModeIdx )
		{
			mode				= g_data->surfaceFormats [color_mode];
			mode.presentMode	= g_data->presentModes [present_mode];

			// function does not check errors, surface mode will try to change in next frames
			if ( g_data->output->SetSurfaceMode( mode ))
			{
				g_mode->colorModeIdx	 = color_mode;
				g_mode->presentModeIdx	 = present_mode;
				imgui->defaultSurfFormat = ESurfaceFormat_Cast( mode.colorFormat, mode.colorSpace );
			}
		}
	}

/*
=================================================
	DrawTask::_UpdateEditorTab
=================================================
*/
	void  EditorUI::DrawTask::_UpdateEditorTab ()
	{
		if ( ImGui::TreeNodeEx( "Debugger" ))
		{
			_UpdateEditor_Debugger();
		}

		if ( ImGui::TreeNodeEx( "Capture" ))
		{
			_UpdateEditor_Capture();
		}

		if ( ImGui::TreeNodeEx( "Sliders", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			_UpdateEditor_Sliders();
		}

		if ( ImGui::TreeNodeEx( "Statistic", ImGuiTreeNodeFlags_DefaultOpen ))
		{
			const auto	ColoredButton = [] (RGBA32f col)
			{{
				float	h	= ImGui::GetTextLineHeight();
				col.a = 1.f;
				ImGui::PushStyleColor( ImGuiCol_Button,			col );
				ImGui::PushStyleColor( ImGuiCol_ButtonHovered,	col );
				ImGui::PushStyleColor( ImGuiCol_ButtonActive,	col );
				ImGui::Button( "##PixelColor", ImVec2{h,h} );
				ImGui::PopStyleColor(3);
			}};
			const auto	sp = s_UIInteraction.selectedPixel.Read();

			ImGui::Text( "mouse pos:   %s", ToString( sp.pos ).c_str() );
			ImGui::Text( "mouse unorm: %s", ToString( sp.pendingPos ).c_str() );

			ImGui::Text( "raw color:   %s", ToString( sp.color, 3 ).c_str() );
			ImGui::SameLine();
			ColoredButton( sp.color );

			RGBA32f	srgb = RemoveSRGBCurve( Saturate( sp.color ));
			ImGui::Text( "sRGB color:  %s", ToString( srgb, 3 ).c_str() );
			ImGui::SameLine();
			ColoredButton( srgb );

			ImGui::TreePop();
			ImGui::Separator();
		}
	}

/*
=================================================
	DrawTask::_UpdateEditor_Debugger
=================================================
*/
	void  EditorUI::DrawTask::_UpdateEditor_Debugger ()
	{
		using EDebugModeBits = UIInteraction::EDebugModeBits;

		const auto	DbgMode_ShaderStages = [this] (const EDebugModeBits inModes, const EShaderStages inStages)
		{{
			const char*	cur_mode	= ToString( IPass::EDebugMode(imgui->dbgModeIdx) ).data();
			const char*	cur_stage	= imgui->dbgStageIdx < uint(EShader::_Count) ? ToString( EShader(imgui->dbgStageIdx) ).data() : "";

			if ( ImGui::BeginCombo( "Mode", cur_mode ))
			{
				for (auto modes = inModes; modes.Any();)
				{
					const auto	mode = modes.ExtractFirst();
					if ( mode == Default )
						continue;

					if ( ImGui::Selectable( ToString( mode ).data(), imgui->dbgModeIdx == uint(mode) ))
						imgui->dbgModeIdx = uint(mode);
				}
				ImGui::EndCombo();
			}

			if ( ImGui::BeginCombo( "Stage", cur_stage ))
			{
				for (auto sh : BitIndexIterate<EShader>( inStages ))
				{
					if ( ImGui::Selectable( ToString( sh ).data(), imgui->dbgStageIdx == uint(sh) ))
						imgui->dbgStageIdx = uint(sh);
				}
				ImGui::EndCombo();
			}
		}};

		const auto	dbg_passes	= s_UIInteraction.GetAllPassDbgInfo();
		const char*	cur_pass	= imgui->dbgPassIdx < dbg_passes.size() ?
									dbg_passes[imgui->dbgPassIdx]->name.c_str() :
									(imgui->dbgPassIdx == dbg_passes.size() ? "All" : "");

		ImGui::Text( "G  - run shader debugger for pixel under cursor" );
		ImGui::Separator();

		if ( ImGui::BeginCombo( "Pass", cur_pass ))
		{
			for (usize i = 0; i < dbg_passes.size(); ++i) {
				if ( ImGui::Selectable( dbg_passes[i]->name.c_str(), imgui->dbgPassIdx == i ))
					imgui->dbgPassIdx = uint(i);
			}
			if ( ImGui::Selectable( "All", imgui->dbgPassIdx == dbg_passes.size() ))
				imgui->dbgPassIdx = uint(dbg_passes.size());
			ImGui::EndCombo();
		}

		if ( imgui->dbgPassIdx < dbg_passes.size() )
			DbgMode_ShaderStages( dbg_passes[ imgui->dbgPassIdx ]->modes, dbg_passes[ imgui->dbgPassIdx ]->stages );
		else
		if ( imgui->dbgPassIdx == dbg_passes.size() )
			DbgMode_ShaderStages( EDebugModeBits{}.SetAll(), EShaderStages::Unknown );

		ImGui::Separator();

		ImGui::Checkbox( "Show debug view", INOUT &imgui->showDbgViews );
		ImGui::TreePop();
		ImGui::Separator();


		auto	dbg = s_UIInteraction.debugger.WriteLock();

		dbg->target	= null;

		if ( imgui->runShaderDebugger )
		{
			imgui->runShaderDebugger = false;

			dbg->mode	= IPass::EDebugMode(imgui->dbgModeIdx);
			dbg->coord	= imgui->mousePos / float2(rtSize);

			if ( imgui->dbgPassIdx < dbg_passes.size() )
			{
				dbg->target	= dbg_passes[imgui->dbgPassIdx]->pass;
				dbg->stage	= EShaderStages(0) | EShader(imgui->dbgStageIdx);
			}
			else
			if ( imgui->dbgPassIdx == dbg_passes.size() )
			{
				dbg->target	= BitCast<void*>(usize(0x1));
				dbg->stage	= EShaderStages::All;
			}
		}
	}

/*
=================================================
	DrawTask::_UpdateEditor_Capture
=================================================
*/
	void  EditorUI::DrawTask::_UpdateEditor_Capture ()
	{
		auto	capture = s_UIInteraction.capture.WriteLock();

		// image capture
		if ( ImGui::Button( "Screenshot (I)" ))
			capture->screenshot = true;

		if ( ImGui::BeginCombo( "Image format", ToString(capture->imageFormat).data() ))
		{
			for (uint i = 1; i < uint(EImageFormat::_Count); ++i)
			{
				if ( ImGui::Selectable( ToString( EImageFormat(i) ).data(), uint(capture->imageFormat) == i ))
					capture->imageFormat = EImageFormat(i);
			}
			ImGui::EndCombo();
		}


		// video capture
		if ( not capture->video )
		{
			ImGui::PushStyleColor( ImGuiCol_Button,			start_btn_idle );
			ImGui::PushStyleColor( ImGuiCol_ButtonHovered,	start_btn_hover );
			ImGui::PushStyleColor( ImGuiCol_ButtonActive,	start_btn_press );
			if ( ImGui::Button( "Start recording (U)" ))
				capture->video = true;
			ImGui::PopStyleColor(3);
		}
		else
		{
			ImGui::PushStyleColor( ImGuiCol_Button,			stop_btn_idle );
			ImGui::PushStyleColor( ImGuiCol_ButtonHovered,	stop_btn_hover );
			ImGui::PushStyleColor( ImGuiCol_ButtonActive,	stop_btn_press );
			if ( ImGui::Button( "Stop recording (U)" ))
				capture->video = false;
			ImGui::PopStyleColor(3);
		}

		ImGui::InputFloat( "Bitrate (Mbit/s)", INOUT &capture->bitrate, 1.f, 102.4f );

		if ( ImGui::BeginCombo( "Video format", ToString(capture->videoFormat).data() ))
		{
			for (uint i = 1; i < uint(EVideoFormat::_Count); ++i)
			{
				if ( ImGui::Selectable( ToString( EVideoFormat(i) ).data(), uint(capture->videoFormat) == i ))
					capture->videoFormat = EVideoFormat(i);
			}
			ImGui::EndCombo();
		}

		if ( ImGui::BeginCombo( "Video codec", ToString(capture->videoCodec).data() ))
		{
			for (uint i = 1; i < uint(EVideoCodec::_Count); ++i)
			{
				if ( ImGui::Selectable( ToString( EVideoCodec(i) ).data(), uint(capture->videoCodec) == i ))
					capture->videoCodec = EVideoCodec(i);
			}
			ImGui::EndCombo();
		}

		if ( ImGui::BeginCombo( "Color preset", ToString(capture->colorPreset).data() ))
		{
			for (uint i = 1; i < uint(EVideoColorPreset::_Count); ++i)
			{
				if ( ImGui::Selectable( ToString( EVideoColorPreset(i) ).data(), uint(capture->colorPreset) == i ))
					capture->colorPreset = EVideoColorPreset(i);
			}
			ImGui::EndCombo();
		}

		ImGui::TreePop();
		ImGui::Separator();
	}

/*
=================================================
	DrawTask::_UpdateEditor_Sliders
=================================================
*/
	void  EditorUI::DrawTask::_UpdateEditor_Sliders ()
	{
		const auto	all_sliders = s_UIInteraction.GetAllSliders();
		const char	xyzw[]		= "xyzw";

		for (auto& pass : all_sliders)
		{
			if ( ImGui::TreeNodeEx( pass.Get<1>()->passName.c_str(), ImGuiTreeNodeFlags_DefaultOpen ))
			{
				auto	sliders = pass.Get<0>()->WriteLock();

				for (uint i = 0; i < UIInteraction::MaxSlidersPerType; ++i)
				{
					const auto&		name	= pass.Get<1>()->names [i] [UIInteraction::IntSliderIdx];
					int4&			slider	= sliders->intSliders [i];
					int4 const*		range	= pass.Get<1>()->intRange [i].data();
					const uint		vsize	= pass.Get<1>()->intVecSize [i];

					if ( name.empty() ) continue;
					for (uint j = 0; j < vsize; ++j)
					{
						const auto	sname = String{name} << '.' << xyzw[j] << "##CustomSliderI" << char('0'+i) << char('0'+j);
						ImGui::SliderInt( sname.c_str(), INOUT &slider[j], range[0][j], range[1][j] );
					}
				}

				for (uint i = 0; i < UIInteraction::MaxSlidersPerType; ++i)
				{
					const auto&		name	= pass.Get<1>()->names [i] [UIInteraction::FloatSliderIdx];
					float4&			slider	= sliders->floatSliders [i];
					float4 const*	range	= pass.Get<1>()->floatRange [i].data();
					const uint		vsize	= pass.Get<1>()->floatVecSize [i];

					if ( name.empty() ) continue;
					for (uint j = 0; j < vsize; ++j)
					{
						const auto	sname = String{name} << '.' << xyzw[j] << "##CustomSliderF" << char('0'+i) << char('0'+j);
						ImGui::SliderFloat( sname.c_str(), INOUT &slider[j], range[0][j], range[1][j] );
					}
				}

				for (uint i = 0; i < UIInteraction::MaxSlidersPerType; ++i)
				{
					const auto&		name	= pass.Get<1>()->names [i] [UIInteraction::ColorSelectorIdx];
					auto&			slider	= sliders->colors[i];

					if ( name.empty() ) continue;
					ImGui::ColorEdit4( (name + "##CustomColor" + char('0'+i)).c_str(), INOUT slider.data(), ImGuiColorEditFlags_None );
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
		ImGui::Separator();
	}

/*
=================================================
	DrawTask::_UpdateDbgView
=================================================
*/
	void  EditorUI::DrawTask::_UpdateDbgView (INOUT float2 &wnd_pos)
	{
		// setup style
		{
			// window / frame
			ImGui::PushStyleColor( ImGuiCol_WindowBg,	RGBA8u{ 20, 0, 40, 255} );
		}

		const auto	dbg_wnd_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar |
									ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse;

		if ( not imgui->showDbgViews )
			return;

		for (uint i = 0; i < UIInteraction::MaxDebugViews; ++i)
		{
			RC<Image>	img		= s_UIInteraction.GetDbgView( i );
			float4&		data	= imgui->dbgViewData[i];			// { pos(x,y), scale, scaleIdx }

			if ( not img )
				continue;

			const auto	wnd_name = String{img->GetName()} << "##DbgView-"s << ToString(i+1);

			ImGui::SetNextWindowPos( ImVec2{wnd_pos.x, wnd_pos.y}, ImGuiCond_Once );
			ImGui::SetNextWindowSize( ImVec2{200.f, 200.f}, ImGuiCond_Once );

			if ( ImGui::Begin( wnd_name.c_str(), null, dbg_wnd_flags ))
			{
				const ImVec2	wpos	= ImGui::GetWindowPos();
				const ImVec2	wsize	= ImGui::GetWindowSize();
				const ImVec2	maxsc	{ ImGui::GetScrollMaxX(), ImGui::GetScrollMaxY() };
				const ImageDesc	desc	= img->GetImageDesc();
				bool			mwheel	= false;

				wnd_pos.x = wpos.x + wsize.x + wnd_step;

				if ( ImGui::IsWindowHovered( ImGuiHoveredFlags_None ))
				{
					data.w = float(Clamp( data.w + imgui->mouseWheel.y * 0.25f, -1.f, 10.f ));
					mwheel = IsNotZero( imgui->mouseWheel.y );
				}
				data.z = Pow( 2.f, data.w );
				data.x = ImGui::GetScrollX() / ImGui::GetScrollMaxX();
				data.y = ImGui::GetScrollY() / ImGui::GetScrollMaxY();

				const ImVec2	img_size{ desc.dimension.x * data.z, desc.dimension.y * data.z };
				/*if ( mwheel )
				{
					const float2	delta = (imgui->mousePos - float2{wpos.x, wpos.y}) * data.z;
					ImGui::SetScrollX( ImGui::GetScrollX() + delta.x );
					ImGui::SetScrollY( ImGui::GetScrollY() + delta.y );
				}*/

				ImGui::Image( BitCast<ImTextureID>(usize(i+1)), img_size );
			}
			ImGui::End();
		}
	}

/*
=================================================
	DrawTask::_UpdatePopups
=================================================
*/
	void  EditorUI::DrawTask::_UpdatePopups ()
	{
		const bool	compiling		= t._compiling.load();
		const bool	pause_rendering	= t._pauseRendering.load();

		if ( not (compiling or pause_rendering) )
			return;

		// setup style
		{
			// window / frame
			ImGui::PushStyleColor( ImGuiCol_WindowBg,	RGBA8u{ 30, 0, 20, 255} );
		}

		const char	text1 [] = "Compiling...";
		const char	text2 [] = "Paused";
		const auto	text	 = StringView(compiling ? text1 : text2);

		const auto	wnd_flags =	ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
								ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
								ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize;
		ImGuiIO &	io		= ImGui::GetIO();
		const float	h		= ImGui::GetTextLineHeight();
		const float	w		= h * text.length();

		ImGui::SetNextWindowPos( ImVec2{(io.DisplaySize.x-w)/2, io.DisplaySize.y/2-h}, ImGuiCond_Always );

		ImGui::Begin( "CompilingPopup", null, wnd_flags );
		ImGui::Text( "%s", text.data() );
		ImGui::End();
	}

/*
=================================================
	DrawTask::_ShowHelp
=================================================
*/
	void  EditorUI::DrawTask::_ShowHelp ()
	{
		if ( not imgui->showHelp )
			return;

		const auto	wnd_flags =	ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
								ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
								ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize;

		ImGuiIO &	io		= ImGui::GetIO();
		const float	y		= io.DisplaySize.y / 2 - ImGui::GetTextLineHeight() * Parser::CalculateNumberOfLines( imgui->helpText );
		const float	x		= io.DisplaySize.x / 2;

		ImGui::SetNextWindowPos( ImVec2{x,y}, ImGuiCond_Always );

		ImGui::Begin( "Help", null, wnd_flags );
		ImGui::Text( "%s", imgui->helpText.c_str() );
		ImGui::End();
	}

/*
=================================================
	DrawTask::_RecursiveVisitFolder
=================================================
*/
	void  EditorUI::DrawTask::_RecursiveVisitFolder (const Path &rootPath, const ScriptFolder &dir)
	{
		const ImGuiTreeNodeFlags	base_flags	=	//ImGuiTreeNodeFlags_OpenOnArrow
													//| ImGuiTreeNodeFlags_OpenOnDoubleClick
													//| ImGuiTreeNodeFlags_SpanAvailWidth
													ImGuiTreeNodeFlags_SpanFullWidth;

		auto		node_flags	= base_flags;
		usize		node_id		= dir.firstId;
		const usize	clicked		= imgui->nodeClicked;

		if ( not dir.name.empty() )
		{
			auto	flags = node_flags;

			if ( clicked > dir.firstId and clicked < dir.lastId )
				flags |= ImGuiTreeNodeFlags_DefaultOpen;

			bool	node_open = ImGui::TreeNodeEx( BitCast<void*>(node_id), flags, "%s", dir.name.c_str() );
			if ( not node_open )
				return;
		}

		for (auto& folder : dir.folders) {
			_RecursiveVisitFolder( rootPath / dir.name, *folder );
		}

		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;// | ImGuiTreeNodeFlags_Bullet;

		for (auto& script : dir.scripts)
		{
			node_flags &= ~ImGuiTreeNodeFlags_Selected;

			++node_id;

			if ( node_id == clicked )
				node_flags |= ImGuiTreeNodeFlags_Selected;

			ImGui::TreeNodeEx( BitCast<void*>(node_id), node_flags, "%s", script.c_str() );

			if ( ImGui::IsItemClicked() and not ImGui::IsItemToggledOpen() )
			{
				imgui->nodeClicked	= node_id;
				imgui->scriptName	= ToString( rootPath / dir.name / script );

				_LoadScript( rootPath / dir.name / script );
			}
		}

		if ( not dir.name.empty() )
			ImGui::TreePop();
	}

/*
=================================================
	DrawTask::_LoadScript
=================================================
*/
	void  EditorUI::DrawTask::_LoadScript (const Path &rootPath)
	{
		t._compiling.store( true );

		imgui->activeTab	= 1;
		imgui->dbgPassIdx	= UMax;
		imgui->dbgModeIdx	= UMax;
		imgui->dbgStageIdx	= UMax;

		Path	path = t._scriptDir.root;
		path /= rootPath;
		path.replace_extension( "as" );

		Scheduler().Run(
			ETaskQueue::Background,
			[] (RC<ResEditorCore> core, Path inPath, Atomic<bool> &compiling, Atomic<bool> &pauseRendering) -> CoroTask
			{
				Unused( core->RunRenderScriptAsync( inPath ));
				compiling.store( false );
				pauseRendering.store( false );
				co_return;
			}
			( t._core.GetRC<ResEditorCore>(), path, t._compiling, t._pauseRendering ),
			Tuple{},
			"UI::LoadScript"
		);
	}

/*
=================================================
	DrawTask::_UploadVB
=================================================
*/
	bool  EditorUI::DrawTask::_UploadVB (DrawCtx &dctx, const ImDrawData &drawData)
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
		dctx.BindVertexBuffer( 0, vstream.id, vstream.offset );
		dctx.BindIndexBuffer( istream.id, istream.offset, IndexDesc<ImDrawIdx>::value );

		return true;
	}

/*
=================================================
	DrawTask::_UpdateDS
=================================================
*/
	bool  EditorUI::DrawTask::_UpdateDS (FrameUID fid)
	{
		DescriptorSetID	ds = t._res.descSets[ fid.Index() ];

		StaticArray< ImageViewID, 8 >	textures;
		textures.fill( t._res.fontView );

		for (uint i = 0; i < UIInteraction::MaxDebugViews; ++i)
		{
			RC<Image>	img = s_UIInteraction.GetDbgView( i );
			if ( img )
				textures[i+1] = img->GetViewId();
		}

	  #if RmG_UI_ON_HOST
		auto				du_rc	= GetGraphicsLib()->CreateDescriptorUpdater();
		IDescriptorUpdater&	updater	= *du_rc;
	  #else
		DescriptorUpdater	updater;
	  #endif

		CHECK_ERR( updater.Set( ds, EDescUpdateMode::Partialy ));
		CHECK_ERR( textures.size() == updater.ImageCount( UniformName{"un_Textures"} ));
		updater.BindImages( UniformName{"un_Textures"}, textures );
		CHECK_ERR( updater.Flush() );

		return true;
	}

/*
=================================================
	DrawTask::_DrawUI
=================================================
*/
	bool  EditorUI::DrawTask::_DrawUI (DrawCtx &dctx, const ImDrawData &drawData, const PipelineSet &ppln)
	{
		const bool	is_minimized = (drawData.DisplaySize.x <= 0.0f or drawData.DisplaySize.y <= 0.0f);

		if_unlikely( drawData.TotalVtxCount <= 0 or is_minimized )
			return false;

		DescriptorSetID	ds = t._res.descSets[ dctx.GetFrameId().Index() ];

		dctx.BindPipeline( ppln.blend );
		dctx.BindDescriptorSet( t._res.dsIndex, ds );

		{
			ImVec2 const&			scale	= imgui->ctx->IO.DisplayFramebufferScale;
			ShaderTypes::imgui_ub	ub_data;

			ub_data.scale.x		= 2.0f / (drawData.DisplaySize.x * scale.x);
			ub_data.scale.y		= 2.0f / (drawData.DisplaySize.y * scale.y);

			ub_data.translate.x	= -1.0f - (drawData.DisplayPos.x * ub_data.scale.x);
			ub_data.translate.y	= -1.0f - (drawData.DisplayPos.y * ub_data.scale.y);

			dctx.PushConstant( t._res.pc1Index, ub_data );
		}

		CHECK_ERR( _UploadVB( dctx, drawData ));

		uint	idx_offset	= 0;
		uint	vtx_offset	= 0;
		uint	cur_tex		= UMax;

		for (int i = 0; i < drawData.CmdListsCount; ++i)
		{
			ImDrawList const&	cmd_list = *drawData.CmdLists[i];

			for (int j = 0; j < cmd_list.CmdBuffer.Size; ++j)
			{
				ImDrawCmd const&	cmd = cmd_list.CmdBuffer[j];
				const uint			tex	= uint(usize(cmd.TextureId));

				if ( tex != cur_tex )
				{
					cur_tex = tex;
					dctx.PushConstant( t._res.pc2Index, ShaderTypes::imgui_pc{tex} );
					dctx.BindPipeline( tex == 0 ? ppln.blend : ppln.opaque );
				}

				if_likely( cmd.UserCallback == null )
				{
					RectI	scissor;
					scissor.left	= int(cmd.ClipRect.x + 0.5f);
					scissor.top		= int(cmd.ClipRect.y + 0.5f);
					scissor.right	= int(cmd.ClipRect.z + 0.5f);
					scissor.bottom	= int(cmd.ClipRect.w + 0.5f);

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
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	EditorUI::EditorUI (ResEditorCore &core, Path scriptPath) :
		_core{core}, _scriptDir{RVRef(scriptPath)}
	{
		SetHelpText( "" );
	}

/*
=================================================
	destructor
=================================================
*/
	EditorUI::~EditorUI ()
	{
		auto&	res_mngr = GetResMngr();

		res_mngr.ReleaseResourceArray( _res.descSets );
		res_mngr.ReleaseResources( _res.fontImg, _res.fontView );

		{
			auto	imgui = _imgui.WriteLock();
			if ( imgui->ctx != null )
				ImGui::DestroyContext( imgui->ctx );
		}
	}

/*
=================================================
	Init
=================================================
*/
	bool  EditorUI::Init (IOutputSurface &surface, EWindowMode wndMode)
	{
		if ( _initialized.load() )
			return true;

		CHECK_ERR( surface.IsInitialized() );

		if ( _InitSurface( surface ) and _LoadPipelinePack() )
		{
			_initialized.store( true );

			if ( EWindowMode_IsFullscreen( wndMode )) {
				_windowMode.current.store( 1 | 2 );
				_windowMode.fullscreenMode = wndMode;
			}else{
				_windowMode.current.store( 0 | 2 );
				_windowMode.windowedMode = wndMode;
			}
			return true;
		}
		return false;
	}

/*
=================================================
	_InitSurface
=================================================
*/
	bool  EditorUI::_InitSurface (IOutputSurface &surface)
	{
		auto		g_data	= _graphics.WriteLock();
		const auto	info	= surface.GetSurfaceInfo();

		g_data->output			= &surface;
		g_data->surfaceFormats	= surface.GetSurfaceFormats();
		g_data->presentModes	= surface.GetPresentModes();

		{
			auto	g_mode			= s_UIInteraction.graphics.WriteLock();
			g_mode->colorModeIdx	= int(FindArrayElementIndex( ArrayView<SurfaceFormat>{g_data->surfaceFormats}, SurfaceFormat{info} ));
			g_mode->presentModeIdx	= int(FindArrayElementIndex( ArrayView<EPresentMode>{g_data->presentModes}, info.presentMode ));
		}

		_imgui->defaultSurfFormat = ESurfaceFormat_Cast( info.colorFormat, info.colorSpace );
		return true;
	}

/*
=================================================
	_LoadPipelinePack
=================================================
*/
	bool  EditorUI::_LoadPipelinePack ()
	{
		#if RmG_UI_ON_HOST
			CHECK_ERR( _LoadPipelinePack( GetResMngr() ));
		#endif

		CHECK_ERR( _LoadPipelinePack( GraphicsScheduler().GetResourceManager() ));
		CHECK_ERR( _InitUI( Default ));

		return true;
	}

	bool  EditorUI::_LoadPipelinePack (auto& resMngr) const
	{
		StringView	fname;
		switch_enum( resMngr.GetDevice().GetGraphicsAPI() )
		{
			case EGraphicsAPI::Vulkan :		fname = "vk/ui_pipelines.bin";	break;
			case EGraphicsAPI::Metal :		fname = "mac/ui_pipelines.bin";	break;
		}

		auto	file = MakeRC<FileRStream>( NtStringView{fname} );
		CHECK_ERR( file->IsOpen() );

		PipelinePackDesc	desc;
		desc.stream		= file;
		desc.options	= EPipelinePackOpt::All;
		desc.dbgName	= "editor ui pack";

		auto	pack_id = resMngr.LoadPipelinePack( desc );
		CHECK_ERR( resMngr.InitializeResources( RVRef(pack_id) ));

		return true;
	}

/*
=================================================
	_InitUI
=================================================
*/
	bool  EditorUI::_InitUI (PipelinePackID pack)
	{
		auto	imgui = _imgui.WriteLock();

		CHECK( _profiler.Initialize( null ));

		// init ImGUI context
		if ( imgui->ctx == null )
		{
			IMGUI_CHECKVERSION();

			imgui->ctx = ImGui::CreateContext();
			CHECK_ERR( imgui->ctx != null );

			ImGui::StyleColorsDark();
		}


		auto&		res_mngr	= GetResMngr();
		const auto	max_frames	= GraphicsScheduler().GetMaxFrames();

		// initialize font atlas
		{
			uint8_t*	pixels;
			int			width, height;
			imgui->ctx->IO.Fonts->GetTexDataAsRGBA32( OUT &pixels, OUT &width, OUT &height );

			_res.fontImg = res_mngr.CreateImage( ImageDesc{}.SetDimension( width, height )
													.SetFormat( EPixelFormat::RGBA8_UNorm )
													.SetUsage( EImageUsage::Sampled | EImageUsage::TransferDst ),
												 "Imgui font image" );
			CHECK_ERR( _res.fontImg );

			_res.fontView = res_mngr.CreateImageView( ImageViewDesc{}, _res.fontImg, "Imgui font image view" );
			CHECK_ERR( _res.fontView );
		}

		_res.rtech = res_mngr.LoadRenderTech( pack, RenderTechName{"UI.RTech"}, Default );
		CHECK_ERR( _res.rtech );

		const Pair< EPixelFormat, const char* >	formats [] = {
			{ EPixelFormat::BGRA8_UNorm,	"BGRA8" },
			{ EPixelFormat::sBGR8_A8,		"sBGR8_A8" },
			{ EPixelFormat::RGBA8_UNorm,	"RGBA8" },
			{ EPixelFormat::sRGB8_A8,		"sRGB8_A8" },
			{ EPixelFormat::RGB10_A2_UNorm,	"RGB10_A2" },
			{ EPixelFormat::RGBA16F,		"RGBA16F" }
		};
		GraphicsPipelineID	ppln;

		for (auto& fmt : formats)
		{
			PipelineSet		ps;
			ps.pass		= RenderTechPassName{"UI."s + fmt.second};
			ps.blend	= _res.rtech->GetGraphicsPipeline( PipelineName{"imgui."s + fmt.second} );
			ps.opaque	= _res.rtech->GetGraphicsPipeline( PipelineName{"imgui.opaque."s + fmt.second} );

			if ( ps.blend and ps.opaque )
			{
				_res.pplns.emplace( fmt.first, ps );
				ppln = ps.blend;
			}
		}
		CHECK_ERR( not _res.pplns.empty() );

		CHECK_ERR( res_mngr.CreateDescriptorSets( OUT _res.dsIndex, OUT _res.descSets.data(), max_frames,
												  ppln, DescriptorSetName{"imgui.ds"} ));

		_res.pc1Index = res_mngr.GetPushConstantIndex< ShaderTypes::imgui_ub >( ppln, PushConstantName{"ub"} );
		_res.pc2Index = res_mngr.GetPushConstantIndex< ShaderTypes::imgui_pc >( ppln, PushConstantName{"pc"} );
		CHECK_ERR( _res.pc1Index and _res.pc2Index );

		return true;
	}

/*
=================================================
	Draw
=================================================
*/
	AsyncTask  EditorUI::Draw (ArrayView<AsyncTask> deps)
	{
		if_unlikely( not _initialized.load() )
			return null;

		if ( not _imgui.ConstPtr()->showUI )
			return null;

		auto&	rg		= RenderGraph();
		auto	batch	= rg.UI();
		CHECK_ERR( batch );

		auto	surf_acquire = rg.BeginOnSurface( batch );
		CHECK_ERR( surf_acquire );

		AsyncTask	upload;
		const bool	is_first	= batch.CmdPool_IsEmpty();

		return batch.Task< DrawTask >( Tuple{ this, rg.GetSurfaceArg(), is_first }, {"MainUI pass"} )
						.Run( Tuple{surf_acquire, deps} );
	}

/*
=================================================
	ProcessInput
=================================================
*/
	INTERNAL_LINKAGE( constexpr auto&	IA = InputActions::Main_UI );

	void  EditorUI::ProcessInput (ActionQueueReader reader, OUT bool &switchMode)
	{
		if ( not _initialized.load() )
			return;

		auto	imgui = _imgui.WriteLock();

		switchMode					= false;
		imgui->mouseLBDown			= false;
		imgui->mouseWheel			= {};
		imgui->runShaderDebugger	= false;
		imgui->reloadScript			= false;

		ActionQueueReader::Header	hdr;
		for (; reader.ReadHeader( OUT hdr );)
		{
			switch_IA( hdr.name )
			{
				case IA.UI_MousePos :
					imgui->mousePos = reader.Data<packed_float2>( hdr.offset );		break;

				case IA.UI_MouseWheel :
					imgui->mouseWheel = reader.Data<packed_float2>( hdr.offset );	break;

				case IA.UI_MouseLBDown :
					imgui->mouseLBDown = true;										break;

				case IA.UI_SwitchInputMode :
					switchMode = true;												break;

				case IA.UI_ShowHide :
					imgui->showUI = not imgui->showUI;								break;

				case IA.UI_StartStopRecording : {
					bool	prev = s_UIInteraction.capture->video;
					s_UIInteraction.capture->video = not prev;						break;
				}
				case IA.UI_Screenshot :
					s_UIInteraction.capture->screenshot = true;						break;

				case IA.UI_ShaderDebugger :
					imgui->runShaderDebugger = true;								break;

				case IA.UI_ReloadScript :
					imgui->reloadScript = true;										break;

				case IA.UI_FrameCapture :
					_requestCapture.store( true );									break;

				case IA.PauseRendering :
					_pauseRendering.store( not _pauseRendering.load() );			break;

				case IA.ShowHelp :
					imgui->showHelp = not imgui->showHelp;							break;

				case IA.FullscreenOnOff :
					_windowMode.current.store( (~_windowMode.current.load() & 1) );	break;

				case IA.UI_CopySliderState :
					_CopySliderState();												break;

				case IA.UI_MouseRBDown :
				case IA.UI_ResExport :
				case IA.CustomKey1 :
				case IA.Freeze :			break;	// ignore
			}
			switch_end
		}
	}

/*
=================================================
	_CopySliderState
=================================================
*/
	void  EditorUI::_CopySliderState ()
	{
		const auto	all_sliders = s_UIInteraction.GetAllSliders();
		String		str2;
		Array< Pair< String, StringView >>	arr;

		for (auto& pass : all_sliders)
		{
			arr.clear();

			auto	sliders = pass.Get<0>()->WriteLock();

			for (uint i = 0; i < UIInteraction::MaxSlidersPerType; ++i)
			{
				StringView	name	= pass.Get<1>()->names [i] [UIInteraction::IntSliderIdx];
				int4&		slider	= sliders->intSliders [i];
				const uint	vsize	= pass.Get<1>()->intVecSize [i];

				if ( name.empty() ) continue;

				String	str;
				for (uint j = 0; j < vsize; ++j)
					str << ToString( slider[j] ) << ", ";

				arr.emplace_back( RVRef(str), name );
			}

			for (uint i = 0; i < UIInteraction::MaxSlidersPerType; ++i)
			{
				StringView	name	= pass.Get<1>()->names [i] [UIInteraction::FloatSliderIdx];
				float4&		slider	= sliders->floatSliders [i];
				const uint	vsize	= pass.Get<1>()->floatVecSize [i];

				if ( name.empty() ) continue;

				String	str;
				for (uint j = 0; j < vsize; ++j)
					str << ToString( slider[j], 4 ) << "f, ";

				arr.emplace_back( RVRef(str), name );
			}

			for (uint i = 0; i < UIInteraction::MaxSlidersPerType; ++i)
			{
				StringView	name	= pass.Get<1>()->names [i] [UIInteraction::ColorSelectorIdx];
				RGBA32f&	slider	= sliders->colors[i];

				if ( name.empty() ) continue;

				String	str;
				for (uint j = 0; j < 4; ++j)
					str << ToString( slider[j], 4 ) << "f, ";

				arr.emplace_back( RVRef(str), name );
			}

			if ( arr.empty() )
				continue;

			const usize	tab_size	= 4;
			usize		max_len		= 0;

			for (auto& [params_str, name] : arr) {
				max_len = Max( max_len, params_str.size() );
			}
			max_len = AlignUp( max_len, tab_size ) + 1;

			str2 << "\n// pass: " << pass.Get<1>()->passName;

			for (auto& [params_str, name] : arr)
			{
				str2 << "\n\t\t" << params_str;
				for (usize i = AlignDown( params_str.size(), tab_size ); i < max_len; i += 4)
					str2 << '\t';
				str2 << "// " << name;
			}
			str2 << '\n';
		}

		CHECK( PlatformUtils::ClipboardPut( str2 ));
		AE_LOGI( "slider state copied to clipboard" );
	}

/*
=================================================
	SetHelpText
=================================================
*/
	void  EditorUI::SetHelpText (String txt)
	{
R"(UI controls:
  'Esc'       - switch to camera controls
  'Backspace' - pause rendering, keep only UI
  'F1'        - show/hide help
  'Tab' - - - - show/hide UI
  'U'         - start/stop video recording (if supported)
  'I'   - - - - make screenshot
  'G'         - run shader debugger for pixel under cursor
  'F2'  - - - - RenderDoc frame capture (if enabled)
  'F3'        - export resources from current script (if defined)
  'F4'  - - - - copy slider state to clipboard
  'F5'        - reload script
  'F11' - - - - fullscreen / windowed
  'Right mouse btn' - pass mouse position to shader
)" >> txt;

		_imgui->helpText = RVRef(txt);
	}

/*
=================================================
	SetSurfaceFormat
=================================================
*/
	void  EditorUI::SetSurfaceFormat (ESurfaceFormat fmt)
	{
		auto	imgui = _imgui.WriteLock();

		imgui->reqSurfFormat = (fmt != Default ? fmt : imgui->defaultSurfFormat);
	}

/*
=================================================
	_CheckScriptDir
=================================================
*/
	void  EditorUI::_CheckScriptDir (INOUT ScriptDirData &scriptDir)
	{
		if ( not scriptDir.timer.Tick() )
			return;

		scriptDir.rootInfo	= Default;

		usize	node_id = 0;
		_RecursiveCheckScriptDir( INOUT scriptDir.rootInfo, INOUT node_id, scriptDir.root, 0, scriptDir.maxDepth );
	}

	void  EditorUI::_RecursiveCheckScriptDir (INOUT ScriptFolder &rootDst, INOUT usize &nodeID, const Path &rootDir, uint depth, const uint maxDepth)
	{
		ASSERT( depth < maxDepth );

		rootDst.firstId = nodeID;
		++nodeID;

		// process directories
		for (auto& dir : FileSystem::Enum( rootDir ))
		{
			if ( not dir.IsDirectory() )
				continue;

			if ( not IsAnsiString( dir.Get().filename().native() ))
				continue;

			const String	name = ToString( dir.Get().filename().native() );

			if ( depth+1 < maxDepth and not FileSystem::IsEmptyDirectory( dir ))
			{
				auto&	dst = rootDst.folders.emplace_back();
				dst.reset( new ScriptFolder{} );
				dst->name	= name;

				_RecursiveCheckScriptDir( INOUT *dst, INOUT nodeID, dir, depth+1, maxDepth );
			}
		}

		// process files
		for (auto& dir : FileSystem::Enum( rootDir ))
		{
			if ( not dir.IsFile() )
				continue;

			if ( not IsAnsiString( dir.Get().filename().native() ))
				continue;

			const String	name = ToString( dir.Get().filename().native() );

			if ( EndsWith( name, ".as" ))
				rootDst.scripts.push_back( name.substr( 0, name.length()-3 ));
		}

		nodeID += rootDst.scripts.size();
		rootDst.lastId = nodeID;
	}

/*
=================================================
	GetNewWindowMode
=================================================
*/
	auto  EditorUI::GetNewWindowMode () -> Optional<EWindowMode>
	{
		ubyte	cur = _windowMode.current.fetch_or( 1 << 1 );

		if ( HasBit( cur, 1 ))
			return NullOptional;	// already used

		return HasBit( cur, 0 ) ? _windowMode.fullscreenMode : _windowMode.windowedMode;
	}


} // AE::ResEditor
