// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Public/Monitor.h"

namespace AE::App
{

/*
=================================================
	Print
=================================================
*/
	void  Monitor::Print () C_NE___
	{
	#ifdef AE_ENABLE_LOGS
		TRY{
			String	str;
			str << "\n  Monitor:      '"s << StringView{name} << "'"
				<< "\n  ID:           " << ToString( uint(id) )
				<< "\n  workArea:     " << ToString( workArea.pixels ) << " pix"
				<< "\n  region:       " << ToString( region.pixels ) << " pix"
				<< "\n  physicalSize: " << ToString( physicalSize.meters ) << " meters"
				<< "\n  density:      " << ToString( ppi ) << " pix/inch, " << ToString( PixelsPerMillimeter() ) << " pix/mm"
				<< "\n  pixel size:   " << ToString( MillimetersPerPixel() * 1000.0f ) << " um"
				<< "\n  PPD on 1m:    " << ToString( MaxPixelsPerDegree( 1.f )) << " pix/deg"
				<< "\n  FOV on 1m:    " << ToString( FieldOfViewDeg( 1.f )) << " deg"
				<< "\n  frequency:    " << ToString( freq ) << " Hz"
				<< "\n  external:     " << ToString( isExternal )
				<< "\n  orientation:  ";

			switch_enum( orient )
			{
				case EOrientation::Default :	str << "default";	break;
				case EOrientation::Sensor :		str << "sensor";	break;
				case EOrientation::Deg_0 :		str << "0 deg";		break;
				case EOrientation::Deg_90 :		str << "90 deg";	break;
				case EOrientation::Deg_180 :	str << "180 deg";	break;
				case EOrientation::Deg_270 :	str << "270 deg";	break;
			}
			switch_end

			if ( not cutout.empty() )
			{
				str << "\n  cutout: {";
				for (auto& r : cutout) {
					str << ToString( r.pixels ) << ", ";
				}
				str.pop_back();
				str.pop_back();
				str << " }";
			}

			// HDRConfig
			str << "\n  hdr.red:      (" << ToString( hdr.red.x   ) << ", " << ToString( hdr.red.y   ) << ")"
				<< "\n  hdr.green:    (" << ToString( hdr.green.x ) << ", " << ToString( hdr.green.y ) << ")"
				<< "\n  hdr.blue:     (" << ToString( hdr.blue.x  ) << ", " << ToString( hdr.blue.y  ) << ")"
				<< "\n  hdr.white:    (" << ToString( hdr.white.x ) << ", " << ToString( hdr.white.y ) << ")";
			str << "\n  hdr.luminance: (avr: " << ToString( hdr.luminance.avr.GetNonScaled() ) << " max: "
					<< ToString( hdr.luminance.max.GetNonScaled() ) << " min: "
					<< ToString( hdr.luminance.min.GetNonScaled() ) << ") cd/m2";
			str << "\n  isTrueHDR:    " << ToString( hdr.IsTrueHDR() );

			AE_LOGI( str );
		}
		CATCH_ALL();
	#endif
	}

/*
=================================================
	SurfaceTransform
=================================================
*/
	ESurfaceTransform  Monitor::SurfaceTransform () C_NE___
	{
		switch_enum( orient )
		{
			case EOrientation::Deg_0 :		return ESurfaceTransform::Deg_0;
			case EOrientation::Deg_90 :		return ESurfaceTransform::Deg_90;
			case EOrientation::Deg_180 :	return ESurfaceTransform::Deg_180;
			case EOrientation::Deg_270 :	return ESurfaceTransform::Deg_270;

			case EOrientation::Default :
			case EOrientation::Sensor :		return ESurfaceTransform::Identity;
		}
		switch_end
		RETURN_ERR( "unknown surface transform" );
	}

/*
=================================================
	FieldOfView
=================================================
*/
	Rad2  Monitor::FieldOfView (float distInMeters) C_NE___
	{
		return Rad2{ ATan( physicalSize.meters.x * 0.5f, distInMeters ),
					 ATan( physicalSize.meters.y * 0.5f, distInMeters )} * 2.0f;
	}

/*
=================================================
	PixelsPerDegree
----
	doesn't have correction for curved screen
=================================================
*/
	float2  Monitor::PixelsPerDegree (float distInMeters) C_NE___
	{
		return RegionSize() / FieldOfViewDeg( distInMeters );
	}


} // AE::App
