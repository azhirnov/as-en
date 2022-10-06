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

	}	// _hidden_



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
			template <typename Func>	Self &	Unary (EUnaryOperator op, Func func);
			template <typename Func>	Self &	BinaryAssign (EBinaryOperator op, Func func);
			template <typename Func>	Self &	Binary (EBinaryOperator op, Func func);
			template <typename Func>	Self &	BinaryRH (EBinaryOperator op, Func func);
			

			// index
			template <typename OutType, typename ...InTypes>	Self &	Index ();	// x[...]
			template <typename Func>							Self &	Index (Func func);


			// call
			template <typename OutType, typename ...InTypes>	Self &	Call ( OutType (T::*) (InTypes...) );	// x(...)
			template <typename OutType, typename ...InTypes>	Self &	Call ( OutType (T::*) (InTypes...) const );
			template <typename OutType, typename ...InTypes>	Self &	Call ( OutType (*) (T&, InTypes...) );
			template <typename OutType, typename ...InTypes>	Self &	Call ( OutType (*) (const T&, InTypes...) );


			// convert
			template <typename OutType> Self &	Convert ();		// y(x)
			template <typename OutType> Self &	Convert ( OutType (T::*) () const );
			template <typename OutType> Self &	Convert ( OutType (*) (const T &) );


			// cast
			template <typename OutType> Self &	Cast ();		// y(x)
			template <typename OutType> Self &	Cast ( OutType& (T::*) () );
			template <typename OutType> Self &	Cast ( OutType const& (T::*) () const );
			template <typename OutType> Self &	Cast ( OutType& (*) (T &) );
			template <typename OutType> Self &	Cast ( OutType const& (*) (const T &) );


			// compare
			template <typename Func>	Self &	Equals (Func func);		// x == y
			template <typename Func>	Self &	Compare (Func func);	// x <> y
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
		

		bool  CreatePodValue (int flags = 0);
		bool  CreateClassValue (int flags = 0);
		bool  CreateRef (int flags = 0, Bool hasFactory = true);
		bool  CreateRef (T* (*create)(), void (T:: *addRef)(), void (T:: *releaseRef)(), int flags = 0);


		template <typename Func>
		bool  AddConstructor (Func ctorPtr);
		
		template <typename Func>
		bool  AddFactoryCtor (Func ctorPtr);

		template <typename B>
		bool  AddProperty (B T::* value, StringView name);
		
		template <typename A, typename B>
		bool  AddProperty (A T::* base, B A::* value, StringView name);

		template <typename B>
		bool  AddProperty (const T &self, B &value, StringView name);

		template <typename Func>
		bool  AddMethod (Func methodPtr, StringView name);
		
		template <typename Func>
		bool  AddMethodFromGlobal (Func funcPtr, StringView name);
		
		template <typename Func>
		bool  AddMethodFromGlobalObjFirst (Func funcPtr, StringView name);
		
		template <typename Func>
		bool  AddMethodFromGlobalObjLast (Func funcPtr, StringView name);

		ND_ OperatorBinder						Operators ()			{ return OperatorBinder( this ); }

		ND_ NtStringView						Name ()			const	{ return _name; }

		ND_ const ScriptEnginePtr &				GetEngine ()	const	{ return _engine; }
		ND_ Ptr< AngelScript::asIScriptEngine >	GetASEngine ()			{ return _engine->Get(); }


	private:
		bool  _Create (int flags);
		
		template <typename T1>
		struct _IsSame;

		struct _Util;
	};


}	// AE::Scripting
