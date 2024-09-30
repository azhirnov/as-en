// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Remote/Messages.h"

namespace AE::Profiler
{

	//
	// Remote General Profiler Server
	//

	class GeneralProfilerServer
	{
	// types
	public:
		using ECounter			= GeneralProfiler::ECounter;
		using ECounterSet		= GeneralProfiler::ECounterSet;
		using Counters_t		= GeneralProfiler::Counters_t;
		using CpuUsage_t		= GeneralProfiler::CpuUsage_t;
	private:
		using ClientServer_t	= Networking::ClientServerBase;
		using MsgProducer		= Networking::IAsyncCSMessageProducer;

		class MsgConsumer final : public Networking::ICSMessageConsumer
		{
		private:
			GeneralProfilerServer&	_server;
		public:
			MsgConsumer (GeneralProfilerServer &server)					__NE___ : _server{server} {}
			Networking::CSMessageGroupID  GetGroupID ()					C_NE_OV	{ return CSMessageGroup::Debug; }
			void  Consume (ChunkList<const Networking::CSMessagePtr>)	__NE_OV;
		};

		friend class GeneralProfilerClient;
		enum class EStatus : uint
		{
			NotInitialized,
			NotSupported,
			Initialized,
		};


	// variables
	private:
		struct {
			Atomic<EStatus>				status			{EStatus::NotInitialized};
			ubyte						index			= 0;
			GeneralProfiler				profiler;
			GeneralProfiler::Counters_t	counters;
			Timer						timer;
			uint						cpuCoreCount	= 0;
		}							_prof;

		StaticRC<MsgConsumer>		_msgConsumer;
		RC<MsgProducer>				_msgProducer;


	// methods
	public:
		GeneralProfilerServer ()											__NE___ : _msgConsumer{*this} {}

		ND_ bool  Initialize (ClientServer_t &, RC<MsgProducer> mp)			__NE___;
			void  Deinitialize ()											__NE___;

			void  Update ()													__NE___;

	private:
		void  _GenProf_InitReq (Networking::CSMsg_GenProf_InitReq const &)	__NE___;
		void  _SendCpuClusters ()											__NE___;
		void  _SendSamples ()												__NE___;
		void  _SendCpuUsage ()												__NE___;
	};



	//
	// Remote General Profiler Client
	//

	class GeneralProfilerClient final : public EnableRC<GeneralProfilerClient>
	{
	// types
	public:
		using ECounter		= GeneralProfiler::ECounter;
		using ECounterSet	= GeneralProfiler::ECounterSet;
		using Counters_t	= GeneralProfiler::Counters_t;
		using CpuUsage_t	= GeneralProfiler::CpuUsage_t;
		using CpuClusters_t	= GeneralProfiler::CpuClusters_t;

	private:
		using MsgProducer	= Networking::IAsyncCSMessageProducer;
		using MsgConsumer	= Networking::ICSMessageConsumer;
		using EStatus		= GeneralProfilerServer::EStatus;


	// variables
	private:
		mutable Threading::RWSpinLock	_guard;
		RC<MsgProducer>					_msgProducer;

		ubyte							_countersIdx	= 0;
		ubyte							_pendingIdx		= 0;

		Timer							_connectionLostTimer	{seconds{10}};
		ECounterSet						_requiredCS;

		mutable bool					_hasCpuUsage [2]	= {};
		uint							_cpuCoreCount		= 0;
		CpuUsage_t						_userSpace [2];
		CpuUsage_t						_kernelSpace [2];
		CpuClusters_t					_cpuClusters;

		float							_invdt [2]		= {};
		Counters_t						_counters [2];
		ECounterSet						_enabled;
		EStatus							_status			= EStatus::NotInitialized;


	// methods
	public:
		explicit GeneralProfilerClient (RC<MsgProducer> mp)				__NE___;

		ND_ bool  Initialize (const ECounterSet &counterSet)			__NE___;
			void  Deinitialize ()										__NE___;
		ND_ bool  IsInitialized ()										C_NE___	{ SHAREDLOCK( _guard );  return _IsInitialized(); }

			void  Sample (OUT Counters_t &result, INOUT float &invdt)	__NE___;
		ND_ bool  GetUsage (OUT CpuUsage_t &, OUT CpuUsage_t &)			C_NE___;

		ND_ ECounterSet		EnabledCounterSet ()						C_NE___	{ SHAREDLOCK( _guard );  return _enabled; }
		ND_ CpuClusters_t	GetCpuClusters ()							C_NE___	{ SHAREDLOCK( _guard );  return _cpuClusters; }
		ND_ uint			GetCpuCoreCount ()							C_NE___	{ SHAREDLOCK( _guard );  return _cpuCoreCount; }

		ND_ RC<MsgConsumer>  GetMsgConsumer ()							__NE___;


	private:
		ND_ bool  _Initialize (const ECounterSet &counterSet)			__NE___;
			void  _Consume (ChunkList<const Networking::CSMessagePtr>)	__NE___;

		ND_ bool  _IsInitialized ()										C_NE___	{ return _status == EStatus::Initialized; }
		ND_ bool  _IsNotInitialized ()									C_NE___	{ return _status == EStatus::NotInitialized; }

	private:
		void  _InitRes (Networking::CSMsg_GenProf_InitRes const&)		__NE___;
		void  _NextSample (Networking::CSMsg_GenProf_NextSample const&)	__NE___;
		void  _Sample (Networking::CSMsg_GenProf_Sample const&)			__NE___;
		void  _CpuCluster (Networking::CSMsg_GenProf_CpuCluster const&)	__NE___;
		void  _CpuUsage (Networking::CSMsg_GenProf_CpuUsage const&)		__NE___;
	};


} // AE::Profiler
