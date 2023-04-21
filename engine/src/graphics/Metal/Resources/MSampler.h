// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/SamplerDesc.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics
{

	//
	// Metal Sampler immutable data
	//

	class MSampler
	{
	// variables
	private:
		MetalSamplerRC				_sampler;
		
		DRC_ONLY( RWDataRaceCheck	_drCheck; )


	// methods
	public:
		MSampler ()																			__NE___	{}
		~MSampler ()																		__NE___;

		ND_ bool  Create (const MDevice &dev, const SamplerDesc &desc, StringView dbgName)	__NE___;
			void  Destroy (MResourceManager &)												__NE___;

		ND_ MetalSampler	Handle ()														C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _sampler; }
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
