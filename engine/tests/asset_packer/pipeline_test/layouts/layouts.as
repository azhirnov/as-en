

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
		DescriptorSetLayout@	ds = DescriptorSetLayout( "DS_PerDraw" );
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
		pl.DSLayout( DrawCmd_DSIdx,		"DS_PerDraw" );
		pl.DSLayout( Material_DSIdx,	"DS_Material" );
	}
	{
		PipelineLayout@		pl = PipelineLayout( "Graphics_PL_2" );
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
	{
		ShaderStructType@	st = ShaderStructType( "VBInput1" );
		st.Set( "packed_float3			Position;" +
				"packed_ushort_norm2	Texcoord;" );

		VertexBufferInput@	vb = VertexBufferInput( "VBInput1" );
		vb.Add( "vb", st );
	}
	{
		ShaderStructType@	st = ShaderStructType( "VBInput2" );
		st.Set( "packed_float2			Position;" +
				"packed_ushort_norm2	Texcoord;" );

		VertexBufferInput@	vb = VertexBufferInput( "VBInput2" );
		vb.Add( "vb", st );
	}
}


void main ()
{
	GraphicsLayout();
	ComputeLayout();
	RayTracingLayout();
	VertexBuffers();
}
