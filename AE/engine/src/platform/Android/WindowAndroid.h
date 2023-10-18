// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Android/AndroidCommon.h"

#ifdef AE_PLATFORM_ANDROID
# include "platform/Public/Common.h"
# include "platform/Private/WindowBase.h"
# include "platform/Android/Java.h"
# include "platform/Android/InputActionsAndroid.h"
# include "platform/Private/WindowSurface.h"

namespace AE::App
{
    using namespace AE::Java;


    //
    // Android Window
    //

    class WindowAndroid final : public WindowBase
    {
        friend class ApplicationAndroid;

    // types
    private:
        using WinID = jint;


    // variables
    private:
        uint2                   _surfSize;
        InputActionsAndroid     _input;

        struct {
            JavaObj                 window;
            ANativeWindow*          nativeWindow    = null;
        }                       _java;
        struct {
            JavaMethod< void () >   close;
        }                       _methods;


    // methods
    public:
        WindowAndroid ()                                __NE___;


    // IWindow //
        void            Close ()                        __NE_OV;

        uint2           GetSurfaceSize ()               C_NE_OV;
        Monitor         GetMonitor ()                   C_NE_OV;

        IInputActions&  InputActions ()                 __NE_OV { return _input; }
        NativeWindow    GetNative ()                    C_NE_OV;

        // desktop only
        void  SetSize (const uint2 &)                   __NE_OV {}
        void  SetPosition (const int2 &)                __NE_OV {}
        void  SetPosition (Monitor::ID, const int2 &)   __NE_OV {}
        void  SetTitle (NtStringView)                   __NE_OV {}
        void  SetFocus ()                               C_NE_OV {}
        bool  SetMode (EWindowMode, Monitor::ID)        __NE_OV { return false; }


    private:
        void  _Init (Unique<IWndListener>   listener,
                     IInputActions*         dstActions) __NE___;

        ND_ ApplicationAndroid&  _App ()                __NE___;


    // called from java
    private:
        static WinID JNICALL  native_OnCreate (JNIEnv*, jclass, jobject wnd)                                            __NE___;
        static void  JNICALL  native_OnDestroy (JNIEnv*, jclass, WinID wndId)                                           __NE___;
        static void  JNICALL  native_OnStart (JNIEnv*, jclass, WinID wndId)                                             __NE___;
        static void  JNICALL  native_OnStop (JNIEnv*, jclass, WinID wndId)                                              __NE___;
        static void  JNICALL  native_OnEnterForeground (JNIEnv*, jclass, WinID wndId)                                   __NE___;
        static void  JNICALL  native_OnEnterBackground (JNIEnv*, jclass, WinID wndId)                                   __NE___;
        static void  JNICALL  native_SurfaceChanged (JNIEnv*, jclass, WinID wndId, jobject surface)                     __NE___;
        static void  JNICALL  native_SurfaceDestroyed (JNIEnv*, jclass, WinID wndId)                                    __NE___;
        static jint  JNICALL  native_Update (JNIEnv*, jclass, WinID wndId)                                              __NE___;
        static void  JNICALL  native_OnKey (JNIEnv*, jclass, WinID wndId, jint keycode, jint action, jint count)        __NE___;
        static void  JNICALL  native_OnTouch (JNIEnv*, jclass, WinID wndId, jint action, jint changedIndex,
                                              jint touchCount, jfloatArray touchData)                                   __NE___;
        static void  JNICALL  native_OnOrientationChanged (JNIEnv*, jclass, WinID wndId, jint newOrientation)           __NE___;
        static void  JNICALL  native_UpdateSensor (JNIEnv*, jclass, WinID wndId, int sensor, jfloatArray values)        __NE___;
    };


} // AE::App

#endif // AE_PLATFORM_ANDROID
