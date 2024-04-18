// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Controllers/FPVCamera.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	FPVCamera::FPVCamera (RC<DynamicDim> dim, float2 clipPlanes, Rad fovY, const MovingScale &movingScale,
						  float2 rotationScale, float3 initialPos, bool reverseZ) __Th___ :
		_dynDim{ RVRef(dim) },				_clipPlanes{ clipPlanes },	_fovY{ fovY },
		_rotationScale{ rotationScale },	_movingScale{ movingScale },
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
	void  FPVCamera::ProcessInput (ActionQueueReader reader, secondsf timeDelta) __NE___
	{
		constexpr auto&	IA		= InputActions::Controller_FPVCamera;
		constexpr auto&	BaseIA	= InputActions::SwitchInputMode;

		float3	move;
		float2	rotation;
		float	zoom		= 0.f;
		bool	reset		= false;
		Quat	rot_quat	{Zero};

		ActionQueueReader::Header	hdr;
		for (; reader.ReadHeader( OUT hdr );)
		{
			StaticAssert( (IA.actionCount - BaseIA.actionCount) == 4 );

			switch ( uint{hdr.name} )
			{
				case IA.Camera_Rotate :
					rotation += reader.DataCopy<float2>( hdr.offset );		break;

				case IA.Camera_Move :
					move += reader.DataCopy<float3>( hdr.offset );			break;

				case IA.Camera_Zoom :
					zoom += reader.Data<packed_float2>( hdr.offset ).y;		break;

				case IA.Camera_Reset :
					reset = true;											break;
			}
		}

		EXLOCK( _guard );

		if_unlikely( reset )
			return _Reset();

		move = _movingScale.Apply( move ) * timeDelta.count();
		rotation *= _rotationScale * _zoom;

		_camera.Rotate( Rad{rotation.x}, Rad{rotation.y} );
		_camera.Move3D( move );

		if_unlikely( IsNotZero( zoom ) or _dynDim->IsChanged( INOUT _dimAspect ))
		{
			zoom  = Clamp( zoom, -2.f, 2.f );
			zoom  = zoom * _zoomSpeed * timeDelta.count();
			_zoom = Clamp( _zoom - zoom, _3d_minZoom, _3d_maxZoom );
			_camera.SetPerspective( _fovY * _zoom, _dimAspect, _clipPlanes.x, _clipPlanes.y, Bool{_reverseZ} );
		}

		_UpdateMatrix();
	}

/*
=================================================
	_Reset
=================================================
*/
	void  FPVCamera::_Reset ()
	{
		_camera.SetPosition( _initialPos );
		_camera.ResetOrientation();
		_zoom		= 1.0f;
		_dimAspect	= _dynDim->Aspect();

		_camera.SetPerspective( _fovY, _dimAspect, _clipPlanes.x, _clipPlanes.y, Bool{_reverseZ} );

		_UpdateMatrix();
	}

/*
=================================================
	_UpdateMatrix
=================================================
*/
	void  FPVCamera::_UpdateMatrix ()
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
	void  FPVCamera::CopyTo (OUT AE::ShaderTypes::CameraData &camera) C_NE___
	{
		_CopyToCameraData( OUT camera, _camera.Frustum() );
	}

/*
=================================================
	GetHelpText
=================================================
*/
	StringView  FPVCamera::GetHelpText () C_NE___
	{
		return R"(
FPVCamera controls:
  'W' 'S'       - move forward/backward
  'A' 'D'       - move left/right
  'LeftShift'   - move down
  'Space'       - move up
  'Mouse'       - rotation
  'Arrows'      - rotation
  'Mouse wheel' - zoom
  'R'           - reset position, rotation, zoom
)";
	}


} // AE::ResEditor
