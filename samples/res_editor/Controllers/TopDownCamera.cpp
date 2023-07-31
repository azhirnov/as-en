// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Controllers/TopDownCamera.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
    constructor
=================================================
*/
    TopDownCamera::TopDownCamera (RC<DynamicDim> dim, const MovingScale &movingScale,
                                  float rotationScale, float2 initialPos) __Th___ :
        _dynDim{ RVRef(dim) },          _movingScale{movingScale},
        _rotationScale{rotationScale},  _initialPos{initialPos}
    {
        CHECK_THROW( _dynDim );
        _Reset();
    }

/*
=================================================
    ProcessInput
=================================================
*/
    void  TopDownCamera::ProcessInput (ActionQueueReader reader, secondsf timeDelta) __NE___
    {
        float2  move;
        float   angle   = 0.f;
        bool    reset   = false;

        ActionQueueReader::Header   hdr;
        for (; reader.ReadHeader( OUT hdr );)
        {
            if_unlikely( hdr.name == InputActionName{"Camera.Move"} )
                move += float2{reader.Data<packed_float2>( hdr.offset )};

            if_unlikely( hdr.name == InputActionName{"Camera.Rotate"} )
                angle += reader.Data<float>( hdr.offset );

            if_unlikely( hdr.name == InputActionName{"Camera.Reset"} )
                reset = true;
        }


        EXLOCK( _guard );

        if_unlikely( reset )
            return _Reset();

        move = _movingScale.Apply( move ) * timeDelta.count();
        move = float2x2{_view} * move;

        _position   += move;
        _angle      = (_angle + angle * _rotationScale).WrapToPi();

        _UpdateMatrix();
    }

/*
=================================================
    _Reset
=================================================
*/
    void  TopDownCamera::_Reset ()
    {
        _position   = _initialPos;

        _UpdateMatrix();
    }

/*
=================================================
    _UpdateMatrix
=================================================
*/
    void  TopDownCamera::_UpdateMatrix ()
    {
        _view = float4x4::RotateZ( _angle ).Translate( float3{ _position, 0.f });

        // TODO: proj
    }

/*
=================================================
    CopyTo
=================================================
*/
    void  TopDownCamera::CopyTo (OUT AE::ShaderTypes::CameraData &camera) C_NE___
    {
        Frustum_t   frustum;
        frustum.Setup( GetViewProj() );

        _CopyToCameraData( OUT camera, frustum );
    }


} // AE::ResEditor
