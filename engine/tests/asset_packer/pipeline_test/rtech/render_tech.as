

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


void DeferredRTech ()
{
	RenderTechnique@ rtech = RenderTechnique( "MinDeferred" );
	rtech.AddFeatureSet( "MinimalFS" );
	
	{
		GraphicsPass@	pass = rtech.AddGraphicsPass( "DepthPrepass" );

		pass.SetRenderPass( "Multipass.V2", /*subpass*/"DepthPrepass" );
	}
	{
		GraphicsPass@	pass = rtech.AddGraphicsPass( "GBuffer" );

		pass.SetRenderPass( "Multipass.V2", /*subpass*/"GBuffer" );
	}
	{
		GraphicsPass@	pass = rtech.AddGraphicsPass( "Translucent" );

		pass.SetRenderPass( "Multipass.V2", /*subpass*/"Translucent" );
	}
	{
		GraphicsPass@	pass = rtech.AddGraphicsPass( "Lighting" );

		pass.SetRenderPass( "Multipass.V2", /*subpass*/"Lighting" );
	}
	{
		GraphicsPass@	pass = rtech.AddGraphicsPass( "PostProcess" );

		pass.SetRenderPass( "Multipass.V2", /*subpass*/"PostProcess" );
	}
}


void MeshRTech ()
{
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
	RenderTechnique@ rtech = RenderTechnique( "RayTracing" );

	rtech.AddFeatureSet( "MinRecursiveRayTracing" );
	
	{
		ComputePass@	pass = rtech.AddComputePass( "Trace1" );
	}
}


void main ()
{
	MinimalRTech();
	DeferredRTech();
	MeshRTech();
	RayTracingRTech();
}
