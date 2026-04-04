namespace RenderTechs
{
	using Hash_t                = AE::Base::HashVal32;
	using RenderTechName_t      = AE::Graphics::RenderTechName;
	using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
	using AttachmentName_t      = AE::Graphics::AttachmentName;
	using PipelineName_t        = AE::Graphics::PipelineName;
	using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
	using DSLayoutName_t        = AE::Graphics::DSLayoutName;
	using ExecutionSetName_t    = AE::Graphics::IndirectExecutionSetName;

	static constexpr struct _UI_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xd1c8aabcu}};}  // 'UI.RTech'

		// graphics (0)
		static constexpr struct _Main
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x1f1a625au}};}  // 'Main'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x15599318u}};  // 'ui.global.ds'

			// RenderPass 'UI.RenderPass' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  ui_color {Hash_t{0x26e9219u}};  // 'ui.color'
			static constexpr PipelineName_t  ui_dbgDraw {Hash_t{0xf85c1e6u}};  // 'ui.dbgDraw'
			static constexpr PipelineName_t  ui_font_alpha {Hash_t{0xa0c173fbu}};  // 'ui.font_alpha'
			static constexpr PipelineName_t  ui_image_alpha {Hash_t{0x6b337090u}};  // 'ui.image_alpha'
			static constexpr PipelineName_t  ui_image_rgba {Hash_t{0x189acbf0u}};  // 'ui.image_rgba'
			static constexpr PipelineName_t  ui_sdf_font {Hash_t{0x4b2ba4c0u}};  // 'ui.sdf_font'
		} Main = {};
	} UI_RTech;
}
