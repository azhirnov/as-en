// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Indirect command types.

	Requires AEStyle preprocessor.
*/

#ifdef __cplusplus
# pragma once
#endif

struct DispatchIndirectCommand
{
	uint	groupCountX;
	uint	groupCountY;
	uint	groupCountZ;
};

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


struct DrawIndirectCommand
{
	uint	vertexCount;
	uint	instanceCount;
	uint	firstVertex;
	uint	firstInstance;
};

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
//-----------------------------------------------------------------------------


struct DrawIndexedIndirectCommand
{
	uint	indexCount;
	uint	instanceCount;
	uint	firstIndex;
	int		vertexOffset;
	uint	firstInstance;
};

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
//-----------------------------------------------------------------------------


struct DrawMeshTasksIndirectCommand
{
	uint	taskCountX;
	uint	taskCountY;
	uint	taskCountZ;
};

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


struct TraceRayIndirectCommand
{
	uint	width;
	uint	height;
	uint	depth;
};

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


struct TraceRayIndirectCommand2
{
	gl::DeviceAddress	raygenShaderRecordAddress;
	ulong				raygenShaderRecordSize;
	gl::DeviceAddress	missShaderBindingTableAddress;
	ulong				missShaderBindingTableSize;
	ulong				missShaderBindingTableStride;
	gl::DeviceAddress	hitShaderBindingTableAddress;
	ulong				hitShaderBindingTableSize;
	ulong				hitShaderBindingTableStride;
	gl::DeviceAddress	callableShaderBindingTableAddress;
	ulong				callableShaderBindingTableSize;
	ulong				callableShaderBindingTableStride;
	uint				width;
	uint				height;
	uint				depth;
};
//-----------------------------------------------------------------------------
