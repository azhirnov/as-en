// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <input_actions.as>

void ASmain (WinAPI_ActionBindings& bindings)
{
	// Sample Selector bindings
	{
		RC<WinAPI_BindingsMode>	bind = bindings.CreateMode( "SampleSelector" );

	//	bind.Add( WinAPI_Input::CursorPos,
	//			  ActionInfo( "ui.Mouse", EValueType::Float2, EGestureType::Move ));

	//	bind.Add( WinAPI_Input::MouseLeft,
	//			  ActionInfo( "ui.MouseLBDown", EGestureType::Hold ));

	//	bind.Add( WinAPI_Input::MouseLeft,
	//			  ActionInfo( "ui.MouseLBClick", EGestureType::Click ));
	}

	// Canvas2D bindings
	{
		RC<WinAPI_BindingsMode>	bind = bindings.CreateMode( "Canvas2D" );
		bind.Inherit( "SampleSelector" );

		bind.Add( WinAPI_Input::Enter,
				  ActionInfo( "Enter", EGestureType::Down ));
		bind.Add( WinAPI_Input::CursorPos,
				  ActionInfo( "Cursor", EValueType::Float2, EGestureType::Move ));
	}

	// imGUI bindings
	{
		RC<WinAPI_BindingsMode>	bind = bindings.CreateMode( "imGUI" );
		bind.Inherit( "SampleSelector" );

		bind.Add( WinAPI_Input::CursorPos,
				  ActionInfo( "MousePos", EValueType::Float2, EGestureType::Move ));
		bind.Add( WinAPI_Input::MouseLeft,
				  ActionInfo( "MouseLBDown", EGestureType::Hold ));
		bind.Add( WinAPI_Input::MouseWheel,
				  ActionInfo( "MouseWheel", EValueType::Float2, EGestureType::Move ));

		bind.Add( WinAPI_Input::TouchPos,
				  ActionInfo( "Touch.Move", EValueType::Float2, EGestureType::Move ));
		bind.Add( WinAPI_Input::TouchPos,
				  ActionInfo( "Touch.Click", EValueType::Float2, EGestureType::Click ));
	}

	// HwCamera bindings
	{
		RC<WinAPI_BindingsMode>	bind = bindings.CreateMode( "HwCamera" );

		bind.Add( WinAPI_Input::TouchPos,
				  ActionInfo( "Touch.Move", EValueType::Float2, EGestureType::Move ));
		bind.Add( WinAPI_Input::TouchPos,
				  ActionInfo( "Touch.Click", EValueType::Float2, EGestureType::Click ));
	}

	// Camera3D bindings
	{
		RC<WinAPI_BindingsMode>	bind = bindings.CreateMode( "Camera3D" );
		bind.Inherit( "SampleSelector" );

		const float4	move_scale		(0.001f);
		const float4	rotation_scale	(0.01f);

		bind.Add( WinAPI_Input::W,
				  ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("+00"), move_scale ));
		bind.Add( WinAPI_Input::S,
				  ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("-00"), move_scale ));
		bind.Add( WinAPI_Input::A,
				  ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("0+0"), move_scale ));
		bind.Add( WinAPI_Input::D,
				  ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("0-0"), move_scale ));
		bind.Add( WinAPI_Input::LeftShift,
				  ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("00-"), move_scale ));
		bind.Add( WinAPI_Input::Space,
				  ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("00+"), move_scale ));

		bind.Add( WinAPI_Input::TouchDelta_norm,
				  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Move, float4(2.0f) ));

		bind.Add( WinAPI_Input::ArrowLeft,
				  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("-0"), rotation_scale ));
		bind.Add( WinAPI_Input::ArrowRight,
				  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("+0"), rotation_scale ));
		bind.Add( WinAPI_Input::ArrowUp,
				  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("0-"), rotation_scale ));
		bind.Add( WinAPI_Input::ArrowDown,
				  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("0+"), rotation_scale ));
	}
}
