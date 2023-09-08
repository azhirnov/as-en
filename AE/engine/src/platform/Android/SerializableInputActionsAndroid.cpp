// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_ANDROID
# include <android/keycodes.h>
#endif
#include "pch/Scripting.h"

#include "platform/Private/EnumToString.h"
#include "platform/Android/SerializableInputActionsAndroid.h"

namespace AE::App
{
# ifdef AE_PLATFORM_ANDROID

    #define AE_ANDROID_KEY_CODES_VISITOR( _key_, _code_, _name_, _and_code_ ) \
        STATIC_ASSERT( uint(SerializableInputActionsAndroid::EInputType::_key_) == uint(_and_code_) );

    AE_ANDROID_KEY_CODES( AE_ANDROID_KEY_CODES_VISITOR )
    #undef AE_ANDROID_KEY_CODES_VISITOR

# endif

/*
=================================================
    EInputTypeToString
=================================================
*/
namespace {
    ND_ String  EInputTypeToString (SerializableInputActionsAndroid::EInputType value)
    {
        using EInputType = SerializableInputActionsAndroid::EInputType;

        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            #define AE_ANDROID_KEY_CODES_VISITOR( _key_, _code_, _name_, _and_code_ )   case EInputType::_key_ :    return _name_;
            AE_ANDROID_KEY_CODES( AE_ANDROID_KEY_CODES_VISITOR )
            #undef AE_ANDROID_KEY_CODES_VISITOR

            case EInputType::MultiTouch :       return "MultiTouch";

            // Cursor2DBegin
            case EInputType::TouchPos :         return "TouchPos";
            case EInputType::TouchPos_mm :      return "TouchPos_mm";
            case EInputType::TouchDelta :       return "TouchDelta";
            case EInputType::TouchDelta_norm :  return "TouchDelta_norm";
            // Cursor2DEnd

            // Sensors1fBegin
            case EInputType::AirTemperature :   return "AirTemperature";
            case EInputType::AmbientLight :     return "AmbientLight";
            case EInputType::AirPressure :      return "AirPressure";
            case EInputType::Proximity :        return "Proximity";
            case EInputType::RelativeHumidity : return "RelativeHumidity";
            case EInputType::StepCount :        return "StepCount";
            case EInputType::BatteryState :     return "BatteryState";
            // Sensors1fEnd

            // Sensors2dBegin
            case EInputType::GeoLocation :      return "GeoLocation";
            // Sensors2dEnd

            // Sensors3fBegin
            case EInputType::Accelerometer :    return "Accelerometer";
            case EInputType::Gravity :          return "Gravity";
            case EInputType::Gyroscope :        return "Gyroscope";
            case EInputType::LinearAcceleration:return "LinearAcceleration";
            case EInputType::MagneticField :    return "MagneticField";
            case EInputType::RotationVector :   return "RotationVector";
            // Sensors3fEnd

            // Sensors4x4fBegin
            case EInputType::Pose6DOF :         return "Pose6DOF";
            // Sensors4x4fEnd

            case EInputType::KeyBegin :
            case EInputType::KeyEnd :

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
    String  SerializableInputActionsAndroid::ToString (const Reflection &refl) C_Th___
    {
        String      str      = "InputActionsAndroid {\n";
        const auto  mode_arr = _ToArray(_modeMap);

        for (auto& [name, mode] : mode_arr)
        {
            str << "  '" << refl.Get( name ) << "' {\n";

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
    struct SerializableInputActionsAndroid::ScriptBindingsMode final : ScriptBindingsModeBase
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
    struct SerializableInputActionsAndroid::ScriptActionBindings final : ScriptActionBindingsBase
    {
    public:
        ScriptActionBindings () {}
        ScriptActionBindings (SerializableInputActionsAndroid& self, Reflection &refl) :
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
    void  SerializableInputActionsAndroid::Bind (const ScriptEnginePtr &se) __Th___
    {
        // Input
        {
            EnumBinder<EInputType>  binder{ se };
            binder.Create();

            #define AE_ANDROID_KEY_CODES_VISITOR( _key_, _code_, _name_, _and_code_ )       binder.AddValue( _name_, EInputType::_key_ );
            AE_ANDROID_KEY_CODES( AE_ANDROID_KEY_CODES_VISITOR )
            #undef AE_ANDROID_KEY_CODES_VISITOR

            binder.AddValue( "MultiTouch",          EInputType::MultiTouch );

            STATIC_ASSERT( uint(EInputType::Cursor2DBegin) == 523 );
            STATIC_ASSERT( uint(EInputType::Cursor2DEnd)   == 526 );
            binder.AddValue( "TouchPos",            EInputType::TouchPos );
            binder.AddValue( "TouchPos_mm",         EInputType::TouchPos_mm );
            binder.AddValue( "TouchDelta",          EInputType::TouchDelta );
            binder.AddValue( "TouchDelta_norm",     EInputType::TouchDelta_norm );

            STATIC_ASSERT( uint(EInputType::Sensors1fBegin) == 527 );
            STATIC_ASSERT( uint(EInputType::Sensors1fEnd)   == 533 );
            binder.AddValue( "AirTemperature",      EInputType::AirTemperature );
            binder.AddValue( "AmbientLight",        EInputType::AmbientLight );
            binder.AddValue( "AirPressure",         EInputType::AirPressure );
            binder.AddValue( "Proximity",           EInputType::Proximity );
            binder.AddValue( "RelativeHumidity",    EInputType::RelativeHumidity );
            binder.AddValue( "StepCount",           EInputType::StepCount );
            binder.AddValue( "BatteryState",        EInputType::BatteryState );

            STATIC_ASSERT( uint(EInputType::Sensors2dBegin) == 534 );
            STATIC_ASSERT( uint(EInputType::Sensors2dEnd)   == 534 );
            binder.AddValue( "GeoLocation",         EInputType::GeoLocation );

            STATIC_ASSERT( uint(EInputType::Sensors3fBegin) == 535 );
            STATIC_ASSERT( uint(EInputType::Sensors3fEnd)   == 540 );
            binder.AddValue( "Accelerometer",       EInputType::Accelerometer );
            binder.AddValue( "Gravity",             EInputType::Gravity );
            binder.AddValue( "Gyroscope",           EInputType::Gyroscope );
            binder.AddValue( "LinearAcceleration",  EInputType::LinearAcceleration );
            binder.AddValue( "MagneticField",       EInputType::MagneticField );
            binder.AddValue( "RotationVector",      EInputType::RotationVector );

            STATIC_ASSERT( uint(EInputType::Sensors4x4fBegin) == 541 );
            STATIC_ASSERT( uint(EInputType::Sensors4x4fEnd)   == 541 );
            binder.AddValue( "Pose6DOF",            EInputType::Pose6DOF );
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
    bool  SerializableInputActionsAndroid::LoadFromScript (const Scripting::ScriptEnginePtr &se, String script, const SourceLoc &loc, Reflection &refl)
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
