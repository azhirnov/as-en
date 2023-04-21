// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Android/AndroidCommon.h"

#ifdef AE_PLATFORM_ANDROID
# include "platform/Private/InputActionsBase.h"
# include "platform/Private/GestureRecognizer.h"
# include "platform/Android/SerializableInputActionsAndroid.h"

namespace AE::App
{

	//
	// Input Actions for Android
	//

	class InputActionsAndroid final : public InputActionsBase
	{
	// types
	private:
		using EInputType = SerializableInputActionsAndroid::EInputType;
		
		static constexpr uint	_Version	= SerializableInputActionsAndroid::_Version;


	// variables
	private:
		DubleBufferedQueue	_dbQueue;
		
		GestureRecognizer	_gestureRecognizer;

		
	// methods
	public:
		InputActionsAndroid ()																		__NE___;
		
		void  SetKey (int key, EGestureState state, Duration_t timestamp, uint count)				__NE___;
		void  SetTouch (uint touchId, float x, float y, EGestureState state, Duration_t timestamp)	__NE___;
		void  SetMonitor (const uint2 &surfaceSize, const Monitor &)								__NE___;

		void  SetQueue (DubleBufferedQueue *)														__NE___;
		
		void  Update (Duration_t timeSinceStart)													__NE___;


	// IInputActions //
		bool  LoadSerialized (MemRefRStream &stream)												__NE_OV;
		

	// ISerializable //
		bool  Serialize (Serializing::Serializer &)													C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &)												__NE_OV;


	private:
		ND_ static constexpr bool  _IsKey (EInputType type)			{ return SerializableInputActionsAndroid::_IsKey( type ); }
		ND_ static constexpr bool  _IsCursor1D (EInputType type)	{ return SerializableInputActionsAndroid::_IsCursor1D( type ); }
		ND_ static constexpr bool  _IsCursor2D (EInputType type)	{ return SerializableInputActionsAndroid::_IsCursor2D( type ); }
	};


} // AE::App

#endif // AE_PLATFORM_ANDROID
