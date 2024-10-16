// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "profiler/Remote/RemotePowerVRProfiler.h"

namespace AE::Profiler
{
	using namespace AE::Networking;

#ifdef AE_ENABLE_PVRCOUNTER
/*
=================================================
	MsgConsumer::Consume
=================================================
*/
	void  PowerVRProfilerServer::MsgConsumer::Consume (ChunkList<const Networking::CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_PVRProf_ ## _name_::UID :	_server._ ## _name_( *msg->As< CSMsg_PVRProf_ ## _name_ >() );	break;
				CASE( InitReq )
				#undef CASE
			}
		}
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  PowerVRProfilerServer::Initialize (ClientServer_t &clientServer, RC<MsgProducer> mp) __NE___
	{
		CHECK_ERR( mp );

		CHECK_ERR( clientServer.Add( _msgConsumer.GetRC() ));

		_msgProducer = RVRef(mp);

		_prof.status.store( EStatus::NotInitialized );
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  PowerVRProfilerServer::Deinitialize () __NE___
	{
		_prof.profiler.Deinitialize();
		_prof.status.store( EStatus::NotInitialized );

		_msgProducer = null;
	}

/*
=================================================
	_PVRProf_InitReq
=================================================
*/
	void  PowerVRProfilerServer::_InitReq (CSMsg_PVRProf_InitReq const &inMsg) __NE___
	{
		if ( _prof.status.load() != EStatus::NotInitialized )
			return;

		_prof.timer.Start( inMsg.updateInterval );
		_prof.samplesIdx = 0;
		_prof.timingIdx  = 0;

		_prof.profiler.Deinitialize();

		bool	ok = _prof.profiler.Initialize( inMsg.enable );

		auto	msg = _msgProducer->CreateMsg< CSMsg_PVRProf_InitRes >();
		if ( msg )
		{
			msg->ok = ok;
			if ( ok ) {
				msg->enabled = _prof.profiler.EnabledCounterSet();
			}
			CHECK( _msgProducer->AddMessage( msg ));
		}

		_prof.status.store( (ok ? EStatus::Initialized : EStatus::NotSupported), EMemoryOrder::Release );
	}

/*
=================================================
	Update
=================================================
*/
	void  PowerVRProfilerServer::Update () __NE___
	{
		_UpdatePVRProfiler();
	}

/*
=================================================
	_UpdatePVRProfiler
=================================================
*/
	void  PowerVRProfilerServer::_UpdatePVRProfiler () __NE___
	{
		if ( _prof.status.load() != EStatus::Initialized )
			return;

		MemoryBarrier( EMemoryOrder::Acquire );

		_prof.profiler.Tick();

		_SendTimings();

		const auto	dt	= _prof.timer.Tick();
		if ( dt )
		{
			_SendSamples( dt.As<milliseconds>() );
		}
	}

/*
=================================================
	_SendSamples
=================================================
*/
	void  PowerVRProfilerServer::_SendSamples (milliseconds dt) __NE___
	{
		float	invdt;
		_prof.profiler.Sample( OUT _prof.counters, OUT invdt );
		invdt = 1.f / TimeCast<secondsf>(dt).count();

		if ( _prof.counters.empty() )
			return;

		// send first message
		bool	is_sent = false;
		{
			auto	msg = _msgProducer->CreateMsg< CSMsg_PVRProf_NextSample >();
			if ( msg )
			{
				msg->index	= _prof.samplesIdx;
				msg->invdt	= invdt;

				is_sent = _msgProducer->AddMessage( msg );
			}
		}

		// send payload
		if ( is_sent )
		{
			using			KeyVal	= CSMsg_PVRProf_Sample::KeyVal;
			constexpr usize	step	= usize((NetConfig::TCP_MaxMsgSize - SizeOf<CSMsg_PVRProf_Sample>) / SizeOf<KeyVal>);
			usize			sent	= 0;

			for (auto it = _prof.counters.begin(); it != _prof.counters.end();)
			{
				const usize	count = Min( _prof.counters.size() - sent, step+1 );

				auto	msg = _msgProducer->CreateMsg< CSMsg_PVRProf_Sample >( SizeOf<KeyVal> * (count-1) );
				if ( not msg )
					break;

				msg->index	= _prof.samplesIdx;
				msg->count	= ubyte(count);

				auto	it2 = it;
				for (usize i = 0; i < count; ++i, ++it2)
				{
					msg->arr[i] = *it2;
				}

				if ( not _msgProducer->AddMessage( msg ))
					break;

				it	 = it2;
				sent += count;
			}
			ASSERT( sent == _prof.counters.size() );

			_prof.samplesIdx ++;
		}
	}

/*
=================================================
	_SendTimings
=================================================
*/
	void  PowerVRProfilerServer::_SendTimings () __NE___
	{
		_prof.profiler.ReadTimingData( OUT _prof.timing );

		if ( _prof.timing.empty() )
			return;

		using TimeScope			= PowerVRProfiler::TimeScope;
		constexpr usize	step	= usize((NetConfig::TCP_MaxMsgSize - SizeOf<CSMsg_PVRProf_Timing>) / SizeOf<TimeScope>);
		usize			sent	= 0;

		for (; sent < _prof.timing.size();)
		{
			const usize	count = Min( _prof.timing.size() - sent, step+1 );

			auto	msg = _msgProducer->CreateMsg< CSMsg_PVRProf_Timing >( SizeOf<TimeScope> * (count-1) );
			if ( not msg )
				break;

			msg->index	= _prof.timingIdx;
			msg->count	= ubyte(count);

			for (usize i = 0; i < count; ++i)
			{
				msg->arr[i] = _prof.timing[sent+i];
			}

			if ( not _msgProducer->AddMessage( msg ))
				break;

			sent += count;
		}
		ASSERT( sent == _prof.timing.size() );

		_prof.timingIdx ++;
	}

#endif // AE_ENABLE_PVRCOUNTER
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	PowerVRProfilerClient::PowerVRProfilerClient (RC<MsgProducer> mp) __NE___ :
		_msgProducer{ RVRef(mp) }
	{}

/*
=================================================
	Initialize
=================================================
*/
	bool  PowerVRProfilerClient::Initialize (const ECounterSet &counterSet) __NE___
	{
		CHECK_ERR( counterSet.Any() );

		_requiredCS = counterSet;
		return _Initialize( counterSet );
	}

	bool  PowerVRProfilerClient::_Initialize (const ECounterSet &counterSet) __NE___
	{
		auto	msg = _msgProducer->CreateMsg< CSMsg_PVRProf_InitReq >();
		if ( msg )
		{
			msg->enable			= counterSet;
			msg->updateInterval	= secondsf{1.f};

			if ( _msgProducer->AddMessage( msg ))
				return true;
		}
		return false;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  PowerVRProfilerClient::Deinitialize () __NE___
	{
	}

/*
=================================================
	_Consume
=================================================
*/
	void  PowerVRProfilerClient::_Consume (ChunkList<const CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			ASSERT( msg->GroupId() == CSMessageGroup::Debug );
			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_PVRProf_ ## _name_::UID :	_ ## _name_( *msg->As< CSMsg_PVRProf_ ## _name_ >() );	break;
				CASE( InitRes )
				CASE( NextSample )
				CASE( Sample )
				CASE( Timing )
				#undef CASE
			}
		}
	}

