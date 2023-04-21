#include <input_actions>

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
}
