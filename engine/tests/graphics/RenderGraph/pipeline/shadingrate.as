#include <pipeline_compiler>

void PerDrawSRPipeline ()
{
	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "per_draw" );
	ppln.AddFeatureSet( "MinimalFS" );
	ppln.AddFeatureSet( "part.ShadingRate.compat" );
	
	{
		RC<Shader>	vs	= Shader();
		vs.file		= "shadingrate.glsl";
		vs.options	= EShaderOpt::Optimize;
		vs.Define( "PER_DRAW_SHADING_RATE" );
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "shadingrate.glsl";
		fs.options	= EShaderOpt::Optimize;
		fs.Define( "PER_DRAW_SHADING_RATE" );
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "per_draw" );
		spec.AddToRenderTech( "VRSTestRT", "nonVRS" );
		spec.SetDynamicState( EPipelineDynamicState::FragmentShadingRate );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}


void PerPrimitiveSRPipeline ()
{
	RC<ShaderStructType>	st = ShaderStructType( "Vertex_VRS" );
	st.Set( "packed_float2		Position;" +
			"int				ShadingRate;" );

	RC<VertexBufferInput>	vb = VertexBufferInput( "per_primitive.vb" );
	vb.Add( "vb", st );

	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "per_primitive" );
	ppln.AddFeatureSet( "MinimalFS" );
	ppln.AddFeatureSet( "part.ShadingRate.compat" );
	ppln.SetVertexInput( vb );
	
	{
		RC<Shader>	vs	= Shader();
		vs.file		= "shadingrate.glsl";
		vs.options	= EShaderOpt::Optimize;
		vs.Define( "PER_PRIMITIVE_SHADING_RATE" );
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "shadingrate.glsl";
		fs.options	= EShaderOpt::Optimize;
		fs.Define( "PER_PRIMITIVE_SHADING_RATE" );
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "per_primitive" );
		spec.AddToRenderTech( "VRSTestRT", "nonVRS" );
		spec.SetDynamicState( EPipelineDynamicState::FragmentShadingRate );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}


void AttachmentSRPipeline ()
{
	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "vrs_attachment" );
	ppln.AddFeatureSet( "MinimalFS" );
	ppln.AddFeatureSet( "part.ShadingRate.NV" );
	
	{
		RC<Shader>	vs	= Shader();
		vs.file		= "shadingrate.glsl";
		vs.options	= EShaderOpt::Optimize;
		vs.Define( "ATTACHMENT_SHADING_RATE" );
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "shadingrate.glsl";
		fs.options	= EShaderOpt::Optimize;
		fs.Define( "ATTACHMENT_SHADING_RATE" );
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "vrs_attachment" );
		spec.AddToRenderTech( "VRSTestRT", "VRS" );
		spec.SetDynamicState( EPipelineDynamicState::FragmentShadingRate );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}


void ASmain ()
{
	if ( ! IsVulkan() )
		return;

	PerDrawSRPipeline();
	PerPrimitiveSRPipeline();
	AttachmentSRPipeline();
}
