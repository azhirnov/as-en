// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Core/UIScreen.h"
#include "Core/ScriptExe.h"

namespace AE::UIEditor
{

	//
	// UI Editor Application
	//

	class UIEditorApplication final : public AppV1::AppCoreV1
	{
	// methods
	public:
		UIEditorApplication ()				__NE___;

		void  OnStart (IApplication &)		__NE_OV;
	};



	//
	// UI Editor Core
	//

	class UIEditorCore final : public AppV1::IBaseApp
	{
	// types
	private:
		class ImGuiDrawTask;

		struct MainLoopData
		{
			Ptr<IInputActions>		input;		// lifetime is same as Window/VRDevice lifetime
			Ptr<IOutputSurface>		output;		// lifetime is same as Window/VRDevice lifetime
		};
		using MainLoopSync_t	= Threading::DRCSynchronized< MainLoopData >;


		struct ScriptFolder
		{
			String							name;
			usize							baseId	= UMax;		// ID for selection
			Array< Unique<ScriptFolder> >	folders;
			Array< String >					scripts;
		};

		struct ScriptDirData
		{
			Timer				timer;
			const uint			maxDepth	{3};
			const Path			root;
			ScriptFolder		rootInfo;

			explicit ScriptDirData (Path path) : root{RVRef(path)} { timer.StartSignaled(seconds{10}); }
		};

		struct EditorUIData
		{
			ImGuiRenderer			imgui;
			RenderTechPipelinesPtr	rtech;
			ScriptDirData			scriptDir;
			bool					reloadScript	= false;
			String					scriptName;
			uint					activeTab		= UMax;
			usize					nodeClicked		= UMax;

			EditorUIData () : scriptDir{AE_UI_SCRIPT_FOLDER} {}
		};
		using EditorUISync = Synchronized< RWSpinLock, EditorUIData >;

	public:
		struct SharedConfig
		{
			Atomic<bool>			dbgLayouts		{false};
		};

		using UIScreenCtorSync = Synchronized< Mutex, SharedPtr<UIScreen::Ctor> >;


	// variables
	private:
		MainLoopSync_t					_mainLoop;

		Unique<ScriptExe>				_script;
		RC<UIScreen>					_uiScreen;
		UIScreenCtorSync				_pendingUIScreen;
		EditorUISync					_imguiSync;

		RC<ArrayRStream>				_inputActionsData;
		RC<ResourceUploadManager>		_uploadMngr;
		StrongImageAndViewID			_dummyImage;

		Atomic<bool>					_initialized	{false};

	public:
		static SharedConfig	config;


	// methods
	public:
		UIEditorCore ()																__NE___;
		~UIEditorCore ()															__NE___;

		ND_ bool  InitVFS ();
		ND_	bool  LoadInputActions ();
		ND_ bool  RunUIScriptAsync (const Path &);

	private:
		ND_ bool  _LoadPipelinePack (IOutputSurface &);

		ND_ static AsyncCoro  _ProcessInput1 (TsInputActions input, RC<UIScreen> ui, ActionQueueReader reader);
		ND_ static AsyncCoro  _ProcessInput2 (TsInputActions input, RC<UIEditorCore> core, ActionQueueReader reader);
		ND_ static AsyncCoro  _SetInputMode (Ptr<IInputActions> input, InputModeName mode);

		static void  _CheckScriptDir (INOUT ScriptDirData &);
		static void  _RecursiveCheckScriptDir (INOUT ScriptFolder &dst, INOUT usize &nodeID, const Path &dir, uint depth, uint maxDepth);


	// IBaseApp (main thread) //
	private:
		bool  OnSurfaceCreated (IWindow &)											__NE_OV;
		void  StartRendering (Ptr<IInputActions>, Ptr<IOutputSurface>, EWndState)	__NE_OV;
		void  StopRendering (Ptr<IOutputSurface>)									__NE_OV;
		void  RenderFrame ()														__NE_OV;
		void  WaitFrame (const Threading::EThreadArray	&threadMask,
						 Ptr<IWindow>					window)						__NE_OV;

		void  _InitInputActions (IInputActions &);
	};


} // AE::UIEditor
