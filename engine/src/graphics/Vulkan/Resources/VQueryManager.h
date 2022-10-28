// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Vulkan/VDevice.h"

namespace AE::Graphics
{
	enum class EQueryType : ubyte
	{
		Timestamp,
		PipelineStatistic,
		Performance,
		AccelStructCompactedSize,
		_Count,
		Unknown	= 0xFF,
	};



	//
	// Vulkan Query Manager
	//

	class VQueryManager
	{
	// types
	public:
		struct Query
		{
			VkQueryPool		pool		= Default;
			ushort			first		= UMax;
			ushort			count		= 0;
			ushort			numPasses	= 0;

			ND_ explicit operator bool () const	{ return pool != Default and count > 0; }
		};

		struct PipelineStatistic
		{
			//ulong	inputAssemblyPrimitives;	// VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT
			ulong	beforeClipping;				// VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT
			ulong	afterClipping;				// VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT
			ulong	fragShaderInvocations;		// VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT
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

			ND_ explicit operator bool () const	{ return handle != Default; }
		};

		using PoolArr_t = StaticArray< QueryPool, uint(EQueryType::_Count) >;

		static constexpr uint	TimestampQueryPerFrame			= 1000;
		static constexpr uint	PipelineStatQueryPerFrame		= 1000;
		static constexpr uint	PerformanceQueryPerFrame		= 1000;
		static constexpr uint	ASCompactedSizeQueryPerFrame	= 1000;


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
		VQueryManager ();
		~VQueryManager ();

		ND_ bool  Initialize (const VDevice &dev, uint maxFrames);
			void  Deinitialize (const VDevice &dev);

			void  NextFrame (const VDevice &dev, FrameUID frameId);

		ND_ bool  AcquireProfilingLock (const VDevice &dev);
			bool  ReleaseProfilingLock (const VDevice &dev);

		ND_ Query  AllocQuery (EQueueType queue, EQueryType type, uint count = 1) const;

		ND_ bool  SupportsCalibratedTimestamps () const		{ DRC_SHAREDLOCK( _drCheck );  return _calibratedTs; }

			VkResult  GetTimestamp (const VDevice &dev, const Query &q, OUT ulong* result, Bytes size) const;	// raw
			VkResult  GetTimestamp (const VDevice &dev, const Query &q, OUT double* result, Bytes size) const;	// nanoseconds
			VkResult  GetTimestamp (const VDevice &dev, const Query &q, OUT nanosecondsd* result, Bytes size) const;
			VkResult  GetTimestampCalibrated (const VDevice &dev, const Query &q, OUT ulong* result, OUT ulong* maxDeviation, Bytes size) const;
			VkResult  GetPerformanceCounter (const VDevice &dev, const Query &q, OUT VkPerformanceCounterResultKHR* result, Bytes size) const;
			VkResult  GetPipelineStatistic (const VDevice &dev, const Query &q, OUT PipelineStatistic* result, Bytes size) const;
			VkResult  GetRTASCompactedSize (const VDevice &dev, const Query &q, OUT Bytes64u* result, Bytes size) const;

	private:
		static void  _ResetPoolOnHost (const VDevice &dev, uint idx, QueryPool &pool);
		static void  _ResetPool (const VDevice &dev, VkCommandBuffer cmdbuf, uint idx, QueryPool &pool);
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
