// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "vfs/Disk/DiskStaticStorage.h"
#include "vfs/Disk/DiskDynamicStorage.h"
#include "vfs/Network/Messages.h"

#include "res_editor/Core/EditorCore.h"
#include "res_editor/Scripting/ScriptExe.h"

AE_DECL_SCRIPT_OBJ(	AE::ResEditor::ResEditorAppConfig,	"Config" );

#if AE_PUBLIC_VERSION and AE_GRAPHICS_STRONG_VALIDATION == 0
# error AE_GRAPHICS_STRONG_VALIDATION must be enabled for public version
#endif

#if defined(AE_ENABLE_VULKAN) and not defined(AE_CFG_RELEASE)
# define ENABLE_RDC		1
#else
# define ENABLE_RDC		0
#endif

namespace AE::ResEditor
{
	using namespace AE::Threading;

namespace
{
	static ResEditorAppConfig		s_REConfig;
	static constexpr auto			c_WindowMode	= EWindowMode::Resizable;

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
			cfg.threading.maxThreads			= 2;
			cfg.threading.maxIOAccessThreads	= 1;
			cfg.threading.mask					= { EThread::PerFrame, EThread::Renderer, EThread::Background, EThread::FileIO };
		}

		// graphics
		{
			cfg.graphics.maxFrames = 2;

			cfg.graphics.device.appName			= "ResourceEditor";
			cfg.graphics.device.requiredQueues	= EQueueMask::Graphics;
			cfg.graphics.device.optionalQueues	= Default;
			cfg.graphics.device.devFlags		= (s_REConfig.setStableGPUClock ? EDeviceFlags::SetStableClock : Default) |
												  (s_REConfig.enableRenderDoc ? EDeviceFlags::EnableRenderDoc : Default) |
												  EDeviceFlags::EnablePerfCounters;

		  #if AE_PUBLIC_VERSION and defined(AE_RELEASE)
			cfg.graphics.device.validation		= EDeviceValidation::Disabled;
		  #else
			cfg.graphics.device.validation		= EDeviceValidation::Enabled;
		  #endif

			cfg.graphics.swapchain.colorFormat	= EPixelFormat::RGBA8_UNorm;

			cfg.graphics.swapchain.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;
			cfg.graphics.swapchain.options		= EImageOpt::BlitDst;
			cfg.graphics.swapchain.presentMode	= EPresentMode::FIFO;		// vsync
			cfg.graphics.swapchain.minImageCount= 2;

			cfg.graphics.useRenderGraph			= true;
		}

		// window
		{
			cfg.window.title	= "ResourceEditor";
			cfg.window.size		= {1600, 900};
			cfg.window.mode		= c_WindowMode;
		}

		// VR
		{
			cfg.enableVR		= false;
			cfg.vr.dimension	= uint2{2048};
			cfg.vr.format		= EPixelFormat::BGRA8_UNorm;
			cfg.vr.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Transfer;	// default
			cfg.vr.options		= EImageOpt::BlitDst;
		}

		cfg.enableNetwork = true;

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		cfg.window.mode						= EWindowMode::NonResizable;
		cfg.graphics.maxFrames				= 2;
		cfg.graphics.swapchain.minImageCount= 2;
		cfg.graphics.graphicsLibPath		= s_REConfig.graphicsLibPath;
		cfg.graphics.enableSyncLog			= false;

		cfg.graphics.deviceAddr	= Networking::IpAddress::FromInt( s_REConfig.ipAddress[0], s_REConfig.ipAddress[1], s_REConfig.ipAddress[2], s_REConfig.ipAddress[3], 0 );
		CHECK_THROW_MSG( cfg.graphics.deviceAddr.IsValid(),
			"Invalid RemoveDevice IP address, in 'res_editor_cfg.as' set 'cfg.RemoteDeviceIpAddress(...)' to an existing IP address" );
	  #endif

		CHECK( cfg.graphics.maxFrames <= cfg.graphics.swapchain.minImageCount );
		return cfg;
	}

/*
=================================================
	ResEditorAppConfig_VFSPath
=================================================
*/
	static void  ResEditorAppConfig_VFSPath (ResEditorAppConfig &self, const String &path, const String &prefix)
	{
		CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
			"VFSPath '"s << ToString(path) << "' must be existed folder" );

		self.vfsPaths.emplace_back( FileSystem::ToAbsolute( Path{path} ), prefix );
	}

	static void  ResEditorAppConfig_MakeVFSPath (ResEditorAppConfig &self, const String &path, const String &prefix)
	{
		FileSystem::CreateDirectories( path );
		ResEditorAppConfig_VFSPath( self, path, prefix );
	}

/*
=================================================
	ResEditorAppConfig_NetVFS
=================================================
*/
	static void  ResEditorAppConfig_NetVFS (ResEditorAppConfig &self, const String &host, const String &service, const String &prefix)
	{
		self.netVFS.emplace_back( host, service, prefix );
	}

