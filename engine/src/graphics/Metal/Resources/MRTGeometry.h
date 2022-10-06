// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Public/BufferDesc.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics
{

	//
	// Metal Ray Tracing Geometry
	//

	class MRTGeometry final
	{
	// variables
	private:
		RTGeometryDesc				_desc;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MRTGeometry () {}
		~MRTGeometry ();

		ND_ bool  Create (MResourceManager &, const RTGeometryDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
			void  Destroy (MResourceManager &);
		
		ND_ RTGeometryDesc const&		Description ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ bool						IsExclusiveSharing ()	const	{ return true; }
		
		DEBUG_ONLY(  ND_ StringView		GetDebugName ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		ND_ static RTASBuildSizes	GetBuildSizes (const MResourceManager &, const RTGeometryBuild &desc);
	};
	

}	// AE::Graphics

#endif	// AE_ENABLE_METAL
