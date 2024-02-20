// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Private/SerializableInputActions.h"
#include "platform/Android/SerializableInputActionsAndroid.h"

namespace AE::App
{
    static constexpr uint   InputActionsGLFW_Name   = "ia.GLFW"_Hash;


    //
    // Serializable Input Actions for GLFW
    //

    class SerializableInputActionsGLFW final : public SerializableInputActions
    {
        friend class InputActionsGLFW;

    // types
    public:
        #define AE_GLFW_KEY_CODES( _visitor_ ) \
            _visitor_( Key_Space,           32,     "Space",        GLFW_KEY_SPACE          )\
            _visitor_( Key_Apostrophe,      39,     "Apostrophe",   GLFW_KEY_APOSTROPHE     )\
            _visitor_( Key_Comma,           44,     "Comma",        GLFW_KEY_COMMA          )\
            _visitor_( Key_Minus,           45,     "Minus",        GLFW_KEY_MINUS          )\
            _visitor_( Key_Period,          46,     "Period",       GLFW_KEY_PERIOD         )\
            _visitor_( Key_Slash,           47,     "Slash",        GLFW_KEY_SLASH          )\
            _visitor_( Key_0,               48,     "0",            GLFW_KEY_0              )\
            _visitor_( Key_1,               49,     "1",            GLFW_KEY_1              )\
            _visitor_( Key_2,               50,     "2",            GLFW_KEY_2              )\
            _visitor_( Key_3,               51,     "3",            GLFW_KEY_3              )\
            _visitor_( Key_4,               52,     "4",            GLFW_KEY_4              )\
            _visitor_( Key_5,               53,     "5",            GLFW_KEY_5              )\
            _visitor_( Key_6,               54,     "6",            GLFW_KEY_6              )\
            _visitor_( Key_7,               55,     "7",            GLFW_KEY_7              )\
            _visitor_( Key_8,               56,     "8",            GLFW_KEY_8              )\
            _visitor_( Key_9,               57,     "9",            GLFW_KEY_9              )\
            _visitor_( Key_Semicolon,       59,     "Semicolon",    GLFW_KEY_SEMICOLON      )\
            _visitor_( Key_Equal,           61,     "Equal",        GLFW_KEY_EQUAL          )\
            _visitor_( Key_A,               65,     "A",            GLFW_KEY_A              )\
            _visitor_( Key_B,               66,     "B",            GLFW_KEY_B              )\
            _visitor_( Key_C,               67,     "C",            GLFW_KEY_C              )\
            _visitor_( Key_D,               68,     "D",            GLFW_KEY_D              )\
            _visitor_( Key_E,               69,     "E",            GLFW_KEY_E              )\
            _visitor_( Key_F,               70,     "F",            GLFW_KEY_F              )\
            _visitor_( Key_G,               71,     "G",            GLFW_KEY_G              )\
            _visitor_( Key_H,               72,     "H",            GLFW_KEY_H              )\
            _visitor_( Key_I,               73,     "I",            GLFW_KEY_I              )\
            _visitor_( Key_J,               74,     "J",            GLFW_KEY_J              )\
            _visitor_( Key_K,               75,     "K",            GLFW_KEY_K              )\
            _visitor_( Key_L,               76,     "L",            GLFW_KEY_L              )\
            _visitor_( Key_M,               77,     "M",            GLFW_KEY_M              )\
            _visitor_( Key_N,               78,     "N",            GLFW_KEY_N              )\
            _visitor_( Key_O,               79,     "O",            GLFW_KEY_O              )\
            _visitor_( Key_P,               80,     "P",            GLFW_KEY_P              )\
            _visitor_( Key_Q,               81,     "Q",            GLFW_KEY_Q              )\
            _visitor_( Key_R,               82,     "R",            GLFW_KEY_R              )\
            _visitor_( Key_S,               83,     "S",            GLFW_KEY_S              )\
            _visitor_( Key_T,               84,     "T",            GLFW_KEY_T              )\
            _visitor_( Key_U,               85,     "U",            GLFW_KEY_U              )\
            _visitor_( Key_V,               86,     "V",            GLFW_KEY_V              )\
            _visitor_( Key_W,               87,     "W",            GLFW_KEY_W              )\
            _visitor_( Key_X,               88,     "X",            GLFW_KEY_X              )\
            _visitor_( Key_Y,               89,     "Y",            GLFW_KEY_Y              )\
            _visitor_( Key_Z,               90,     "Z",            GLFW_KEY_Z              )\
            _visitor_( Key_LeftBracket,     91,     "LeftBracket",  GLFW_KEY_LEFT_BRACKET   )\
            _visitor_( Key_BackSlash,       92,     "BackSlash",    GLFW_KEY_BACKSLASH      )\
            _visitor_( Key_RightBracket,    93,     "RightBracket", GLFW_KEY_RIGHT_BRACKET  )\
            _visitor_( Key_GraveAccent,     96,     "GraveAccent",  GLFW_KEY_GRAVE_ACCENT   )\
            _visitor_( Key_World1,          161,    "World1",       GLFW_KEY_WORLD_1        )\
            _visitor_( Key_World2,          162,    "World2",       GLFW_KEY_WORLD_2        )\
            _visitor_( Key_Escape,          256,    "Escape",       GLFW_KEY_ESCAPE         )\
            _visitor_( Key_Enter,           257,    "Enter",        GLFW_KEY_ENTER          )\
            _visitor_( Key_Tab,             258,    "Tab",          GLFW_KEY_TAB            )\
            _visitor_( Key_Backspace,       259,    "Backspace",    GLFW_KEY_BACKSPACE      )\
            _visitor_( Key_Insert,          260,    "Insert",       GLFW_KEY_INSERT         )\
            _visitor_( Key_Delete,          261,    "Delete",       GLFW_KEY_DELETE         )\
            _visitor_( Key_ArrowRight,      262,    "ArrowRight",   GLFW_KEY_RIGHT          )\
            _visitor_( Key_ArrowLeft,       263,    "ArrowLeft",    GLFW_KEY_LEFT           )\
            _visitor_( Key_ArrowDown,       264,    "ArrowDown",    GLFW_KEY_DOWN           )\
            _visitor_( Key_ArrowUp,         265,    "ArrowUp",      GLFW_KEY_UP             )\
            _visitor_( Key_PageUp,          266,    "PageUp",       GLFW_KEY_PAGE_UP        )\
            _visitor_( Key_PageDown,        267,    "PageDown",     GLFW_KEY_PAGE_DOWN      )\
            _visitor_( Key_Home,            268,    "Home",         GLFW_KEY_HOME           )\
            _visitor_( Key_End,             269,    "End",          GLFW_KEY_END            )\
            _visitor_( Key_CapsLock,        280,    "CapsLock",     GLFW_KEY_CAPS_LOCK      )\
            _visitor_( Key_ScrollLock,      281,    "ScrollLock",   GLFW_KEY_SCROLL_LOCK    )\
            _visitor_( Key_NumLock,         282,    "NumLock",      GLFW_KEY_NUM_LOCK       )\
            _visitor_( Key_PrintScreen,     283,    "PrintScreen",  GLFW_KEY_PRINT_SCREEN   )\
            _visitor_( Key_Pause,           284,    "Pause",        GLFW_KEY_PAUSE          )\
            _visitor_( Key_F1,              290,    "F1",           GLFW_KEY_F1             )\
            _visitor_( Key_F2,              291,    "F2",           GLFW_KEY_F2             )\
            _visitor_( Key_F3,              292,    "F3",           GLFW_KEY_F3             )\
            _visitor_( Key_F4,              293,    "F4",           GLFW_KEY_F4             )\
            _visitor_( Key_F5,              294,    "F5",           GLFW_KEY_F5             )\
            _visitor_( Key_F6,              295,    "F6",           GLFW_KEY_F6             )\
            _visitor_( Key_F7,              296,    "F7",           GLFW_KEY_F7             )\
            _visitor_( Key_F8,              297,    "F8",           GLFW_KEY_F8             )\
            _visitor_( Key_F9,              298,    "F9",           GLFW_KEY_F9             )\
            _visitor_( Key_F10,             299,    "F10",          GLFW_KEY_F10            )\
            _visitor_( Key_F11,             300,    "F11",          GLFW_KEY_F11            )\
            _visitor_( Key_F12,             301,    "F12",          GLFW_KEY_F12            )\
            _visitor_( Key_F13,             302,    "F13",          GLFW_KEY_F13            )\
            _visitor_( Key_F14,             303,    "F14",          GLFW_KEY_F14            )\
            _visitor_( Key_F15,             304,    "F15",          GLFW_KEY_F15            )\
            _visitor_( Key_F16,             305,    "F16",          GLFW_KEY_F16            )\
            _visitor_( Key_F17,             306,    "F17",          GLFW_KEY_F17            )\
            _visitor_( Key_F18,             307,    "F18",          GLFW_KEY_F18            )\
            _visitor_( Key_F19,             308,    "F19",          GLFW_KEY_F19            )\
            _visitor_( Key_F20,             309,    "F20",          GLFW_KEY_F20            )\
            _visitor_( Key_F21,             310,    "F21",          GLFW_KEY_F21            )\
            _visitor_( Key_F22,             311,    "F22",          GLFW_KEY_F22            )\
            _visitor_( Key_F23,             312,    "F23",          GLFW_KEY_F23            )\
            _visitor_( Key_F24,             313,    "F24",          GLFW_KEY_F24            )\
            _visitor_( Key_F25,             314,    "F25",          GLFW_KEY_F25            )\
            _visitor_( Key_LeftShift,       340,    "LeftShift",    GLFW_KEY_LEFT_SHIFT     )\
            _visitor_( Key_LeftControl,     341,    "LeftControl",  GLFW_KEY_LEFT_CONTROL   )\
            _visitor_( Key_LeftAlt,         342,    "LeftAlt",      GLFW_KEY_LEFT_ALT       )\
            _visitor_( Key_LeftSuper,       343,    "LeftSuper",    GLFW_KEY_LEFT_SUPER     )\
            _visitor_( Key_RightShift,      344,    "RightShift",   GLFW_KEY_RIGHT_SHIFT    )\
            _visitor_( Key_RightControl,    345,    "RightControl", GLFW_KEY_RIGHT_CONTROL  )\
            _visitor_( Key_RightAlt,        346,    "RightAlt",     GLFW_KEY_RIGHT_ALT      )\
            _visitor_( Key_RightSuper,      347,    "RightSuper",   GLFW_KEY_RIGHT_SUPER    )\
            _visitor_( Key_Menu,            348,    "Menu",         GLFW_KEY_MENU           )\
            /* keypad / numpad */\
            _visitor_( Key_KP_0,            320,    "KP_0",         GLFW_KEY_KP_0           )\
            _visitor_( Key_KP_1,            321,    "KP_1",         GLFW_KEY_KP_1           )\
            _visitor_( Key_KP_2,            322,    "KP_2",         GLFW_KEY_KP_2           )\
            _visitor_( Key_KP_3,            323,    "KP_3",         GLFW_KEY_KP_3           )\
            _visitor_( Key_KP_4,            324,    "KP_4",         GLFW_KEY_KP_4           )\
            _visitor_( Key_KP_5,            325,    "KP_5",         GLFW_KEY_KP_5           )\
            _visitor_( Key_KP_6,            326,    "KP_6",         GLFW_KEY_KP_6           )\
            _visitor_( Key_KP_7,            327,    "KP_7",         GLFW_KEY_KP_7           )\
            _visitor_( Key_KP_8,            328,    "KP_8",         GLFW_KEY_KP_8           )\
            _visitor_( Key_KP_9,            329,    "KP_9",         GLFW_KEY_KP_9           )\
            _visitor_( Key_KP_Decimal,      330,    "KP_Decimal",   GLFW_KEY_KP_DECIMAL     )\
            _visitor_( Key_KP_Divide,       331,    "KP_Divide",    GLFW_KEY_KP_DIVIDE      )\
            _visitor_( Key_KP_Multiply,     332,    "KP_Multiply",  GLFW_KEY_KP_MULTIPLY    )\
            _visitor_( Key_KP_Subtract,     333,    "KP_Subtract",  GLFW_KEY_KP_SUBTRACT    )\
            _visitor_( Key_KP_Add,          334,    "KP_Add",       GLFW_KEY_KP_ADD         )\
            _visitor_( Key_KP_Enter,        335,    "KP_Enter",     GLFW_KEY_KP_ENTER       )\
            _visitor_( Key_KP_Equal,        336,    "KP_Equal",     GLFW_KEY_KP_EQUAL       )\
            /* mouse */\
            _visitor_( MouseBtn1,           0,      "MouseBtn1",    GLFW_MOUSE_BUTTON_1     )\
            _visitor_( MouseBtn2,           1,      "MouseBtn2",    GLFW_MOUSE_BUTTON_2     )\
            _visitor_( MouseBtn3,           2,      "MouseBtn3",    GLFW_MOUSE_BUTTON_3     )\
            _visitor_( MouseBtn4,           3,      "MouseBtn4",    GLFW_MOUSE_BUTTON_4     )\
            _visitor_( MouseBtn5,           4,      "MouseBtn5",    GLFW_MOUSE_BUTTON_5     )\
            _visitor_( MouseBtn6,           5,      "MouseBtn6",    GLFW_MOUSE_BUTTON_6     )\
            _visitor_( MouseBtn7,           6,      "MouseBtn7",    GLFW_MOUSE_BUTTON_7     )\
            _visitor_( MouseBtn8,           7,      "MouseBtn8",    GLFW_MOUSE_BUTTON_8     )\

