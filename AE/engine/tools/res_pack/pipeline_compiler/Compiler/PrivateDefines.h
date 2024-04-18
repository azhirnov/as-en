// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once


#define PRIVATE_COMP_RETURN_ERR( _text_, _ret_ )				\
		{if_likely( not _quietWarnings ) {						\
			AE_LOGE( _text_ );									\
		}else_unlikely{											\
			AE_LOGI( _text_ );									\
		}return (_ret_);										\
		}

#define COMP_RETURN_ERR( ... ) \
		PRIVATE_COMP_RETURN_ERR( AE_PRIVATE_GETARG_0( __VA_ARGS__ ), AE_PRIVATE_GETARG_1( __VA_ARGS__, ::AE::Base::Default ))


#define PRIVATE_COMP_CHECK_ERR( _expr_, _text_, _ret_ )			\
		{if_likely(( _expr_ )) {}								\
		 else_unlikely											\
			PRIVATE_COMP_RETURN_ERR( _text_, (_ret_) )			\
		}

#define PRIVATE_COMP_CHECK_LOG( _expr_, _log_, _text_, _ret_ )	\
		{if_likely(( _expr_ )) {}								\
		 else_unlikely{											\
			(_log_) << _text_;									\
			return (_ret_);										\
		}}

#define COMP_CHECK_ERR( /* expr, message */... )																		\
		PRIVATE_COMP_CHECK_ERR(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ),														\
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE_TOSTRING( AE_PRIVATE_GETARG_0( __VA_ARGS__ ))),	\
								::AE::Base::Default )

#define COMP_CHECK_LOG( /* expr, log, message */... )																	\
		PRIVATE_COMP_CHECK_LOG(	AE_PRIVATE_GETARG_0( __VA_ARGS__ ),														\
								AE_PRIVATE_GETARG_1( __VA_ARGS__, log ),												\
								AE_PRIVATE_GETARG_2( __VA_ARGS__, AE_TOSTRING( AE_PRIVATE_GETARG_0( __VA_ARGS__ ))),	\
								::AE::Base::Default )

