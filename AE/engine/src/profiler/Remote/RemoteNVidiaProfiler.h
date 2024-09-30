// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Remote/Messages.h"

namespace AE::Profiler
{

	//
	// Remote NVidia Profiler Server
	//

	class NVidiaProfilerServer
	{
	// types
	public:
		using ECounter			= NVidiaProfiler::ECounter;
		using ECounterSet		= NVidiaProfiler::ECounterSet;
		using Counters_t		= NVidiaProfiler::Counters_t;
	private:
		using ClientServer_t	= Networking::ClientServerBase;
		using MsgProducer		= Networking::IAsyncCSMessageProducer;

		class MsgConsumer final : public Networking::ICSMessageConsumer
		{
		private:
			NVidiaProfilerServer&	_server;
		public:
			MsgConsumer (NVidiaProfilerServer &server)					__NE___ : _server{server} {}
			Networking::CSMessageGroupID  GetGroupID ()					C_NE_OV	{ return CSMessageGroup::Debug; }
			void  Consume (ChunkList<const Networking::CSMessagePtr>)	__NE_OV;
		};

		friend class NVidiaProfilerClient;
		enum class EStatus : uint
		{
			NotInitialized,
			NotSupported,
			Initialized,
		};


	// variables
	private:
		struct {
			Atomic<EStatus>				status		{EStatus::NotInitialized};
			ubyte						index		= 0;
			NVidiaProfiler				profiler;
			NVidiaProfiler::Counters_t	counters;
			Timer						timer;
		}							_prof;

		StaticRC<MsgConsumer>		_msgConsumer;
		RC<MsgProducer>				_msgProducer;


	// methods
	public:
		NVidiaProfilerServer ()													__NE___ : _msgConsumer{*this} {}

		ND_ bool  Initialize (ClientServer_t &, RC<MsgProducer> mp)				__NE___;
			void  Deinitialize ()												__NE___;

			void  Update ()														__NE___;

	private:
		void  _NVidiaProf_InitReq (Networking::CSMsg_NVidiaProf_InitReq const &)__NE___;
		void  _UpdateNVidiaProfiler ()											__NE___;
	};



	//
	// Remote NVidia Profiler Client
	//

	class NVidiaProfilerClient final : public EnableRC<NVidiaProfilerClient>
	{
	// types
	public:
		using ECounter		= NVidiaProfiler::ECounter;
		using ECounterSet	= NVidiaProfiler::ECounterSet;
		using Counters_t	= NVidiaProfiler::Counters_t;
		using HWInfo		= NVidiaProfiler::HWInfo;

	private:
		using MsgProducer	= Networking::IAsyncCSMessageProducer;
		using MsgConsumer	= Networking::ICSMessageConsumer;
		using EStatus		= NVidiaProfilerServer::EStatus;


	// variables
	private:
		mutable Threading::RWSpinLock	_guard;
		RC<MsgProducer>					_msgProducer;

		ubyte							_countersIdx	= 0;
		ubyte							_pendingIdx		= 0;

		Timer							_connectionLostTimer	{seconds{10}};
		ECounterSet						_requiredCS;

		float							_invdt [2]		= {};
		Counters_t						_counters [2];
		ECounterSet						_enabled;
		HWInfo							_hwInfo;
		EStatus							_status			= EStatus::NotInitialized;


	// methods
	public:
		explicit NVidiaProfilerClient (RC<MsgProducer> mp)					__NE___;

		ND_ bool  Initialize (const ECounterSet &counterSet)				__NE___;
			void  Deinitialize ()											__NE___;
		ND_ bool  IsInitialized ()											C_NE___	{ SHAREDLOCK( _guard );  return _IsInitialized(); }

			void  Sample (OUT Counters_t &result, INOUT float &invdt)		__NE___;

		ND_ ECounterSet			EnabledCounterSet ()						C_NE___	{ SHAREDLOCK( _guard );  return _enabled; }
		ND_ HWInfo				GetHWInfo ()								C_NE___	{ SHAREDLOCK( _guard );  return _hwInfo; }
		ND_ RC<MsgConsumer>		GetMsgConsumer ()							__NE___;


	private:
		ND_ bool  _Initialize (const ECounterSet &counterSet)				__NE___;
			void  _Consume (ChunkList<const Networking::CSMessagePtr>)		__NE___;

		ND_ bool  _IsInitialized ()											C_NE___	{ return _status == EStatus::Initialized; }
		ND_ bool  _IsNotInitialized ()										C_NE___	{ return _status == EStatus::NotInitialized; }

	private:
		void  _InitRes (Networking::CSMsg_NVidiaProf_InitRes const&)		__NE___;
		void  _NextSample (Networking::CSMsg_NVidiaProf_NextSample const&)	__NE___;
		void  _Sample (Networking::CSMsg_NVidiaProf_Sample const&)			__NE___;
	};


} // AE::Profiler
