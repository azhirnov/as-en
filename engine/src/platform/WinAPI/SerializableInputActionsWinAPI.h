// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Private/SerializableInputActions.h"

namespace AE::App
{
	static constexpr uint	InputActionsWinAPI_Name	= uint("ia.Win"_StringToID);


	//
	// Serializable Input Actions for WinAPI
	//

	class SerializableInputActionsWinAPI final : public SerializableInputActions
	{
		friend class InputActionsWinAPI;

	// types
	public:
		#define AE_WINAPI_KEY_CODES( _visitor_ ) \
			_visitor_( Key_Back,			0x08,		"Backspace",		VK_BACK				)\
			_visitor_( Key_Tab,				0x09,		"Tab",				VK_TAB				)\
			_visitor_( Key_Clear,			0x0C,		"Clear",			VK_CLEAR			)\
			_visitor_( Key_Return,			0x0D,		"Enter",			VK_RETURN			)\
			_visitor_( Key_L_Shift,			0x10,		"LeftShift",		VK_SHIFT			)\
			_visitor_( Key_R_Shift,			0x10 << 8,	"RightShift",		VK_SHIFT << 8		)\
			_visitor_( Key_L_Control,		0x11,		"LeftCtrl",			VK_CONTROL			)\
			_visitor_( Key_R_Control,		0x11 << 8,	"RightCtrl",		VK_CONTROL << 8		)\
			_visitor_( Key_L_Menu,			0x12,		"LeftAlt",			VK_MENU				)	/* alt */\
			_visitor_( Key_R_Menu,			0x12 << 8,	"RightAlt",			VK_MENU << 8		)\
			_visitor_( Key_Pause,			0x13,		"Pause",			VK_PAUSE			)\
			_visitor_( Key_Capital,			0x14,		"CapsLock",			VK_CAPITAL			)\
			_visitor_( Key_Escape,			0x1B,		"Escape",			VK_ESCAPE			)\
			_visitor_( Key_Space,			0x20,		"Space",			VK_SPACE			)\
			_visitor_( Key_Prior,			0x21 << 8,	"PageUp",			VK_PRIOR << 8		)	/* page up */\
			_visitor_( Key_Next,			0x22 << 8,	"PageDown",			VK_NEXT << 8		)	/* page down */\
			_visitor_( Key_End,				0x23 << 8,	"End",				VK_END << 8			)\
			_visitor_( Key_Home,			0x24 << 8,	"Home",				VK_HOME << 8		)\
			_visitor_( Key_Left,			0x25 << 8,	"ArrowLeft",		VK_LEFT << 8		)	/* arrows */\
			_visitor_( Key_Up,				0x26 << 8,	"ArrowUp",			VK_UP << 8			)\
			_visitor_( Key_Right,			0x27 << 8,	"ArrowRight",		VK_RIGHT << 8		)\
			_visitor_( Key_Down,			0x28 << 8,	"ArrowDown",		VK_DOWN << 8		)\
			_visitor_( Key_Select,			0x29,		"Select",			VK_SELECT			)\
			_visitor_( Key_Print,			0x2A,		"Print",			VK_PRINT			)\
			_visitor_( Key_Execute,			0x2B,		"Execute",			VK_EXECUTE			)\
			_visitor_( Key_Snapshot,		0x2C,		"Snapshot",			VK_SNAPSHOT			)\
			_visitor_( Key_Insert,			0x2D << 8,	"Insert",			VK_INSERT << 8		)\
			_visitor_( Key_Delete,			0x2E << 8,	"Delete",			VK_DELETE << 8		)\
			_visitor_( Key_Help,			0x2F,		"Help",				VK_HELP				)\
			_visitor_( Key_0,				0x30,		"0",				0x30				)\
			_visitor_( Key_1,				0x31,		"1",				0x31				)\
			_visitor_( Key_2,				0x32,		"2",				0x32				)\
			_visitor_( Key_3,				0x33,		"3",				0x33				)\
			_visitor_( Key_4,				0x34,		"4",				0x34				)\
			_visitor_( Key_5,				0x35,		"5",				0x35				)\
			_visitor_( Key_6,				0x36,		"6",				0x36				)\
			_visitor_( Key_7,				0x37,		"7",				0x37				)\
			_visitor_( Key_8,				0x38,		"8",				0x38				)\
			_visitor_( Key_9,				0x39,		"9",				0x39				)\
			_visitor_( Key_A,				0x41,		"A",				0x41				)\
			_visitor_( Key_B,				0x42,		"B",				0x42				)\
			_visitor_( Key_C,				0x43,		"C",				0x43				)\
			_visitor_( Key_D,				0x44,		"D",				0x44				)\
			_visitor_( Key_E,				0x45,		"E",				0x45				)\
			_visitor_( Key_F,				0x46,		"F",				0x46				)\
			_visitor_( Key_G,				0x47,		"G",				0x47				)\
			_visitor_( Key_H,				0x48,		"H",				0x48				)\
			_visitor_( Key_I,				0x49,		"I",				0x49				)\
			_visitor_( Key_J,				0x4A,		"J",				0x4A				)\
			_visitor_( Key_K,				0x4B,		"K",				0x4B				)\
			_visitor_( Key_L,				0x4C,		"L",				0x4C				)\
			_visitor_( Key_M,				0x4D,		"M",				0x4D				)\
			_visitor_( Key_N,				0x4E,		"N",				0x4E				)\
			_visitor_( Key_O,				0x4F,		"O",				0x4F				)\
			_visitor_( Key_P,				0x50,		"P",				0x50				)\
			_visitor_( Key_Q,				0x51,		"Q",				0x51				)\
			_visitor_( Key_R,				0x52,		"R",				0x52				)\
			_visitor_( Key_S,				0x53,		"S",				0x53				)\
			_visitor_( Key_T,				0x54,		"T",				0x54				)\
			_visitor_( Key_U,				0x55,		"U",				0x55				)\
			_visitor_( Key_V,				0x56,		"V",				0x56				)\
			_visitor_( Key_W,				0x57,		"W",				0x57				)\
			_visitor_( Key_X,				0x58,		"X",				0x58				)\
			_visitor_( Key_Y,				0x59,		"Y",				0x59				)\
			_visitor_( Key_Z,				0x5A,		"Z",				0x5A				)\
			_visitor_( Key_L_Win,			0x5B,		"LeftWin",			VK_LWIN				)\
			_visitor_( Key_R_Win,			0x5C << 8,	"RightWin",			VK_RWIN << 8		)\
			_visitor_( Key_L_Apps,			0x5D,		"LeftApps",			VK_APPS				)\
			_visitor_( Key_R_Apps,			0x5D << 8,	"RightApps",		VK_APPS << 8		)\
			_visitor_( Key_Sleep,			0x5F,		"Sleep",			VK_SLEEP			)\
			_visitor_( Key_F1,				0x70,		"F1",				VK_F1				)\
			_visitor_( Key_F2,				0x71,		"F2",				VK_F2				)\
			_visitor_( Key_F3,				0x72,		"F3",				VK_F3				)\
			_visitor_( Key_F4,				0x73,		"F4",				VK_F4				)\
			_visitor_( Key_F5,				0x74,		"F5",				VK_F5				)\
			_visitor_( Key_F6,				0x75,		"F6",				VK_F6				)\
			_visitor_( Key_F7,				0x76,		"F7",				VK_F7				)\
			_visitor_( Key_F8,				0x77,		"F8",				VK_F8				)\
			_visitor_( Key_F9,				0x78,		"F9",				VK_F9				)\
			_visitor_( Key_F10,				0x79,		"F10",				VK_F10				)\
			_visitor_( Key_F11,				0x7A,		"F11",				VK_F11				)\
			_visitor_( Key_F12,				0x7B,		"F12",				VK_F12				)\
			_visitor_( Key_F13,				0x7C,		"F13",				VK_F13				)\
			_visitor_( Key_F14,				0x7D,		"F14",				VK_F14				)\
			_visitor_( Key_F15,				0x7E,		"F15",				VK_F15				)\
			_visitor_( Key_F16,				0x7F,		"F16",				VK_F16				)\
			_visitor_( Key_F17,				0x80,		"F17",				VK_F17				)\
			_visitor_( Key_F18,				0x81,		"F18",				VK_F18				)\
			_visitor_( Key_F19,				0x82,		"F19",				VK_F19				)\
			_visitor_( Key_F20,				0x83,		"F20",				VK_F20				)\
			_visitor_( Key_F21,				0x84,		"F21",				VK_F21				)\
			_visitor_( Key_F22,				0x85,		"F22",				VK_F22				)\
			_visitor_( Key_F23,				0x86,		"F23",				VK_F23				)\
			_visitor_( Key_F24,				0x87,		"F24",				VK_F24				)\
			_visitor_( Key_NumLock,			0x90,		"NumLock",			VK_NUMLOCK			)\
			_visitor_( Key_ScrollLock,		0x91,		"ScrollLock",		VK_SCROLL			)\
			_visitor_( Key_BrowserBack,		0xA6,		"BrowserBack",		VK_BROWSER_BACK		)\
			_visitor_( Key_BrowserForward,	0xA7,		"BrowserForward",	VK_BROWSER_FORWARD	)\
			_visitor_( Key_BrowserRefresh,	0xA8,		"BrowserRefresh",	VK_BROWSER_REFRESH	)\
			_visitor_( Key_BrowserStop,		0xA9,		"BrowserStop",		VK_BROWSER_STOP		)\
			_visitor_( Key_BrowserSearch,	0xAA,		"BrowserSearch",	VK_BROWSER_SEARCH	)\
			_visitor_( Key_BrowserFavorites,0xAB,		"BrowserFavorites",	VK_BROWSER_FAVORITES)\
			_visitor_( Key_BrowserHome,		0xAC,		"BrowserHome",		VK_BROWSER_HOME		)\
			_visitor_( Key_VolumeMute,		0xAD,		"VolumeMute",		VK_VOLUME_MUTE		)\
			_visitor_( Key_VolumeDown,		0xAE,		"VolumeDown",		VK_VOLUME_DOWN		)\
			_visitor_( Key_VolumeUp,		0xAF,		"VolumeUp",			VK_VOLUME_UP		)\
			_visitor_( Key_MediaNextTrack,	0xB0,		"MediaNextTrack",	VK_MEDIA_NEXT_TRACK	)\
			_visitor_( Key_MediaPrevTrack,	0xB1,		"MediaPrevTrack",	VK_MEDIA_PREV_TRACK	)\
			_visitor_( Key_MediaStop,		0xB2,		"MediaStop",		VK_MEDIA_STOP		)\
			_visitor_( Key_MediaPlayPause,	0xB3,		"MediaPlayPause",	VK_MEDIA_PLAY_PAUSE	)\
			_visitor_( Key_LaunchMail,		0xB4,		"LaunchMail",		VK_LAUNCH_MAIL		)\
			_visitor_( Key_LaunchMediaSelect,0xB5,		"LaunchMediaSelect",VK_LAUNCH_MEDIA_SELECT )\
			_visitor_( Key_LaunchApp1,		0xB6,		"LaunchApp1",		VK_LAUNCH_APP1		)\
			_visitor_( Key_LaunchApp2,		0xB7,		"LaunchApp2",		VK_LAUNCH_APP2		)\
			_visitor_( Key_Semicolon,		0xBA,		"Semicolon",		VK_OEM_1			)	/* ';:' */\
			_visitor_( Key_Equal,			0xBB,		"Equal",			VK_OEM_PLUS			)	/* '+=' */\
			_visitor_( Key_Comma,			0xBC,		"Comma",			VK_OEM_COMMA		)	/* ',<' */\
			_visitor_( Key_Minus,			0xBD,		"Minus",			VK_OEM_MINUS		)	/* '-_' */\
			_visitor_( Key_Period,			0xBE,		"Period",			VK_OEM_PERIOD		)	/* '.>' */\
			_visitor_( Key_Slash,			0xBF,		"Slash",			VK_OEM_2			)	/* '/?' */\
			_visitor_( Key_GraveAccent,		0xC0,		"GraveAccent",		VK_OEM_3			)	/* '`~' */\
			_visitor_( Key_LeftBracket,		0xDB,		"LeftBracket",		VK_OEM_4			)	/* '[{' */\
			_visitor_( Key_BackSlash,		0xDC,		"BackSlash",		VK_OEM_5			)	/* '\|' */\
			_visitor_( Key_RightBracket,	0xDD,		"RightBracket",		VK_OEM_6			)	/* ']}' */\
			_visitor_( Key_Apostrophe,		0xDE,		"Apostrophe",		VK_OEM_7			)	/* ''"' */\
			/* keypad / numpad */\
			_visitor_( Key_KP_Enter,		0x0D << 8,	"KP_Enter",			VK_RETURN << 8		)\
			_visitor_( Key_KP_0,			0x60,		"KP_0",				VK_NUMPAD0			)\
			_visitor_( Key_KP_1,			0x61,		"KP_1",				VK_NUMPAD1			)\
			_visitor_( Key_KP_2,			0x62,		"KP_2",				VK_NUMPAD2			)\
			_visitor_( Key_KP_3,			0x63,		"KP_3",				VK_NUMPAD3			)\
			_visitor_( Key_KP_4,			0x64,		"KP_4",				VK_NUMPAD4			)\
			_visitor_( Key_KP_5,			0x65,		"KP_5",				VK_NUMPAD5			)\
			_visitor_( Key_KP_6,			0x66,		"KP_6",				VK_NUMPAD6			)\
			_visitor_( Key_KP_7,			0x67,		"KP_7",				VK_NUMPAD7			)\
			_visitor_( Key_KP_8,			0x68,		"KP_8",				VK_NUMPAD8			)\
			_visitor_( Key_KP_9,			0x69,		"KP_9",				VK_NUMPAD9			)\
			_visitor_( Key_KP_Multiply,		0x6A,		"KP_Multiply",		VK_MULTIPLY			)\
			_visitor_( Key_KP_Add,			0x6B,		"KP_Add",			VK_ADD				)\
			_visitor_( Key_KP_Separator,	0x6C,		"KP_Separator",		VK_SEPARATOR		)\
			_visitor_( Key_KP_Subtract,		0x6D,		"KP_Subtract",		VK_SUBTRACT			)\
			_visitor_( Key_KP_Decimal,		0x6E,		"KP_Decimal",		VK_DECIMAL			)\
			_visitor_( Key_KP_Divide,		0x6F,		"KP_Divide",		VK_DIVIDE			)\
			_visitor_( Key_KP_Divide2,		0x6F << 8,	"KP_Divide2",		VK_DIVIDE << 8		)	/* TODO ? */\
			_visitor_( Key_KP_End,			0x23,		"KP_End",			VK_END				)\
			_visitor_( Key_KP_ArrowLeft,	0x25,		"KP_ArrowLeft",		VK_LEFT				)\
			_visitor_( Key_KP_ArrowUp,		0x26,		"KP_ArrowUp",		VK_UP				)\
			_visitor_( Key_KP_ArrowRight,	0x27,		"KP_ArrowRight",	VK_RIGHT			)\
			_visitor_( Key_KP_ArrowDown,	0x28,		"KP_ArrowDown",		VK_DOWN				)\
			_visitor_( Key_KP_Home,			0x24,		"KP_Home",			VK_HOME				)\
			_visitor_( Key_KP_PageUp,		0x21,		"KP_PageUp",		VK_PRIOR			)\
			_visitor_( Key_KP_PageDown,		0x22,		"KP_PageDown",		VK_NEXT				)\
			_visitor_( Key_KP_Insert,		0x2D,		"KP_Insert",		VK_INSERT			)\
			_visitor_( Key_KP_Delete,		0x2E,		"KP_Delete",		VK_DELETE			)

