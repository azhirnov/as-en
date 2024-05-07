// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// Query type
	//
	enum class EQueryType : ubyte
	{
		Timestamp,
		PipelineStatistic,
		Performance,
		AccelStructCompactedSize,
		AccelStructSize,				// require 'VK_KHR_ray_tracing_maintenance1'	// TODO: add to FeatureSet ?
		AccelStructSerializationSize,
		_Count,
		Unknown	= 0xFF,
	};



	//
	// Query Manager interface
	//

	class NO_VTABLE IQueryManager
	{
	// types
	public:
		struct GraphicsPipelineStatistic
		{
			//ulong	inputAssemblyPrimitives;	// VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT
			ulong	beforeClipping;				// VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT
			ulong	afterClipping;				// VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT
			ulong	fragShaderInvocations;		// VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT
			// VK_QUERY_PIPELINE_STATISTIC_TASK_SHADER_INVOCATIONS_BIT_EXT	- meshShaderQueries
			// VK_QUERY_PIPELINE_STATISTIC_MESH_SHADER_INVOCATIONS_BIT_EXT	- meshShaderQueries
			// VK_QUERY_TYPE_MESH_PRIMITIVES_GENERATED_EXT					- meshShaderQueries
		};

		struct IQuery
		{};

	protected:
		static constexpr uint	_FrameHistorySize = GraphicsConfig::MaxFrames+1;

		struct PackedIdx
		{
			uint	writeIndex	: 8;
			uint	readIndex	: 8;

			PackedIdx ()				__NE___	: writeIndex{0}, readIndex{0} {}
			PackedIdx (uint w, uint r)	__NE___	: writeIndex{w}, readIndex{r} {}
		};


	// variables
	protected:
		StructAtomic<PackedIdx>		_packedIdx;
		uint						_maxFrames		: 8;


	// methods
	protected:
		ND_ bool	_Initialize (uint maxFrames)	__NE___;
		ND_ auto	_NextFrame (FrameUID)			__NE___ -> Tuple<uint, PackedIdx>;

	public:
		ND_ uint	WriteIndex (FrameUID)			C_NE___;
		ND_ uint	ReadIndex (FrameUID)			C_NE___;

		ND_ uint	WriteIndex ()					C_NE___	{ return _packedIdx.load().writeIndex; }
		ND_ uint	ReadIndex ()					C_NE___	{ return _packedIdx.load().readIndex; }
		ND_ uint2	ReadAndWriteIndices ()			C_NE___	{ auto tmp = _packedIdx.load();  return uint2{tmp.readIndex, tmp.writeIndex}; }

		virtual	bool  GetTimestamp (const IQuery &, OUT ulong* result, Bytes size)										C_NE___ = 0;	// raw
		virtual	bool  GetTimestamp (const IQuery &, OUT double* result, Bytes size)										C_NE___ = 0;	// nanoseconds in GPU-space
		virtual	bool  GetTimestamp (const IQuery &, OUT nanosecondsd* result, Bytes size)								C_NE___ = 0;	// nanoseconds in GPU-space

		virtual	bool  GetTimestampCalibrated (const IQuery &, OUT ulong* result, OUT ulong* maxDeviation, Bytes size)				C_NE___ = 0;	// nanoseconds in CPU-space
		virtual	bool  GetTimestampCalibrated (const IQuery &, OUT double* result, OUT double* maxDeviation, Bytes size)				C_NE___ = 0;	// nanoseconds in CPU-space
		virtual	bool  GetTimestampCalibrated (const IQuery &, OUT nanosecondsd* result, OUT nanosecondsd* maxDeviation, Bytes size)	C_NE___ = 0;	// nanoseconds in CPU-space

		virtual	bool  GetPipelineStatistic (const IQuery &, OUT GraphicsPipelineStatistic* result, Bytes size)			C_NE___ = 0;
	};


} // AE::Graphics
