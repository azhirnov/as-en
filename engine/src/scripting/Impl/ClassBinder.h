// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ScriptEngine.h"
#include "scripting/Impl/ScriptTypes.h"

namespace AE::Scripting
{

	enum class EUnaryOperator
	{
		Inverse,
		Not,
		PreInc,
		PreDec,
		PostInc,
		PostDec
	};

	enum class EBinaryOperator
	{
		Assign,
		Add,
		Sub,
		Mul,
		Div,
		Mod,
		And,
		Or,
		Xor,
		ShiftLeft,
		ShiftRight
	};


	namespace _hidden_
	{
		struct OperatorBinderHelper
		{
			static const char *  _UnaryToStr (EUnaryOperator op);
			static const char *  _BinToStr (EBinaryOperator op);
			static const char *  _BinAssignToStr (EBinaryOperator op);
			static const char *  _BinRightToStr (EBinaryOperator op);
		};

		template <typename Func>
		ND_ constexpr bool IsGlobal ();

	} // _hidden_



	//
	// Class Binder
	//
	
	template <typename T>
	struct ClassBinder final
	{
	// types
	public:
		using Self		= ClassBinder<T>;
		using Class_t	= T;
		

		//
		// Operator Binder (helper class)
		// see http://www.angelcode.com/angelscript/sdk/docs/manual/doc_script_class_ops.html
		//
		struct OperatorBinder final : Scripting::_hidden_::OperatorBinderHelper
		{
			friend struct ClassBinder<T>;

		// types
		public:
			using Self = OperatorBinder;


		// variables
		private:
			Ptr< ClassBinder<T> >		_binder;


		// methods
		private:
			OperatorBinder () {}
			explicit OperatorBinder (ClassBinder<T> *ptr) : _binder(ptr) {}

		public:
			template <typename Func>	Self &	Unary (EUnaryOperator op, Func func)			__Th___;
			template <typename Func>	Self &	BinaryAssign (EBinaryOperator op, Func func)	__Th___;
			template <typename Func>	Self &	Binary (EBinaryOperator op, Func func)			__Th___;
			template <typename Func>	Self &	BinaryRH (EBinaryOperator op, Func func)		__Th___;
			

			// index
			template <typename OutType, typename ...InTypes>	Self &	Index ()				__Th___;	// x[...]
			template <typename Func>							Self &	Index (Func func)		__Th___;


			// call
			template <typename OutType, typename ...InTypes>	Self &	Call ( OutType (T::*) (InTypes...) )			__Th___;	// x(...)
			template <typename OutType, typename ...InTypes>	Self &	Call ( OutType (T::*) (InTypes...) const )		__Th___;
			template <typename OutType, typename ...InTypes>	Self &	Call ( OutType (*) (T&, InTypes...) )			__Th___;
			template <typename OutType, typename ...InTypes>	Self &	Call ( OutType (*) (const T&, InTypes...) )		__Th___;


			// convert
			template <typename OutType> Self &	Convert ()									__Th___;	// y(x)
			template <typename OutType> Self &	Convert ( OutType (T::*) () const )			__Th___;
			template <typename OutType> Self &	Convert ( OutType (*) (const T &) )			__Th___;


			// cast
			template <typename OutType> Self &	Cast ()										__Th___;	// y(x)
			template <typename OutType> Self &	Cast ( OutType& (T::*) () )					__Th___;
			template <typename OutType> Self &	Cast ( OutType const& (T::*) () const )		__Th___;
			template <typename OutType> Self &	Cast ( OutType& (*) (T &) )					__Th___;
			template <typename OutType> Self &	Cast ( OutType const& (*) (const T &) )		__Th___;


			// compare
			template <typename Func>	Self &	Equals (Func func)							__Th___;	// x == y
			template <typename Func>	Self &	Compare (Func func)							__Th___;	// x <> y
		};


	// variables
	private:
		ScriptEnginePtr		_engine;
		String				_name;
		int					_flags;


	// methods
	public:
		explicit ClassBinder (const ScriptEnginePtr &eng);
		ClassBinder (const ScriptEnginePtr &eng, StringView name);
		

		void  CreatePodValue (int flags = 0)															__Th___;
		void  CreateClassValue (int flags = 0)															__Th___;
		void  CreateRef (int flags = 0, Bool hasFactory = true)											__Th___;
		void  CreateRef (T* (*create)(), void (T:: *addRef)(), void (T:: *releaseRef)(), int flags = 0)	__Th___;


