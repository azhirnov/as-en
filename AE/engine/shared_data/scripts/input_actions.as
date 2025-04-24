//7e3f8044
#pragma once
#include <vector>
#include <string>

#define funcdef // typedef for function

using int8		= std::int8_t;
using uint8		= std::uint8_t;
using int16		= std::int16_t;
using uint16	= std::uint16_t;
using uint		= std::uint32_t;
using int32		= std::int32_t;
using uint32	= std::uint32_t;
using int64		= std::int64_t;
using uint64	= std::uint64_t;
using string	= std::string;

template <typename T>
struct RC;

template <typename T>
using array = std::vector<T>;

using namespace std::string_literals;

struct ubyte2;
struct ubyte3;
struct WinAPI_ActionBindings;
struct ubyte4;
struct VecSwizzle;
struct Android_ActionBindings;
struct short4;
struct ushort3;
struct sbyte2;
struct Android_BindingsMode;
struct ActionInfo;
struct ushort2;
struct int4;
struct OpenVR_BindingsMode;
struct int2;
struct bool3;
struct int3;
struct bool4;
struct bool2;
struct GLFW_BindingsMode;
struct short3;
struct sbyte3;
struct short2;
struct ushort4;
struct sbyte4;
struct float4;
struct uint4;
struct uint3;
struct uint2;
struct float2;
struct float3;
struct OpenVR_ActionBindings;
struct WinAPI_BindingsMode;
struct GLFW_ActionBindings;

enum class EGestureState : uint8
{
	Begin,
	Update,
	End,
	Cancel,
	Outside,
};
uint8  operator | (EGestureState lhs, EGestureState rhs);
uint8  operator | (uint8 lhs, EGestureState rhs);
uint8  operator | (EGestureState lhs, uint8 rhs);

enum class EGestureType : uint8
{
	Hold,
	Move,
	LongPress_Move,
	ScaleRotate2D,
	Down,
	Click,
	LongPress,
	DoubleClick,
};
uint8  operator | (EGestureType lhs, EGestureType rhs);
uint8  operator | (uint8 lhs, EGestureType rhs);
uint8  operator | (EGestureType lhs, uint8 rhs);

enum class EValueType : uint8
{
	Unknown,
	Float,
	Float2,
	Float3,
	Float4,
	Quat,
	Float4x4,
	Chars,
	GNS,
};
uint8  operator | (EValueType lhs, EValueType rhs);
uint8  operator | (uint8 lhs, EValueType rhs);
uint8  operator | (EValueType lhs, uint8 rhs);

enum class GLFW_Input : uint16
{
	Space,
	Apostrophe,
	Comma,
	Minus,
	Period,
	Slash,
	Semicolon,
	Equal,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	LeftBracket,
	BackSlash,
	RightBracket,
	GraveAccent,
	World1,
	World2,
	Escape,
	Enter,
	Tab,
	Backspace,
	Insert,
	Delete,
	ArrowRight,
	ArrowLeft,
	ArrowDown,
	ArrowUp,
	PageUp,
	PageDown,
	Home,
	End,
	CapsLock,
	ScrollLock,
	NumLock,
	PrintScreen,
	Pause,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,
	F16,
	F17,
	F18,
	F19,
	F20,
	F21,
	F22,
	F23,
	F24,
	F25,
	LeftShift,
	LeftControl,
	LeftAlt,
	LeftSuper,
	RightShift,
	RightControl,
	RightAlt,
	RightSuper,
	Menu,
	KP_0,
	KP_1,
	KP_2,
	KP_3,
	KP_4,
	KP_5,
	KP_6,
	KP_7,
	KP_8,
	KP_9,
	KP_Decimal,
	KP_Divide,
	KP_Multiply,
	KP_Subtract,
	KP_Add,
	KP_Enter,
	KP_Equal,
	MouseBtn1,
	MouseBtn2,
	MouseBtn3,
	MouseBtn4,
	MouseBtn5,
	MouseBtn6,
	MouseBtn7,
	MouseBtn8,
	Accelerometer,
	MagneticField,
	GeoLocation,
	Gyroscope,
	AmbientLight,
	AirPressure,
	Proximity,
	Gravity,
	LinearAcceleration,
	RotationVector,
	RelativeHumidity,
	AirTemperature,
	GameRotationVector,
	Pose6DOF,
	MultiTouch,
	MouseWheel,
	CursorPos,
	CursorPos_mm,
	CursorDelta,
	CursorDelta_norm,
	TouchPos,
	TouchPos_mm,
	TouchDelta,
	TouchDelta_norm,
	MouseLeft,
	MouseRight,
	MouseMiddle,
};
uint16  operator | (GLFW_Input lhs, GLFW_Input rhs);
uint16  operator | (uint16 lhs, GLFW_Input rhs);
uint16  operator | (GLFW_Input lhs, uint16 rhs);
static constexpr GLFW_Input GLFW_Input_0 = GLFW_Input(48);
static constexpr GLFW_Input GLFW_Input_1 = GLFW_Input(49);
static constexpr GLFW_Input GLFW_Input_2 = GLFW_Input(50);
static constexpr GLFW_Input GLFW_Input_3 = GLFW_Input(51);
static constexpr GLFW_Input GLFW_Input_4 = GLFW_Input(52);
static constexpr GLFW_Input GLFW_Input_5 = GLFW_Input(53);
static constexpr GLFW_Input GLFW_Input_6 = GLFW_Input(54);
static constexpr GLFW_Input GLFW_Input_7 = GLFW_Input(55);
static constexpr GLFW_Input GLFW_Input_8 = GLFW_Input(56);
static constexpr GLFW_Input GLFW_Input_9 = GLFW_Input(57);

enum class WinAPI_Input : uint16
{
	Backspace,
	Tab,
	Clear,
	Enter,
	LeftShift,
	RightShift,
	LeftCtrl,
	RightCtrl,
	LeftAlt,
	RightAlt,
	Pause,
	CapsLock,
	Escape,
	Space,
	PageUp,
	PageDown,
	End,
	Home,
	ArrowLeft,
	ArrowUp,
	ArrowRight,
	ArrowDown,
	Select,
	Print,
	Execute,
	Snapshot,
	Insert,
	Delete,
	Help,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	LeftWin,
	RightWin,
	LeftApps,
	RightApps,
	Sleep,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,
	F16,
	F17,
	F18,
	F19,
	F20,
	F21,
	F22,
	F23,
	F24,
	NumLock,
	ScrollLock,
	BrowserBack,
	BrowserForward,
	BrowserRefresh,
	BrowserStop,
	BrowserSearch,
	BrowserFavorites,
	BrowserHome,
	VolumeMute,
	VolumeDown,
	VolumeUp,
	MediaNextTrack,
	MediaPrevTrack,
	MediaStop,
	MediaPlayPause,
	LaunchMail,
	LaunchMediaSelect,
	LaunchApp1,
	LaunchApp2,
	Semicolon,
	Equal,
	Comma,
	Minus,
	Period,
	Slash,
	GraveAccent,
	LeftBracket,
	BackSlash,
	RightBracket,
	Apostrophe,
	KP_Enter,
	KP_0,
	KP_1,
	KP_2,
	KP_3,
	KP_4,
	KP_5,
	KP_6,
	KP_7,
	KP_8,
	KP_9,
	KP_Multiply,
	KP_Add,
	KP_Separator,
	KP_Subtract,
	KP_Decimal,
	KP_Divide,
	KP_Divide2,
	KP_End,
	KP_ArrowLeft,
	KP_ArrowUp,
	KP_ArrowRight,
	KP_ArrowDown,
	KP_Home,
	KP_PageUp,
	KP_PageDown,
	KP_Insert,
	KP_Delete,
	MouseBtn0,
	MouseBtn1,
	MouseBtn2,
	MouseBtn3,
	MouseBtn4,
	MultiTouch,
	MouseWheel,
	CursorPos,
	CursorPos_mm,
	CursorDelta,
	CursorDelta_norm,
	TouchPos,
	TouchPos_mm,
	TouchDelta,
	TouchDelta_norm,
	MouseLeft,
	MouseRight,
	MouseMiddle,
};
uint16  operator | (WinAPI_Input lhs, WinAPI_Input rhs);
uint16  operator | (uint16 lhs, WinAPI_Input rhs);
uint16  operator | (WinAPI_Input lhs, uint16 rhs);
static constexpr WinAPI_Input WinAPI_Input_0 = WinAPI_Input(48);
static constexpr WinAPI_Input WinAPI_Input_1 = WinAPI_Input(49);
static constexpr WinAPI_Input WinAPI_Input_2 = WinAPI_Input(50);
static constexpr WinAPI_Input WinAPI_Input_3 = WinAPI_Input(51);
static constexpr WinAPI_Input WinAPI_Input_4 = WinAPI_Input(52);
static constexpr WinAPI_Input WinAPI_Input_5 = WinAPI_Input(53);
static constexpr WinAPI_Input WinAPI_Input_6 = WinAPI_Input(54);
static constexpr WinAPI_Input WinAPI_Input_7 = WinAPI_Input(55);
static constexpr WinAPI_Input WinAPI_Input_8 = WinAPI_Input(56);
static constexpr WinAPI_Input WinAPI_Input_9 = WinAPI_Input(57);

enum class Android_Input : uint16
{
	Back,
	Star,
	Paund,
	VolumeUp,
	VolumeDown,
	VolumeMute,
	Power,
	Camera,
	Clear,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	Comma,
	Period,
	LeftAlt,
	RightAlt,
	LeftShift,
	RightShift,
	Tab,
	Space,
	Sym,
	Browser,
	LaunchMail,
	Enter,
	Delete,
	Grave,
	Minus,
	Equal,
	LeftBracket,
	RightBracket,
	BackSlash,
	Semicolon,
	Apostrophe,
	Slash,
	At,
	Num,
	Plus,
	Menu,
	Notification,
	Search,
	MicMute,
	PageUp,
	PageDown,
	PictSymbols,
	SwitchCharset,
	Escape,
	ForwardDelete,
	LeftControl,
	RightControl,
	CapsLock,
	ScrollLock,
	LeftMeta,
	RightMeta,
	Function,
	SysRq,
	PauseBreak,
	MoveHome,
	MoveEnd,
	Insert,
	Forward,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	NumLock,
	Info,
	ChannelUp,
	ChannelDown,
	ZoomIn,
	ZoomOut,
	Window,
	Guide,
	DVR,
	Bookmark,
	Captions,
	Settings,
	AppSwitch,
	LangSwitch,
	MannerMode,
	Contacts,
	Calendar,
	Music,
	Calculator,
	ZenkakuHankaku,
	Eisu,
	Muhenkan,
	Henkan,
	KatakanaHiragana,
	Yen,
	Ro,
	Kana,
	Assist,
	BrightnessDown,
	BrightnessUp,
	Sleep,
	Wakeup,
	Pairing,
	LastChannel,
	VoiceAssist,
	Help,
	NavPrev,
	NavNext,
	NavIn,
	NavOut,
	StemPrimary,
	Stem1,
	Stem2,
	Stem3,
	SoftSleep,
	Cut,
	Copy,
	Paste,
	SysNavUp,
	SysNavDown,
	SysNavLeft,
	SysNavRight,
	AllApps,
	Refresh,
	ThumbsUp,
	ThumbsDown,
	ProfileSwitch,
	MediaPlayPause,
	MediaStop,
	MediaNext,
	MediaPrev,
	MediaRewind,
	MediaFastForward,
	MediaPlay,
	MediaPause,
	MediaClose,
	MediaEject,
	MediaRecord,
	MediaAudioTrack,
	MediaTopMenu,
	MediaSkipForward,
	MediaSkipBackward,
	MediaStepForward,
	MediaStepBackward,
	DPadUp,
	DPadDown,
	DPadLeft,
	DPadRight,
	DPadCenter,
	DPadUpLeft,
	DPadDownLeft,
	DPadUpRight,
	DPadDownRight,
	GPadA,
	GPadB,
	GPadC,
	GPadX,
	GPadY,
	GPadZ,
	GPadL1,
	GPadR1,
	GPadL2,
	GPadR2,
	GPadThumbL,
	GPadThumbR,
	GPadStart,
	GPadSelect,
	GPadMode,
	GPad1,
	GPad2,
	GPad3,
	GPad4,
	GPad5,
	GPad6,
	GPad7,
	GPad8,
	GPad9,
	GPad10,
	GPad11,
	GPad12,
	GPad13,
	GPad14,
	GPad15,
	GPad16,
	KP_0,
	KP_1,
	KP_2,
	KP_3,
	KP_4,
	KP_5,
	KP_6,
	KP_7,
	KP_8,
	KP_9,
	KP_Divide,
	KP_Multiply,
	KP_Subtract,
	KP_Add,
	KP_Dot,
	KP_Comma,
	KP_Enter,
	KP_Equal,
	KP_LeftParen,
	KP_RightParen,
	TV,
	TV_Power,
	TV_Input,
	TV_STBPower,
	TV_STBInput,
	AV_Power,
	AVR_Input,
	TV_ProgRed,
	TV_ProgGreen,
	TV_ProgYellow,
	TV_ProgBlue,
	TV_DataService,
	TV_RadioService,
	TV_Teletext,
	TV_NumberEntry,
	TV_TerrestrialAnalog,
	TV_TerrestrialDigital,
	TV_Satellite,
	TV_SatelliteBS,
	TV_SattelliteCS,
	TV_SatelliteService,
	TV_Network,
	TV_AntennaCable,
	TV_InputHdmi1,
	TV_InputHdmi2,
	TV_InputHdmi3,
	TV_InputHdmi4,
	TV_inputComposite1,
	TV_inputComposite2,
	TV_InputComponent1,
	TV_InputComponent2,
	TV_InputVGA1,
	TV_AudioDesc,
	TV_AudioDescMixUp,
	TV_AudioDescMixDown,
	TV_ZoomMode,
	TV_ContentsMenu,
	TV_MediaContextMenu,
	TV_TimerProgramming,
	Accelerometer,
	MagneticField,
	GeoLocation,
	Gyroscope,
	AmbientLight,
	AirPressure,
	Proximity,
	Gravity,
	LinearAcceleration,
	RotationVector,
	RelativeHumidity,
	AirTemperature,
	GameRotationVector,
	Pose6DOF,
	MultiTouch,
	TouchPos,
	TouchPos_mm,
	TouchDelta,
	TouchDelta_norm,
};
uint16  operator | (Android_Input lhs, Android_Input rhs);
uint16  operator | (uint16 lhs, Android_Input rhs);
uint16  operator | (Android_Input lhs, uint16 rhs);
static constexpr Android_Input Android_Input_0 = Android_Input(7);
static constexpr Android_Input Android_Input_1 = Android_Input(8);
static constexpr Android_Input Android_Input_2 = Android_Input(9);
static constexpr Android_Input Android_Input_3 = Android_Input(10);
static constexpr Android_Input Android_Input_4 = Android_Input(11);
static constexpr Android_Input Android_Input_5 = Android_Input(12);
static constexpr Android_Input Android_Input_6 = Android_Input(13);
static constexpr Android_Input Android_Input_7 = Android_Input(14);
static constexpr Android_Input Android_Input_8 = Android_Input(15);
static constexpr Android_Input Android_Input_9 = Android_Input(16);
static constexpr Android_Input Android_Input_3DMode = Android_Input(206);
static constexpr Android_Input Android_Input_11 = Android_Input(227);
static constexpr Android_Input Android_Input_12 = Android_Input(228);

