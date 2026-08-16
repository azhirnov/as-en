// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Indirect command types.

	Requires AEStyle preprocessor.
*/

#ifdef __cplusplus
# pragma once
#endif

#ifndef DispatchIndirectCommand_defined
struct DispatchIndirectCommand
{
	uint	groupCountX;
	uint	groupCountY;
	uint	groupCountZ;
};
#endif
#define DispatchIndirectCommand_SizeOf  12

ND_ DispatchIndirectCommand  DispatchIndirectCommand_Create (uint groupCountX, uint groupCountY, uint groupCountZ)
{
	DispatchIndirectCommand	result;
	result.groupCountX		= groupCountX;
	result.groupCountY		= groupCountY;
	result.groupCountZ		= groupCountZ;
	return result;
}

ND_ DispatchIndirectCommand  DispatchIndirectCommand_Create (uint3 groupCount)
{
	DispatchIndirectCommand	result;
	result.groupCountX		= groupCount.x;
	result.groupCountY		= groupCount.y;
	result.groupCountZ		= groupCount.z;
	return result;
}
//-----------------------------------------------------------------------------


#ifndef DrawIndirectCommand_defined
struct DrawIndirectCommand
{
	uint	vertexCount;
	uint	instanceCount;
	uint	firstVertex;
	uint	firstInstance;
};
#endif
#define DrawIndirectCommand_SizeOf  16

ND_ DrawIndirectCommand  DrawIndirectCommand_Create (uint	vertexCount,
													 uint	instanceCount,
													 uint	firstVertex,
													 uint	firstInstance) {
	DrawIndirectCommand		result;
	result.vertexCount		= vertexCount;
	result.instanceCount	= instanceCount;
	result.firstVertex		= firstVertex;
	result.firstInstance	= firstInstance;
	return result;
}

ND_ DrawIndirectCommand  DrawIndirectCommand_Create (uint	vertexCount) {
	return DrawIndirectCommand_Create( vertexCount, 1, 0, 0 );
}
//-----------------------------------------------------------------------------


#ifndef DrawIndexedIndirectCommand_defined
struct DrawIndexedIndirectCommand
{
	uint	indexCount;
	uint	instanceCount;
	uint	firstIndex;
	int		vertexOffset;
	uint	firstInstance;
};
#endif
#define DrawIndexedIndirectCommand_SizeOf  20

ND_ DrawIndexedIndirectCommand  DrawIndexedIndirectCommand_Create (uint	indexCount,
																   uint	instanceCount,
																   uint	firstIndex,
																   int	vertexOffset,
																   uint	firstInstance) {
	DrawIndexedIndirectCommand	result;
	result.indexCount			= indexCount;
	result.instanceCount		= instanceCount;
	result.firstIndex			= firstIndex;
	result.vertexOffset			= vertexOffset;
	result.firstInstance		= firstInstance;
	return result;
}

ND_ DrawIndexedIndirectCommand  DrawIndexedIndirectCommand_Create (uint	indexCount) {
	return DrawIndexedIndirectCommand_Create( indexCount, 1, 0, 0, 0 );
}
//-----------------------------------------------------------------------------


#ifndef DrawMeshTasksIndirectCommand_defined
struct DrawMeshTasksIndirectCommand
{
	uint	taskCountX;
	uint	taskCountY;
	uint	taskCountZ;
};
#endif
#define DrawMeshTasksIndirectCommand_SizeOf  12

ND_ DrawMeshTasksIndirectCommand  DrawMeshTasksIndirectCommand_Create (uint taskCountX, uint taskCountY, uint taskCountZ)
{
	DrawMeshTasksIndirectCommand	result;
	result.taskCountX	= taskCountX;
	result.taskCountY	= taskCountY;
	result.taskCountZ	= taskCountZ;
	return result;
}

ND_ DrawMeshTasksIndirectCommand  DrawMeshTasksIndirectCommand_Create (const uint3 taskCount)
{
	DrawMeshTasksIndirectCommand	result;
	result.taskCountX	= taskCount.x;
	result.taskCountY	= taskCount.y;
	result.taskCountZ	= taskCount.z;
	return result;
}
//-----------------------------------------------------------------------------


#ifndef TraceRayIndirectCommand_defined
struct TraceRayIndirectCommand
{
	uint	width;
	uint	height;
	uint	depth;
};
#endif
#define TraceRayIndirectCommand_SizeOf  12

