// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Android/InputActionsAndroid.h"

#ifdef AE_PLATFORM_ANDROID

namespace AE::App
{

/*
=================================================
    constructor
=================================================
*/
    InputActionsAndroid::InputActionsAndroid () __NE___ :
        InputActionsBase{_dbQueue},
        _gestureRecognizer{ ushort(EInputType::TouchPos),   ushort(EInputType::TouchPos_mm),
                            ushort(EInputType::TouchDelta), ushort(EInputType::TouchDelta_norm), ushort(EInputType::MultiTouch) }
    {}

/*
=================================================
    Update
=================================================
*/
    void  InputActionsAndroid::Update (Duration_t timeSinceStart) __NE___
    {
        InputActionsBase::Update( timeSinceStart );

        _gestureRecognizer.Update( timeSinceStart, *this );
    }

/*
=================================================
    SetKey
=================================================
*/
    void  InputActionsAndroid::SetKey (int key, EGestureState state, Duration_t timestamp, uint repeatCount) __NE___
    {
        DRC_EXLOCK( _drCheck );
        ASSERT( repeatCount == 0 ); // TODO: for key up is always 0

        _UpdateKey( EInputType(key), state, ControllerID::Keyboard, timestamp );
    }

/*
=================================================
    SetTouch
=================================================
*/
    void  InputActionsAndroid::SetTouch (uint touchId, float x, float y, EGestureState state, Duration_t timestamp) __NE___
    {
        DRC_EXLOCK( _drCheck );

        _gestureRecognizer.SetTouch( touchId, float2{x,y}, state, timestamp );
    }

/*
=================================================
    SetSensor
=================================================
*/
    void  InputActionsAndroid::SetSensor (int sensorId, ArrayView<float> values) __NE___
    {
        enum {
            TYPE_ACCELEROMETER          = 1,    // [3]
            TYPE_MAGNETIC_FIELD         = 2,    // [3]
            TYPE_GRAVITY                = 9,    // [3]
            TYPE_GYROSCOPE              = 4,    // [3]
            TYPE_LINEAR_ACCELERATION    = 10,   // [3]
            TYPE_ROTATION_VECTOR        = 11,   // [4]
        };

        EInputType  type = Default;
        switch ( sensorId )
        {
            case TYPE_ACCELEROMETER :           type = EInputType::Accelerometer;
            case TYPE_MAGNETIC_FIELD :          type = EInputType::MagneticField;
            case TYPE_GRAVITY :                 type = EInputType::Gravity;
            case TYPE_GYROSCOPE :               type = EInputType::Gyroscope;
            case TYPE_LINEAR_ACCELERATION :     type = EInputType::LinearAcceleration;
            {
                ASSERT( values.size() == 3 );
                _Update3F( type, EGestureType::Move, ControllerID::Sensor, float3{values[0], values[1], values[2]}, EGestureState::Update );
                break;
            }
            case TYPE_ROTATION_VECTOR :
            {
                ASSERT( values.size() == 4 );
                _Update4F( EInputType::RotationVector, EGestureType::Move, ControllerID::Sensor,
                           float4{values[0], values[1], values[2], values[3]}, EGestureState::Update );
                break;
            }
        }
    }

/*
=================================================
    SetMonitor
=================================================
*/
    void  InputActionsAndroid::SetMonitor (const uint2 &surfaceSize, const Monitor &monitor) __NE___
    {
        DRC_EXLOCK( _drCheck );

        _toSNorm    = 1.0f / float2(surfaceSize);
        _pixToMm    = monitor.MillimetersPerPixel();
    }

/*
=================================================
    LoadSerialized
=================================================
*/
    bool  InputActionsAndroid::LoadSerialized (MemRefRStream &stream) __NE___
    {
        DRC_EXLOCK( _drCheck );
        _Reset();

        return SerializableInputActions::LoadSerialized( OUT _modeMap, _Version, InputActionsAndroid_Name, stream );
    }

/*
=================================================
    Serialize
=================================================
*/
    bool  InputActionsAndroid::Serialize (Serializing::Serializer &ser) C_NE___
    {
        DRC_EXLOCK( _drCheck );

        return SerializableInputActions::Serialize( _modeMap, _Version, ser );
    }

/*
=================================================
    Deserialize
=================================================
*/
    bool  InputActionsAndroid::Deserialize (Serializing::Deserializer &des) __NE___
    {
        DRC_EXLOCK( _drCheck );

        _Reset();

        CHECK_ERR( SerializableInputActions::Deserialize( OUT _modeMap, _Version, des ));
        return true;
    }

/*
=================================================
    SetQueue
=================================================
*/
    void  InputActionsAndroid::SetQueue (DubleBufferedQueue *q) __NE___
    {
        CHECK_ERRV( q != null );

        _dbQueueRef = DubleBufferedQueueRef{*q};
    }


} // AE::App

#endif // AE_PLATFORM_ANDROID
