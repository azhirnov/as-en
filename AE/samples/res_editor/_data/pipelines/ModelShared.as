// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Always included for GraphicsPass
*/
#include "ModelTypes.as"

void  ASmain ()
{
	RC<FeatureSet>	fs = GetDefaultFeatureSet();

	Assert( fs.hasShaderStorageBufferArrayDynamicIndexing() );
	Assert( fs.hasShaderSampledImageArrayDynamicIndexing() );

	VertexBuffers();
	BufferTypes( /*withFS*/false, fs.hasRayTracingPipeline() );
	InitPipelineLayout();

	if ( fs.hasMeshShader() )
		InitMeshPipelineLayout();

	if ( fs.hasRayTracingPipeline() )
		InitRayTracingPipelineLayout();
}
