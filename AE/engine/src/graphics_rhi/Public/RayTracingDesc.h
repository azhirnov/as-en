// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	RTGeometry			- Bottom Level Acceleration Structure (BLAS).
						  Triangle BLAS can provide better ray intersection performance.
	RTScene				- Top Level Acceleration Structure (TLAS).
	RTCluster			- Cluster Acceleration Structure (CLAS).
						  CLAS were designed foremost to speed up BLAS builds in a GPU-driven pipeline.
	RTPartitionedScene	- Partitioned Top Level Acceleration Structure (PTLAS).
						  Separates TLAS into partitions that can be updated separately.

	All buffers, buffer offsets and strides are unused during 'IResourceManager::GetRT***Sizes()'.
*/

#pragma once

#include "graphics_rhi/Public/IDs.h"
#include "graphics_rhi/Public/RayTracingEnums.h"
#include "graphics_rhi/Public/VertexDesc.h"
#include "graphics_rhi/Public/BufferDesc.h"

namespace AE::Graphics
{
	// 3x4 row-major affine transformation matrix
	//
	//      rotation        translation
	// [0,0]  [0,1]  [0,2]    [0,3]
	// [1,0]  [1,1]  [1,2]    [1,3]
	// [2,0]  [2,1]  [2,2]    [2,3]
	using RTMatrixStorage = MatrixStorage< float, 3, 4, EMatrixOrder::RowMajor, sizeof(float) >;


	//
	// Ray Tracing Geometry Build command
	//
	struct RTGeometryBuild
	{
	// types
		struct BufferWithOffset
		{
			BufferID		id;
			Bytes			offset;

			BufferWithOffset ()							__NE___	{}
			BufferWithOffset (BufferID id, Bytes off)	__NE___ : id{id}, offset{off} {}
		};

		struct BufferWithOffsetAndStride : BufferWithOffset
		{
			Bytes			stride;
		};

		struct TrianglesInfo
		{
			ERTGeometryOpt	options				= Default;
			uint			maxPrimitives		= 0;
			uint			maxVertex			= 0;			// vulkan only
			EVertexType		vertexFormat		= Default;
			EIndex			indexType			= Default;		// optional
			bool			allowTransforms		= false;
		};

		struct TrianglesData
		{
			BufferID		vertexData;			// requires EBufferUsage::ASBuild_ReadOnly,	content: 'vertexFormat',	access: EResourceState::BuildRTAS_Read
												// - if x component of vertex is NaN then triangle is inactive.

			BufferID		indexData;			// requires EBufferUsage::ASBuild_ReadOnly,	content: 'indexType',		access: EResourceState::BuildRTAS_Read,  optional?
			BufferID		transformData;		// requires EBufferUsage::ASBuild_ReadOnly,	content: RTMatrixStorage,	access: EResourceState::BuildRTAS_Read,  optional
			Bytes32u		vertexStride;
			Bytes			vertexDataOffset;
			Bytes			indexDataOffset;
			Bytes			transformDataOffset;
		};

		struct AABBsInfo
		{
			ERTGeometryOpt	options				= Default;
			uint			maxAABBs			= 0;
		};

		struct AABBsData
		{
			BufferID		data;				// requires EBufferUsage::ASBuild_ReadOnly,	content: float[6],	access: EResourceState::BuildRTAS_Read
												// - if min.x coordinate is NaN then AABB is inactive.
			Bytes			dataOffset;
			Bytes			stride;
		};

		using Triangles		= TupleArrayView< TrianglesInfo,	TrianglesData	>;
		using AABBs			= TupleArrayView< AABBsInfo,		AABBsData		>;
		using ScratchBuffer	= BufferWithOffset;


	// variables
		Triangles		triangles;
		AABBs			aabbs;
		ERTASOptions	options		= Default;
		ScratchBuffer	scratch;				// requires EBufferUsage::ASBuild_Scratch,	access: EResourceState::BuildRTAS_ScratchBuffer


	// methods
		RTGeometryBuild () __NE___ {}