/*
=================================================
	ResEditorAppConfig_UIDataDir
=================================================
*/
	static void  ResEditorAppConfig_UIDataDir (ResEditorAppConfig &self, const String &path)
	{
		if ( not FileSystem::IsDirectory( path ))
		{
			CHECK_THROW_MSG( FileSystem::CreateDirectories( path ),
				"Failed to create folder '"s << ToString(path) << "'" );
		}

		CHECK_THROW( self.uiDataFolder.empty() );
		self.uiDataFolder = FileSystem::ToAbsolute( Path{path} );
	}

/*
=================================================
	ResEditorAppConfig_PipelineSearchDir
=================================================
*/
	static void  ResEditorAppConfig_PipelineSearchDir (ResEditorAppConfig &self, const String &path)
	{
		CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
			"PipelineSearchDir '"s << ToString(path) << "' must be existed folder" );

		self.pipelineSearchDirs.push_back( FileSystem::ToAbsolute( Path{path} ));
	}

/*
=================================================
	ResEditorAppConfig_PipelineIncludeDir
=================================================
*/
	static void  ResEditorAppConfig_PipelineIncludeDir (ResEditorAppConfig &self, const String &path)
	{
		CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
			"PipelineIncludeDir '"s << ToString(path) << "' must be existed folder" );

		self.pipelineIncludeDirs.push_back( FileSystem::ToAbsolute( Path{path} ));
	}

/*
=================================================
	ResEditorAppConfig_ShaderSearchDir
=================================================
*/
	static void  ResEditorAppConfig_ShaderSearchDir (ResEditorAppConfig &self, const String &path)
	{
		CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
			"ShaderSearchDir '"s << ToString(path) << "' must be existed folder" );

		self.shaderSearchDirs.push_back( FileSystem::ToAbsolute( Path{path} ));
	}

/*
=================================================
	ResEditorAppConfig_ShaderIncludeDir
=================================================
*/
	static void  ResEditorAppConfig_ShaderIncludeDir (ResEditorAppConfig &self, const String &path)
	{
		CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
			"ShaderIncludeDir '"s << ToString(path) << "' must be existed folder" );

		self.shaderIncludeDirs.push_back( FileSystem::ToAbsolute( Path{path} ));
	}

/*
=================================================
	ResEditorAppConfig_ShaderTraceDir
=================================================
*/
	static void  ResEditorAppConfig_ShaderTraceDir (ResEditorAppConfig &self, const String &path)
	{
		if ( not FileSystem::IsDirectory( path ))
		{
			CHECK_THROW_MSG( FileSystem::CreateDirectories( path ),
				"Failed to create folder '"s << ToString(path) << "'" );
		}

		CHECK_THROW( self.shaderTraceFolder.empty() );
		self.shaderTraceFolder = FileSystem::ToAbsolute( Path{path} );
	}

/*
=================================================
	ResEditorAppConfig_ScreenshotDir
=================================================
*/
	static void  ResEditorAppConfig_ScreenshotDir (ResEditorAppConfig &self, const String &path)
	{
		if ( not FileSystem::IsDirectory( path ))
		{
			CHECK_THROW_MSG( FileSystem::CreateDirectories( path ),
				"Failed to create folder '"s << ToString(path) << "'" );
		}

		CHECK_THROW( self.screenshotFolder.empty() );
		self.screenshotFolder = FileSystem::ToAbsolute( Path{path} );
	}

/*
=================================================
	ResEditorAppConfig_VideoDir
=================================================
*/
	static void  ResEditorAppConfig_VideoDir (ResEditorAppConfig &self, const String &path)
	{
		if ( not FileSystem::IsDirectory( path ))
		{
			CHECK_THROW_MSG( FileSystem::CreateDirectories( path ),
				"Failed to create folder '"s << ToString(path) << "'" );
		}

		CHECK_THROW( self.videoFolder.empty() );
		self.videoFolder = FileSystem::ToAbsolute( Path{path} );
	}

/*
=================================================
	ResEditorAppConfig_ExportDir
=================================================
*/
	static void  ResEditorAppConfig_ExportDir (ResEditorAppConfig &self, const String &path)
	{
		if ( not FileSystem::IsDirectory( path ))
		{
			CHECK_THROW_MSG( FileSystem::CreateDirectories( path ),
				"Failed to create folder '"s << ToString(path) << "'" );
		}

		CHECK_THROW( self.exportFolder.empty() );
		self.exportFolder = FileSystem::ToAbsolute( Path{path} );
	}

/*
=================================================
	ResEditorAppConfig_ScriptDir
=================================================
*/
	static void  ResEditorAppConfig_ScriptDir (ResEditorAppConfig &self, const String &path)
	{
		CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
			"ScriptDir '"s << ToString(path) << "' must be existed folder" );

		CHECK_THROW( self.scriptFolder.empty() );
		self.scriptFolder = FileSystem::ToAbsolute( Path{path} );
	}

/*
=================================================
	ResEditorAppConfig_CallableScriptDir
=================================================
*/
	static void  ResEditorAppConfig_CallableScriptDir (ResEditorAppConfig &self, const String &path)
	{
		CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
			"CallableScriptDir '"s << ToString(path) << "' must be existed folder" );

		CHECK_THROW( self.scriptCallableFolder.empty() );
		self.scriptCallableFolder = FileSystem::ToAbsolute( Path{path} );
	}

