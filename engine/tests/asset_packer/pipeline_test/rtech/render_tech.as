#include <pipeline_compiler>

void MinimalRTech ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "MinForward" );

	rtech.AddFeatureSet( "MinimalFS" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Graphics1" );

		pass.SetRenderPass( "Simple.RenderTarget", /*subpass*/"Main" );
		//pass.SetDSLayout( "" );
		//pass.SetRenderState();
		//pass.SetMutableStates();
	}
	{
		RC<ComputePass>		pass = rtech.AddComputePass( "Compute1" );
	}
}


void DeferredRTech ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "MinDeferred" );
	rtech.AddFeatureSet( "MinimalFS" );
	
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "DepthPrepass" );

		pass.SetRenderPass( "Multipass.V2", /*subpass*/"DepthPrepass" );
	}
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "GBuffer" );

		pass.SetRenderPass( "Multipass.V2", /*subpass*/"GBuffer" );
	}
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Translucent" );

		pass.SetRenderPass( "Multipass.V2", /*subpass*/"Translucent" );
	}
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Lighting" );

		pass.SetRenderPass( "Multipass.V2", /*subpass*/"Lighting" );
	}
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "PostProcess" );

		pass.SetRenderPass( "Multipass.V2", /*subpass*/"PostProcess" );
	}
}


void MeshRTech ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "MeshForward" );

	rtech.AddFeatureSet( "MinMeshShader" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Graphics1" );

		pass.SetRenderPass( "Simple.Present", /*subpass*/"Main" );
		//pass.SetDSLayout( "" );
		//pass.SetRenderState();
		//pass.SetMutableStates();
	}
}


void RayTracingRTech ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "RayTracing" );

	rtech.AddFeatureSet( "MinRecursiveRayTracing" );
	
	{
		RC<ComputePass>		pass = rtech.AddComputePass( "Trace1" );
	}
}


void main ()
{
	MinimalRTech();
	DeferredRTech();
	MeshRTech();
	RayTracingRTech();
}
