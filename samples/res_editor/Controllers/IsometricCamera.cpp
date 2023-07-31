// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Controllers/IsometricCamera.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
    constructor
=================================================
*/
    IsometricCamera::IsometricCamera (RC<DynamicDim> dim, float2 clipPlanes, Rad fovY,
                                      float2 rotationScale, float offsetScale,
                                      float3 initialPos, float initialOffset) __Th___ :
        _dynDim{ RVRef(dim) },      _clipPlanes{ clipPlanes },
        _fovY{ fovY },              _rotationScale{ rotationScale },
        _offsetScale{ offsetScale },
        _initialPos{ initialPos },  _initialOffset{ initialOffset }
    {
        CHECK_THROW( _dynDim );
        _Reset();
    }

/*
=================================================
    ProcessInput
=================================================
*/
    void  IsometricCamera::ProcessInput (ActionQueueReader reader, secondsf timeDelta) __NE___
    {
        packed_float3   move;
        packed_float2   rotation;
        float           offset  = 0.f;
        bool            reset   = false;

        ActionQueueReader::Header   hdr;
        for (; reader.ReadHeader( OUT hdr );)
        {
            if_unlikely( hdr.name == InputActionName{"Camera.Rotate"} )
                rotation += reader.Data<packed_float2>( hdr.offset );

            if_unlikely( hdr.name == InputActionName{"Camera.Move"} )
                move += reader.Data<packed_float3>( hdr.offset );

            if_unlikely( hdr.name == InputActionName{"Camera.Zoom"} )
                offset += reader.Data<packed_float2>( hdr.offset ).y;

            if_unlikely( hdr.name == InputActionName{"Camera.Reset"} )
                reset = true;
        }


        EXLOCK( _guard );

        if_unlikely( reset )
            return _Reset();

        _camera.Rotate( Rad{rotation.x * _rotationScale.x}, Rad{rotation.y * _rotationScale.y} );
        _camera.AddOffset( offset * _offsetScale * timeDelta.count() );

        _UpdateMatrix();
    }

/*
=================================================
    _Reset
=================================================
*/
    void  IsometricCamera::_Reset ()
    {
        _camera.SetPosition( _initialPos );
        _camera.SetRotation( QuatF::Identity() );
        _camera.SetOffset( _initialOffset );

        _dimAspect  = _dynDim->Aspect();

        _camera.SetPerspective( _fovY, _dimAspect, _clipPlanes.x, _clipPlanes.y );

        _UpdateMatrix();
    }

/*
=================================================
    _UpdateMatrix
=================================================
*/
    void  IsometricCamera::_UpdateMatrix ()
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
    void  IsometricCamera::CopyTo (OUT AE::ShaderTypes::CameraData &camera) C_NE___
    {
        _CopyToCameraData( OUT camera, _camera.Frustum() );
    }


} // AE::ResEditor
