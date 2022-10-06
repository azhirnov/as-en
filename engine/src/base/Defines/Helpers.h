// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Config.h"


// helper macro
#define AE_PRIVATE_GETARG_0( _0_, ... )				_0_
#define AE_PRIVATE_GETARG_1( _0_, _1_, ... )		_1_
#define AE_PRIVATE_GETARG_2( _0_, _1_, _2_, ... )	_2_
#define AE_PRIVATE_GETRAW( _value_ )				_value_
#define AE_TOSTRING( ... )							#__VA_ARGS__
#define AE_PRIVATE_UNITE_RAW( _arg0_, _arg1_ )		AE_PRIVATE_UNITE( _arg0_, _arg1_ )
#define AE_PRIVATE_UNITE( _arg0_, _arg1_ )			_arg0_ ## _arg1_
#define AE_ARGS( ... )								__VA_ARGS__


// debug only scope
#ifndef DEBUG_ONLY
# if defined(AE_DEBUG)
#	define DEBUG_ONLY( /* code */... )	__VA_ARGS__
# else
#	define DEBUG_ONLY( /* code */... )
# endif
#endif


// bit operators
#define AE_BIT_OPERATORS( _type_ ) \
	ND_ constexpr _type_  operator |  (_type_ lhs, _type_ rhs)	{ return _type_( AE::Base::ToNearUInt(lhs) | AE::Base::ToNearUInt(rhs) ); } \
	ND_ constexpr _type_  operator &  (_type_ lhs, _type_ rhs)	{ return _type_( AE::Base::ToNearUInt(lhs) & AE::Base::ToNearUInt(rhs) ); } \
	\
	constexpr _type_&  operator |= (_type_ &lhs, _type_ rhs)	{ return lhs = _type_( AE::Base::ToNearUInt(lhs) | AE::Base::ToNearUInt(rhs) ); } \
	constexpr _type_&  operator &= (_type_ &lhs, _type_ rhs)	{ return lhs = _type_( AE::Base::ToNearUInt(lhs) & AE::Base::ToNearUInt(rhs) ); } \
	\
	ND_ constexpr _type_  operator ~ (_type_ lhs)				{ return _type_(~AE::Base::ToNearUInt(lhs)); } \
	ND_ constexpr bool   operator ! (_type_ lhs)				{ return not AE::Base::ToNearUInt(lhs); } \
	

// enable/disable checks for enums
#if defined(AE_COMPILER_MSVC)
#	define BEGIN_ENUM_CHECKS() \
		__pragma (warning (push)) \
		__pragma (warning (error: 4061)) /*enumerator 'identifier' in switch of enum 'enumeration' is not explicitly handled by a case label*/ \
		__pragma (warning (error: 4062)) /*enumerator 'identifier' in switch of enum 'enumeration' is not handled*/ \
		__pragma (warning (error: 4063)) /*case 'number' is not a valid value for switch of enum 'type'*/ \

#	define END_ENUM_CHECKS() \
		__pragma (warning (pop)) \

#elif defined(AE_COMPILER_CLANG)
#	define BEGIN_ENUM_CHECKS() \
		 _Pragma( "clang diagnostic error \"-Wswitch\"" )

#	define END_ENUM_CHECKS() \
		 _Pragma( "clang diagnostic ignored \"-Wswitch\"" )

#else
#	define BEGIN_ENUM_CHECKS()
#	define END_ENUM_CHECKS()

#endif


// compile time messages
#ifndef AE_COMPILATION_MESSAGE
#	if defined(AE_COMPILER_CLANG)
#		define AE_PRIVATE_MESSAGE_TOSTR(x)	#x
#		define AE_COMPILATION_MESSAGE( _message_ )	_Pragma(AE_PRIVATE_MESSAGE_TOSTR( GCC warning ("" _message_) ))

#	elif defined(AE_COMPILER_MSVC)
#		define AE_COMPILATION_MESSAGE( _message_ )	__pragma(message( _message_ ))

#	else
#		define AE_COMPILATION_MESSAGE( _message_ )	// not supported
#	endif
#endif


// replace assertions by exceptions
#if 0

#	include <stdexcept>

#	undef  AE_PRIVATE_BREAK_POINT
#	define AE_PRIVATE_BREAK_POINT()	{}

#	undef  AE_LOGE
#	define AE_LOGE	AE_LOGI

	// keep ASSERT and CHECK behaviour because they may be used in destructor
	// but override CHECK_ERR, CHECK_FATAL and RETURN_ERR to allow user to handle this errors

#	undef  AE_PRIVATE_CHECK_ERR
#	define AE_PRIVATE_CHECK_ERR( _expr_, _ret_ ) \
		{if ( !(_expr_) ) { \
			throw AE::Exception{ AE_TOSTRING( _expr_ )}; \
		}}

#	undef  CHECK_FATAL
#	define CHECK_FATAL( _expr_ ) \
		{if ( !(_expr_) ) { \
			throw AE::Exception{ AE_TOSTRING( _expr_ )}; \
		}}

#	undef  AE_PRIVATE_RETURN_ERR
#	define AE_PRIVATE_RETURN_ERR( _text_, _ret_ ) \
		{throw AE::Exception{ _text_ };}

#endif


// setup for build on CI
#ifdef AE_CI_BUILD

#	undef  AE_PRIVATE_BREAK_POINT
#	define AE_PRIVATE_BREAK_POINT()	{}

#	undef  AE_PRIVATE_CHECK
#	define AE_PRIVATE_CHECK( _expr_, _text_ ) \
		{if ( !(_expr_) ) { \
			AE_LOGI( _text_ ); \
			AE_PRIVATE_EXIT(); \
		}}

#	undef  AE_PRIVATE_CHECK_ERR
#	define AE_PRIVATE_CHECK_ERR( _expr_, _ret_ ) \
		{if ( !(_expr_) ) { \
			AE_LOGI( AE_TOSTRING( _expr_ )); \
			AE_PRIVATE_EXIT(); \
		}}

#	undef  CHECK_FATAL
#	define CHECK_FATAL( /* expr */... ) \
		{if ( !(__VA_ARGS__) ) { \
			AE_LOGI( AE_TOSTRING( __VA_ARGS__ )); \
			AE_PRIVATE_EXIT(); \
		}}

#	undef  AE_PRIVATE_RETURN_ERR
#	define AE_PRIVATE_RETURN_ERR( _text_, _ret_ ) \
		{AE_LOGI( _text_ ); \
		 AE_PRIVATE_EXIT(); \
		}

# ifdef AE_DEBUG
#	undef  ASSERT
#	define ASSERT	CHECK
# endif

#	include <cassert>
#	undef  assert
#	define assert( /* expr */ ... ) \
		AE_PRIVATE_CHECK( (__VA_ARGS__), AE_TOSTRING( __VA_ARGS__ ))

#endif	// AE_CI_BUILD

