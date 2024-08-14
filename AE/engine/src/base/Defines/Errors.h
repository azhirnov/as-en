// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once


// debug break
#if defined(AE_CI_BUILD_TEST) or defined(AE_CI_BUILD_PERF)
# define AE_PRIVATE_BREAK_POINT()		{}

#elif defined(AE_DEBUG)
# if defined(AE_COMPILER_MSVC)
#	define AE_PRIVATE_BREAK_POINT()		{__debugbreak();}

# elif defined(AE_PLATFORM_ANDROID)
#	include <csignal>
#	define AE_PRIVATE_BREAK_POINT()		{std::raise( SIGINT );}

# elif defined(AE_PLATFORM_APPLE)
#	define AE_PRIVATE_BREAK_POINT()		{__builtin_debugtrap();}

# elif defined(AE_PLATFORM_EMSCRIPTEN)
	namespace AE { void _ems_debugbreak (); }
#	define AE_PRIVATE_BREAK_POINT()		{AE::_ems_debugbreak();}

# elif defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)
#	include <csignal>
#	define AE_PRIVATE_BREAK_POINT()		{std::raise( SIGTRAP );}
# endif

#else
# define AE_PRIVATE_BREAK_POINT()		{}
#endif


// exit
#ifndef AE_PRIVATE_EXIT
# if defined(AE_PLATFORM_ANDROID)
#	define AE_PRIVATE_EXIT()			{std::terminate();}
# else
#	define AE_PRIVATE_EXIT()			{::exit( EXIT_FAILURE );}
# endif
#endif


// debug only check
#ifdef AE_CFG_DEBUG
#	define DBG_CHECK								CHECK
#	define DBG_CHECK_MSG							CHECK_MSG
#	define DBG_WARNING( _msg_ )						CHECK_MSG( false, _msg_ )
#else
#	define DBG_CHECK( /* expr */... )				{}
#	define DBG_CHECK_MSG( /* expr, message */... )	{}
#	define DBG_WARNING( /* message */... )			{}
#endif


// debug/dev only check
#ifdef AE_DEBUG
#	define ASSERT									CHECK		// TODO: DBG_CHECK
#	define ASSERT_Eq								CHECK_Eq	// ==
#	define ASSERT_NE								CHECK_NE	// !=
#	define ASSERT_Gt								CHECK_Gt	// >
#	define ASSERT_GE								CHECK_GE	// >=
#	define ASSERT_Lt								CHECK_Lt	// <
#	define ASSERT_LE								CHECK_LE	// <=
#	define ASSERT_MSG								CHECK_MSG
# else
#	define ASSERT( /* expr */... )					{}
#	define ASSERT_Eq( /* lhs, rhs */... )			{}			// ==
#	define ASSERT_NE( /* lhs, rhs */... )			{}			// !=
#	define ASSERT_Gt( /* lhs, rhs */... )			{}			// >
#	define ASSERT_GE( /* lhs, rhs */... )			{}			// >=
#	define ASSERT_Lt( /* lhs, rhs */... )			{}			// <
#	define ASSERT_LE( /* lhs, rhs */... )			{}			// <=
#	define ASSERT_MSG( /* expr, msg */... )			{}
# endif


// debug/dev/ci check
#if defined(AE_DEBUG) or defined(AE_CI_BUILD_TEST)
#	define DEV_CHECK								CHECK
#else
#	define DEV_CHECK( /* expr */... )				{Unused(__VA_ARGS__);}
#endif


// debug/dev/ci check
#if defined(AE_DEBUG) or defined(AE_CI_BUILD_TEST)
#	define NonNull( /* expr */... )					CHECK( (__VA_ARGS__) != null )
#else
#	define NonNull( /* expr */... )					ASSUME( (__VA_ARGS__) != null )
#endif


// log
#ifndef AE_LOG_DBG
# ifdef AE_DEBUG
#	define AE_LOG_DBG								AE_LOGI
# else
#	define AE_LOG_DBG( /* msg, file, line */... )	{}
# endif
#endif

