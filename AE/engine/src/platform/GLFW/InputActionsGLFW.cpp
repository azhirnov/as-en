// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_GLFW
# include "platform/GLFW/InputActionsGLFW.h"

# include "GLFW/glfw3.h"

namespace AE::App
{

/*
=================================================
    constructor
=================================================
*/
    InputActionsGLFW::InputActionsGLFW (DubleBufferedQueue* q) __NE___ :
        InputActionsBase{ q != null ? *q : _dbQueue },
        _touchActive{ false },  _touchBegin{ false }, _touchEnd{ false },
        _gestureRecognizer{ ushort(EInputType::TouchPos),   ushort(EInputType::TouchPos_mm),
                            ushort(EInputType::TouchDelta), ushort(EInputType::TouchDelta_norm), ushort(EInputType::MultiTouch) }
    {}

/*
=================================================
    SetKey
----
    keys in range [GLFW_KEY_SPACE .. GLFW_KEY_LAST]
=================================================
*/
    void  InputActionsGLFW::SetKey (int key, EGestureState state, Duration_t timestamp) __NE___
    {
        DRC_EXLOCK( _drCheck );

        if_unlikely( _curMode == null )
            return;

        _UpdateKey( EInputType(key), state, ControllerID::Keyboard, timestamp );
    }

/*
=================================================
    SetMouseButton
----
    buttons in range [GLFW_MOUSE_BUTTON_LEFT .. GLFW_MOUSE_BUTTON_8]
=================================================
*/
    void  InputActionsGLFW::SetMouseButton (int button, EGestureState state, Duration_t timestamp) __NE___
    {
        DRC_EXLOCK( _drCheck );

        if_unlikely( _curMode == null )
            return;

        if_likely( button == GLFW_MOUSE_BUTTON_LEFT )
        {
            _touchBegin     = not _touchActive and (state == EGestureState::Begin);
            _touchActive    = (state == EGestureState::Begin) or (state == EGestureState::Update);
            _touchEnd       = (state >= EGestureState::End);
        }
        _UpdateKey( EInputType(button), state, ControllerID::Mouse, timestamp );
    }

/*
=================================================
    SetCursorPos
=================================================
*/
    void  InputActionsGLFW::SetCursorPos (const float2 pos) __NE___
    {
        DRC_EXLOCK( _drCheck );

        if_unlikely( _curMode == null )
            return;

        const auto  id = ControllerID::Mouse;

        _Update2F( EInputType::CursorPos, EGestureType::Move, id, pos, EGestureState::Update );

        const float2    pos_mm = pos * _pixToMm;
        _Update2F( EInputType::CursorPos_mm, EGestureType::Move, id, pos_mm, EGestureState::Update );

        const float2    delta = pos - _cursorPosPx;
        _Update2F( EInputType::CursorDelta, EGestureType::Move, id, delta, EGestureState::Update );

        const float2    delta_norm = delta * _toSNorm;
        _Update2F( EInputType::CursorDelta_norm, EGestureType::Move, id, delta_norm, EGestureState::Update );

        _cursorPosPx    = pos;  // may be changed in 'CursorPosChanged()'
        _cursorDeltaPx  = delta;
    }

/*
=================================================
    SetMouseWheel
=================================================
*/
    void  InputActionsGLFW::SetMouseWheel (const float2 delta) __NE___
    {
        DRC_EXLOCK( _drCheck );

        if_unlikely( _curMode == null )
            return;

        const auto  id = ControllerID::Mouse;

        _Update2F( EInputType::MouseWheel, EGestureType::Move, id, delta, EGestureState::Update );
    }

/*
=================================================
    SetMonitor
=================================================
*/
    void  InputActionsGLFW::SetMonitor (const uint2 &surfaceSize, const Monitor &monitor) __NE___
    {
        DRC_EXLOCK( _drCheck );

        _toSNorm    = 1.0f / float2{surfaceSize};
        _pixToMm    = monitor.MillimetersPerPixel();
    }

/*
=================================================
    Update
=================================================
*/
    void  InputActionsGLFW::Update (Duration_t timeSinceStart) __NE___
    {
        InputActionsBase::Update( timeSinceStart );

        if_unlikely( _touchActive | _touchBegin | _touchEnd )
        {
            const EGestureState     state = _touchEnd ? EGestureState::End : (_touchBegin ? EGestureState::Begin : EGestureState::Update);

            _gestureRecognizer.SetTouch( 0, _cursorPosPx, state, timeSinceStart );
        }

        _touchBegin = false;
        _touchEnd   = false;

        _gestureRecognizer.Update( timeSinceStart, *this );
    }

/*
=================================================
    LoadSerialized
=================================================
*/
    bool  InputActionsGLFW::LoadSerialized (MemRefRStream &stream) __NE___
    {
        DRC_EXLOCK( _drCheck );
        _Reset();

        CHECK_ERR( SerializableInputActions::LoadSerialized( OUT _modeMap, _Version, InputActionsGLFW_Name, stream ));

        if ( _vrEmulation )
            CHECK( _ValidateForVREmulation() );

        return true;
    }

/*
=================================================
    Serialize
=================================================
*/
    bool  InputActionsGLFW::Serialize (Serializing::Serializer &ser) C_NE___
    {
        DRC_EXLOCK( _drCheck );

        // TODO: remove VR emulation

        return SerializableInputActions::Serialize( _modeMap, _Version, ser );
    }

/*
=================================================
    Deserialize
=================================================
*/
    bool  InputActionsGLFW::Deserialize (Serializing::Deserializer &des) __NE___
    {
        DRC_EXLOCK( _drCheck );

        _Reset();

        CHECK_ERR( SerializableInputActions::Deserialize( OUT _modeMap, _Version, des ));

        if ( _vrEmulation )
            CHECK( _ValidateForVREmulation() );

        return true;
    }

/*
=================================================
    _ValidateForVREmulation
=================================================
*/
    bool  InputActionsGLFW::_ValidateForVREmulation ()
    {
        CHECK_ERR( not _modeMap.empty() );

        for (auto [name, mode] : _modeMap)
        {
            mode.lockAndHideCursor = false;

            const auto  EraseInputKey = [m = &mode] (EInputType type)
            {{
                m->actions.erase( _Pack( type, EGestureType::Move, EGestureState::Begin   ));
                m->actions.erase( _Pack( type, EGestureType::Move, EGestureState::Update  ));
                m->actions.erase( _Pack( type, EGestureType::Move, EGestureState::End     ));
                m->actions.erase( _Pack( type, EGestureType::Move, EGestureState::Cancel  ));
                m->actions.erase( _Pack( type, EGestureType::Move, EGestureState::Outside ));
            }};

            EraseInputKey( EInputType::CursorPos );
            EraseInputKey( EInputType::CursorPos_mm );
            EraseInputKey( EInputType::CursorDelta );
            EraseInputKey( EInputType::CursorDelta_norm );
            EraseInputKey( EInputType::TouchPos );
            EraseInputKey( EInputType::TouchPos_mm );
            EraseInputKey( EInputType::TouchDelta );
            EraseInputKey( EInputType::TouchDelta_norm );

            {
                ActionInfo  info;
                info.name       = InputActionName{".VRE.Rotate"};
                info.valueType  = EValueType::Float2;
                info.gesture    = EGestureType::Move;

                mode.actions.insert_or_assign( _Pack( EInputType::TouchDelta_norm, EGestureType::Move, EGestureState::Update ), info );
            }
        }
        return true;
    }


} // AE::App

#endif // AE_ENABLE_GLFW
