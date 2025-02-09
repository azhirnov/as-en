// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ui_editor/Core/EditorCore.h"

#include "ui_editor/_ui_data/cpp/types.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace AE::UIEditor
{
	using namespace AE::Threading;
	using namespace AE::Graphics;

	UIEditorCore::SharedConfig  UIEditorCore::config;

namespace
{
	static constexpr auto&	RTech	= RenderTechs::ImGui_RTech;
	static constexpr auto&	IA		= InputActions::Main_UI;

/*
=================================================
	GetAppConfig
=================================================
*/
	static AppV1::AppConfig  GetAppConfig ()
	{
		AppV1::AppConfig	cfg;

		// threading
		{
			cfg.threading.maxThreads			= 1;
			cfg.threading.maxIOAccessThreads	= 1;
			cfg.threading.mask					= { EThread::PerFrame, EThread::Renderer, EThread::Background, EThread::FileIO };
		}

		// graphics
		{
			cfg.graphics.maxFrames				= 2;

			cfg.graphics.device.appName			= "UI Editor";
			cfg.graphics.device.requiredQueues	= EQueueMask::Graphics;
			cfg.graphics.device.optionalQueues	= Default;
			cfg.graphics.device.validation		= EDeviceValidation::Enabled;
		//	cfg.graphics.device.devFlags		= EDeviceFlags::SetStableClock;

			cfg.graphics.swapchain.colorFormat	= EPixelFormat::RGBA8_UNorm;
			cfg.graphics.swapchain.colorSpace	= EColorSpace::sRGB_nonlinear;
			cfg.graphics.swapchain.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;
			cfg.graphics.swapchain.options		= EImageOpt::BlitDst;
			cfg.graphics.swapchain.presentMode	= EPresentMode::FIFO;
			cfg.graphics.swapchain.minImageCount= 2;

			cfg.graphics.useRenderGraph			= true;
		}

		// window
		{
			cfg.window.title	= "UI Editor";
			cfg.window.size		= {1600, 896};
			cfg.window.mode		= EWindowMode::Resizable;
		}

		return cfg;
	}

} // namespace
//-----------------------------------------------------------------------------


	//
	// Draw Task
	//
	class UIEditorCore::ImGuiDrawTask final : public RenderTask
	{
	// variables
	private:
		RC<UIEditorCore>				core;
		IOutputSurface &				surface;
		EditorUISync::WriteNoLock_t		ui;
		const bool						isFirst;
		RC<UIScreen>					uiScreen;

		inline static const float		wnd_step = 20.f;


	// methods
	public:
		ImGuiDrawTask (UIEditorCore* core, Ptr<IOutputSurface> surf, RC<UIScreen> screen, CommandBatchPtr batch, DebugLabel) __NE___ :
			RenderTask{ batch, {"ImGui::Draw", HtmlColor::Aqua} },
			core{ core }, surface{ *surf },
			ui{ core->_imguiSync.WriteNoLock() },
			isFirst{ screen == null },
			uiScreen{ RVRef(screen) }
		{}

		void  Run () __Th_OV;

	private:
		void  _Update ();
		void  _UpdateMain (OUT float2 &wnd_pos);
		void  _UpdateEditorTab ();

		void  _RecursiveVisitFolder (const Path &rootPath, const ScriptFolder &);
		void  _LoadScript (const Path &rootPath);

		static void  PushStyleColor (ImGuiCol idx, RGBA8u color)
		{
			ImGui::PushStyleColor( idx, AE::Base::BitCast<ImU32>(color) );
		}
	};

/*
=================================================
	ImGuiDrawTask::Run
=================================================
*/
	void  UIEditorCore::ImGuiDrawTask::Run ()
	{
		EXLOCK( ui );
		core->_CheckScriptDir( ui->scriptDir );

		DirectCtx::Transfer		copy_ctx {*this};
		CHECK_TE( ui->imgui.Upload( copy_ctx.GetBaseContext() ));

		IOutputSurface::RenderTargets_t		targets;
		CHECK_TE( surface.GetTargets( OUT targets ));

		const auto&		rt = targets[0];

		DirectCtx::Graphics		gfx_ctx { *this, copy_ctx.ReleaseCommandBuffer() };
		{
			gfx_ctx.AddSurfaceTargets( targets );

			auto	draw_ctx = gfx_ctx.BeginRenderPass( RenderPassDesc{ *ui->rtech, RTech.Main, rt.RegionSize() }
										.AddViewport( rt.RegionSize() )
										.AddTarget( RTech.Main.att_Color, rt.viewId, (isFirst ? EResourceState::Invalidate : Default), Default ),
									{DbgName(), DbgColor()} );

			if ( isFirst )
				draw_ctx.ClearAttachment( RTech.Main.att_Color, RGBA32f{0.f}, rt.region, rt.layer, 1 );

			CHECK_TE( ui->imgui.Render( draw_ctx.GetBaseContext(), rt, [this](){ _Update(); }));

			gfx_ctx.EndRenderPass( draw_ctx );
		}

		Execute( gfx_ctx );
	}

/*
=================================================
	ImGuiDrawTask::_Update
=================================================
*/
	void  UIEditorCore::ImGuiDrawTask::_Update ()
	{
		ImGuiRenderer::AEStyleScope		ae_style {ui->imgui.GetContext()};

		float2		wnd_pos	{0.f};
		_UpdateMain( OUT wnd_pos );

		if ( ui->reloadScript )
		{
			ui->reloadScript = false;
			_LoadScript( ui->scriptName );
		}
	}

/*
=================================================
	ImGuiDrawTask::_UpdateMain
=================================================
*/
	void  UIEditorCore::ImGuiDrawTask::_UpdateMain (OUT float2 &wnd_pos)
	{
		ImGui::SetNextWindowSizeConstraints( ImVec2{370.f, 400.f}, ImGui::GetIO().DisplaySize );

		const auto	wnd_flags	= ImGuiWindowFlags_NoSavedSettings;

		if ( ImGui::Begin( ("UI Editor: "s << ui->scriptName << " ##UIEditor").c_str(), null, wnd_flags ))
		{
			wnd_pos.x = ImGui::GetWindowPos().x + ImGui::GetWindowWidth() + wnd_step;
			wnd_pos.y = ImGui::GetWindowPos().y;

			if ( ImGui::BeginTabBar( "ResEditorTabs", ImGuiTabBarFlags_None ))
			{
				const ImGuiTabItemFlags		tab_scripts_flags	= (ui->activeTab == 0 ? ImGuiTabItemFlags_SetSelected : 0);
				const ImGuiTabItemFlags		tab_editor_flags	= (ui->activeTab == 1 ? ImGuiTabItemFlags_SetSelected : 0);
			//	const ImGuiTabItemFlags		tab_graphics_flags	= (ui->activeTab == 2 ? ImGuiTabItemFlags_SetSelected : 0);
				ui->activeTab = UMax;

				// script files tree view
				if ( ImGui::BeginTabItem( "Scripts", null, tab_scripts_flags ))
				{
					ImGui::BeginChild( "Scripts", ImVec2{ImGui::GetContentRegionAvail().x, 0.f}, true, ImGuiWindowFlags_HorizontalScrollbar );

					_RecursiveVisitFolder( Path{}, ui->scriptDir.rootInfo );

					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				if ( ImGui::BeginTabItem( "Editor", null, tab_editor_flags ))
				{
					_UpdateEditorTab();
					ImGui::EndTabItem();
				}

				/*if ( ImGui::BeginTabItem( "Graphics", null, tab_graphics_flags ))
				{
					_UpdateGraphicsTab();
					ImGui::EndTabItem();
				}*/
				ImGui::EndTabBar();
			}
		}
		ImGui::End();
	}

/*
=================================================
	ImGuiDrawTask::_UpdateEditorTab
=================================================
*/
	void  UIEditorCore::ImGuiDrawTask::_UpdateEditorTab ()
	{
		auto&	cfg	= UIEditorCore::config;

		ImGui::NewLine();

		if ( not cfg.dbgLayouts.load() )
		{
			ImGuiRenderer::AEStyleScope_StartBtn	style {ui->imgui.GetContext()};

			if ( ImGui::Button( "Enable Debug Layouts" ))
				cfg.dbgLayouts.store( true );
		}
		else
		{
			ImGuiRenderer::AEStyleScope_StopBtn		style {ui->imgui.GetContext()};

			if ( ImGui::Button( "Disable Debug Layouts" ))
				cfg.dbgLayouts.store( false );
		}

		if ( not uiScreen )
			return;

		auto*	ui_style = uiScreen->GetStyleCollection();
		if ( ui_style == null )
			return;

		{
			float	anim_time	= 1.f / ui_style->GetSettings().colorAnimSpeed;
			String	value_str	= ToString(anim_time, 2) << "s";

			if ( ImGui::SliderFloat( "Anim time", INOUT &anim_time, 0.01f, 1.f, value_str.c_str() ))
				UI::StyleCollection::UnsafeSetter::SetAnimSpeed( *ui_style, 1.f/anim_time );
		}
	}

/*
=================================================
	ImGuiDrawTask::_LoadScript
=================================================
*/
	void  UIEditorCore::ImGuiDrawTask::_LoadScript (const Path &rootPath)
	{
		ui->activeTab	= 1;

		Path	path = ui->scriptDir.root;
		path /= rootPath;
		path.replace_extension( "as" );

		Scheduler().Run(
			ETaskQueue::Background,
			[] (RC<UIEditorCore> core, Path inPath) -> CoroTask
			{
				Unused( core->RunUIScriptAsync( inPath ));
				co_return;
			}
			( core, path ),
			Tuple{},
			"UI::LoadScript"
		);
	}

/*
=================================================
	ImGuiDrawTask::_RecursiveVisitFolder
=================================================
*/
	void  UIEditorCore::ImGuiDrawTask::_RecursiveVisitFolder (const Path &rootPath, const ScriptFolder &dir)
	{
		const ImGuiTreeNodeFlags	base_flags	=	ImGuiTreeNodeFlags_OpenOnArrow		| //ImGuiTreeNodeFlags_OpenOnDoubleClick |
													ImGuiTreeNodeFlags_SpanAvailWidth	| ImGuiTreeNodeFlags_SpanFullWidth;

		auto	node_flags	= base_flags;
		usize	node_id		= dir.baseId;

		if ( not dir.name.empty() )
		{
			bool	node_open = ImGui::TreeNodeEx( BitCast<void*>(node_id), node_flags, "%s", dir.name.c_str() );
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

			if ( node_id == ui->nodeClicked )
				node_flags |= ImGuiTreeNodeFlags_Selected;

			ImGui::TreeNodeEx( BitCast<void*>(node_id), node_flags, "%s", script.c_str() );

			if ( ImGui::IsItemClicked() and not ImGui::IsItemToggledOpen() )
			{
				ui->nodeClicked	= node_id;
				ui->scriptName	= ToString( rootPath / dir.name / script );

				_LoadScript( rootPath / dir.name / script );
			}
		}

		if ( not dir.name.empty() )
			ImGui::TreePop();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	UIEditorApplication::UIEditorApplication () __NE___ :
		AppCoreV1{ GetAppConfig(), MakeRC<UIEditorCore>() }
	{
		if ( not FileSystem::SetCurrentPath( AE_RES_FOLDER ))
			FileSystem::FindAndSetCurrent( "samples/ui_editor", 5 );
	}

/*
=================================================
	OnStart
=================================================
*/
	void  UIEditorApplication::OnStart (IApplication &app) __NE___
	{
		auto&	core = RefCast<UIEditorCore>(GetBaseApp());
		CHECK_FATAL( core.LoadInputActions() );

		AppCoreV1::OnStart( app );

		CHECK_FATAL( _OnStartImpl( app ));

		CHECK_FATAL( core.InitVFS() );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	UIEditorCore::UIEditorCore () __NE___ :
		_script{ new ScriptExe{} }
	{}

	UIEditorCore::~UIEditorCore () __NE___
	{
		_mainLoop.Write( Default );

		GraphicsScheduler().GetResourceManager().ReleaseResources( _dummyImage.view, _dummyImage.image );

		if ( _uploadMngr )
			_uploadMngr->Deinitialize();
	}

/*
=================================================
	LoadInputActions
=================================================
*/
	bool  UIEditorCore::LoadInputActions ()
	{
		// load input actions
		_inputActionsData = MakeRC<ArrayRStream>();

		FileRStream		file {Path{"controls.bin"}};		// TODO: use VFS
		CHECK_ERR( file.IsOpen() );
		CHECK_ERR( _inputActionsData->LoadRemainingFrom( file ));

		return true;
	}

/*
=================================================
	InitVFS
=================================================
*/
	bool  UIEditorCore::InitVFS ()
	{
		CHECK_ERR( _script->InitVFS() );

		GetVFS().MakeImmutable();
		return true;
	}

/*
=================================================
	RunUIScriptAsync
----
	should be used in background thread
=================================================
*/
	bool  UIEditorCore::RunUIScriptAsync (const Path &scriptPath)
	{
		Ptr<IOutputSurface>	output = _mainLoop->output;
		CHECK_ERR( output );

		auto	screen_ctor = _script->Run( scriptPath, *output );
		if ( not screen_ctor )
			return false;

		_pendingUIScreen.Write( RVRef(screen_ctor) );
		return true;
	}

/*
=================================================
	_InitInputActions
=================================================
*/
	void  UIEditorCore::_InitInputActions (IInputActions &ia)
	{
		MemRefRStream	stream{ _inputActionsData->GetData() };

		CHECK( ia.LoadSerialized( stream ));

		CHECK( ia.SetMode( IA ));
	}

/*
=================================================
	OnSurfaceCreated
=================================================
*/
	bool  UIEditorCore::OnSurfaceCreated (IWindow &wnd) __NE___
	{
		if ( _initialized.load() )
			return true;

		_uploadMngr = MakeRC<ResourceUploadManager>();

		// create dummy resource
		{
			auto&	res_mngr = GraphicsScheduler().GetResourceManager();

			_dummyImage.image = res_mngr.CreateImage( ImageDesc{}
														.SetDimension( 1,1 )
														.SetFormat( EPixelFormat::RGBA8_UNorm )
														.SetUsage( EImageUsage::TransferDst | EImageUsage::Sampled ));
			CHECK_ERR( _dummyImage.image );

			_dummyImage.view = res_mngr.CreateImageView( ImageViewDesc{}, _dummyImage.image );
			CHECK_ERR( _dummyImage.view );

			CHECK_ERR( _uploadMngr->FirstTransition( _dummyImage.image, EResourceState::Invalidate, EResourceState::ShaderSample | EResourceState::FragmentShader ));
		}

		auto&	output = wnd.GetSurface();

		if ( _LoadPipelinePack( output ))
		{
			_initialized.store( true );
			return true;
		}
		return false;
	}

/*
=================================================
	_LoadPipelinePack
=================================================
*/
	bool  UIEditorCore::_LoadPipelinePack (IOutputSurface &output)
	{
		CHECK_ERR( output.IsInitialized() );

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		auto	rp_info = output.GetRenderPassInfo();
		CHECK_ERR( rp_info.attachments.size() == 1 );
		CHECK_ERR( rp_info.attachments[0].samples == 1_samples );

		StringView	fname;
		switch_enum( res_mngr.GetDevice().GetGraphicsAPI() )
		{
			case EGraphicsAPI::Vulkan :		fname = "vk/ui_pipelines.bin";	break;
			case EGraphicsAPI::Metal :		fname = "mac/ui_pipelines.bin";	break;
		}
		switch_end

		auto	file = MakeRC<FileRStream>( NtStringView{fname} );
		CHECK_ERR( file->IsOpen() );

		PipelinePackDesc	desc;
		desc.stream			= file;
		desc.options		= EPipelinePackOpt::All;
		desc.surfaceFormat	= rp_info.attachments[0].format;
		desc.dbgName		= "editor ui pack";

		CHECK_ERR( desc.surfaceFormat != Default );

		auto	pack_id = res_mngr.LoadPipelinePack( desc );
		CHECK_ERR( res_mngr.InitializeResources( RVRef(pack_id) ));

		auto	rtech = res_mngr.LoadRenderTech( Default, RTech );
		CHECK_ERR( rtech );

		_imguiSync->rtech = rtech;

		CHECK_ERR( _imguiSync->imgui.Initialize( null, rtech,
									 {Tuple{ EPixelFormat::SwapchainColor, RTech.Main, RTech.Main.imgui }} ));

		return true;
	}

/*
=================================================
	StartRendering
=================================================
*/
	void  UIEditorCore::StartRendering (Ptr<IInputActions> input, Ptr<IOutputSurface> output, EWndState state) __NE___
	{
		ASSERT( bool{input} == bool{output} );

		const bool	focused	= (state == EWndState::Focused);
		bool		ia_changed;

		{
			auto	main_loop = _mainLoop.WriteLock();

			if ( not focused and main_loop->output != null )
				return;

			ia_changed = (main_loop->input != input) and (input != null);

			main_loop->input  = input;
			main_loop->output = output;
		}

		if ( ia_changed )
			_InitInputActions( *input );
	}

/*
=================================================
	StopRendering
=================================================
*/
	void  UIEditorCore::StopRendering (Ptr<IOutputSurface> output) __NE___
	{
		auto	main_loop = _mainLoop.WriteLock();

		if ( output == null or main_loop->output == output )
			main_loop->output = null;
	}

/*
=================================================
	WaitFrame
=================================================
*/
	void  UIEditorCore::WaitFrame (const Threading::EThreadArray	&threadMask,
								   Ptr<IWindow>						,
								   Ptr<IVRDevice>					) __NE___
	{
		CHECK( GraphicsScheduler().WaitNextFrame( threadMask, AE::DefaultTimeout ));
	}

/*
=================================================
	RenderFrame
=================================================
*/
	void  UIEditorCore::RenderFrame () __NE___
	{
		Ptr<IInputActions>		input;
		Ptr<IOutputSurface>		output;
		auto&					rg		= *GraphicsScheduler().GetRenderGraphPtr();

		{
			auto	main_loop = _mainLoop.ReadLock();

			if ( main_loop->input == null				or
				 main_loop->output == null				or
				 not main_loop->output->IsInitialized() )
				return;

			input	= main_loop->input;
			output	= main_loop->output;
		}

		const auto	frame_id = rg.GetFrameId();
		CHECK_ERRV( rg.BeginFrame() );

		if ( SharedPtr<UIScreen::Ctor> screen_ctor = RVRef(_pendingUIScreen).Extract() )
		{
			if ( _uiScreen )
				_uiScreen->Deinitialize();

			_uiScreen = screen_ctor->Create( *_uploadMngr, _dummyImage.view );
		}

		AsyncTask	proc_input;

		if ( input->GetMode() == IA )
		{
			proc_input = Scheduler().Run( ETaskQueue::PerFrame,
										  _ProcessInput2( input, GetRC<UIEditorCore>(), input->ReadInput( frame_id )),
										  Tuple{}, "Core::ProcessInput" );
		}else
		if ( _uiScreen )
		{
			proc_input = Scheduler().Run( ETaskQueue::PerFrame,
										  _ProcessInput1( input, _uiScreen, input->ReadInput( frame_id )),
										  Tuple{}, "Core::ProcessInput" );
		}

		AsyncTask	submit;
		{
			auto	batch	= rg.CmdBatch( EQueueType::Graphics, {"UI"} )
								.Begin();
			CHECK_ERRV( batch );

			AsyncTask	surf_acquire = rg.BeginOnSurface( output, batch );
			CHECK_ERRV( surf_acquire );

			AsyncTask	upload = _uploadMngr->UploadAsync( *batch.AsBatch(), 1 );

			AsyncTask	draw_screen;
			if ( _uiScreen )
				draw_screen = _uiScreen->Draw( batch, output, List{surf_acquire, proc_input} );

			AsyncTask	draw_ui;
			if ( input->GetMode() == IA )
			{
				draw_ui = batch.Task< ImGuiDrawTask >( Tuple{ this, output, _uiScreen }, {"EditorUI pass"} )
									.Run( Tuple{surf_acquire, proc_input} );
			}
			submit = batch.SubmitAsTask( Tuple{ draw_ui, draw_screen, upload });
		}

		Unused( rg.EndFrame( Tuple{submit} ));

		input->NextFrame( frame_id.Next() );
	}

/*
=================================================
	_ProcessInput
=================================================
*/
	CoroTask  UIEditorCore::_ProcessInput1 (TsInputActions input, RC<UIScreen> ui, ActionQueueReader reader)
	{
		bool	switch_mode = false;
		ui->ProcessInput( reader, OUT switch_mode );

		if_unlikely( switch_mode )
		{
			Scheduler().Run( ETaskQueue::Main,
							 _SetInputMode( input.Unsafe(), IA ),
							 Tuple{}, "Core::SetInputMode" );
		}
		co_return;
	}

	CoroTask  UIEditorCore::_ProcessInput2 (TsInputActions input, RC<UIEditorCore> core, ActionQueueReader reader)
	{
		auto	ui_sync		= core->_imguiSync.WriteLock();
		auto&	imgui		= ui_sync->imgui;
		bool	switch_mode = false;

		imgui.mouseLBDown	= false;
		imgui.mouseWheel	= {};

		ActionQueueReader::Header	hdr;
		for (; reader.ReadHeader( OUT hdr );)
		{
			switch_IA( hdr.name )
			{
				case IA.UI_MousePos :
					imgui.mousePos = reader.Data<packed_float2>( hdr.offset );		break;

				case IA.UI_MouseWheel :
					imgui.mouseWheel = reader.Data<packed_float2>( hdr.offset );	break;

				case IA.UI_MouseLBDown :
					imgui.mouseLBDown = true;										break;

				case IA.UI_ReloadScript :
					ui_sync->reloadScript = true;									break;

				case IA.SwitchInputMode :
					switch_mode = true;												break;

				case IA.UI_ShowHide :	break;	// TODO
			}
			switch_end
		}

		if_unlikely( switch_mode )
		{
			Scheduler().Run( ETaskQueue::Main,
							 _SetInputMode( input.Unsafe(), InputActions::Screen_UI ),
							 Tuple{}, "Core::SetInputMode" );
		}
		co_return;
	}

/*
=================================================
	_SetInputMode
=================================================
*/
	CoroTask  UIEditorCore::_SetInputMode (Ptr<IInputActions> input, InputModeName mode)
	{
		CHECK( input->SetMode( mode ));
		co_return;
	}

/*
=================================================
	_CheckScriptDir
=================================================
*/
	void  UIEditorCore::_CheckScriptDir (INOUT ScriptDirData &scriptDir)
	{
		if ( not scriptDir.timer.Tick() )
			return;

		scriptDir.rootInfo	= Default;

		usize	node_id = 0;
		_RecursiveCheckScriptDir( INOUT scriptDir.rootInfo, INOUT node_id, scriptDir.root, 0, scriptDir.maxDepth );
	}

	void  UIEditorCore::_RecursiveCheckScriptDir (INOUT ScriptFolder &rootDst, INOUT usize &nodeID, const Path &rootDir, uint depth, const uint maxDepth)
	{
		ASSERT( depth < maxDepth );

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

		rootDst.baseId = nodeID;

		// process files
		for (auto& dir : FileSystem::Enum( rootDir ))
		{
			if ( not dir.IsFile() )
				continue;

			if ( not IsAnsiString( dir.Get().filename().native() ))
				continue;

			const String	name = ToString( dir.Get().filename().native() );

			if ( EndsWith( name, ".as" ))
			{
				++nodeID;
				rootDst.scripts.push_back( name.substr( 0, name.length()-3 ));
			}
			//else
			//	AE_LOG_DBG( "Skip non-script file: '"s << ToString( dir.Get() ) << "'" );
		}
	}

} // AE::UIEditor
//-----------------------------------------------------------------------------


using namespace AE::Base;
using namespace AE::App;
using namespace AE::UIEditor;

#define REQUIRE_APACHE_2
#include "base/Defines/DetectLicense.inl.h"


Unique<IApplication::IAppListener>  AE_OnAppCreated ()
{
	StaticLogger::InitDefault();

	AE_LOG_DBG( "License: "s << AE_LICENSE );

	return MakeUnique<UIEditorApplication>();
}

void  AE_OnAppDestroyed ()
{
	StaticLogger::Deinitialize( True{"checkMemLeaks"} );
}
