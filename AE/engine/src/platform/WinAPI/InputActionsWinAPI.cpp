// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_WINAPI_WINDOW
# include "base/Platforms/WindowsHeader.cpp.h"

# include "platform/WinAPI/InputActionsWinAPI.h"
# include "platform/WinAPI/WindowWinAPI.h"

# define USE_RAW_INPUT      1

namespace AE::App
{
namespace
{
/*
=================================================
    GetInputDevices
=================================================
*/
#if USE_RAW_INPUT
    using RawInputDevices_t = FixedArray< RAWINPUTDEVICE, 8 >;

    ND_ static RawInputDevices_t  GetInputDevices ()
    {
        RawInputDevices_t   result;

        // mouse
        {
            auto&   rid = result.emplace_back();
            rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
            rid.usUsage     = HID_USAGE_GENERIC_MOUSE;
            rid.dwFlags     = 0;
            rid.hwndTarget  = 0;
        }

        // keyboard
        {
            auto&   rid = result.emplace_back();
            rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
            rid.usUsage     = HID_USAGE_GENERIC_KEYBOARD;
            rid.dwFlags     = 0;
            rid.hwndTarget  = 0;
        }

        // TODO: sensors
        //  https://docs.microsoft.com/en-us/windows-hardware/drivers/hid/sensor-hid-class-driver

        // TODO: use 'GetRawInputDeviceList' to get supported devices

        return result;
    }
#endif

} // namespace


/*
=================================================
    constructor
=================================================
*/
    InputActionsWinAPI::InputActionsWinAPI (TsDoubleBufferedQueue* q) __NE___ :
        InputActionsBase{ q },
        _touchActive{ false },  _touchBegin{ false }, _touchEnd{ false },
        _gestureRecognizer{ ushort(EInputType::TouchPos),   ushort(EInputType::TouchPos_mm),
                            ushort(EInputType::TouchDelta), ushort(EInputType::TouchDelta_norm), ushort(EInputType::MultiTouch) }
    {}

/*
=================================================
    _SetCursorPos
=================================================
*/
    void  InputActionsWinAPI::_SetCursorPos (const float2 pos) __NE___
    {
        //ASSERT( All( pos < _surfaceSize ));

        constexpr auto  id = ControllerID::Mouse;

        _Update2F( EInputType::CursorPos, EGestureType::Move, id, pos, EGestureState::Update );

        const float2    pos_mm = pos * _pixToMm;
        _Update2F( EInputType::CursorPos_mm, EGestureType::Move, id, pos_mm, EGestureState::Update );

        #if not USE_RAW_INPUT
            const float2    delta = pos - _cursorPosPx;
            _Update2F( EInputType::CursorDelta, EGestureType::Move, id, delta, EGestureState::Update );

            const float2    delta_norm = delta * _toSNorm;
            _Update2F( EInputType::CursorDelta_norm, EGestureType::Move, id, delta_norm, EGestureState::Update );
        #endif

        _cursorPosPx = pos;
    }

/*
=================================================
    _SetCursorDelta
=================================================
*/
    void  InputActionsWinAPI::_SetCursorDelta (const float2 deltaSNorm) __NE___
    {
        constexpr auto  id          = ControllerID::Mouse;
        const float2    delta_pix   = deltaSNorm * _surfaceSize;

        _Update2F( EInputType::CursorDelta,      EGestureType::Move, id, delta_pix,  EGestureState::Update );
        _Update2F( EInputType::CursorDelta_norm, EGestureType::Move, id, deltaSNorm, EGestureState::Update);

        _cursorDeltaSNorm = deltaSNorm;
    }

/*
=================================================
    _SetMouseWheel
=================================================
*/
    void  InputActionsWinAPI::_SetMouseWheel (uint idx, float delta) __NE___
    {
        _mouseWheel[idx] = delta;
    }

/*
=================================================
    Register
=================================================
*/
    bool  InputActionsWinAPI::Register (void* wnd) __NE___
    {
        DRC_EXLOCK( _drCheck );

      #if USE_RAW_INPUT
        auto            devices = GetInputDevices();
        const HWND      hwnd    = BitCast<HWND>( wnd );

        for (auto& rid : devices) {
            rid.hwndTarget = hwnd;
        }

        if ( ::RegisterRawInputDevices( devices.data(), UINT(devices.size()), sizeof(devices[0]) ) == FALSE )  // winxp
        {
            Unused( WindowsUtils::CheckError( "RegisterRawInputDevices failed: ", SourceLoc_Current() ));
            return false;
        }
      #endif

        return true;
    }

/*
=================================================
    Unregister
=================================================
*/
    void  InputActionsWinAPI::Unregister () __NE___
    {
        DRC_EXLOCK( _drCheck );

      #if USE_RAW_INPUT
        auto    devices = GetInputDevices();

        for (auto& rid : devices)
        {
            rid.dwFlags     = RIDEV_REMOVE;
            rid.hwndTarget  = 0;
        }

        CHECK( ::RegisterRawInputDevices( devices.data(), UINT(devices.size()), sizeof(devices[0]) ) != FALSE );  // winxp
      #endif
    }

/*
=================================================
    SetMonitor
=================================================
*/
    void  InputActionsWinAPI::SetMonitor (const uint2 &surfaceSize, const Monitor &monitor) __NE___
    {
        DRC_EXLOCK( _drCheck );

        _surfaceSize    = float2(surfaceSize);
        _toSNorm        = 1.0f / _surfaceSize;
        _pixToMm        = monitor.MillimetersPerPixel();
    }

/*
=================================================
    Update
=================================================
*/
    void  InputActionsWinAPI::Update (Duration_t timeSinceStart) __NE___
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