ND_ TraceRayIndirectCommand  TraceRayIndirectCommand_Create (uint width, uint height, uint depth)
{
	TraceRayIndirectCommand	result;
	result.width	= width;
	result.height	= height;
	result.depth	= depth;
	return result;
}

ND_ TraceRayIndirectCommand  TraceRayIndirectCommand_Create (const uint3 dim)
{
	TraceRayIndirectCommand	result;
	result.width	= dim.x;
	result.height	= dim.y;
	result.depth	= dim.z;
	return result;
}
//-----------------------------------------------------------------------------


#ifdef AE_ray_query
	#ifndef TraceRayIndirectCommand2_defined
	struct TraceRayIndirectCommand2
	{
		gl::DeviceAddress	raygenShaderRecordAddress;				// non-null
		ulong				raygenShaderRecordSize;					// == shaderGroupHandleSize

		gl::DeviceAddress	missShaderBindingTableAddress;			// optional
		ulong				missShaderBindingTableSize;				// multiple of 'missShaderBindingTableStride'
		ulong				missShaderBindingTableStride;			// >= shaderGroupHandleSize

		gl::DeviceAddress	hitShaderBindingTableAddress;			// optional
		ulong				hitShaderBindingTableSize;				// multiple of 'hitShaderBindingTableStride'
		ulong				hitShaderBindingTableStride;			// >= shaderGroupHandleSize

		gl::DeviceAddress	callableShaderBindingTableAddress;		// optional
		ulong				callableShaderBindingTableSize;			// multiple of 'callableShaderBindingTableStride'
		ulong				callableShaderBindingTableStride;		// >= shaderGroupHandleSize

		uint				width;									// >= 1
		uint				height;									// >= 1
		uint				depth;									// >= 1
	};
	#endif
	/*
	ND_ TraceRayIndirectCommand2  TraceRayIndirectCommand2_Create ()
	{
		TraceRayIndirectCommand2	result;
		result.raygenShaderRecordAddress			= 0;
		result.raygenShaderRecordSize				= 0;
		result.missShaderBindingTableAddress		= 0;
		result.missShaderBindingTableSize			= 0;
		result.missShaderBindingTableStride			= 0;
		result.hitShaderBindingTableAddress			= 0;
		result.hitShaderBindingTableSize			= 0;
		result.hitShaderBindingTableStride			= 0;
		result.callableShaderBindingTableAddress	= 0;
		result.callableShaderBindingTableSize		= 0;
		result.callableShaderBindingTableStride		= 0;
		result.width								= 0;
		result.height								= 0;
		result.depth								= 1;
		return result;
	}*/
#endif
#define TraceRayIndirectCommand2_SizeOf  104
//-----------------------------------------------------------------------------


#ifndef ASBuildIndirectCommand_defined
struct ASBuildIndirectCommand
{
	// Triangles count, AABBs count, Instances count
	uint		primitiveCount;

	uint		primitiveOffset;
	uint		firstVertex;
	uint		transformOffset;
};
#endif
#define ASBuildIndirectCommand_SizeOf  16

ND_ ASBuildIndirectCommand  ASBuildIndirectCommand_Create (uint primitiveCount, uint primitiveOffset, uint firstVertex, uint transformOffset)
{
	ASBuildIndirectCommand	result;
	result.primitiveCount	= primitiveCount;
	result.primitiveOffset	= primitiveOffset;
	result.firstVertex		= firstVertex;
	result.transformOffset	= transformOffset;
	return result;
}

ND_ ASBuildIndirectCommand  ASBuildIndirectCommand_Create (uint primitiveCount)
{
	ASBuildIndirectCommand	result;
	result.primitiveCount	= primitiveCount;
	result.primitiveOffset	= 0;
	result.firstVertex		= 0;
	result.transformOffset	= 0;
	return result;
}
//-----------------------------------------------------------------------------


// VkIndexType
#define VK_INDEX_TYPE_UINT16	0
#define VK_INDEX_TYPE_UINT32	1
//#define VK_INDEX_TYPE_UINT8		1000265000	// TODO: add to feature set

#ifndef BindIndexBufferIndirectCommand_defined
struct BindIndexBufferIndirectCommand
{
	gl::DeviceAddress	bufferAddress;		// 'EResourceState::IndexBuffer', must be aligned to index size
	uint				bufferSize;			// index buffer size
	uint				indexType;			// 'VkIndexType'
};
#endif
#define BindIndexBufferIndirectCommand_SizeOf  16

