// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void  DeclUIRenderTech ()
{
    RC<RenderTechnique> rtech = RenderTechnique( "UI.RTech" );
    {
        RC<ShaderStructType>    st1 = ShaderStructType( "ui.global.ublock" );
        st1.Set( "float2    posScale;" +
                 "float2    posBias;" );

        RC<ShaderStructType>    st2 = ShaderStructType( "ui.material.ublock" );
        st2.Set( "uint      id;" );

        RC<DescriptorSetLayout> ds = DescriptorSetLayout( "ui.ds0" );
        ds.SampledImage( EShaderStages::Fragment, "un_Texture", ArraySize(1), EImageType::FImage2D );
        ds.UniformBufferDynamic( EShaderStages::Vertex | EShaderStages::Fragment, "globalUB",   ArraySize(1), "ui.global.ublock"    );
        ds.UniformBufferDynamic( EShaderStages::Vertex | EShaderStages::Fragment, "mtrUB",      ArraySize(1), "ui.material.ublock"  );
    }
    {
        RC<GraphicsPass>    pass = rtech.AddGraphicsPass( "Main" );
        pass.SetRenderPass( "UIPass", /*subpass*/"Main" );
        pass.SetDSLayout( "ui.ds0" );
    }
}


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


void  ASmain ()
{
    DeclUIRenderTech();
    DeclCanvasRenderTech();
    DeclImGuiRenderTech();
    DeclScene3DRenderTech();
}
