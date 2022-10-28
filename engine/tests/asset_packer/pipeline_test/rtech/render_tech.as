

void MinimalRTech ()
{
	RenderTechnique@ rtech = RenderTechnique( "MinForward" );

	rtech.AddFeatureSet( "MinimalFS" );

	{
		GraphicsPass@	pass = rtech.AddGraphicsPass( "Graphics1" );

		pass.SetRenderPass( "Simple.RenderTarget", /*subpass*/"Main" );
		//pass.SetDSLayout( "" );
		//pass.SetRenderState();
		//pass.SetMutableStates();
	}
	{
		ComputePass@	pass = rtech.AddComputePass( "Compute1" );
	}
}


void MeshRTech ()
{
	if ( !IsVulkan() )
		return;

	RenderTechnique@ rtech = RenderTechnique( "MeshForward" );

	rtech.AddFeatureSet( "MinMeshShader" );

	{
		GraphicsPass@	pass = rtech.AddGraphicsPass( "Graphics1" );

		pass.SetRenderPass( "Simple.Present", /*subpass*/"Main" );
		//pass.SetDSLayout( "" );
		//pass.SetRenderState();
		//pass.SetMutableStates();
	}
}


void RayTracingRTech ()
{
	if ( !IsVulkan() )
		return;
	
	RenderTechnique@ rtech = RenderTechnique( "RayTracing" );

	rtech.AddFeatureSet( "MinRecursiveRayTracing" );
	
	{
		ComputePass@	pass = rtech.AddComputePass( "Trace1" );
	}
}


void main ()
{
	MinimalRTech();
	MeshRTech();
	RayTracingRTech();
}
