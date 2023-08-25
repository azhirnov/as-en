namespace RenderTechs
{
    using RenderTechName_t      = AE::Graphics::RenderTechName;
    using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
    using AttachmentName_t      = AE::Graphics::AttachmentName;
    using PipelineName_t        = AE::Graphics::PipelineName;
    using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
    using DescriptorSetName_t   = AE::Graphics::DescriptorSetName;

    static constexpr struct _Canvas_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"Canvas.RTech"};}

        // graphics (0)
        static constexpr struct _Main
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Main"};}

            // RenderPass 'UIPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  canvas2d_draw1 {"canvas2d.draw1"};
            static constexpr PipelineName_t  canvas2d_draw2 {"canvas2d.draw2"};
            static constexpr PipelineName_t  canvas2d_draw3 {"canvas2d.draw3"};
            static constexpr PipelineName_t  font_draw {"font.draw"};
            static constexpr PipelineName_t  sdf_font_draw {"sdf_font.draw"};
        } Main;
    } Canvas_RTech;

    static constexpr struct _ImGui_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"ImGui.RTech"};}

        // graphics (0)
        static constexpr struct _Main
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Main"};}

            // RenderPass 'UIPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  imgui {"imgui"};
        } Main;
    } ImGui_RTech;

    static constexpr struct _Scene3D_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"Scene3D.RTech"};}

        // graphics (0)
        static constexpr struct _Main
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Main"};}

            // RenderPass 'SceneRPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 2;
            static constexpr AttachmentName_t  att_Depth {"Depth"};
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  simple3d_draw1 {"simple3d.draw1"};
            static constexpr PipelineName_t  simple3d_draw2 {"simple3d.draw2"};
        } Main;
    } Scene3D_RTech;

    static constexpr struct _UI_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"UI.RTech"};}

        // graphics (0)
        static constexpr struct _Main
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Main"};}
            static constexpr DescriptorSetName_t  dsLayout {"ui.ds0"};

            // RenderPass 'UIPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};
        } Main;
    } UI_RTech;
}