enum class OpenVR_Input : uint16
{
};
uint16  operator | (OpenVR_Input lhs, OpenVR_Input rhs);
uint16  operator | (uint16 lhs, OpenVR_Input rhs);
uint16  operator | (OpenVR_Input lhs, uint16 rhs);

using sbyte = int8;
using ubyte = uint8;
using sshort = int16;
using ushort = uint16;
using sint = int;
using slong = int64;
using ulong = uint64;
int  Abs (int x);
int  MirroredWrap (int x, int min, int max);
int  Square (int x);
int  Min (int x, int y);
int  Max (int x, int y);
int  Clamp (int x, int min, int max);
int  Wrap (int x, int min, int max);
int  Average (int x, int y);
int  IntLog2 (int x);
int  CeilIntLog2 (int x);
int  BitScanReverse (int x);
int  BitScanForward (int x);
uint  BitCount (int x);
bool  IsPowerOfTwo (int x);
bool  IsSingleBitSet (int x);
bool  AllBits (int x, int y);
bool  AnyBits (int x, int y);
int  ExtractBit (int & x);
int  ExtractBitIndex (int & x);
int  BitRotateLeft (int x, uint shift);
int  BitRotateRight (int x, uint shift);
int  FloorPOT (int x);
int  CeilPOT (int x);
int  AlignDown (int x, int align);
int  AlignUp (int x, int align);
bool  IsMultipleOf (int x, int align);
int  SafeDiv (int x, int y, int defVal);
int  DivCeil (int x, int y);
uint  Square (uint x);
uint  Min (uint x, uint y);
uint  Max (uint x, uint y);
uint  Clamp (uint x, uint min, uint max);
uint  Wrap (uint x, uint min, uint max);
uint  Average (uint x, uint y);
int  IntLog2 (uint x);
int  CeilIntLog2 (uint x);
int  BitScanReverse (uint x);
int  BitScanForward (uint x);
uint  BitCount (uint x);
bool  IsPowerOfTwo (uint x);
bool  IsSingleBitSet (uint x);
bool  AllBits (uint x, uint y);
bool  AnyBits (uint x, uint y);
uint  ExtractBit (uint & x);
uint  ExtractBitIndex (uint & x);
uint  BitRotateLeft (uint x, uint shift);
uint  BitRotateRight (uint x, uint shift);
uint  FloorPOT (uint x);
uint  CeilPOT (uint x);
uint  AlignDown (uint x, uint align);
uint  AlignUp (uint x, uint align);
bool  IsMultipleOf (uint x, uint align);
uint  SafeDiv (uint x, uint y, uint defVal);
uint  DivCeil (uint x, uint y);
float  Abs (float x);
float  MirroredWrap (float x, float min, float max);
float  Square (float x);
float  Min (float x, float y);
float  Max (float x, float y);
float  Clamp (float x, float min, float max);
float  Wrap (float x, float min, float max);
float  Average (float x, float y);
float  Floor (float x);
float  Ceil (float x);
float  Trunc (float x);
float  Fract (float x);
float  Round (float x);
float  Mod (float x, float y);
float  Sqrt (float x);
float  Pow (float x, float pow);
float  Ln (float x);
float  Log (float x, float base);
float  Log2 (float x);
float  Log10 (float x);
float  Exp (float x);
float  Exp2 (float x);
float  Exp10 (float x);
float  ExpMinus1 (float x);
float  Sin (float x);
float  SinH (float x);
float  ASin (float x);
float  Cos (float x);
float  CosH (float x);
float  ACos (float x);
float  Tan (float x);
float  TanH (float x);
float  ATan (float y, float x);
float  ToRad (float x);
float  ToDeg (float x);
float  Lerp (float x, float y, float factor);
float  ToSNorm (float x);
float  ToUNorm (float x);
float  Remap (float srcMin, float srcMax, float dstMin, float dstMax, float x);
float  RemapClamp (float srcMin, float srcMax, float dstMin, float dstMax, float x);
int  RoundToInt (float x);
uint  RoundToUint (float x);
float  IsInfinity (float x);
float  IsNaN (float x);
float  IsFinite (float x);
float  Pi ();
struct bool2
{
	bool2 ();
	bool2 (const bool2&);
	bool2&  operator = (const bool2&);
	bool x;
	bool y;
	bool2 (bool v);
	bool2 (const bool3 & v3);
	bool2 (const bool4 & v4);
	bool2 (bool x, bool y);
	bool2 (const short2 & v2);
	bool2 (const short3 & v3);
	bool2 (const short4 & v4);
	bool2 (const ushort2 & v2);
	bool2 (const ushort3 & v3);
	bool2 (const ushort4 & v4);
	bool2 (const int2 & v2);
	bool2 (const int3 & v3);
	bool2 (const int4 & v4);
	bool2 (const uint2 & v2);
	bool2 (const uint3 & v3);
	bool2 (const uint4 & v4);
	bool2 (const float2 & v2);
	bool2 (const float3 & v3);
	bool2 (const float4 & v4);
	bool  opEquals (const bool2 &, const bool2 &);
	int  opCmp (const bool2 &, const bool2 &);
	bool2  opCom () const;
};

struct bool3
{
	bool3 ();
	bool3 (const bool3&);
	bool3&  operator = (const bool3&);
	bool x;
	bool y;
	bool z;
	bool3 (bool v);
	bool3 (const bool2 & v2);
	bool3 (const bool4 & v4);
	bool3 (const bool2 & xy, bool z);
	bool3 (bool x, bool y, bool z);
	bool3 (const short2 & v2);
	bool3 (const short3 & v3);
	bool3 (const short4 & v4);
	bool3 (const ushort2 & v2);
	bool3 (const ushort3 & v3);
	bool3 (const ushort4 & v4);
	bool3 (const int2 & v2);
	bool3 (const int3 & v3);
	bool3 (const int4 & v4);
	bool3 (const uint2 & v2);
	bool3 (const uint3 & v3);
	bool3 (const uint4 & v4);
	bool3 (const float2 & v2);
	bool3 (const float3 & v3);
	bool3 (const float4 & v4);
	bool  opEquals (const bool3 &, const bool3 &);
	int  opCmp (const bool3 &, const bool3 &);
	bool3  opCom () const;
};

struct bool4
{
	bool4 ();
	bool4 (const bool4&);
	bool4&  operator = (const bool4&);
	bool x;
	bool y;
	bool z;
	bool w;
	bool4 (bool v);
	bool4 (const bool2 & v2);
	bool4 (const bool3 & v3);
	bool4 (const bool2 & xy, const bool2 & zw);
	bool4 (const bool3 & xyz, bool w);
	bool4 (bool x, bool y, bool z, bool w);
	bool4 (bool x, const bool3 & yzw);
	bool4 (const short2 & v2);
	bool4 (const short3 & v3);
	bool4 (const short4 & v4);
	bool4 (const ushort2 & v2);
	bool4 (const ushort3 & v3);
	bool4 (const ushort4 & v4);
	bool4 (const int2 & v2);
	bool4 (const int3 & v3);
	bool4 (const int4 & v4);
	bool4 (const uint2 & v2);
	bool4 (const uint3 & v3);
	bool4 (const uint4 & v4);
	bool4 (const float2 & v2);
	bool4 (const float3 & v3);
	bool4 (const float4 & v4);
	bool  opEquals (const bool4 &, const bool4 &);
	int  opCmp (const bool4 &, const bool4 &);
	bool4  opCom () const;
};

struct sbyte2
{
	sbyte2 ();
	sbyte2 (const sbyte2&);
	sbyte2&  operator = (const sbyte2&);
	int8 x;
	int8 y;
	sbyte2 (int8 v);
	sbyte2 (const sbyte3 & v3);
	sbyte2 (const sbyte4 & v4);
	sbyte2 (int8 x, int8 y);
	sbyte2 (const short2 & v2);
	sbyte2 (const short3 & v3);
	sbyte2 (const short4 & v4);
	sbyte2 (const ushort2 & v2);
	sbyte2 (const ushort3 & v3);
	sbyte2 (const ushort4 & v4);
	sbyte2 (const int2 & v2);
	sbyte2 (const int3 & v3);
	sbyte2 (const int4 & v4);
	sbyte2 (const uint2 & v2);
	sbyte2 (const uint3 & v3);
	sbyte2 (const uint4 & v4);
	sbyte2 (const float2 & v2);
	sbyte2 (const float3 & v3);
	sbyte2 (const float4 & v4);
	sbyte2 &  opAddAssign (const sbyte2 &);
	sbyte2 &  opAddAssign (int8);
	sbyte2  opAdd (const sbyte2 &) const;
	sbyte2  opAdd (int8) const;
	sbyte2  opAdd_r (int8, const sbyte2 &);
	sbyte2 &  opSubAssign (const sbyte2 &);
	sbyte2 &  opSubAssign (int8);
	sbyte2  opSub (const sbyte2 &) const;
	sbyte2  opSub (int8) const;
	sbyte2  opSub_r (int8, const sbyte2 &);
	sbyte2 &  opMulAssign (const sbyte2 &);
	sbyte2 &  opMulAssign (int8);
	sbyte2  opMul (const sbyte2 &) const;
	sbyte2  opMul (int8) const;
	sbyte2  opMul_r (int8, const sbyte2 &);
	sbyte2 &  opDivAssign (const sbyte2 &);
	sbyte2 &  opDivAssign (int8);
	sbyte2  opDiv (const sbyte2 &) const;
	sbyte2  opDiv (int8) const;
	sbyte2  opDiv_r (int8, const sbyte2 &);
	sbyte2 &  opModAssign (const sbyte2 &);
	sbyte2 &  opModAssign (int8);
	sbyte2  opMod (const sbyte2 &) const;
	sbyte2  opMod (int8) const;
	sbyte2  opMod_r (int8, const sbyte2 &);
	sbyte2 &  opAndAssign (const sbyte2 &);
	sbyte2 &  opAndAssign (int8);
	sbyte2  opAnd (const sbyte2 &) const;
	sbyte2  opAnd (int8) const;
	sbyte2  opAnd_r (int8, const sbyte2 &);
	sbyte2 &  opOrAssign (const sbyte2 &);
	sbyte2 &  opOrAssign (int8);
	sbyte2  opOr (const sbyte2 &) const;
	sbyte2  opOr (int8) const;
	sbyte2  opOr_r (int8, const sbyte2 &);
	sbyte2 &  opXorAssign (const sbyte2 &);
	sbyte2 &  opXorAssign (int8);
	sbyte2  opXor (const sbyte2 &) const;
	sbyte2  opXor (int8) const;
	sbyte2  opXor_r (int8, const sbyte2 &);
	sbyte2 &  opShlAssign (const sbyte2 &);
	sbyte2 &  opShlAssign (int8);
	sbyte2  opShl (const sbyte2 &) const;
	sbyte2  opShl (int8) const;
	sbyte2  opShl_r (int8, const sbyte2 &);
	sbyte2 &  opShrAssign (const sbyte2 &);
	sbyte2 &  opShrAssign (int8);
	sbyte2  opShr (const sbyte2 &) const;
	sbyte2  opShr (int8) const;
	sbyte2  opShr_r (int8, const sbyte2 &);
	bool  opEquals (const sbyte2 &, const sbyte2 &);
	int  opCmp (const sbyte2 &, const sbyte2 &);
};

