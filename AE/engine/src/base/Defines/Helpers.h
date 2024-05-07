// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once


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
// #ifdef/endif takes at least 3 lines, this macro doesn't add overhead on line count
#ifndef DEBUG_ONLY
# ifdef AE_DEBUG
#	define DEBUG_ONLY( /* code */... )		__VA_ARGS__
# else
#	define DEBUG_ONLY( /* code */... )
# endif
#endif

// profile only scope
#ifndef PROFILE_ONLY
# if defined(AE_CFG_DEBUG) or defined(AE_CFG_DEVELOP) or defined(AE_CFG_PROFILE)
#	define PROFILE_ONLY( /* code */... )	__VA_ARGS__
# else
#	define PROFILE_ONLY( /* code */... )
# endif
#endif


// bit operators
// requires '#include "base/Math/BitMath.h"'
#define AE_BIT_OPERATORS( _type_ )																																\
	ND_ constexpr _type_	operator |  (_type_ lhs, _type_ rhs)	__NE___	{ return _type_( AE::Base::ToNearUInt(lhs) | AE::Base::ToNearUInt(rhs) ); }			\
	ND_ constexpr _type_	operator &  (_type_ lhs, _type_ rhs)	__NE___	{ return _type_( AE::Base::ToNearUInt(lhs) & AE::Base::ToNearUInt(rhs) ); }			\
																																								\
		constexpr _type_&	operator |= (_type_ &lhs, _type_ rhs)	__NE___	{ return lhs = _type_( AE::Base::ToNearUInt(lhs) | AE::Base::ToNearUInt(rhs) ); }	\
		constexpr _type_&	operator &= (_type_ &lhs, _type_ rhs)	__NE___	{ return lhs = _type_( AE::Base::ToNearUInt(lhs) & AE::Base::ToNearUInt(rhs) ); }	\
																																								\
	ND_ constexpr _type_	operator ~ (_type_ lhs)					__NE___	{ return _type_(~AE::Base::ToNearUInt(lhs)); }										\
	ND_ constexpr bool		operator ! (_type_ lhs)					__NE___	{ return not AE::Base::ToNearUInt(lhs); }											\


// enable/disable checks for enums
#if defined(AE_COMPILER_MSVC)
#	define AE_BEGIN_ENUM_CHECKS()																												\
		__pragma (warning (push))																												\
		__pragma (warning (error: 4061)) /*enumerator 'identifier' in switch of enum 'enumeration' is not explicitly handled by a case label*/	\
		__pragma (warning (error: 4062)) /*enumerator 'identifier' in switch of enum 'enumeration' is not handled*/								\
		__pragma (warning (error: 4063)) /*case 'number' is not a valid value for switch of enum 'type'*/										\

#	define AE_END_ENUM_CHECKS() \
		__pragma (warning (pop)) \

#elif defined(AE_COMPILER_CLANG)
#	define AE_BEGIN_ENUM_CHECKS()						\
		_Pragma( "clang diagnostic push" )				\
		_Pragma( "clang diagnostic error \"-Wswitch\"" )\

#	define AE_END_ENUM_CHECKS() \
		_Pragma( "clang diagnostic pop" )

#elif defined(AE_COMPILER_GCC)
#	define AE_BEGIN_ENUM_CHECKS()						\
		_Pragma( "GCC diagnostic push" )				\
		_Pragma( "GCC diagnostic error \"-Wswitch\"" )	\

#	define AE_END_ENUM_CHECKS() \
		_Pragma( "GCC diagnostic pop" )

#else
#	define AE_BEGIN_ENUM_CHECKS()
#	define AE_END_ENUM_CHECKS()
#endif

#define switch_enum( ... )	AE_BEGIN_ENUM_CHECKS();  switch ( __VA_ARGS__ )
#define switch_end			AE_END_ENUM_CHECKS();


// compile time messages
#ifndef AE_COMPILATION_MESSAGE
#	if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)
#		define AE_COMPILATION_MESSAGE( _message_ )	_Pragma(AE_TOSTRING( GCC warning ("" _message_) ))

#	elif defined(AE_COMPILER_MSVC)
#		define AE_COMPILATION_MESSAGE( _message_ )	__pragma(message( _message_ ))

#	else
#		define AE_COMPILATION_MESSAGE( _message_ )	// not supported
#	endif
#endif


// enable/disable exceptions
#ifdef AE_ENABLE_EXCEPTIONS
#	define TRY					try
#	define CATCH_ALL( ... )		catch(...) { __VA_ARGS__; }
#	define IsNoExcept( ... )	noexcept( __VA_ARGS__ )
#	define NoExcept( ... )		noexcept( __VA_ARGS__ )
#	define CNoExcept( ... )		const noexcept( __VA_ARGS__ )
#else
#	define TRY
#	define CATCH_ALL( ... )		{}
#	define IsNoExcept( ... )	true
#	define NoExcept( ... )
#	define CNoExcept( ... )		const
#endif


// helper for 'template <..., EnableIf<..., bool>=true >'
#define ENABLEIF( ... )			EnableIf< (__VA_ARGS__), bool > = true
#define ENABLEIF_IMPL( ... )	EnableIf< (__VA_ARGS__), bool >

#define DISABLEIF( ... )		DisableIf< (__VA_ARGS__), bool > = true
#define DISABLEIF_IMPL( ... )	DisableIf< (__VA_ARGS__), bool >


// offsetof without warnings
#if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	define AE_DISABLE_OFFSETOF_WARNINGS( ... )						\
		_Pragma( "clang diagnostic push" )							\
		_Pragma( "clang diagnostic ignored \"-Winvalid-offsetof\"" )\
		__VA_ARGS__													\
		_Pragma( "clang diagnostic pop" )

#elif defined(AE_COMPILER_GCC)
#	define AE_DISABLE_OFFSETOF_WARNINGS( ... )						\
		_Pragma( "GCC diagnostic push" )							\
		_Pragma( "GCC diagnostic ignored \"-Winvalid-offsetof\"" )	\
		__VA_ARGS__													\
		_Pragma( "GCC diagnostic pop" )
#else
#	define AE_DISABLE_OFFSETOF_WARNINGS( ... )						\
		__VA_ARGS__
#endif