/*
=================================================
	ResEditorAppConfig_AddScriptIncludeDir
=================================================
*/
	static void  ResEditorAppConfig_AddScriptIncludeDir (ResEditorAppConfig &self, const String &path)
	{
		CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
			"ScriptIncludeDir '"s << path << "' must be existed folder" );

		self.scriptIncludeDirs.push_back( FileSystem::ToAbsolute( Path{path} ));
	}

/*
=================================================
	ResEditorAppConfig_SetRemoteDeviceIpAddress
=================================================
*/
	static void  ResEditorAppConfig_SetRemoteDeviceIpAddress (ResEditorAppConfig &self, uint p0, uint p1, uint p2, uint p3)
	{
		self.ipAddress[0] = ubyte(p0);
		self.ipAddress[1] = ubyte(p1);
		self.ipAddress[2] = ubyte(p2);
		self.ipAddress[3] = ubyte(p3);
	}

/*
=================================================
	ResEditorAppConfig_SetGraphicsLibPath
=================================================
*/
	static void  ResEditorAppConfig_SetGraphicsLibPath (ResEditorAppConfig &self, const String &path)
	{
		CHECK_THROW_MSG( FileSystem::IsFile( path ),
			"GraphicsLibPath '"s << path << "' is not exists" );

		self.graphicsLibPath = path;
	}

/*
=================================================
	ResEditorAppConfig_AddTestFolder
=================================================
*/
	static void  ResEditorAppConfig_AddTestFolder (ResEditorAppConfig &self, const String &inPath)
	{
		const auto	path = Path{self.scriptFolder} / inPath;

		CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
			"TestFolder '"s << ToString(path) << "' must be existed folder" );

		self.testFolders.push_back( FileSystem::ToAbsolute( path ));
	}

/*
=================================================
	ResEditorAppConfig_AddTestOutput
=================================================
*/
	static void  ResEditorAppConfig_AddTestOutput (ResEditorAppConfig &self, const String &path)
	{
		self.testOutput = FileSystem::ToAbsolute( Path{path} );
	}

/*
=================================================
	ResEditorAppConfig_SetRemoteInputServerPort
=================================================
*/
	static void  ResEditorAppConfig_SetRemoteInputServerPort (ResEditorAppConfig &self, uint port)
	{
		self.remoteIAPort = ushort(port);
	}