struct sbyte3
{
	sbyte3 ();
	sbyte3 (const sbyte3&);
	sbyte3&  operator = (const sbyte3&);
	int8 x;
	int8 y;
	int8 z;
	sbyte3 (int8 v);
	sbyte3 (const sbyte2 & v2);
	sbyte3 (const sbyte4 & v4);
	sbyte3 (const sbyte2 & xy, int8 z);
	sbyte3 (int8 x, int8 y, int8 z);
	sbyte3 (const short2 & v2);
	sbyte3 (const short3 & v3);
	sbyte3 (const short4 & v4);
	sbyte3 (const ushort2 & v2);
	sbyte3 (const ushort3 & v3);
	sbyte3 (const ushort4 & v4);
	sbyte3 (const int2 & v2);
	sbyte3 (const int3 & v3);
	sbyte3 (const int4 & v4);
	sbyte3 (const uint2 & v2);
	sbyte3 (const uint3 & v3);
	sbyte3 (const uint4 & v4);
	sbyte3 (const float2 & v2);
	sbyte3 (const float3 & v3);
	sbyte3 (const float4 & v4);
	sbyte3 &  opAddAssign (const sbyte3 &);
	sbyte3 &  opAddAssign (int8);
	sbyte3  opAdd (const sbyte3 &) const;
	sbyte3  opAdd (int8) const;
	sbyte3  opAdd_r (int8, const sbyte3 &);
	sbyte3 &  opSubAssign (const sbyte3 &);
	sbyte3 &  opSubAssign (int8);
	sbyte3  opSub (const sbyte3 &) const;
	sbyte3  opSub (int8) const;
	sbyte3  opSub_r (int8, const sbyte3 &);
	sbyte3 &  opMulAssign (const sbyte3 &);
	sbyte3 &  opMulAssign (int8);
	sbyte3  opMul (const sbyte3 &) const;
	sbyte3  opMul (int8) const;
	sbyte3  opMul_r (int8, const sbyte3 &);
	sbyte3 &  opDivAssign (const sbyte3 &);
	sbyte3 &  opDivAssign (int8);
	sbyte3  opDiv (const sbyte3 &) const;
	sbyte3  opDiv (int8) const;
	sbyte3  opDiv_r (int8, const sbyte3 &);
	sbyte3 &  opModAssign (const sbyte3 &);
	sbyte3 &  opModAssign (int8);
	sbyte3  opMod (const sbyte3 &) const;
	sbyte3  opMod (int8) const;
	sbyte3  opMod_r (int8, const sbyte3 &);
	sbyte3 &  opAndAssign (const sbyte3 &);
	sbyte3 &  opAndAssign (int8);
	sbyte3  opAnd (const sbyte3 &) const;
	sbyte3  opAnd (int8) const;
	sbyte3  opAnd_r (int8, const sbyte3 &);
	sbyte3 &  opOrAssign (const sbyte3 &);
	sbyte3 &  opOrAssign (int8);
	sbyte3  opOr (const sbyte3 &) const;
	sbyte3  opOr (int8) const;
	sbyte3  opOr_r (int8, const sbyte3 &);
	sbyte3 &  opXorAssign (const sbyte3 &);
	sbyte3 &  opXorAssign (int8);
	sbyte3  opXor (const sbyte3 &) const;
	sbyte3  opXor (int8) const;
	sbyte3  opXor_r (int8, const sbyte3 &);
	sbyte3 &  opShlAssign (const sbyte3 &);
	sbyte3 &  opShlAssign (int8);
	sbyte3  opShl (const sbyte3 &) const;
	sbyte3  opShl (int8) const;
	sbyte3  opShl_r (int8, const sbyte3 &);
	sbyte3 &  opShrAssign (const sbyte3 &);
	sbyte3 &  opShrAssign (int8);
	sbyte3  opShr (const sbyte3 &) const;
	sbyte3  opShr (int8) const;
	sbyte3  opShr_r (int8, const sbyte3 &);
	bool  opEquals (const sbyte3 &, const sbyte3 &);
	int  opCmp (const sbyte3 &, const sbyte3 &);
};

struct sbyte4
{
	sbyte4 ();
	sbyte4 (const sbyte4&);
	sbyte4&  operator = (const sbyte4&);
	int8 x;
	int8 y;
	int8 z;
	int8 w;
	sbyte4 (int8 v);
	sbyte4 (const sbyte2 & v2);
	sbyte4 (const sbyte3 & v3);
	sbyte4 (const sbyte2 & xy, const sbyte2 & zw);
	sbyte4 (const sbyte3 & xyz, int8 w);
	sbyte4 (int8 x, int8 y, int8 z, int8 w);
	sbyte4 (int8 x, const sbyte3 & yzw);
	sbyte4 (const short2 & v2);
	sbyte4 (const short3 & v3);
	sbyte4 (const short4 & v4);
	sbyte4 (const ushort2 & v2);
	sbyte4 (const ushort3 & v3);
	sbyte4 (const ushort4 & v4);
	sbyte4 (const int2 & v2);
	sbyte4 (const int3 & v3);
	sbyte4 (const int4 & v4);
	sbyte4 (const uint2 & v2);
	sbyte4 (const uint3 & v3);
	sbyte4 (const uint4 & v4);
	sbyte4 (const float2 & v2);
	sbyte4 (const float3 & v3);
	sbyte4 (const float4 & v4);
	sbyte4 &  opAddAssign (const sbyte4 &);
	sbyte4 &  opAddAssign (int8);
	sbyte4  opAdd (const sbyte4 &) const;
	sbyte4  opAdd (int8) const;
	sbyte4  opAdd_r (int8, const sbyte4 &);
	sbyte4 &  opSubAssign (const sbyte4 &);
	sbyte4 &  opSubAssign (int8);
	sbyte4  opSub (const sbyte4 &) const;
	sbyte4  opSub (int8) const;
	sbyte4  opSub_r (int8, const sbyte4 &);
	sbyte4 &  opMulAssign (const sbyte4 &);
	sbyte4 &  opMulAssign (int8);
	sbyte4  opMul (const sbyte4 &) const;
	sbyte4  opMul (int8) const;
	sbyte4  opMul_r (int8, const sbyte4 &);
	sbyte4 &  opDivAssign (const sbyte4 &);
	sbyte4 &  opDivAssign (int8);
	sbyte4  opDiv (const sbyte4 &) const;
	sbyte4  opDiv (int8) const;
	sbyte4  opDiv_r (int8, const sbyte4 &);
	sbyte4 &  opModAssign (const sbyte4 &);
	sbyte4 &  opModAssign (int8);
	sbyte4  opMod (const sbyte4 &) const;
	sbyte4  opMod (int8) const;
	sbyte4  opMod_r (int8, const sbyte4 &);
	sbyte4 &  opAndAssign (const sbyte4 &);
	sbyte4 &  opAndAssign (int8);
	sbyte4  opAnd (const sbyte4 &) const;
	sbyte4  opAnd (int8) const;
	sbyte4  opAnd_r (int8, const sbyte4 &);
	sbyte4 &  opOrAssign (const sbyte4 &);
	sbyte4 &  opOrAssign (int8);
	sbyte4  opOr (const sbyte4 &) const;
	sbyte4  opOr (int8) const;
	sbyte4  opOr_r (int8, const sbyte4 &);
	sbyte4 &  opXorAssign (const sbyte4 &);
	sbyte4 &  opXorAssign (int8);
	sbyte4  opXor (const sbyte4 &) const;
	sbyte4  opXor (int8) const;
	sbyte4  opXor_r (int8, const sbyte4 &);
	sbyte4 &  opShlAssign (const sbyte4 &);
	sbyte4 &  opShlAssign (int8);
	sbyte4  opShl (const sbyte4 &) const;
	sbyte4  opShl (int8) const;
	sbyte4  opShl_r (int8, const sbyte4 &);
	sbyte4 &  opShrAssign (const sbyte4 &);
	sbyte4 &  opShrAssign (int8);
	sbyte4  opShr (const sbyte4 &) const;
	sbyte4  opShr (int8) const;
	sbyte4  opShr_r (int8, const sbyte4 &);
	bool  opEquals (const sbyte4 &, const sbyte4 &);
	int  opCmp (const sbyte4 &, const sbyte4 &);
};

struct ubyte2
{
	ubyte2 ();
	ubyte2 (const ubyte2&);
	ubyte2&  operator = (const ubyte2&);
	uint8 x;
	uint8 y;
	ubyte2 (uint8 v);
	ubyte2 (const ubyte3 & v3);
	ubyte2 (const ubyte4 & v4);
	ubyte2 (uint8 x, uint8 y);
	ubyte2 (const short2 & v2);
	ubyte2 (const short3 & v3);
	ubyte2 (const short4 & v4);
	ubyte2 (const ushort2 & v2);
	ubyte2 (const ushort3 & v3);
	ubyte2 (const ushort4 & v4);
	ubyte2 (const int2 & v2);
	ubyte2 (const int3 & v3);
	ubyte2 (const int4 & v4);
	ubyte2 (const uint2 & v2);
	ubyte2 (const uint3 & v3);
	ubyte2 (const uint4 & v4);
	ubyte2 (const float2 & v2);
	ubyte2 (const float3 & v3);
	ubyte2 (const float4 & v4);
	ubyte2 &  opAddAssign (const ubyte2 &);
	ubyte2 &  opAddAssign (uint8);
	ubyte2  opAdd (const ubyte2 &) const;
	ubyte2  opAdd (uint8) const;
	ubyte2  opAdd_r (uint8, const ubyte2 &);
	ubyte2 &  opSubAssign (const ubyte2 &);
	ubyte2 &  opSubAssign (uint8);
	ubyte2  opSub (const ubyte2 &) const;
	ubyte2  opSub (uint8) const;
	ubyte2  opSub_r (uint8, const ubyte2 &);
	ubyte2 &  opMulAssign (const ubyte2 &);
	ubyte2 &  opMulAssign (uint8);
	ubyte2  opMul (const ubyte2 &) const;
	ubyte2  opMul (uint8) const;
	ubyte2  opMul_r (uint8, const ubyte2 &);
	ubyte2 &  opDivAssign (const ubyte2 &);
	ubyte2 &  opDivAssign (uint8);
	ubyte2  opDiv (const ubyte2 &) const;
	ubyte2  opDiv (uint8) const;
	ubyte2  opDiv_r (uint8, const ubyte2 &);
	ubyte2 &  opModAssign (const ubyte2 &);
	ubyte2 &  opModAssign (uint8);
	ubyte2  opMod (const ubyte2 &) const;
	ubyte2  opMod (uint8) const;
	ubyte2  opMod_r (uint8, const ubyte2 &);
	ubyte2 &  opAndAssign (const ubyte2 &);
	ubyte2 &  opAndAssign (uint8);
	ubyte2  opAnd (const ubyte2 &) const;
	ubyte2  opAnd (uint8) const;
	ubyte2  opAnd_r (uint8, const ubyte2 &);
	ubyte2 &  opOrAssign (const ubyte2 &);
	ubyte2 &  opOrAssign (uint8);
	ubyte2  opOr (const ubyte2 &) const;
	ubyte2  opOr (uint8) const;
	ubyte2  opOr_r (uint8, const ubyte2 &);
	ubyte2 &  opXorAssign (const ubyte2 &);
	ubyte2 &  opXorAssign (uint8);
	ubyte2  opXor (const ubyte2 &) const;
	ubyte2  opXor (uint8) const;
	ubyte2  opXor_r (uint8, const ubyte2 &);
	ubyte2 &  opShlAssign (const ubyte2 &);
	ubyte2 &  opShlAssign (uint8);
	ubyte2  opShl (const ubyte2 &) const;
	ubyte2  opShl (uint8) const;
	ubyte2  opShl_r (uint8, const ubyte2 &);
	ubyte2 &  opShrAssign (const ubyte2 &);
	ubyte2 &  opShrAssign (uint8);
	ubyte2  opShr (const ubyte2 &) const;
	ubyte2  opShr (uint8) const;
	ubyte2  opShr_r (uint8, const ubyte2 &);
	bool  opEquals (const ubyte2 &, const ubyte2 &);
	int  opCmp (const ubyte2 &, const ubyte2 &);
};

