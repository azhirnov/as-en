// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

    //
    // Clock
    //

    struct Clock
    {
    // types
    public:
        using Clock_t       = std::chrono::high_resolution_clock;
        using TimePoint_t   = Clock_t::time_point;
        using Duration_t    = Clock_t::duration;


    // variables
    private:
        TimePoint_t     _lastTick;


    // methods
    public:
        Clock ()                                                __NE___ : _lastTick{Clock_t::now()} {}
        explicit Clock (TimePoint_t lastTick)                   __NE___ : _lastTick{lastTick} {}

            void        Start (TimePoint_t lastTick)            __NE___ { _lastTick = lastTick; }
            void        Start ()                                __NE___ { _lastTick = Clock_t::now(); }

        template <typename ToDuration>
        ND_ ToDuration  Tick (TimePoint_t now = Clock_t::now()) __NE___ { return TimeCast<ToDuration>( Tick( now )); }
        ND_ Duration_t  Tick (TimePoint_t now = Clock_t::now()) __NE___;

        template <typename ToDuration>
        ND_ ToDuration  TimeSince ()                            C_NE___ { return TimeCast<ToDuration>( TimeSince()); }
        ND_ Duration_t  TimeSince ()                            C_NE___ { return Clock_t::now() - _lastTick; }

        ND_ TimePoint_t Now ()                                  C_NE___ { return _lastTick; }
    };


/*
=================================================
    Tick
=================================================
*/
    inline Clock::Duration_t  Clock::Tick (TimePoint_t now) __NE___
    {
        const auto  dt = now - _lastTick;
        _lastTick = now;
        return dt;
    }


} // AE::Base
