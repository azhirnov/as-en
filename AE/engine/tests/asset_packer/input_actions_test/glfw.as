#include <input_actions.as>

void ASmain (GLFW_ActionBindings& bindings)
{
	// UI bindings
	{
		RC<GLFW_BindingsMode>	bind = bindings.CreateMode( "UI" );
		bind.LockAndHideCursor();

		bind.Add( GLFW_Input::Enter,
				  ActionInfo( "Enter", EGestureType::Down ));

		bind.Add( GLFW_Input::Escape,
				  ActionInfo( "Back", EGestureType::Down ));

		bind.Add( GLFW_Input::MouseLeft,
				  ActionInfo( "Push", EValueType::Float4, EGestureType::Hold, VecSwizzle("0100") ));

		bind.Add( GLFW_Input::MouseRight,
				  ActionInfo( "Options", EGestureType::Down ));

		bind.Add( GLFW_Input::CursorPos,
				  ActionInfo( "Cursor", EValueType::Float2, EGestureType::Move, float4(0.5, 0.25, 0.0, 0.0) ));
	}

	// Camera3D bindings
	{
		RC<GLFW_BindingsMode>	bind = bindings.CreateMode( "Camera3D" );

		bind.Add( GLFW_Input::TouchDelta_norm,
				  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Move, float4(-2.0f) ));

		bind.Add( GLFW_Input::GameRotationVector,
				  ActionInfo( "Camera.Rotate3D", EGestureType::Move ));

		bind.Add( GLFW_Input::Gravity,
				  ActionInfo( "Camera.Gravity", EGestureType::Move ));
	}
}
