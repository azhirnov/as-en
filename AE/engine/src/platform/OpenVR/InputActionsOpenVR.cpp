// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_OPENVR
# include "platform/OpenVR/InputActionsOpenVR.h"

namespace AE::App
{

/*
=================================================
    SetPose
=================================================
*/
    void  InputActionsOpenVR::SetPose (ControllerID id, const float3x3 &rot, const float3 &pos, const float3 &vel, const float3 &avel) __NE___
    {
        Unused( id, rot, pos, vel, avel );
        // TODO
    }

/*
=================================================
    OnAxisStateChanged
=================================================
*/
    void  InputActionsOpenVR::OnAxisStateChanged (ControllerID id, uint key, const float2 &value, const float2 &delta, float dt) __NE___
    {
        Unused( id, key, value, delta, dt );
        // TODO
    }

/*
=================================================
    OnButton
=================================================
*/
    void  InputActionsOpenVR::OnButton (ControllerID id, uint axisIdx, EGestureState state) __NE___
    {
        Unused( id, axisIdx, state );
        // TODO
    }

/*
=================================================
    LoadSerialized
=================================================
*/
    bool  InputActionsOpenVR::LoadSerialized (MemRefRStream &stream) __NE___
    {
        DRC_EXLOCK( _drCheck );
        _Reset();

        return SerializableInputActions::LoadSerialized( OUT _modeMap, _Version, InputActionsOpenVR_Name, stream );
    }

/*
=================================================
    Serialize
=================================================
*/
    bool  InputActionsOpenVR::Serialize (Serializing::Serializer &ser) C_NE___
    {
        DRC_EXLOCK( _drCheck );

        return SerializableInputActions::Serialize( _modeMap, _Version, ser );
    }

/*
=================================================
    Deserialize
=================================================
*/
    bool  InputActionsOpenVR::Deserialize (Serializing::Deserializer &des) __NE___
    {
        DRC_EXLOCK( _drCheck );

        _Reset();

        CHECK_ERR( SerializableInputActions::Deserialize( OUT _modeMap, _Version, des ));
        return true;
    }


} // AE::App

#endif // AE_ENABLE_OPENVR
