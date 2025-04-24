// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Impl/ScriptArgList.h"
#include "scripting/Impl/ScriptEngine.h"

namespace AE::Scripting::_hidden_
{
	template <typename T>
	struct IsValidArg {
		using A = Base::RemoveAllQualifiers<T>;
		static constexpr bool	value = IsVoid<T> or IsInteger<A> or IsFloatPoint<A> or IsCompleteType< ScriptTypeInfo<A> >;
	};
	
	template <typename ...Types>
	static constexpr bool	AllArgsValid = TypeList< Types... >::template ForEach_And< IsValidArg >();




	//
	// Global Function Wrapper
	//

	template <typename Fn>
	struct FnWrap;


	template <typename R, typename ...Args>
	struct FnWrap< R (*) (Args...) >
	{
		using type = R (*) (Args...);

		template < R (*fp) (Args...) >
		AE_FLATTEN_FN static void  Call (ScriptArgList args) __Th___
		{
			if constexpr( AllArgsValid< R, Args... >)
				_Call<fp>( args, MakeUSizeSequence< 0, CountOf<Args...>() >{} );
			else
				CHECK_THROW( false );
		}

	private:
		template < R (*fp) (Args...), usize ...I >
		static void  _Call (ScriptArgList args, USizeSequence<I...>) __Th___
		{
			if constexpr( IsVoid<R> )
				fp( args.Arg<Args>(I)... );
			else
				args.Return<R>( fp( args.Arg<Args>(I)... ));
		}
	};
	
	template <typename R, typename ...Args>
	struct FnWrap< R (*) (Args...) noexcept >
	{
		using type = R (*) (Args...);

		template < R (*fp) (Args...) noexcept >
		AE_FLATTEN_FN static void  Call (ScriptArgList args) __Th___
		{
			if constexpr( AllArgsValid< R, Args... >)
				_Call<fp>( args, MakeUSizeSequence< 0, CountOf<Args...>() >{} );
			else
				CHECK_THROW( false );
		}

	private:
		template < R (*fp) (Args...) noexcept, usize ...I >
		static void  _Call (ScriptArgList args, USizeSequence<I...>) __Th___
		{
			if constexpr( IsVoid<R> )
				fp( args.Arg<Args>(I)... );
			else
				args.Return<R>( fp( args.Arg<Args>(I)... ));
		}
	};

	
	
	//
	// Assign Operator Wrapper
	//

	template <typename Fn>
	struct FnWrap_AssignOp;


	template <typename A0, typename A1>
	struct FnWrap_AssignOp< A0& (*) (A0&, A1) >
	{
		using type = A0& (A0::*) (A1);

		template < A0& (*fp)(A0&, A1) >
		static void  Call (ScriptArgList args) __Th___
		{
			auto*	obj = args.GetObject<A0>();
			CHECK_THROW( obj != null );

			fp( INOUT *obj, args.Arg<A1>(0) );
			args.Return< A0* >( obj );
		}
	};


	
	//
	// Binary Operator Wrapper
	//

	template <typename Fn>
	struct FnWrap_BinOp;


	template <typename R, typename A0, typename A1>
	struct FnWrap_BinOp< R (*) (const A0&, A1) >
	{
		using type = R (A0::*) (A1) const;

		template < R (*fp)(const A0&, A1) >
		static void  Call (ScriptArgList args) __Th___
		{
			auto*	obj = args.GetObject<A0>();
			CHECK_THROW( obj != null );

			args.Return<R>( fp( *obj, args.Arg<A1>(0) ));
		}
	};


	template <typename Fn>
	struct FnWrap_BinRHOp;

	template <typename R, typename A0, typename A1>
	struct FnWrap_BinRHOp< R (*) (A0, const A1&) >
	{
		using type = R (*) (A0, const A1&);

		template < R (*fp) (A0, const A1&) >
		static void  Call (ScriptArgList args) __Th___
		{
			auto*	obj = args.GetObject<A1>();
			CHECK_THROW( obj != null );

			args.Return<R>( fp( args.Arg<A0>(0), *obj ));
		}
	};


	template <typename Fn>
	struct FnWrap_BinConstOp;


	template <typename R, typename A0, typename A1>
	struct FnWrap_BinConstOp< R (*) (const A0&, const A1&) >
	{
		using type = R (*) (const A0&, const A1&);

		template < R (*fp) (const A0&, const A1&) >
		static void  Call (ScriptArgList args) __Th___
		{
			args.Return<R>( fp( args.Arg<A0>(0), args.Arg<A1>(1) ));
		}
	};

