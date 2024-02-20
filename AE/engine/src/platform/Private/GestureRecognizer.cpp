// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Private/GestureRecognizer.h"

namespace AE::App
{
namespace
{
/*
=================================================
    GetFirstTouch
=================================================
*/
    ND_ inline ubyte  GetFirstTouch (uint activeTouches)
    {
        int     idx = BitScanForward( activeTouches );
        return idx >= 0 ? ubyte(idx) : UMax;
    }
}

/*
=================================================
    constructor
=================================================
*/
    GestureRecognizer::GestureRecognizer (ushort touchPosPxCode, ushort touchPosMmCode, ushort touchDeltaPxCode,
                                          ushort touchDeltaNormCode, ushort multiTouchCode) __NE___ :
        _touchPosPxCode{touchPosPxCode},        _touchPosMmCode{touchPosMmCode},
        _touchDeltaPxCode{touchDeltaPxCode},    _touchDeltaNormCode{touchDeltaNormCode},
        _multiTouchCode{multiTouchCode}
    {}

/*
=================================================
    Update
=================================================
*/
    void  GestureRecognizer::Update (Duration_t timestamp, InputActionsBase &ia) __NE___
    {
        const uint  active_count = uint(BitCount( _activeTouches ));

        // touch delta
        if ( active_count == 1 )
        {
            const ubyte     touch_idx   = GetFirstTouch( _activeTouches );
            const auto      state       = _touchStates[touch_idx];
            const Touch&    touch       = _touchData[touch_idx];

            ia._Update2F( _touchDeltaPxCode,   EGestureType::Move, _id, touch.delta,               state );
            ia._Update2F( _touchDeltaNormCode, EGestureType::Move, _id, touch.delta * ia._toSNorm, state );
        }

        // single touch
        _RecognizeDragging( active_count, timestamp, ia );
        _RecognizeTaps( active_count, timestamp, ia );

        // multi touch
        _Recognize2Touch( active_count, timestamp, ia );

        for (const uint i : BitIndexIterate( _activeTouches ))
        {
            if_unlikely( _touchStates[i] == EGestureState::End )
                _activeTouches &= ~(1u << i);
        }
    }

/*
=================================================
    SetTouch
=================================================
*/
    void  GestureRecognizer::SetTouch (const uint id, const float2 pos, const EGestureState state, const Duration_t timestamp) __NE___
    {
        ASSERT( id <= 0xFFFF );
        ASSERT( state <= EGestureState::Outside );
        //ASSERT( All( IsNotZero( pos )));

        for (const uint i : BitIndexIterate( _activeTouches ))
        {
            if_likely( id == uint(_touchIDs[i]) )
            {
                auto&   touch   = _touchData[i];
                touch.delta     = pos - touch.pos;
                touch.pos       = pos;
                _touchStates[i] = state;
                return;
            }
        }

        if_likely( state == EGestureState::Begin )
        {
            const int   i = BitScanForward( ~_activeTouches );
            if_likely( i >= 0 )
            {
                auto&   touch   = _touchData[i];
                touch.delta     = float2{};
                touch.pos       = pos;
                touch.startTime = timestamp;
                touch.startPos  = pos;

                _touchStates[i] = state;
                _touchIDs[i]    = TouchID(id);

                _activeTouches |= (1u << i);
            }
        }
    }

/*
=================================================
    _RecognizeTaps
=================================================
*/
    void  GestureRecognizer::_RecognizeTaps (const uint activeCount, const Duration_t timestamp, InputActionsBase &ia)
    {
        if ( activeCount == 0 )
        {
            //ASSERT( not _tapRecognizer.isActive );
            return;
        }

        const float     MaxDistanceSqr  = 15.0f;    // mm^2
        const ubyte     touch_idx       = GetFirstTouch( _activeTouches );
        const Touch&    touch           = _touchData[touch_idx];
        const auto      state           = _touchStates[touch_idx];
        const float     px_to_mm        = ia._pixToMm;
        const float2    pos_px          = touch.startPos;
        const float2    pos_mm          = touch.startPos * px_to_mm;

        if_likely( not _tapRecognizer.isActive )
        {
            // start
            if_unlikely( activeCount == 1 and state == EGestureState::Begin )
            {
                _tapRecognizer.isActive     = true;
                _tapRecognizer.doubleTap    = ((_tapRecognizer.touchIdx == touch_idx) and (timestamp - _tapRecognizer.lastTapTime) < _DoubleTapMaxDuration);
                _tapRecognizer.touchIdx     = touch_idx;
                _tapRecognizer.lastTapTime  = _tapRecognizer.doubleTap ? _tapRecognizer.lastTapTime : timestamp;

                // touch down event
                ia._Update2F( _touchPosPxCode, EGestureType::Down, _id, pos_px, EGestureState::End );
                ia._Update2F( _touchPosMmCode, EGestureType::Down, _id, pos_mm, EGestureState::End );
            }
            return;
        }

        ASSERT( _tapRecognizer.touchIdx == touch_idx );
        //ASSERT( not _dragRecognizer.IsActive() );

        const float         dist    = DistanceSq( touch.pos * px_to_mm, pos_mm );
        const Duration_t    dt      = (timestamp - touch.startTime);
        const float         factor  = Min( 1.f, float(dt.count()) / _LongPressDuration.count() );

        // cancel on multitouch
        if_unlikely( activeCount > 1 )
        {
            _tapRecognizer.isActive = false;
            return;
        }

        // cancel if difference is too big
        if_unlikely( dist > MaxDistanceSqr )
        {
            _tapRecognizer.isActive = false;

            // start dragging
            _dragRecognizer.type        = EGestureType::Move;
            _dragRecognizer.state       = EGestureState::Update;
            _dragRecognizer.touchIdx    = touch_idx;

            ia._Update2F( _touchPosPxCode, _dragRecognizer.type, _id, pos_px, EGestureState::Begin );
            ia._Update2F( _touchPosMmCode, _dragRecognizer.type, _id, pos_mm, EGestureState::Begin );
            return;
        }

        // cancel if pressed for a long time
        if_unlikely( dt >= _LongPressDuration )
        {
            _tapRecognizer.isActive = false;

            // start dragging
            _dragRecognizer.type        = EGestureType::LongPress_Move;
            _dragRecognizer.state       = EGestureState::Update;
            _dragRecognizer.touchIdx    = touch_idx;

            // trigger long press event
            ia._Update3F( _touchPosPxCode, _dragRecognizer.type, _id, float3{ pos_px, factor }, EGestureState::Begin );
            ia._Update3F( _touchPosMmCode, _dragRecognizer.type, _id, float3{ pos_mm, factor }, EGestureState::Begin );
            return;
        }

        // stop
        if_unlikely( state == EGestureState::End )
        {
            const Duration_t    dt2 = (timestamp - _tapRecognizer.lastTapTime);

            _tapRecognizer.lastTapTime  = timestamp;
            _tapRecognizer.isActive     = false;

            // double tap
            if ( _tapRecognizer.doubleTap and dt2 < _DoubleTapMaxDuration )
            {
                ia._Update2F( _touchPosPxCode, EGestureType::DoubleClick, _id, pos_px, EGestureState::End );
                ia._Update2F( _touchPosMmCode, EGestureType::DoubleClick, _id, pos_mm, EGestureState::End );

                _tapRecognizer.lastTapTime = Duration_t{0}; // to forbid triple tap
            }
            else
            // single tap
            if ( dt < _SingleTapMaxDuration )
            {
                ia._Update2F( _touchPosPxCode, EGestureType::Click, _id, pos_px, EGestureState::End );
                ia._Update2F( _touchPosMmCode, EGestureType::Click, _id, pos_mm, EGestureState::End );
            }
            return;
        }

        ia._Update3F( _touchPosPxCode, EGestureType::LongPress, _id, float3{ pos_px, factor }, EGestureState::Update );
        ia._Update3F( _touchPosMmCode, EGestureType::LongPress, _id, float3{ pos_mm, factor }, EGestureState::Update );
    }

/*
=================================================
    _RecognizeDragging
=================================================
*/
    void  GestureRecognizer::_RecognizeDragging (const uint activeCount, const Duration_t, InputActionsBase &ia)
    {
        if ( activeCount == 0 )
        {
            //ASSERT( not _dragRecognizer.IsActive() );
            return;
        }

        const ubyte     touch_idx = GetFirstTouch( _activeTouches );

        if_likely( not _dragRecognizer.IsActive() )
            return;

        ASSERT( _tapRecognizer.touchIdx == touch_idx );

        const Touch&    touch       = _touchData[touch_idx];
        const auto      state       = _touchStates[touch_idx];
        const float     px_to_mm    = ia._pixToMm;
        const float2    pos_px      = touch.pos;
        const float2    pos_mm      = touch.pos * px_to_mm;

        // stop
        if_unlikely( state == EGestureState::End )
        {
            _dragRecognizer.state = EGestureState::End;
        }

        // cancel on multitouch
        if_unlikely( activeCount > 1 )
        {
            _dragRecognizer.state = EGestureState::End; //Cancel;
        }

        ia._Update2F( _touchPosPxCode, _dragRecognizer.type, _id, pos_px, _dragRecognizer.state );
        ia._Update2F( _touchPosMmCode, _dragRecognizer.type, _id, pos_mm, _dragRecognizer.state );
    }

/*
=================================================
    _Recognize2Touch
=================================================
*/
    void  GestureRecognizer::_Recognize2Touch (const uint activeCount, const Duration_t, InputActionsBase &ia)
    {
        const int       idx0        = BitScanForward( _activeTouches );
        const int       idx1        = BitScanForward( _activeTouches & ~SafeLeftBitShift( 1u, idx0 ));
        const float     px_to_mm    = ia._pixToMm;
        EGestureState   g_state     = EGestureState::Update;

        if_likely( not _twoTouchRecognizer.isActive )
        {
            if_unlikely( activeCount == 2 and
                         (_touchStates[idx0] == EGestureState::Begin or
                          _touchStates[idx1] == EGestureState::Begin) )
            {
                const float2    pos0_mm = _touchData[idx0].pos * px_to_mm;
                const float2    pos1_mm = _touchData[idx1].pos * px_to_mm;

                _twoTouchRecognizer.isActive    = true;
                _twoTouchRecognizer.touchID0    = _touchIDs[idx0];
                _twoTouchRecognizer.touchID1    = _touchIDs[idx1];
                _twoTouchRecognizer.scale       = Distance( pos0_mm, pos1_mm );
                _twoTouchRecognizer.rotate      = float(ATan( pos0_mm.y - pos1_mm.y, pos0_mm.x - pos1_mm.x ));

                const float4    value {0.f, 0.f, _twoTouchRecognizer.scale, _twoTouchRecognizer.rotate};
                ia._Update4F( _multiTouchCode, EGestureType::ScaleRotate2D, _id, value, EGestureState::Begin );
            }
            return;
        }

        const bool  is_valid    = activeCount                   == 2                and
                                  _twoTouchRecognizer.touchID0  == _touchIDs[idx0]  and
                                  _twoTouchRecognizer.touchID1  == _touchIDs[idx1];
        if_likely( is_valid )
        {
            const float2    pos0_mm     = _touchData[idx0].pos * px_to_mm;
            const float2    pos1_mm     = _touchData[idx1].pos * px_to_mm;
            const float2    scale_angle { Distance( pos0_mm, pos1_mm ),
                                          float(ATan( pos0_mm.y - pos1_mm.y, pos0_mm.x - pos1_mm.x )) };
            const float4    value       { _twoTouchRecognizer.scale  - scale_angle.x,
                                          _twoTouchRecognizer.rotate - scale_angle.y,
                                          scale_angle.x, scale_angle.y };

            if ( _touchStates[idx0] == EGestureState::End or
                 _touchStates[idx1] == EGestureState::End )
            {
                _twoTouchRecognizer.isActive = false;
                g_state = EGestureState::End;
            }

            _twoTouchRecognizer.scale   = scale_angle.x;
            _twoTouchRecognizer.rotate  = scale_angle.y;

            ia._Update4F( _multiTouchCode, EGestureType::ScaleRotate2D, _id, value, g_state );
        }
        else
        {
            _twoTouchRecognizer.isActive = false;
            g_state = EGestureState::End;

            ia._Update4F( _multiTouchCode, EGestureType::ScaleRotate2D, _id, float4{0.f}, g_state );
        }
    }


} // AE::App
