// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#include <input_actions.as>
#include "Bindings.as"

void ASmain (WinAPI_ActionBindings& bindings)
{
	// Editor UI bindings
	{
		RC<WinAPI_BindingsMode>	bind = bindings.CreateMode( "Main.UI" );

		bind.Add( WinAPI_Input::CursorPos,
				  ActionInfo( "UI.MousePos", EValueType::Float2, EGestureType::Move ));
		bind.Add( WinAPI_Input::MouseLeft,
				  ActionInfo( "UI.MouseLBDown", EGestureType::Hold ));
		bind.Add( WinAPI_Input::MouseWheel,
				  ActionInfo( "UI.MouseWheel", EValueType::Float2, EGestureType::Move ));

		bind.Add( WinAPI_Input::Tab,
				  ActionInfo( "UI.ShowHide", EGestureType::Down ));
		bind.Add( WinAPI_Input::F5,
				  ActionInfo( "UI.ReloadScript", EGestureType::Down ));
		bind.Add( WinAPI_Input::Escape,
				  ActionInfo( "SwitchInputMode", EGestureType::Down ));
	}

	// UI Screen bindings
	{
		RC<WinAPI_BindingsMode>	bind = bindings.CreateMode( "Screen.UI" );

		BindUI( bind );

		bind.Add( WinAPI_Input::Escape,
				  ActionInfo( "SwitchInputMode", EGestureType::Down ));
	}
}
