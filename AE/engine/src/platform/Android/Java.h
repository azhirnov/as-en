// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	https://developer.android.com/training/articles/perf-jni
*/

#pragma once

#ifdef AE_PLATFORM_ANDROID
# include "platform/Public/Common.h"

namespace AE::Java
{
	using namespace AE::Base;

	class JavaObj;

	template <typename Fn>
	class JavaStaticMethod;

	template <typename Fn>
	class JavaMethod;

	namespace _hidden_
	{
		template <typename T>
		struct JavaMethodResult;
	}



	//
	// Java Environment
	//

	class JavaEnv : public Noncopyable
	{
	public:
		static constexpr int	Version = JNI_VERSION_1_6;


	// variables
	private:
		JNIEnv *	_env			= null;
		bool		_mustBeDetached	= false;


	// methods
	public:
		explicit JavaEnv (JNIEnv* env)					__NE___;
		JavaEnv ()										__NE___;
		~JavaEnv ()										__NE___;

			void  Attach ()								__NE___;
			void  Detach ()								__NE___;

			void  ThrowException (NtStringView msg)		C_NE___;
			void  ExceptionClear ()						C_NE___;
		ND_ bool  HasException ()						C_NE___;

		ND_ JNIEnv*  Env ()								C_NE___	{ ASSERT( _env );  return _env; }
		ND_ JNIEnv*  operator -> ()						C_NE___	{ ASSERT( _env );  return _env; }

			static void  SetVM (JavaVM* ptr)			__NE___;
	};



	//
	// Java Array
	//

	template <typename T>
	struct JavaArray {};