// log info
#ifndef AE_LOGI
#	define AE_LOGI( /* msg, file, line */... )										\
			AE_PRIVATE_LOG_I( AE_PRIVATE_GETARG_0( __VA_ARGS__, "" ),				\
							  AE_PRIVATE_GETARG_1( __VA_ARGS__, __FILE__ ),			\
							  AE_PRIVATE_GETARG_2( __VA_ARGS__, __FILE__, __LINE__ ))
#endif

// log error
#ifndef AE_LOGE
#	define AE_LOGE( /* msg, file, line */... )										\
			AE_PRIVATE_LOG_E( AE_PRIVATE_GETARG_0( __VA_ARGS__, "" ),				\
							  AE_PRIVATE_GETARG_1( __VA_ARGS__, __FILE__ ),			\
							  AE_PRIVATE_GETARG_2( __VA_ARGS__, __FILE__, __LINE__ ))
#endif

// log warning (silent)
#ifndef AE_LOGW
#	define AE_LOGW( /* msg, file, line */... )										\
			AE_PRIVATE_LOG_W( AE_PRIVATE_GETARG_0( __VA_ARGS__, "" ),				\
							  AE_PRIVATE_GETARG_1( __VA_ARGS__, __FILE__ ),			\
							  AE_PRIVATE_GETARG_2( __VA_ARGS__, __FILE__, __LINE__ ))
#endif


// check function return value
#if 1
#	define CHECK_MSG( _expr_, _text_ )												\
		{if_likely( bool{_expr_} ) {}												\
		 else_unlikely {															\
			AE_LOGE( _text_ );														\
		}}

#   define CHECK( /*expr*/... )														\
		CHECK_MSG( (__VA_ARGS__), AE_TOSTRING( __VA_ARGS__ ))
#endif


// check function return value and return error code
#if 1
#	define AE_PRIVATE_CHECK_ERR2( _expr_, _ret_, _text_ )							\
		{if_likely( bool{_expr_} ) {}												\
		 else_unlikely {															\
			AE_LOGE( _text_ );														\
			return (_ret_);															\
		}}

#	define AE_PRIVATE_CHECK_ERR( _expr_, _ret_ )									\
		AE_PRIVATE_CHECK_ERR2( _expr_, _ret_, AE_TOSTRING( _expr_ ))

#	define CHECK_ERR( /* expr, return_if_false */... )								\
		AE_PRIVATE_CHECK_ERR(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ),					\
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default ))

#	define CHECK_ERR_MSG( /* expr, message */... )									\
		AE_PRIVATE_CHECK_ERR2(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ),					\
								AE::Base::Default,									\
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING( AE_PRIVATE_GETARG_0( __VA_ARGS__ ))) )

#	define CHECK_ERRV( /* expr */... )												\
		AE_PRIVATE_CHECK_ERR( (__VA_ARGS__), void() )
#endif


// check function return value and exit
#if 1
#	define CHECK_FATAL_MSG( _expr_, _text_ )										\
		{if_likely( bool{_expr_} ) {}												\
		 else_unlikely {															\
			AE_LOGE( _text_ );														\
			AE_PRIVATE_EXIT();														\
		}}

#	define CHECK_FATAL( /* expr, message */... )									\
		CHECK_FATAL_MSG( AE_PRIVATE_GETARG_0( __VA_ARGS__ ),						\
						 AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING( AE_PRIVATE_GETARG_0( __VA_ARGS__ ))) )
#endif


// return error code
#if 1
#	define AE_PRIVATE_RETURN_ERR( _text_, _ret_ )									\
		{ AE_LOGE( _text_ );  return (_ret_); }

#	define RETURN_ERR( /* msg, return */... )										\
		AE_PRIVATE_RETURN_ERR(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ),					\
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default ))

#	define RETURN_ERRV( _text_ )													\
		{ AE_LOGE( _text_ );  return; }
