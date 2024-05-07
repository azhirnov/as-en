namespace RenderTechs
{
	using Hash_t                = AE::Base::HashVal32;
	using RenderTechName_t      = AE::Graphics::RenderTechName;
	using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
	using AttachmentName_t      = AE::Graphics::AttachmentName;
	using PipelineName_t        = AE::Graphics::PipelineName;
	using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
	using DescriptorSetName_t   = AE::Graphics::DescriptorSetName;

	static constexpr struct _MeshForward
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x3ef5a74au}};}  // 'MeshForward'

		// graphics (0)
		static constexpr struct _Graphics1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x645d7d6eu}};}  // 'Graphics1'

			// RenderPass 'Simple.Present' subpass 'Main'

			static constexpr uint  attachmentsCount = 2;
			static constexpr AttachmentName_t  att_Depth {Hash_t{0x3b62336du}};  // 'Depth'
			static constexpr AttachmentName_t  att_Color1 {Hash_t{0x5dc0e615u}};  // 'Color1'

			// pipelines
			static constexpr PipelineName_t  mesh_1_rp_Simple {Hash_t{0x8ea2ab77u}};  // 'mesh_1 rp:Simple'
			static constexpr PipelineName_t  mesh_2_rp_Simple {Hash_t{0x1740cd76u}};  // 'mesh_2 rp:Simple'
		} Graphics1 = {};
	} MeshForward;

	static constexpr struct _MinDeferred
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xf038c52du}};}  // 'MinDeferred'

		// graphics (0)
		static constexpr struct _Graphics1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x645d7d6eu}};}  // 'Graphics1'

			// RenderPass 'Simple.RenderTarget' subpass 'Main'

			static constexpr uint  attachmentsCount = 2;
			static constexpr AttachmentName_t  att_Depth {Hash_t{0x3b62336du}};  // 'Depth'
			static constexpr AttachmentName_t  att_Color1 {Hash_t{0x5dc0e615u}};  // 'Color1'
		} Graphics1 = {};

		// graphics (1)
		static constexpr struct _Graphics2
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0xfd542cd4u}};}  // 'Graphics2'

			// RenderPass 'Simple.RenderTarget' subpass 'Main'

			static constexpr uint  attachmentsCount = 2;
			static constexpr AttachmentName_t  att_Depth {Hash_t{0x3b62336du}};  // 'Depth'
			static constexpr AttachmentName_t  att_Color1 {Hash_t{0x5dc0e615u}};  // 'Color1'
		} Graphics2 = {};

		// graphics (2)
		static constexpr struct _Graphics3
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x8a531c42u}};}  // 'Graphics3'

			// RenderPass 'Simple.RenderTarget' subpass 'Main'

			static constexpr uint  attachmentsCount = 2;
			static constexpr AttachmentName_t  att_Depth {Hash_t{0x3b62336du}};  // 'Depth'
			static constexpr AttachmentName_t  att_Color1 {Hash_t{0x5dc0e615u}};  // 'Color1'
		} Graphics3 = {};

		// compute (3)
		static constexpr struct _Compute1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0xb43bd5dau}};}  // 'Compute1'
		} Compute1 = {};

		// graphics (4)
		static constexpr struct _DepthPrepass
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0xeae26aadu}};}  // 'DepthPrepass'

			// RenderPass 'Multipass.V2' subpass 'DepthPrepass'

			static constexpr uint  attachmentsCount = 4;
			static constexpr AttachmentName_t  att_Depth {Hash_t{0x3b62336du}};  // 'Depth'
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'
			static constexpr AttachmentName_t  att_Normal {Hash_t{0x26296dd2u}};  // 'Normal'
			static constexpr AttachmentName_t  att_SwapchainImage {Hash_t{0x2fa71879u}};  // 'SwapchainImage'
		} DepthPrepass = {};

		// graphics (5)
		static constexpr struct _GBuffer
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x9bf4c0eeu}};}  // 'GBuffer'

			// RenderPass 'Multipass.V2' subpass 'GBuffer'
		} GBuffer = {};

		// graphics (6)
		static constexpr struct _Translucent
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x8738a583u}};}  // 'Translucent'

			// RenderPass 'Multipass.V2' subpass 'Translucent'
		} Translucent = {};

		// graphics (7)
		static constexpr struct _Lighting
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x327e7725u}};}  // 'Lighting'

			// RenderPass 'Multipass.V2' subpass 'Lighting'
		} Lighting = {};

		// graphics (8)
		static constexpr struct _PostProcess
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x62dad3bu}};}  // 'PostProcess'

			// RenderPass 'Multipass.V2' subpass 'PostProcess'
		} PostProcess = {};
	} MinDeferred;

	static constexpr struct _MinForward
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x82fa874u}};}  // 'MinForward'

		// graphics (0)
		static constexpr struct _Graphics1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x645d7d6eu}};}  // 'Graphics1'

			// RenderPass 'Simple.RenderTarget' subpass 'Main'

			static constexpr uint  attachmentsCount = 2;
			static constexpr AttachmentName_t  att_Depth {Hash_t{0x3b62336du}};  // 'Depth'
			static constexpr AttachmentName_t  att_Color1 {Hash_t{0x5dc0e615u}};  // 'Color1'

			// pipelines
			static constexpr PipelineName_t  graphics_1_rp_Simple {Hash_t{0xc90b6bc3u}};  // 'graphics_1 rp:Simple'
			static constexpr PipelineName_t  graphics_4_rp_Simple {Hash_t{0xb85cc781u}};  // 'graphics_4 rp:Simple'
		} Graphics1 = {};

		// compute (1)
		static constexpr struct _Compute1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0xb43bd5dau}};}  // 'Compute1'

			// pipelines
			static constexpr PipelineName_t  compute_1_def {Hash_t{0x12d3ec14u}};  // 'compute_1.def'
			static constexpr PipelineName_t  compute_2_def {Hash_t{0x557396c4u}};  // 'compute_2.def'
			static constexpr PipelineName_t  compute_3_def {Hash_t{0x6813bf74u}};  // 'compute_3.def'
		} Compute1 = {};
	} MinForward;

	static constexpr struct _RayTracing
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xd573045cu}};}  // 'RayTracing'

		// compute (0)
		static constexpr struct _Trace1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x2590222au}};}  // 'Trace1'

			// pipelines
			static constexpr PipelineName_t  raytracing_1_def {Hash_t{0xbb76cca4u}};  // 'raytracing_1.def'
		} Trace1 = {};

		static constexpr struct _SBT {
			static constexpr RTShaderBindingName_t  raytracing_1_sbt0 {Hash_t{0x95aec76cu}};  // 'raytracing_1.sbt0'
		} sbt = {};
	} RayTracing;

	static constexpr struct _UI_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xd1c8aabcu}};}  // 'UI.RTech'

		// graphics (0)
		static constexpr struct _Graphics0
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x135a4df8u}};}  // 'Graphics0'

			// RenderPass 'UIRenderPass.def' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  graphics_1_rp_UI {Hash_t{0x5765dc83u}};  // 'graphics_1 rp:UI'
		} Graphics0 = {};
	} UI_RTech;
}
