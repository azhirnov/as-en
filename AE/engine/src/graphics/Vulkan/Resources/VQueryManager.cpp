// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VQueryManager.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

namespace AE::Graphics
{

/*
=================================================
	constructor
=================================================
*/
	VQueryManager::VQueryManager () __NE___ :
		_device{ GraphicsScheduler().GetDevice() },
		_hostReset{false}, _perfQuery{false}, _calibratedTs{false}
	{}

/*
=================================================
	destructor
=================================================
*/
	VQueryManager::~VQueryManager () __NE___
	{
		DRC_EXLOCK( _drCheck );

		for (auto& pool : _poolArr) {
			CHECK( not pool );
		}
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  VQueryManager::Initialize (uint maxFrames) __NE___
	{
		DRC_EXLOCK( _drCheck );

		CHECK_ERR( IQueryManager::_Initialize( maxFrames ));

		_hostReset			= _device.GetVExtensions().hostQueryReset;
		_perfQuery			= false; //_device.GetVExtensions().performanceQuery;	// TODO
		_calibratedTs		= _device.GetVExtensions().calibratedTimestamps;

		_timestampPeriod	= nanosecondsf{_device.GetVProperties().properties.limits.timestampPeriod};
		_timestampAllowed	= Default;

		for (auto& q : _device.GetQueues())
		{
			if ( q.timestampValidBits == 0 ) continue;
			_timestampAllowed |= q.type;
			_tsBits[ uint(q.type) ] = q.timestampValidBits;
		}

		if ( _timestampAllowed != Default and _timestampPeriod > nanosecondsf{0.f} )
		{
			auto&	pool = _poolArr [uint(EQueryType::Timestamp)];
			pool.maxCount = VConfig::TimestampQueryPerFrame;

			VkQueryPoolCreateInfo	timestamp_ci = {};
			timestamp_ci.sType		= VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
			timestamp_ci.queryType	= VK_QUERY_TYPE_TIMESTAMP;
			timestamp_ci.queryCount	= _maxFrames * pool.maxCount;

			VK_CHECK_ERR( _device.vkCreateQueryPool( _device.GetVkDevice(), &timestamp_ci, null, OUT &pool.handle ));

			if ( _calibratedTs )
			{
				FixedArray< VkTimeDomainEXT, 32 >	time_domains;

				uint	count = 0;
				VK_CHECK_ERR( vkGetPhysicalDeviceCalibrateableTimeDomainsEXT( _device.GetVkPhysicalDevice(), OUT &count, null ));

				time_domains.resize( count );
				VK_CHECK_ERR( vkGetPhysicalDeviceCalibrateableTimeDomainsEXT( _device.GetVkPhysicalDevice(), INOUT &count, OUT time_domains.data() ));

				_calibratedTs = false;
				for (auto td : time_domains)
				{
				  #ifdef AE_PLATFORM_WINDOWS
					_calibratedTs |= (td == VK_TIME_DOMAIN_QUERY_PERFORMANCE_COUNTER_EXT);
				  #else
					_calibratedTs |= (td == VK_TIME_DOMAIN_CLOCK_MONOTONIC_EXT);	// VK_TIME_DOMAIN_CLOCK_MONOTONIC_RAW_EXT
				  #endif
				}
			}
		}

		if ( _device.GetVProperties().features.pipelineStatisticsQuery )
		{
			static constexpr auto	StatBits =
				//VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT	|
				VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT		|	// before clipping
				VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT			|	// after clipping
				VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT;
			StaticAssert( sizeof(GraphicsPipelineStatistic) == sizeof(ulong) * CT_BitCount< StatBits >);

			auto&	pool = _poolArr [uint(EQueryType::PipelineStatistic)];
			pool.maxCount = VConfig::PipelineStatQueryPerFrame;

			VkQueryPoolCreateInfo	ppln_stat_ci = {};
			ppln_stat_ci.sType		= VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
			ppln_stat_ci.queryType	= VK_QUERY_TYPE_PIPELINE_STATISTICS;
			ppln_stat_ci.queryCount	= _maxFrames * pool.maxCount;
			ppln_stat_ci.pipelineStatistics = StatBits;

			VK_CHECK_ERR( _device.vkCreateQueryPool( _device.GetVkDevice(), &ppln_stat_ci, null, OUT &pool.handle ));
		}

		if ( _perfQuery )
		{
			Array<VkPerformanceCounterKHR>				vk_counters;
			Array<VkPerformanceCounterDescriptionKHR>	vk_counter_desc;

			for (auto& q : _device.GetQueues())
			{
				uint	count = 0;
				VK_CHECK_ERR( vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(
									_device.GetVkPhysicalDevice(), uint(q.familyIndex), OUT &count, null, null ));

				if ( count == 0 )
					continue;

				vk_counters.resize( count );
				vk_counter_desc.resize( count );

				for (usize i = 0; i < count; ++i) {
					vk_counters[i].sType = VK_STRUCTURE_TYPE_PERFORMANCE_COUNTER_KHR;
					vk_counter_desc[i].sType = VK_STRUCTURE_TYPE_PERFORMANCE_COUNTER_DESCRIPTION_KHR;
				}

				VK_CHECK_ERR( vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(
									_device.GetVkPhysicalDevice(), uint(q.familyIndex), INOUT &count,
									OUT vk_counters.data(), OUT vk_counter_desc.data() ));

				for (uint i = 0; i < count; ++i)
				{
					AE_LOGI( "Queue '"s << StringView{q.debugName} <<
							 "', perf counter: '" << vk_counter_desc[i].name <<
							 "', cat: '" << vk_counter_desc[i].category <<
							 "', desc: '" << vk_counter_desc[i].description << "'" );
				}
			}

			auto&	pool = _poolArr [uint(EQueryType::Performance)];
			pool.maxCount = VConfig::PerformanceQueryPerFrame;

			VkQueryPoolPerformanceCreateInfoKHR		perf_info	= {};
			perf_info.sType				= VK_STRUCTURE_TYPE_QUERY_POOL_PERFORMANCE_CREATE_INFO_KHR;
			perf_info.queueFamilyIndex	= 0;	// TODO
			perf_info.counterIndexCount	= 0;
			perf_info.pCounterIndices	= null;	// TODO

			VkQueryPoolCreateInfo	performance_ci = {};
			performance_ci.sType		= VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
			performance_ci.pNext		= &perf_info;
			performance_ci.queryType	= VK_QUERY_TYPE_PERFORMANCE_QUERY_KHR;
			performance_ci.queryCount	= _maxFrames * pool.maxCount;

			VK_CHECK_ERR( _device.vkCreateQueryPool( _device.GetVkDevice(), &performance_ci, null, OUT &pool.handle ));

			uint	num_passes	= 0;
			vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR( _device.GetVkPhysicalDevice(), OUT &perf_info, OUT &num_passes );

			pool.numPasses = CheckCast<ushort>( num_passes );
		}

		if ( _device.GetVExtensions().accelerationStructure )
		{
			auto&	pool = _poolArr [uint(EQueryType::AccelStructCompactedSize)];
			pool.maxCount = VConfig::ASCompactedSizeQueryPerFrame;

			VkQueryPoolCreateInfo	as_comp_ci = {};
			as_comp_ci.sType		= VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
			as_comp_ci.queryType	= VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR;
			as_comp_ci.queryCount	= _maxFrames * pool.maxCount;

			VK_CHECK_ERR( _device.vkCreateQueryPool( _device.GetVkDevice(), &as_comp_ci, null, OUT &pool.handle ));
		}

		for (auto& pool : _poolArr)
		{
			pool.count.store( pool.maxCount );

			for (auto& cnt : pool.countArr)
				cnt = pool.maxCount;
		}

		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  VQueryManager::Deinitialize () __NE___
	{
		DRC_EXLOCK( _drCheck );

		for (auto& pool : _poolArr)
		{
			if ( pool )
				_device.vkDestroyQueryPool( _device.GetVkDevice(), pool.handle, null );

			pool.handle = Default;
		}

		_maxFrames = 0;
	}

/*
=================================================
	NextFrame
=================================================
*/
	void  VQueryManager::NextFrame (const FrameUID frameId) __NE___
	{
		DRC_EXLOCK( _drCheck );

		const auto [i, p] = IQueryManager::_NextFrame( frameId );

		for (auto& pool : _poolArr) {
			pool.countArr[i] = pool.count.exchange( 0 );
		}

		if ( _hostReset )
		{
			for (auto& pool : _poolArr) {
				_ResetPoolOnHost( _device, p.writeIndex, pool, pool.countArr[p.writeIndex] );
			}
		}
	}

/*
=================================================
	ResetQueries
=================================================
*/
	void  VQueryManager::ResetQueries (VkCommandBuffer cmdbuf, FrameUID frameId) __NE___
	{
		if ( _hostReset )
			return;

		const uint	j = WriteIndex( frameId );

		for (auto& pool : _poolArr) {
			_ResetPool( _device, cmdbuf, j, pool, pool.countArr[j] );
		}
	}

/*
=================================================
	AcquireProfilingLock
=================================================
*/
	bool  VQueryManager::AcquireProfilingLock () __NE___
	{
		CHECK_ERR( _perfQuery != 0 );
		CHECK_ERR( not _perfLockAcquired.load() );

		VkAcquireProfilingLockInfoKHR	info = {};
		info.sType		= VK_STRUCTURE_TYPE_ACQUIRE_PROFILING_LOCK_INFO_KHR;
		info.timeout	= UMax;

		VK_CHECK_ERR( _device.vkAcquireProfilingLockKHR( _device.GetVkDevice(), &info ));
		_perfLockAcquired.store( true );

		return true;
	}

/*
=================================================
	ReleaseProfilingLock
=================================================
*/
	bool  VQueryManager::ReleaseProfilingLock () __NE___
	{
		CHECK_ERR( _perfQuery != 0 );
		CHECK_ERR( _perfLockAcquired.load() );

		_device.vkReleaseProfilingLockKHR( _device.GetVkDevice() );
		_perfLockAcquired.store( false );

		return true;
	}

/*
=================================================
	_ResetPool
----
	docs:
This command defines an execution dependency between other query commands that reference the same query.
The first synchronization scope includes all commands which reference the queries in queryPool indicated by firstQuery and queryCount that occur earlier in submission order.
The second synchronization scope includes all commands which reference the queries in queryPool indicated by firstQuery and queryCount that occur later in submission order.
The operation of this command happens after the first scope and happens before the second scope.
Supported queue types: Graphics / Compute
=================================================
*/
	void  VQueryManager::_ResetPool (const VDevice &dev, VkCommandBuffer cmdbuf, uint idx, QueryPool &pool, uint count) __NE___
	{
		ASSERT( count <= pool.maxCount );
		if_likely( pool and count > 0 )
			dev.vkCmdResetQueryPool( cmdbuf, pool.handle, idx * pool.maxCount, count );
	}

/*
=================================================
	_ResetPoolOnHost
=================================================
*/
	void  VQueryManager::_ResetPoolOnHost (const VDevice &dev, uint idx, QueryPool &pool, uint count) __NE___
	{
		ASSERT( count <= pool.maxCount );
		if_likely( pool and count > 0 )
			dev.vkResetQueryPoolEXT( dev.GetVkDevice(), pool.handle, idx * pool.maxCount, count );
	}

/*
=================================================
	AllocQuery
=================================================
*/
	VQueryManager::Query  VQueryManager::AllocQuery (EQueueType queueType, EQueryType type, uint count) C_NE___
	{
		return _AllocQuery( WriteIndex(), queueType, type, count );
	}

	VQueryManager::Query  VQueryManager::AllocQuery (FrameUID frameId, EQueueType queueType, EQueryType type, uint count) C_NE___
	{
		return _AllocQuery( WriteIndex( frameId ), queueType, type, count );
	}

	VQueryManager::Query  VQueryManager::_AllocQuery (const uint writeIdx, EQueueType queueType, EQueryType type, uint count) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		CHECK_ERR( type < EQueryType::_Count );

		switch_enum( type )
		{
			case_likely EQueryType::Timestamp :
				if_unlikely( NoBits( _timestampAllowed, EQueueMask(0) | queueType ))
					return Default;
				break;

			case EQueryType::Performance :
				if_unlikely( not _perfLockAcquired.load() )
					return Default;
				break;

			case EQueryType::PipelineStatistic :
				if_unlikely( queueType != EQueueType::Graphics )
					return Default;
				break;

			case EQueryType::AccelStructCompactedSize :
			case EQueryType::AccelStructSize :
			case EQueryType::AccelStructSerializationSize :
				if_unlikely( NoBits( EQueueMask::Graphics | EQueueMask::AsyncCompute, EQueueMask(0) | queueType ))
					return Default;
				break;

			case EQueryType::_Count :
			case EQueryType::Unknown : break;
		}
		switch_end

		auto&	pool = _poolArr[ uint(type) ];
		if_unlikely( not pool )
			return Default;	// not initialized

		Query	result;

		result.first = CheckCast<ushort>( pool.count.fetch_add( count ));
		if_unlikely( not (result.first + count < pool.maxCount) )
			return Default;	// overflow

		result.pool		= pool.handle;
		result.count	= CheckCast<ushort>( count );
		result.first	= CheckCast<ushort>( result.first + pool.maxCount * writeIdx);
		result.numPasses= CheckCast<ushort>( pool.numPasses );
		result.type		= type;
		result.queue	= queueType;

		return result;
	}

/*
=================================================
	GetTimestamp
----
	docs:
This command defines an execution dependency between other query commands that reference the same query.

The first synchronization scope includes all commands which reference the queries in queryPool indicated by query that occur earlier in submission order.
If flags does not include VK_QUERY_RESULT_WAIT_BIT, vkCmdEndQueryIndexedEXT, vkCmdWriteTimestamp2, vkCmdEndQuery, and vkCmdWriteTimestamp are excluded from this scope.

The second synchronization scope includes all commands which reference the queries in queryPool indicated by query that occur later in submission order.
=================================================
*/
	template <typename T>
	bool  VQueryManager::_GetTimestamp (const Query &q, OUT T* result, const Bytes resultSize) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		StaticAssert( sizeof(*result) == sizeof(ulong) );

		CHECK_ERR( q and result != null );
		CHECK_ERR( resultSize >= (SizeOf<ulong> * q.count) );
		ASSERT( q.type == EQueryType::Timestamp );

		auto&	pool = _poolArr[ uint(q.type) ];
		Unused( pool );
	//	ASSERT( q.first >= pool.maxCount * ReadIndex() );
	//	ASSERT( q.first <  pool.maxCount * (ReadIndex()+1) );

		Result64*	tmp;	AllocateOnStack( OUT tmp, q.count );
		auto		err	= _device.vkGetQueryPoolResults( _device.GetVkDevice(), q.pool, q.first, q.count, sizeof(Result64) * q.count, tmp,
														 sizeof(Result64), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WITH_AVAILABILITY_BIT );

		bool		available	= true;
		const ulong	mask		= ToBitMask<ulong>( _tsBits[ uint(q.queue) ]);

		for (uint i = 0, cnt = q.count; i < cnt; ++i)
		{
			if constexpr( IsSameTypes< T, ulong >)
				result[i] = tmp[i].result & mask;
			else
			if constexpr( IsSameTypes< T, double >)
				result[i] = double(tmp[i].result & mask) * double(_timestampPeriod.count());
			else
			if constexpr( IsSameTypes< T, nanosecondsd >)
				result[i] = nanosecondsd{ double(tmp[i].result & mask) * double(_timestampPeriod.count()) };
			else
				return false;

			available &= tmp[i].IsAvailable();
		}
		return err == VK_SUCCESS and available;
	}

