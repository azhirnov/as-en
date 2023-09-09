// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Private/SerializableInputActions.h"
#include "platform/Public/IApplication.h"

namespace AE::App
{

    //
    // Input Actions base implementation
    //

    class InputActionsBase : public IInputActions
    {
        friend class GestureRecognizer;

    // types
    protected:
        using Duration_t    = IApplication::Duration_t;

        using InputKey      = SerializableInputActions::InputKey;
        using ActionInfo    = SerializableInputActions::ActionInfo;
        using ActionMap_t   = SerializableInputActions::ActionMap_t;
        using InputMode     = SerializableInputActions::InputMode;
        using ModeMap_t     = SerializableInputActions::ModeMap_t;

        static constexpr uint       _MaxActionsPerMode      = SerializableInputActions::_MaxActionsPerMode;
        static constexpr uint       _MaxHeaders             = 2000;
        static constexpr Bytes      _DataSizePerHeader      {8};

        static constexpr Duration_t _LongPressDuration      {2000};     // milliseconds
        static constexpr Duration_t _DoubleTapMaxDuration   {400};
        static constexpr Duration_t _ClickMaxDuration       {400};


        struct PressedKey
        {
            Duration_t      timestamp       {0};
            ControllerID    controllerId    = ControllerID::Default;
            ushort          data            = 0;
        };
        using PressedKeys_t = FixedMap< InputKey, PressedKey, 32 >;


        struct BindAction
        {
            bool                    isActive    = false;

            // from request
            InputModeName           mode;
            InputActionName         action;
            EValueType              valueType   = Default;
            EGestureType            gesture     = Default;

            // result
            FixedSet< InputKey, 8 > keys;
        };


        //
        // Duble Buffered Queue
        //
        class DubleBufferedQueue
        {
        // variables
        private:
            Graphics::AtomicFrameUID    _curFrameId;
            ActionQueue                 _actionQueues [2];
            void*                       _actionsQueueMem    = null;


        // methods
        public:
            DubleBufferedQueue ()                                                                                               __NE___;
            ~DubleBufferedQueue ()                                                                                              __NE___;

            bool  Insert (const InputActionName &name, ControllerID id, EGestureState state, const void* data, Bytes dataSize)  __NE___ { return CurrentQueue().Insert( name, id, state, data, dataSize ); }
            bool  Insert (const InputActionName &name, ControllerID id, EGestureState state)                                    __NE___ { return CurrentQueue().Insert( name, id, state ); }

            ND_ ActionQueue&  CurrentQueue ()                                                                                   __NE___ { return _actionQueues[ _curFrameId.load().Remap2() ]; }

            void  _NextFrame (FrameUID frameId)                                                                                 __NE___;

            ND_ ActionQueueReader  _ReadInput (FrameUID frameId)                                                                C_NE___;
        };


        class DubleBufferedQueueRef
        {
        // variables
        private:
            Ptr<DubleBufferedQueue>     _ref;

        // methods
        public:
            DubleBufferedQueueRef (DubleBufferedQueue &q)                                                                       __NE___ : _ref{&q} {}
            DubleBufferedQueueRef (DubleBufferedQueueRef &&other)                                                               __NE___ : _ref{other._ref} { other._ref = null; }

            void  operator = (const DubleBufferedQueueRef &rhs)                                                                 __NE___ { _ref = rhs._ref; }

            void  NextFrame (FrameUID frameId)                                                                                  __NE___ { _ref->_NextFrame( frameId ); }

            ND_ ActionQueueReader  ReadInput (FrameUID frameId)                                                                 C_NE___ { return _ref->_ReadInput( frameId ); }

            ND_ DubleBufferedQueue*         Get ()                                                                              __NE___ { return _ref.get(); }
            ND_ DubleBufferedQueue const*   Get ()                                                                              C_NE___ { return _ref.get(); }

            template <typename T>
            bool  Insert (const InputActionName &name, ControllerID id, EGestureState state, const T &data)                     __NE___ { return _ref->Insert( name, id, state, &data, Sizeof(data) ); }
            bool  Insert (const InputActionName &name, ControllerID id, EGestureState state, const void* data, Bytes dataSize)  __NE___ { return _ref->Insert( name, id, state, data, dataSize ); }
            bool  Insert (const InputActionName &name, ControllerID id, EGestureState state)                                    __NE___ { return _ref->Insert( name, id, state ); }
        };


    // variables
    protected:
        Ptr<InputMode const>        _curMode;
        InputModeName               _inputMode;

        DubleBufferedQueueRef       _dbQueueRef;

        PressedKeys_t               _pressedKeys;

        ModeMap_t                   _modeMap;

        BindAction                  _bindAction;

        float2                      _toSNorm;       // 1/px
        float2                      _pixToMm;       // mm/px

        bool                        _vrEmulation    = false;

        DRC_ONLY(
            DataRaceCheck           _drCheck;
        )


    // methods
    public:
        explicit InputActionsBase (DubleBufferedQueue &q)                                                       __NE___ : _dbQueueRef{q} {}
        ~InputActionsBase ()                                                                                    __NE_OV;

            void  Update (Duration_t timeSinceStart)                                                            __NE___;

        ND_ bool  RequiresLockAndHideCursor ()                                                                  C_NE___;

