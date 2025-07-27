// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/Scripting.h"
#include "GLFWCommon.cpp.h"

#include "platform/GLFW/SerializableInputActionsGLFW.h"

namespace AE::App
{
# ifdef AE_ENABLE_GLFW

	#define AE_GLFW_KEY_CODES_VISITOR( _key_, _code_, _name_, _glfw_code_ ) \
		StaticAssert( uint(SerializableInputActionsGLFW::EInputType::_key_) == uint(_glfw_code_) );

	AE_GLFW_KEY_CODES( AE_GLFW_KEY_CODES_VISITOR )
	#undef AE_GLFW_KEY_CODES_VISITOR

# endif

/*
=================================================
	InputTypeToString
=================================================
*/
	String  SerializableInputActionsGLFW::InputTypeToString (InputType_t value) C_Th___
	{
		switch_enum( EInputType(value) )
		{
			#define AE_GLFW_KEY_CODES_VISITOR( _key_, _code_, _name_, ... )	case EInputType::_key_ : return _name_;
			AE_GLFW_KEY_CODES( AE_GLFW_KEY_CODES_VISITOR )
			#undef AE_GLFW_KEY_CODES_VISITOR

			#define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )				case EInputType::_type_ : return AE_TOSTRING( _type_ );
			AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
			#undef AE_ANDROID_SERNSORS_VISITOR

			//case EInputType::MouseBegin :
			//case EInputType::MouseEnd :			break;

			//case EInputType::KeyBegin :
			//case EInputType::KeyEnd :				break;

			case EInputType::MultiTouch :			return "MultiTouch";

			case EInputType::MouseWheel :			return "MouseWheel";
			case EInputType::CursorPos :			return "CursorPos";
			case EInputType::CursorPos_mm :			return "CursorPos_mm";
			case EInputType::CursorDelta :			return "CursorDelta";
			case EInputType::CursorDelta_norm :		return "CursorDelta_norm";

			case EInputType::TouchPos :				return "TouchPos";
			case EInputType::TouchPos_mm :			return "TouchPos_mm";
			case EInputType::TouchDelta :			return "TouchDelta";
			case EInputType::TouchDelta_norm :		return "TouchDelta_norm";

			case EInputType::_Count :
			case EInputType::Unknown :				break;
		}
		switch_end

		return "code_"s << Base::ToString( value );
	}

/*
=================================================
	SensorBitsToString
=================================================
*/
	String  SerializableInputActionsGLFW::SensorBitsToString (ESensorBits bits) C_Th___
	{
		String	str;
		for (auto idx : BitIndexIterate<ESensorType>(bits))
		{
			if ( not str.empty() )
				str << ", ";

			switch_enum( idx )
			{
				#define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )	case ESensorType::Android_ ## _type_ : str << "Android_" #_type_; break;
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
	_InputTypeToSensorType
=================================================
*/
	SerializableInputActionsGLFW::ESensorType  SerializableInputActionsGLFW::_InputTypeToSensorType (const EInputType inputType) __NE___
	{
		switch ( inputType )
		{
			#define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )	case EInputType::_type_ : return ESensorType::Android_ ## _type_;
			AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
			#undef AE_ANDROID_SERNSORS_VISITOR
		}
		return Default;
	}

/*
=================================================
	RequiredValueType
=================================================
*/
	SerializableInputActionsGLFW::EValueType  SerializableInputActionsGLFW::RequiredValueType (const InputType_t type) C_NE___
	{
		const auto	input_type = EInputType(type);
		switch ( input_type )
		{
			#define AE_ANDROID_SERNSORS_VISITOR( _type_, _bitIndex_, _api_, _valType_, ... )	case EInputType::_type_ : return EValueType::_valType_;
			AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
			#undef AE_ANDROID_SERNSORS_VISITOR

			case EInputType::MultiTouch :		return EValueType::Float2;	// float2 (scale, rotate)
			case EInputType::MouseWheel :		return EValueType::Float2;	// float2 (delta)
			case EInputType::CursorPos :		return EValueType::Float2;	// float2 (absolute in pixels)
			case EInputType::CursorPos_mm :		return EValueType::Float2;	// float2 (absolute in mm)
			case EInputType::CursorDelta :		return EValueType::Float2;	// float2 (delta in pixels)
			case EInputType::CursorDelta_norm :	return EValueType::Float2;	// snorm2
			case EInputType::TouchPos :			return EValueType::Float2;	// float2 (absolute in pixels)
			case EInputType::TouchPos_mm :		return EValueType::Float2;	// float2 (absolute in mm)
			case EInputType::TouchDelta :		return EValueType::Float2;	// float2 (delta in pixels)
			case EInputType::TouchDelta_norm :	return EValueType::Float2;	// snorm2
		}
		return Default;
	}

} // AE::App
