// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/InputSurface.h"
#include "graphics/Public/VideoEnums.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/IDs.h"

namespace AE::App
{

	//
	// Hardware Camera interface
	//

	class IHwCamera : public EnableRC<IHwCamera>
	{
	// types
	public:
		using CameraID = FixedString<64>;

		enum class EFace : ubyte
		{
			Front,
			Back,
			External,
			_Count,
			Unknown		= 0xFF
		};

		enum class EFocusDistanceCalibration : ubyte
		{
			Uncalibrated,
			Approximate,
			Calibrated,
			_Count,
			Unknown		= 0xFF
		};

		enum class EImageFormat : ubyte
		{
			Depth16				= ubyte(Graphics::EPixelFormatExternal::Android_Depth16),
			DepthJPEG			= ubyte(Graphics::EPixelFormatExternal::Android_DepthJPEG),
			DepthPointCloud		= ubyte(Graphics::EPixelFormatExternal::Android_DepthPointCloud),
			JPEG				= ubyte(Graphics::EPixelFormatExternal::Android_JPEG),
			Private				= ubyte(Graphics::EPixelFormatExternal::Android_Private),
			Raw16				= ubyte(Graphics::EPixelFormatExternal::Android_Raw16),
			Raw12				= ubyte(Graphics::EPixelFormatExternal::Android_Raw12),
			Raw10				= ubyte(Graphics::EPixelFormatExternal::Android_Raw10),
			RawPrivate			= ubyte(Graphics::EPixelFormatExternal::Android_RawPrivate),
			NV16				= ubyte(Graphics::EPixelFormatExternal::Android_NV16),
			NV21				= ubyte(Graphics::EPixelFormatExternal::Android_NV21),
			YCBCR_P010			= ubyte(Graphics::EPixelFormatExternal::Android_YCBCR_P010),
			YUV_420				= ubyte(Graphics::EPixelFormatExternal::Android_YUV_420),
			YUV_422				= ubyte(Graphics::EPixelFormatExternal::Android_YUV_422),
			YUV_444				= ubyte(Graphics::EPixelFormatExternal::Android_YUV_444),
			YUY2				= ubyte(Graphics::EPixelFormatExternal::Android_YUY2),
			YV12				= ubyte(Graphics::EPixelFormatExternal::Android_YV12),
			Y8					= ubyte(Graphics::EPixelFormatExternal::Android_Y8),
			Unknown				= 0xFF
		};
		StaticAssert( uint(Graphics::EPixelFormatExternal::_Android_End) == 19 );

		enum class EAutoExposureMode : ubyte
		{
			Off,
			On,
			OnAutoFlash,
			OnAlwaysFlash,
			OnAutoFlashRedeye,
			OnExternalFlash,
			_Count,
			Unknown		= 0xFF
		};

		enum class EAutoFocusMode : ubyte
		{
			Off,
			Auto,
			Macro,
			ContinuousVideo,
			ContinuousPicture,
			ExtDoF,					// extended depth of field
			_Count,
			Unknown		= 0xFF
		};

		enum class EAutoWhiteBalanceMode : ushort
		{
			Off,
			Auto,
			Incandescent,
			Fluorescent,
			WarmFluorescent,
			Daylight,
			CloudyDaylight,
			Twilight,
			Shade,
			_Count,
			Unknown		= 0xFF
		};

		enum class EControlAAAMode : ubyte
		{
			Off,
			Auto,
			UseSceneMode,
			OffKeepState,
			UseExtendedSceneMode,
			_Count,
			Unknown		= 0xFF
		};

		enum class ESceneMode : uint
		{
			Disabled,
			FacePriority,
			Action,
			Portrait,
			Landscape,
			Night,
			NightPortrait,
			Theatre,
			Beach,
			Snow,
			Sunset,
			Steadyphoto,
			Fireworks,
			Sports,
			Party,
			Candlelight,
			Barcode,
			HDR			= 18,
			_Count,
			Unknown		= 0xFF
		};

		enum class EExtendedSceneModeCap : ubyte
		{
			Disabled,
			BokehStillCapture,
			BokehContinuous,
			_Count,
			Unknown		= 0xFF
		};

