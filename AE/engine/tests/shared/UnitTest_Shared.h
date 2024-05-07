// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"
#include "base/FileSystem/Path.h"

using namespace AE;
using namespace AE::Base;

#define TEST( ... )		CHECK_FATAL_MSG( (__VA_ARGS__), AE_TOSTRING(__VA_ARGS__) )

#define __PRIVATE_TEST_OP( _lhs_, _op_, _rhs_ )															\
	{																									\
		const auto	_tmp_lhs_ = (_lhs_);																\
		const auto	_tmp_rhs_ = (_rhs_);																\
		CHECK_FATAL_MSG( AE::Math::All( _tmp_lhs_ _op_ _tmp_rhs_ ),										\
			AE::Base::String{AE_TOSTRING(_lhs_)} << " (" << AE::Base::ToString(_tmp_lhs_) << ") " <<	\
			AE_TOSTRING(_op_) << " (" << AE::Base::ToString(_tmp_rhs_) << ") " << AE_TOSTRING(_rhs_))	\
	}
#define TEST_Eq( _lhs_, _rhs_ )		__PRIVATE_TEST_OP( (_lhs_), ==, (_rhs_) )
#define TEST_NE( _lhs_, _rhs_ )		__PRIVATE_TEST_OP( (_lhs_), !=, (_rhs_) )
#define TEST_Gt( _lhs_, _rhs_ )		__PRIVATE_TEST_OP( (_lhs_), >,  (_rhs_) )
#define TEST_GE( _lhs_, _rhs_ )		__PRIVATE_TEST_OP( (_lhs_), >=, (_rhs_) )
#define TEST_Lt( _lhs_, _rhs_ )		__PRIVATE_TEST_OP( (_lhs_), <,  (_rhs_) )
#define TEST_LE( _lhs_, _rhs_ )		__PRIVATE_TEST_OP( (_lhs_), <=, (_rhs_) )


#define __PRIVATE_CHECK_THROW( _var_, _check_, ... )	\
	bool	_var_ = false;								\
	try {												\
		__VA_ARGS__										\
	} catch(...) {										\
		_var_ = true;									\
	}													\
	TEST( _check_ _var_ );								\


#define TEST_THROW( ... )	{ __PRIVATE_CHECK_THROW( AE_PRIVATE_UNITE_RAW( test_is_throw_, __LINE__ ), true  ==, __VA_ARGS__ ); }
#define TEST_NOTHROW( ... )	{ __PRIVATE_CHECK_THROW( AE_PRIVATE_UNITE_RAW( test_is_throw_, __LINE__ ), false ==, __VA_ARGS__ ); }

#define TEST_PASSED()		AE_LOGI( AE::Base::String{AE_FUNCTION_NAME} + " - passed" );

#ifdef AE_PLATFORM_ANDROID
#	define BEGIN_TEST()									\
		const Path	curr {path};						\
		_ae_fs_::current_path( curr );					\
		StaticLogger::LoggerDbgScope log{};

#elif defined(AE_PLATFORM_APPLE)
#	define BEGIN_TEST()																				\
		const Path	curr = Path{argv[0]}.parent_path().parent_path().parent_path().parent_path();	\
		Unused( argc );																				\
		_ae_fs_::current_path( curr );																\
		StaticLogger::LoggerDbgScope log{};

#else
#	define BEGIN_TEST()											\
		_ae_fs_::current_path( Path{argv[0]}.parent_path() );	\
		StaticLogger::LoggerDbgScope log{};						\
		const Path	curr = Path{argv[0]}.parent_path();			\
		Unused( argc );
#endif
