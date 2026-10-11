// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_TRACY
#include "profiler/Impl/HwpcProfiler.h"

namespace AE::Profiler
{

/*
=================================================
	_InitTracy
=================================================
*/
	void  HwpcProfiler::_InitTracy ()
	{
		const bool	per_frame	= not _isRemote;

		_InitArmCountersTracy( per_frame );
		_InitMaliCountersTracy( per_frame );
		_InitAdrenoCountersTracy( per_frame );
		_InitNVidiaCountersTracy( per_frame );
		_InitIntelCountersTracy( per_frame );
		_InitPowerVRCountersTracy( per_frame );
	}
//-----------------------------------------------------------------------------


#if 1
/*
=================================================
	_InitArmCountersTracy
=================================================
*/
	void  HwpcProfiler::_InitArmCountersTracy (bool perFrame)
	{
	}

/*
=================================================
	_UpdateArmCountersTracy
=================================================
*/
	void  HwpcProfiler::_UpdateArmCountersTracy (bool perFrame, float invFC)
	{
	}
#endif
//-----------------------------------------------------------------------------


#if 1
/*
=================================================
	_InitMaliCountersTracy
=================================================
*/
	void  HwpcProfiler::_InitMaliCountersTracy (bool perFrame)
	{
	}

/*
=================================================
	_UpdateMaliCountersTracy
=================================================
*/
	void  HwpcProfiler::_UpdateMaliCountersTracy (bool perFrame, float invFC)
	{
	}
#endif
//-----------------------------------------------------------------------------


#if 1
/*
=================================================
	_InitAdrenoCountersTracy
=================================================
*/
	void  HwpcProfiler::_InitAdrenoCountersTracy (bool perFrame)
	{
	}

/*
=================================================
	_UpdateAdrenoCountersTracy
=================================================
*/
	void  HwpcProfiler::_UpdateAdrenoCountersTracy (bool perFrame, float invFC)
	{
	}
#endif
//-----------------------------------------------------------------------------


#if 1
/*
=================================================
	_InitNVidiaCountersTracy
=================================================
*/
	void  HwpcProfiler::_InitNVidiaCountersTracy (bool perFrame)
	{
	}

/*
=================================================
	_UpdateNVidiaCountersTracy
=================================================
*/
	void  HwpcProfiler::_UpdateNVidiaCountersTracy (bool perFrame, float invFC)
	{
	}
#endif
//-----------------------------------------------------------------------------


#if 1
/*
=================================================
	_InitIntelCountersTracy
=================================================
*/
	void  HwpcProfiler::_InitIntelCountersTracy (bool perFrame)
	{
	}

/*
=================================================
	_UpdateIntelCountersTracy
=================================================
*/
	void  HwpcProfiler::_UpdateIntelCountersTracy (bool perFrame, float invFC)
	{
	}
#endif
//-----------------------------------------------------------------------------


#if 1
/*
=================================================
	_InitPowerVRCountersTracy
=================================================
*/
	void  HwpcProfiler::_InitPowerVRCountersTracy (bool perFrame)
	{
	}

/*
=================================================
	_UpdatePowerVRCountersTracy
=================================================
*/
	void  HwpcProfiler::_UpdatePowerVRCountersTracy (bool perFrame, float invFC)
	{
	}
#endif


} // AE::Profiler
#endif // AE_ENABLE_TRACY
