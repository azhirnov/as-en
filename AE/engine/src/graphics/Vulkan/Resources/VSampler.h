// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/SamplerDesc.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// Vulkan Sampler immutable data
	//

	class VSampler final
	{
	// variables
	private:
		VkSampler					_sampler			= Default;
		VkSamplerYcbcrConversion	_ycbcrConversion	= Default;
		VkFormat					_ycbcrFormat		= VK_FORMAT_UNDEFINED;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		VSampler ()																			__NE___	{}
		~VSampler ()																		__NE___;

		ND_ bool  Create (const VResourceManager &, const SamplerDesc &,
						  const VkSamplerYcbcrConversionCreateInfo *, StringView dbgName)	__NE___;
			void  Destroy (VResourceManager &)												__NE___;

		ND_ VkSampler					Handle ()											C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _sampler; }
		ND_ VkSamplerYcbcrConversion	YcbcrConversion ()									C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _ycbcrConversion; }
		ND_ VkFormat					YcbcrFormat ()										C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _ycbcrFormat; }

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()										C_NE___	{ return Default; })

		ND_ static bool  IsSupported (const VDevice &dev,
									  const SamplerDesc &desc,
									  const SamplerYcbcrConversionDesc &)					__NE___;
		ND_ static bool  IsSupported (const SamplerDesc &desc,
									  const SamplerYcbcrConversionDesc &,
									  VkFormatFeatureFlags available)						__NE___;

		ND_ static bool  ConvertSampler (OUT VkSamplerYcbcrConversionCreateInfo &convCI,
										 const SamplerDesc &sampDesc,
										 const SamplerYcbcrConversionDesc &ycbcrDesc,
										 const VDevice &dev,
										 IAllocator &alloc)									__NE___;

	private:
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