		enum class EDistortionCorrectionMode : ubyte
		{
			Off,
			Fast,
			HighQuality,
			_Count,
			Unknown		= 0xFF
		};

		enum class EEdgeMode : ubyte
		{
			Off,
			Fast,
			HighQuality,
			ZeroShutterLag,
			_Count,
			Unknown		= 0xFF
		};

		enum class EVideoStabilizationMode : ubyte
		{
			Off,
			On,
			_Count,
			Unknown		= 0xFF
		};

		enum class EOpticalStabilizationMode : ubyte
		{
			Off,
			On,
			_Count,
			Unknown		= 0xFF
		};

		enum class EFlashState : ubyte
		{
			Unavailable,
			Charging,
			Ready,
			Fired,
			Partial,
			_Count,
			Unknown		= 0xFF
		};

		using Meters			= DefaultPhysicalQuantity<float>::Meter;
		using Millimeters		= DefaultPhysicalQuantity<float>::Millimeter;
		using Diopters			= DefaultPhysicalQuantity<float>::Diopter;
		using Nanoseconds		= DefaultPhysicalQuantity<float>::Nanosecond;
		using FramesPerSecond	= DefaultPhysicalQuantity<float>::Frequency;

		enum class ECapability : ubyte
		{
			FlashLight,

			// lens
			Lens_Distortion,
			Lens_Apertures,
			Lens_IntrinsicCalibration,
			Lens_FocusDistanceCalibration,
			Lens_HyperfocalDistance,
			Lens_MinFocusDistance,

			// sensor
			Sensor_ExposureTimeRange,
			Sensor_SensitivityRange,
			Sensor_MaxAnalogSensitivity,

			// capabilities
			DepthFormat,
			RawFormat,
			LogicalMultiCamera,
			ManualSensor,
			Monochrome,
			MotionTracking,
			ReadSensorSetting,

			_Count
		};


		class CameraStaticInfo : public Serializing::ISerializable
		{
		// variables
		public:
			CameraID								id;
			EFace									face						= Default;
			EnumSet< ECapability >					capabilities;

			struct {
				FixedArray< Millimeters, 8 >			focalLengths;
				EFocusDistanceCalibration				focusDistCal				= EFocusDistanceCalibration::Uncalibrated;				// cap: Lens_FocusDistanceCalibration
				Diopters								hyperfocalDist;				// \__ Diopters only if focusDistCal != Uncalibrated	// cap: Lens_HyperfocalDistance
				Diopters								minFocusDist;				// /													// cap: Lens_MinFocusDistance
				StaticArray< float, 5 >					distortion;					// 3 - radial, 2 tangential								// cap: Lens_Distortion
				StaticArray< float, 5 >					intrinsicCalibration;																// cap: Lens_IntrinsicCalibration
				FixedArray< float, 8 >					apertures;					// units: f-number										// cap: Lens_Apertures
				EnumSet< EOpticalStabilizationMode>		opticalStabilizationModes;
			}										lens;

			struct {
				Range< Nanoseconds >					exposureTimeRange;																	// cap: Sensor_ExposureTimeRange
				Range< int >							sensitivityRange;																	// cap: Sensor_SensitivityRange
				int										maxAnalogSensitivity	= 0;														// cap: Sensor_MaxAnalogSensitivity
			}										sensor;

			struct {
				Range< float >							zoomRatioRange;
				EnumSet< ESceneMode >					sceneModes;
				EnumSet< EAutoExposureMode >			autoExposureModes;
				EnumSet< EAutoFocusMode >				autoFocusModes;
				EnumSet< EAutoWhiteBalanceMode >		autoWhiteBalanceModes;
				EnumSet< EControlAAAMode >				aaaModes;
				EnumSet< EExtendedSceneModeCap >		extSceneModeCaps;
				EnumSet< EDistortionCorrectionMode >	distortionCorrections;
				EnumSet< EEdgeMode >					edgeModes;
				EnumSet< EVideoStabilizationMode >		videoStabilizationModes;
			}										control;