/*
=================================================
	_LoadResEditorAppConfigFromScript
=================================================
*/
	ND_ static bool  _LoadResEditorAppConfigFromScript (const Path &filename) __Th___
	{
		using namespace AE::Scripting;

		ScriptEnginePtr		se = MakeRC<ScriptEngine>();
		CHECK_THROW( se->Create() );

		CoreBindings::BindString( se );
		CoreBindings::BindArray( se );
		{
			ClassBinder<ResEditorAppConfig>		binder{ se };
			binder.CreateClassValue();
			binder.AddMethodFromGlobal( &ResEditorAppConfig_VFSPath,					"VFSPath",				{"path", "prefixInVFS"} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_MakeVFSPath,				"MakeVFSPath",			{"path", "prefixInVFS"} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_NetVFS,						"NetVFS",				{"host", "service", "prefixInVFS"} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_UIDataDir,					"UIDataDir",			{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_PipelineSearchDir,			"PipelineSearchDir",	{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_PipelineIncludeDir,			"PipelineIncludeDir",	{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_ShaderSearchDir,			"ShaderSearchDir",		{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_ShaderIncludeDir,			"ShaderIncludeDir",		{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_ScriptDir,					"ScriptDir",			{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_CallableScriptDir,			"CallableScriptDir",	{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_AddScriptIncludeDir,		"ScriptIncludeDir",		{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_ShaderTraceDir,				"ShaderTraceDir",		{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_ScreenshotDir,				"ScreenshotDir",		{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_VideoDir,					"VideoDir",				{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_ExportDir,					"ExportDir",			{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_SetRemoteDeviceIpAddress,	"RemoteDeviceIpAddress",{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_SetGraphicsLibPath,			"GraphicsLibPath",		{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_AddTestFolder,				"TestFolder",			{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_AddTestOutput,				"TestOutput",			{} );
			binder.AddMethodFromGlobal( &ResEditorAppConfig_SetRemoteInputServerPort,	"RemoteInputServerPort",{} );
			binder.AddProperty( &ResEditorAppConfig::setStableGPUClock,					"setStableGPUClock"		);
			binder.AddProperty( &ResEditorAppConfig::enableRenderDoc,					"enableRenderDoc"		);
		}

		ScriptEngine::ModuleSource	src;
		{
			FileRStream		file {filename};
			CHECK_ERR( file.IsOpen() );

			src.name			= ToString( filename.stem() );
			src.dbgLocation		= {};
			src.usePreprocessor	= false;
			CHECK_ERR( file.Read( file.RemainingSize(), OUT src.script ));
		}

		ScriptModulePtr		module = se->CreateModule( {src} );
		CHECK_ERR_MSG( module,
			"Failed to parse script '"s << ToString(filename) << "', fix errors or delete the file to allow the application to create a default script" );

		auto	fn = se->CreateScript< void (ResEditorAppConfig &) >( "main", module );
		CHECK_ERR_MSG( fn,
			"Script '"s << ToString(filename) << "' entry point 'main' is not exist" );

		ResEditorAppConfig	tmp;
		CHECK_ERR_MSG( fn->Run( OUT tmp ),
			"Failed to run script '"s << ToString(filename) << "', fix errors or delete the file to allow the application to create a default script" );

		s_REConfig = RVRef(tmp);
		return true;
	}

/*
=================================================
	_CreateDefaultResEditorAppConfig
=================================================
*/
	ND_ static bool  _CreateDefaultResEditorAppConfig (const Path &filename)
	{
		String	str;
		if ( AE_PUBLIC_VERSION )
		{
			str << R"(
void main (Config &out cfg)
{
	const string	vfs_path 			= "";
	const string	local_path			= "data/";
	const string	shader_data_path	= "shared_data/";
	const string	ui_path				= "ui";
)";
		}
		else
		{
			Path	engine_path;
			if ( FileSystem::SearchBackward( FileSystem::CurrentPath(), "AE/samples/res_editor", 3, OUT engine_path ))
			{
				ASSERT( engine_path.is_absolute() );
				engine_path = engine_path.parent_path().parent_path().parent_path();
			}

			String	path = ToString( engine_path );
			if ( not path.empty() and path.back() != '/' )
				path << '/';

			str <<
"void main (Config &out cfg)\n"
"{\n"
"	const string	base_path 			= \"" << path  << "\";\n"
"	const string	vfs_path 			= base_path + \"AE-Data/\";\n"
"	const string	local_path			= base_path + \"AE/samples/res_editor/_data/\";\n"
"	const string	shader_data_path	= base_path + \"AE/engine/shared_data/\";\n"
"	const string	ui_path				= base_path + \"AE-Temp/samples/res_editor\";\n";
		}

		str << R"(
	// VFS //
	//	attach path on disk to VFS
	cfg.VFSPath( vfs_path + "shadertoy_data",	"shadertoy/" );
	cfg.VFSPath( vfs_path + "res_editor_data",	"res/" );
	//	create directory and add as mutable file system
	cfg.MakeVFSPath( local_path + "../_export",	"export/" );
	//	connect to network file system
	//cfg.NetVFS( "localhost", "4000", "net/" );

	// pipeline dirs //
	//	where to search pipelines for 'UnifiedGeometry' and 'Model'.
	cfg.PipelineSearchDir( local_path + "pipelines" );
	cfg.PipelineIncludeDir( local_path + "pipeline_inc" );

	// shaders //
	//	where to search shaders for pipelines and passes.
	cfg.ShaderSearchDir( local_path + "shaders" );
	cfg.ShaderIncludeDir( shader_data_path + "shaders" );
	cfg.ShaderIncludeDir( local_path + "shaders" );

	// scripts //
	//	all files with '.as' extension will be added to script list in editor.
	cfg.ScriptDir( local_path + "scripts" );
	//	scripts which can be used directly and for 'RunScript()' call.
	cfg.CallableScriptDir( local_path + "scripts/callable" );
	//	scripts which can be included in other scripts.
	cfg.ScriptIncludeDir( local_path + "script_inc" );

	// output //
	//	path for imgui
	cfg.UIDataDir( ui_path );
	//	where to put shader traces
	cfg.ShaderTraceDir( local_path + "../_shader_trace" );
	//	where to save screenshots
	cfg.ScreenshotDir( local_path + "../_screenshots" );
	//	where to save video
	cfg.VideoDir( local_path + "../_video" );
	//	where to save export (images, models, scenes, etc)
	cfg.ExportDir( local_path + "../_export" );

	// graphics settings //
	//	NV only: set stable GPU clock for profiling, otherwise driver can move GPU to low power mode.
	cfg.setStableGPUClock = false;

	//	on start attach RenderDoc to the app, this will disable some new extensions.
	cfg.enableRenderDoc = false;

	// remote input //
	//cfg.RemoteInputServerPort( 0 );
)";

#ifdef AE_ENABLE_REMOTE_GRAPHICS
		str << R"(
	// remote graphics device //
	cfg.RemoteDeviceIpAddress( 192, 168, 0, 0 );
	//cfg.GraphicsLibPath( "" );
)";
#endif

		str << R"(
	// tests //
	//cfg.TestOutput( vfs_path + "/samples/res_editor/ref" );
	//cfg.TestFolder( "callable" );
	//cfg.TestFolder( "games" );
	//cfg.TestFolder( "samples-2d" );
	//cfg.TestFolder( "samples-3d" );
	//cfg.TestFolder( "samples-rt" );
	//cfg.TestFolder( "sphere" );
	//cfg.TestFolder( "tests" );
	//cfg.TestFolder( "tools" );
}
)";

		FileWStream		file {filename};
		return	file.IsOpen()				and
				file.Write( StringView{str} );
	}

/*
=================================================
	InitResEditorAppConfig
=================================================
*/
	static void  InitResEditorAppConfig () __NE___
	{
		const Path	path = FileSystem::CurrentPath() / "res_editor_cfg.as";

		if ( not FileSystem::IsFile( path ))
			CHECK_FATAL( _CreateDefaultResEditorAppConfig( path ));

		try{
			if ( not _LoadResEditorAppConfigFromScript( path ))
			{
				// recreate
				CHECK_FATAL( _CreateDefaultResEditorAppConfig( path ));
				CHECK_FATAL( _LoadResEditorAppConfigFromScript( path ));
			}

			if ( not AE_PUBLIC_VERSION )
			{
				s_REConfig.scriptHeaderOutFolder	= AE_SHARED_DATA "/scripts";
				s_REConfig.cppTypesFolder			= AE_LOCAL_DATA_FOLDER "/cpp";
			}
		}
		catch(...) {
			CHECK_FATAL( !"failed to run initial script" );
		}
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	ResEditorAppConfig::Get
=================================================
*/
	ResEditorAppConfig const&  ResEditorAppConfig::Get ()
	{
		return s_REConfig;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ResEditorApplication::ResEditorApplication () __NE___ :
		AppCoreV1{ GetAppConfig(), MakeRCTh<ResEditorCore>() }
	{
		auto&	re_cfg = ResEditorAppConfig::Get();

		// for imgui
		{
			const auto&		ui_path = re_cfg.uiDataFolder;

			CHECK_FATAL( FileSystem::IsDirectory( ui_path ));
			CHECK_FATAL( FileSystem::SetCurrentPath( ui_path ));	// TODO: use VFS ?
		}
		CHECK_FATAL( FileSystem::IsDirectory( re_cfg.scriptFolder ));
		CHECK_FATAL( _InitVFS() );
	}

/*
=================================================
	destructor
=================================================
*/
	ResEditorApplication::~ResEditorApplication () __NE___
	{
	}

/*
=================================================
	_InitVFS
=================================================
*/
	bool  ResEditorApplication::_InitVFS () __NE___
	{
		auto&	re_cfg = ResEditorAppConfig::Get();

		for (auto& [path, in_prefix] : re_cfg.vfsPaths)
		{
			ASSERT( not in_prefix.empty() );
			String	prefix = in_prefix;

			if ( prefix.empty() )			continue;
			if ( prefix.back() != '/' )		prefix << '/';

			CHECK_ERR( GetVFS().AddStorage( VFS::VirtualFileStorageFactory::CreateStaticFolder( path, prefix )));
		}

		for (auto& [host, service, in_prefix] : re_cfg.netVFS)
		{
			Networking::IpAddress	addr = Networking::IpAddress::FromServiceTCP( host, service );

			ASSERT( not in_prefix.empty() );
			String	prefix = in_prefix;

			if ( prefix.empty() )			continue;
			if ( prefix.back() != '/' )		prefix << '/';

			auto&	client = _Core()._vfsClients.emplace_back( MakeRC<ResEditorCore::VFSClient>() );
			CHECK_ERR( client->Init( addr, prefix ));
			CHECK_ERR( GetVFS().AddStorage( client->Storage() ));
		}

		if ( not re_cfg.exportFolder.empty() )
		{
			CHECK_ERR( GetVFS().AddStorage( VFS::StorageName{"export"},
										    VFS::VirtualFileStorageFactory::CreateDynamicFolder( re_cfg.exportFolder, "export/" )));
		}

		GetVFS().MakeImmutable();
		return true;
	}

/*
=================================================
	OnStart
=================================================
*/
	void  ResEditorApplication::OnStart (IApplication &app) __NE___
	{
		_app = &app;
		AppCoreV1::OnStart( app );

		CHECK_FATAL( Cast<ResEditorCore>(&GetBaseApp())->OnStart() );

		CHECK_FATAL( _OnStartImpl( app ));
	}

/*
=================================================
	OnStop
=================================================
*/
	void  ResEditorApplication::OnStop (IApplication &app) __NE___
	{
		AppCoreV1::OnStop( app );
		_app = null;
	}

/*
=================================================
	_Core
=================================================
*/
	ResEditorCore&  ResEditorApplication::_Core () __NE___
	{
		return RefCast<ResEditorCore>( GetBaseApp() );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	VFSClient::Init
=================================================
*/
	bool  ResEditorCore::VFSClient::Init (const Networking::IpAddress &addr, StringView prefix)
	{
		using namespace AE::Networking;

		auto	mf = MakeRC<Networking::MessageFactory>();
		CHECK_ERR( Networking::Register_NetVFS( *mf ));

		CHECK_ERR( _Initialize( RVRef(mf), MakeRC<DefaultServerProviderV1>( addr ), null, _frameId ));
		CHECK_ERR( _AddChannelReliableTCP( "VFS "s << prefix ));

		_storage = VFS::VirtualFileStorageFactory::CreateNetworkStorage( *this, prefix );
		CHECK_ERR( _storage );

		return true;
	}

/*
=================================================
	VFSClient::Tick
=================================================
*/
	AsyncTask  ResEditorCore::VFSClient::Tick ()
	{
		return	Scheduler().Run(
					ETaskQueue::Background,
					[] (RC<VFSClient> client) -> CoroTask
					{
						auto	stat = client->Update( client->_frameId );
						if ( stat )
							client->_frameId.Inc();

						co_return;
					}( GetRC() ),
					Tuple{},
					"VFS client tick" );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	RemoteInputServer::Init
=================================================
*/
	bool  ResEditorCore::RemoteInputServer::Init (ushort port, IInputActions &ia) __NE___
	{
		using namespace AE::Networking;

		_remoteIA.Init( ia );

		auto	mf = MakeRC<MessageFactory>();

		CHECK_ERR( Register_RemoteControl( *mf ));
		CHECK_ERR( _Initialize( RVRef(mf), MakeRC<DefaultClientListener>(), null, _frameId ));
		CHECK_ERR( _AddChannelReliableTCP( port, "RemoteInput" ));

		CHECK_ERR( Add( _msgProducer.GetRC() ));
		CHECK_ERR( Add( _remoteIA.GetMsgConsumer() ));

		return true;
	}

/*
=================================================
	RemoteInputServer::Tick
=================================================
*/
	AsyncTask  ResEditorCore::RemoteInputServer::Tick () __NE___
	{
		// in main thread
		{
			_remoteIA.EnableSensors( *_msgProducer );
		}

		return	Scheduler().Run(
					ETaskQueue::Background,
					[] (RC<RemoteInputServer> server) -> CoroTask
					{
						auto	stat = server->Update( server->_frameId );
						if ( stat )
							server->_frameId.Inc();

						co_return;
					}( GetRC() ),
					Tuple{},
					"IA Server tick" );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ResEditorCore::ResEditorCore () :
		_ui{ *this, ResEditorAppConfig::Get().scriptFolder }
	{}

/*
=================================================
	destructor
=================================================
*/
	ResEditorCore::~ResEditorCore ()
	{
	  #if ENABLE_RDC
		GraphicsScheduler().GetDevice().GetRenderDocApi().PrintCaptures();
	  #endif

		_mainLoop.Write( Default );
	}

/*
=================================================
	OnStart
=================================================
*/
	bool  ResEditorCore::OnStart ()
	{
		NOTHROW_ERR(
			_rg.reset( new RenderGraphImpl{} );

			auto&				re_cfg = ResEditorAppConfig::Get();
			ScriptExe::Config	cfg;
			cfg.cppTypesFolder			= re_cfg.cppTypesFolder;
			cfg.scriptHeaderOutFolder	= re_cfg.scriptHeaderOutFolder;
			cfg.vfsPaths				= re_cfg.vfsPaths;
			cfg.scriptIncludeDirs		= re_cfg.scriptIncludeDirs;

			_script.reset( new ScriptExe{ RVRef(cfg) });	// throw

			_test.scripts.clear();
			for (auto& folder : re_cfg.testFolders)
			{
				for (auto& entry : FileSystem::EnumRecursive( folder ))
				{
					if ( entry.IsFile() and entry.Get().extension() == ".as" )
						_test.scripts.push_back( entry.Get() );
				}
			}

			if ( not _test.scripts.empty() )
			{
				CHECK_ERR( not re_cfg.testOutput.empty() );
				s_REConfig.testOutput /= GraphicsScheduler().GetDevice().GetDeviceName();
				FileSystem::CreateDirectories( re_cfg.testOutput );

				_test.isActive.store( true );
			}
		)
		return	_LoadInputActions();
	}

/*
=================================================
	_LoadInputActions
=================================================
*/
	bool  ResEditorCore::_LoadInputActions ()
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
	OnSurfaceCreated
=================================================
*/
	bool  ResEditorCore::OnSurfaceCreated (IWindow &wnd) __NE___
	{
		_window = &wnd;
		return _ui.Init( wnd.GetSurface(), c_WindowMode );
	}

/*
=================================================
	_InitInputActions
=================================================
*/
	void  ResEditorCore::_InitInputActions (IInputActions &ia) __NE___
	{
		MemRefRStream	stream{ _inputActionsData->GetData() };

		CHECK( ia.LoadSerialized( stream ));

		CHECK( ia.SetMode( InputModeName{"Main.UI"} ));

		if ( auto port = ResEditorAppConfig::Get().remoteIAPort;  port != 0 )
		{
			_remoteIAServer = MakeRC<RemoteInputServer>();
			CHECK( _remoteIAServer->Init( port, ia ));
		}
	}

/*
=================================================
	StartRendering
=================================================
*/
	void  ResEditorCore::StartRendering (Ptr<IInputActions> input, Ptr<IOutputSurface> output, EWndState state) __NE___
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
	void  ResEditorCore::StopRendering (Ptr<IOutputSurface> output) __NE___
	{
		auto	main_loop	= _mainLoop.WriteLock();

		if ( output == null or main_loop->output == output )
			main_loop->output = null;
	}

/*
=================================================
	RunRenderScriptAsync
----
	should be used in background thread
=================================================
*/
	bool  ResEditorCore::RunRenderScriptAsync (const Path &scriptPath)
	{
		auto&	re_cfg = ResEditorAppConfig::Get();

		ScriptExe::ScriptConfig	cfg;
		cfg.dynSize			= UIInteraction::Instance().GetDynamicSize();
		cfg.shaderDirs		= re_cfg.shaderSearchDirs;
		cfg.includeDirs		= re_cfg.shaderIncludeDirs;
		cfg.pipelineDirs	= re_cfg.pipelineSearchDirs;
		cfg.scriptDir		= re_cfg.scriptCallableFolder;
		cfg.monitor			= _monitor.Read();
		cfg.enableRandomizer= not _test.isActive.load();

		auto	output	= _mainLoop.ConstPtr()->output;
		if ( output )
		{
			auto	infos = output->GetTargetInfo();
			CHECK_ERR( infos.size() == 1 );
			cfg.dynSize->Resize( infos[0].dimension );
		}

		auto	renderer = _script->Run( scriptPath, cfg );
		if ( not renderer )
			return false;

		if ( _test.isActive.load() )
			renderer->SetFreezeTime( true );

		MakeTask( [this, renderer] ()
				{
					_ui.SetHelpText( renderer->GetHelpText() );
					_ui.SetSurfaceFormat( renderer->GetSurfaceFormat() );
					_mainLoop->renderer = RVRef(renderer);
				}, {},
				"StartRendering",
				ETaskQueue::Main );

		return true;
	}

/*
=================================================
	_ProcessInput
=================================================
*/
	CoroTask  ResEditorCore::_ProcessInput (TsInputActions input, RC<Renderer> renderer, Ptr<EditorUI> ui, ActionQueueReader reader)
	{
		if ( ui )
		{
			bool	switch_mode = false;
			ui->ProcessInput( reader, OUT switch_mode );

			if_unlikely( switch_mode and renderer and renderer->GetInputMode().IsDefined() )
			{
				Scheduler().Run( ETaskQueue::Main,
								 _SetInputMode( input.Unsafe(), renderer->GetInputMode() ),
								 Tuple{}, "Core::SetInputMode" );
			}
		}

		if ( renderer )
		{
			bool	switch_mode = false;
			renderer->ProcessInput( reader, OUT switch_mode );

			if_unlikely( switch_mode )
			{
				Scheduler().Run( ETaskQueue::Main,
								 _SetInputMode( input.Unsafe(), InputModeName{"Main.UI"} ),
								 Tuple{}, "Core::SetInputMode" );
			}
		}

		co_return;
	}

/*
=================================================
	_SetInputMode
=================================================
*/
	CoroTask  ResEditorCore::_SetInputMode (Ptr<IInputActions> input, InputModeName mode)
	{
		CHECK( input->SetMode( mode ));
		co_return;
	}

/*
=================================================
	RenderFrame
=================================================
*/
	void  ResEditorCore::RenderFrame () __NE___
	{
		#if ENABLE_RDC
		if ( _ui.IsCaptureRequested() )
		{
			auto&	dev = GraphicsScheduler().GetDevice();
			if ( dev.HasRenderDocApi() )
			{
				CHECK( dev.GetRenderDocApi().TriggerFrameCapture() );
			}
		}
		#endif

		Ptr<IInputActions>		input;
		Ptr<IOutputSurface>		output;
		RC<Renderer>			renderer;
		{
			auto	main_loop = _mainLoop.ReadLock();

			input		= main_loop->input;
			output		= main_loop->output;
			renderer	= main_loop->renderer;

			if ( output == null or not output->IsInitialized() )
				renderer = null;
		}

		auto&			rg			= RenderGraph();
		Ptr<EditorUI>	ui			= output and _ui.IsInitialized() ? &_ui : null;
		AsyncTask		proc_input;

		if ( input )
		{
			ActionQueueReader reader = input->ReadInput( rg.GetPrevFrameId() );

			proc_input = Scheduler().Run( ETaskQueue::PerFrame,
										  _ProcessInput( input, renderer, ui, reader ),
										  Tuple{}, "Core::ProcessInput" );
		}

		Array<AsyncTask>	client_tasks;
		for (auto& client : _vfsClients) {
			client_tasks.push_back( client->Tick() );
		}

		// rendering depends on input processing
		if ( (renderer or ui) and rg.BeginFrame( output ))
		{
			if ( _remoteIAServer )
				GraphicsScheduler().AddNextFrameDeps( _remoteIAServer->Tick() );

			AsyncTask	draw_task	= renderer	? renderer->Execute({ proc_input })	: null;
			AsyncTask	ui_task		= ui		? ui->Draw({ proc_input })			: null;

			AsyncTask	end_frame	= rg.EndFrame( Tuple{ draw_task, ui_task, WeakDepArray{client_tasks} });

			if ( input )
				input->NextFrame( rg.GetNextFrameId() );
		}
		else
		{
			ThreadUtils::Sleep_15ms();
		}

		_UpdateTests( renderer );
	}

/*
=================================================
	_UpdateTests
=================================================
*/
	void  ResEditorCore::_UpdateTests (RC<Renderer> renderer)
	{
		const auto	LoadNextScript = [this] ()
		{{
			_mainLoop->renderer = null;

			if_unlikely( _test.scripts.empty() )
			{
				_test.isActive.store( false );
				ResEditorAppConfig::Get().screenshotPrefix.Write( Default );
				AE_LOGW( "---- All tests are completed ----" );
			}
			else
			{
				auto	path = _test.scripts.ExtractFront();
				ResEditorAppConfig::Get().screenshotPrefix.Write( ToString(path.parent_path().filename()) << '-' << ToString(path.filename()) );
				_test.status.store( ETestStatus::Load );

				Scheduler().Run(
					ETaskQueue::Background,
					[] (RC<ResEditorCore> core, Path inPath) -> CoroTask
					{
						if ( not core->RunRenderScriptAsync( inPath ))
							core->_test.status.store( ETestStatus::Complete );
						co_return;
					}
					( GetRC<ResEditorCore>(), RVRef(path) ),
					Tuple{},
					"Tests::LoadScript"
				);
			}
		}};

		if_likely( not _test.isActive.load() )
			return;

		switch_enum( _test.status.load() )
		{
			case ETestStatus::Load :
				if ( renderer )
				{
					_test.status.store( ETestStatus::Upload );
					_test.framesToCapture.store( 1000 );
				}
				return;

			case ETestStatus::Upload :
			{
				if ( not renderer )
				{
					_test.status.store( ETestStatus::Complete );
					return;
				}

				if ( renderer->IsUploadComplete() or _test.framesToCapture.Dec() < 0 )
				{
					// make screenshot
					auto	capture = UIInteraction::Instance().capture.WriteLock();

					capture->testScreenshot = true;
					capture->imageFormat	= EImageFormat::PNG;

					_test.framesToSwitch.store( GraphicsConfig::MaxFrames );
					_test.status.store( ETestStatus::Screenshot );
				}
				return;
			}

			case ETestStatus::Screenshot :
				if ( _test.framesToSwitch.Dec() < 0 )
					_test.status.store( ETestStatus::Complete );
				return;

			case ETestStatus::Complete :
				return LoadNextScript();
		}
		switch_end
	}

/*
=================================================
	WaitFrame
=================================================
*/
	void  ResEditorCore::WaitFrame (const Threading::EThreadArray	&threadMask,
									Ptr<IWindow>					window,
									Ptr<IVRDevice>					) __NE___
	{
		milliseconds	timeout = AE::DefaultTimeout;

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		timeout = seconds{2};
	  #endif
	  #ifndef AE_CFG_RELEASE
		if ( PlatformUtils::IsUnderDebugger() )
			timeout = minutes{60};
	  #endif

		CHECK( GraphicsScheduler().WaitNextFrame( threadMask, timeout ));

		if ( window )
		{
			if ( _window != window )
			{
				_window = window;
				_monitor.Write( window->GetMonitor() );
			}

			if ( auto new_mode = _ui.GetNewWindowMode();  new_mode.has_value() )
				Unused( window->SetMode( *new_mode ));
		}
	}

} // AE::ResEditor
//-----------------------------------------------------------------------------


using namespace AE;
using namespace AE::Base;
using namespace AE::App;
using namespace AE::ResEditor;

#define REQUIRE_APACHE_2
#include "base/Defines/DetectLicense.inl.h"


/*
=================================================
	AE_OnAppCreated
=================================================
*/
Unique<IApplication::IAppListener>  AE_OnAppCreated ()
{
#if AE_PUBLIC_VERSION
	StaticLogger::Initialize();
	StaticLogger::AddLogger( ILogger::CreateConsoleOutput() );

#  ifdef AE_DEBUG
	StaticLogger::AddLogger( ILogger::CreateIDEOutput() );
	StaticLogger::AddLogger( ILogger::CreateHtmlOutput( "log" ));
#  endif
#else
	StaticLogger::InitDefault();
#endif

	AE_LOGI( String{AE_ENGINE_NAME} << ' ' << ToString(AE_VERSION.Get<0>()) << '.' << ToString(AE_VERSION.Get<1>()) << '.' << ToString(AE_VERSION.Get<2>()) );
	AE_LOG_DBG( "License: "s << AE_LICENSE );

	InitResEditorAppConfig();

	return MakeUnique<ResEditorApplication>();
}

/*
=================================================
	AE_OnAppDestroyed
=================================================
*/
void  AE_OnAppDestroyed ()
{
	// Don't check for memleaks because of
	// false positive in glslang when used dynamic allocation in static variable.
	StaticLogger::Deinitialize( false );
}
