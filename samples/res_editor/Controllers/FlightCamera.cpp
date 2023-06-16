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
								float3 rotationScale, float3 initialPos) __Th___ :
		_dynDim{ RVRef(dim) }, _clipPlanes{clipPlanes}, _rotationScale{rotationScale},
		_fovY{fovY}, _minThrust{engineThrustRange.x}, _maxThrust{engineThrustRange.y},
		_initialPos{initialPos}
	{
		CHECK_THROW( _dynDim );
		Reset();
	}

/*
=================================================
	ProcessInput
=================================================
*/
	void  FlightCamera::ProcessInput (ActionQueueReader reader, secondsf timeDelta) __NE___
	{
		packed_float3	move;
		packed_float3	yaw_pitch_roll;
		float			thrust		= 0.f;
		float			zoom		= 0.f;
		bool			reset		= false;

		ActionQueueReader::Header	hdr;
		for (; reader.ReadHeader( OUT hdr );)
		{
			if_unlikely( hdr.name == InputActionName{"FlightCamera.Rotate"} )
				yaw_pitch_roll += reader.Data<packed_float3>( hdr.offset );

			if_unlikely( hdr.name == InputActionName{"FlightCamera.Thrust"} )
				thrust += reader.Data<float>( hdr.offset );

			if_unlikely( hdr.name == InputActionName{"FlightCamera.Zoom"} )
				zoom += reader.Data<packed_float2>( hdr.offset ).x;

			if_unlikely( hdr.name == InputActionName{"FlightCamera.Reset"} )
				reset = true;
		}


		EXLOCK( _guard );
		
		if_unlikely( reset )
			return Reset();
		
		const float	dt = Min( timeDelta.count(), 1.f/30.f );

		_engineThrust = Clamp( _engineThrust + thrust, _minThrust, _maxThrust );
		move.x -= _engineThrust * dt;
		
		_camera.Rotate( Rad{yaw_pitch_roll.x * _rotationScale.x},
						Rad{yaw_pitch_roll.y * _rotationScale.y},
						Rad{yaw_pitch_roll.z * _rotationScale.z} );
		_camera.Move( move );

		if ( IsNotZero( zoom ) or _dynDim->IsChanged( INOUT _dimAspect ))
		{
			_zoom = Clamp( _zoom + zoom, _minZoom, _maxZoom );
			_camera.SetPerspective( _fovY * _zoom, _dimAspect, _clipPlanes.x, _clipPlanes.y );
		}

		_UpdateMatrix();
	}
	
/*
=================================================
	Reset
=================================================
*/
	void  FlightCamera::Reset () __NE___
	{
		_camera.SetPosition( _initialPos );
		_camera.SetRotation( QuatF::Identity() );
		_zoom			= 1.0f;
		_engineThrust	= 0.f;
		_dimAspect		= _dynDim->Aspect();

		_camera.SetPerspective( _fovY, _dimAspect, _clipPlanes.x, _clipPlanes.y );

		_UpdateMatrix();
	}

/*
=================================================
	_UpdateMatrix
=================================================
*/
	void  FlightCamera::_UpdateMatrix ()
	{
		_view			= _camera.GetCamera().ToViewMatrix();
		_viewProj		= _camera.GetCamera().ToViewProjMatrix();
		_invViewProj	= _viewProj.Inversed();
	}
	
/*
=================================================
	CopyTo
=================================================
*/
	void  FlightCamera::CopyTo (OUT AE::ShaderTypes::CameraData &camera) C_NE___
	{
		_CopyToCameraData( OUT camera, _camera.GetFrustum() );
	}


} // AE::ResEditor