struct ubyte3
{
	ubyte3 ();
	ubyte3 (const ubyte3&);
	ubyte3&  operator = (const ubyte3&);
	uint8 x;
	uint8 y;
	uint8 z;
	ubyte3 (uint8 v);
	ubyte3 (const ubyte2 & v2);
	ubyte3 (const ubyte4 & v4);
	ubyte3 (const ubyte2 & xy, uint8 z);
	ubyte3 (uint8 x, uint8 y, uint8 z);
	ubyte3 (const short2 & v2);
	ubyte3 (const short3 & v3);
	ubyte3 (const short4 & v4);
	ubyte3 (const ushort2 & v2);
	ubyte3 (const ushort3 & v3);
	ubyte3 (const ushort4 & v4);
	ubyte3 (const int2 & v2);
	ubyte3 (const int3 & v3);
	ubyte3 (const int4 & v4);
	ubyte3 (const uint2 & v2);
	ubyte3 (const uint3 & v3);
	ubyte3 (const uint4 & v4);
	ubyte3 (const float2 & v2);
	ubyte3 (const float3 & v3);
	ubyte3 (const float4 & v4);
	ubyte3 &  opAddAssign (const ubyte3 &);
	ubyte3 &  opAddAssign (uint8);
	ubyte3  opAdd (const ubyte3 &) const;
	ubyte3  opAdd (uint8) const;
	ubyte3  opAdd_r (uint8, const ubyte3 &);
	ubyte3 &  opSubAssign (const ubyte3 &);
	ubyte3 &  opSubAssign (uint8);
	ubyte3  opSub (const ubyte3 &) const;
	ubyte3  opSub (uint8) const;
	ubyte3  opSub_r (uint8, const ubyte3 &);
	ubyte3 &  opMulAssign (const ubyte3 &);
	ubyte3 &  opMulAssign (uint8);
	ubyte3  opMul (const ubyte3 &) const;
	ubyte3  opMul (uint8) const;
	ubyte3  opMul_r (uint8, const ubyte3 &);
	ubyte3 &  opDivAssign (const ubyte3 &);
	ubyte3 &  opDivAssign (uint8);
	ubyte3  opDiv (const ubyte3 &) const;
	ubyte3  opDiv (uint8) const;
	ubyte3  opDiv_r (uint8, const ubyte3 &);
	ubyte3 &  opModAssign (const ubyte3 &);
	ubyte3 &  opModAssign (uint8);
	ubyte3  opMod (const ubyte3 &) const;
	ubyte3  opMod (uint8) const;
	ubyte3  opMod_r (uint8, const ubyte3 &);
	ubyte3 &  opAndAssign (const ubyte3 &);
	ubyte3 &  opAndAssign (uint8);
	ubyte3  opAnd (const ubyte3 &) const;
	ubyte3  opAnd (uint8) const;
	ubyte3  opAnd_r (uint8, const ubyte3 &);
	ubyte3 &  opOrAssign (const ubyte3 &);
	ubyte3 &  opOrAssign (uint8);
	ubyte3  opOr (const ubyte3 &) const;
	ubyte3  opOr (uint8) const;
	ubyte3  opOr_r (uint8, const ubyte3 &);
	ubyte3 &  opXorAssign (const ubyte3 &);
	ubyte3 &  opXorAssign (uint8);
	ubyte3  opXor (const ubyte3 &) const;
	ubyte3  opXor (uint8) const;
	ubyte3  opXor_r (uint8, const ubyte3 &);
	ubyte3 &  opShlAssign (const ubyte3 &);
	ubyte3 &  opShlAssign (uint8);
	ubyte3  opShl (const ubyte3 &) const;
	ubyte3  opShl (uint8) const;
	ubyte3  opShl_r (uint8, const ubyte3 &);
	ubyte3 &  opShrAssign (const ubyte3 &);
	ubyte3 &  opShrAssign (uint8);
	ubyte3  opShr (const ubyte3 &) const;
	ubyte3  opShr (uint8) const;
	ubyte3  opShr_r (uint8, const ubyte3 &);
	bool  opEquals (const ubyte3 &, const ubyte3 &);
	int  opCmp (const ubyte3 &, const ubyte3 &);
};

struct ubyte4
{
	ubyte4 ();
	ubyte4 (const ubyte4&);
	ubyte4&  operator = (const ubyte4&);
	uint8 x;
	uint8 y;
	uint8 z;
	uint8 w;
	ubyte4 (uint8 v);
	ubyte4 (const ubyte2 & v2);
	ubyte4 (const ubyte3 & v3);
	ubyte4 (const ubyte2 & xy, const ubyte2 & zw);
	ubyte4 (const ubyte3 & xyz, uint8 w);
	ubyte4 (uint8 x, uint8 y, uint8 z, uint8 w);
	ubyte4 (uint8 x, const ubyte3 & yzw);
	ubyte4 (const short2 & v2);
	ubyte4 (const short3 & v3);
	ubyte4 (const short4 & v4);
	ubyte4 (const ushort2 & v2);
	ubyte4 (const ushort3 & v3);
	ubyte4 (const ushort4 & v4);
	ubyte4 (const int2 & v2);
	ubyte4 (const int3 & v3);
	ubyte4 (const int4 & v4);
	ubyte4 (const uint2 & v2);
	ubyte4 (const uint3 & v3);
	ubyte4 (const uint4 & v4);
	ubyte4 (const float2 & v2);
	ubyte4 (const float3 & v3);
	ubyte4 (const float4 & v4);
	ubyte4 &  opAddAssign (const ubyte4 &);
	ubyte4 &  opAddAssign (uint8);
	ubyte4  opAdd (const ubyte4 &) const;
	ubyte4  opAdd (uint8) const;
	ubyte4  opAdd_r (uint8, const ubyte4 &);
	ubyte4 &  opSubAssign (const ubyte4 &);
	ubyte4 &  opSubAssign (uint8);
	ubyte4  opSub (const ubyte4 &) const;
	ubyte4  opSub (uint8) const;
	ubyte4  opSub_r (uint8, const ubyte4 &);
	ubyte4 &  opMulAssign (const ubyte4 &);
	ubyte4 &  opMulAssign (uint8);
	ubyte4  opMul (const ubyte4 &) const;
	ubyte4  opMul (uint8) const;
	ubyte4  opMul_r (uint8, const ubyte4 &);
	ubyte4 &  opDivAssign (const ubyte4 &);
	ubyte4 &  opDivAssign (uint8);
	ubyte4  opDiv (const ubyte4 &) const;
	ubyte4  opDiv (uint8) const;
	ubyte4  opDiv_r (uint8, const ubyte4 &);
	ubyte4 &  opModAssign (const ubyte4 &);
	ubyte4 &  opModAssign (uint8);
	ubyte4  opMod (const ubyte4 &) const;
	ubyte4  opMod (uint8) const;
	ubyte4  opMod_r (uint8, const ubyte4 &);
	ubyte4 &  opAndAssign (const ubyte4 &);
	ubyte4 &  opAndAssign (uint8);
	ubyte4  opAnd (const ubyte4 &) const;
	ubyte4  opAnd (uint8) const;
	ubyte4  opAnd_r (uint8, const ubyte4 &);
	ubyte4 &  opOrAssign (const ubyte4 &);
	ubyte4 &  opOrAssign (uint8);
	ubyte4  opOr (const ubyte4 &) const;
	ubyte4  opOr (uint8) const;
	ubyte4  opOr_r (uint8, const ubyte4 &);
	ubyte4 &  opXorAssign (const ubyte4 &);
	ubyte4 &  opXorAssign (uint8);
	ubyte4  opXor (const ubyte4 &) const;
	ubyte4  opXor (uint8) const;
	ubyte4  opXor_r (uint8, const ubyte4 &);
	ubyte4 &  opShlAssign (const ubyte4 &);
	ubyte4 &  opShlAssign (uint8);
	ubyte4  opShl (const ubyte4 &) const;
	ubyte4  opShl (uint8) const;
	ubyte4  opShl_r (uint8, const ubyte4 &);
	ubyte4 &  opShrAssign (const ubyte4 &);
	ubyte4 &  opShrAssign (uint8);
	ubyte4  opShr (const ubyte4 &) const;
	ubyte4  opShr (uint8) const;
	ubyte4  opShr_r (uint8, const ubyte4 &);
	bool  opEquals (const ubyte4 &, const ubyte4 &);
	int  opCmp (const ubyte4 &, const ubyte4 &);
};

struct short2
{
	short2 ();
	short2 (const short2&);
	short2&  operator = (const short2&);
	int16 x;
	int16 y;
	short2 (int16 v);
	short2 (const short3 & v3);
	short2 (const short4 & v4);
	short2 (int16 x, int16 y);
	short2 (const ushort2 & v2);
	short2 (const ushort3 & v3);
	short2 (const ushort4 & v4);
	short2 (const int2 & v2);
	short2 (const int3 & v3);
	short2 (const int4 & v4);
	short2 (const uint2 & v2);
	short2 (const uint3 & v3);
	short2 (const uint4 & v4);
	short2 (const float2 & v2);
	short2 (const float3 & v3);
	short2 (const float4 & v4);
	short2 &  opAddAssign (const short2 &);
	short2 &  opAddAssign (int16);
	short2  opAdd (const short2 &) const;
	short2  opAdd (int16) const;
	short2  opAdd_r (int16, const short2 &);
	short2 &  opSubAssign (const short2 &);
	short2 &  opSubAssign (int16);
	short2  opSub (const short2 &) const;
	short2  opSub (int16) const;
	short2  opSub_r (int16, const short2 &);
	short2 &  opMulAssign (const short2 &);
	short2 &  opMulAssign (int16);
	short2  opMul (const short2 &) const;
	short2  opMul (int16) const;
	short2  opMul_r (int16, const short2 &);
	short2 &  opDivAssign (const short2 &);
	short2 &  opDivAssign (int16);
	short2  opDiv (const short2 &) const;
	short2  opDiv (int16) const;
	short2  opDiv_r (int16, const short2 &);
	short2 &  opModAssign (const short2 &);
	short2 &  opModAssign (int16);
	short2  opMod (const short2 &) const;
	short2  opMod (int16) const;
	short2  opMod_r (int16, const short2 &);
	short2 &  opAndAssign (const short2 &);
	short2 &  opAndAssign (int16);
	short2  opAnd (const short2 &) const;
	short2  opAnd (int16) const;
	short2  opAnd_r (int16, const short2 &);
	short2 &  opOrAssign (const short2 &);
	short2 &  opOrAssign (int16);
	short2  opOr (const short2 &) const;
	short2  opOr (int16) const;
	short2  opOr_r (int16, const short2 &);
	short2 &  opXorAssign (const short2 &);
	short2 &  opXorAssign (int16);
	short2  opXor (const short2 &) const;
	short2  opXor (int16) const;
	short2  opXor_r (int16, const short2 &);
	short2 &  opShlAssign (const short2 &);
	short2 &  opShlAssign (int16);
	short2  opShl (const short2 &) const;
	short2  opShl (int16) const;
	short2  opShl_r (int16, const short2 &);
	short2 &  opShrAssign (const short2 &);
	short2 &  opShrAssign (int16);
	short2  opShr (const short2 &) const;
	short2  opShr (int16) const;
	short2  opShr_r (int16, const short2 &);
	bool  opEquals (const short2 &, const short2 &);
	int  opCmp (const short2 &, const short2 &);
};

struct short3
{
	short3 ();
	short3 (const short3&);
	short3&  operator = (const short3&);
	int16 x;
	int16 y;
	int16 z;
	short3 (int16 v);
	short3 (const short2 & v2);
	short3 (const short4 & v4);
	short3 (const short2 & xy, int16 z);
	short3 (int16 x, int16 y, int16 z);
	short3 (const ushort2 & v2);
	short3 (const ushort3 & v3);
	short3 (const ushort4 & v4);
	short3 (const int2 & v2);
	short3 (const int3 & v3);
	short3 (const int4 & v4);
	short3 (const uint2 & v2);
	short3 (const uint3 & v3);
	short3 (const uint4 & v4);
	short3 (const float2 & v2);
	short3 (const float3 & v3);
	short3 (const float4 & v4);
	short3 &  opAddAssign (const short3 &);
	short3 &  opAddAssign (int16);
	short3  opAdd (const short3 &) const;
	short3  opAdd (int16) const;
	short3  opAdd_r (int16, const short3 &);
	short3 &  opSubAssign (const short3 &);
	short3 &  opSubAssign (int16);
	short3  opSub (const short3 &) const;
	short3  opSub (int16) const;
	short3  opSub_r (int16, const short3 &);
	short3 &  opMulAssign (const short3 &);
	short3 &  opMulAssign (int16);
	short3  opMul (const short3 &) const;
	short3  opMul (int16) const;
	short3  opMul_r (int16, const short3 &);
	short3 &  opDivAssign (const short3 &);
	short3 &  opDivAssign (int16);
	short3  opDiv (const short3 &) const;
	short3  opDiv (int16) const;
	short3  opDiv_r (int16, const short3 &);
	short3 &  opModAssign (const short3 &);
	short3 &  opModAssign (int16);
	short3  opMod (const short3 &) const;
	short3  opMod (int16) const;
	short3  opMod_r (int16, const short3 &);
	short3 &  opAndAssign (const short3 &);
	short3 &  opAndAssign (int16);
	short3  opAnd (const short3 &) const;
	short3  opAnd (int16) const;
	short3  opAnd_r (int16, const short3 &);
	short3 &  opOrAssign (const short3 &);
	short3 &  opOrAssign (int16);
	short3  opOr (const short3 &) const;
	short3  opOr (int16) const;
	short3  opOr_r (int16, const short3 &);
	short3 &  opXorAssign (const short3 &);
	short3 &  opXorAssign (int16);
	short3  opXor (const short3 &) const;
	short3  opXor (int16) const;
	short3  opXor_r (int16, const short3 &);
	short3 &  opShlAssign (const short3 &);
	short3 &  opShlAssign (int16);
	short3  opShl (const short3 &) const;
	short3  opShl (int16) const;
	short3  opShl_r (int16, const short3 &);
	short3 &  opShrAssign (const short3 &);
	short3 &  opShrAssign (int16);
	short3  opShr (const short3 &) const;
	short3  opShr (int16) const;
	short3  opShr_r (int16, const short3 &);
	bool  opEquals (const short3 &, const short3 &);
	int  opCmp (const short3 &, const short3 &);
};

