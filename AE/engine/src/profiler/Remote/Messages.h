// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Profilers/ArmProfiler.h"
#include "profiler/Profilers/MaliProfiler.h"
#include "profiler/Profilers/NVidiaProfiler.h"
#include "profiler/Profilers/AdrenoProfiler.h"
#include "profiler/Profilers/PowerVRProfiler.h"
#include "profiler/Profilers/GeneralProfiler.h"
#include "pch/Networking.h"

namespace AE::Networking
{
	DECL_CSMSG( ArmProf_InitReq,  Debug,
		Profiler::ArmProfiler::ECounterSet	enable;
		secondsf							updateInterval;
	);

	DECL_CSMSG( ArmProf_InitRes,  Debug,
		bool								ok;
		Profiler::ArmProfiler::ECounterSet	enabled;
	);

	DECL_CSMSG( ArmProf_NextSample,  Debug,
		ubyte		index;
		float		invdt;	// seconds
	);

	DECL_CSMSG( ArmProf_Sample,  Debug,
		using KeyVal = Pair< Profiler::ArmProfiler::ECounter, slong >;
		ubyte		index;
		ubyte		count;
		KeyVal		arr [1];
	);
	//--------------------------------------------------------


	CSMSG_ENC_DEC( ArmProf_InitReq,			enable, updateInterval );
	CSMSG_ENC_DEC( ArmProf_InitRes,			ok, enabled );
	CSMSG_ENC_DEC( ArmProf_NextSample,		index, invdt );
	CSMSG_ENC_DEC_EXARRAY( ArmProf_Sample,	count, arr,  AE_ARGS( index, count ));
	//--------------------------------------------------------


	ND_ inline bool  Register_ArmProfiler (MessageFactory &mf) __NE___
	{
		return	mf.Register<
					CSMsg_ArmProf_InitReq,
					CSMsg_ArmProf_InitRes,
					CSMsg_ArmProf_NextSample,
					CSMsg_ArmProf_Sample
				>( False{} );
	}
//=============================================================================



	DECL_CSMSG( MaliProf_InitReq,  Debug,
		Profiler::MaliProfiler::ECounterSet	enable;
		secondsf							updateInterval;
	);

	DECL_CSMSG( MaliProf_InitRes,  Debug,
		bool								ok;
		Profiler::MaliProfiler::ECounterSet	enabled;
		Profiler::MaliProfiler::HWInfo		info;
	);

	DECL_CSMSG( MaliProf_NextSample,  Debug,
		ubyte		index;
		float		invdt;	// seconds
	);

	DECL_CSMSG( MaliProf_Sample,  Debug,
		using KeyVal = Pair< Profiler::MaliProfiler::ECounter, double >;
		ubyte		index;
		ubyte		count;
		KeyVal		arr [1];
	);
	//--------------------------------------------------------


	CSMSG_ENC_DEC( MaliProf_InitReq,		enable, updateInterval );
	CSMSG_ENC_DEC( MaliProf_InitRes,		ok, enabled, info );
	CSMSG_ENC_DEC( MaliProf_NextSample,		index, invdt );
	CSMSG_ENC_DEC_EXARRAY( MaliProf_Sample,	count, arr,  AE_ARGS( index, count ));
	//--------------------------------------------------------


	ND_ inline bool  Register_MaliProfiler (MessageFactory &mf) __NE___
	{
		return	mf.Register<
					CSMsg_MaliProf_InitReq,
					CSMsg_MaliProf_InitRes,
					CSMsg_MaliProf_NextSample,
					CSMsg_MaliProf_Sample
				>( False{} );
	}
//=============================================================================



	DECL_CSMSG( PVRProf_InitReq,  Debug,
		Profiler::PowerVRProfiler::ECounterSet	enable;
		secondsf								updateInterval;
	);

	DECL_CSMSG( PVRProf_InitRes,  Debug,
		bool									ok;
		Profiler::PowerVRProfiler::ECounterSet	enabled;
	);

