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

	static constexpr struct _AsyncComp_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x2e8723e5u}};}  // 'AsyncComp.RTech'

		// graphics (0)
		static constexpr struct _Draw_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x18c0f564u}};}  // 'Draw_1'

			// RenderPass 'DrawTest.Draw_1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  async_comp1_graphics {Hash_t{0xae150324u}};  // 'async_comp1.graphics'
		} Draw_1 = {};

		// compute (1)
		static constexpr struct _Compute_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x1e10a981u}};}  // 'Compute_1'

			// pipelines
			static constexpr PipelineName_t  async_comp1_compute {Hash_t{0x47053fe0u}};  // 'async_comp1.compute'
		} Compute_1 = {};
	} AsyncComp_RTech;

	static constexpr struct _DebugDraw_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x30b65c23u}};}  // 'DebugDraw.RTech'

		// graphics (0)
		static constexpr struct _Draw_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x18c0f564u}};}  // 'Draw_1'

			// RenderPass 'DrawTest.Draw_1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'
		} Draw_1 = {};

		// graphics (1)
		static constexpr struct _Test4_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x1a76367eu}};}  // 'Test4-1'

			// RenderPass 'DrawTest4.Pass1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'
		} Test4_1 = {};

		// graphics (2)
		static constexpr struct _Test4_2
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x837f67c4u}};}  // 'Test4-2'

			// RenderPass 'DrawTest4.Pass2' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'
		} Test4_2 = {};
	} DebugDraw_RTech;

	static constexpr struct _DrawMeshes_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x26c5c361u}};}  // 'DrawMeshes.RTech'

		// graphics (0)
		static constexpr struct _DrawMeshes_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3d490784u}};}  // 'DrawMeshes_1'

			// RenderPass 'DrawTest.Draw_1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'
		} DrawMeshes_1 = {};
	} DrawMeshes_RTech;

	static constexpr struct _DrawTest_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xd3a497bau}};}  // 'DrawTest.RTech'

		// graphics (0)
		static constexpr struct _Draw_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x18c0f564u}};}  // 'Draw_1'

			// RenderPass 'DrawTest.Draw_1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  draw1 {Hash_t{0x1313009bu}};  // 'draw1'
			static constexpr PipelineName_t  draw2 {Hash_t{0x8a1a5121u}};  // 'draw2'
			static constexpr PipelineName_t  draw3 {Hash_t{0xfd1d61b7u}};  // 'draw3'
		} Draw_1 = {};

		// graphics (1)
		static constexpr struct _Test4_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x1a76367eu}};}  // 'Test4-1'

			// RenderPass 'DrawTest4.Pass1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  draw4_1 {Hash_t{0xa453ca03u}};  // 'draw4-1'
		} Test4_1 = {};

		// graphics (2)
		static constexpr struct _Test4_2
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x837f67c4u}};}  // 'Test4-2'

			// RenderPass 'DrawTest4.Pass2' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  draw4_2 {Hash_t{0x3d5a9bb9u}};  // 'draw4-2'
		} Test4_2 = {};
	} DrawTest_RTech;

	static constexpr struct _IndirectCmds_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xeebd1c82u}};}  // 'IndirectCmds.RTech'

		// graphics (0)
		static constexpr struct _Main
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x1f1a625au}};}  // 'Main'

			// RenderPass 'DrawTest.Draw_1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  icb_draw1_v1 {Hash_t{0x6dca48b4u}};  // 'icb.draw1.v1'
			static constexpr PipelineName_t  icb_draw1_v2 {Hash_t{0xf4c3190eu}};  // 'icb.draw1.v2'
			static constexpr PipelineName_t  icb_draw1_v3 {Hash_t{0x83c42998u}};  // 'icb.draw1.v3'

			// indirect execution sets

			static constexpr struct _ICB {
				static constexpr ExecutionSetName_t  icb_set1 {Hash_t{0x60eb7c4u}};  // 'icb.set1'
			} execSet;
		} Main = {};
	} IndirectCmds_RTech;

	static constexpr struct _MultiView_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x9ac79f7au}};}  // 'MultiView.RTech'

		// graphics (0)
		static constexpr struct _MultiView
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x1a68c8dcu}};}  // 'MultiView'

			// RenderPass 'MultiViewRP.Draw' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  multiview {Hash_t{0x38abe541u}};  // 'multiview'
		} MultiView = {};

		// graphics (1)
		static constexpr struct _ViewportArray
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x92359595u}};}  // 'ViewportArray'

			// RenderPass 'DrawTest.Draw_1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  viewportArray {Hash_t{0x214bde60u}};  // 'viewportArray'
		} ViewportArray = {};
	} MultiView_RTech;

	static constexpr struct _OpacityMicromap_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x6dca41bfu}};}  // 'OpacityMicromap.RTech'

		// compute (0)
		static constexpr struct _RayTrace_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x983c0e4bu}};}  // 'RayTrace_1'
		} RayTrace_1 = {};
	} OpacityMicromap_RTech;

	static constexpr struct _RayQuery_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x2dc6fd52u}};}  // 'RayQuery.RTech'

		// compute (0)
		static constexpr struct _RayTrace_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x983c0e4bu}};}  // 'RayTrace_1'
		} RayTrace_1 = {};
	} RayQuery_RTech;

	static constexpr struct _RayTracing_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xb5798f9bu}};}  // 'RayTracing.RTech'

		// compute (0)
		static constexpr struct _RayTrace_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x983c0e4bu}};}  // 'RayTrace_1'
		} RayTrace_1 = {};
	} RayTracing_RTech;

	static constexpr struct _VRS_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x9ac0931eu}};}  // 'VRS.RTech'

		// graphics (0)
		static constexpr struct _nonVRS
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x5179c2bfu}};}  // 'nonVRS'

			// RenderPass 'DrawTest.Draw_1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'
		} nonVRS = {};

		// graphics (1)
		static constexpr struct _VRS
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0xed9fb488u}};}  // 'VRS'

			// RenderPass 'VRSTest.Draw' subpass 'Main'

			static constexpr uint  attachmentsCount = 2;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'
			static constexpr AttachmentName_t  att_ShadingRate {Hash_t{0xe0af6b1u}};  // 'ShadingRate'
		} VRS = {};
	} VRS_RTech;

	static constexpr struct _Ycbcr_RTech
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x7d1d9a9bu}};}  // 'Ycbcr.RTech'

		// graphics (0)
		static constexpr struct _Main
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x1f1a625au}};}  // 'Main'

			// RenderPass 'DrawTest.Draw_1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  ycbcr_draw1 {Hash_t{0x6ba93e51u}};  // 'ycbcr.draw1'
		} Main = {};
	} Ycbcr_RTech;
}