		enum class EInputType : ushort
		{
			#define AE_WINAPI_KEY_CODES_VISITOR( _key_, _code_, _name_, _winapi_code_ )		_key_ = _code_,
			AE_WINAPI_KEY_CODES( AE_WINAPI_KEY_CODES_VISITOR )
			#undef AE_WINAPI_KEY_CODES_VISITOR

			MouseBtn0			= 0,	// left
			MouseBtn1			= 1,	// right
			MouseBtn2			= 2,	// middle
			MouseBtn3			= 3,
			MouseBtn4			= 4,
			
			MouseBegin			= MouseBtn0,
			MouseEnd			= MouseBtn4,

			KeyBegin			= 8,
			KeyEnd				= 0xDF << 8,

			Cursor1DBegin		= KeyEnd + 1,
			MouseWheelX			= Cursor1DBegin,// float (delta)
			MouseWheelY,						// float (delta)
			Cursor1DEnd			= MouseWheelY,
			
			Cursor2DBegin,
			CursorPos			= Cursor2DBegin,// float2 (absolute in pixels)
			CursorPos_mm,						// float2 (absolute in mm)
			CursorDelta,						// float2 (delta in pixels)
			CursorDelta_norm,					// snorm2
			TouchPos,							// float2 (absolute in pixels)
			TouchPos_mm,						// float2 (absolute in mm)
			TouchDelta,							// float2 (delta in pixels)
			TouchDelta_norm,					// snorm2
			Cursor2DEnd			= TouchDelta_norm,

