// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>


void  DeclCanvasRenderTech ()
{
    RC<RenderTechnique> rtech = RenderTechnique( "Canvas.RTech" );
    {
        RC<GraphicsPass>    pass = rtech.AddGraphicsPass( "Main" );
        pass.SetRenderPass( "UIPass", /*subpass*/"Main" );
    }
}


void  DeclImGuiRenderTech ()
{
    RC<RenderTechnique> rtech = RenderTechnique( "ImGui.RTech" );
    {
        RC<GraphicsPass>    pass = rtech.AddGraphicsPass( "Main" );
        pass.SetRenderPass( "UIPass", /*subpass*/"Main" );
    }
}


void  DeclScene3DRenderTech ()
{
    RC<RenderTechnique> rtech = RenderTechnique( "Scene3D.RTech" );
    {
        RC<GraphicsPass>    pass = rtech.AddGraphicsPass( "Main" );
        pass.SetRenderPass( "SceneRPass", /*subpass*/"Main" );
    }
}


void  DeclHWCameraRenderTech ()
{
    RC<RenderTechnique> rtech = RenderTechnique( "HwCamera.RTech" );
    {
        RC<GraphicsPass>    pass = rtech.AddGraphicsPass( "Main" );
        pass.SetRenderPass( "UIPass", /*subpass*/"Main" );
    }
}


void  ASmain ()
{
    DeclCanvasRenderTech();
    DeclImGuiRenderTech();
    DeclScene3DRenderTech();
    DeclHWCameraRenderTech();
}
