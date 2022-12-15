// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"

using namespace AE;
using namespace AE::Base;

#define TEST	CHECK_FATAL

#define __PRIVATE_TEST_OP( _lhs_, _op_, _rhs_ ) \
		CHECK_FATAL_MSG( All( (_lhs_) _op_ (_rhs_) ), \
			AE::Base::String{AE_TOSTRING( _lhs_ )} << " (" << AE::Base::ToString(_lhs_) << ") " << AE_TOSTRING(_op_) << " (" << AE::Base::ToString(_rhs_) << ") " << AE_TOSTRING( _rhs_ ))

#define TEST_Eq( _lhs_, _rhs_ )		__PRIVATE_TEST_OP( (_lhs_), ==, (_rhs_) )
#define TEST_NE( _lhs_, _rhs_ )		__PRIVATE_TEST_OP( (_lhs_), !=, (_rhs_) )
#define TEST_Gt( _lhs_, _rhs_ )		__PRIVATE_TEST_OP( (_lhs_), >,  (_rhs_) )
#define TEST_GE( _lhs_, _rhs_ )		__PRIVATE_TEST_OP( (_lhs_), >=, (_rhs_) )
#define TEST_Lt( _lhs_, _rhs_ )		__PRIVATE_TEST_OP( (_lhs_), <,  (_rhs_) )
#define TEST_LE( _lhs_, _rhs_ )		__PRIVATE_TEST_OP( (_lhs_), <=, (_rhs_) )


#define __PRIVATE_CHECK_THROW( _var_, _check_, ... ) \
	bool	_var_ = false; \
	try { \
		__VA_ARGS__ \
	} catch(...) { \
		_var_ = true; \
	} \
	TEST( _check_ _var_ ); \


#define TEST_THROW( ... )	{ __PRIVATE_CHECK_THROW( AE_PRIVATE_UNITE_RAW( test_is_throw_, __LINE__ ), true  ==, __VA_ARGS__ ); }
#define TEST_NOTHROW( ... )	{ __PRIVATE_CHECK_THROW( AE_PRIVATE_UNITE_RAW( test_is_throw_, __LINE__ ), false ==, __VA_ARGS__ ); }


#define TEST_PASSED()	AE_LOGI( String{AE_FUNCTION_NAME} + " - passed" );
