// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/BufferDesc.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// Vulkan Ray Tracing Scene
	//

	class VRTScene final
	{
	// variables
	private:
		VkAccelerationStructureKHR	_accelStruct	= Default;
		VkBuffer					_buffer			= Default;
		RTSceneDesc					_desc;
		DeviceAddress				_address		= Default;

		Strong<MemoryID>			_memoryId;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VRTScene ()																									__NE___	{}
		~VRTScene ()																								__NE___;

		ND_ bool  Create (VResourceManager &, const RTSceneDesc &, GfxMemAllocatorPtr, StringView dbgName)			__NE___;
			void  Destroy (VResourceManager &)																		__NE___;

		ND_ VkAccelerationStructureKHR	Handle ()																	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _accelStruct; }
		ND_ DeviceAddress				GetDeviceAddress ()															C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _address; }
		ND_ RTSceneDesc const&			Description ()																C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ MemoryID					MemoryId ()																	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
		ND_ bool						IsExclusiveSharing ()														C_NE___	{ return false; }

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()																C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static bool				IsSerializedMemoryCompatible (const VDevice &dev, const void* ptr, Bytes size)	__NE___;

		ND_ static RTASBuildSizes	GetBuildSizes (const VResourceManager &, const RTSceneBuild &desc)				__NE___;

		ND_ static bool				ConvertBuildInfo (const VResourceManager &, const RTSceneBuild &desc,
													  OUT VkAccelerationStructureGeometryKHR &geom,
													  OUT VkAccelerationStructureBuildRangeInfoKHR &range,
													  OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo)	__NE___;

		ND_ static bool				IsSupported (const VResourceManager &, const RTSceneDesc &desc)					__NE___;
		ND_ static bool				IsSupported (const VResourceManager &, const RTSceneBuild &build)				__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
