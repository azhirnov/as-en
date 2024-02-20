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
    //  StaticAssert( uint(SerializableInputActionsOpenVR::EInputType::_key_) == uint(_ovr_code_) );

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
            #define AE_OPENVR_KEY_CODES_VISITOR( _key_, _code_, _name_, _ovr_code_ )    case EInputType::_key_ :    return _name_;
            AE_OPENVR_KEY_CODES( AE_OPENVR_KEY_CODES_VISITOR )
            #undef AE_OPENVR_KEY_CODES_VISITOR

            case EInputType::_Count :
            case EInputType::Unknown :  break;
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
        CHECK( bits == Default );   // not supported
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
            switch_enum( EInputType::Unknown )
            {
                case EInputType::Unknown :
                case EInputType::_Count :
                #define AE_OPENVR_KEY_CODES_VISITOR( _key_, _code_, _name_, ... )   case EInputType::_key_  : binder.AddValue( _name_, EInputType::_key_ );
                AE_OPENVR_KEY_CODES( AE_OPENVR_KEY_CODES_VISITOR )
                #undef AE_OPENVR_KEY_CODES_VISITOR
                default : break;
            }
            switch_end
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
    bool  SerializableInputActionsOpenVR::LoadFromScript (const Scripting::ScriptEnginePtr &se, String script, ArrayView<Path> includeDirs,
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
