// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/MatrixStorage.h"
#include "base/Containers/TupleArrayView.h"

#include "graphics/Public/IDs.h"
#include "graphics/Public/RayTracingEnums.h"
#include "graphics/Public/VertexDesc.h"
#include "graphics/Public/VulkanTypes.h"

namespace AE::Graphics
{
	//      rotation        translation
	// [0,0]  [0,1]  [0,2]    [0,3]
	// [1,0]  [1,1]  [1,2]    [1,3]
	// [2,0]  [2,1]  [2,2]    [2,3]
	using RTMatrixStorage = MatrixStorage< float, 3, 4, EMatrixOrder::ColumnMajor, 4 >;


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
			BufferID		vertexData;			// requires EBufferUsage::ASBuild_ReadOnly, content: 'vertexFormat'
			BufferID		indexData;			// requires EBufferUsage::ASBuild_ReadOnly, content: 'indexType'
			BufferID		transformData;		// requires EBufferUsage::ASBuild_ReadOnly,	content: RTMatrixStorage
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
			BufferID		data;				// requires EBufferUsage::ASBuild_ReadOnly
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
		ScratchBuffer	scratch;


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

		ND_ bool  operator == (const RTGeometryDesc &rhs)	__NE___	{ return (size == rhs.size) & (options == rhs.options); }
		ND_ bool  IsExclusiveSharing ()						C_NE___	{ return false; }
	};

	

	//
	// Ray Tracing Scene Build command
	//
	struct RTSceneBuild
	{
	// types
		#if defined(AE_ENABLE_VULKAN)
			// VkAccelerationStructureInstanceKHR
			struct Instance
			{
				RTMatrixStorage		transform;
				uint				instanceCustomIndex	: 24;
				uint				mask				:  8;
				uint				instanceSBTOffset	: 24;
				uint				flags				:  8;	// ERTInstanceOpt | VkGeometryInstanceFlags
				VDeviceAddress		rtas;

				Instance ()												__NE___;
				Instance&  SetIdentity ()								__NE___	{ transform			= RTMatrixStorage::Identity();	return *this; }
				Instance&  SetTransform (const RTMatrixStorage &value)	__NE___	{ transform			= value;		return *this; }
				Instance&  SetTransform (const float3x4 &value)			__NE___	{ transform			= value;		return *this; }
				Instance&  SetMask (uint value)							__NE___	{ mask				= value;		return *this; }
				Instance&  SetInstanceOffset (uint value)				__NE___	{ instanceSBTOffset	= value;		return *this; }
				Instance&  SetFlags (ERTInstanceOpt value)				__NE___;
			};

		#elif defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
			// MTLAccelerationStructureInstanceDescriptor
			struct Instance
			{
				RTMatrixStorage		transform;
				uint				options;					// ERTInstanceOpt | MTLAccelerationStructureInstanceOptions
				uint				mask;
				uint				instanceSBTOffset;
				uint				rtasIndex;
				
				Instance ()												__NE___;
				Instance&  SetIdentity ()								__NE___	{ transform			= RTMatrixStorage::Identity();	return *this; }
				Instance&  SetTransform (const RTMatrixStorage &value)	__NE___	{ transform			= value;		return *this; }
				Instance&  SetTransform (const float3x4 &value)			__NE___	{ transform			= value;		return *this; }
				Instance&  SetMask (uint value)							__NE___	{ mask				= value;		return *this; }
				Instance&  SetInstanceOffset (uint value)				__NE___	{ instanceSBTOffset	= value;		return *this; }
				Instance&  SetFlags (ERTInstanceOpt value)				__NE___;
			};
			
		#else
		#	error not implemented
		#endif
		STATIC_ASSERT( sizeof(Instance) == 64 );

		// TODO: MTLAccelerationStructureUserIDInstanceDescriptor, MTLAccelerationStructureMotionInstanceDescriptor

		using ScratchBuffer		= RTGeometryBuild::BufferWithOffset;
		using InstanceBuffer	= RTGeometryBuild::BufferWithOffsetAndStride;
		using GeomArray_t		= Array< RTGeometryID >;
		using GeomMap_t			= FlatHashMap< RTGeometryID, uint >;


	// variables
		uint				maxInstanceCount	= 0;
		ERTASOptions		options				= Default;
		ScratchBuffer		scratch;
		InstanceBuffer		instanceData;
		
	  #if defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
		GeomArray_t			geomArray;
		GeomMap_t			geomMap;
	  #endif


	// methods
		RTSceneBuild ()								__NE___	{}
		RTSceneBuild (uint count, ERTASOptions opt)	__NE___ : maxInstanceCount{count}, options{opt} {}

		RTSceneBuild&  SetScratchBuffer (BufferID id, Bytes offset = 0_b) __NE___
		{
			scratch.id		= id;
			scratch.offset	= offset;
			return *this;
		}

		RTSceneBuild&  SetInstanceData (BufferID id, Bytes offset = 0_b, Bytes stride = SizeOf<Instance>) __NE___
		{
			instanceData.id		= id;
			instanceData.offset	= offset;
			instanceData.stride	= stride;
			return *this;
		}

		ND_ bool  SetGeometry (RTGeometryID id, INOUT Instance &inst) __NE___;
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
		
		ND_ bool  operator == (const RTSceneDesc &rhs)	__NE___	{ return (size == rhs.size) & (options == rhs.options); }
		ND_ bool  IsExclusiveSharing ()					C_NE___	{ return false; }
	};



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
		uint	maxMissShaders : 16;
		uint	hitGroupStride : 16;	// in shader only 4 bits are used
		uint	maxInstances;			// 'instanceSBTOffset' has 24 bits
		// TODO: max callable

		RTShaderBindingDesc () __NE___ :
			maxMissShaders{0xFFFF}, hitGroupStride{0xFFFF}, maxInstances{UMax} {}
	};
	

} // AE::Graphics
