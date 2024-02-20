// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Platforms/WindowsHeader.cpp.h"
#include "pch/Scripting.h"

#include "platform/WinAPI/SerializableInputActionsWinAPI.h"

namespace AE::App
{
# ifdef AE_PLATFORM_WINDOWS

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
            #define AE_WINAPI_KEY_CODES_VISITOR( _key_, _code_, _name_, _and_code_ )    case EInputType::_key_ :    return _name_;
            AE_WINAPI_KEY_CODES( AE_WINAPI_KEY_CODES_VISITOR )
            #undef AE_WINAPI_KEY_CODES_VISITOR

            case EInputType::KeyEnd :           break;

            case EInputType::MouseBtn0 :        return "MouseBtn0";
            case EInputType::MouseBtn1 :        return "MouseBtn1";
            case EInputType::MouseBtn2 :        return "MouseBtn2";
            case EInputType::MouseBtn3 :        return "MouseBtn3";
            case EInputType::MouseBtn4 :        return "MouseBtn4";

            case EInputType::MultiTouch :       return "MultiTouch";

            case EInputType::MouseWheel :       return "MouseWheel";
            case EInputType::CursorPos :        return "CursorPos";
            case EInputType::CursorPos_mm :     return "CursorPos_mm";
            case EInputType::CursorDelta :      return "CursorDelta";
            case EInputType::CursorDelta_norm : return "CursorDelta_norm";
            case EInputType::TouchPos :         return "TouchPos";
            case EInputType::TouchPos_mm :      return "TouchPos_mm";
            case EInputType::TouchDelta :       return "TouchDelta";
            case EInputType::TouchDelta_norm :  return "TouchDelta_norm";

            case EInputType::_Count :
            case EInputType::Unknown :          break;
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
        CHECK( bits == Default );   // not supported
        return Default;
    }

/*
=================================================
    RequiredValueType
=================================================
*/
    SerializableInputActionsWinAPI::EValueType  SerializableInputActionsWinAPI::RequiredValueType (const InputType_t type) C_NE___
    {
        const auto  input_type = EInputType(type);
        switch ( input_type )
        {
            case EInputType::MultiTouch :       return EValueType::Float2;  // float2 (scale, rotate)
            case EInputType::MouseWheel :       return EValueType::Float2;  // float2 (delta)
            case EInputType::CursorPos :        return EValueType::Float2;  // float2 (absolute in pixels)
            case EInputType::CursorPos_mm :     return EValueType::Float2;  // float2 (absolute in mm)
            case EInputType::CursorDelta :      return EValueType::Float2;  // float2 (delta in pixels)
            case EInputType::CursorDelta_norm : return EValueType::Float2;  // snorm2
            case EInputType::TouchPos :         return EValueType::Float2;  // float2 (absolute in pixels)
            case EInputType::TouchPos_mm :      return EValueType::Float2;  // float2 (absolute in mm)
            case EInputType::TouchDelta :       return EValueType::Float2;  // float2 (delta in pixels)
            case EInputType::TouchDelta_norm :  return EValueType::Float2;  // snorm2
        }
        return Default;
    }
//-----------------------------------------------------------------------------


#ifdef AE_ENABLE_SCRIPTING
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
            EnumBinder<EInputType>  binder{ se };
            binder.Create();
            switch_enum( EInputType::Unknown )
            {
                case EInputType::Unknown :
                case EInputType::_Count :
                case EInputType::KeyEnd :

                #define BIND( _name_ )                                              case EInputType::_name_ : binder.AddValue( #_name_, EInputType::_name_ );
                #define AE_WINAPI_KEY_CODES_VISITOR( _key_, _code_, _name_, ... )   case EInputType::_key_  : binder.AddValue( _name_, EInputType::_key_ );
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
            binder.AddValue( "MouseLeft",   EInputType::MouseBtn0 );
            binder.AddValue( "MouseRight",  EInputType::MouseBtn1 );
            binder.AddValue( "MouseMiddle", EInputType::MouseBtn2 );
        }

        // BindingsMode
        {
            ClassBinder<ScriptBindingsMode>  binder{ se };
            binder.CreateRef();

            binder.AddMethod( &ScriptBindingsMode::Add,                 "Add"               );
            binder.AddMethod( &ScriptBindingsMode::Inherit,             "Inherit"           );
            binder.AddMethod( &ScriptBindingsMode::LockAndHideCursor,   "LockAndHideCursor" );
        }

        // ActionBindings
        {
            ClassBinder<ScriptActionBindings>   binder{ se };
            binder.CreateRef( &Scripting::AngelScriptHelper::FactoryCreate<ScriptActionBindings>, null, null, 0 );

            binder.AddMethod( &ScriptActionBindings::CreateMode, "CreateMode" );
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

        ScriptActionBindings    bindings{ *this, refl };

        auto    mod = se->CreateModule( {ScriptEngine::ModuleSource{ "def"s, RVRef(script), loc, True{"preprocessor"} }},
                                        Default,
                                        includeDirs );
        CHECK_ERR( mod );

        auto    scr = se->CreateScript< void (ScriptActionBindings *) >( "ASmain", mod );
        CHECK_ERR( scr and scr->Run( &bindings ));

        return true;
    }

#endif // AE_ENABLE_SCRIPTING

} // AE::App
