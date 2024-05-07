// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/SamplerDesc.h"
# include "graphics/Remote/RCommon.h"

namespace AE::Graphics
{

	//
	// Remote Sampler immutable data
	//

	class RSampler final
	{
	// variables
	private:
		RmSamplerID					_sampId;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		RSampler ()														__NE___	{}
		~RSampler ()													__NE___;

		ND_ bool  Create (const RResourceManager &, RmSamplerID id)		__NE___;
			void  Destroy (RResourceManager &)							__NE___;

		ND_ RmSamplerID					Handle ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _sampId; }
		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()					C_NE___	{ return Default; })
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
