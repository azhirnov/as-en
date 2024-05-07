// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RQueryManager.h"
# include "graphics/Remote/RRenderTaskScheduler.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	constructor / destructor
=================================================
*/
	RQueryManager::RQueryManager () __NE___
	{}

	RQueryManager::~RQueryManager () __NE___
	{}

/*
=================================================
	Initialize
=================================================
*/
	bool  RQueryManager::Initialize (uint maxFrames) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _Initialize( maxFrames ));

		Msg::Query_Init					msg;
		RC<Msg::Query_Init_Response>	res;

		CHECK_ERR( GraphicsScheduler().GetDevice().SendAndWait( msg, OUT res ));

		_calibratedTs = res->calibratedTimestamps;
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  RQueryManager::Deinitialize () __NE___
	{
		DRC_EXLOCK( _drCheck );
	}

/*
=================================================
	NextFrame
=================================================
*/
	void  RQueryManager::NextFrame (const FrameUID frameId) __NE___
	{
		DRC_EXLOCK( _drCheck );

		Unused( _NextFrame( frameId ));
	}

/*
=================================================
	AllocQuery
=================================================
*/
	RQueryManager::Query  RQueryManager::AllocQuery (EQueueType queueType, EQueryType queryType, uint count) C_NE___
	{
		Msg::Query_Alloc				msg;
		RC<Msg::Query_Alloc_Response>	res;

		msg.queueType	= queueType;
		msg.queryType	= queryType;
		msg.count		= count;

		CHECK_ERR( GraphicsScheduler().GetDevice().SendAndWait( msg, OUT res ));
		return Query{ res->query };
	}

/*
=================================================
	_GetTimestamp
=================================================
*/
	template <typename MsgType, typename RespType, typename T>
	bool  RQueryManager::_GetTimestamp (const Query &q, OUT T* result, const Bytes size) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		StaticAssert( sizeof(*result) == sizeof(ulong) );

		CHECK_ERR( q and result != null );
		CHECK_ERR( size >= (SizeOf<ulong> * q.count) );
		ASSERT( q.type == EQueryType::Timestamp );

		MsgType			msg;
		RC<RespType>	res;

		msg.query	= q;
		msg.size	= size;

		CHECK_ERR( GraphicsScheduler().GetDevice().SendAndWait( msg, OUT res ));

		if_likely( not res->result.empty() )
		{
			CHECK_ERR( ArraySizeOf(res->result) <= size );

			MemCopy( OUT result, res->result.data(), ArraySizeOf(res->result) );
			return true;
		}
		return false;
	}

/*
=================================================
	GetTimestamp
=================================================
*/
	bool  RQueryManager::GetTimestamp (const IQuery &q, OUT ulong* result, Bytes size) C_NE___
	{
		return _GetTimestamp< Msg::Query_GetTimestampUL, Msg::Query_GetTimestampUL_Response >( static_cast<Query const&>(q), OUT result, size );
	}

	bool  RQueryManager::GetTimestamp (const IQuery &q, OUT double* result, Bytes size) C_NE___
	{
		return _GetTimestamp< Msg::Query_GetTimestampD, Msg::Query_GetTimestampD_Response >( static_cast<Query const&>(q), OUT result, size );
	}

	bool  RQueryManager::GetTimestamp (const IQuery &q, OUT nanosecondsd* result, Bytes size) C_NE___
	{
		return _GetTimestamp< Msg::Query_GetTimestampNs, Msg::Query_GetTimestampNs_Response >( static_cast<Query const&>(q), OUT result, size );
	}

/*
=================================================
	_GetTimestampCalibrated
=================================================
*/
	template <typename MsgType, typename RespType, typename T>
	bool  RQueryManager::_GetTimestampCalibrated (const Query &q, OUT T* result, OUT T* maxDeviation, const Bytes size) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		StaticAssert( sizeof(*result) == sizeof(ulong) );

		CHECK_ERR( q and result != null and maxDeviation != null );
		CHECK_ERR( _calibratedTs );
		CHECK_ERR( q.count <= 2 );
		CHECK_ERR( size >= (SizeOf<ulong> * q.count) );
		ASSERT( q.type == EQueryType::Timestamp );

		MsgType			msg;
		RC<RespType>	res;

		msg.query	= q;
		msg.size	= size;

		CHECK_ERR( GraphicsScheduler().GetDevice().SendAndWait( msg, OUT res ));

		if_likely( not res->result.empty() )
		{
			CHECK_ERR( res->result.size() == res->maxDeviation.size() );
			CHECK_ERR( ArraySizeOf(res->result) <= size );
			CHECK_ERR( ArraySizeOf(res->maxDeviation) <= size );

			MemCopy( OUT result, res->result.data(), ArraySizeOf(res->result) );
			MemCopy( OUT maxDeviation, res->maxDeviation.data(), ArraySizeOf(res->maxDeviation) );
			return true;
		}
		return false;
	}

/*
=================================================
	GetTimestampCalibrated
=================================================
*/
	bool  RQueryManager::GetTimestampCalibrated (const IQuery &q, OUT ulong* result, OUT ulong* maxDeviation, Bytes size) C_NE___
	{
		return _GetTimestampCalibrated< Msg::Query_GetTimestampCalibratedUL, Msg::Query_GetTimestampCalibratedUL_Response >( static_cast<Query const&>(q), OUT result, OUT maxDeviation, size );
	}

	bool  RQueryManager::GetTimestampCalibrated (const IQuery &q, OUT double* result, OUT double* maxDeviation, Bytes size) C_NE___
	{
		return _GetTimestampCalibrated< Msg::Query_GetTimestampCalibratedD, Msg::Query_GetTimestampCalibratedD_Response >( static_cast<Query const&>(q), OUT result, OUT maxDeviation, size );
	}

	bool  RQueryManager::GetTimestampCalibrated (const IQuery &q, OUT nanosecondsd* result, OUT nanosecondsd* maxDeviation, Bytes size) C_NE___
	{
		return _GetTimestampCalibrated< Msg::Query_GetTimestampCalibratedNs, Msg::Query_GetTimestampCalibratedNs_Response >( static_cast<Query const&>(q), OUT result, OUT maxDeviation, size );
	}

/*
=================================================
	GetPipelineStatistic
=================================================
*/
	bool  RQueryManager::GetPipelineStatistic (const IQuery &iq, OUT GraphicsPipelineStatistic* result, const Bytes size) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		StaticAssert( IsMultipleOf( sizeof(*result), sizeof(ulong) ));

		auto&	q = static_cast<Query const&>(iq);

		CHECK_ERR( q and result != null );
		CHECK_ERR( size >= (SizeOf<GraphicsPipelineStatistic> * q.count) );
		CHECK( q.type == EQueryType::PipelineStatistic );

		Msg::Query_GetPipelineStatistic					msg;
		RC<Msg::Query_GetPipelineStatistic_Response>	res;

		msg.query	= q;
		msg.size	= size;

		CHECK_ERR( GraphicsScheduler().GetDevice().SendAndWait( msg, OUT res ));

		if_unlikely( res->result.empty() )
			return false;

		CHECK_ERR( ArraySizeOf(res->result) <= size );
		MemCopy( OUT result, res->result.data(), ArraySizeOf(res->result) );

		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