		// methods
		public:
			bool  Serialize (Serializing::Serializer &)		C_NE_OV;
			bool  Deserialize (Serializing::Deserializer &)	__NE_OV;
			void  Print ()									C_NE___;
			void  Validate ()								__NE___;
		};


		struct VideoMode
		{
			uint2				dim;	// pixels
			FramesPerSecond		fps;
		};
		using VideoModeMap_t = FlatHashMap< EImageFormat, Array<VideoMode> >;


		class CameraInfo final : public CameraStaticInfo
		{
		// variables
		public:
			VideoModeMap_t		videoModeMap;	// sorted from high 'fps' to low and from high 'dim' to low


		// methods
		public:
			bool  Serialize (Serializing::Serializer &)				C_NE_OV;
			bool  Deserialize (Serializing::Deserializer &)			__NE_OV;
			void  Print ()											C_NE___;
			void  Validate ()										__NE___;

			bool  ChooseFormat (INOUT EImageFormat &)				C_NE___;
			bool  ChooseVideoMode (EImageFormat, INOUT VideoMode &)	C_NE___;
		};


		class CaptureRequest final : public Serializing::ISerializable
		{
		// variables
		public:
			bool						flashLight				= false;		// check cap: FlashLight
			bool						opticalStabilization	= false;
			bool						videoStabilization		= false;
			EAutoExposureMode			autoExposure			= Default;		// one of available modes in 'control.autoExposureModes'
			EAutoFocusMode				autoFocus				= Default;		// one of available modes in 'control.autoFocusModes'
			EAutoWhiteBalanceMode		autoWhiteBalance		= Default;		// one of available modes in 'control.autoWhiteBalanceModes'
			EControlAAAMode				aaaMode					= Default;		// one of available modes in 'control.aaaModes'
			EExtendedSceneModeCap		extSceneMode			= Default;		// one of available modes in 'control.extSceneModes'
			EDistortionCorrectionMode	distortionCorrection	= Default;		// one of available modes in 'control.distortionCorrections'
			EEdgeMode					edgeMode				= Default;		// one of available modes in 'control.edgeModes'
			float						zoomRatio				= 1.f;			// must be in range 'lens.zoomRatioRange'
			float						aperture				= 0.f;			// must be in range 'lens.apertures'
			Millimeters					focalLength;							// must be in range 'lens.focalLengths'
			Diopters					focusDist;								// >= 0


		// methods
		public:
			bool  Serialize (Serializing::Serializer &)			C_NE_OV;
			bool  Deserialize (Serializing::Deserializer &)		__NE_OV;
			void  Print ()										C_NE___;
			void  Validate (const CameraInfo &info)				__NE___;
		};


		class CaptureResponse final : public Serializing::ISerializable
		{
		// variables
		public:
			EFlashState		flashState	= EFlashState::Unavailable;

		// methods
		public:
			bool  Serialize (Serializing::Serializer &)			C_NE_OV;
			bool  Deserialize (Serializing::Deserializer &)		__NE_OV;
			void  Print ()										C_NE___;
		};


		class CameraCapture : public IInputSurface, public EnableRC<CameraCapture>
		{
		// interface
		public:
			virtual bool  UpdateParams (const CaptureRequest &)	__NE___ = 0;
			virtual void  GetLastState (OUT CaptureResponse &)	C_NE___ = 0;
		};


	// interface
	public:


		// Set samplers which can be used for resource creation.
		// See [IInputSurface::SetSamplerCache](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/platform/Public/InputSurface.h)
		//
			virtual void  SetSamplerCache (ArrayView<Graphics::SamplerName>)	__NE___ = 0;


		// TODO
		//
		ND_ virtual Promise< ArrayView< CameraInfo >>  GetInfoAsync ()			__NE___ = 0;


		// TODO
		//
		ND_ virtual Promise< RC< CameraCapture >>	OpenCamera (const CameraID			&id,
																uint2					dim,
																EImageFormat			format,
																const CaptureRequest	&params)	__NE___ = 0;
	};


} // AE::App
