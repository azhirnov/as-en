// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void ASmain ()
{
	RC<ShaderStructType>	st = ShaderStructType( "Vertex_multiview" );
	st.Set( EStructLayout::InternalIO,
			"packed_float2		Position;" +
			"packed_ubyte_norm4	Color;" );

	RC<VertexBufferInput>	vb = VertexBufferInput( "multiview.vb" );
	vb.Add( "vb", st );

	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "multiview" );
	ppln.SetVertexInput( vb );
	ppln.AddFeatureSet( "part.MultiView" );

	{
		RC<Shader>	vs	= Shader();
		vs.file		= "draw2_vs.glsl";			// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics_rhi/RenderGraph/shaders/draw2_vs.glsl)
		vs.options	= EShaderOpt::Optimize;
		vs.Define( "MULTI_VIEW" );
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "draw1_fs.glsl";			// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics_rhi/RenderGraph/shaders/draw1_fs.glsl)
		fs.options	= EShaderOpt::Optimize;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "multiview" );
		spec.AddToRenderTech( "MultiView.RTech", "MultiView" );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}
