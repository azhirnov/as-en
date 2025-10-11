// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_ANDROID
# include "platform/Android/AndroidCommon.h"

namespace AE::Java
{
	INTERNAL_LINKAGE( JavaVM*	s_JavaVM = null );

/*
=================================================
	constructor
=================================================
*/
	JavaEnv::JavaEnv (JNIEnv* env) __NE___ :
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
		if ( _env != null )
			return;	// already attached

		JavaVM*	jvm = GetJavaVM();
		if ( jvm == null )
		{
			DBG_WARNING( "JavaVM is null" );
			return;
		}

		void*	env		= null;
		jint	status	= jvm->GetEnv( OUT &env, Version );

		_env = Cast<JNIEnv>( env );

		if ( status == JNI_OK and _env != null )
		{
			_mustBeDetached = false;
			return;
		}

		if ( status == JNI_EDETACHED )
		{
			if ( jvm->AttachCurrentThread( OUT &_env, null ) < 0 )
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
			JavaVM*	jvm = GetJavaVM();
			if ( jvm == null )
			{
				DBG_WARNING( "JavaVM is null" );
				return;
			}

			jvm->DetachCurrentThread();

			_env			= null;
			_mustBeDetached	= false;
		}
	}

/*
=================================================
	SetVM
=================================================
*/
	void JavaEnv::SetVM (JavaVM* ptr) __NE___
	{
		ASSERT( s_JavaVM == null );

		s_JavaVM = ptr;
	}

/*
=================================================
	ThrowException
=================================================
*/
	void JavaEnv::ThrowException (NtStringView msg) C_NE___
	{
		CHECK_ERRV( _env != null );

		JavaClass	jc{ "java/lang/Error" };
		_env->ThrowNew( jc.Get(), msg.c_str() );
	}

/*
=================================================
	ExceptionClear
=================================================
*/
	void JavaEnv::ExceptionClear () C_NE___
	{
		CHECK_ERRV( _env != null );

		_env->ExceptionClear();
	}

/*
=================================================
	HasException
=================================================
*/
	bool JavaEnv::HasException () C_NE___
	{
		CHECK_ERR( _env != null );
		return _env->ExceptionCheck() == JNI_TRUE;
	}

/*
=================================================
	GetJavaVM
=================================================
*/
	JavaVM*  JavaEnv::GetJavaVM () __NE___
	{
		return s_JavaVM;
	}


} // AE::Java

#endif // AE_PLATFORM_ANDROID
