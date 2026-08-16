// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Vulkan/Resources/VRTMicromap.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "graphics_rhi/Private/EnumUtils.h"

namespace AE::Graphics
{
	StaticAssert( sizeof(VkMicromapTriangleEXT)						== sizeof(RTMicromapBuild::Triangle) );
	StaticAssert( offsetof(VkMicromapTriangleEXT, dataOffset)		== offsetof(RTMicromapBuild::Triangle, dataOffset) );
	StaticAssert( offsetof(VkMicromapTriangleEXT, subdivisionLevel)	== offsetof(RTMicromapBuild::Triangle, subdivisionLevel) );
	StaticAssert( offsetof(VkMicromapTriangleEXT, format)			== offsetof(RTMicromapBuild::Triangle, format) );
	StaticAssert( sizeof(VkMicromapTriangleEXT::dataOffset)			== sizeof(RTMicromapBuild::Triangle::dataOffset) );
	StaticAssert( sizeof(VkMicromapTriangleEXT::subdivisionLevel)	== sizeof(RTMicromapBuild::Triangle::subdivisionLevel) );
	StaticAssert( sizeof(VkMicromapTriangleEXT::format)				== sizeof(RTMicromapBuild::Triangle::format) );

	StaticAssert( sizeof(VkMicromapUsageEXT)						== sizeof(RTMicromapInfo::Usage) );
	StaticAssert( offsetof(VkMicromapUsageEXT, count)				== offsetof(RTMicromapInfo::Usage, triangleCount) );
	StaticAssert( offsetof(VkMicromapUsageEXT, subdivisionLevel)	== offsetof(RTMicromapInfo::Usage, subdivisionLevel) );
	StaticAssert( offsetof(VkMicromapUsageEXT, format)				== offsetof(RTMicromapInfo::Usage, format) );
	StaticAssert( sizeof(VkMicromapUsageEXT::count)					== sizeof(RTMicromapInfo::Usage::triangleCount) );
	StaticAssert( sizeof(VkMicromapUsageEXT::subdivisionLevel)		== sizeof(RTMicromapInfo::Usage::subdivisionLevel) );
	//StaticAssert( sizeof(VkMicromapUsageEXT::format)				== sizeof(RTMicromapInfo::Usage::format) );

	StaticAssert( uint(EOpacityMicromapFormat::TwoState)			== VK_OPACITY_MICROMAP_FORMAT_2_STATE_EXT );
	StaticAssert( uint(EOpacityMicromapFormat::FourState)			== VK_OPACITY_MICROMAP_FORMAT_4_STATE_EXT );

	StaticAssert( uint(EDisplacementMicromapFormat::Tris64_Bytes64)		== VK_DISPLACEMENT_MICROMAP_FORMAT_64_TRIANGLES_64_BYTES_NV );
	StaticAssert( uint(EDisplacementMicromapFormat::Tris256_Bytes128)	== VK_DISPLACEMENT_MICROMAP_FORMAT_256_TRIANGLES_128_BYTES_NV );
	StaticAssert( uint(EDisplacementMicromapFormat::Tris1024_Bytes128)	== VK_DISPLACEMENT_MICROMAP_FORMAT_1024_TRIANGLES_128_BYTES_NV );