struct short4
{
	short4 ();
	short4 (const short4&);
	short4&  operator = (const short4&);
	int16 x;
	int16 y;
	int16 z;
	int16 w;
	short4 (int16 v);
	short4 (const short2 & v2);
	short4 (const short3 & v3);
	short4 (const short2 & xy, const short2 & zw);
	short4 (const short3 & xyz, int16 w);
	short4 (int16 x, int16 y, int16 z, int16 w);
	short4 (int16 x, const short3 & yzw);
	short4 (const ushort2 & v2);
	short4 (const ushort3 & v3);
	short4 (const ushort4 & v4);
	short4 (const int2 & v2);
	short4 (const int3 & v3);
	short4 (const int4 & v4);
	short4 (const uint2 & v2);
	short4 (const uint3 & v3);
	short4 (const uint4 & v4);
	short4 (const float2 & v2);
	short4 (const float3 & v3);
	short4 (const float4 & v4);
	short4 &  opAddAssign (const short4 &);
	short4 &  opAddAssign (int16);
	short4  opAdd (const short4 &) const;
	short4  opAdd (int16) const;
	short4  opAdd_r (int16, const short4 &);
	short4 &  opSubAssign (const short4 &);
	short4 &  opSubAssign (int16);
	short4  opSub (const short4 &) const;
	short4  opSub (int16) const;
	short4  opSub_r (int16, const short4 &);
	short4 &  opMulAssign (const short4 &);
	short4 &  opMulAssign (int16);
	short4  opMul (const short4 &) const;
	short4  opMul (int16) const;
	short4  opMul_r (int16, const short4 &);
	short4 &  opDivAssign (const short4 &);
	short4 &  opDivAssign (int16);
	short4  opDiv (const short4 &) const;
	short4  opDiv (int16) const;
	short4  opDiv_r (int16, const short4 &);
	short4 &  opModAssign (const short4 &);
	short4 &  opModAssign (int16);
	short4  opMod (const short4 &) const;
	short4  opMod (int16) const;
	short4  opMod_r (int16, const short4 &);
	short4 &  opAndAssign (const short4 &);
	short4 &  opAndAssign (int16);
	short4  opAnd (const short4 &) const;
	short4  opAnd (int16) const;
	short4  opAnd_r (int16, const short4 &);
	short4 &  opOrAssign (const short4 &);
	short4 &  opOrAssign (int16);
	short4  opOr (const short4 &) const;
	short4  opOr (int16) const;
	short4  opOr_r (int16, const short4 &);
	short4 &  opXorAssign (const short4 &);
	short4 &  opXorAssign (int16);
	short4  opXor (const short4 &) const;
	short4  opXor (int16) const;
	short4  opXor_r (int16, const short4 &);
	short4 &  opShlAssign (const short4 &);
	short4 &  opShlAssign (int16);
	short4  opShl (const short4 &) const;
	short4  opShl (int16) const;
	short4  opShl_r (int16, const short4 &);
	short4 &  opShrAssign (const short4 &);
	short4 &  opShrAssign (int16);
	short4  opShr (const short4 &) const;
	short4  opShr (int16) const;
	short4  opShr_r (int16, const short4 &);
	bool  opEquals (const short4 &, const short4 &);
	int  opCmp (const short4 &, const short4 &);
};

struct ushort2
{
	ushort2 ();
	ushort2 (const ushort2&);
	ushort2&  operator = (const ushort2&);
	uint16 x;
	uint16 y;
	ushort2 (uint16 v);
	ushort2 (const ushort3 & v3);
	ushort2 (const ushort4 & v4);
	ushort2 (uint16 x, uint16 y);
	ushort2 (const short2 & v2);
	ushort2 (const short3 & v3);
	ushort2 (const short4 & v4);
	ushort2 (const int2 & v2);
	ushort2 (const int3 & v3);
	ushort2 (const int4 & v4);
	ushort2 (const uint2 & v2);
	ushort2 (const uint3 & v3);
	ushort2 (const uint4 & v4);
	ushort2 (const float2 & v2);
	ushort2 (const float3 & v3);
	ushort2 (const float4 & v4);
	ushort2 &  opAddAssign (const ushort2 &);
	ushort2 &  opAddAssign (uint16);
	ushort2  opAdd (const ushort2 &) const;
	ushort2  opAdd (uint16) const;
	ushort2  opAdd_r (uint16, const ushort2 &);
	ushort2 &  opSubAssign (const ushort2 &);
	ushort2 &  opSubAssign (uint16);
	ushort2  opSub (const ushort2 &) const;
	ushort2  opSub (uint16) const;
	ushort2  opSub_r (uint16, const ushort2 &);
	ushort2 &  opMulAssign (const ushort2 &);
	ushort2 &  opMulAssign (uint16);
	ushort2  opMul (const ushort2 &) const;
	ushort2  opMul (uint16) const;
	ushort2  opMul_r (uint16, const ushort2 &);
	ushort2 &  opDivAssign (const ushort2 &);
	ushort2 &  opDivAssign (uint16);
	ushort2  opDiv (const ushort2 &) const;
	ushort2  opDiv (uint16) const;
	ushort2  opDiv_r (uint16, const ushort2 &);
	ushort2 &  opModAssign (const ushort2 &);
	ushort2 &  opModAssign (uint16);
	ushort2  opMod (const ushort2 &) const;
	ushort2  opMod (uint16) const;
	ushort2  opMod_r (uint16, const ushort2 &);
	ushort2 &  opAndAssign (const ushort2 &);
	ushort2 &  opAndAssign (uint16);
	ushort2  opAnd (const ushort2 &) const;
	ushort2  opAnd (uint16) const;
	ushort2  opAnd_r (uint16, const ushort2 &);
	ushort2 &  opOrAssign (const ushort2 &);
	ushort2 &  opOrAssign (uint16);
	ushort2  opOr (const ushort2 &) const;
	ushort2  opOr (uint16) const;
	ushort2  opOr_r (uint16, const ushort2 &);
	ushort2 &  opXorAssign (const ushort2 &);
	ushort2 &  opXorAssign (uint16);
	ushort2  opXor (const ushort2 &) const;
	ushort2  opXor (uint16) const;
	ushort2  opXor_r (uint16, const ushort2 &);
	ushort2 &  opShlAssign (const ushort2 &);
	ushort2 &  opShlAssign (uint16);
	ushort2  opShl (const ushort2 &) const;
	ushort2  opShl (uint16) const;
	ushort2  opShl_r (uint16, const ushort2 &);
	ushort2 &  opShrAssign (const ushort2 &);
	ushort2 &  opShrAssign (uint16);
	ushort2  opShr (const ushort2 &) const;
	ushort2  opShr (uint16) const;
	ushort2  opShr_r (uint16, const ushort2 &);
	bool  opEquals (const ushort2 &, const ushort2 &);
	int  opCmp (const ushort2 &, const ushort2 &);
};

struct ushort3
{
	ushort3 ();
	ushort3 (const ushort3&);
	ushort3&  operator = (const ushort3&);
	uint16 x;
	uint16 y;
	uint16 z;
	ushort3 (uint16 v);
	ushort3 (const ushort2 & v2);
	ushort3 (const ushort4 & v4);
	ushort3 (const ushort2 & xy, uint16 z);
	ushort3 (uint16 x, uint16 y, uint16 z);
	ushort3 (const short2 & v2);
	ushort3 (const short3 & v3);
	ushort3 (const short4 & v4);
	ushort3 (const int2 & v2);
	ushort3 (const int3 & v3);
	ushort3 (const int4 & v4);
	ushort3 (const uint2 & v2);
	ushort3 (const uint3 & v3);
	ushort3 (const uint4 & v4);
	ushort3 (const float2 & v2);
	ushort3 (const float3 & v3);
	ushort3 (const float4 & v4);
	ushort3 &  opAddAssign (const ushort3 &);
	ushort3 &  opAddAssign (uint16);
	ushort3  opAdd (const ushort3 &) const;
	ushort3  opAdd (uint16) const;
	ushort3  opAdd_r (uint16, const ushort3 &);
	ushort3 &  opSubAssign (const ushort3 &);
	ushort3 &  opSubAssign (uint16);
	ushort3  opSub (const ushort3 &) const;
	ushort3  opSub (uint16) const;
	ushort3  opSub_r (uint16, const ushort3 &);
	ushort3 &  opMulAssign (const ushort3 &);
	ushort3 &  opMulAssign (uint16);
	ushort3  opMul (const ushort3 &) const;
	ushort3  opMul (uint16) const;
	ushort3  opMul_r (uint16, const ushort3 &);
	ushort3 &  opDivAssign (const ushort3 &);
	ushort3 &  opDivAssign (uint16);
	ushort3  opDiv (const ushort3 &) const;
	ushort3  opDiv (uint16) const;
	ushort3  opDiv_r (uint16, const ushort3 &);
	ushort3 &  opModAssign (const ushort3 &);
	ushort3 &  opModAssign (uint16);
	ushort3  opMod (const ushort3 &) const;
	ushort3  opMod (uint16) const;
	ushort3  opMod_r (uint16, const ushort3 &);
	ushort3 &  opAndAssign (const ushort3 &);
	ushort3 &  opAndAssign (uint16);
	ushort3  opAnd (const ushort3 &) const;
	ushort3  opAnd (uint16) const;
	ushort3  opAnd_r (uint16, const ushort3 &);
	ushort3 &  opOrAssign (const ushort3 &);
	ushort3 &  opOrAssign (uint16);
	ushort3  opOr (const ushort3 &) const;
	ushort3  opOr (uint16) const;
	ushort3  opOr_r (uint16, const ushort3 &);
	ushort3 &  opXorAssign (const ushort3 &);
	ushort3 &  opXorAssign (uint16);
	ushort3  opXor (const ushort3 &) const;
	ushort3  opXor (uint16) const;
	ushort3  opXor_r (uint16, const ushort3 &);
	ushort3 &  opShlAssign (const ushort3 &);
	ushort3 &  opShlAssign (uint16);
	ushort3  opShl (const ushort3 &) const;
	ushort3  opShl (uint16) const;
	ushort3  opShl_r (uint16, const ushort3 &);
	ushort3 &  opShrAssign (const ushort3 &);
	ushort3 &  opShrAssign (uint16);
	ushort3  opShr (const ushort3 &) const;
	ushort3  opShr (uint16) const;
	ushort3  opShr_r (uint16, const ushort3 &);
	bool  opEquals (const ushort3 &, const ushort3 &);
	int  opCmp (const ushort3 &, const ushort3 &);
};

struct ushort4
{
	ushort4 ();
	ushort4 (const ushort4&);
	ushort4&  operator = (const ushort4&);
	uint16 x;
	uint16 y;
	uint16 z;
	uint16 w;
	ushort4 (uint16 v);
	ushort4 (const ushort2 & v2);
	ushort4 (const ushort3 & v3);
	ushort4 (const ushort2 & xy, const ushort2 & zw);
	ushort4 (const ushort3 & xyz, uint16 w);
	ushort4 (uint16 x, uint16 y, uint16 z, uint16 w);
	ushort4 (uint16 x, const ushort3 & yzw);
	ushort4 (const short2 & v2);
	ushort4 (const short3 & v3);
	ushort4 (const short4 & v4);
	ushort4 (const int2 & v2);
	ushort4 (const int3 & v3);
	ushort4 (const int4 & v4);
	ushort4 (const uint2 & v2);
	ushort4 (const uint3 & v3);
	ushort4 (const uint4 & v4);
	ushort4 (const float2 & v2);
	ushort4 (const float3 & v3);
	ushort4 (const float4 & v4);
	ushort4 &  opAddAssign (const ushort4 &);
	ushort4 &  opAddAssign (uint16);
	ushort4  opAdd (const ushort4 &) const;
	ushort4  opAdd (uint16) const;
	ushort4  opAdd_r (uint16, const ushort4 &);
	ushort4 &  opSubAssign (const ushort4 &);
	ushort4 &  opSubAssign (uint16);
	ushort4  opSub (const ushort4 &) const;
	ushort4  opSub (uint16) const;
	ushort4  opSub_r (uint16, const ushort4 &);
	ushort4 &  opMulAssign (const ushort4 &);
	ushort4 &  opMulAssign (uint16);
	ushort4  opMul (const ushort4 &) const;
	ushort4  opMul (uint16) const;
	ushort4  opMul_r (uint16, const ushort4 &);
	ushort4 &  opDivAssign (const ushort4 &);
	ushort4 &  opDivAssign (uint16);
	ushort4  opDiv (const ushort4 &) const;
	ushort4  opDiv (uint16) const;
	ushort4  opDiv_r (uint16, const ushort4 &);
	ushort4 &  opModAssign (const ushort4 &);
	ushort4 &  opModAssign (uint16);
	ushort4  opMod (const ushort4 &) const;
	ushort4  opMod (uint16) const;
	ushort4  opMod_r (uint16, const ushort4 &);
	ushort4 &  opAndAssign (const ushort4 &);
	ushort4 &  opAndAssign (uint16);
	ushort4  opAnd (const ushort4 &) const;
	ushort4  opAnd (uint16) const;
	ushort4  opAnd_r (uint16, const ushort4 &);
	ushort4 &  opOrAssign (const ushort4 &);
	ushort4 &  opOrAssign (uint16);
	ushort4  opOr (const ushort4 &) const;
	ushort4  opOr (uint16) const;
	ushort4  opOr_r (uint16, const ushort4 &);
	ushort4 &  opXorAssign (const ushort4 &);
	ushort4 &  opXorAssign (uint16);
	ushort4  opXor (const ushort4 &) const;
	ushort4  opXor (uint16) const;
	ushort4  opXor_r (uint16, const ushort4 &);
	ushort4 &  opShlAssign (const ushort4 &);
	ushort4 &  opShlAssign (uint16);
	ushort4  opShl (const ushort4 &) const;
	ushort4  opShl (uint16) const;
	ushort4  opShl_r (uint16, const ushort4 &);
	ushort4 &  opShrAssign (const ushort4 &);
	ushort4 &  opShrAssign (uint16);
	ushort4  opShr (const ushort4 &) const;
	ushort4  opShr (uint16) const;
	ushort4  opShr_r (uint16, const ushort4 &);
	bool  opEquals (const ushort4 &, const ushort4 &);
	int  opCmp (const ushort4 &, const ushort4 &);
};

