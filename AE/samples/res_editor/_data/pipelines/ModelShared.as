// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Always included for GraphicsPass
*/
#include "Model.as"

void  ASmain ()
{
    VertexBuffers();
    BufferTypes( /*withFS*/false );
    InitPipelineLayout();

    InitMeshPipelineLayout();
    InitRayTracingPipelineLayout();
}
