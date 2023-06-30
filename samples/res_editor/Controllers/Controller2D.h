// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Controllers/IController.h"

namespace AE::ResEditor
{

    //
    // Controller 2D
    //

    class Controller2D final : public IController
    {
    // variables
    private:
        mutable RWSpinLock  _guard;

        RC<DynamicDim>      _dynDim;

        float4x4            _matrix;
        float2              _bias;
        float               _scale      = 1.0f;
        Rad                 _angle;


    // methods
    public:
        explicit Controller2D (RC<DynamicDim> dim)                  __Th___;

        void  Add (const float2 &bias, float scale, Rad angle)      __NE___;
        void  Set (const float2 &bias, float scale, Rad angle)      __NE___;
        void  Reset ()                                              __NE___ { Set( float2{}, 0.f, 0_rad ); }


        // IController //
        void  ProcessInput (ActionQueueReader, secondsf)            __NE_OV;

        InputModeName   GetInputMode ()                             C_NE_OV { return InputModeName{"Controller.2D"}; }

        float3          GetPosition ()                              C_NE_OV { SHAREDLOCK( _guard );  return float3{_bias, 0.f}; }
        float4x4        GetViewProj ()                              C_NE_OV { SHAREDLOCK( _guard );  return _matrix; }
        float4x4        GetView ()                                  C_NE_OV { SHAREDLOCK( _guard );  return _matrix; }
        float4x4        GetProj ()                                  C_NE_OV { return float4x4::Identity(); }
    //  RaysGrid_t      GetRaysGrid ()                              C_NE_OV;

        void            CopyTo (OUT AE::ShaderTypes::CameraData &)  C_NE_OV;

    private:
        void  _UpdateMatrix ();
    };


} // AE::ResEditor