	#define JNI_ARRAY_TYPE( _type_, _jtype_ )															\
		template <>																						\
		class JavaArray< _type_ >																		\
		{																								\
		/* types */																						\
		public:																							\
			using Value_t	= _type_;																	\
			using jarray_t	= _type_ ## Array;															\
			using Self		= JavaArray< _type_ >;														\
																										\
																										\
		/* variables */																					\
		private:																						\
			_type_*		_data			= null;															\
			jarray_t	_jarray			= null;															\
			usize		_count			= 0;															\
			bool		_readOnly		= false;														\
			bool		_fromNative		= false;														\
																										\
																										\
		/* methods */																					\
		public:																							\
			JavaArray() __NE___ = default;																\
																										\
			explicit JavaArray (jarray_t arr, Bool readOnly = False{}, const JavaEnv &je = {}) __NE___ :\
				_jarray{ arr }, _readOnly{ readOnly }													\
			{																							\
				_data	= je->Get ## _jtype_ ## ArrayElements( _jarray, 0 );							\
				_count	= je->GetArrayLength( _jarray );												\
			}																							\
																										\
			explicit JavaArray (ArrayView<_type_> arr, const JavaEnv &je = {}) __NE___ :				\
				_fromNative{ true }																		\
			{																							\
				_jarray	= je->New ## _jtype_ ## Array( arr.size() );									\
				_data	= je->Get ## _jtype_ ## ArrayElements( _jarray, null );							\
				_count	= je->GetArrayLength( _jarray );												\
																										\
				ASSERT( arr.size() == _count );															\
				for (usize i = 0; i < _count; ++i) {													\
					_data[i] = arr[i];																	\
				}																						\
			}																							\
																										\
			JavaArray (Self&& other) __NE___ :															\
				_fromNative{ other._fromNative }														\
			{																							\
				std::swap( _data,     other._data );													\
				std::swap( _jarray,   other._jarray );													\
				std::swap( _count,    other._count );													\
				std::swap( _readOnly, other._readOnly );												\
			}																							\
																										\
			JavaArray (const Self &) = delete;															\
																										\
			~JavaArray () __NE___																		\
			{																							\
				_Release();																				\
			}																							\
																										\
			Self&  operator = (const Self &rhs)	= delete;												\
																										\
			Self&  operator = (Self &&rhs) __NE___														\
			{																							\
				_Release();																				\
				std::swap( _data,     rhs._data );														\
				std::swap( _jarray,   rhs._jarray );													\
				std::swap( _count,    rhs._count );														\
				std::swap( _readOnly, rhs._readOnly );													\
				_fromNative = rhs._fromNative;															\
				return *this;																			\
			}																							\
																										\
			ND_ const _type_&	operator [] (usize i) C_NE___											\
			{																							\
				ASSERT( i < _count );																	\
				return _data[i];																		\
			}																							\
																										\
			ND_ operator ArrayView<_type_> ()	C_NE___	{ return { _data, _count }; }					\
																										\
			ND_ const _type_*	data ()			C_NE___	{ return _data; }								\
			ND_ usize			size ()			C_NE___	{ return _count; }								\
																										\
			ND_ bool			IsReadOnly ()	C_NE___	{ return _readOnly; }							\
																										\
			ND_ const _type_*	begin ()		C_NE___	{ return _data; }								\
			ND_ const _type_*	end ()			C_NE___	{ return _data + _count; }						\
																										\
			ND_ jarray_t		Get ()			C_NE___	{ return _jarray; }								\
																										\
		private:																						\
			void  _Release () __NE___																	\
			{																							\
				if ( _fromNative )																		\
				{																						\
					if ( _jarray )																		\
						JavaEnv{}->DeleteLocalRef( _jarray );											\
				}																						\
				else																					\
				if ( _data and _jarray ) {																\
					JavaEnv{}->Release ## _jtype_ ## ArrayElements( _jarray, _data,						\
															_readOnly ? JNI_ABORT : 0 );				\
				}																						\
				_data		= null;																		\
				_jarray		= null;																		\
				_count		= 0;																		\
				_readOnly	= false;																	\
			}																							\
		}

	JNI_ARRAY_TYPE( jbyte,		Byte );
	JNI_ARRAY_TYPE( jchar,		Char );
	JNI_ARRAY_TYPE( jshort,		Short );
	JNI_ARRAY_TYPE( jint,		Int );
	JNI_ARRAY_TYPE( jlong,		Long );
	JNI_ARRAY_TYPE( jfloat,		Float );
	JNI_ARRAY_TYPE( jdouble,	Double );
	#undef JNI_ARRAY_TYPE



	//
	// UTF8 String
	//

	struct JavaString
	{
	// variables
	private:
		const char*		_data		= null;
		usize			_length		= 0;
		jstring			_jstr		= null;
		bool			_fromNative	= false;	// true if created in native code


	// methods
	public:
		JavaString ()													__NE___	{}
		JavaString (JavaString &&other)									__NE___;
		JavaString (const JavaString &)									= delete;

		explicit JavaString (jstring str, const JavaEnv &je = {})		__NE___;
		explicit JavaString (NtStringView str, const JavaEnv &je = {})	__NE___;

		~JavaString ()													__NE___	{ _Release(); }

		JavaString&  operator = (const JavaString &)					= delete;
		JavaString&  operator = (JavaString &&rhs)						__NE___;

		ND_ jstring			Get ()										C_NE___	{ return _jstr; }
		ND_ const char *	c_str ()									C_NE___	{ return _data; }
		ND_ usize			length ()									C_NE___	{ return _length; }
		ND_ usize			size ()										C_NE___	{ return _length; }

		ND_ operator StringView ()										C_NE___	{ return StringView{ c_str(), length() }; }
		ND_ explicit operator String ()									C_Th___	{ return String{ c_str(), length() }; }
		ND_ explicit operator Path ()									C_Th___	{ return Path{StringView{ c_str(), length() }}; }

	private:
		void  _Release ()												__NE___;
	};



	//
	// Java Class
	//

	class JavaClass
	{
	// variables
	private:
		jclass	_class = null;


	// methods
	public:
		explicit JavaClass (NtStringView className)													__NE___;
		explicit JavaClass (jclass jc, const JavaEnv &je = {})										__NE___	{ _Set( je, jc ); }
		explicit JavaClass (const JavaObj &)														__NE___;
		JavaClass (JavaClass&& jc) 																	__NE___ : _class{jc.Get()} { jc._class = null; }
		JavaClass (const JavaClass &jc)																__NE___	{ _Set( JavaEnv{}, jc.Get() ); }
		JavaClass ()																				__NE___	= default;
		~JavaClass()																				__NE___	{ _Release(); }

		template <typename T>
		explicit JavaClass (InPlaceType<T>)															__NE___;

		JavaClass&  operator = (const JavaClass &)													__NE___;
		JavaClass&  operator = (JavaClass&&)														__NE___;

		ND_ jclass  Get ()																			C_NE___	{ return _class; }

		ND_ explicit operator bool ()																C_NE___	{ return _class != null; }

		template <typename Fn>
		ND_ JavaStaticMethod<Fn>  StaticMethod (NtStringView fnName)								C_NE___;

		template <typename Fn>
		bool  StaticMethod (NtStringView name, OUT JavaStaticMethod<Fn> &m)							C_NE___;

		template <typename Ret, typename ...Args>
		bool  RegisterMethod (NtStringView name, Ret (JNICALL *fn)(JNIEnv*, jobject, Args...))		C_NE___;

		template <typename Ret, typename ...Args>
		bool  RegisterStaticMethod (NtStringView name, Ret (JNICALL *fn)(JNIEnv*, jclass, Args...))	C_NE___;

	private:
		void  _Set (const JavaEnv &, jclass)														__NE___;
		void  _Release ()																			__NE___;
	};



	//
	// Java Object
	//

	class JavaObj
	{
	// variables
	private:
		jobject		_obj	= null;


	// methods
	public:
		JavaObj ()																__NE___	= default;
		JavaObj (JavaObj&& jo) 													__NE___	: _obj{jo._obj} { jo._obj = null; }
		JavaObj (const JavaObj &jo)												__NE___	{ _Set( JavaEnv{}, jo.Get() ); }
		explicit JavaObj (jobject jo, const JavaEnv &je = {})					__NE___	{ _Set( je, jo ); }
		~JavaObj ()																__NE___	{ _Release(); }

		template <typename ...Args>
		explicit JavaObj (const JavaClass&, const Args&... args)				__NE___;

		JavaObj&  operator = (const JavaObj &)									__NE___;
		JavaObj&  operator = (JavaObj &&)										__NE___;

		ND_ jobject  Get ()														C_NE___	{ return _obj; }

		ND_ explicit operator bool ()											C_NE___	{ return _obj != null; }

		template <typename Fn>
		ND_ JavaMethod<Fn>  Method (NtStringView name)							C_NE___;

		template <typename Fn>
		bool  Method (NtStringView name, OUT JavaMethod<Fn> &m)					C_NE___;

		template <typename Fn>
		ND_ JavaStaticMethod<Fn>  StaticMethod (NtStringView name)				C_NE___;

		template <typename Fn>
		bool  StaticMethod (NtStringView name, OUT JavaStaticMethod<Fn> &m)		C_NE___;

	private:
		void  _Set (const JavaEnv &, jobject)									__NE___;
		void  _Release ()														__NE___;
	};



	//
	// JavaStaticMethod
	//

	template <typename Ret, typename ...Args>
	class JavaStaticMethod< Ret (Args...) >
	{
	// types
	public:
		using Self			= JavaStaticMethod< Ret (Args...) >;
		using Signature_t	= Ret (Args...);


	// variables
	private:
		JavaClass	_class;
		jmethodID	_method	= null;


	// methods
	public:
		JavaStaticMethod ()											__NE___	= default;
		JavaStaticMethod (const JavaClass& jc, jmethodID method)	__NE___	: _class{jc}, _method{method} {}

		template <typename ...ArgTypes>
		auto  operator () (const ArgTypes&... args)					C_NE___ -> Java::_hidden_::JavaMethodResult<Ret>;

		ND_ explicit operator bool ()								C_NE___	{ return _method != null; }
	};



	//
	// Java Method
	//

	template <typename Ret, typename ...Args>
	class JavaMethod< Ret (Args...) >
	{
	// types
	public:
		using Self			= JavaMethod< Ret (Args...) >;
		using Signature_t	= Ret (Args...);


	// variables
	private:
		JavaObj		_obj;
		jmethodID	_method	= null;


	// methods
	public:
		JavaMethod ()											__NE___	= default;
		JavaMethod (const JavaObj& obj, jmethodID method)		__NE___	: _obj{obj}, _method{method} {}

		template <typename ...ArgTypes>
		auto  operator () (const ArgTypes&... args)				C_NE___	-> Java::_hidden_::JavaMethodResult<Ret>;

		ND_ explicit operator bool ()							C_NE___	{ return _method != null; }
	};



