// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Android/AndroidCommon.h"

#ifdef AE_PLATFORM_ANDROID
# include "platform/Private/InputActionsBase.h"
# include "platform/Private/GestureRecognizer.h"
# include "platform/Android/SerializableInputActionsAndroid.h"

namespace AE::App
{
    using namespace AE::Java;


    //
    // Input Actions for Android
    //

    class InputActionsAndroid final : public InputActionsBase
    {
    // types
    public:
        using EnableSensorsFn_t = JavaMethod< void (jint) >;
    private:
        using EInputType        = SerializableInputActionsAndroid::EInputType;
        using ESensorType       = SerializableInputActionsAndroid::ESensorType;

        static constexpr uint   _Version    = SerializableInputActionsAndroid::_Version;


    // variables
    private:
        Quat                    _screenOrient;

        GestureRecognizer       _gestureRecognizer;

        struct {
            EnableSensorsFn_t       enableSensors;
        }                       _methods;


    // methods
    public:
        explicit InputActionsAndroid (TsDoubleBufferedQueue* q = null)                              __NE___;

        void  Initialize (EnableSensorsFn_t fn)                                                     __NE___;

        void  SetKey (jint key, EGestureState state, Duration_t timestamp, uint count)              __NE___;
        void  SetTouch (uint touchId, float x, float y, EGestureState state, Duration_t timestamp)  __NE___;
        void  SetMonitor (const uint2 &surfaceSize, const Monitor &)                                __NE___;
        void  SetSensor (jint sensorId, ArrayView<float> values)                                    __NE___;

        // api for external input
        void  SetSensor1f (EInputType type, float value)                                            __NE___ { _Update1F( type, EGestureType::Move, ControllerID::Sensor, value, EGestureState::Update ); }
        void  SetSensor3f (EInputType type, float3 value)                                           __NE___ { _Update3F( type, EGestureType::Move, ControllerID::Sensor, value, EGestureState::Update ); }
        void  SetSensorQf (EInputType type, Quat value)                                             __NE___ { _UpdateQuatF( type, EGestureType::Move, ControllerID::Sensor, value, EGestureState::Update ); }
        void  SetGNS (const GNSData &value)                                                         __NE___ { _UpdateGNS( EInputType::GeoLocation, EGestureType::Move, ControllerID::Sensor, value, EGestureState::Update ); }

        void  Update (Duration_t timeSinceStart)                                                    __NE___;


    // IInputActions //
        bool  LoadSerialized (MemRefRStream &stream)                                                __NE_OV;
        bool  SetMode (InputModeName::Ref value)                                                    __NE_OV;


    // ISerializable //
        bool  Serialize (Serializing::Serializer &)                                                 C_NE_OV;
        bool  Deserialize (Serializing::Deserializer &)                                             __NE_OV;


    private:
        ND_ static EInputType   _SensorTypeToInputType (ESensorType type)   __NE___ { return SerializableInputActionsAndroid::SensorTypeToInputType( type ); }
    };


} // AE::App

#endif // AE_PLATFORM_ANDROID
