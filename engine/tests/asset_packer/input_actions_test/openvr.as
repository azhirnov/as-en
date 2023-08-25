#include <input_actions.as>

void ASmain (OpenVR_ActionBindings& bindings)
{
    // UI bindings
    {
        RC<OpenVR_BindingsMode> bind = bindings.CreateMode( "UI" );
    }
}
