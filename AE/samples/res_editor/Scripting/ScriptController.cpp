// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Controllers/ScaleBiasCamera.h"
#include "res_editor/Controllers/TopDownCamera.h"
#include "res_editor/Controllers/OrbitalCamera.h"
#include "res_editor/Controllers/FlightCamera.h"
#include "res_editor/Controllers/FPSCamera.h"
#include "res_editor/Controllers/FPVCamera.h"
#include "res_editor/Controllers/RemoteCamera.h"

namespace AE::ResEditor
{
namespace
{
/*
=================================================
	ScriptBaseController_ToBase
=================================================
*/
	template <typename T>
	static ScriptBaseController*  ScriptBaseController_ToBase (T* ptr)
	{
		StaticAssert( IsBaseOf< ScriptBaseController, T >);

		ScriptBaseControllerPtr	result{ ptr };
		return result.Detach();
	}
}
//-----------------------------------------------------------------------------


	using namespace AE::Scripting;

/*
=================================================
	destructor
=================================================
*/
	ScriptBaseController::~ScriptBaseController ()
	{
		if ( not _controller )
			AE_LOGW( "Unused controller" );
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptBaseController::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptBaseController>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
	}

	template <typename B>
	void  ScriptBaseController::_BindBase (B &binder) __Th___
	{
		using T = typename B::Class_t;

		binder.Operators().ImplCast( &ScriptBaseController_ToBase<T> );

		binder.Comment( "Returns dynamic dimension which is attached to the camera.\n"
						"If not specified then render target size will be used from first pass where camera attached." );
		binder.AddMethod( &ScriptBaseController::_Dimension,	"Dimension",	{} );

		binder.Comment( "Set dynamic dimension for camera.\n"
						"If camera is used in single pass use default value.\n"
						"If camera is used in multiple passes with different resolution then set explicit dimension.\n"
						"When dimension changed camera with perspective projection will be resized for new aspect ratio." );
		binder.AddMethod( &ScriptBaseController::_SetDimension,	"Dimension",	{} );
	}

/*
=================================================
	_SetDimension
=================================================
*/
	void  ScriptBaseController::_SetDimension (const ScriptDynamicDimPtr &dynDim) __Th___
	{
		CHECK_THROW_MSG( dynDim and dynDim->Get() );
		CHECK_THROW_MSG( not _dynamicDim, "Dimension is already set" );

		_dynamicDim = dynDim;
	}