		template <typename Func>
		void  AddConstructor (Func ctorPtr)									__Th___;
		
		template <typename Func>
		void  AddFactoryCtor (Func ctorPtr)									__Th___;

		template <typename B>
		void  AddProperty (B T::* value, StringView name)					__Th___;
		
		template <typename A, typename B>
		void  AddProperty (A T::* base, B A::* value, StringView name)		__Th___;

		template <typename B>
		void  AddProperty (const T &self, B &value, StringView name)		__Th___;

		template <typename Func>
		void  AddMethod (Func methodPtr, StringView name)					__Th___;
		
		template <typename Func>
		void  AddMethodFromGlobal (Func funcPtr, StringView name)			__Th___;
		
		template <typename Func>
		void  AddMethodFromGlobalObjFirst (Func funcPtr, StringView name)	__Th___;
		
		template <typename Func>
		void  AddMethodFromGlobalObjLast (Func funcPtr, StringView name)	__Th___;

		ND_ OperatorBinder						Operators ()			{ return OperatorBinder( this ); }

		ND_ StringView							Name ()			const	{ return _name; }

		ND_ const ScriptEnginePtr &				GetEngine ()	const	{ return _engine; }
		ND_ Ptr< AngelScript::asIScriptEngine >	GetASEngine ()			{ return _engine->Get(); }


	private:
		void  _Create (int flags) __Th___;
		
		template <typename T1>
		struct _IsSame;

		struct _Util;
	};

	
/*
=================================================
	IsGlobal
=================================================
*/
namespace _hidden_ {
	template <typename Func>
	inline constexpr bool  IsGlobal ()
	{
		return IsSameTypes< typename FunctionInfo<Func>::clazz, void >;
	}
} // _hidden_


/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline ClassBinder<T>::ClassBinder (const ScriptEnginePtr &eng) :
		_engine{ eng }, _flags{ 0 }
	{
		ScriptTypeInfo< T >::Name( OUT _name );
	}
	
	template <typename T>
	inline ClassBinder<T>::ClassBinder (const ScriptEnginePtr &eng, StringView name) :
		_engine{ eng }, _name{ name }, _flags{ 0 }
	{}
	
/*
=================================================
	_IsSame
=================================================
*/
	template <typename T>
	template <typename T1>
	struct ClassBinder<T>::_IsSame {
		static constexpr bool	value =	IsSameTypes< T *, T1 >		 or
										IsSameTypes< T &, T1 >		 or
										IsSameTypes< T const *, T1 > or
										IsSameTypes< T const &, T1 >;
	};
	
/*
=================================================
	_Util
=================================================
*/
	struct _Util
	{
		template <typename TIn, typename TOut>
		static String Desc (StringView name)
		{
			String signature;
			ScriptTypeInfo< TOut >::Name( OUT signature );		((signature += " ") += name) += "(";
			ScriptTypeInfo< TIn >::ArgName( OUT signature );	signature += ") const";
			return signature;
		}
	};

/*
=================================================
	CreatePodValue
=================================================
*/
	template <typename T>
	inline void  ClassBinder<T>::CreatePodValue (int flags) __Th___
	{
		using namespace AngelScript;
		
		STATIC_ASSERT( alignof(T) <= 16 );

		_flags = asOBJ_VALUE | asOBJ_POD | flags;
		
		if constexpr( alignof(T) == 16 )
			_flags |= asOBJ_APP_ALIGN16;
		else
		if constexpr( alignof(T) == 8 )
			_flags |= asOBJ_APP_CLASS_ALIGN8;

		AS_CHECK_THROW( GetASEngine()->RegisterObjectType( _name.c_str(), sizeof(T), _flags ));
		
		_Create( _flags );	// throw
	}
	
/*
=================================================
	CreateClassValue
=================================================
*/
	template <typename T>
	inline void  ClassBinder<T>::CreateClassValue (int flags) __Th___
	{
		using namespace AngelScript;

		//STATIC_ASSERT( alignof(T) <= 8 );

		_flags = asOBJ_VALUE | asOBJ_APP_CLASS_CDAK | flags;

		//if constexpr( alignof(T) == 16 )
		//	_flags |= asOBJ_APP_ALIGN16;
		//else
		if constexpr( alignof(T) == 8 )
			_flags |= asOBJ_APP_CLASS_ALIGN8;

		AS_CHECK_THROW( GetASEngine()->RegisterObjectType( _name.c_str(), sizeof(T), _flags ));

		_Create( _flags );	// throw
	}

/*
=================================================
	CreateRef
=================================================
*/
	template <typename T>
	inline void  ClassBinder<T>::CreateRef (int flags, const Bool hasFactory) __Th___
	{
		using constructor_t = T * (*) ();

		constructor_t create = null;

		if ( hasFactory )
			create = &AngelScriptHelper::FactoryCreateRC<T>;
	
		return CreateRef( create, &T::__AddRef, &T::__Release, flags );
	}
	
/*
=================================================
	CreateRef
=================================================
*/
	template <typename T>
	inline void  ClassBinder<T>::CreateRef (T* (*create)(), void (T:: *addRef)(), void (T:: *releaseRef)(), int flags) __Th___
	{
		using namespace AngelScript;

		_flags = asOBJ_REF | flags | (addRef != null and releaseRef != null ? 0 : asOBJ_NOCOUNT);

		AS_CHECK_THROW( GetASEngine()->RegisterObjectType( _name.c_str(), sizeof(T), _flags ));
		
		if ( addRef != null )
		{
			AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_ADDREF, "void AddRef()",
								asSMethodPtr<sizeof(void (T::*)())>::Convert(static_cast<void (T::*)()>(addRef)), asCALL_THISCALL ));
		}

		if ( releaseRef != null )
		{
			AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_RELEASE, "void Release()",
								asSMethodPtr<sizeof(void (T::*)())>::Convert(static_cast<void (T::*)()>(releaseRef)), asCALL_THISCALL ));
		}

		if ( create != null )
		{
			AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_FACTORY,
														(_name + "@ new_" + _name + "()").c_str(),
														asFUNCTION( create ), asCALL_CDECL ));
		}
		/*
		AS_CHECK_THROW( GetASEngine()->RegisterObjectMethod( _name.c_str(),
										(String(_name) << " & opAssign(const " << _name << " &in)").c_str(),
										asMETHOD( T, operator = ), asCALL_THISCALL ));
		*/
	}

