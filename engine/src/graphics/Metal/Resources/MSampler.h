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
		MSampler () {}
		~MSampler ();

		ND_ bool  Create (const MDevice &dev, const SamplerDesc &desc, StringView dbgName);
			void  Destroy (MResourceManager &);

		ND_ MetalSampler	Handle ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _sampler; }
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
