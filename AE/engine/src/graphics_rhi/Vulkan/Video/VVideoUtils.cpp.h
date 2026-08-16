// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics_rhi/Vulkan/VEnumCast.h"

namespace AE::Graphics
{
namespace
{

/*
=================================================
	ConvertProfiles
=================================================
*/
	inline bool  ConvertProfiles (const VDevice &dev, const VideoProfileList &inProfiles, IAllocatorRef alloc, OUT const VkVideoProfileInfoKHR* &outProfiles) __NE___
	{
		const auto&	ext = dev.GetVExtensions();
		CHECK_ERR( ext.videoQueue );

		auto*	out_profiles = alloc.Allocate< VkVideoProfileInfoKHR >( inProfiles.size() );
		CHECK_ERR( out_profiles != null );

		outProfiles = out_profiles;

		for (auto [src_prof, idx] : WithIndex( inProfiles ))
		{
			auto&	dst_prof = out_profiles[idx];

			dst_prof.sType					= VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR;
			dst_prof.videoCodecOperation	= VEnumCast( src_prof.mode, src_prof.codec );
			dst_prof.chromaSubsampling		= VEnumCast( src_prof.chromaSubsampling );
			dst_prof.lumaBitDepth			= VEnumCast( src_prof.lumaBitDepth );
			dst_prof.chromaBitDepth			= VEnumCast( src_prof.chromaBitDepth );

			CHECK_ERR(	dst_prof.videoCodecOperation	!= VK_VIDEO_CODEC_OPERATION_NONE_KHR		and
						dst_prof.chromaSubsampling		!= VK_VIDEO_CHROMA_SUBSAMPLING_INVALID_KHR	and
						dst_prof.lumaBitDepth			!= VK_VIDEO_COMPONENT_BIT_DEPTH_INVALID_KHR	and
						dst_prof.chromaBitDepth			!= VK_VIDEO_COMPONENT_BIT_DEPTH_INVALID_KHR	);

			switch_enum( dst_prof.videoCodecOperation )
			{
				// decode h264
				case VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR :
				{
					CHECK_ERR( ext.videoDecodeQueue and ext.videoDecodeH264 );

					auto*	decode_h264_profile = alloc.Allocate< VkVideoDecodeH264ProfileInfoKHR >();
					CHECK_ERR( decode_h264_profile != null );

					dst_prof.pNext				= decode_h264_profile;
					decode_h264_profile->pNext	= null;
					decode_h264_profile->sType	= VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR;

					if ( auto* spec = UnionGet<VideoProfile::Decode_H264>( src_prof.spec ))
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

					auto*	decode_h265_profile = alloc.Allocate< VkVideoDecodeH265ProfileInfoKHR >();
					CHECK_ERR( decode_h265_profile != null );

					dst_prof.pNext				= decode_h265_profile;
					decode_h265_profile->pNext	= null;
					decode_h265_profile->sType	= VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_PROFILE_INFO_KHR;

					if ( auto* spec = UnionGet<VideoProfile::Decode_H265>( src_prof.spec ))
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

					auto*	encode_h264_profile = alloc.Allocate< VkVideoEncodeH264ProfileInfoKHR >();
					CHECK_ERR( encode_h264_profile != null );

					dst_prof.pNext				= encode_h264_profile;
					encode_h264_profile->pNext	= null;
					encode_h264_profile->sType	= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_PROFILE_INFO_KHR;

					if ( auto* spec = UnionGet<VideoProfile::Encode_H264>( src_prof.spec ))
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

					auto*	encode_h265_profile = alloc.Allocate< VkVideoEncodeH265ProfileInfoKHR >();
					CHECK_ERR( encode_h265_profile != null );

					dst_prof.pNext				= encode_h265_profile;
					encode_h265_profile->pNext	= null;
					encode_h265_profile->sType	= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_PROFILE_INFO_KHR;

					if ( auto* spec = UnionGet<VideoProfile::Encode_H265>( src_prof.spec ))
					{
						encode_h265_profile->stdProfileIdc	= VEnumCast( spec->stdProfileIdc );
					}else{
						encode_h265_profile->stdProfileIdc	= STD_VIDEO_H265_PROFILE_IDC_MAIN;
					}
					break;
				}

				case VK_VIDEO_CODEC_OPERATION_DECODE_VP9_BIT_KHR :
				case VK_VIDEO_CODEC_OPERATION_DECODE_AV1_BIT_KHR :
				case VK_VIDEO_CODEC_OPERATION_ENCODE_AV1_BIT_KHR :
				case VK_VIDEO_CODEC_OPERATION_NONE_KHR :
				case VK_VIDEO_CODEC_OPERATION_FLAG_BITS_MAX_ENUM_KHR :
				default_unlikely :
					RETURN_ERR( "unsupported video codec op" );
			}
			switch_end
		}
		return true;
	}

/*
=================================================
	GetProfileWithCapabilities
=================================================
*/
	inline bool  GetProfileWithCapabilities (const VDevice &dev, const VideoProfile &profile, IAllocatorRef alloc,
											 OUT VkVideoProfileInfoKHR &outProfileInfo, OUT VkVideoCapabilitiesKHR &outCapabilities) __NE___
	{
		outProfileInfo	= {};
		outCapabilities	= {};

		const auto&		ext = dev.GetVExtensions();
		CHECK_ERR( ext.videoQueue );

		outCapabilities.sType	= VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR;

		outProfileInfo.sType				= VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR;
		outProfileInfo.videoCodecOperation	= VEnumCast( profile.mode, profile.codec );
		outProfileInfo.chromaSubsampling	= VEnumCast( profile.chromaSubsampling );
		outProfileInfo.lumaBitDepth			= VEnumCast( profile.lumaBitDepth );
		outProfileInfo.chromaBitDepth		= VEnumCast( profile.chromaBitDepth );

		CHECK_ERR(	outProfileInfo.videoCodecOperation	!= VK_VIDEO_CODEC_OPERATION_NONE_KHR		and
					outProfileInfo.chromaSubsampling	!= VK_VIDEO_CHROMA_SUBSAMPLING_INVALID_KHR	and
					outProfileInfo.lumaBitDepth			!= VK_VIDEO_COMPONENT_BIT_DEPTH_INVALID_KHR	and
					outProfileInfo.chromaBitDepth		!= VK_VIDEO_COMPONENT_BIT_DEPTH_INVALID_KHR	);

		switch_enum( outProfileInfo.videoCodecOperation )
		{
			// decode h264
			case VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR :
			{
				CHECK_ERR( ext.videoDecodeQueue and ext.videoDecodeH264 );

				auto*	decode_cap			= alloc.Allocate< VkVideoDecodeCapabilitiesKHR >();
				auto*	decode_cap_h264		= alloc.Allocate< VkVideoDecodeH264CapabilitiesKHR >();
				auto*	decode_h264_profile	= alloc.Allocate< VkVideoDecodeH264ProfileInfoKHR >();
				CHECK_ERR(	decode_cap			!= null	and
							decode_cap_h264		!= null	and
							decode_h264_profile	!= null );

				outCapabilities.pNext		= decode_cap;

				decode_cap->pNext			= decode_cap_h264;
				decode_cap->sType			= VK_STRUCTURE_TYPE_VIDEO_DECODE_CAPABILITIES_KHR;

				decode_cap_h264->pNext		= null;
				decode_cap_h264->sType		= VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR;

				outProfileInfo.pNext		= decode_h264_profile;

				decode_h264_profile->pNext	= null;
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

				auto*	decode_cap			= alloc.Allocate< VkVideoDecodeCapabilitiesKHR >();
				auto*	decode_cap_h265		= alloc.Allocate< VkVideoDecodeH265CapabilitiesKHR >();
				auto*	decode_h265_profile	= alloc.Allocate< VkVideoDecodeH265ProfileInfoKHR >();
				CHECK_ERR(	decode_cap			!= null	and
							decode_cap_h265		!= null	and
							decode_h265_profile	!= null );

				outCapabilities.pNext		= decode_cap;

				decode_cap->pNext			= decode_cap_h265;
				decode_cap->sType			= VK_STRUCTURE_TYPE_VIDEO_DECODE_CAPABILITIES_KHR;

				decode_cap_h265->pNext		= null;
				decode_cap_h265->sType		= VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_CAPABILITIES_KHR;

				outProfileInfo.pNext		= decode_h265_profile;

				decode_h265_profile->pNext	= null;
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

				auto*	encode_cap			= alloc.Allocate< VkVideoEncodeCapabilitiesKHR >();
				auto*	encode_cap_h264		= alloc.Allocate< VkVideoEncodeH264CapabilitiesKHR >();
				auto*	encode_h264_profile	= alloc.Allocate< VkVideoEncodeH264ProfileInfoKHR >();
				CHECK_ERR(	encode_cap			!= null	and
							encode_cap_h264		!= null	and
							encode_h264_profile	!= null );

				outCapabilities.pNext		= encode_cap;

				encode_cap->pNext			= encode_cap_h264;
				encode_cap->sType			= VK_STRUCTURE_TYPE_VIDEO_ENCODE_CAPABILITIES_KHR;

				encode_cap_h264->pNext		= null;
				encode_cap_h264->sType		= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_CAPABILITIES_KHR;

				outProfileInfo.pNext		= encode_h264_profile;

				encode_h264_profile->pNext	= null;
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

				auto*	encode_cap			= alloc.Allocate< VkVideoEncodeCapabilitiesKHR >();
				auto*	encode_cap_h265		= alloc.Allocate< VkVideoEncodeH265CapabilitiesKHR >();
				auto*	encode_h265_profile	= alloc.Allocate< VkVideoEncodeH265ProfileInfoKHR >();
				CHECK_ERR(	encode_cap			!= null	and
							encode_cap_h265		!= null	and
							encode_h265_profile	!= null );

				outCapabilities.pNext		= encode_cap;

				encode_cap->pNext			= encode_cap_h265;
				encode_cap->sType			= VK_STRUCTURE_TYPE_VIDEO_ENCODE_CAPABILITIES_KHR;

				encode_cap_h265->pNext		= null;
				encode_cap_h265->sType		= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_CAPABILITIES_KHR;

				outProfileInfo.pNext		= encode_h265_profile;

				encode_h265_profile->pNext	= null;
				encode_h265_profile->sType	= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_PROFILE_INFO_KHR;

				if ( auto* spec = UnionGet<VideoProfile::Encode_H265>( profile.spec ))
				{
					encode_h265_profile->stdProfileIdc	= VEnumCast( spec->stdProfileIdc );
				}else{
					encode_h265_profile->stdProfileIdc	= STD_VIDEO_H265_PROFILE_IDC_MAIN;
				}
				break;
			}

			case VK_VIDEO_CODEC_OPERATION_DECODE_VP9_BIT_KHR :
			case VK_VIDEO_CODEC_OPERATION_DECODE_AV1_BIT_KHR :
			case VK_VIDEO_CODEC_OPERATION_ENCODE_AV1_BIT_KHR :
			case VK_VIDEO_CODEC_OPERATION_NONE_KHR :
			case VK_VIDEO_CODEC_OPERATION_FLAG_BITS_MAX_ENUM_KHR :
			default_unlikely :
				RETURN_ERR( "unsupported video codec op" );
		}
		switch_end

		VK_CHECK_ERR( VulkanInstanceFn::vkGetPhysicalDeviceVideoCapabilitiesKHR( dev.GetVkPhysicalDevice(), &outProfileInfo, OUT &outCapabilities ));
		return true;
	}

} // namespace
} // AE::Graphics
