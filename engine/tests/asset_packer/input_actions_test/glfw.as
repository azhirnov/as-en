
void main (GLFW_ActionBindings& bindings)
{
	// UI bindings
	{
		GLFW_BindingsMode@	bind = bindings.CreateMode( "UI" );

		bind.Add( GLFW_Input::Enter,
				  ActionInfo( "Enter", EGestureType::Down ));
		
		bind.Add( GLFW_Input::Escape,
				  ActionInfo( "Back", EGestureType::Down ));

		bind.Add( GLFW_Input::MouseLeft,
				  ActionInfo( "Push", EGestureType::Hold ));
		
		bind.Add( GLFW_Input::MouseRight,
				  ActionInfo( "Options", EGestureType::Down ));

		bind.Add( GLFW_Input::CursorPosXY,
				  ActionInfo( "Cursor", EValueType::Float2, EGestureType::Move ));
	}
}
