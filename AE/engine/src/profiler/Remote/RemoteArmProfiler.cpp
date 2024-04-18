// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "profiler/Remote/RemoteArmProfiler.h"

namespace AE::Profiler
{
	using namespace AE::Networking;

/*
=================================================
	MsgConsumer::Consume
=================================================
*/
	void  ArmProfilerServer::MsgConsumer::Consume (ChunkList<const Networking::CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_ ## _name_::UID :	_server._ ## _name_( *msg->As< CSMsg_ ## _name_ >() );	break;
				CASE( ArmProf_InitReq )
				#undef CASE
			}
		}
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  ArmProfilerServer::Initialize (ClientServer_t &clientServer, RC<MsgProducer> mp) __NE___
	{
		CHECK_ERR( mp );

		CHECK_ERR( clientServer.Add( _msgConsumer.GetRC() ));

		_msgProducer = RVRef(mp);

		_armProf.initialized.store( false );
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  ArmProfilerServer::Deinitialize () __NE___
	{
		_armProf.profiler.Deinitialize();
		_armProf.initialized.store( false );

		_msgProducer = null;
	}

/*
=================================================
	_ArmProf_InitReq
=================================================
*/
	void  ArmProfilerServer::_ArmProf_InitReq (CSMsg_ArmProf_InitReq const &inMsg) __NE___
	{
		if ( _armProf.initialized.load() )
			return;

		_armProf.timer.Start( inMsg.updateInterval );
		_armProf.index = 0;

		_armProf.profiler.Deinitialize();

		bool	ok = _armProf.profiler.Initialize( inMsg.enable );
		ASSERT( ok );

		auto	msg = _msgProducer->CreateMsg< CSMsg_ArmProf_InitRes >();
		if ( msg )
		{
			msg->ok			= ok;
			msg->enabled	= _armProf.profiler.EnabledCounterSet();
			msg->supported	= _armProf.profiler.SupportedCounterSet();

			CHECK( _msgProducer->AddMessage( msg ));
		}

		_armProf.initialized.store( ok, EMemoryOrder::Release );
	}

/*
=================================================
	Update
=================================================
*/
	void  ArmProfilerServer::Update () __NE___
	{
		_UpdateArmProfiler();
	}

/*
=================================================
	_UpdateArmProfiler
=================================================
*/
	void  ArmProfilerServer::_UpdateArmProfiler () __NE___
	{
		if ( not _armProf.initialized.load() )
			return;

		MemoryBarrier( EMemoryOrder::Acquire );

		const auto	dt	= _armProf.timer.Tick();

		if ( not dt )
			return;

		_armProf.profiler.Sample( OUT _armProf.counters );

		if ( _armProf.counters.empty() )
			return;

		// send first message
		bool	is_sent = false;
		{
			auto	msg = _msgProducer->CreateMsg< CSMsg_ArmProf_NextSample >();
			if ( msg )
			{
				msg->index	= _armProf.index;
				msg->dtInMs	= ushort(dt.As<milliseconds>().count());

				is_sent = _msgProducer->AddMessage( msg );
			}
		}

		// send payload
		if ( is_sent )
		{
			using			KeyVal	= CSMsg_ArmProf_Sample::KeyVal;
			constexpr usize	step	= usize((NetConfig::TCP_MaxMsgSize - SizeOf<CSMsg_ArmProf_Sample>) / SizeOf<KeyVal>);
			usize			sent	= 0;

			for (auto it = _armProf.counters.begin(); it != _armProf.counters.end();)
			{
				auto	msg = _msgProducer->CreateMsg< CSMsg_ArmProf_Sample >( SizeOf<KeyVal> * step );
				if ( not msg )
					break;

				const usize	count = Min( _armProf.counters.size() - sent, step+1 );

				msg->index	= _armProf.index;
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
			ASSERT( sent == _armProf.counters.size() );

			_armProf.index ++;
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ArmProfilerClient::ArmProfilerClient (RC<MsgProducer> mp) __NE___ :
		_msgProducer{ RVRef(mp) }
	{}

/*
=================================================
	Initialize
=================================================
*/
	bool  ArmProfilerClient::Initialize (const ECounterSet &counterSet) __NE___
	{
		_requiredCS = counterSet;
		return _Initialize( counterSet );
	}

	bool  ArmProfilerClient::_Initialize (const ECounterSet &counterSet) __NE___
	{
		auto	msg = _msgProducer->CreateMsg< CSMsg_ArmProf_InitReq >();
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
	void  ArmProfilerClient::Deinitialize () __NE___
	{
	}

/*
=================================================
	_Consume
=================================================
*/
	void  ArmProfilerClient::_Consume (ChunkList<const CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			ASSERT( msg->GroupId() == CSMessageGroup::Debug );
			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_ArmProf_ ## _name_::UID :	_ ## _name_( *msg->As< CSMsg_ArmProf_ ## _name_ >() );	break;
				CASE( InitRes )
				CASE( NextSample )
				CASE( Sample )
				default :					DBG_WARNING( "unknown message id" ); break;
				#undef CASE
			}
		}
	}

/*
=================================================
	Sample
=================================================
*/
	void  ArmProfilerClient::Sample (OUT Counters_t &result) __NE___
	{
		EXLOCK( _guard );

		if ( not _initialized )
		{
			if_unlikely( _connectionLostTimer.Tick() )
				Unused( _Initialize( _requiredCS ));
			return;
		}

		auto&	curr = _counters[ _completeIdx & 1 ];

		std::swap( result, curr );
		curr.clear();
	}

/*
=================================================
	_InitRes
=================================================
*/
	inline void  ArmProfilerClient::_InitRes (CSMsg_ArmProf_InitRes const& msg) __NE___
	{
		EXLOCK( _guard );

		_initialized	= msg.ok;
		_enabled		= msg.enabled;
		_supported		= msg.supported;

		_connectionLostTimer.Restart();
	}

/*
=================================================
	_NextSample
=================================================
*/
	inline void  ArmProfilerClient::_NextSample (CSMsg_ArmProf_NextSample const& msg) __NE___
	{
		EXLOCK( _guard );

		if ( _initialized )
		{
			_pendingIdx	= msg.index;
			_interval	= milliseconds{ msg.dtInMs };

			_connectionLostTimer.Restart();
		}
	}

/*
=================================================
	_Sample
=================================================
*/
	inline void  ArmProfilerClient::_Sample (CSMsg_ArmProf_Sample const& msg) __NE___
	{
		EXLOCK( _guard );

		if ( _initialized and _pendingIdx == msg.index )
		{
			auto&	curr = _counters[ _pendingIdx & 1 ];

			for (uint i = 0, cnt = msg.count; i < cnt; ++i)
				curr.insert_or_assign( msg.arr[i].first, msg.arr[i].second );
		}
	}

/*
=================================================
	GetMsgConsumer
=================================================
*/
	RC<ArmProfilerClient::MsgConsumer>  ArmProfilerClient::GetMsgConsumer () __NE___
	{
		return MakeRCNe<Networking::DefaultCSMessageConsumer>( [this](auto ml){ _Consume( ml ); }, CSMessageGroup::Debug );
	}


} // AE::Profiler
