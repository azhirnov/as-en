// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Common.h"

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
		Path			renderDocFolder;

		// graphics settings
		uint			screenWidth			= 0;
		uint			screenHeight		= 0;
		uint			monitorId			= UMax;
		bool			setStableGPUClock	= false;
		bool			enableRenderDoc		= false;
		String			deviceName;
		EDeviceValidation			gapiValidation = EDeviceValidation::Enabled;
		Array<Graphics::EDriver>	driverList;

		// remote graphics device
		ubyte4			ipAddress			{0};
		ushort			ipPort				= 0;
		String			graphicsLibPath;

		// remote input
		ushort			remoteIAPort		= 0;

		// tests
		Array<Path>		testFolders;
		Path			testOutput;

		// other
		mutable ScreenshotPrefix_t	screenshotPrefix;

		EWindowMode		windowMode			= EWindowMode::Resizable;

		bool			_loaded				= false;


		ResEditorAppConfig ()									= default;
		ResEditorAppConfig (ResEditorAppConfig &&)				= default;
		ResEditorAppConfig (const ResEditorAppConfig &)			= default;
		ResEditorAppConfig&  operator = (ResEditorAppConfig &&)	= default;

		ND_ static ResEditorAppConfig const&	Get ()			__NE___;
		ND_ static ResEditorAppConfig &			Edit ()			__NE___;

			static void  Init (IApplication &)					__NE___;
	};

} // AE::ResEditor