struct int2
{
	int2 ();
	int2 (const int2&);
	int2&  operator = (const int2&);
	int x;
	int y;
	int2 (int v);
	int2 (const int3 & v3);
	int2 (const int4 & v4);
	int2 (int x, int y);
	int2 (const short2 & v2);
	int2 (const short3 & v3);
	int2 (const short4 & v4);
	int2 (const ushort2 & v2);
	int2 (const ushort3 & v3);
	int2 (const ushort4 & v4);
	int2 (const uint2 & v2);
	int2 (const uint3 & v3);
	int2 (const uint4 & v4);
	int2 (const float2 & v2);
	int2 (const float3 & v3);
	int2 (const float4 & v4);
	int2 &  opAddAssign (const int2 &);
	int2 &  opAddAssign (int);
	int2  opAdd (const int2 &) const;
	int2  opAdd (int) const;
	int2  opAdd_r (int, const int2 &);
	int2 &  opSubAssign (const int2 &);
	int2 &  opSubAssign (int);
	int2  opSub (const int2 &) const;
	int2  opSub (int) const;
	int2  opSub_r (int, const int2 &);
	int2 &  opMulAssign (const int2 &);
	int2 &  opMulAssign (int);
	int2  opMul (const int2 &) const;
	int2  opMul (int) const;
	int2  opMul_r (int, const int2 &);
	int2 &  opDivAssign (const int2 &);
	int2 &  opDivAssign (int);
	int2  opDiv (const int2 &) const;
	int2  opDiv (int) const;
	int2  opDiv_r (int, const int2 &);
	int2 &  opModAssign (const int2 &);
	int2 &  opModAssign (int);
	int2  opMod (const int2 &) const;
	int2  opMod (int) const;
	int2  opMod_r (int, const int2 &);
	int2 &  opAndAssign (const int2 &);
	int2 &  opAndAssign (int);
	int2  opAnd (const int2 &) const;
	int2  opAnd (int) const;
	int2  opAnd_r (int, const int2 &);
	int2 &  opOrAssign (const int2 &);
	int2 &  opOrAssign (int);
	int2  opOr (const int2 &) const;
	int2  opOr (int) const;
	int2  opOr_r (int, const int2 &);
	int2 &  opXorAssign (const int2 &);
	int2 &  opXorAssign (int);
	int2  opXor (const int2 &) const;
	int2  opXor (int) const;
	int2  opXor_r (int, const int2 &);
	int2 &  opShlAssign (const int2 &);
	int2 &  opShlAssign (int);
	int2  opShl (const int2 &) const;
	int2  opShl (int) const;
	int2  opShl_r (int, const int2 &);
	int2 &  opShrAssign (const int2 &);
	int2 &  opShrAssign (int);
	int2  opShr (const int2 &) const;
	int2  opShr (int) const;
	int2  opShr_r (int, const int2 &);
	bool  opEquals (const int2 &, const int2 &);
	int  opCmp (const int2 &, const int2 &);
	int2  opNeg () const;
};

struct int3
{
	int3 ();
	int3 (const int3&);
	int3&  operator = (const int3&);
	int x;
	int y;
	int z;
	int3 (int v);
	int3 (const int2 & v2);
	int3 (const int4 & v4);
	int3 (const int2 & xy, int z);
	int3 (int x, int y, int z);
	int3 (const short2 & v2);
	int3 (const short3 & v3);
	int3 (const short4 & v4);
	int3 (const ushort2 & v2);
	int3 (const ushort3 & v3);
	int3 (const ushort4 & v4);
	int3 (const uint2 & v2);
	int3 (const uint3 & v3);
	int3 (const uint4 & v4);
	int3 (const float2 & v2);
	int3 (const float3 & v3);
	int3 (const float4 & v4);
	int3 &  opAddAssign (const int3 &);
	int3 &  opAddAssign (int);
	int3  opAdd (const int3 &) const;
	int3  opAdd (int) const;
	int3  opAdd_r (int, const int3 &);
	int3 &  opSubAssign (const int3 &);
	int3 &  opSubAssign (int);
	int3  opSub (const int3 &) const;
	int3  opSub (int) const;
	int3  opSub_r (int, const int3 &);
	int3 &  opMulAssign (const int3 &);
	int3 &  opMulAssign (int);
	int3  opMul (const int3 &) const;
	int3  opMul (int) const;
	int3  opMul_r (int, const int3 &);
	int3 &  opDivAssign (const int3 &);
	int3 &  opDivAssign (int);
	int3  opDiv (const int3 &) const;
	int3  opDiv (int) const;
	int3  opDiv_r (int, const int3 &);
	int3 &  opModAssign (const int3 &);
	int3 &  opModAssign (int);
	int3  opMod (const int3 &) const;
	int3  opMod (int) const;
	int3  opMod_r (int, const int3 &);
	int3 &  opAndAssign (const int3 &);
	int3 &  opAndAssign (int);
	int3  opAnd (const int3 &) const;
	int3  opAnd (int) const;
	int3  opAnd_r (int, const int3 &);
	int3 &  opOrAssign (const int3 &);
	int3 &  opOrAssign (int);
	int3  opOr (const int3 &) const;
	int3  opOr (int) const;
	int3  opOr_r (int, const int3 &);
	int3 &  opXorAssign (const int3 &);
	int3 &  opXorAssign (int);
	int3  opXor (const int3 &) const;
	int3  opXor (int) const;
	int3  opXor_r (int, const int3 &);
	int3 &  opShlAssign (const int3 &);
	int3 &  opShlAssign (int);
	int3  opShl (const int3 &) const;
	int3  opShl (int) const;
	int3  opShl_r (int, const int3 &);
	int3 &  opShrAssign (const int3 &);
	int3 &  opShrAssign (int);
	int3  opShr (const int3 &) const;
	int3  opShr (int) const;
	int3  opShr_r (int, const int3 &);
	bool  opEquals (const int3 &, const int3 &);
	int  opCmp (const int3 &, const int3 &);
	int3  opNeg () const;
};

struct int4
{
	int4 ();
	int4 (const int4&);
	int4&  operator = (const int4&);
	int x;
	int y;
	int z;
	int w;
	int4 (int v);
	int4 (const int2 & v2);
	int4 (const int3 & v3);
	int4 (const int2 & xy, const int2 & zw);
	int4 (const int3 & xyz, int w);
	int4 (int x, int y, int z, int w);
	int4 (int x, const int3 & yzw);
	int4 (const short2 & v2);
	int4 (const short3 & v3);
	int4 (const short4 & v4);
	int4 (const ushort2 & v2);
	int4 (const ushort3 & v3);
	int4 (const ushort4 & v4);
	int4 (const uint2 & v2);
	int4 (const uint3 & v3);
	int4 (const uint4 & v4);
	int4 (const float2 & v2);
	int4 (const float3 & v3);
	int4 (const float4 & v4);
	int4 &  opAddAssign (const int4 &);
	int4 &  opAddAssign (int);
	int4  opAdd (const int4 &) const;
	int4  opAdd (int) const;
	int4  opAdd_r (int, const int4 &);
	int4 &  opSubAssign (const int4 &);
	int4 &  opSubAssign (int);
	int4  opSub (const int4 &) const;
	int4  opSub (int) const;
	int4  opSub_r (int, const int4 &);
	int4 &  opMulAssign (const int4 &);
	int4 &  opMulAssign (int);
	int4  opMul (const int4 &) const;
	int4  opMul (int) const;
	int4  opMul_r (int, const int4 &);
	int4 &  opDivAssign (const int4 &);
	int4 &  opDivAssign (int);
	int4  opDiv (const int4 &) const;
	int4  opDiv (int) const;
	int4  opDiv_r (int, const int4 &);
	int4 &  opModAssign (const int4 &);
	int4 &  opModAssign (int);
	int4  opMod (const int4 &) const;
	int4  opMod (int) const;
	int4  opMod_r (int, const int4 &);
	int4 &  opAndAssign (const int4 &);
	int4 &  opAndAssign (int);
	int4  opAnd (const int4 &) const;
	int4  opAnd (int) const;
	int4  opAnd_r (int, const int4 &);
	int4 &  opOrAssign (const int4 &);
	int4 &  opOrAssign (int);
	int4  opOr (const int4 &) const;
	int4  opOr (int) const;
	int4  opOr_r (int, const int4 &);
	int4 &  opXorAssign (const int4 &);
	int4 &  opXorAssign (int);
	int4  opXor (const int4 &) const;
	int4  opXor (int) const;
	int4  opXor_r (int, const int4 &);
	int4 &  opShlAssign (const int4 &);
	int4 &  opShlAssign (int);
	int4  opShl (const int4 &) const;
	int4  opShl (int) const;
	int4  opShl_r (int, const int4 &);
	int4 &  opShrAssign (const int4 &);
	int4 &  opShrAssign (int);
	int4  opShr (const int4 &) const;
	int4  opShr (int) const;
	int4  opShr_r (int, const int4 &);
	bool  opEquals (const int4 &, const int4 &);
	int  opCmp (const int4 &, const int4 &);
	int4  opNeg () const;
};

struct uint2
{
	uint2 ();
	uint2 (const uint2&);
	uint2&  operator = (const uint2&);
	uint x;
	uint y;
	uint2 (uint v);
	uint2 (const uint3 & v3);
	uint2 (const uint4 & v4);
	uint2 (uint x, uint y);
	uint2 (const short2 & v2);
	uint2 (const short3 & v3);
	uint2 (const short4 & v4);
	uint2 (const ushort2 & v2);
	uint2 (const ushort3 & v3);
	uint2 (const ushort4 & v4);
	uint2 (const int2 & v2);
	uint2 (const int3 & v3);
	uint2 (const int4 & v4);
	uint2 (const float2 & v2);
	uint2 (const float3 & v3);
	uint2 (const float4 & v4);
	uint2 &  opAddAssign (const uint2 &);
	uint2 &  opAddAssign (uint);
	uint2  opAdd (const uint2 &) const;
	uint2  opAdd (uint) const;
	uint2  opAdd_r (uint, const uint2 &);
	uint2 &  opSubAssign (const uint2 &);
	uint2 &  opSubAssign (uint);
	uint2  opSub (const uint2 &) const;
	uint2  opSub (uint) const;
	uint2  opSub_r (uint, const uint2 &);
	uint2 &  opMulAssign (const uint2 &);
	uint2 &  opMulAssign (uint);
	uint2  opMul (const uint2 &) const;
	uint2  opMul (uint) const;
	uint2  opMul_r (uint, const uint2 &);
	uint2 &  opDivAssign (const uint2 &);
	uint2 &  opDivAssign (uint);
	uint2  opDiv (const uint2 &) const;
	uint2  opDiv (uint) const;
	uint2  opDiv_r (uint, const uint2 &);
	uint2 &  opModAssign (const uint2 &);
	uint2 &  opModAssign (uint);
	uint2  opMod (const uint2 &) const;
	uint2  opMod (uint) const;
	uint2  opMod_r (uint, const uint2 &);
	uint2 &  opAndAssign (const uint2 &);
	uint2 &  opAndAssign (uint);
	uint2  opAnd (const uint2 &) const;
	uint2  opAnd (uint) const;
	uint2  opAnd_r (uint, const uint2 &);
	uint2 &  opOrAssign (const uint2 &);
	uint2 &  opOrAssign (uint);
	uint2  opOr (const uint2 &) const;
	uint2  opOr (uint) const;
	uint2  opOr_r (uint, const uint2 &);
	uint2 &  opXorAssign (const uint2 &);
	uint2 &  opXorAssign (uint);
	uint2  opXor (const uint2 &) const;
	uint2  opXor (uint) const;
	uint2  opXor_r (uint, const uint2 &);
	uint2 &  opShlAssign (const uint2 &);
	uint2 &  opShlAssign (uint);
	uint2  opShl (const uint2 &) const;
	uint2  opShl (uint) const;
	uint2  opShl_r (uint, const uint2 &);
	uint2 &  opShrAssign (const uint2 &);
	uint2 &  opShrAssign (uint);
	uint2  opShr (const uint2 &) const;
	uint2  opShr (uint) const;
	uint2  opShr_r (uint, const uint2 &);
	bool  opEquals (const uint2 &, const uint2 &);
	int  opCmp (const uint2 &, const uint2 &);
};

