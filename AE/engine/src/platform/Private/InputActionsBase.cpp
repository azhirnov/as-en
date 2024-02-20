// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Private/InputActionsBase.h"

namespace AE::App
{
    using namespace AE::Serializing;

/*
=================================================
    constructor
=================================================
*/
    InputActionsBase::TsDoubleBufferedQueue::TsDoubleBufferedQueue () __NE___
    {
        const Bytes hdr_size    = SizeOf<TsActionQueue::Header> * _MaxHeaders;
        const Bytes data_size   = _DataSizePerHeader * _MaxHeaders;
        const Bytes q_size      = (hdr_size + data_size) * CountOf( _actionQueues );

        _actionsQueueMem = UntypedAllocator::Allocate( q_size );

        Bytes   offset;
        for (usize i = 0; i < CountOf( _actionQueues ); ++i)
        {
            Reconstruct( INOUT _actionQueues[i], TsActionQueue{ _actionsQueueMem + offset, hdr_size, data_size });
            offset += hdr_size + data_size;
        }
        CHECK( offset == q_size );
    }

/*
=================================================
    destructor
=================================================
*/
    InputActionsBase::TsDoubleBufferedQueue::~TsDoubleBufferedQueue () __NE___
    {
        UntypedAllocator::Deallocate( _actionsQueueMem );
    }

/*
=================================================
    _ReadInput
=================================================
*/
    IInputActions::ActionQueueReader  InputActionsBase::TsDoubleBufferedQueue::_ReadInput (const FrameUID frameId) C_NE___
    {
        DEBUG_ONLY(
            const auto  cur_frame = _curFrameId.load();

            // must be current or previous frame
            ASSERT( cur_frame == Default                or
                    cur_frame == frameId                or
                    cur_frame == FrameUID{frameId}.Inc() );
        )
        return ActionQueueReader{ _actionQueues[ frameId.Remap2() ]};
    }

/*
=================================================
    _NextFrame
=================================================
*/
    void  InputActionsBase::TsDoubleBufferedQueue::_NextFrame (const FrameUID frameId) __NE___
    {
        DEBUG_ONLY(
            const auto  cur_frame = _curFrameId.load();

            ASSERT( cur_frame == Default                or
                    FrameUID{cur_frame}.Inc() == frameId );
        )
        _curFrameId.store( frameId );

        CurrentQueue().Reset();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    destructor
=================================================
*/
    InputActionsBase::~InputActionsBase () __NE___
    {}

/*
=================================================
    SetMode
=================================================
*/
    bool  InputActionsBase::SetMode (InputModeName::Ref value) __NE___
    {
        DRC_EXLOCK( _drCheck );

        if_unlikely( not value.IsDefined() )
        {
            _curMode    = null;
            _bindAction = Default;
            _inputMode  = Default;
            return true;
        }

        ASSERT( not _modeMap.empty() );

        auto    it = _modeMap.find( value );
        if_unlikely( it == _modeMap.end() )
        {
            DEBUG_ONLY( AE_LOGE( "InputMode '"s << value.GetName() << "' is not exists" ));
            return false;
        }

        if ( _curMode == &it->second )
            return true;

        _pressedKeys.clear();
        _curMode    = null;
        _bindAction = Default;

        _curMode    = &it->second;
        _inputMode  = value;

        return true;
    }

/*
=================================================
    GetReflection
=================================================
*/
    bool  InputActionsBase::GetReflection (InputModeName::Ref mode, InputActionName::Ref action, OUT Reflection &) C_NE___
    {
        Unused( mode, action );
        // TODO
        return false;
    }

/*
=================================================
    BeginBindAction
=================================================
*/
    bool  InputActionsBase::BeginBindAction (InputModeName::Ref mode, InputActionName::Ref action, EValueType type, EGestureType gesture) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( not _bindAction.isActive );

        _bindAction.isActive = true;

        _bindAction.mode        = mode;
        _bindAction.action      = action;
        _bindAction.valueType   = type;
        _bindAction.gesture     = gesture;

        _bindAction.keys.clear();

        return true;
    }

/*
=================================================
    EndBindAction
=================================================
*/
    bool  InputActionsBase::EndBindAction () __NE___
    {
        DRC_EXLOCK( _drCheck );

        if ( not _bindAction.isActive )
            return false;

        _bindAction.keys.clear();
        return true;
    }

/*
=================================================
    _Reset
=================================================
*/
    void  InputActionsBase::_Reset () __NE___
    {
        _curMode = null;
        _modeMap.clear();
        _pressedKeys.clear();

        _bindAction = Default;
    }

/*
=================================================
    Update
=================================================
*/
    void  InputActionsBase::Update (Duration_t timeSinceStart) __NE___
    {
        DRC_EXLOCK( _drCheck );

        if_unlikely( _curMode == null )
            return;

        auto    key_it = _pressedKeys.begin();

        StaticAssert( uint(EGestureType::Click)+1       == uint(EGestureType::DoubleClick) );
        StaticAssert( uint(EGestureType::DoubleClick)+1 == uint(EGestureType::Hold) );
        StaticAssert( uint(EGestureType::Hold)+1        == uint(EGestureType::LongPress) );

        _Skip< EGestureType::Click >( INOUT key_it );

        _Update< EGestureType::DoubleClick  >( INOUT key_it, timeSinceStart );
        _Update< EGestureType::Hold         >( INOUT key_it, timeSinceStart );
        _Update< EGestureType::LongPress    >( INOUT key_it, timeSinceStart );
    }

/*
=================================================
    _Update
=================================================
*/
    template <EGestureType GestureType>
    void  InputActionsBase::_Skip (INOUT PressedKeys_t::iterator &key_it) __NE___
    {
        for (; key_it != _pressedKeys.end();)
        {
            auto [key, gesture, state_] = _Unpack( key_it->first );

            if_unlikely( gesture > GestureType )
                return;

            ++key_it;
        }
    }

