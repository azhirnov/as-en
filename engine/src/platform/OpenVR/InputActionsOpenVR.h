// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_OPENVR
# include "platform/Private/InputActionsBase.h"
# include "platform/OpenVR/SerializableInputActionsOpenVR.h"
# include "openvr_capi.h"

namespace AE::App
{

	//
	// Input Actions for OpenVR
	//

	class InputActionsOpenVR final : public InputActionsBase
	{
	// types
	private:
		using EInputType = SerializableInputActionsOpenVR::EInputType;
		
		static constexpr uint	_Version	= SerializableInputActionsOpenVR::_Version;


	// variables
	private:
		DubleBufferedQueue	_dbQueue;
		

	// methods
	public:
		explicit InputActionsOpenVR (DubleBufferedQueue *q) __NE___	: InputActionsBase{q ? *q : _dbQueue} {}
		
		void  SetPose (ControllerID id, const float3x3 &rot, const float3 &pos, const float3 &vel, const float3 &avel)	__NE___;
		void  OnAxisStateChanged (ControllerID id, uint key, const float2 &value, const float2 &delta, float dt)		__NE___;
		void  OnButton (ControllerID id, uint axisIdx, EGestureState state)												__NE___;
		

	// IInputActions //
		bool  LoadSerialized (MemRefRStream &stream)	__NE_OV;


	// ISerializable //
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
		

	private:
		ND_ static constexpr bool  _IsKey (EInputType type)			{ return SerializableInputActionsOpenVR::_IsKey( type ); }
		ND_ static constexpr bool  _IsCursor1D (EInputType type)	{ return SerializableInputActionsOpenVR::_IsCursor1D( type ); }
		ND_ static constexpr bool  _IsCursor2D (EInputType type)	{ return SerializableInputActionsOpenVR::_IsCursor2D( type ); }
	};


} // AE::App

#endif // AE_ENABLE_OPENVR