        if_unlikely( Any( IsNotZero( _mouseWheel )))
        {
            const auto  id = ControllerID::Mouse;
            _Update2F( EInputType::MouseWheel, EGestureType::Move, id, _mouseWheel, EGestureState::Update );
            _mouseWheel = {};
        }
    }

/*
=================================================
    _ProcessMessage
=================================================
*/
    bool  InputActionsWinAPI::ProcessMessage (uint uMsg, usize wParam, ssize lParam, Duration_t timestamp, bool hasFocus) __NE___
    {
        DRC_EXLOCK( _drCheck );

        if_unlikely( _curMode == null )
            return false;

        if_unlikely( not hasFocus )
            return false;

        switch ( uMsg )
        {
          #if USE_RAW_INPUT
            case_likely WM_INPUT :
            {
                // RIM_INPUT     - Input occurred while the application was in the foreground.
                // RIM_INPUTSINK - Input occurred while the application was not in the foreground.
                if ( GET_RAWINPUT_CODE_WPARAM(wParam) != RIM_INPUT )
                    return false;

                // 'GetRawInputData()' - one RAWINPUT structure at a time.
                ubyte   input_data  [60];
                uint    data_size   = sizeof(input_data);
                uint    copied_size = ::GetRawInputData( BitCast<HRAWINPUT>(lParam), RID_INPUT, OUT input_data, &data_size, sizeof(RAWINPUTHEADER) );  // winxp

                if_likely( copied_size != UMax )
                {
                    RAWINPUT*   p_data = Cast<RAWINPUT>(input_data);

                    // KEYBOARD //
                    if_likely( p_data->header.dwType == RIM_TYPEKEYBOARD )
                    {
                        constexpr auto  id      = ControllerID::Keyboard;
                        const auto&     kb      = p_data->data.keyboard;
                        const uint      code    = kb.VKey << (kb.Flags & RI_KEY_E0 ? 8 : 0); //(kb.Flags & RI_KEY_E1 ? 16 : 0));
                        const bool      down    = (kb.Flags & RI_KEY_BREAK) == 0;

                        ASSERT( code >= uint(EInputType::KeyBegin) and code <= uint(EInputType::KeyEnd) );

                        _UpdateKey( EInputType(code), down ? EGestureState::Begin : EGestureState::End, id, timestamp );
                    }

                    // MOUSE //
                    if_likely( p_data->header.dwType == RIM_TYPEMOUSE )
                    {
                        constexpr auto  id          = ControllerID::Mouse;
                        const auto&     mouse       = p_data->data.mouse;
                        const int2      m_last      { mouse.lLastX, mouse.lLastY };
                        const auto      btn_flags   = mouse.usButtonFlags;

                        // TODO: use 'p_data->header.hDevice' to support multiple devices

                        // move
                        if_likely( mouse.usFlags == MOUSE_MOVE_RELATIVE and Any( IsNotZero( m_last )) )
                            _SetCursorDelta( float2(m_last) * 0.01f );

                        if_unlikely( mouse.usFlags == MOUSE_MOVE_ABSOLUTE )     // generated only for virtual display (RDP, etc)
                            _SetCursorPos( _surfaceSize * (float2(m_last) / 65535.0f) );

                        // wheel
                        if_unlikely( btn_flags & RI_MOUSE_WHEEL )
                            _SetMouseWheel( 1, float(BitCast<short>(mouse.usButtonData)) / WHEEL_DELTA );

                        if_unlikely( btn_flags & RI_MOUSE_HWHEEL )
                            _SetMouseWheel( 0, float(BitCast<short>(mouse.usButtonData)) / WHEEL_DELTA );

                        // buttons
                        if_unlikely( AnyBits( btn_flags, RI_MOUSE_BUTTON_1_DOWN | RI_MOUSE_BUTTON_1_UP ))
                        {
                            _touchBegin     = not _touchActive and AllBits( btn_flags, RI_MOUSE_BUTTON_1_DOWN );
                            _touchActive    = AllBits( btn_flags, RI_MOUSE_BUTTON_1_DOWN );
                            _touchEnd       = AllBits( btn_flags, RI_MOUSE_BUTTON_1_UP );

                            _UpdateKey( EInputType::MouseBtn0, (btn_flags & RI_MOUSE_BUTTON_1_DOWN ? EGestureState::Begin : EGestureState::End), id, timestamp );
                        }

                        if_unlikely( AnyBits( btn_flags, RI_MOUSE_BUTTON_2_DOWN | RI_MOUSE_BUTTON_2_UP ))
                            _UpdateKey( EInputType::MouseBtn1, (btn_flags & RI_MOUSE_BUTTON_2_DOWN ? EGestureState::Begin : EGestureState::End), id, timestamp );

                        if_unlikely( AnyBits( btn_flags, RI_MOUSE_BUTTON_3_DOWN | RI_MOUSE_BUTTON_3_UP ))
                            _UpdateKey( EInputType::MouseBtn2, (btn_flags & RI_MOUSE_BUTTON_3_DOWN ? EGestureState::Begin : EGestureState::End), id, timestamp );

                        if_unlikely( AnyBits( btn_flags, RI_MOUSE_BUTTON_4_DOWN | RI_MOUSE_BUTTON_4_UP ))
                            _UpdateKey( EInputType::MouseBtn3, (btn_flags & RI_MOUSE_BUTTON_4_DOWN ? EGestureState::Begin : EGestureState::End), id, timestamp );

                        if_unlikely( AnyBits( btn_flags, RI_MOUSE_BUTTON_5_DOWN | RI_MOUSE_BUTTON_5_UP ))
                            _UpdateKey( EInputType::MouseBtn4, (btn_flags & RI_MOUSE_BUTTON_5_DOWN ? EGestureState::Begin : EGestureState::End), id, timestamp );
                    }
                }else{
                    AE_LOG_DBG( "GetRawInputData failed" );
                }
                return true;
            }
          #endif

            case WM_MOUSEMOVE :
            {
                _SetCursorPos( float2(int2( LOWORD(lParam), HIWORD(lParam) )));
                return true;
            }

            case WM_UNICHAR :
            case WM_CHAR :
            case WM_INPUTLANGCHANGE :   break;  // TODO

          #if not USE_RAW_INPUT
            case WM_KEYDOWN :
            case WM_SYSKEYDOWN :
                break;

            case WM_SYSKEYUP :
            case WM_KEYUP :
                break;

            case WM_MOUSEWHEEL :
            case WM_MOUSEHWHEEL :
            {
                _SetMouseWheel( uMsg == WM_MOUSEWHEEL ? 0 : 1, float(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA );
                return true;
            }

            case WM_LBUTTONDOWN :
            case WM_LBUTTONUP :
            {
                _UpdateKey( EInputType::MouseBtn0, (uMsg == WM_LBUTTONDOWN ? EGestureState::Begin : EGestureState::End) );
                return true;
            }
            case WM_RBUTTONDOWN :
            case WM_RBUTTONUP :
            {
                _UpdateKey( EInputType::MouseBtn1, (uMsg == WM_RBUTTONDOWN ? EGestureState::Begin : EGestureState::End) );
                return true;
            }
            case WM_MBUTTONDOWN :
            case WM_MBUTTONUP :
            {
                _UpdateKey( EInputType::MouseBtn2, (uMsg == WM_MBUTTONDOWN ? EGestureState::Begin : EGestureState::End) );
                return true;
            }
            case WM_XBUTTONDOWN :
            case WM_XBUTTONUP :
            {
                _UpdateKey( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 ? EInputType::MouseBtn3 : EInputType::MouseBtn4,
                            (uMsg == WM_XBUTTONDOWN ? EGestureState::Begin : EGestureState::End) );
                return true;
            }
          #endif

            case WM_POINTERDOWN :
            case WM_POINTERUP :
            case WM_POINTERUPDATE :
            {
                const EGestureState states[] = {
                    EGestureState::Update,  // WM_POINTERUPDATE
                    EGestureState::Begin,   // WM_POINTERDOWN
                    EGestureState::End      // WM_POINTERUP
                };

                float2  pos     = float2{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)} * _pixToMm;
                uint    id      = GET_POINTERID_WPARAM(wParam);
                auto    state   = states[ uMsg - WM_POINTERUPDATE ];

                _gestureRecognizer.SetTouch( id, pos, state, timestamp );
                return true;
            }
        }

        return false;
    }

/*
=================================================
    LoadSerialized
=================================================
*/
    bool  InputActionsWinAPI::LoadSerialized (MemRefRStream &stream) __NE___
    {
        DRC_EXLOCK( _drCheck );
        _Reset();

        return SerializableInputActions::LoadSerialized( OUT _modeMap, _Version, InputActionsWinAPI_Name, stream );
    }

/*
=================================================
    Serialize
=================================================
*/
    bool  InputActionsWinAPI::Serialize (Serializing::Serializer &ser) C_NE___
    {
        DRC_EXLOCK( _drCheck );

        return SerializableInputActions::Serialize( _modeMap, _Version, ser );
    }

/*
=================================================
    Deserialize
=================================================
*/
    bool  InputActionsWinAPI::Deserialize (Serializing::Deserializer &des) __NE___
    {
        DRC_EXLOCK( _drCheck );

        _Reset();

        CHECK_ERR( SerializableInputActions::Deserialize( OUT _modeMap, _Version, des ));
        return true;
    }


} // AE::App

#endif // AE_PLATFORM_WINDOWS and not AE_ENABLE_GLFW
