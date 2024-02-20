// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/Scripting.h"

#ifdef AE_ENABLE_GLFW
# include "GLFW/glfw3.h"
#endif

#include "platform/GLFW/SerializableInputActionsGLFW.h"

namespace AE::App
{
# ifdef AE_ENABLE_GLFW

    #define AE_GLFW_KEY_CODES_VISITOR( _key_, _code_, _name_, _glfw_code_ ) \
        StaticAssert( uint(SerializableInputActionsGLFW::EInputType::_key_) == uint(_glfw_code_) );

    AE_GLFW_KEY_CODES( AE_GLFW_KEY_CODES_VISITOR )
    #undef AE_GLFW_KEY_CODES_VISITOR

# endif

/*
=================================================
    InputTypeToString
=================================================
*/
    String  SerializableInputActionsGLFW::InputTypeToString (InputType_t value) C_Th___
    {
        switch_enum( EInputType(value) )
        {
            #define AE_GLFW_KEY_CODES_VISITOR( _key_, _code_, _name_, ... ) case EInputType::_key_ : return _name_;
            AE_GLFW_KEY_CODES( AE_GLFW_KEY_CODES_VISITOR )
            #undef AE_GLFW_KEY_CODES_VISITOR

            #define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )              case EInputType::_type_ : return AE_TOSTRING( _type_ );
            AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
            #undef AE_ANDROID_SERNSORS_VISITOR

            //case EInputType::MouseBegin :
            //case EInputType::MouseEnd :           break;

            //case EInputType::KeyBegin :
            //case EInputType::KeyEnd :             break;

            case EInputType::MultiTouch :           return "MultiTouch";

            case EInputType::MouseWheel :           return "MouseWheel";
            case EInputType::CursorPos :            return "CursorPos";
            case EInputType::CursorPos_mm :         return "CursorPos_mm";
            case EInputType::CursorDelta :          return "CursorDelta";
            case EInputType::CursorDelta_norm :     return "CursorDelta_norm";

            case EInputType::TouchPos :             return "TouchPos";
            case EInputType::TouchPos_mm :          return "TouchPos_mm";
            case EInputType::TouchDelta :           return "TouchDelta";
            case EInputType::TouchDelta_norm :      return "TouchDelta_norm";

            case EInputType::_Count :
            case EInputType::Unknown :              break;
        }
        switch_end

        return "code_"s << Base::ToString( value );
    }

/*
=================================================
    SensorBitsToString
=================================================
*/
    String  SerializableInputActionsGLFW::SensorBitsToString (ESensorBits bits) C_Th___
    {
        String  str;
        for (auto idx : BitIndexIterate<ESensorType>(bits))
        {
            if ( not str.empty() )
                str << ", ";

            switch_enum( idx )
            {
                #define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )  case ESensorType::Android_ ## _type_ : str << "Android_" #_type_; break;
                AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
                #undef AE_ANDROID_SERNSORS_VISITOR

                case ESensorType::Unknown : break;
            }
            switch_end
        }
        return str;
    }

/*
=================================================
    _InputTypeToSensorType
=================================================
*/
    SerializableInputActionsGLFW::ESensorType  SerializableInputActionsGLFW::_InputTypeToSensorType (const EInputType inputType) __NE___
    {
        switch ( inputType )
        {
            #define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )  case EInputType::_type_ : return ESensorType::Android_ ## _type_;
            AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
            #undef AE_ANDROID_SERNSORS_VISITOR
        }
        return Default;
    }

/*
=================================================
    RequiredValueType
=================================================
*/
    SerializableInputActionsGLFW::EValueType  SerializableInputActionsGLFW::RequiredValueType (const InputType_t type) C_NE___
    {
        const auto  input_type = EInputType(type);
        switch ( input_type )
        {
            #define AE_ANDROID_SERNSORS_VISITOR( _type_, _bitIndex_, _api_, _valType_, ... )    case EInputType::_type_ : return EValueType::_valType_;
            AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
            #undef AE_ANDROID_SERNSORS_VISITOR

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
            EnumBinder<EInputType>  binder{ se };
            binder.Create();
            switch_enum( EInputType::Unknown )
            {
                case EInputType::Unknown :
                case EInputType::_Count :

                #define BIND( _name_ )                                              case EInputType::_name_ : binder.AddValue( #_name_, EInputType::_name_ );
                #define AE_GLFW_KEY_CODES_VISITOR( _key_, _code_, _name_, ... )     case EInputType::_key_  : binder.AddValue( _name_, EInputType::_key_ );
                AE_GLFW_KEY_CODES( AE_GLFW_KEY_CODES_VISITOR )
                #define AE_ANDROID_SERNSORS_VISITOR( _type_, ... )                  case EInputType::_type_ : binder.AddValue( #_type_, EInputType::_type_ );
                AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )

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

                #undef AE_GLFW_KEY_CODES_VISITOR
                #undef AE_ANDROID_SERNSORS_VISITOR
                #undef BIND
                default : break;
            }
            switch_end
            binder.AddValue( "MouseLeft",   EInputType::MouseLeft );
            binder.AddValue( "MouseRight",  EInputType::MouseRight );
            binder.AddValue( "MouseMiddle", EInputType::MouseMiddle );
        }

        // BindingsMode
        {
            ClassBinder<ScriptBindingsMode>     binder{ se };
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
    bool  SerializableInputActionsGLFW::LoadFromScript (const Scripting::ScriptEnginePtr &se, String script, ArrayView<Path> includeDirs,
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

        // enable sensors
        for (auto [name, mode] : _modeMap)
        {
            ASSERT( mode.enableSensors == Default );

            for (const auto& [key, info] : mode.actions)
            {
                auto code = EInputType(_Unpack( key ).Get<0>());

                ESensorType sensor = _InputTypeToSensorType( EInputType(code) );

                mode.enableSensors |= ESensorBits(1ull << uint(sensor));
            }
        }

        return true;
    }

#endif // AE_ENABLE_SCRIPTING

} // AE::App
