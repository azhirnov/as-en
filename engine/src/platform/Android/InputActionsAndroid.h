// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/Common.h"

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

		float2				_toSNorm;
		float2				_pixToMm;
		
		GestureRecognizer	_gestureRecognizer;

		
	// methods
	public:
		InputActionsAndroid () : InputActionsBase{_dbQueue} {}
		
		void  SetKey (int key, EGestureState state);
		void  SetTouch (uint touchId, float x, float y, EGestureState state, Duration_t timestamp);
		void  SetMonitor (const uint2 &surfaceSize, const Monitor &);

		void  SetQueue (DubleBufferedQueue *);


	// IInputActions //
		bool  LoadSerialized (MemRefRStream &stream) override;
		

	// ISerializable //
		bool  Serialize (Serializing::Serializer &) const override;
		bool  Deserialize (Serializing::Deserializer &) override;


	private:
		ND_ static constexpr bool  _IsKey (EInputType type)			{ return SerializableInputActionsAndroid::_IsKey( type ); }
		ND_ static constexpr bool  _IsCursor1D (EInputType type)	{ return SerializableInputActionsAndroid::_IsCursor1D( type ); }
		ND_ static constexpr bool  _IsCursor2D (EInputType type)	{ return SerializableInputActionsAndroid::_IsCursor2D( type ); }
	};


} // AE::App

#endif // AE_PLATFORM_ANDROID
