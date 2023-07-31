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
    void  FPSCamera::ProcessInput (ActionQueueReader reader, secondsf timeDelta) __NE___
    {
        packed_float3   move;
        packed_float2   rotation;
        float           zoom    = 0.f;
        bool            reset   = false;

        ActionQueueReader::Header   hdr;
        for (; reader.ReadHeader( OUT hdr );)
        {
            if_unlikely( hdr.name == InputActionName{"Camera.Rotate"} )
                rotation += reader.Data<packed_float2>( hdr.offset );

            if_unlikely( hdr.name == InputActionName{"Camera.Move"} )
                move += reader.Data<packed_float3>( hdr.offset );

            if_unlikely( hdr.name == InputActionName{"Camera.Zoom"} )
                zoom += reader.Data<packed_float2>( hdr.offset ).y;

            if_unlikely( hdr.name == InputActionName{"Camera.Reset"} )
                reset = true;
        }


        EXLOCK( _guard );

        if_unlikely( reset )
            return _Reset();

        move = _movingScale.Apply( move ) * timeDelta.count();

        _camera.Rotate( Rad{rotation.x * _rotationScale.x}, Rad{rotation.y * _rotationScale.y} );
        _camera.Move( move );

        if ( IsNotZero( zoom ) or _dynDim->IsChanged( INOUT _dimAspect ))
        {
            _zoom = Clamp( _zoom - zoom * _zoomStep, _minZoom, _maxZoom );
            _camera.SetPerspective( _fovY * _zoom, _dimAspect, _clipPlanes.x, _clipPlanes.y );
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


} // AE::ResEditor