//-----------------------------------------------------------------------------
// JavaString

/*
=================================================
	constructor
=================================================
*/
	inline JavaString::JavaString (jstring str, const JavaEnv &je) __NE___
	{
		_jstr	= str;
		_data	= je->GetStringUTFChars( str, 0 );
		_length	= je->GetStringUTFLength( str );
	}

	inline JavaString::JavaString (NtStringView str, const JavaEnv &je) __NE___ : _fromNative{ true }
	{
		_jstr = je->NewStringUTF( str.c_str() );
	}

	inline JavaString::JavaString (JavaString &&other) __NE___ : _fromNative{ other._fromNative }
	{
		std::swap( _data,   other._data );
		std::swap( _jstr,   other._jstr );
		std::swap( _length, other._length );
	}

/*
=================================================
	operator =
=================================================
*/
	inline JavaString&  JavaString::operator = (JavaString &&rhs) __NE___
	{
		_Release();
		std::swap( _data,   rhs._data );
		std::swap( _jstr,   rhs._jstr );
		std::swap( _length, rhs._length );
		_fromNative = rhs._fromNative;
		return *this;
	}

/*
=================================================
	_Release
=================================================
*/
	inline void JavaString::_Release () __NE___
	{
		JavaEnv je;
		if ( _fromNative )
		{
			if ( _jstr )
				je->DeleteLocalRef( _jstr );
		}
		else
		if ( _data and _jstr )
			je->ReleaseStringUTFChars( _jstr, _data );

		_data	= null;
		_length	= 0;
		_jstr	= null;
	}



