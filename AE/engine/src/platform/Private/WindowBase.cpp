// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Private/WindowBase.h"
#include "platform/Private/ApplicationBase.h"

namespace AE::App
{
namespace
{
/*
=================================================
	destructor
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
			case EState::Focused :			return AnyEqual( dst, EState::InForeground						 );
			case EState::InBackground :		return AnyEqual( dst, EState::InForeground,	EState::Stopped		 );
			case EState::Stopped :			return AnyEqual( dst, EState::Destroyed,	EState::InBackground );
			case EState::Destroyed :		return false;

			default :						return false;
		}
		switch_end
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
		ASSERT(	not _surface.IsInitialized() );
	}

/*
=================================================
	CreateRenderSurface
=================================================
*/
	bool  WindowBase::CreateRenderSurface (const Graphics::SwapchainDesc &desc) __NE___
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
	_SetState
=================================================
*/
	void  WindowBase::_SetState (EState newState)
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
	_SetStateV2
=================================================
*/
	void  WindowBase::_SetStateV2 (EState newState)
	{
		if_unlikely( _wndState == newState )
			return;

		if_unlikely( not IsValidNewState( _wndState, newState ))
			return;

		_wndState = newState;

		if_likely( _listener )
		{
			switch_enum( _wndState )
			{
				case EState::Started :
					_listener->OnStateChanged( *this, _wndState );
					_surface.CreateSwapchain();
					_listener->OnSurfaceCreated( *this );
					break;

				case EState::Stopped :
					_listener->OnSurfaceDestroyed( *this );
					_surface.DestroySwapchain();
					_listener->OnStateChanged( *this, _wndState );
					break;

				case EState::Created :
				case EState::Destroyed :
				case EState::InForeground :
				case EState::InBackground :
				case EState::Focused :
					_listener->OnStateChanged( *this, _wndState );
					break;

				case EState::Unknown :
				default :
					DBG_WARNING( "unknown state" );
					break;
			}
			switch_end
		}
	}

/*
=================================================
	_DestroyListener
=================================================
*/
	void  WindowBase::_DestroyListener ()
	{
		if ( _wndState == EState::Focused )
			_SetStateV2( EState::InForeground );

		_SetStateV2( EState::InBackground );
		_SetStateV2( EState::Stopped );
		_SetStateV2( EState::Destroyed );

		_listener.reset();
	}


} // AE::App
