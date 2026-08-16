// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Core/ResEditorAppConfig.h"

#include "graphics_rhi/Scripting/GraphicsBindings.h"

AE_DECL_SCRIPT_OBJ(	AE::ResEditor::ResEditorAppConfig,	"Config" );

namespace AE::ResEditor
{
namespace
{
	static ResEditorAppConfig		s_REConfig;

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
	static void  ResEditorAppConfig_SetRemoteDeviceIpAddress (ResEditorAppConfig &self, uint p0, uint p1, uint p2, uint p3, uint port)
	{
		self.ipAddress[0] = ubyte(p0);
		self.ipAddress[1] = ubyte(p1);
		self.ipAddress[2] = ubyte(p2);
		self.ipAddress[3] = ubyte(p3);
		self.ipPort = ushort(port);
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
		self.windowMode = EWindowMode::NonResizable;
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
			src.usePreprocessor	= true;
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
		s_REConfig._loaded = true;

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
	//cfg.gapiValidation = EDeviceValidation::SynchronizationPreset;
	cfg.enableRenderDoc = false;
	cfg.TestOutput( test_ref_path );
	cfg.TestFolder( "screenshot-test" );
	cfg.TestFolder( "tests" );

	cfg.TestFolder( "callable" );
	cfg.TestFolder( "color-space" );
	cfg.TestFolder( "compute" );
	cfg.TestFolder( "games" );
	cfg.TestFolder( "gbuffer-classify" );
	cfg.TestFolder( "gen-geom" );
	cfg.TestFolder( "geom-cull" );
	cfg.TestFolder( "light-cull" );
	cfg.TestFolder( "light-refl" );
	cfg.TestFolder( "light-tech" );
	cfg.TestFolder( "neural-shader" );
	cfg.TestFolder( "nonuniform" );
	cfg.TestFolder( "packing" );
	cfg.TestFolder( "particles" );
	cfg.TestFolder( "perf" );
	cfg.TestFolder( "planets" );
	cfg.TestFolder( "posteffects" );
	cfg.TestFolder( "projections" );
	cfg.TestFolder( "ray-trace" );
	cfg.TestFolder( "samples-2d" );
	cfg.TestFolder( "samples-3d" );
	cfg.TestFolder( "shadows" );
	cfg.TestFolder( "tools" );
	cfg.TestFolder( "vfx" );
	cfg.TestFolder( "video" );
	cfg.TestFolder( "voxels" );
	//*/
}
)";

		FileWStream		file {filename};
		return	file.IsOpen()				and
				file.Write( StringView{str} );
	}

} // namespace


/*
=================================================
	Init
=================================================
*/
	void  ResEditorAppConfig::Init (IApplication &app) __NE___
	{
	#ifdef AE_PLATFORM_ANDROID
	#else
		Unused( app );
	#endif

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
			CHECK_FATAL_MSG( false, "failed to run initial script" );
		}
	}

/*
=================================================
	Get / Edit
=================================================
*/
	ResEditorAppConfig const&  ResEditorAppConfig::Get () __NE___
	{
		ASSERT( s_REConfig._loaded );
		return s_REConfig;
	}

	ResEditorAppConfig&  ResEditorAppConfig::Edit () __NE___
	{
		ASSERT( s_REConfig._loaded );
		return s_REConfig;
	}

} // AE::ResEditor
