#include <pipeline_compiler>

void ASmain ()
{
	{
		RC<ShaderStructType>	st = ShaderStructType( "imgui.io" );
		st.Set( "float4		color;" +
				"float2		uv;" );
	}{
		RC<ShaderStructType>	st = ShaderStructType( "imgui.ub" );
		st.Set( "float2		scale;" +
				"float2		translate;" );
	
		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "imgui.ds0" );
		ds.CombinedImage( EShaderStages::Fragment, "un_Texture", EImageType::FImage2D, "LinearRepeat" );
		ds.UniformBuffer( EShaderStages::Vertex, "ub", ArraySize(1), "imgui.ub" );
	}{
		RC<PipelineLayout>		pl = PipelineLayout( "imgui.pl" );
		pl.DSLayout( 0, "imgui.ds0" );
	}
	{
		RC<ShaderStructType>	st = ShaderStructType( "imgui.vertex" );
		st.Set( "packed_float2		Position;"	+
				"packed_float2		UV;"		+
				"packed_ubyte_norm4	Color;"		);

		RC<VertexBufferInput>	vb = VertexBufferInput( "VB.imgui.vertex" );
		vb.Add( "Vertex",	"imgui.vertex" );
	}


	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "imgui.draw1" );
	ppln.AddFeatureSet( "MinimalFS" );
	ppln.SetLayout( "imgui.pl" );
	ppln.SetVertexInput( "VB.imgui.vertex" );
	ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "imgui.io" );
	
	{
		RC<Shader>	vs = Shader();
		vs.file		= "imgui.glsl";
		vs.options	= EShaderOpt::Optimize;
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs = Shader();
		fs.file		= "imgui.glsl";
		fs.options	= EShaderOpt::Optimize;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "imgui" );
		spec.AddToRenderTech( "ImGui.RTech", "Main" );
		spec.AddToRenderTech( "HDR.RTech", "Main" );
		spec.SetViewportCount( 1 );

		RenderState	rs;

		RenderState_ColorBuffer	cb;
		cb.srcBlendFactor	.set( EBlendFactor::SrcAlpha );
		cb.dstBlendFactor	.set( EBlendFactor::OneMinusSrcAlpha );
		cb.blendOp			.set( EBlendOp::Add );
		cb.blend			= true;
		rs.color.SetColorBuffer( 0, cb );

		rs.depth.test = false;

		rs.inputAssembly.topology = EPrimitive::TriangleList;

		rs.rasterization.cullMode = ECullMode::None;

		spec.SetRenderState( rs );
	}
}
