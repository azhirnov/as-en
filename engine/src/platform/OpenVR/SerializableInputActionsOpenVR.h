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
		bool  IsKey (ushort type)		const override	{ return _IsKey( EInputType(type) ); }
		bool  IsCursor1D (ushort type)	const override	{ return _IsCursor1D( EInputType(type) ); }
		bool  IsCursor2D (ushort type)	const override	{ return _IsCursor2D( EInputType(type) ); }
		
	  #if not AE_OPTIMIZE_IDS
		String  ToString () const override;
	  #endif

	  #ifdef AE_ENABLE_SCRIPTING
		bool  LoadFromScript (const Scripting::ScriptEnginePtr &se, String script, const SourceLoc &loc) override;

		ND_ static bool  Bind (const Scripting::ScriptEnginePtr &se);
	  #endif

	private:
		ND_ static constexpr bool  _IsKey (EInputType type);
		ND_ static constexpr bool  _IsCursor1D (EInputType type);
		ND_ static constexpr bool  _IsCursor2D (EInputType type);
	};


} // AE::App


#ifdef AE_ENABLE_SCRIPTING
namespace AE::Scripting
{
	AE_DECL_SCRIPT_OBJ_RC(	AE::App::SerializableInputActionsOpenVR::ScriptBindingsMode,	"OpenVR_BindingsMode"	);
	AE_DECL_SCRIPT_OBJ(		AE::App::SerializableInputActionsOpenVR::ScriptActionBindings,	"OpenVR_ActionBindings"	);
	AE_DECL_SCRIPT_TYPE(	AE::App::SerializableInputActionsOpenVR::EInputType,			"OpenVR_Input"			);
}
#endif