        enum class ESensorType : int
        {
            #define AE_ANDROID_SERNSORS_VISITOR( _type_, _bitIndex_, ... )  Android_ ## _type_ = _bitIndex_,
            AE_ANDROID_SERNSORS( AE_ANDROID_SERNSORS_VISITOR )
            #undef AE_ANDROID_SERNSORS_VISITOR

            Unknown             = 0xFF,
        };

        enum class EInputType : InputType_t
        {
            #define AE_GLFW_KEY_CODES_VISITOR( _key_, _code_, _name_, _glfw_code_ )     _key_ = _code_,
            AE_GLFW_KEY_CODES( AE_GLFW_KEY_CODES_VISITOR )
            #undef AE_GLFW_KEY_CODES_VISITOR

            MouseBegin          = MouseBtn1,        // GLFW_MOUSE_BUTTON_1
            MouseEnd            = MouseBtn8,        // GLFW_MOUSE_BUTTON_LAST

            MouseLeft           = MouseBtn1,
            MouseRight          = MouseBtn2,
            MouseMiddle         = MouseBtn3,

            KeyBegin            = 32,               // GLFW_KEY_SPACE
            KeyEnd              = 348,              // GLFW_KEY_LAST

            MultiTouch          = KeyEnd + 10,      // float2 (scale, rotate)
            Cursor2DBegin,
            MouseWheel          = Cursor2DBegin,    // float2 (delta)
            CursorPos,                              // float2 (absolute in pixels)
            CursorPos_mm,                           // float2 (absolute in mm)
            CursorDelta,                            // float2 (delta in pixels)
            CursorDelta_norm,                       // snorm2
            TouchPos,                               // float2 (absolute in pixels)
            TouchPos_mm,                            // float2 (absolute in mm)
            TouchDelta,                             // float2 (delta in pixels)
            TouchDelta_norm,                        // snorm2
            Cursor2DEnd         = TouchDelta_norm,

