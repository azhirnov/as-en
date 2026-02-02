// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "vfs/Disk/DiskStaticStorage.h"
#include "vfs/Disk/DiskDynamicStorage.h"
#include "vfs/Network/Messages.h"

#include "Core/EditorCore.h"
#include "Scripting/ScriptExe.h"

AE_DECL_SCRIPT_OBJ(	AE::ResEditor::ResEditorAppConfig,	"Config" );

#if AE_PORTABLE_APP and AE_GRAPHICS_STRONG_VALIDATION == 0
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
			cfg.graphics.maxFrames				= 2;

			cfg.graphics.device.appName			= "ResourceEditor";
			cfg.graphics.device.requiredQueues	= EQueueMask::Graphics;
			cfg.graphics.device.optionalQueues	= Default;
			cfg.graphics.device.devFlags		= (s_REConfig.setStableGPUClock ? EDeviceFlags::SetStableClock : Default) |
												  (s_REConfig.enableRenderDoc ? EDeviceFlags::EnableRenderDoc : Default) |
												  EDeviceFlags::EnablePerfCounters;
			cfg.graphics.device.deviceName		= s_REConfig.deviceName;

		  #ifdef AE_CFG_RELEASE
			cfg.graphics.device.validation		= EDeviceValidation::Disabled;
		  #else
			cfg.graphics.device.validation		= s_REConfig.gapiValidation;
		  #endif

			cfg.graphics.swapchain.colorFormat	= EPixelFormat::RGBA8_UNorm;

			cfg.graphics.swapchain.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;
			cfg.graphics.swapchain.options		= EImageOpt::BlitDst;
			cfg.graphics.swapchain.presentMode	= EPresentMode::FIFO;		// vsync
			cfg.graphics.swapchain.minImageCount= 2;
			cfg.graphics.swapchain.usePreTransform = false;
		//	cfg.graphics.swapchain.scaling		= EPresentScaling::AspectRatioStretch;
		//	cfg.graphics.swapchain.scale		= 0.5f;

			cfg.graphics.useRenderGraph			= true;

			for (usize i = 0, cnt = Min( cfg.graphics.driverList.size(), s_REConfig.driverList.size() ); i < cnt; ++i) {
				cfg.graphics.driverList[i] = s_REConfig.driverList[i];
			}

			// custom size for staging
		//	cfg.graphics.staging.maxWriteDynamicSize = 256_MiB;
			cfg.graphics.staging.maxReadDynamicSize	 = 128_MiB;
		}

		// window
		{
			cfg.window.title	= "ResourceEditor";
			cfg.window.size		= uint2{ s_REConfig.screenWidth, s_REConfig.screenHeight };
			cfg.window.mode		= c_WindowMode;
			cfg.window.monitorId = Monitor::ID( s_REConfig.monitorId );
		}

		// VR
		{
			cfg.enableVR		= false;
			cfg.vr.dimension	= ImageDim2_t{2048};
			cfg.vr.colorFormat	= EPixelFormat::BGRA8_UNorm;
			cfg.vr.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Transfer;	// default
			cfg.vr.options		= EImageOpt::BlitDst;

		//	cfg.vrDevices.push_back( IVRSession::EDeviceType::OpenXR );
		//	cfg.vrDevices.push_back( IVRSession::EDeviceType::OpenVR );
			cfg.vrDevices.push_back( IVRSession::EDeviceType::Emulator );
		}

		cfg.enableNetwork = true;

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		cfg.graphics.staging.maxWriteDynamicSize = 8_MiB;	// limited by network bandwidth
		cfg.graphics.staging.maxReadDynamicSize	 = 8_MiB;
		cfg.graphics.staging.dynamicBlockSize	 = 8_MiB;

		cfg.window.mode							= EWindowMode::NonResizable;
		cfg.graphics.maxFrames					= 2;
		cfg.graphics.swapchain.minImageCount	= 2;
		cfg.graphics.graphicsLibPath			= s_REConfig.graphicsLibPath;
		cfg.graphics.enableSyncLog				= false;

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
	static void  ResEditorAppConfig_StaticVFSPath (ResEditorAppConfig &self, const String &path, const String &prefix)
	{
		if ( not FileSystem::IsDirectory( path ))
		{
			AE_LOGW( "VFSPath '"s << ToString(path) << "' is not a directory" );
			return;
		}

		self.vfsPaths.emplace_back( FileSystem::ToAbsolute( Path{path} ), prefix );
	}

	static void  ResEditorAppConfig_DynamicVFSPath (ResEditorAppConfig &self, const String &path, const String &prefix)
	{
		FileSystem::CreateDirectories( path );
		self.vfsPaths.emplace_back( FileSystem::ToAbsolute( Path{path} ), prefix );
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
	ResEditorAppConfig_RenderDocDir
=================================================
*/
	static void  ResEditorAppConfig_RenderDocDir (ResEditorAppConfig &self, const String &path)
	{
		if ( not FileSystem::IsDirectory( path ))
		{
			CHECK_THROW_MSG( FileSystem::CreateDirectories( path ),
				"Failed to create folder '"s << ToString(path) << "'" );
		}

		CHECK_THROW( self.renderDocFolder.empty() );
		self.renderDocFolder = FileSystem::ToAbsolute( Path{path} );
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
	ResEditorAppConfig_AddGraphicsDriver
=================================================
*/
	static void  ResEditorAppConfig_AddGraphicsDriver (ResEditorAppConfig &self, const String &driverName)
	{
		CHECK_THROW_MSG( not driverName.empty() );

		const auto	EDriver_ToString = [](Graphics::EDriver type) -> StringView
		{{
			switch_enum( type )
			{
				case EDriver::Unknown :		break;
				case EDriver::_Count :		break;
				#define CASE( _name_ )		case EDriver::_name_ :	return AE_TOSTRING( _name_ );
				CASE( LavaPipe )
			  #ifdef AE_PLATFORM_LINUX
				case EDriver::_LinuxDrivers : break;
				CASE( RADV )
				CASE( AMD_VLK )
				CASE( AMD_PRO )
				CASE( ANV )
				CASE( IntelPro )
				CASE( Nouveau )
				CASE( NVK )
				CASE( NVPro )
				CASE( VirtGPU )
				CASE( GFXStream )
			  #endif
				#undef CASE
			}
			switch_end
			return {};
		}};

		String	supported;

		for (uint i = 0, cnt = uint(Graphics::EDriver::_Count); i < cnt; ++i)
		{
			StringView	name = EDriver_ToString( Graphics::EDriver(i) );
			if ( EqualIC( driverName, name ))
			{
				self.driverList.push_back( Graphics::EDriver(i) );
				return;
			}

			supported << name << ", ";
		}

		if ( not supported.empty() )
			supported.erase( supported.end()-2, supported.end() );

		CHECK_THROW_MSG( false,
			"Unknown graphics driver '"s << driverName << "', known drivers: " << supported );
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
		GraphicsBindings::Bind_EDeviceValidation( se );
		{
			ClassBinder<ResEditorAppConfig>		binder{ se };
			binder.CreateClassValue();
			AS_METHOD( binder, ResEditorAppConfig_StaticVFSPath,			"VFSPath",				{"path", "prefixInVFS"} );	// deprecated
			AS_METHOD( binder, ResEditorAppConfig_DynamicVFSPath,			"MakeVFSPath",			{"path", "prefixInVFS"} );	// deprecated
			AS_METHOD( binder, ResEditorAppConfig_StaticVFSPath,			"StaticVFSPath",		{"path", "prefixInVFS"} );
			AS_METHOD( binder, ResEditorAppConfig_DynamicVFSPath,			"DynamicVFSPath",		{"path", "prefixInVFS"} );
			AS_METHOD( binder, ResEditorAppConfig_NetVFS,					"NetVFS",				{"host", "service", "prefixInVFS"} );
			AS_METHOD( binder, ResEditorAppConfig_UIDataDir,				"UIDataDir",			{} );
			AS_METHOD( binder, ResEditorAppConfig_PipelineSearchDir,		"PipelineSearchDir",	{} );
			AS_METHOD( binder, ResEditorAppConfig_PipelineIncludeDir,		"PipelineIncludeDir",	{} );
			AS_METHOD( binder, ResEditorAppConfig_ShaderSearchDir,			"ShaderSearchDir",		{} );
			AS_METHOD( binder, ResEditorAppConfig_ShaderIncludeDir,			"ShaderIncludeDir",		{} );
			AS_METHOD( binder, ResEditorAppConfig_ScriptDir,				"ScriptDir",			{} );
			AS_METHOD( binder, ResEditorAppConfig_CallableScriptDir,		"CallableScriptDir",	{} );
			AS_METHOD( binder, ResEditorAppConfig_AddScriptIncludeDir,		"ScriptIncludeDir",		{} );
			AS_METHOD( binder, ResEditorAppConfig_ShaderTraceDir,			"ShaderTraceDir",		{} );
			AS_METHOD( binder, ResEditorAppConfig_ScreenshotDir,			"ScreenshotDir",		{} );
			AS_METHOD( binder, ResEditorAppConfig_VideoDir,					"VideoDir",				{} );
			AS_METHOD( binder, ResEditorAppConfig_ExportDir,				"ExportDir",			{} );
			AS_METHOD( binder, ResEditorAppConfig_RenderDocDir,				"RenderDocDir",			{} );
			AS_METHOD( binder, ResEditorAppConfig_SetRemoteDeviceIpAddress,	"RemoteDeviceIpAddress",{} );
			AS_METHOD( binder, ResEditorAppConfig_SetGraphicsLibPath,		"GraphicsLibPath",		{} );
			AS_METHOD( binder, ResEditorAppConfig_AddTestFolder,			"TestFolder",			{} );
			AS_METHOD( binder, ResEditorAppConfig_AddTestOutput,			"TestOutput",			{} );
			AS_METHOD( binder, ResEditorAppConfig_SetRemoteInputServerPort,	"RemoteInputServerPort",{} );
			AS_METHOD( binder, ResEditorAppConfig_AddGraphicsDriver,		"AddGraphicsDriver",	{} );
			binder.AddProperty( &ResEditorAppConfig::setStableGPUClock,		"setStableGPUClock"		);
			binder.AddProperty( &ResEditorAppConfig::enableRenderDoc,		"enableRenderDoc"		);
			binder.AddProperty( &ResEditorAppConfig::screenWidth,			"screenWidth"			);
			binder.AddProperty( &ResEditorAppConfig::screenHeight,			"screenHeight"			);
			binder.AddProperty( &ResEditorAppConfig::monitorId,				"monitorId"				);
			binder.AddProperty( &ResEditorAppConfig::deviceName,			"deviceName"			);
			binder.AddProperty( &ResEditorAppConfig::gapiValidation,		"gapiValidation"		);
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
		if ( AE_PORTABLE_APP )
		{
			str << R"(
void main (Config &out cfg)
{
	const string	vfs_path 			= "../";
	const string	local_path			= vfs_path + "src/";
	const string	shader_data_path	= vfs_path + "shared_data/";
	const string	ui_path				= vfs_path + "ui";
	const string	test_ref_path		= vfs_path + "test_ref/";
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
"	const string	ui_path				= base_path + \"AE-Temp/samples/res_editor\";\n"
"	const string	test_ref_path		= vfs_path + \"/samples/res_editor/ref\";\n";
		}

		str << R"(
	// VFS //
	//	attach path on disk to VFS
	//	all file paths listed at startup, new files will be accessible after app restart
	cfg.StaticVFSPath( vfs_path + "shadertoy_data",  "shadertoy/" );
	cfg.StaticVFSPath( vfs_path + "res_editor_data", "res/" );
	//	create directory if not exists
	//	new files can be added at runtime
	cfg.DynamicVFSPath( local_path + "../_export",   "export/" );
	//	connect to network file system
	//cfg.NetVFS( "localhost", "4000", "net/" );

	// pipeline dirs //
	//	where to search pipelines
	cfg.PipelineSearchDir( local_path + "pipelines" );
	cfg.PipelineIncludeDir( local_path + "pipeline_inc" );

	// shaders //
	//	where to search shaders for pipelines and passes
	cfg.ShaderSearchDir( local_path + "shaders" );
	cfg.ShaderIncludeDir( shader_data_path + "shaders" );
	cfg.ShaderIncludeDir( local_path + "shaders" );
	cfg.ShaderIncludeDir( local_path + "script_inc" );
	cfg.ShaderIncludeDir( local_path + "pipeline_inc" );

	// scripts //
	//	all files with '.as' extension will be added to script list in editor
	cfg.ScriptDir( local_path + "scripts" );
	//	scripts which can be used directly and for 'RunScript()' call
	cfg.CallableScriptDir( local_path + "scripts/callable" );
	//	scripts which can be included in other scripts
	cfg.ScriptIncludeDir( local_path + "script_inc" );

	// output //
	//	path for imgui and ui pipelines
	cfg.UIDataDir( ui_path );
	//	where to put shader traces
	cfg.ShaderTraceDir( local_path + "../_shader_trace" );
	//	where to save screenshots
	cfg.ScreenshotDir( local_path + "../_screenshots" );
	//	where to save video
	cfg.VideoDir( local_path + "../_video" );
	//	where to save export (images, models, scenes, etc)
	cfg.ExportDir( local_path + "../_export" );
	//	where to save RenderDoc captures
	cfg.RenderDocDir( local_path + "../_renderdoc" );

	// graphics settings //
	cfg.screenWidth  = 1600;
	cfg.screenHeight = 900;
	//cfg.monitorId  = 0;  // optional
	//	AMD/NV only: set stable GPU clock for profiling, otherwise driver can move GPU to low power mode or use temporary boost.
	cfg.setStableGPUClock = false;
	//	on start attach RenderDoc to the app, overlay is hidden, press F2 to capture frame
	cfg.enableRenderDoc = false;
	//	GPU index or part of name
	//cfg.deviceName = "";
	//cfg.gapiValidation = EDeviceValidation::Enabled;
	//cfg.AddGraphicsDriver( "LavaPipe" );

	// remote input //
	//	see 'Setup Remote Input' in 'docs/Remote.md'
	//cfg.RemoteInputServerPort( 0 );
)";

#ifdef AE_ENABLE_REMOTE_GRAPHICS
		str << R"(
	// remote graphics device //
	cfg.RemoteDeviceIpAddress( 192, 168, 0, 0 );
	cfg.GraphicsLibPath( "GraphicsRHI-shared.dll" );
)";
#endif

		str << R"(
	// tests //
	/*
	// uncomment to run tests on start
	cfg.screenWidth  = 1600;
	cfg.screenHeight = 900;
	cfg.TestOutput( test_ref_path );
	cfg.TestFolder( "screenshot-test" );
	cfg.TestFolder( "tests" );

	cfg.TestFolder( "callable" );
	cfg.TestFolder( "color-space" );
	cfg.TestFolder( "compute" );
	cfg.TestFolder( "games" );
	cfg.TestFolder( "gbuffer-classify" );
	cfg.TestFolder( "light-cull" );
	cfg.TestFolder( "light-tech" );
	cfg.TestFolder( "neural-shader" );
	cfg.TestFolder( "neural-shader/mlp-training" );
	cfg.TestFolder( "nonuniform" );
	cfg.TestFolder( "packing" );
	cfg.TestFolder( "particles" );
	cfg.TestFolder( "perf" );
	cfg.TestFolder( "planets" );
	cfg.TestFolder( "posteffects" );
	cfg.TestFolder( "ray-trace" );
	cfg.TestFolder( "samples-2d" );
	cfg.TestFolder( "samples-3d" );
	cfg.TestFolder( "shadows" );
	cfg.TestFolder( "tools" );
	cfg.TestFolder( "vfx" );
	//*/
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

			if ( not AE_PORTABLE_APP )
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

		Unused( PlatformUtils::SetSystemSleepState( ESystemSleepState::DisplayAlwaysOn ));
	}

/*
=================================================
	destructor
=================================================
*/
	ResEditorApplication::~ResEditorApplication () __NE___
	{
		Unused( PlatformUtils::SetSystemSleepState( ESystemSleepState::Default ));
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
					[] (RC<VFSClient> client) -> AsyncCoro
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
					[] (RC<RemoteInputServer> server) -> AsyncCoro
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
----
	Thread-safe: only main thread
=================================================
*/
	bool  ResEditorCore::OnSurfaceCreated (IWindow &wnd) __NE___
	{
	  #if ENABLE_RDC
		// initialize render doc
		{
			auto&	re_cfg = ResEditorAppConfig::Get();
			if ( re_cfg.enableRenderDoc and not re_cfg.renderDocFolder.empty() )
			{
				GraphicsScheduler().GetDevice().GetRenderDocApi().CaptureFolder( ToString( re_cfg.renderDocFolder ) << '/' );

				EFileSystemWatchBits filter;
				filter.insert( EFileSystemWatch::FileCreated );
				CHECK( _rdCaptureWatch.Start( re_cfg.renderDocFolder, filter ));
			}
		}
	  #endif

		_window = &wnd;
		return _ui.Init( wnd.GetSurface(), c_WindowMode, wnd.GetMonitor().uiScale );
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
----
	Thread-safe: only main thread
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
----
	Thread-safe: only main thread
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
			cfg.dynSize->Resize( infos[0].Dimension() );
		}

		auto	renderer = _script->Run( scriptPath, cfg );
		if ( not renderer )
			return false;

		if ( _test.isActive.load() )
			renderer->SetFreezeTime( true );

		Unused( CreateAsyncRev(
			GetRC<ResEditorCore>(), renderer,
			[] (RC<ResEditorCore> self, RC<Renderer> renderer) -> ScheduledCoro<ETaskQueue::Main>
			{
				self->_ui.SetHelpText( renderer->GetHelpText() );
				self->_ui.SetSurfaceFormat( renderer->GetSurfaceFormat() );
				self->_mainLoop->renderer = RVRef(renderer);
				co_return;
			}));

		return true;
	}

/*
=================================================
	_ProcessInput
=================================================
*/
	AsyncCoro  ResEditorCore::_ProcessInput (TsInputActions input, RC<Renderer> renderer, Ptr<EditorUI> ui, ActionQueueReader reader)
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
	AsyncCoro  ResEditorCore::_SetInputMode (Ptr<IInputActions> input, InputModeName mode)
	{
		CHECK( input->SetMode( mode ));
		co_return;
	}

/*
=================================================
	RenderFrame
----
	Thread-safe: only main thread
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
				AE_LOGI( "Trigger frame capture..." );
			}
			else
				AE_LOGI( "RenderDoc is not attached, can't capture frame" );
		}
		_CheckRdEvents();
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
					[] (RC<ResEditorCore> core, Path inPath) -> AsyncCoro
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

					_test.framesToSwitch.store( GraphicsConfig::MaxFrames*2 );
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
----
	Thread-safe: only main thread
=================================================
*/
	void  ResEditorCore::WaitFrame (const Threading::EThreadArray	&threadMask,
									Ptr<IWindow>					window) __NE___
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
			{
				if ( auto* desk_wnd = window->AsDesktopWindow() )
					Unused( desk_wnd->SetMode( *new_mode ));
			}
		}
	}

/*
=================================================
	_CheckRdEvents
----
	Thread-safe: only main thread
=================================================
*/
	void  ResEditorCore::_CheckRdEvents ()
	{
		FileWatch::EventArray_t	events;
		Unused( _rdCaptureWatch.GetEvents( OUT events ));

		for (auto& ev : events)
		{
			if ( ev.action == EFileSystemAction::Added )
				AE_LOGI( "Added RenderDoc capture '"s << ToString(ev.path) << "'" );
		}
	}

} // AE::ResEditor
//-----------------------------------------------------------------------------


using namespace AE;
using namespace AE::Base;
using namespace AE::App;
using namespace AE::ResEditor;

#define REQUIRE_LGPLv3
#include "base/Defines/DetectLicense.inl.h"


/*
=================================================
	AE_OnAppCreated
=================================================
*/
Unique<IApplication::IAppListener>  AE_OnAppCreated ()
{
#if AE_PORTABLE_APP
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
