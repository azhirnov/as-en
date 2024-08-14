// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "profiler/Remote/RemoteMaliProfiler.h"

namespace AE::Profiler
{
	using namespace AE::Networking;

/*
=================================================
	MsgConsumer::Consume
=================================================
*/
	void  MaliProfilerServer::MsgConsumer::Consume (ChunkList<const Networking::CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_ ## _name_::UID :	_server._ ## _name_( *msg->As< CSMsg_ ## _name_ >() );	break;
				CASE( MaliProf_InitReq )
				#undef CASE
			}
		}
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  MaliProfilerServer::Initialize (ClientServer_t &clientServer, RC<MsgProducer> mp) __NE___
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
	void  MaliProfilerServer::Deinitialize () __NE___
	{
		_prof.profiler.Deinitialize();
		_prof.status.store( EStatus::NotInitialized );

		_msgProducer = null;
	}

/*
=================================================
	_MaliProf_InitReq
=================================================
*/
	void  MaliProfilerServer::_MaliProf_InitReq (CSMsg_MaliProf_InitReq const &inMsg) __NE___
	{
		if ( _prof.status.load() != EStatus::NotInitialized )
			return;

		_prof.timer.Start( inMsg.updateInterval );
		_prof.index = 0;

		_prof.profiler.Deinitialize();

		bool	ok = _prof.profiler.Initialize( inMsg.enable );

		auto	msg = _msgProducer->CreateMsg< CSMsg_MaliProf_InitRes >();
		if ( msg )
		{
			msg->ok = ok;
			if ( ok ) {
				msg->enabled = _prof.profiler.EnabledCounterSet();
				msg->info	 = _prof.profiler.GetHWInfo();
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
	void  MaliProfilerServer::Update () __NE___
	{
		_UpdateMaliProfiler();
	}

/*
=================================================
	_UpdateMaliProfiler
=================================================
*/
	void  MaliProfilerServer::_UpdateMaliProfiler () __NE___
	{
		if ( _prof.status.load() != EStatus::Initialized )
			return;

		MemoryBarrier( EMemoryOrder::Acquire );

		const auto	dt	= _prof.timer.Tick();

		if_likely( not dt )
			return;

		_prof.profiler.Sample( OUT _prof.counters );

		if ( _prof.counters.empty() )
			return;

		// send first message
		bool	is_sent = false;
		{
			auto	msg = _msgProducer->CreateMsg< CSMsg_MaliProf_NextSample >();
			if ( msg )
			{
				msg->index	= _prof.index;
				msg->dtInMs	= ushort(dt.As<milliseconds>().count());

				is_sent = _msgProducer->AddMessage( msg );
			}
		}

		// send payload
		if ( is_sent )
		{
			using			KeyVal	= CSMsg_MaliProf_Sample::KeyVal;
			constexpr usize	step	= usize((NetConfig::TCP_MaxMsgSize - SizeOf<CSMsg_MaliProf_Sample>) / SizeOf<KeyVal>);
			usize			sent	= 0;

			for (auto it = _prof.counters.begin(); it != _prof.counters.end();)
			{
				auto	msg = _msgProducer->CreateMsg< CSMsg_MaliProf_Sample >( SizeOf<KeyVal> * step );
				if ( not msg )
					break;

				const usize	count = Min( _prof.counters.size() - sent, step+1 );

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
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	MaliProfilerClient::MaliProfilerClient (RC<MsgProducer> mp) __NE___ :
		_msgProducer{ RVRef(mp) }
	{}

/*
=================================================
	Initialize
=================================================
*/
	bool  MaliProfilerClient::Initialize (const ECounterSet &counterSet) __NE___
	{
		CHECK_ERR( counterSet.Any() );

		_requiredCS = counterSet;
		return _Initialize( counterSet );
	}

	bool  MaliProfilerClient::_Initialize (const ECounterSet &counterSet) __NE___
	{
		auto	msg = _msgProducer->CreateMsg< CSMsg_MaliProf_InitReq >();
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
	void  MaliProfilerClient::Deinitialize () __NE___
	{
	}

/*
=================================================
	_Consume
=================================================
*/
	void  MaliProfilerClient::_Consume (ChunkList<const CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			ASSERT( msg->GroupId() == CSMessageGroup::Debug );
			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_MaliProf_ ## _name_::UID :	_ ## _name_( *msg->As< CSMsg_MaliProf_ ## _name_ >() );	break;
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
	void  MaliProfilerClient::Sample (OUT Counters_t &result) __NE___
	{
		result.clear();
		EXLOCK( _guard );

		if ( _IsNotInitialized() )
		{
			if_unlikely( _connectionLostTimer.Tick() )
				Unused( _Initialize( _requiredCS ));
			return;
		}

		auto&	curr = _counters[ _countersIdx & 1 ];

		std::swap( result, curr );
		curr.clear();
	}

/*
=================================================
	_InitRes
=================================================
*/
	inline void  MaliProfilerClient::_InitRes (CSMsg_MaliProf_InitRes const& msg) __NE___
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
	inline void  MaliProfilerClient::_NextSample (CSMsg_MaliProf_NextSample const& msg) __NE___
	{
		EXLOCK( _guard );

		if ( _IsInitialized() )
		{
			_countersIdx	= (_countersIdx+1) & 1;
			_pendingIdx		= msg.index;
			_interval		= milliseconds{ msg.dtInMs };

			_connectionLostTimer.Restart();
		}
	}

/*
=================================================
	_Sample
=================================================
*/
	inline void  MaliProfilerClient::_Sample (CSMsg_MaliProf_Sample const& msg) __NE___
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
	RC<MaliProfilerClient::MsgConsumer>  MaliProfilerClient::GetMsgConsumer () __NE___
	{
		return MakeRCNe<Networking::DefaultCSMessageConsumer>( [this](auto ml){ _Consume( ml ); }, CSMessageGroup::Debug );
	}


} // AE::Profiler
