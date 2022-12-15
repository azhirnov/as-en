// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once


// debug break
#ifdef AE_DBG_OR_DEV
# if defined(AE_COMPILER_MSVC)
#	define AE_PRIVATE_BREAK_POINT()		__debugbreak()

# elif defined(AE_PLATFORM_ANDROID)
#	include <csignal>
#	define AE_PRIVATE_BREAK_POINT()		std::raise( SIGINT )

# elif defined(AE_PLATFORM_APPLE)
#	define AE_PRIVATE_BREAK_POINT()		__builtin_debugtrap()

# elif defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)
#	include <csignal>
#	define AE_PRIVATE_BREAK_POINT()		std::raise( SIGTRAP )
# endif

#else
# define AE_PRIVATE_BREAK_POINT()	{}
#endif


// exit
#ifndef AE_PRIVATE_EXIT
# if defined(AE_PLATFORM_ANDROID)
#	define AE_PRIVATE_EXIT()	std::terminate()
# else
#	define AE_PRIVATE_EXIT()	::exit( EXIT_FAILURE )
# endif
#endif


// debug only check
#ifndef ASSERT
# ifdef AE_DEBUG
#	define ASSERT									CHECK		// TODO: DBG_CHECK
#	define ASSERT_Eq								CHECK_Eq	// ==
#	define ASSERT_NE								CHECK_NE	// !=
#	define ASSERT_Gt								CHECK_Gt	// >
#	define ASSERT_GE								CHECK_GE	// >=
#	define ASSERT_Lt								CHECK_Lt	// <
#	define ASSERT_LE								CHECK_LE	// <=
#	define DBG_CHECK_ERR							CHECK_ERR
#	define DBG_CHECK_MSG							CHECK_MSG
#	define DBG_CHECK_ERR_MSG						CHECK_ERR_MSG
#	define DBG_WARNING( _msg_ )						CHECK_MSG( false, _msg_ )
# else
#	define ASSERT( /* expr */... )					{}
#	define ASSERT_Eq( /* lhs, rhs */... )			{}			// ==
#	define ASSERT_NE( /* lhs, rhs */... )			{}			// !=
#	define ASSERT_Gt( /* lhs, rhs */... )			{}			// >
#	define ASSERT_GE( /* lhs, rhs */... )			{}			// >=
#	define ASSERT_Lt( /* lhs, rhs */... )			{}			// <
#	define ASSERT_LE( /* lhs, rhs */... )			{}			// <=
#	define DBG_CHECK_ERR( /* expr, return */... )	{}
#	define DBG_CHECK_MSG( /* expr, message */... )	{}
#	define DBG_CHECK_ERR_MSG( /* expr, msg */... )	{}
#	define DBG_WARNING( /* message */... )			{}
# endif
#endif


// development check
#ifndef DEV_CHECK
# ifdef AE_DBG_OR_DEV
#	define DEV_CHECK								CHECK
#	define DEV_CHECK_ERR							CHECK_ERR
#	define DEV_CHECK_MSG							CHECK_MSG
#	define DEV_CHECK_ERR_MSG						CHECK_ERR_MSG
#	define DEV_WARNING( _msg_ )						CHECK_MSG( false, _msg_ )
# else
#	define DEV_CHECK( /* expr */... )				{}
#	define DEV_CHECK_ERR( /* expr, return */... )	{}
#	define DEV_CHECK_MSG( /* expr, message */... )	{}
#	define DEV_CHECK_ERR_MSG( /* expr, msg */... )	{}
#	define DEV_WARNING( /* message */... )			{}
# endif
#endif


// log
#ifndef AE_LOG_DBG
# ifdef AE_DEBUG
#	define AE_LOG_DBG								AE_LOGI
# else
#	define AE_LOG_DBG( /* msg, file, line */... )	{}
# endif
#endif

#ifndef AE_LOG_DEV
# ifdef AE_DBG_OR_DEV
#	define AE_LOG_DEV								AE_LOGI
# else
#	define AE_LOG_DEV( /* msg, file, line */... )	{}
# endif
#endif

#ifndef AE_LOG_PROF
# ifdef AE_DBG_OR_DEV
#	define AE_LOG_PROF								AE_LOGI
# else
#	define AE_LOG_PROF( /* msg, file, line */... )	{}
# endif
#endif

// log info
#ifndef AE_LOGI
#	define AE_LOGI( /* msg, file, line */... ) \
			AE_PRIVATE_LOG_I( AE_PRIVATE_GETARG_0( __VA_ARGS__, "" ), \
							  AE_PRIVATE_GETARG_1( __VA_ARGS__, __FILE__ ), \
							  AE_PRIVATE_GETARG_2( __VA_ARGS__, __FILE__, __LINE__ ))
#endif

