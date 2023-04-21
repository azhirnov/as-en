// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Private/SerializableInputActions.h"

namespace AE::App
{
	static constexpr uint	InputActionsOpenVR_Name	= uint("ia.OVR"_StringToID);


	//
	// Serializable Input Actions for OpenVR
	//

	class SerializableInputActionsOpenVR final : public SerializableInputActions
	{
		friend class InputActionsOpenVR;

	// types
	public:
		#define AE_OPENVR_KEY_CODES( _visitor_ ) \
			_visitor_( Key_Space,			32,		"Space",		GLFW_KEY_SPACE			)\

		enum class EInputType : ushort
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
		SerializableInputActionsOpenVR () : SerializableInputActions{_Version} {}


	// SerializableInputActions //
		bool  IsKey (ushort type)			const override	{ return _IsKey( EInputType(type) ); }
		bool  IsKeyOrTouch (ushort type)	const override	{ return _IsKeyOrTouch( EInputType(type) ); }
		bool  IsCursor1D (ushort type)		const override	{ return _IsCursor1D( EInputType(type) ); }
		bool  IsCursor2D (ushort type)		const override	{ return _IsCursor2D( EInputType(type) ); }
		
		String  ToString ()					const override;

	  #ifdef AE_ENABLE_SCRIPTING
		bool  LoadFromScript (const Scripting::ScriptEnginePtr &se, String script, const SourceLoc &loc) override;

		static void  Bind (const Scripting::ScriptEnginePtr &se) __Th___;
	  #endif

	private:
		ND_ static constexpr bool  _IsKey (EInputType type);
		ND_ static constexpr bool  _IsKeyOrTouch (EInputType type);
		ND_ static constexpr bool  _IsCursor1D (EInputType type);
		ND_ static constexpr bool  _IsCursor2D (EInputType type);
	};

	
/*
=================================================
	_Is***
=================================================
*/
	forceinline constexpr bool  SerializableInputActionsOpenVR::_IsKey (EInputType) {
		return false;	//((type >= EInputType::MouseBegin) & (type <= EInputType::MouseEnd)) |
				//((type >= EInputType::KeyBegin)   & (type <= EInputType::KeyEnd));
	}
	
	forceinline constexpr bool  SerializableInputActionsOpenVR::_IsKeyOrTouch (EInputType) {
		return false;
	}

	forceinline constexpr bool  SerializableInputActionsOpenVR::_IsCursor1D (EInputType) {
		return false; //(type >= EInputType::Cursor1DBegin) & (type <= EInputType::Cursor1DEnd);
	}

	forceinline constexpr bool  SerializableInputActionsOpenVR::_IsCursor2D (EInputType) {
		return false; //(type >= EInputType::Cursor2DBegin) & (type <= EInputType::Cursor2DEnd);
	}


} // AE::App


#ifdef AE_ENABLE_SCRIPTING
namespace AE::Scripting
{
	AE_DECL_SCRIPT_OBJ_RC(	AE::App::SerializableInputActionsOpenVR::ScriptBindingsMode,	"OpenVR_BindingsMode"	);
	AE_DECL_SCRIPT_OBJ(		AE::App::SerializableInputActionsOpenVR::ScriptActionBindings,	"OpenVR_ActionBindings"	);
	AE_DECL_SCRIPT_TYPE(	AE::App::SerializableInputActionsOpenVR::EInputType,			"OpenVR_Input"			);
}
#endif
