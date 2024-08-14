// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VulkanLoader.h"

# include <vk_video/vulkan_video_codecs_common.h>
# include <vk_video/vulkan_video_codec_h264std.h>
# include <vk_video/vulkan_video_codec_h264std_decode.h>
# include <vk_video/vulkan_video_codec_h264std_encode.h>
# include <vk_video/vulkan_video_codec_h265std.h>
# include <vk_video/vulkan_video_codec_h265std_decode.h>
# include <vk_video/vulkan_video_codec_h265std_encode.h>

# include "graphics/Vulkan/Video/VVideoSession.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Private/EnumToString.h"
# include "graphics/Vulkan/Video/VVideoUtils.cpp.h"

namespace AE::Graphics
{
	StaticAssert( IGfxMemAllocator::VideoStorageArr_t::capacity() == VConfig::MaxVideoMemReq );

/*
=================================================
	destructor
=================================================
*/
	VVideoSession::~VVideoSession () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( _session == Default );
		ASSERT( _params == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VVideoSession::Create (VResourceManager &resMngr, const VideoSessionDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _session == Default and _params == Default );
		CHECK_ERR( _memAllocator == null );
		CHECK_ERR( allocator != null );

		// validate desc
		{
			const bool	encode_mode		= desc.profile.mode == EVideoCodecMode::Encode;
			const bool	decode_mode		= desc.profile.mode == EVideoCodecMode::Decode;
			const bool	encode_queue	= desc.queue == EQueueType::VideoEncode;
			const bool	decode_queue	= desc.queue == EQueueType::VideoDecode;

			CHECK_ERR( encode_mode or decode_mode );
			CHECK_ERR( encode_queue or decode_queue );
			CHECK_ERR( (encode_mode == encode_queue) and (decode_mode == decode_queue) );
		}

		auto&	dev		= resMngr.GetDevice();
		auto	queue	= dev.GetQueue( desc.queue );
		CHECK_ERR( queue );

		_desc = desc;
		CHECK_ERR( Validate( dev, INOUT _desc ));

		VkVideoProfileInfoKHR					profile_info	= {};
		VkVideoSessionCreateInfoKHR				session_ci		= {};
		VkVideoSessionParametersCreateInfoKHR	params_ci		= {};

		profile_info.sType						= VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR;
		profile_info.videoCodecOperation		= VEnumCast( _desc.profile.mode, _desc.profile.codec );
		profile_info.chromaSubsampling			= VEnumCast( _desc.profile.chromaSubsampling );
		profile_info.lumaBitDepth				= VEnumCast_VideoComponentBitDepth( _desc.profile.lumaBitDepth );
		profile_info.chromaBitDepth				= VEnumCast_VideoComponentBitDepth( _desc.profile.chromaBitDepth );

		session_ci.sType						= VK_STRUCTURE_TYPE_VIDEO_SESSION_CREATE_INFO_KHR;
		session_ci.queueFamilyIndex				= uint(queue->familyIndex);
		session_ci.flags						= 0;
		session_ci.pVideoProfile				= &profile_info;
		session_ci.pictureFormat				= VEnumCast( _desc.pictureFormat );
		session_ci.maxCodedExtent				= { _desc.maxCodedExtent.x, _desc.maxCodedExtent.y };
		session_ci.referencePictureFormat		= VEnumCast( _desc.referencePictureFormat );
		session_ci.maxDpbSlots					= _desc.maxDpbSlots;
		session_ci.maxActiveReferencePictures	= _desc.maxActiveReferencePictures;

		params_ci.sType							= VK_STRUCTURE_TYPE_VIDEO_SESSION_PARAMETERS_CREATE_INFO_KHR;

		switch_enum( profile_info.videoCodecOperation )
		{
			// decode h264
			case VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR :
			{
				VkVideoDecodeH264ProfileInfoKHR*					decode_h264_profile;		AllocateOnStack_ZeroMem( OUT decode_h264_profile,	 1 );
				VkVideoDecodeH264SessionParametersCreateInfoKHR*	decode_h264_params;			AllocateOnStack_ZeroMem( OUT decode_h264_params,	 1 );
				VkVideoDecodeH264SessionParametersAddInfoKHR*		decode_h264_params_add;		AllocateOnStack_ZeroMem( OUT decode_h264_params_add, 1 );
				VkExtensionProperties*								decode_h264_stdhdr;			AllocateOnStack_ZeroMem( OUT decode_h264_stdhdr,	 1 );
				CHECK_ERR(	decode_h264_profile		!= null	and
							decode_h264_params		!= null	and
							decode_h264_params_add	!= null	and
							decode_h264_stdhdr		!= null	);

				CopyCString( OUT decode_h264_stdhdr->extensionName, VK_STD_VULKAN_VIDEO_CODEC_H264_DECODE_EXTENSION_NAME );
				decode_h264_stdhdr->specVersion			= VK_STD_VULKAN_VIDEO_CODEC_H264_DECODE_SPEC_VERSION;

				profile_info.pNext						= decode_h264_profile;
				session_ci.pStdHeaderVersion			= decode_h264_stdhdr;
				params_ci.pNext							= decode_h264_params;

				decode_h264_profile->sType				= VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR;
				decode_h264_profile->stdProfileIdc		= STD_VIDEO_H264_PROFILE_IDC_MAIN;
				decode_h264_profile->pictureLayout		= VK_VIDEO_DECODE_H264_PICTURE_LAYOUT_INTERLACED_INTERLEAVED_LINES_BIT_KHR;

				decode_h264_params->sType				= VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_SESSION_PARAMETERS_CREATE_INFO_KHR;
				decode_h264_params->pParametersAddInfo	= decode_h264_params_add;

				decode_h264_params_add->sType			= VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_SESSION_PARAMETERS_ADD_INFO_KHR;
				break;
			}

			// decode h265
			case VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR :
			{
				VkVideoDecodeH265ProfileInfoKHR*					decode_h265_profile;		AllocateOnStack_ZeroMem( OUT decode_h265_profile,	 1 );
				VkVideoDecodeH265SessionParametersCreateInfoKHR*	decode_h265_params;			AllocateOnStack_ZeroMem( OUT decode_h265_params,	 1 );
				VkVideoDecodeH265SessionParametersAddInfoKHR*		decode_h265_params_add;		AllocateOnStack_ZeroMem( OUT decode_h265_params_add, 1 );
				VkExtensionProperties*								decode_h265_stdhdr;			AllocateOnStack_ZeroMem( OUT decode_h265_stdhdr,	 1 );
				CHECK_ERR(	decode_h265_profile		!= null	and
							decode_h265_params		!= null	and
							decode_h265_params_add	!= null	and
							decode_h265_stdhdr		!= null	);

				CopyCString( OUT decode_h265_stdhdr->extensionName, VK_STD_VULKAN_VIDEO_CODEC_H265_DECODE_EXTENSION_NAME );
				decode_h265_stdhdr->specVersion			= VK_STD_VULKAN_VIDEO_CODEC_H265_DECODE_SPEC_VERSION;

				profile_info.pNext						= decode_h265_profile;
				session_ci.pStdHeaderVersion			= decode_h265_stdhdr;
				params_ci.pNext							= decode_h265_params;

				decode_h265_profile->sType				= VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_PROFILE_INFO_KHR;
				decode_h265_profile->stdProfileIdc		= STD_VIDEO_H265_PROFILE_IDC_MAIN;

				decode_h265_params->sType				= VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_SESSION_PARAMETERS_CREATE_INFO_KHR;
				decode_h265_params->pParametersAddInfo	= decode_h265_params_add;

				decode_h265_params_add->sType			= VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_SESSION_PARAMETERS_ADD_INFO_KHR;
				break;
			}

			// encode h264
			case VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR :
			{
				VkVideoEncodeH264ProfileInfoKHR*					encode_h264_profile;		AllocateOnStack_ZeroMem( OUT encode_h264_profile,	 1 );
				VkVideoEncodeH264SessionParametersCreateInfoKHR*	encode_h264_params;			AllocateOnStack_ZeroMem( OUT encode_h264_params,	 1 );
				VkVideoEncodeH264SessionParametersAddInfoKHR*		encode_h264_params_add;		AllocateOnStack_ZeroMem( OUT encode_h264_params_add, 1 );
				VkExtensionProperties*								encode_h264_stdhdr;			AllocateOnStack_ZeroMem( OUT encode_h264_stdhdr,	 1 );
				CHECK_ERR(	encode_h264_profile		!= null	and
							encode_h264_params		!= null	and
							encode_h264_params_add	!= null	and
							encode_h264_stdhdr		!= null	);

				CopyCString( OUT encode_h264_stdhdr->extensionName, VK_STD_VULKAN_VIDEO_CODEC_H264_ENCODE_EXTENSION_NAME );
				encode_h264_stdhdr->specVersion			= VK_STD_VULKAN_VIDEO_CODEC_H264_ENCODE_SPEC_VERSION;

				profile_info.pNext						= encode_h264_profile;
				session_ci.pStdHeaderVersion			= encode_h264_stdhdr;
				params_ci.pNext							= encode_h264_params;

				encode_h264_profile->sType				= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_PROFILE_INFO_KHR;
				encode_h264_profile->stdProfileIdc		= STD_VIDEO_H264_PROFILE_IDC_MAIN;

				encode_h264_params->sType				= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_SESSION_PARAMETERS_CREATE_INFO_KHR;
				encode_h264_params->pParametersAddInfo	= encode_h264_params_add;

				encode_h264_params_add->sType			= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_SESSION_PARAMETERS_ADD_INFO_KHR;
				break;
			}

			// encode h265
			case VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR :
			{
				VkVideoEncodeH265ProfileInfoKHR*					encode_h265_profile;		AllocateOnStack_ZeroMem( OUT encode_h265_profile,	 1 );
				VkVideoEncodeH265SessionParametersCreateInfoKHR*	encode_h265_params;			AllocateOnStack_ZeroMem( OUT encode_h265_params,	 1 );
				VkVideoEncodeH265SessionParametersAddInfoKHR*		encode_h265_params_add;		AllocateOnStack_ZeroMem( OUT encode_h265_params_add, 1 );
				VkExtensionProperties*								encode_h265_stdhdr;			AllocateOnStack_ZeroMem( OUT encode_h265_stdhdr,	 1 );
				CHECK_ERR(	encode_h265_profile		!= null	and
							encode_h265_params		!= null	and
							encode_h265_params_add	!= null	and
							encode_h265_stdhdr		!= null	);

				CopyCString( OUT encode_h265_stdhdr->extensionName, VK_STD_VULKAN_VIDEO_CODEC_H265_ENCODE_EXTENSION_NAME );
				encode_h265_stdhdr->specVersion			= VK_STD_VULKAN_VIDEO_CODEC_H265_ENCODE_SPEC_VERSION;

				profile_info.pNext						= encode_h265_profile;
				session_ci.pStdHeaderVersion			= encode_h265_stdhdr;
				params_ci.pNext							= encode_h265_params;

				encode_h265_profile->sType				= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_PROFILE_INFO_KHR;
				encode_h265_profile->stdProfileIdc		= STD_VIDEO_H265_PROFILE_IDC_MAIN;

				encode_h265_params->sType				= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_SESSION_PARAMETERS_CREATE_INFO_KHR;
				encode_h265_params->pParametersAddInfo	= encode_h265_params_add;

				encode_h265_params_add->sType			= VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_SESSION_PARAMETERS_ADD_INFO_KHR;
				break;
			}

			case VK_VIDEO_CODEC_OPERATION_DECODE_AV1_BIT_KHR :
				// TODO

			case VK_VIDEO_CODEC_OPERATION_NONE_KHR :
			case VK_VIDEO_CODEC_OPERATION_FLAG_BITS_MAX_ENUM_KHR :
			default_unlikely :
				RETURN_ERR( "unsupported video codec op" );
		}
		switch_end

		VK_CHECK_ERR( dev.vkCreateVideoSessionKHR( dev.GetVkDevice(), &session_ci, null, OUT &_session ));
		dev.SetObjectName( _session, dbgName, VK_OBJECT_TYPE_VIDEO_SESSION_KHR );

		CHECK_ERR( allocator->AllocForVideoSession( _session, _desc.memType, OUT _memStorages ));
		_memAllocator = RVRef(allocator);

		params_ci.videoSession = _session;

		VK_CHECK_ERR( dev.vkCreateVideoSessionParametersKHR( dev.GetVkDevice(), &params_ci, null, OUT &_params ));
		dev.SetObjectName( _params, dbgName, VK_OBJECT_TYPE_VIDEO_SESSION_PARAMETERS_KHR );

		// TODO: vkUpdateVideoSessionParametersKHR

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VVideoSession::Destroy (VResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		auto&	dev = resMngr.GetDevice();

		if ( _params != Default )
			dev.vkDestroyVideoSessionParametersKHR( dev.GetVkDevice(), _params, null );

		if ( _session != Default )
			dev.vkDestroyVideoSessionKHR( dev.GetVkDevice(), _session, null );

		if ( _memAllocator )
		{
			for (auto& st : _memStorages) {
				CHECK( _memAllocator->Dealloc( INOUT st ));
			}
		}

		_session		= Default;
		_params			= Default;
		_params			= Default;
		_desc			= Default;
		_memAllocator	= null;
		_memStorages.clear();

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	_FindPixFormatInVideoFormatProperties
=================================================
*/
namespace
{
	ND_ static bool  _FindPixFormatInVideoFormatProperties (INOUT EPixelFormat &fmt, ArrayView<VkVideoFormatPropertiesKHR> vformats, StringView name)
	{
		if ( vformats.empty() )
			return false;

		if ( fmt == Default )
		{
			for (auto& vf : vformats)
			{
				if_unlikely( vf.imageTiling	== VK_IMAGE_TILING_OPTIMAL	and
							 vf.imageType	== VK_IMAGE_TYPE_2D			)
				{
					fmt = AEEnumCast( vf.format );
					return true;
				}
			}
			return false;
		}
		else
		{
			const VkFormat	pix_fmt = VEnumCast( fmt );

			for (auto& vf : vformats)
			{
				if_unlikely( vf.format		== pix_fmt					and
							 vf.imageTiling	== VK_IMAGE_TILING_OPTIMAL	and
							 vf.imageType	== VK_IMAGE_TYPE_2D			)
					return true;
			}

			#if AE_DBG_GRAPHICS
			{
				String	str;
				str << "'" << name << " = " << ToString( fmt ) << "' is not supported, available formats: ";

				for (auto& vf : vformats) {
					str << ToString( AEEnumCast( vf.format )) << ", ";
				}
				str.pop_back();
				str.pop_back();

				AE_LOGI( str );
			}
			#else
				Unused( name );
			#endif
			return false;
		}
	}
}
/*
=================================================
	Validate
=================================================
*/
	bool  VVideoSession::Validate (const VDevice &dev, INOUT VideoSessionDesc &desc) __NE___
	{
		return WithVideoProfile( dev, desc.profile,
				[&dev, &desc] (const VkVideoProfileInfoKHR &profileInfo, const VkVideoCapabilitiesKHR &capabilities) -> bool
				{
					desc.maxDpbSlots				= Min( desc.maxDpbSlots, capabilities.maxDpbSlots );
					desc.maxActiveReferencePictures	= Min( desc.maxActiveReferencePictures, capabilities.maxActiveReferencePictures );

					if ( All( desc.maxCodedExtent == uint2{0} )) {
						desc.maxCodedExtent.x		= capabilities.minCodedExtent.width;
						desc.maxCodedExtent.y		= capabilities.minCodedExtent.height;
					}
					if ( All( desc.maxCodedExtent == UMax )) {
						desc.maxCodedExtent.x		= capabilities.maxCodedExtent.width;
						desc.maxCodedExtent.y		= capabilities.maxCodedExtent.height;
					}
					CHECK_ERR( All( desc.maxCodedExtent >= uint2{capabilities.minCodedExtent.width, capabilities.minCodedExtent.height} ));
					CHECK_ERR( All( desc.maxCodedExtent <= uint2{capabilities.maxCodedExtent.width, capabilities.maxCodedExtent.height} ));

					// TODO: minBitstreamBufferOffsetAlignment, minBitstreamBufferSizeAlignment

					StaticAssert( uint(EVideoCodecMode::_Count) == 2 );

					if ( desc.profile.mode == EVideoCodecMode::Decode )
					{
						VkPhysicalDeviceVideoFormatInfoKHR	vinfo		= {};
						VkVideoProfileListInfoKHR			prof_list	= {};

						vinfo.sType				= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR;
						vinfo.pNext				= &prof_list;
						vinfo.imageUsage		= VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR;	// decode output picture

						prof_list.sType			= VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR;
						prof_list.profileCount	= 1;
						prof_list.pProfiles		= &profileInfo;

						StaticArray< VkVideoFormatPropertiesKHR, 16 >	vformats	= {};
						uint											count		= uint(vformats.size());
						for (auto& vf : vformats) { vf.sType = VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR; }

						VK_CHECK_ERR( vkGetPhysicalDeviceVideoFormatPropertiesKHR( dev.GetVkPhysicalDevice(), &vinfo, INOUT &count, OUT vformats.data() ));
						CHECK_ERR( _FindPixFormatInVideoFormatProperties( INOUT desc.pictureFormat, ArrayView{ vformats.data(), count }, "pictureFormat" ));

						if ( desc.maxActiveReferencePictures > 0 )
						{
							vinfo.imageUsage = VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR;	// decode output picture and reconstructed picture

							count = uint(vformats.size());
							VK_CHECK_ERR( vkGetPhysicalDeviceVideoFormatPropertiesKHR( dev.GetVkPhysicalDevice(), &vinfo, INOUT &count, OUT vformats.data() ));
							CHECK_ERR( _FindPixFormatInVideoFormatProperties( INOUT desc.referencePictureFormat, ArrayView{ vformats.data(), count }, "referencePictureFormat" ));
						}
					}
					else
					if ( desc.profile.mode == EVideoCodecMode::Encode )
					{
						VkPhysicalDeviceVideoFormatInfoKHR	vinfo		= {};
						VkVideoProfileListInfoKHR			prof_list	= {};

						vinfo.sType				= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR;
						vinfo.pNext				= &prof_list;
						vinfo.imageUsage		= VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR;	// encode input picture

						prof_list.sType			= VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR;
						prof_list.profileCount	= 1;
						prof_list.pProfiles		= &profileInfo;

						StaticArray< VkVideoFormatPropertiesKHR, 16 >	vformats	= {};
						uint											count		= uint(vformats.size());
						for (auto& vf : vformats) { vf.sType = VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR; }

						VK_CHECK_ERR( vkGetPhysicalDeviceVideoFormatPropertiesKHR( dev.GetVkPhysicalDevice(), &vinfo, INOUT &count, OUT vformats.data() ));
						CHECK_ERR( _FindPixFormatInVideoFormatProperties( INOUT desc.pictureFormat, ArrayView{ vformats.data(), count }, "pictureFormat" ));

						if ( desc.maxActiveReferencePictures > 0 )
						{
							vinfo.imageUsage = VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR;	// encode input picture and reconstructed picture

							count = uint(vformats.size());
							VK_CHECK_ERR( vkGetPhysicalDeviceVideoFormatPropertiesKHR( dev.GetVkPhysicalDevice(), &vinfo, INOUT &count, OUT vformats.data() ));
							CHECK_ERR( _FindPixFormatInVideoFormatProperties( INOUT desc.referencePictureFormat, ArrayView{ vformats.data(), count }, "referencePictureFormat" ));
						}
					}
					else
						RETURN_ERR( "unknown video codec mode" );

					switch_enum( profileInfo.videoCodecOperation )
					{
						// decode h264
						case VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR :
						{
							auto*	decode_cap		= Cast<VkVideoDecodeCapabilitiesKHR>( capabilities.pNext );
							auto*	decode_cap_h264	= Cast<VkVideoDecodeH264CapabilitiesKHR>( decode_cap->pNext );

							CHECK_ERR(	decode_cap->sType		== VK_STRUCTURE_TYPE_VIDEO_DECODE_CAPABILITIES_KHR		and
										decode_cap_h264->sType	== VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR	);
							CHECK_ERR( capabilities.stdHeaderVersion.specVersion >= VK_STD_VULKAN_VIDEO_CODEC_H264_DECODE_SPEC_VERSION );

							// TODO
							break;
						}

						// decode h265
						case VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR :
						{
							auto*	decode_cap		= Cast<VkVideoDecodeCapabilitiesKHR>( capabilities.pNext );
							auto*	decode_cap_h265	= Cast<VkVideoDecodeH265CapabilitiesKHR>( decode_cap->pNext );

							CHECK_ERR(	decode_cap->sType		== VK_STRUCTURE_TYPE_VIDEO_DECODE_CAPABILITIES_KHR		and
										decode_cap_h265->sType	== VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_CAPABILITIES_KHR	);
							CHECK_ERR( capabilities.stdHeaderVersion.specVersion >= VK_STD_VULKAN_VIDEO_CODEC_H265_DECODE_SPEC_VERSION );

							// TODO
							break;
						}

						// encode h264
						case VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR :
						{
							auto*	encode_cap		= Cast<VkVideoEncodeCapabilitiesKHR>( capabilities.pNext );
							auto*	encode_cap_h264	= Cast<VkVideoEncodeH264CapabilitiesKHR>( encode_cap->pNext );

							CHECK_ERR(	encode_cap->sType		== VK_STRUCTURE_TYPE_VIDEO_ENCODE_CAPABILITIES_KHR		and
										encode_cap_h264->sType	== VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_CAPABILITIES_KHR	);
							CHECK_ERR( capabilities.stdHeaderVersion.specVersion >= VK_STD_VULKAN_VIDEO_CODEC_H264_ENCODE_SPEC_VERSION );

							// TODO
							break;
						}

						// encode h265
						case VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR :
						{
							auto*	encode_cap		= Cast<VkVideoEncodeCapabilitiesKHR>( capabilities.pNext );
							auto*	encode_cap_h265	= Cast<VkVideoEncodeH265CapabilitiesKHR>( encode_cap->pNext );

							CHECK_ERR(	encode_cap->sType		== VK_STRUCTURE_TYPE_VIDEO_ENCODE_CAPABILITIES_KHR		and
										encode_cap_h265->sType	== VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_CAPABILITIES_KHR	);
							CHECK_ERR( capabilities.stdHeaderVersion.specVersion >= VK_STD_VULKAN_VIDEO_CODEC_H265_ENCODE_SPEC_VERSION );

							// TODO
							break;
						}

						case VK_VIDEO_CODEC_OPERATION_DECODE_AV1_BIT_KHR :
							// TODO

						case VK_VIDEO_CODEC_OPERATION_NONE_KHR :
						case VK_VIDEO_CODEC_OPERATION_FLAG_BITS_MAX_ENUM_KHR :
							break;
					}
					switch_end
					return true;
				});
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  VVideoSession::IsSupported (const VResourceManager &, const VideoSessionDesc &) __NE___
	{
		// TODO
		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