          // --- Android sensors ---
          // android device can be used as remote input device
            Sensors1fBegin      = Cursor2DEnd + 1,
            AirTemperature      = Sensors1fBegin,   // float  (Celsius)
            AmbientLight,                           // float  (lux)
            AirPressure,                            // float  (hPa or mbar)
            Proximity,                              // float  (cm) - on most devices it is 0 (too close) or >0 (too far, 3-5cm)
            RelativeHumidity,                       // float  (%)
            Sensors1fEnd        = RelativeHumidity,

            Sensors3fBegin      = Sensors1fEnd + 1,
            Accelerometer       = Sensors3fBegin,   // float3 (m/s2)
            Gravity,                                // float3 (m/s2)
            Gyroscope,                              // float3 (rad/s)
            LinearAcceleration,                     // float3 (m/s2)
            MagneticField,                          // float3 (micro tesla, uT)
            Sensors3fEnd        = MagneticField,

            Sensors4fBegin      = Sensors3fEnd + 1,
            RotationVector      = Sensors4fBegin,   // float4 (quaternion)
            GameRotationVector,                     // float4 (quaternion)
            Sensors4fEnd        = GameRotationVector,

            Sensors4x4fBegin    = Sensors4fEnd + 1,
            Pose6DOF            = Sensors4x4fBegin, // float4x4{ float4 quaternion;  float3 translation;  float4 delta_quaternion;  float3 delta_translation;  float seq; }
            Sensors4x4fEnd      = Pose6DOF,

