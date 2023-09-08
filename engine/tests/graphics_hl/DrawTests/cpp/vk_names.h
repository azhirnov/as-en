namespace RenderTechs
{
	using RenderTechName_t      = AE::Graphics::RenderTechName;
	using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
	using AttachmentName_t      = AE::Graphics::AttachmentName;
	using PipelineName_t        = AE::Graphics::PipelineName;
	using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
	using DescriptorSetName_t   = AE::Graphics::DescriptorSetName;

	static constexpr struct _CanvasDrawTest
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{"CanvasDrawTest"};}

		// graphics (0)
		static constexpr struct _Draw_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{"Draw_1"};}

			// RenderPass 'DrawTest.Draw_1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {"Color"};

			// pipelines
			static constexpr PipelineName_t  draw2 {"draw2"};
			static constexpr PipelineName_t  rect_trilist {"rect_trilist"};
			static constexpr PipelineName_t  rect_trilist_lines {"rect_trilist_lines"};
			static constexpr PipelineName_t  rect_tristrip {"rect_tristrip"};
		} Draw_1;
	} CanvasDrawTest;
}
