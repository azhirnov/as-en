// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Private/SerializableInputActions.h"

namespace AE::App
{
    static constexpr uint   InputActionsAndroid_Name    = uint("ia.And"_StringToID);


    //
    // Serializable Input Actions for Android
    //

    class SerializableInputActionsAndroid final : public SerializableInputActions
    {
        friend class InputActionsAndroid;

    // types
    public:
        #define AE_ANDROID_KEY_CODES( _visitor_ ) \
            _visitor_( Key_Back,            4,      "Back",             AKEYCODE_BACK               )   /* BrowserBack ? */\
            _visitor_( Key_0,               7,      "0",                AKEYCODE_0                  )\
            _visitor_( Key_1,               8,      "1",                AKEYCODE_1                  )\
            _visitor_( Key_2,               9,      "2",                AKEYCODE_2                  )\
            _visitor_( Key_3,               10,     "3",                AKEYCODE_3                  )\
            _visitor_( Key_4,               11,     "4",                AKEYCODE_4                  )\
            _visitor_( Key_5,               12,     "5",                AKEYCODE_5                  )\
            _visitor_( Key_6,               13,     "6",                AKEYCODE_6                  )\
            _visitor_( Key_7,               14,     "7",                AKEYCODE_7                  )\
            _visitor_( Key_8,               15,     "8",                AKEYCODE_8                  )\
            _visitor_( Key_9,               16,     "9",                AKEYCODE_9                  )\
            _visitor_( Key_Star,            17,     "Star",             AKEYCODE_STAR               )   /* '*' */\
            _visitor_( Key_Pound,           18,     "Paund",            AKEYCODE_POUND              )   /* '#' */\
            _visitor_( Key_VolumeUp,        24,     "VolumeUp",         AKEYCODE_VOLUME_UP          )\
            _visitor_( Key_VolumeDown,      25,     "VolumeDown",       AKEYCODE_VOLUME_DOWN        )\
            _visitor_( Key_VolumeMute,      164,    "VolumeMute",       AKEYCODE_VOLUME_MUTE        )\
            _visitor_( Key_Power,           26,     "Power",            AKEYCODE_POWER              )\
            _visitor_( Key_Camera,          27,     "Camera",           AKEYCODE_CAMERA             )\
            _visitor_( Key_Clear,           28,     "Clear",            AKEYCODE_CLEAR              )\
            _visitor_( Key_A,               29,     "A",                AKEYCODE_A                  )\
            _visitor_( Key_B,               30,     "B",                AKEYCODE_B                  )\
            _visitor_( Key_C,               31,     "C",                AKEYCODE_C                  )\
            _visitor_( Key_D,               32,     "D",                AKEYCODE_D                  )\
            _visitor_( Key_E,               33,     "E",                AKEYCODE_E                  )\
            _visitor_( Key_F,               34,     "F",                AKEYCODE_F                  )\
            _visitor_( Key_G,               35,     "G",                AKEYCODE_G                  )\
            _visitor_( Key_H,               36,     "H",                AKEYCODE_H                  )\
            _visitor_( Key_I,               37,     "I",                AKEYCODE_I                  )\
            _visitor_( Key_J,               38,     "J",                AKEYCODE_J                  )\
            _visitor_( Key_K,               39,     "K",                AKEYCODE_K                  )\
            _visitor_( Key_L,               40,     "L",                AKEYCODE_L                  )\
            _visitor_( Key_M,               41,     "M",                AKEYCODE_M                  )\
            _visitor_( Key_N,               42,     "N",                AKEYCODE_N                  )\
            _visitor_( Key_O,               43,     "O",                AKEYCODE_O                  )\
            _visitor_( Key_P,               44,     "P",                AKEYCODE_P                  )\
            _visitor_( Key_Q,               45,     "Q",                AKEYCODE_Q                  )\
            _visitor_( Key_R,               46,     "R",                AKEYCODE_R                  )\
            _visitor_( Key_S,               47,     "S",                AKEYCODE_S                  )\
            _visitor_( Key_T,               48,     "T",                AKEYCODE_T                  )\
            _visitor_( Key_U,               49,     "U",                AKEYCODE_U                  )\
            _visitor_( Key_V,               50,     "V",                AKEYCODE_V                  )\
            _visitor_( Key_W,               51,     "W",                AKEYCODE_W                  )\
            _visitor_( Key_X,               52,     "X",                AKEYCODE_X                  )\
            _visitor_( Key_Y,               53,     "Y",                AKEYCODE_Y                  )\
            _visitor_( Key_Z,               54,     "Z",                AKEYCODE_Z                  )\
            _visitor_( Key_Comma,           55,     "Comma",            AKEYCODE_COMMA              )   /* ',' */\
            _visitor_( Key_Period,          56,     "Period",           AKEYCODE_PERIOD             )   /* '.' */\
            _visitor_( Key_LeftAlt,         57,     "LeftAlt",          AKEYCODE_ALT_LEFT           )\
            _visitor_( Key_RightAlt,        58,     "RightAlt",         AKEYCODE_ALT_RIGHT          )\
            _visitor_( Key_LeftShift,       59,     "LeftShift",        AKEYCODE_SHIFT_LEFT         )\
            _visitor_( Key_RightShift,      60,     "RightShift",       AKEYCODE_SHIFT_RIGHT        )\
            _visitor_( Key_Tab,             61,     "Tab",              AKEYCODE_TAB                )\
            _visitor_( Key_Space,           62,     "Space",            AKEYCODE_SPACE              )\
            _visitor_( Key_Sym,             63,     "Sym",              AKEYCODE_SYM                )   /* Symbol modifier key */\
            _visitor_( Key_Browser,         64,     "Browser",          AKEYCODE_EXPLORER           )\
            _visitor_( Key_LaunchMail,      65,     "LaunchMail",       AKEYCODE_ENVELOPE           )\
            _visitor_( Key_Enter,           66,     "Enter",            AKEYCODE_ENTER              )\
            _visitor_( Key_Delete,          67,     "Delete",           AKEYCODE_DEL                )\
            _visitor_( Key_Grave,           68,     "Grave",            AKEYCODE_GRAVE              )   /* '`' */\
            _visitor_( Key_Minus,           69,     "Minus",            AKEYCODE_MINUS              )   /* '-' */\
            _visitor_( Key_Equals,          70,     "Equals",           AKEYCODE_EQUALS             )   /* '=' */\
            _visitor_( Key_LeftBracket,     71,     "LeftBracket",      AKEYCODE_LEFT_BRACKET       )   /* '[' */\
            _visitor_( Key_RightBracket,    72,     "RightBracket",     AKEYCODE_RIGHT_BRACKET      )   /* ']' */\
            _visitor_( Key_BackSlash,       73,     "BackSlash",        AKEYCODE_BACKSLASH          )   /* '\' */\
            _visitor_( Key_Semicolon,       74,     "Semicolon",        AKEYCODE_SEMICOLON          )   /* ';' */\
            _visitor_( Key_Apostrophe,      75,     "Apostrophe",       AKEYCODE_APOSTROPHE         )   /* ''' */\
            _visitor_( Key_Slash,           76,     "Slash",            AKEYCODE_SLASH              )   /* '/' */\
            _visitor_( Key_At,              77,     "At",               AKEYCODE_AT                 )   /* '@' */\
            _visitor_( Key_Num,             78,     "Num",              AKEYCODE_NUM                )\
            _visitor_( Key_Plus,            81,     "Plus",             AKEYCODE_PLUS               )\
            _visitor_( Key_Menu,            82,     "Menu",             AKEYCODE_MENU               )\
            _visitor_( Key_Notification,    83,     "Notification",     AKEYCODE_NOTIFICATION       )\
            _visitor_( Key_Search,          84,     "Search",           AKEYCODE_SEARCH             )\
            _visitor_( Key_MicMute,         91,     "MicMute",          AKEYCODE_MUTE               )\
            _visitor_( Key_PageUp,          92,     "PageUp",           AKEYCODE_PAGE_UP            )\
            _visitor_( Key_PageDown,        93,     "PageDown",         AKEYCODE_PAGE_DOWN          )\
            _visitor_( Key_PictSymbols,     94,     "PictSymbols",      AKEYCODE_PICTSYMBOLS        )\
            _visitor_( Key_SwitchCharset,   95,     "SwitchCharset",    AKEYCODE_SWITCH_CHARSET     )\
            _visitor_( Key_Escape,          111,    "Escape",           AKEYCODE_ESCAPE             )\
            _visitor_( Key_ForwardDelete,   112,    "ForwardDelete",    AKEYCODE_FORWARD_DEL        )\
            _visitor_( Key_LeftControl,     113,    "LeftControl",      AKEYCODE_CTRL_LEFT          )\
            _visitor_( Key_RightControl,    114,    "RightControl",     AKEYCODE_CTRL_RIGHT         )\
            _visitor_( Key_CapsLock,        115,    "CapsLock",         AKEYCODE_CAPS_LOCK          )\
            _visitor_( Key_ScrollLock,      116,    "ScrollLock",       AKEYCODE_SCROLL_LOCK        )\
            _visitor_( Key_LeftMeta,        117,    "LeftMeta",         AKEYCODE_META_LEFT          )\
            _visitor_( Key_RightMeta,       118,    "RightMeta",        AKEYCODE_META_RIGHT         )\
            _visitor_( Key_Function,        119,    "Function",         AKEYCODE_FUNCTION           )\
            _visitor_( Key_SysRq,           120,    "SysRq",            AKEYCODE_SYSRQ              )\
            _visitor_( Key_PauseBreak,      121,    "PauseBreak",       AKEYCODE_BREAK              )\
            _visitor_( Key_MoveHome,        122,    "MoveHome",         AKEYCODE_MOVE_HOME          )\
            _visitor_( Key_MoveEnd,         123,    "MoveEnd",          AKEYCODE_MOVE_END           )\
            _visitor_( Key_Insert,          124,    "Insert",           AKEYCODE_INSERT             )\
            _visitor_( Key_Forward,         125,    "Forward",          AKEYCODE_FORWARD            )   /* BrowserForward ? */\
            _visitor_( Key_F1,              131,    "F1",               AKEYCODE_F1                 )\
            _visitor_( Key_F2,              132,    "F2",               AKEYCODE_F2                 )\
            _visitor_( Key_F3,              133,    "F3",               AKEYCODE_F3                 )\
            _visitor_( Key_F4,              134,    "F4",               AKEYCODE_F4                 )\
            _visitor_( Key_F5,              135,    "F5",               AKEYCODE_F5                 )\
            _visitor_( Key_F6,              136,    "F6",               AKEYCODE_F6                 )\
            _visitor_( Key_F7,              137,    "F7",               AKEYCODE_F7                 )\
            _visitor_( Key_F8,              138,    "F8",               AKEYCODE_F8                 )\
            _visitor_( Key_F9,              139,    "F9",               AKEYCODE_F9                 )\
            _visitor_( Key_F10,             140,    "F10",              AKEYCODE_F10                )\
            _visitor_( Key_F11,             141,    "F11",              AKEYCODE_F11                )\
            _visitor_( Key_F12,             142,    "F12",              AKEYCODE_F12                )\
            _visitor_( Key_NumLock,         143,    "NumLock",          AKEYCODE_NUM_LOCK           )\
            _visitor_( Key_Info,            165,    "Info",             AKEYCODE_INFO               )\
            _visitor_( Key_ChannelUp,       166,    "ChannelUp",        AKEYCODE_CHANNEL_UP         )\
            _visitor_( Key_ChannelDown,     167,    "ChannelDown",      AKEYCODE_CHANNEL_DOWN       )\
            _visitor_( Key_ZoomIn,          168,    "ZoomIn",           AKEYCODE_ZOOM_IN            )\
            _visitor_( Key_ZoomOut,         169,    "ZoomOut",          AKEYCODE_ZOOM_OUT           )\
            _visitor_( Key_Window,          171,    "Window",           AKEYCODE_WINDOW             )   /* picture in picture */\
            _visitor_( Key_Guide,           172,    "Guide",            AKEYCODE_GUIDE              )\
            _visitor_( Key_DVR,             173,    "DVR",              AKEYCODE_DVR                )\
            _visitor_( Key_Bookmark,        174,    "Bookmark",         AKEYCODE_BOOKMARK           )\
            _visitor_( Key_Captions,        175,    "Captions",         AKEYCODE_CAPTIONS           )\
            _visitor_( Key_Settings,        176,    "Settings",         AKEYCODE_SETTINGS           )\
            _visitor_( Key_AppSwitch,       187,    "AppSwitch",        AKEYCODE_APP_SWITCH         )\
            _visitor_( Key_LangSwitch,      204,    "LangSwitch",       AKEYCODE_LANGUAGE_SWITCH    )\
            _visitor_( Key_MannerMode,      205,    "MannerMode",       AKEYCODE_MANNER_MODE        )\
            _visitor_( Key_3DMode,          206,    "3DMode",           AKEYCODE_3D_MODE            )\
            _visitor_( Key_Contacts,        207,    "Contacts",         AKEYCODE_CONTACTS           )\
            _visitor_( Key_Calendar,        208,    "Calendar",         AKEYCODE_CALENDAR           )\
            _visitor_( Key_Music,           209,    "Music",            AKEYCODE_MUSIC              )\
            _visitor_( Key_Calculator,      210,    "Calculator",       AKEYCODE_CALCULATOR         )\
            _visitor_( Key_ZenkakuHankaku,  211,    "ZenkakuHankaku",   AKEYCODE_ZENKAKU_HANKAKU    )\
            _visitor_( Key_Eisu,            212,    "Eisu",             AKEYCODE_EISU               )\
            _visitor_( Key_Muhenkan,        213,    "Muhenkan",         AKEYCODE_MUHENKAN           )\
            _visitor_( Key_Henkan,          214,    "Henkan",           AKEYCODE_HENKAN             )\
            _visitor_( Key_KatakanaHiragana,215,    "KatakanaHiragana", AKEYCODE_KATAKANA_HIRAGANA  )\
            _visitor_( Key_Yen,             216,    "Yen",              AKEYCODE_YEN                )\
            _visitor_( Key_Ro,              217,    "Ro",               AKEYCODE_RO                 )\
            _visitor_( Key_Kana,            218,    "Kana",             AKEYCODE_KANA               )\
            _visitor_( Key_Assist,          219,    "Assist",           AKEYCODE_ASSIST             )\
            _visitor_( Key_BrightnessDown,  220,    "BrightnessDown",   AKEYCODE_BRIGHTNESS_DOWN    )\
            _visitor_( Key_BrightnessUp,    221,    "BrightnessUp",     AKEYCODE_BRIGHTNESS_UP      )\
            _visitor_( Key_Sleep,           223,    "Sleep",            AKEYCODE_SLEEP              )\
            _visitor_( Key_Wakeup,          224,    "Wakeup",           AKEYCODE_WAKEUP             )\
            _visitor_( Key_Pairing,         225,    "Pairing",          AKEYCODE_PAIRING            )\
            _visitor_( Key_11,              227,    "11",               AKEYCODE_11                 )\
            _visitor_( Key_12,              228,    "12",               AKEYCODE_12                 )\
            _visitor_( Key_LastChannel,     229,    "LastChannel",      AKEYCODE_LAST_CHANNEL       )\
            _visitor_( Key_VoiceAssist,     231,    "VoiceAssist",      AKEYCODE_VOICE_ASSIST       )\
            _visitor_( Key_Help,            259,    "Help",             AKEYCODE_HELP               )\
            _visitor_( Key_NavPrev,         260,    "NavPrev",          AKEYCODE_NAVIGATE_PREVIOUS  )\
            _visitor_( Key_NavNext,         261,    "NavNext",          AKEYCODE_NAVIGATE_NEXT      )\
            _visitor_( Key_NavIn,           262,    "NavIn",            AKEYCODE_NAVIGATE_IN        )\
            _visitor_( Key_NavOut,          263,    "NavOut",           AKEYCODE_NAVIGATE_OUT       )\
            _visitor_( Key_StemPrimary,     264,    "StemPrimary",      AKEYCODE_STEM_PRIMARY       )\
            _visitor_( Key_Stem1,           265,    "Stem1",            AKEYCODE_STEM_1             )\
            _visitor_( Key_Stem2,           266,    "Stem2",            AKEYCODE_STEM_2             )\
            _visitor_( Key_Stem3,           267,    "Stem3",            AKEYCODE_STEM_3             )\
            _visitor_( Key_SoftSleep,       276,    "SoftSleep",        AKEYCODE_SOFT_SLEEP         )\
            _visitor_( Key_Cut,             277,    "Cut",              AKEYCODE_CUT                )\
            _visitor_( Key_Copy,            278,    "Copy",             AKEYCODE_COPY               )\
            _visitor_( Key_Paste,           279,    "Paste",            AKEYCODE_PASTE              )\
            _visitor_( Key_SysNavUp,        280,    "SysNavUp",         AKEYCODE_SYSTEM_NAVIGATION_UP   )\
            _visitor_( Key_SysNavDown,      281,    "SysNavDown",       AKEYCODE_SYSTEM_NAVIGATION_DOWN )\
            _visitor_( Key_SysNavLeft,      282,    "SysNavLeft",       AKEYCODE_SYSTEM_NAVIGATION_LEFT )\
            _visitor_( Key_SysNavRight,     283,    "SysNavRight",      AKEYCODE_SYSTEM_NAVIGATION_RIGHT)\
            _visitor_( Key_AllApps,         284,    "AllApps",          AKEYCODE_ALL_APPS           )\
            _visitor_( Key_Refresh,         285,    "Refresh",          AKEYCODE_REFRESH            )\
            _visitor_( Key_ThumbsUp,        286,    "ThumbsUp",         AKEYCODE_THUMBS_UP          )\
            _visitor_( Key_ThumbsDown,      287,    "ThumbsDown",       AKEYCODE_THUMBS_DOWN        )\
            _visitor_( Key_ProfileSwitch,   288,    "ProfileSwitch",    AKEYCODE_PROFILE_SWITCH     )\
            /* media */\
            _visitor_( Key_MediaPlayPause,  85,     "MediaPlayPause",   AKEYCODE_MEDIA_PLAY_PAUSE   )\
            _visitor_( Key_MediaStop,       86,     "MediaStop",        AKEYCODE_MEDIA_STOP         )\
            _visitor_( Key_MediaNext,       87,     "MediaNext",        AKEYCODE_MEDIA_NEXT         )\
            _visitor_( Key_MediaPrev,       88,     "MediaPrev",        AKEYCODE_MEDIA_PREVIOUS     )\
            _visitor_( Key_MediaRewind,     89,     "MediaRewind",      AKEYCODE_MEDIA_REWIND       )\
            _visitor_( Key_MediaFastForward, 90,    "MediaFastForward", AKEYCODE_MEDIA_FAST_FORWARD )\
            _visitor_( Key_MediaPlay,       126,    "MediaPlay",        AKEYCODE_MEDIA_PLAY         )\
            _visitor_( Key_MediaPause,      127,    "MediaPause",       AKEYCODE_MEDIA_PAUSE        )\
            _visitor_( Key_MediaClose,      128,    "MediaClose",       AKEYCODE_MEDIA_CLOSE        )\
            _visitor_( Key_MediaEject,      129,    "MediaEject",       AKEYCODE_MEDIA_EJECT        )\
            _visitor_( Key_MediaRecord,     130,    "MediaRecord",      AKEYCODE_MEDIA_RECORD       )\
            _visitor_( Key_MediaAudioTrack, 222,    "MediaAudioTrack",  AKEYCODE_MEDIA_AUDIO_TRACK  )\
            _visitor_( Key_MediaTopMenu,    226,    "MediaTopMenu",     AKEYCODE_MEDIA_TOP_MENU     )\
            _visitor_( Key_MediaSkipForward, 272,   "MediaSkipForward", AKEYCODE_MEDIA_SKIP_FORWARD )\
            _visitor_( Key_MediaSkipBackward,273,   "MediaSkipBackward",AKEYCODE_MEDIA_SKIP_BACKWARD)\
            _visitor_( Key_MediaStepForward, 274,   "MediaStepForward", AKEYCODE_MEDIA_STEP_FORWARD )\
            _visitor_( Key_MediaStepBackward,275,   "MediaStepBackward",AKEYCODE_MEDIA_STEP_BACKWARD)\
            /* dpad */\
            _visitor_( Key_DPadUp,          19,     "DPadUp",           AKEYCODE_DPAD_UP            )\
            _visitor_( Key_DPadDown,        20,     "DPadDown",         AKEYCODE_DPAD_DOWN          )\
            _visitor_( Key_DPadLeft,        21,     "DPadLeft",         AKEYCODE_DPAD_LEFT          )\
            _visitor_( Key_DPadRight,       22,     "DPadRight",        AKEYCODE_DPAD_RIGHT         )\
            _visitor_( Key_DPadCenter,      23,     "DPadCenter",       AKEYCODE_DPAD_CENTER        )\
            _visitor_( Key_DPadUpLeft,      268,    "DPadUpLeft",       AKEYCODE_DPAD_UP_LEFT       )\
            _visitor_( Key_DPadDownLeft,    269,    "DPadDownLeft",     AKEYCODE_DPAD_DOWN_LEFT     )\
            _visitor_( Key_DPadUpRight,     270,    "DPadUpRight",      AKEYCODE_DPAD_UP_RIGHT      )\
            _visitor_( Key_DPadDownRight,   271,    "DPadDownRight",    AKEYCODE_DPAD_DOWN_RIGHT    )\
            /* gamepad */\
            _visitor_( Key_GPadA,           96,     "GPadA",            AKEYCODE_BUTTON_A           )\
            _visitor_( Key_GPadB,           97,     "GPadB",            AKEYCODE_BUTTON_B           )\
            _visitor_( Key_GPadC,           98,     "GPadC",            AKEYCODE_BUTTON_C           )\
            _visitor_( Key_GPadX,           99,     "GPadX",            AKEYCODE_BUTTON_X           )\
            _visitor_( Key_GPadY,           100,    "GPadY",            AKEYCODE_BUTTON_Y           )\
            _visitor_( Key_GPadZ,           101,    "GPadZ",            AKEYCODE_BUTTON_Z           )\
            _visitor_( Key_GPadL1,          102,    "GPadL1",           AKEYCODE_BUTTON_L1          )\
            _visitor_( Key_GPadR1,          103,    "GPadR1",           AKEYCODE_BUTTON_R1          )\
            _visitor_( Key_GPadL2,          104,    "GPadL2",           AKEYCODE_BUTTON_L2          )\
            _visitor_( Key_GPadR2,          105,    "GPadR2",           AKEYCODE_BUTTON_R2          )\
            _visitor_( Key_GPadThumbL,      106,    "GPadThumbL",       AKEYCODE_BUTTON_THUMBL      )\
            _visitor_( Key_GPadThumbR,      107,    "GPadThumbR",       AKEYCODE_BUTTON_THUMBR      )\
            _visitor_( Key_GPadStart,       108,    "GPadStart",        AKEYCODE_BUTTON_START       )\
            _visitor_( Key_GPadSelect,      109,    "GPadSelect",       AKEYCODE_BUTTON_SELECT      )\
            _visitor_( Key_GPadMode,        110,    "GPadMode",         AKEYCODE_BUTTON_MODE        )\
            _visitor_( Key_GPad1,           188,    "GPad1",            AKEYCODE_BUTTON_1           )\
            _visitor_( Key_GPad2,           189,    "GPad2",            AKEYCODE_BUTTON_2           )\
            _visitor_( Key_GPad3,           190,    "GPad3",            AKEYCODE_BUTTON_3           )\
            _visitor_( Key_GPad4,           191,    "GPad4",            AKEYCODE_BUTTON_4           )\
            _visitor_( Key_GPad5,           192,    "GPad5",            AKEYCODE_BUTTON_5           )\
            _visitor_( Key_GPad6,           193,    "GPad6",            AKEYCODE_BUTTON_6           )\
            _visitor_( Key_GPad7,           194,    "GPad7",            AKEYCODE_BUTTON_7           )\
            _visitor_( Key_GPad8,           195,    "GPad8",            AKEYCODE_BUTTON_8           )\
            _visitor_( Key_GPad9,           196,    "GPad9",            AKEYCODE_BUTTON_9           )\
            _visitor_( Key_GPad10,          197,    "GPad10",           AKEYCODE_BUTTON_10          )\
            _visitor_( Key_GPad11,          198,    "GPad11",           AKEYCODE_BUTTON_11          )\
            _visitor_( Key_GPad12,          199,    "GPad12",           AKEYCODE_BUTTON_12          )\
            _visitor_( Key_GPad13,          200,    "GPad13",           AKEYCODE_BUTTON_13          )\
            _visitor_( Key_GPad14,          201,    "GPad14",           AKEYCODE_BUTTON_14          )\
            _visitor_( Key_GPad15,          202,    "GPad15",           AKEYCODE_BUTTON_15          )\
            _visitor_( Key_GPad16,          203,    "GPad16",           AKEYCODE_BUTTON_16          )\
            /* keypad / numpad */\
            _visitor_( Key_KP_0,            144,    "KP_0",             AKEYCODE_NUMPAD_0           )\
            _visitor_( Key_KP_1,            145,    "KP_1",             AKEYCODE_NUMPAD_1           )\
            _visitor_( Key_KP_2,            146,    "KP_2",             AKEYCODE_NUMPAD_2           )\
            _visitor_( Key_KP_3,            147,    "KP_3",             AKEYCODE_NUMPAD_3           )\
            _visitor_( Key_KP_4,            148,    "KP_4",             AKEYCODE_NUMPAD_4           )\
            _visitor_( Key_KP_5,            149,    "KP_5",             AKEYCODE_NUMPAD_5           )\
            _visitor_( Key_KP_6,            150,    "KP_6",             AKEYCODE_NUMPAD_6           )\
            _visitor_( Key_KP_7,            151,    "KP_7",             AKEYCODE_NUMPAD_7           )\
            _visitor_( Key_KP_8,            152,    "KP_8",             AKEYCODE_NUMPAD_8           )\
            _visitor_( Key_KP_9,            153,    "KP_9",             AKEYCODE_NUMPAD_9           )\
            _visitor_( Key_KP_Divide,       154,    "KP_Divide",        AKEYCODE_NUMPAD_DIVIDE      )\
            _visitor_( Key_KP_Multiply,     155,    "KP_Multiply",      AKEYCODE_NUMPAD_MULTIPLY    )\
            _visitor_( Key_KP_Subtract,     156,    "KP_Subtract",      AKEYCODE_NUMPAD_SUBTRACT    )\
            _visitor_( Key_KP_Add,          157,    "KP_Add",           AKEYCODE_NUMPAD_ADD         )\
            _visitor_( Key_KP_Dot,          158,    "KP_Dot",           AKEYCODE_NUMPAD_DOT         )\
            _visitor_( Key_KP_Comma,        159,    "KP_Comma",         AKEYCODE_NUMPAD_COMMA       )\
            _visitor_( Key_KP_Enter,        160,    "KP_Enter",         AKEYCODE_NUMPAD_ENTER       )\
            _visitor_( Key_KP_Equal,        161,    "KP_Equal",         AKEYCODE_NUMPAD_EQUALS      )\
            _visitor_( Key_KP_LeftParen,    162,    "KP_LeftParen",     AKEYCODE_NUMPAD_LEFT_PAREN  )   /* '(' */\
            _visitor_( Key_KP_RightParen,   163,    "KP_RightParen",    AKEYCODE_NUMPAD_RIGHT_PAREN )   /* ')' */\
            /* TV */\
            _visitor_( Key_TV,                  170,    "TV",                   AKEYCODE_TV                     )\
            _visitor_( Key_TV_Power,            177,    "TV_Power",             AKEYCODE_TV_POWER               )\
            _visitor_( Key_TV_Input,            178,    "TV_Input",             AKEYCODE_TV_INPUT               )\
            _visitor_( Key_TV_STBPower,         179,    "TV_STBPower",          AKEYCODE_STB_POWER              )\
            _visitor_( Key_TV_STBInput,         180,    "TV_STBInput",          AKEYCODE_STB_INPUT              )\
            _visitor_( Key_AV_Power,            181,    "AV_Power",             AKEYCODE_AVR_POWER              )\
            _visitor_( Key_AVR_Input,           182,    "AVR_Input",            AKEYCODE_AVR_INPUT              )\
            _visitor_( Key_TV_ProgRed,          183,    "TV_ProgRed",           AKEYCODE_PROG_RED               )\
            _visitor_( Key_TV_ProgGreen,        184,    "TV_ProgGreen",         AKEYCODE_PROG_GREEN             )\
            _visitor_( Key_TV_ProgYellow,       185,    "TV_ProgYellow",        AKEYCODE_PROG_YELLOW            )\
            _visitor_( Key_TV_ProgBlue,         186,    "TV_ProgBlue",          AKEYCODE_PROG_BLUE              )\
            _visitor_( Key_TV_DataService,      230,    "TV_DataService",       AKEYCODE_TV_DATA_SERVICE        )\
            _visitor_( Key_TV_RadioService,     232,    "TV_RadioService",      AKEYCODE_TV_RADIO_SERVICE       )\
            _visitor_( Key_TV_Teletext,         233,    "TV_Teletext",          AKEYCODE_TV_TELETEXT            )\
            _visitor_( Key_TV_NumberEntry,      234,    "TV_NumberEntry",       AKEYCODE_TV_NUMBER_ENTRY        )\
            _visitor_( Key_TV_TerrestrialAnalog, 235,   "TV_TerrestrialAnalog", AKEYCODE_TV_TERRESTRIAL_ANALOG  )\
            _visitor_( Key_TV_TerrestrialDigital, 236,  "TV_TerrestrialDigital",AKEYCODE_TV_TERRESTRIAL_DIGITAL )\
            _visitor_( Key_TV_Satellite,        237,    "TV_Satellite",         AKEYCODE_TV_SATELLITE           )\
            _visitor_( Key_TV_SatelliteBS,      238,    "TV_SatelliteBS",       AKEYCODE_TV_SATELLITE_BS        )\
            _visitor_( Key_TV_SattelliteCS,     239,    "TV_SattelliteCS",      AKEYCODE_TV_SATELLITE_CS        )\
            _visitor_( Key_TV_SatelliteService, 240,    "TV_SatelliteService",  AKEYCODE_TV_SATELLITE_SERVICE   )\
            _visitor_( Key_TV_Network,          241,    "TV_Network",           AKEYCODE_TV_NETWORK             )\
            _visitor_( Key_TV_AntennaCable,     242,    "TV_AntennaCable",      AKEYCODE_TV_ANTENNA_CABLE       )\
            _visitor_( Key_TV_InputHdmi1,       243,    "TV_InputHdmi1",        AKEYCODE_TV_INPUT_HDMI_1        )\
            _visitor_( Key_TV_InputHdmi2,       244,    "TV_InputHdmi2",        AKEYCODE_TV_INPUT_HDMI_2        )\
            _visitor_( Key_TV_InputHdmi3,       245,    "TV_InputHdmi3",        AKEYCODE_TV_INPUT_HDMI_3        )\
            _visitor_( Key_TV_InputHdmi4,       246,    "TV_InputHdmi4",        AKEYCODE_TV_INPUT_HDMI_4        )\
            _visitor_( Key_TV_inputComposite1,  247,    "TV_inputComposite1",   AKEYCODE_TV_INPUT_COMPOSITE_1   )\
            _visitor_( Key_TV_inputComposite2,  248,    "TV_inputComposite2",   AKEYCODE_TV_INPUT_COMPOSITE_2   )\
            _visitor_( Key_TV_InputComponent1,  249,    "TV_InputComponent1",   AKEYCODE_TV_INPUT_COMPONENT_1   )\
            _visitor_( Key_TV_InputComponent2,  250,    "TV_InputComponent2",   AKEYCODE_TV_INPUT_COMPONENT_2   )\
            _visitor_( Key_TV_InputVGA1,        251,    "TV_InputVGA1",         AKEYCODE_TV_INPUT_VGA_1         )\
            _visitor_( Key_TV_AudioDesc,        252,    "TV_AudioDesc",         AKEYCODE_TV_AUDIO_DESCRIPTION   )\
            _visitor_( Key_TV_AudioDescMixUp,   253,    "TV_AudioDescMixUp",    AKEYCODE_TV_AUDIO_DESCRIPTION_MIX_UP    )\
            _visitor_( Key_TV_AudioDescMixDown, 254,    "TV_AudioDescMixDown",  AKEYCODE_TV_AUDIO_DESCRIPTION_MIX_DOWN  )\
            _visitor_( Key_TV_ZoomMode,         255,    "TV_ZoomMode",          AKEYCODE_TV_ZOOM_MODE           )\
            _visitor_( Key_TV_ContentsMenu,     256,    "TV_ContentsMenu",      AKEYCODE_TV_CONTENTS_MENU       )\
            _visitor_( Key_TV_MediaContextMenu, 257,    "TV_MediaContextMenu",  AKEYCODE_TV_MEDIA_CONTEXT_MENU  )\
            _visitor_( Key_TV_TimerProgramming, 258,    "TV_TimerProgramming",  AKEYCODE_TV_TIMER_PROGRAMMING   )\

