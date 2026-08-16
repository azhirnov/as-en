// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "platform/Private/ApplicationBase.h"

#include "platform/OpenVR/VRSessionOpenVR.h"
#include "platform/OpenXR/VRSessionOpenXR.h"
#include "platform/Private/VRDeviceEmulator.h"

#include "platform/WinAPI/UtilsWinAPI.h"

namespace AE::App
{

/*
=================================================
	constructor
=================================================
*/
	ApplicationBase::ApplicationBase (Unique<IAppListener> listener) __NE___ :
		_listener{ RVRef(listener) }
	{
		CHECK( _listener );

	  #ifdef AE_PLATFORM_WINDOWS
		Unused( _nvapi.Load() );
	  #endif
	}

/*
=================================================
	destructor
=================================================
*/
	ApplicationBase::~ApplicationBase () __NE___
	{
		CHECK( not _isRunning.load() );
		CHECK( not _listener );
		CHECK( _windows.empty() );
	}

/*
=================================================
	CreateVRSession
=================================================
*/
	WindowPtr  ApplicationBase::CreateVRSession (WndListenerPtr listener, IInputActions* dst, IVRSession::EDeviceType type) __NE___
	{
		using EDeviceType = IVRSession::EDeviceType;

		CHECK_ERR( _isRunning.load() );
		CHECK_ERR( listener );

		DRC_EXLOCK( _stCheck );

		switch_enum( type )
		{
			case EDeviceType::Emulator :
			{
			#if 1
				SharedPtr<VRDeviceEmulator>	vr{ new VRDeviceEmulator{ *this, RVRef(listener), dst }};
				if ( not vr->Create() )
					return Default;

				_AddWindow( vr );
				return vr;
			#else
				break;
			#endif
			}

			case EDeviceType::OpenVR :
			{
			#ifdef AE_ENABLE_OPENVR
				SharedPtr<VRSessionOpenVR>	vr{ new VRSessionOpenVR{ RVRef(listener), dst }};
				if ( not vr->Create() )
					return Default;

				_AddWindow( vr );
				return vr;
			#else
				break;
			#endif
			}

			case EDeviceType::OpenXR :
			{
			#ifdef AE_ENABLE_OPENXR
				SharedPtr<VRSessionOpenXR>	vr{ new VRSessionOpenXR{ *this, RVRef(listener), dst }};
				if ( not vr->Create() )
					return Default;

				_AddWindow( vr );
				return vr;
			#else
				break;
			#endif
			}

			case EDeviceType::Unknown : break;
		}
		switch_end

		return Default;
	}

/*
=================================================
	_BeforeUpdate
=================================================
*/
	void  ApplicationBase::_BeforeUpdate () __NE___
	{
		ASSERT( _isRunning.load() );

		_timeSinceStart.store( _timer.TimeSince<Duration_t>().count() );

		if_likely( _listener )
			_listener->BeforeWndUpdate( *this );
	}

/*
=================================================
	_Update
=================================================
*/
	void  ApplicationBase::_Update () __NE___
	{
		ASSERT( _isRunning.load() );

		for (usize i = 0; i < _windows.size();)
		{
			// ProcessMessages() will return 'false' if window is closed
			if_likely( auto wnd = _windows[i].lock();  wnd and wnd->ProcessMessages() )
				++i;
			else
				_windows.fast_erase( i );
		}
	}

/*
=================================================
	_AfterUpdate
=================================================
*/
	void  ApplicationBase::_AfterUpdate () __NE___
	{
		if_likely( _listener )
			_listener->AfterWndUpdate( *this );
	}

/*
=================================================
	_Destroy
=================================================
*/
	void  ApplicationBase::_Destroy () __NE___
	{
		_isRunning.store( false );

		if ( _listener )
		{
			_listener->OnStop( *this );
			_listener.reset();
		}
	}

/*
=================================================
	Terminate
=================================================
*/
	void  ApplicationBase::Terminate () __NE___
	{
		_isRunning.store( false );
	}

/*
=================================================
	GetMonitor
=================================================
*/
	Monitor::ID  ApplicationBase::GetMonitor (const int2 pos) C_NE___
	{
		MonitorsView_t	monitors = GetCachedMonitors();

		for (const Monitor& monitor : monitors)
		{
			if ( Base::IsIntersects( monitor.region.pixels.LeftTop(), monitor.region.pixels.RightBottom(), pos, pos+1 ))
				return monitor.id;
		}
		return Default;
	}

/*
=================================================
	GetMonitorFromNative
=================================================
*/
	Monitor::ID  ApplicationBase::GetMonitorFromNative (Monitor::NativeMonitor_t handle) C_NE___
	{
		MonitorsView_t	monitors = GetCachedMonitors();

		for (const Monitor& monitor : monitors)
		{
			if ( monitor.native == handle )
				return monitor.id;
		}
		return Default;
	}

/*
=================================================
	_AddWindow
=================================================
*/
	void  ApplicationBase::_AddWindow (SharedPtr<WindowBase> wnd) __NE___
	{
		_windows.emplace_back( wnd );
	}

/*
=================================================
	_GetStoragePrefix
=================================================
*/
	StringView  ApplicationBase::_GetStoragePrefix (EAppStorage type) __NE___
	{
		switch_enum( type )
		{
			case EAppStorage::Builtin :			return "builtin:/";
			case EAppStorage::Cache :			return "cache:/";
			case EAppStorage::ExternalCache :	return "ext-cache:/";
			case EAppStorage::UserData :		return "user-data:/";
			case EAppStorage::SharedData :		return "shared-data:/";
			case EAppStorage::_Count :
			default :							return {};
		}
		switch_end
	}

/*
=================================================
	MountStorage
=================================================
*/
	bool  ApplicationBase::MountStorage (EAppStorage type) __NE___
	{
		if ( HasBit( _mountStorages.get(), uint(type) ))
			return true;  // already mount

		auto	storage = OpenStorage( type );
		CHECK_ERR( storage );

		_mountStorages.lock();

		uint	bits = _mountStorages.get();

		if ( HasBit( bits, uint(type) ))
		{
			_mountStorages.unlock();
			return true;  // mount in another thread
		}

		VFS::StorageName	name;
		switch_enum( type )
		{
			case EAppStorage::Builtin :			name = Storage_Builtin;			break;
			case EAppStorage::Cache :			name = Storage_Cache;			break;
			case EAppStorage::ExternalCache :	name = Storage_ExternalCache;	break;
			case EAppStorage::UserData :		name = Storage_UserData;		break;
			case EAppStorage::SharedData :		name = Storage_SharedData;		break;
			case EAppStorage::_Count :			break;
		}
		switch_end

		bool	res = GetVFS().AddStorage( name, RVRef(storage) );

		bits = SetBit( bits, res, uint(type) );

		_mountStorages.UnlockAndSet( bits );
		return res;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	StartScreenCapture
=================================================
*/
	Promise<RC<IScreenCapture>>  IApplicationTS::StartScreenCapture (const IScreenCapture::Config &cfg) __NE___
	{
		return Scheduler().Run(
					ETaskQueue::Main,
					[](auto app, auto config) -> Promise<RC<IScreenCapture>>
					{
						co_return app->StartScreenCapture( config );
					}( _app, cfg ));
	}

} // AE::App