	bool  VQueryManager::GetTimestamp (const IQuery &q, OUT ulong* result, Bytes resultSize) C_NE___
	{
		return _GetTimestamp( static_cast<Query const&>(q), OUT result, resultSize );
	}

	bool  VQueryManager::GetTimestamp (const IQuery &q, OUT double* result, Bytes resultSize) C_NE___
	{
		return _GetTimestamp( static_cast<Query const&>(q), OUT result, resultSize );
	}

	bool  VQueryManager::GetTimestamp (const IQuery &q, OUT nanosecondsd* result, Bytes resultSize) C_NE___
	{
		return _GetTimestamp( static_cast<Query const&>(q), OUT Cast<double>(result), resultSize );
	}

/*
=================================================
	GetTimestampCalibrated
=================================================
*/
	template <typename T>
	bool  VQueryManager::_GetTimestampCalibrated (const Query &q, OUT T* result, OUT T* maxDeviation, const Bytes resultSize) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		StaticAssert( sizeof(*result) == sizeof(ulong) );

		CHECK_ERR( q and result != null and maxDeviation != null );
		CHECK_ERR( _calibratedTs != 0 );
		CHECK_ERR( q.count <= 2 );
		CHECK_ERR( resultSize >= (SizeOf<ulong> * q.count) );
		ASSERT( q.type == EQueryType::Timestamp );