//-----------------------------------------------------------------------------
// JavaClass

/*
=================================================
	constructor
=================================================
*/
	inline JavaClass::JavaClass (NtStringView className) __NE___
	{
		JavaEnv	je;
		jclass	jc = je->FindClass( className.c_str() );

		if ( not jc )
		{
			CHECK_MSG( false, "java class is not found" );
			return;
		}
		_Set( je, jc );
	}

	inline JavaClass::JavaClass (const JavaObj& obj) __NE___
	{
		if ( not obj )
		{
			CHECK_MSG( false, "java object is null" );
			return;
		}

		JavaEnv	je;
		jclass	jc = je->GetObjectClass( obj.Get() );

		if ( not jc )
		{
			CHECK_MSG( false, "failed to get object class" );
			return;
		}
		_Set( je, jc );
	}

/*
=================================================
	operator =
=================================================
*/
	inline JavaClass&  JavaClass::operator = (const JavaClass& jc) __NE___
	{
		_Release();
		_Set( JavaEnv{}, jc.Get() );
		return *this;
	}

	inline JavaClass&  JavaClass::operator = (JavaClass&& jc) __NE___
	{
		_Release();
		_class		= jc._class;
		jc._class	= null;
		return *this;
	}

/*
=================================================
	_Set
=================================================
*/
	inline void JavaClass::_Set (const JavaEnv &je, jclass jc) __NE___
	{
		if ( jc )
			_class = static_cast<jclass>( je->NewGlobalRef( jc ));
	}

/*
=================================================
	_Release
=================================================
*/
	inline void JavaClass::_Release () __NE___
	{
		if ( _class )
		{
			JavaEnv{}->DeleteGlobalRef( _class );
			_class = null;
		}
	}



//-----------------------------------------------------------------------------
// JavaObj

/*
=================================================
	operator =
=================================================
*/
	inline JavaObj&  JavaObj::operator = (const JavaObj& jo) __NE___
	{
		_Release();
		_Set( JavaEnv{}, jo.Get() );
		return *this;
	}

	inline JavaObj&  JavaObj::operator = (JavaObj&& jo) __NE___
	{
		_Release();
		_obj	= jo.Get();
		jo._obj	= null;
		return *this;
	}

/*
=================================================
	_Set
=================================================
*/
	inline void JavaObj::_Set (const JavaEnv &je, jobject jo) __NE___
	{
		if ( jo )
			_obj = je->NewGlobalRef( jo );
	}

/*
=================================================
	_Release
=================================================
*/
	inline void JavaObj::_Release () __NE___
	{
		if ( _obj )
		{
			JavaEnv{}->DeleteGlobalRef( _obj );
			_obj = null;
		}
	}