            GeoLocation,                            // GNS
          // ---

            _Count,
            Unknown             = 0xFFFF,
        };

        struct ScriptBindingsMode;
        struct ScriptActionBindings;

    private:
        // for serialization
        static constexpr uint   _Version    = (1u << 16) | _BaseVersion;


    // methods
    public:
        SerializableInputActionsGLFW ()                                 __NE___ : SerializableInputActions{_Version} {}


    // SerializableInputActions //
        bool  IsKey (InputType_t type)                                  C_NE_OV { return _IsKey( EInputType(type) ); }
        bool  IsKeyOrTouch (InputType_t type)                           C_NE_OV { return _IsKeyOrTouch( EInputType(type) ); }
        bool  IsVec1D (InputType_t type)                                C_NE_OV { return _IsVec1D( EInputType(type) ); }
        bool  IsVec2D (InputType_t type)                                C_NE_OV { return _IsVec2D( EInputType(type) ); }
        bool  IsVec3D (InputType_t type)                                C_NE_OV { return _IsVec3D( EInputType(type) ); }

        EValueType  RequiredValueType (InputType_t inputType)           C_NE_OV;
        String      InputTypeToString (InputType_t)                     C_Th_OV;
        String      SensorBitsToString (ESensorBits)                    C_Th_OV;
        StringView  GetApiName ()                                       C_NE_OV { return "GLFW"; }

