// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

#if 0 //ndef AE_DEBUG
#	define VK_CALL( ... )		{ (void)(__VA_ARGS__); }
#	define VK_CHECK( ... )		{ if ((__VA_ARGS__) != VK_SUCCESS) return false; }

#else
#	define VK_CALL( /* expr */... ) \
	{ \
		const ::VkResult __vk_err__ = (__VA_ARGS__); \
		Unused( ::AE::Graphics::__vk_CheckErrors( __vk_err__, AE_TOSTRING( __VA_ARGS__ ), AE_FUNCTION_NAME, __FILE__, __LINE__ )); \
	}

#	define AE_PRIVATE_VK_CALL_R( _expr_, _msg_, _ret_, ... ) \
	{ \
		const ::VkResult __vk_err__ = (_expr_); \
		if ( not ::AE::Graphics::__vk_CheckErrors( __vk_err__, (_msg_), AE_FUNCTION_NAME, __FILE__, __LINE__ )) \
			return _ret_; \
	}

#	define AE_PRIVATE_VK_CALL2_R( _func_, _ret_, ... ) \
		AE_PRIVATE_VK_CALL_R( _func_, AE_TOSTRING( _func_ ), _ret_ )

#	define VK_CHECK( /* expr, return_on_error */... ) \
		AE_PRIVATE_VK_CALL2_R( AE_PRIVATE_GETARG_0( __VA_ARGS__ ), AE_PRIVATE_GETARG_1( __VA_ARGS__, ::AE::Base::Default ))

#	define VK_ERR( _err_, /* msg, return_on_error */... ) \
		AE_PRIVATE_VK_CALL_R( (_err_), AE_PRIVATE_GETARG_0( __VA_ARGS__ ), AE_PRIVATE_GETARG_1( __VA_ARGS__, ::AE::Base::Default ))
#endif


namespace AE::Graphics
{
	ND_ bool __vk_CheckErrors (VkResult errCode, const char *vkcall, const char *func, const char *file, int line);
	
} // AE::Graphics

#endif // AE_ENABLE_VULKAN
