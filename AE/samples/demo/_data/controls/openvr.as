// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <input_actions.as>

void ASmain (OpenVR_ActionBindings& bindings)
{
    // Simple3D bindings
    {
        RC<OpenVR_BindingsMode> bind = bindings.CreateMode( "Simple3D" );
    }
}