        ND_ static DubleBufferedQueue*  GetQueue (IInputActions* act)                                           __NE___;

            void EnableVREmulation ()                                                                           __NE___ { DRC_EXLOCK( _drCheck );  _vrEmulation = true; }


    // IInputActions //
        void  NextFrame (FrameUID frameId)                                                                      __NE_OV { _dbQueueRef.NextFrame( frameId ); }

        ActionQueueReader   ReadInput (FrameUID frameId)                                                        C_NE_OV { return _dbQueueRef.ReadInput( frameId ); }

        InputModeName       GetMode ()                                                                          C_NE_OV { DRC_EXLOCK( _drCheck );  return _inputMode; }

        bool  SetMode (const InputModeName &value)                                                              __NE_OV;

        bool  GetReflection (const InputModeName &mode, const InputActionName &action, OUT Reflection &)        C_NE_OV;

        bool  BeginBindAction (const InputModeName &mode, const InputActionName &action,
                               EValueType type, EGestureType gesture)                                           __NE_OV;
        bool  EndBindAction ()                                                                                  __NE_OV;
        bool  IsBindActionActive ()                                                                             C_NE_OV { DRC_EXLOCK( _drCheck );  return _bindAction.isActive; }


    protected:
        void  _Reset ()                                                                                         __NE___;

        template <typename T>
        ND_ static constexpr InputKey   _Pack (T key,
                                               EGestureType gesture,
                                               EGestureState state = EGestureState::Update)                     __NE___ { return SerializableInputActions::_Pack( key, gesture, state ); }

        ND_ static constexpr auto       _Unpack (InputKey key)                                                  __NE___ -> Tuple< uint, EGestureType, EGestureState > { return SerializableInputActions::_Unpack( key ); }


        template <typename T> void  _Update1F (T type, EGestureType gesture, ControllerID id,
                                               float  value, EGestureState state)                               __NE___ { _Update4F( type, gesture, id, float4{value},           state ); }
        template <typename T> void  _Update2F (T type, EGestureType gesture, ControllerID id,
                                               float2 value, EGestureState state)                               __NE___ { _Update4F( type, gesture, id, float4{value, 0.f, 0.f}, state ); }
        template <typename T> void  _Update3F (T type, EGestureType gesture, ControllerID id,
                                               float3 value, EGestureState state)                               __NE___ { _Update4F( type, gesture, id, float4{value, 0.f},      state ); }
        template <typename T> void  _Update4F (T type, EGestureType gesture, ControllerID id,
                                               float4 value, EGestureState state)                               __NE___;

        template <typename T>
        void  _UpdateKey (T type, EGestureState state, ControllerID id, Duration_t timestamp)                   __NE___ { _UpdateKey1( ushort(type), state, id, timestamp ); }

        ND_ bool  _IsActiveQueue (const DubleBufferedQueue &q)                                                  C_NE___ { return _dbQueueRef.Get() == &q; }

    private:
        template <EGestureType GT>
        void  _Update (INOUT PressedKeys_t::iterator &, Duration_t timeSinceStart);

        template <EGestureType GT>
        void  _Skip (INOUT PressedKeys_t::iterator &);

        void  _UpdateKey1 (ushort type, EGestureState state, ControllerID id, Duration_t timestamp)             __NE___;

        template <EGestureType GT>
        void  _UpdateKey2 (ushort type, EGestureState state, ControllerID id, Duration_t timestamp)             __NE___;
    };
//-----------------------------------------------------------------------------



/*
=================================================
    ValueType_ElementCount
=================================================
*/
    ND_ constexpr uint  ValueType_ElementCount (IInputActions::EValueType type) __NE___
    {
        using EValueType = IInputActions::EValueType;
        BEGIN_ENUM_CHECKS();
        switch ( type )
        {
            case EValueType::Bool :
            case EValueType::Int :
            case EValueType::Float :    return 1;
            case EValueType::Float2 :   return 2;
            case EValueType::Float3 :   return 3;
            case EValueType::Float4 :   return 4;
            case EValueType::Quat :
            case EValueType::Float4x4 :
            case EValueType::Unknown :
            case EValueType::String :   break;  // default
            case EValueType::_Count :
            default :                   DBG_WARNING( "unknown value type" ); break;
        }
        END_ENUM_CHECKS();
        return 0;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _Update4F
=================================================
*/
    template <typename T>
    void  InputActionsBase::_Update4F (T type, EGestureType gesture, ControllerID id, float4 value, EGestureState state) __NE___
    {
        auto    it = _curMode->actions.find( _Pack( type, gesture, state ));
        if_unlikely( it != _curMode->actions.end() )
        {
            auto&       info    = it->second;
            const auto  v       = info.Transform( value );
            const auto  v_size  = SizeOf<float> * ValueType_ElementCount( info.valueType );

            ASSERT( info.valueType >= EValueType::Float2 and info.valueType <= EValueType::Float4 );

            _dbQueueRef.Insert( info.name, id, state, &v[0], v_size );
        }
    }

/*
=================================================
    RequiresLockAndHideCursor
=================================================
*/
    inline bool  InputActionsBase::RequiresLockAndHideCursor () C_NE___
    {
        if_likely( _curMode )
        {
            return _curMode->lockAndHideCursor;
        }
        return false;
    }


} // AE::App