		RTGeometryBuild (ArrayView<TrianglesInfo>	trianglesInfo,	ArrayView<TrianglesData>	trianglesData,
						 ArrayView<AABBsInfo>		aabbsInfo,		ArrayView<AABBsData>		aabbsData,
						 ERTASOptions opt) __NE___ :
			triangles{ trianglesInfo, trianglesData },
			aabbs{ aabbsInfo, aabbsData },
			options{ opt }
		{}

		RTGeometryBuild (ArrayView<TrianglesInfo> trianglesInfo, ArrayView<AABBsInfo> aabbsInfo, ERTASOptions opt) __NE___ :
			RTGeometryBuild{ trianglesInfo, Default, aabbsInfo, Default, opt }
		{}

		RTGeometryBuild&  SetScratchBuffer (BufferID id, Bytes offset = 0_b) __NE___
		{
			ASSERT( id );
			scratch.id		= id;
			scratch.offset	= offset;
			return *this;
		}

		ND_ usize  GeometryCount ()		C_NE___	{ return triangles.size() + aabbs.size(); }
	};



	//
	// Ray Tracing Geometry description
	//
	struct RTGeometryDesc
	{
		Bytes			size;		// same as RTASBuildSizes::size
		ERTASOptions	options		= Default;

		RTGeometryDesc ()									__NE___ {}
		RTGeometryDesc (Bytes size, ERTASOptions opt)		__NE___ : size{size}, options{opt} {}

		ND_ bool  operator == (const RTGeometryDesc &rhs)	__NE___	{ return (size == rhs.size) and (options == rhs.options); }
		ND_ bool  IsExclusiveSharing ()						C_NE___	{ return false; }
	};
//-----------------------------------------------------------------------------



	//
	// Ray Tracing Scene Build command
	//
	struct RTSceneBuild
	{
	// types
		// VkAccelerationStructureInstanceKHR
		struct InstanceVk
		{
			using Self = InstanceVk;

		// variables
			RTMatrixStorage		transform;
			uint				instanceCustomIndex	: 24;
			uint				mask				:  8;
			uint				instanceSBTOffset	: 24;
			uint				flags				:  8;	// ERTInstanceOpt | VkGeometryInstanceFlags
			DeviceAddress		rtas;						// - if 0 then instance is inactive.

		// methods
			InstanceVk ()										__NE___ = default;
			Self&  Init ()										__NE___;
			Self&  SetIdentity ()								__NE___	{ transform			= RTMatrixStorage::Identity();	return *this; }
			Self&  SetTransform (const RTMatrixStorage &value)	__NE___	{ transform			= value;		return *this; }
			Self&  SetTransform (const float4x3 &value)			__NE___	{ transform			= value;		return *this; }
			Self&  SetRotation (const Quat &q)					__NE___	{ transform.Inject( float3x3{q} );	return *this; }
			Self&  SetTranslation (const float3 &pos)			__NE___	{ transform.SetTranslation( pos );	return *this; }
			Self&  SetMask (uint value)							__NE___	{ mask				= value;		ASSERT( mask == value or value == UMax );	return *this; }
			Self&  SetInstanceOffset (uint value)				__NE___	{ instanceSBTOffset	= value;		ASSERT( instanceSBTOffset == value );		return *this; }
			Self&  SetInstanceCustomIndex (uint value)			__NE___	{ instanceCustomIndex = value;		ASSERT( instanceCustomIndex == value );		return *this; }
			Self&  SetFlags (ERTInstanceOpt value)				__NE___;
		};


		// MTLAccelerationStructureInstanceDescriptor
		struct InstanceMtl
		{
			using Self = InstanceMtl;

		// variables
			RTMatrixStorage		transform;
			uint				options;					// ERTInstanceOpt | MTLAccelerationStructureInstanceOptions
			uint				mask;
			uint				instanceSBTOffset;
			uint				rtasIndex;

