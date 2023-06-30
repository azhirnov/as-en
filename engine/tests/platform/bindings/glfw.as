#include <input_actions>

void ASmain (ActionBindings& bindings)
{
    // UI bindings
    {
        RC<BindingsMode>    bind = bindings.CreateMode( "UI" );

        bind.Add( GLFW_Input::Enter,
                  ActionInfo( "Enter", EGestureType::Down ));
    }

    // Game bindings
    {
        RC<BindingsMode>    bind = bindings.CreateMode( "Game" );
    }

    // Player bindings
    {
        RC<BindingsMode>    bind = bindings.CreateMode( "Player" );
        bind.Inherit( "Game" );
    }

    // Vehicle bindings
    {
        RC<BindingsMode>    bind = bindings.CreateMode( "Vehicle" );
        bind.Inherit( "Game" );
    }
}
