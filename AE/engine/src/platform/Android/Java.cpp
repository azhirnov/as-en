// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Android/Java.h"

#ifdef AE_PLATFORM_ANDROID

namespace AE::Java
{

/*
=================================================
    GetJavaVM
=================================================
*/
namespace {
    ND_ JavaVM*&  GetJavaVM () __NE___
    {
        static JavaVM*  jvm = null;
        return jvm;
    }
}

/*
=================================================
    constructor
=================================================
*/
    JavaEnv::JavaEnv (JNIEnv *env) __NE___ :
        _env{ env }
    {
        Attach();
    }

    JavaEnv::JavaEnv () __NE___
    {
        Attach();
    }

/*
=================================================
    destructor
=================================================
*/
    JavaEnv::~JavaEnv () __NE___
    {
        Detach();
    }

/*
=================================================
    Attach
=================================================
*/
    void JavaEnv::Attach () __NE___
    {
        if ( _env )
            return; // already attached

        JavaVM* jvm = GetJavaVM();
        if ( not jvm )
        {
            DBG_WARNING( "JavaVM is null" );
            return;
        }

        void*   env     = null;
        jint    status  = jvm->GetEnv( OUT &env, Version );

        _env = Cast<JNIEnv>( env );

        if ( status == JNI_OK and _env )
        {
            _mustBeDetached = false;
            return;
        }

        if ( status == JNI_EDETACHED )
        {
            if ( jvm->AttachCurrentThread( &_env, null ) < 0 )
            {
                DBG_WARNING( "can't attach to current java thread" );
                return;
            }
            _mustBeDetached = true;
            return;
        }

        DBG_WARNING( "unknown error" );
    }

/*
=================================================
    Detach
=================================================
*/
    void JavaEnv::Detach () __NE___
    {
        if ( _mustBeDetached )
        {
            JavaVM* jvm = GetJavaVM();
            if ( not jvm )
            {
                DBG_WARNING( "JavaVM is null" );
                return;
            }

            jvm->DetachCurrentThread();

            _env            = null;
            _mustBeDetached = false;
        }
    }

/*
=================================================
    SetVM
=================================================
*/
    void JavaEnv::SetVM (JavaVM *ptr) __NE___
    {
        ASSERT( GetJavaVM() == null );

        GetJavaVM() = ptr;
    }

/*
=================================================
    ThrowException
=================================================
*/
    void JavaEnv::ThrowException (NtStringView msg) C_NE___
    {
        CHECK_ERRV( _env );

        JavaClass   jc{ "java/lang/Error" };
        _env->ThrowNew( jc.Get(), msg.c_str() );
    }

/*
=================================================
    ExceptionClear
=================================================
*/
    void JavaEnv::ExceptionClear () C_NE___
    {
        CHECK_ERRV( _env );

        _env->ExceptionClear();
    }

/*
=================================================
    HasException
=================================================
*/
    bool JavaEnv::HasException () C_NE___
    {
        CHECK_ERR( _env, false );

        return _env->ExceptionCheck() == JNI_TRUE;
    }


} // AE::Java

#endif // AE_PLATFORM_ANDROID
