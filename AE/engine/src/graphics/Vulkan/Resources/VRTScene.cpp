// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VRTScene.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Private/EnumUtils.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VRTScene::~VRTScene () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( _buffer == Default );
		ASSERT( _accelStruct == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VRTScene::Create (VResourceManager &resMngr, const RTSceneDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _buffer == Default );
		CHECK_ERR( _accelStruct == Default );
		CHECK_ERR( desc.size > 0 );

		_desc = desc;
		GRES_CHECK( IsSupported( resMngr, desc ));

		// create buffer
		VkBufferCreateInfo	buf_ci = {};
		buf_ci.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buf_ci.flags		= 0;
		buf_ci.usage		= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
		buf_ci.size			= VkDeviceSize( _desc.size );
		buf_ci.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		auto&	dev = resMngr.GetDevice();
		VK_CHECK_ERR( dev.vkCreateBuffer( dev.GetVkDevice(), &buf_ci, null, OUT &_buffer ));

		_memoryId = resMngr.CreateMemoryObj( _buffer, BufferDesc{}.SetMemory( EMemoryType::DeviceLocal ), RVRef(allocator), dbgName );
		CHECK_ERR( _memoryId );

		VkAccelerationStructureCreateInfoKHR	blas_ci = {};
		blas_ci.sType		= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		blas_ci.createFlags	= 0;	// VK_ACCELERATION_STRUCTURE_CREATE_MOTION_BIT_NV
		blas_ci.buffer		= _buffer;
		blas_ci.offset		= 0;
		blas_ci.size		= VkDeviceSize( _desc.size );
		blas_ci.type		= VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;

		VK_CHECK_ERR( dev.vkCreateAccelerationStructureKHR( dev.GetVkDevice(), &blas_ci, null, OUT &_accelStruct ));

		dev.SetObjectName( _accelStruct, dbgName, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR );

		VkAccelerationStructureDeviceAddressInfoKHR	addr_info = {};
		addr_info.sType					= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		addr_info.accelerationStructure	= _accelStruct;

		_address = BitCast<DeviceAddress>( dev.vkGetAccelerationStructureDeviceAddressKHR( dev.GetVkDevice(), &addr_info ));
		CHECK_ERR( _address != Default );

		DEBUG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VRTScene::Destroy (VResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		auto&	dev = resMngr.GetDevice();

		if ( _buffer != Default )
			dev.vkDestroyBuffer( dev.GetVkDevice(), _buffer, null );

		if ( _accelStruct != Default )
			dev.vkDestroyAccelerationStructureKHR( dev.GetVkDevice(), _accelStruct, null );

		resMngr.ImmediatelyRelease( INOUT _memoryId );

		_address		= Default;
		_memoryId		= Default;
		_accelStruct	= Default;
		_buffer			= Default;
		_desc			= Default;

		DEBUG_ONLY( _debugName.clear(); )
	}

/*
=================================================
	GetBuildSizes
=================================================
*/
	RTASBuildSizes  VRTScene::GetBuildSizes (const VResourceManager &resMngr, const RTSceneBuild &desc) __NE___
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
	bool  VRTScene::ConvertBuildInfo (const VResourceManager &resMngr, const RTSceneBuild &desc,
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
	bool  VRTScene::IsSupported (const VResourceManager &resMngr, const RTSceneDesc &desc) __NE___
	{
		if_unlikely( resMngr.GetFeatureSet().accelerationStructure() != EFeature::RequireTrue )
			return false;

		if_unlikely( desc.size == 0 )
			return false;

		// TODO: desc.options

		return true;
	}

/*
=================================================
	IsSupported (RTSceneBuild)
=================================================
*/
	bool  VRTScene::IsSupported (const VResourceManager &resMngr, const RTSceneBuild &build) __NE___
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

			if_unlikely( buf == null																or
						 build.instanceData.offset >= buf->Size()									or
						 size > buf->Size()															or
						 not AllBits( buf->Description().usage, EBufferUsage::ASBuild_ReadOnly )	or
						 not IsMultipleOf( ulong(buf->GetDeviceAddress() + build.instanceData.offset), props.instanceDataAlign ))
				return false;
		}

		if ( build.scratch.id != Default )
		{
			auto*	buf = resMngr.GetResource( build.scratch.id, False{"don't inc ref"}, True{"quiet"} );
			if_unlikely( buf == null															or
						 build.scratch.offset >= buf->Size()									or
						 not AllBits( buf->Description().usage, EBufferUsage::ASBuild_Scratch )	or
						 not IsMultipleOf( ulong(buf->GetDeviceAddress() + build.scratch.offset), props.scratchBufferAlign ))
				return false;
		}

		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