	template <typename R, typename Class, typename A0>
	struct FnWrap_BinConstOp< R (Class::*) (const A0&) const >
	{
		using type = R (Class::*) (const A0&) const;

		template < R (Class::*fp) (const A0&) const >
		static void  Call (ScriptArgList args) __Th___
		{
			const auto*	obj = args.GetObject<Class>();
			CHECK_THROW( obj != null );

			args.Return<R>( (obj->*fp)( args.Arg<A0>(0) ));
		}
	};

	template <typename R, typename Class, typename A0>
	struct FnWrap_BinConstOp< R (Class::*) (const A0&) const noexcept >
	{
		using type = R (Class::*) (const A0&) const;

		template < R (Class::*fp) (const A0&) const noexcept >
		static void  Call (ScriptArgList args) __Th___
		{
			const auto*	obj = args.GetObject<Class>();
			CHECK_THROW( obj != null );

			args.Return<R>( (obj->*fp)( args.Arg<A0>(0) ));
		}
	};


	
	//
	// Unary Operator Wrapper
	//

	template <typename Fn>
	struct FnWrap_UnaryOp;

	template <typename A0>
	struct FnWrap_UnaryOp< A0 (*) (const A0&) >
	{
		using type = A0 (A0::*) () const;

		template < A0 (*fp)(const A0&) >
		static void  Call (ScriptArgList args) __Th___
		{
			auto*	obj = args.GetObject<A0>();
			CHECK_THROW( obj != null );

			args.Return<A0>( fp( *obj ));
		}
	};


	
	//
	// Member Function Wrapper
	//

	template <typename Fn>
	struct FnWrap_ObjFirst;


	template <typename Class, typename C>
	ND_ exact_t  PtrOrRef (C* p)
	{
		if constexpr( IsPointer< Class >)
			return p;
		else{
			StaticAssert( IsLValueRef< Class >);
			return *p;
		}
	}


	template <typename R, typename Class, typename ...Args>
	struct FnWrap_ObjFirst< R (Class::*) (Args...) >
	{
		using type = R (Class::*) (Args...);

		template < R (Class::*fp) (Args...) >
		AE_FLATTEN_FN static void  Call (ScriptArgList args) __Th___
		{
			if constexpr( AllArgsValid< R, Args... >)
				_Call<fp>( args, MakeUSizeSequence< 0, CountOf<Args...>() >{} );
			else
				CHECK_THROW( false );
		}

	private:
		template < R (Class::*fp)(Args...), usize ...I >
		static void  _Call (ScriptArgList args, USizeSequence<I...>) __Th___
		{
			auto*	obj = args.GetObject<Class>();
			CHECK_THROW( obj != null );

			if constexpr( IsVoid<R> )
				(obj->*fp)( args.Arg<Args>(I)... );
			else
				args.Return<R>( (obj->*fp)( args.Arg<Args>(I)... ));
		}
	};

	
	template <typename R, typename Class, typename ...Args>
	struct FnWrap_ObjFirst< R (Class::*) (Args...) noexcept >
	{
		using type = R (Class::*) (Args...);

		template < R (Class::*fp) (Args...) noexcept >
		AE_FLATTEN_FN static void  Call (ScriptArgList args) __Th___
		{
			if constexpr( AllArgsValid< R, Args... >)
				_Call<fp>( args, MakeUSizeSequence< 0, CountOf<Args...>() >{} );
			else
				CHECK_THROW( false );
		}

	private:
		template < R (Class::*fp)(Args...) noexcept, usize ...I >
		static void  _Call (ScriptArgList args, USizeSequence<I...>) __Th___
		{
			auto*	obj = args.GetObject<Class>();
			CHECK_THROW( obj != null );

			if constexpr( IsVoid<R> )
				(obj->*fp)( args.Arg<Args>(I)... );
			else
				args.Return<R>( (obj->*fp)( args.Arg<Args>(I)... ));
		}
	};


	template <typename R, typename Class, typename ...Args>
	struct FnWrap_ObjFirst< R (Class::*) (Args...) const >
	{
		using type = R (Class::*) (Args...) const;

		template < R (Class::*fp) (Args...) const >
		AE_FLATTEN_FN static void  Call (ScriptArgList args) __Th___
		{
			if constexpr( AllArgsValid< R, Args... >)
				_Call<fp>( args, MakeUSizeSequence< 0, CountOf<Args...>() >{} );
			else
				CHECK_THROW( false );
		}

	private:
		template < R (Class::*fp)(Args...) const, usize ...I >
		static void  _Call (ScriptArgList args, USizeSequence<I...>) __Th___
		{
			const auto*	obj = args.GetObject<Class>();
			CHECK_THROW( obj != null );

			if constexpr( IsVoid<R> )
				(obj->*fp)( args.Arg<Args>(I)... );
			else
				args.Return<R>( (obj->*fp)( args.Arg<Args>(I)... ));
		}
	};


