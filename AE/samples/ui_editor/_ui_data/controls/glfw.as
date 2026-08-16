// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#include <input_actions.as>
#include "Bindings.as"

void ASmain (GLFW_ActionBindings& bindings)
{
	// Editor UI bindings
	{
		RC<GLFW_BindingsMode>	bind = bindings.CreateMode( "Main.UI" );

		bind.Add( GLFW_Input::CursorPos,
				  ActionInfo( "UI.MousePos", EValueType::Float2, EGestureType::Move ));
		bind.Add( GLFW_Input::MouseLeft,
				  ActionInfo( "UI.MouseLBDown", EGestureType::Hold ));
		bind.Add( GLFW_Input::MouseWheel,
				  ActionInfo( "UI.MouseWheel", EValueType::Float2, EGestureType::Move ));

		bind.Add( GLFW_Input::Tab,
				  ActionInfo( "UI.ShowHide", EGestureType::Down ));
		bind.Add( GLFW_Input::F5,
				  ActionInfo( "UI.ReloadScript", EGestureType::Down ));
		bind.Add( GLFW_Input::Escape,
				  ActionInfo( "SwitchInputMode", EGestureType::Down ));
	}

	// UI Screen bindings
	{
		RC<GLFW_BindingsMode>	bind = bindings.CreateMode( "Screen.UI" );

		BindUI( bind );

		bind.Add( GLFW_Input::Escape,
				  ActionInfo( "SwitchInputMode", EGestureType::Down ));
	}
}
