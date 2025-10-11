// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/OpenXR/OpenXRCommon.h"
#include "platform/OpenXR/SerializableInputActionsOpenXR.h"

namespace AE::App
{

	#define AE_OPENXR_KEY_CODES_VISITOR( _key_, _code_, _name_, _ovr_code_ )
	//	StaticAssert( uint(SerializableInputActionsOpenXR::EInputType::_key_) == uint(_ovr_code_) );

	AE_OPENXR_KEY_CODES( AE_OPENXR_KEY_CODES_VISITOR )
	#undef AE_OPENXR_KEY_CODES_VISITOR

/*
=================================================
	InputTypeToString
=================================================
*/
	String  SerializableInputActionsOpenXR::InputTypeToString (InputType_t value) C_Th___
	{
		switch_enum( EInputType(value) )
		{
			#define AE_OPENXR_KEY_CODES_VISITOR( _key_, _code_, _name_, _ovr_code_ )	case EInputType::_key_ :	return _name_;
			AE_OPENXR_KEY_CODES( AE_OPENXR_KEY_CODES_VISITOR )
			#undef AE_OPENXR_KEY_CODES_VISITOR

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
	String  SerializableInputActionsOpenXR::SensorBitsToString (ESensorBits bits) C_Th___
	{
		CHECK( bits == Default );	// not supported
		return Default;
	}

/*
=================================================
	RequiredValueType
=================================================
*/
	SerializableInputActionsOpenXR::EValueType  SerializableInputActionsOpenXR::RequiredValueType (InputType_t) C_NE___
	{
		// TODO
		return Default;
	}

} // AE::App
