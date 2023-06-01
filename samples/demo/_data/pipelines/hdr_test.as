// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler>

void ASmain ()
{
	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "hdr_test.draw1" );
	ppln.AddFeatureSet( "MinimalFS" );
	
	{
		RC<Shader>	vs	= Shader();
		vs.file		= "hdr_test.glsl";		// file:///<path>/samples/demo/_data/shaders/hdr_test.glsl
		vs.options	= EShaderOpt::Optimize;
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "hdr_test.glsl";		// file:///<path>/samples/demo/_data/shaders/hdr_test.glsl
		fs.options	= EShaderOpt::Optimize;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "hdr_test" );
		spec.AddToRenderTech( "HDR.RTech", "Main" );
		spec.SetViewportCount( 1 );
		
		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;
		spec.SetRenderState( rs );
	}
}