		auto&	pool = _poolArr[ uint(q.type) ];
		Unused( pool );
		ASSERT( q.first >= pool.maxCount * ReadIndex() );
		ASSERT( q.first <  pool.maxCount * (ReadIndex()+1) );

		Result64*	tmp;	AllocateOnStack( OUT tmp, q.count );
		auto		err = _device.vkGetQueryPoolResults( _device.GetVkDevice(), q.pool, q.first, q.count, sizeof(Result64) * q.count, tmp,
														 sizeof(Result64), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WITH_AVAILABILITY_BIT );
		if_unlikely( err != VK_SUCCESS )
			return err;

		VkCalibratedTimestampInfoEXT	info[2] = {};
		info[0].sType		= VK_STRUCTURE_TYPE_CALIBRATED_TIMESTAMP_INFO_EXT;
		info[1].sType		= VK_STRUCTURE_TYPE_CALIBRATED_TIMESTAMP_INFO_EXT;
	  #ifdef AE_PLATFORM_WINDOWS
		info[0].timeDomain	= VK_TIME_DOMAIN_QUERY_PERFORMANCE_COUNTER_EXT;
		info[1].timeDomain	= VK_TIME_DOMAIN_QUERY_PERFORMANCE_COUNTER_EXT;
	  #else
		info[0].timeDomain	= VK_TIME_DOMAIN_CLOCK_MONOTONIC_EXT;
		info[1].timeDomain	= VK_TIME_DOMAIN_CLOCK_MONOTONIC_EXT;
	  #endif

