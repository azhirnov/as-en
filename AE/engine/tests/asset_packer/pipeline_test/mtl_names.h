namespace RenderTechs
{
    using RenderTechName_t      = AE::Graphics::RenderTechName;
    using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
    using AttachmentName_t      = AE::Graphics::AttachmentName;
    using PipelineName_t        = AE::Graphics::PipelineName;
    using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
    using DescriptorSetName_t   = AE::Graphics::DescriptorSetName;

    static constexpr struct _MeshForward
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"MeshForward"};}

        // graphics (0)
        static constexpr struct _Graphics1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Graphics1"};}

            // RenderPass 'Simple.Present' subpass 'Main'

            static constexpr uint  attachmentsCount = 2;
            static constexpr AttachmentName_t  att_Depth {"Depth"};
            static constexpr AttachmentName_t  att_Color1 {"Color1"};

            // pipelines
            static constexpr PipelineName_t  mesh_1_rp_Simple {"mesh_1 rp:Simple"};
        } Graphics1;
    } MeshForward;

    static constexpr struct _MinDeferred
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"MinDeferred"};}

        // graphics (0)
        static constexpr struct _Graphics1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Graphics1"};}

            // RenderPass 'Simple.RenderTarget' subpass 'Main'

            static constexpr uint  attachmentsCount = 2;
            static constexpr AttachmentName_t  att_Depth {"Depth"};
            static constexpr AttachmentName_t  att_Color1 {"Color1"};
        } Graphics1;

        // graphics (1)
        static constexpr struct _Graphics2
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Graphics2"};}

            // RenderPass 'Simple.RenderTarget' subpass 'Main'

            static constexpr uint  attachmentsCount = 2;
            static constexpr AttachmentName_t  att_Depth {"Depth"};
            static constexpr AttachmentName_t  att_Color1 {"Color1"};
        } Graphics2;

        // graphics (2)
        static constexpr struct _Graphics3
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Graphics3"};}

            // RenderPass 'Simple.RenderTarget' subpass 'Main'

            static constexpr uint  attachmentsCount = 2;
            static constexpr AttachmentName_t  att_Depth {"Depth"};
            static constexpr AttachmentName_t  att_Color1 {"Color1"};
        } Graphics3;

        // compute (3)
        static constexpr struct _Compute1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Compute1"};}
        } Compute1;

        // graphics (4)
        static constexpr struct _DepthPrepass
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"DepthPrepass"};}

            // RenderPass 'Multipass.V2' subpass 'DepthPrepass'

            static constexpr uint  attachmentsCount = 4;
            static constexpr AttachmentName_t  att_Depth {"Depth"};
            static constexpr AttachmentName_t  att_Color {"Color"};
            static constexpr AttachmentName_t  att_Normal {"Normal"};
            static constexpr AttachmentName_t  att_SwapchainImage {"SwapchainImage"};
        } DepthPrepass;

        // graphics (5)
        static constexpr struct _GBuffer
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"GBuffer"};}

            // RenderPass 'Multipass.V2' subpass 'GBuffer'
        } GBuffer;

        // graphics (6)
        static constexpr struct _Translucent
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Translucent"};}

            // RenderPass 'Multipass.V2' subpass 'Translucent'
        } Translucent;

        // graphics (7)
        static constexpr struct _Lighting
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Lighting"};}

            // RenderPass 'Multipass.V2' subpass 'Lighting'
        } Lighting;

        // graphics (8)
        static constexpr struct _PostProcess
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"PostProcess"};}

            // RenderPass 'Multipass.V2' subpass 'PostProcess'
        } PostProcess;
    } MinDeferred;

    static constexpr struct _MinForward
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"MinForward"};}

        // graphics (0)
        static constexpr struct _Graphics1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Graphics1"};}

            // RenderPass 'Simple.RenderTarget' subpass 'Main'

            static constexpr uint  attachmentsCount = 2;
            static constexpr AttachmentName_t  att_Depth {"Depth"};
            static constexpr AttachmentName_t  att_Color1 {"Color1"};

            // pipelines
            static constexpr PipelineName_t  graphics_1_rp_Simple {"graphics_1 rp:Simple"};
            static constexpr PipelineName_t  graphics_4_rp_Simple {"graphics_4 rp:Simple"};
        } Graphics1;

        // compute (1)
        static constexpr struct _Compute1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Compute1"};}

            // pipelines
            static constexpr PipelineName_t  compute_1_def {"compute_1.def"};
            static constexpr PipelineName_t  compute_2_def {"compute_2.def"};
            static constexpr PipelineName_t  compute_3_def {"compute_3.def"};
        } Compute1;
    } MinForward;

    static constexpr struct _RayTracing
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"RayTracing"};}

        // compute (0)
        static constexpr struct _Trace1
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Trace1"};}
        } Trace1;
    } RayTracing;

    static constexpr struct _UI_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{"UI.RTech"};}

        // graphics (0)
        static constexpr struct _Graphics0
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Graphics0"};}

            // RenderPass 'UIRenderPass.def' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName_t  graphics_1_rp_UI {"graphics_1 rp:UI"};
        } Graphics0;
    } UI_RTech;
}