//-----------------------------------------------------------------------------
// JavaMethodSignature

	namespace _hidden_
	{
		//
		// Jni Type Name
		//

		template <typename T>
		struct JniTypeName;

		#define JNI_TYPE_NAME( _typeName_, _signature_ )							\
			template <>																\
			struct JniTypeName< _typeName_ >										\
			{																		\
				using type = _typeName_;											\
				static void Append (INOUT String &s) __Th___ { s += _signature_; }	\
			}

		JNI_TYPE_NAME( void,			'V' );
		JNI_TYPE_NAME( jboolean,		'Z' );
		JNI_TYPE_NAME( jbyte,			'B' );
		JNI_TYPE_NAME( jchar,			'C' );
		JNI_TYPE_NAME( jshort,			'S' );
		JNI_TYPE_NAME( jint,			'I' );
		JNI_TYPE_NAME( jlong,			'J' );
		JNI_TYPE_NAME( jfloat,			'F' );
		JNI_TYPE_NAME( jdouble,			'D' );

		JNI_TYPE_NAME( jstring,			"Ljava/lang/String;"	);
		JNI_TYPE_NAME( jobject,			"Ljava/lang/Object;"	);
		JNI_TYPE_NAME( jthrowable,		"Ljava/lang/Throwable;"	);

		JNI_TYPE_NAME( jbyteArray,		"[B" );
		JNI_TYPE_NAME( jcharArray,		"[C" );
		JNI_TYPE_NAME( jshortArray,		"[S" );
		JNI_TYPE_NAME( jintArray,		"[I" );
		JNI_TYPE_NAME( jlongArray,		"[J" );
		JNI_TYPE_NAME( jfloatArray,		"[F" );
		JNI_TYPE_NAME( jdoubleArray,	"[D" );
		#undef JNI_TYPE_NAME

		template <typename T>
		struct JniTypeName< T* >
		{
			using type = T*;
			static void  Append (INOUT String &s) __Th___ { s += '['; JniTypeName<T>::Append( INOUT s ); }
		};



		//
		// Java Method Signature
		//

		template <typename Fn>
		class JavaMethodSignature;

		template <typename Ret, typename ...Args>
		class JavaMethodSignature< Ret (Args...) >
		{
		// variables
		private:
			String	_sig;


		// methods
		public:
			JavaMethodSignature ()				__NE___
			{
				TRY{
					_sig += '(';

					if constexpr( CountOf< Args... >() > 0 )
						_Append<Args...>();

					_sig += ')';
					JniTypeName<Ret>::Append( INOUT _sig );
				}
				CATCH_ALL(
					_sig.clear();
				)
			}

			ND_ String const&  signature ()		C_NE___	{ return _sig; }

		private:
			template <typename Arg0, typename ...ArgsN>
			void  _Append ()					__NE___
			{
				JniTypeName<Arg0>::Append( INOUT _sig );

				if constexpr( CountOf< ArgsN... >() > 0 )
					_Append<ArgsN...>();
			}
		};

	} // _hidden_



//-----------------------------------------------------------------------------
// JavaClass

/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	JavaClass::JavaClass (InPlaceType<T>) __NE___
	{
		String	class_name;
		Java::_hidden_::JniTypeName<T>::Append( INOUT class_name );

		// remove first 'L'
		if ( class_name.length() > 0 and class_name.front() == 'L' ) {
			class_name = class_name.substr( 1 );
		}
		// remove last ';'
		if ( class_name.length() and class_name.back() == ';' ) {
			class_name.erase( class_name.end() - 1 );
		}

		JavaEnv	je;
		jclass	jc = je->FindClass( class_name.c_str() );

		if ( not jc ) {
			CHECK( !"java class is not found" );
			return;
		}
		_Set( je, jc );
	}

/*
=================================================
	RegisterStaticMethod
=================================================
*/
	template <typename Ret, typename ...Args>
	bool  JavaClass::RegisterStaticMethod (NtStringView name, Ret (JNICALL *fn)(JNIEnv*, jclass, Args...)) C_NE___
	{
		CHECK_ERR( _class );

		JavaEnv	je;
		Java::_hidden_::JavaMethodSignature<Ret (Args...)> sig;

		JNINativeMethod	info = {
			name.c_str(),
			sig.signature().c_str(),
			reinterpret_cast<void*>( fn )
		};

		CHECK_ERR( je->RegisterNatives( _class, &info, 1 ) == 0 );
		return true;
	}