		bool		available		= true;
		const ulong	mask			= ToBitMask<ulong>( _tsBits[ uint(q.queue) ]);
		ulong*		result_u64		= Cast<ulong>( result );
		ulong*		deviation_u64	= Cast<ulong>( maxDeviation );

		for (uint i = 0, cnt = q.count; i < cnt; ++i)
		{
			result_u64[i]  = tmp[i].result & mask;
			available     &= tmp[i].IsAvailable();
		}

		err = _device.vkGetCalibratedTimestampsEXT( _device.GetVkDevice(), q.count, info, INOUT result_u64, OUT deviation_u64 );

		if constexpr( IsSameTypes< T, double >)
		{
			for (uint i = 0, cnt = q.count; i < cnt; ++i)
			{
				result[i]		= double(result_u64[i]);
				maxDeviation[i]	= double(deviation_u64[i]);
			}
		}
		else
		if constexpr( IsSameTypes< T, nanosecondsd >)
		{
			for (uint i = 0, cnt = q.count; i < cnt; ++i)
			{
				result[i]		= nanosecondsd(result_u64[i]);
				maxDeviation[i]	= nanosecondsd(deviation_u64[i]);
			}
		}
		else
			return false;

		return err == VK_SUCCESS and available;
	}

	bool  VQueryManager::GetTimestampCalibrated (const IQuery &q, OUT ulong* result, OUT ulong* maxDeviation, Bytes resultSize) C_NE___
	{
		return _GetTimestampCalibrated( static_cast<Query const&>(q), OUT result, OUT maxDeviation, resultSize );
	}

	bool  VQueryManager::GetTimestampCalibrated (const IQuery &q, OUT double* result, OUT double* maxDeviation, Bytes resultSize) C_NE___
	{
		return _GetTimestampCalibrated( static_cast<Query const&>(q), OUT result, OUT maxDeviation, resultSize );
	}

	bool  VQueryManager::GetTimestampCalibrated (const IQuery &q, OUT nanosecondsd* result, OUT nanosecondsd* maxDeviation, Bytes resultSize) C_NE___
	{
		return _GetTimestampCalibrated( static_cast<Query const&>(q), OUT Cast<double>(result), OUT Cast<double>(maxDeviation), resultSize );
	}

