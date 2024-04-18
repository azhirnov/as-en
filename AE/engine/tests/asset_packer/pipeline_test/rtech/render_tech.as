#include <pipeline_compiler.as>

void MinimalRTech ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "MinForward" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Graphics1" );
		pass.SetRenderPass( "Simple.RenderTarget", /*subpass*/"Main" );
	}{
		RC<ComputePass>		pass = rtech.AddComputePass( "Compute1" );
	}
}


void DeferredRTech ()
{
	RC<RenderTechnique>	rtech = RenderTechnique( "MinDeferred" );

	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Graphics1" );
		pass.SetRenderPass( "Simple.RenderTarget", /*subpass*/"Main" );
	}{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Graphics2" );
		pass.SetRenderPass( "Simple.RenderTarget", /*subpass*/"Main" );
	}{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Graphics3" );
		pass.SetRenderPass( "Simple.RenderTarget", /*subpass*/"Main" );
	}{
		RC<ComputePass>		pass = rtech.AddComputePass( "Compute1" );
	}{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "DepthPrepass" );
		pass.SetRenderPass( "Multipass.V2", /*subpass*/"DepthPrepass" );
	}{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "GBuffer" );
		pass.SetRenderPass( "Multipass.V2", /*subpass*/"GBuffer" );
	}{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Translucent" );
		pass.SetRenderPass( "Multipass.V2", /*subpass*/"Translucent" );
	}{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Lighting" );
		pass.SetRenderPass( "Multipass.V2", /*subpass*/"Lighting" );
	}{
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


void  UIRTech ()
{
	RC<RenderTechnique> rtech = RenderTechnique( "UI.RTech" );
	{
		RC<GraphicsPass>	pass = rtech.AddGraphicsPass( "Graphics0" );
		pass.SetRenderPass( "UIRenderPass.def", /*subpass*/"Main" );
	}
}


void ASmain ()
{
	MinimalRTech();
	DeferredRTech();
	MeshRTech();
	RayTracingRTech();
	UIRTech();
}