#endif


// CHECK_ERR for using inside task
#if 1
#	define AE_PRIVATE_CHECK_TASK( _expr_, _text_ )															\
		{if_likely( bool{_expr_} ) {}																		\
		 else_unlikely {																					\
			AE_LOGE( AE_TOSTRING( _text_ ));																\
			StaticAssert( AE::Base::IsBaseOfNoQual< AE::Threading::IAsyncTask, decltype(*this) >);			\
			ASSERT( AE::Base::StringView{"Run"} == AE_FUNCTION_NAME );										\
			return this->OnFailure(); /* call 'IAsyncTask::OnFailure()' */									\
		}}

#	define CHECK_TE( /* expr, message */... )																\
		AE_PRIVATE_CHECK_TASK(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ),											\
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING( __VA_ARGS__ )) )
#endif


// CHECK_ERR for using inside promise
#if 1
#	define CHECK_PE( /* expr, return_if_false */... )														\
		AE_PRIVATE_CHECK_ERR(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ),											\
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Threading::CancelPromise ))

#	define CHECK_PE_MSG( /* expr, message */... )															\
		AE_PRIVATE_CHECK_ERR2(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ),											\
								AE::Threading::CancelPromise,												\
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING( AE_PRIVATE_GETARG_0( __VA_ARGS__ ))) )
#endif


// same as CHECK_ERR for using inside coroutine
#if 1
#	define AE_PRIVATE_CHECK_CORO( _expr_, _text_ )															\
		{if_likely( bool{_expr_} ) {}																		\
		 else_unlikely {																					\
			AE_LOGE( AE_TOSTRING( _text_ ));																\
			co_await AE::Threading::_hidden_::AsyncTaskCoro_Error{};	/* call 'IAsyncTask::OnFailure()' */\
			co_return;	/* exit from coroutine */															\
		}}

#	define CHECK_CE( /* expr, message */... )																\
		AE_PRIVATE_CHECK_CORO(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ),											\
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING( __VA_ARGS__ )) )
#endif


// compile time assert
#if 1
#	define StaticAssert								static_assert

# if AE_PLATFORM_BITS == 32
#	define StaticAssert32							static_assert
# else
#	define StaticAssert32( /* expr, msg */... )		static_assert(true)
# endif

# if AE_PLATFORM_BITS == 64
#	define StaticAssert64							static_assert
# else
#	define StaticAssert64( /* expr, msg */... )		static_assert(true)
# endif

# ifdef AE_ENABLE_EXCEPTIONS
#	define CheckNothrow								static_assert
# else
#	define CheckNothrow( /* expr, msg */... )		static_assert(true)
#endif

# ifdef AE_DEBUG
#	define StaticAssertDbg							static_assert
# else
#	define StaticAssertDbg( /* expr, msg */... )	static_assert(true)
# endif

# ifdef AE_RELEASE
#	define StaticAssertRel							static_assert
# else
#	define StaticAssertRel( /* expr, msg */... )	static_assert(true)
# endif
#endif


//
#if 1
#	define AE_PRIVATE_CHECK_OP( _lhs_, _op_, _rhs_ )													\
	{																									\
		const auto	_tmp_lhs_ = (_lhs_);																\
		const auto	_tmp_rhs_ = (_rhs_);																\
		CHECK_MSG( AE::Math::All( _tmp_lhs_ _op_ _tmp_rhs_ ),											\
			AE::Base::String{AE_TOSTRING(_lhs_)} << " (" << AE::Base::ToString(_tmp_lhs_) << ") " <<	\
			AE_TOSTRING(_op_) << " (" << AE::Base::ToString(_tmp_rhs_) << ") " << AE_TOSTRING(_rhs_))	\
	}