/*
=================================================
	RegisterMethod
=================================================
*/
	template <typename Ret, typename ...Args>
	bool  JavaClass::RegisterMethod (NtStringView name, Ret (JNICALL *fn)(JNIEnv*, jobject, Args...)) C_NE___
	{
		CHECK_ERR( _class );

		JavaEnv	je;
		Java::_hidden_::JavaMethodSignature<Ret (Args...)> sig;
		JNINativeMethod info = {
			name.c_str(),
			sig.signature().c_str(),
			reinterpret_cast<void*>( fn )
		};

		CHECK_ERR( je->RegisterNatives( _class, &info, 1 ) == 0 );
		return true;
	}

/*
=================================================
	StaticMethod
=================================================
*/
	template <typename Fn>
	JavaStaticMethod<Fn>  JavaClass::StaticMethod (NtStringView name) C_NE___
	{
		JavaStaticMethod<Fn>	result;
		StaticMethod( name, OUT result );
		return result;
	}

	template <typename Fn>
	bool  JavaClass::StaticMethod (NtStringView name, OUT JavaStaticMethod<Fn> &m) C_NE___
	{
		CHECK_ERR( _class );

		JavaEnv	je;
		Java::_hidden_::JavaMethodSignature<Fn> sig;

		jmethodID	m_id = je->GetStaticMethodID( _class, name.c_str(), sig.signature().c_str() );
		CHECK_ERR( m_id != 0 );

		m = JavaStaticMethod<Fn>{ *this, m_id };
		return true;
	}



//-----------------------------------------------------------------------------
// JavaObj

/*
=================================================
	constructor
=================================================
*/
	template <typename ...Args>
	JavaObj::JavaObj (const JavaClass& jc, const Args&... args) __NE___
	{
		if ( not jc ) {
			CHECK( !"invalid java class" );
			return;
		}

		JavaEnv	je;
		Java::_hidden_::JavaMethodSignature<void (Args...)> sig;

		jmethodID	ctor_id = je->GetMethodID( jc.Get(), "<init>", sig.signature().c_str() );
		if ( not ctor_id ) {
			CHECK( !"constructor doesn't exists" );
			return;
		}

		jobject		jo = je->NewObjectV( jc.Get(), FwdArg<Args>(args)... );
		if ( not jo ) {
			CHECK( !"failed to create java object" );
			return;
		}

		_Set( je, jo );
	}

/*
=================================================
	StaticMethod
=================================================
*/
	template <typename Fn>
	JavaStaticMethod<Fn>  JavaObj::StaticMethod (NtStringView name) C_NE___
	{
		return JavaClass{ *this }.StaticMethod<Fn>( name );
	}

	template <typename Fn>
	bool  JavaObj::StaticMethod (NtStringView name, OUT JavaStaticMethod<Fn> &m) C_NE___
	{
		return JavaClass{ *this }.StaticMethod( name, OUT m );
	}

/*
=================================================
	Method
=================================================
*/
	template <typename Fn>
	JavaMethod<Fn>  JavaObj::Method (NtStringView name) C_NE___
	{
		JavaMethod<Fn>	result;
		Method( name, OUT result );
		return result;
	}

	template <typename Fn>
	bool  JavaObj::Method (NtStringView name, OUT JavaMethod<Fn> &m) C_NE___
	{
		CHECK_ERR( _obj );

		JavaEnv	je;
		Java::_hidden_::JavaMethodSignature<Fn> sig;

		jmethodID	m_id = je->GetMethodID( JavaClass(*this).Get(), name.c_str(), sig.signature().c_str() );
		CHECK_ERR( m_id != 0 );

		m = JavaMethod<Fn>{ *this, m_id };
		return true;
	}


