// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Android/ApplicationAndroid.h"

#ifdef AE_PLATFORM_ANDROID
# include "graphics/Vulkan/VSwapchain.h"
# include "platform/Android/FileSystemAndroid.h"


// must be implemented in client code
extern "C" JNIEXPORT jint  JNI_OnLoad   (JavaVM* vm, void *);
extern "C" JNIEXPORT void  JNI_OnUnload (JavaVM* vm, void *);

// to trigger link errors
namespace {
    static const size_t s_JNI_ptr = size_t(&JNI_OnLoad) + size_t(&JNI_OnUnload);
}


namespace AE::App
{

/*
=================================================
    GetApp
=================================================
*/
namespace {
    ND_ static ApplicationAndroid&  GetApp () __NE___
    {
        auto*   app = ApplicationAndroid::_GetAppInstance();
        ASSERT( app );
        return *app;
    }
}
/*
=================================================
    _GetAppInstance
=================================================
*/
    ApplicationAndroid*&  ApplicationAndroid::_GetAppInstance () __NE___
    {
        static ApplicationAndroid*  app = new ApplicationAndroid{ AE_OnAppCreated() };  // throw
        return app;
    }

/*
=================================================
    constructor
=================================================
*/
    ApplicationAndroid::ApplicationAndroid (Unique<IAppListener> listener) __NE___ :
        ApplicationBase{ RVRef(listener) }
    {
        Unused( s_JNI_ptr );
    }

/*
=================================================
    destructor
=================================================
*/
    ApplicationAndroid::~ApplicationAndroid () __NE___
    {
        _OnDestroy();

        ASSERT( _hwCamera.release().use_count() <= 1 );
    }

/*
=================================================
    CreateWindow
=================================================
*/
    WindowPtr  ApplicationAndroid::CreateWindow (WndListenerPtr listener, const WindowDesc &, IInputActions* dstActions) __NE___
    {
        CHECK_ERR( listener );

        DRC_EXLOCK( _stCheck );
        EXLOCK( _windowsGuard );

        if ( _windows.size() == 1 and not _windows.front().second->_listener )
        {
            auto&   wnd = _windows.front().second;
            wnd->_Init( RVRef(listener), dstActions );
            return wnd;
        }

        RETURN_ERR( "multi-window is not supported yet" );
    }

/*
=================================================
    OpenStorage
=================================================
*/
    RC<IVirtualFileStorage>  ApplicationAndroid::OpenStorage (EAppStorage type) __NE___
    {
        switch_enum( type )
        {
            case EAppStorage::Builtin :
            {
                auto    fs = MakeRC<FileSystemAndroid>();
                CHECK_ERR( fs->Create( _paths->jniAssetMngr, "" ));
                return fs;
            }

            case EAppStorage::Cache :
            {
                Path    dir = _paths->externalCache;
                CHECK_ERR( not dir.empty() );
                return VFS::VirtualFileStorageFactory::CreateDynamicFolder( dir );
            }
        }
        switch_end
    }

/*
=================================================
    GetMonitors
=================================================
*/
    ArrayView<Monitor>  ApplicationAndroid::GetMonitors (bool update) __NE___
    {
        DRC_SHAREDLOCK( _drCheck );
        DRC_EXLOCK( _stCheck );     // for compatibility
        Unused( update );

        return ArrayView<Monitor>{ &_displayInfo, 1 };
    }

/*
=================================================
    GetVulkanInstanceExtensions
=================================================
*/
    ArrayView<const char*>  ApplicationAndroid::GetVulkanInstanceExtensions () __NE___
    {
        return Graphics::VSwapchain::GetInstanceExtensions();
    }

/*
=================================================
    Terminate
=================================================
*/
    void  ApplicationAndroid::Terminate () __NE___
    {
        _isRunning.store( false );

        EXLOCK( _windowsGuard );

        for (auto& obj_wnd : _windows)
        {
            obj_wnd.second->Close();
        }
    }

/*
=================================================
    BeforeUpdate
=================================================
*/
    void  ApplicationAndroid::BeforeUpdate () __NE___
    {
        ApplicationBase::_BeforeUpdate();
    }

/*
=================================================
    AfterUpdate
=================================================
*/
    void  ApplicationAndroid::AfterUpdate () __NE___
    {
        bool    wnd_is_empty;
        {
            EXLOCK( _windowsGuard );

            for (usize i = 0; i < _windows.size();)
            {
                if_likely( _windows[i].second->_wndState != IWindow::EState::Destroyed )
                    ++i;
                else
                    _windows.fast_erase( i );
            }

            wnd_is_empty = _windows.empty();
        }

        ApplicationBase::_AfterUpdate();

        if_unlikely( wnd_is_empty )
        {
            _OnDestroy();
        }
    }

/*
=================================================
    _OnDestroy
=================================================
*/
    void  ApplicationAndroid::_OnDestroy () __NE___
    {
        DRC_EXLOCK( _drCheck );

        ApplicationBase::_Destroy();

        _java.application       = Default;
        _java.assetManager      = Default;
        _paths->jniAssetMngr    = null;

        //_methods.createWindow = Default;

        EXLOCK( _windowsGuard );

        // windows must be destroyed before destroying app
        for (auto& obj_wnd : _windows)
        {
            CHECK( obj_wnd.second->_wndState == IWindow::EState::Destroyed );
            CHECK( obj_wnd.second.use_count() == 1 );
        }
        _windows.clear();
    }

/*
=================================================
    _AddWindow
=================================================
*/
    ApplicationAndroid::WinID  ApplicationAndroid::_AddWindow (SharedPtr<WindowAndroid> wnd) __NE___
    {
        WinID   id;
        {
            EXLOCK( _windowsGuard );

            id = ++_windowCounter;
            _windows.emplace_back( id, wnd );
        }

        if ( not _started and _listener )
        {
            _started = true;
            _listener->OnStart( *this );
        }

        return id;
    }

/*
=================================================
    SetRotation
=================================================
*/
    void  ApplicationAndroid::SetRotation (int orientation) __NE___
    {
        DRC_EXLOCK( _drCheck );

        enum {
            ROTATION_0   = 0x00,
            ROTATION_90  = 0x01,
            ROTATION_180 = 0x02,
            ROTATION_270 = 0x03,
        };
        switch ( orientation )
        {
            case ROTATION_0   :     _displayInfo.orient = Monitor::EOrientation::Orient_0_deg;      break;
            case ROTATION_90  :     _displayInfo.orient = Monitor::EOrientation::Orient_90_deg;     break;
            case ROTATION_180 :     _displayInfo.orient = Monitor::EOrientation::Orient_180_deg;    break;
            case ROTATION_270 :     _displayInfo.orient = Monitor::EOrientation::Orient_270_deg;    break;
        }
    }

/*
=================================================
    native_OnCreate
=================================================
*/
    void JNICALL ApplicationAndroid::native_OnCreate (JNIEnv* env, jclass, jobject appCtx, jobject assetMngr) __NE___
    {
        auto&   app = GetApp();
        DRC_EXLOCK( app._drCheck );

        JavaEnv je{ env };

        app._java.application       = JavaObj{ appCtx, je };
        app._java.assetManager      = JavaObj{ assetMngr, je };
        app._paths->jniAssetMngr    = AAssetManager_fromJava( env, app._java.assetManager.Get() );

        app._java.application.Method( "ShowToast", OUT app._methods.showToast );
        app._java.application.Method( "IsNetworkConnected", OUT app._methods.isNetworkConnected );
        //app._java.application.Method( "CreateWindow", OUT app._methods.createWindow );
    }

/*
=================================================
    native_SetDirectories
=================================================
*/
    void JNICALL ApplicationAndroid::native_SetDirectories (JNIEnv*, jclass,
                                                            jstring internalAppData, jstring internalCache,
                                                            jstring externalAppData, jstring externalCache) __NE___
    {
        auto&   app = GetApp();
        DRC_EXLOCK( app._drCheck );

        app._paths->internalAppData = Path{JavaString{ internalAppData }};
        app._paths->internalCache   = Path{JavaString{ internalCache }};
        app._paths->externalAppData = Path{JavaString{ externalAppData }};
        app._paths->externalCache   = Path{JavaString{ externalCache }};
    }

/*
=================================================
    native_SetDisplayInfo
=================================================
*/
    void JNICALL  ApplicationAndroid::native_SetDisplayInfo (JNIEnv* env, jclass,
                                                             jint minWidth, jint minHeight,
                                                             jint maxWidth, jint maxHeight,
                                                             float dpi, jint orientation,
                                                             float avrLum, float maxLum, float minLum,
                                                             jintArray cutoutRects, const jint cutoutRectCount) __NE___
    {
        auto&   app = GetApp();
        DRC_EXLOCK( app._drCheck );

        auto&   disp = app._displayInfo;

        disp.workArea.pixels    = RectI{ 0, 0, minWidth, minHeight };
        disp.region.pixels      = RectI{ 0, 0, maxWidth, maxHeight };
        disp.ppi                = dpi;
        disp.physicalSize       = disp._CalculatePhysicalSize();
        app.SetRotation( orientation );

        disp.luminance.avr      = Monitor::Luminance_t{ avrLum };
        disp.luminance.max      = Monitor::Luminance_t{ maxLum };
        disp.luminance.min      = Monitor::Luminance_t{ minLum };

        if ( cutoutRectCount > 0 )
        {
            ASSERT( IsMultipleOf( cutoutRectCount, 4 ));

            JavaArray<jint> cutout_rects { cutoutRects, True{"readOnly"}, JavaEnv{env} };

            for (jint i = 0; i < cutoutRectCount; i += 4)
            {
                auto&   dst         = disp.cutout.emplace_back();
                dst.pixels.left     = cutout_rects[i+0];
                dst.pixels.top      = cutout_rects[i+1];
                dst.pixels.right    = cutout_rects[i+2];
                dst.pixels.bottom   = cutout_rects[i+3];
            }
        }

        DEBUG_ONLY( disp.Print() );
    }

/*
=================================================
    native_SetSystemInfo
=================================================
*/
    void JNICALL  ApplicationAndroid::native_SetSystemInfo (JNIEnv*, jclass, jstring iso3Lang0, jstring iso3Lang1) __NE___
    {
        auto&   app = GetApp();
        DRC_EXLOCK( app._drCheck );

        app._locales.clear();
        app._locales.push_back( LocaleName{ JavaString{ iso3Lang0 }.c_str() });
        app._locales.push_back( LocaleName{ JavaString{ iso3Lang1 }.c_str() });
    }

/*
=================================================
    native_SetSystemInfo
=================================================
*/
    void JNICALL  ApplicationAndroid::native_EnableCamera (JNIEnv*, jclass) __NE___
    {
        auto&   app = GetApp();
        DRC_EXLOCK( app._drCheck );

        if ( not app._hwCamera.load() )
        {
            auto    camera = MakeRC<HwCameraAndroid>();
            CHECK_ERRV( camera->Initialize() );

            app._hwCamera.store( RVRef(camera) );
            app._listener->OnEvent( app, EAppEvent::CameraEnabled );
        }
    }

/*
=================================================
    OnJniLoad
=================================================
*/
    jint ApplicationAndroid::OnJniLoad (JavaVM* vm) __NE___
    {
        using namespace AE::Java;

        JNIEnv* env;
        if ( vm->GetEnv( OUT reinterpret_cast<void**>(&env), JavaEnv::Version ) != JNI_OK )
            return -1;

        JavaEnv::SetVM( vm );

        // register application native methods
        {
            JavaClass   app_class{ "AE/engine/BaseApplication" };
            CHECK_ERR( app_class );

            app_class.RegisterStaticMethod( "native_OnCreate",          &ApplicationAndroid::native_OnCreate );
            app_class.RegisterStaticMethod( "native_SetDirectories",    &ApplicationAndroid::native_SetDirectories );
            app_class.RegisterStaticMethod( "native_SetDisplayInfo",    &ApplicationAndroid::native_SetDisplayInfo );
            app_class.RegisterStaticMethod( "native_SetSystemInfo",     &ApplicationAndroid::native_SetSystemInfo );
            app_class.RegisterStaticMethod( "native_EnableCamera",      &ApplicationAndroid::native_EnableCamera );
        }

        // register activity native methods
        {
            JavaClass   wnd_class{ "AE/engine/BaseActivity" };
            CHECK_ERR( wnd_class );

            wnd_class.RegisterStaticMethod( "native_OnCreate",              &WindowAndroid::native_OnCreate );
            wnd_class.RegisterStaticMethod( "native_OnDestroy",             &WindowAndroid::native_OnDestroy );
            wnd_class.RegisterStaticMethod( "native_OnStart",               &WindowAndroid::native_OnStart );
            wnd_class.RegisterStaticMethod( "native_OnStop",                &WindowAndroid::native_OnStop );
            wnd_class.RegisterStaticMethod( "native_OnEnterForeground",     &WindowAndroid::native_OnEnterForeground );
            wnd_class.RegisterStaticMethod( "native_OnEnterBackground",     &WindowAndroid::native_OnEnterBackground );
            wnd_class.RegisterStaticMethod( "native_SurfaceChanged",        &WindowAndroid::native_SurfaceChanged );
            wnd_class.RegisterStaticMethod( "native_SurfaceDestroyed",      &WindowAndroid::native_SurfaceDestroyed );
            wnd_class.RegisterStaticMethod( "native_Update",                &WindowAndroid::native_Update );
            wnd_class.RegisterStaticMethod( "native_OnKey",                 &WindowAndroid::native_OnKey );
            wnd_class.RegisterStaticMethod( "native_OnTouch",               &WindowAndroid::native_OnTouch );
            wnd_class.RegisterStaticMethod( "native_OnOrientationChanged",  &WindowAndroid::native_OnOrientationChanged );
            wnd_class.RegisterStaticMethod( "native_UpdateSensor",          &WindowAndroid::native_UpdateSensor );
        }

        CHECK( ApplicationAndroid::_GetAppInstance() != null );

        return JavaEnv::Version;
    }

/*
=================================================
    OnJniUnload
=================================================
*/
    void ApplicationAndroid::OnJniUnload (JavaVM* vm) __NE___
    {
        using namespace AE::Java;

        auto&   app = ApplicationAndroid::_GetAppInstance();
        delete app;
        app = null;

        AE_OnAppDestroyed();

        JavaEnv::SetVM( null );
    }


} // AE::App

#endif // AE_PLATFORM_ANDROID
