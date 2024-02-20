#include <input_actions.as>

void ASmain (Android_ActionBindings& bindings)
{
    // UI bindings
    {
        RC<Android_BindingsMode>    bind = bindings.CreateMode( "UI" );
    }

    // Camera3D bindings
    {
        RC<Android_BindingsMode>    bind = bindings.CreateMode( "Camera3D" );

        bind.Add( Android_Input::TouchDelta_norm,
                  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Move, float4(-2.0f) ));

        bind.Add( Android_Input::GameRotationVector,
                  ActionInfo( "Camera.Rotate3D", EGestureType::Move ));

        bind.Add( Android_Input::Gravity,
                  ActionInfo( "Camera.Gravity", EGestureType::Move ));
    }
}
