// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Vulkan/Resources/VImage.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/Utils/NextChain.h"

namespace AE::Graphics
{
namespace {
/*
=================================================
	ChooseAspect
=================================================
*/
	ND_ static VkImageAspectFlagBits  ChooseAspect (EPixelFormat format) __NE___
	{
		VkImageAspectFlagBits	result = Zero;

		if ( EPixelFormat_IsColor( format ))
			result |= VK_IMAGE_ASPECT_COLOR_BIT;
		else
		{
			if ( EPixelFormat_HasDepth( format ))
				result |= VK_IMAGE_ASPECT_DEPTH_BIT;

			if ( EPixelFormat_HasStencil( format ))
				result |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		return result;
	}

/*
=================================================
	CheckFormatFeatures
=================================================
*/
	ND_ static bool  CheckFormatFeatures (const ResourceManager &resMngr, VkFormat format, EImageUsage usage, EImageOpt options, bool optTiling) __NE___
	{
		if ( AllBits( options, EImageOpt::ExtendedUsage ))
			return true;

		const auto&		dev	= resMngr.GetDevice();
		const auto&		fs	= resMngr.GetFeatureSet();

		VkFormatProperties	fmt_props = {};
		VulkanInstanceFn::vkGetPhysicalDeviceFormatProperties( dev.GetVkPhysicalDevice(), format, OUT &fmt_props );
		// TODO: VK_KHR_format_feature_flags2

		VkFormatFeatureFlags		required	= 0;
		const VkFormatFeatureFlags	available	= (optTiling ? fmt_props.optimalTilingFeatures : fmt_props.linearTilingFeatures) |
												  (dev.GetVExtensions().maintenance1 ? 0 : VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);

		for (auto t : BitfieldIterate( usage ))
		{
			switch_enum( t )
			{
				case EImageUsage::TransferSrc :
					required |= VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;

					if ( AllBits( options, EImageOpt::BlitSrc ))
						required |= VK_FORMAT_FEATURE_BLIT_SRC_BIT;
					break;

				case EImageUsage::TransferDst :
					required |= VK_FORMAT_FEATURE_TRANSFER_DST_BIT;

					if ( AllBits( options, EImageOpt::BlitDst ))
						required |= VK_FORMAT_FEATURE_BLIT_DST_BIT;
					break;

				case EImageUsage::DepthStencilAttachment :
					required |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
					break;

				case EImageUsage::ColorAttachment :
					required |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;

					if ( AllBits( options, EImageOpt::ColorAttachmentBlend ))
						required |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
					break;

				case EImageUsage::Storage :
					required |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;

					if ( AllBits( options, EImageOpt::StorageAtomic ))
						required |= VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
					break;

				case EImageUsage::Sampled :
					required |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;

					if ( AllBits( options, EImageOpt::SampledLinear ))
						required |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

					if ( AllBits( options, EImageOpt::SampledMinMax ))
						required |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
					break;

				case EImageUsage::ShadingRate :
					if ( fs.attachmentFragmentShadingRate != FeatureSet::EFeature::RequireTrue )
						return false;

					required |= VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
					break;
					
				case EImageUsage::FragmentDensityMap :
					if ( fs.fragmentDensityMap != FeatureSet::EFeature::RequireTrue )
						return false;

					required |= VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
					break;

				case EImageUsage::InputAttachment :		break;

				case EImageUsage::_Last :
				case EImageUsage::All :
				case EImageUsage::Transfer :
				case EImageUsage::Unknown :
				case EImageUsage::RWAttachment :
				default_unlikely :						DBG_WARNING( "unknown image usage" );	break;
			}
			switch_end
		}

		return AllBits( available, required );
	}
}
//-----------------------------------------------------------------------------



/*
=================================================
	destructor
=================================================
*/
	VImage::~VImage () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( _image == Default );
		ASSERT( _memoryId == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VImage::Create (ResourceManager &resMngr, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _image == Default );
		CHECK_ERR( _memoryId == Default );
		CHECK_ERR( All( desc.dimension > ImageDim_t{0} ));
		CHECK_ERR( desc.imageDim != Default );
		CHECK_ERR( desc.arrayLayers > 0_layer );
		CHECK_ERR( desc.mipLevels > 0_mipmap );
		CHECK_ERR( desc.format != Default );
		CHECK_ERR( desc.usage != Default );
		CHECK_ERR( EPixelFormat_PlaneCount( desc.format ) == 0 );	// use VideoImage instead

		_desc = desc;
		_desc.Validate();
		GRES_CHECK( IsSupported( resMngr, _desc ));

		const bool	opt_tiling	= AnyBits( _desc.memType, EMemoryType::DeviceLocal );
		auto&		dev			= resMngr.GetDevice();


		// create image
		VkImageCreateInfo	info;
		VNextChain			p_next	{info};
		info.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.flags			= VEnumCast( _desc.options );
		info.imageType		= VEnumCast( _desc.imageDim );
		info.format			= VEnumCast( _desc.format );
		info.extent.width	= _desc.dimension.x;
		info.extent.height	= _desc.dimension.y;
		info.extent.depth	= _desc.dimension.z;
		info.mipLevels		= _desc.mipLevels.Get();
		info.arrayLayers	= _desc.arrayLayers.Get();
		info.samples		= VEnumCast( _desc.samples );
		info.tiling			= (opt_tiling ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR);
		info.usage			= VEnumCast( _desc.usage, _desc.memType );
		info.initialLayout	= (opt_tiling ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PREINITIALIZED);
		info.queueFamilyIndexCount = 0;

		VQueueFamilyIndices_t	queue_family_indices;

		// setup sharing mode
		if ( _desc.queues != Default )
		{
			dev.GetQueueFamilies( _desc.queues, OUT queue_family_indices );

			info.sharingMode			= VK_SHARING_MODE_CONCURRENT;
			info.pQueueFamilyIndices	= queue_family_indices.data();
			info.queueFamilyIndexCount	= uint(queue_family_indices.size());
		}

		// reset to exclusive mode
		if ( info.queueFamilyIndexCount <= 1 )
		{
			info.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
			info.pQueueFamilyIndices	= null;
			info.queueFamilyIndexCount	= 0;
		}

		// VK_KHR_image_format_list
		FixedArray< VkFormat, ImageDesc::FormatList_t{}.max_size() >	fmt_list;
		for (auto fmt : _desc.viewFormats) {
			if ( fmt != Default )
				fmt_list.push_back( VEnumCast( fmt ));
		}
		VkImageFormatListCreateInfo		fmt_list_info = {};
		if ( not fmt_list.empty() )
		{
			fmt_list_info.sType				= VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO;
			fmt_list_info.viewFormatCount	= uint(fmt_list.size());
			fmt_list_info.pViewFormats		= fmt_list.data();

			if ( dev.GetVExtensions().imageFormatList )
			{
				p_next.Add( fmt_list_info );
			}
			else
				AE_LOG_DBG( "'VK_KHR_image_format_list' extension is not supported" );
		}

		// VK_EXT_image_compression_control
		VkImageCompressionControlEXT	compress_info = {};
		if ( dev.GetVExtensions().imageCompressionCtrl and AllBits( _desc.options, EImageOpt::LossyRTCompression ))
		{
			compress_info.sType		= VK_STRUCTURE_TYPE_IMAGE_COMPRESSION_CONTROL_EXT;
			compress_info.flags		= VK_IMAGE_COMPRESSION_FIXED_RATE_DEFAULT_EXT;	// lossy compression
										// VK_IMAGE_COMPRESSION_DEFAULT_EXT - lossless compression
			p_next.Add( compress_info );
		}

		// TODO: VkImageStencilUsageCreateInfo  (VK_EXT_separate_stencil_usage)

		VK_CHECK_ERR( dev.vkCreateImage( dev.GetVkDevice(), &info, null, OUT &_image ));

		_memoryId = resMngr.CreateMemoryObj( _image, _desc, RVRef(allocator), dbgName );
		CHECK_ERR( _memoryId );

		dev.SetObjectName( _image, dbgName, VK_OBJECT_TYPE_IMAGE );

		_aspectMask = ChooseAspect( _desc.format );

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  VImage::Create (ResourceManager &resMngr, const VulkanImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _image == Default );
		CHECK_ERR( _memoryId == Default );
		CHECK_ERR( desc.format != Zero );
		CHECK_ERR( desc.usage != Zero );
		ASSERT( desc.memFlags != Zero );

		_image				= desc.image;
		_desc.imageDim		= AEEnumCast( desc.imageType );
		_desc.options		= AEEnumCast( desc.flags ) | desc.options;
		_desc.dimension		= CheckCast<ImageDim_t>( desc.dimension );
		_desc.format		= AEEnumCast( desc.format );
		_desc.arrayLayers	= ImageLayer{ desc.arrayLayers };
		_desc.mipLevels		= MipmapLevel{ desc.mipLevels };
		_desc.samples		= AEEnumCast( desc.samples );
		_desc.memType		= AEEnumCast( desc.memFlags, not desc.canBeDestroyed );
		_desc.queues		= desc.queues;

		EMemoryType	req_mem;
		AEEnumCast( desc.usage, OUT _desc.usage, OUT req_mem );

		switch ( desc.tiling ) {
			case VK_IMAGE_TILING_OPTIMAL :	req_mem |= EMemoryType::DeviceLocal;	break;
			case VK_IMAGE_TILING_LINEAR :	req_mem |= EMemoryType::HostCoherent;	break;
		}
		GRES_CHECK( AllBits( _desc.memType, req_mem ));	// specified memory type is not valid
		GRES_CHECK( IsSupported( resMngr, _desc ));

		if ( desc.allocMemory )
		{
			CHECK_ERR( allocator );
			_memoryId = resMngr.CreateMemoryObj( _image, _desc, RVRef(allocator), dbgName );
			CHECK_ERR( _memoryId );
		}

		const auto&		dev = resMngr.GetDevice();
		dev.SetObjectName( _image, dbgName, VK_OBJECT_TYPE_IMAGE );

		_aspectMask = desc.aspectMask;
		if ( _aspectMask == Zero )
			_aspectMask = ChooseAspect( _desc.format );

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
----
	Immediately destroy image and free memory,
	is it safe because 'ResourceManager' will delay destruction by N frames.
	For more info see 'ResourceManager::ReleaseExpiredResourcesTask'.
=================================================
*/
	void  VImage::Destroy (ResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		const bool	is_internal = NoBits( _desc.memType, EMemoryType::_External );
		auto&		dev			= resMngr.GetDevice();

		if ( is_internal and _image != Default )
			dev.vkDestroyImage( dev.GetVkDevice(), _image, null );

		resMngr.ImmediatelyRelease( INOUT _memoryId );

		_memoryId		= Default;
		_image			= Default;
		_desc			= Default;
		_aspectMask		= Zero;

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	GetNativeDescription
=================================================
*/
	VulkanImageDesc  VImage::GetNativeDescription () C_NE___
	{
		VulkanImageDesc		desc;
		desc.image			= _image;
		desc.imageType		= VEnumCast( _desc.imageDim );
		desc.flags			= VEnumCast( _desc.options );
		desc.options		= _desc.options;
		desc.usage			= VEnumCast( _desc.usage, _desc.memType );
		desc.format			= VEnumCast( _desc.format );
		desc.samples		= VEnumCast( _desc.samples );
		desc.dimension		= _desc.Dimension();
		desc.arrayLayers	= _desc.arrayLayers.Get();
		desc.mipLevels		= _desc.mipLevels.Get();
		desc.tiling			= AllBits( _desc.memType, EMemoryType::DeviceLocal ) ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
		desc.queues			= _desc.queues;
		desc.memFlags		= VEnumCast( _desc.memType );
		desc.canBeDestroyed	= NoBits( _desc.memType, EMemoryType::_External );
		return desc;
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  VImage::IsSupported (const ResourceManager &resMngr, const ImageDesc &desc) __NE___
	{
		const auto&		dev			= resMngr.GetDevice();
		const auto&		dev_props	= dev.GetVProperties();
		const auto&		ext			= dev.GetVExtensions();
		const bool		opt_tiling	= AllBits( desc.memType, EMemoryType::DeviceLocal );
		const VkFormat	format		= VEnumCast( desc.format );

		if_unlikely( not Image_IsSupported( resMngr, desc, Bool{ext.imageFormatList} ))
			return false;

		// check format features
		if_unlikely( not CheckFormatFeatures( resMngr, format, desc.usage, desc.options, opt_tiling ))
			return false;

		// validate options
		{
			if ( AllBits( desc.options, EImageOpt::SparseResidency ))
			{
				switch ( desc.imageDim )
				{
					case EImageDim_1D :
						return false;

					case EImageDim_2D :
						if_unlikely( not dev_props.features.sparseResidencyImage2D )
							return false;

						switch ( desc.samples.Get() )
						{
							case 1 :	break;
							case 2 :	if_unlikely( not dev_props.features.sparseResidency2Samples )  return false;	break;
							case 4 :	if_unlikely( not dev_props.features.sparseResidency4Samples )  return false;	break;
							case 8 :	if_unlikely( not dev_props.features.sparseResidency8Samples )  return false;	break;
							case 16 :	if_unlikely( not dev_props.features.sparseResidency16Samples ) return false;	break;
							default_unlikely :	DBG_WARNING( "unsupported sample count" );	return false;
						}
						break;

					case EImageDim_3D :
						if_unlikely( not dev_props.features.sparseResidencyImage3D )
							return false;
						break;

					case EImageDim::Unknown :
					default_unlikely :	DBG_WARNING( "unknown image dimension" );	return false;
				}
			}
			/*
				case EImageOpt::LossyRTCompression :		// checked in Image_IsSupported()
				case EImageOpt::SparseAliased :				// checked in Image_IsSupported()
				case EImageOpt::BlockTexelViewCompatible :	// checked in Image_IsSupported()
				case EImageOpt::BlitSrc :					// checked in CheckFormatFeatures()
				case EImageOpt::BlitDst :					// checked in CheckFormatFeatures()
				case EImageOpt::ExtendedUsage :				// checked in Image_IsSupported()
				case EImageOpt::StorageAtomic :				// checked in FeatureSet
				case EImageOpt::VertexPplnStore :			// checked in FeatureSet and Image_IsSupported()
				case EImageOpt::FragmentPplnStore :			// checked in FeatureSet and Image_IsSupported()
				case EImageOpt::SampledLinear :				// checked in FeatureSet
				case EImageOpt::SampledMinMax :				// checked in Image_IsSupported()
				case EImageOpt::ColorAttachmentBlend :		// checked in FeatureSet
				case EImageOpt::CubeCompatible :			// checked in Image_IsSupported()
				case EImageOpt::MutableFormat :				// checked in Image_IsSupported()
				case EImageOpt::Array2DCompatible :			// checked in Image_IsSupported()
				case EImageOpt::Alias :						// checked in Image_IsSupported()
				case EImageOpt::SampleLocationsCompatible :	// checked in Image_IsSupported()
				case EImageOpt::SparseResidencyAliased :	// checked in Image_IsSupported()
				case EImageOpt::Subsampled :				// checked in FeatureSet
			*/
		}

		// check image properties
		{
			VkImageFormatProperties2			props2			= {};
			VkImageFormatProperties&			props			= props2.imageFormatProperties;
			VkImageCompressionPropertiesEXT		compress_props	= {};
			VkPhysicalDeviceImageFormatInfo2	info			= {};
			VkImageCompressionControlEXT		compress_info	= {};
			VkResult							err;

			props2.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;

			info.sType	= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
			info.format	= format;
			info.type	= VEnumCast( desc.imageDim );
			info.tiling	= opt_tiling ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
			info.usage	= VEnumCast( desc.usage, desc.memType );
			info.flags	= VEnumCast( desc.options );

			compress_info.sType		= VK_STRUCTURE_TYPE_IMAGE_COMPRESSION_CONTROL_EXT;
			compress_props.sType	= VK_STRUCTURE_TYPE_IMAGE_COMPRESSION_PROPERTIES_EXT;

			if ( ext.deviceProps2 )
			{
				VNextChain	p_next_prop		{props2};
				VNextChain	p_next_info		{info};

				if ( AllBits( desc.options, EImageOpt::LossyRTCompression ))
				{
					ASSERT( ext.imageCompressionCtrl );

					// enable fixed-rate (lossy) compression, otherwise it will be lossless compression
					compress_info.flags	= VK_IMAGE_COMPRESSION_FIXED_RATE_DEFAULT_EXT;

					p_next_info.Add( compress_info );
					p_next_prop.Add( compress_props );
				}

				err = vkGetPhysicalDeviceImageFormatProperties2KHR( dev.GetVkPhysicalDevice(), &info, OUT &props2 );
			}else{
				err = vkGetPhysicalDeviceImageFormatProperties( dev.GetVkPhysicalDevice(), format, info.type, info.tiling, info.usage, info.flags, OUT &props );
			}

			if_unlikely( err == VK_ERROR_FORMAT_NOT_SUPPORTED )
				return false;
			VK_CHECK_ERR( err );

			if_unlikely( desc.dimension.x > props.maxExtent.width  or
			 			 desc.dimension.y > props.maxExtent.height or
						 desc.dimension.z > props.maxExtent.depth )
				return false;

			if_unlikely( desc.mipLevels.Get() > props.maxMipLevels )
				return false;

			if_unlikely( desc.arrayLayers.Get() > props.maxArrayLayers )
				return false;

			if_unlikely( NoBits( props.sampleCounts, desc.samples.Get() ))
				return false;

			// properties will report supported fixed compression rates or other flags indicating that lossy compression is not supported.
			// VK_IMAGE_COMPRESSION_FIXED_RATE_DEFAULT_EXT is not used here.
			if_unlikely( AllBits( desc.options, EImageOpt::LossyRTCompression ) and
						 compress_props.imageCompressionFlags != VK_IMAGE_COMPRESSION_FIXED_RATE_EXPLICIT_EXT )
				return false;
		}

		return true;
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  VImage::IsSupported (const ResourceManager &resMngr, const ImageDesc &desc, const ImageViewDesc &view) __NE___
	{
		if_unlikely( not ImageView_IsSupported( resMngr, desc, view ))
			return false;

		const auto&		dev	= resMngr.GetDevice();

		if ( view.extUsage != Default )
		{
			if_unlikely( not dev.GetVExtensions().maintenance2 )
				return false;

			if_unlikely( not CheckFormatFeatures( resMngr, VEnumCast( view.format ), view.extUsage, Default, true ))
				return false;
		}

		return true;
	}

/*
=================================================
	GetMemoryAlignment
=================================================
*/
	Bytes  VImage::GetMemoryAlignment (const VDevice &dev, const ImageDesc &desc) __NE___
	{
		Bytes	align	= 4_b;

		if ( EMemoryType_IsNonCoherent( desc.memType ))
			align = Max( align, dev.GetDeviceProperties().res.minNonCoherentAtomSize );

		return align;
	}

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
