// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS

# include "graphics/Public/BufferDesc.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Remote/RQueue.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Ray Tracing Geometry
	//

	class RRTGeometry final
	{
	// variables
	private:
		RmRTGeometryID					_geomId;
		RmMemoryID						_memoryId;
		DeviceAddress					_address	= Default;
		RTGeometryDesc					_desc;
		GfxMemAllocatorPtr				_allocator;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RRTGeometry ()																								__NE___	{}
		~RRTGeometry ()																								__NE___;

		ND_ bool  Create (RResourceManager &, const RTGeometryDesc &, GfxMemAllocatorPtr, StringView dbgName)		__NE___;
			void  Destroy (RResourceManager &)																		__NE___;

		ND_ RmRTGeometryID			Handle ()																		C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _geomId; }
		ND_ DeviceAddress			GetDeviceAddress ()																C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _address; }
		ND_ RTGeometryDesc const&	Description ()																	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ RmMemoryID				MemoryId ()																		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
		ND_ bool					IsExclusiveSharing ()															C_NE___	{ return false; }

		GFX_DBG_ONLY( ND_ StringView GetDebugName ()																C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		ND_ static bool				IsSerializedMemoryCompatible (const RDevice &dev, const void* ptr, Bytes size)	__NE___;

		ND_ static RTASBuildSizes	GetBuildSizes (const RResourceManager &, const RTGeometryBuild &desc)			__NE___;

		ND_ static bool				IsSupported (const RResourceManager &, const RTGeometryDesc &desc)				__NE___;
		ND_ static bool				IsSupported (const RResourceManager &, const RTGeometryBuild &build)			__NE___;

		ND_ static bool				ConvertBuildInfo (const RResourceManager &, INOUT RTGeometryBuild &,
													  RTempLinearAllocator &)										__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
