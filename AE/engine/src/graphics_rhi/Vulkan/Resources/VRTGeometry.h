// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/BufferDesc.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Public/IDs.h"
# include "graphics_rhi/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// Vulkan Ray Tracing Geometry
	//

	class VRTGeometry final
	{
	// variables
	private:
		DeviceAddress				_address		= Default;
		VkAccelerationStructureKHR	_accelStruct	= Default;
		VkBuffer					_storage		= Default;
		Bytes						_offset;
		RTGeometryDesc				_desc;

		Strong<MemoryID>			_memoryId;

		GFX_DBG_ONLY( DebugName_t	_debugName;	)


	// methods
	public:
		VRTGeometry ()																										__NE___	{}
		~VRTGeometry ()																										__NE___;

		ND_ bool  Create (ResourceManager &, const RTGeometryDesc &, GfxMemAllocatorPtr, StringView dbgName)				__NE___;
			void  Destroy (ResourceManager &)																				__NE___;

		ND_ VkAccelerationStructureKHR	Handle ()																			C_NE___	{ return _accelStruct; }
		ND_ DeviceAddress				GetDeviceAddress ()																	C_NE___	{ return _address; }
		ND_ RTGeometryDesc const&		Description ()																		C_NE___	{ return _desc; }
		ND_ MemoryID					MemoryId ()																			C_NE___	{ return _memoryId; }
		ND_ bool						IsExclusiveSharing ()																C_NE___	{ return true; }
		ND_ BufferSubRange				GetBufferStorage ()																	C_NE___;

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()																		C_NE___	{ return _debugName; })

		ND_ static bool				IsSerializedMemoryCompatible (const VDevice &dev, const void* ptr, Bytes size)			__NE___;

		ND_ static RTASBuildSizes	GetBuildSizes (const ResourceManager &, const RTGeometryBuild &desc)					__NE___;

		ND_ static bool				ConvertBuildInfo (const ResourceManager &, IAllocatorRef allocator, const RTGeometryBuild &,
													  OUT VkAccelerationStructureBuildRangeInfoKHR* &ranges,
													  OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo)			__NE___;

		ND_ static bool				ConvertBuildInfo (const ResourceManager &, IAllocatorRef allocator,
													  const RTGeometryBuild &, OUT uint* &maxPrimitiveCounts,
													  OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo)			__NE___;

		ND_ static bool				IsSupported (const ResourceManager &, const RTGeometryDesc &desc)						__NE___;
		ND_ static bool				IsSupported (const ResourceManager &, const RTGeometryBuild &build)						__NE___;

	private:
		template <bool IsForBuilding>
		ND_ static bool  _Convert (const ResourceManager &, const RTGeometryBuild &desc, IAllocatorRef allocator,
								   OUT uint* &primitiveCount, OUT VkAccelerationStructureBuildRangeInfoKHR* &ranges,
								   OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo)								__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
