// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/RayTracingEnums.h"
#include "graphics/Public/VertexDesc.h"
#include "graphics/Public/BufferDesc.h"

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



	//
	// Ray Tracing Scene Build command
	//
	struct RTSceneBuild
	{
	// types
		// VkAccelerationStructureInstanceKHR
		struct InstanceVk
		{
			RTMatrixStorage		transform;
			uint				instanceCustomIndex	: 24;
			uint				mask				:  8;
			uint				instanceSBTOffset	: 24;
			uint				flags				:  8;	// ERTInstanceOpt | VkGeometryInstanceFlags
			DeviceAddress		rtas;						// - if 0 then instance is inactive.

			InstanceVk ()												__NE___ = default;
			InstanceVk&  Init ()										__NE___;
			InstanceVk&  SetIdentity ()									__NE___	{ transform			= RTMatrixStorage::Identity();	return *this; }
			InstanceVk&  SetTransform (const RTMatrixStorage &value)	__NE___	{ transform			= value;		return *this; }
			InstanceVk&  SetTransform (const float4x3 &value)			__NE___	{ transform			= value;		return *this; }
			InstanceVk&  SetMask (uint value)							__NE___	{ mask				= value;		ASSERT( mask == value or value == UMax );	return *this; }
			InstanceVk&  SetInstanceOffset (uint value)					__NE___	{ instanceSBTOffset	= value;		ASSERT( instanceSBTOffset == value );		return *this; }
			InstanceVk&  SetInstanceCustomIndex (uint value)			__NE___	{ instanceCustomIndex = value;		ASSERT( instanceCustomIndex == value );		return *this; }
			InstanceVk&  SetFlags (ERTInstanceOpt value)				__NE___;
		};

		// MTLAccelerationStructureInstanceDescriptor
		struct InstanceMtl
		{
			RTMatrixStorage		transform;
			uint				options;					// ERTInstanceOpt | MTLAccelerationStructureInstanceOptions
			uint				mask;
			uint				instanceSBTOffset;
			uint				rtasIndex;

			InstanceMtl ()												__NE___ = default;
			InstanceMtl&  Init ()										__NE___;
			InstanceMtl&  SetIdentity ()								__NE___	{ transform			= RTMatrixStorage::Identity();	return *this; }
			InstanceMtl&  SetTransform (const RTMatrixStorage &value)	__NE___	{ transform			= value;		return *this; }
			InstanceMtl&  SetTransform (const float4x3 &value)			__NE___	{ transform			= value;		return *this; }
			InstanceMtl&  SetMask (uint value)							__NE___	{ mask				= value;		return *this; }
			InstanceMtl&  SetInstanceOffset (uint value)				__NE___	{ instanceSBTOffset	= value;		return *this; }
			InstanceMtl&  SetInstanceCustomIndex (uint)					__NE___	{ DBG_WARNING( "not supported" );	return *this; }
			InstanceMtl&  SetFlags (ERTInstanceOpt value)				__NE___;
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
		ScratchBuffer		scratch;
		InstanceBuffer		instanceData;
		GeometrySet_t		uniqueGeoms;


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
