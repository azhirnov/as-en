// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if 0
#include "threading/Primitives/DataRaceCheck.h"
#include "graphics/Public/ResourceManager.h"
#include "graphics/Public/CommandBufferTypes.h"

namespace AE::Graphics
{

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
	public:
		#ifdef AE_ENABLE_VULKAN
			VkStridedDeviceAddressRegionKHR		raygen;
			VkStridedDeviceAddressRegionKHR		miss;
			VkStridedDeviceAddressRegionKHR		hit;
			VkStridedDeviceAddressRegionKHR		callable;

		#elif defined(AE_ENABLE_METAL)

		#endif

	private:
		BufferID							_bufferId;
		ShaderBindingTableManager const&	_mngr;


	// methods
	public:
		explicit ShaderBindingTable (ShaderBindingTableManager &mngr);
		~ShaderBindingTable ();

		void  BindRayGen (const RayTracingGroupName &name);
		void  BindMiss (const RayTracingGroupName &name, uint missIndex);
		void  BindHitGroup (const RayTracingGroupName &name, const RTInstanceName &instance, const RTGeometryName &geometry, uint rayIdx);
		void  BindHitGroup (const RayTracingGroupName &name, const RTInstanceName &instance, uint rayIdx);
		void  BindCallable (const RayTracingGroupName &name, uint callableIdx);

		template <typename CopyCtx>
		void  Upload (CopyCtx &ctx, EStagingHeapType heapType = EStagingHeapType::Static);
	};


} // AE::Graphics
#endif