    template <EGestureType GestureType>
    void  InputActionsBase::_Update (INOUT PressedKeys_t::iterator &key_it, Duration_t timeSinceStart) __NE___
    {
        for (; key_it != _pressedKeys.end();)
        {
            Duration_t  dt              = (timeSinceStart - key_it->second.timestamp);
            auto [key, gesture, state_] = _Unpack( key_it->first );     // don't use 'state_' !

            if_unlikely( gesture != GestureType )
                return;

            if constexpr( GestureType == EGestureType::DoubleClick )
            {
                if_unlikely( dt > _DoubleTapMaxDuration ){
                    key_it = _pressedKeys.EraseByIter( key_it );
                    continue;
                }
            }
            else
            {
                auto    act_it = _curMode->actions.find( key_it->first );
                ASSERT( act_it != _curMode->actions.end() );

                if_likely( act_it != _curMode->actions.end() )
                {
                    auto&       info    = act_it->second;
                    const auto  id      = key_it->second.controllerId;
                    const Bytes v_size  = SizeOf<float> * ValueType_ElementCount( info.valueType );

                    ASSERT( info.gesture == gesture );

                    if constexpr( GestureType == EGestureType::Hold )
                    {
                        const auto  v = info.Transform( float4{float(dt.count())} );

                        _dbQueueRef.Insert( info.name, id, EGestureState::Update, &v[0], v_size );
                    }
                    else
                    if constexpr( GestureType == EGestureType::LongPress )
                    {
                        if ( key_it->second.data == 0 )
                        {
                            const auto  v = info.Transform( float4{ Min( 1.f, float(dt.count()) / _LongPressDuration.count() )});   // 0..1

                            if ( dt < _LongPressDuration ){
                                _dbQueueRef.Insert( info.name, id, EGestureState::Update, &v[0], v_size );
                            }else{
                                _dbQueueRef.Insert( info.name, id, EGestureState::End, &v[0], v_size );
                                key_it->second.data = 1;
                            }
                        }
                    }
                }
            }
            ++key_it;
        }
    }

/*
=================================================
    SetQueue
=================================================
*/
    void  InputActionsBase::SetQueue (TsDoubleBufferedQueue* q) __NE___
    {
        DRC_EXLOCK( _drCheck );

        if ( q != null )
            _dbQueueRef = DoubleBufferedQueueRef{ *q };
        else
            _dbQueueRef = DoubleBufferedQueueRef{ _dbQueue };
    }

/*
=================================================
    GetQueue
=================================================
*/
    InputActionsBase::TsDoubleBufferedQueue*  InputActionsBase::GetQueue (IInputActions* act) __NE___
    {
        ASSERT( CastAllowed<InputActionsBase>( act ));

        if ( act != null )
            return Cast<InputActionsBase>( act )->_dbQueueRef.Get();

        return null;
    }


/*
=================================================
    _UpdateKey1
=================================================
*/
    void  InputActionsBase::_UpdateKey1 (ushort type, EGestureState state, ControllerID id, Duration_t timestamp) __NE___
    {
        ASSERT( state != EGestureState::Update );

        _UpdateKey2< EGestureType::Hold         >( ushort(type), state, id, timestamp );
        _UpdateKey2< EGestureType::LongPress    >( ushort(type), state, id, timestamp );
        _UpdateKey2< EGestureType::Down         >( ushort(type), state, id, timestamp );
        _UpdateKey2< EGestureType::Click        >( ushort(type), state, id, timestamp );
        _UpdateKey2< EGestureType::DoubleClick  >( ushort(type), state, id, timestamp );
    }

