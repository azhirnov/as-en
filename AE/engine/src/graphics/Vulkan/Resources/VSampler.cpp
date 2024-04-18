// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "base/Platforms/AndroidApi26.h"
# include "graphics/Private/AndroidUtils.h"

# include "graphics/Vulkan/Resources/VSampler.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"


namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VSampler::~VSampler () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( _sampler == Default );
		CHECK( _ycbcrConversion == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VSampler::Create (const VResourceManager &resMngr, const SamplerDesc &desc, const VkSamplerYcbcrConversionCreateInfo* ycbcrDesc, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _sampler == Default );
		CHECK_ERR( _ycbcrConversion	== Default );

		VkSamplerCreateInfo				sampler_ci;
		VkSamplerYcbcrConversionInfo	conv_info	= {};

		sampler_ci.sType			= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_ci.pNext			= null;
		sampler_ci.flags			= VEnumCast( desc.usage );
		sampler_ci.magFilter		= VEnumCast( desc.magFilter );
		sampler_ci.minFilter		= VEnumCast( desc.minFilter );
		sampler_ci.mipmapMode		= VEnumCast( desc.mipmapMode );
		sampler_ci.addressModeU		= VEnumCast( desc.addressMode.x );
		sampler_ci.addressModeV		= VEnumCast( desc.addressMode.y );
		sampler_ci.addressModeW		= VEnumCast( desc.addressMode.z );
		sampler_ci.mipLodBias		= desc.mipLodBias;
		sampler_ci.anisotropyEnable	= desc.maxAnisotropy.has_value() ? VK_TRUE : VK_FALSE;
		sampler_ci.maxAnisotropy	= desc.maxAnisotropy.value_or( 0.0f );
		sampler_ci.compareEnable	= desc.compareOp.has_value() ? VK_TRUE : VK_FALSE;
		sampler_ci.compareOp		= VEnumCast( desc.compareOp.value_or( ECompareOp::Always ));
		sampler_ci.minLod			= desc.minLod;
		sampler_ci.maxLod			= desc.maxLod;
		sampler_ci.borderColor		= VEnumCast( desc.borderColor );
		sampler_ci.unnormalizedCoordinates= desc.unnormalizedCoordinates ? VK_TRUE : VK_FALSE;

		auto&	dev = resMngr.GetDevice();
		//GRES_CHECK( IsSupported( dev, sampler_ci ));

		if ( ycbcrDesc != null )
		{
			VK_CHECK_ERR( dev.vkCreateSamplerYcbcrConversionKHR( dev.GetVkDevice(), ycbcrDesc, null, OUT &_ycbcrConversion ));
			dev.SetObjectName( _ycbcrConversion, dbgName, VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION );

			sampler_ci.pNext		= &conv_info;

			conv_info.sType			= VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
			conv_info.conversion	= _ycbcrConversion;

			_ycbcrFormat			= ycbcrDesc->format;
		}

		VK_CHECK_ERR( dev.vkCreateSampler( dev.GetVkDevice(), &sampler_ci, null, OUT &_sampler ));
		dev.SetObjectName( _sampler, dbgName, VK_OBJECT_TYPE_SAMPLER );

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VSampler::Destroy (VResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		auto&	dev = resMngr.GetDevice();

		if ( _ycbcrConversion != Default )
			dev.vkDestroySamplerYcbcrConversionKHR( dev.GetVkDevice(), _ycbcrConversion, null );

		if ( _sampler != Default )
			dev.vkDestroySampler( dev.GetVkDevice(), _sampler, null );

		_sampler			= Default;
		_ycbcrConversion	= Default;
		_ycbcrFormat		= VK_FORMAT_UNDEFINED;
	}

/*
=================================================
	IsSupported (SamplerYcbcrConversionDesc)
=================================================
*/
	bool  VSampler::IsSupported (const VDevice &dev, const SamplerDesc &desc, const SamplerYcbcrConversionDesc &ycbcrDesc) __NE___
	{
		const VkFormat	fmt = VEnumCast( ycbcrDesc.format );
		ASSERT( fmt != VK_FORMAT_UNDEFINED );

		VkFormatProperties	props = {};
		vkGetPhysicalDeviceFormatProperties( dev.GetVkPhysicalDevice(), fmt, OUT &props );

		return IsSupported( desc, ycbcrDesc, props.optimalTilingFeatures );
	}

	bool  VSampler::IsSupported (const SamplerDesc &desc,
								 const SamplerYcbcrConversionDesc &ycbcrDesc,
								 const VkFormatFeatureFlags available) __NE___
	{
		VkFormatFeatureFlags	required = 0;

		if ( ycbcrDesc.chromaFilter != EFilter::Nearest )
			required |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;

		if ( ycbcrDesc.xChromaOffset == ESamplerChromaLocation::Midpoint or
			 ycbcrDesc.yChromaOffset == ESamplerChromaLocation::Midpoint )
			required |= VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;

		if ( ycbcrDesc.xChromaOffset == ESamplerChromaLocation::CositedEven or
			 ycbcrDesc.yChromaOffset == ESamplerChromaLocation::CositedEven )
			required |= VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;

		if ( ycbcrDesc.chromaFilter != desc.minFilter or
			 ycbcrDesc.chromaFilter != desc.magFilter )
			required |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;

		if ( ycbcrDesc.forceExplicitReconstruction )
			required |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
						VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;

		return AllBits( available, required );
	}

/*
=================================================
	ConvertSampler
=================================================
*/
	bool  VSampler::ConvertSampler (OUT VkSamplerYcbcrConversionCreateInfo &convCI,
									const SamplerDesc &sampDesc,
									const SamplerYcbcrConversionDesc &desc,
									const VDevice &dev,
									IAllocator &allocator) __NE___
	{
		convCI = {};
		convCI.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;

	  #ifdef AE_PLATFORM_ANDROID
		if ( desc.extFormat != Default )
		{
			CHECK_ERR( desc.format == Default );
			CHECK_ERR( dev.GetVExtensions().androidExternalMemoryHwBuf );

			auto&					api26		= AndroidApi26::Instance();
			AHardwareBuffer_Desc	hwbuf_desc	= {};
			AHardwareBuffer*		hwbuf		= null;

			hwbuf_desc.width	= 2;
			hwbuf_desc.height	= 2;
			hwbuf_desc.layers	= 1;
			hwbuf_desc.format	= int(AndroidFormatCast( desc.extFormat ));
			hwbuf_desc.usage	= AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE;

			if ( api26.hwbuf.allocate( &hwbuf_desc, OUT &hwbuf ) != 0 )
				return false;	// not supported

			VkAndroidHardwareBufferFormatPropertiesANDROID	format_info	= {};
			VkAndroidHardwareBufferPropertiesANDROID		props_info	= {};

			format_info.sType	= VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_FORMAT_PROPERTIES_ANDROID;
			props_info.sType	= VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_PROPERTIES_ANDROID;
			props_info.pNext	= &format_info;

			bool ok = (dev.vkGetAndroidHardwareBufferPropertiesANDROID( dev.GetVkDevice(), hwbuf, OUT &props_info ) == VK_SUCCESS);
			api26.hwbuf.release( hwbuf );
			if ( not ok )
				return false;

			if ( format_info.format == VK_FORMAT_UNDEFINED )
			{
				auto&	ext_format = *allocator.Allocate< VkExternalFormatANDROID >();
				ext_format					= {};
				ext_format.sType			= VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_ANDROID;
				ext_format.externalFormat	= format_info.externalFormat;

				convCI.pNext	= &ext_format;
				convCI.format	= VK_FORMAT_UNDEFINED;
			}else{
				convCI.format	= format_info.format;
			}

			if ( not IsSupported( sampDesc, desc, format_info.formatFeatures ))
				return false;

			convCI.chromaFilter	= VEnumCast( desc.chromaFilter );
			convCI.forceExplicitReconstruction = desc.forceExplicitReconstruction ? VK_TRUE : VK_FALSE;

			convCI.ycbcrModel		= (desc.ycbcrModel == Default	 ? format_info.suggestedYcbcrModel : VEnumCast( desc.ycbcrModel ));
			convCI.ycbcrRange		= (desc.ycbcrRange == Default	 ? format_info.suggestedYcbcrRange : VEnumCast( desc.ycbcrRange ));
			convCI.components		= (desc.components.IsUndefined() ? format_info.samplerYcbcrConversionComponents : VEnumCast( desc.components ));
			convCI.xChromaOffset	= (desc.xChromaOffset == Default ? format_info.suggestedXChromaOffset : VEnumCast( desc.xChromaOffset ));
			convCI.yChromaOffset	= (desc.yChromaOffset == Default ? format_info.suggestedYChromaOffset : VEnumCast( desc.yChromaOffset ));

			return true;
		}
	  #else
		Unused( dev, allocator );
	  #endif

		if ( not IsSupported( dev, sampDesc, desc ))
			return false;

		convCI.format			= VEnumCast( desc.format );
		convCI.ycbcrModel		= VEnumCast( desc.ycbcrModel );
		convCI.ycbcrRange		= VEnumCast( desc.ycbcrRange );
		convCI.components		= VEnumCast( desc.components );
		convCI.xChromaOffset	= VEnumCast( desc.xChromaOffset );
		convCI.yChromaOffset	= VEnumCast( desc.yChromaOffset );
		convCI.chromaFilter		= VEnumCast( desc.chromaFilter );
		convCI.forceExplicitReconstruction = desc.forceExplicitReconstruction ? VK_TRUE : VK_FALSE;

		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
