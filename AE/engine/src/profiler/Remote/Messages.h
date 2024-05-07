// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Utils/ArmProfiler.h"
#include "profiler/Utils/AdrenoProfiler.h"
#include "profiler/Utils/PowerVRProfiler.h"
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
		Profiler::ArmProfiler::ECounterSet	supported;
	);

	DECL_CSMSG( ArmProf_NextSample,  Debug,
		ubyte		index;
		ushort		dtInMs;
	);

	DECL_CSMSG( ArmProf_Sample,  Debug,
		using KeyVal = Pair< Profiler::ArmProfiler::ECounter, double >;
		ubyte		index;
		ubyte		count;
		KeyVal		arr [1];
	);
	//--------------------------------------------------------


	CSMSG_ENC_DEC( ArmProf_InitReq,			enable, updateInterval );
	CSMSG_ENC_DEC( ArmProf_InitRes,			ok, enabled, supported );
	CSMSG_ENC_DEC( ArmProf_NextSample,		index, dtInMs );
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
//-----------------------------------------------------------------------------



	DECL_CSMSG( PVRProf_InitReq,  Debug,
		Profiler::PowerVRProfiler::ECounterSet	enable;
		secondsf								updateInterval;
	);

	DECL_CSMSG( PVRProf_InitRes,  Debug,
		bool									ok;
		Profiler::PowerVRProfiler::ECounterSet	enabled;
		Profiler::PowerVRProfiler::ECounterSet	supported;
	);

	DECL_CSMSG( PVRProf_NextSample,  Debug,
		ubyte		index;
		ushort		dtInMs;
	);

	DECL_CSMSG( PVRProf_Sample,  Debug,
		using KeyVal = Pair< Profiler::PowerVRProfiler::ECounter, float >;
		ubyte		index;
		ubyte		count;
		KeyVal		arr [1];
	);
	//--------------------------------------------------------


	CSMSG_ENC_DEC( PVRProf_InitReq,			enable, updateInterval );
	CSMSG_ENC_DEC( PVRProf_InitRes,			ok, enabled, supported );
	CSMSG_ENC_DEC( PVRProf_NextSample,		index, dtInMs );
	CSMSG_ENC_DEC_EXARRAY( PVRProf_Sample,	count, arr,  AE_ARGS( index, count ));
	//--------------------------------------------------------


	ND_ inline bool  Register_PVRProfiler (MessageFactory &mf) __NE___
	{
		return	mf.Register<
					CSMsg_PVRProf_InitReq,
					CSMsg_PVRProf_InitRes,
					CSMsg_PVRProf_NextSample,
					CSMsg_PVRProf_Sample
				>( False{} );
	}
//-----------------------------------------------------------------------------



	DECL_CSMSG( AdrenoProf_InitReq,  Debug,
		Profiler::AdrenoProfiler::ECounterSet	enable;
		secondsf								updateInterval;
	);

	DECL_CSMSG( AdrenoProf_InitRes,  Debug,
		bool									ok;
		Profiler::AdrenoProfiler::ECounterSet	enabled;
	);

	DECL_CSMSG( AdrenoProf_NextSample,  Debug,
		ubyte		index;
		ushort		dtInMs;
	);

	DECL_CSMSG( AdrenoProf_Sample,  Debug,
		using KeyVal = Pair< Profiler::AdrenoProfiler::ECounter, ulong >;
		ubyte		index;
		ubyte		count;
		KeyVal		arr [1];
	);
	//--------------------------------------------------------


	CSMSG_ENC_DEC( AdrenoProf_InitReq,			enable, updateInterval );
	CSMSG_ENC_DEC( AdrenoProf_InitRes,			ok, enabled );
	CSMSG_ENC_DEC( AdrenoProf_NextSample,		index, dtInMs );
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
//-----------------------------------------------------------------------------


	ND_ inline bool  Register_RemoteProfilers (MessageFactory &mf) __NE___
	{
		return	Register_ArmProfiler( mf )	and
				Register_PVRProfiler( mf )	and
				Register_AdrenoProfiler( mf );
	}


} // AE::Networking