	void  ScriptBaseController::SetDimensionIfNotSet (const ScriptDynamicDimPtr &dynDim) __Th___
	{
		if ( not _dynamicDim )
		{
			CHECK_THROW_MSG( dynDim and dynDim->Get() );

			_dynamicDim = dynDim;
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Bind
=================================================
*/
	void  ScriptControllerScaleBias::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptControllerScaleBias>  binder{ se };
		binder.CreateRef();
		_BindBase( binder );
	}

/*
=================================================
	ToController
=================================================
*/
	RC<IController>  ScriptControllerScaleBias::ToController () __Th___
	{
		CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

		if ( _controller )
			return _controller;

		_controller = MakeRCTh<ScaleBiasCamera>( _dynamicDim->Get() );
		return _controller;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Bind
=================================================
*/
	void  ScriptControllerTopDown::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptControllerTopDown>  binder{ se };
		binder.CreateRef();

		binder.Comment( "Set scale for forward and backward movement." );
		binder.AddMethod( &ScriptControllerTopDown::ForwardBackwardScale1,		"ForwardBackwardScale",	{} );
		binder.AddMethod( &ScriptControllerTopDown::ForwardBackwardScale2,		"ForwardBackwardScale",	{"forward", "backward"} );

		binder.Comment( "Set scale for side (left/right) movement." );
		binder.AddMethod( &ScriptControllerTopDown::SideMovementScale,			"SideMovementScale",	{} );

		binder.Comment( "Set rotation scale for mouse/touches/arrows." );
		binder.AddMethod( &ScriptControllerTopDown::SetRotationScale,			"RotationScale",		{} );

		binder.Comment( "Set initial position" );
		binder.AddMethod( &ScriptControllerTopDown::SetPosition,				"Position",				{} );

		_BindBase( binder );
	}

/*
=================================================
	SetRotationScale
=================================================
*/
	void  ScriptControllerTopDown::SetRotationScale (float value) __Th___
	{
		_rotationScale = value;
	}

/*
=================================================
	ForwardBackwardScale*
=================================================
*/
	void  ScriptControllerTopDown::ForwardBackwardScale1 (float value) __Th___
	{
		_movingScale.forward  = value;
		_movingScale.backward = value;
	}

	void  ScriptControllerTopDown::ForwardBackwardScale2 (float forward, float backward) __Th___
	{
		_movingScale.forward  = forward;
		_movingScale.backward = backward;
	}

/*
=================================================
	SideMovementScale
=================================================
*/
	void  ScriptControllerTopDown::SideMovementScale (float value) __Th___
	{
		_movingScale.side = value;
	}

/*
=================================================
	ToController
=================================================
*/
	RC<IController>  ScriptControllerTopDown::ToController () __Th___
	{
		CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

		if ( _controller )
			return _controller;

		_controller = MakeRCTh<TopDownCamera>( _dynamicDim->Get(), _movingScale, _rotationScale, _initialPos );
		return _controller;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	SetFovY
=================================================
*/
	void  ScriptControllerOrbitalCamera::SetFovY (float value) __Th___
	{
		CHECK_THROW_MSG( value >= 1.0f );
		CHECK_THROW_MSG( value <= 90.0f );

		_fovY = value;
	}

/*
=================================================
	SetClipPlanes
=================================================
*/
	void  ScriptControllerOrbitalCamera::SetClipPlanes1 (float near, float far) __Th___
	{
		CHECK_THROW_MSG( near > 0.0f );
		CHECK_THROW_MSG( near < far );
		CHECK_THROW_MSG( (far - near) > 1.0f );

		_clipPlanes = float2{ near, far };
	}

	void  ScriptControllerOrbitalCamera::SetClipPlanes2 (float near) __Th___
	{
		CHECK_THROW_MSG( near > 0.0f );

		_clipPlanes = float2{ near, Infinity<float>() };
	}

/*
=================================================
	ReverseZ
=================================================
*/
	void  ScriptControllerOrbitalCamera::ReverseZ (bool enable) __Th___
	{
		_reverseZ = enable;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptControllerOrbitalCamera::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptControllerOrbitalCamera>  binder{ se };
		binder.CreateRef();
		_BindBase( binder );

		binder.Comment( "Set field or view on Y axis in radians. On X axis it will be calculate automaticaly by aspect ratio." );
		binder.AddMethod( &ScriptControllerOrbitalCamera::SetFovY,				"FovY",				{} );

		binder.Comment( "Set near and far clip planes." );
		binder.AddMethod( &ScriptControllerOrbitalCamera::SetClipPlanes1,		"ClipPlanes",		{"near", "far"} );

		binder.Comment( "Set near clip plane for infinity projection." );
		binder.AddMethod( &ScriptControllerOrbitalCamera::SetClipPlanes2,		"ClipPlanes",		{"near"} );

		binder.AddMethod( &ScriptControllerOrbitalCamera::ReverseZ,				"ReverseZ",			{} );

		binder.Comment( "Set rotation scale for mouse/touches/arrows." );
		binder.AddMethod( &ScriptControllerOrbitalCamera::SetRotationScale1,	"RotationScale",	{"xy"} );
		binder.AddMethod( &ScriptControllerOrbitalCamera::SetRotationScale2,	"RotationScale",	{"x", "y"} );

		//binder.Comment( "" );
		binder.AddMethod( &ScriptControllerOrbitalCamera::SetOffsetScale,		"OffsetScale",		{} );

		binder.Comment( "Set initial position." );
		binder.AddMethod( &ScriptControllerOrbitalCamera::SetPosition,			"Position",			{} );

		//binder.Comment( "" );
		binder.AddMethod( &ScriptControllerOrbitalCamera::SetOffset,			"Offset",			{} );
	}

/*
=================================================
	ToController
=================================================
*/
	RC<IController>  ScriptControllerOrbitalCamera::ToController () __Th___
	{
		CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

		if ( _controller )
			return _controller;

		_controller = MakeRCTh<OrbitalCamera>( _dynamicDim->Get(), _clipPlanes, Rad::FromDeg( _fovY ),
											   _rotationScale, _offsetScale, _initialPos, _initialOffset,
											   _reverseZ );
		return _controller;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	SetFovY
=================================================
*/
	void  ScriptControllerCamera3D::SetFovY (float value) __Th___
	{
		CHECK_THROW_MSG( value >= 1.0f );
		CHECK_THROW_MSG( value <= 90.0f );

		_fovY = value;
	}

/*
=================================================
	SetClipPlanes
=================================================
*/
	void  ScriptControllerCamera3D::SetClipPlanes1 (float near, float far) __Th___
	{
		CHECK_THROW_MSG( near > 0.0f );
		CHECK_THROW_MSG( near < far );
		CHECK_THROW_MSG( (far - near) > 1.0f );

		_clipPlanes = float2{ near, far };
	}

	void  ScriptControllerCamera3D::SetClipPlanes2 (float near) __Th___
	{
		CHECK_THROW_MSG( near > 0.0f );

		_clipPlanes = float2{ near, Infinity<float>() };
	}

/*
=================================================
	ReverseZ
=================================================
*/
	void  ScriptControllerCamera3D::ReverseZ (bool enable) __Th___
	{
		_reverseZ = enable;
	}

/*
=================================================
	ForwardBackwardScale*
=================================================
*/
	void  ScriptControllerCamera3D::ForwardBackwardScale1 (float value) __Th___
	{
		_movingScale.forward  = value;
		_movingScale.backward = value;
	}

	void  ScriptControllerCamera3D::ForwardBackwardScale2 (float forward, float backward) __Th___
	{
		_movingScale.forward  = forward;
		_movingScale.backward = backward;
	}

/*
=================================================
	UpDownScale*
=================================================
*/
	void  ScriptControllerCamera3D::UpDownScale1 (float value) __Th___
	{
		_movingScale.up   = value;
		_movingScale.down = value;
	}

	void  ScriptControllerCamera3D::UpDownScale2 (float up, float down) __Th___
	{
		_movingScale.up   = up;
		_movingScale.down = down;
	}

/*
=================================================
	SideMovementScale
=================================================
*/
	void  ScriptControllerCamera3D::SideMovementScale (float value) __Th___
	{
		_movingScale.side = value;
	}

/*
=================================================
	_BindCamera3D
=================================================
*/
	template <typename B>
	void  ScriptControllerCamera3D::_BindCamera3D (B &binder) __Th___
	{
		binder.Comment( "Set field or view on Y axis in radians. On X axis it will be calculate automaticaly by aspect ratio." );
		binder.AddMethod( &ScriptControllerCamera3D::SetFovY,				"FovY",				{} );

		binder.Comment( "Set near and far clip planes." );
		binder.AddMethod( &ScriptControllerCamera3D::SetClipPlanes1,		"ClipPlanes",		{"near", "far"} );

		binder.Comment( "Set near clip plane for infinity projection." );
		binder.AddMethod( &ScriptControllerCamera3D::SetClipPlanes2,		"ClipPlanes",		{"near"} );

		binder.AddMethod( &ScriptControllerCamera3D::ReverseZ,				"ReverseZ",			{} );

		binder.Comment( "Set initial position." );
		binder.AddMethod( &ScriptControllerCamera3D::SetPosition,			"Position",			{} );

		_BindBase( binder );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	SetEngineThrustRange
=================================================
*/
	void  ScriptControllerFlightCamera::SetEngineThrustRange (float min, float max) __Th___
	{
		CHECK_THROW_MSG( min < max );
		CHECK_THROW_MSG( max > 0.f );
		CHECK_THROW_MSG( min <= 0.f );

		_engineThrustRange.x = min;
		_engineThrustRange.y = max;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptControllerFlightCamera::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptControllerFlightCamera>  binder{ se };
		binder.CreateRef();

		binder.Comment( "Set rotation scale for mouse/touches/arrows." );
		binder.AddMethod( &ScriptControllerCamera3D::SetRotationScale1,			"RotationScale",	{} );
		binder.AddMethod( &ScriptControllerCamera3D::SetRotationScale3,			"RotationScale",	{"yaw", "pitch", "roll"} );

		binder.AddMethod( &ScriptControllerFlightCamera::SetEngineThrustRange,	"EngineThrust",		{"min", "max"} );

		_BindCamera3D( binder );
	}

/*
=================================================
	ToController
=================================================
*/
	RC<IController>  ScriptControllerFlightCamera::ToController () __Th___
	{
		CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

		if ( _controller )
			return _controller;

		_controller = MakeRCTh<FlightCamera>( _dynamicDim->Get(), _clipPlanes, Rad::FromDeg( _fovY ),
											  _engineThrustRange, _rotationScale, _initialPos, _reverseZ );
		return _controller;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Bind
=================================================
*/
	void  ScriptControllerFPVCamera::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptControllerFPVCamera>  binder{ se };
		binder.CreateRef();

		binder.Comment( "Set scale for forward and backward movement." );
		binder.AddMethod( &ScriptControllerCamera3D::ForwardBackwardScale1,	"ForwardBackwardScale",	{} );
		binder.AddMethod( &ScriptControllerCamera3D::ForwardBackwardScale2,	"ForwardBackwardScale",	{"forward", "backward"} );

		binder.Comment( "Set scale for up and down movement." );
		binder.AddMethod( &ScriptControllerCamera3D::UpDownScale1,			"UpDownScale",			{} );
		binder.AddMethod( &ScriptControllerCamera3D::UpDownScale2,			"UpDownScale",			{"up", "down"} );

		binder.Comment( "Set scale for side (left/right) movement." );
		binder.AddMethod( &ScriptControllerCamera3D::SideMovementScale,		"SideMovementScale",	{} );

		binder.Comment( "Set rotation scale for mouse/touches/arrows." );
		binder.AddMethod( &ScriptControllerCamera3D::SetRotationScale1,		"RotationScale",		{"xy"} );
		binder.AddMethod( &ScriptControllerCamera3D::SetRotationScale2,		"RotationScale",		{"x", "y"} );

		_BindCamera3D( binder );
	}

/*
=================================================
	ToController
=================================================
*/
	RC<IController>  ScriptControllerFPVCamera::ToController () __Th___
	{
		CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

		if ( _controller )
			return _controller;

		_controller = MakeRCTh<FPSCamera>( _dynamicDim->Get(), _clipPlanes, Rad::FromDeg( _fovY ), _movingScale,
										   float2{_rotationScale}, _initialPos, _reverseZ );
		return _controller;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Bind
=================================================
*/
	void  ScriptControllerRemoteCamera::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptControllerRemoteCamera>  binder{ se };
		binder.CreateRef();

		binder.Comment( "Set scale for forward and backward movement." );
		binder.AddMethod( &ScriptControllerCamera3D::ForwardBackwardScale1,	"ForwardBackwardScale",	{} );
		binder.AddMethod( &ScriptControllerCamera3D::ForwardBackwardScale2,	"ForwardBackwardScale",	{"forward", "backward"} );

		binder.Comment( "Set scale for up and down movement." );
		binder.AddMethod( &ScriptControllerCamera3D::UpDownScale1,			"UpDownScale",			{} );
		binder.AddMethod( &ScriptControllerCamera3D::UpDownScale2,			"UpDownScale",			{"up", "down"} );

		binder.Comment( "Set scale for side (left/right) movement." );
		binder.AddMethod( &ScriptControllerCamera3D::SideMovementScale,		"SideMovementScale",	{} );

		_BindCamera3D( binder );
	}

/*
=================================================
	ToController
=================================================
*/
	RC<IController>  ScriptControllerRemoteCamera::ToController () __Th___
	{
		CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

		if ( _controller )
			return _controller;

		_controller = MakeRCTh<RemoteCamera>( _dynamicDim->Get(), _clipPlanes, Rad::FromDeg( _fovY ), _movingScale,
											  _initialPos, _reverseZ );
		return _controller;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Bind
=================================================
*/
	void  ScriptControllerFreeCamera::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptControllerFreeCamera>  binder{ se };
		binder.CreateRef();

		binder.Comment( "Set scale for forward and backward movement." );
		binder.AddMethod( &ScriptControllerCamera3D::ForwardBackwardScale1,	"ForwardBackwardScale",	{} );
		binder.AddMethod( &ScriptControllerCamera3D::ForwardBackwardScale2,	"ForwardBackwardScale",	{"forward", "backward"} );

		binder.Comment( "Set scale for up and down movement." );
		binder.AddMethod( &ScriptControllerCamera3D::UpDownScale1,			"UpDownScale",			{} );
		binder.AddMethod( &ScriptControllerCamera3D::UpDownScale2,			"UpDownScale",			{"up", "down"} );

		binder.Comment( "Set scale for side (left/right) movement." );
		binder.AddMethod( &ScriptControllerCamera3D::SideMovementScale,		"SideMovementScale",	{} );

		binder.Comment( "Set rotation scale for mouse/touches/arrows." );
		binder.AddMethod( &ScriptControllerCamera3D::SetRotationScale1,		"RotationScale",		{"xy"} );
		binder.AddMethod( &ScriptControllerCamera3D::SetRotationScale2,		"RotationScale",		{"x", "y"} );

		_BindCamera3D( binder );
	}

/*
=================================================
	ToController
=================================================
*/
	RC<IController>  ScriptControllerFreeCamera::ToController () __Th___
	{
		CHECK_THROW_MSG( _dynamicDim, "Dimension is not set" );

		if ( _controller )
			return _controller;

		_controller = MakeRCTh<FPVCamera>( _dynamicDim->Get(), _clipPlanes, Rad::FromDeg( _fovY ), _movingScale,
										   float2{_rotationScale}, _initialPos, _reverseZ );
		return _controller;
	}


} // AE::ResEditor
