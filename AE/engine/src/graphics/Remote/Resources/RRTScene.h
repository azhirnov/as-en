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
	// Remote Graphics Ray Tracing Scene
	//

	class RRTScene final
	{
	// variables
	private:
		RmRTSceneID						_sceneId;
		RmMemoryID						_memoryId;
		DeviceAddress					_address	= Default;
		RTSceneDesc						_desc;
		GfxMemAllocatorPtr				_allocator;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RRTScene ()																									__NE___	{}
		~RRTScene ()																								__NE___;

		ND_ bool  Create (RResourceManager &, const RTSceneDesc &, GfxMemAllocatorPtr, StringView dbgName)			__NE___;
			void  Destroy (RResourceManager &)																		__NE___;

		ND_ RmRTSceneID				Handle ()																		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _sceneId; }
		ND_ DeviceAddress			GetDeviceAddress ()																C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _address; }
		ND_ RTSceneDesc const&		Description ()																	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ RmMemoryID				MemoryId ()																		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
		ND_ bool					IsExclusiveSharing ()															C_NE___	{ return false; }

		GFX_DBG_ONLY( ND_ StringView GetDebugName ()																C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static bool				IsSerializedMemoryCompatible (const RDevice &dev, const void* ptr, Bytes size)	__NE___;

		ND_ static RTASBuildSizes	GetBuildSizes (const RResourceManager &, const RTSceneBuild &desc)				__NE___;

		ND_ static bool				IsSupported (const RResourceManager &, const RTSceneDesc &desc)					__NE___;
		ND_ static bool				IsSupported (const RResourceManager &, const RTSceneBuild &build)				__NE___;

		ND_ static bool				ConvertBuildInfo (const RResourceManager &, INOUT RTSceneBuild &)				__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