/*
=================================================
	GetPerformanceCounter
=================================================
*
	bool  VQueryManager::GetPerformanceCounter (const Query &q, OUT VkPerformanceCounterResultKHR* result, Bytes resultSize) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		CHECK_ERR( q and result != null );

		return _device.vkGetQueryPoolResults( _device.GetVkDevice(), q.pool, q.first, q.count, usize(resultSize), result,
											  sizeof(*result), VK_QUERY_RESULT_WAIT_BIT );
	}

/*
=================================================
	GetPipelineStatistic
=================================================
*/
	bool  VQueryManager::GetPipelineStatistic (const IQuery &iq, OUT GraphicsPipelineStatistic* result, const Bytes resultSize) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		StaticAssert( IsMultipleOf( sizeof(*result), sizeof(ulong) ));

		auto&	q = static_cast<Query const&>(iq);

		CHECK_ERR( q and result != null );
		CHECK_ERR( resultSize >= (SizeOf<GraphicsPipelineStatistic> * q.count) );
		CHECK( q.type == EQueryType::PipelineStatistic );

		auto&	pool = _poolArr[ uint(q.type) ];
		Unused( pool );
		ASSERT( q.first >= pool.maxCount * ReadIndex() );
		ASSERT( q.first <  pool.maxCount * (ReadIndex()+1) );

		PipelineStatisticResult*	tmp;	AllocateOnStack( OUT tmp, q.count );
		auto						err = _device.vkGetQueryPoolResults( _device.GetVkDevice(), q.pool, q.first, q.count, sizeof(PipelineStatisticResult) * q.count, tmp,
																		 sizeof(PipelineStatisticResult), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WITH_AVAILABILITY_BIT );
		bool	available = true;
		for (uint i = 0, cnt = q.count; i < cnt; ++i)
		{
			result[i]  = tmp[i];
			available &= tmp[i].IsAvailable();
		}
		return err == VK_SUCCESS and available;
	}

