// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/RCommon.h"

namespace AE::Graphics
{

	//
	// Remote Pipeline Cache
	//

	class RPipelineCache final
	{
	// variables
	private:
		RmPipelineCacheID			_cacheId;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RPipelineCache ()																	__NE___	{}
		~RPipelineCache ()																	__NE___;

		ND_ bool  Create (const RResourceManager&, StringView dbgName)						__NE___;
		ND_ bool  Create (const RResourceManager&, StringView dbgName, RC<RStream> stream)	__NE___;
			void  Destroy (const RResourceManager &)										__NE___;

		ND_ bool  GetData (const RDevice &, OUT Array<char> &data)							C_NE___;

		ND_ RmPipelineCacheID	Handle ()													C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _cacheId; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()										C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
