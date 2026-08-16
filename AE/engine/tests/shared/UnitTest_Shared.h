// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"
#include "base/Algorithms/Parser.h"
#include "base/Platforms/Platform.h"
#include "base/../../GlobalConfig.h"

using namespace AE;
using namespace AE::Base;

#define TEST( ... )					CHECK_FATAL_MSG( (__VA_ARGS__), AE_TOSTRING(__VA_ARGS__) )
#define TEST_MSG( _expr_, _msg_ )	CHECK_FATAL_MSG( (_expr_), (_msg_) )

#define TEST_NO_LOG( ... )										\
	{															\
		StaticLogger::Deinitialize( false );					\
																\
		bool	_res_ = (__VA_ARGS__);							\
																\
		StaticLogger::InitDefault();							\
																\
		CHECK_FATAL_MSG( _res_, AE_TOSTRING(__VA_ARGS__) );		\
	}

#define __PRIVATE_TEST_OP( _lhs_, _op_, _rhs_ )															\
	{																									\
		const auto	_tmp_lhs_ = (_lhs_);																\
		const auto	_tmp_rhs_ = (_rhs_);																\
		CHECK_FATAL_MSG( AE::Base::All( _tmp_lhs_ _op_ _tmp_rhs_ ),										\
			AE::Base::String{AE_TOSTRING(_lhs_)} << " (" << AE::Base::ToString(_tmp_lhs_) << ") " <<	\
			AE_TOSTRING(_op_) << " (" << AE::Base::ToString(_tmp_rhs_) << ") " << AE_TOSTRING(_rhs_))	\
	}
#define TEST_Eq( _lhs_, _rhs_ )				__PRIVATE_TEST_OP( (_lhs_), ==, (_rhs_) )
#define TEST_NE( _lhs_, _rhs_ )				__PRIVATE_TEST_OP( (_lhs_), !=, (_rhs_) )
#define TEST_Gt( _lhs_, _rhs_ )				__PRIVATE_TEST_OP( (_lhs_), >,  (_rhs_) )
#define TEST_GE( _lhs_, _rhs_ )				__PRIVATE_TEST_OP( (_lhs_), >=, (_rhs_) )
#define TEST_Lt( _lhs_, _rhs_ )				__PRIVATE_TEST_OP( (_lhs_), <,  (_rhs_) )
#define TEST_LE( _lhs_, _rhs_ )				__PRIVATE_TEST_OP( (_lhs_), <=, (_rhs_) )

#define TEST_BitEq( _lhs_, _rhs_, _acc_ )\
	{																									\
		const auto	_tmp_lhs_ = (_lhs_);																\
		const auto	_tmp_rhs_ = (_rhs_);																\
		CHECK_FATAL_MSG( AE::Base::All( AE::Base::BitEqual( _tmp_lhs_, _tmp_rhs_, (_acc_) )),			\
			AE::Base::String{AE_TOSTRING(_lhs_)} << " (" << AE::Base::ToString(_tmp_lhs_) << ") " <<	\
			" == (" << AE::Base::ToString(_tmp_rhs_) << ") " << AE_TOSTRING(_rhs_))						\
	}


#define __PRIVATE_CHECK_THROW( _var_, _check_, ... )\
	bool	_var_ = false;							\
	try {											\
		__VA_ARGS__									\
	} catch(...) {									\
		_var_ = true;								\
	}												\
	TEST( _check_ _var_ );							\


#define TEST_THROW( ... )	{ __PRIVATE_CHECK_THROW( AE_PRIVATE_UNITE_RAW( test_is_throw_, __LINE__ ), true  ==, __VA_ARGS__ ); }
#define TEST_NOTHROW( ... )	{ __PRIVATE_CHECK_THROW( AE_PRIVATE_UNITE_RAW( test_is_throw_, __LINE__ ), false ==, __VA_ARGS__ ); }

#define TEST_PASSED()		AE_LOGI( AE::Base::String{AE_FUNCTION_NAME} + " - passed" );

#ifdef AE_PLATFORM_APPLE
#	define _BEGIN_TEST2()																						\
		const auto	SetOrGetCurrDir = [argc, argv]()															\
		{{																										\
			Path	curr = Path{argv[0]}.parent_path().parent_path().parent_path().parent_path();				\
			curr = AE::Base::Parser::GetCommandLinePath( ArrayView{ argv, usize(Max(argc,0)) }, "-p", curr );	\
			std::error_code ec;																					\
			std::filesystem::current_path( curr, OUT ec );														\
			if ( ec ) curr = std::filesystem::current_path( OUT ec );											\
			return curr;																						\
		}}
#elif defined(AE_ANDROID_CONSOLE_MODE)
	// by default use current path
#	define _BEGIN_TEST2()																						\
		const auto	SetOrGetCurrDir = [argc, argv]()															\
		{{																										\
			std::error_code ec;																					\
			Path	curr = std::filesystem::current_path( OUT ec );												\
			curr = AE::Base::Parser::GetCommandLinePath( ArrayView{ argv, usize(Max(argc,0)) }, "-p", curr );	\
			std::filesystem::current_path( curr, OUT ec );														\
			return curr;																						\
		}}
#else
#	define _BEGIN_TEST2()																						\
		const auto	SetOrGetCurrDir = [argc, argv]()															\
		{{																										\
			Path	curr = Path{argv[0]}.parent_path();															\
			curr = AE::Base::Parser::GetCommandLinePath( ArrayView{ argv, usize(Max(argc,0)) }, "-p", curr );	\
			std::error_code ec;																					\
			std::filesystem::current_path( curr, OUT ec );														\
			if ( ec ) curr = std::filesystem::current_path( OUT ec );											\
			return curr;																						\
		}}
#endif


#define BEGIN_TEST()																								\
	TEST( AE::Base::CpuArchInfo::Get().CheckCompilationOptions() );													\
	StaticLogger::LoggerScope log{};																				\
	_BEGIN_TEST2();																									\
	const Path	curr		= SetOrGetCurrDir();																	\
	String		test_name	= AE::Base::Parser::GetCommandLineArg( ArrayView{ argv, usize(Max(argc,0)) }, "-t" );	\
	AE_LOG_DBG( "Current path: "s << curr.string() );																\
	if ( test_name.empty() ){																						\
		Unused( AE::Base::PlatformUtils::GetEnvironmentVariable( "AE_TEST_NAME", OUT test_name ));					\
	}

#if defined(AE_PLATFORM_ANDROID) and not defined(AE_ANDROID_CONSOLE_MODE)
#	define TEST_ENTRY()		extern "C" AE_DLL_EXPORT int  AEMain (const int argc, char const* argv[])
#else
#	define TEST_ENTRY()		int main (const int argc, char const* argv[])
#endif

#define RUN_TEST( _name_, ... )										\
	if ( test_name.empty() or test_name == AE_TOSTRING(_name_) ) {	\
		_name_( __VA_ARGS__ );										\
	}