		// methods
			InstanceMtl ()										__NE___ = default;
			Self&  Init ()										__NE___;
			Self&  SetIdentity ()								__NE___	{ transform			= RTMatrixStorage::Identity();	return *this; }
			Self&  SetTransform (const RTMatrixStorage &value)	__NE___	{ transform			= value;		return *this; }
			Self&  SetTransform (const float4x3 &value)			__NE___	{ transform			= value;		return *this; }
			Self&  SetRotation (const Quat &q)					__NE___	{ transform.Inject( float3x3{q} );	return *this; }
			Self&  SetTranslation (const float3 &pos)			__NE___	{ transform.SetTranslation( pos );	return *this; }
			Self&  SetMask (uint value)							__NE___	{ mask				= value;		return *this; }
			Self&  SetInstanceOffset (uint value)				__NE___	{ instanceSBTOffset	= value;		return *this; }
			Self&  SetInstanceCustomIndex (uint)				__NE___	{ DBG_WARNING( "not supported" );	return *this; }
			Self&  SetFlags (ERTInstanceOpt value)				__NE___;
		};


		union Instance
		{
			InstanceVk	vk;
			InstanceMtl	mtl;
			char		_unused [64];

			Instance () __NE___ {}
		};

		static constexpr Bytes	InstanceSize {64};
		StaticAssert( sizeof(InstanceVk)	== InstanceSize );
		StaticAssert( sizeof(InstanceMtl)	== InstanceSize );
		StaticAssert( sizeof(Instance)		== InstanceSize );
		StaticAssert( sizeof(ERTInstanceOpt) == 1 );

		// TODO: MTLAccelerationStructureUserIDInstanceDescriptor, MTLAccelerationStructureMotionInstanceDescriptor

		using ScratchBuffer		= RTGeometryBuild::BufferWithOffset;
		using InstanceBuffer	= RTGeometryBuild::BufferWithOffsetAndStride;
		using GeometrySet_t		= FixedSet< RTGeometryID, 64 >;


	// variables
		uint				maxInstanceCount	= 0;
		ERTASOptions		options				= Default;
		ScratchBuffer		scratch;						// requires EBufferUsage::ASBuild_Scratch,	access: EResourceState::BuildRTAS_ScratchBuffer
		InstanceBuffer		instanceData;
		GeometrySet_t		uniqueGeoms;					// used by RG and in Metal


	// methods
		RTSceneBuild ()								__NE___	{}
		RTSceneBuild (uint count, ERTASOptions opt)	__NE___ : maxInstanceCount{count}, options{opt} {}

		RTSceneBuild&  SetScratchBuffer (BufferID id, Bytes offset = 0_b) __NE___
		{
			scratch.id		= id;
			scratch.offset	= offset;
			return *this;
		}

		RTSceneBuild&  SetInstanceData (BufferID id, Bytes offset = 0_b, Bytes stride = InstanceSize) __NE___
		{
			instanceData.id		= id;
			instanceData.offset	= offset;
			instanceData.stride	= stride;
			return *this;
		}

		// access: EResourceState::BuildRTAS_Read
		ND_ bool  SetGeometry (RTGeometryID id, INOUT InstanceVk &inst)		__NE___;
		ND_ bool  SetGeometry (RTGeometryID id, INOUT InstanceMtl &inst)	__NE___;
	};



	//
	// Ray Tracing Scene description
	//
	struct RTSceneDesc
	{
	// variables
		Bytes			size;		// same as RTASBuildSizes::size
		ERTASOptions	options		= Default;


	// methods
		RTSceneDesc ()									__NE___	{}
		RTSceneDesc (Bytes size, ERTASOptions opt)		__NE___	: size{size}, options{opt} {}

		ND_ bool  operator == (const RTSceneDesc &rhs)	__NE___	{ return (size == rhs.size) and (options == rhs.options); }
		ND_ bool  IsExclusiveSharing ()					C_NE___	{ return false; }
	};
//-----------------------------------------------------------------------------



	//
	// Ray Tracing Partitioned Scene Information
	//
	struct RTPartitionedSceneInfo
	{
		using Self = RTPartitionedSceneInfo;

	// variables
		ERTASOptions	options							= Default;
		uint			instanceCount					= 0;
		uint			maxInstancesPerPartition		= 0;
		uint			partitionCount					= 0;
		uint			maxInstancesInGlobalPartition	= 0;


	// methods
		RTPartitionedSceneInfo ()								__NE___ {}