/*
=================================================
	Sample
=================================================
*/
	void  PowerVRProfilerClient::Sample (OUT Counters_t &result, INOUT float &invdt) __NE___
	{
		result.clear();
		invdt = 0.f;

		EXLOCK( _guard );

		if ( _IsNotInitialized() )
		{
			if_unlikely( _connectionLostTimer.Tick() )
				Unused( _Initialize( _requiredCS ));
			return;
		}

		auto&	curr = _counters[ _countersIdx & 1 ];
		invdt		 = _invdt[ _countersIdx & 1 ];

		std::swap( result, curr );
		curr.clear();
	}

/*
=================================================
	ReadTimingData
=================================================
*/
	void  PowerVRProfilerClient::ReadTimingData (OUT TimeScopeArr_t &result) __NE___
	{
		EXLOCK( _guard );

		if ( _IsNotInitialized() )
		{
			result.clear();
			return;
		}

		auto&	curr = _timings[ _timingsIdx & 1 ];

		// keep previous values if new timings are not available
		if ( not curr.empty() )
		{
			std::swap( result, curr );
			curr.clear();
		}
	}

/*
=================================================
	_InitRes
=================================================
*/
	inline void  PowerVRProfilerClient::_InitRes (CSMsg_PVRProf_InitRes const& msg) __NE___
	{
		EXLOCK( _guard );

		_status		= msg.ok ? EStatus::Initialized : EStatus::NotSupported;
		_enabled	= msg.enabled;

		_connectionLostTimer.Restart();
	}

/*
=================================================
	_NextSample
=================================================
*/
	inline void  PowerVRProfilerClient::_NextSample (CSMsg_PVRProf_NextSample const& msg) __NE___
	{
		EXLOCK( _guard );

		if ( _IsInitialized() )
		{
			_invdt[ _countersIdx & 1 ] = msg.invdt;

			_countersIdx		= (_countersIdx+1) & 1;
			_pendingCountersIdx	= msg.index;

			_connectionLostTimer.Restart();
		}
	}

/*
=================================================
	_Sample
=================================================
*/
	inline void  PowerVRProfilerClient::_Sample (CSMsg_PVRProf_Sample const& msg) __NE___
	{
		EXLOCK( _guard );

		if ( _IsInitialized() and _pendingCountersIdx == msg.index )
		{
			auto&	curr = _counters[ (_countersIdx+1) & 1 ];

			for (uint i = 0, cnt = msg.count; i < cnt; ++i)
				curr.insert_or_assign( msg.arr[i].first, msg.arr[i].second );
		}
	}

/*
=================================================
	_Timing
=================================================
*/
	inline void  PowerVRProfilerClient::_Timing (CSMsg_PVRProf_Timing const& msg) __NE___
	{
		EXLOCK( _guard );

		if ( not _IsInitialized() )
			return;

		if ( _pendingTimingsIdx != msg.index )
		{
			_pendingTimingsIdx	= msg.index;
			_timingsIdx			= (_timingsIdx+1) & 1;
			_timings[ (_timingsIdx+1) & 1 ].clear();
		}

		auto&	curr = _timings[ (_timingsIdx+1) & 1 ];
		curr.reserve( curr.size() + msg.count );

		for (uint i = 0, cnt = msg.count; i < cnt; ++i)
			curr.push_back( msg.arr[i] );
	}

/*
=================================================
	GetMsgConsumer
=================================================
*/
	RC<PowerVRProfilerClient::MsgConsumer>  PowerVRProfilerClient::GetMsgConsumer () __NE___
	{
		return MakeRCNe<Networking::DefaultCSMessageConsumer>( [this](auto ml){ _Consume( ml ); }, CSMessageGroup::Debug );
	}


} // AE::Profiler