        enum class EInputType : ushort
        {
            KeyBegin            = 0,
            KeyEnd              = 512,

            #define AE_ANDROID_KEY_CODES_VISITOR( _key_, _code_, _name_, _and_code_ )       _key_ = _code_,
            AE_ANDROID_KEY_CODES( AE_ANDROID_KEY_CODES_VISITOR )
            #undef AE_ANDROID_KEY_CODES_VISITOR

            // touch
            MultiTouch          = KeyEnd + 10,      // float2 (scale, rotate)
            Cursor2DBegin,
            TouchPos            = Cursor2DBegin,    // float2 (absolute in pixels)
            TouchPos_mm,                            // float2 (absolute in mm)
            TouchDelta,                             // float2 (delta in pixels)
            TouchDelta_norm,                        // snorm2
            Cursor2DEnd         = TouchDelta_norm,

            // https://developer.android.com/guide/topics/sensors/sensors_overview

            Sensors1fBegin      = Cursor2DEnd + 1,
            AirTemperature      = Sensors1fBegin,   // float  (celsious)
            AmbientLight,                           // float  (lux)
            AirPressure,                            // float  (hPa or mbar)
            Proximity,                              // float  (mm)
            RelativeHumidity,                       // float  (%)
            StepCount,                              // ?
            BatteryState,                           // ?
            Sensors1fEnd        = BatteryState,