		Self&  SetOptions (ERTASOptions value)					__NE___	{ options						= value;	return *this; }
		Self&  SetInstanceCount (uint value)					__NE___	{ instanceCount					= value;	return *this; }
		Self&  SetPartitionCount (uint value)					__NE___	{ partitionCount				= value;	return *this; }
		Self&  SetMaxInstancesPerPartition (uint value)			__NE___	{ maxInstancesPerPartition		= value;	return *this; }
		Self&  SetMaxInstancesInGlobalPartition (uint value)	__NE___	{ maxInstancesInGlobalPartition	= value;	return *this; }
	};



	//
	// Ray Tracing Partitioned Scene Build command
	//
	struct RTPartitionedSceneBuild : RTPartitionedSceneInfo
	{
	// types
		using BufferWithOffset	= RTGeometryBuild::BufferWithOffset;
		using BufferOrAddress	= Union< NullUnion, BufferWithOffset, DeviceAddress >;


		// VkPartitionedAccelerationStructureWriteInstanceDataNV
		struct WriteInstanceVk
		{
			using Self	 = WriteInstanceVk;
			using AABB_t = StaticArray< float, 6 >;

		// variables
			RTMatrixStorage		transform;
			AABB_t				explicitAABB;
			uint				instanceCustomIndex;
			uint				instanceMask;			// 8 bits
			uint				instanceSBTOffset;		// 24 bits
			uint				instanceFlags;			// EPartitionedInstanceOpt | VkPartitionedAccelerationStructureInstanceFlagsNV
			uint				instanceIndex;
			uint				partitionIndex;
			DeviceAddress		rtas;					// - if 0 then instance is inactive.

		// methods
			WriteInstanceVk ()									__NE___ = default;
			Self&  Init ()										__NE___;
			Self&  SetIdentity ()								__NE___	{ transform		= RTMatrixStorage::Identity();	return *this; }
			Self&  SetTransform (const RTMatrixStorage &value)	__NE___	{ transform		= value;			return *this; }
			Self&  SetTransform (const float4x3 &value)			__NE___	{ transform		= value;			return *this; }
			Self&  SetRotation (const Quat &q)					__NE___	{ transform.Inject( float3x3{q} );	return *this; }
			Self&  SetTranslation (const float3 &pos)			__NE___	{ transform.SetTranslation( pos );	return *this; }
			Self&  SetInstanceMask (uint value)					__NE___	{ instanceMask	= value;			return *this; }
			Self&  SetInstanceOffset (uint value)				__NE___	{ instanceSBTOffset	= value;		return *this; }
			Self&  SetInstanceCustomIndex (uint value)			__NE___	{ instanceCustomIndex = value;		return *this; }
			Self&  SetInstanceIndex (uint value)				__NE___	{ instanceIndex	= value;			return *this; }
			Self&  SetPartitionIndex (uint value)				__NE___ { partitionIndex= value;			return *this; }
			Self&  SetInstanceFlags (ERTInstanceOpt value)		__NE___;
			Self&  SetInstanceFlags (EPartitionedInstanceOpt)	__NE___;
		};


		// VkPartitionedAccelerationStructureUpdateInstanceDataNV
		struct UpdateInstanceVk
		{
			using Self = UpdateInstanceVk;

		// variables
			uint				instanceIndex;
			uint				instanceSBTOffset;
			DeviceAddress		rtas;					// - if 0 then instance is inactive.

		// methods
			UpdateInstanceVk ()									__NE___ = default;
			Self&  SetInstanceOffset (uint value)				__NE___	{ instanceSBTOffset	= value;	return *this; }
			Self&  SetInstanceIndex (uint value)				__NE___	{ instanceIndex		= value;	return *this; }
		};


		// VkPartitionedAccelerationStructureWritePartitionTranslationDataNV
		struct WritePartitionTranslationVk
		{
			using Self = WritePartitionTranslationVk;

		// variables
			uint				partitionIndex;
			packed_float3		partitionTranslation;

		// methods
			WritePartitionTranslationVk ()						__NE___ = default;
			Self&  SetPartitionIndex (uint value)				__NE___ { partitionIndex		= value;	return *this; }
			Self&  SetPartitionTranslation (float3 value)		__NE___ { partitionTranslation	= value;	return *this; }
		};


