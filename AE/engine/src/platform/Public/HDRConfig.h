// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "platform/Public/AppEnums.h"

namespace AE::App
{

	//
	// HDR Config
	//

	struct HDRConfig
	{
	// types
	public:
		using Luminance_t	= DefaultPhysicalQuantity<float>::Luminance;


	// variables
	public:
		packed_float2	red;
		packed_float2	green;
		packed_float2	blue;
		packed_float2	white;

		struct {
			Luminance_t		avr;		// Max frame-average luminance data. Some displays can not present all pixels in 'max' luminance.
			Luminance_t		max;
			Luminance_t		min;
		}				luminance;


	// methods
	public:

		ND_ bool  IsTrueHDR ()		C_NE___;
	};



/*
=================================================
	IsTrueHDR
----
	detect OLED, QLED and other HDR displays by luminance config
=================================================
*/
	inline bool  HDRConfig::IsTrueHDR () C_NE___
	{
		if ( luminance.max <= Luminance_t{0.f} )
			return false;	// unknown

		if ( luminance.avr < luminance.max and
			 luminance.min > Luminance_t{0.f} )
			return true;	// may be OLED

		return false;	// non-OLED
	}

} // AE::App
