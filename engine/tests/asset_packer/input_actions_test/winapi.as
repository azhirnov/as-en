#include <input_actions>

void ASmain (WinAPI_ActionBindings& bindings)
{
    // UI bindings
    {
        RC<WinAPI_BindingsMode> bind = bindings.CreateMode( "UI" );

        bind.Add( WinAPI_Input::Enter,
                  ActionInfo( "Enter", EGestureType::Down ));

        bind.Add( WinAPI_Input::Escape,
                  ActionInfo( "Back", EGestureType::Down ));

        bind.Add( WinAPI_Input::MouseLeft,
                  ActionInfo( "Push", EGestureType::Hold ));

        bind.Add( WinAPI_Input::MouseRight,
                  ActionInfo( "Options", EGestureType::Down ));

        bind.Add( WinAPI_Input::CursorPos,
                  ActionInfo( "Cursor", EValueType::Float2, EGestureType::Move ));
    }
}
