namespace RenderTechs
{
	using Hash_t                = AE::Base::HashVal32;
	using RenderTechName_t      = AE::Graphics::RenderTechName;
	using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
	using AttachmentName_t      = AE::Graphics::AttachmentName;
	using PipelineName_t        = AE::Graphics::PipelineName;
	using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
	using DescriptorSetName_t   = AE::Graphics::DescriptorSetName;

	static constexpr struct _AsyncCompTestRT
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x5e0c4c21u}};}  // 'AsyncCompTestRT'

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
	} AsyncCompTestRT;

	static constexpr struct _DebugDrawTestRT
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x403d33e7u}};}  // 'DebugDrawTestRT'

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
	} DebugDrawTestRT;

	static constexpr struct _DrawMeshesTestRT
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x564eaca5u}};}  // 'DrawMeshesTestRT'

		// graphics (0)
		static constexpr struct _DrawMeshes_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3d490784u}};}  // 'DrawMeshes_1'

			// RenderPass 'DrawTest.Draw_1' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'
		} DrawMeshes_1 = {};
	} DrawMeshesTestRT;

	static constexpr struct _DrawTestRT
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x9734bd66u}};}  // 'DrawTestRT'

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
	} DrawTestRT;

	static constexpr struct _RayQueryTestRT
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x5d4d9296u}};}  // 'RayQueryTestRT'

		// compute (0)
		static constexpr struct _RayTrace_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x983c0e4bu}};}  // 'RayTrace_1'
		} RayTrace_1 = {};
	} RayQueryTestRT;

	static constexpr struct _RayTracingTestRT
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xc5f2e05fu}};}  // 'RayTracingTestRT'

		// compute (0)
		static constexpr struct _RayTrace_1
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x983c0e4bu}};}  // 'RayTrace_1'
		} RayTrace_1 = {};
	} RayTracingTestRT;

	static constexpr struct _VRSTestRT
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xea4bfcdau}};}  // 'VRSTestRT'

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
	} VRSTestRT;

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
