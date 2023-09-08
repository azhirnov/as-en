namespace RenderTechs
{
    using RenderTechName_t      = AE::Graphics::RenderTechName;
    using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
    using AttachmentName_t      = AE::Graphics::AttachmentName;
    using PipelineName_t        = AE::Graphics::PipelineName;
    using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
    using DescriptorSetName_t   = AE::Graphics::DescriptorSetName;

    static constexpr struct _Bloom_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"Bloom.RTech"};}

        // graphics (0)
        static constexpr struct _Map
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Map"};}

            // RenderPass 'BloomPass.map' subpass 'Main'

            static constexpr uint  attachmentsCount = 2;
            static constexpr AttachmentName_t  att_Color {"Color"};
            static constexpr AttachmentName_t  att_DepthStencil {"DepthStencil"};

            // pipelines
            static constexpr PipelineName_t  bloom_entities_aos {"bloom.entities.aos"};
            static constexpr PipelineName_t  bloom_entities_soa {"bloom.entities.soa"};
            static constexpr PipelineName_t  bloom_map_aos {"bloom.map.aos"};
            static constexpr PipelineName_t  bloom_map_soa {"bloom.map.soa"};
        } Map;

        // graphics (1)
        static constexpr struct _PrePass
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"PrePass"};}

            // RenderPass 'BloomPass.hdr' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  bloom_prepass {"bloom.prepass"};
        } PrePass;

        // graphics (2)
        static constexpr struct _Downscale
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Downscale"};}

            // RenderPass 'BloomPass.hdr' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  bloom_downscale1p {"bloom.downscale1p"};
            static constexpr PipelineName_t  bloom_downscale2p13 {"bloom.downscale2p13"};
            static constexpr PipelineName_t  bloom_downscale2p9 {"bloom.downscale2p9"};
        } Downscale;

        // graphics (3)
        static constexpr struct _Upscale
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Upscale"};}

            // RenderPass 'BloomPass.hdr' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  bloom_upscale {"bloom.upscale"};
        } Upscale;

        // graphics (4)
        static constexpr struct _Final
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Final"};}

            // RenderPass 'BloomPass.ui' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  bloom_final_b0_tm0 {"bloom.final.b0.tm0"};
            static constexpr PipelineName_t  bloom_final_b0_tm1 {"bloom.final.b0.tm1"};
            static constexpr PipelineName_t  bloom_final_b0_tm2 {"bloom.final.b0.tm2"};
            static constexpr PipelineName_t  bloom_final_b0_tm3 {"bloom.final.b0.tm3"};
            static constexpr PipelineName_t  bloom_final_b1_tm0 {"bloom.final.b1.tm0"};
            static constexpr PipelineName_t  bloom_final_b1_tm1 {"bloom.final.b1.tm1"};
            static constexpr PipelineName_t  bloom_final_b1_tm2 {"bloom.final.b1.tm2"};
            static constexpr PipelineName_t  bloom_final_b1_tm3 {"bloom.final.b1.tm3"};
        } Final;

        // graphics (5)
        static constexpr struct _UI
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"UI"};}

            // RenderPass 'BloomPass.ui' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  imgui {"imgui"};
        } UI;
    } Bloom_RTech;

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

    static constexpr struct _HDR_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"HDR.RTech"};}

        // graphics (0)
        static constexpr struct _Main
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Main"};}

            // RenderPass 'UIPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  hdr_test {"hdr_test"};
            static constexpr PipelineName_t  imgui {"imgui"};
        } Main;
    } HDR_RTech;

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

    static constexpr struct _Mesh_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"Mesh.RTech"};}

        // graphics (0)
        static constexpr struct _Scene
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Scene"};}

            // RenderPass 'SceneRPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 2;
            static constexpr AttachmentName_t  att_Depth {"Depth"};
            static constexpr AttachmentName_t  att_Color {"Color"};
        } Scene;

        // graphics (1)
        static constexpr struct _UI
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"UI"};}

            // RenderPass 'UIPass.2' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  imgui {"imgui"};
        } UI;
    } Mesh_RTech;

    static constexpr struct _RayQuery_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"RayQuery.RTech"};}

        // compute (0)
        static constexpr struct _RayTrace
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"RayTrace"};}
        } RayTrace;
    } RayQuery_RTech;

    static constexpr struct _RayTracing_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"RayTracing.RTech"};}

        // compute (0)
        static constexpr struct _RayTrace
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"RayTrace"};}
        } RayTrace;
    } RayTracing_RTech;

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

            // pipelines
            static constexpr PipelineName_t  ui_draw1 {"ui.draw1"};
        } Main;
    } UI_RTech;

    static constexpr struct _Ycbcr_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"Ycbcr.RTech"};}

        // graphics (0)
        static constexpr struct _Main
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Main"};}

            // RenderPass 'UIPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  ycbcr_draw1 {"ycbcr.draw1"};
        } Main;
    } Ycbcr_RTech;
}