            Sensors2dBegin      = Sensors1fEnd + 1,
            GeoLocation         = Sensors2dBegin,   // double2 ?
            Sensors2dEnd        = GeoLocation,

            Sensors3fBegin      = Sensors2dEnd + 1,
            Accelerometer       = Sensors3fBegin,   // float3 (m/s2)
            Gravity,                                // float3 (m/s2)
            Gyroscope,                              // float3 (rad/s)
            LinearAcceleration,                     // float3 (m/s2)
            MagneticField,                          // float3 (micro tesla)
            RotationVector,                         // float3 (rad ?)
            Sensors3fEnd        = RotationVector,

            Sensors4x4fBegin    = Sensors3fEnd + 1,
            Pose6DOF            = Sensors4x4fBegin, // float4x4 ?
            Sensors4x4fEnd      = Pose6DOF,

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
        SerializableInputActionsAndroid () : SerializableInputActions{_Version} {}


    // SerializableInputActions //
        bool  IsKey (ushort type)           const override  { return _IsKey( EInputType(type) ); }
        bool  IsKeyOrTouch (ushort type)    const override  { return _IsKeyOrTouch( EInputType(type) ); }
        bool  IsCursor1D (ushort type)      const override  { return _IsCursor1D( EInputType(type) ); }
        bool  IsCursor2D (ushort type)      const override  { return _IsCursor2D( EInputType(type) ); }