	template <typename R, typename Class, typename ...Args>
	struct FnWrap_ObjFirst< R (Class::*) (Args...) const noexcept >
	{
		using type = R (Class::*) (Args...) const;

		template < R (Class::*fp) (Args...) const noexcept >
		AE_FLATTEN_FN static void  Call (ScriptArgList args) __Th___
		{
			if constexpr( AllArgsValid< R, Args... >)
				_Call<fp>( args, MakeUSizeSequence< 0, CountOf<Args...>() >{} );
			else
				CHECK_THROW( false );
		}

	private:
		template < R (Class::*fp)(Args...) const noexcept, usize ...I >
		static void  _Call (ScriptArgList args, USizeSequence<I...>) __Th___
		{
			const auto*	obj = args.GetObject<Class>();
			CHECK_THROW( obj != null );

			if constexpr( IsVoid<R> )
				(obj->*fp)( args.Arg<Args>(I)... );
			else
				args.Return<R>( (obj->*fp)( args.Arg<Args>(I)... ));
		}
	};


	template <typename R, typename Class, typename ...Args>
	struct FnWrap_ObjFirst< R (*) (Class, Args...) >
	{
		using C = RemoveAllQualifiers< Class >;
		using type = R (C::*) (Args...);

		template < R (*fp) (Class, Args...) >
		AE_FLATTEN_FN static void  Call (ScriptArgList args) __Th___
		{
			if constexpr( AllArgsValid< R, Class, Args... >)
				_Call<fp>( args, MakeUSizeSequence< 0, CountOf<Args...>() >{} );
			else
				CHECK_THROW( false );
		}

	private:
		template < R (*fp) (Class, Args...), usize ...I >
		static void  _Call (ScriptArgList args, USizeSequence<I...>) __Th___
		{
			auto*	obj = args.GetObject<C>();
			CHECK_THROW( obj != null );
			
			if constexpr( IsVoid<R> )
				fp( PtrOrRef<Class>(obj), args.Arg<Args>(I)... );
			else
				args.Return<R>( fp( PtrOrRef<Class>(obj), args.Arg<Args>(I)... ));
		}
	};


	template <typename R, typename Class, typename ...Args>
	struct FnWrap_ObjFirst< R (*) (Class, Args...) noexcept >
	{
		using C = RemoveAllQualifiers< Class >;
		using type = R (C::*) (Args...);

		template < R (*fp) (Class, Args...) noexcept >
		AE_FLATTEN_FN static void  Call (ScriptArgList args) __Th___
		{
			if constexpr( AllArgsValid< R, Class, Args... >)
				_Call<fp>( args, MakeUSizeSequence< 0, CountOf<Args...>() >{} );
			else
				CHECK_THROW( false );
		}

	private:
		template < R (*fp) (Class, Args...) noexcept, usize ...I >
		static void  _Call (ScriptArgList args, USizeSequence<I...>) __Th___
		{
			auto*	obj = args.GetObject<C>();
			CHECK_THROW( obj != null );
			
			if constexpr( IsVoid<R> )
				fp( PtrOrRef<Class>(obj), args.Arg<Args>(I)... );
			else
				args.Return<R>( fp( PtrOrRef<Class>(obj), args.Arg<Args>(I)... ));
		}
	};

} // AE::Scripting


