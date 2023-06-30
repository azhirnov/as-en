// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Controllers/Controller2D.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
    constructor
=================================================
*/
    Controller2D::Controller2D (RC<DynamicDim> dim) __Th___ :
        _dynDim{ RVRef(dim) }
    {
        CHECK_THROW( _dynDim );
        _UpdateMatrix();
    }

/*
=================================================
    ProcessInput
=================================================
*/
    void  Controller2D::ProcessInput (ActionQueueReader reader, secondsf) __NE___
    {
        const uint2 dim = _dynDim ? _dynDim->Dimension2() : uint2{1};
        float2      bias;
        float       scale   = 0.f;
        bool        reset   = false;

        ActionQueueReader::Header   hdr;
        for (; reader.ReadHeader( OUT hdr );)
        {
            if_unlikely( hdr.name == InputActionName{"Camera2D.Bias"} )
                bias = reader.Data<packed_float2>( hdr.offset );

            if_unlikely( hdr.name == InputActionName{"Camera2D.Scale"} )
                scale = reader.Data<packed_float2>( hdr.offset ).y;

            if_unlikely( hdr.name == InputActionName{"Camera2D.Reset"} )
                reset = true;
        }


        EXLOCK( _guard );

        if_unlikely( reset )
            return Reset();

        _bias   += bias * _scale;
        _scale  += scale;

        // validate
        _bias   = Clamp( _bias, -float2{dim}, float2{dim} );
        _scale  = Clamp( _scale, 0.1f, 10.f );

        _UpdateMatrix();
    }

/*
=================================================
    Add
=================================================
*/
    void  Controller2D::Add (const float2 &bias, float scale, Rad angle) __NE___
    {
        EXLOCK( _guard );

        _bias   += bias * _scale;
        _scale  += scale;
        _angle  += angle;

        _UpdateMatrix();
    }

/*
=================================================
    Set
=================================================
*/
    void  Controller2D::Set (const float2 &bias, float scale, Rad angle) __NE___
    {
        EXLOCK( _guard );

        _bias   = bias;
        _scale  = scale;
        _angle  = angle;

        _UpdateMatrix();
    }

/*
=================================================
    _UpdateMatrix
=================================================
*/
    void  Controller2D::_UpdateMatrix ()
    {
        _matrix = float4x4::Translate( float3{ _bias, 0.f }) * float4x4::Scale( _scale );
    }

/*
=================================================
    CopyTo
=================================================
*/
    void  Controller2D::CopyTo (OUT AE::ShaderTypes::CameraData &camera) C_NE___
    {
        Frustum_t   frustum;
        frustum.Setup( _matrix );

        _CopyToCameraData( OUT camera, frustum );
    }


} // AE::ResEditor
