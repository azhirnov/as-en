// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Android/ApplicationAndroid.h"
#include "platform/GLFW/ApplicationGLFW.h"
#include "platform/WinAPI/ApplicationWinAPI.h"
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
						_RunTests( *a );
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
		if ( lib.Load( libName ))
		{
			int (*fn) (Args...);
			if ( lib.GetProcAddr( fnName, OUT fn ))
			{
				AE_LOGI( "-- Begin "s << libName );
				fn( args... );
			}
			else
				AE_LOGE( "Failed to get fn "s << fnName << " from " << libName );
		}
		else
			AE_LOGE( "Failed to load "s << libName );
	}

	void  _RunTests (IApplication &app) const
	{
		auto			asset_storage	= app.OpenStorage( EAppStorage::Builtin );
		auto			cache_storage	= app.OpenStorage( EAppStorage::ExternalCache );
		const String	cache_path		= ToString( app.GetStoragePath( EAppStorage::ExternalCache ));

		AE_LOGI( ">> Begin tests" );
		{
			_LoadAndRun( "libTestsBase.so",				"Tests_Base",			cache_path.c_str() );
			_LoadAndRun( "libTestsScripting.so",		"Tests_Scripting",		cache_path.c_str() );
			_LoadAndRun( "libTestsSerializing.so",		"Tests_Serializing",	cache_path.c_str() );
			_LoadAndRun( "libTestsThreading.so",		"Tests_Threading",		cache_path.c_str() );
			_LoadAndRun( "libTestsNetworking.so",		"Tests_Networking",		cache_path.c_str() );
			_LoadAndRun( "libTestsECS-st.so",			"Tests_ECSst",			cache_path.c_str() );
			_LoadAndRun( "libTestsVFS.so",				"Tests_VFS",			cache_path.c_str() );
			_LoadAndRun( "libTestsHuLang.so",			"Tests_HuLang",			cache_path.c_str() );
			_LoadAndRun( "libTestsGraphics.so",			"Tests_Graphics2",		asset_storage.get(), cache_storage.get() );
			_LoadAndRun( "libTestsGraphicsHL.so",		"Tests_GraphicsHL2",	asset_storage.get(), cache_storage.get() );

			_LoadAndRun( "libTestsAtlasTools.so",		"Tests_AtlasTools",		cache_path.c_str() );
			_LoadAndRun( "libTestsGeometryTools.so",	"Tests_GeometryTools",	cache_path.c_str() );
		}
		AE_LOGI( "<< Tests complete" );


		#ifdef AE_RELEASE
		AE_LOGI( ">> Begin performance tests" );
		{
			_LoadAndRun( "libPerfBase.so",				"Perf_Base",			cache_path.c_str() );
			_LoadAndRun( "libPerfThreading.so",			"Perf_Threading",		cache_path.c_str() );
		}
		AE_LOGI( "<< Performance tests complete" );
		#endif
	}
};


Unique<IApplication::IAppListener>  AE_OnAppCreated ()
{
	AE::Base::StaticLogger::InitDefault();

	return MakeUnique<AppListener>();
}

void  AE_OnAppDestroyed ()
{
	AE::Base::StaticLogger::Deinitialize(true);
}

#ifdef AE_PLATFORM_ANDROID

	extern "C" JNIEXPORT jint  JNI_OnLoad (JavaVM* vm, void*)
	{
		return ApplicationAndroid::OnJniLoad( vm );
	}

	extern "C" void JNI_OnUnload (JavaVM* vm, void *)
	{
		return ApplicationAndroid::OnJniUnload( vm );
	}

#endif // AE_PLATFORM_ANDROID
