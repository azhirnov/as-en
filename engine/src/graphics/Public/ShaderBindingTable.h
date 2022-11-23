// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Primitives/DataRaceCheck.h"
#include "graphics/Public/ResourceManager.h"
#include "graphics/Public/CommandBufferTypes.h"

namespace AE::Graphics
{
	enum class RTMissIndex		: uint {};
	enum class RTRayIndex		: uint {};
	enum class RTCallableIndex	: uint {};



	//
	// Shader Binding Table Manager
	//

	class ShaderBindingTableManager
	{
		friend class ShaderBindingTable;

	// types
	public:
		struct Geometry
		{
			uint			index	= UMax;
			RTGeometryID	id;
		};

		using GeometryNames_t	= FlatHashMap< RTGeometryName::Optimized_t, uint >;
		using InstanceNames_t	= FlatHashMap< RTInstanceName::Optimized_t, Geometry >;

		struct RTGeometryInfo
		{
			GeometryNames_t		nameToIdx;
		};

		struct RTSceneInfo
		{
			InstanceNames_t		nameToGeom;
		};

		using RTGeometryMap_t	= FlatHashMap< RTGeometryID, RTGeometryInfo >;
		using RTSceneMap_t		= FlatHashMap< RTSceneID,	 RTSceneInfo >;


	// variables
	private:
		RTGeometryMap_t		_geomMap;
		RTSceneMap_t		_sceneMap;

		const Bytes32u		_groupStride;

		GfxMemAllocatorPtr	_gfxAlloc;

		DRC_ONLY(
			Threading::RWDataRaceCheck	_drCheck;
		)


	// methods
	public:
		ShaderBindingTableManager ();
		~ShaderBindingTableManager ();

		bool  Add (RTGeometryID id, ArrayView<RTGeometryName> names);
		bool  Add (RTSceneID id, ArrayView<RTInstanceName> names, ArrayView<RTGeometryID> geomIds);

		void  Remove (RTGeometryID id);
		void  Remove (RTSceneID id);
		void  Clear ();
	};
	


	//
	// Shader Binding Table
	//

	class ShaderBindingTable
	{
	// variables
	#ifdef AE_ENABLE_VULKAN
	public:
		VkStridedDeviceAddressRegionKHR		raygen;
		VkStridedDeviceAddressRegionKHR		miss;
		VkStridedDeviceAddressRegionKHR		hit;
		VkStridedDeviceAddressRegionKHR		callable;
	private:
		BufferID							_bufferId;

	#elif defined(AE_ENABLE_METAL)
	public:
		MetalIntersectionFnTable			intersectionTable;
		MetalVisibleFnTable					visibleTable;

	#endif


	// methods
	public:
		explicit ShaderBindingTable () {}
		~ShaderBindingTable () {}

		bool  BindRayGen (const RayTracingGroupName &name);
		bool  BindMiss (const RayTracingGroupName &name, RTMissIndex missIndex);
		bool  BindHitGroup (const RayTracingGroupName &name, const RTInstanceName &instance, const RTGeometryName &geometry, RTRayIndex rayIdx);
		bool  BindHitGroup (const RayTracingGroupName &name, const RTInstanceName &instance, RTRayIndex rayIdx);
		bool  BindCallable (const RayTracingGroupName &name, RTCallableIndex callableIdx);

		template <typename CopyCtx>
		bool  Upload (CopyCtx &ctx, EStagingHeapType heapType = EStagingHeapType::Static);
	};

	
/*
=================================================
	Upload
=================================================
*/
	template <typename CopyCtx>
	bool  ShaderBindingTable::Upload (CopyCtx &, EStagingHeapType)
	{
	//	ctx.UploadBuffer( _bufferId, heapType );
		return false;
	}


} // AE::Graphics