			_Count,
			Unknown				= 0xFFFF,
		};

		struct ScriptBindingsMode;
		struct ScriptActionBindings;

	private:
		// for serialization
		static constexpr uint	_Version	= (1u << 16) | _BaseVersion;


	// methods
	public:
		SerializableInputActionsWinAPI () : SerializableInputActions{_Version} {}


	// SerializableInputActions //
		bool  IsKey (ushort type)		const override	{ return _IsKey( EInputType(type) ); }
		bool  IsCursor1D (ushort type)	const override	{ return _IsCursor1D( EInputType(type) ); }
		bool  IsCursor2D (ushort type)	const override	{ return _IsCursor2D( EInputType(type) ); }
		
		String  ToString () const override;

	  #ifdef AE_ENABLE_SCRIPTING
		bool  LoadFromScript (const Scripting::ScriptEnginePtr &se, String script, const SourceLoc &loc) override;

		static void  Bind (const Scripting::ScriptEnginePtr &se) __Th___;
	  #endif

	private:
		ND_ static constexpr bool  _IsKey (EInputType type);
		ND_ static constexpr bool  _IsCursor1D (EInputType type);
		ND_ static constexpr bool  _IsCursor2D (EInputType type);
	};

	
/*
=================================================
	_Is***
=================================================
*/
	forceinline constexpr bool  SerializableInputActionsWinAPI::_IsKey (EInputType type)
	{
		return	((type >= EInputType::MouseBegin) & (type <= EInputType::MouseEnd)) |
				((type >= EInputType::KeyBegin)   & (type <= EInputType::KeyEnd));
	}

	forceinline constexpr bool  SerializableInputActionsWinAPI::_IsCursor1D (EInputType type) {
		return (type >= EInputType::Cursor1DBegin) & (type <= EInputType::Cursor1DEnd);
	}

	forceinline constexpr bool  SerializableInputActionsWinAPI::_IsCursor2D (EInputType type) {
		return (type >= EInputType::Cursor2DBegin) & (type <= EInputType::Cursor2DEnd);
	}


} // AE::App


#ifdef AE_ENABLE_SCRIPTING
namespace AE::Scripting
{
	AE_DECL_SCRIPT_OBJ_RC(	AE::App::SerializableInputActionsWinAPI::ScriptBindingsMode,	"WinAPI_BindingsMode"	);
	AE_DECL_SCRIPT_OBJ(		AE::App::SerializableInputActionsWinAPI::ScriptActionBindings,	"WinAPI_ActionBindings"	);
	AE_DECL_SCRIPT_TYPE(	AE::App::SerializableInputActionsWinAPI::EInputType,			"WinAPI_Input"			);
}
#endif
