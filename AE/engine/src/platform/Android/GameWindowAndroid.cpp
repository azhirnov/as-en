// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_PLATFORM_ANDROID
# include "platform/Android/GameAppAndroid.h"
# include "platform/Android/GameWindowAndroid.h"

namespace AE::App
{

/*
=================================================
	constructor
=================================================
*/
	GameWindowAndroid::GameWindowAndroid (GameAppAndroid& app, Unique<IWndListener> listener) __NE___ :
		WindowBaseWithSurface{ app, RVRef(listener) }
	{}

/*
=================================================
	Close
=================================================
*/
	void  GameWindowAndroid::Close () __NE___
	{
		DRC_EXLOCK( _drCheck );

	}

/*
=================================================
	GetSurfaceSize
=================================================
*/
	uint2  GameWindowAndroid::GetSurfaceSize () C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		return _surfSize;
	}

/*
=================================================
	GetMonitor
=================================================
*/
	Monitor  GameWindowAndroid::GetMonitor () C_NE___
	{
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		return _app.GetMonitors( false ).front();
	}

/*
=================================================
	GetNative
=================================================
*/
	NativeWindow  GameWindowAndroid::GetNative () C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		return {};
	}

/*
=================================================
	_Init
=================================================
*
	void  GameWindowAndroid::_Init (Unique<IWndListener> listener, IInputActions* dstActions) __NE___
	{
		CHECK_ERRV( not _listener );
		CHECK_ERRV( listener );

		if ( dstActions != null )
			_input.SetQueue( InputActionsBase::GetQueue( dstActions ));

		_listener = RVRef(listener);

		if ( _wndState == EState::Destroyed )
		{
			_listener->OnStateChanged( *this, EState::Created );
			_listener->OnStateChanged( *this, EState::Destroyed );
			return;
		}

		if ( _wndState <= EState::InForeground )
		{
			for (EState s = EState::Created; s <= EState::InForeground; s = EState(uint(s) + 1))
			{
				_listener->OnStateChanged( *this, s );
			}
		}
		else
		if ( _wndState == EState::InBackground )
		{
			_listener->OnStateChanged( *this, EState::Created );
			_listener->OnStateChanged( *this, EState::Started );
			_listener->OnStateChanged( *this, EState::InBackground );
		}
		else
		if ( _wndState == EState::Stopped )
		{
			_listener->OnStateChanged( *this, EState::Created );
			_listener->OnStateChanged( *this, EState::Started );
			_listener->OnStateChanged( *this, EState::Stopped );
		}

		if ( _java.nativeWindow != null )
			_listener->OnSurfaceCreated( *this );
	}

/*
=================================================
	SetBrightness
=================================================
*/
	bool  GameWindowAndroid::SetBrightness (Percent level) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		return false;
	}

/*
=================================================
	SetColorSpace
----
	set max brightness for HDR color space
=================================================
*/
	bool  GameWindowAndroid::SetColorSpace (EColorSpace colorSpace) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		return false;
	}

/*
=================================================
	ProcessMessages
=================================================
*/
	bool  GameWindowAndroid::ProcessMessages () __NE___
	{
		return true;
	}

} // AE::App

#endif // AE_PLATFORM_ANDROID
