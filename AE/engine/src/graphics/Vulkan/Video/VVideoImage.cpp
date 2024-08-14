// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Video/VVideoImage.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/Video/VVideoUtils.cpp.h"
# include "graphics/Vulkan/Resources/VSampler.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VVideoImage::~VVideoImage () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( _image			== Default );
		ASSERT( _view			== Default );
		ASSERT( _memAllocator	== null );
	}
/*
=================================================
	Create
=================================================
*/
	bool  VVideoImage::Create (VResourceManager &resMngr, const VideoImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _image == Default );
		CHECK_ERR( _view  == Default );
		CHECK_ERR( allocator );

		if ( desc.profile.IsDefined() )
			return _CreateForVideo( resMngr, desc, RVRef(allocator), dbgName );
		else
			return _CreateForYcbcr( resMngr, desc, RVRef(allocator), dbgName );
	}

/*
=================================================
	_CreateForVideo
=================================================
*/
	bool  VVideoImage::_CreateForVideo (VResourceManager &resMngr, const VideoImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		ASSERT( desc.profile.IsDefined() );
		CHECK_ERR( desc.videoUsage != Default );

		_desc = desc;

		VkImageCreateInfo		image_ci	= {};
		VkImageViewCreateInfo	view_ci		= {};

		auto&	dev = resMngr.GetDevice();
		CHECK_ERR( Validate( dev, INOUT _desc, OUT image_ci, OUT view_ci, OUT _pictureAccessGranularity ));

		ASSERT( _desc.options	 == desc.options );
		ASSERT( _desc.usage		 == desc.usage );
		ASSERT( _desc.videoUsage == desc.videoUsage );
		ASSERT( AllBits( image_ci.flags, VK_IMAGE_CREATE_DISJOINT_BIT ) == RangeU{ 1u, 3u }.Contains( EPixelFormat_PlaneCount( _desc.format )) );

		GRES_CHECK( IsSupported( resMngr, _desc ));

		// create image
		{
			VkVideoProfileListInfoKHR	prof_list	 = {};
			VkVideoProfileInfoKHR		profile_info = {};

			CHECK_ERR( ConvertVideoProfile( _desc.profile, OUT profile_info ));

			const bool	opt_tiling	= AnyBits( _desc.memType, EMemoryType::DeviceLocal );

			prof_list.sType			= VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR;
			prof_list.profileCount	= 1;
			prof_list.pProfiles		= &profile_info;

			image_ci.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_ci.pNext			= &prof_list;
			//image_ci.flags		- set in Validate()
			//image_ci.imageType	- set in Validate()
			//image_ci.format		- set in Validate()
			image_ci.extent.width	= _desc.dimension.x;
			image_ci.extent.height	= _desc.dimension.y;
			image_ci.extent.depth	= 1;
			image_ci.mipLevels		= 1;
			image_ci.arrayLayers	= _desc.arrayLayers.Get();
			image_ci.samples		= VK_SAMPLE_COUNT_1_BIT;
			//image_ci.tiling		- set in Validate()
			//image_ci.usage		- set in Validate()
			image_ci.initialLayout	= (opt_tiling ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PREINITIALIZED);

			VQueueFamilyIndices_t	queue_family_indices;

			// setup sharing mode
			if ( _desc.queues != Default )
			{
				dev.GetQueueFamilies( _desc.queues, OUT queue_family_indices );

				image_ci.sharingMode			= VK_SHARING_MODE_CONCURRENT;
				image_ci.pQueueFamilyIndices	= queue_family_indices.data();
				image_ci.queueFamilyIndexCount	= uint(queue_family_indices.size());
			}

			// reset to exclusive mode
			if ( image_ci.queueFamilyIndexCount <= 1 )
			{
				image_ci.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
				image_ci.pQueueFamilyIndices	= null;
				image_ci.queueFamilyIndexCount	= 0;
			}

			VK_CHECK_ERR( dev.vkCreateImage( dev.GetVkDevice(), &image_ci, null, OUT &_image ));


			VulkanImageDesc			vk_desc;
			vk_desc.image			= _image;
			vk_desc.imageType		= image_ci.imageType;
			vk_desc.flags			= VkImageCreateFlagBits(image_ci.flags);
			vk_desc.usage			= VkImageUsageFlagBits(image_ci.usage);
			vk_desc.format			= image_ci.format;
			vk_desc.samples			= image_ci.samples;
			vk_desc.tiling			= image_ci.tiling;
			vk_desc.dimension		= uint3{ _desc.dimension, 1u };
			vk_desc.arrayLayers		= image_ci.arrayLayers;
			vk_desc.mipLevels		= image_ci.mipLevels;
			vk_desc.queues			= _desc.queues;
			vk_desc.memFlags		= VEnumCast( _desc.memType );
			vk_desc.canBeDestroyed	= false;
			vk_desc.allocMemory		= false;

			_imageId = resMngr.CreateImage( vk_desc, dbgName );
			CHECK_ERR( _imageId );

			CHECK_ERR( allocator->AllocForVideoImage( _image, _desc, OUT _memStorages ));
			_memAllocator = RVRef(allocator);
		}

		VkSamplerYcbcrConversion	ycbcr_conv = Default;
		{
			auto	samp_id	= resMngr.GetSampler( Default, _desc.ycbcrConversion );		// TODO
			auto*	samp	= resMngr.GetResource( samp_id, True{"incRef"}, True{"quiet"} );
			if ( samp != null )
			{
				_ycbcrSampler	= Strong<SamplerID>{ samp_id };
				ycbcr_conv		= samp->YcbcrConversion();
				CHECK_ERR( ycbcr_conv != Default );
			}
		}

		// create view
		{
			VkSamplerYcbcrConversionInfo	conv_info = {};
			conv_info.sType			= VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
			conv_info.conversion	= ycbcr_conv;

			view_ci.sType		= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_ci.pNext		= (ycbcr_conv != Default ? &conv_info : null);
			view_ci.flags		= 0;
			view_ci.image		= _image;
			view_ci.viewType	= _desc.arrayLayers.Get() > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
			//view_ci.format	 - set in Validate()
			//view_ci.components - set in Validate()

			view_ci.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
			view_ci.subresourceRange.baseMipLevel	= 0;
			view_ci.subresourceRange.levelCount		= 1;
			view_ci.subresourceRange.baseArrayLayer	= 0;
			view_ci.subresourceRange.layerCount		= _desc.arrayLayers.Get();

			VK_CHECK_ERR( dev.vkCreateImageView( dev.GetVkDevice(), &view_ci, null, OUT &_view ));

			VulkanImageViewDesc			vk_desc;
			vk_desc.view				= _view;
			vk_desc.flags				= VkImageViewCreateFlagBits(view_ci.flags);
			vk_desc.viewType			= view_ci.viewType;
			vk_desc.format				= view_ci.format;
			vk_desc.components			= view_ci.components;
			vk_desc.subresourceRange	= view_ci.subresourceRange;
			vk_desc.canBeDestroyed		= false;

			_viewId = resMngr.CreateImageView( vk_desc, _imageId, dbgName );
			CHECK_ERR( _viewId );
		}

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	_CreateForYcbcr
=================================================
*/
	bool  VVideoImage::_CreateForYcbcr (VResourceManager &resMngr, const VideoImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		ASSERT( not desc.profile.IsDefined() );
		CHECK_ERR( desc.videoUsage == Default );

		_desc = desc;
		_desc.profile = Default;

		auto&	dev = resMngr.GetDevice();

		VkImageCreateInfo		image_ci		= {};
		VkImageViewCreateInfo	view_ci			= {};
		const uint				plane_count		= EPixelFormat_PlaneCount( _desc.format );
		const bool				is_multiplane	= (plane_count > 0 and plane_count <= 3);

		GRES_CHECK( IsSupported( resMngr, _desc ));

		// create image
		{
			const bool	opt_tiling	= AnyBits( _desc.memType, EMemoryType::DeviceLocal );

			image_ci.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_ci.flags			= VEnumCast( _desc.options ) | (is_multiplane ? VK_IMAGE_CREATE_DISJOINT_BIT : Zero);	// TODO: check VK_FORMAT_FEATURE_DISJOINT_BIT
			image_ci.imageType		= VK_IMAGE_TYPE_2D;
			image_ci.format			= VEnumCast( _desc.format );
			image_ci.extent.width	= _desc.dimension.x;
			image_ci.extent.height	= _desc.dimension.y;
			image_ci.extent.depth	= 1;
			image_ci.mipLevels		= 1;
			image_ci.arrayLayers	= _desc.arrayLayers.Get();
			image_ci.samples		= VK_SAMPLE_COUNT_1_BIT;
			image_ci.tiling			= opt_tiling ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
			image_ci.usage			= VEnumCast( _desc.usage, _desc.memType ) | VEnumCast( _desc.videoUsage );
			image_ci.initialLayout	= (opt_tiling ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PREINITIALIZED);

			VQueueFamilyIndices_t	queue_family_indices;

			// setup sharing mode
			if ( _desc.queues != Default )
			{
				dev.GetQueueFamilies( _desc.queues, OUT queue_family_indices );

				image_ci.sharingMode			= VK_SHARING_MODE_CONCURRENT;
				image_ci.pQueueFamilyIndices	= queue_family_indices.data();
				image_ci.queueFamilyIndexCount	= uint(queue_family_indices.size());
			}

			// reset to exclusive mode
			if ( image_ci.queueFamilyIndexCount <= 1 )
			{
				image_ci.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
				image_ci.pQueueFamilyIndices	= null;
				image_ci.queueFamilyIndexCount	= 0;
			}

			VK_CHECK_ERR( dev.vkCreateImage( dev.GetVkDevice(), &image_ci, null, OUT &_image ));


			VulkanImageDesc			vk_desc;
			vk_desc.image			= _image;
			vk_desc.imageType		= image_ci.imageType;
			vk_desc.flags			= VkImageCreateFlagBits(image_ci.flags);
			vk_desc.usage			= VkImageUsageFlagBits(image_ci.usage);
			vk_desc.format			= image_ci.format;
			vk_desc.samples			= image_ci.samples;
			vk_desc.tiling			= image_ci.tiling;
			vk_desc.dimension		= uint3{ _desc.dimension, 1u };
			vk_desc.arrayLayers		= image_ci.arrayLayers;
			vk_desc.mipLevels		= image_ci.mipLevels;
			vk_desc.queues			= _desc.queues;
			vk_desc.memFlags		= VEnumCast( _desc.memType );
			vk_desc.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
			vk_desc.canBeDestroyed	= false;
			vk_desc.allocMemory		= false;

			if ( is_multiplane )
			{
				vk_desc.aspectMask = Zero;
				for (uint i = 0; i < plane_count; ++i)
					vk_desc.aspectMask |= VkImageAspectFlagBits(VK_IMAGE_ASPECT_PLANE_0_BIT << i);
			}

			_imageId = resMngr.CreateImage( vk_desc, dbgName );
			CHECK_ERR( _imageId );

			CHECK_ERR( allocator->AllocForVideoImage( _image, _desc, OUT _memStorages ));
			_memAllocator = RVRef(allocator);
		}

		VkSamplerYcbcrConversion	ycbcr_conv = Default;
		if ( _desc.ycbcrConversion.IsDefined() )
		{
			auto	samp_id	= resMngr.GetSampler( _desc.ycbcrConvPack, _desc.ycbcrConversion );
			CHECK_ERR( samp_id );

			auto*	samp	= resMngr.GetResource( samp_id, True{"incRef"}, True{"quiet"} );

			_ycbcrSampler	= Strong<SamplerID>{ samp_id };
			ycbcr_conv		= samp->YcbcrConversion();
			CHECK_ERR( ycbcr_conv != Default );
			CHECK_ERR( image_ci.format == samp->YcbcrFormat() );
		}

		// create view
		{
			VkSamplerYcbcrConversionInfo	conv_info = {};
			conv_info.sType			= VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
			conv_info.conversion	= ycbcr_conv;

			view_ci.sType		= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_ci.pNext		= (ycbcr_conv != Default ? &conv_info : null);
			view_ci.flags		= 0;
			view_ci.image		= _image;
			view_ci.viewType	= _desc.arrayLayers.Get() > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
			view_ci.format		= image_ci.format;
			view_ci.components	= { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };

			view_ci.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
			view_ci.subresourceRange.baseMipLevel	= 0;
			view_ci.subresourceRange.levelCount		= 1;
			view_ci.subresourceRange.baseArrayLayer	= 0;
			view_ci.subresourceRange.layerCount		= _desc.arrayLayers.Get();

			VK_CHECK_ERR( dev.vkCreateImageView( dev.GetVkDevice(), &view_ci, null, OUT &_view ));

			VulkanImageViewDesc			vk_desc;
			vk_desc.view				= _view;
			vk_desc.flags				= VkImageViewCreateFlagBits(view_ci.flags);
			vk_desc.viewType			= view_ci.viewType;
			vk_desc.format				= view_ci.format;
			vk_desc.components			= view_ci.components;
			vk_desc.subresourceRange	= view_ci.subresourceRange;
			vk_desc.canBeDestroyed		= false;

			_viewId = resMngr.CreateImageView( vk_desc, _imageId, dbgName );
			CHECK_ERR( _viewId );
		}

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VVideoImage::Destroy (VResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		resMngr.ImmediatelyRelease( INOUT _imageId );
		resMngr.ImmediatelyRelease( INOUT _viewId );
		resMngr.ImmediatelyRelease( INOUT _ycbcrSampler );

		auto&	dev = resMngr.GetDevice();

		if ( _view != Default )
			dev.vkDestroyImageView( dev.GetVkDevice(), _view, null );

		if ( _image != Default )
			dev.vkDestroyImage( dev.GetVkDevice(), _image, null );

		if ( _memAllocator )
		{
			for (auto& st : _memStorages) {
				CHECK( _memAllocator->Dealloc( INOUT st ));
			}
		}

		_image			= Default;
		_view			= Default;
		_desc			= Default;
		_imageId		= Default;
		_viewId			= Default;
		_memAllocator	= null;
		_memStorages.clear();

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	_ValidatePixFormatInVideoFormatProperties
=================================================
*/
namespace
{
	ND_ static bool  _ValidatePixFormatInVideoFormatProperties (ArrayView<VkVideoFormatPropertiesKHR> vformats, INOUT VideoImageDesc &desc,
																OUT VkImageCreateInfo &imageCI, OUT VkImageViewCreateInfo &viewCI)
	{
		if ( vformats.empty() )
			return false;

		const bool	req_opt_tiling	= AnyBits( desc.memType, EMemoryType::DeviceLocal );
		const auto	InitImageCI		= [&desc, &imageCI, &viewCI] (const VkVideoFormatPropertiesKHR &vf)
		{{
			imageCI.format		= vf.format;
			imageCI.flags		= vf.imageCreateFlags;
			imageCI.imageType	= vf.imageType;
			imageCI.tiling		= vf.imageTiling;
			imageCI.usage		= vf.imageUsageFlags;

			viewCI.format		= vf.format;
			viewCI.components	= vf.componentMapping;

			EMemoryType	mem_type = Default;
			AEEnumCast( VkImageUsageFlagBits(vf.imageUsageFlags), OUT desc.usage, OUT mem_type, OUT desc.videoUsage );

			desc.options	= AEEnumCast( VkImageCreateFlagBits( vf.imageCreateFlags ));
			desc.memType	&= (vf.imageTiling == VK_IMAGE_TILING_OPTIMAL ? EMemoryType::UnifiedCached : EMemoryType::HostCachedCoherent);

			// TODO: EPixelFormat_DimGranularity
		}};

		if ( desc.format == Default )
		{
			for (auto& vf : vformats)
			{
				const bool	opt_tiling = (vf.imageTiling == VK_IMAGE_TILING_OPTIMAL);

				if_unlikely( req_opt_tiling	== opt_tiling		and
							 vf.imageType	== VK_IMAGE_TYPE_2D	)
				{
					desc.format = AEEnumCast( vf.format );
					InitImageCI( vf );
					return true;
				}
			}
			return false;
		}
		else
		{
			const VkFormat	pix_fmt = VEnumCast( desc.format );

			for (auto& vf : vformats)
			{
				const bool	opt_tiling = (vf.imageTiling == VK_IMAGE_TILING_OPTIMAL);

				if_unlikely( vf.format		== pix_fmt			and
							 req_opt_tiling	== opt_tiling		and
							 vf.imageType	== VK_IMAGE_TYPE_2D	)
				{
					InitImageCI( vf );
					return true;
				}
			}
			return false;
		}
	}
}
/*
=================================================
	Validate
=================================================
*/
	bool  VVideoImage::Validate (const VDevice &dev, INOUT VideoImageDesc &desc) __NE___
	{
		VkImageCreateInfo		temp1;
		VkImageViewCreateInfo	temp2;
		ushort2					temp3;
		return Validate( dev, INOUT desc, OUT temp1, OUT temp2, OUT temp3 );
	}

	bool  VVideoImage::Validate (const VDevice &dev, INOUT VideoImageDesc &desc, OUT VkImageCreateInfo &imageCI,
								 OUT VkImageViewCreateInfo &viewCI, OUT ushort2 &pictureAccessGranularity) __NE___
	{
		return WithVideoProfile( dev, desc.profile,
				[&] (const VkVideoProfileInfoKHR &profileInfo, const VkVideoCapabilitiesKHR &capabilities) -> bool
				{
					if ( All( desc.dimension == uint2{0} )) {
						desc.dimension.x = capabilities.minCodedExtent.width;
						desc.dimension.y = capabilities.minCodedExtent.height;
					}
					if ( All( desc.dimension == UMax )) {
						desc.dimension.x = capabilities.maxCodedExtent.width;
						desc.dimension.y = capabilities.maxCodedExtent.height;
					}
					CHECK_ERR( All( desc.dimension >= uint2{capabilities.minCodedExtent.width, capabilities.minCodedExtent.height} ));
					CHECK_ERR( All( desc.dimension <= uint2{capabilities.maxCodedExtent.width, capabilities.maxCodedExtent.height} ));

					pictureAccessGranularity = ushort2{ uint2{ capabilities.pictureAccessGranularity.width, capabilities.pictureAccessGranularity.height }};

					VkPhysicalDeviceVideoFormatInfoKHR	vinfo		= {};
					VkVideoProfileListInfoKHR			prof_list	= {};

					vinfo.sType				= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR;
					vinfo.pNext				= &prof_list;

					prof_list.sType			= VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR;
					prof_list.profileCount	= 1;
					prof_list.pProfiles		= &profileInfo;

					StaticAssert( uint(EVideoCodecMode::_Count) == 2 );

					if ( desc.profile.mode == EVideoCodecMode::Decode )
					{
						if ( AllBits( desc.videoUsage, EVideoImageUsage::DecodeDst ))
							vinfo.imageUsage |= VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR;	// decode output picture

						if ( AllBits( desc.videoUsage, EVideoImageUsage::DecodeDpb ))
							vinfo.imageUsage |= VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR;	// decode output picture and reconstructed picture
					}
					else
					if ( desc.profile.mode == EVideoCodecMode::Encode )
					{
						if ( AllBits( desc.videoUsage, EVideoImageUsage::EncodeSrc ))
							vinfo.imageUsage |= VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR;	// encode input picture

						if ( AllBits( desc.videoUsage, EVideoImageUsage::EncodeDpb ))
							vinfo.imageUsage |= VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR;	// encode input picture and reconstructed picture
					}
					else
						RETURN_ERR( "unknown video codec mode" );

					CHECK_ERR( vinfo.imageUsage != 0 );

					StaticArray< VkVideoFormatPropertiesKHR, 16 >	vformats	= {};
					uint											count		= uint(vformats.size());
					for (auto& vf : vformats) { vf.sType = VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR; }

					VK_CHECK_ERR( vkGetPhysicalDeviceVideoFormatPropertiesKHR( dev.GetVkPhysicalDevice(), &vinfo, INOUT &count, OUT vformats.data() ));
					CHECK_ERR( _ValidatePixFormatInVideoFormatProperties( ArrayView{ vformats.data(), count }, INOUT desc, OUT imageCI, OUT viewCI ));

					return true;
				});
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  VVideoImage::IsSupported (const VResourceManager &resMngr, const VideoImageDesc &desc) __NE___
	{
		const auto&		dev				= resMngr.GetDevice();
		const uint2		dim_granularity = EPixelFormat_DimGranularity( desc.format );
		bool			result			= true;

		result &= dev.GetVExtensions().samplerYcbcrConversion;

		result &= All( IsMultipleOf( desc.dimension, dim_granularity ));

		return result;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