        String  ToString ()                 const override;

      #ifdef AE_ENABLE_SCRIPTING
        bool  LoadFromScript (const Scripting::ScriptEnginePtr &se, String script, const SourceLoc &loc) override;

        static void  Bind (const Scripting::ScriptEnginePtr &se) __Th___;
      #endif

    private:
        ND_ static constexpr bool  _IsKey (EInputType type);
        ND_ static constexpr bool  _IsKeyOrTouch (EInputType type);
        ND_ static constexpr bool  _IsCursor1D (EInputType type);
        ND_ static constexpr bool  _IsCursor2D (EInputType type);
        ND_ static constexpr bool  _IsSensor1f (EInputType type);
        ND_ static constexpr bool  _IsSensor2d (EInputType type);
        ND_ static constexpr bool  _IsSensor3f (EInputType type);
        ND_ static constexpr bool  _IsSensor4x4f (EInputType type);
    };


/*
=================================================
    _Is***
=================================================
*/
    forceinline constexpr bool  SerializableInputActionsAndroid::_IsKey (EInputType type) {
        return (type >= EInputType::KeyBegin) & (type <= EInputType::KeyEnd);
    }

    forceinline constexpr bool  SerializableInputActionsAndroid::_IsKeyOrTouch (EInputType type) {
        return _IsKey( type ) | (type == EInputType::TouchPos) | (type == EInputType::TouchPos_mm);
    }

