// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Controllers/IController.h"

namespace AE::ResEditor
{

    //
    // Flight Camera
    //

    class FlightCamera final : public IController
    {
    // variables
    private:
        mutable RWSpinLock      _guard;

        RC<DynamicDim>          _dynDim;

        float4x4                _view;          // cached
        float4x4                _viewProj;      // cached
        float4x4                _invViewProj;   // cached

        TFlightCamera<float>    _camera;
        const float2            _clipPlanes;
        const float3            _rotationScale; // yaw, pitch, roll
        const Rad               _fovY;
        const float             _minThrust;
        const float             _maxThrust;
        const float3            _initialPos;
        const float             _zoomStep       = 0.1f;
        const bool              _reverseZ;

        float                   _zoom           = 1.0f;
        float                   _engineThrust   = 0.f;
        float                   _dimAspect      = 1.f;


    // methods
    public:
        FlightCamera (RC<DynamicDim> dim, float2 clipPlanes, Rad fovY,
                      float2 engineThrustRange, float3 rotationScale,
                      float3 initialPos, bool reverseZ)                 __Th___;

        // IController //
        void  ProcessInput (ActionQueueReader, secondsf)                __NE_OV;

        InputModeName   GetInputMode ()                                 C_NE_OV { return InputModeName{"Controller.FlightCamera"}; }

        float3          GetPosition ()                                  C_NE_OV { SHAREDLOCK( _guard );  return _camera.Position(); }
        float4x4        GetViewProj ()                                  C_NE_OV { SHAREDLOCK( _guard );  return _viewProj; }
        float4x4        GetInvViewProj ()                               C_NE_OV { SHAREDLOCK( _guard );  return _invViewProj; }
        float4x4        GetProj ()                                      C_NE_OV { SHAREDLOCK( _guard );  return _camera.Projection(); }
        float4x4        GetView ()                                      C_NE_OV { SHAREDLOCK( _guard );  return _view; }
        float           GetZoom ()                                      C_NE_OV { SHAREDLOCK( _guard );  return _zoom; }
        float2          GetClipPlanes ()                                C_NE_OV { return _clipPlanes; }
        StringView      GetHelpText ()                                  C_NE_OV;
    //  RaysGrid_t      GetRaysGrid ()                                  C_NE_OV;


        void            CopyTo (OUT AE::ShaderTypes::CameraData &)      C_NE_OV;

    private:
        void  _Reset ();
        void  _UpdateMatrix ();
    };


} // AE::ResEditor