/*
=================================================
	_Create
=================================================
*/
	template <typename T>
	inline void  ClassBinder<T>::_Create (const int flags) __Th___
	{
		using namespace AngelScript;

		// constructor
		if ( AllBits( flags, asOBJ_APP_CLASS_CONSTRUCTOR ))
		{
			AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_CONSTRUCT, "void f()",
											asFUNCTION( &AngelScriptHelper::Constructor<T> ), asCALL_GENERIC ));
		}

		// destructor
		if ( AllBits( flags, asOBJ_APP_CLASS_DESTRUCTOR ))
		{
			AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_DESTRUCT,  "void f()",
											asFUNCTION( &AngelScriptHelper::Destructor<T> ), asCALL_GENERIC ));
		}

		// copy constructor
		if ( AllBits( flags, asOBJ_APP_CLASS_COPY_CONSTRUCTOR ))
		{
			AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_CONSTRUCT,
											("void f(const " + _name + " &in)").c_str(),
											asFUNCTION( &AngelScriptHelper::CopyConstructor<T> ), asCALL_GENERIC ));
		}

		// assignment 
		if ( AllBits( flags, asOBJ_APP_CLASS_ASSIGNMENT ))
		{
			AS_CHECK_THROW( GetASEngine()->RegisterObjectMethod( _name.c_str(),
											(_name + " & opAssign(const " + _name + " &in)").c_str(),
											asFUNCTION( &AngelScriptHelper::CopyAssign<T> ), asCALL_GENERIC ));
		}
	}
	
/*
=================================================
	AddConstructor
=================================================
*/
	template <typename T>
	template <typename Func>
	inline void  ClassBinder<T>::AddConstructor (Func ctorPtr) __Th___
	{
		using namespace AngelScript;

		STATIC_ASSERT(( not IsBaseOf< AngelScriptHelper::SimpleRefCounter, T > ));
		STATIC_ASSERT(( IsSameTypes< void *, typename GlobalFunction<Func>::TypeList_t::Front::type > ));

		String	signature("void f ");
		GlobalFunction<Func>::GetArgs( OUT signature, 1 );	// skip	(void *)

		AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_CONSTRUCT,
										signature.c_str(), asFUNCTION( *ctorPtr ), asCALL_CDECL_OBJFIRST ));
	}
	
