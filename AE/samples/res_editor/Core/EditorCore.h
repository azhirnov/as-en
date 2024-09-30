// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Core/EditorUI.h"
#include "res_editor/Passes/Renderer.h"

namespace AE::ResEditor
{
	using AE::Networking::IpAddress;
	using AE::Networking::CSMessageGroupID;
	using AE::Networking::CSMessagePtr;
	using AE::Networking::EChannel;


	struct ResEditorAppConfig
	{
		using VFSPaths_t			= Array< Pair< Path, String >>;
		using NetVFS_t				= Array< Tuple< String, String, String >>;
		using ScreenshotPrefix_t	= Synchronized< RWSpinLock, String >;

		// VFS
		VFSPaths_t		vfsPaths;
		NetVFS_t		netVFS;

		// UI
		Path			uiDataFolder;

		// pipelines
		Array<Path>		pipelineSearchDirs;
		Array<Path>		pipelineIncludeDirs;

		// shaders
		Array<Path>		shaderSearchDirs;
		Array<Path>		shaderIncludeDirs;

		// script config
		Path			scriptFolder;
		Path			scriptCallableFolder;
		Array<Path>		scriptIncludeDirs;
		Path			cppTypesFolder;
		Path			scriptHeaderOutFolder;

		// output
		Path			shaderTraceFolder;
		Path			screenshotFolder;
		Path			videoFolder;
		Path			exportFolder;

		// graphics settings
		uint			screenWidth			= 1600;
		uint			screenHeight		= 900;
		bool			setStableGPUClock	= false;
		bool			enableRenderDoc		= false;

		// remote graphics device
		ubyte4			ipAddress		{0};
		String			graphicsLibPath;

		// remote input
		ushort			remoteIAPort	= 0;

		// tests
		Array<Path>		testFolders;
		Path			testOutput;

		// other
		mutable ScreenshotPrefix_t	screenshotPrefix;


		ResEditorAppConfig ()									= default;
		ResEditorAppConfig (ResEditorAppConfig &&)				= default;
		ResEditorAppConfig (const ResEditorAppConfig &)			= default;
		ResEditorAppConfig&  operator = (ResEditorAppConfig &&)	= default;

		ND_ static ResEditorAppConfig const&  Get ();
	};



	//
	// Resource Editor Application
	//

	class ResEditorApplication final : public AppV1::AppCoreV1
	{
	// variables
	private:
		Ptr<IApplication>	_app;


	// methods
	public:
		ResEditorApplication ()				__NE___;
		~ResEditorApplication ()			__NE_OV;

		void  OnStart (IApplication &)		__NE_OV;
		void  OnStop  (IApplication &)		__NE_OV;

		ND_ auto  GetApp ()					C_NE___	{ return _app; }

	private:
		bool  _InitVFS ()					__NE___;

		ND_ ResEditorCore&	_Core ()		__NE___;
	};



	//
	// Resource Editor Core
	//

	class ResEditorCore final : public AppV1::IBaseApp
	{
		friend class ResEditorApplication;

	// types
	private:
		class ProcessInputTask;

		struct MainLoopData
		{
			Ptr<IInputActions>		input;		// lifetime is same as Window/VRDevice lifetime
			Ptr<IOutputSurface>		output;		// lifetime is same as Window/VRDevice lifetime
			RC<Renderer>			renderer;
		};
		using MainLoopDataSync	= Synchronized< SharedMutex, MainLoopData >;

		using MonitorSync		= Synchronized< SharedMutex, App::Monitor >;


		//
		// VFS Client
		//
		class VFSClient final : public Networking::BaseClient, public EnableRC<VFSClient>
		{
		// variables
		private:
			RC<IVirtualFileStorage>		_storage;
			FrameUID					_frameId;

		public:
			VFSClient ()	__NE___ : _frameId{FrameUID::Init(2)} {}
			~VFSClient ()	__NE_OV	{ _Deinitialize(); }

			ND_ bool		Init (const IpAddress &addr, StringView prefix);
			ND_ AsyncTask	Tick ();
			ND_ auto		Storage ()		{ return _storage; }
		};
		using VFSClients_t = Array< RC< VFSClient >>;


		//
		// Remote Input Server
		//
		class RemoteInputServer final : public Networking::BaseServer, public EnableRC<RemoteInputServer>
		{
		// types
		private:
			class _MsgProducer final : public Networking::SyncCSMessageProducer< InPlaceLinearAllocator< usize{4_Kb} >>
			{
			public:
				EnumSet<EChannel>  GetChannels ()	C_NE_OV	{ return {EChannel::Reliable}; }
			};

		// variables
		private:
			RemoteInputActions			_remoteIA;
			FrameUID					_frameId;
			StaticRC<_MsgProducer>		_msgProducer;

		// methods
		public:
			RemoteInputServer ()									__NE___ : _frameId{FrameUID::Init(2)} {}
			~RemoteInputServer ()									__NE_OV	{ _Deinitialize(); }

			ND_ bool		Init (ushort port, IInputActions &ia)	__NE___;
			ND_ AsyncTask	Tick ()									__NE___;

			ND_ auto&		MsgProducer ()							__NE___	{ return *_msgProducer; }
		};

		enum class ETestStatus : uint
		{
			Load,
			Upload,
			Screenshot,
			Complete,
		};


	// variables
	private:
		MainLoopDataSync			_mainLoop;

		EditorUI					_ui;
		Unique<ScriptExe>			_script;

		Ptr<IWindow>				_window;
		MonitorSync					_monitor;

		RC<ArrayRStream>			_inputActionsData;
		RC<RemoteInputServer>		_remoteIAServer;

		Unique<RenderGraphImpl>		_rg;

		VFSClients_t				_vfsClients;

		struct {
			RingBuffer<Path>			scripts;				// main thread only
			Atomic<bool>				isActive		{false};
			Atomic<int>					framesToSwitch	{0};
			Atomic<int>					framesToCapture	{0};	// for video
			Atomic<ETestStatus>			status			{ETestStatus::Complete};
		}							_test;


	// methods
	public:
		ResEditorCore ();
		~ResEditorCore ();


	// API for EditorUI
	public:
		ND_ bool  RunRenderScriptAsync (const Path &);


	// API for ResEditorWindow
	public:
		ND_ bool  OnStart ();
	private:
		ND_	bool  _LoadInputActions ();


	// main loop
	private:
		ND_ static CoroTask  _ProcessInput (TsInputActions input, RC<Renderer> renderer, Ptr<EditorUI> ui, ActionQueueReader reader);
		ND_ static CoroTask  _SetInputMode (Ptr<IInputActions> input, InputModeName mode);

		void  _UpdateTests (RC<Renderer> renderer);


	// IBaseApp //
	private:
		bool  OnSurfaceCreated (IWindow &)											__NE_OV;
		void  StartRendering (Ptr<IInputActions>, Ptr<IOutputSurface>, EWndState)	__NE_OV;
		void  StopRendering (Ptr<IOutputSurface>)									__NE_OV;

		void  RenderFrame ()														__NE_OV;
		void  WaitFrame (const Threading::EThreadArray	&threadMask,
						 Ptr<IWindow>					window,
						 Ptr<IVRDevice>					vrDevice)					__NE_OV;

		void  _InitInputActions (IInputActions &)									__NE___;
	};


} // AE::ResEditor
