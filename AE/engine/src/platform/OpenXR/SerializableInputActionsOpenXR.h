// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Private/SerializableInputActions.h"

namespace AE::App
{
	static constexpr uint	InputActionsOpenXR_Name	= "ia.OXR"_Hash;


	//
	// Serializable Input Actions for OpenXR
	//

	class SerializableInputActionsOpenXR final : public SerializableInputActions
	{
		friend class InputActionsOpenXR;

	// types
	public:
		#define AE_OPENXR_KEY_CODES( _visitor_ )

		enum class EInputType : InputType_t
		{
			#define AE_OPENXR_KEY_CODES_VISITOR( _key_, _code_, _name_, _ovr_code_ )		_key_ = _code_,
			AE_OPENXR_KEY_CODES( AE_OPENXR_KEY_CODES_VISITOR )
			#undef AE_OPENXR_KEY_CODES_VISITOR

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
		SerializableInputActionsOpenXR ()								__NE___	: SerializableInputActions{_Version} {}


	// SerializableInputActions //
		bool  IsKey (InputType_t type)									C_NE_OV	{ return _IsKey( EInputType(type) ); }
		bool  IsKeyOrTouch (InputType_t type)							C_NE_OV	{ return _IsKeyOrTouch( EInputType(type) ); }
		bool  IsVec1D (InputType_t type)								C_NE_OV	{ return _IsVec1D( EInputType(type) ); }
		bool  IsVec2D (InputType_t type)								C_NE_OV	{ return _IsVec2D( EInputType(type) ); }
		bool  IsVec3D (InputType_t)										C_NE_OV	{ return false; }

		EValueType	RequiredValueType (InputType_t inputType)			C_NE_OV;
		String		InputTypeToString (InputType_t)						C_Th_OV;
		String		SensorBitsToString (ESensorBits)					C_Th_OV;
		StringView	GetApiName ()										C_NE_OV	{ return "OpenXR"; }

	  #ifdef AE_ENABLE_SCRIPTING
		bool  LoadFromScript (const Scripting::ScriptEnginePtr &se, String script,
							  ArrayView<Path> includeDirs, const SourceLoc &loc,
							  INOUT Reflection &refl)					__NE___;

		static void  Bind (const Scripting::ScriptEnginePtr &se)		__Th___;
	  #endif

	private:
		NdCx__ static bool  _IsKey (EInputType type)					__NE___;
		NdCx__ static bool  _IsKeyOrTouch (EInputType type)				__NE___;
		NdCx__ static bool  _IsVec1D (EInputType type)					__NE___;
		NdCx__ static bool  _IsVec2D (EInputType type)					__NE___;
	};


/*
=================================================
	_Is***
=================================================
*/
	__CxIn bool  SerializableInputActionsOpenXR::_IsKey (EInputType) __NE___ {
		return false;	//((type >= EInputType::MouseBegin) and (type <= EInputType::MouseEnd)) or
				//((type >= EInputType::KeyBegin)   and (type <= EInputType::KeyEnd));
	}

	__CxIn bool  SerializableInputActionsOpenXR::_IsKeyOrTouch (EInputType) __NE___ {
		return false;
	}

	__CxIn bool  SerializableInputActionsOpenXR::_IsVec1D (EInputType) __NE___ {
		return false; //(type >= EInputType::Cursor1DBegin) and (type <= EInputType::Cursor1DEnd);
	}

	__CxIn bool  SerializableInputActionsOpenXR::_IsVec2D (EInputType) __NE___ {
		return false; //(type >= EInputType::Cursor2DBegin) and (type <= EInputType::Cursor2DEnd);
	}


} // AE::App


#ifdef AE_ENABLE_SCRIPTING
	AE_DECL_SCRIPT_OBJ_RC(	AE::App::SerializableInputActionsOpenXR::ScriptBindingsMode,	"OpenXR_BindingsMode"	);
	AE_DECL_SCRIPT_OBJ(		AE::App::SerializableInputActionsOpenXR::ScriptActionBindings,	"OpenXR_ActionBindings"	);
	AE_DECL_SCRIPT_TYPE(	AE::App::SerializableInputActionsOpenXR::EInputType,			"OpenXR_Input"			);
#endif
