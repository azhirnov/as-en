// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include "Model.as"

void  ASmain ()
{
    VertexBuffers();
    BufferTypes( /*withFS*/false );
    InitPipelineLayout();

    InitMeshPipelineLayout();
    InitRayTracingPipelineLayout();
}