// log error
#ifndef AE_LOGE
#	define AE_LOGE( /* msg, file, line */... ) \
			AE_PRIVATE_LOG_E( AE_PRIVATE_GETARG_0( __VA_ARGS__, "" ), \
							  AE_PRIVATE_GETARG_1( __VA_ARGS__, __FILE__ ), \
							  AE_PRIVATE_GETARG_2( __VA_ARGS__, __FILE__, __LINE__ ))
#endif

// log silent error
#ifndef AE_LOG_SE
#	define AE_LOG_SE( /* msg, file, line */... ) \
			AE_PRIVATE_LOG_SE( AE_PRIVATE_GETARG_0( __VA_ARGS__, "" ), \
							   AE_PRIVATE_GETARG_1( __VA_ARGS__, __FILE__ ), \
							   AE_PRIVATE_GETARG_2( __VA_ARGS__, __FILE__, __LINE__ ))
#endif


// check function return value
#if 1
#	define CHECK_MSG( _expr_, _text_ )						\
		{if_likely(( _expr_ )) {}							\
		 else_unlikely {									\
			AE_LOGE( _text_ );								\
		}}

#   define CHECK( /*expr*/... ) \
		CHECK_MSG( (__VA_ARGS__), AE_TOSTRING( __VA_ARGS__ ))
#endif


// check function return value and return error code
#if 1
#	define AE_PRIVATE_CHECK_ERR2( _expr_, _ret_, _text_ )	\
		{if_likely(( _expr_ )) {}							\
		 else_unlikely {									\
			AE_LOGE( _text_ );								\
			return (_ret_);									\
		}}

#	define AE_PRIVATE_CHECK_ERR( _expr_, _ret_ ) \
		AE_PRIVATE_CHECK_ERR2( _expr_, _ret_, AE_TOSTRING( _expr_ ))

#	define CHECK_ERR( /* expr, return_if_false */... ) \
		AE_PRIVATE_CHECK_ERR(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ), \
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default ))

#	define CHECK_ERR_MSG( /* expr, message */... ) \
		AE_PRIVATE_CHECK_ERR2(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ), \
								AE::Base::Default, \
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING( AE_PRIVATE_GETARG_0( __VA_ARGS__ ))) )

#	define CHECK_ERRV( /* expr */... ) \
		AE_PRIVATE_CHECK_ERR( (__VA_ARGS__), void() )
#endif


// check function return value and exit
#if 1
#	define CHECK_FATAL_MSG( _expr_, _text_ )	\
		{if_likely(( _expr_ )) {}				\
		 else_unlikely {						\
			AE_LOGE( _text_ );					\
			AE_PRIVATE_EXIT();					\
		}}

#	define CHECK_FATAL( /* expr */... ) \
		CHECK_FATAL_MSG( (__VA_ARGS__), AE_TOSTRING( __VA_ARGS__ ))
#endif


// return error code
#if 1
#	define AE_PRIVATE_RETURN_ERR( _text_, _ret_ ) \
		{ AE_LOGE( _text_ );  return (_ret_); }

#	define RETURN_ERR( /* msg, return */... ) \
		AE_PRIVATE_RETURN_ERR( AE_PRIVATE_GETARG_0( __VA_ARGS__ ), AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default ))

#	define RETURN_ERRV( _text_ ) \
		{ AE_LOGE( _text_ );  return; }
#endif


// CHECK_ERR for using inside task
#if 1
#	define AE_PRIVATE_CHECK_TASK( _expr_, _text_ )																			\
		{if_likely(( _expr_ )) {}																							\
		 else_unlikely {																									\
			AE_LOGE( AE_TOSTRING( _text_ ));																				\
			STATIC_ASSERT( AE::Base::IsBaseOf< AE::Threading::IAsyncTask, AE::Base::RemoveAllQualifiers<decltype(*this)> >);\
			ASSERT( AE::Base::StringView{"Run"} == AE_FUNCTION_NAME );														\
			return this->OnFailure(); /* call 'IAsyncTask::OnFailure()' */													\
		}}

#	define CHECK_TE( /* expr, message */... ) \
		AE_PRIVATE_CHECK_TASK(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ), \
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING( __VA_ARGS__ )) )
#endif


// CHECK_ERR for using inside task
#if 1
#	define CHECK_PE( /* expr, return_if_false */... ) \
		AE_PRIVATE_CHECK_ERR(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ), \
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Threading::CancelPromise ))
#endif


// same as CHECK_ERR for using inside coroutine
#if 1
#	define AE_PRIVATE_CHECK_CORO( _expr_, _text_ )															\
		{if_likely(( _expr_ )) {}																			\
		 else_unlikely {																					\
			AE_LOGE( AE_TOSTRING( _text_ ));																\
			co_await AE::Threading::_hidden_::CoroutineRunnerError{};	/* call 'IAsyncTask::OnFailure()' */\
			co_return;	/* exit from coroutine */															\
		}}

