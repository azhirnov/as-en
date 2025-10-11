// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/OpenXR/OpenXRCommon.h"

#ifdef AE_ENABLE_OPENXR
# include "platform/Private/InputActionsBase.h"
# include "platform/OpenXR/SerializableInputActionsOpenXR.h"

namespace AE::App
{

	//
	// Input Actions for OpenXR
	//

	class InputActionsOpenXR final : public InputActionsBase
	{
	// types
	private:
		using EInputType = SerializableInputActionsOpenXR::EInputType;

		static constexpr uint	_Version	= SerializableInputActionsOpenXR::_Version;


	// variables
	private:


	// methods
	public:
		explicit InputActionsOpenXR (TsDoubleBufferedQueue* q)		__NE___ : InputActionsBase{ q } {}

		void  SetPose (ControllerID id, const float3x3 &rot, const float3 &pos, const float3 &vel, const float3 &avel)	__NE___;
		void  OnAxisStateChanged (ControllerID id, uint key, const float2 &value, const float2 &delta, float dt)		__NE___;
		void  OnButton (ControllerID id, uint axisIdx, EGestureState state)												__NE___;


	// IInputActions //
		bool  LoadSerialized (MemRefRStream &stream)				__NE_OV;


	// ISerializable //
		bool  Serialize (Serializing::Serializer &)					C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &)				__NE_OV;


	private:
	};


} // AE::App

#endif // AE_ENABLE_OPENXR