/*
=================================================
	GetRTASProperty
=================================================
*/
	bool  VQueryManager::GetRTASProperty (const Query &q, OUT Bytes64u* result, const Bytes resultSize) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		StaticAssert( sizeof(*result) == sizeof(ulong) );

		CHECK_ERR( q and result != null );
		CHECK_ERR( resultSize >= (SizeOf<Bytes64u> * q.count) );

		ASSERT(	q.type == EQueryType::AccelStructCompactedSize		or
				q.type == EQueryType::AccelStructSerializationSize	or
				q.type == EQueryType::AccelStructSize				);

		auto&	pool = _poolArr[ uint(q.type) ];
		Unused( pool );
		ASSERT( q.first >= pool.maxCount * ReadIndex() );
		ASSERT( q.first <  pool.maxCount * (ReadIndex()+1) );

		Result64*	tmp;	AllocateOnStack( OUT tmp, q.count );
		auto		err = _device.vkGetQueryPoolResults( _device.GetVkDevice(), q.pool, q.first, q.count, sizeof(Result64) * q.count, tmp,
														 sizeof(Result64), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WITH_AVAILABILITY_BIT );
		bool	available = true;
		for (uint i = 0, cnt = q.count; i < cnt; ++i)
		{
			result[i]  = Bytes64u{ tmp[i].result };
			available &= tmp[i].IsAvailable();
		}
		return err == VK_SUCCESS and available;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