		// VkStridedDeviceAddressNV
		struct ArgDataVk
		{
			DeviceAddress		data;		// reference to 'WriteInstanceVk', 'UpdateInstanceVk' or 'WritePartitionTranslationVk'.
											// TODO: usage?  access?
			Bytes				stride;
		};


		// VkBuildPartitionedAccelerationStructureIndirectCommandNV
		struct BuildIndirectCommandVk
		{
		// variables
			EPartitionedCmd		opType		= Default;	// VkPartitionedAccelerationStructureOpTypeNV
			uint				argCount	= 0;		// number of 'ArgDataVk' elements.
			DeviceAddress		argData		= Default;	// array of 'ArgDataVk'.					TODO: usage?  access?
			Bytes				argDataStride;			// must be 0 or >= ArgDataSize.

		// methods
			BuildIndirectCommandVk ()							__NE___ = default;

			ND_ bool  Set (EPartitionedCmd type, uint count, BufferID id, Bytes offset = 0_b, Bytes stride = 0_b) __NE___;
		};


		static constexpr Bytes	WriteInstanceSize				{104};
		static constexpr Bytes	UpdateInstanceSize				{16};
		static constexpr Bytes	WritePartitionTranslationSize	{16};
		static constexpr Bytes	BuildIndirectCommandSize		{24};
		static constexpr Bytes	ArgDataSize						{16};
		static constexpr Bytes	InfosCountSize					{4};

		StaticAssert( sizeof(WriteInstanceVk)				== WriteInstanceSize );
		StaticAssert( sizeof(UpdateInstanceVk)				== UpdateInstanceSize );
		StaticAssert( sizeof(WritePartitionTranslationVk)	== WritePartitionTranslationSize );
		StaticAssert( sizeof(BuildIndirectCommandVk)		== BuildIndirectCommandSize );
		StaticAssert( sizeof(ArgDataVk)						== ArgDataSize );

		using GeometrySet_t	= FixedSet< RTGeometryID, 8 >;
		using Self			= RTPartitionedSceneBuild;


	// variables
		BufferOrAddress		srcPartitionedScene;	// optional, may be same as 'dstPartitionedScene'.
		BufferOrAddress		dstPartitionedScene;	// requires EBufferUsage::RTAS_Storage,  access: EResourceState::BuildRTAS_ReadWrite.
													// must be aligned to 256 bytes.

		BufferOrAddress		scratch;				// requires EBufferUsage::ASBuild_Scratch,	access: EResourceState::BuildRTAS_ScratchBuffer.

		BufferOrAddress		srcInfos;				// contains array of 'BuildIndirectCommandVk'.
													// number of inputs must be >= 'srcInfosCount'.
													// each element of array must have a unique 'EPartitionedCmd'.
													// requires EBufferUsage::ASBuild_ReadOnly,  access: EResourceState::BuildRTAS_IndirectBuffer.

		BufferOrAddress		srcInfosCount;			// contains 'uint' value.
													// must be aligned to 4 bytes.
													// requires EBufferUsage::ASBuild_ReadOnly,  access: EResourceState::BuildRTAS_IndirectBuffer.

		GeometrySet_t		uniqueGeoms;			// used by RG


	// methods
		RTPartitionedSceneBuild ()											__NE___ {}

		Self&  SetSrcScene (BufferID id, Bytes offset = 0_b)				__NE___	{ ASSERT( id );  srcPartitionedScene.emplace<BufferWithOffset>( id, offset );	return *this; }
		Self&  SetDstScene (BufferID id, Bytes offset = 0_b)				__NE___	{ ASSERT( id );  dstPartitionedScene.emplace<BufferWithOffset>( id, offset );	return *this; }
		Self&  SetSrcInfos (BufferID id, Bytes offset = 0_b)				__NE___	{ ASSERT( id );  srcInfos.emplace<BufferWithOffset>( id, offset );				return *this; }
		Self&  SetSrcInfosCount (BufferID id, Bytes offset = 0_b)			__NE___	{ ASSERT( id );  srcInfosCount.emplace<BufferWithOffset>( id, offset );			return *this; }
		Self&  SetScratchBuffer (BufferID id, Bytes offset = 0_b)			__NE___	{ ASSERT( id );  scratch.emplace<BufferWithOffset>( id, offset );				return *this; }

