// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Private/VRDeviceBase.h"

namespace AE::App
{

/*
=================================================
    _StateChanged
=================================================
*/
    bool  VRDeviceBase::_StateChanged (EState newState) const
    {
        const bool  skip_focused    = ((_hmdState == EState::InForeground) & (newState == EState::InBackground)) |
                                      ((_hmdState == EState::InBackground) & (newState == EState::InForeground));
        const int   diff            = Abs( int(_hmdState) - int(newState) );

        return  (diff == 1) | skip_focused;
    }

/*
=================================================
    _SetState
=================================================
*/
    void  VRDeviceBase::_SetState (EState newState)
    {
        if_unlikely( not _StateChanged( newState ))
            return;

        _hmdState = newState;

        if_likely( _listener )
            _listener->OnStateChanged( *this, _hmdState );
    }

/*
=================================================
    _DestroyListener
=================================================
*/
    void  VRDeviceBase::_DestroyListener ()
    {
        _SetState( EState::InBackground );
        _SetState( EState::Stopped );
        _SetState( EState::Destroyed );

        _listener.reset();
    }

} // AE::App
