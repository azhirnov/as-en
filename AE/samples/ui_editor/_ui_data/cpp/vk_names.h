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
		} Main = {};
	} ImGui_RTech;
}
