// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Public/Application.h"
#include "vfs/VirtualFileSystem.h"

using namespace AE::VFS;
using namespace AE::App;


class AppListener final : public IApplication::IAppListener
{
private:
	StdThread		_thread;
	Atomic<bool>	_complete	{false};

public:
	AppListener ()								__NE___ {}
	~AppListener ()								__NE_OV {}

	void  OnStart (IApplication &app)			__NE_OV
	{
		_thread = StdThread{ [this, a = &app]()
					{
						ThreadUtils::SetName( "--test--" );
						AE_LOGW( "---- begin tests ----" );
						_RunTests( *a );
						AE_LOGW( "---- end tests ----" );
						_complete.store( true );
					}};
	}
	void  OnStop (IApplication &)				__NE_OV {}

	void  BeforeWndUpdate (IApplication &)		__NE_OV {}
	void  AfterWndUpdate (IApplication &app)	__NE_OV
	{
		if ( _complete.load() )
			app.Terminate();
	}

	template <typename ...Args>
	void  _LoadAndRun (StringView libName, StringView fnName, Args ...args) const
	{
		Library	lib;
		AE_LOGI( "Try load "s << libName );

		if ( lib.Load( libName ))
		{
			AE_LOGI( "Try get function address "s << fnName );

			int (*fn) (Args...);
			if ( lib.GetProcAddr( fnName, OUT fn ))
			{
				// set IsUndefDebugger
				void (*set_dbg)(bool);
				if ( lib.GetProcAddr( "Android_SetIsUnderDebugger", OUT set_dbg ))
				{
					set_dbg( PlatformUtils::IsUnderDebugger() );
				}

				AE_LOGW( "-- Begin "s << libName );
				fn( args... );
				AE_LOGW( "-- End "s << libName );
			}
			else
				AE_LOGE( "Failed to get fn "s << fnName << " from " << libName );
		}
		else
			AE_LOGE( "Failed to load "s << libName );
	}

	void  _RunTests (IApplication &app) const
	{
		auto		asset_storage	= app.OpenStorage( EAppStorage::Builtin );
		auto		cache_storage	= app.OpenStorage( EAppStorage::ExternalCache );
		String		cache_path		= ToString( app.GetStoragePath( EAppStorage::ExternalCache ));
		char const*	argv[]			= { "exe path", "-p", cache_path.c_str() };
		const int	argc			= int(CountOf( argv ));

		AE_LOGI( ">> Begin tests" );
		{
			_LoadAndRun( "libTestsBase.so",				"AEMain",				argc, argv );
			_LoadAndRun( "libTestsScripting.so",		"AEMain",				argc, argv );
			_LoadAndRun( "libTestsSerializing.so",		"AEMain",				argc, argv );
			_LoadAndRun( "libTestsThreading.so",		"AEMain",				argc, argv );
			_LoadAndRun( "libTestsNetworking.so",		"AEMain",				argc, argv );
			_LoadAndRun( "libTestsECS.so",				"AEMain",				argc, argv );
			_LoadAndRun( "libTestsVFS.so",				"AEMain",				argc, argv );
			_LoadAndRun( "libTestsHuLang.so",			"AEMain",				argc, argv );
		//	_LoadAndRun( "libTestsLogic.so",			"AEMain",				argc, argv );
			_LoadAndRun( "libTestsGraphicsRHI.so",		"Tests_GraphicsRHI",	asset_storage.get(), cache_storage.get() );
			_LoadAndRun( "libTestsGraphics.so",			"Tests_Graphics",		asset_storage.get(), cache_storage.get() );
			_LoadAndRun( "libTestsVideo.so",			"AEMain",				argc, argv );

			_LoadAndRun( "libTestsAtlasTools.so",		"AEMain",				argc, argv );
			_LoadAndRun( "libTestsGeometryTools.so",	"AEMain",				argc, argv );

		//	_LoadAndRun( "libNetworkStressTest.so",		"AEMain",				argc, argv );
		}
		AE_LOGI( "<< Tests complete" );


		AE_LOGI( ">> Begin performance tests" );
		{
			_LoadAndRun( "libPerfGraphics.so",			"Perf_Graphics",		asset_storage.get() );
		  #ifdef AE_RELEASE
			_LoadAndRun( "libPerfBase.so",				"AEMain",				argc, argv );
			_LoadAndRun( "libPerfThreading.so",			"AEMain",				argc, argv );
		  #endif
		}
		AE_LOGI( "<< Performance tests complete" );
	}
};

/*
=================================================
	AE_OnAppCreated / AE_OnAppDestroyed
=================================================
*/
Unique<IApplication::IAppListener>  AE_OnAppCreated (const int argc, char const* argv[])
{
	Unused( argc, argv );
	AE::Base::StaticLogger::InitDefault();

	return MakeUnique<AppListener>();
}

void  AE_OnAppDestroyed ()
{
	AE::Base::StaticLogger::Deinitialize( True{"checkMemLeaks"} );
}

/*
=================================================
	JNI_OnLoad / JNI_OnUnload
=================================================
*/
#ifdef AE_PLATFORM_ANDROID
#	include "platform/Android/ApplicationAndroid.h"

	extern "C" JNIEXPORT jint  JNI_OnLoad (JavaVM* vm, void*)
	{
		return ApplicationAndroid::OnJniLoad( vm );
	}

	extern "C" void JNI_OnUnload (JavaVM* vm, void *)
	{
		return ApplicationAndroid::OnJniUnload( vm );
	}

#endif // AE_PLATFORM_ANDROID
