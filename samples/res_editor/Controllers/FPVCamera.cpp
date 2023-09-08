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
                                  float2 rotationScale, float3 initialPos) __Th___ :
        _dynDim{ RVRef(dim) }, _clipPlanes{clipPlanes}, _fovY{fovY},
        _rotationScale{rotationScale}, _movingScale{movingScale},
        _initialPos{initialPos}
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
        constexpr auto& IA      = InputActions::Controller_FPVCamera;
        constexpr auto& BaseIA  = InputActions::SwitchInputMode;

        packed_float3   move;
        packed_float2   rotation;
        float           zoom    = 0.f;
        bool            reset   = false;

        ActionQueueReader::Header   hdr;
        for (; reader.ReadHeader( OUT hdr );)
        {
            STATIC_ASSERT( (IA.actionCount - BaseIA.actionCount) == 4 );
            switch ( uint{hdr.name} )
            {
                case IA.Camera_Rotate :
                    rotation += reader.Data<packed_float2>( hdr.offset );   break;

                case IA.Camera_Move :
                    move += reader.Data<packed_float3>( hdr.offset );       break;

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

        _camera.Rotate( Rad{rotation.x * _rotationScale.x}, Rad{rotation.y * _rotationScale.y} );
        _camera.Move3D( move );

        if ( IsNotZero( zoom ) or _dynDim->IsChanged( INOUT _dimAspect ))
        {
            _zoom = Clamp( _zoom - zoom * _zoomStep, _3d_minZoom, _3d_maxZoom );
            _camera.SetPerspective( _fovY * _zoom, _dimAspect, _clipPlanes.x, _clipPlanes.y );
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
        _camera.SetRotation( QuatF::Identity() );
        _zoom       = 1.0f;
        _dimAspect  = _dynDim->Aspect();

        _camera.SetPerspective( _fovY, _dimAspect, _clipPlanes.x, _clipPlanes.y );

        _UpdateMatrix();
    }

/*
=================================================
    _UpdateMatrix
=================================================
*/
    void  FPVCamera::_UpdateMatrix ()
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
