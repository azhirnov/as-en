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
		struct Query : IQuery
		{
			VkQueryPool		pool		= Default;
			ushort			first		= UMax;
			ushort			count		= 0;
			ushort			numPasses	= 0;
			EQueryType		type		= Default;
			EQueueType		queue		= Default;

			ND_ explicit operator bool ()	C_NE___	{ return pool != Default and count > 0; }
		};

	private:
		using PerFrameCount_t = StaticArray< uint, _FrameHistorySize >;

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

		struct Result64
		{
			ulong	result;
			ulong	available;

			ND_ bool  IsAvailable ()	C_NE___	{ return available != 0; }
		};

		struct PipelineStatisticResult : GraphicsPipelineStatistic
		{
			ulong	available;

			ND_ bool  IsAvailable ()	C_NE___	{ return available != 0; }
		};

		using TimestampBitsPerQueue_t = StaticArray< uint, uint(EQueueType::_Count) >;


	// variables
	private:
		VDevice const&				_device;

		PoolArr_t					_poolArr			{};
		EQueueMask					_timestampAllowed	= Default;
		nanosecondsf				_timestampPeriod	{1.f};

		TimestampBitsPerQueue_t		_tsBits;

		uint						_hostReset		: 1;
		uint						_perfQuery		: 1;
		uint						_calibratedTs	: 1;

		Atomic<bool>				_perfLockAcquired	{false};

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		VQueryManager ()																							__NE___;
		~VQueryManager ()																							__NE___;

		ND_ bool  Initialize (uint maxFrames)																		__NE___;
			void  Deinitialize ()																					__NE___;

			void  NextFrame (FrameUID frameId)																		__NE___;
			void  ResetQueries (VkCommandBuffer cmdbuf, FrameUID frameId)											__NE___;

		ND_ bool  AcquireProfilingLock ()																			__NE___;
			bool  ReleaseProfilingLock ()																			__NE___;

		ND_ Query  AllocQuery (EQueueType queue, EQueryType type, uint count = 1)									C_NE___;
		ND_ Query  AllocQuery (FrameUID frameId, EQueueType queue, EQueryType type, uint count = 1)					C_NE___;

		ND_ bool  SupportsCalibratedTimestamps ()																	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _calibratedTs; }

			bool  GetTimestamp (const IQuery &, OUT ulong* result, Bytes resultSize)								C_NE_OV;	// raw
			bool  GetTimestamp (const IQuery &, OUT double* result, Bytes resultSize)								C_NE_OV;	// nanoseconds in GPU-space
			bool  GetTimestamp (const IQuery &, OUT nanosecondsd* result, Bytes resultSize)							C_NE_OV;	// nanoseconds in GPU-space

			bool  GetTimestampCalibrated (const IQuery &, OUT ulong* result, OUT ulong* maxDeviation, Bytes resultSize)					C_NE_OV;	// nanoseconds in CPU-space
			bool  GetTimestampCalibrated (const IQuery &, OUT double* result, OUT double* maxDeviation, Bytes resultSize)				C_NE_OV;	// nanoseconds in CPU-space
			bool  GetTimestampCalibrated (const IQuery &, OUT nanosecondsd* result, OUT nanosecondsd* maxDeviation, Bytes resultSize)	C_NE_OV;	// nanoseconds in CPU-space

		//	bool  GetPerformanceCounter (const Query &q, OUT VkPerformanceCounterResultKHR* result, Bytes resultSize)	C_NE___;
			bool  GetPipelineStatistic (const IQuery &, OUT GraphicsPipelineStatistic* result, Bytes resultSize)		C_NE_OV;
			bool  GetRTASProperty (const Query &q, OUT Bytes64u* result, Bytes resultSize)								C_NE___;

	private:
		static void  _ResetPoolOnHost (const VDevice &, uint idx, QueryPool &pool, uint count)						__NE___;
		static void  _ResetPool (const VDevice &, VkCommandBuffer cmdbuf, uint idx, QueryPool &pool, uint count)	__NE___;

		template <typename T>
		ND_	bool  _GetTimestamp (const Query &q, OUT T* result, Bytes size)											C_NE___;

		template <typename T>
		ND_	bool  _GetTimestampCalibrated (const Query &q, OUT T* result, OUT T* maxDeviation, Bytes size)			C_NE___;

		ND_ Query  _AllocQuery (uint writeIdx, EQueueType queue, EQueryType type, uint count)						C_NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
