// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Private/SerializableInputActions.h"

namespace AE::App
{
	static constexpr uint	InputActionsOpenVR_Name	= "ia.OVR"_Hash;


	//
	// Serializable Input Actions for OpenVR
	//

	class SerializableInputActionsOpenVR final : public SerializableInputActions
	{
		friend class InputActionsOpenVR;

	// types
	public:
		#define AE_OPENVR_KEY_CODES( _visitor_ )

		enum class EInputType : InputType_t
		{
			#define AE_OPENVR_KEY_CODES_VISITOR( _key_, _code_, _name_, _ovr_code_ )		_key_ = _code_,
			AE_OPENVR_KEY_CODES( AE_OPENVR_KEY_CODES_VISITOR )
			#undef AE_OPENVR_KEY_CODES_VISITOR

			_Count,
			Unknown				= 0xFFFF,
		};

		struct ScriptBindingsMode;
		struct ScriptActionBindings;

	private:
		// for serialization
		static constexpr uint	_Version	= (1u << 16) | _BaseVersion;


	// methods
	public:
		SerializableInputActionsOpenVR ()								__NE___	: SerializableInputActions{_Version} {}


	// SerializableInputActions //
		bool  IsKey (InputType_t type)									C_NE_OV	{ return _IsKey( EInputType(type) ); }
		bool  IsKeyOrTouch (InputType_t type)							C_NE_OV	{ return _IsKeyOrTouch( EInputType(type) ); }
		bool  IsVec1D (InputType_t type)								C_NE_OV	{ return _IsVec1D( EInputType(type) ); }
		bool  IsVec2D (InputType_t type)								C_NE_OV	{ return _IsVec2D( EInputType(type) ); }
		bool  IsVec3D (InputType_t)										C_NE_OV	{ return false; }

		EValueType	RequiredValueType (InputType_t inputType)			C_NE_OV;
		String		InputTypeToString (InputType_t)						C_Th_OV;
		String		SensorBitsToString (ESensorBits)					C_Th_OV;
		StringView	GetApiName ()										C_NE_OV	{ return "OpenVR"; }

	  #ifdef AE_ENABLE_SCRIPTING
		bool  LoadFromScript (const Scripting::ScriptEnginePtr &se, String script,
							  ArrayView<Path> includeDirs, const SourceLoc &loc,
							  INOUT Reflection &refl)					__NE_OV;

		static void  Bind (const Scripting::ScriptEnginePtr &se)		__Th___;
	  #endif

	private:
		ND_ static constexpr bool  _IsKey (EInputType type)				__NE___;
		ND_ static constexpr bool  _IsKeyOrTouch (EInputType type)		__NE___;
		ND_ static constexpr bool  _IsVec1D (EInputType type)			__NE___;
		ND_ static constexpr bool  _IsVec2D (EInputType type)			__NE___;
	};


/*
=================================================
	_Is***
=================================================
*/
	inline constexpr bool  SerializableInputActionsOpenVR::_IsKey (EInputType) __NE___ {
		return false;	//((type >= EInputType::MouseBegin) and (type <= EInputType::MouseEnd)) or
				//((type >= EInputType::KeyBegin)   and (type <= EInputType::KeyEnd));
	}

	inline constexpr bool  SerializableInputActionsOpenVR::_IsKeyOrTouch (EInputType) __NE___ {
		return false;
	}

	inline constexpr bool  SerializableInputActionsOpenVR::_IsVec1D (EInputType) __NE___ {
		return false; //(type >= EInputType::Cursor1DBegin) and (type <= EInputType::Cursor1DEnd);
	}

	inline constexpr bool  SerializableInputActionsOpenVR::_IsVec2D (EInputType) __NE___ {
		return false; //(type >= EInputType::Cursor2DBegin) and (type <= EInputType::Cursor2DEnd);
	}


} // AE::App


#ifdef AE_ENABLE_SCRIPTING
	AE_DECL_SCRIPT_OBJ_RC(	AE::App::SerializableInputActionsOpenVR::ScriptBindingsMode,	"OpenVR_BindingsMode"	);
	AE_DECL_SCRIPT_OBJ(		AE::App::SerializableInputActionsOpenVR::ScriptActionBindings,	"OpenVR_ActionBindings"	);
	AE_DECL_SCRIPT_TYPE(	AE::App::SerializableInputActionsOpenVR::EInputType,			"OpenVR_Input"			);
#endif
