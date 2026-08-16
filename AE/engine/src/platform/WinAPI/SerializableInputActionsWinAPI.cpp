// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "base/Platforms/WindowsHeader.cpp.h"
#include "pch/Scripting.h"

#include "platform/WinAPI/SerializableInputActionsWinAPI.h"

namespace AE::App
{
# ifdef AE_PLATFORM_WINDOWS

	static constexpr auto	c_RawCharType = SerializableInputActions::c_RawCharType;

	#define AE_WINAPI_KEY_CODES_VISITOR( _key_, _code_, _name_, _winapi_code_ ) \
		StaticAssert( uint(SerializableInputActionsWinAPI::EInputType::_key_) == uint(_winapi_code_) );

	AE_WINAPI_KEY_CODES( AE_WINAPI_KEY_CODES_VISITOR )
	#undef AE_WINAPI_KEY_CODES_VISITOR

# endif

/*
=================================================
	InputTypeToString
=================================================
*/
	String  SerializableInputActionsWinAPI::InputTypeToString (InputType_t value) C_Th___
	{
		switch_enum( EInputType(value) )
		{
			#define AE_WINAPI_KEY_CODES_VISITOR( _key_, _code_, _name_, _and_code_ )	case EInputType::_key_ :	return _name_;
			AE_WINAPI_KEY_CODES( AE_WINAPI_KEY_CODES_VISITOR )
			#undef AE_WINAPI_KEY_CODES_VISITOR

			case EInputType::KeyEnd :			break;

			case EInputType::MouseBtn0 :		return "MouseBtn0";
			case EInputType::MouseBtn1 :		return "MouseBtn1";
			case EInputType::MouseBtn2 :		return "MouseBtn2";
			case EInputType::MouseBtn3 :		return "MouseBtn3";
			case EInputType::MouseBtn4 :		return "MouseBtn4";

			case EInputType::MultiTouch :		return "MultiTouch";

			case EInputType::MouseWheel :		return "MouseWheel";
			case EInputType::CursorPos :		return "CursorPos";
			case EInputType::CursorPos_mm :		return "CursorPos_mm";
			case EInputType::CursorDelta :		return "CursorDelta";
			case EInputType::CursorDelta_norm :	return "CursorDelta_norm";
			case EInputType::TouchPos :			return "TouchPos";
			case EInputType::TouchPos_mm :		return "TouchPos_mm";
			case EInputType::TouchDelta :		return "TouchDelta";
			case EInputType::TouchDelta_norm :	return "TouchDelta_norm";

			case EInputType::_Count :
			case EInputType::Unknown :			break;
		}
		switch_end
		return "code_"s << Base::ToString( value );
	}

/*
=================================================
	SensorBitsToString
=================================================
*/
	String  SerializableInputActionsWinAPI::SensorBitsToString (ESensorBits bits) C_Th___
	{
		CHECK( bits == Default );	// not supported
		return Default;
	}

/*
=================================================
	RequiredValueType
=================================================
*/
	SerializableInputActionsWinAPI::EValueType  SerializableInputActionsWinAPI::RequiredValueType (const InputType_t type) C_NE___
	{
		const auto	input_type = EInputType(type);
		switch ( input_type )
		{
			case EInputType::MultiTouch	:		return EValueType::Float2;	// float2 (scale, rotate)
			case EInputType::MouseWheel	:		return EValueType::Float2;	// float2 (delta)
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
