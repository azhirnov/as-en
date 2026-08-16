// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Vulkan/Resources/VRTGeometry.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "graphics_rhi/Private/EnumUtils.h"
# include "graphics_rhi/Vulkan/Utils/NextChain.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VRTGeometry::~VRTGeometry () __NE___
	{
		ASSERT( _accelStruct == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VRTGeometry::Create (ResourceManager &resMngr, const RTGeometryDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
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
		blas_ci.type		= VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

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
	void  VRTGeometry::Destroy (ResourceManager &resMngr) __NE___
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
	_Convert
=================================================
*/
	template <bool IsForBuilding>
	bool  VRTGeometry::_Convert (const ResourceManager &resMngr, const RTGeometryBuild &desc, IAllocatorRef allocator,
								 OUT uint* &outPrimitiveCount, OUT VkAccelerationStructureBuildRangeInfoKHR* &outRanges,
								 OUT VkAccelerationStructureBuildGeometryInfoKHR &outBuildInfo) __NE___
	{
		auto&		build_info	= outBuildInfo;

		const usize	geom_count	= desc.triangles.size() + desc.aabbs.size();
		auto*		p_geom		= allocator.Allocate< VkAccelerationStructureGeometryKHR >( geom_count );
		auto*		prim_count	= IsForBuilding ? null : allocator.Allocate< uint >( geom_count );
		auto*		ranges		= IsForBuilding ? allocator.Allocate< VkAccelerationStructureBuildRangeInfoKHR >( geom_count ) : null;
		auto*		micormaps	= desc.micromaps.empty() ? null : allocator.Allocate< VkAccelerationStructureTrianglesOpacityMicromapEXT >( desc.micromaps.size() );
		CHECK_ERR( p_geom != null and (prim_count != null or ranges != null) );
		CHECK_ERR( desc.micromaps.empty() or micormaps != null );

		build_info				= {};
		build_info.sType		= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		build_info.flags		= VEnumCast( desc.options );
		build_info.type			= VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		build_info.pGeometries	= p_geom;
		build_info.geometryCount= CheckCast{ geom_count };
		outPrimitiveCount		= prim_count;
		outRanges				= ranges;

		CHECK_ERR( desc.triangles.empty()	or desc.triangles.data< RTGeometryBuild::TrianglesInfo >() != null );
		CHECK_ERR( desc.aabbs.empty()		or desc.aabbs.data< RTGeometryBuild::AABBsInfo >() != null );

		if constexpr( IsForBuilding )
		{
			CHECK_ERR( desc.triangles.empty()	or desc.triangles.data< RTGeometryBuild::TrianglesData >() != null );
			CHECK_ERR( desc.aabbs.empty()		or desc.aabbs.data< RTGeometryBuild::AABBsData >() != null );
		}

		for (usize i : IndicesOnly( desc.micromaps ))
		{
			auto&	info	= desc.micromaps[i];
			auto&	dst		= micormaps[i];
			auto*	mm		= resMngr.GetResource( info.micromapId,	 False{"don't inc ref"}, True{"quiet"} );
			auto*	ib		= resMngr.GetResource( info.indexBuffer, False{"don't inc ref"}, True{"quiet"} );
			CHECK_ERR( (mm != null) or (ib != null) );

			dst = {};
			dst.sType				= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_TRIANGLES_OPACITY_MICROMAP_EXT;
			dst.indexType			= info.indexType == Default ? VK_INDEX_TYPE_NONE_KHR : VEnumCast( info.indexType );
			dst.usageCountsCount	= uint(info.usage.size());
			dst.pUsageCounts		= Cast<VkMicromapUsageEXT>( info.usage.data() );	// it is valid cast, see checks in 'VRTMicromap.cpp'
			dst.indexStride			= VkDeviceSize{ info.indexStride };

			if ( mm != null )
			{
				dst.micromap = mm->Handle();
			}
			if ( ib != null )
			{
				GRES_CHECK( info.indexStride > 0 );
				GRES_CHECK( info.indexType != Default );
				GRES_CHECK( ib->HasDeviceAddress() );
				dst.indexBuffer.deviceAddress = BitCast<VkDeviceAddress>( ib->GetDeviceAddress() + info.indexBufferOffset );
			}
			else
			{
				GRES_CHECK( info.indexType == Default );
			}
		}

		for (usize i : IndicesOnly( desc.triangles ))
		{
			auto&	info		= desc.triangles.at< RTGeometryBuild::TrianglesInfo >(i);
			auto&	dst			= *(p_geom++);
			auto&	tri			= dst.geometry.triangles;

			dst.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
			dst.pNext			= null;
			dst.geometryType	= VK_GEOMETRY_TYPE_TRIANGLES_KHR;
			dst.flags			= VEnumCast( info.options );

			tri.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
			tri.pNext			= null;
			tri.vertexFormat	= VEnumCast( info.vertexFormat );
			tri.maxVertex		= info.maxVertex;
			tri.indexType		= info.indexType == Default ? VK_INDEX_TYPE_NONE_KHR : VEnumCast( info.indexType );

			VNextChain			p_tri_next {tri};

			if constexpr( IsForBuilding )
			{
				auto&	data	= desc.triangles.at< RTGeometryBuild::TrianglesData >(i);

				auto*	vb	= resMngr.GetResource( data.vertexData,		False{"don't inc ref"}, True{"quiet"} );
				auto*	ib	= resMngr.GetResource( data.indexData,		False{"don't inc ref"}, True{"quiet"} );
				auto*	tb	= resMngr.GetResource( data.transformData,	False{"don't inc ref"}, True{"quiet"} );
				CHECK_ERR( vb != null );
				CHECK_ERR( (ib != null) == data.indexData.IsValid() );
				CHECK_ERR( (tb != null) == data.transformData.IsValid() );
				CHECK_ERR( (info.indexType != Default) == data.indexData.IsValid() );
				CHECK_ERR( info.allowTransforms == data.transformData.IsValid() );

				GRES_CHECK( AllBits( vb->Description().usage, EBufferUsage::ASBuild_ReadOnly ));
				GRES_CHECK( ib == null or AllBits( ib->Description().usage, EBufferUsage::ASBuild_ReadOnly ));
				GRES_CHECK( tb == null or AllBits( tb->Description().usage, EBufferUsage::ASBuild_ReadOnly ));

				tri.vertexData.deviceAddress	= BitCast<VkDeviceAddress>( vb->GetDeviceAddress() + data.vertexDataOffset );
				tri.vertexStride				= VkDeviceSize(data.vertexStride);
				tri.indexData.deviceAddress		= ib != null ? BitCast<VkDeviceAddress>( ib->GetDeviceAddress() + data.indexDataOffset )     : 0;
				tri.transformData.deviceAddress = tb != null ? BitCast<VkDeviceAddress>( tb->GetDeviceAddress() + data.transformDataOffset ) : 0;

				// must be aligned to the size in bytes of the smallest component of the format in vertexFormat
				GRES_CHECK( IsMultipleOf( tri.vertexData.deviceAddress, EVertexType_SizeOf( info.vertexFormat & ~EVertexType::_VecMask )));

				GRES_CHECK( Bytes{data.vertexStride} >= EVertexType_SizeOf( info.vertexFormat ));
				GRES_CHECK( (info.indexType == Default) or IsMultipleOf( tri.indexData.deviceAddress, EIndex_SizeOf( info.indexType )) );
				GRES_CHECK( (not info.allowTransforms) or IsMultipleOf( tri.transformData.deviceAddress, 16 ) );
				GRES_CHECK( vb->Size() >= (data.vertexDataOffset + Bytes{data.vertexStride} * info.maxVertex) );
				GRES_CHECK( (info.indexType == Default) or (ib->Size() >= (data.indexDataOffset + info.maxPrimitives * EIndex_SizeOf( info.indexType ))) );
				GRES_CHECK( (not info.allowTransforms) or (tb->Size() >= (data.transformDataOffset + SizeOf<VkTransformMatrixKHR>)) );

				auto&	range = *(ranges++);
				range.primitiveCount	= info.maxPrimitives;
				range.primitiveOffset	= 0;
				range.firstVertex		= 0;
				range.transformOffset	= 0;
			}
			else
			{
				// address is not used, just test for != 0
				tri.transformData.deviceAddress = info.allowTransforms ? 1 : 0;

				tri.vertexData.deviceAddress	= 0;	// unused
				tri.vertexStride				= 0;	// unused
				tri.indexData.deviceAddress		= 0;	// unused

				*(prim_count++) = info.maxPrimitives;
			}

			if ( info.micromapIndex != UMax )
			{
				CHECK_ERR( info.micromapIndex < desc.micromaps.size() );
				p_tri_next.AddConst( micormaps[ info.micromapIndex ]);
			}
		}

		for (usize i : IndicesOnly( desc.aabbs ))
		{
			auto&	info		= desc.aabbs.at< RTGeometryBuild::AABBsInfo >(i);
			auto&	dst			= *(p_geom++);
			auto&	aabb		= dst.geometry.aabbs;

			dst.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
			dst.pNext			= null;
			dst.geometryType	= VK_GEOMETRY_TYPE_AABBS_KHR;
			dst.flags			= VEnumCast( info.options );

			aabb.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
			aabb.pNext			= null;

			if constexpr( IsForBuilding )
			{
				auto&	data = desc.aabbs.at< RTGeometryBuild::AABBsData >(i);

				auto*	buf = resMngr.GetResource( data.data, False{"don't inc ref"}, True{"quiet"} );
				CHECK_ERR( buf != null );
				GRES_CHECK( AllBits( buf->Description().usage, EBufferUsage::ASBuild_ReadOnly ));

				aabb.data.deviceAddress = BitCast<VkDeviceAddress>( buf->GetDeviceAddress() + data.dataOffset );
				aabb.stride				= VkDeviceSize(data.stride);

				GRES_CHECK( IsMultipleOf( aabb.data.deviceAddress, 8 ));
				GRES_CHECK( buf->Size() >= (data.dataOffset + SizeOf<VkAabbPositionsKHR> * info.maxAABBs) );

				auto&	range = *(ranges++);
				range.primitiveCount	= info.maxAABBs;
				range.primitiveOffset	= 0;
				range.firstVertex		= 0;
				range.transformOffset	= 0;
			}
			else
			{
				aabb.data.deviceAddress	= 0;	// unused
				aabb.stride				= 0;	// unused

				*(prim_count++) = info.maxAABBs;
			}
		}

		return true;
	}

/*
=================================================
	GetBuildSizes
=================================================
*/
	RTASBuildSizes  VRTGeometry::GetBuildSizes (const ResourceManager &resMngr, const RTGeometryBuild &desc) __NE___
	{
		GRES_CHECK( IsSupported( resMngr, desc ));

		IAllocatorAdaptor< VTempLinearAllocator >		allocator;
		VkAccelerationStructureBuildGeometryInfoKHR		build_info;
		VkAccelerationStructureBuildSizesInfoKHR		size_info	= {};
		VkAccelerationStructureBuildRangeInfoKHR *		ranges		= null;
		uint *											prim_count	= null;

		size_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

		CHECK_ERR( _Convert<false>( resMngr, desc, allocator, OUT prim_count, OUT ranges, OUT build_info ));
		NonNull( prim_count );

		// from specs:
		//	The 'srcAccelerationStructure', 'dstAccelerationStructure', 'mode', 'scratchData' members of pBuildInfo are ignored.

		auto&	dev = resMngr.GetDevice();
		dev.vkGetAccelerationStructureBuildSizesKHR( dev.GetVkDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &build_info, prim_count, OUT &size_info );

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
	bool  VRTGeometry::ConvertBuildInfo (const ResourceManager &resMngr, IAllocatorRef allocator, const RTGeometryBuild &desc,
										 OUT VkAccelerationStructureBuildRangeInfoKHR* &ranges, OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo) __NE___
	{
		GRES_CHECK( IsSupported( resMngr, desc ));

		uint*	prim_count	= null;
		CHECK_ERR( _Convert<true>( resMngr, desc, allocator, OUT prim_count, OUT ranges, OUT buildInfo ));
		NonNull( ranges );
		return true;
	}

	bool  VRTGeometry::ConvertBuildInfo (const ResourceManager &resMngr, IAllocatorRef allocator, const RTGeometryBuild &desc,
										 OUT uint* &maxPrimitiveCounts, OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo) __NE___
	{
		GRES_CHECK( IsSupported( resMngr, desc ));

		VkAccelerationStructureBuildRangeInfoKHR*	ranges = null;
		CHECK_ERR( _Convert<true>( resMngr, desc, allocator, OUT maxPrimitiveCounts, OUT ranges, OUT buildInfo ));
		return true;
	}

/*
=================================================
	IsSerializedMemoryCompatible
=================================================
*/
	bool  VRTGeometry::IsSerializedMemoryCompatible (const VDevice &dev, const void* ptr, Bytes size) __NE___
	{
		CHECK_ERR( size >= sizeof(VK_UUID_SIZE)*2 );

		VkAccelerationStructureVersionInfoKHR	info;
		info.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_VERSION_INFO_KHR;
		info.pNext			= null;
		info.pVersionData	= Cast<uint8_t>(ptr);

		VkAccelerationStructureCompatibilityKHR	compat = VK_ACCELERATION_STRUCTURE_COMPATIBILITY_INCOMPATIBLE_KHR;

		dev.vkGetDeviceAccelerationStructureCompatibilityKHR( dev.GetVkDevice(), &info, OUT &compat );

		return compat == VK_ACCELERATION_STRUCTURE_COMPATIBILITY_COMPATIBLE_KHR;
	}

/*
=================================================
	IsSupported (RTGeometryDesc)
=================================================
*/
	bool  VRTGeometry::IsSupported (const ResourceManager &resMngr, const RTGeometryDesc &desc) __NE___
	{
		return RTGeometry_IsSupported( resMngr, desc );
	}

/*
=================================================
	IsSupported (RTGeometryBuild)
=================================================
*/
	bool  VRTGeometry::IsSupported (const ResourceManager &resMngr, const RTGeometryBuild &build) __NE___
	{
		auto&		props		= resMngr.GetDevice().GetDeviceProperties().rayTracing;
		const usize	geom_count	= build.triangles.size() + build.aabbs.size();
		ulong		max_prims	= 0;

		if_unlikely( geom_count == 0 or geom_count > props.maxGeometries )
			return false;

		FlatHashSet<EVertexType>	vertex_types;

		for (usize i = 0; i < build.triangles.size(); ++i)
		{
			auto&	info = build.triangles.at< RTGeometryBuild::TrianglesInfo >(i);

			max_prims += info.maxPrimitives;
			vertex_types.insert( info.vertexFormat );
		}

		for (usize i = 0; i < build.aabbs.size(); ++i)
		{
			auto&	info = build.aabbs.at< RTGeometryBuild::AABBsInfo >(i);

			max_prims += info.maxAABBs;
		}

		for (auto vt : vertex_types) {
			if_unlikely( not VBuffer::IsSupportedForASVertex( resMngr, vt ))
				return false;
		}

		if_unlikely( max_prims  > props.maxPrimitives	or
					 geom_count > props.maxGeometries )
			return false;

		return true;
	}

/*
=================================================
	GetBufferStorage
=================================================
*/
	BufferSubRange  VRTGeometry::GetBufferStorage () C_NE___
	{
		ASSERT( _storage != Default );
		return BufferSubRange{ _storage, _offset, _desc.size };
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