//-----------------------------------------------------------------------------
// JavaMethodCaller

	namespace _hidden_
	{
		//
		// JavaMethodResult
		//

		template <typename T>
		struct ND_ JavaMethodResult
		{
			Optional<T>		_value;

			JavaMethodResult ()				__NE___	{}
			JavaMethodResult (T val)		__NE___	: _value{val} {}

			ND_ explicit operator bool ()	C_NE___	{ return _value.has_value(); }
			ND_ T		 operator * ()		C_NE___	{ return _value.value_or( T{} ); }
		};

		template <>
		struct JavaMethodResult<void>
		{
			const bool	_succeeded	= false;

			JavaMethodResult ()							__NE___	{}
			explicit JavaMethodResult (bool succeeded)	__NE___	: _succeeded{succeeded} {}

			ND_ explicit operator bool ()				C_NE___	{ return _succeeded; }
		};


		//
		// Java Method Caller
		//

		template <typename Ret>
		struct JavaMethodCaller;

		template <>
		struct JavaMethodCaller<void>
		{
			template <typename ...Args>
			static JavaMethodResult<void>  CallStatic (const JavaClass &jc, jmethodID method, Args&&... args) __NE___
			{
				JavaEnv	je;
				ASSERT( not je.HasException() );
				je->CallStaticVoidMethod( jc.Get(), method, FwdArg<Args>(args)... );
				CHECK_ERR( not je.HasException() );
				return JavaMethodResult<void>(true);
			}

			template <typename ...Args>
			static JavaMethodResult<void>  CallNonvirtual (const JavaObj &obj, const JavaClass &jc, jmethodID method, Args&&... args) __NE___
			{
				JavaEnv	je;
				ASSERT( not je.HasException() );
				je->CallNonvirtualVoidMethod( obj.Get(), jc.Get(), method, FwdArg<Args>(args)... );
				CHECK_ERR( not je.HasException() );
				return JavaMethodResult<void>(true);
			}

			template <typename ...Args>
			static JavaMethodResult<void>  Call (const JavaObj &obj, jmethodID method, Args&&... args) __NE___
			{
				JavaEnv	je;
				ASSERT( not je.HasException() );
				je->CallVoidMethod( obj.Get(), method, FwdArg<Args>(args)... );
				CHECK_ERR( not je.HasException() );
				return JavaMethodResult<void>(true);
			}
		};

		template <>
		struct JavaMethodCaller< jstring >
		{
			template <typename ...Args>
			static JavaMethodResult<jstring>  CallStatic (const JavaClass &jc, jmethodID method, Args&&... args) __NE___
			{
				JavaEnv	je;
				ASSERT( not je.HasException() );
				auto result = static_cast<jstring>( je->CallStaticObjectMethod( jc.Get(), method, FwdArg<Args>(args)... ));
				CHECK_ERR( not je.HasException() );
				return result;
			}

			template <typename ...Args>
			static JavaMethodResult<jstring>  CallNonvirtual (const JavaObj &obj, const JavaClass &jc, jmethodID method, Args&&... args) __NE___
			{
				JavaEnv	je;
				ASSERT( not je.HasException() );
				auto result = static_cast<jstring>( je->CallNonvirtualObjectMethod( obj.Get(), jc.Get(), method, FwdArg<Args>(args)... ));
				CHECK_ERR( not je.HasException() );
				return result;
			}

			template <typename ...Args>
			static JavaMethodResult<jstring>  Call (const JavaObj &obj, jmethodID method, Args&&... args) __NE___
			{
				JavaEnv	je;
				ASSERT( not je.HasException() );
				auto result = static_cast<jstring>( je->CallObjectMethod( obj.Get(), method, FwdArg<Args>(args)... ));
				CHECK_ERR( not je.HasException() );
				return result;
			}
		};

		#define DECL_METHOD_CALLER( _typeName_, _suffix_ )																								\
			template <>																																	\
			struct JavaMethodCaller< _typeName_ >																										\
			{																																			\
				template <typename ...Args>																												\
				static JavaMethodResult<_typeName_>  CallStatic (const JavaClass &jc, jmethodID method, Args&&... args) __NE___							\
				{																																		\
					JavaEnv	je;																															\
					ASSERT( not je.HasException() );																									\
					auto result = je->CallStatic ## _suffix_ ## Method( jc.Get(), method, FwdArg<Args>(args)... );										\
					CHECK_ERR( not je.HasException() );																									\
					return result;																														\
				}																																		\
																																						\
				template <typename ...Args>																												\
				static JavaMethodResult<_typeName_>  CallNonvirtual (const JavaObj &obj, const JavaClass &jc, jmethodID method, Args&&... args) __NE___	\
				{																																		\
					JavaEnv	je;																															\
					ASSERT( not je.HasException() );																									\
					auto result = je->CallNonvirtual ## _suffix_ ## Method( obj.Get(), jc.Get(), method, FwdArg<Args>(args)... );						\
					CHECK_ERR( not je.HasException() );																									\
					return result;																														\
				}																																		\
																																						\
				template <typename ...Args>																												\
				static JavaMethodResult<_typeName_>  Call (const JavaObj &obj, jmethodID method, Args&&... args) __NE___								\
				{																																		\
					JavaEnv	je;																															\
					ASSERT( not je.HasException() );																									\
					auto result = je->Call ## _suffix_ ## Method( obj.Get(), method, FwdArg<Args>(args)... );											\
					CHECK_ERR( not je.HasException() );																									\
					return result;																														\
				}																																		\
			}

		DECL_METHOD_CALLER( jobject,	Object	);
		DECL_METHOD_CALLER( jboolean,	Boolean	);
		DECL_METHOD_CALLER( jbyte,		Byte	);
		DECL_METHOD_CALLER( jchar,		Char	);
		DECL_METHOD_CALLER( jshort,		Short	);
		DECL_METHOD_CALLER( jint,		Int		);
		DECL_METHOD_CALLER( jlong,		Long	);
		DECL_METHOD_CALLER( jfloat,		Float	);
		DECL_METHOD_CALLER( jdouble,	Double	);
		#undef DECL_METHOD_CALLER

		#define DECL_TYPE_TO_JAVATYPE( _javatype_, _cpptype_ )				\
			ND_ inline _javatype_  TypeToJava (const _cpptype_ &x) __NE___ {\
				return _javatype_(x);										\
			}
		DECL_TYPE_TO_JAVATYPE( jobject,		jobject	 );
		DECL_TYPE_TO_JAVATYPE( jstring,		jstring	 );
		DECL_TYPE_TO_JAVATYPE( jboolean,	jboolean );
		DECL_TYPE_TO_JAVATYPE( jbyte,		jbyte	 );
		DECL_TYPE_TO_JAVATYPE( jchar,		jchar	 );
		DECL_TYPE_TO_JAVATYPE( jshort,		jshort	 );
		DECL_TYPE_TO_JAVATYPE( jint,		jint	 );
		DECL_TYPE_TO_JAVATYPE( jlong,		jlong	 );
		DECL_TYPE_TO_JAVATYPE( jfloat,		jfloat	 );
		DECL_TYPE_TO_JAVATYPE( jdouble,		jdouble	 );
		DECL_TYPE_TO_JAVATYPE( jboolean,	bool	 );
		DECL_TYPE_TO_JAVATYPE( jint,		uint	 );
		DECL_TYPE_TO_JAVATYPE( jlong,		ulong	 );
		#undef DECL_TYPE_TO_JAVATYPE

		ND_ inline jobject  TypeToJava (const JavaObj &obj) __NE___ {
			return obj.Get();
		}

		ND_ inline jstring  TypeToJava (const JavaString& jstr) __NE___ {
			return jstr.Get();
		}

		template <typename T>
		ND_ inline auto  TypeToJava (const JavaArray<T>& arr) __NE___ {
			return arr.Get();
		}

	} // _hidden_



//-----------------------------------------------------------------------------
// JavaStaticMethod / JavaMethod

/*
=================================================
	operator ()
=================================================
*/
	template <typename Ret, typename ...Args>
	template <typename ...ArgTypes>
	Java::_hidden_::JavaMethodResult<Ret>  JavaStaticMethod< Ret (Args...) >::operator () (const ArgTypes&... args) C_NE___
	{
		ASSERT( _method );
		return Java::_hidden_::JavaMethodCaller<Ret>::CallStatic( _class, _method, Java::_hidden_::TypeToJava(args)... );
	}

	template <typename Ret, typename ...Args>
	template <typename ...ArgTypes>
	Java::_hidden_::JavaMethodResult<Ret>  JavaMethod< Ret (Args...) >::operator () (const ArgTypes&... args) C_NE___
	{
		ASSERT( _method );
		return Java::_hidden_::JavaMethodCaller<Ret>::Call( _obj, _method, Java::_hidden_::TypeToJava(args)... );
	}


} // AE::Java

#endif // AE_PLATFORM_ANDROID
