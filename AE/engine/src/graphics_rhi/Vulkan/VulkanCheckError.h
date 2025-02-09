// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "base/Utils/SourceLoc.h"

# ifdef AE_CFG_RELEASE
#	define VK_CHECK( /* expr */... )	{AE::Base::Unused(__VA_ARGS__);}

#	define AE_PRIVATE_VK_CHECK_R( _expr_, _msg_, _ret_, ... )																				\
	{																																		\
		const ::VkResult __vk_err__ = (_expr_);																								\
		if_unlikely( __vk_err__ != VK_SUCCESS )																								\
			return _ret_;																													\
	}

#	define AE_PRIVATE_VK_THROW( _expr_, _msg_, ... )																						\
	{																																		\
		const ::VkResult __vk_err__ = (_expr_);																								\
		if_unlikely( __vk_err__ != VK_SUCCESS )																								\
			throw AE::Graphics::VkException(0) );																							\
	}

# else
#	define VK_CHECK( /* expr */... )																										\
	{																																		\
		const ::VkResult __vk_err__ = (__VA_ARGS__);																						\
		AE::Base::Unused( AE::Graphics::__vk_CheckErrors( __vk_err__, AE_TOSTRING( __VA_ARGS__ ), AE_FUNCTION_NAME, SourceLoc_Current() ));	\
	}

#	define AE_PRIVATE_VK_CHECK_R( _expr_, _msg_, _ret_, ... )																				\
	{																																		\
		const ::VkResult __vk_err__ = (_expr_);																								\
		if_unlikely( not AE::Graphics::__vk_CheckErrors( __vk_err__, (_msg_), AE_FUNCTION_NAME, SourceLoc_Current() ))						\
			return _ret_;																													\
	}

#	define AE_PRIVATE_VK_THROW( _expr_, _msg_, ... )																						\
	{																																		\
		const ::VkResult __vk_err__ = (_expr_);																								\
		AE_PRIVATE_CHECK_THROW( (AE::Graphics::__vk_CheckErrors( __vk_err__, (_msg_), AE_FUNCTION_NAME, SourceLoc_Current() )),				\
								AE::Graphics::VkException(0) );																				\
	}
# endif // AE_CFG_RELEASE


# define AE_PRIVATE_VK_CHECK2_R( _func_, _ret_, ... )																						\
		AE_PRIVATE_VK_CHECK_R( (_func_), AE_TOSTRING( _func_ ), (_ret_) )

# define VK_CHECK_ERR( /* expr, return_on_error */... )																						\
		AE_PRIVATE_VK_CHECK2_R( AE_PRIVATE_GETARG_0( __VA_ARGS__ ), AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default ))

# define VK_CHECK_ERRV( _expr_ )																											\
		AE_PRIVATE_VK_CHECK2_R( (_expr_), void() )

# define VK_ERR( _err_, /* msg, return_on_error */... )																						\
		AE_PRIVATE_VK_CHECK_R( (_err_), AE_PRIVATE_GETARG_0( __VA_ARGS__ ), AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default ))

# define VK_CHECK_THROW( _expr_ )																											\
		AE_PRIVATE_VK_THROW( (_expr_), AE_TOSTRING( _expr_ ))



namespace AE::Graphics
{
	using namespace AE::Base;

	ND_ bool __vk_CheckErrors (VkResult errCode, const char* vkcall, const char* func, const SourceLoc &loc) __NE___;

	enum class VkException {};

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