struct uint3
{
	uint3 ();
	uint3 (const uint3&);
	uint3&  operator = (const uint3&);
	uint x;
	uint y;
	uint z;
	uint3 (uint v);
	uint3 (const uint2 & v2);
	uint3 (const uint4 & v4);
	uint3 (const uint2 & xy, uint z);
	uint3 (uint x, uint y, uint z);
	uint3 (const short2 & v2);
	uint3 (const short3 & v3);
	uint3 (const short4 & v4);
	uint3 (const ushort2 & v2);
	uint3 (const ushort3 & v3);
	uint3 (const ushort4 & v4);
	uint3 (const int2 & v2);
	uint3 (const int3 & v3);
	uint3 (const int4 & v4);
	uint3 (const float2 & v2);
	uint3 (const float3 & v3);
	uint3 (const float4 & v4);
	uint3 &  opAddAssign (const uint3 &);
	uint3 &  opAddAssign (uint);
	uint3  opAdd (const uint3 &) const;
	uint3  opAdd (uint) const;
	uint3  opAdd_r (uint, const uint3 &);
	uint3 &  opSubAssign (const uint3 &);
	uint3 &  opSubAssign (uint);
	uint3  opSub (const uint3 &) const;
	uint3  opSub (uint) const;
	uint3  opSub_r (uint, const uint3 &);
	uint3 &  opMulAssign (const uint3 &);
	uint3 &  opMulAssign (uint);
	uint3  opMul (const uint3 &) const;
	uint3  opMul (uint) const;
	uint3  opMul_r (uint, const uint3 &);
	uint3 &  opDivAssign (const uint3 &);
	uint3 &  opDivAssign (uint);
	uint3  opDiv (const uint3 &) const;
	uint3  opDiv (uint) const;
	uint3  opDiv_r (uint, const uint3 &);
	uint3 &  opModAssign (const uint3 &);
	uint3 &  opModAssign (uint);
	uint3  opMod (const uint3 &) const;
	uint3  opMod (uint) const;
	uint3  opMod_r (uint, const uint3 &);
	uint3 &  opAndAssign (const uint3 &);
	uint3 &  opAndAssign (uint);
	uint3  opAnd (const uint3 &) const;
	uint3  opAnd (uint) const;
	uint3  opAnd_r (uint, const uint3 &);
	uint3 &  opOrAssign (const uint3 &);
	uint3 &  opOrAssign (uint);
	uint3  opOr (const uint3 &) const;
	uint3  opOr (uint) const;
	uint3  opOr_r (uint, const uint3 &);
	uint3 &  opXorAssign (const uint3 &);
	uint3 &  opXorAssign (uint);
	uint3  opXor (const uint3 &) const;
	uint3  opXor (uint) const;
	uint3  opXor_r (uint, const uint3 &);
	uint3 &  opShlAssign (const uint3 &);
	uint3 &  opShlAssign (uint);
	uint3  opShl (const uint3 &) const;
	uint3  opShl (uint) const;
	uint3  opShl_r (uint, const uint3 &);
	uint3 &  opShrAssign (const uint3 &);
	uint3 &  opShrAssign (uint);
	uint3  opShr (const uint3 &) const;
	uint3  opShr (uint) const;
	uint3  opShr_r (uint, const uint3 &);
	bool  opEquals (const uint3 &, const uint3 &);
	int  opCmp (const uint3 &, const uint3 &);
};

struct uint4
{
	uint4 ();
	uint4 (const uint4&);
	uint4&  operator = (const uint4&);
	uint x;
	uint y;
	uint z;
	uint w;
	uint4 (uint v);
	uint4 (const uint2 & v2);
	uint4 (const uint3 & v3);
	uint4 (const uint2 & xy, const uint2 & zw);
	uint4 (const uint3 & xyz, uint w);
	uint4 (uint x, uint y, uint z, uint w);
	uint4 (uint x, const uint3 & yzw);
	uint4 (const short2 & v2);
	uint4 (const short3 & v3);
	uint4 (const short4 & v4);
	uint4 (const ushort2 & v2);
	uint4 (const ushort3 & v3);
	uint4 (const ushort4 & v4);
	uint4 (const int2 & v2);
	uint4 (const int3 & v3);
	uint4 (const int4 & v4);
	uint4 (const float2 & v2);
	uint4 (const float3 & v3);
	uint4 (const float4 & v4);
	uint4 &  opAddAssign (const uint4 &);
	uint4 &  opAddAssign (uint);
	uint4  opAdd (const uint4 &) const;
	uint4  opAdd (uint) const;
	uint4  opAdd_r (uint, const uint4 &);
	uint4 &  opSubAssign (const uint4 &);
	uint4 &  opSubAssign (uint);
	uint4  opSub (const uint4 &) const;
	uint4  opSub (uint) const;
	uint4  opSub_r (uint, const uint4 &);
	uint4 &  opMulAssign (const uint4 &);
	uint4 &  opMulAssign (uint);
	uint4  opMul (const uint4 &) const;
	uint4  opMul (uint) const;
	uint4  opMul_r (uint, const uint4 &);
	uint4 &  opDivAssign (const uint4 &);
	uint4 &  opDivAssign (uint);
	uint4  opDiv (const uint4 &) const;
	uint4  opDiv (uint) const;
	uint4  opDiv_r (uint, const uint4 &);
	uint4 &  opModAssign (const uint4 &);
	uint4 &  opModAssign (uint);
	uint4  opMod (const uint4 &) const;
	uint4  opMod (uint) const;
	uint4  opMod_r (uint, const uint4 &);
	uint4 &  opAndAssign (const uint4 &);
	uint4 &  opAndAssign (uint);
	uint4  opAnd (const uint4 &) const;
	uint4  opAnd (uint) const;
	uint4  opAnd_r (uint, const uint4 &);
	uint4 &  opOrAssign (const uint4 &);
	uint4 &  opOrAssign (uint);
	uint4  opOr (const uint4 &) const;
	uint4  opOr (uint) const;
	uint4  opOr_r (uint, const uint4 &);
	uint4 &  opXorAssign (const uint4 &);
	uint4 &  opXorAssign (uint);
	uint4  opXor (const uint4 &) const;
	uint4  opXor (uint) const;
	uint4  opXor_r (uint, const uint4 &);
	uint4 &  opShlAssign (const uint4 &);
	uint4 &  opShlAssign (uint);
	uint4  opShl (const uint4 &) const;
	uint4  opShl (uint) const;
	uint4  opShl_r (uint, const uint4 &);
	uint4 &  opShrAssign (const uint4 &);
	uint4 &  opShrAssign (uint);
	uint4  opShr (const uint4 &) const;
	uint4  opShr (uint) const;
	uint4  opShr_r (uint, const uint4 &);
	bool  opEquals (const uint4 &, const uint4 &);
	int  opCmp (const uint4 &, const uint4 &);
};

struct float2
{
	float2 ();
	float2 (const float2&);
	float2&  operator = (const float2&);
	float x;
	float y;
	float2 (float v);
	float2 (const float3 & v3);
	float2 (const float4 & v4);
	float2 (float x, float y);
	float2 (const short2 & v2);
	float2 (const short3 & v3);
	float2 (const short4 & v4);
	float2 (const ushort2 & v2);
	float2 (const ushort3 & v3);
	float2 (const ushort4 & v4);
	float2 (const int2 & v2);
	float2 (const int3 & v3);
	float2 (const int4 & v4);
	float2 (const uint2 & v2);
	float2 (const uint3 & v3);
	float2 (const uint4 & v4);
	float2 &  opAddAssign (const float2 &);
	float2 &  opAddAssign (float);
	float2  opAdd (const float2 &) const;
	float2  opAdd (float) const;
	float2  opAdd_r (float, const float2 &);
	float2 &  opSubAssign (const float2 &);
	float2 &  opSubAssign (float);
	float2  opSub (const float2 &) const;
	float2  opSub (float) const;
	float2  opSub_r (float, const float2 &);
	float2 &  opMulAssign (const float2 &);
	float2 &  opMulAssign (float);
	float2  opMul (const float2 &) const;
	float2  opMul (float) const;
	float2  opMul_r (float, const float2 &);
	float2 &  opDivAssign (const float2 &);
	float2 &  opDivAssign (float);
	float2  opDiv (const float2 &) const;
	float2  opDiv (float) const;
	float2  opDiv_r (float, const float2 &);
	float2 &  opModAssign (const float2 &);
	float2 &  opModAssign (float);
	float2  opMod (const float2 &) const;
	float2  opMod (float) const;
	float2  opMod_r (float, const float2 &);
	bool  opEquals (const float2 &, const float2 &);
	int  opCmp (const float2 &, const float2 &);
	float2  opNeg () const;
};

struct float3
{
	float3 ();
	float3 (const float3&);
	float3&  operator = (const float3&);
	float x;
	float y;
	float z;
	float3 (float v);
	float3 (const float2 & v2);
	float3 (const float4 & v4);
	float3 (const float2 & xy, float z);
	float3 (float x, float y, float z);
	float3 (const short2 & v2);
	float3 (const short3 & v3);
	float3 (const short4 & v4);
	float3 (const ushort2 & v2);
	float3 (const ushort3 & v3);
	float3 (const ushort4 & v4);
	float3 (const int2 & v2);
	float3 (const int3 & v3);
	float3 (const int4 & v4);
	float3 (const uint2 & v2);
	float3 (const uint3 & v3);
	float3 (const uint4 & v4);
	float3 &  opAddAssign (const float3 &);
	float3 &  opAddAssign (float);
	float3  opAdd (const float3 &) const;
	float3  opAdd (float) const;
	float3  opAdd_r (float, const float3 &);
	float3 &  opSubAssign (const float3 &);
	float3 &  opSubAssign (float);
	float3  opSub (const float3 &) const;
	float3  opSub (float) const;
	float3  opSub_r (float, const float3 &);
	float3 &  opMulAssign (const float3 &);
	float3 &  opMulAssign (float);
	float3  opMul (const float3 &) const;
	float3  opMul (float) const;
	float3  opMul_r (float, const float3 &);
	float3 &  opDivAssign (const float3 &);
	float3 &  opDivAssign (float);
	float3  opDiv (const float3 &) const;
	float3  opDiv (float) const;
	float3  opDiv_r (float, const float3 &);
	float3 &  opModAssign (const float3 &);
	float3 &  opModAssign (float);
	float3  opMod (const float3 &) const;
	float3  opMod (float) const;
	float3  opMod_r (float, const float3 &);
	bool  opEquals (const float3 &, const float3 &);
	int  opCmp (const float3 &, const float3 &);
	float3  opNeg () const;
};

struct float4
{
	float4 ();
	float4 (const float4&);
	float4&  operator = (const float4&);
	float x;
	float y;
	float z;
	float w;
	float4 (float v);
	float4 (const float2 & v2);
	float4 (const float3 & v3);
	float4 (const float2 & xy, const float2 & zw);
	float4 (const float3 & xyz, float w);
	float4 (float x, float y, float z, float w);
	float4 (float x, const float3 & yzw);
	float4 (const short2 & v2);
	float4 (const short3 & v3);
	float4 (const short4 & v4);
	float4 (const ushort2 & v2);
	float4 (const ushort3 & v3);
	float4 (const ushort4 & v4);
	float4 (const int2 & v2);
	float4 (const int3 & v3);
	float4 (const int4 & v4);
	float4 (const uint2 & v2);
	float4 (const uint3 & v3);
	float4 (const uint4 & v4);
	float4 &  opAddAssign (const float4 &);
	float4 &  opAddAssign (float);
	float4  opAdd (const float4 &) const;
	float4  opAdd (float) const;
	float4  opAdd_r (float, const float4 &);
	float4 &  opSubAssign (const float4 &);
	float4 &  opSubAssign (float);
	float4  opSub (const float4 &) const;
	float4  opSub (float) const;
	float4  opSub_r (float, const float4 &);
	float4 &  opMulAssign (const float4 &);
	float4 &  opMulAssign (float);
	float4  opMul (const float4 &) const;
	float4  opMul (float) const;
	float4  opMul_r (float, const float4 &);
	float4 &  opDivAssign (const float4 &);
	float4 &  opDivAssign (float);
	float4  opDiv (const float4 &) const;
	float4  opDiv (float) const;
	float4  opDiv_r (float, const float4 &);
	float4 &  opModAssign (const float4 &);
	float4 &  opModAssign (float);
	float4  opMod (const float4 &) const;
	float4  opMod (float) const;
	float4  opMod_r (float, const float4 &);
	bool  opEquals (const float4 &, const float4 &);
	int  opCmp (const float4 &, const float4 &);
	float4  opNeg () const;
};