ND_ BindIndexBufferIndirectCommand  BindIndexBufferIndirectCommand_Create (gl::DeviceAddress bufferAddress, uint bufferSize, uint indexType)
{
	BindIndexBufferIndirectCommand	result;
	result.bufferAddress	= bufferAddress;
	result.bufferSize		= bufferSize;
	result.indexType		= indexType;
	return result;
}
//-----------------------------------------------------------------------------


#ifndef BindVertexBufferIndirectCommand_defined
struct BindVertexBufferIndirectCommand
{
	gl::DeviceAddress	bufferAddress;		// 'EResourceState::VertexBuffer'
	uint				bufferSize;			// vertex buffer size
	uint				stride;				// vertex size with padding
};
#endif
#define BindVertexBufferIndirectCommand_SizeOf  16

ND_ BindVertexBufferIndirectCommand  BindVertexBufferIndirectCommand_Create (gl::DeviceAddress bufferAddress, uint bufferSize, uint stride)
{
	BindVertexBufferIndirectCommand		result;
	result.bufferAddress	= bufferAddress;
	result.bufferSize		= bufferSize;
	result.stride			= stride;
	return result;
}
//-----------------------------------------------------------------------------


#ifndef DrawIndirectCountIndirectCommand_defined
struct DrawIndirectCountIndirectCommand
{
	// buffer layout:
	//	[uint]   [uint]   [uint]  -- drawCmdCount [commandCount]
	//	| stride | stride |

	gl::DeviceAddress	bufferAddress;		// 'EResourceState::IndirectBuffer'
	uint				stride;				// stride for the command arguments
	uint				commandCount;		// number of commands to execute
};
#endif
#define DrawIndirectCountIndirectCommand_SizeOf  16

ND_ DrawIndirectCountIndirectCommand  DrawIndirectCountIndirectCommand_Create (gl::DeviceAddress bufferAddress, uint stride, uint commandCount)
{
	DrawIndirectCountIndirectCommand	result;
	result.bufferAddress	= bufferAddress;
	result.stride			= stride;
	result.commandCount		= commandCount;
	return result;
}
//-----------------------------------------------------------------------------


#ifdef AE_opacity_micromap
	#ifdef __cplusplus
		#define AE_opacity_micromap_maxOpacity2StateSubdivisionLevel	12
		#define AE_opacity_micromap_maxOpacity4StateSubdivisionLevel	12

		enum class EOpacityMicromapFormat
		{
			TwoState,
			FourState,
		};

		enum class EOpacityMicromapSpecialIndex : int
		{
			FullyTransparent,						// entire triangle is fully transparent
			FullyOpaque,							// entire triangle is fully opaque
			FullyUnknownTransparent,				// ???
			FullyUnknownOpaque,						// ???
			ClusterGeometryDisableOpacityMicromap,	// opacity value will be picked from 'baseGeometryIndexAndGeometryFlags'
		};

	#else
		#define EOpacityMicromapFormat				uint
		#define EOpacityMicromapFormat_TwoState		(1)
		#define EOpacityMicromapFormat_FourState	(2)

		#define EOpacityMicromapSpecialIndex										int
		#define EOpacityMicromapSpecialIndex_FullyTransparent						(-1)
		#define EOpacityMicromapSpecialIndex_FullyOpaque							(-2)
		#define EOpacityMicromapSpecialIndex_FullyUnknownTransparent				(-3)
		#define EOpacityMicromapSpecialIndex_FullyUnknownOpaque						(-4)
		#define EOpacityMicromapSpecialIndex_ClusterGeometryDisableOpacityMicromap	(-5)
	#endif

	#ifndef MicromapTriangle_defined
	struct MicromapTriangle
	{
		uint	dataOffset;
		uint	subdivisionLevel_format;	// 2x ushort
	};
	#endif
	#define MicromapTriangle_SizeOf		8

	ND_ MicromapTriangle  MicromapTriangle_Create (uint dataOffset, uint subdivisionLevel, EOpacityMicromapFormat format)
	{
		MicromapTriangle	result;
		result.dataOffset				= dataOffset;
		result.subdivisionLevel_format	= (subdivisionLevel & 0xFFFF) | (format << 16);
		return result;
	}
#endif
//-----------------------------------------------------------------------------
