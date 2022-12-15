// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics
{

	//
	// Metal Pipeline Cache
	//

	class MPipelineCache final
	{
	// variables
	private:
		MetalBinaryArchiveRC		_pplnCache;

		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)

	
	// methods
	public:
		MPipelineCache ()								__NE___	{}
		~MPipelineCache ()								__NE___;

		ND_ bool  Create (const MResourceManager& resMngr, StringView dbgName)					__NE___;
		ND_ bool  Create (const MResourceManager& resMngr, StringView dbgName, const Path &)	__NE___;
			void  Destroy (MResourceManager &)													__NE___;

		ND_ bool  Serialize (const Path &)				C_NE___;

		ND_ MetalBinaryArchive		Handle ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _pplnCache; }
		
		DEBUG_ONLY( ND_ StringView  GetDebugName ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
