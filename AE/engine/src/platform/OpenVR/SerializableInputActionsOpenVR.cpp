// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/Scripting.h"

#ifdef AE_ENABLE_OPENVR
# include "openvr_capi.h"
#endif

#include "platform/OpenVR/SerializableInputActionsOpenVR.h"

namespace AE::App
{
# ifdef AE_ENABLE_OPENVR

	#define AE_OPENVR_KEY_CODES_VISITOR( _key_, _code_, _name_, _ovr_code_ )
	//	StaticAssert( uint(SerializableInputActionsOpenVR::EInputType::_key_) == uint(_ovr_code_) );

	AE_OPENVR_KEY_CODES( AE_OPENVR_KEY_CODES_VISITOR )
	#undef AE_OPENVR_KEY_CODES_VISITOR

# endif

/*
=================================================
	InputTypeToString
=================================================
*/
	String  SerializableInputActionsOpenVR::InputTypeToString (InputType_t value) C_Th___
	{
		switch_enum( EInputType(value) )
		{
			#define AE_OPENVR_KEY_CODES_VISITOR( _key_, _code_, _name_, _ovr_code_ )	case EInputType::_key_ :	return _name_;
			AE_OPENVR_KEY_CODES( AE_OPENVR_KEY_CODES_VISITOR )
			#undef AE_OPENVR_KEY_CODES_VISITOR

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
	String  SerializableInputActionsOpenVR::SensorBitsToString (ESensorBits bits) C_Th___
	{
		CHECK( bits == Default );	// not supported
		return Default;
	}

/*
=================================================
	RequiredValueType
=================================================
*/
	SerializableInputActionsOpenVR::EValueType  SerializableInputActionsOpenVR::RequiredValueType (InputType_t) C_NE___
	{
		// TODO
		return Default;
	}

} // AE::App
