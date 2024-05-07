namespace RenderTechs
{
	using Hash_t                = AE::Base::HashVal32;
	using RenderTechName_t      = AE::Graphics::RenderTechName;
	using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
	using AttachmentName_t      = AE::Graphics::AttachmentName;
	using PipelineName_t        = AE::Graphics::PipelineName;
	using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
	using DescriptorSetName_t   = AE::Graphics::DescriptorSetName;

	static constexpr struct _CanvasDrawTest
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x188afcd5u}};}  // 'CanvasDrawTest'

		// graphics (0)
		static constexpr struct _Draw_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x18c0f564u}};}  // 'Draw_1'

			// RenderPass 'DrawTest.Draw_1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  draw2 {Hash_t{0x8a1a5121u}};  // 'draw2'
			static constexpr PipelineName_t  rect_trilist {Hash_t{0x26dd4574u}};  // 'rect_trilist'
			static constexpr PipelineName_t  rect_tristrip {Hash_t{0x6b247febu}};  // 'rect_tristrip'
		} Draw_1 = {};
	} CanvasDrawTest;

	static constexpr struct _DesktopCanvasDrawTest
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xd6c6cddu}};}  // 'DesktopCanvasDrawTest'

		// graphics (0)
		static constexpr struct _Draw_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x18c0f564u}};}  // 'Draw_1'

			// RenderPass 'DrawTest.Draw_1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  rect_trilist_lines {Hash_t{0x2d67a3f2u}};  // 'rect_trilist_lines'
		} Draw_1 = {};
	} DesktopCanvasDrawTest;
}
