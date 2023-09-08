// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"
#include "res_editor/Controllers/IController.h"

namespace AE::ResEditor
{

    //
    // Base Controller interface
    //
    class ScriptBaseController : public EnableScriptRC
    {
    // variables
    protected:
        RC<IController>         _controller;
        ScriptDynamicDimPtr     _dynamicDim;


    // methods
    public:
        static void  Bind (const ScriptEnginePtr &se)                   __Th___;

        ND_ virtual RC<IController>  ToController ()                    __Th___ = 0;

        void  SetDimensionIfNotSet (const ScriptDynamicDimPtr &)        __Th___;

    protected:
        template <typename B>
        static void  _BindBase (B &binder)                              __Th___;

        ScriptDynamicDim*   _Dimension ()                               __Th___ { return ScriptDynamicDimPtr{_dynamicDim}.Detach(); }

        void  _SetDimension (const ScriptDynamicDimPtr &)               __Th___;
    };



    //
    // Controller Scale Bias
    //
    class ScriptControllerScaleBias final : public ScriptBaseController
    {
    // methods
    public:
        static void  Bind (const ScriptEnginePtr &se)                   __Th___;

        RC<IController>  ToController ()                                __Th_OV;
    };



    //
    // Controller Top Down
    //
    class ScriptControllerTopDown final : public ScriptBaseController
    {
    // types
    private:
        using MovingScale   = IController::MovingScale;

    // variables
    private:
        MovingScale     _movingScale;
        float           _rotationScale  {1.f};
        float2          _initialPos     {0.f};

    // methods
    public:
        static void  Bind (const ScriptEnginePtr &se)                   __Th___;

        void  SetRotationScale (float value)                            __Th___;
        void  ForwardBackwardScale1 (float value)                       __Th___;
        void  ForwardBackwardScale2 (float forward, float backward)     __Th___;
        void  SideMovementScale (float value)                           __Th___;

        void  SetPosition (const packed_float2 &pos)                    __Th___ { _initialPos = pos; }

        RC<IController>  ToController ()                                __Th_OV;
    };



    //
    // Controller Orbital Camera
    //
    class ScriptControllerOrbitalCamera final : public ScriptBaseController
    {
    // variables
    protected:
        float           _fovY           = 60.f;
        float2          _clipPlanes     {0.1f, 100.f};
        float2          _rotationScale  {1.f};
        float3          _initialPos     {0.f};
        float           _initialOffset  {0.f};
        float           _offsetScale    {1.f};

    // methods
    public:
        void  SetFovY (float value)                                     __Th___;
        void  SetClipPlanes (float near, float far)                     __Th___;

        void  SetRotationScale1 (float value)                           __Th___ { _rotationScale = float2{value}; }
        void  SetRotationScale2 (float x, float y)                      __Th___ { _rotationScale = float2{x,y}; }
        void  SetOffsetScale (float value)                              __Th___ { _offsetScale = value; }

        void  SetPosition (const packed_float3 &pos)                    __Th___ { _initialPos = pos; }
        void  SetOffset (float value)                                   __Th___ { _initialOffset = value; }

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;

        RC<IController>  ToController ()                                __Th_OV;
    };



    //
    // Controller Camera 3D
    //
    class ScriptControllerCamera3D : public ScriptBaseController
    {
    // types
    protected:
        using MovingScale   = IController::MovingScale;

    // variables
    protected:
        float           _fovY           = 60.f;
        float2          _clipPlanes     {0.1f, 100.f};
        float3          _rotationScale  {1.f};
        MovingScale     _movingScale;
        float3          _initialPos     {0.f};

    // methods
    public:
        void  SetFovY (float value)                                     __Th___;
        void  SetClipPlanes (float near, float far)                     __Th___;

        void  SetRotationScale1 (float value)                           __Th___ { _rotationScale = float3{value}; }
        void  SetRotationScale2 (float x, float y)                      __Th___ { _rotationScale = float3{x,y,0.f}; }
        void  SetRotationScale3 (float x, float y, float z)             __Th___ { _rotationScale = float3{x,y,z}; }

        void  ForwardBackwardScale1 (float value)                       __Th___;
        void  ForwardBackwardScale2 (float forward, float backward)     __Th___;
        void  UpDownScale1 (float value)                                __Th___;
        void  UpDownScale2 (float up, float down)                       __Th___;
        void  SideMovementScale (float value)                           __Th___;

        void  SetPosition (const packed_float3 &pos)                    __Th___ { _initialPos = pos; }
    };



    //
    // Controller Flight Camera
    //
    class ScriptControllerFlightCamera final : public ScriptControllerCamera3D
    {
    // variables
    private:
        float2      _engineThrustRange  {-2.f, 10.f};

    // methods
    public:
        void  SetEngineThrustRange (float min, float max)               __Th___;

        static void  Bind (const ScriptEnginePtr &se)                   __Th___;

        RC<IController>  ToController ()                                __Th_OV;
    };



    //
    // Controller FPV Camera
    //
    class ScriptControllerFPVCamera final : public ScriptControllerCamera3D
    {
    // methods
    public:
        static void  Bind (const ScriptEnginePtr &se)                   __Th___;

        RC<IController>  ToController ()                                __Th_OV;
    };



    //
    // Controller Free Camera
    //
    class ScriptControllerFreeCamera final : public ScriptControllerCamera3D
    {
    // methods
    public:
        static void  Bind (const ScriptEnginePtr &se)                   __Th___;

        RC<IController>  ToController ()                                __Th_OV;
    };


} // AE::ResEditor
