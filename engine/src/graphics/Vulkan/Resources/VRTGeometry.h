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
	// Vulkan Ray Tracing Geometry
	//

	class VRTGeometry final
	{
	// variables
	private:
		VDeviceAddress				_address		= Default;
		VkAccelerationStructureKHR	_accelStruct	= Default;
		VkBuffer					_buffer			= Default;
		RTGeometryDesc				_desc;
		
		Strong<VMemoryID>			_memoryId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VRTGeometry () {}
		~VRTGeometry ();

		ND_ bool  Create (VResourceManager &, const RTGeometryDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
			void  Destroy (VResourceManager &);
		
		ND_ VkAccelerationStructureKHR	Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _accelStruct; }
		ND_ VDeviceAddress				GetDeviceAddress ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _address; }
		ND_ RTGeometryDesc const&		Description ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ VMemoryID					MemoryID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
		ND_ bool						IsExclusiveSharing ()	const	{ return true; }
		
		DEBUG_ONLY(  ND_ StringView		GetDebugName ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		ND_ static RTASBuildSizes	GetBuildSizes (const VResourceManager &, const RTGeometryBuild &desc);

		ND_ static bool				ConvertBuildInfo (const VResourceManager &, IAlignedAllocatorRef allocator, const RTGeometryBuild &desc,
													  OUT VkAccelerationStructureBuildRangeInfoKHR* &ranges,
													  OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo);

	private:
		template <bool IsForBuilding>
		ND_ static bool  _Convert (const VResourceManager &, const RTGeometryBuild &desc, IAlignedAllocatorRef allocator,
								   OUT uint* &primitiveCount, OUT VkAccelerationStructureBuildRangeInfoKHR* &ranges,
								   OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo);
	};
	

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
