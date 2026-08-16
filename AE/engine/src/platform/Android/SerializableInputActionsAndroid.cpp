// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_ANDROID
# include <android/keycodes.h>
# include <android/sensor.h>
#endif

#include "pch/Scripting.h"
#include "platform/Android/SerializableInputActionsAndroid.h"

namespace AE::App
{
# ifdef AE_PLATFORM_ANDROID

	#define AE_ANDROID_KEY_CODES_VISITOR( _key_, _code_, _name_, _and_code_ )\
		StaticAssert( uint(SerializableInputActionsAndroid::EInputType::_key_) == uint(_and_code_) );

	AE_ANDROID_KEY_CODES( AE_ANDROID_KEY_CODES_VISITOR )
	#undef AE_ANDROID_KEY_CODES_VISITOR

	#define AE_ANDROID_SERNSORS_VISITOR( _type_, _bitIndex_, _api_, _valType_, _and_code_ )\
		StaticAssert( _bitIndex_ == uint(_and_code_) );

	AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
	#undef AE_ANDROID_SERNSORS_VISITOR

# endif

/*
=================================================
	InputTypeToString
=================================================
*/
	String  SerializableInputActionsAndroid::InputTypeToString (InputType_t value) C_Th___
	{
		switch_enum( EInputType(value) )
		{
			#define AE_ANDROID_KEY_CODES_VISITOR( _key_, _code_, _name_, ... )	case EInputType::_key_ : return _name_;
			AE_ANDROID_KEY_CODES( AE_ANDROID_KEY_CODES_VISITOR )
			#undef AE_ANDROID_KEY_CODES_VISITOR

			#define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )					case EInputType::_type_ : return AE_TOSTRING( _type_ );
			AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
			#undef AE_ANDROID_SERNSORS_VISITOR

			case EInputType::MultiTouch :		return "MultiTouch";

			// Cursor2DBegin
			case EInputType::TouchPos :			return "TouchPos";
			case EInputType::TouchPos_mm :		return "TouchPos_mm";
			case EInputType::TouchDelta :		return "TouchDelta";
			case EInputType::TouchDelta_norm :	return "TouchDelta_norm";
			// Cursor2DEnd

			case EInputType::KeyBegin :
			case EInputType::KeyEnd :

			case EInputType::_Count :
			case EInputType::Unknown :	break;
		}
		switch_end

		return "code_"s << Base::ToString( value );
	}

/*
=================================================
	SensorBitsToString
=================================================
*/
	String  SerializableInputActionsAndroid::SensorBitsToString (ESensorBits bits) C_Th___
	{
		String	str;
		for (auto idx : BitIndexIterate<ESensorType>(bits))
		{
			if ( not str.empty() )
				str << ", ";

			switch_enum( idx )
			{
				#define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )	case ESensorType::_type_ : str << AE_TOSTRING( _type_ ); break;
				AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
				#undef AE_ANDROID_SERNSORS_VISITOR

				case ESensorType::Unknown :	break;
			}
			switch_end
		}
		return str;
	}

/*
=================================================
	InputTypeToSensorType
=================================================
*/
	SerializableInputActionsAndroid::ESensorType  SerializableInputActionsAndroid::InputTypeToSensorType (const EInputType inputType) __NE___
	{
		switch ( inputType )
		{
			#define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )	case EInputType::_type_ : return ESensorType::_type_;
			AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
			#undef AE_ANDROID_SERNSORS_VISITOR
		}
		return Default;
	}

/*
=================================================
	SensorTypeToInputType
=================================================
*/
	SerializableInputActionsAndroid::EInputType  SerializableInputActionsAndroid::SensorTypeToInputType (const ESensorType sensorType) __NE___
	{
		switch_enum( sensorType )
		{
			#define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )	case ESensorType::_type_ : return EInputType::_type_;
			AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
			#undef AE_ANDROID_SERNSORS_VISITOR

			case ESensorType::Unknown :		break;
		}
		switch_end
		return Default;
	}

/*
=================================================
	SensorTypeToValueType
=================================================
*/
	SerializableInputActionsAndroid::EValueType  SerializableInputActionsAndroid::SensorTypeToValueType (const ESensorType sensorType) __NE___
	{
		switch_enum( sensorType )
		{
			#define AE_ANDROID_SERNSORS_VISITOR( _type_, _bitIndex_, _api_, _valType_, ... )	case ESensorType::_type_ : return EValueType::_valType_;
			AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
			#undef AE_ANDROID_SERNSORS_VISITOR

			case ESensorType::Unknown :		break;
		}
		switch_end
		return Default;
	}

/*
=================================================
	RequiredValueType
=================================================
*/
	SerializableInputActionsAndroid::EValueType  SerializableInputActionsAndroid::RequiredValueType (const InputType_t type) C_NE___
	{
		const auto	input_type = EInputType(type);
		switch ( input_type )
		{
			#define AE_ANDROID_SERNSORS_VISITOR( _type_, _bitIndex_, _api_, _valType_, ... )	case EInputType::_type_ : return EValueType::_valType_;
			AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
			#undef AE_ANDROID_SERNSORS_VISITOR

			case EInputType::MultiTouch :		return EValueType::Float2;	// float2 (scale, rotate)
			case EInputType::TouchPos :			return EValueType::Float2;	// float2 (absolute in pixels)
			case EInputType::TouchPos_mm :		return EValueType::Float2;	// float2 (absolute in mm)
			case EInputType::TouchDelta :		return EValueType::Float2;	// float2 (delta in pixels)
			case EInputType::TouchDelta_norm :	return EValueType::Float2;	// snorm2
		}
		return Default;
	}

} // AE::App
