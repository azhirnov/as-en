// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_OPENXR
# include "platform/OpenXR/InputActionsOpenXR.h"

namespace AE::App
{

/*
=================================================
	SetPose
=================================================
*/
	void  InputActionsOpenXR::SetPose (ControllerID id, const float3x3 &rot, const float3 &pos, const float3 &vel, const float3 &avel) __NE___
	{
		Unused( id, rot, pos, vel, avel );
		// TODO
	}

/*
=================================================
	OnAxisStateChanged
=================================================
*/
	void  InputActionsOpenXR::OnAxisStateChanged (ControllerID id, uint key, const float2 &value, const float2 &delta, float dt) __NE___
	{
		Unused( id, key, value, delta, dt );
		// TODO
	}

/*
=================================================
	OnButton
=================================================
*/
	void  InputActionsOpenXR::OnButton (ControllerID id, uint axisIdx, EGestureState state) __NE___
	{
		Unused( id, axisIdx, state );
		// TODO
	}

/*
=================================================
	LoadSerialized
=================================================
*/
	bool  InputActionsOpenXR::LoadSerialized (MemRefRStream &stream) __NE___
	{
		DRC_EXLOCK( _drCheck );
		_Reset();

		return SerializableInputActions::LoadSerialized( OUT _modeMap, _Version, InputActionsOpenXR_Name, stream );
	}

/*
=================================================
	Serialize
=================================================
*/
	bool  InputActionsOpenXR::Serialize (Serializing::Serializer &ser) C_NE___
	{
		DRC_EXLOCK( _drCheck );

		return SerializableInputActions::Serialize( _modeMap, _Version, ser );
	}

/*
=================================================
	Deserialize
=================================================
*/
	bool  InputActionsOpenXR::Deserialize (Serializing::Deserializer &des) __NE___
	{
		DRC_EXLOCK( _drCheck );

		_Reset();

		CHECK_ERR( SerializableInputActions::Deserialize( OUT _modeMap, _Version, des ));
		return true;
	}


} // AE::App

#endif // AE_ENABLE_OPENXR
