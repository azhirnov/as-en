// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Controllers/FlightCamera.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	FlightCamera::FlightCamera (RC<DynamicDim> dim, float2 clipPlanes, Rad fovY, float2 engineThrustRange,
								float3 rotationScale, float3 initialPos, bool reverseZ) __Th___ :
		_dynDim{ RVRef(dim) },				_clipPlanes{ clipPlanes },
		_rotationScale{ rotationScale },	_fovY{ fovY },
		_minThrust{ engineThrustRange.x },	_maxThrust{ engineThrustRange.y },
		_initialPos{ initialPos },			_reverseZ{ reverseZ }
	{
		CHECK_THROW( _dynDim );
		_Reset();
	}

/*
=================================================
	ProcessInput
=================================================
*/
	void  FlightCamera::ProcessInput (ActionQueueReader reader, secondsf timeDelta) __NE___
	{
		constexpr auto&	IA		= InputActions::Controller_FlightCamera;
		constexpr auto&	BaseIA	= InputActions::SwitchInputMode;

		packed_float3	move;
		packed_float3	yaw_pitch_roll;
		float			thrust		= 0.f;
		float			zoom		= 0.f;
		bool			reset		= false;
		bool			reset_roll	= false;

		ActionQueueReader::Header	hdr;
		for (; reader.ReadHeader( OUT hdr );)
		{
			StaticAssert( (IA.actionCount - BaseIA.actionCount) == 5 );
			switch ( uint{hdr.name} )
			{
				case IA.FlightCamera_Rotate :
					yaw_pitch_roll += reader.Data<packed_float3>( hdr.offset );		break;

				case IA.FlightCamera_Thrust :
					thrust += reader.Data<float>( hdr.offset );						break;

				case IA.FlightCamera_Zoom :
					zoom += reader.Data<packed_float2>( hdr.offset ).y;				break;

				case IA.FlightCamera_Reset :
					reset = true;													break;

				case IA.FlightCamera_ResetRoll :
					reset_roll = true;												break;
			}
		}


		EXLOCK( _guard );

		if_unlikely( reset )
			return _Reset();

		// TODO: reset_roll

		_engineThrust = Clamp( _engineThrust + thrust, _minThrust, _maxThrust );
		move.x -= _engineThrust * timeDelta.count();

		_camera.Rotate( Rad{yaw_pitch_roll.x * _rotationScale.x},
						Rad{yaw_pitch_roll.y * _rotationScale.y},
						Rad{yaw_pitch_roll.z * _rotationScale.z} );
		_camera.Move( move );

		if_unlikely( IsNotZero( zoom ) or _dynDim->IsChanged( INOUT _dimAspect ))
		{
			_zoom = Clamp( _zoom - zoom * _zoomStep, _3d_minZoom, _3d_maxZoom );
			_camera.SetPerspective( _fovY * _zoom, _dimAspect, _clipPlanes.x, _clipPlanes.y, Bool{_reverseZ} );
		}

		_UpdateMatrix();
	}

/*
=================================================
	_Reset
=================================================
*/
	void  FlightCamera::_Reset ()
	{
		_camera.SetPosition( _initialPos );
		_camera.SetOrientation( Quat::Identity() );
		_zoom			= 1.0f;
		_engineThrust	= 0.f;
		_dimAspect		= _dynDim->Aspect();

		_camera.SetPerspective( _fovY, _dimAspect, _clipPlanes.x, _clipPlanes.y, Bool{_reverseZ} );

		_UpdateMatrix();
	}

/*
=================================================
	_UpdateMatrix
=================================================
*/
	void  FlightCamera::_UpdateMatrix ()
	{
		_view			= _camera.ToViewMatrix();
		_viewProj		= _camera.ToViewProjMatrix();
		_invViewProj	= _viewProj.Inversed();
	}

/*
=================================================
	CopyTo
=================================================
*/
	void  FlightCamera::CopyTo (OUT AE::ShaderTypes::CameraData &camera) C_NE___
	{
		_CopyToCameraData( OUT camera, _camera.Frustum() );
	}

/*
=================================================
	GetHelpText
=================================================
*/
	StringView  FlightCamera::GetHelpText () C_NE___
	{
		return R"(
FlightCamera controls:
  'W' 'S'       - pitch
  'A' 'D' - - - - roll
  'Q' 'E'       - yaw
  'Z' 'C' - - - - decrease/increase engine thrust
  'Mouse'       - vertical: pitch, horizontal: roll
  'Mouse wheel' - zoom
  'T'           - reset roll
  'R'   - - - - - reset position, rotation, zoom
)";
	}


} // AE::ResEditor