#	define CHECK_CE( /* expr, message */... ) \
		AE_PRIVATE_CHECK_CORO(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ), \
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING( __VA_ARGS__ )) )
#endif


// compile time assert
#if 1
#	define STATIC_ASSERT( /* expr, msg */... ) \
		static_assert(	AE_PRIVATE_GETRAW( AE_PRIVATE_GETARG_0( __VA_ARGS__ )), \
						AE_PRIVATE_GETRAW( AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING(__VA_ARGS__))) )
#endif


//
#if 1
#	define AE_PRIVATE_CHECK_OP( _lhs_, _op_, _rhs_ ) \
		CHECK_MSG( AE::Math::All( (_lhs_) _op_ (_rhs_) ), \
			AE::Base::String{AE_TOSTRING(_lhs_)} << " (" << AE::Base::ToString(_lhs_) << ") " << AE_TOSTRING(_op_) << " (" << AE::Base::ToString(_rhs_) << ") " << AE_TOSTRING(_rhs_))

#	define CHECK_Eq( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_OP( (_lhs_), ==, (_rhs_) )
#	define CHECK_NE( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_OP( (_lhs_), !=, (_rhs_) )
#	define CHECK_Gt( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_OP( (_lhs_), >,  (_rhs_) )
#	define CHECK_GE( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_OP( (_lhs_), >=, (_rhs_) )
#	define CHECK_Lt( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_OP( (_lhs_), <,  (_rhs_) )
#	define CHECK_LE( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_OP( (_lhs_), <=, (_rhs_) )

#	define AE_PRIVATE_CHECK_ERR_OP( _lhs_, _op_, _rhs_ ) \
		CHECK_ERR_MSG( AE::Math::All( (_lhs_) _op_ (_rhs_) ), \
			AE::Base::String{AE_TOSTRING(_lhs_)} << " (" << AE::Base::ToString(_lhs_) << ") " << AE_TOSTRING(_op_) << " (" << AE::Base::ToString(_rhs_) << ") " << AE_TOSTRING(_rhs_))

#	define CHECK_ERR_Eq( _lhs_, _rhs_ )		AE_PRIVATE_CHECK_ERR_OP( (_lhs_), ==, (_rhs_) )
#	define CHECK_ERR_NE( _lhs_, _rhs_ )		AE_PRIVATE_CHECK_ERR_OP( (_lhs_), !=, (_rhs_) )
#	define CHECK_ERR_Gt( _lhs_, _rhs_ )		AE_PRIVATE_CHECK_ERR_OP( (_lhs_), >,  (_rhs_) )
#	define CHECK_ERR_GE( _lhs_, _rhs_ )		AE_PRIVATE_CHECK_ERR_OP( (_lhs_), >=, (_rhs_) )
#	define CHECK_ERR_Lt( _lhs_, _rhs_ )		AE_PRIVATE_CHECK_ERR_OP( (_lhs_), <,  (_rhs_) )
#	define CHECK_ERR_LE( _lhs_, _rhs_ )		AE_PRIVATE_CHECK_ERR_OP( (_lhs_), <=, (_rhs_) )
#endif


// 
#if 1
#	define AE_PRIVATE_CHECK_THROW_MSG( _expr_, _text_ )		\
		{if_likely(( _expr_ )) {}							\
		 else_unlikely {									\
			AE_LOGE( _text_ );								\
			throw AE::Exception{ _text_ };					\
		}}

#	define CHECK_THROW_MSG( /* expr, msg */... ) \
		AE_PRIVATE_CHECK_THROW_MSG(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ), \
									AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING( __VA_ARGS__ )) )


#	define AE_PRIVATE_CHECK_THROW( _expr_, _exception_ )	\
		{if_likely(( _expr_ )) {}							\
		 else_unlikely {									\
			throw (_exception_);							\
		}}

#	define CHECK_THROW( /*expr, exception*/... ) \
		AE_PRIVATE_CHECK_THROW(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ), \
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default ))

#	define CATCH( ... )			\
		try { __VA_ARGS__; }	\
		catch(...) {}


#	define AE_PRIVATE_CATCH_ERR( _src_, _return_on_exc_ )									\
		try { _src_; }																		\
		catch(...) {																		\
			AE_LOGE( AE::Base::String{"catched exception from: "} + AE_TOSTRING(_src_) );	\
			return _return_on_exc_;															\
		}

#	define CATCH_ERR( /* expr, return_on_exc*/... ) \
		AE_PRIVATE_CATCH_ERR( AE_PRIVATE_GETARG_0( __VA_ARGS__ ), \
							  AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default ))

#endif
