// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "platform/Private/WindowBase.h"
#include "platform/Private/ApplicationBase.h"
#include "platform/Public/DesktopWindow.h"

namespace AE::App
{
namespace
{
/*
=================================================
	IsValidNewState
=================================================
*/
	ND_ static bool  IsValidNewState (IWindow::EState src, IWindow::EState dst) __NE___
	{
		using EState = IWindow::EState;

		switch_enum( src )
		{
			case EState::Unknown :			return AnyEqual( dst, EState::Created,		EState::Destroyed	 );
			case EState::Created :			return AnyEqual( dst, EState::Started,		EState::Destroyed	 );
			case EState::Started :			return AnyEqual( dst, EState::InForeground,	EState::Stopped		 );
			case EState::InForeground :		return AnyEqual( dst, EState::Focused,		EState::InBackground );
			case EState::Focused :			return AnyEqual( dst, EState::InForeground,	EState::InBackground );
			case EState::InBackground :		return AnyEqual( dst, EState::Focused, 		EState::InForeground,	EState::Stopped	);
			case EState::Stopped :			return AnyEqual( dst, EState::Destroyed,	EState::InBackground );
			case EState::Destroyed :
			case EState::_Count :			break;
		}
		switch_end
		return false;
	}

/*
=================================================
	GetNextState
=================================================
*/
	ND_ static IWindow::EState  GetNextState (IWindow::EState src, int dir) __NE___
	{
		using EState = IWindow::EState;
		switch_enum( src )
		{
			case EState::Unknown :			return dir > 0 ? EState::Created 		: EState::Destroyed;
			case EState::Created :			return dir > 0 ? EState::Started 		: EState::Destroyed;
			case EState::Started :			return dir > 0 ? EState::InForeground 	: EState::Stopped;
			case EState::InForeground :		return dir > 0 ? EState::Focused 		: EState::InBackground;
			case EState::Focused :			return dir > 0 ? EState::InBackground	: EState::InForeground;
			case EState::InBackground :		return dir > 0 ? EState::Stopped		: EState::InForeground;
			case EState::Stopped :			return dir > 0 ? EState::Destroyed		: EState::InBackground;
			case EState::Destroyed :
			case EState::_Count :			break;
		}
		switch_end
		return EState::_Count;
	}

}
//-----------------------------------------------------------------------------



/*
=================================================
	destructor
=================================================
*/
	WindowBase::~WindowBase () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( _wndState == EState::Destroyed );
	}

/*
=================================================
	_SetState
=================================================
*/
	void  WindowBase::_SetState (EState newState) __NE___
	{
		if_unlikely( _wndState == newState )
			return;

		ASSERT( IsValidNewState( _wndState, newState ));

		if_unlikely( not IsValidNewState( _wndState, newState ))
			return;

		_wndState = newState;

		if_likely( _listener )
			_listener->OnStateChanged( *this, _wndState );
	}

/*
=================================================
	_SetStateImpl
=================================================
*/
	void  WindowBase::_SetStateImpl (EState newState) __NE___
	{
		ASSERT( IsValidNewState( _wndState, newState ));

		_wndState = newState;

		if_unlikely( not _listener )
			return;

		switch_enum( _wndState )
		{
			case EState::Started :
				_listener->OnStateChanged( *this, _wndState );
				_CreateSwapchain();
				_listener->OnSurfaceCreated( *this );
				break;

			case EState::Stopped :
				_listener->OnSurfaceDestroyed( *this );
				_DestroySwapchain();
				_listener->OnStateChanged( *this, _wndState );
				break;

			case EState::Created :
			case EState::Destroyed :
			case EState::InForeground :
			case EState::InBackground :
			case EState::Focused :
				_listener->OnStateChanged( *this, _wndState );
				break;

			case EState::_Count :
			case EState::Unknown :
			default :
				DBG_WARNING( "unknown state" );
				break;
		}
		switch_end
	}

/*
=================================================
	_SetStateV2
=================================================
*/
	void  WindowBase::_SetStateV2 (EState newState) __NE___
	{
		if_unlikely( _wndState == newState )
			return;

		if_unlikely( not IsValidNewState( _wndState, newState ))
			return;

		_SetStateImpl( newState );
	}

/*
=================================================
	_SetStateV3
=================================================
*/
	void  WindowBase::_SetStateV3 (EState newState) __NE___
	{
		if_unlikely( _wndState == newState )
			return;

		// validate
		{
			ASSERT( newState > EState::Unknown and newState < EState::_Count );

			if ( (_wndState == EState::InForeground and newState == EState::InBackground) or
				 (_wndState == EState::InBackground and newState == EState::InForeground) )
			{
				// don't go through 'Focused' state
				_wndState = EState::Focused;
			}else
			if ( _wndState > EState::Focused ){
				ASSERT( newState >= EState::Focused );
			}else
			if ( _wndState < EState::Focused ){
				ASSERT( newState <= EState::Focused );
			}
		}

		const int	dir = Sign( int(newState) - int(_wndState) );

		for (EState state = _wndState; state != newState;)
		{
			state = GetNextState( state, dir );
			if ( state >= EState::_Count )
				break;

			_SetStateImpl( state );
		}
	}

/*
=================================================
	_DestroyListener
=================================================
*/
	void  WindowBase::_DestroyListener () __NE___
	{
		if ( _wndState == EState::Focused )
			_SetStateV2( EState::InForeground );

		_SetStateV2( EState::InBackground );
		_SetStateV2( EState::Stopped );
		_SetStateV2( EState::Destroyed );

		_listener.reset();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	destructor
=================================================
*/
	WindowBaseWithSurface::~WindowBaseWithSurface () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( _wndState == EState::Destroyed );
		ASSERT(	not _surface.IsInitialized() );
	}

/*
=================================================
	CreateRenderSurface
=================================================
*/
	bool  WindowBaseWithSurface::CreateRenderSurface (const Graphics::SwapchainDesc &desc) __NE___
	{
		DRC_EXLOCK( _drCheck );
		DRC_EXLOCK( _app.GetSingleThreadCheck() );

		CHECK_ERR( _surface.Init( *this, desc ));

		if ( auto state = GetState();  state >= EState::Started and state < EState::Stopped )
		{
			_surface.CreateSwapchain();
		}
		return true;
	}

/*
=================================================
	_ResizeWindowToSurface
=================================================
*/
	void  WindowBaseWithSurface::_ResizeWindowToSurface () __NE___
	{
		if ( not _surface.IsInitialized() )
			return;

		auto	info = _surface.GetTargetInfo();
		ASSERT_Eq( info.size(), 1 );

		if ( info.size() > 0 and
			 Any( info[0].Dimension() != GetSurfaceSize() ))
		{
			if ( auto* wnd = AsDesktopWindow() )
				wnd->SetSize( info[0].Dimension() );
		}
	}


} // AE::App
