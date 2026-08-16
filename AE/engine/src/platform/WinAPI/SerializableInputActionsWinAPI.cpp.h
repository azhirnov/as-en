// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_SCRIPTING
namespace AE::App
{
	using namespace AE::Scripting;


	//
	// Bindings Mode
	//
	struct SerializableInputActionsWinAPI::ScriptBindingsMode final : ScriptBindingsModeBase
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
	struct SerializableInputActionsWinAPI::ScriptActionBindings final : ScriptActionBindingsBase
	{
	public:
		ScriptActionBindings () {}
		ScriptActionBindings (SerializableInputActionsWinAPI& self, Reflection &refl) :
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
	void  SerializableInputActionsWinAPI::Bind (const ScriptEnginePtr &se) __Th___
	{
		// Input
		{
			EnumBinder<EInputType>	binder{ se };
			binder.Create();
			switch_enum( EInputType::Unknown )
			{
				case EInputType::Unknown :
				case EInputType::_Count :
				case EInputType::KeyEnd :

				#define BIND( _name_ )												case EInputType::_name_ : binder.AddValue( #_name_, EInputType::_name_ );
				#define AE_WINAPI_KEY_CODES_VISITOR( _key_, _code_, _name_, ... )	case EInputType::_key_  : binder.AddValue( _name_, EInputType::_key_ );
				AE_WINAPI_KEY_CODES( AE_WINAPI_KEY_CODES_VISITOR )

				BIND( MouseBtn0 )
				BIND( MouseBtn1 )
				BIND( MouseBtn2 )
				BIND( MouseBtn3 )
				BIND( MouseBtn4 )

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

				#undef AE_WINAPI_KEY_CODES_VISITOR
				#undef BIND
				default : break;
			}
			switch_end
			binder.AddValue( "MouseLeft",	EInputType::MouseBtn0 );
			binder.AddValue( "MouseRight",	EInputType::MouseBtn1 );
			binder.AddValue( "MouseMiddle",	EInputType::MouseBtn2 );
		}

		// BindingsMode
		{
			ClassBinder<ScriptBindingsMode>	 binder{ se };
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
	bool  SerializableInputActionsWinAPI::LoadFromScript (const Scripting::ScriptEnginePtr &se, String script, ArrayView<Path> includeDirs,
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

		return true;
	}

} // AE::App
#endif // AE_ENABLE_SCRIPTING
