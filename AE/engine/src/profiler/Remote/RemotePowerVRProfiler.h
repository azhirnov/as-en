// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Remote/Messages.h"

namespace AE::Profiler
{

	//
	// Remote PowerVR Profiler Server
	//

	class PowerVRProfilerServer
	{
	// types
	public:
		using ECounter			= PowerVRProfiler::ECounter;
		using ECounterSet		= PowerVRProfiler::ECounterSet;
		using Counters_t		= PowerVRProfiler::Counters_t;
	private:
		using ClientServer_t	= Networking::ClientServerBase;
		using MsgProducer		= Networking::IAsyncCSMessageProducer;

		class MsgConsumer final : public Networking::ICSMessageConsumer
		{
		private:
			PowerVRProfilerServer&	_server;
		public:
			MsgConsumer (PowerVRProfilerServer &server)					__NE___ : _server{server} {}
			Networking::CSMessageGroupID  GetGroupID ()					C_NE_OV	{ return CSMessageGroup::Debug; }
			void  Consume (ChunkList<const Networking::CSMessagePtr>)	__NE_OV;
		};

		friend class PowerVRProfilerClient;
		enum class EStatus : uint
		{
			NotInitialized,
			NotSupported,
			Initialized,
		};


	// variables
	private:
		struct {
			Atomic<EStatus>					status		{EStatus::NotInitialized};
			ubyte							samplesIdx	= 0;
			ubyte							timingIdx	= 0;
			PowerVRProfiler					profiler;
			PowerVRProfiler::Counters_t		counters;
			PowerVRProfiler::TimeScopeArr_t	timing;
			Timer							timer;
		}							_prof;

		StaticRC<MsgConsumer>		_msgConsumer;
		RC<MsgProducer>				_msgProducer;


	// methods
	public:
		PowerVRProfilerServer ()										__NE___ : _msgConsumer{*this} {}

		ND_ bool  Initialize (ClientServer_t &, RC<MsgProducer> mp)		__NE___;
			void  Deinitialize ()										__NE___;

			void  Update ()												__NE___;

	private:
		void  _InitReq (Networking::CSMsg_PVRProf_InitReq const &)		__NE___;
		void  _UpdatePVRProfiler ()										__NE___;
		void  _SendSamples (milliseconds dt)							__NE___;
		void  _SendTimings ()											__NE___;
	};



	//
	// Remote PowerVR Profiler Client
	//

	class PowerVRProfilerClient final : public EnableRC<PowerVRProfilerClient>
	{
	// types
	public:
		using ECounter		= PowerVRProfiler::ECounter;
		using ECounterSet	= PowerVRProfiler::ECounterSet;
		using Counters_t	= PowerVRProfiler::Counters_t;
		using TimeScopeArr_t= PowerVRProfiler::TimeScopeArr_t;

	private:
		using MsgProducer	= Networking::IAsyncCSMessageProducer;
		using MsgConsumer	= Networking::ICSMessageConsumer;
		using EStatus		= PowerVRProfilerServer::EStatus;


	// variables
	private:
		mutable Threading::RWSpinLock	_guard;
		RC<MsgProducer>					_msgProducer;

		ubyte							_countersIdx			= 0;
		ubyte							_timingsIdx				= 0;
		ubyte							_pendingCountersIdx		= 0;
		ubyte							_pendingTimingsIdx		= 0;

		Timer							_connectionLostTimer	{seconds{10}};
		ECounterSet						_requiredCS;

		float							_invdt [2]		= {};
		Counters_t						_counters [2];
		TimeScopeArr_t					_timings [2];
		ECounterSet						_enabled;
		EStatus							_status					= EStatus::NotInitialized;


	// methods
	public:
		explicit PowerVRProfilerClient (RC<MsgProducer> mp)				__NE___;

		ND_ bool  Initialize (const ECounterSet &counterSet)			__NE___;
			void  Deinitialize ()										__NE___;
		ND_ bool  IsInitialized ()										C_NE___	{ SHAREDLOCK( _guard );  return _IsInitialized(); }

			void  Tick ()												C_NE___	{}
			void  Sample (OUT Counters_t &result, INOUT float &invdt)	__NE___;
			void  ReadTimingData (OUT TimeScopeArr_t &)					__NE___;

		ND_ ECounterSet  EnabledCounterSet ()							C_NE___	{ SHAREDLOCK( _guard );  return _enabled; }

		ND_ RC<MsgConsumer>  GetMsgConsumer ()							__NE___;


	private:
		ND_ bool  _Initialize (const ECounterSet &counterSet)			__NE___;
			void  _Consume (ChunkList<const Networking::CSMessagePtr>)	__NE___;

		ND_ bool  _IsInitialized ()										C_NE___	{ return _status == EStatus::Initialized; }
		ND_ bool  _IsNotInitialized ()									C_NE___	{ return _status == EStatus::NotInitialized; }

	private:
		void  _InitRes (Networking::CSMsg_PVRProf_InitRes const&)		__NE___;
		void  _NextSample (Networking::CSMsg_PVRProf_NextSample const&)	__NE___;
		void  _Sample (Networking::CSMsg_PVRProf_Sample const&)			__NE___;
		void  _Timing (Networking::CSMsg_PVRProf_Timing const&)			__NE___;
	};


} // AE::Profiler