      #ifdef AE_ENABLE_SCRIPTING
        bool  LoadFromScript (const Scripting::ScriptEnginePtr &se, String script,
                              ArrayView<Path> includeDirs, const SourceLoc &loc,
                              INOUT Reflection &refl)                   __NE_OV;

        static void  Bind (const Scripting::ScriptEnginePtr &se)        __Th___;
      #endif

    private:
        ND_ static ESensorType  _InputTypeToSensorType (EInputType)     __NE___;

        ND_ static constexpr bool  _IsKey (EInputType type)             __NE___;
        ND_ static constexpr bool  _IsKeyOrTouch (EInputType type)      __NE___;
        ND_ static constexpr bool  _IsVec1D (EInputType type)           __NE___;
        ND_ static constexpr bool  _IsVec2D (EInputType type)           __NE___;
        ND_ static constexpr bool  _IsVec3D (EInputType type)           __NE___;
        ND_ static constexpr bool  _IsSensor1f (EInputType type)        __NE___;
        ND_ static constexpr bool  _IsSensor3f (EInputType type)        __NE___;
        ND_ static constexpr bool  _IsSensor4f (EInputType type)        __NE___;
    };


/*
=================================================
    _Is***
=================================================
*/
    inline constexpr bool  SerializableInputActionsGLFW::_IsKey (EInputType type) __NE___ {
        return  ((type >= EInputType::MouseBegin) and (type <= EInputType::MouseEnd)) or
                ((type >= EInputType::KeyBegin)   and (type <= EInputType::KeyEnd));
    }

    inline constexpr bool  SerializableInputActionsGLFW::_IsKeyOrTouch (EInputType type) __NE___ {
        return  _IsKey( type ) or (type == EInputType::TouchPos) or (type == EInputType::TouchPos_mm);
    }

    inline constexpr bool  SerializableInputActionsGLFW::_IsVec1D (EInputType type) __NE___ {
        return _IsSensor1f( type );
    }

    inline constexpr bool  SerializableInputActionsGLFW::_IsVec2D (EInputType type) __NE___ {
        return  ((type >= EInputType::Cursor2DBegin) and (type <= EInputType::Cursor2DEnd)) or
                (type == EInputType::MultiTouch);
    }

    inline constexpr bool  SerializableInputActionsGLFW::_IsVec3D (EInputType type) __NE___ {
        return  _IsSensor3f( type );
    }

    inline constexpr bool  SerializableInputActionsGLFW::_IsSensor1f (EInputType type) __NE___ {
        return (type >= EInputType::Sensors1fBegin) and (type <= EInputType::Sensors1fEnd);
    }

    inline constexpr bool  SerializableInputActionsGLFW::_IsSensor3f (EInputType type) __NE___ {
        return (type >= EInputType::Sensors3fBegin) and (type <= EInputType::Sensors3fEnd);
    }

    inline constexpr bool  SerializableInputActionsGLFW::_IsSensor4f (EInputType type) __NE___ {
        return (type >= EInputType::Sensors4fBegin) and (type <= EInputType::Sensors4fEnd);
    }

} // AE::App


#ifdef AE_ENABLE_SCRIPTING
    AE_DECL_SCRIPT_OBJ_RC(  AE::App::SerializableInputActionsGLFW::ScriptBindingsMode,      "GLFW_BindingsMode"     );
    AE_DECL_SCRIPT_OBJ(     AE::App::SerializableInputActionsGLFW::ScriptActionBindings,    "GLFW_ActionBindings"   );
    AE_DECL_SCRIPT_TYPE(    AE::App::SerializableInputActionsGLFW::EInputType,              "GLFW_Input"            );
#endif
