// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
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
//-----------------------------------------------------------------------------