#ifdef AS_MAX_PORTABILITY

	#define AS_GLOBAL_FN( _se_, _fn_, ... )																					\
		(_se_)->AddGenericFn< decltype(_fn_) >(																				\
			&AE::Scripting::_hidden_::FnWrap< decltype(&_fn_) >::template Call< (&_fn_) >, __VA_ARGS__ )

	#define AS_METHOD( _binder_, _fn_, ... )																				\
		(_binder_).AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::type >(			\
			&AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::template Call< (&_fn_) >, __VA_ARGS__ )

	#define AS_METHOD_T( _binder_, _fn_, ... )																				\
		(_binder_).template AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::type >(	\
			&AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::template Call< (&_fn_) >, __VA_ARGS__ )

	#define AS_OP_BIN_ASSIGN( _binder_, _op_, _fn_ )																		\
		(_binder_).AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_AssignOp< decltype(&_fn_) >::type >(			\
			&AE::Scripting::_hidden_::FnWrap_AssignOp< decltype(&_fn_) >::template Call< (&_fn_) >,							\
			Scripting::_hidden_::OperatorBinderHelper::_BinAssignToStr( _op_ ), {} )

	#define AS_OP_BIN_ASSIGN_T( _binder_, _op_, _fn_ )																		\
		(_binder_).template AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_AssignOp< decltype(&_fn_) >::type >(	\
			&AE::Scripting::_hidden_::FnWrap_AssignOp< decltype(&_fn_) >::template Call< (&_fn_) >,							\
			Scripting::_hidden_::OperatorBinderHelper::_BinAssignToStr( _op_ ), {} )

	#define AS_OP_BINARY( _binder_, _op_, _fn_ )																			\
		(_binder_).AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_BinOp< decltype(&_fn_) >::type >(				\
			&AE::Scripting::_hidden_::FnWrap_BinOp< decltype(&_fn_) >::template Call< (&_fn_) >,							\
			Scripting::_hidden_::OperatorBinderHelper::_BinToStr( _op_ ), {} )

	#define AS_OP_BINARY_T( _binder_, _op_, _fn_ )																			\
		(_binder_).template AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_BinOp< decltype(&_fn_) >::type >(	\
			&AE::Scripting::_hidden_::FnWrap_BinOp< decltype(&_fn_) >::template Call< (&_fn_) >,							\
			Scripting::_hidden_::OperatorBinderHelper::_BinToStr( _op_ ), {} )

	#define AS_OP_BINARY_RH( _binder_, _op_, _fn_ )																			\
		(_binder_).AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_BinRHOp< decltype(&_fn_) >::type >(			\
			&AE::Scripting::_hidden_::FnWrap_BinRHOp< decltype(&_fn_) >::template Call< (&_fn_) >,							\
			Scripting::_hidden_::OperatorBinderHelper::_BinRightToStr( _op_ ), {} )

	#define AS_OP_BINARY_RH_T( _binder_, _op_, _fn_ )																		\
		(_binder_).template AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_BinRHOp< decltype(&_fn_) >::type >(	\
			&AE::Scripting::_hidden_::FnWrap_BinRHOp< decltype(&_fn_) >::template Call< (&_fn_) >,							\
			Scripting::_hidden_::OperatorBinderHelper::_BinRightToStr( _op_ ), {} )

	#define AS_OP_UNARY( _binder_, _op_, _fn_ )																				\
		(_binder_).AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_UnaryOp< decltype(&_fn_) >::type >(			\
			&AE::Scripting::_hidden_::FnWrap_UnaryOp< decltype(&_fn_) >::template Call< (&_fn_) >,							\
			Scripting::_hidden_::OperatorBinderHelper::_UnaryToStr( _op_ ), {} )

	#define AS_OP_UNARY_T( _binder_, _op_, _fn_ )																			\
		(_binder_).template AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_UnaryOp< decltype(&_fn_) >::type >(	\
			&AE::Scripting::_hidden_::FnWrap_UnaryOp< decltype(&_fn_) >::template Call< (&_fn_) >,							\
			Scripting::_hidden_::OperatorBinderHelper::_UnaryToStr( _op_ ), {} )

	#define AS_OP_EQUAL( _binder_, _fn_ )																					\
		(_binder_).AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_BinConstOp< decltype(&_fn_) >::type >(		\
			&AE::Scripting::_hidden_::FnWrap_BinConstOp< decltype(&_fn_) >::template Call< (&_fn_) >, "opEquals", {} )
	
	#define AS_OP_EQUAL_T( _binder_, _fn_ )																					\
		(_binder_).template AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_BinConstOp< decltype(&_fn_) >::type >(\
			&AE::Scripting::_hidden_::FnWrap_BinConstOp< decltype(&_fn_) >::template Call< (&_fn_) >, "opEquals", {} )

	#define AS_OP_CMP( _binder_, _fn_ )																						\
		(_binder_).AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_BinConstOp< decltype(&_fn_) >::type >(		\
			&AE::Scripting::_hidden_::FnWrap_BinConstOp< decltype(&_fn_) >::template Call< (&_fn_) >, "opCmp", {} )
	
	#define AS_OP_CMP_T( _binder_, _fn_ )																					\
		(_binder_).template AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_BinConstOp< decltype(&_fn_) >::type >(\
			&AE::Scripting::_hidden_::FnWrap_BinConstOp< decltype(&_fn_) >::template Call< (&_fn_) >, "opCmp", {} )

	#define AS_OP_CALL( _binder_, _fn_ )																					\
		(_binder_).AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::type >(			\
			&AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::template Call< (&_fn_) >, "opCall", {} )

	#define AS_OP_CALL_T( _binder_, _fn_ )																					\
		(_binder_).template AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::type >(	\
			&AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::template Call< (&_fn_) >, "opCall", {} )

	#define AS_EXPL_CAST( _binder_, _fn_ )																					\
		(_binder_).AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::type >(			\
			&AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::template Call< (&_fn_) >, "opCast", {} )

	#define AS_EXPL_CAST_T( _binder_, _fn_ )																				\
		(_binder_).template AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::type >(	\
			&AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::template Call< (&_fn_) >, "opCast", {} )

	#define AS_IMPL_CAST( _binder_, _fn_ )																					\
		(_binder_).AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::type >(			\
			&AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::template Call< (&_fn_) >, "opImplCast", {} )

	#define AS_IMPL_CAST_T( _binder_, _fn_ )																				\
		(_binder_).template AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::type >(	\
			&AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::template Call< (&_fn_) >, "opImplCast", {} )

	#define AS_OP_CONVERT( _binder_, _fn_ )																					\
		(_binder_).AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::type >(			\
			&AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::template Call< (&_fn_) >, "opConv", {} )

	#define AS_OP_CONVERT_T( _binder_, _fn_ )																				\
		(_binder_).template AddGenericMethod< typename AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::type >(	\
			&AE::Scripting::_hidden_::FnWrap_ObjFirst< decltype(&_fn_) >::template Call< (&_fn_) >, "opConv", {} )

