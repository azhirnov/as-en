#include <input_actions>

void ASmain (Android_ActionBindings& bindings)
{
	// Canvas2D bindings
	{
		RC<Android_BindingsMode>	bind = bindings.CreateMode( "Canvas2D" );
	}

	// imGUI bindings
	{
		RC<Android_BindingsMode>	bind = bindings.CreateMode( "imGUI" );

		//bind.Add( Android_Input::TouchPos,
		//		  ActionInfo( "Touch", EValueType::Float2, EGestureType::Move ));
		
		bind.Add( Android_Input::TouchPos,
				  ActionInfo( "Test.Move", EValueType::Float2, EGestureType::Move ));
		bind.Add( Android_Input::TouchPos,
				  ActionInfo( "Test.Down", EValueType::Float2, EGestureType::Down ));
		bind.Add( Android_Input::TouchPos,
				  ActionInfo( "Test.Click", EValueType::Float2, EGestureType::Click ));
		bind.Add( Android_Input::TouchPos,
				  ActionInfo( "Test.DoubleClick", EValueType::Float2, EGestureType::DoubleClick ));
		bind.Add( Android_Input::TouchPos,
				  ActionInfo( "Test.LongPress", EValueType::Float2, EGestureType::LongPress_Move ));
		bind.Add( Android_Input::MultiTouch,
				  ActionInfo( "Test.ScaleRotate2D", EValueType::Float4, EGestureType::ScaleRotate2D ));
	}

	// Simple3D bindings
	{
		RC<Android_BindingsMode>	bind = bindings.CreateMode( "Simple3D" );

		bind.Add( Android_Input::TouchDelta_norm,
				  ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Move, float4(2.0f) ));
	}
}
