// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/QueryManager.h"
# include "graphics/Vulkan/VDevice.h"

namespace AE::Graphics
{

	//
	// Vulkan Query Manager
	//

	class VQueryManager final : public IQueryManager
	{
	// types
	public:
		struct Query
		{
			VkQueryPool		pool		= Default;
			ushort			first		= UMax;
			ushort			count		= 0;
			ushort			numPasses	= 0;
			EQueryType		type		= Default;

			ND_ explicit operator bool ()	C_NE___	{ return pool != Default and count > 0; }
		};

		struct PipelineStatistic
		{
			//ulong	inputAssemblyPrimitives;	// VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT
			ulong	beforeClipping;				// VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT
			ulong	afterClipping;				// VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT
			ulong	fragShaderInvocations;		// VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT
			// VK_QUERY_PIPELINE_STATISTIC_TASK_SHADER_INVOCATIONS_BIT_EXT 
			// VK_QUERY_PIPELINE_STATISTIC_MESH_SHADER_INVOCATIONS_BIT_EXT
			// VK_QUERY_TYPE_MESH_PRIMITIVES_GENERATED_EXT
		};

	private:
		using PerFrameCount_t = StaticArray< uint, GraphicsConfig::MaxFrames+1 >;

		struct QueryPool
		{
			mutable Atomic<uint>	count		{0};
			VkQueryPool				handle		= Default;
			uint					maxCount	= 0;		// per frame
			uint					numPasses	= 1;
			PerFrameCount_t			countArr	{};

			ND_ explicit operator bool ()	C_NE___	{ return handle != Default; }
		};

		using PoolArr_t = StaticArray< QueryPool, uint(EQueryType::_Count) >;


	// variables
	private:
		PoolArr_t		_poolArr			{};
		EQueueMask		_timestampAllowed	= Default;
		float			_timestampPeriod	= 1.f;	// nanoseconds

		uint			_prevIndex		: 8;
		uint			_index			: 8;
		uint			_maxCount		: 8;

		uint			_hostReset		: 1;
		uint			_perfQuery		: 1;
		uint			_calibratedTs	: 1;

		Atomic<bool>	_perfLockAcquired	{false};

		DRC_ONLY(
			RWDataRaceCheck	_drCheck;
		)


	// methods
	public:
		VQueryManager ()																													__NE___;
		~VQueryManager ()																													__NE___;

		ND_ bool  Initialize (const VDevice &dev, uint maxFrames)																			__NE___;
			void  Deinitialize (const VDevice &dev)																							__NE___;

			void  NextFrame (const VDevice &dev, FrameUID frameId)																			__NE___;

		ND_ bool  AcquireProfilingLock (const VDevice &dev)																					__NE___;
			bool  ReleaseProfilingLock (const VDevice &dev)																					__NE___;

		ND_ Query  AllocQuery (EQueueType queue, EQueryType type, uint count = 1)															C_NE___;

		ND_ bool  SupportsCalibratedTimestamps ()																							C_NE___		{ DRC_SHAREDLOCK( _drCheck );  return _calibratedTs; }

			VkResult  GetTimestamp (const VDevice &dev, const Query &q, OUT ulong* result, Bytes size)										C_NE___;	// raw
			VkResult  GetTimestamp (const VDevice &dev, const Query &q, OUT double* result, Bytes size)										C_NE___;	// nanoseconds
			VkResult  GetTimestamp (const VDevice &dev, const Query &q, OUT nanosecondsd* result, Bytes size)								C_NE___;
			VkResult  GetTimestampCalibrated (const VDevice &dev, const Query &q, OUT ulong* result, OUT ulong* maxDeviation, Bytes size)	C_NE___;
			VkResult  GetPerformanceCounter (const VDevice &dev, const Query &q, OUT VkPerformanceCounterResultKHR* result, Bytes size)		C_NE___;
			VkResult  GetPipelineStatistic (const VDevice &dev, const Query &q, OUT PipelineStatistic* result, Bytes size)					C_NE___;
			VkResult  GetRTASProperty (const VDevice &dev, const Query &q, OUT Bytes64u* result, Bytes size)								C_NE___;

	private:
		static void  _ResetPoolOnHost (const VDevice &dev, uint idx, QueryPool &pool)														__NE___;
		static void  _ResetPool (const VDevice &dev, VkCommandBuffer cmdbuf, uint idx, QueryPool &pool)										__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