#else

	#define AS_GLOBAL_FN( _se_, _fn_, ... )				(_se_)->AddFunction< decltype(_fn_) >( _fn_, __VA_ARGS__ )
	#define AS_METHOD( _binder_, _fn_, ... )			(_binder_).AddMethodOrGlobal( &_fn_, __VA_ARGS__ )
	#define AS_METHOD_T( _binder_, _fn_, ... )			AS_METHOD( (_binder_), _fn_, __VA_ARGS__ )
	#define AS_OP_BIN_ASSIGN( _binder_, _op_, _fn_ )	(_binder_).Operators().BinaryAssign( (_op_), &_fn_ )
	#define AS_OP_BIN_ASSIGN_T( _binder_, _op_, _fn_ )	AS_OP_BIN_ASSIGN( (_binder_), (_op_), _fn_ )
	#define AS_OP_BINARY( _binder_, _op_, _fn_ )		(_binder_).Operators().Binary( (_op_), &_fn_ )
	#define AS_OP_BINARY_T( _binder_, _op_, _fn_ )		AS_OP_BINARY( (_binder_), (_op_), _fn_ )
	#define AS_OP_BINARY_RH( _binder_, _op_, _fn_ )		(_binder_).Operators().BinaryRH( (_op_), &_fn_ )
	#define AS_OP_BINARY_RH_T( _binder_, _op_, _fn_ )	AS_OP_BINARY_RH( (_binder_), (_op_), _fn_ )
	#define AS_OP_UNARY( _binder_, _op_, _fn_ )			(_binder_).Operators().Unary( (_op_), &_fn_ )
	#define AS_OP_UNARY_T( _binder_, _op_, _fn_ )		AS_OP_UNARY( (_binder_), (_op_), _fn_ )
	#define AS_OP_EQUAL( _binder_, _fn_ )				(_binder_).Operators().Equal( &_fn_ )
	#define AS_OP_EQUAL_T( _binder_, _fn_ )				AS_OP_EQUAL( (_binder_), _fn_ )
	#define AS_OP_CMP( _binder_, _fn_ )					(_binder_).Operators().Compare( &_fn_ )
	#define AS_OP_CMP_T( _binder_, _fn_ )				AS_OP_CMP( (_binder_), _fn_ )
	#define AS_OP_CALL( _binder_, _fn_ )				(_binder_).Operators().Call( &_fn_ )
	#define AS_OP_CALL_T( _binder_, _fn_ )				AS_OP_CALL( (_binder_), _fn_ )
	#define AS_EXPL_CAST( _binder_, _fn_ )				(_binder_).Operators().ExpCast( &_fn_ )
	#define AS_EXPL_CAST_T( _binder_, _fn_ )			AS_EXPL_CAST( (_binder_), _fn_ )
	#define AS_IMPL_CAST( _binder_, _fn_ )				(_binder_).Operators().ImplCast( &_fn_ )
	#define AS_IMPL_CAST_T( _binder_, _fn_ )			AS_IMPL_CAST( (_binder_), _fn_ )
	#define AS_OP_CONVERT( _binder_, _fn_ )				(_binder_).Operators().Convert( &_fn_ )
	#define AS_OP_CONVERT_T( _binder_, _fn_ )			AS_OP_CONVERT( (_binder_), _fn_ )

#endif // AS_MAX_PORTABILITY
