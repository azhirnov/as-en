// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/FixedArray.h"
#include "base/Containers/FixedMap.h"
#include "base/Algorithms/ArrayUtils.h"

#include "base/Math/Vec.h"
#include "base/Math/Rectangle.h"
#include "base/Math/Matrix.h"

#include "base/DataSource/Stream.h"
#include "base/CompileTime/TypeList.h"
#include "base/Utils/Helpers.h"
#include "base/Utils/NamedID.h"

#include "threading/TaskSystem/TaskScheduler.h"
#include "threading/Primitives/DataRaceCheck.h"

#include "graphics/Public/FrameUID.h"

#include "vfs/VirtualFileSystem.h"

namespace AE::App::_hidden_
{
	// tag: NamedID UID
	static constexpr uint	AppIDs_Start = 3 << 24;

} // AE::App::_hidden_


namespace AE::App
{
	using namespace AE::Base;

	using Threading::Atomic;
	using Threading::BytesAtomic;
	using Threading::AsyncTask;

#  if AE_ENABLE_DATA_RACE_CHECK
	using Threading::DataRaceCheck;
	using Threading::RWDataRaceCheck;
	using Threading::SingleThreadCheck;
#  endif
	
	using Graphics::FrameUID;

	using VFS::IVirtualFileStorage;

	
	class IApplication;
	class IWindow;
	class IVRDevice;
	class IOutputSurface;


	using WindowPtr		= SharedPtr< IWindow >;
	using VRDevicePtr	= SharedPtr< IVRDevice >;


	// Used to identify different gamepads and to send feedback actions (vibrations, ...).
	enum class ControllerID : ushort
	{
		Default		= 0,
		Invalid		= 0xFFFF,
		
		_First		= 1,
		_VRFirst	= 1 << 14,

	  // default IDs:
		Mouse		= _First,
		Keyboard,
		Touchscreen,

		VR_HMD		= _VRFirst,
		VR_LeftHand,
		VR_RightHand,
	};


	using InputModeName		= NamedID< 32, App::_hidden_::AppIDs_Start + 1, AE_OPTIMIZE_IDS >;
	using InputActionName	= NamedID< 32, App::_hidden_::AppIDs_Start + 2, AE_OPTIMIZE_IDS >;
	using LocaleName		= NamedID< 8,  App::_hidden_::AppIDs_Start + 3, AE_OPTIMIZE_IDS >;	// format: en-US, ru-RU


	struct PlatformConfig
	{
		static constexpr uint	MaxMonitors		= 4;
		static constexpr uint	MaxWindows		= 8;
	};

} // AE::App