	StaticAssert( int(EOpacityMicromapSpecialIndex::FullyTransparent)			== VK_OPACITY_MICROMAP_SPECIAL_INDEX_FULLY_TRANSPARENT_EXT );
	StaticAssert( int(EOpacityMicromapSpecialIndex::FullyOpaque)				== VK_OPACITY_MICROMAP_SPECIAL_INDEX_FULLY_OPAQUE_EXT );
	StaticAssert( int(EOpacityMicromapSpecialIndex::FullyUnknownTransparent)	== VK_OPACITY_MICROMAP_SPECIAL_INDEX_FULLY_UNKNOWN_TRANSPARENT_EXT );
	StaticAssert( int(EOpacityMicromapSpecialIndex::FullyUnknownOpaque)			== VK_OPACITY_MICROMAP_SPECIAL_INDEX_FULLY_UNKNOWN_OPAQUE_EXT );
	StaticAssert( int(EOpacityMicromapSpecialIndex::ClusterGeometryDisableOpacityMicromap) == VK_OPACITY_MICROMAP_SPECIAL_INDEX_CLUSTER_GEOMETRY_DISABLE_OPACITY_MICROMAP_NV );

/*
=================================================
	destructor
=================================================
*/
	VRTMicromap::~VRTMicromap () __NE___
	{
		ASSERT( _micromap == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VRTMicromap::Create (ResourceManager &resMngr, const RTMicromapDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		CHECK_ERR( _micromap == Default );
		CHECK_ERR( desc.size > 0 );

		_desc = desc;
		GRES_CHECK( IsSupported( resMngr, desc ));

		// create buffer
		auto&	dev = resMngr.GetDevice();
		_memoryId	= resMngr.CreateMemoryObj( _desc.size, VK_BUFFER_USAGE_2_MICROMAP_STORAGE_BIT_EXT, RVRef(allocator), dbgName );
		CHECK_ERR( _memoryId );

		auto*	mem_obj = resMngr.GetResource( _memoryId );
		CHECK_ERR( mem_obj != null );

		VulkanMemoryObjInfo	mem_info;
		CHECK_ERR( mem_obj->GetMemoryInfo( OUT mem_info ));
		CHECK_ERR( mem_info.buffer != Default );
		CHECK_ERR( IsMultipleOf( mem_info.offset, 256_b ));  // from specs

		// create micromap
		VkMicromapCreateInfoEXT	info = {};
		info.sType		= VK_STRUCTURE_TYPE_MICROMAP_CREATE_INFO_EXT;
		info.buffer		= mem_info.buffer;
		info.offset		= VkDeviceSize( mem_info.offset );
		info.size		= VkDeviceSize( _desc.size );
		info.type		= VEnumCast( _desc.type );

		VK_CHECK_ERR( dev.vkCreateMicromapEXT( dev.GetVkDevice(), &info, null, OUT &_micromap ));

		dev.SetObjectName( _micromap, dbgName, VK_OBJECT_TYPE_MICROMAP_EXT );

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
	void  VRTMicromap::Destroy (ResourceManager &resMngr) __NE___
	{
		auto&	dev = resMngr.GetDevice();

		if ( _micromap != Default )
			dev.vkDestroyMicromapEXT( dev.GetVkDevice(), _micromap, null );

		resMngr.ImmediatelyRelease( INOUT _memoryId );

		_memoryId	= Default;
		_micromap	= Default;
		_desc		= Default;
		_storage	= Default;
		_offset		= 0_b;

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	GetBuildSizes
=================================================
*/
	RTMicromapBuildSizes  VRTMicromap::GetBuildSizes (const ResourceManager &resMngr, const RTMicromapInfo &desc) __NE___
	{
		GRES_CHECK( IsSupported( resMngr, desc ));

		auto&	dev = resMngr.GetDevice();

		VkMicromapBuildInfoEXT			build_info;
		VkMicromapBuildSizesInfoEXT		size_info	= {};

		size_info.sType = VK_STRUCTURE_TYPE_MICROMAP_BUILD_SIZES_INFO_EXT;

		CHECK_ERR( ConvertBuildInfo( resMngr, desc, OUT build_info ));

		dev.vkGetMicromapBuildSizesEXT( dev.GetVkDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &build_info, OUT &size_info );

		RTMicromapBuildSizes	res;
		res.micromapSize		= Bytes{ size_info.micromapSize };
		res.buildScratchSize	= Bytes{ size_info.buildScratchSize };
		res.discardable			= size_info.discardable;
		return res;
	}

/*
=================================================
	ConvertBuildInfo
=================================================
*/
	bool  VRTMicromap::ConvertBuildInfo (const ResourceManager &resMngr, const RTMicromapInfo &desc, OUT VkMicromapBuildInfoEXT &buildInfo) __NE___
	{
		CHECK_ERR( not desc.usage.empty() );

		buildInfo = {};
		buildInfo.sType = VK_STRUCTURE_TYPE_MICROMAP_BUILD_INFO_EXT;

		buildInfo.type	= VEnumCast( desc.type );
		buildInfo.flags	= VEnumCast( desc.buildFlags );
		buildInfo.mode	= VK_BUILD_MICROMAP_MODE_BUILD_EXT;

		GRES_CHECK( RTMicromapUsage_IsSupported( resMngr, desc.type, desc.usage ));

		buildInfo.usageCountsCount	= uint(desc.usage.size());
		buildInfo.pUsageCounts		= Cast<VkMicromapUsageEXT>( desc.usage.data() );
		return true;
	}

/*
=================================================
	ConvertBuildInfo
=================================================
*/
	bool  VRTMicromap::ConvertBuildInfo (const ResourceManager &resMngr, const RTMicromapBuild &desc, OUT VkMicromapBuildInfoEXT &buildInfo) __NE___
	{
		if ( not ConvertBuildInfo( resMngr, RefCast<RTMicromapInfo>(desc), OUT buildInfo ))
			return false;

		auto*	data_buf		= resMngr.GetResource( desc.data.id, False{"don't inc ref"}, True{"quiet"} );
		auto*	scratch			= resMngr.GetResource( desc.scratch.id, False{"don't inc ref"}, True{"quiet"} );
		auto*	triangle_array	= resMngr.GetResource( desc.triangleArray.id, False{"don't inc ref"}, True{"quiet"} );
		CHECK_ERR( data_buf != null and scratch != null and triangle_array != null );

		GRES_CHECK( desc.data.offset < data_buf->Size() );
		GRES_CHECK( desc.scratch.offset < scratch->Size() );
		GRES_CHECK( desc.triangleArray.offset < triangle_array->Size() );
		GRES_CHECK( desc.triangleArray.stride >= sizeof(VkMicromapTriangleEXT) );
		GRES_CHECK( AllBits( scratch->Description().usage, EBufferUsage::MMBuild_Scratch ));
		GRES_CHECK( AllBits( data_buf->Description().usage, EBufferUsage::MMBuild_ReadOnly ));
		GRES_CHECK( AllBits( triangle_array->Description().usage, EBufferUsage::MMBuild_ReadOnly ));

		buildInfo.data.deviceAddress			= BitCast<VkDeviceAddress>( data_buf->GetDeviceAddress() + desc.data.offset );
		buildInfo.scratchData.deviceAddress		= BitCast<VkDeviceAddress>( scratch->GetDeviceAddress() + desc.scratch.offset );
		buildInfo.triangleArray.deviceAddress	= BitCast<VkDeviceAddress>( triangle_array->GetDeviceAddress() + desc.triangleArray.offset );
		buildInfo.triangleArrayStride			= VkDeviceAddress{ desc.triangleArray.stride };
		return true;
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  VRTMicromap::IsSupported (const ResourceManager &resMngr, const RTMicromapInfo &desc) __NE___
	{
		return RTMicromapInfo_IsSupported( resMngr, desc );
	}

	bool  VRTMicromap::IsSupported (const ResourceManager &resMngr, const RTMicromapDesc &desc) __NE___
	{
		return RTMicromapDesc_IsSupported( resMngr, desc );
	}
/*
=================================================
	GetBufferStorage
=================================================
*/
	BufferSubRange  VRTMicromap::GetBufferStorage () C_NE___
	{
		ASSERT( _storage != Default );
		return BufferSubRange{ _storage, _offset, _desc.size };
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
