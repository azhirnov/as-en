// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Common.h"
#include "base/Math/BitMath.h"
#include "base/Utils/Helpers.h"

#ifdef AE_PLATFORM_WINDOWS
#   define AE_SYNC_EVENT_MODE   1
#else
#   define AE_SYNC_EVENT_MODE   1
#endif


#if (AE_SYNC_EVENT_MODE == 0)

namespace AE::Threading
{

    //
    // Synchronization Event (WinAPI)
    //

    struct SyncEvent : public Noncopyable
    {
    //types
    public:
        enum class EFlags
        {
            ManualReset         = 0,
            AutoReset           = 1 << 0,
            InitStateSignaled   = 1 << 2,
        };


    // variables
    private:
        void*   _event  = null;


    // methods
    public:
        explicit SyncEvent (EFlags flags = EFlags::AutoReset) __NE___;
        ~SyncEvent ()       __NE___;

        void  Signal ()     __NE___;
        void  Reset ()      __NE___;

        void  Wait ()       __NE___ { return void( _Wait( UMax )); }

        ND_ bool  Test ()   __NE___ { return _Wait(0); }

        template <typename Rep, typename Period>
        ND_ bool  Wait (const std::chrono::duration<Rep, Period>& timeout) __NE___
        {
            constexpr uint  MaxTimeout  = UMax;
            const auto      dt          = Cast<milliseconds>( timeout ).count();

            return _Wait( dt > MaxTimeout ? MaxTimeout : uint(dt) );
        }

    private:
        ND_ bool  _Wait (uint timeout) __NE___;
    };

} // AE::Threading
//-----------------------------------------------------------------------------


#elif (AE_SYNC_EVENT_MODE == 1)

namespace AE::Threading
{

    //
    // Synchronization Event
    //

    struct SyncEvent : public Noncopyable
    {
    //types
    public:
        enum class EFlags
        {
            ManualReset         = 0,
            AutoReset           = 1 << 0,
            InitStateSignaled   = 1 << 2,
        };


    // variables
    private:
        ConditionVariable   _cv;
        Mutex               _mutex;
        bool                _autoReset : 1;
        bool                _triggered : 1;


    // methods
    public:
        explicit SyncEvent (EFlags flags = EFlags::AutoReset) __NE___ :
            _autoReset{ AllBits( flags, EFlags::AutoReset )},
            _triggered{ AllBits( flags, EFlags::InitStateSignaled )}
        {}

        void  Signal ()     __NE___
        {
            EXLOCK( _mutex );
            _triggered = true;
            _autoReset ? _cv.notify_one() : _cv.notify_all();
        }

        void  Reset ()      __NE___
        {
            EXLOCK( _mutex );
            _triggered = false;
        }

        void  Wait ()       __NE___
        {
            std::unique_lock lock{ _mutex };

            while ( not _triggered ) {
                _cv.wait( lock );
            }

            if ( _autoReset )
                _triggered = false;
        }

        ND_ bool  Test ()   __NE___
        {
            EXLOCK( _mutex );
            return _triggered;
        }

        template <typename Rep, typename Period>
        ND_ bool  Wait (const std::chrono::duration<Rep, Period>& timeout) __NE___
        {
            bool    res = false;

            std::unique_lock lock{ _mutex };

            if ( not _triggered )
            {
                res = _cv.wait_for( lock, timeout );

                if ( not _triggered )
                    res = false;
            }

            if ( res and _autoReset )
                _triggered = false;

            return res;
        }
    };

} // AE::Threading

#endif // AE_SYNC_EVENT_MODE
