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
		VDeviceAddress				_address		= Default;
		
		Strong<VMemoryID>			_memoryId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VRTScene () {}
		~VRTScene ();

		ND_ bool  Create (VResourceManager &, const RTSceneDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
			void  Destroy (VResourceManager &);
		
		ND_ VkAccelerationStructureKHR	Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _accelStruct; }
		ND_ VDeviceAddress				GetDeviceAddress ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _address; }
		ND_ RTSceneDesc const&			Description ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ VMemoryID					MemoryID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
		ND_ bool						IsExclusiveSharing ()	const	{ return true; }
		
		DEBUG_ONLY(  ND_ StringView		GetDebugName ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
			
		ND_ static RTASBuildSizes	GetBuildSizes (const VResourceManager &, const RTSceneBuild &desc);

			static void				ConvertBuildInfo (const VResourceManager &, const RTSceneBuild &desc,
													  OUT VkAccelerationStructureGeometryKHR &geom,
													  OUT VkAccelerationStructureBuildRangeInfoKHR &range,
													  OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo);

	private:
		//ND_ static bool  _Convert (const VResourceManager &, const RTSceneBuild &desc, VTempLinearAllocator &allocator,
		//						   OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo);
	};
	

}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
