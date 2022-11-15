// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/TypeList.h"

namespace AE::Base::_hidden_
{

	template <typename T>
	struct _FuncInfo {};
		
	template <typename T>
	struct _FuncInfo< T * > {};
		
	template <typename T, typename Class>
	struct _FuncInfo< T (Class::*) >	{};
		
		
	template <typename Result, typename ...Args>
	struct _FuncInfo< Result (Args...) >
	{
		using args		= AE::Base::TypeList< Args... >;
		using result	= Result;
		using type		= Result (*) (Args...);
		using clazz		= void;

		static constexpr bool	is_const	= false;
		static constexpr bool	is_volatile	= false;
		static constexpr bool	is_noexcept	= false;
	};
		
	template <typename Result, typename ...Args>
	struct _FuncInfo< Result (*) (Args...) >
	{
		using args		= AE::Base::TypeList< Args... >;
		using result	= Result;
		using type		= Result (*) (Args...);
		using clazz		= void;
		
		static constexpr bool	is_const	= false;
		static constexpr bool	is_volatile	= false;
		static constexpr bool	is_noexcept	= false;
	};
		
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

	#define _DECL_FUNC_INFO( _cv_qual_ ) \
		template <typename Class, typename Result, typename ...Args> \
		struct _FuncInfo< Result (Class::*) (Args...) _cv_qual_ > \
		{ \
			using args		= AE::Base::TypeList< Args... >; \
			using result	= Result; \
			using type		= Result (Class::*) (Args...) _cv_qual_; \
			using clazz		= Class; \
			\
			static constexpr bool	is_const	= IsConst< int _cv_qual_ >; \
			static constexpr bool	is_volatile	= IsVolatile< int _cv_qual_ >; \
			static constexpr bool	is_noexcept	= false; \
		};
	_DECL_FUNC_INFO( const );
	_DECL_FUNC_INFO( volatile );
	_DECL_FUNC_INFO( const volatile );
	_DECL_FUNC_INFO( & );
	_DECL_FUNC_INFO( const & );
	_DECL_FUNC_INFO( volatile & );
	_DECL_FUNC_INFO( const volatile & );
	_DECL_FUNC_INFO( && );
	_DECL_FUNC_INFO( const && );
	_DECL_FUNC_INFO( volatile && );
	_DECL_FUNC_INFO( const volatile && );
	#undef _DECL_FUNC_INFO
		

#if 1
	#define _DECL_FUNC_INFO_EX( ... ) \
		template <typename Class, typename Result, typename ...Args> \
		struct _FuncInfo< Result (Class::*) (Args...) __VA_ARGS__ noexcept > \
		{ \
			using args		= AE::Base::TypeList< Args... >; \
			using result	= Result; \
			using type		= Result (Class::*) (Args...) __VA_ARGS__ noexcept; \
			using clazz		= Class; \
			\
			static constexpr bool	is_const	= IsConst< int __VA_ARGS__ >; \
			static constexpr bool	is_volatile	= IsVolatile< int __VA_ARGS__ >; \
			static constexpr bool	is_noexcept	= true; \
		};
	
	_DECL_FUNC_INFO_EX( );
	_DECL_FUNC_INFO_EX( const );
	_DECL_FUNC_INFO_EX( volatile );
	_DECL_FUNC_INFO_EX( const volatile );
	_DECL_FUNC_INFO_EX( & );
	_DECL_FUNC_INFO_EX( const & );
	_DECL_FUNC_INFO_EX( volatile & );
	_DECL_FUNC_INFO_EX( const volatile & );
	_DECL_FUNC_INFO_EX( && );
	_DECL_FUNC_INFO_EX( const && );
	_DECL_FUNC_INFO_EX( volatile && );
	_DECL_FUNC_INFO_EX( const volatile && );
	#undef _DECL_FUNC_INFO_EX
#endif
		

	template < typename T, bool L >
	struct _FuncInfo2 {
		using type = _FuncInfo<T>;
	};
	
	template < typename T >
	struct _FuncInfo2<T, true> {
		using type = _FuncInfo< decltype(&T::operator()) >;
	};

	template < typename T >
	struct _FuncInfo3 {
		using type = typename _FuncInfo2< T, IsClass<T> >::type;
	};

} // AE::Base::_hidden_


namespace AE::Base
{
	template <typename T>
	using FunctionInfo = typename Base::_hidden_::_FuncInfo3<T>::type;

} // AE::Base
