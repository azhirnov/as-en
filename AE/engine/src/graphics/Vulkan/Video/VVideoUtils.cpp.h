// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Vulkan/VEnumCast.h"

namespace AE::Graphics
{
namespace
{
/*
=================================================
	ConvertVideoProfile
=================================================
*/
	ND_ bool  ConvertVideoProfile (const VideoProfile &src, OUT VkVideoProfileInfoKHR &dst) __NE___
	{
		dst.sType				= VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR;
		dst.videoCodecOperation	= VEnumCast( src.mode, src.codec );
		dst.chromaSubsampling	= VEnumCast( src.chromaSubsampling );
		dst.lumaBitDepth		= VEnumCast_VideoComponentBitDepth( src.lumaBitDepth );
		dst.chromaBitDepth		= VEnumCast_VideoComponentBitDepth( src.chromaBitDepth );

		CHECK_ERR(	dst.videoCodecOperation	!= VK_VIDEO_CODEC_OPERATION_NONE_KHR		and
					dst.chromaSubsampling	!= VK_VIDEO_CHROMA_SUBSAMPLING_INVALID_KHR	and
					dst.lumaBitDepth		!= VK_VIDEO_COMPONENT_BIT_DEPTH_INVALID_KHR	and
					dst.chromaBitDepth		!= VK_VIDEO_COMPONENT_BIT_DEPTH_INVALID_KHR	);
		return true;
	}

/*
=================================================
	WithVideoProfile
=================================================
*/
	template <typename FN>
	bool  WithVideoProfile (const VDevice &dev, const VideoProfile &profile, FN &&fn) __NE___
	{
		VkVideoProfileInfoKHR	profile_info		= {};
		VkVideoCapabilitiesKHR	out_capabilities	= {};
		const auto&				ext					= dev.GetVExtensions();

		CHECK_ERR( ext.videoQueue );

		out_capabilities.sType	= VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR;
		CHECK_ERR( ConvertVideoProfile( profile, OUT profile_info ));

		switch_enum( profile_info.videoCodecOperation )
		{
			// decode h264
			case VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR :
			{
				CHECK_ERR( ext.videoDecodeQueue and ext.videoDecodeH264 );

				VkVideoDecodeCapabilitiesKHR*		decode_cap;				AllocateOnStack_ZeroMem( OUT decode_cap, 1 );
				VkVideoDecodeH264CapabilitiesKHR*	decode_cap_h264;		AllocateOnStack_ZeroMem( OUT decode_cap_h264, 1 );
				VkVideoDecodeH264ProfileInfoKHR*	decode_h264_profile;	AllocateOnStack_ZeroMem( OUT decode_h264_profile, 1 );
				CHECK_ERR(	decode_cap			!= null	and
							decode_cap_h264		!= null	and
							decode_h264_profile	!= null );

				out_capabilities.pNext		= decode_cap;
				decode_cap->pNext			= decode_cap_h264;
				decode_cap->sType			= VK_STRUCTURE_TYPE_VIDEO_DECODE_CAPABILITIES_KHR;
				decode_cap_h264->sType		= VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR;

				profile_info.pNext			= decode_h264_profile;
				decode_h264_profile->sType	= VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR;

				if ( auto* spec = UnionGet<VideoProfile::Decode_H264>( profile.spec ))
				{
					decode_h264_profile->stdProfileIdc	= VEnumCast( spec->stdProfileIdc );
					decode_h264_profile->pictureLayout	= VEnumCast( spec->pictureLayout );
				}else{
					decode_h264_profile->stdProfileIdc	= STD_VIDEO_H264_PROFILE_IDC_MAIN;
					decode_h264_profile->pictureLayout	= VK_VIDEO_DECODE_H264_PICTURE_LAYOUT_INTERLACED_INTERLEAVED_LINES_BIT_KHR;
				}
				break;
			}

			// decode h265
			case VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR :
			{
				CHECK_ERR( ext.videoDecodeQueue and ext.videoDecodeH265 );

				VkVideoDecodeCapabilitiesKHR*		decode_cap;				AllocateOnStack_ZeroMem( OUT decode_cap, 1 );
				VkVideoDecodeH265CapabilitiesKHR*	decode_cap_h265;		AllocateOnStack_ZeroMem( OUT decode_cap_h265, 1 );
				VkVideoDecodeH265ProfileInfoKHR*	decode_h265_profile;	AllocateOnStack_ZeroMem( OUT decode_h265_profile, 1 );
				CHECK_ERR(	decode_cap			!= null	and
							decode_cap_h265		!= null	and
							decode_h265_profile	!= null );

				out_capabilities.pNext		= decode_cap;
				decode_cap->pNext			= decode_cap_h265;
				decode_cap->sType			= VK_STRUCTURE_TYPE_VIDEO_DECODE_CAPABILITIES_KHR;
				decode_cap_h265->sType		= VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_CAPABILITIES_KHR;

				profile_info.pNext			= decode_h265_profile;
				decode_h265_profile->sType	= VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_PROFILE_INFO_KHR;

				if ( auto* spec = UnionGet<VideoProfile::Decode_H265>( profile.spec ))
				{
					decode_h265_profile->stdProfileIdc	= VEnumCast( spec->stdProfileIdc );
				}else{
					decode_h265_profile->stdProfileIdc	= STD_VIDEO_H265_PROFILE_IDC_MAIN;
				}
				break;
			}

			// encode h264
			case VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR :
			{
				CHECK_ERR( ext.videoEncodeQueue and ext.videoEncodeH264 );

				VkVideoEncodeCapabilitiesKHR*		encode_cap;				AllocateOnStack_ZeroMem( OUT encode_cap, 1 );
				VkVideoEncodeH264CapabilitiesKHR*	encode_cap_h264;		AllocateOnStack_ZeroMem( OUT encode_cap_h264, 1 );
				VkVideoEncodeH264ProfileInfoKHR*	encode_h264_profile;	AllocateOnStack_ZeroMem( OUT encode_h264_profile, 1 );
				CHECK_ERR(	encode_cap			!= null	and
							encode_cap_h264		!= null	and
							encode_h264_profile	!= null );

				out_capabilities.pNext		= encode_cap;
				encode_cap->pNext			= encode_cap_h264;
				encode_cap->sType			= VK_STRUCTURE_TYPE_VIDEO_ENCODE_CAPABILITIES_KHR;
				encode_cap_h264->sType		= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_CAPABILITIES_KHR;

				profile_info.pNext			= encode_h264_profile;
				encode_h264_profile->sType	= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_PROFILE_INFO_KHR;

				if ( auto* spec = UnionGet<VideoProfile::Encode_H264>( profile.spec ))
				{
					encode_h264_profile->stdProfileIdc	= VEnumCast( spec->stdProfileIdc );
				}else{
					encode_h264_profile->stdProfileIdc	= STD_VIDEO_H264_PROFILE_IDC_MAIN;
				}
				break;
			}

			// encode h265
			case VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR :
			{
				CHECK_ERR( ext.videoEncodeQueue and ext.videoEncodeH265 );

				VkVideoEncodeCapabilitiesKHR*		encode_cap;				AllocateOnStack_ZeroMem( OUT encode_cap, 1 );
				VkVideoEncodeH265CapabilitiesKHR*	encode_cap_h265;		AllocateOnStack_ZeroMem( OUT encode_cap_h265, 1 );
				VkVideoEncodeH265ProfileInfoKHR*	encode_h265_profile;	AllocateOnStack_ZeroMem( OUT encode_h265_profile, 1 );
				CHECK_ERR(	encode_cap			!= null	and
							encode_cap_h265		!= null	and
							encode_h265_profile	!= null );

				out_capabilities.pNext		= encode_cap;
				encode_cap->pNext			= encode_cap_h265;
				encode_cap->sType			= VK_STRUCTURE_TYPE_VIDEO_ENCODE_CAPABILITIES_KHR;
				encode_cap_h265->sType		= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_CAPABILITIES_KHR;

				profile_info.pNext			= encode_h265_profile;
				encode_h265_profile->sType	= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_PROFILE_INFO_KHR;

				if ( auto* spec = UnionGet<VideoProfile::Encode_H265>( profile.spec ))
				{
					encode_h265_profile->stdProfileIdc	= VEnumCast( spec->stdProfileIdc );
				}else{
					encode_h265_profile->stdProfileIdc	= STD_VIDEO_H265_PROFILE_IDC_MAIN;
				}
				break;
			}

			case VK_VIDEO_CODEC_OPERATION_NONE_KHR :
			case VK_VIDEO_CODEC_OPERATION_FLAG_BITS_MAX_ENUM_KHR :
			default_unlikely :
				RETURN_ERR( "unsupported video codec op" );
		}
		switch_end

		VK_CHECK_ERR( vkGetPhysicalDeviceVideoCapabilitiesKHR( dev.GetVkPhysicalDevice(), &profile_info, OUT &out_capabilities ));

		return fn( profile_info, out_capabilities );
	}

} // namespace
} // AE::Graphics
