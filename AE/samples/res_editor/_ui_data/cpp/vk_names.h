namespace RenderTechs
{
    using RenderTechName_t      = AE::Graphics::RenderTechName;
    using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
    using AttachmentName_t      = AE::Graphics::AttachmentName;
    using PipelineName_t        = AE::Graphics::PipelineName;
    using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
    using DescriptorSetName_t   = AE::Graphics::DescriptorSetName;

    static constexpr struct _Histogram_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"Histogram.RTech"};}

        // compute (0)
        static constexpr struct _Compute
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Compute"};}

            // pipelines
            static constexpr PipelineName_t  Histogram_CSPass1 {"Histogram.CSPass1"};
            static constexpr PipelineName_t  Histogram_CSPass2 {"Histogram.CSPass2"};
        } Compute;

        // graphics (1)
        static constexpr struct _Graphics
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Graphics"};}

            // RenderPass 'Histogram.RPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  Histogram_draw {"Histogram.draw"};
        } Graphics;
    } Histogram_RTech;

    static constexpr struct _LinearDepth_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"LinearDepth.RTech"};}

        // graphics (0)
        static constexpr struct _Graphics
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Graphics"};}

            // RenderPass 'LinearDepth.RPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  LinearDepth_draw {"LinearDepth.draw"};
        } Graphics;
    } LinearDepth_RTech;

    static constexpr struct _UI_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"UI.RTech"};}

        // graphics (0)
        static constexpr struct _UI_BGRA8
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"UI.BGRA8"};}

            // RenderPass 'UI.RPass.BGRA8' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  imgui_BGRA8 {"imgui.BGRA8"};
            static constexpr PipelineName_t  imgui_opaque_BGRA8 {"imgui.opaque.BGRA8"};
        } UI_BGRA8;

        // graphics (1)
        static constexpr struct _UI_RGBA8
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"UI.RGBA8"};}

            // RenderPass 'UI.RPass.RGBA8' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  imgui_RGBA8 {"imgui.RGBA8"};
            static constexpr PipelineName_t  imgui_opaque_RGBA8 {"imgui.opaque.RGBA8"};
        } UI_RGBA8;

        // graphics (2)
        static constexpr struct _UI_RGB10_A2
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"UI.RGB10_A2"};}

            // RenderPass 'UI.RPass.RGB10_A2' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  imgui_RGB10_A2 {"imgui.RGB10_A2"};
            static constexpr PipelineName_t  imgui_opaque_RGB10_A2 {"imgui.opaque.RGB10_A2"};
        } UI_RGB10_A2;

        // graphics (3)
        static constexpr struct _UI_RGBA16F
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"UI.RGBA16F"};}

            // RenderPass 'UI.RPass.RGBA16F' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  imgui_RGBA16F {"imgui.RGBA16F"};
            static constexpr PipelineName_t  imgui_opaque_RGBA16F {"imgui.opaque.RGBA16F"};
        } UI_RGBA16F;
    } UI_RTech;
}
