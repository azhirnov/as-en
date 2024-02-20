// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Controllers/RemoteCamera.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
    constructor
=================================================
*/
    RemoteCamera::RemoteCamera (RC<DynamicDim> dim, float2 clipPlanes, Rad fovY, const MovingScale &movingScale,
                                float3 initialPos, bool reverseZ) __Th___ :
        _dynDim{ RVRef(dim) },          _clipPlanes{ clipPlanes },  _fovY{ fovY },
        _movingScale{ movingScale },    _initialPos{ initialPos },  _reverseZ{ reverseZ }
    {
        CHECK_THROW( _dynDim );
        _Reset();
    }

/*
=================================================
    ProcessInput
=================================================
*/
    void  RemoteCamera::ProcessInput (ActionQueueReader reader, secondsf timeDelta) __NE___
    {
        constexpr auto& IA      = InputActions::Controller_RemoteCamera;
        constexpr auto& BaseIA  = InputActions::SwitchInputMode;

        float3  move;
        bool    reset       = false;
        Quat    rot_quat    {Zero};
        float   gravity     = 0.f;

        ActionQueueReader::Header   hdr;
        for (; reader.ReadHeader( OUT hdr );)
        {
            StaticAssert( (IA.actionCount - BaseIA.actionCount) == 2 );
            StaticAssert( IA.GLFW.actionCount == 2 );

            switch ( uint{hdr.name} )
            {
                case IA.RmCamera_Move :
                    move += reader.DataCopy<float3>( hdr.offset );  break;

                case IA.RmCamera_Reset :
                    reset = true;                                   break;

                case IA.GLFW.RmCamera_Rotate :
                    rot_quat = reader.DataCopy<Quat>( hdr.offset ); break;

                case IA.GLFW.RmCamera_Gravity :
                    gravity = reader.Data<float>( hdr.offset );     break;
            }
        }

        EXLOCK( _guard );

        if_unlikely( reset )
            return _Reset();

        move = _movingScale.Apply( move ) * timeDelta.count();

        if ( rot_quat.LengthSq() > 0.f )
            _camera.SetOrientation( rot_quat );

        _camera.Move3D( move );

        _UpdateMatrix();
    }

/*
=================================================
    _Reset
=================================================
*/
    void  RemoteCamera::_Reset ()
    {
        _camera.SetPosition( _initialPos );
        _camera.ResetOrientation();
        _dimAspect  = _dynDim->Aspect();

        _camera.SetPerspective( _fovY, _dimAspect, _clipPlanes.x, _clipPlanes.y, Bool{_reverseZ} );

        _UpdateMatrix();
    }

/*
=================================================
    _UpdateMatrix
=================================================
*/
    void  RemoteCamera::_UpdateMatrix ()
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
    void  RemoteCamera::CopyTo (OUT AE::ShaderTypes::CameraData &camera) C_NE___
    {
        _CopyToCameraData( OUT camera, _camera.Frustum() );
    }

/*
=================================================
    GetHelpText
=================================================
*/
    StringView  RemoteCamera::GetHelpText () C_NE___
    {
        return R"(
RemoteCamera controls:
  'W' 'S'       - move forward/backward
  'A' 'D'       - move left/right
  'LeftShift'   - move down
  'Space'       - move up
  'R'           - reset position
  Use Android device to control rotation
)";
    }


} // AE::ResEditor
