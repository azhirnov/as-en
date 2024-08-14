namespace RenderTechs
{
	using Hash_t                = AE::Base::HashVal32;
	using RenderTechName_t      = AE::Graphics::RenderTechName;
	using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
	using AttachmentName_t      = AE::Graphics::AttachmentName;
	using PipelineName_t        = AE::Graphics::PipelineName;
	using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
	using DescriptorSetName_t   = AE::Graphics::DescriptorSetName;

	static constexpr struct _Histogram_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x7085cdccu}};}  // 'Histogram.RTech'

		// compute (0)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'

			// pipelines
			static constexpr PipelineName_t  Histogram_CSPass1 {Hash_t{0x99987a14u}};  // 'Histogram.CSPass1'
			static constexpr PipelineName_t  Histogram_CSPass2 {Hash_t{0x912baeu}};  // 'Histogram.CSPass2'
		} Compute = {};

		// graphics (1)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'Histogram.RPass' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  Histogram_draw {Hash_t{0x7765bf1du}};  // 'Histogram.draw'
		} Graphics = {};
	} Histogram_RTech;

	static constexpr struct _LinearDepth_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x10ac9c9au}};}  // 'LinearDepth.RTech'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'LinearDepth.RPass' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  LinearDepth_draw {Hash_t{0xeeba3c82u}};  // 'LinearDepth.draw'
		} Graphics = {};
	} LinearDepth_RTech;

	static constexpr struct _StencilView_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x3c77554eu}};}  // 'StencilView.RTech'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'StencilView.RPass' subpass 'Main'

			static constexpr uint  attachmentsCount = 2;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'
			static constexpr AttachmentName_t  att_Stencil {Hash_t{0x2f63936au}};  // 'Stencil'

			// pipelines
			static constexpr PipelineName_t  StencilView_draw {Hash_t{0xeced633cu}};  // 'StencilView.draw'
		} Graphics = {};
	} StencilView_RTech;

	static constexpr struct _UI_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xd1c8aabcu}};}  // 'UI.RTech'

		// graphics (0)
		static constexpr struct _UI_BGRA8
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x1e79218eu}};}  // 'UI.BGRA8'

			// RenderPass 'UI.RPass.BGRA8' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  imgui_BGRA8 {Hash_t{0x1195dfd1u}};  // 'imgui.BGRA8'
			static constexpr PipelineName_t  imgui_opaque_BGRA8 {Hash_t{0x74d11b1eu}};  // 'imgui.opaque.BGRA8'
		} UI_BGRA8 = {};

		// graphics (1)
		static constexpr struct _UI_sBGR8_A8
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x78289e94u}};}  // 'UI.sBGR8_A8'

			// RenderPass 'UI.RPass.sBGR8_A8' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  imgui_opaque_sBGR8_A8 {Hash_t{0x789fd3adu}};  // 'imgui.opaque.sBGR8_A8'
			static constexpr PipelineName_t  imgui_sBGR8_A8 {Hash_t{0x3c4b9576u}};  // 'imgui.sBGR8_A8'
		} UI_sBGR8_A8 = {};

		// graphics (2)
		static constexpr struct _UI_RGBA8
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x62bf157cu}};}  // 'UI.RGBA8'

			// RenderPass 'UI.RPass.RGBA8' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  imgui_RGBA8 {Hash_t{0x6d53eb23u}};  // 'imgui.RGBA8'
			static constexpr PipelineName_t  imgui_opaque_RGBA8 {Hash_t{0x8172fecu}};  // 'imgui.opaque.RGBA8'
		} UI_RGBA8 = {};

		// graphics (3)
		static constexpr struct _UI_sRGB8_A8
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x7f1695d8u}};}  // 'UI.sRGB8_A8'

			// RenderPass 'UI.RPass.sRGB8_A8' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  imgui_opaque_sRGB8_A8 {Hash_t{0x7fa1d8e1u}};  // 'imgui.opaque.sRGB8_A8'
			static constexpr PipelineName_t  imgui_sRGB8_A8 {Hash_t{0x3b759e3au}};  // 'imgui.sRGB8_A8'
		} UI_sRGB8_A8 = {};

		// graphics (4)
		static constexpr struct _UI_RGB10_A2
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0xb4f28e24u}};}  // 'UI.RGB10_A2'

			// RenderPass 'UI.RPass.RGB10_A2' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  imgui_RGB10_A2 {Hash_t{0xf09185c6u}};  // 'imgui.RGB10_A2'
			static constexpr PipelineName_t  imgui_opaque_RGB10_A2 {Hash_t{0xb445c31du}};  // 'imgui.opaque.RGB10_A2'
		} UI_RGB10_A2 = {};

		// graphics (5)
		static constexpr struct _UI_RGBA16F
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0xb6fcec0eu}};}  // 'UI.RGBA16F'

			// RenderPass 'UI.RPass.RGBA16F' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  imgui_RGBA16F {Hash_t{0xd1da7d62u}};  // 'imgui.RGBA16F'
			static constexpr PipelineName_t  imgui_opaque_RGBA16F {Hash_t{0x1b1d50eu}};  // 'imgui.opaque.RGBA16F'
		} UI_RGBA16F = {};
	} UI_RTech;
}
