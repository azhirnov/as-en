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
				<< "\nppi:          " << ToString( ppi ) << " pix/inch"
				<< "\nfrequency:    " << ToString( freq ) << " Hz"
				<< "\nexternal:     " << ToString( isExternal )
				<< "\norientation:  ";

			switch_enum( orient )
			{
				case EOrientation::Default :		str << "default";	break;
				case EOrientation::Sensor :			str << "sensor";	break;
				case EOrientation::Orient_0_deg :	str << "0 deg";		break;
				case EOrientation::Orient_90_deg :	str << "90 deg";	break;
				case EOrientation::Orient_180_deg :	str << "180 deg";	break;
				case EOrientation::Orient_270_deg :	str << "270 deg";	break;
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

			str << "\nluminance:    (avr: " << ToString( luminance.avr.GetNonScaled() ) << " max: "
					<< ToString( luminance.max.GetNonScaled() ) << " min: "
					<< ToString( luminance.min.GetNonScaled() ) << " cd/m2)";

			AE_LOGI( str );
		}
		CATCH_ALL();
	#endif
	}

/*
=================================================
	WithRotation
=================================================
*
	Monitor  Monitor::WithRotation () C_NE___
	{
		const int2	size = region.pixels.Size();
		Monitor		res;

		res.id				= id;
		res.workArea		= PixelsRectI{ _Rotate( orient, workArea.pixels, size )};
		res.region			= PixelsRectI{ _Rotate( orient, region.pixels,   size )};
		res.physicalSize	= Meters2f{ _Rotate( orient, physicalSize.meters )};
		res.curvatureRadius	= Meters2f{ _Rotate( orient, curvatureRadius.meters )};
		res.ppi				= ppi;
		res.freq			= freq;
		res.orient			= EOrientation::Orient_0_deg;
		res.isExternal		= isExternal;
		res.name			= name;
		res.native			= native;

		for (auto& r : cutout)
			res.cutout.push_back( PixelsRectI{ _Rotate( orient, r.pixels, size )});

		return res;
	}

/*
=================================================
	RotationMatrix
=================================================
*/
	float2x2  Monitor::RotationMatrix () C_NE___
	{
		switch_enum( orient )
		{
			case EOrientation::Orient_90_deg :	return float2x2{ {-1.f, 0.f}, {0.f,  1.f} };
			case EOrientation::Orient_180_deg :	return float2x2{ {-1.f, 0.f}, {0.f, -1.f} };
			case EOrientation::Orient_270_deg :	return float2x2{ { 1.f, 0.f}, {0.f, -1.f} };

			case EOrientation::Orient_0_deg :
			case EOrientation::Default :
			case EOrientation::Sensor :
			default :							return float2x2{ { 1.f, 0.f}, {0.f,  1.f} };
		}
		switch_end
	}

/*
=================================================
	RotationQuat
=================================================
*/
	Quat  Monitor::RotationQuat () C_NE___
	{
		switch_enum( orient )
		{
			case EOrientation::Orient_90_deg :	return Quat::RotateZ( 90_deg );
			case EOrientation::Orient_180_deg :	return Quat::RotateZ( 180_deg );
			case EOrientation::Orient_270_deg :	return Quat::RotateZ( 270_deg );

			case EOrientation::Orient_0_deg :
			case EOrientation::Default :
			case EOrientation::Sensor :
			default :							return Quat::Identity();
		}
		switch_end
	}


} // AE::App