bool  All (const bool2 & x);
bool  Any (const bool2 & x);
bool  All (const bool3 & x);
bool  Any (const bool3 & x);
bool  All (const bool4 & x);
bool  Any (const bool4 & x);
float2  Abs (const float2 & x);
float2  MirroredWrap (const float2 & x, const float2 & min, const float2 & max);
float2  MirroredWrap (const float2 & x, float min, float max);
float  Sum (const float2 & x);
float  Area (const float2 & x);
float2  Min (const float2 & x, const float2 & y);
float2  Max (const float2 & x, const float2 & y);
float2  Clamp (const float2 & val, const float2 & min, const float2 & max);
float2  Clamp (const float2 & val, float min, float max);
float2  Wrap (const float2 & val, const float2 & min, const float2 & max);
float2  Wrap (const float2 & val, float min, float max);
float  VecToLinear (const float2 & pos, const float2 & dim);
float2  ToSNorm (const float2 & x);
float2  ToUNorm (const float2 & x);
float  Dot (const float2 & x, const float2 & y);
float2  Lerp (const float2 & x, const float2 & y, float factor);
float2  Normalize (const float2 & x);
float2  Floor (const float2 & x);
float2  Ceil (const float2 & x);
float2  Round (const float2 & x);
float2  Fract (const float2 & x);
float2  Sqrt (const float2 & x);
float2  Ln (const float2 & x);
float2  Log2 (const float2 & x);
float2  Log (const float2 & x, const float2 & base);
float2  Log (const float2 & x, float base);
float2  Pow (const float2 & x, const float2 & pow);
float2  Pow (const float2 & x, float pow);
float2  Exp (const float2 & x);
float2  Exp2 (const float2 & x);
float2  Exp10 (const float2 & x);
float  Length (const float2 & x);
float  LengthSq (const float2 & x);
float  Distance (const float2 & x, const float2 & y);
float  DistanceSq (const float2 & x, const float2 & y);
float3  Abs (const float3 & x);
float3  MirroredWrap (const float3 & x, const float3 & min, const float3 & max);
float3  MirroredWrap (const float3 & x, float min, float max);
float  Sum (const float3 & x);
float  Area (const float3 & x);
float3  Min (const float3 & x, const float3 & y);
float3  Max (const float3 & x, const float3 & y);
float3  Clamp (const float3 & val, const float3 & min, const float3 & max);
float3  Clamp (const float3 & val, float min, float max);
float3  Wrap (const float3 & val, const float3 & min, const float3 & max);
float3  Wrap (const float3 & val, float min, float max);
float  VecToLinear (const float3 & pos, const float3 & dim);
float3  Cross (const float3 & x, const float3 & y);
float3  ToSNorm (const float3 & x);
float3  ToUNorm (const float3 & x);
float  Dot (const float3 & x, const float3 & y);
float3  Lerp (const float3 & x, const float3 & y, float factor);
float3  Normalize (const float3 & x);
float3  Floor (const float3 & x);
float3  Ceil (const float3 & x);
float3  Round (const float3 & x);
float3  Fract (const float3 & x);
float3  Sqrt (const float3 & x);
float3  Ln (const float3 & x);
float3  Log2 (const float3 & x);
float3  Log (const float3 & x, const float3 & base);
float3  Log (const float3 & x, float base);
float3  Pow (const float3 & x, const float3 & pow);
float3  Pow (const float3 & x, float pow);
float3  Exp (const float3 & x);
float3  Exp2 (const float3 & x);
float3  Exp10 (const float3 & x);
float  Length (const float3 & x);
float  LengthSq (const float3 & x);
float  Distance (const float3 & x, const float3 & y);
float  DistanceSq (const float3 & x, const float3 & y);
float4  Abs (const float4 & x);
float4  MirroredWrap (const float4 & x, const float4 & min, const float4 & max);
float4  MirroredWrap (const float4 & x, float min, float max);
float  Sum (const float4 & x);
float  Area (const float4 & x);
float4  Min (const float4 & x, const float4 & y);
float4  Max (const float4 & x, const float4 & y);
float4  Clamp (const float4 & val, const float4 & min, const float4 & max);
float4  Clamp (const float4 & val, float min, float max);
float4  Wrap (const float4 & val, const float4 & min, const float4 & max);
float4  Wrap (const float4 & val, float min, float max);
float  VecToLinear (const float4 & pos, const float4 & dim);
float4  ToSNorm (const float4 & x);
float4  ToUNorm (const float4 & x);
float  Dot (const float4 & x, const float4 & y);
float4  Lerp (const float4 & x, const float4 & y, float factor);
float4  Normalize (const float4 & x);
float4  Floor (const float4 & x);
float4  Ceil (const float4 & x);
float4  Round (const float4 & x);
float4  Fract (const float4 & x);
float4  Sqrt (const float4 & x);
float4  Ln (const float4 & x);
float4  Log2 (const float4 & x);
float4  Log (const float4 & x, const float4 & base);
float4  Log (const float4 & x, float base);
float4  Pow (const float4 & x, const float4 & pow);
float4  Pow (const float4 & x, float pow);
float4  Exp (const float4 & x);
float4  Exp2 (const float4 & x);
float4  Exp10 (const float4 & x);
float  Length (const float4 & x);
float  LengthSq (const float4 & x);
float  Distance (const float4 & x, const float4 & y);
float  DistanceSq (const float4 & x, const float4 & y);
sbyte2  DivCeil (const sbyte2 & x, const sbyte2 & y);
sbyte2  DivCeil (const sbyte2 & x, int8 y);
sbyte3  DivCeil (const sbyte3 & x, const sbyte3 & y);
sbyte3  DivCeil (const sbyte3 & x, int8 y);
sbyte4  DivCeil (const sbyte4 & x, const sbyte4 & y);
sbyte4  DivCeil (const sbyte4 & x, int8 y);
ubyte2  DivCeil (const ubyte2 & x, const ubyte2 & y);
ubyte2  DivCeil (const ubyte2 & x, uint8 y);
ubyte3  DivCeil (const ubyte3 & x, const ubyte3 & y);
ubyte3  DivCeil (const ubyte3 & x, uint8 y);
ubyte4  DivCeil (const ubyte4 & x, const ubyte4 & y);
ubyte4  DivCeil (const ubyte4 & x, uint8 y);
short2  DivCeil (const short2 & x, const short2 & y);
short2  DivCeil (const short2 & x, int16 y);
short3  DivCeil (const short3 & x, const short3 & y);
short3  DivCeil (const short3 & x, int16 y);
short4  DivCeil (const short4 & x, const short4 & y);
short4  DivCeil (const short4 & x, int16 y);
ushort2  DivCeil (const ushort2 & x, const ushort2 & y);
ushort2  DivCeil (const ushort2 & x, uint16 y);
ushort3  DivCeil (const ushort3 & x, const ushort3 & y);
ushort3  DivCeil (const ushort3 & x, uint16 y);
ushort4  DivCeil (const ushort4 & x, const ushort4 & y);
ushort4  DivCeil (const ushort4 & x, uint16 y);
int2  Abs (const int2 & x);
int2  MirroredWrap (const int2 & x, const int2 & min, const int2 & max);
int2  MirroredWrap (const int2 & x, int min, int max);
int  Sum (const int2 & x);
int  Area (const int2 & x);
int2  Min (const int2 & x, const int2 & y);
int2  Max (const int2 & x, const int2 & y);
int2  Clamp (const int2 & val, const int2 & min, const int2 & max);
int2  Clamp (const int2 & val, int min, int max);
int2  Wrap (const int2 & val, const int2 & min, const int2 & max);
int2  Wrap (const int2 & val, int min, int max);
int  VecToLinear (const int2 & pos, const int2 & dim);
int2  DivCeil (const int2 & x, const int2 & y);
int2  DivCeil (const int2 & x, int y);
int3  Abs (const int3 & x);
int3  MirroredWrap (const int3 & x, const int3 & min, const int3 & max);
int3  MirroredWrap (const int3 & x, int min, int max);
int  Sum (const int3 & x);
int  Area (const int3 & x);
int3  Min (const int3 & x, const int3 & y);
int3  Max (const int3 & x, const int3 & y);
int3  Clamp (const int3 & val, const int3 & min, const int3 & max);
int3  Clamp (const int3 & val, int min, int max);
int3  Wrap (const int3 & val, const int3 & min, const int3 & max);
int3  Wrap (const int3 & val, int min, int max);
int  VecToLinear (const int3 & pos, const int3 & dim);
int3  DivCeil (const int3 & x, const int3 & y);
int3  DivCeil (const int3 & x, int y);
int4  Abs (const int4 & x);
int4  MirroredWrap (const int4 & x, const int4 & min, const int4 & max);
int4  MirroredWrap (const int4 & x, int min, int max);
int  Sum (const int4 & x);
int  Area (const int4 & x);
int4  Min (const int4 & x, const int4 & y);
int4  Max (const int4 & x, const int4 & y);
int4  Clamp (const int4 & val, const int4 & min, const int4 & max);
int4  Clamp (const int4 & val, int min, int max);
int4  Wrap (const int4 & val, const int4 & min, const int4 & max);
int4  Wrap (const int4 & val, int min, int max);
int  VecToLinear (const int4 & pos, const int4 & dim);
int4  DivCeil (const int4 & x, const int4 & y);
int4  DivCeil (const int4 & x, int y);
uint  Sum (const uint2 & x);
uint  Area (const uint2 & x);
uint2  Min (const uint2 & x, const uint2 & y);
uint2  Max (const uint2 & x, const uint2 & y);
uint2  Clamp (const uint2 & val, const uint2 & min, const uint2 & max);
uint2  Clamp (const uint2 & val, uint min, uint max);
uint2  Wrap (const uint2 & val, const uint2 & min, const uint2 & max);
uint2  Wrap (const uint2 & val, uint min, uint max);
uint  VecToLinear (const uint2 & pos, const uint2 & dim);
uint2  DivCeil (const uint2 & x, const uint2 & y);
uint2  DivCeil (const uint2 & x, uint y);
uint  Sum (const uint3 & x);
uint  Area (const uint3 & x);
uint3  Min (const uint3 & x, const uint3 & y);
uint3  Max (const uint3 & x, const uint3 & y);
uint3  Clamp (const uint3 & val, const uint3 & min, const uint3 & max);
uint3  Clamp (const uint3 & val, uint min, uint max);
uint3  Wrap (const uint3 & val, const uint3 & min, const uint3 & max);
uint3  Wrap (const uint3 & val, uint min, uint max);
uint  VecToLinear (const uint3 & pos, const uint3 & dim);
uint3  DivCeil (const uint3 & x, const uint3 & y);
uint3  DivCeil (const uint3 & x, uint y);
uint  Sum (const uint4 & x);
uint  Area (const uint4 & x);
uint4  Min (const uint4 & x, const uint4 & y);
uint4  Max (const uint4 & x, const uint4 & y);
uint4  Clamp (const uint4 & val, const uint4 & min, const uint4 & max);
uint4  Clamp (const uint4 & val, uint min, uint max);
uint4  Wrap (const uint4 & val, const uint4 & min, const uint4 & max);
uint4  Wrap (const uint4 & val, uint min, uint max);
uint  VecToLinear (const uint4 & pos, const uint4 & dim);
uint4  DivCeil (const uint4 & x, const uint4 & y);
uint4  DivCeil (const uint4 & x, uint y);
string  FindAndReplace (const string &, const string &, const string &);
bool  StartsWith (const string &, const string &);
bool  StartsWithIC (const string &, const string &);
bool  EndsWith (const string &, const string &);
bool  EndsWithIC (const string &, const string &);
struct VecSwizzle
{
	VecSwizzle ();
	VecSwizzle (const VecSwizzle&);
	VecSwizzle&  operator = (const VecSwizzle&);
	VecSwizzle (const string & swizzle);
};

struct ActionInfo
{
	ActionInfo ();
	ActionInfo (const ActionInfo&);
	ActionInfo&  operator = (const ActionInfo&);
	ActionInfo (const string &, EValueType);
	ActionInfo (const string &, EValueType, const float4 &);
	ActionInfo (const string &, EGestureType);
	ActionInfo (const string &, EValueType, EGestureType);
	ActionInfo (const string &, EValueType, EGestureType, const float4 &);
	ActionInfo (const string &, EValueType, EGestureType, const VecSwizzle &);
	ActionInfo (const string &, EValueType, EGestureType, const VecSwizzle &, const float4 &);
	EValueType type;
	EGestureType gesture;
	VecSwizzle swizzle;
};

struct GLFW_BindingsMode
{
	GLFW_BindingsMode ();
	void  Add (GLFW_Input, const ActionInfo &);
	void  Inherit (const string &);
	void  LockAndHideCursor ();
};

struct GLFW_ActionBindings
{
	GLFW_ActionBindings ();
	RC<GLFW_BindingsMode>  CreateMode (const string &);
};

struct WinAPI_BindingsMode
{
	WinAPI_BindingsMode ();
	void  Add (WinAPI_Input, const ActionInfo &);
	void  Inherit (const string &);
	void  LockAndHideCursor ();
};

struct WinAPI_ActionBindings
{
	WinAPI_ActionBindings ();
	RC<WinAPI_BindingsMode>  CreateMode (const string &);
};

struct Android_BindingsMode
{
	Android_BindingsMode ();
	void  Add (Android_Input, const ActionInfo &);
	void  Inherit (const string &);
};

struct Android_ActionBindings
{
	Android_ActionBindings ();
	RC<Android_BindingsMode>  CreateMode (const string &);
};

struct OpenVR_BindingsMode
{
	OpenVR_BindingsMode ();
	void  Add (OpenVR_Input, const ActionInfo &);
	void  Inherit (const string &);
};

struct OpenVR_ActionBindings
{
	OpenVR_ActionBindings ();
	RC<OpenVR_BindingsMode>  CreateMode (const string &);
};

template <>
struct RC<WinAPI_ActionBindings> : WinAPI_ActionBindings
{
	RC (const WinAPI_ActionBindings &);
};

template <>
struct RC<Android_ActionBindings> : Android_ActionBindings
{
	RC (const Android_ActionBindings &);
};

template <>
struct RC<Android_BindingsMode> : Android_BindingsMode
{
	RC (const Android_BindingsMode &);
};

template <>
struct RC<OpenVR_BindingsMode> : OpenVR_BindingsMode
{
	RC (const OpenVR_BindingsMode &);
};

template <>
struct RC<GLFW_BindingsMode> : GLFW_BindingsMode
{
	RC (const GLFW_BindingsMode &);
};

template <>
struct RC<OpenVR_ActionBindings> : OpenVR_ActionBindings
{
	RC (const OpenVR_ActionBindings &);
};

template <>
struct RC<WinAPI_BindingsMode> : WinAPI_BindingsMode
{
	RC (const WinAPI_BindingsMode &);
};

template <>
struct RC<GLFW_ActionBindings> : GLFW_ActionBindings
{
	RC (const GLFW_ActionBindings &);
};

