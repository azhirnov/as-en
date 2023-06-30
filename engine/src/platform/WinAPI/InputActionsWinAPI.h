// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/WinAPI/WinAPICommon.h"

#ifdef AE_WINAPI_WINDOW
# include "platform/Private/InputActionsBase.h"
# include "platform/Private/GestureRecognizer.h"
# include "platform/WinAPI/SerializableInputActionsWinAPI.h"

namespace AE::App
{

    //
    // Input Actions for WinAPI
    //

    class InputActionsWinAPI final : public InputActionsBase
    {
    // types
    private:
        using EInputType = SerializableInputActionsWinAPI::EInputType;

        static constexpr uint   _Version = SerializableInputActionsWinAPI::_Version;


    // variables
    private:
        DubleBufferedQueue  _dbQueue;

        float2              _cursorPosPx;
        float2              _cursorDeltaSNorm;
        float2              _mouseWheel;

        bool                _touchActive        : 1;
        bool                _touchBegin         : 1;
        bool                _touchEnd           : 1;

        float2              _surfaceSize;

        GestureRecognizer   _gestureRecognizer;


    // methods
    public:
        explicit InputActionsWinAPI (DubleBufferedQueue *q)                 __NE___;

        ND_ bool  ProcessMessage (uint uMsg, usize wParam, ssize lParam,
                                  Duration_t timestamp, bool hasFocus)      __NE___;
        ND_ bool  Register (void* wnd)                                      __NE___;
            void  Unregister ()                                             __NE___;
            void  SetMonitor (const uint2 &surfaceSize, const Monitor &)    __NE___;
            void  CursorPosChanged (float2 pos)                             __NE___ { _cursorPosPx = pos; }

            void  Update (Duration_t timeSinceStart)                        __NE___;


    // IInputActions //
        bool  LoadSerialized (MemRefRStream &stream)                        __NE_OV;


    // ISerializable //
        bool  Serialize (Serializing::Serializer &)                         C_NE_OV;
        bool  Deserialize (Serializing::Deserializer &)                     __NE_OV;


    private:
        void  _SetCursorPos (float2 pos)                                    __NE___;
        void  _SetCursorDelta (float2 pos)                                  __NE___;
        void  _SetMouseWheel (uint idx, float delta)                        __NE___;

        ND_ static constexpr bool  _IsKey (EInputType type)                 __NE___ { return SerializableInputActionsWinAPI::_IsKey( type ); }
        ND_ static constexpr bool  _IsCursor1D (EInputType type)            __NE___ { return SerializableInputActionsWinAPI::_IsCursor1D( type ); }
        ND_ static constexpr bool  _IsCursor2D (EInputType type)            __NE___ { return SerializableInputActionsWinAPI::_IsCursor2D( type ); }
    };


} // AE::App

#endif // AE_WINAPI_WINDOW
