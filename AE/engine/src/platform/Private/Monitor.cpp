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
			str << "\nMonitor:      '"s << StringView{name} << "'"
				<< "\nID:           " << ToString( uint(id) )
				<< "\nworkArea:     " << ToString( workArea.pixels ) << " pix"
				<< "\nregion:       " << ToString( region.pixels ) << " pix"
				<< "\nphysicalSize: " << ToString( physicalSize.meters ) << " meters"
				<< "\ndensity:      " << ToString( ppi ) << " pix/inch, " << ToString( PixelsPerMillimeter() ) << " pix/mm"
				<< "\npixel size:   " << ToString( MillimetersPerPixel() * 1000.0f ) << " um"
				<< "\nPPD on 1m:    " << ToString( MaxPixelsPerDegree( 1.f )) << " pix/deg"
				<< "\nFOV on 1m:    " << ToString( FieldOfViewDeg( 1.f )) << " deg"
				<< "\nfrequency:    " << ToString( freq ) << " Hz"
				<< "\nexternal:     " << ToString( isExternal )
				<< "\norientation:  ";

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
				str << "\ncutout: {";
				for (auto& r : cutout) {
					str << ToString( r.pixels ) << ", ";
				}
				str.pop_back();
				str.pop_back();
				str << " }";
			}

			// HDRConfig
			str << "\nhdr.red:      (" << ToString( hdr.red.x   ) << ", " << ToString( hdr.red.y   ) << ")"
				<< "\nhdr.green:    (" << ToString( hdr.green.x ) << ", " << ToString( hdr.green.y ) << ")"
				<< "\nhdr.blue:     (" << ToString( hdr.blue.x  ) << ", " << ToString( hdr.blue.y  ) << ")"
				<< "\nhdr.white:    (" << ToString( hdr.white.x ) << ", " << ToString( hdr.white.y ) << ")";
			str << "\nhdr.luminance: (avr: " << ToString( hdr.luminance.avr.GetNonScaled() ) << " max: "
					<< ToString( hdr.luminance.max.GetNonScaled() ) << " min: "
					<< ToString( hdr.luminance.min.GetNonScaled() ) << ") cd/m2";
			str << "\nisTrueHDR:    " << ToString( hdr.IsTrueHDR() );

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
