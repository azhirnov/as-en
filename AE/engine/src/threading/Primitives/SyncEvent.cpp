// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/Primitives/SyncEvent.h"

#if (AE_SYNC_EVENT_MODE == 0)
# include "base/Platforms/WindowsHeader.cpp.h"

namespace AE::Threading
{

/*
=================================================
    constructor
=================================================
*/
    SyncEvent::SyncEvent (EFlags flags) __NE___
    {
        StaticAssert( sizeof(_event) == sizeof(HANDLE) );

        _event = ::CreateEventA( null,
                                 not AllBits( flags, EFlags::AutoReset ),
                                 AllBits( flags, EFlags::InitStateSignaled ),
                                 null );
    }

/*
=================================================
    destructor
=================================================
*/
    SyncEvent::~SyncEvent () __NE___
    {
        auto    res = ::CloseHandle( _event );
        Unused( res );
        ASSERT( res != FALSE );
    }

/*
=================================================
    Signal
=================================================
*/
    void  SyncEvent::Signal () __NE___
    {
        auto    res = ::SetEvent( static_cast<HANDLE>( _event ));
        Unused( res );
        ASSERT( res != FALSE );
    }

/*
=================================================
    Reset
=================================================
*/
    void  SyncEvent::Reset () __NE___
    {
        auto    res = ::ResetEvent( static_cast<HANDLE>( _event ));
        Unused( res );
        ASSERT( res != FALSE );
    }

/*
=================================================
    _Wait
=================================================
*/
    bool  SyncEvent::_Wait (uint timeoutMS) __NE___
    {
        return ::WaitForSingleObject( static_cast<HANDLE>( _event ), timeoutMS ) == WAIT_OBJECT_0;
    }


} // AE::Threading

#endif // AE_SYNC_EVENT_MODE
