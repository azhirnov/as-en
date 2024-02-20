// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/GLFW/GLFWCommon.h"

#ifdef AE_ENABLE_GLFW
# include "platform/Private/InputActionsBase.h"
# include "platform/Private/GestureRecognizer.h"
# include "platform/GLFW/SerializableInputActionsGLFW.h"

namespace AE::App
{

    //
    // Input Actions for GLFW
    //

    class InputActionsGLFW final : public InputActionsBase
    {
    // types
    private:
        using EInputType = SerializableInputActionsGLFW::EInputType;

        static constexpr uint   _Version = SerializableInputActionsGLFW::_Version;


    // variables
    private:
        float2                  _cursorPosPx;
        float2                  _cursorDeltaPx;

        bool                    _touchActive        : 1;
        bool                    _touchBegin         : 1;
        bool                    _touchEnd           : 1;

        GestureRecognizer       _gestureRecognizer;


    // methods
    public:
        explicit InputActionsGLFW (TsDoubleBufferedQueue* q)                        __NE___;

        // api for window
        void  SetKey (int key, EGestureState state, Duration_t timestamp)           __NE___;
        void  SetMouseButton (int button, EGestureState state, Duration_t timestamp)__NE___;
        void  SetCursorPos (float2 pos)                                             __NE___;
        void  SetMouseWheel (float2 delta)                                          __NE___;
        void  SetMonitor (const uint2 &surfaceSize, const Monitor &)                __NE___;
        void  CursorPosChanged (float2 pos)                                         __NE___ { _cursorPosPx = pos; }

        // api for external input
        void  SetSensor1f (EInputType type, float value)                            __NE___ { _Update1F( type, EGestureType::Move, ControllerID::Sensor, value, EGestureState::Update ); }
        void  SetSensor3f (EInputType type, float3 value)                           __NE___ { _Update3F( type, EGestureType::Move, ControllerID::Sensor, value, EGestureState::Update ); }
        void  SetSensorQf (EInputType type, Quat value)                             __NE___ { _UpdateQuatF( type, EGestureType::Move, ControllerID::Sensor, value, EGestureState::Update ); }
        void  SetGNS (const GNSData &value)                                         __NE___ { _UpdateGNS( EInputType::GeoLocation, EGestureType::Move, ControllerID::Sensor, value, EGestureState::Update ); }

        void  Update (Duration_t timeSinceStart)                                    __NE___;


    // IInputActions //
        bool  LoadSerialized (MemRefRStream &stream)                                __NE_OV;


    // ISerializable //
        bool  Serialize (Serializing::Serializer &)                                 C_NE_OV;
        bool  Deserialize (Serializing::Deserializer &)                             __NE_OV;


    private:
        bool  _ValidateForVREmulation ();
    };


} // AE::App

#endif // AE_ENABLE_GLFW
