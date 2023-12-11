// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Controllers/ScaleBiasCamera.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{

/*
=================================================
    constructor
=================================================
*/
    ScaleBiasCamera::ScaleBiasCamera (RC<DynamicDim> dim) __Th___ :
        _dynDim{ RVRef(dim) }
    {
        CHECK_THROW( _dynDim );
        _Reset();
    }

/*
=================================================
    ProcessInput
=================================================
*/
    void  ScaleBiasCamera::ProcessInput (ActionQueueReader reader, secondsf) __NE___
    {
        constexpr auto& IA      = InputActions::Controller_ScaleBias;
        constexpr auto& BaseIA  = InputActions::SwitchInputMode;

        const uint2 dim     = _dynDim ? _dynDim->Dimension2() : uint2{1};
        float2      bias    {0.f};
        float       scale   = 0.f;
        bool        reset   = false;

        ActionQueueReader::Header   hdr;
        for (; reader.ReadHeader( OUT hdr );)
        {
            StaticAssert( (IA.actionCount - BaseIA.actionCount) == 3 );
            switch ( uint{hdr.name} )
            {
                case IA.Camera_Bias :
                    bias += reader.DataCopy<float2>( hdr.offset );          break;

                case IA.Camera_Scale :
                    scale += reader.Data<packed_float2>( hdr.offset ).y;    break;

                case IA.Camera_Reset :
                    reset = true;                                           break;
            }
        }


        EXLOCK( _guard );

        if_unlikely( reset )
            return _Reset();

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
    void  ScaleBiasCamera::Add (const float2 &bias, float scale, Rad angle) __NE___
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
    void  ScaleBiasCamera::Set (const float2 &bias, float scale, Rad angle) __NE___
    {
        EXLOCK( _guard );

        _bias   = bias;
        _scale  = scale;
        _angle  = angle;

        _UpdateMatrix();
    }

/*
=================================================
    _Reset
=================================================
*/
    void  ScaleBiasCamera::_Reset ()
    {
        _bias   = float2{};
        _scale  = 0.f;
        _angle  = 0_rad;

        _UpdateMatrix();
    }

/*
=================================================
    _UpdateMatrix
=================================================
*/
    void  ScaleBiasCamera::_UpdateMatrix ()
    {
        _matrix = float4x4::Translated( float3{ _bias, 0.f }) * float4x4::Scaled( _scale );
    }

/*
=================================================
    CopyTo
=================================================
*/
    void  ScaleBiasCamera::CopyTo (OUT AE::ShaderTypes::CameraData &camera) C_NE___
    {
        Frustum_t   frustum;
        frustum.Setup( _matrix );

        _CopyToCameraData( OUT camera, frustum );
    }

/*
=================================================
    GetHelpText
=================================================
*/
    StringView  ScaleBiasCamera::GetHelpText () C_NE___
    {
        return R"(
ScaleBiasCamera controls:
  'Left mouse btn' - press to move camera
  'Mouse wheel'    - zoom
  'R'              - reset position, rotation, zoom
)";
    }


} // AE::ResEditor
