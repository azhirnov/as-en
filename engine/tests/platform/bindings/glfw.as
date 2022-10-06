
void main (ActionBindings& bindings)
{
	// UI bindings
	{
		BindingsMode@	bind = bindings.CreateMode( "UI" );

		bind.Add( GLFW_Input::Enter,
				  ActionInfo( "Enter", EGestureType::Down ));
	}

	// Game bindings
	{
		BindingsMode@	bind = bindings.CreateMode( "Game" );
	}
	
	// Player bindings
	{
		BindingsMode@	bind = bindings.CreateMode( "Player" );
		bind.Inherit( "Game" );
	}
	
	// Vehicle bindings
	{
		BindingsMode@	bind = bindings.CreateMode( "Vehicle" );
		bind.Inherit( "Game" );
	}
}
