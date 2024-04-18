// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/DataSource/SyncDataSource.h"

namespace AE::Threading
{
namespace
{

/*
=================================================
	WaitForRequest
=================================================
*/
	template <typename ReqType>
	inline bool  WaitForRequest (ReqType &req) __NE___
	{
		#ifdef AE_DEBUG
		constexpr uint	max_attempt	= UMax;
		#else
		constexpr uint	max_attempt	= 1000;	// 1000 * 0.5ms = ~500ms
		#endif

		constexpr uint	max_tasks	= 6;

		auto&			sched		= Scheduler();
		const auto		seed		= sched.GetDefaultSeed();

		for (uint a = 0; a < max_attempt; ++a)
		{
			if ( req.IsFinished() )
				return true;

			uint	i = 0;
			for (; (i < max_tasks) and sched.ProcessTask( ETaskQueue::Background, seed ); ++i) {}

			if ( i < max_tasks )
				i += uint(sched.ProcessFileIO());

			if_unlikely( i == 0 )
				ThreadUtils::Sleep_500us();
		}
		return false;
	}
}

/*
=================================================
	ReadBlock
=================================================
*/
	Bytes  SyncRDataSource::ReadBlock (const Bytes pos, OUT void* buffer, Bytes size) __NE___
	{
		auto	req	= _ds->ReadBlock( pos, size );

		if_likely( WaitForRequest( *req ) and req->IsCompleted() )
		{
			auto	res	 = req->GetResult();
					size = Min( size, res.dataSize );
			MemCopy( OUT buffer, res.data, size );
			return size;
		}

		return 0_b;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	WriteBlock
=================================================
*/
	Bytes  SyncWDataSource::WriteBlock (Bytes pos, const void* buffer, const Bytes size) __NE___
	{
		auto	req = _ds->WriteBlock( pos, buffer, size, null );

		if_likely( WaitForRequest( *req ) and req->IsCompleted() )
		{
			auto	res = req->GetResult();
			return Min( size, res.dataSize );
		}

		return 0_b;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ReadSeq
=================================================
*/
	Bytes  SyncRStream::ReadSeq (OUT void* buffer, Bytes size) __NE___
	{
		auto	req = _stream->ReadSeq( buffer, size, null );

		if_likely( WaitForRequest( *req ) and req->IsCompleted() )
		{
			auto	res	 = req->GetResult();
					size = Min( size, res.dataSize );
			MemCopy( OUT buffer, res.data, size );
			return size;
		}

		return 0_b;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	WriteSeq
=================================================
*/
	Bytes  SyncWStream::WriteSeq (const void* buffer, Bytes size) __NE___
	{
		auto	req = _stream->WriteSeq( buffer, size, null );

		if_likely( WaitForRequest( *req ) and req->IsCompleted() )
		{
			auto	res = req->GetResult();
			return Min( size, res.dataSize );
		}

		return 0_b;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ReadSeq
=================================================
*/
	Bytes  SyncRStreamOnAsyncDS::ReadSeq (OUT void* buffer, Bytes size) __NE___
	{
		auto	req	= _ds->ReadBlock( _pos.fetch_add( size ), buffer, size, null );

		if_likely( WaitForRequest( *req ) and req->IsCompleted() )
		{
			auto	res	 = req->GetResult();
					size = Min( size, res.dataSize );
			MemCopy( OUT buffer, res.data, size );
			return size;
		}

		return 0_b;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	WriteSeq
=================================================
*/
	Bytes  SyncWStreamOnAsyncDS::WriteSeq (const void* buffer, const Bytes size) __NE___
	{
		auto	req = _ds->WriteBlock( _pos.fetch_add( size ), buffer, size, null );

		if_likely( WaitForRequest( *req ) and req->IsCompleted() )
		{
			auto	res = req->GetResult();
			return Min( size, res.dataSize );
		}

		return 0_b;
	}


} // AE::Threading