		Self&  SetSrcScene (DeviceAddress addr)								__NE___	{ ASSERT( addr != Default );  srcPartitionedScene.emplace<DeviceAddress>( addr );	return *this; }
		Self&  SetDstScene (DeviceAddress addr)								__NE___	{ ASSERT( addr != Default );  dstPartitionedScene.emplace<DeviceAddress>( addr );	return *this; }
		Self&  SetSrcInfos (DeviceAddress addr)								__NE___	{ ASSERT( addr != Default );  srcInfos.emplace<DeviceAddress>( addr );				return *this; }
		Self&  SetSrcInfosCount (DeviceAddress addr)						__NE___	{ ASSERT( addr != Default );  srcInfosCount.emplace<DeviceAddress>( addr );			return *this; }
		Self&  SetScratchBuffer (DeviceAddress addr)						__NE___	{ ASSERT( addr != Default );  scratch.emplace<DeviceAddress>( addr );				return *this; }

		// access: EResourceState::BuildRTAS_Read
		ND_ bool  SetGeometry (RTGeometryID id, INOUT WriteInstanceVk &)	__NE___;
		ND_ bool  SetGeometry (RTGeometryID id, INOUT UpdateInstanceVk &)	__NE___;
	};
//-----------------------------------------------------------------------------



	//
	// Ray Tracing Cluster Info
	//
	struct RTClusterInfo
	{
	// types

		// used when 'opType = BuildClustersGeometry'
		struct RTGeometryInput
		{
			uint				maxTotalClusterCount;
			uint				maxClusterPerAS;
		};

		// used when 'opType = BuildTriangleCluster or BuildTriangleClusterTemplate or InstantiateTriangleCluster'
		struct TriangleClusterInput
		{
			EVertexType			vertexFormat					= Default;
			uint				maxGeometryIndexValue;			// maximum geometry index value for any constructed geometry.
			uint				maxClusterUniqueGeometryCount;	// maximum number of unique values of the geometry index for each cluster or cluster template.
			uint				maxClusterTriangleCount;		// maximum number of triangles in a cluster or cluster template.
			uint				maxClusterVertexCount;			// maximum number of unique vertices in the cluster’s index buffer.
			uint				maxTotalTriangleCount;			// sum of all triangles across all clusters or cluster templates.
			uint				maxTotalVertexCount;			// maximum number of vertices across all clusters or cluster templates.
			uint				minPositionTruncateBitCount		= 32;
		};

		// used when 'opType = MoveObjects'
		struct MoveObjectsInput
		{
			ERTClusterType		type					= Default;
			bool				noMoveOverlap			= false;
			Bytes				maxMovedBytes;
		};

		using InputData = Union< NullUnion, RTGeometryInput, TriangleClusterInput, MoveObjectsInput >;


	// variables
		uint				maxRTASCount		= 0;
		ERTASOptions		options				= Default;

		ERTClusterOpType	opType				= Default;
		ERTClusterOpMode	opMode				= Default;
		InputData			opInput;


	// methods
		RTClusterInfo ()				__NE___ {}

		template <ERTClusterOpType Type>
		ND_ auto&  SetInputType ()		__NE___;
	};



	//
	// Ray Tracing Cluster Build command
	//
	struct RTClusterBuild : RTClusterInfo
	{
	// types
		using BufferWithOffset		= RTGeometryBuild::BufferWithOffset;

		struct BufferWithOffsetSizeStride : RTGeometryBuild::BufferWithOffsetAndStride
		{
			Bytes			size;
		};

		struct RTGeometryData
		{
			uint							clusterReferencesCount;
			uint							clusterReferencesStride;
			DeviceAddress					clusterReferences;
		};

		struct GeometryIndexAndGeometryFlags
		{
			uint							geometryIndex	: 24;
			uint							reserved		: 5;
			uint							geometryFlags	: 3;
		};

