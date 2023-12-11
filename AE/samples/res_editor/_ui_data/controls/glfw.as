// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <input_actions.as>

void ASmain (GLFW_ActionBindings& bindings)
{
    // shared input binding
    {
        RC<GLFW_BindingsMode>   bind = bindings.CreateMode( "Shared" );

        bind.Add( GLFW_Input::Backspace,
                  ActionInfo( "PauseRendering", EGestureType::Down ));
        bind.Add( GLFW_Input::F1,
                  ActionInfo( "ShowHelp", EGestureType::Down ));
        bind.Add( GLFW_Input::Tab,
                  ActionInfo( "UI.ShowHide", EGestureType::Down ));
        bind.Add( GLFW_Input::I,
                  ActionInfo( "UI.Screenshot", EGestureType::Down ));
        bind.Add( GLFW_Input::F5,
                  ActionInfo( "UI.ReloadScript", EGestureType::Down ));
        bind.Add( GLFW_Input::F3,
                  ActionInfo( "UI.ResExport", EGestureType::Down ));
        bind.Add( GLFW_Input::F11,
                  ActionInfo( "FullscreenOnOff", EGestureType::Down ));
    }

    // switch input mode
    {
        RC<GLFW_BindingsMode>   bind = bindings.CreateMode( "SwitchInputMode" );
        bind.Inherit( "Shared" );

        bind.Add( GLFW_Input::Escape,
                  ActionInfo( "SwitchInputMode", EGestureType::Down ));

        bind.Add( GLFW_Input::1,
                  ActionInfo( "CustomKey1", EValueType::Float, EGestureType::Down, float4(1.f) ));
        bind.Add( GLFW_Input::2,
                  ActionInfo( "CustomKey1", EValueType::Float, EGestureType::Down, float4(2.f) ));
        bind.Add( GLFW_Input::3,
                  ActionInfo( "CustomKey1", EValueType::Float, EGestureType::Down, float4(3.f) ));
        bind.Add( GLFW_Input::4,
                  ActionInfo( "CustomKey1", EValueType::Float, EGestureType::Down, float4(4.f) ));
        bind.Add( GLFW_Input::5,
                  ActionInfo( "CustomKey1", EValueType::Float, EGestureType::Down, float4(5.f) ));
        bind.Add( GLFW_Input::6,
                  ActionInfo( "CustomKey1", EValueType::Float, EGestureType::Down, float4(6.f) ));
        bind.Add( GLFW_Input::7,
                  ActionInfo( "CustomKey1", EValueType::Float, EGestureType::Down, float4(7.f) ));
        bind.Add( GLFW_Input::8,
                  ActionInfo( "CustomKey1", EValueType::Float, EGestureType::Down, float4(8.f) ));
        bind.Add( GLFW_Input::9,
                  ActionInfo( "CustomKey1", EValueType::Float, EGestureType::Down, float4(9.f) ));
        bind.Add( GLFW_Input::0,
                  ActionInfo( "CustomKey1", EValueType::Float, EGestureType::Down, float4(10.f) ));
    }

    // UI bindings
    {
        RC<GLFW_BindingsMode>   bind = bindings.CreateMode( "Main.UI" );
        bind.Inherit( "Shared" );

        bind.Add( GLFW_Input::Escape,
                  ActionInfo( "UI.SwitchInputMode", EGestureType::Down ));

        bind.Add( GLFW_Input::CursorPos,
                  ActionInfo( "UI.MousePos", EValueType::Float2, EGestureType::Move ));
        bind.Add( GLFW_Input::MouseLeft,
                  ActionInfo( "UI.MouseLBDown", EGestureType::Hold ));
        bind.Add( GLFW_Input::MouseRight,
                  ActionInfo( "UI.MouseRBDown", EGestureType::Hold ));
        bind.Add( GLFW_Input::MouseWheel,
                  ActionInfo( "UI.MouseWheel", EValueType::Float2, EGestureType::Move ));

        bind.Add( GLFW_Input::U,
                  ActionInfo( "UI.StartStopRecording", EGestureType::Down ));
        bind.Add( GLFW_Input::G,
                  ActionInfo( "UI.ShaderDebugger", EGestureType::Down ));
        bind.Add( GLFW_Input::F2,
                  ActionInfo( "UI.FrameCapture", EGestureType::Down ));
    }

    // ScaleBias camera
    {
        RC<GLFW_BindingsMode>   bind = bindings.CreateMode( "Controller.ScaleBias" );
        bind.Inherit( "SwitchInputMode" );

        bind.Add( GLFW_Input::TouchDelta_norm,
                  ActionInfo( "Camera.Bias", EValueType::Float2, EGestureType::Move, float4(1.f, -1.f, 0.f, 0.f) ));
        bind.Add( GLFW_Input::MouseWheel,
                  ActionInfo( "Camera.Scale", EValueType::Float2, EGestureType::Move, 0.1f ));
        bind.Add( GLFW_Input::R,
                  ActionInfo( "Camera.Reset", EGestureType::Down ));
    }

    // TopDown camera
    {
        RC<GLFW_BindingsMode>   bind = bindings.CreateMode( "Controller.TopDown" );
        bind.Inherit( "SwitchInputMode" );

        const float4    arrow_scale (0.01f);

        bind.Add( GLFW_Input::W,
                  ActionInfo( "Camera.Move", EValueType::Float2, EGestureType::Hold, VecSwizzle("0-") ));
        bind.Add( GLFW_Input::S,
                  ActionInfo( "Camera.Move", EValueType::Float2, EGestureType::Hold, VecSwizzle("0+") ));
        bind.Add( GLFW_Input::A,
                  ActionInfo( "Camera.Move", EValueType::Float2, EGestureType::Hold, VecSwizzle("-0") ));
        bind.Add( GLFW_Input::D,
                  ActionInfo( "Camera.Move", EValueType::Float2, EGestureType::Hold, VecSwizzle("+0") ));

        bind.Add( GLFW_Input::Q,
                  ActionInfo( "Camera.Rotate", EValueType::Float, EGestureType::Hold, VecSwizzle("-"), arrow_scale ));
        bind.Add( GLFW_Input::E,
                  ActionInfo( "Camera.Rotate", EValueType::Float, EGestureType::Hold, VecSwizzle("+"), arrow_scale ));

        bind.Add( GLFW_Input::R,
                  ActionInfo( "Camera.Reset", EGestureType::Down ));

        bind.Add( GLFW_Input::CursorPos,
                  ActionInfo( "UI.MousePos", EValueType::Float2, EGestureType::Move ));
        bind.Add( GLFW_Input::MouseRight,
                  ActionInfo( "UI.MouseRBDown", EGestureType::Hold ));
    }

    // Orbital Camera
    {
        RC<GLFW_BindingsMode>   bind = bindings.CreateMode( "Controller.Orbital" );
        bind.Inherit( "SwitchInputMode" );

        const float4    arrow_scale     (0.01f);
        const float     mouse_scale     = 4.0f;

        bind.Add( GLFW_Input::TouchDelta_norm,
                  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Move, float4(1.f, 1.f, 0.f, 0.f) * mouse_scale ));

        bind.Add( GLFW_Input::ArrowLeft,
                  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("-0"), arrow_scale ));
        bind.Add( GLFW_Input::ArrowRight,
                  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("+0"), arrow_scale ));
        bind.Add( GLFW_Input::ArrowUp,
                  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("0-"), arrow_scale ));
        bind.Add( GLFW_Input::ArrowDown,
                  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("0+"), arrow_scale ));

        bind.Add( GLFW_Input::W,
                  ActionInfo( "Camera.Move", EValueType::Float2, EGestureType::Hold, VecSwizzle("0-") ));
        bind.Add( GLFW_Input::S,
                  ActionInfo( "Camera.Move", EValueType::Float2, EGestureType::Hold, VecSwizzle("0+") ));
        bind.Add( GLFW_Input::A,
                  ActionInfo( "Camera.Move", EValueType::Float2, EGestureType::Hold, VecSwizzle("-0") ));
        bind.Add( GLFW_Input::D,
                  ActionInfo( "Camera.Move", EValueType::Float2, EGestureType::Hold, VecSwizzle("+0") ));

        bind.Add( GLFW_Input::MouseWheel,
                  ActionInfo( "Camera.Zoom", EValueType::Float2, EGestureType::Move ));

        bind.Add( GLFW_Input::R,
                  ActionInfo( "Camera.Reset", EGestureType::Down ));
    }

    // Camera 3D
    {
        RC<GLFW_BindingsMode>   bind = bindings.CreateMode( "Controller.Camera3D" );
        bind.Inherit( "SwitchInputMode" );
        bind.LockAndHideCursor();

        const float4    arrow_scale     (0.01f);
        const float     mouse_scale     = 4.0f;

        bind.Add( GLFW_Input::W,
                  ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("+00") ));
        bind.Add( GLFW_Input::S,
                  ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("-00") ));
        bind.Add( GLFW_Input::A,
                  ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("0-0") ));
        bind.Add( GLFW_Input::D,
                  ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("0+0") ));
        bind.Add( GLFW_Input::LeftShift,
                  ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("00+") ));
        bind.Add( GLFW_Input::Space,
                  ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("00-") ));

        bind.Add( GLFW_Input::CursorDelta_norm,
                  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Move, float4(1.f, 1.f, 0.f, 0.f) * mouse_scale ));
        bind.Add( GLFW_Input::MouseWheel,
                  ActionInfo( "Camera.Zoom", EValueType::Float2, EGestureType::Move ));
        bind.Add( GLFW_Input::R,
                  ActionInfo( "Camera.Reset", EGestureType::Down ));

        bind.Add( GLFW_Input::ArrowLeft,
                  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("-0"), arrow_scale ));
        bind.Add( GLFW_Input::ArrowRight,
                  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("+0"), arrow_scale ));
        bind.Add( GLFW_Input::ArrowUp,
                  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("0+"), arrow_scale ));
        bind.Add( GLFW_Input::ArrowDown,
                  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("0-"), arrow_scale ));
    }

    // FPS camera
    {
        RC<GLFW_BindingsMode>   bind = bindings.CreateMode( "Controller.FPSCamera" );
        bind.Inherit( "Controller.Camera3D" );
    }

    // Free camera
    {
        RC<GLFW_BindingsMode>   bind = bindings.CreateMode( "Controller.FPVCamera" );
        bind.Inherit( "Controller.Camera3D" );
    }

    // Flight camera
    {
        RC<GLFW_BindingsMode>   bind = bindings.CreateMode( "Controller.FlightCamera" );
        bind.Inherit( "SwitchInputMode" );
        bind.LockAndHideCursor();

        const float4    yaw_scale       (0.005f);
        const float4    pitch_scale     (0.02f);
        const float4    roll_scale      (0.02f);
        const float4    thrust_scale    (0.01f);
        const float     mouse_scale     = 4.0f;

        // yaw
        bind.Add( GLFW_Input::Q,
                  ActionInfo( "FlightCamera.Rotate", EValueType::Float3, EGestureType::Hold, VecSwizzle("-00"), yaw_scale ));
        bind.Add( GLFW_Input::E,
                  ActionInfo( "FlightCamera.Rotate", EValueType::Float3, EGestureType::Hold, VecSwizzle("+00"), yaw_scale ));

        // pitch
        bind.Add( GLFW_Input::W,
                  ActionInfo( "FlightCamera.Rotate", EValueType::Float3, EGestureType::Hold, VecSwizzle("0+0"), pitch_scale ));
        bind.Add( GLFW_Input::S,
                  ActionInfo( "FlightCamera.Rotate", EValueType::Float3, EGestureType::Hold, VecSwizzle("0-0"), pitch_scale ));

        // roll
        bind.Add( GLFW_Input::A,
                  ActionInfo( "FlightCamera.Rotate", EValueType::Float3, EGestureType::Hold, VecSwizzle("00-"), roll_scale ));
        bind.Add( GLFW_Input::D,
                  ActionInfo( "FlightCamera.Rotate", EValueType::Float3, EGestureType::Hold, VecSwizzle("00+"), roll_scale ));

        // roll, pitch
        bind.Add( GLFW_Input::CursorDelta_norm,
                  ActionInfo( "FlightCamera.Rotate", EValueType::Float3, EGestureType::Move, VecSwizzle("0yx"), mouse_scale ));

        bind.Add( GLFW_Input::T,
                  ActionInfo( "FlightCamera.ResetRoll", EGestureType::Down ));
        bind.Add( GLFW_Input::R,
                  ActionInfo( "FlightCamera.Reset", EGestureType::Down ));

        bind.Add( GLFW_Input::MouseWheel,
                  ActionInfo( "FlightCamera.Zoom", EValueType::Float2, EGestureType::Move ));

        bind.Add( GLFW_Input::Z,
                  ActionInfo( "FlightCamera.Thrust", EValueType::Float, EGestureType::Hold, VecSwizzle("-"), thrust_scale ));
        bind.Add( GLFW_Input::C,
                  ActionInfo( "FlightCamera.Thrust", EValueType::Float, EGestureType::Hold, VecSwizzle("+"), thrust_scale ));
    }
}
