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
		MPipelineCache () {}
		~MPipelineCache ();

		ND_ bool  Create (const MResourceManager& resMngr, StringView dbgName);
		ND_ bool  Create (const MResourceManager& resMngr, StringView dbgName, const Path &);
			void  Destroy (MResourceManager &);

		ND_ bool  Serialize (const Path &) const;

		ND_ MetalBinaryArchive		Handle ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _pplnCache; }
		
		DEBUG_ONLY( ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
