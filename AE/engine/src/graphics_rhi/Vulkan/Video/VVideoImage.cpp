// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Video/VVideoImage.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/Video/VVideoUtils.cpp.h"
# include "graphics_rhi/Vulkan/Resources/VSampler.h"

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
	bool  VVideoImage::Create (ResourceManager &resMngr, const VideoImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _image == Default );
		CHECK_ERR( _view  == Default );
		CHECK_ERR( allocator );

		if ( not desc.profiles.empty() )
			return _CreateForVideo( resMngr, desc, RVRef(allocator), dbgName );
		else
			return _CreateForYcbcr( resMngr, desc, RVRef(allocator), dbgName );
	}

/*
=================================================
	_CreateForVideo
=================================================
*/
	bool  VVideoImage::_CreateForVideo (ResourceManager &resMngr, const VideoImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		ASSERT( not desc.profiles.empty() );
		CHECK_ERR( desc.videoUsage != Default );

		_desc = desc;

		VkImageViewCreateInfo	view_ci		= {};
		ValidationParams		params;

		auto&	dev = resMngr.GetDevice();
		CHECK_ERR( Validate( dev, INOUT _desc, OUT view_ci, OUT params ));
		_pictureAccessGranularity = params.pictureAccessGranularity;

		ASSERT( _desc.options	 == desc.options );
		ASSERT( _desc.usage		 == desc.usage );
		ASSERT( _desc.videoUsage == desc.videoUsage );

		GRES_CHECK( IsSupported( resMngr, _desc ));

		// create image
		{
			VkImageCreateInfo				image_ci		= {};
			VkVideoProfileListInfoKHR		prof_list		= {};
			VkVideoProfileInfoKHR			profile_info	= {};
			InPlaceLinearAllocator<1024>	alloc;
			const bool						opt_tiling		= AnyBits( _desc.memType, EMemoryType::DeviceLocal );

			prof_list.sType			= VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR;
			prof_list.profileCount	= uint(_desc.profiles.size());
			prof_list.pProfiles		= &profile_info;

			CHECK_ERR( ConvertProfiles( dev, _desc.profiles, alloc, OUT prof_list.pProfiles ));

			image_ci.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_ci.pNext			= &prof_list;
			image_ci.flags			= VEnumCast( _desc.options );
			image_ci.imageType		= VK_IMAGE_TYPE_2D;
			image_ci.format			= view_ci.format;
			image_ci.extent.width	= _desc.dimension.x;
			image_ci.extent.height	= _desc.dimension.y;
			image_ci.extent.depth	= 1;
			image_ci.mipLevels		= 1;
			image_ci.arrayLayers	= _desc.arrayLayers.Get();		// TODO: VK_EXT_ycbcr_image_arrays
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

			ASSERT( image_ci.format == image_ci.format );
			VK_CHECK_ERR( dev.vkCreateImage( dev.GetVkDevice(), &image_ci, null, OUT &_image ));

			VulkanImageDesc2		vk_desc;
			vk_desc.imageHandle		= _image;
			vk_desc.dimension		= ImageDim_t{ _desc.dimension, ushort{1} };
			vk_desc.arrayLayers		= _desc.arrayLayers;
			vk_desc.imageDim		= EImageDim_2D;
			vk_desc.mipLevels		= 1_mipmap;
			vk_desc.format			= _desc.format;
			vk_desc.samples			= 1_samples;
			vk_desc.options			= _desc.options;
			vk_desc.usage			= _desc.usage;
			vk_desc.memType			= _desc.memType;
			vk_desc.queues			= _desc.queues;
			vk_desc.canBeDestroyed	= false;

			if ( AllBits( _desc.options, EImageOpt::SeparatePlanes ))
			{
				vk_desc.allocMemory = false;

				_imageId = resMngr.CreateImage( vk_desc, dbgName );
				CHECK_ERR( _imageId );

				CHECK_ERR( allocator->AllocForVideoImage( _image, _desc, OUT _memStorages ));
				_memAllocator = RVRef(allocator);
			}
			else
			{
				vk_desc.allocMemory = true;

				_imageId = resMngr.CreateImage( vk_desc, dbgName, RVRef(allocator) );
				CHECK_ERR( _imageId );
			}
		}

		VkSamplerYcbcrConversion	ycbcr_conv = Default;
		{
			auto	samp_id	= resMngr.GetSampler( _desc.ycbcrConvPack, _desc.ycbcrConversion );
			CHECK_ERR( samp_id );

			auto*	samp	= resMngr.GetResource( samp_id, True{"incRef"}, True{"quiet"} );
			CHECK_ERR( samp != null );

			_ycbcrSampler	= Strong<SamplerID>{ samp_id };
			ycbcr_conv		= samp->YcbcrConversion();
			CHECK_ERR( ycbcr_conv != Default );
			CHECK_ERR( view_ci.format == samp->YcbcrFormat() );
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

			VulkanImageViewDesc2		vk_desc;
			vk_desc.viewHandle			= _view;
			vk_desc.viewType			= EImage_2D;
			vk_desc.format				= _desc.format;
			vk_desc.aspectMask			= EImageAspect::Color;
			vk_desc.options				= Default;
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
	bool  VVideoImage::_CreateForYcbcr (ResourceManager &resMngr, const VideoImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		ASSERT( desc.profiles.empty() );
		CHECK_ERR( desc.videoUsage == Default );	// for videoUsage must define video profile

		auto&	dev = resMngr.GetDevice();

		_desc = desc;
		_desc.profiles.clear();

		VkImageCreateInfo		image_ci	= {};
		VkImageViewCreateInfo	view_ci		= {};

		GRES_CHECK( IsSupported( resMngr, _desc ));

		// create image
		{
			const bool	opt_tiling	= AnyBits( _desc.memType, EMemoryType::DeviceLocal );

			image_ci.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_ci.flags			= VEnumCast( _desc.options );
			image_ci.imageType		= VK_IMAGE_TYPE_2D;
			image_ci.format			= VEnumCast( _desc.format );
			image_ci.extent.width	= _desc.dimension.x;
			image_ci.extent.height	= _desc.dimension.y;
			image_ci.extent.depth	= 1;
			image_ci.mipLevels		= 1;
			image_ci.arrayLayers	= _desc.arrayLayers.Get();	// TODO: VK_EXT_ycbcr_image_arrays
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


			VulkanImageDesc2		vk_desc;
			vk_desc.imageHandle		= _image;
			vk_desc.dimension		= ImageDim_t{ _desc.dimension, ushort{1} };
			vk_desc.arrayLayers		= _desc.arrayLayers;
			vk_desc.imageDim		= EImageDim_2D;
			vk_desc.mipLevels		= 1_mipmap;
			vk_desc.format			= _desc.format;
			vk_desc.samples			= 1_samples;
			vk_desc.options			= _desc.options;
			vk_desc.usage			= _desc.usage;
			vk_desc.memType			= _desc.memType;
			vk_desc.queues			= _desc.queues;
			vk_desc.canBeDestroyed	= false;

			if ( AllBits( _desc.options, EImageOpt::SeparatePlanes ))
			{
				vk_desc.allocMemory = false;

				_imageId = resMngr.CreateImage( vk_desc, dbgName );
				CHECK_ERR( _imageId );

				CHECK_ERR( allocator->AllocForVideoImage( _image, _desc, OUT _memStorages ));
				_memAllocator = RVRef(allocator);
			}
			else
			{
				vk_desc.allocMemory = true;

				_imageId = resMngr.CreateImage( vk_desc, dbgName, RVRef(allocator) );
				CHECK_ERR( _imageId );
			}
		}

		VkSamplerYcbcrConversion	ycbcr_conv = Default;
		if ( _desc.ycbcrConversion.IsDefined() )
		{
			auto	samp_id	= resMngr.GetSampler( _desc.ycbcrConvPack, _desc.ycbcrConversion );
			CHECK_ERR( samp_id );

			auto*	samp	= resMngr.GetResource( samp_id, True{"incRef"}, True{"quiet"} );
			CHECK_ERR( samp != null );

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

			VulkanImageViewDesc2		vk_desc;
			vk_desc.viewHandle			= _view;
			vk_desc.viewType			= EImage_2D;
			vk_desc.format				= _desc.format;
			vk_desc.aspectMask			= EImageAspect::Color;
			vk_desc.options				= Default;
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
	void  VVideoImage::Destroy (ResourceManager &resMngr) __NE___
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
	ND_ static bool  _ValidatePixFormatInVideoFormatProperties (ArrayView<VkVideoFormatPropertiesKHR> vformats, INOUT VideoImageDesc &desc, OUT VkImageViewCreateInfo &viewCI)
	{
		if ( vformats.empty() )
			return false;

		const bool	req_opt_tiling	= AnyBits( desc.memType, EMemoryType::DeviceLocal );
		const auto	ValidateDesc	= [&] (const VkVideoFormatPropertiesKHR &vf) -> bool
		{{
			EImageUsage			supported_usage;
			EVideoImageUsage	supported_video_usage;
			EMemoryType			supported_mem_type;
			EImageOpt			supported_options		= AEEnumCast( VkImageCreateFlagBits(vf.imageCreateFlags) );

			CHECK_ERR( AEEnumCast( VkImageUsageFlagBits(vf.imageUsageFlags), OUT supported_usage, OUT supported_mem_type, OUT supported_video_usage ));

			desc.memType		&= supported_mem_type;
			desc.usage			&= supported_usage;
			desc.videoUsage		&= supported_video_usage;
			desc.options		&= supported_options;

			//desc.options		|= (AllBits( vf.imageCreateFlags, VK_IMAGE_CREATE_DISJOINT_BIT ) ? EImageOpt::SeparatePlanes : Zero)

			viewCI.format		= vf.format;
			viewCI.components	= vf.componentMapping;

			// TODO: EPixelFormat_DimGranularity
			return true;
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
					return ValidateDesc( vf );
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
					return ValidateDesc( vf );
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
		VkImageViewCreateInfo	temp2;
		ValidationParams		temp3;
		return Validate( dev, INOUT desc, OUT temp2, OUT temp3 );
	}

	bool  VVideoImage::Validate (const VDevice &dev, INOUT VideoImageDesc &desc, OUT VkImageViewCreateInfo &viewCI, OUT ValidationParams &params) __NE___
	{
		VkPhysicalDeviceVideoFormatInfoKHR	vinfo = {};
		InPlaceLinearAllocator<1024>		alloc;
		uint2								min_extent{~0u}, max_extent{0}, granularity{1};

		// get capabilities per profile
		for (auto& prof : desc.profiles)
		{
			VkVideoProfileInfoKHR	profile_info;
			VkVideoCapabilitiesKHR	capabilities;

			if_unlikely( not GetProfileWithCapabilities( dev, prof, alloc, OUT profile_info, OUT capabilities ))
				return false;

			min_extent	= Min( min_extent, uint2{ capabilities.minCodedExtent.width, capabilities.minCodedExtent.height });
			max_extent	= Max( max_extent, uint2{ capabilities.maxCodedExtent.width, capabilities.maxCodedExtent.height });
			granularity	= AlignUp( granularity, uint2{ capabilities.pictureAccessGranularity.width, capabilities.pictureAccessGranularity.height });

			alloc.Discard();

			StaticAssert( uint(EVideoCodecMode::_Count) == 2 );

			if ( prof.mode == EVideoCodecMode::Decode )
			{
				if ( AllBits( desc.videoUsage, EVideoImageUsage::DecodeDst ))
					vinfo.imageUsage |= VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR;	// decode output picture

				if ( AllBits( desc.videoUsage, EVideoImageUsage::DecodeDpb ))
					vinfo.imageUsage |= VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR;	// decode output picture and reconstructed picture
			}
			else
			if ( prof.mode == EVideoCodecMode::Encode )
			{
				if ( AllBits( desc.videoUsage, EVideoImageUsage::EncodeSrc ))
					vinfo.imageUsage |= VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR;	// encode input picture

				if ( AllBits( desc.videoUsage, EVideoImageUsage::EncodeDpb ))
					vinfo.imageUsage |= VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR;	// encode input picture and reconstructed picture
			}
			else
				RETURN_ERR( "unknown video codec mode" );
		}

		// validate extent
		{
			if ( All( desc.dimension == ImageDim2_t{0} ))
				desc.dimension = CheckCast{min_extent};

			if ( All( desc.dimension == UMax ))
				desc.dimension = CheckCast{max_extent};

			CHECK_ERR( All( desc.Dimension2() >= min_extent ));
			CHECK_ERR( All( desc.Dimension2() <= max_extent ));
			CHECK_ERR( All(IsMultipleOf( desc.Dimension2(), granularity )));

			params.pictureAccessGranularity = ushort2{granularity};
		}

		// validate format & flags
		VkVideoProfileListInfoKHR			prof_list	= {};

		prof_list.sType			= VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR;
		prof_list.profileCount	= uint(desc.profiles.size());

		vinfo.sType				= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR;
		vinfo.pNext				= &prof_list;

		CHECK_ERR( ConvertProfiles( dev, desc.profiles, alloc, OUT prof_list.pProfiles ));
		CHECK_ERR( vinfo.imageUsage != 0 );

		StaticArray< VkVideoFormatPropertiesKHR, 16 >	vformats	= {};
		uint											count		= uint(vformats.size());
		for (auto& vf : vformats) { vf.sType = VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR; }

		VK_CHECK_ERR( vkGetPhysicalDeviceVideoFormatPropertiesKHR( dev.GetVkPhysicalDevice(), &vinfo, INOUT &count, OUT vformats.data() ));
		CHECK_ERR( _ValidatePixFormatInVideoFormatProperties( ArrayView{ vformats.data(), count }, INOUT desc, OUT viewCI ));

		return true;
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  VVideoImage::IsSupported (const ResourceManager &resMngr, const VideoImageDesc &desc) __NE___
	{
		const auto&		dev				= resMngr.GetDevice();
		const uint2		dim_granularity = EPixelFormat_DimGranularity( desc.format );
		const bool		opt_tiling		= AnyBits( desc.memType, EMemoryType::DeviceLocal );
		//const uint	plane_count		= EPixelFormat_PlaneCount( desc.format );
		//const bool	is_multiplane	= (plane_count > 0 and plane_count <= 3);
		bool			result			= true;

		result &= dev.GetVExtensions().samplerYcbcrConversion;

		result &= All( IsMultipleOf( desc.Dimension2(), dim_granularity ));

		//result &= (AllBits( desc.options, EImageOpt::SeparatePlanes ) == is_multiplane);
		result &= VImage::CheckFormatFeatures( resMngr, VEnumCast( desc.format ), desc.usage, desc.options, opt_tiling );

		return result;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