    template <EGestureType GestureType>
    void  InputActionsBase::_UpdateKey2 (ushort type, EGestureState state, ControllerID id, Duration_t timestamp) __NE___
    {
        const InputKey  key = _Pack( type, GestureType, EGestureState::Update );
        auto            it  = _curMode->actions.find( _Pack( type, GestureType, state ));

        if_likely( it != _curMode->actions.end() )
        {
            auto&       info    = it->second;
            const auto  v       = info.Transform( float4{float(state < EGestureState::End)} );
            const auto  v_size  = SizeOf<float> * ValueType_ElementCount( info.valueType );

            ASSERT( info.gesture == GestureType );

            if constexpr( GestureType == EGestureType::Hold     or
                          GestureType == EGestureType::LongPress )
            {
                if ( state == EGestureState::Begin ){
                    _pressedKeys.emplace( key, PressedKey{ timestamp, id });
                }else{
                    _pressedKeys.EraseByKey( key );
                }
            }
            else
            if constexpr( GestureType == EGestureType::Down )
            {
                if ( state == EGestureState::Begin and _pressedKeys.emplace( key, PressedKey{ timestamp, id }).second ){
                    _dbQueueRef.Insert( info.name, id, EGestureState::End, &v[0], v_size );
                }
                if ( state == EGestureState::End ){
                    _pressedKeys.EraseByKey( key );
                }
            }
            else
            if constexpr( GestureType == EGestureType::Click )
            {
                if ( state == EGestureState::Begin ){
                    _pressedKeys.emplace( key, PressedKey{ timestamp, id });
                }else
                if ( PressedKey pressed;
                     _pressedKeys.Extract( key, OUT pressed ) and (timestamp - pressed.timestamp) <= _ClickMaxDuration )
                {
                    _dbQueueRef.Insert( info.name, id, EGestureState::End, &v[0], v_size );
                }
            }
            else
            if constexpr( GestureType == EGestureType::DoubleClick )
            {
                if ( state == EGestureState::Begin ){
                    _pressedKeys.emplace( key, PressedKey{ timestamp, id });
                }else
                if ( auto key_it = _pressedKeys.find( key );  key_it != _pressedKeys.end() )
                {
                    if ( key_it->second.data == 0 ){
                        _dbQueueRef.Insert( info.name, id, EGestureState::Update, &v[0], v_size );
                        key_it->second.data++;
                    }else{
                        if ( (timestamp - key_it->second.timestamp) <= _DoubleTapMaxDuration )
                            _dbQueueRef.Insert( info.name, id, EGestureState::End, &v[0], v_size );
                        _pressedKeys.EraseByIter( key_it );
                    }
                }
            }
        }

        if_unlikely( _bindAction.isActive )
        {
            _bindAction.keys.insert( key );
        }
    }


} // AE::App
