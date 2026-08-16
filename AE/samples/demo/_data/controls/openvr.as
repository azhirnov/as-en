// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#include <input_actions.as>

void ASmain (OpenVR_ActionBindings& bindings)
{
	// Camera3D bindings
	{
		RC<OpenVR_BindingsMode>	bind = bindings.CreateMode( "Camera3D" );
	}
}