#	define CHECK_Eq( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_OP( (_lhs_), ==, (_rhs_) )
#	define CHECK_NE( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_OP( (_lhs_), !=, (_rhs_) )
#	define CHECK_Gt( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_OP( (_lhs_), >,  (_rhs_) )
#	define CHECK_GE( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_OP( (_lhs_), >=, (_rhs_) )
#	define CHECK_Lt( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_OP( (_lhs_), <,  (_rhs_) )
#	define CHECK_LE( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_OP( (_lhs_), <=, (_rhs_) )

#	define AE_PRIVATE_CHECK_ERR_OP( _lhs_, _op_, _rhs_ )												\
	{																									\
		const auto	_tmp_lhs_ = (_lhs_);																\
		const auto	_tmp_rhs_ = (_rhs_);																\
		CHECK_ERR_MSG( AE::Math::All( _tmp_lhs_ _op_ _tmp_rhs_ ),										\
			AE::Base::String{AE_TOSTRING(_lhs_)} << " (" << AE::Base::ToString(_tmp_lhs_) << ") " <<	\
			AE_TOSTRING(_op_) << " (" << AE::Base::ToString(_tmp_rhs_) << ") " << AE_TOSTRING(_rhs_))	\
	}
#	define CHECK_ERR_Eq( _lhs_, _rhs_ )		AE_PRIVATE_CHECK_ERR_OP( (_lhs_), ==, (_rhs_) )
#	define CHECK_ERR_NE( _lhs_, _rhs_ )		AE_PRIVATE_CHECK_ERR_OP( (_lhs_), !=, (_rhs_) )
#	define CHECK_ERR_Gt( _lhs_, _rhs_ )		AE_PRIVATE_CHECK_ERR_OP( (_lhs_), >,  (_rhs_) )
#	define CHECK_ERR_GE( _lhs_, _rhs_ )		AE_PRIVATE_CHECK_ERR_OP( (_lhs_), >=, (_rhs_) )
#	define CHECK_ERR_Lt( _lhs_, _rhs_ )		AE_PRIVATE_CHECK_ERR_OP( (_lhs_), <,  (_rhs_) )
#	define CHECK_ERR_LE( _lhs_, _rhs_ )		AE_PRIVATE_CHECK_ERR_OP( (_lhs_), <=, (_rhs_) )
#endif


// check and throw exception
#ifdef AE_ENABLE_EXCEPTIONS
#	define AE_PRIVATE_CHECK_THROW_MSG( _expr_, _text_ )													\
		{if_likely( bool{_expr_} ) {}																	\
		 else_unlikely {																				\
			AE_LOGW( _text_ );																			\
			throw AE::Exception{ _text_ };																\
		}}

#	define CHECK_THROW_MSG( /* expr, msg */... )														\
		AE_PRIVATE_CHECK_THROW_MSG(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ),									\
									AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING( __VA_ARGS__ )) )


#	define AE_PRIVATE_CHECK_THROW( _expr_, _exception_ )												\
		{if_likely( bool{_expr_} ) {}																	\
		 else_unlikely {																				\
			AE_LOGW( AE_TOSTRING( _expr_ ));															\
			throw (_exception_);																		\
		}}

#	define CHECK_THROW( /*expr, exception*/... )														\
		AE_PRIVATE_CHECK_THROW(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ),										\
								AE_PRIVATE_GETARG_1( __VA_ARGS__,										\
									AE::Exception{AE_TOSTRING( __VA_ARGS__ )} ))

#	define AE_PRIVATE_CHECK_THROW_OP( _lhs_, _op_, _rhs_ )												\
	{																									\
		const auto	_tmp_lhs_ = (_lhs_);																\
		const auto	_tmp_rhs_ = (_rhs_);																\
		AE_PRIVATE_CHECK_THROW_MSG( AE::Math::All( _tmp_lhs_ _op_ _tmp_rhs_ ),							\
			AE::Base::String{AE_TOSTRING(_lhs_)} << " (" << AE::Base::ToString(_tmp_lhs_) << ") " <<	\
			AE_TOSTRING(_op_) << " (" << AE::Base::ToString(_tmp_rhs_) << ") " << AE_TOSTRING(_rhs_))	\
	}
