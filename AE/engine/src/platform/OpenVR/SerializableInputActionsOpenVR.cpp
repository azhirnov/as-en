// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/Scripting.h"

#ifdef AE_ENABLE_OPENVR
# include "openvr_capi.h"
#endif

#include "platform/Private/EnumToString.h"
#include "platform/OpenVR/SerializableInputActionsOpenVR.h"

namespace AE::App
{
# ifdef AE_ENABLE_OPENVR

    #define AE_OPENVR_KEY_CODES_VISITOR( _key_, _code_, _name_, _ovr_code_ )
    //  STATIC_ASSERT( uint(SerializableInputActionsOpenVR::EInputType::_key_) == uint(_ovr_code_) );

    AE_OPENVR_KEY_CODES( AE_OPENVR_KEY_CODES_VISITOR )
    #undef AE_OPENVR_KEY_CODES_VISITOR

# endif

/*
=================================================
    EInputTypeToString
=================================================
*/
namespace {
    ND_ String  EInputTypeToString (SerializableInputActionsOpenVR::EInputType value)
    {
        using EInputType = SerializableInputActionsOpenVR::EInputType;

        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            #define AE_OPENVR_KEY_CODES_VISITOR( _key_, _code_, _name_, _ovr_code_ )    case EInputType::_key_ :    return _name_;
            AE_OPENVR_KEY_CODES( AE_OPENVR_KEY_CODES_VISITOR )
            #undef AE_OPENVR_KEY_CODES_VISITOR

            case EInputType::_Count :
            case EInputType::Unknown :  break;
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
    String  SerializableInputActionsOpenVR::ToString (const Reflection &refl) C_Th___
    {
        String      str      = "InputActionsOpenVR {\n";
        const auto  mode_arr = _ToArray(_modeMap);

        for (auto& [name, mode] : mode_arr)
        {
            str << "  '" << refl.Get( name ) << "' {\n";

            const auto  act_arr = _ToArray( mode->actions );
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
    struct SerializableInputActionsOpenVR::ScriptBindingsMode final : ScriptBindingsModeBase
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
    };


    //
    // Action Bindings
    //
    struct SerializableInputActionsOpenVR::ScriptActionBindings final : ScriptActionBindingsBase
    {
    public:
        ScriptActionBindings () {}
        ScriptActionBindings (SerializableInputActionsOpenVR& self, Reflection &refl) :
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
    void  SerializableInputActionsOpenVR::Bind (const ScriptEnginePtr &se) __Th___
    {
        // Input
        {
            EnumBinder<EInputType>  binder{ se };
            binder.Create();

            #define AE_OPENVR_KEY_CODES_VISITOR( _key_, _code_, _name_, _ovr_code_ )    binder.AddValue( _name_, EInputType::_key_ );
            AE_OPENVR_KEY_CODES( AE_OPENVR_KEY_CODES_VISITOR )
            #undef AE_OPENVR_KEY_CODES_VISITOR
        }

        // BindingsMode
        {
            ClassBinder<ScriptBindingsMode>     binder{ se };
            binder.CreateRef();

            binder.AddMethod( &ScriptBindingsMode::Add,     "Add"       );
            binder.AddMethod( &ScriptBindingsMode::Inherit, "Inherit"   );
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
    bool  SerializableInputActionsOpenVR::LoadFromScript (const Scripting::ScriptEnginePtr &se, String script, const SourceLoc &loc, Reflection &refl)
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
