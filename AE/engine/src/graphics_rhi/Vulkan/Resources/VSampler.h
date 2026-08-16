// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/SamplerDesc.h"
# include "graphics_rhi/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// Vulkan Sampler immutable data
	//

	class VSampler final : private VulkanInstanceFn
	{
	// variables
	private:
		VkSampler					_sampler			= Default;
		VkSamplerYcbcrConversion	_ycbcrConversion	= Default;
		VkFormat					_ycbcrFormat		= VK_FORMAT_UNDEFINED;
		VkSamplerCreateInfo *		_samplerCI			= null;


	// methods
	public:
		VSampler ()																			__NE___	{}
		~VSampler ()																		__NE___;

		ND_ bool  Create (const ResourceManager &, const SamplerDesc &,
						  const VkSamplerYcbcrConversionCreateInfo *,
						  IAllocator*, StringView dbgName)									__NE___;
			void  Destroy (ResourceManager &)												__NE___;

		ND_ VkSampler					Handle ()											C_NE___	{ return _sampler; }
		ND_ VkSamplerYcbcrConversion	YcbcrConversion ()									C_NE___	{ return _ycbcrConversion; }
		ND_ VkFormat					YcbcrFormat ()										C_NE___	{ return _ycbcrFormat; }
		ND_ VkSamplerCreateInfo const*	GetCreateInfo ()									C_NE___	{ return _samplerCI; }

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
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
