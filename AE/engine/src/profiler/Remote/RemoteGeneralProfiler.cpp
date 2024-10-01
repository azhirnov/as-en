// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "profiler/Remote/RemoteGeneralProfiler.h"

namespace AE::Profiler
{
	using namespace AE::Networking;

/*
=================================================
	MsgConsumer::Consume
=================================================
*/
	void  GeneralProfilerServer::MsgConsumer::Consume (ChunkList<const Networking::CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_ ## _name_::UID :	_server._ ## _name_( *msg->As< CSMsg_ ## _name_ >() );	break;
				CASE( GenProf_InitReq )
				#undef CASE
			}
		}
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  GeneralProfilerServer::Initialize (ClientServer_t &clientServer, RC<MsgProducer> mp) __NE___
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
	void  GeneralProfilerServer::Deinitialize () __NE___
	{
		_prof.profiler.Deinitialize();
		_prof.status.store( EStatus::NotInitialized );

		_msgProducer = null;
	}

/*
=================================================
	_GenProf_InitReq
=================================================
*/
	void  GeneralProfilerServer::_GenProf_InitReq (CSMsg_GenProf_InitReq const &inMsg) __NE___
	{
		if ( _prof.status.load() != EStatus::NotInitialized )
			return;

		_prof.timer.Start( inMsg.updateInterval );
		_prof.index = 0;

		_prof.profiler.Deinitialize();

		bool	ok = _prof.profiler.Initialize( inMsg.enable );

		if ( ok )
			_SendCpuClusters();

		auto	msg = _msgProducer->CreateMsg< CSMsg_GenProf_InitRes >();
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
	_SendCpuClusters
=================================================
*/
	void  GeneralProfilerServer::_SendCpuClusters () __NE___
	{
		const auto	src_clusters	= _prof.profiler.GetCpuClusters();
		uint		idx				= 0;

		for (auto& src : src_clusters)
		{
			if ( auto msg = _msgProducer->CreateMsg< CSMsg_GenProf_CpuCluster >( StringSizeOf(src.name) ))
			{
				msg->idx			= ubyte(idx);
				msg->logicalCores	= uint(src.logicalCores.to_ulong());

				msg.Put( &CSMsg_GenProf_CpuCluster::name, &CSMsg_GenProf_CpuCluster::length, StringView{src.name} );

				CHECK( _msgProducer->AddMessage( msg ));
			}
			++idx;
		}

		_prof.cpuCoreCount = _prof.profiler.GetCpuCoreCount();
	}

/*
=================================================
	Update
=================================================
*/
	void  GeneralProfilerServer::Update () __NE___
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

		CpuUsage_t	user, kernel;
		const bool	has_cpu_usage = _prof.profiler.GetUsage( OUT user, OUT kernel );

		if ( _prof.counters.empty() and not has_cpu_usage )
			return;

		// send first message
		bool	is_sent = false;
		{
			auto	msg = _msgProducer->CreateMsg< CSMsg_GenProf_NextSample >();
			if ( msg )
			{
				msg->index	= _prof.index;
				msg->invdt	= invdt;

				is_sent = _msgProducer->AddMessage( msg );
			}
		}

		if ( not is_sent )
			return;

		// send payload
		if ( not _prof.counters.empty() )
		{
			using			KeyVal	= CSMsg_GenProf_Sample::KeyVal;
			constexpr usize	step	= usize((NetConfig::TCP_MaxMsgSize - SizeOf<CSMsg_GenProf_Sample>) / SizeOf<KeyVal>);
			usize			sent	= 0;

			for (auto it = _prof.counters.begin(); it != _prof.counters.end();)
			{
				const usize	count = Min( _prof.counters.size() - sent, step+1 );

				auto	msg = _msgProducer->CreateMsg< CSMsg_GenProf_Sample >( SizeOf<KeyVal> * (count-1) );
				if ( not msg )
					break;

				msg->index	= _prof.index;
				msg->count	= ubyte(count);

				auto	it2 = it;
				for (usize i = 0; i < count; ++i, ++it2)
				{
					msg->arr[i].first	= it2->first;
					msg->arr[i].second	= float(it2->second);
				}

				if ( not _msgProducer->AddMessage( msg ))
					break;

				it	 = it2;
				sent += count;
			}
			ASSERT( sent == _prof.counters.size() );
		}

		// cpu usage
		if ( has_cpu_usage )
		{
			const uint	core_cnt = _prof.cpuCoreCount;

			if ( auto msg = _msgProducer->CreateMsg< CSMsg_GenProf_CpuUsage >( SizeOf<float> * (core_cnt-1) ))
			{
				msg->index	= _prof.index;
				msg->count	= ubyte(core_cnt);
				msg->type	= 0;

				MemCopy( OUT msg->arr, user.data(), SizeOf<float> * core_cnt );

				Unused( _msgProducer->AddMessage( msg ));
			}

			if ( auto msg = _msgProducer->CreateMsg< CSMsg_GenProf_CpuUsage >( SizeOf<float> * (core_cnt-1) ))
			{
				msg->index	= _prof.index;
				msg->count	= ubyte(core_cnt);
				msg->type	= 1;

				MemCopy( OUT msg->arr, kernel.data(), SizeOf<float> * core_cnt );

				Unused( _msgProducer->AddMessage( msg ));
			}
		}

		_prof.index ++;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	GeneralProfilerClient::GeneralProfilerClient (RC<MsgProducer> mp) __NE___ :
		_msgProducer{ RVRef(mp) }
	{}

/*
=================================================
	Initialize
=================================================
*/
	bool  GeneralProfilerClient::Initialize (const ECounterSet &counterSet) __NE___
	{
		CHECK_ERR( counterSet.Any() );

		_requiredCS = counterSet;
		return _Initialize( counterSet );
	}

	bool  GeneralProfilerClient::_Initialize (const ECounterSet &counterSet) __NE___
	{
		auto	msg = _msgProducer->CreateMsg< CSMsg_GenProf_InitReq >();
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
	void  GeneralProfilerClient::Deinitialize () __NE___
	{
	}

/*
=================================================
	_Consume
=================================================
*/
	void  GeneralProfilerClient::_Consume (ChunkList<const CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			ASSERT( msg->GroupId() == CSMessageGroup::Debug );
			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_GenProf_ ## _name_::UID :	_ ## _name_( *msg->As< CSMsg_GenProf_ ## _name_ >() );	break;
				CASE( InitRes )
				CASE( NextSample )
				CASE( Sample )
				CASE( CpuCluster )
				CASE( CpuUsage )
				#undef CASE
			}
		}
	}

