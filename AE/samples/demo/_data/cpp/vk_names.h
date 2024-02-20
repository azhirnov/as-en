namespace RenderTechs
{
    using Hash_t                = AE::Base::HashVal32;
    using RenderTechName_t      = AE::Graphics::RenderTechName;
    using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
    using AttachmentName_t      = AE::Graphics::AttachmentName;
    using PipelineName_t        = AE::Graphics::PipelineName;
    using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
    using DescriptorSetName_t   = AE::Graphics::DescriptorSetName;

    static constexpr struct _Canvas_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x589bb645u}};}  // 'Canvas.RTech'

        // graphics (0)
        static constexpr struct _Main
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x1f1a625au}};}  // 'Main'

            // RenderPass 'UIPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

            // pipelines
            static constexpr PipelineName_t  canvas2d_draw1 {Hash_t{0x821bc820u}};  // 'canvas2d.draw1'
            static constexpr PipelineName_t  canvas2d_draw2 {Hash_t{0x1b12999au}};  // 'canvas2d.draw2'
            static constexpr PipelineName_t  canvas2d_draw3 {Hash_t{0x6c15a90cu}};  // 'canvas2d.draw3'
            static constexpr PipelineName_t  font_draw {Hash_t{0xa4c6d58eu}};  // 'font.draw'
            static constexpr PipelineName_t  sdf_font_draw {Hash_t{0x91af4121u}};  // 'sdf_font.draw'
        } Main;
    } Canvas_RTech;

    static constexpr struct _HwCamera_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x3b9de46eu}};}  // 'HwCamera.RTech'

        // graphics (0)
        static constexpr struct _Main
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x1f1a625au}};}  // 'Main'

            // RenderPass 'UIPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

            // pipelines
            static constexpr PipelineName_t  hw_camera_NV16 {Hash_t{0xbe70194du}};  // 'hw_camera-NV16'
            static constexpr PipelineName_t  hw_camera_NV21 {Hash_t{0xb39df2du}};  // 'hw_camera-NV21'
            static constexpr PipelineName_t  hw_camera_YUV420 {Hash_t{0x57ca02fdu}};  // 'hw_camera-YUV420'
            static constexpr PipelineName_t  hw_camera_YUY2 {Hash_t{0xdbb1ed84u}};  // 'hw_camera-YUY2'
            static constexpr PipelineName_t  hw_camera_YV12 {Hash_t{0x74d3b272u}};  // 'hw_camera-YV12'
        } Main;
    } HwCamera_RTech;

    static constexpr struct _ImGui_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xda495997u}};}  // 'ImGui.RTech'

        // graphics (0)
        static constexpr struct _Main
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x1f1a625au}};}  // 'Main'

            // RenderPass 'UIPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

            // pipelines
            static constexpr PipelineName_t  imgui {Hash_t{0xb0f24415u}};  // 'imgui'
        } Main;
    } ImGui_RTech;

    static constexpr struct _Scene3D_RTech
    {
        constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xdf18d4beu}};}  // 'Scene3D.RTech'

        // graphics (0)
        static constexpr struct _Main
        {
            constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x1f1a625au}};}  // 'Main'

            // RenderPass 'SceneRPass' subpass 'Main'

            static constexpr uint  attachmentsCount = 2;
            static constexpr AttachmentName_t  att_Depth {Hash_t{0x3b62336du}};  // 'Depth'
            static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

            // pipelines
            static constexpr PipelineName_t  camera3d_draw1 {Hash_t{0x4094e898u}};  // 'camera3d.draw1'
            static constexpr PipelineName_t  camera3d_draw2 {Hash_t{0xd99db922u}};  // 'camera3d.draw2'
        } Main;
    } Scene3D_RTech;
}
