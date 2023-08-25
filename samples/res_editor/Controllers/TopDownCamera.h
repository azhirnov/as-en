// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Controllers/IController.h"

namespace AE::ResEditor
{

    //
    // Top Down Camera (2D)
    //

    class TopDownCamera final : public IController
    {
    // variables
    private:
        mutable RWSpinLock      _guard;

        RC<DynamicDim>          _dynDim;

        float4x4                _view;
        float4x4                _proj;

        float2                  _position;
        Rad                     _angle;

        const MovingScale       _movingScale;
        const float             _rotationScale;
        const float2            _initialPos;


    // methods
    public:
        TopDownCamera (RC<DynamicDim> dim, const MovingScale &,
                       float rotationScale, float2 initialPos)      __Th___;

        // IController //
        void  ProcessInput (ActionQueueReader, secondsf)            __NE_OV;

        InputModeName   GetInputMode ()                             C_NE_OV { return InputModeName{"Controller.TopDown"}; }

        float3          GetPosition ()                              C_NE_OV { SHAREDLOCK( _guard );  return float3{_position, 0.f}; }
        float4x4        GetViewProj ()                              C_NE_OV { SHAREDLOCK( _guard );  return _proj * _view; }
        float4x4        GetView ()                                  C_NE_OV { SHAREDLOCK( _guard );  return _view; }
        float4x4        GetProj ()                                  C_NE_OV { SHAREDLOCK( _guard );  return _proj; }
        float2          GetClipPlanes ()                            C_NE_OV { return float2{-10.f, 10.f}; }
        StringView      GetHelpText ()                              C_NE_OV;
    //  RaysGrid_t      GetRaysGrid ()                              C_NE_OV;

        void            CopyTo (OUT AE::ShaderTypes::CameraData &)  C_NE_OV;

    private:
        void  _Reset ();
        void  _UpdateMatrix ();
    };


} // AE::ResEditor