/*
=================================================
	Sample
=================================================
*/
	void  GeneralProfilerClient::Sample (OUT Counters_t &result, INOUT float &invdt) __NE___
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
	GetUsage
=================================================
*/
	bool  GeneralProfilerClient::GetUsage (OUT CpuUsage_t &user, OUT CpuUsage_t &kernel) C_NE___
	{
		EXLOCK( _guard );

		const uint	idx = _countersIdx & 1;

		if ( _IsNotInitialized() or not _hasCpuUsage[idx] )
			return false;

		const auto&		src_user	= _userSpace[idx];
		const auto&		src_kernel	= _kernelSpace[idx];

		MemCopy( OUT user, src_user );
		MemCopy( OUT kernel, src_kernel );

		_hasCpuUsage[idx] = false;
		return true;
	}

/*
=================================================
	_InitRes
=================================================
*/
	inline void  GeneralProfilerClient::_InitRes (CSMsg_GenProf_InitRes const& msg) __NE___
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
	inline void  GeneralProfilerClient::_NextSample (CSMsg_GenProf_NextSample const& msg) __NE___
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
	inline void  GeneralProfilerClient::_Sample (CSMsg_GenProf_Sample const& msg) __NE___
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
	_CpuCluster
=================================================
*/
	inline void  GeneralProfilerClient::_CpuCluster (Networking::CSMsg_GenProf_CpuCluster const& msg) __NE___
	{
		EXLOCK( _guard );
		CHECK_ERRV( msg.idx < _cpuClusters.capacity() );

		_cpuClusters.resize( Max( _cpuClusters.size(), msg.idx+1u ));

		auto&	dst = _cpuClusters[ msg.idx ];
		dst.name			= StringView{ msg.name, msg.length };
		dst.logicalCores	= msg.logicalCores;

		_cpuCoreCount = 0;
		for (auto& cluster : _cpuClusters)
			_cpuCoreCount += uint(cluster.logicalCores.count());
	}

/*
=================================================
	_CpuUsage
=================================================
*/
	inline void  GeneralProfilerClient::_CpuUsage (Networking::CSMsg_GenProf_CpuUsage const& msg) __NE___
	{
		EXLOCK( _guard );

		if ( _IsInitialized() and _pendingIdx == msg.index )
		{
			const uint	idx		= (_countersIdx+1) & 1;
			auto&		curr	= (msg.type == 0 ? _userSpace[idx] : _kernelSpace[idx]);

			ASSERT_Eq( _cpuCoreCount, msg.count );

			for (uint i = 0, cnt = msg.count; i < cnt; ++i)
				curr[i] = msg.arr[i];

			_hasCpuUsage[idx] = true;
		}
	}

/*
=================================================
	GetMsgConsumer
=================================================
*/
	RC<GeneralProfilerClient::MsgConsumer>  GeneralProfilerClient::GetMsgConsumer () __NE___
	{
		return MakeRCNe<Networking::DefaultCSMessageConsumer>( [this](auto ml){ _Consume( ml ); }, CSMessageGroup::Debug );
	}


} // AE::Profiler
