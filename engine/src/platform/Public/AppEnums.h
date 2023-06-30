// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/Common.h"

namespace AE::App
{
    using namespace AE::Base;

    enum class EGestureType : ubyte
    {
        // single
        //   state: End
        Down,               // params:                          |   on key down
        Click,              // params:                          |   on key up - may be slower
        DoubleClick,        // params:                          |   on first click - update, on second click - end

        // continious
        //   state: Begin -> Update -> End/Cancel
        Hold,               // params:                          |   
        LongPress,          // params: f1(0..1)                 |
        Move,               // params: f1/2/3/4                 |
        LongPress_Move,     // params: f3(pos, 0..1)            |   long press to start gesture, then move - only for mouse or touch
        ScaleRotate2D,      // params: f2/4(delta, absolute)    |   2 pointer gesture

        _Count,
        Unknown = 0xF,
    };


    enum class EGestureState : ubyte
    {
        Begin,
        Update,
        End,        // used for single event too
        Cancel,     // for Android
        Outside,    // for Android
        Unknown = 0xF
    };


} // AE::App
