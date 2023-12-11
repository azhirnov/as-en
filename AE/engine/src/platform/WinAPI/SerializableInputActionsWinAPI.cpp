// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Platforms/WindowsHeader.cpp.h"
#include "pch/Scripting.h"

#include "platform/Private/EnumToString.h"
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
    EInputTypeToString
=================================================
*/
namespace {
    ND_ String  EInputTypeToString (SerializableInputActionsWinAPI::EInputType value)
    {
        using EInputType = SerializableInputActionsWinAPI::EInputType;

        BEGIN_ENUM_CHECKS();
        switch ( value )
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
        END_ENUM_CHECKS();

        return "code_"s << Base::ToString( uint(value) );
    }
}

/*
=================================================
    ToString
=================================================
*/
    String  SerializableInputActionsWinAPI::ToString (const Reflection &refl) C_Th___
    {
        String      str      = "InputActionsWinAPI {\n";
        const auto  mode_arr = _ToArray(_modeMap);

        for (auto& [name, mode] : mode_arr)
        {
            str << "  '" << refl.Get( name ) << "' {\n";
            str << "    lockAndHideCursor: " << Base::ToString( mode->lockAndHideCursor ) << "\n";

            const auto  act_arr = _ToArray(mode->actions);
            for (auto& [key, info] : act_arr)
            {
                auto [code, gesture, state] = _Unpack( key );

                str <<   "    InputKey: " << EInputTypeToString( EInputType(code) ) << ", state: " << Base::ToString( state )
                    << "\n    {"
                    << "\n      name:    '" << refl.Get( info->name ) << "'"
                    << "\n      value:   " << Base::ToString( info->valueType )
                    << "\n      gesture: " << Base::ToString( info->gesture )
                    << "\n      swizzle: " << Base::ToString( info->swizzle )
                    << "\n      scale:   " << Base::ToString( info->GetScale() )
                    << "\n    }\n";
            }

            str << "  }\n";
        }
        str << "}\n\n";

        return str;
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

            #define AE_WINAPI_KEY_CODES_VISITOR( _key_, _code_, _name_, _winapi_code_ )     binder.AddValue( _name_, EInputType::_key_ );
            AE_WINAPI_KEY_CODES( AE_WINAPI_KEY_CODES_VISITOR )
            #undef AE_WINAPI_KEY_CODES_VISITOR

            binder.AddValue( "MouseBtn0",           EInputType::MouseBtn0 );
            binder.AddValue( "MouseBtn1",           EInputType::MouseBtn1 );
            binder.AddValue( "MouseBtn2",           EInputType::MouseBtn2 );
            binder.AddValue( "MouseBtn3",           EInputType::MouseBtn3 );
            binder.AddValue( "MouseBtn4",           EInputType::MouseBtn4 );
            binder.AddValue( "MouseLeft",           EInputType::MouseBtn0 );
            binder.AddValue( "MouseRight",          EInputType::MouseBtn1 );
            binder.AddValue( "MouseMiddle",         EInputType::MouseBtn2 );

            binder.AddValue( "MultiTouch",          EInputType::MultiTouch );

            binder.AddValue( "MouseWheel",          EInputType::MouseWheel );
            binder.AddValue( "CursorPos",           EInputType::CursorPos );
            binder.AddValue( "CursorPos_mm",        EInputType::CursorPos_mm );
            binder.AddValue( "CursorDelta",         EInputType::CursorDelta );
            binder.AddValue( "CursorDelta_norm",    EInputType::CursorDelta_norm );
            binder.AddValue( "TouchPos",            EInputType::TouchPos );
            binder.AddValue( "TouchPos_mm",         EInputType::TouchPos_mm );
            binder.AddValue( "TouchDelta",          EInputType::TouchDelta );
            binder.AddValue( "TouchDelta_norm",     EInputType::TouchDelta_norm );
            StaticAssert( uint(EInputType::Cursor2DBegin) == 57099 );
            StaticAssert( uint(EInputType::Cursor2DEnd)   == 57107 );
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
    bool  SerializableInputActionsWinAPI::LoadFromScript (const ScriptEnginePtr &se, String script, const SourceLoc &loc, Reflection &refl)
    {
        CHECK_ERR( se );
        CHECK_ERR( not script.empty() );

        ScriptActionBindings    bindings{ *this, refl };

        auto    mod = se->CreateModule({ScriptEngine::ModuleSource{ "def"s, RVRef(script), loc, True{"preprocessor"} }});
        CHECK_ERR( mod );

        auto    scr = se->CreateScript< void (ScriptActionBindings *) >( "ASmain", mod );
        CHECK_ERR( scr and scr->Run( &bindings ));

        return true;
    }

#endif // AE_ENABLE_SCRIPTING

} // AE::App