    forceinline constexpr bool  SerializableInputActionsAndroid::_IsCursor1D (EInputType type) {
        return _IsSensor1f( type );
    }

    forceinline constexpr bool  SerializableInputActionsAndroid::_IsCursor2D (EInputType type) {
        return  ((type >= EInputType::Cursor2DBegin) & (type <= EInputType::Cursor2DEnd))   |
                _IsSensor2d( type )                                                         |
                (type == EInputType::MultiTouch);
    }

    forceinline constexpr bool  SerializableInputActionsAndroid::_IsSensor1f (EInputType type) {
        return (type >= EInputType::Sensors1fBegin) & (type <= EInputType::Sensors1fEnd);
    }

    forceinline constexpr bool  SerializableInputActionsAndroid::_IsSensor2d (EInputType type) {
        return (type >= EInputType::Sensors2dBegin) & (type <= EInputType::Sensors2dEnd);
    }

    forceinline constexpr bool  SerializableInputActionsAndroid::_IsSensor3f (EInputType type) {
        return (type >= EInputType::Sensors3fBegin) & (type <= EInputType::Sensors3fEnd);
    }

    forceinline constexpr bool  SerializableInputActionsAndroid::_IsSensor4x4f (EInputType type) {
        return (type >= EInputType::Sensors4x4fBegin) & (type <= EInputType::Sensors4x4fEnd);
    }

} // AE::App


#ifdef AE_ENABLE_SCRIPTING
    AE_DECL_SCRIPT_OBJ_RC(  AE::App::SerializableInputActionsAndroid::ScriptBindingsMode,       "Android_BindingsMode"      );
    AE_DECL_SCRIPT_OBJ(     AE::App::SerializableInputActionsAndroid::ScriptActionBindings,     "Android_ActionBindings"    );
    AE_DECL_SCRIPT_TYPE(    AE::App::SerializableInputActionsAndroid::EInputType,               "Android_Input"             );
#endif