		struct TriangleClusterData
		{
			uint							clusterID;
			EClusterFlags					clusterFlags;
			uint							triangleCount						: 9;
			uint							vertexCount							: 9;
			uint							positionTruncateBitCount			: 6;
			EClusterIndexType				indexType							: 4;
			EClusterIndexType				opacityMicromapIndexType			: 4;		// not supported yet
			GeometryIndexAndGeometryFlags	baseGeometryIndexAndGeometryFlags;
			Bytes16u						indexBufferStride;
			Bytes16u						vertexBufferStride;
			Bytes16u						geometryIndexAndFlagsBufferStride;
			Bytes16u						opacityMicromapIndexBufferStride;				// not supported yet
			DeviceAddress					indexBuffer;
			DeviceAddress					vertexBuffer;
			DeviceAddress					geometryIndexAndFlagsBuffer;
			DeviceAddress					opacityMicromapArray;							// not supported yet
			DeviceAddress					opacityMicromapIndexBuffer;						// not supported yet
		};

		struct TriangleClusterTemplateData
		{
			uint							clusterID;
			EClusterFlags					clusterFlags;
			uint							triangleCount						: 9;
			uint							vertexCount							: 9;
			uint							positionTruncateBitCount			: 6;
			EClusterIndexType				indexType							: 4;
			EClusterIndexType				opacityMicromapIndexType			: 4;		// not supported yet
			GeometryIndexAndGeometryFlags	baseGeometryIndexAndGeometryFlags;
			Bytes16u						indexBufferStride;
			Bytes16u						vertexBufferStride;
			Bytes16u						geometryIndexAndFlagsBufferStride;
			Bytes16u						opacityMicromapIndexBufferStride;				// not supported yet
			DeviceAddress					indexBuffer;
			DeviceAddress					vertexBuffer;
			DeviceAddress					geometryIndexAndFlagsBuffer;
			DeviceAddress					opacityMicromapArray;							// not supported yet
			DeviceAddress					opacityMicromapIndexBuffer;						// not supported yet
			DeviceAddress					instantiationBoundingBoxLimit;
		};

		struct InstantiateClusterData
		{
			uint							clusterIdOffset;
			uint							geometryIndexOffset		: 24;
			uint							reserved				: 8;
			DeviceAddress					clusterTemplateAddress;
			DeviceAddress					vertexBuffer;
			Bytes							vertexBufferStride;
		};

		struct MoveObjectsData
		{
			DeviceAddress					srcAccelerationStructure;
		};

		static constexpr Bytes	DstAddressesArrayAlign			{8};
		static constexpr Bytes	DstSizesArrayAlign				{4};

		static constexpr Bytes	RTGeometryDataSize				{16};
		static constexpr Bytes	TriangleClusterDataSize			{64};
		static constexpr Bytes	TriangleClusterTemplateDataSize	{72};
		static constexpr Bytes	InstantiateClusterDataSize		{32};
		static constexpr Bytes	MoveObjectsDataSize				{8};

		StaticAssert( RTGeometryDataSize				== sizeof(RTGeometryData)				);
		StaticAssert( TriangleClusterDataSize			== sizeof(TriangleClusterData)			);
		StaticAssert( TriangleClusterTemplateDataSize	== sizeof(TriangleClusterTemplateData)	);
		StaticAssert( InstantiateClusterDataSize		== sizeof(InstantiateClusterData)		);
		StaticAssert( MoveObjectsDataSize				== sizeof(MoveObjectsData)				);
		StaticAssert( sizeof(GeometryIndexAndGeometryFlags) == 4 );


	// variables
		ERTClusterAddressResolution	addressResolution	= Default;

		BufferWithOffset			scratch;				// requires EBufferUsage::ASBuild_Scratch,	access: EResourceState::BuildRTAS_ScratchBuffer.

		BufferWithOffset			dstImplicitData;		// must not be null when 'opMode = ImplicitDst'.
															// if 'opType != MoveObjects' then size must be >= 'RTASBuildSizes::rtasSize'.
															// if 'opType == MoveObjects' then size must be equal to or larger than the sum of all the built acceleration structures that are being moved.
															// requires EBufferUsage::RTAS_Storage,  access: EResourceState::BuildRTAS_ReadWrite.

		BufferWithOffsetSizeStride	dstAddressesArray;		// must not be null when 'opMode = ImplicitDst or ExplicitDst',
															// contains 'DeviceAddress' ...
															// stride must be >= 8.
															// if 'opMode = ExplicitDst' addresses must be aligned based on the cluster acceleration structure type.
															// requires EBufferUsage::RTAS_Storage,  access: EResourceState::BuildRTAS_ReadWrite.

