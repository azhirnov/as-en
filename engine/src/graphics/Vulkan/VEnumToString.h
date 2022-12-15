// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Private/EnumToString.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{
	
/*
=================================================
	VkColorSpaceKHR_ToString
=================================================
*/
	ND_ inline StringView  VkColorSpaceKHR_ToString (VkColorSpaceKHR value) __NE___
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR :			return "sRGB Nonlinear";
			case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT :		return "Display P3 Nonlinear";
			case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT :		return "Extended sRGB Linear";
			case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT :			return "Display P3 Linear";
			case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT :			return "DCI P3 Nonlinear";
			case VK_COLOR_SPACE_BT709_LINEAR_EXT :				return "BT709 Linear";
			case VK_COLOR_SPACE_BT709_NONLINEAR_EXT :			return "BT709 Nonlinear";
			case VK_COLOR_SPACE_BT2020_LINEAR_EXT :				return "BT2020 Linear";
			case VK_COLOR_SPACE_HDR10_ST2084_EXT :				return "HDR10 ST2084";
			case VK_COLOR_SPACE_DOLBYVISION_EXT :				return "Dolbyvision";
			case VK_COLOR_SPACE_HDR10_HLG_EXT :					return "HDR10 HLG";
			case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT :			return "AdobeRGB Linear";
			case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT :		return "AdobeRGB Nonlinear";
			case VK_COLOR_SPACE_PASS_THROUGH_EXT :				return "Pass through";
			case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT :	return "Extended sRGB Nonlinear";
			case VK_COLOR_SPACE_DISPLAY_NATIVE_AMD :			return "Display Native AMD";
			case VK_COLOR_SPACE_MAX_ENUM_KHR :					break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown color space type!" );
	}
	
/*
=================================================
	VkPresentModeKHR_ToString
=================================================
*/
	ND_ inline String  VkPresentModeKHR_ToString (VkPresentModeKHR value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case VK_PRESENT_MODE_IMMEDIATE_KHR :					return "Immediate";
			case VK_PRESENT_MODE_MAILBOX_KHR :						return "Mailbox";
			case VK_PRESENT_MODE_FIFO_KHR :							return "FIFO";
			case VK_PRESENT_MODE_FIFO_RELAXED_KHR :					return "FIFO Relaxed";
			case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR :		return "Shared Dmand Refresh";
			case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR :	return "Shared Continoous Refresh";
			case VK_PRESENT_MODE_MAX_ENUM_KHR :						break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown present mode!" );
	}
	
/*
=================================================
	VkSurfaceTransformFlagBitsKHR_ToString
=================================================
*/
	ND_ inline String  VkSurfaceTransformFlagBitsKHR_ToString (VkSurfaceTransformFlagBitsKHR value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR :					return "Identity";
			case VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR :					return "90";
			case VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR :					return "180";
			case VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR :					return "270";
			case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR :			return "HorizontalMirror";
			case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR:	return "HorizontalMirror 90";
			case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR:	return "HorizontalMirror 180";
			case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR:	return "HorizontalMirror 270";
			case VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR :						return "Inherit";
			case VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR :				break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown surface transform!" );
	}
	
/*
=================================================
	VkCompositeAlphaFlagBitsKHR_ToString
=================================================
*/
	ND_ inline String  VkCompositeAlphaFlagBitsKHR_ToString (VkCompositeAlphaFlagBitsKHR value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR :			return "Opaque";
			case VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR :	return "Pre-multiplied";
			case VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR :	return "Post-multiplied";
			case VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR :			return "Inherit";
			case VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown composite alpha!" );
	}
	
/*
=================================================
	VkImageUsageFlags_ToString
=================================================
*/
	ND_ inline String  VkImageUsageFlags_ToString (VkImageUsageFlags value)
	{
		String	result;
		for (VkImageUsageFlags i = 1; i <= value; i <<= 1)
		{
			if ( not AllBits( value, i ))
				continue;

			if ( result.size() )
				result << " | ";

			BEGIN_ENUM_CHECKS();
			switch ( VkImageUsageFlagBits(i) )
			{
				case VK_IMAGE_USAGE_TRANSFER_SRC_BIT :				result << "TransferSrc";				break;
				case VK_IMAGE_USAGE_TRANSFER_DST_BIT :				result << "TranferDst";					break;
				case VK_IMAGE_USAGE_SAMPLED_BIT :					result << "Sampled";					break;
				case VK_IMAGE_USAGE_STORAGE_BIT :					result << "Storage";					break;
				case VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT :			result << "ColorAttachment";			break;
				case VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT :	result << "DepthStencilAttachment";		break;
				case VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT :		result << "TransientAttachment";		break;
				case VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT :			result << "InputAttachment";			break;
				case VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR: result << "ShadingRate";		break;
				case VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT :	result << "FragmentDensityMap";			break;

				case VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR :
				case VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR :
				case VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR :
				case VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR :
				case VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR :
				case VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR :
				case VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI:
				case VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT :
				case VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM :
				case VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM :
				case VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM :			ASSERT(false); break;
			}
			END_ENUM_CHECKS();
		}
		return result;
	}

/*
=================================================
	VkFormat_ToString
=================================================
*/
	ND_ inline StringView  VkFormat_ToString (VkFormat value) __NE___
	{
#		define FMT_BUILDER( _engineFmt_, _vkFormat_ ) \
			case _vkFormat_ : return #_vkFormat_;
		
		switch ( value )
		{
			AE_PRIVATE_VKPIXELFORMATS( FMT_BUILDER )
		}

#		undef FMT_BUILDER

		RETURN_ERR( "invalid pixel format" );
	}

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