/*
=================================================
	AddFactoryCtor
=================================================
*/
	template <typename T>
	template <typename Func>
	inline void  ClassBinder<T>::AddFactoryCtor (Func ctorPtr) __Th___
	{
		using namespace AngelScript;
		
		STATIC_ASSERT(( IsBaseOf< AngelScriptHelper::SimpleRefCounter, T > ));
		STATIC_ASSERT(( IsSameTypes< T*, typename GlobalFunction<Func>::Result_t > ));

		String	signature(_name + "@ new_" + _name);
		GlobalFunction<Func>::GetArgs( OUT signature );

		AS_CHECK_THROW( GetASEngine()->RegisterObjectBehaviour( _name.c_str(), asBEHAVE_FACTORY,
										signature.c_str(), asFUNCTION( *ctorPtr ), asCALL_CDECL ));
	}
	
/*
=================================================
	AddProperty
=================================================
*/
	template <typename T>
	template <typename B>
	inline void  ClassBinder<T>::AddProperty (B T::* value, StringView name) __Th___
	{
		String	signature;
		ScriptTypeInfo<B>::Name( OUT signature );
		(signature += ' ') += name;

		AS_CHECK_THROW( GetASEngine()->RegisterObjectProperty( _name.c_str(), signature.c_str(), int(OffsetOf( value )) ));
	}
	
	template <typename T>
	template <typename A, typename B>
	inline void  ClassBinder<T>::AddProperty (A T::* base, B A::* value, StringView name) __Th___
	{
		String	signature;
		ScriptTypeInfo<B>::Name( OUT signature );
		(signature += ' ') += name;

		Bytes	base_off	= OffsetOf( base );
		Bytes	value_off	= OffsetOf( value );
		CHECK( base_off + value_off < SizeOf<T> );

		AS_CHECK_THROW( GetASEngine()->RegisterObjectProperty( _name.c_str(), signature.c_str(), int(base_off + value_off) ));
	}

	template <typename T>
	template <typename B>
	inline void  ClassBinder<T>::AddProperty (const T &self, B &value, StringView name) __Th___
	{
		String	signature;
		ScriptTypeInfo<B>::Name( OUT signature );
		(signature += ' ') += name;

		const ssize		offset = BitCast<ssize>(&value) - BitCast<ssize>(&self);
		CHECK( offset >= 0 and offset <= ssize(sizeof(T) - sizeof(B)) );

		AS_CHECK_THROW( GetASEngine()->RegisterObjectProperty( _name.c_str(), signature.c_str(), int(offset) ));
	}

/*
=================================================
	AddMethod
=================================================
*/
	template <typename T>
	template <typename Func>
	inline void  ClassBinder<T>::AddMethod (Func methodPtr, StringView name) __Th___
	{
		using namespace AngelScript;

		String	signature;
		MemberFunction<Func>::GetDescriptor( OUT signature, name );

		AS_CHECK_THROW( GetASEngine()->RegisterObjectMethod( _name.c_str(), signature.c_str(),
								asSMethodPtr< sizeof( void (T::*)() ) >::Convert( reinterpret_cast<void (T::*)()>(methodPtr) ),
								asCALL_THISCALL ));
	}
	
/*
=================================================
	AddMethodFromGlobal
=================================================
*/
	template <typename T>
	template <typename Func>
	inline void  ClassBinder<T>::AddMethodFromGlobal (Func funcPtr, StringView name) __Th___
	{
		using Args = typename GlobalFunction<Func>::TypeList_t;
		STATIC_ASSERT( Args::Count > 0 );

		constexpr bool	obj_first	= _IsSame< typename Args::Front::type >::value;
		constexpr bool	obj_last	= _IsSame< typename Args::Back::type >::value;
		STATIC_ASSERT( obj_first or obj_last );

		if constexpr( obj_first )
			return AddMethodFromGlobalObjFirst( funcPtr, name );
		else
			return AddMethodFromGlobalObjLast( funcPtr, name );
	}
	
