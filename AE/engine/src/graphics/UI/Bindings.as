// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

void  BindUI (RC<GLFW_BindingsMode> bind)
{
	bind.Add( GLFW_Input::CursorPos,
			  ActionInfo( "ui.MousePos", EValueType::Float2, EGestureType::Move ));
	bind.Add( GLFW_Input::MouseWheel,
			  ActionInfo( "ui.MouseWheel", EValueType::Float2, EGestureType::Move ));					// { horizontal, vertical }

	bind.Add( GLFW_Input::TouchPos,
			  ActionInfo( "ui.TouchMove", EValueType::Float2, EGestureType::Move ));
	bind.Add( GLFW_Input::TouchPos,
			  ActionInfo( "ui.TouchDown", EValueType::Float2, EGestureType::Down ));
	bind.Add( GLFW_Input::TouchPos,
			  ActionInfo( "ui.TouchPress", EValueType::Float3, EGestureType::LongPress ));				// { pos, factor }
	bind.Add( GLFW_Input::TouchPos,
			  ActionInfo( "ui.TouchClick", EValueType::Float2, EGestureType::Click ));
	bind.Add( GLFW_Input::TouchPos,
			  ActionInfo( "ui.TouchDoubleClick", EValueType::Float2, EGestureType::DoubleClick ));
	bind.Add( GLFW_Input::TouchPos,
			  ActionInfo( "ui.TouchLongPressMove", EValueType::Float2, EGestureType::LongPress_Move ));	// { pos, factor }
	bind.Add( GLFW_Input::MultiTouch,
			  ActionInfo( "ui.ScaleRotate2D", EValueType::Float2, EGestureType::ScaleRotate2D ));
}

void  BindUI (RC<WinAPI_BindingsMode> bind)
{
	bind.Add( WinAPI_Input::CursorPos,
			  ActionInfo( "ui.MousePos", EValueType::Float2, EGestureType::Move ));
	bind.Add( WinAPI_Input::MouseWheel,
			  ActionInfo( "ui.MouseWheel", EValueType::Float2, EGestureType::Move ));					// { horizontal, vertical }

	bind.Add( WinAPI_Input::TouchPos,
			  ActionInfo( "ui.TouchMove", EValueType::Float2, EGestureType::Move ));
	bind.Add( WinAPI_Input::TouchPos,
			  ActionInfo( "ui.TouchDown", EValueType::Float2, EGestureType::Down ));
	bind.Add( WinAPI_Input::TouchPos,
			  ActionInfo( "ui.TouchPress", EValueType::Float3, EGestureType::LongPress ));				// { pos, factor }
	bind.Add( WinAPI_Input::TouchPos,
			  ActionInfo( "ui.TouchClick", EValueType::Float2, EGestureType::Click ));
	bind.Add( WinAPI_Input::TouchPos,
			  ActionInfo( "ui.TouchDoubleClick", EValueType::Float2, EGestureType::DoubleClick ));
	bind.Add( WinAPI_Input::TouchPos,
			  ActionInfo( "ui.TouchLongPressMove", EValueType::Float2, EGestureType::LongPress_Move ));	// { pos, factor }
	bind.Add( WinAPI_Input::MultiTouch,
			  ActionInfo( "ui.ScaleRotate2D", EValueType::Float2, EGestureType::ScaleRotate2D ));
}

void  BindUI (RC<Android_BindingsMode> bind)
{
	bind.Add( Android_Input::TouchPos,
			  ActionInfo( "ui.TouchMove", EValueType::Float2, EGestureType::Move ));
	bind.Add( Android_Input::TouchPos,
			  ActionInfo( "ui.TouchDown", EValueType::Float2, EGestureType::Down ));
	bind.Add( Android_Input::TouchPos,
			  ActionInfo( "ui.TouchPress", EValueType::Float3, EGestureType::LongPress ));				// { pos, factor }
	bind.Add( Android_Input::TouchPos,
			  ActionInfo( "ui.TouchClick", EValueType::Float2, EGestureType::Click ));
	bind.Add( Android_Input::TouchPos,
			  ActionInfo( "ui.TouchDoubleClick", EValueType::Float2, EGestureType::DoubleClick ));
	bind.Add( Android_Input::TouchPos,
			  ActionInfo( "ui.TouchLongPressMove", EValueType::Float2, EGestureType::LongPress_Move ));	// { pos, factor }
	bind.Add( Android_Input::MultiTouch,
			  ActionInfo( "ui.ScaleRotate2D", EValueType::Float2, EGestureType::ScaleRotate2D ));
}
