// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Controllers/FPSCamera.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
    constructor
=================================================
*/
    FPSCamera::FPSCamera (RC<DynamicDim> dim, float2 clipPlanes, Rad fovY, const MovingScale &movingScale,
                          float2 rotationScale, float3 initialPos, bool reverseZ) __Th___ :
        _dynDim{ RVRef(dim) },              _clipPlanes{ clipPlanes },      _fovY{fovY},
        _rotationScale{ rotationScale },    _movingScale{ movingScale },
        _initialPos{ initialPos },          _reverseZ{ reverseZ }
    {
        CHECK_THROW( _dynDim );
        _Reset();
    }

/*
=================================================
    ProcessInput
=================================================
*/
    void  FPSCamera::ProcessInput (ActionQueueReader reader, secondsf timeDelta) __NE___
    {
        constexpr auto& IA      = InputActions::Controller_FPSCamera;
        constexpr auto& BaseIA  = InputActions::SwitchInputMode;

        float3  move;
        float2  rotation;
        float   zoom    = 0.f;
        bool    reset   = false;

        ActionQueueReader::Header   hdr;
        for (; reader.ReadHeader( OUT hdr );)
        {
            StaticAssert( (IA.actionCount - BaseIA.actionCount) == 4 );
            switch ( uint{hdr.name} )
            {
                case IA.Camera_Rotate :
                    rotation += reader.DataCopy<float2>( hdr.offset );      break;

                case IA.Camera_Move :
                    move += reader.DataCopy<float3>( hdr.offset );          break;

                case IA.Camera_Zoom :
                    zoom += reader.Data<packed_float2>( hdr.offset ).y;     break;

                case IA.Camera_Reset :
                    reset = true;                                           break;
            }
        }


        EXLOCK( _guard );

        if_unlikely( reset )
            return _Reset();

        move = _movingScale.Apply( move ) * timeDelta.count();

        rotation *= _rotationScale * _zoom;

        _camera.Rotate( Rad{rotation.x}, Rad{rotation.y} );
        _camera.Move( move );

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
    void  FPSCamera::_Reset ()
    {
        _camera.SetPosition( _initialPos );
        _camera.ResetRotation();
        _zoom       = 1.0f;
        _dimAspect  = _dynDim->Aspect();

        _camera.SetPerspective( _fovY, _dimAspect, _clipPlanes.x, _clipPlanes.y, Bool{_reverseZ} );

        _UpdateMatrix();
    }

/*
=================================================
    _UpdateMatrix
=================================================
*/
    void  FPSCamera::_UpdateMatrix ()
    {
        _view           = _camera.ToViewMatrix();
        _viewProj       = _camera.ToViewProjMatrix();
        _invViewProj    = _viewProj.Inversed();
    }

/*
=================================================
    CopyTo
=================================================
*/
    void  FPSCamera::CopyTo (OUT AE::ShaderTypes::CameraData &camera) C_NE___
    {
        _CopyToCameraData( OUT camera, _camera.Frustum() );
    }

/*
=================================================
    GetHelpText
=================================================
*/
    StringView  FPSCamera::GetHelpText () C_NE___
    {
        return R"(
FPSCamera controls:
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