/*
=================================================
	AddMethodFromGlobalObjFirst
=================================================
*/
	template <typename T>
	template <typename Func>
	inline void  ClassBinder<T>::AddMethodFromGlobalObjFirst (Func funcPtr, StringView name) __Th___
	{
		using namespace AngelScript;
		
		using FuncInfo	= FunctionInfo<Func>;
		using FrontArg	= typename FuncInfo::args::Front::type;

		STATIC_ASSERT( _IsSame< FrontArg >::value );

		String	signature;
		GlobalFunction<Func>::GetDescriptor( OUT signature, name, 1, 0 );

		if constexpr( IsConst< FrontArg > or IsConst<RemovePointer< FrontArg >> )
		{
			signature += " const";
		}

		AS_CHECK_THROW( GetASEngine()->RegisterObjectMethod( _name.c_str(), signature.c_str(), asFUNCTION( *funcPtr ), asCALL_CDECL_OBJFIRST ));
	}
		
/*
=================================================
	AddMethodFromGlobalObjLast
=================================================
*/
	template <typename T>
	template <typename Func>
	inline void  ClassBinder<T>::AddMethodFromGlobalObjLast (Func funcPtr, StringView name) __Th___
	{
		using namespace AngelScript;
		
		using FuncInfo	= FunctionInfo<Func>;
		using BackArg	= typename FuncInfo::args::template Get< FuncInfo::args::Count-1 >;

		STATIC_ASSERT( _IsSame< BackArg >::value );

		String	signature;
		GlobalFunction<Func>::GetDescriptor( OUT signature, name, 0, 1 );
		
		if constexpr( IsConst< BackArg > or IsConst<RemovePointer< BackArg >> )
		{
			signature += " const";
		}

		AS_CHECK_THROW( GetASEngine()->RegisterObjectMethod( _name.c_str(), signature.c_str(), asFUNCTION( *funcPtr ), asCALL_CDECL_OBJLAST ));
	}

/*
=================================================
	Unary
=================================================
*/
	template <typename T> template <typename Func>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Unary (EUnaryOperator op, Func func) __Th___
	{
		if constexpr( Scripting::_hidden_::IsGlobal<Func>() )
			_binder->AddMethodFromGlobalObjFirst( func, _UnaryToStr( op ));
		else
			_binder->AddMethod( func, _UnaryToStr( op ));
		
		return *this;
	}

/*
=================================================
	BinaryAssign
=================================================
*/
	template <typename T> template <typename Func>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::BinaryAssign (EBinaryOperator op, Func func) __Th___
	{
		if constexpr( Scripting::_hidden_::IsGlobal<Func>() )
			_binder->AddMethodFromGlobalObjFirst( func, _BinAssignToStr( op ));
		else
			_binder->AddMethod( func, _BinAssignToStr( op ));

		return *this;
	}

/*
=================================================
	Index
=================================================
*/
	template <typename T> template <typename OutType, typename ...InTypes>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Index () __Th___
	{
		if constexpr( IsConst<OutType>() )
			return Index( static_cast< OutType (T::*) (InTypes...) const >( &T::operator [] ));
		else
			return Index( static_cast< OutType (T::*) (InTypes...) >( &T::operator [] ));
	}
	
	template <typename T> template <typename Func>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Index (Func func) __Th___
	{
		if constexpr( Scripting::_hidden_::IsGlobal<Func>() )
			_binder->AddMethodFromGlobalObjFirst( func, "opIndex" );
		else
			_binder->AddMethod( func, "opIndex" );
		
		return *this;
	}
	
/*
=================================================
	Call
=================================================
*/
	template <typename T> template <typename OutType, typename ...InTypes>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Call ( OutType (T::*func) (InTypes...) ) __Th___
	{
		_binder->AddMethod( func, "opCall" );
		return *this;
	}
	
	template <typename T> template <typename OutType, typename ...InTypes>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Call ( OutType (T::*func) (InTypes...) const ) __Th___
	{
		_binder->AddMethod( func, "opCall" );
		return *this;
	}
	
	template <typename T> template <typename OutType, typename ...InTypes>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Call ( OutType (*func) (T&, InTypes...) ) __Th___
	{
		_binder->AddMethodFromGlobalObjFirst( func, "opCall" );
		return *this;
	}
	
	template <typename T> template <typename OutType, typename ...InTypes>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Call ( OutType (*func) (const T&, InTypes...) ) __Th___
	{
		_binder->AddMethodFromGlobalObjFirst( func, "opCall" );
		return *this;
	}

