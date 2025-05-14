//f60471aa
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

struct float2;
struct float3;
struct GLFW_ActionBindings;
struct WinAPI_BindingsMode;
struct OpenVR_ActionBindings;
struct GLFW_BindingsMode;
struct ushort4;
struct short2;
struct short3;
struct sbyte4;
struct sbyte3;
struct uint4;
struct float4;
struct uint3;
struct uint2;
struct VecSwizzle;
struct Android_ActionBindings;
struct OpenVR_BindingsMode;
struct ushort3;
struct sbyte2;
struct ActionInfo;
struct Android_BindingsMode;
struct ushort2;
struct short4;
struct bool4;
struct int3;
struct bool2;
struct bool3;
struct int2;
struct int4;
struct ubyte3;
struct ubyte2;
struct WinAPI_ActionBindings;
struct ubyte4;

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
struct RC<GLFW_ActionBindings> : GLFW_ActionBindings
{
	RC (const GLFW_ActionBindings &);
};

template <>
struct RC<WinAPI_BindingsMode> : WinAPI_BindingsMode
{
	RC (const WinAPI_BindingsMode &);
};

template <>
struct RC<OpenVR_ActionBindings> : OpenVR_ActionBindings
{
	RC (const OpenVR_ActionBindings &);
};

template <>
struct RC<GLFW_BindingsMode> : GLFW_BindingsMode
{
	RC (const GLFW_BindingsMode &);
};

template <>
struct RC<Android_ActionBindings> : Android_ActionBindings
{
	RC (const Android_ActionBindings &);
};

template <>
struct RC<OpenVR_BindingsMode> : OpenVR_BindingsMode
{
	RC (const OpenVR_BindingsMode &);
};

template <>
struct RC<Android_BindingsMode> : Android_BindingsMode
{
	RC (const Android_BindingsMode &);
};

template <>
struct RC<WinAPI_ActionBindings> : WinAPI_ActionBindings
{
	RC (const WinAPI_ActionBindings &);
};