#	define CHECK_THROW_Eq( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_THROW_OP( (_lhs_), ==, (_rhs_) )
#	define CHECK_THROW_NE( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_THROW_OP( (_lhs_), !=, (_rhs_) )
#	define CHECK_THROW_Gt( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_THROW_OP( (_lhs_), >,  (_rhs_) )
#	define CHECK_THROW_GE( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_THROW_OP( (_lhs_), >=, (_rhs_) )
#	define CHECK_THROW_Lt( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_THROW_OP( (_lhs_), <,  (_rhs_) )
#	define CHECK_THROW_LE( _lhs_, _rhs_ )			AE_PRIVATE_CHECK_THROW_OP( (_lhs_), <=, (_rhs_) )

#else

// TODO: emulate exceptions

#	define CHECK_THROW( /*expr, exception*/... )	CHECK_FATAL( AE_PRIVATE_GETARG_0( __VA_ARGS__ ))
#	define CHECK_THROW_MSG( /* expr, msg */... )	CHECK_FATAL( __VA_ARGS__ )

#endif


// catch exceptions
#ifdef AE_ENABLE_EXCEPTIONS
#	define NOTHROW( ... )																				\
		try { __VA_ARGS__; }																			\
		catch(...) {}


#	define AE_PRIVATE_CATCH_ERR( _src_, _return_on_exc_ )												\
		try { _src_; }																					\
		catch(...) {																					\
			AE_LOGE( AE::Base::String{"caught exception from: "} + AE_TOSTRING(_src_) );				\
			return _return_on_exc_;																		\
		}

#	define NOTHROW_ERR( /* src, return_on_exc*/... )													\
		AE_PRIVATE_CATCH_ERR( AE_PRIVATE_GETARG_0( __VA_ARGS__ ),										\
							  AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default ))

#	define NOTHROW_ERRV( _src_ )																		\
		NOTHROW_ERR( (_src_), void() )

#else
#	define NOTHROW( ... )									{__VA_ARGS__;}
#	define NOTHROW_ERR( /* src, return_on_exc*/... )		{AE_PRIVATE_GETARG_0( __VA_ARGS__ );}
#	define NOTHROW_ERRV( _src_ )							{_src_;}
#endif


// assumption
#if 1
# if __has_cpp_attribute(assume)
#	define AE_PRIVATE_ASSUME( _expr_ )		{ [[assume( bool{_expr_} )]]; }

# elif defined(AE_COMPILER_MSVC)
#	define AE_PRIVATE_ASSUME( _expr_ )		{__assume( bool{_expr_} );}

# elif defined(AE_COMPILER_CLANG)
#	define AE_PRIVATE_ASSUME( _expr_ )		{__builtin_assume( bool{_expr_} );}

# elif defined(AE_COMPILER_GCC)
#	define AE_PRIVATE_ASSUME( _expr_ )		{ if (not bool{_expr_}) __builtin_unreachable(); }
# else
#	error not implemented!
# endif

# ifdef AE_DEBUG
#	define ASSUME							ASSERT
# else
#	define ASSUME							AE_PRIVATE_ASSUME
# endif
#endif


// mark branches of the new code until someone will enter to this brunch
#if defined(AE_CI_BUILD_TEST)
#	define UNTESTED							CHECK_MSG( false, "UNTESTED" )

#elif defined(AE_CI_BUILD_PERF)
#	define UNTESTED							{}

#elif defined(AE_DEBUG)
#	define UNTESTED							AE_PRIVATE_BREAK_POINT()
#else
#	define UNTESTED							{}
#endif


// mark TODO
#if defined(AE_CI_BUILD_TEST) or defined(AE_DEBUG)
#	define TODO( ... )						CHECK_MSG( false, "TODO: "s << __VA_ARGS__ )
#else
#	define TODO( ... )						{}
#endif
