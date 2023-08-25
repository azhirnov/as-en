namespace RenderTechs
{
    using RenderTechName_t      = AE::Graphics::RenderTechName;
    using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
    using AttachmentName_t      = AE::Graphics::AttachmentName;
    using PipelineName_t        = AE::Graphics::PipelineName;
    using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
    using DescriptorSetName_t   = AE::Graphics::DescriptorSetName;

    static constexpr struct _AsyncCompTestRT
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"AsyncCompTestRT"};}

        // graphics (0)
        static constexpr struct _Draw_1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Draw_1"};}

            // RenderPass 'DrawTest.Draw_1' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  async_comp1_graphics {"async_comp1.graphics"};
        } Draw_1;

        // compute (1)
        static constexpr struct _Compute_1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Compute_1"};}

            // pipelines
            static constexpr PipelineName_t  async_comp1_compute {"async_comp1.compute"};
        } Compute_1;
    } AsyncCompTestRT;

    static constexpr struct _DrawMeshesTestRT
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"DrawMeshesTestRT"};}

        // graphics (0)
        static constexpr struct _DrawMeshes_1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"DrawMeshes_1"};}

            // RenderPass 'DrawTest.Draw_1' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  draw_mesh1 {"draw_mesh1"};
            static constexpr PipelineName_t  draw_mesh2 {"draw_mesh2"};
        } DrawMeshes_1;
    } DrawMeshesTestRT;

    static constexpr struct _DrawTestRT
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"DrawTestRT"};}

        // graphics (0)
        static constexpr struct _Draw_1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Draw_1"};}

            // RenderPass 'DrawTest.Draw_1' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  dbg2_draw {"dbg2_draw"};
            static constexpr PipelineName_t  dbg3_draw {"dbg3_draw"};
            static constexpr PipelineName_t  draw1 {"draw1"};
            static constexpr PipelineName_t  draw2 {"draw2"};
            static constexpr PipelineName_t  draw3 {"draw3"};
        } Draw_1;

        // graphics (1)
        static constexpr struct _Test4_1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Test4-1"};}

            // RenderPass 'DrawTest4.Pass1' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  draw4_1 {"draw4-1"};
        } Test4_1;

        // graphics (2)
        static constexpr struct _Test4_2
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Test4-2"};}

            // RenderPass 'DrawTest4.Pass2' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  draw4_2 {"draw4-2"};
        } Test4_2;
    } DrawTestRT;

    static constexpr struct _RayQueryTestRT
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"RayQueryTestRT"};}

        // compute (0)
        static constexpr struct _RayTrace_1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"RayTrace_1"};}

            // pipelines
            static constexpr PipelineName_t  dbg5_rquery_def {"dbg5_rquery.def"};
            static constexpr PipelineName_t  rquery1_def {"rquery1.def"};
        } RayTrace_1;
    } RayQueryTestRT;

    static constexpr struct _RayTracingTestRT
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"RayTracingTestRT"};}

        // compute (0)
        static constexpr struct _RayTrace_1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"RayTrace_1"};}

            // pipelines
            static constexpr PipelineName_t  dbg4_rtrace1_def {"dbg4_rtrace1.def"};
            static constexpr PipelineName_t  rtrace1_def {"rtrace1.def"};
        } RayTrace_1;

        static constexpr struct _SBT {
            static constexpr RTShaderBindingName_t  dbg4_rtrace1_sbt0 {"dbg4_rtrace1.sbt0"};
            static constexpr RTShaderBindingName_t  rtrace1_sbt0 {"rtrace1.sbt0"};
        } sbt;
    } RayTracingTestRT;

    static constexpr struct _VRSTestRT
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"VRSTestRT"};}

        // graphics (0)
        static constexpr struct _nonVRS
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"nonVRS"};}

            // RenderPass 'DrawTest.Draw_1' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  per_draw {"per_draw"};
            static constexpr PipelineName_t  per_primitive {"per_primitive"};
        } nonVRS;

        // graphics (1)
        static constexpr struct _VRS
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"VRS"};}

            // RenderPass 'VRSTest.Draw' subpass 'Main'

            static constexpr uint  attachmentsCount = 2;
            static constexpr AttachmentName_t  att_Color {"Color"};
            static constexpr AttachmentName_t  att_ShadingRate {"ShadingRate"};

            // pipelines
            static constexpr PipelineName_t  vrs_attachment {"vrs_attachment"};
        } VRS;
    } VRSTestRT;
}
