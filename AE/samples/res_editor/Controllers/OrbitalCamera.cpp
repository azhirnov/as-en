// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Controllers/OrbitalCamera.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	OrbitalCamera::OrbitalCamera (RC<DynamicDim> dim, float2 clipPlanes, Rad fovY,
								  float2 rotationScale, float offsetScale,
								  float3 initialPos, float initialOffset, bool reverseZ) __Th___ :
		_dynDim{ RVRef(dim) },		_clipPlanes{ clipPlanes },
		_fovY{ fovY },				_rotationScale{ rotationScale },
		_offsetScale{ offsetScale },
		_initialPos{ initialPos },	_initialOffset{ initialOffset },
		_reverseZ{ reverseZ }
	{
		CHECK_THROW( _dynDim );
		_Reset();
	}

/*
=================================================
	ProcessInput
=================================================
*/
	void  OrbitalCamera::ProcessInput (ActionQueueReader reader, secondsf timeDelta) __NE___
	{
		constexpr auto&	IA		= InputActions::Controller_Orbital;
		constexpr auto&	BaseIA	= InputActions::SwitchInputMode;

		packed_float3	move;
		packed_float2	rotation;
		float			offset	= 0.f;
		bool			reset	= false;

		ActionQueueReader::Header	hdr;
		for (; reader.ReadHeader( OUT hdr );)
		{
			StaticAssert( (IA.actionCount - BaseIA.actionCount) == 4 );
			switch ( uint{hdr.name} )
			{
				case IA.Camera_Rotate :
					rotation += reader.Data<packed_float2>( hdr.offset );	break;

				case IA.Camera_Move :
					move += reader.Data<packed_float3>( hdr.offset );		break;

				case IA.Camera_Zoom :
					offset += reader.Data<packed_float2>( hdr.offset ).y;	break;

				case IA.Camera_Reset :
					reset = true;											break;
			}
		}


		EXLOCK( _guard );

		if_unlikely( reset )
			return _Reset();

		_camera.Rotate( Rad{rotation.x * _rotationScale.x}, Rad{rotation.y * _rotationScale.y} );
		_camera.AddOffset( offset * _offsetScale * timeDelta.count() );

		if_unlikely( _dynDim->IsChanged( INOUT _dimAspect ))
		{
			_camera.SetPerspective( _fovY, _dimAspect, _clipPlanes.x, _clipPlanes.y, Bool{_reverseZ} );
		}

		_UpdateMatrix();
	}

/*
=================================================
	_Reset
=================================================
*/
	void  OrbitalCamera::_Reset ()
	{
		_camera.SetPosition( _initialPos );
		_camera.ResetOrientation();
		_camera.SetOffset( _initialOffset );

		_dimAspect	= _dynDim->Aspect();

		_camera.SetPerspective( _fovY, _dimAspect, _clipPlanes.x, _clipPlanes.y, Bool{_reverseZ} );

		_UpdateMatrix();
	}

/*
=================================================
	_UpdateMatrix
=================================================
*/
	void  OrbitalCamera::_UpdateMatrix ()
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
	void  OrbitalCamera::CopyTo (OUT AE::ShaderTypes::CameraData &camera) C_NE___
	{
		_CopyToCameraData( OUT camera, _camera.Frustum() );
	}

/*
=================================================
	GetHelpText
=================================================
*/
	StringView  OrbitalCamera::GetHelpText () C_NE___
	{
		return R"(
OrbitalCamera controls:
  'W' 'S'          - move forward/backward
  'A' 'D'  - - - - - move left/right
  'Arrows'         - rotation
  'Left mouse btn' - press to rotate camera
  'Mouse wheel'    - zoom
  'R'  - - - - - - - reset position, rotation, zoom
)";
	}


} // AE::ResEditor
