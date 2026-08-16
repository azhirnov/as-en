// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/CompileTime/TypeList.h"

namespace AE::Base::_hidden_
{

	template <typename T>
	struct _FuncInfo;


	#define _DECL_FUNC_INFO( _ptr_, _ex_qual_ )					\
		template <typename Result, typename ...Args>			\
		struct _FuncInfo< Result _ptr_ (Args...) _ex_qual_ >	\
		{														\
			using args		= AE::Base::TypeList< Args... >;	\
			using result	= Result;							\
			using type		= Result (*) (Args...) _ex_qual_;	\
			using clazz		= void;								\
																\
			static constexpr bool	is_const	= false;		\
			static constexpr bool	is_volatile	= false;		\
			static constexpr bool	is_noexcept	= noexcept( static_cast< int (*) () _ex_qual_ >(null)() );	\
		}
	_DECL_FUNC_INFO( ,		);
	_DECL_FUNC_INFO( (*),	);
	_DECL_FUNC_INFO( ,		noexcept );
	_DECL_FUNC_INFO( (*),	noexcept );
	#undef _DECL_FUNC_INFO


	template <typename Class, typename Result, typename ...Args>
	struct _FuncInfo< Result (Class::*) (Args...) >
	{
		using args		= AE::Base::TypeList< Args... >;
		using result	= Result;
		using type		= Result (Class::*) (Args...);
		using clazz		= Class;

		static constexpr bool	is_const	= false;
		static constexpr bool	is_volatile	= false;
		static constexpr bool	is_noexcept	= false;
	};

	template <typename Class, typename Result, typename ...Args>
	struct _FuncInfo< Result (Class::*) (Args...) noexcept >
	{
		using args		= AE::Base::TypeList< Args... >;
		using result	= Result;
		using type		= Result (Class::*) (Args...) noexcept;
		using clazz		= Class;

		static constexpr bool	is_const	= false;
		static constexpr bool	is_volatile	= false;
		static constexpr bool	is_noexcept	= true;
	};

	template <typename Result, typename ...Args>
	struct _FuncInfo< Function< Result (Args...) > >
	{
		using args		= AE::Base::TypeList< Args... >;
		using result	= Result;
		using type		= Result (*) (Args...);
		using clazz		= void;

		static constexpr bool	is_const	= false;
		static constexpr bool	is_volatile	= false;
		static constexpr bool	is_noexcept	= false;
	};

	#define _DECL_FUNC_INFO( _cv_qual_, _ex_qual_ )								\
		template <typename Class, typename Result, typename ...Args>			\
		struct _FuncInfo< Result (Class::*) (Args...) _cv_qual_ _ex_qual_ >		\
		{																		\
			using args		= AE::Base::TypeList< Args... >;					\
			using result	= Result;											\
			using type		= Result (Class::*) (Args...) _cv_qual_ _ex_qual_;	\
			using clazz		= Class;											\
																				\
			static constexpr bool	is_const	= IsConst< int _cv_qual_ >;		\
			static constexpr bool	is_volatile	= IsVolatile< int _cv_qual_ >;	\
			static constexpr bool	is_noexcept	= noexcept( static_cast< int (*) () _ex_qual_ >(null)() );	\
		}
	_DECL_FUNC_INFO( const,				);
	_DECL_FUNC_INFO( volatile,			);
	_DECL_FUNC_INFO( const volatile,	);
	_DECL_FUNC_INFO( &,					);
	_DECL_FUNC_INFO( const &,			);
	_DECL_FUNC_INFO( volatile &,		);
	_DECL_FUNC_INFO( const volatile &,	);
	_DECL_FUNC_INFO( &&,				);
	_DECL_FUNC_INFO( const &&,			);
	_DECL_FUNC_INFO( volatile &&,		);
	_DECL_FUNC_INFO( const volatile &&,	);

	_DECL_FUNC_INFO( const,				noexcept );
	_DECL_FUNC_INFO( volatile,			noexcept );
	_DECL_FUNC_INFO( const volatile,	noexcept );
	_DECL_FUNC_INFO( &,					noexcept );
	_DECL_FUNC_INFO( const &,			noexcept );
	_DECL_FUNC_INFO( volatile &,		noexcept );
	_DECL_FUNC_INFO( const volatile &,	noexcept );
	_DECL_FUNC_INFO( &&,				noexcept );
	_DECL_FUNC_INFO( const &&,			noexcept );
	_DECL_FUNC_INFO( volatile &&,		noexcept );
	_DECL_FUNC_INFO( const volatile &&,	noexcept );
	#undef _DECL_FUNC_INFO


	template <typename T, bool IsClass>
	struct _FuncInfo2 {
		using type = _FuncInfo< RemoveReference< T >>;
	};

	template <typename T>
	struct _FuncInfo2<T, true> {
		using type = _FuncInfo< RemoveReference< decltype(&T::operator()) >>;
	};

	template <typename T>
	struct _FuncInfo3 {
		using type = typename _FuncInfo2< T, IsClass<T> >::type;
	};


	template <typename T, typename ...Args>
	struct _TmplFuncInfo2;

	template <typename T, typename ...Args>
	struct _TmplFuncInfo2< T, TypeList< Args... >>{
		using type = _FuncInfo< RemoveReference< decltype(&T::template operator()< Args... >) >>;
	};

	template <typename T, typename ArgsTL>
	struct _TmplFuncInfo3
	{
		StaticAssert( IsClass< T >);
		StaticAssert( IsTypeList< ArgsTL >);
		using type = typename _TmplFuncInfo2< T, ArgsTL >::type;
	};

} // AE::Base::_hidden_


namespace AE::Base
{
	template <typename T>
	using FunctionInfo = typename Base::_hidden_::_FuncInfo3<T>::type;

	template <typename T, typename ArgsTL>
	using TemplateFunctionInfo = typename Base::_hidden_::_TmplFuncInfo3< T, ArgsTL >::type;

	template <typename T>
	static constexpr bool	IsGlobalFunction = IsVoid< typename FunctionInfo<T>::clazz >;


/*
=================================================
	FnUnsafeCast
=================================================
*/
	template <typename R, typename T>
	NdCx__ R  FnUnsafeCast (T fn) __NE___
	{
		#if defined(__clang__)
		# if __clang_major__ >= 18
		#	pragma clang diagnostic push
		#	pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
		# endif
		#endif

		StaticAssert( IsGlobalFunction<R> == IsGlobalFunction<T> );

		return reinterpret_cast<R>(fn);

		#if defined(__clang__)
		# if __clang_major__ >= 18
		#	pragma clang diagnostic pop
		# endif
		#endif
	}

} // AE::Base
