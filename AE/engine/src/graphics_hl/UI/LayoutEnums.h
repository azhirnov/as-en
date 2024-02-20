// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_hl/UI/Common.h"

namespace AE::UI
{

    enum class EStyleState : ubyte
    {
        Unknown     = 0,
        Invisible   = 1 << 0,   //
        Disabled    = 1 << 1,   //
        MouseOver   = 1 << 2,   //
        TouchDown   = 1 << 3,   //
        Selected    = 1 << 4,   // for checkbox and other
        Moving      = 1 << 5,   // layout moving, align to pixels must be disabled
        Focused     = 1 << 6,   //
        _Last,
        _All        = ((_Last - 1) << 1) - 1,
        _Inherited  = Invisible | Disabled | Moving,
    };
    AE_BIT_OPERATORS( EStyleState );


    enum class EStackOrigin : ubyte
    {
        Left,       // horizontal stack, from left to right
        Right,      // horizontal stack, from right to left
        Bottom,     // vertical stack, from bottom to top
        Top,        // vertical stack, from top to bottom
        Unknown     = 0xFF,
    };


    enum class EDirection : ubyte
    {
        Horizontal,
        Vertical,
        _Count,
        Unknown     = 0xFF,
    };


    enum class ELayoutAlign : ubyte
    {
        Unknown     = 0,
        Left        = 1 << 0,
        Right       = 1 << 1,
        Bottom      = 1 << 2,
        Top         = 1 << 3,
        CenterX     = 1 << 4,
        CenterY     = 1 << 5,

        FillX       = Left | Right,
        FillY       = Bottom | Top,
        Center      = CenterX | CenterY,
        Fill        = FillX | FillY,
    };
    AE_BIT_OPERATORS( ELayoutAlign );


    enum class EEdge : ubyte
    {
        Left,
        Right,
        Bottom,
        Top,
        _Count
    };


} // AE::UI
