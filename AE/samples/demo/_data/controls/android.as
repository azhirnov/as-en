// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <input_actions.as>

void ASmain (Android_ActionBindings& bindings)
{
    // Sample Selector bindings
    {
        RC<Android_BindingsMode>    bind = bindings.CreateMode( "SampleSelector" );
    }

    // Canvas2D bindings
    {
        RC<Android_BindingsMode>    bind = bindings.CreateMode( "Canvas2D" );
        bind.Add( Android_Input::TouchPos,
                  ActionInfo( "Enter", EGestureType::DoubleClick ));
        bind.Add( Android_Input::TouchPos,
                  ActionInfo( "Cursor", EValueType::Float2, EGestureType::Move ));
    }

    // imGUI bindings
    {
        RC<Android_BindingsMode>    bind = bindings.CreateMode( "imGUI" );

        bind.Add( Android_Input::TouchPos,
                  ActionInfo( "Touch.Click", EValueType::Float2, EGestureType::Click ));
        bind.Add( Android_Input::TouchPos,
                  ActionInfo( "Touch.Move", EValueType::Float2, EGestureType::Move ));
    }

    // HwCamera bindings
    {
        RC<Android_BindingsMode>    bind = bindings.CreateMode( "HwCamera" );

        bind.Add( Android_Input::TouchPos,
                  ActionInfo( "Touch.Click", EValueType::Float2, EGestureType::Click ));
        bind.Add( Android_Input::TouchPos,
                  ActionInfo( "Touch.Move", EValueType::Float2, EGestureType::Move ));
    }

    // Camera3D bindings
    {
        RC<Android_BindingsMode>    bind = bindings.CreateMode( "Camera3D" );

        bind.Add( Android_Input::TouchDelta_norm,
                  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Move, float4(-2.0f) ));

    //  bind.Add( Android_Input::GameRotationVector,
    //            ActionInfo( "Camera.Rotate3D", EGestureType::Move ));

    //  bind.Add( Android_Input::RotationVector,
    //            ActionInfo( "Camera.Rotate3D", EGestureType::Move ));

    //  bind.Add( Android_Input::Gravity,
    //            ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Move, VecSwizzle("y0"), float4(0.02f) ));

        bind.Add( Android_Input::AirTemperature,
                  ActionInfo( "Camera.Sensor3f", EValueType::Float3, EGestureType::Move, VecSwizzle("xxx") ));
    }
}