	DECL_CSMSG( PVRProf_NextSample,  Debug,
		ubyte		index;
		float		invdt;	// seconds
	);

	DECL_CSMSG( PVRProf_Sample,  Debug,
		using KeyVal = Pair< Profiler::PowerVRProfiler::ECounter, float >;
		ubyte		index;
		ubyte		count;
		KeyVal		arr [1];
	);

	DECL_CSMSG( PVRProf_Timing,  Debug,
		using TimeScope = Profiler::PowerVRProfiler::TimeScope;
		ubyte		index;
		ubyte		count;
		TimeScope	arr [1];
	);
	//--------------------------------------------------------


	CSMSG_ENC_DEC( PVRProf_InitReq,			enable, updateInterval );
	CSMSG_ENC_DEC( PVRProf_InitRes,			ok, enabled );
	CSMSG_ENC_DEC( PVRProf_NextSample,		index, invdt );
	CSMSG_ENC_DEC_EXARRAY( PVRProf_Sample,	count, arr,  AE_ARGS( index, count ));
	CSMSG_ENC_DEC_EXARRAY( PVRProf_Timing,	count, arr,  AE_ARGS( index, count ));
	//--------------------------------------------------------


	ND_ inline bool  Register_PVRProfiler (MessageFactory &mf) __NE___
	{
		return	mf.Register<
					CSMsg_PVRProf_InitReq,
					CSMsg_PVRProf_InitRes,
					CSMsg_PVRProf_NextSample,
					CSMsg_PVRProf_Sample,
					CSMsg_PVRProf_Timing
				>( False{} );
	}
//=============================================================================



	DECL_CSMSG( AdrenoProf_InitReq,  Debug,
		Profiler::AdrenoProfiler::ECounterSet	enable;
		secondsf								updateInterval;
	);

	DECL_CSMSG( AdrenoProf_InitRes,  Debug,
		bool									ok;
		Profiler::AdrenoProfiler::ECounterSet	enabled;
		Profiler::AdrenoProfiler::HWInfo		info;
	);

	DECL_CSMSG( AdrenoProf_NextSample,  Debug,
		ubyte		index;
		float		invdt;	// seconds
	);

	DECL_CSMSG( AdrenoProf_Sample,  Debug,
		using KeyVal = Pair< Profiler::AdrenoProfiler::ECounter, ulong >;
		ubyte		index;
		ubyte		count;
		KeyVal		arr [1];
	);
	//--------------------------------------------------------


	CSMSG_ENC_DEC( AdrenoProf_InitReq,			enable, updateInterval );
	CSMSG_ENC_DEC( AdrenoProf_InitRes,			ok, enabled, info );
	CSMSG_ENC_DEC( AdrenoProf_NextSample,		index, invdt );
	CSMSG_ENC_DEC_EXARRAY( AdrenoProf_Sample,	count, arr,  AE_ARGS( index, count ));
	//--------------------------------------------------------


	ND_ inline bool  Register_AdrenoProfiler (MessageFactory &mf) __NE___
	{
		return	mf.Register<
					CSMsg_AdrenoProf_InitReq,
					CSMsg_AdrenoProf_InitRes,
					CSMsg_AdrenoProf_NextSample,
					CSMsg_AdrenoProf_Sample
				>( False{} );
	}
//=============================================================================



	DECL_CSMSG( NVidiaProf_InitReq,  Debug,
		Profiler::NVidiaProfiler::ECounterSet	enable;
		secondsf								updateInterval;
	);

	DECL_CSMSG( NVidiaProf_InitRes,  Debug,
		bool									ok;
		Profiler::NVidiaProfiler::ECounterSet	enabled;
	//	Profiler::NVidiaProfiler::HWInfo		info;
	);

	DECL_CSMSG( NVidiaProf_NextSample,  Debug,
		ubyte		index;
		float		invdt;	// seconds
	);

