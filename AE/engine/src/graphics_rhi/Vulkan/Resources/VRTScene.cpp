// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Vulkan/Resources/VRTScene.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "graphics_rhi/Private/EnumUtils.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VRTScene::~VRTScene () __NE___
	{
		ASSERT( _accelStruct == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VRTScene::Create (ResourceManager &resMngr, const RTSceneDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		CHECK_ERR( _accelStruct == Default );
		CHECK_ERR( desc.size > 0 );

		_desc = desc;
		GRES_CHECK( IsSupported( resMngr, desc ));

		// create buffer
		auto&	dev = resMngr.GetDevice();
		_memoryId	= resMngr.CreateMemoryObj( _desc.size, VK_BUFFER_USAGE_2_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, RVRef(allocator), dbgName );
		CHECK_ERR( _memoryId );

		auto*	mem_obj = resMngr.GetResource( _memoryId );
		CHECK_ERR( mem_obj != null );

		VulkanMemoryObjInfo	mem_info;
		CHECK_ERR( mem_obj->GetMemoryInfo( OUT mem_info ));
		CHECK_ERR( mem_info.buffer != Default );
		CHECK_ERR( IsMultipleOf( mem_info.offset, 256_b ));  // from specs

		// create acceleration structure
		VkAccelerationStructureCreateInfoKHR	blas_ci = {};
		blas_ci.sType		= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		blas_ci.createFlags	= 0;	// TODO: VK_ACCELERATION_STRUCTURE_CREATE_MOTION_BIT_NV
		blas_ci.buffer		= mem_info.buffer;
		blas_ci.offset		= VkDeviceSize( mem_info.offset );
		blas_ci.size		= VkDeviceSize( _desc.size );
		blas_ci.type		= VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;

		VK_CHECK_ERR( dev.vkCreateAccelerationStructureKHR( dev.GetVkDevice(), &blas_ci, null, OUT &_accelStruct ));

		dev.SetObjectName( _accelStruct, dbgName, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR );

		VkAccelerationStructureDeviceAddressInfoKHR	addr_info = {};
		addr_info.sType					= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		addr_info.accelerationStructure	= _accelStruct;

		_address = BitCast<DeviceAddress>( dev.vkGetAccelerationStructureDeviceAddressKHR( dev.GetVkDevice(), &addr_info ));
		CHECK_ERR( _address != Default );

		_storage = mem_info.buffer;
		_offset	 = mem_info.offset;

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VRTScene::Destroy (ResourceManager &resMngr) __NE___
	{
		auto&	dev = resMngr.GetDevice();

		if ( _accelStruct != Default )
			dev.vkDestroyAccelerationStructureKHR( dev.GetVkDevice(), _accelStruct, null );

		resMngr.ImmediatelyRelease( INOUT _memoryId );

		_address		= Default;
		_memoryId		= Default;
		_accelStruct	= Default;
		_desc			= Default;
		_storage		= Default;
		_offset			= 0_b;

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	GetBuildSizes
=================================================
*/
	RTASBuildSizes  VRTScene::GetBuildSizes (const ResourceManager &resMngr, const RTSceneBuild &desc) __NE___
	{
		GRES_CHECK( IsSupported( resMngr, desc ));

		VkAccelerationStructureGeometryKHR					geom		= {};
		VkAccelerationStructureGeometryInstancesDataKHR&	instances	= geom.geometry.instances;
		VkAccelerationStructureBuildGeometryInfoKHR			build_info	= {};
		VkAccelerationStructureBuildSizesInfoKHR			size_info	= {};

		geom.sType					= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geom.geometryType			= VK_GEOMETRY_TYPE_INSTANCES_KHR;

		instances.sType				= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		instances.arrayOfPointers	= VK_FALSE;	// TODO ???

		build_info.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		build_info.flags			= VEnumCast( desc.options );
		build_info.type				= VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		build_info.pGeometries		= &geom;
		build_info.geometryCount	= 1;

		size_info.sType				= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

		const uint	prim_count		= desc.maxInstanceCount;

		// from specs:
		//	The 'srcAccelerationStructure', 'dstAccelerationStructure', 'mode', 'scratchData' members of pBuildInfo are ignored.

		auto&	dev = resMngr.GetDevice();
		dev.vkGetAccelerationStructureBuildSizesKHR( dev.GetVkDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &build_info, &prim_count, OUT &size_info );

		RTASBuildSizes	res;
		res.rtasSize			= Bytes{size_info.accelerationStructureSize};
		res.buildScratchSize	= Bytes{size_info.buildScratchSize};
		res.updateScratchSize	= Bytes{size_info.updateScratchSize};
		return res;
	}

/*
=================================================
	ConvertBuildInfo
=================================================
*/
	bool  VRTScene::ConvertBuildInfo (const ResourceManager &resMngr, const RTSceneBuild &desc,
									  OUT VkAccelerationStructureGeometryKHR &outGeometry,
									  OUT VkAccelerationStructureBuildRangeInfoKHR &outRange,
									  OUT VkAccelerationStructureBuildGeometryInfoKHR &outBuildInfo) __NE___
	{
		GRES_CHECK( IsSupported( resMngr, desc ));
		Unused( resMngr );

		VkAccelerationStructureGeometryKHR&					geom		= outGeometry;
		VkAccelerationStructureGeometryInstancesDataKHR&	instances	= geom.geometry.instances;
		VkAccelerationStructureBuildGeometryInfoKHR&		build_info	= outBuildInfo;

		geom						= {};
		geom.sType					= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geom.geometryType			= VK_GEOMETRY_TYPE_INSTANCES_KHR;

		instances.sType				= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		instances.arrayOfPointers	= VK_FALSE;

		build_info					= {};
		build_info.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		build_info.flags			= VEnumCast( desc.options );
		build_info.type				= VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		build_info.pGeometries		= &geom;
		build_info.geometryCount	= 1;

		outRange.primitiveCount		= desc.maxInstanceCount;
		outRange.primitiveOffset	= 0;
		outRange.firstVertex		= 0;
		outRange.transformOffset	= 0;

		return true;
	}

/*
=================================================
	IsSerializedMemoryCompatible
=================================================
*/
	bool  VRTScene::IsSerializedMemoryCompatible (const VDevice &dev, const void* ptr, Bytes size) __NE___
	{
		CHECK_ERR( size >= sizeof(VK_UUID_SIZE)*2 );

		VkAccelerationStructureVersionInfoKHR	info;
		info.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_VERSION_INFO_KHR;
		info.pNext			= null;
		info.pVersionData	= Cast<ubyte>(ptr);

		VkAccelerationStructureCompatibilityKHR	compat = VK_ACCELERATION_STRUCTURE_COMPATIBILITY_INCOMPATIBLE_KHR;

		dev.vkGetDeviceAccelerationStructureCompatibilityKHR( dev.GetVkDevice(), &info, OUT &compat );

		return compat == VK_ACCELERATION_STRUCTURE_COMPATIBILITY_COMPATIBLE_KHR;
	}

/*
=================================================
	IsSupported (RTSceneDesc)
=================================================
*/
	bool  VRTScene::IsSupported (const ResourceManager &resMngr, const RTSceneDesc &desc) __NE___
	{
		return RTScene_IsSupported( resMngr, desc );
	}

/*
=================================================
	IsSupported (RTSceneBuild)
=================================================
*/
	bool  VRTScene::IsSupported (const ResourceManager &resMngr, const RTSceneBuild &build) __NE___
	{
		auto&	dev		= resMngr.GetDevice();
		auto&	props	= dev.GetDeviceProperties().rayTracing;

		if_unlikely( build.instanceData.id != Default										and
					 build.instanceData.stride != sizeof(VkAccelerationStructureInstanceKHR) )
			return false;	// custom stride is not supported

		if_unlikely( build.maxInstanceCount > props.maxInstances )
			return false;

		if_unlikely( (build.instanceData.id != Default) != (build.scratch.id != Default) )
			return false;

		if ( build.instanceData.id != Default )
		{
			auto*	buf		= resMngr.GetResource( build.instanceData.id, False{"don't inc ref"}, True{"quiet"} );
			Bytes	size	= build.instanceData.offset + build.instanceData.stride * build.maxInstanceCount;

			if_unlikely( buf == null															or
						 build.instanceData.offset >= buf->Size()								or
						 size > buf->Size()														or
						 NoBits( buf->Description().usage, EBufferUsage::ASBuild_ReadOnly )		or
						 not IsMultipleOf( ulong(buf->GetDeviceAddress() + build.instanceData.offset), props.instanceDataAlign ))
				return false;
		}

		if ( build.scratch.id != Default )
		{
			auto*	buf = resMngr.GetResource( build.scratch.id, False{"don't inc ref"}, True{"quiet"} );
			if_unlikely( buf == null														or
						 build.scratch.offset >= buf->Size()								or
						 NoBits( buf->Description().usage, EBufferUsage::ASBuild_Scratch )	or
						 not IsMultipleOf( ulong(buf->GetDeviceAddress() + build.scratch.offset), props.scratchBufferAlign ))
				return false;
		}

		return true;
	}

/*
=================================================
	GetBufferStorage
=================================================
*/
	BufferSubRange  VRTScene::GetBufferStorage () C_NE___
	{
		ASSERT( _storage != Default );
		return BufferSubRange{ _storage, _offset, _desc.size };
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
