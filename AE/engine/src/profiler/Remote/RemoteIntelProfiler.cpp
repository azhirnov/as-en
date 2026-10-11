// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "profiler/Remote/RemoteIntelProfiler.h"

namespace AE::Profiler
{
	using namespace AE::Networking;

#ifdef AE_PLATFORM_LINUX
/*
 =================================================
	MsgConsumer::Consume
 =================================================
 */
	void  IntelProfilerServer::MsgConsumer::Consume (ChunkList<const Networking::CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_ ## _name_::UID :	_server._ ## _name_( *msg->As< CSMsg_ ## _name_ >() );	break;
				CASE( IntelProf_InitReq )
				#undef CASE
			}
		}
	}

/*
 =================================================
	Initialize
 =================================================
 */
	bool  IntelProfilerServer::Initialize (ClientServer_t &clientServer, RC<MsgProducer> mp) __NE___
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
	void  IntelProfilerServer::Deinitialize () __NE___
	{
		_prof.profiler.Deinitialize();
		_prof.status.store( EStatus::NotInitialized );

		_msgProducer = null;
	}

/*
 =================================================
	_IntelProf_InitReq
 =================================================
 */
	void  IntelProfilerServer::_IntelProf_InitReq (CSMsg_IntelProf_InitReq const &inMsg) __NE___
	{
		if ( _prof.status.load() != EStatus::NotInitialized )
			return;

		_prof.timer.Start( inMsg.updateInterval );
		_prof.index = 0;

		_prof.profiler.Deinitialize();

		bool	ok = _prof.profiler.Initialize( inMsg.enable );

		auto	msg = _msgProducer->CreateMsg< CSMsg_IntelProf_InitRes >();
		if ( msg )
		{
			msg->ok = ok;
			if ( ok ) {
				msg->enabled	= _prof.profiler.EnabledCounterSet();
				msg->info		= _prof.profiler.GetHWInfo();
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
	void  IntelProfilerServer::Update () __NE___
	{
		_UpdateIntelProfiler();
	}

/*
 =================================================
	_UpdateIntelProfiler
 =================================================
 */
	void  IntelProfilerServer::_UpdateIntelProfiler () __NE___
	{
		if ( _prof.status.load() != EStatus::Initialized )
			return;

		MemoryBarrier( EMemoryOrder::Acquire );

		const auto	dt	= _prof.timer.Tick();

		if_likely( not dt )
			return;

		float	invdt;
		_prof.profiler.Sample( OUT _prof.counters, OUT invdt );
		invdt = 1.f / dt.As<secondsf>().count();

		if ( _prof.counters.empty() )
			return;

		// send first message
		bool	is_sent = false;
		{
			auto	msg = _msgProducer->CreateMsg< CSMsg_IntelProf_NextSample >();
			if ( msg )
			{
				msg->index	= _prof.index;
				msg->invdt	= invdt;

				is_sent = _msgProducer->AddMessage( msg );
			}
		}

		// send payload
		if ( is_sent )
		{
			using			KeyVal	= CSMsg_IntelProf_Sample::KeyVal;
			constexpr usize	step	= usize((NetConfig::TCP_MaxMsgSize - SizeOf<CSMsg_IntelProf_Sample>) / SizeOf<KeyVal>);
			usize			sent	= 0;

			for (auto it = _prof.counters.begin(); it != _prof.counters.end();)
			{
				const usize	count = Min( _prof.counters.size() - sent, step+1 );

				auto	msg = _msgProducer->CreateMsg< CSMsg_IntelProf_Sample >( SizeOf<KeyVal> * (count-1) );
				if ( not msg )
					break;

				msg->index	= _prof.index;
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

			_prof.index ++;
		}
	}

#endif // AE_PLATFORM_LINUX
//-----------------------------------------------------------------------------



/*
 =================================================
	constructor
 =================================================
 */
	IntelProfilerClient::IntelProfilerClient (RC<MsgProducer> mp) __NE___ :
		_msgProducer{ RVRef(mp) }
	{}

/*
 =================================================
	Initialize
 =================================================
 */
	bool  IntelProfilerClient::Initialize (const ECounterSet &counterSet) __NE___
	{
		CHECK_ERR( counterSet.Any() );

		_requiredCS = counterSet;
		return _Initialize( counterSet );
	}

	bool  IntelProfilerClient::_Initialize (const ECounterSet &counterSet) __NE___
	{
		auto	msg = _msgProducer->CreateMsg< CSMsg_IntelProf_InitReq >();
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
	void  IntelProfilerClient::Deinitialize () __NE___
	{
	}

/*
 =================================================
	_Consume
 =================================================
 */
	void  IntelProfilerClient::_Consume (ChunkList<const CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			ASSERT( msg->GroupId() == CSMessageGroup::Debug );
			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_IntelProf_ ## _name_::UID :	_ ## _name_( *msg->As< CSMsg_IntelProf_ ## _name_ >() );	break;
				CASE( InitRes )
				CASE( NextSample )
				CASE( Sample )
				#undef CASE
			}
		}
	}

/*
 =================================================
	Sample
 =================================================
 */
	void  IntelProfilerClient::Sample (OUT Counters_t &result, INOUT float &invdt) __NE___
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
	_InitRes
 =================================================
 */
	inline void  IntelProfilerClient::_InitRes (CSMsg_IntelProf_InitRes const& msg) __NE___
	{
		EXLOCK( _guard );

		_status		= msg.ok ? EStatus::Initialized : EStatus::NotSupported;
		_enabled	= msg.enabled;
		_hwInfo		= msg.info;

		_connectionLostTimer.Restart();
	}

/*
 =================================================
	_NextSample
 =================================================
 */
	inline void  IntelProfilerClient::_NextSample (CSMsg_IntelProf_NextSample const& msg) __NE___
	{
		EXLOCK( _guard );

		if ( _IsInitialized() )
		{
			_invdt[ _countersIdx & 1 ] = msg.invdt;

			_countersIdx	= (_countersIdx+1) & 1;
			_pendingIdx		= msg.index;

			_connectionLostTimer.Restart();
		}
	}

/*
 =================================================
	_Sample
 =================================================
 */
	inline void  IntelProfilerClient::_Sample (CSMsg_IntelProf_Sample const& msg) __NE___
	{
		EXLOCK( _guard );

		if ( _IsInitialized() and _pendingIdx == msg.index )
		{
			auto&	curr = _counters[ (_countersIdx+1) & 1 ];

			for (uint i = 0, cnt = msg.count; i < cnt; ++i)
				curr.insert_or_assign( msg.arr[i].first, msg.arr[i].second );
		}
	}

/*
 =================================================
	GetMsgConsumer
 =================================================
 */
	RC<IntelProfilerClient::MsgConsumer>  IntelProfilerClient::GetMsgConsumer () __NE___
	{
		return MakeRCNe<Networking::DefaultCSMessageConsumer>( [this](auto ml){ _Consume( ml ); }, CSMessageGroup::Debug );
	}


} // AE::Profiler
