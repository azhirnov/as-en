

void GraphicsLayout ()
{
	const uint	RenderPass_DSIdx	= 0;
	const uint	Material_DSIdx		= 1;
	const uint	DrawCmd_DSIdx		= 2;

	{
		ShaderStructType@	st = ShaderStructType( "UBlock" );
		st.Set( "float4x4  mvp;" );
	}
	{
		DescriptorSetLayout@	ds = DescriptorSetLayout( "DS_PerDraw2D" );
		ds.AddFeatureSet( "MinimalFS" );
		ds.SetUsage( EDescSetUsage::UpdateTemplate | EDescSetUsage::ArgumentBuffer );
		ds.Define( "#define DEF_VALUE_1" );

		ds.UniformBuffer( EShaderStages::Vertex, "drawUB", ArraySize(1), "UBlock" );
	}
	{
		DescriptorSetLayout@	ds = DescriptorSetLayout( "DS_Material" );
		ds.AddFeatureSet( "MinimalFS" );
		
		ds.SampledImage( EShaderStages::Fragment, "un_ColorTexture", ArraySize(1), EImageType::FImage2D );
		ds.ImtblSampler( EShaderStages::Fragment, "un_ColorTexture_sampler", "LinearRepeat"  );
	}
	{
		PipelineLayout@		pl = PipelineLayout( "Graphics_PL_1" );
		pl.DSLayout( DrawCmd_DSIdx,		"DS_PerDraw2D" );
		pl.DSLayout( Material_DSIdx,	"DS_Material" );
	}
	{
		PipelineLayout@		pl = PipelineLayout( "Graphics_PL_2" );
		pl.DSLayout( Material_DSIdx,	"DS_Material" );
	}
	{
		DescriptorSetLayout@	ds = DescriptorSetLayout( "DS_PerDraw3D" );
		ds.AddFeatureSet( "MinimalFS" );
		ds.SetUsage( EDescSetUsage::UpdateTemplate | EDescSetUsage::ArgumentBuffer );

		ds.UniformBuffer( EShaderStages::Vertex, "drawUB", ArraySize(1), "UBlock" );
	}
	{
		PipelineLayout@		pl = PipelineLayout( "Graphics_PL_4" );
		pl.DSLayout( DrawCmd_DSIdx,		"DS_PerDraw3D" );
		pl.DSLayout( Material_DSIdx,	"DS_Material" );
	}
	{
		ShaderStructType@	pc1 = ShaderStructType( "PushConst1" );
		pc1.Set( "float2	scale;" +
				 "float2	bias;" );
		
		ShaderStructType@	pc2 = ShaderStructType( "PushConst2" );
		pc2.Set( "float4	color;" );

		PipelineLayout@		pl = PipelineLayout( "Graphics_PL_3" );
		pl.PushConst( "pc1", pc1, EShaderStages::Vertex );
		pl.PushConst( "pc2", pc2, EShaderStages::Fragment );
		pl.DSLayout( 0, "DS_Material" );
	}
}


void ComputeLayout ()
{
	{
		DescriptorSetLayout@	ds = DescriptorSetLayout( "DS_Compute" );
		ds.AddFeatureSet( "MinimalFS" );
		
		ds.StorageImage( EShaderStages::Compute, "un_OutImage", ArraySize(1), EImageType::FImage2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
	}
	{
		PipelineLayout@		pl = PipelineLayout( "Compute_PL_1" );
		pl.DSLayout( 0, "DS_Compute" );
	}
	{
		PipelineLayout@		pl = PipelineLayout( "Compute_PL_1_Dbg" );
		pl.DSLayout( 0, "DS_Compute" );
		pl.AddDebugDSLayout( 1, EShaderOpt::Trace, EShaderStages::Compute );
	}
}


void RayTracingLayout ()
{
	if ( ! IsVulkan() )
		return;

	DescriptorSetLayout@	ds = DescriptorSetLayout( "DS_RayTracing" );
	ds.AddFeatureSet( "MinRecursiveRayTracing" );
		
	ds.StorageImage( EShaderStages::RayGen, "un_OutImage", ArraySize(1), EImageType::FImage2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
	ds.RayTracingScene( EShaderStages::RayGen, "un_TLAS", ArraySize(1) );
	
	
	PipelineLayout@		pl = PipelineLayout( "RayTracing_PL_1" );
	pl.DSLayout( 0, ds );
}


void VertexBuffers ()
{
	// layouts
	{
		ShaderStructType@	st = ShaderStructType( "vb_layout1" );
		st.Set( "float3		Position;" +
				"float2		Texcoord;" );

		VertexBufferInput@	vb = VertexBufferInput( "vb_layout1" );
		vb.Add( "vb", st );
	}
	{
		ShaderStructType@	st = ShaderStructType( "vb_layout2" );
		st.Set( "float2		Position;" +
				"float2		Texcoord;" );

		VertexBufferInput@	vb = VertexBufferInput( "vb_layout2" );
		vb.Add( "vb", st );
	}

	// definition
	{
		ShaderStructType@	st = ShaderStructType( "vb_input1" );
		st.Set( "packed_float3			Position;" +
				"packed_ushort_norm2	Texcoord;" );

		VertexBufferInput@	vb = VertexBufferInput( "vb_input1" );
		vb.Add( "vb", st );
		SameAttribs( "vb_layout1", "vb_input1" );
	}
	{
		ShaderStructType@	st = ShaderStructType( "vb_input2" );
		st.Set( "packed_float2			Position;" +
				"packed_ushort_norm2	Texcoord;" );

		VertexBufferInput@	vb = VertexBufferInput( "vb_input2" );
		vb.Add( "vb", st );
		SameAttribs( "vb_layout2", "vb_input2" );
	}
	{
		ShaderStructType@	st1 = ShaderStructType( "VB_3_Pos" );
		st1.Set( "packed_float3		Position;" );

		ShaderStructType@	st2 = ShaderStructType( "VB_3_Attribs" );
		st2.Set( "packed_float2		Texcoord;" );

		VertexBufferInput@	vb = VertexBufferInput( "vb_input3" );
		vb.Add( "Position",	st1 );
		vb.Add( "Attribs",	st2 );
		SameAttribs( "vb_layout1", "vb_input3" );
	}
}


void ShaderInputOutput ()
{
	{
		ShaderStructType@	st = ShaderStructType( "graphics_1.io" );
		st.Set( "float2  Texcoord;" );
	}{
		ShaderStructType@	st = ShaderStructType( "graphics_4.io" );
		st.Set( "float2  texCoord;" );
	}{
		ShaderStructType@	st = ShaderStructType( "mesh_1.io" );
		st.Set( "float2  texcoord;" +
				"float4  color;" );
	}
}


void main ()
{
	GraphicsLayout();
	ComputeLayout();
	RayTracingLayout();
	VertexBuffers();
	ShaderInputOutput();
}