	DECL_CSMSG( NVidiaProf_Sample,  Debug,
		using KeyVal = Pair< Profiler::NVidiaProfiler::ECounter, float >;
		ubyte		index;
		ubyte		count;
		KeyVal		arr [1];
	);
	//--------------------------------------------------------


	CSMSG_ENC_DEC( NVidiaProf_InitReq,			enable, updateInterval );
	CSMSG_ENC_DEC( NVidiaProf_InitRes,			ok, enabled );
	CSMSG_ENC_DEC( NVidiaProf_NextSample,		index, invdt );
	CSMSG_ENC_DEC_EXARRAY( NVidiaProf_Sample,	count, arr,  AE_ARGS( index, count ));
	//--------------------------------------------------------


	ND_ inline bool  Register_NVidiaProfiler (MessageFactory &mf) __NE___
	{
		return	mf.Register<
					CSMsg_NVidiaProf_InitReq,
					CSMsg_NVidiaProf_InitRes,
					CSMsg_NVidiaProf_NextSample,
					CSMsg_NVidiaProf_Sample
				>( False{} );
	}
//=============================================================================



	DECL_CSMSG( GenProf_InitReq,  Debug,
		Profiler::GeneralProfiler::ECounterSet	enable;
		secondsf								updateInterval;
	);

	DECL_CSMSG( GenProf_InitRes,  Debug,
		bool									ok;
		Profiler::GeneralProfiler::ECounterSet	enabled;
	);

	DECL_CSMSG( GenProf_CpuCluster,  Debug,
		ubyte		idx;
		ubyte		length;
		uint		logicalCores;
		char		name [1];
	);

	DECL_CSMSG( GenProf_NextSample,  Debug,
		ubyte		index;
		float		invdt;	// seconds
	);

	DECL_CSMSG( GenProf_Sample,  Debug,
		using KeyVal = Pair< Profiler::GeneralProfiler::ECounter, float >;
		ubyte		index;
		ubyte		count;
		KeyVal		arr [1];
	);

	DECL_CSMSG( GenProf_CpuUsage,  Debug,
		ubyte		index;
		ubyte		count;
		ubyte		type;		// user or kernel
		float		arr [1];
	);
	//--------------------------------------------------------


	CSMSG_ENC_DEC( GenProf_InitReq,				enable, updateInterval );
	CSMSG_ENC_DEC( GenProf_InitRes,				ok, enabled );
	CSMSG_ENC_DEC( GenProf_NextSample,			index, invdt );
	CSMSG_ENC_DEC_EXARRAY( GenProf_Sample,		count, arr,  AE_ARGS( index, count ));
	CSMSG_ENC_DEC_EXARRAY( GenProf_CpuCluster,	length, name,  AE_ARGS( idx, length, logicalCores ));
	CSMSG_ENC_DEC_EXARRAY( GenProf_CpuUsage,	count, arr,  AE_ARGS( index, count, type ));
	//--------------------------------------------------------


	ND_ inline bool  Register_GeneralProfiler (MessageFactory &mf) __NE___
	{
		return	mf.Register<
					CSMsg_GenProf_InitReq,
					CSMsg_GenProf_InitRes,
					CSMsg_GenProf_NextSample,
					CSMsg_GenProf_Sample,
					CSMsg_GenProf_CpuCluster,
					CSMsg_GenProf_CpuUsage
				>( False{} );
	}
//=============================================================================


	ND_ inline bool  Register_RemoteProfilers (MessageFactory &mf) __NE___
	{
		return	Register_ArmProfiler( mf )		and
				Register_AdrenoProfiler( mf )	and
				Register_MaliProfiler( mf )		and
				Register_PVRProfiler( mf )		and
				Register_NVidiaProfiler( mf )	and
				Register_GeneralProfiler( mf );
	}


} // AE::Networking
