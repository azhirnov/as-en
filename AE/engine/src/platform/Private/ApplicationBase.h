// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "platform/Public/Application.h"
#include "platform/Private/WindowBase.h"
#include "platform/Private/NvAPILib.h"

namespace AE::App
{

	//
	// Application Base
	//

	class ApplicationBase : public IApplication, public Noncopyable
	{
	// types
	public:
		using Monitors_t	= FixedArray< Monitor, PlatformConfig::MaxMonitors >;

	protected:
		using WindowWPtr	= WeakPtr< WindowBase >;
		using WindowArray_t	= FixedArray< WindowWPtr, PlatformConfig::MaxWindows >;

		using StorageCache_t = StaticArray< AtomicRC<IVirtualFileStorage>, uint(EAppStorage::_Count) >;

		using MountBits_t	= Threading::TValueWithSpinLockBit< uint, 31, false >;	// bitset of EAppStorage


	// variables
	protected:
		Atomic< int >			_timeSinceStart	{0};
		Atomic< bool >			_isRunning		{true};

		Unique< IAppListener >	_listener;
		const Clock				_timer;

		WindowArray_t			_windows;

	  #ifdef AE_PLATFORM_WINDOWS
		NvAPILib				_nvapi;
	  #endif

		StorageCache_t			_storageCache;
		MountBits_t				_mountStorages	{0};

		DRC_ONLY(
			mutable SingleThreadCheck	_stCheck;
		)


	// methods
	protected:
		explicit ApplicationBase (Unique<IAppListener>)											__NE___;
		~ApplicationBase ()																		__NE___;

		// main thread
		void  _BeforeUpdate ()																	__NE___;
		void  _Update ()																		__NE___;
		void  _AfterUpdate ()																	__NE___;

		void  _Destroy ()																		__NE___;

		static StringView	_GetStoragePrefix (EAppStorage)										__NE___;

		virtual void  _AddWindow (SharedPtr<WindowBase>)										__NE___;

	public:
		WindowPtr    CreateVRSession (WndListenerPtr, IInputActions*, IVRSession::EDeviceType)	__NE_OV;

		Duration_t   GetTimeSinceStart ()														C_NE_OF	{ return Duration_t{_timeSinceStart.load()}; }

		Monitor::ID  GetMonitor (int2 pos)														C_NE_OF;
		Monitor::ID  GetMonitorFromNative (Monitor::NativeMonitor_t)							C_NE_OF;

		void		 Terminate ()																__NE_OV;

		bool		 MountStorage (EAppStorage type)											__NE_OV;

		DRC_ONLY( ND_ SingleThreadCheck&	GetSingleThreadCheck ()								C_NE___	{ return _stCheck; })

	  #ifdef AE_PLATFORM_WINDOWS
		ND_ NvAPILib&		GetNvAPI ()															__NE___	{ return _nvapi; }
		ND_ NvAPILib const&	GetNvAPI ()															C_NE___	{ return _nvapi; }
	  #endif
	};


} // AE::App
