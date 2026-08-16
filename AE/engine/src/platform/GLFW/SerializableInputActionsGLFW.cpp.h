// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_SCRIPTING
namespace AE::App
{
	using namespace AE::Scripting;


	//
	// Bindings Mode
	//
	struct SerializableInputActionsGLFW::ScriptBindingsMode final : ScriptBindingsModeBase
	{
	public:
		ScriptBindingsMode () {}
		ScriptBindingsMode (SerializableInputActions& self, InputMode& mode, Reflection &refl) :
			ScriptBindingsModeBase{ self, mode, refl } {}

		void  Add (EInputType type, const ScriptActionInfo &value) __Th___ {
			_Add( ushort(type), value );
		}

		void  Inherit (const String &name) __Th___ {
			_Inherit( name );
		}

		void  LockAndHideCursor () __Th___ {
			_mode->lockAndHideCursor = true;
		}
	};


	//
	// Action Bindings
	//
	struct SerializableInputActionsGLFW::ScriptActionBindings final : ScriptActionBindingsBase
	{
	public:
		ScriptActionBindings () {}
		ScriptActionBindings (SerializableInputActionsGLFW& self, Reflection &refl) :
			ScriptActionBindingsBase{ self, refl } {}

		ScriptBindingsMode*  CreateMode (const String &name) __Th___ {
			return _CreateMode< ScriptBindingsMode >( name );
		}
	};

/*
=================================================
	Bind
=================================================
*/
	void  SerializableInputActionsGLFW::Bind (const ScriptEnginePtr &se) __Th___
	{
		// Input
		{
			EnumBinder<EInputType>	binder{ se };
			binder.Create();
			switch_enum( EInputType::Unknown )
			{
				case EInputType::Unknown :
				case EInputType::_Count :

				#define AE_GLFW_KEY_CODES_VISITOR( _key_, _code_, _name_, ... )			case EInputType::_key_  : binder.AddValue( _name_, EInputType::_key_ );
				AE_GLFW_KEY_CODES( AE_GLFW_KEY_CODES_VISITOR )
				#undef AE_GLFW_KEY_CODES_VISITOR

				#define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )						case EInputType::_type_ : binder.AddValue( #_type_, EInputType::_type_ );
				AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
				#undef AE_ANDROID_SERNSORS_VISITOR

				#define AE_GLFW_GAMEPAD_AXIS_VISITOR( _key_, _code_, _name_, ... )		case EInputType::_key_  : binder.AddValue( _name_, EInputType::_key_ );
				AE_GLFW_GAMEPAD_AXIS( AE_GLFW_GAMEPAD_AXIS_VISITOR )
				#undef AE_GLFW_GAMEPAD_AXIS_VISITOR

				#define AE_GLFW_GAMEPAD_BUTTON_VISITOR( _key_, _code_, _name_, ... )	case EInputType::_key_  : binder.AddValue( _name_, EInputType::_key_ );
				AE_GLFW_GAMEPAD_BUTTON( AE_GLFW_GAMEPAD_BUTTON_VISITOR )
				#undef AE_GLFW_GAMEPAD_BUTTON_VISITOR

				#define BIND( _name_ )													case EInputType::_name_ : binder.AddValue( #_name_, EInputType::_name_ );

				BIND( MultiTouch )

				BIND( MouseWheel )
				BIND( CursorPos )
				BIND( CursorPos_mm )
				BIND( CursorDelta )
				BIND( CursorDelta_norm )

				BIND( TouchPos )
				BIND( TouchPos_mm )
				BIND( TouchDelta )
				BIND( TouchDelta_norm )

				#undef BIND
				default : break;
			}
			switch_end
			binder.AddValue( "MouseLeft",	EInputType::MouseLeft );
			binder.AddValue( "MouseRight",	EInputType::MouseRight );
			binder.AddValue( "MouseMiddle",	EInputType::MouseMiddle );
		}

		// BindingsMode
		{
			ClassBinder<ScriptBindingsMode>		binder{ se };
			binder.CreateRef();

			AS_METHOD( binder, ScriptBindingsMode::Add,					"Add"				);
			AS_METHOD( binder, ScriptBindingsMode::Inherit,				"Inherit"			);
			AS_METHOD( binder, ScriptBindingsMode::LockAndHideCursor,	"LockAndHideCursor"	);
		}

		// ActionBindings
		{
			ClassBinder<ScriptActionBindings>	binder{ se };
			binder.CreateRef( &Scripting::AngelScriptHelper::FactoryCreate<ScriptActionBindings>, null, null, 0 );

			AS_METHOD( binder, ScriptActionBindings::CreateMode, "CreateMode" );
		}
	}

/*
=================================================
	LoadFromScript
=================================================
*/
	bool  SerializableInputActionsGLFW::LoadFromScript (const Scripting::ScriptEnginePtr &se, String script, ArrayView<Path> includeDirs,
														const SourceLoc &loc, INOUT Reflection &refl) __NE___
	{
		CHECK_ERR( se );
		CHECK_ERR( not script.empty() );

		ScriptActionBindings	bindings{ *this, refl };

		auto	mod = se->CreateModule(	{ScriptEngine::ModuleSource{ "def"s, RVRef(script), loc, True{"preprocessor"} }},
										Default,
										includeDirs );
		CHECK_ERR( mod );

		auto	scr = se->CreateScript< void (ScriptActionBindings *) >( "ASmain", mod );
		CHECK_ERR( scr and scr->Run( &bindings ));

		// enable sensors
		for (auto [name, mode] : _modeMap)
		{
			ASSERT( mode.enableSensors == Default );

			for (const auto& [key, info] : mode.actions)
			{
				auto code = EInputType(_Unpack( key ).Get<0>());

				ESensorType	sensor = _InputTypeToSensorType( EInputType(code) );

				mode.enableSensors |= ESensorBits(1ull << uint(sensor));
			}
		}

		return true;
	}

} // AE::App
#endif // AE_ENABLE_SCRIPTING
