// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Public/HwCamera.h"
#include "graphics/Private/EnumToString.h"

namespace AE::Base
{
	using namespace AE::App;

	ND_ StringView  ToString (IHwCamera::EFace type)
	{
		switch_enum( type )
		{
			case IHwCamera::EFace::Front :		return "Front";
			case IHwCamera::EFace::Back :		return "Back";
			case IHwCamera::EFace::External :	return "External";
			case IHwCamera::EFace::Unknown :
			case IHwCamera::EFace::_Count :		break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (IHwCamera::EImageFormat type)
	{
		return ToString( Graphics::EPixelFormatExternal(type) );
	}

	ND_ StringView  ToString (IHwCamera::EFocusDistanceCalibration type)
	{
		switch_enum( type )
		{
			case IHwCamera::EFocusDistanceCalibration::Uncalibrated :	return "Uncalibrated";
			case IHwCamera::EFocusDistanceCalibration::Approximate :	return "Approximate";
			case IHwCamera::EFocusDistanceCalibration::Calibrated :		return "Calibrated";
			case IHwCamera::EFocusDistanceCalibration::Unknown :
			case IHwCamera::EFocusDistanceCalibration::_Count :			break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (IHwCamera::EAutoExposureMode type)
	{
		switch_enum( type )
		{
			case IHwCamera::EAutoExposureMode::Off :				return "Off";
			case IHwCamera::EAutoExposureMode::On :					return "On";
			case IHwCamera::EAutoExposureMode::OnAutoFlash :		return "OnAutoFlash";
			case IHwCamera::EAutoExposureMode::OnAlwaysFlash :		return "OnAlwaysFlash";
			case IHwCamera::EAutoExposureMode::OnAutoFlashRedeye :	return "OnAutoFlashRedeye";
			case IHwCamera::EAutoExposureMode::OnExternalFlash :	return "OnExternalFlash";
			case IHwCamera::EAutoExposureMode::Unknown :
			case IHwCamera::EAutoExposureMode::_Count :				break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (IHwCamera::EAutoFocusMode type)
	{
		switch_enum( type )
		{
			case IHwCamera::EAutoFocusMode::Off :				return "Off";
			case IHwCamera::EAutoFocusMode::Auto :				return "Auto";
			case IHwCamera::EAutoFocusMode::Macro :				return "Macro";
			case IHwCamera::EAutoFocusMode::ContinuousVideo :	return "ContinuousVideo";
			case IHwCamera::EAutoFocusMode::ContinuousPicture :	return "ContinuousPicture";
			case IHwCamera::EAutoFocusMode::ExtDoF :			return "ExtDoF";
			case IHwCamera::EAutoFocusMode::Unknown :
			case IHwCamera::EAutoFocusMode::_Count :			break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (IHwCamera::EAutoWhiteBalanceMode type)
	{
		switch_enum( type )
		{
			case IHwCamera::EAutoWhiteBalanceMode::Off :				return "Off";
			case IHwCamera::EAutoWhiteBalanceMode::Auto :				return "Auto";
			case IHwCamera::EAutoWhiteBalanceMode::Incandescent :		return "Incandescent";
			case IHwCamera::EAutoWhiteBalanceMode::Fluorescent :		return "Fluorescent";
			case IHwCamera::EAutoWhiteBalanceMode::WarmFluorescent :	return "WarmFluorescent";
			case IHwCamera::EAutoWhiteBalanceMode::Daylight :			return "Daylight";
			case IHwCamera::EAutoWhiteBalanceMode::CloudyDaylight :		return "CloudyDaylight";
			case IHwCamera::EAutoWhiteBalanceMode::Twilight :			return "Twilight";
			case IHwCamera::EAutoWhiteBalanceMode::Shade :				return "Shade";
			case IHwCamera::EAutoWhiteBalanceMode::Unknown :
			case IHwCamera::EAutoWhiteBalanceMode::_Count :				break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (IHwCamera::EControlAAAMode type)
	{
		switch_enum( type )
		{
			case IHwCamera::EControlAAAMode::Off :					return "Off";
			case IHwCamera::EControlAAAMode::Auto :					return "Auto";
			case IHwCamera::EControlAAAMode::UseSceneMode :			return "UseSceneMode";
			case IHwCamera::EControlAAAMode::OffKeepState :			return "OffKeepState";
			case IHwCamera::EControlAAAMode::UseExtendedSceneMode :	return "UseExtendedSceneMode";
			case IHwCamera::EControlAAAMode::Unknown :
			case IHwCamera::EControlAAAMode::_Count :				break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (IHwCamera::ESceneMode type)
	{
		switch_enum( type )
		{
			case IHwCamera::ESceneMode::Disabled :		return "Disabled";
			case IHwCamera::ESceneMode::FacePriority :	return "FacePriority";
			case IHwCamera::ESceneMode::Action :		return "Action";
			case IHwCamera::ESceneMode::Portrait :		return "Portrait";
			case IHwCamera::ESceneMode::Landscape :		return "Landscape";
			case IHwCamera::ESceneMode::Night :			return "Night";
			case IHwCamera::ESceneMode::NightPortrait :	return "NightPortrait";
			case IHwCamera::ESceneMode::Theatre :		return "Theatre";
			case IHwCamera::ESceneMode::Beach :			return "Beach";
			case IHwCamera::ESceneMode::Snow :			return "Snow";
			case IHwCamera::ESceneMode::Sunset :		return "Sunset";
			case IHwCamera::ESceneMode::Steadyphoto :	return "Steadyphoto";
			case IHwCamera::ESceneMode::Fireworks :		return "Fireworks";
			case IHwCamera::ESceneMode::Sports :		return "Sports";
			case IHwCamera::ESceneMode::Party :			return "Party";
			case IHwCamera::ESceneMode::Candlelight :	return "Candlelight";
			case IHwCamera::ESceneMode::Barcode :		return "Barcode";
			case IHwCamera::ESceneMode::HDR :			return "HDR";
			case IHwCamera::ESceneMode::Unknown :
			case IHwCamera::ESceneMode::_Count :		break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (IHwCamera::EExtendedSceneModeCap type)
	{
		switch_enum( type )
		{
			case IHwCamera::EExtendedSceneModeCap::Disabled :			return "Disabled";
			case IHwCamera::EExtendedSceneModeCap::BokehStillCapture :	return "BokehStillCapture";
			case IHwCamera::EExtendedSceneModeCap::BokehContinuous :	return "BokehContinuous";
			case IHwCamera::EExtendedSceneModeCap::Unknown :
			case IHwCamera::EExtendedSceneModeCap::_Count :				break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (IHwCamera::EDistortionCorrectionMode type)
	{
		switch_enum( type )
		{
			case IHwCamera::EDistortionCorrectionMode::Off :			return "Off";
			case IHwCamera::EDistortionCorrectionMode::Fast :			return "Fast";
			case IHwCamera::EDistortionCorrectionMode::HighQuality :	return "HighQuality";
			case IHwCamera::EDistortionCorrectionMode::Unknown :
			case IHwCamera::EDistortionCorrectionMode::_Count :			break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (IHwCamera::EEdgeMode type)
	{
		switch_enum( type )
		{
			case IHwCamera::EEdgeMode::Off :			return "Off";
			case IHwCamera::EEdgeMode::Fast :			return "Fast";
			case IHwCamera::EEdgeMode::HighQuality :	return "HighQuality";
			case IHwCamera::EEdgeMode::ZeroShutterLag :	return "ZeroShutterLag";
			case IHwCamera::EEdgeMode::Unknown :
			case IHwCamera::EEdgeMode::_Count :			break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (IHwCamera::EOpticalStabilizationMode type)
	{
		switch_enum( type )
		{
			case IHwCamera::EOpticalStabilizationMode::Off :	return "Off";
			case IHwCamera::EOpticalStabilizationMode::On :		return "On";
			case IHwCamera::EOpticalStabilizationMode::Unknown :
			case IHwCamera::EOpticalStabilizationMode::_Count :	break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (IHwCamera::EVideoStabilizationMode type)
	{
		switch_enum( type )
		{
			case IHwCamera::EVideoStabilizationMode::Off :		return "Off";
			case IHwCamera::EVideoStabilizationMode::On :		return "On";
			case IHwCamera::EVideoStabilizationMode::Unknown :
			case IHwCamera::EVideoStabilizationMode::_Count :	break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (IHwCamera::ECapability type)
	{
		switch_enum( type )
		{
			case IHwCamera::ECapability::FlashLight :					return "FlashLight";
			case IHwCamera::ECapability::Lens_Distortion :				return "Lens_Distortion";
			case IHwCamera::ECapability::Lens_Apertures :				return "Lens_Apertures";
			case IHwCamera::ECapability::Lens_IntrinsicCalibration :	return "Lens_IntrinsicCalibration";
			case IHwCamera::ECapability::Lens_FocusDistanceCalibration :return "Lens_FocusDistanceCalibration";
			case IHwCamera::ECapability::Lens_HyperfocalDistance :		return "Lens_HyperfocalDistance";
			case IHwCamera::ECapability::Lens_MinFocusDistance :		return "Lens_MinFocusDistance";
			case IHwCamera::ECapability::Sensor_ExposureTimeRange :		return "Sensor_ExposureTimeRange";
			case IHwCamera::ECapability::Sensor_SensitivityRange :		return "Sensor_SensitivityRange";
			case IHwCamera::ECapability::Sensor_MaxAnalogSensitivity :	return "Sensor_MaxAnalogSensitivity";
			case IHwCamera::ECapability::DepthFormat :					return "DepthFormat";
			case IHwCamera::ECapability::RawFormat :					return "RawFormat";
			case IHwCamera::ECapability::LogicalMultiCamera :			return "LogicalMultiCamera";
			case IHwCamera::ECapability::ManualSensor :					return "ManualSensor";
			case IHwCamera::ECapability::Monochrome :					return "Monochrome";
			case IHwCamera::ECapability::MotionTracking :				return "MotionTracking";
			case IHwCamera::ECapability::ReadSensorSetting :			return "ReadSensorSetting";
			case IHwCamera::ECapability::_Count :						break;
		}
		switch_end
		return "";
	}

	ND_ String  ToString (const IHwCamera::VideoMode &mode)
	{
		return "("s << ToString( mode.dim.x ) << 'x' << ToString( mode.dim.y ) << '/' << ToString( mode.fps.GetNonScaled(), 1 ) << ')';
	}

} // AE::Base
//-----------------------------------------------------------------------------


namespace AE::App
{
	using namespace AE::Serializing;

/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  IHwCamera::CameraStaticInfo::Serialize (Serializer &ser) C_NE___
	{
		StaticAssert( sizeof(lens) == 128 );
		StaticAssert( sizeof(sensor) == 20 );
		StaticAssert( sizeof(control) == 24 );
		StaticAssert64( sizeof(*this) == 256 );

		return	ser( id, face, capabilities ) and
				ser( lens.focalLengths, lens.focusDistCal, lens.hyperfocalDist,
					 lens.minFocusDist, lens.distortion, lens.intrinsicCalibration, lens.apertures,
					 lens.opticalStabilizationModes ) and
				ser( sensor.exposureTimeRange, sensor.sensitivityRange, sensor.maxAnalogSensitivity ) and
				ser( control.zoomRatioRange, control.sceneModes, control.autoExposureModes, control.autoFocusModes,
					 control.autoWhiteBalanceModes, control.aaaModes, control.extSceneModeCaps, control.distortionCorrections,
					 control.edgeModes, control.videoStabilizationModes );
	}

	bool  IHwCamera::CameraStaticInfo::Deserialize (Deserializer &des) __NE___
	{
		return	des( id, face, capabilities ) and
				des( lens.focalLengths, lens.focusDistCal, lens.hyperfocalDist,
					 lens.minFocusDist, lens.distortion, lens.intrinsicCalibration, lens.apertures,
					 lens.opticalStabilizationModes ) and
				des( sensor.exposureTimeRange, sensor.sensitivityRange, sensor.maxAnalogSensitivity ) and
				des( control.zoomRatioRange, control.sceneModes, control.autoExposureModes, control.autoFocusModes,
					 control.autoWhiteBalanceModes, control.aaaModes, control.extSceneModeCaps, control.distortionCorrections,
					 control.edgeModes, control.videoStabilizationModes );
	}

/*
=================================================
	Print
=================================================
*/
	void  IHwCamera::CameraStaticInfo::Print () C_NE___
	{
		TRY{
			StaticAssert( sizeof(lens) == 128 );
			StaticAssert( sizeof(sensor) == 20 );
			StaticAssert( sizeof(control) == 24 );
			StaticAssert64( sizeof(*this) == 256 );

			String	str;
			str << "\nCameraStaticInfo {"
				<< "\n  id:           " << StringView{id}
				<< "\n  face          " << ToString( face )
				<< "\n  capabilities: " << ToString( capabilities, &ToString )
				<< "\n  lens {"
				<< "\n    focalLengths:              " << ToString( lens.focalLengths.ToArrayView(), &ToString );

			if ( capabilities.contains( ECapability::Lens_FocusDistanceCalibration ))
				str << "\n    focusDistCal:              " << ToString( lens.focusDistCal );
			if ( capabilities.contains( ECapability::Lens_HyperfocalDistance ))
				str << "\n    hyperfocalDist:            " << ToString( lens.hyperfocalDist );
			if ( capabilities.contains( ECapability::Lens_MinFocusDistance ))
				str << "\n    minFocusDist:              " << ToString( lens.minFocusDist );
			if ( capabilities.contains( ECapability::Lens_Distortion ))
				str << "\n    distortion:                " << ToString( lens.distortion, &ToString);
			if ( capabilities.contains( ECapability::Lens_IntrinsicCalibration ))
				str << "\n    intrinsicCalibration:      " << ToString( lens.intrinsicCalibration, &ToString );
			if ( capabilities.contains( ECapability::Lens_Apertures ))
				str << "\n    apertures:                 " << ToString( lens.apertures.ToArrayView(), &ToString );
			if ( lens.opticalStabilizationModes.Any() )
				str << "\n    opticalStabilizationModes: " << ToString( lens.opticalStabilizationModes, &ToString );

			str	<< "\n  }"
				<< "\n  sensor {";

			if ( capabilities.contains( ECapability::Sensor_ExposureTimeRange ))
				str << "\n    exposureTimeRange:    " << ToString( sensor.exposureTimeRange );
			if ( capabilities.contains( ECapability::Sensor_SensitivityRange ))
				str << "\n    sensitivityRange:     " << ToString( sensor.sensitivityRange );
			if ( capabilities.contains( ECapability::Sensor_MaxAnalogSensitivity ))
				str << "\n    maxAnalogSensitivity: " << ToString( sensor.maxAnalogSensitivity );

			str	<< "\n  }"
				<< "\n  control {"
				<< "\n    zoomRatioRange:          " << ToString( control.zoomRatioRange );

			if ( control.sceneModes.Any() )
				str << "\n    sceneModes:              " << ToString( control.sceneModes, &ToString );
			if ( control.autoExposureModes.Any() )
				str << "\n    autoExposureModes:       " << ToString( control.autoExposureModes, &ToString );
			if ( control.autoFocusModes.Any() )
				str << "\n    autoFocusModes:          " << ToString( control.autoFocusModes, &ToString );
			if ( control.autoWhiteBalanceModes.Any() )
				str << "\n    autoWhiteBalanceModes:   " << ToString( control.autoWhiteBalanceModes, &ToString );
			if ( control.aaaModes.Any() )
				str << "\n    aaaModes:                " << ToString( control.aaaModes, &ToString );
			if ( control.extSceneModeCaps.Any() )
				str << "\n    extSceneModeCaps:        " << ToString( control.extSceneModeCaps, &ToString );
			if ( control.distortionCorrections.Any() )
				str << "\n    distortionCorrections:   " << ToString( control.distortionCorrections, &ToString );
			if ( control.edgeModes.Any() )
				str << "\n    edgeModes:               " << ToString( control.edgeModes, &ToString );
			if ( control.videoStabilizationModes.Any() )
				str << "\n    videoStabilizationModes: " << ToString( control.videoStabilizationModes, &ToString );

			str	<< "\n  }"
				<< "\n}";

			AE_LOGI( str );
		}
		CATCH_ALL();
	}

/*
=================================================
	Validate
=================================================
*/
	void  IHwCamera::CameraStaticInfo::Validate () __NE___
	{
		CHECK( not id.empty() );

		// lens //
		CHECK( not lens.focalLengths.empty() );

		if ( not capabilities.contains( ECapability::Lens_FocusDistanceCalibration ))
			CHECK( lens.focusDistCal == EFocusDistanceCalibration::Uncalibrated );

		if ( not capabilities.contains( ECapability::Lens_HyperfocalDistance ))
			CHECK( lens.hyperfocalDist.GetNonScaled() == 0.f );

		if ( not capabilities.contains( ECapability::Lens_MinFocusDistance ))
			CHECK( lens.minFocusDist.GetNonScaled() == 0.f );

		if ( capabilities.contains( ECapability::Lens_HyperfocalDistance ) and
			 capabilities.contains( ECapability::Lens_MinFocusDistance ))
			CHECK( lens.hyperfocalDist.GetNonScaled() >= 0.f and lens.hyperfocalDist <= lens.minFocusDist );

		//if ( not capabilities.contains( ECapability::Lens_Distortion ))
		//	CHECK( lens.distortion == Zero );

		//if ( not capabilities.contains( ECapability::Lens_IntrinsicCalibration ))
		//	CHECK( lens.intrinsicCalibration == Zero );

		if ( not capabilities.contains( ECapability::Lens_Apertures ))
			CHECK( lens.apertures.empty() );


		// sensor //
		CHECK( capabilities.contains( ECapability::Sensor_ExposureTimeRange ) == sensor.exposureTimeRange.IsValid() );
		CHECK( capabilities.contains( ECapability::Sensor_SensitivityRange ) == sensor.sensitivityRange.IsValid() );

		if ( not capabilities.contains( ECapability::Sensor_MaxAnalogSensitivity ))
			CHECK( sensor.maxAnalogSensitivity == 0 );


		// control //
		CHECK( control.zoomRatioRange.IsValid() );
		// TODO
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  IHwCamera::CameraInfo::Serialize (Serializer &ser) C_NE___
	{
		return	CameraStaticInfo::Serialize( ser ) and
				ser( videoModeMap );
	}

	bool  IHwCamera::CameraInfo::Deserialize (Deserializer &des) __NE___
	{
		return	CameraStaticInfo::Deserialize( des ) and
				des( OUT videoModeMap );
	}

/*
=================================================
	Print
=================================================
*/
	void  IHwCamera::CameraInfo::Print () C_NE___
	{
		CameraStaticInfo::Print();
		TRY{
			String	str;
			str << "\nCameraInfo {"
				<< "\n  videoModeMap {";

			for (auto& [fmt, arr] : videoModeMap)
			{
				str << "\n    format:   " << ToString( fmt )
					<< "\n    dim_fps:  " << ToString( arr, &ToString );
			}
			str << "\n  }"
				<< "\n}";

			AE_LOGI( str );
		}
		CATCH_ALL();
	}

/*
=================================================
	Validate
=================================================
*/
	void  IHwCamera::CameraInfo::Validate () __NE___
	{
		CameraStaticInfo::Validate();

		// TODO

		for (auto& [fmt, vmode] : videoModeMap)
		{
			std::sort( vmode.begin(), vmode.end(),
					[](auto& lhs, auto &rhs) {
						return Equal( lhs.fps, rhs.fps )	? lhs.dim.x > rhs.dim.x :
															  lhs.fps > rhs.fps;
					});
		}
	}

/*
=================================================
	ChooseVideoMode
=================================================
*/
	bool  IHwCamera::CameraInfo::ChooseVideoMode (const EImageFormat format, INOUT VideoMode &inoutMode) C_NE___
	{
		auto	it = videoModeMap.find( format );
		if ( it == videoModeMap.end() )
			return false;

		usize		i		= 0;
		const auto	req_fps	= Min( inoutMode.fps, it->second.front().fps );
		const auto	req_w	= Max( inoutMode.dim.x, inoutMode.dim.y );

		// find FPS
		for (; i < it->second.size(); ++i)
		{
			if_unlikely( req_fps <= it->second[i].fps )
				break;
		}

		// find match
		const usize	first_match = i;

		for (; i+1 < it->second.size(); ++i)
		{
			const auto&		max = it->second[i];
			const auto&		min = it->second[i+1];

			if_unlikely( req_fps > min.fps )
				break;

			if_unlikely( req_w >= max.dim.x or
						(req_w < max.dim.x and req_w > min.dim.x) )
			{
				inoutMode = max;
				return true;
			}
		}

		// return smallest mode
		if ( (req_w == 0) and (i < it->second.size()) )
		{
			inoutMode = it->second[i];
			return true;
		}

		// return highest mode
		if ( first_match < it->second.size() )
		{
			inoutMode = it->second[ first_match ];
			return true;
		}

		return false;
	}

/*
=================================================
	ChooseFormat
=================================================
*/
	bool  IHwCamera::CameraInfo::ChooseFormat (INOUT EImageFormat &inoutFormat) C_NE___
	{
		const EImageFormat	req_format = inoutFormat;

		if ( videoModeMap.contains( req_format ))
			return true;

		if ( videoModeMap.contains( EImageFormat::YUV_420 ))
		{
			inoutFormat = EImageFormat::YUV_420;
			return true;
		}

		if ( videoModeMap.contains( EImageFormat::Private ))
		{
			inoutFormat = EImageFormat::Private;
			return true;
		}

		if ( videoModeMap.contains( EImageFormat::RawPrivate ))
		{
			inoutFormat = EImageFormat::RawPrivate;
			return true;
		}

		inoutFormat = Default;
		return false;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  IHwCamera::CaptureRequest::Serialize (Serializer &ser) C_NE___
	{
		StaticAssert64( sizeof(*this) == 40 );

		return ser( flashLight, videoStabilization, autoExposure, autoFocus, autoWhiteBalance, aaaMode,
					extSceneMode, distortionCorrection, edgeMode, zoomRatio );
	}

	bool  IHwCamera::CaptureRequest::Deserialize (Deserializer &des) __NE___
	{
		return des( flashLight, videoStabilization, autoExposure, autoFocus, autoWhiteBalance, aaaMode,
					extSceneMode, distortionCorrection, edgeMode, zoomRatio );
	}

/*
=================================================
	Print
=================================================
*/
	void  IHwCamera::CaptureRequest::Print () C_NE___
	{
		TRY{
			StaticAssert64( sizeof(*this) == 40 );

			String	str;
			str << "\nDynamicParams {"
				<< "\n  flashLight:           " << ToString( flashLight )
				<< "\n  opticalStabilization: " << ToString( opticalStabilization )
				<< "\n  videoStabilization:   " << ToString( videoStabilization )
				<< "\n  autoExposure:         " << ToString( autoExposure )
				<< "\n  autoFocus:            " << ToString( autoFocus )
				<< "\n  autoWhiteBalance:     " << ToString( autoWhiteBalance )
				<< "\n  aaaMode:              " << ToString( aaaMode )
				<< "\n  extSceneMode:         " << ToString( extSceneMode )
				<< "\n  distortionCorrection: " << ToString( distortionCorrection )
				<< "\n  edgeMode:             " << ToString( edgeMode )
				<< "\n  zoomRatio:            " << ToString( zoomRatio, 2 )
				<< "\n  aperture:             " << ToString( aperture, 2 )
				<< "\n  focalLength:          " << ToString( focalLength, 2 )
				<< "\n  focusDist:            " << ToString( focusDist, 2 )
				<< "\n}";

			AE_LOGI( str );
		}
		CATCH_ALL();
	}

/*
=================================================
	Validate
=================================================
*/
	void  IHwCamera::CaptureRequest::Validate (const CameraInfo &info) __NE___
	{
		if ( flashLight )
			CHECK( info.capabilities.contains( ECapability::FlashLight ));

		if ( opticalStabilization )
		{}	// TODO

		if ( videoStabilization )
		{}	// TODO

		if ( autoExposure != Default ) {
			CHECK( info.control.autoExposureModes.contains( autoExposure ));
		}

		if ( autoFocus != Default ) {
			CHECK( info.control.autoFocusModes.contains( autoFocus ));
		}

		if ( autoWhiteBalance != Default ) {
			CHECK( info.control.autoWhiteBalanceModes.contains( autoWhiteBalance ));
		}

		if ( aaaMode != Default ) {
			CHECK( info.control.aaaModes.contains( aaaMode ));
		}

		if ( extSceneMode != Default ) {
			CHECK( aaaMode == EControlAAAMode::UseExtendedSceneMode );
			CHECK( info.control.extSceneModeCaps.contains( extSceneMode ));
		}

		if ( distortionCorrection != Default ) {
			CHECK( info.control.distortionCorrections.contains( distortionCorrection ));
		}

		if ( edgeMode != Default ) {
			CHECK( info.control.edgeModes.contains( edgeMode ));
		}

		CHECK( info.control.zoomRatioRange.Contains( zoomRatio ));
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  IHwCamera::CaptureResponse::Serialize (Serializer &) C_NE___
	{
		return false;
	}

	bool  IHwCamera::CaptureResponse::Deserialize (Deserializer &) __NE___
	{
		return false;
	}

/*
=================================================
	Print
=================================================
*/
	void  IHwCamera::CaptureResponse::Print () C_NE___
	{
	}


} // AE::App
