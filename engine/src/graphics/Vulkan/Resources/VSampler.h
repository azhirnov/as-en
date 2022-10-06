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
		VkSampler					_sampler	= Default;
		
		DRC_ONLY( RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VSampler () {}
		~VSampler ();

		ND_ bool  Create (const VDevice &dev, const VkSamplerCreateInfo &info, StringView dbgName);
			void  Destroy (VResourceManager &);

		ND_ VkSampler	Handle ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _sampler; }
		
		static void  ConvertSampler (const SamplerDesc &desc, OUT VkSamplerCreateInfo &info);
	};
	

}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