/*
=================================================
	Convert
=================================================
*/
	/*template <typename T> template <typename OutType>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Convert () __Th___
	{
		return Convert( static_cast< OutType (T::*) () const >() );
	}*/
	
	template <typename T> template <typename OutType>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Convert ( OutType (T::*func) () const ) __Th___
	{
		_binder->AddMethod( func, "opConv" );
		return *this;
	}
	
	template <typename T> template <typename OutType>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Convert ( OutType (*func) (const T &) ) __Th___
	{
		_binder->AddMethodFromGlobalObjFirst( func, "opConv" );
		return *this;
	}
	
/*
=================================================
	Cast
=================================================
*/
	template <typename T> template <typename OutType>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Cast () __Th___
	{
		return Cast( static_cast< OutType const& (T::*) () const >( &T::operator OutType ));
	}
	
	template <typename T> template <typename OutType>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Cast ( OutType& (T::*func) () ) __Th___
	{
		_binder->AddMethod( func, "opCast" );
		return *this;
	}
	
	template <typename T> template <typename OutType>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Cast ( OutType const& (T::*func) () const ) __Th___
	{
		_binder->AddMethod( func, "opCast" );
		return *this;
	}
	
	template <typename T> template <typename OutType>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Cast ( OutType& (*func) (T &) ) __Th___
	{
		_binder->AddMethodFromGlobalObjFirst( func, "opCast" );
		return *this;
	}
	
	template <typename T> template <typename OutType>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Cast ( OutType const& (*func) (const T &) ) __Th___
	{
		_binder->AddMethodFromGlobalObjFirst( func, "opCast" );
		return *this;
	}

/*
=================================================
	Binary
=================================================
*/
	template <typename T> template <typename Func>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Binary (EBinaryOperator op, Func func) __Th___
	{
		if constexpr( Scripting::_hidden_::IsGlobal<Func>() )
			_binder->AddMethodFromGlobalObjFirst( func, _BinToStr( op ));
		else
			_binder->AddMethod( func, _BinToStr( op ));
		
		return *this;
	}

/*
=================================================
	BinaryRH
=================================================
*/
	template <typename T> template <typename Func>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::BinaryRH (EBinaryOperator op, Func func) __Th___
	{
		STATIC_ASSERT( Scripting::_hidden_::IsGlobal<Func>() );
		
		_binder->AddMethodFromGlobalObjLast( func, _BinRightToStr( op ));
		return *this;
	}

/*
=================================================
	Equals
=================================================
*/
	template <typename T> template <typename Func>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Equals (Func func) __Th___
	{
		using FuncInfo = FunctionInfo<Func>;

		STATIC_ASSERT( IsSameTypes< typename FuncInfo::result, bool > );
		
		if constexpr( Scripting::_hidden_::IsGlobal<Func>() )
		{
			STATIC_ASSERT( FuncInfo::args::Count == 2 );
			STATIC_ASSERT(( IsSameTypes< typename FuncInfo::args::template Get<0>, T > or IsSameTypes< typename FuncInfo::args::template Get<0>, const T& > ));
			STATIC_ASSERT(( IsSameTypes< typename FuncInfo::args::template Get<1>, T > or IsSameTypes< typename FuncInfo::args::template Get<1>, const T& > ));

			_binder->AddMethodFromGlobalObjFirst( func, "opEquals" );
		}
		else
			_binder->AddMethod( func, "opEquals" );
		
		return *this;
	}
	
/*
=================================================
	Compare
=================================================
*/
	template <typename T> template <typename Func>
	inline typename ClassBinder<T>::OperatorBinder&  ClassBinder<T>::OperatorBinder::Compare (Func func) __Th___
	{
		using FuncInfo = FunctionInfo<Func>;

		STATIC_ASSERT( IsSameTypes< typename FuncInfo::result, int > );
		STATIC_ASSERT( FuncInfo::args::Count == 2 );
		STATIC_ASSERT(( IsSameTypes< typename FuncInfo::args::template Get<0>, T > or IsSameTypes< typename FuncInfo::args::template Get<0>, const T& > ));
		STATIC_ASSERT(( IsSameTypes< typename FuncInfo::args::template Get<1>, T > or IsSameTypes< typename FuncInfo::args::template Get<1>, const T& > ));
		
		if constexpr( Scripting::_hidden_::IsGlobal<Func>() )
			_binder->AddMethodFromGlobalObjFirst( func, "opCmp" );
		else
			_binder->AddMethod( func, "opCmp" );
		
		return *this;
	}


} // AE::Scripting
