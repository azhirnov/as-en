// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/QueryManager.h"
# include "graphics/Remote/RDevice.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Query Manager
	//

	class RQueryManager final : public IQueryManager
	{
	// types
	public:
		using SerQuery_t = RemoteGraphics::Msg::Query_Alloc_Response::SerQuery;

		struct Query : IQuery, SerQuery_t
		{
			Query ()						__NE___ {}
			Query (const SerQuery_t &q)		__NE___ : SerQuery_t{q} {}

			ND_ explicit operator bool ()	C_NE___	{ return bool{queryId}; }
		};


	// variables
	private:
		uint						_calibratedTs	: 1;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		RQueryManager ()																							__NE___;
		~RQueryManager ()																							__NE___;

		ND_ bool  Initialize (uint maxFrames)																		__NE___;
			void  Deinitialize ()																					__NE___;

			void  NextFrame (FrameUID frameId)																		__NE___;

		ND_ Query  AllocQuery (EQueueType queue, EQueryType type, uint count = 1)									C_NE___;

		ND_ bool  SupportsCalibratedTimestamps ()																	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _calibratedTs; }

			bool  GetTimestamp (const IQuery &, OUT ulong* result, Bytes size)										C_NE_OV;	// raw
			bool  GetTimestamp (const IQuery &, OUT double* result, Bytes size)										C_NE_OV;	// nanoseconds in GPU-space
			bool  GetTimestamp (const IQuery &, OUT nanosecondsd* result, Bytes size)								C_NE_OV;	// nanoseconds in GPU-space

			bool  GetTimestampCalibrated (const IQuery &, OUT ulong* result, OUT ulong* maxDeviation, Bytes size)				C_NE_OV;	// nanoseconds in CPU-space
			bool  GetTimestampCalibrated (const IQuery &, OUT double* result, OUT double* maxDeviation, Bytes size)				C_NE_OV;	// nanoseconds in CPU-space
			bool  GetTimestampCalibrated (const IQuery &, OUT nanosecondsd* result, OUT nanosecondsd* maxDeviation, Bytes size)	C_NE_OV;	// nanoseconds in CPU-space

			bool  GetPipelineStatistic (const IQuery &, OUT GraphicsPipelineStatistic* result, Bytes size)			C_NE_OV;

	private:
		template <typename MsgType, typename RespType, typename T>
		ND_	bool  _GetTimestamp (const Query &q, OUT T* result, Bytes size)											C_NE___;

		template <typename MsgType, typename RespType, typename T>
		ND_	bool  _GetTimestampCalibrated (const Query &q, OUT T* result, OUT T* maxDeviation, Bytes size)			C_NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
