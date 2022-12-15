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
		MetalAccelStructRC			_handle;
		RTGeometryDesc				_desc;
		Strong<MMemoryID>			_memoryId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MRTGeometry ()										__NE___	{}
		~MRTGeometry ()										__NE___;

		ND_ bool  Create (MResourceManager &, const RTGeometryDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___;
			void  Destroy (MResourceManager &)				__NE___;
		
		ND_ MetalAccelStruct		Handle ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ MMemoryID				MemoryID ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
		ND_ RTGeometryDesc const&	Description ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ bool					IsExclusiveSharing ()	C_NE___	{ return true; }
		
		DEBUG_ONLY( ND_ StringView	GetDebugName ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		ND_ static RTASBuildSizes	GetBuildSizes (const MResourceManager &, const RTGeometryBuild &desc) __NE___;
	};
	

} // AE::Graphics

#endif // AE_ENABLE_METAL