		BufferWithOffsetSizeStride	dstSizesArray;			// optional, contains 'uint' sizes of acceleration structures.
															// if 'opMode = ImplicitDst or ComputeSizes' then sizes are saved.
															// if 'opMode = ExplicitDst' then sizes are read from.
															// stride must be >= 4.
															// requires EBufferUsage::RTAS_Storage, access: EResourceState::BuildRTAS_ReadWrite.

		BufferWithOffsetSizeStride	srcInfosArray;			// if 'opType = MoveObjects' then must be array of 'MoveObjectsData'.
															// if 'opType = BuildClustersGeometry' then must be array of 'RTGeometryData'.
															// if 'opType = BuildTriangleCluster' then must be array of 'TriangleClusterData'.
															// if 'opType = BuildTriangleClusterTemplate' then must be array of 'TriangleClusterTemplateData'.
															// if 'opType = InstantiateTriangleCluster' then must be array of 'InstantiateClusterData'.
															// - array size must be >= 'srcInfosCount'.
															// - stride must be 0 or >= type of stucture.
															// requires EBufferUsage::ASBuild_ReadOnly,  access: EResourceState::BuildRTAS_IndirectBuffer.

		BufferWithOffset			srcInfosCount;			// requires EBufferUsage::ASBuild_ReadOnly,  access: EResourceState::BuildRTAS_IndirectBuffer,
															// value in 'srcInfosCount.buffer' must be <= 'maxRTASCount'.

	// methods
		RTClusterBuild ()		__NE___ {}
	};
//-----------------------------------------------------------------------------



	//
	// Ray Tracing Acceleration Structure Build Sizes
	//
	struct RTASBuildSizes
	{
		Bytes		rtasSize;
		Bytes		buildScratchSize;
		Bytes		updateScratchSize;
	};



	//
	// Ray Tracing Shader Binding Table description
	//
	struct RTShaderBindingDesc
	{
		uint	maxRayTypes		: 16;	// 'sbtRecordStride' has 4 bits, 'missIndex' has 16 bits
		uint	maxCallable		: 16;
		uint	maxInstances;			// 'instanceSBTOffset' has 24 bits

		RTShaderBindingDesc () __NE___ :
			maxRayTypes{0xFFFF}, maxCallable{0xFFFF}, maxInstances{UMax} {}
	};
//-----------------------------------------------------------------------------



/*
=================================================
	SetInputType
=================================================
*/
	template <ERTClusterOpType Type>
	auto&  RTClusterInfo::SetInputType () __NE___
	{
		if constexpr( Type == ERTClusterOpType::MoveObjects )
		{
			opType = Type;
			return opInput.emplace<MoveObjectsInput>();
		}
		else
		if constexpr( Type == ERTClusterOpType::BuildClustersGeometry )
		{
			opType = Type;
			return opInput.emplace<RTGeometryInput>();
		}
		else
		if constexpr( Type == ERTClusterOpType::BuildTriangleCluster		 or
					  Type == ERTClusterOpType::BuildTriangleClusterTemplate or
					  Type == ERTClusterOpType::InstantiateTriangleCluster )
		{
			opType = Type;
			return opInput.emplace<TriangleClusterInput>();
		}
	}

} // AE::Graphics


namespace AE::Base
{
	template <> struct TTriviallySerializable< Graphics::RTGeometryBuild::TrianglesInfo > : CT_True {};
	template <> struct TTriviallySerializable< Graphics::RTGeometryBuild::TrianglesData > : CT_True {};
	template <> struct TTriviallySerializable< Graphics::RTGeometryBuild::AABBsInfo		> : CT_True {};
	template <> struct TTriviallySerializable< Graphics::RTGeometryBuild::AABBsData		> : CT_True {};
	template <> struct TTriviallySerializable< Graphics::RTGeometryBuild::ScratchBuffer	> : CT_True {};

	template <> struct TTriviallySerializable< Graphics::RTSceneBuild::InstanceBuffer	> : CT_True {};
}
