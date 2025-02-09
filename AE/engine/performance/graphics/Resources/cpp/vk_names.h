namespace RenderTechs
{
	using Hash_t                = AE::Base::HashVal32;
	using RenderTechName_t      = AE::Graphics::RenderTechName;
	using RenderTechPassName_t  = AE::Graphics::RenderTechPassName;
	using AttachmentName_t      = AE::Graphics::AttachmentName;
	using PipelineName_t        = AE::Graphics::PipelineName;
	using RTShaderBindingName_t = AE::Graphics::RTShaderBindingName;
	using DSLayoutName_t        = AE::Graphics::DSLayoutName;

	static constexpr struct _InstBenchRT_High
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x2572f8bfu}};}  // 'InstBenchRT.High'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_fiBitCast_Add_FP32 {Hash_t{0x5bbca5fu}};  // 'InstBench.fiBitCast_Add-FP32'
			static constexpr PipelineName_t  InstBench_fiFloatToInt_Add_FP32 {Hash_t{0x10d880bbu}};  // 'InstBench.fiFloatToInt_Add-FP32'
			static constexpr PipelineName_t  InstBench_fiIntToFloat_Add_FP32 {Hash_t{0xbb7d4f04u}};  // 'InstBench.fiIntToFloat_Add-FP32'
			static constexpr PipelineName_t  InstBench_fiParAdd11_FP32 {Hash_t{0x8e0eddb3u}};  // 'InstBench.fiParAdd11-FP32'
			static constexpr PipelineName_t  InstBench_fiParAdd21_FP32 {Hash_t{0xbfe6c72eu}};  // 'InstBench.fiParAdd21-FP32'
			static constexpr PipelineName_t  InstBench_fiParFMulIAdd_FP32 {Hash_t{0x5075435du}};  // 'InstBench.fiParFMulIAdd-FP32'
			static constexpr PipelineName_t  InstBench_fiParMul_FP32 {Hash_t{0x3417b1cu}};  // 'InstBench.fiParMul-FP32'
			static constexpr PipelineName_t  InstBench_fiSeqAdd_BitCast_FP32 {Hash_t{0x2610c0c1u}};  // 'InstBench.fiSeqAdd_BitCast-FP32'
			static constexpr PipelineName_t  InstBench_fiSeqFMulIAdd_BitCast_FP32 {Hash_t{0x4b4f3faeu}};  // 'InstBench.fiSeqFMulIAdd_BitCast-FP32'
			static constexpr PipelineName_t  InstBench_fiSeqMul_BitCast_FP32 {Hash_t{0x4ff24998u}};  // 'InstBench.fiSeqMul_BitCast-FP32'
			static constexpr PipelineName_t  InstBench_fpACosAdd_FP32 {Hash_t{0x928ecbf5u}};  // 'InstBench.fpACosAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpACosAdd_FP32_MED {Hash_t{0xa194382bu}};  // 'InstBench.fpACosAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpACosAdd_2_FP32 {Hash_t{0xe5ac5f4au}};  // 'InstBench.fpACosAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpACosAdd_2_FP32_MED {Hash_t{0x774cd474u}};  // 'InstBench.fpACosAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpACosAdd_3_FP32 {Hash_t{0x2ef08cefu}};  // 'InstBench.fpACosAdd_3-FP32'
			static constexpr PipelineName_t  InstBench_fpACosAdd_3_FP32_MED {Hash_t{0x988ebf4au}};  // 'InstBench.fpACosAdd_3-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpACosH_FP32 {Hash_t{0x46ede266u}};  // 'InstBench.fpACosH-FP32'
			static constexpr PipelineName_t  InstBench_fpACosH_FP32_MED {Hash_t{0xf69325a7u}};  // 'InstBench.fpACosH-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpASinAdd_FP32 {Hash_t{0x6ce9da5fu}};  // 'InstBench.fpASinAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpASinAdd_FP32_MED {Hash_t{0xee1f2a76u}};  // 'InstBench.fpASinAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpASinAdd_2_FP32 {Hash_t{0xdbf4d5bcu}};  // 'InstBench.fpASinAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpASinAdd_2_FP32_MED {Hash_t{0x8be0c9aeu}};  // 'InstBench.fpASinAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpASinAdd_3_FP32 {Hash_t{0x10a80619u}};  // 'InstBench.fpASinAdd_3-FP32'
			static constexpr PipelineName_t  InstBench_fpASinAdd_3_FP32_MED {Hash_t{0x6422a290u}};  // 'InstBench.fpASinAdd_3-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpASinH_FP32 {Hash_t{0x2a4b549eu}};  // 'InstBench.fpASinH-FP32'
			static constexpr PipelineName_t  InstBench_fpASinH_FP32_MED {Hash_t{0xc8cbaf51u}};  // 'InstBench.fpASinH-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpATanAdd_FP32 {Hash_t{0x56058f28u}};  // 'InstBench.fpATanAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpATanAdd_FP32_MED {Hash_t{0x46473b5au}};  // 'InstBench.fpATanAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpATanAdd_2_FP32 {Hash_t{0xa08ba51au}};  // 'InstBench.fpATanAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpATanAdd_2_FP32_MED {Hash_t{0xd8612aaau}};  // 'InstBench.fpATanAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpATanAdd_3_FP32 {Hash_t{0x6bd776bfu}};  // 'InstBench.fpATanAdd_3-FP32'
			static constexpr PipelineName_t  InstBench_fpATanAdd_3_FP32_MED {Hash_t{0x37a34194u}};  // 'InstBench.fpATanAdd_3-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpATanH_FP32 {Hash_t{0x9dcf22e2u}};  // 'InstBench.fpATanH-FP32'
			static constexpr PipelineName_t  InstBench_fpATanH_FP32_MED {Hash_t{0xb3b4dff7u}};  // 'InstBench.fpATanH-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpAbsAdd_FP32 {Hash_t{0x7b911862u}};  // 'InstBench.fpAbsAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpAbsAdd_FP32_MED {Hash_t{0x1a403e1du}};  // 'InstBench.fpAbsAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpAdd_FP32 {Hash_t{0x2d41a6c3u}};  // 'InstBench.fpAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpAdd_FP32_MED {Hash_t{0x2367a2b9u}};  // 'InstBench.fpAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCbrtAdd_2_FP32 {Hash_t{0xd1a6f006u}};  // 'InstBench.fpCbrtAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpCbrtAdd_2_FP32_MED {Hash_t{0xaff13593u}};  // 'InstBench.fpCbrtAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCbrtAdd_3_FP32 {Hash_t{0x1afa23a3u}};  // 'InstBench.fpCbrtAdd_3-FP32'
			static constexpr PipelineName_t  InstBench_fpCbrtAdd_3_FP32_MED {Hash_t{0x40335eadu}};  // 'InstBench.fpCbrtAdd_3-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCbrtExpAdd_FP32 {Hash_t{0x46791246u}};  // 'InstBench.fpCbrtExpAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpCbrtExpAdd_FP32_MED {Hash_t{0xf648b657u}};  // 'InstBench.fpCbrtExpAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCbrtPowAdd_FP32 {Hash_t{0x38cb8d97u}};  // 'InstBench.fpCbrtPowAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpCbrtPowAdd_FP32_MED {Hash_t{0xffa10767u}};  // 'InstBench.fpCbrtPowAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCeilAdd_FP32 {Hash_t{0x6efbed19u}};  // 'InstBench.fpCeilAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpCeilAdd_FP32_MED {Hash_t{0xe7becf7du}};  // 'InstBench.fpCeilAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpClampAdd_FP32 {Hash_t{0xdee9b4eau}};  // 'InstBench.fpClampAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpClampAdd_FP32_MED {Hash_t{0xa395a054u}};  // 'InstBench.fpClampAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpClampSNormAdd_FP32 {Hash_t{0x99836fd5u}};  // 'InstBench.fpClampSNormAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpClampSNormAdd_FP32_MED {Hash_t{0xfdabc56cu}};  // 'InstBench.fpClampSNormAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpClampUNormAdd_FP32 {Hash_t{0x3966b008u}};  // 'InstBench.fpClampUNormAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpClampUNormAdd_FP32_MED {Hash_t{0x5a9f7264u}};  // 'InstBench.fpClampUNormAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCosAdd_FP32 {Hash_t{0xd6032a13u}};  // 'InstBench.fpCosAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpCosAdd_FP32_MED {Hash_t{0x53cffd7u}};  // 'InstBench.fpCosAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCosAdd_2_FP32 {Hash_t{0x3a859d43u}};  // 'InstBench.fpCosAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpCosAdd_2_FP32_MED {Hash_t{0xca45897eu}};  // 'InstBench.fpCosAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCosH_FP32 {Hash_t{0x59536effu}};  // 'InstBench.fpCosH-FP32'
			static constexpr PipelineName_t  InstBench_fpCosH_FP32_MED {Hash_t{0x29bae7aeu}};  // 'InstBench.fpCosH-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCross3_FP32 {Hash_t{0x3abd5f21u}};  // 'InstBench.fpCross3-FP32'
			static constexpr PipelineName_t  InstBench_fpCross3_FP32_MED {Hash_t{0xdede4823u}};  // 'InstBench.fpCross3-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpDistanceAdd_FP32 {Hash_t{0x2405a6bdu}};  // 'InstBench.fpDistanceAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpDistanceAdd_FP32_MED {Hash_t{0x454e47b3u}};  // 'InstBench.fpDistanceAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpDivAdd_FP32 {Hash_t{0xb8b10a98u}};  // 'InstBench.fpDivAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpDivAdd_FP32_MED {Hash_t{0x7bd00868u}};  // 'InstBench.fpDivAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpDot_FP32 {Hash_t{0x6470fec6u}};  // 'InstBench.fpDot-FP32'
			static constexpr PipelineName_t  InstBench_fpDot_FP32_MED {Hash_t{0xbf4bd305u}};  // 'InstBench.fpDot-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpDualAdd_FP32 {Hash_t{0xa74280a9u}};  // 'InstBench.fpDualAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpDualAdd_FP32_MED {Hash_t{0x6d7cb671u}};  // 'InstBench.fpDualAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpDualMulAdd_FP32 {Hash_t{0xd8170176u}};  // 'InstBench.fpDualMulAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpDualMulAdd_FP32_MED {Hash_t{0xf4caabb5u}};  // 'InstBench.fpDualMulAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpExp2Add_FP32 {Hash_t{0x858ff1f3u}};  // 'InstBench.fpExp2Add-FP32'
			static constexpr PipelineName_t  InstBench_fpExp2Add_FP32_MED {Hash_t{0x37cbb337u}};  // 'InstBench.fpExp2Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpExpAdd_FP32 {Hash_t{0xc2494604u}};  // 'InstBench.fpExpAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpExpAdd_FP32_MED {Hash_t{0x9973d147u}};  // 'InstBench.fpExpAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpFMA_FP32 {Hash_t{0x2ac04710u}};  // 'InstBench.fpFMA-FP32'
			static constexpr PipelineName_t  InstBench_fpFMA_FP32_MED {Hash_t{0x35a32f0u}};  // 'InstBench.fpFMA-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpFloorAdd_FP32 {Hash_t{0xbbb3d444u}};  // 'InstBench.fpFloorAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpFloorAdd_FP32_MED {Hash_t{0x3b7a254bu}};  // 'InstBench.fpFloorAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpFractAdd_FP32 {Hash_t{0x65ca58a4u}};  // 'InstBench.fpFractAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpFractAdd_FP32_MED {Hash_t{0x9abcbbfbu}};  // 'InstBench.fpFractAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpInvSqrtAdd_FP32 {Hash_t{0x75ee5fc7u}};  // 'InstBench.fpInvSqrtAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpInvSqrtAdd_FP32_MED {Hash_t{0x5a3f0451u}};  // 'InstBench.fpInvSqrtAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpLengthAdd_FP32 {Hash_t{0x6d8c8c07u}};  // 'InstBench.fpLengthAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpLengthAdd_FP32_MED {Hash_t{0xe71af04du}};  // 'InstBench.fpLengthAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpLerpClamp_FP32 {Hash_t{0xc0395277u}};  // 'InstBench.fpLerpClamp-FP32'
			static constexpr PipelineName_t  InstBench_fpLerpClamp_FP32_MED {Hash_t{0x3ae7cd43u}};  // 'InstBench.fpLerpClamp-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpLog2Add_FP32 {Hash_t{0x5a9980bbu}};  // 'InstBench.fpLog2Add-FP32'
			static constexpr PipelineName_t  InstBench_fpLog2Add_FP32_MED {Hash_t{0x36acbb10u}};  // 'InstBench.fpLog2Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpLogAdd_FP32 {Hash_t{0xb4d7cda8u}};  // 'InstBench.fpLogAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpLogAdd_FP32_MED {Hash_t{0x250af006u}};  // 'InstBench.fpLogAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpMinMaxAdd_FP32 {Hash_t{0x634d0b04u}};  // 'InstBench.fpMinMaxAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpMinMaxAdd_FP32_MED {Hash_t{0x248c99aeu}};  // 'InstBench.fpMinMaxAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpModAdd_FP32 {Hash_t{0x4cd42eadu}};  // 'InstBench.fpModAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpModAdd_FP32_MED {Hash_t{0x213b4720u}};  // 'InstBench.fpModAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpMul_FP32 {Hash_t{0x5714a69bu}};  // 'InstBench.fpMul-FP32'
			static constexpr PipelineName_t  InstBench_fpMul_FP32_MED {Hash_t{0x5ac2b4c7u}};  // 'InstBench.fpMul-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpMulAdd_FP32 {Hash_t{0xc7809d94u}};  // 'InstBench.fpMulAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpMulAdd_FP32_MED {Hash_t{0xcfbd72bau}};  // 'InstBench.fpMulAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpNormalizeAdd_FP32 {Hash_t{0xe9742883u}};  // 'InstBench.fpNormalizeAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpNormalizeAdd_FP32_MED {Hash_t{0x6de00811u}};  // 'InstBench.fpNormalizeAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpPow17Add_FP32 {Hash_t{0xff22aa8eu}};  // 'InstBench.fpPow17Add-FP32'
			static constexpr PipelineName_t  InstBench_fpPow17Add_FP32_MED {Hash_t{0x650e50a1u}};  // 'InstBench.fpPow17Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpPow1ov17Add_FP32 {Hash_t{0x91a664e0u}};  // 'InstBench.fpPow1ov17Add-FP32'
			static constexpr PipelineName_t  InstBench_fpPow1ov17Add_FP32_MED {Hash_t{0xd0237acbu}};  // 'InstBench.fpPow1ov17Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpPow1ov33Add_FP32 {Hash_t{0xc8bfe5d1u}};  // 'InstBench.fpPow1ov33Add-FP32'
			static constexpr PipelineName_t  InstBench_fpPow1ov33Add_FP32_MED {Hash_t{0xf1f4d4f0u}};  // 'InstBench.fpPow1ov33Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpPow1ov7Add_FP32 {Hash_t{0xd17a6df6u}};  // 'InstBench.fpPow1ov7Add-FP32'
			static constexpr PipelineName_t  InstBench_fpPow1ov7Add_FP32_MED {Hash_t{0xf1d18284u}};  // 'InstBench.fpPow1ov7Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpPow33Add_FP32 {Hash_t{0xa63b2bbfu}};  // 'InstBench.fpPow33Add-FP32'
			static constexpr PipelineName_t  InstBench_fpPow33Add_FP32_MED {Hash_t{0x44d9fe9au}};  // 'InstBench.fpPow33Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpPow8Add_FP32 {Hash_t{0xe10b2bb0u}};  // 'InstBench.fpPow8Add-FP32'
			static constexpr PipelineName_t  InstBench_fpPow8Add_FP32_MED {Hash_t{0x2b440c0cu}};  // 'InstBench.fpPow8Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpQdrtInvsqrtAdd_FP32 {Hash_t{0x6b6e6251u}};  // 'InstBench.fpQdrtInvsqrtAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpQdrtInvsqrtAdd_FP32_MED {Hash_t{0x2f2b69c2u}};  // 'InstBench.fpQdrtInvsqrtAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpQdrtPowAdd_FP32 {Hash_t{0x2cede8ffu}};  // 'InstBench.fpQdrtPowAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpQdrtPowAdd_FP32_MED {Hash_t{0x962b8619u}};  // 'InstBench.fpQdrtPowAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpQdrtSqrtAdd_FP32 {Hash_t{0x6fb1f23du}};  // 'InstBench.fpQdrtSqrtAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpQdrtSqrtAdd_FP32_MED {Hash_t{0xa2058440u}};  // 'InstBench.fpQdrtSqrtAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpRoundAdd_FP32 {Hash_t{0xc4d20672u}};  // 'InstBench.fpRoundAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpRoundAdd_FP32_MED {Hash_t{0xdc6b5523u}};  // 'InstBench.fpRoundAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpRoundEvenAdd_FP32 {Hash_t{0x7d14d5a1u}};  // 'InstBench.fpRoundEvenAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpRoundEvenAdd_FP32_MED {Hash_t{0x4858b3ddu}};  // 'InstBench.fpRoundEvenAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve1Add_FP32 {Hash_t{0x6b5d339u}};  // 'InstBench.fpSRGBCurve1Add-FP32'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve1Add_FP32_MED {Hash_t{0x44ea2338u}};  // 'InstBench.fpSRGBCurve1Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve2Add_FP32 {Hash_t{0x3f38effcu}};  // 'InstBench.fpSRGBCurve2Add-FP32'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve2Add_FP32_MED {Hash_t{0xf9204ff6u}};  // 'InstBench.fpSRGBCurve2Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve3Add_FP32 {Hash_t{0x2843fbbfu}};  // 'InstBench.fpSRGBCurve3Add-FP32'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve3Add_FP32_MED {Hash_t{0x24b69673u}};  // 'InstBench.fpSRGBCurve3Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpScalarAdd_FP32 {Hash_t{0x54934136u}};  // 'InstBench.fpScalarAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpScalarAdd_FP32_MED {Hash_t{0xa4a5ec0cu}};  // 'InstBench.fpScalarAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpScalarMulAdd_FP32 {Hash_t{0xd6dfa05u}};  // 'InstBench.fpScalarMulAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpScalarMulAdd_FP32_MED {Hash_t{0x17b88fd1u}};  // 'InstBench.fpScalarMulAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSignAdd_FP32 {Hash_t{0x58c68a17u}};  // 'InstBench.fpSignAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpSignAdd_FP32_MED {Hash_t{0xefabb8ccu}};  // 'InstBench.fpSignAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSignAdd_2_FP32 {Hash_t{0x883f756au}};  // 'InstBench.fpSignAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpSignAdd_2_FP32_MED {Hash_t{0x8bcb7c40u}};  // 'InstBench.fpSignAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSinAdd_FP32 {Hash_t{0x28643bb9u}};  // 'InstBench.fpSinAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpSinAdd_FP32_MED {Hash_t{0x4ab7ed8au}};  // 'InstBench.fpSinAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSinAdd_2_FP32 {Hash_t{0x4dd17b5u}};  // 'InstBench.fpSinAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpSinAdd_2_FP32_MED {Hash_t{0x36e994a4u}};  // 'InstBench.fpSinAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSinH_FP32 {Hash_t{0x35f5d807u}};  // 'InstBench.fpSinH-FP32'
			static constexpr PipelineName_t  InstBench_fpSinH_FP32_MED {Hash_t{0x17e26d58u}};  // 'InstBench.fpSinH-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSmoothstepAdd_FP32 {Hash_t{0xde3229f1u}};  // 'InstBench.fpSmoothstepAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpSmoothstepAdd_FP32_MED {Hash_t{0xb38a47bdu}};  // 'InstBench.fpSmoothstepAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_FP32 {Hash_t{0xb49a2ddau}};  // 'InstBench.fpSqrtAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_FP32_MED {Hash_t{0x80979f9u}};  // 'InstBench.fpSqrtAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_2_FP32 {Hash_t{0xbed9067eu}};  // 'InstBench.fpSqrtAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_2_FP32_MED {Hash_t{0xc599ab72u}};  // 'InstBench.fpSqrtAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_3_FP32 {Hash_t{0x7585d5dbu}};  // 'InstBench.fpSqrtAdd_3-FP32'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_3_FP32_MED {Hash_t{0x2a5bc04cu}};  // 'InstBench.fpSqrtAdd_3-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpStepAdd_FP32 {Hash_t{0x5d534a05u}};  // 'InstBench.fpStepAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpStepAdd_FP32_MED {Hash_t{0xf8483b48u}};  // 'InstBench.fpStepAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpTanAdd_FP32 {Hash_t{0x12886eceu}};  // 'InstBench.fpTanAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpTanAdd_FP32_MED {Hash_t{0xe2effca6u}};  // 'InstBench.fpTanAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpTanAdd_2_FP32 {Hash_t{0x7fa26713u}};  // 'InstBench.fpTanAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpTanAdd_2_FP32_MED {Hash_t{0x656877a0u}};  // 'InstBench.fpTanAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpTanH_FP32 {Hash_t{0x8271ae7bu}};  // 'InstBench.fpTanH-FP32'
			static constexpr PipelineName_t  InstBench_fpTanH_FP32_MED {Hash_t{0x6c9d1dfeu}};  // 'InstBench.fpTanH-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpTruncAdd_FP32 {Hash_t{0x92e15874u}};  // 'InstBench.fpTruncAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpTruncAdd_FP32_MED {Hash_t{0xe94198b4u}};  // 'InstBench.fpTruncAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_iAbsAdd_INT32 {Hash_t{0x33763a40u}};  // 'InstBench.iAbsAdd-INT32'
			static constexpr PipelineName_t  InstBench_iAbsAdd_INT32_LOW {Hash_t{0x3d19a4ccu}};  // 'InstBench.iAbsAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iAbsAdd_INT32_MED {Hash_t{0x428a67afu}};  // 'InstBench.iAbsAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iAdd_INT32 {Hash_t{0xd2ce4ff7u}};  // 'InstBench.iAdd-INT32'
			static constexpr PipelineName_t  InstBench_iAdd_INT32_LOW {Hash_t{0x14978e0au}};  // 'InstBench.iAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iAdd_INT32_MED {Hash_t{0x6b044d69u}};  // 'InstBench.iAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iAdd_UINT32 {Hash_t{0x94cd5212u}};  // 'InstBench.iAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iAdd_UINT32_LOW {Hash_t{0x555ab845u}};  // 'InstBench.iAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iAdd_UINT32_MED {Hash_t{0x2ac97b26u}};  // 'InstBench.iAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iAndAdd_INT32 {Hash_t{0xfc1e3216u}};  // 'InstBench.iAndAdd-INT32'
			static constexpr PipelineName_t  InstBench_iAndAdd_INT32_LOW {Hash_t{0x7b939b84u}};  // 'InstBench.iAndAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iAndAdd_INT32_MED {Hash_t{0x40058e7u}};  // 'InstBench.iAndAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iAndAdd_UINT32 {Hash_t{0x43ee5081u}};  // 'InstBench.iAndAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iAndAdd_UINT32_LOW {Hash_t{0x5f351277u}};  // 'InstBench.iAndAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iAndAdd_UINT32_MED {Hash_t{0x20a6d114u}};  // 'InstBench.iAndAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iBitCount_INT32 {Hash_t{0x12ae04f5u}};  // 'InstBench.iBitCount-INT32'
			static constexpr PipelineName_t  InstBench_iBitCount_INT32_LOW {Hash_t{0x3cc88db7u}};  // 'InstBench.iBitCount-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iBitCount_INT32_MED {Hash_t{0x435b4ed4u}};  // 'InstBench.iBitCount-INT32_MED'
			static constexpr PipelineName_t  InstBench_iBitCount_UINT32 {Hash_t{0x7a035375u}};  // 'InstBench.iBitCount-UINT32'
			static constexpr PipelineName_t  InstBench_iBitCount_UINT32_LOW {Hash_t{0xe0a22877u}};  // 'InstBench.iBitCount-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iBitCount_UINT32_MED {Hash_t{0x9f31eb14u}};  // 'InstBench.iBitCount-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iClampAdd_INT32 {Hash_t{0x81b9a310u}};  // 'InstBench.iClampAdd-INT32'
			static constexpr PipelineName_t  InstBench_iClampAdd_INT32_LOW {Hash_t{0x6ddb3130u}};  // 'InstBench.iClampAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iClampAdd_INT32_MED {Hash_t{0x1248f253u}};  // 'InstBench.iClampAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iClampAdd_UINT32 {Hash_t{0xaaf05225u}};  // 'InstBench.iClampAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iClampAdd_UINT32_LOW {Hash_t{0x932f2d48u}};  // 'InstBench.iClampAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iClampAdd_UINT32_MED {Hash_t{0xecbcee2bu}};  // 'InstBench.iClampAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_INT32 {Hash_t{0x7df98a15u}};  // 'InstBench.iClampConstAdd-INT32'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_INT32_LOW {Hash_t{0x29eb4d5fu}};  // 'InstBench.iClampConstAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_INT32_MED {Hash_t{0x56788e3cu}};  // 'InstBench.iClampConstAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_UINT32 {Hash_t{0xda66e683u}};  // 'InstBench.iClampConstAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_UINT32_LOW {Hash_t{0x4e6661fdu}};  // 'InstBench.iClampConstAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_UINT32_MED {Hash_t{0x31f5a29eu}};  // 'InstBench.iClampConstAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iDivAdd_INT32 {Hash_t{0x6edd0150u}};  // 'InstBench.iDivAdd-INT32'
			static constexpr PipelineName_t  InstBench_iDivAdd_INT32_LOW {Hash_t{0x1d17b149u}};  // 'InstBench.iDivAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iDivAdd_INT32_MED {Hash_t{0x6284722au}};  // 'InstBench.iDivAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iDivAdd_UINT32 {Hash_t{0xdcc37717u}};  // 'InstBench.iDivAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iDivAdd_UINT32_LOW {Hash_t{0xba862850u}};  // 'InstBench.iDivAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iDivAdd_UINT32_MED {Hash_t{0xc515eb33u}};  // 'InstBench.iDivAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iFindLSB_INT32 {Hash_t{0x2d70e639u}};  // 'InstBench.iFindLSB-INT32'
			static constexpr PipelineName_t  InstBench_iFindLSB_INT32_LOW {Hash_t{0x3e81d6bu}};  // 'InstBench.iFindLSB-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iFindLSB_INT32_MED {Hash_t{0x7c7bde08u}};  // 'InstBench.iFindLSB-INT32_MED'
			static constexpr PipelineName_t  InstBench_iFindLSB_UINT32 {Hash_t{0xe8ee030cu}};  // 'InstBench.iFindLSB-UINT32'
			static constexpr PipelineName_t  InstBench_iFindLSB_UINT32_LOW {Hash_t{0x6ff89618u}};  // 'InstBench.iFindLSB-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iFindLSB_UINT32_MED {Hash_t{0x106b557bu}};  // 'InstBench.iFindLSB-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iFindMSB_INT32 {Hash_t{0x3a0bf27au}};  // 'InstBench.iFindMSB-INT32'
			static constexpr PipelineName_t  InstBench_iFindMSB_INT32_LOW {Hash_t{0xde7ec4eeu}};  // 'InstBench.iFindMSB-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iFindMSB_INT32_MED {Hash_t{0xa1ed078du}};  // 'InstBench.iFindMSB-INT32_MED'
			static constexpr PipelineName_t  InstBench_iFindMSB_UINT32 {Hash_t{0x72c6832u}};  // 'InstBench.iFindMSB-UINT32'
			static constexpr PipelineName_t  InstBench_iFindMSB_UINT32_LOW {Hash_t{0xf2f7776eu}};  // 'InstBench.iFindMSB-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iFindMSB_UINT32_MED {Hash_t{0x8d64b40du}};  // 'InstBench.iFindMSB-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_INT32 {Hash_t{0xf97be0e3u}};  // 'InstBench.iMinMaxAdd-INT32'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_INT32_LOW {Hash_t{0x2cc6f722u}};  // 'InstBench.iMinMaxAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_INT32_MED {Hash_t{0x53553441u}};  // 'InstBench.iMinMaxAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_UINT32 {Hash_t{0x8e3c33c0u}};  // 'InstBench.iMinMaxAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_UINT32_LOW {Hash_t{0x60d741c6u}};  // 'InstBench.iMinMaxAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_UINT32_MED {Hash_t{0x1f4482a5u}};  // 'InstBench.iMinMaxAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iModAdd_INT32 {Hash_t{0x389aa057u}};  // 'InstBench.iModAdd-INT32'
			static constexpr PipelineName_t  InstBench_iModAdd_INT32_LOW {Hash_t{0x654a93a4u}};  // 'InstBench.iModAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iModAdd_INT32_MED {Hash_t{0x1ad950c7u}};  // 'InstBench.iModAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iModAdd_UINT32 {Hash_t{0x42f1a515u}};  // 'InstBench.iModAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iModAdd_UINT32_LOW {Hash_t{0x6445ebb7u}};  // 'InstBench.iModAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iModAdd_UINT32_MED {Hash_t{0x1bd628d4u}};  // 'InstBench.iModAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iMul_INT32 {Hash_t{0xb704c331u}};  // 'InstBench.iMul-INT32'
			static constexpr PipelineName_t  InstBench_iMul_INT32_LOW {Hash_t{0xa3537727u}};  // 'InstBench.iMul-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iMul_INT32_MED {Hash_t{0xdcc0b444u}};  // 'InstBench.iMul-INT32_MED'
			static constexpr PipelineName_t  InstBench_iMul_UINT32 {Hash_t{0xe6afff1bu}};  // 'InstBench.iMul-UINT32'
			static constexpr PipelineName_t  InstBench_iMul_UINT32_LOW {Hash_t{0x103220c9u}};  // 'InstBench.iMul-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iMul_UINT32_MED {Hash_t{0x6fa1e3aau}};  // 'InstBench.iMul-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iMulAdd_INT32 {Hash_t{0x67147cecu}};  // 'InstBench.iMulAdd-INT32'
			static constexpr PipelineName_t  InstBench_iMulAdd_INT32_LOW {Hash_t{0x757e6fcbu}};  // 'InstBench.iMulAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iMulAdd_INT32_MED {Hash_t{0xaedaca8u}};  // 'InstBench.iMulAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iMulAdd_UINT32 {Hash_t{0x1e1d41cdu}};  // 'InstBench.iMulAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iMulAdd_UINT32_LOW {Hash_t{0xb958a382u}};  // 'InstBench.iMulAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iMulAdd_UINT32_MED {Hash_t{0xc6cb60e1u}};  // 'InstBench.iMulAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iMulExtended_INT32 {Hash_t{0xf21432adu}};  // 'InstBench.iMulExtended-INT32'
			static constexpr PipelineName_t  InstBench_iMulExtended_INT32_LOW {Hash_t{0xacb79792u}};  // 'InstBench.iMulExtended-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iMulExtended_INT32_MED {Hash_t{0xd32454f1u}};  // 'InstBench.iMulExtended-INT32_MED'
			static constexpr PipelineName_t  InstBench_iOrAdd_INT32 {Hash_t{0x2bfca231u}};  // 'InstBench.iOrAdd-INT32'
			static constexpr PipelineName_t  InstBench_iOrAdd_INT32_LOW {Hash_t{0xcf9a62eau}};  // 'InstBench.iOrAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iOrAdd_INT32_MED {Hash_t{0xb009a189u}};  // 'InstBench.iOrAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iOrAdd_UINT32 {Hash_t{0xe633077au}};  // 'InstBench.iOrAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iOrAdd_UINT32_LOW {Hash_t{0xf58b57d1u}};  // 'InstBench.iOrAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iOrAdd_UINT32_MED {Hash_t{0x8a1894b2u}};  // 'InstBench.iOrAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iShift_INT32 {Hash_t{0xa34e5e14u}};  // 'InstBench.iShift-INT32'
			static constexpr PipelineName_t  InstBench_iShift_INT32_LOW {Hash_t{0xd138707au}};  // 'InstBench.iShift-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iShift_INT32_MED {Hash_t{0xaeabb319u}};  // 'InstBench.iShift-INT32_MED'
			static constexpr PipelineName_t  InstBench_iShift_UINT32 {Hash_t{0xadbf61c1u}};  // 'InstBench.iShift-UINT32'
			static constexpr PipelineName_t  InstBench_iShift_UINT32_LOW {Hash_t{0x59a6687u}};  // 'InstBench.iShift-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iShift_UINT32_MED {Hash_t{0x7a09a5e4u}};  // 'InstBench.iShift-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iShiftConst_INT32 {Hash_t{0xbc108a38u}};  // 'InstBench.iShiftConst-INT32'
			static constexpr PipelineName_t  InstBench_iShiftConst_INT32_LOW {Hash_t{0x186c803fu}};  // 'InstBench.iShiftConst-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iShiftConst_INT32_MED {Hash_t{0x67ff435cu}};  // 'InstBench.iShiftConst-INT32_MED'
			static constexpr PipelineName_t  InstBench_iShiftConst_UINT32 {Hash_t{0x9f7853f6u}};  // 'InstBench.iShiftConst-UINT32'
			static constexpr PipelineName_t  InstBench_iShiftConst_UINT32_LOW {Hash_t{0x3e58768u}};  // 'InstBench.iShiftConst-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iShiftConst_UINT32_MED {Hash_t{0x7c76440bu}};  // 'InstBench.iShiftConst-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iSignAdd_INT32 {Hash_t{0x3a1dec29u}};  // 'InstBench.iSignAdd-INT32'
			static constexpr PipelineName_t  InstBench_iSignAdd_INT32_LOW {Hash_t{0xde65f0fu}};  // 'InstBench.iSignAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iSignAdd_INT32_MED {Hash_t{0x72759c6cu}};  // 'InstBench.iSignAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iSignAdd_2_INT32 {Hash_t{0x539adec2u}};  // 'InstBench.iSignAdd_2-INT32'
			static constexpr PipelineName_t  InstBench_iSignAdd_2_INT32_LOW {Hash_t{0x6bdb7fb8u}};  // 'InstBench.iSignAdd_2-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iSignAdd_2_INT32_MED {Hash_t{0x1448bcdbu}};  // 'InstBench.iSignAdd_2-INT32_MED'
			static constexpr PipelineName_t  InstBench_iXorAdd_INT32 {Hash_t{0x8691cb14u}};  // 'InstBench.iXorAdd-INT32'
			static constexpr PipelineName_t  InstBench_iXorAdd_INT32_LOW {Hash_t{0x674407f6u}};  // 'InstBench.iXorAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iXorAdd_INT32_MED {Hash_t{0x18d7c495u}};  // 'InstBench.iXorAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iXorAdd_UINT32 {Hash_t{0xad9abe54u}};  // 'InstBench.iXorAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iXorAdd_UINT32_LOW {Hash_t{0xe122d5fbu}};  // 'InstBench.iXorAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iXorAdd_UINT32_MED {Hash_t{0x9eb11698u}};  // 'InstBench.iXorAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_uAddCarry_UINT32 {Hash_t{0x48dd9610u}};  // 'InstBench.uAddCarry-UINT32'
			static constexpr PipelineName_t  InstBench_uAddCarry_UINT32_LOW {Hash_t{0xac52c2fcu}};  // 'InstBench.uAddCarry-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_uAddCarry_UINT32_MED {Hash_t{0xd3c1019fu}};  // 'InstBench.uAddCarry-UINT32_MED'
			static constexpr PipelineName_t  InstBench_uMulExtended_UINT32 {Hash_t{0xecd81be8u}};  // 'InstBench.uMulExtended-UINT32'
			static constexpr PipelineName_t  InstBench_uMulExtended_UINT32_LOW {Hash_t{0xaaa1e7d3u}};  // 'InstBench.uMulExtended-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_uMulExtended_UINT32_MED {Hash_t{0xd53224b0u}};  // 'InstBench.uMulExtended-UINT32_MED'
			static constexpr PipelineName_t  InstBench_uSubBorrow_UINT32 {Hash_t{0x3ef962deu}};  // 'InstBench.uSubBorrow-UINT32'
			static constexpr PipelineName_t  InstBench_uSubBorrow_UINT32_LOW {Hash_t{0xc083b56au}};  // 'InstBench.uSubBorrow-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_uSubBorrow_UINT32_MED {Hash_t{0xbf107609u}};  // 'InstBench.uSubBorrow-UINT32_MED'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_High;

	static constexpr struct _InstBenchRT_High_ShaderFloat16
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x6791aebdu}};}  // 'InstBenchRT.High.ShaderFloat16'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_f16ParFMA_FP16 {Hash_t{0xae3b2ad2u}};  // 'InstBench.f16ParFMA-FP16'
			static constexpr PipelineName_t  InstBench_f16ParMulAdd_FP16 {Hash_t{0x6f6e83ecu}};  // 'InstBench.f16ParMulAdd-FP16'
			static constexpr PipelineName_t  InstBench_f16SeqFMA_FP16 {Hash_t{0x7bfd06cu}};  // 'InstBench.f16SeqFMA-FP16'
			static constexpr PipelineName_t  InstBench_f16SeqMulAdd_FP16 {Hash_t{0xa4281e51u}};  // 'InstBench.f16SeqMulAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpACosAdd_FP16 {Hash_t{0xa7d56d6eu}};  // 'InstBench.fpACosAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpACosAdd_2_FP16 {Hash_t{0xd0f7f9d1u}};  // 'InstBench.fpACosAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpACosAdd_3_FP16 {Hash_t{0x1bab2a74u}};  // 'InstBench.fpACosAdd_3-FP16'
			static constexpr PipelineName_t  InstBench_fpACosH_FP16 {Hash_t{0x73b644fdu}};  // 'InstBench.fpACosH-FP16'
			static constexpr PipelineName_t  InstBench_fpASinAdd_FP16 {Hash_t{0x59b27cc4u}};  // 'InstBench.fpASinAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpASinAdd_2_FP16 {Hash_t{0xeeaf7327u}};  // 'InstBench.fpASinAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpASinAdd_3_FP16 {Hash_t{0x25f3a082u}};  // 'InstBench.fpASinAdd_3-FP16'
			static constexpr PipelineName_t  InstBench_fpASinH_FP16 {Hash_t{0x1f10f205u}};  // 'InstBench.fpASinH-FP16'
			static constexpr PipelineName_t  InstBench_fpATanAdd_FP16 {Hash_t{0x635e29b3u}};  // 'InstBench.fpATanAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpATanAdd_2_FP16 {Hash_t{0x95d00381u}};  // 'InstBench.fpATanAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpATanAdd_3_FP16 {Hash_t{0x5e8cd024u}};  // 'InstBench.fpATanAdd_3-FP16'
			static constexpr PipelineName_t  InstBench_fpATanH_FP16 {Hash_t{0xa8948479u}};  // 'InstBench.fpATanH-FP16'
			static constexpr PipelineName_t  InstBench_fpAbsAdd_FP16 {Hash_t{0x4ecabef9u}};  // 'InstBench.fpAbsAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpAdd_FP16 {Hash_t{0x181a0058u}};  // 'InstBench.fpAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpCbrtAdd_2_FP16 {Hash_t{0xe4fd569du}};  // 'InstBench.fpCbrtAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpCbrtAdd_3_FP16 {Hash_t{0x2fa18538u}};  // 'InstBench.fpCbrtAdd_3-FP16'
			static constexpr PipelineName_t  InstBench_fpCbrtExpAdd_FP16 {Hash_t{0x7322b4ddu}};  // 'InstBench.fpCbrtExpAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpCbrtPowAdd_FP16 {Hash_t{0xd902b0cu}};  // 'InstBench.fpCbrtPowAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpCeilAdd_FP16 {Hash_t{0x5ba04b82u}};  // 'InstBench.fpCeilAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpClampAdd_FP16 {Hash_t{0xebb21271u}};  // 'InstBench.fpClampAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpClampSNormAdd_FP16 {Hash_t{0xacd8c94eu}};  // 'InstBench.fpClampSNormAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpClampUNormAdd_FP16 {Hash_t{0xc3d1693u}};  // 'InstBench.fpClampUNormAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpCosAdd_FP16 {Hash_t{0xe3588c88u}};  // 'InstBench.fpCosAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpCosAdd_2_FP16 {Hash_t{0xfde3bd8u}};  // 'InstBench.fpCosAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpCosH_FP16 {Hash_t{0x6c08c864u}};  // 'InstBench.fpCosH-FP16'
			static constexpr PipelineName_t  InstBench_fpCross3_FP16 {Hash_t{0xfe6f9bau}};  // 'InstBench.fpCross3-FP16'
			static constexpr PipelineName_t  InstBench_fpDistanceAdd_FP16 {Hash_t{0x115e0026u}};  // 'InstBench.fpDistanceAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpDivAdd_FP16 {Hash_t{0x8deaac03u}};  // 'InstBench.fpDivAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpDot_FP16 {Hash_t{0x512b585du}};  // 'InstBench.fpDot-FP16'
			static constexpr PipelineName_t  InstBench_fpDualAdd_FP16 {Hash_t{0x92192632u}};  // 'InstBench.fpDualAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpDualMulAdd_FP16 {Hash_t{0xed4ca7edu}};  // 'InstBench.fpDualMulAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpExp2Add_FP16 {Hash_t{0xb0d45768u}};  // 'InstBench.fpExp2Add-FP16'
			static constexpr PipelineName_t  InstBench_fpExpAdd_FP16 {Hash_t{0xf712e09fu}};  // 'InstBench.fpExpAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpFMA_FP16 {Hash_t{0x1f9be18bu}};  // 'InstBench.fpFMA-FP16'
			static constexpr PipelineName_t  InstBench_fpFloorAdd_FP16 {Hash_t{0x8ee872dfu}};  // 'InstBench.fpFloorAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpFractAdd_FP16 {Hash_t{0x5091fe3fu}};  // 'InstBench.fpFractAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpInvSqrtAdd_FP16 {Hash_t{0x40b5f95cu}};  // 'InstBench.fpInvSqrtAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpLengthAdd_FP16 {Hash_t{0x58d72a9cu}};  // 'InstBench.fpLengthAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpLerpClamp_FP16 {Hash_t{0xf562f4ecu}};  // 'InstBench.fpLerpClamp-FP16'
			static constexpr PipelineName_t  InstBench_fpLog2Add_FP16 {Hash_t{0x6fc22620u}};  // 'InstBench.fpLog2Add-FP16'
			static constexpr PipelineName_t  InstBench_fpLogAdd_FP16 {Hash_t{0x818c6b33u}};  // 'InstBench.fpLogAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpMinMaxAdd_FP16 {Hash_t{0x5616ad9fu}};  // 'InstBench.fpMinMaxAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpModAdd_FP16 {Hash_t{0x798f8836u}};  // 'InstBench.fpModAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpMul_FP16 {Hash_t{0x624f0000u}};  // 'InstBench.fpMul-FP16'
			static constexpr PipelineName_t  InstBench_fpMulAdd_FP16 {Hash_t{0xf2db3b0fu}};  // 'InstBench.fpMulAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpNormalizeAdd_FP16 {Hash_t{0xdc2f8e18u}};  // 'InstBench.fpNormalizeAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpPow17Add_FP16 {Hash_t{0xca790c15u}};  // 'InstBench.fpPow17Add-FP16'
			static constexpr PipelineName_t  InstBench_fpPow1ov17Add_FP16 {Hash_t{0xa4fdc27bu}};  // 'InstBench.fpPow1ov17Add-FP16'
			static constexpr PipelineName_t  InstBench_fpPow1ov33Add_FP16 {Hash_t{0xfde4434au}};  // 'InstBench.fpPow1ov33Add-FP16'
			static constexpr PipelineName_t  InstBench_fpPow1ov7Add_FP16 {Hash_t{0xe421cb6du}};  // 'InstBench.fpPow1ov7Add-FP16'
			static constexpr PipelineName_t  InstBench_fpPow33Add_FP16 {Hash_t{0x93608d24u}};  // 'InstBench.fpPow33Add-FP16'
			static constexpr PipelineName_t  InstBench_fpPow8Add_FP16 {Hash_t{0xd4508d2bu}};  // 'InstBench.fpPow8Add-FP16'
			static constexpr PipelineName_t  InstBench_fpQdrtInvsqrtAdd_FP16 {Hash_t{0x5e35c4cau}};  // 'InstBench.fpQdrtInvsqrtAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpQdrtPowAdd_FP16 {Hash_t{0x19b64e64u}};  // 'InstBench.fpQdrtPowAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpQdrtSqrtAdd_FP16 {Hash_t{0x5aea54a6u}};  // 'InstBench.fpQdrtSqrtAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpRoundAdd_FP16 {Hash_t{0xf189a0e9u}};  // 'InstBench.fpRoundAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpRoundEvenAdd_FP16 {Hash_t{0x484f733au}};  // 'InstBench.fpRoundEvenAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve1Add_FP16 {Hash_t{0x33ee75a2u}};  // 'InstBench.fpSRGBCurve1Add-FP16'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve2Add_FP16 {Hash_t{0xa634967u}};  // 'InstBench.fpSRGBCurve2Add-FP16'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve3Add_FP16 {Hash_t{0x1d185d24u}};  // 'InstBench.fpSRGBCurve3Add-FP16'
			static constexpr PipelineName_t  InstBench_fpScalarAdd_FP16 {Hash_t{0x61c8e7adu}};  // 'InstBench.fpScalarAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpScalarMulAdd_FP16 {Hash_t{0x38365c9eu}};  // 'InstBench.fpScalarMulAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpSignAdd_FP16 {Hash_t{0x6d9d2c8cu}};  // 'InstBench.fpSignAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpSignAdd_2_FP16 {Hash_t{0xbd64d3f1u}};  // 'InstBench.fpSignAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpSinAdd_FP16 {Hash_t{0x1d3f9d22u}};  // 'InstBench.fpSinAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpSinAdd_2_FP16 {Hash_t{0x3186b12eu}};  // 'InstBench.fpSinAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpSinH_FP16 {Hash_t{0xae7e9cu}};  // 'InstBench.fpSinH-FP16'
			static constexpr PipelineName_t  InstBench_fpSmoothstepAdd_FP16 {Hash_t{0xeb698f6au}};  // 'InstBench.fpSmoothstepAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_FP16 {Hash_t{0x81c18b41u}};  // 'InstBench.fpSqrtAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_2_FP16 {Hash_t{0x8b82a0e5u}};  // 'InstBench.fpSqrtAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_3_FP16 {Hash_t{0x40de7340u}};  // 'InstBench.fpSqrtAdd_3-FP16'
			static constexpr PipelineName_t  InstBench_fpStepAdd_FP16 {Hash_t{0x6808ec9eu}};  // 'InstBench.fpStepAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpTanAdd_FP16 {Hash_t{0x27d3c855u}};  // 'InstBench.fpTanAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpTanAdd_2_FP16 {Hash_t{0x4af9c188u}};  // 'InstBench.fpTanAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpTanH_FP16 {Hash_t{0xb72a08e0u}};  // 'InstBench.fpTanH-FP16'
			static constexpr PipelineName_t  InstBench_fpTruncAdd_FP16 {Hash_t{0xa7bafeefu}};  // 'InstBench.fpTruncAdd-FP16'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_High_ShaderFloat16;

	static constexpr struct _InstBenchRT_High_ShaderFloat64
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xc6de5956u}};}  // 'InstBenchRT.High.ShaderFloat64'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_fpAbsAdd_FP64 {Hash_t{0xef854912u}};  // 'InstBench.fpAbsAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpAdd_FP64 {Hash_t{0xb955f7b3u}};  // 'InstBench.fpAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpCeilAdd_FP64 {Hash_t{0xfaefbc69u}};  // 'InstBench.fpCeilAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpClampAdd_FP64 {Hash_t{0x4afde59au}};  // 'InstBench.fpClampAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpClampSNormAdd_FP64 {Hash_t{0xd973ea5u}};  // 'InstBench.fpClampSNormAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpClampUNormAdd_FP64 {Hash_t{0xad72e178u}};  // 'InstBench.fpClampUNormAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpCross3_FP64 {Hash_t{0xaea90e51u}};  // 'InstBench.fpCross3-FP64'
			static constexpr PipelineName_t  InstBench_fpDistanceAdd_FP64 {Hash_t{0xb011f7cdu}};  // 'InstBench.fpDistanceAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpDivAdd_FP64 {Hash_t{0x2ca55be8u}};  // 'InstBench.fpDivAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpDot_FP64 {Hash_t{0xf064afb6u}};  // 'InstBench.fpDot-FP64'
			static constexpr PipelineName_t  InstBench_fpDualAdd_FP64 {Hash_t{0x3356d1d9u}};  // 'InstBench.fpDualAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpDualMulAdd_FP64 {Hash_t{0x4c035006u}};  // 'InstBench.fpDualMulAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpFMA_FP64 {Hash_t{0xbed41660u}};  // 'InstBench.fpFMA-FP64'
			static constexpr PipelineName_t  InstBench_fpFloorAdd_FP64 {Hash_t{0x2fa78534u}};  // 'InstBench.fpFloorAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpFractAdd_FP64 {Hash_t{0xf1de09d4u}};  // 'InstBench.fpFractAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpInvSqrtAdd_FP64 {Hash_t{0xe1fa0eb7u}};  // 'InstBench.fpInvSqrtAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpLengthAdd_FP64 {Hash_t{0xf998dd77u}};  // 'InstBench.fpLengthAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpLerpClamp_FP64 {Hash_t{0x542d0307u}};  // 'InstBench.fpLerpClamp-FP64'
			static constexpr PipelineName_t  InstBench_fpMinMaxAdd_FP64 {Hash_t{0xf7595a74u}};  // 'InstBench.fpMinMaxAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpModAdd_FP64 {Hash_t{0xd8c07fddu}};  // 'InstBench.fpModAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpMul_FP64 {Hash_t{0xc300f7ebu}};  // 'InstBench.fpMul-FP64'
			static constexpr PipelineName_t  InstBench_fpMulAdd_FP64 {Hash_t{0x5394cce4u}};  // 'InstBench.fpMulAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpNormalizeAdd_FP64 {Hash_t{0x7d6079f3u}};  // 'InstBench.fpNormalizeAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpRoundAdd_FP64 {Hash_t{0x50c65702u}};  // 'InstBench.fpRoundAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpRoundEvenAdd_FP64 {Hash_t{0xe90084d1u}};  // 'InstBench.fpRoundEvenAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpScalarAdd_FP64 {Hash_t{0xc0871046u}};  // 'InstBench.fpScalarAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpScalarMulAdd_FP64 {Hash_t{0x9979ab75u}};  // 'InstBench.fpScalarMulAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpSignAdd_FP64 {Hash_t{0xccd2db67u}};  // 'InstBench.fpSignAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpSignAdd_2_FP64 {Hash_t{0x1c2b241au}};  // 'InstBench.fpSignAdd_2-FP64'
			static constexpr PipelineName_t  InstBench_fpSmoothstepAdd_FP64 {Hash_t{0x4a267881u}};  // 'InstBench.fpSmoothstepAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_FP64 {Hash_t{0x208e7caau}};  // 'InstBench.fpSqrtAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpStepAdd_FP64 {Hash_t{0xc9471b75u}};  // 'InstBench.fpStepAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpTruncAdd_FP64 {Hash_t{0x6f50904u}};  // 'InstBench.fpTruncAdd-FP64'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_High_ShaderFloat64;

	static constexpr struct _InstBenchRT_High_ShaderFloatInt16
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x184f5867u}};}  // 'InstBenchRT.High.ShaderFloatInt16'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_fiBitCast_Add_FP16 {Hash_t{0x30e06cc4u}};  // 'InstBench.fiBitCast_Add-FP16'
			static constexpr PipelineName_t  InstBench_fiFloatToInt_Add_FP16 {Hash_t{0x25832620u}};  // 'InstBench.fiFloatToInt_Add-FP16'
			static constexpr PipelineName_t  InstBench_fiIntToFloat_Add_FP16 {Hash_t{0x8e26e99fu}};  // 'InstBench.fiIntToFloat_Add-FP16'
			static constexpr PipelineName_t  InstBench_fiParAdd11_FP16 {Hash_t{0xbb557b28u}};  // 'InstBench.fiParAdd11-FP16'
			static constexpr PipelineName_t  InstBench_fiParAdd21_FP16 {Hash_t{0x8abd61b5u}};  // 'InstBench.fiParAdd21-FP16'
			static constexpr PipelineName_t  InstBench_fiParFMulIAdd_FP16 {Hash_t{0x652ee5c6u}};  // 'InstBench.fiParFMulIAdd-FP16'
			static constexpr PipelineName_t  InstBench_fiParMul_FP16 {Hash_t{0x361add87u}};  // 'InstBench.fiParMul-FP16'
			static constexpr PipelineName_t  InstBench_fiSeqAdd_BitCast_FP16 {Hash_t{0x134b665au}};  // 'InstBench.fiSeqAdd_BitCast-FP16'
			static constexpr PipelineName_t  InstBench_fiSeqFMulIAdd_BitCast_FP16 {Hash_t{0x7e149935u}};  // 'InstBench.fiSeqFMulIAdd_BitCast-FP16'
			static constexpr PipelineName_t  InstBench_fiSeqMul_BitCast_FP16 {Hash_t{0x7aa9ef03u}};  // 'InstBench.fiSeqMul_BitCast-FP16'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_High_ShaderFloatInt16;

	static constexpr struct _InstBenchRT_High_ShaderFloatInt64
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xb900af8cu}};}  // 'InstBenchRT.High.ShaderFloatInt64'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_fiBitCast_Add_FP64 {Hash_t{0x91af9b2fu}};  // 'InstBench.fiBitCast_Add-FP64'
			static constexpr PipelineName_t  InstBench_fiFloatToInt_Add_FP64 {Hash_t{0x84ccd1cbu}};  // 'InstBench.fiFloatToInt_Add-FP64'
			static constexpr PipelineName_t  InstBench_fiIntToFloat_Add_FP64 {Hash_t{0x2f691e74u}};  // 'InstBench.fiIntToFloat_Add-FP64'
			static constexpr PipelineName_t  InstBench_fiParAdd11_FP64 {Hash_t{0x1a1a8cc3u}};  // 'InstBench.fiParAdd11-FP64'
			static constexpr PipelineName_t  InstBench_fiParAdd21_FP64 {Hash_t{0x2bf2965eu}};  // 'InstBench.fiParAdd21-FP64'
			static constexpr PipelineName_t  InstBench_fiParFMulIAdd_FP64 {Hash_t{0xc461122du}};  // 'InstBench.fiParFMulIAdd-FP64'
			static constexpr PipelineName_t  InstBench_fiParMul_FP64 {Hash_t{0x97552a6cu}};  // 'InstBench.fiParMul-FP64'
			static constexpr PipelineName_t  InstBench_fiSeqAdd_BitCast_FP64 {Hash_t{0xb20491b1u}};  // 'InstBench.fiSeqAdd_BitCast-FP64'
			static constexpr PipelineName_t  InstBench_fiSeqFMulIAdd_BitCast_FP64 {Hash_t{0xdf5b6edeu}};  // 'InstBench.fiSeqFMulIAdd_BitCast-FP64'
			static constexpr PipelineName_t  InstBench_fiSeqMul_BitCast_FP64 {Hash_t{0xdbe618e8u}};  // 'InstBench.fiSeqMul_BitCast-FP64'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_High_ShaderFloatInt64;

	static constexpr struct _InstBenchRT_High_ShaderInt16
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xf2c1ff5cu}};}  // 'InstBenchRT.High.ShaderInt16'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_iAbsAdd_INT16 {Hash_t{0x62d9cdbu}};  // 'InstBench.iAbsAdd-INT16'
			static constexpr PipelineName_t  InstBench_iAdd_INT16 {Hash_t{0xe795e96cu}};  // 'InstBench.iAdd-INT16'
			static constexpr PipelineName_t  InstBench_iAdd_UINT16 {Hash_t{0xa196f489u}};  // 'InstBench.iAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iAndAdd_INT16 {Hash_t{0xc945948du}};  // 'InstBench.iAndAdd-INT16'
			static constexpr PipelineName_t  InstBench_iAndAdd_UINT16 {Hash_t{0x76b5f61au}};  // 'InstBench.iAndAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iClampAdd_INT16 {Hash_t{0xb4e2058bu}};  // 'InstBench.iClampAdd-INT16'
			static constexpr PipelineName_t  InstBench_iClampAdd_UINT16 {Hash_t{0x9fabf4beu}};  // 'InstBench.iClampAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_INT16 {Hash_t{0x48a22c8eu}};  // 'InstBench.iClampConstAdd-INT16'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_UINT16 {Hash_t{0xef3d4018u}};  // 'InstBench.iClampConstAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iDivAdd_INT16 {Hash_t{0x5b86a7cbu}};  // 'InstBench.iDivAdd-INT16'
			static constexpr PipelineName_t  InstBench_iDivAdd_UINT16 {Hash_t{0xe998d18cu}};  // 'InstBench.iDivAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iFindLSB_INT16 {Hash_t{0x182b40a2u}};  // 'InstBench.iFindLSB-INT16'
			static constexpr PipelineName_t  InstBench_iFindLSB_UINT16 {Hash_t{0xddb5a597u}};  // 'InstBench.iFindLSB-UINT16'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_INT16 {Hash_t{0xcc204678u}};  // 'InstBench.iMinMaxAdd-INT16'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_UINT16 {Hash_t{0xbb67955bu}};  // 'InstBench.iMinMaxAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iModAdd_INT16 {Hash_t{0xdc106ccu}};  // 'InstBench.iModAdd-INT16'
			static constexpr PipelineName_t  InstBench_iModAdd_UINT16 {Hash_t{0x77aa038eu}};  // 'InstBench.iModAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iMul_INT16 {Hash_t{0x825f65aau}};  // 'InstBench.iMul-INT16'
			static constexpr PipelineName_t  InstBench_iMul_UINT16 {Hash_t{0xd3f45980u}};  // 'InstBench.iMul-UINT16'
			static constexpr PipelineName_t  InstBench_iMulAdd_INT16 {Hash_t{0x524fda77u}};  // 'InstBench.iMulAdd-INT16'
			static constexpr PipelineName_t  InstBench_iMulAdd_UINT16 {Hash_t{0x2b46e756u}};  // 'InstBench.iMulAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iOrAdd_INT16 {Hash_t{0x1ea704aau}};  // 'InstBench.iOrAdd-INT16'
			static constexpr PipelineName_t  InstBench_iOrAdd_UINT16 {Hash_t{0xd368a1e1u}};  // 'InstBench.iOrAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iShift_INT16 {Hash_t{0x9615f88fu}};  // 'InstBench.iShift-INT16'
			static constexpr PipelineName_t  InstBench_iShift_UINT16 {Hash_t{0x98e4c75au}};  // 'InstBench.iShift-UINT16'
			static constexpr PipelineName_t  InstBench_iShiftConst_INT16 {Hash_t{0x894b2ca3u}};  // 'InstBench.iShiftConst-INT16'
			static constexpr PipelineName_t  InstBench_iShiftConst_UINT16 {Hash_t{0xaa23f56du}};  // 'InstBench.iShiftConst-UINT16'
			static constexpr PipelineName_t  InstBench_iSignAdd_INT16 {Hash_t{0xf464ab2u}};  // 'InstBench.iSignAdd-INT16'
			static constexpr PipelineName_t  InstBench_iSignAdd_2_INT16 {Hash_t{0x66c17859u}};  // 'InstBench.iSignAdd_2-INT16'
			static constexpr PipelineName_t  InstBench_iXorAdd_INT16 {Hash_t{0xb3ca6d8fu}};  // 'InstBench.iXorAdd-INT16'
			static constexpr PipelineName_t  InstBench_iXorAdd_UINT16 {Hash_t{0x98c118cfu}};  // 'InstBench.iXorAdd-UINT16'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_High_ShaderInt16;

	static constexpr struct _InstBenchRT_High_ShaderInt64
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x538e08b7u}};}  // 'InstBenchRT.High.ShaderInt64'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_iAbsAdd_INT64 {Hash_t{0xa7626b30u}};  // 'InstBench.iAbsAdd-INT64'
			static constexpr PipelineName_t  InstBench_iAdd_INT64 {Hash_t{0x46da1e87u}};  // 'InstBench.iAdd-INT64'
			static constexpr PipelineName_t  InstBench_iAdd_UINT64 {Hash_t{0xd90362u}};  // 'InstBench.iAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iAndAdd_INT64 {Hash_t{0x680a6366u}};  // 'InstBench.iAndAdd-INT64'
			static constexpr PipelineName_t  InstBench_iAndAdd_UINT64 {Hash_t{0xd7fa01f1u}};  // 'InstBench.iAndAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iClampAdd_INT64 {Hash_t{0x15adf260u}};  // 'InstBench.iClampAdd-INT64'
			static constexpr PipelineName_t  InstBench_iClampAdd_UINT64 {Hash_t{0x3ee40355u}};  // 'InstBench.iClampAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_INT64 {Hash_t{0xe9eddb65u}};  // 'InstBench.iClampConstAdd-INT64'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_UINT64 {Hash_t{0x4e72b7f3u}};  // 'InstBench.iClampConstAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iDivAdd_INT64 {Hash_t{0xfac95020u}};  // 'InstBench.iDivAdd-INT64'
			static constexpr PipelineName_t  InstBench_iDivAdd_UINT64 {Hash_t{0x48d72667u}};  // 'InstBench.iDivAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iFindLSB_INT64 {Hash_t{0xb964b749u}};  // 'InstBench.iFindLSB-INT64'
			static constexpr PipelineName_t  InstBench_iFindLSB_UINT64 {Hash_t{0x7cfa527cu}};  // 'InstBench.iFindLSB-UINT64'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_INT64 {Hash_t{0x6d6fb193u}};  // 'InstBench.iMinMaxAdd-INT64'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_UINT64 {Hash_t{0x1a2862b0u}};  // 'InstBench.iMinMaxAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iModAdd_INT64 {Hash_t{0xac8ef127u}};  // 'InstBench.iModAdd-INT64'
			static constexpr PipelineName_t  InstBench_iModAdd_UINT64 {Hash_t{0xd6e5f465u}};  // 'InstBench.iModAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iMul_INT64 {Hash_t{0x23109241u}};  // 'InstBench.iMul-INT64'
			static constexpr PipelineName_t  InstBench_iMul_UINT64 {Hash_t{0x72bbae6bu}};  // 'InstBench.iMul-UINT64'
			static constexpr PipelineName_t  InstBench_iMulAdd_INT64 {Hash_t{0xf3002d9cu}};  // 'InstBench.iMulAdd-INT64'
			static constexpr PipelineName_t  InstBench_iMulAdd_UINT64 {Hash_t{0x8a0910bdu}};  // 'InstBench.iMulAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iOrAdd_INT64 {Hash_t{0xbfe8f341u}};  // 'InstBench.iOrAdd-INT64'
			static constexpr PipelineName_t  InstBench_iOrAdd_UINT64 {Hash_t{0x7227560au}};  // 'InstBench.iOrAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iShift_INT64 {Hash_t{0x375a0f64u}};  // 'InstBench.iShift-INT64'
			static constexpr PipelineName_t  InstBench_iShift_UINT64 {Hash_t{0x39ab30b1u}};  // 'InstBench.iShift-UINT64'
			static constexpr PipelineName_t  InstBench_iShiftConst_INT64 {Hash_t{0x2804db48u}};  // 'InstBench.iShiftConst-INT64'
			static constexpr PipelineName_t  InstBench_iShiftConst_UINT64 {Hash_t{0xb6c0286u}};  // 'InstBench.iShiftConst-UINT64'
			static constexpr PipelineName_t  InstBench_iSignAdd_INT64 {Hash_t{0xae09bd59u}};  // 'InstBench.iSignAdd-INT64'
			static constexpr PipelineName_t  InstBench_iSignAdd_2_INT64 {Hash_t{0xc78e8fb2u}};  // 'InstBench.iSignAdd_2-INT64'
			static constexpr PipelineName_t  InstBench_iXorAdd_INT64 {Hash_t{0x12859a64u}};  // 'InstBench.iXorAdd-INT64'
			static constexpr PipelineName_t  InstBench_iXorAdd_UINT64 {Hash_t{0x398eef24u}};  // 'InstBench.iXorAdd-UINT64'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_High_ShaderInt64;

	static constexpr struct _InstBenchRT_High_ShaderInt8
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xd549dae7u}};}  // 'InstBenchRT.High.ShaderInt8'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_iAbsAdd_INT8 {Hash_t{0xed9f0cf1u}};  // 'InstBench.iAbsAdd-INT8'
			static constexpr PipelineName_t  InstBench_iAdd_INT8 {Hash_t{0x5b72a3bau}};  // 'InstBench.iAdd-INT8'
			static constexpr PipelineName_t  InstBench_iAdd_UINT8 {Hash_t{0x31d13f15u}};  // 'InstBench.iAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iAndAdd_INT8 {Hash_t{0x3f02c3c7u}};  // 'InstBench.iAndAdd-INT8'
			static constexpr PipelineName_t  InstBench_iAndAdd_UINT8 {Hash_t{0x1f0142f4u}};  // 'InstBench.iAndAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iClampAdd_INT8 {Hash_t{0x9f0d7448u}};  // 'InstBench.iClampAdd-INT8'
			static constexpr PipelineName_t  InstBench_iClampAdd_UINT8 {Hash_t{0x62a6d3f2u}};  // 'InstBench.iClampAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_INT8 {Hash_t{0x66ac0d13u}};  // 'InstBench.iClampConstAdd-INT8'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_UINT8 {Hash_t{0x9ee6faf7u}};  // 'InstBench.iClampConstAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iDivAdd_INT8 {Hash_t{0x2ebf1e0bu}};  // 'InstBench.iDivAdd-INT8'
			static constexpr PipelineName_t  InstBench_iDivAdd_UINT8 {Hash_t{0x8dc271b2u}};  // 'InstBench.iDivAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iFindLSB_INT8 {Hash_t{0xeab11d63u}};  // 'InstBench.iFindLSB-INT8'
			static constexpr PipelineName_t  InstBench_iFindLSB_UINT8 {Hash_t{0xce6f96dbu}};  // 'InstBench.iFindLSB-UINT8'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_INT8 {Hash_t{0x5a872500u}};  // 'InstBench.iMinMaxAdd-INT8'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_UINT8 {Hash_t{0x1a649001u}};  // 'InstBench.iMinMaxAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iModAdd_INT8 {Hash_t{0xdada3a3eu}};  // 'InstBench.iModAdd-INT8'
			static constexpr PipelineName_t  InstBench_iModAdd_UINT8 {Hash_t{0xdb85d0b5u}};  // 'InstBench.iModAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iMul_INT8 {Hash_t{0x2127a3e2u}};  // 'InstBench.iMul-INT8'
			static constexpr PipelineName_t  InstBench_iMul_UINT8 {Hash_t{0x541bb3d3u}};  // 'InstBench.iMul-UINT8'
			static constexpr PipelineName_t  InstBench_iMulAdd_INT8 {Hash_t{0x518e8907u}};  // 'InstBench.iMulAdd-INT8'
			static constexpr PipelineName_t  InstBench_iMulAdd_UINT8 {Hash_t{0x840b0c0eu}};  // 'InstBench.iMulAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iOrAdd_INT8 {Hash_t{0xd0400a26u}};  // 'InstBench.iOrAdd-INT8'
			static constexpr PipelineName_t  InstBench_iOrAdd_UINT8 {Hash_t{0xc8e3d2d3u}};  // 'InstBench.iOrAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iShift_INT8 {Hash_t{0x6ae6c9feu}};  // 'InstBench.iShift-INT8'
			static constexpr PipelineName_t  InstBench_iShift_UINT8 {Hash_t{0x40512ef6u}};  // 'InstBench.iShift-UINT8'
			static constexpr PipelineName_t  InstBench_iShiftConst_INT8 {Hash_t{0xeeb18b2du}};  // 'InstBench.iShiftConst-INT8'
			static constexpr PipelineName_t  InstBench_iShiftConst_UINT8 {Hash_t{0x5f0ffadau}};  // 'InstBench.iShiftConst-UINT8'
			static constexpr PipelineName_t  InstBench_iSignAdd_INT8 {Hash_t{0x30054efdu}};  // 'InstBench.iSignAdd-INT8'
			static constexpr PipelineName_t  InstBench_iSignAdd_2_INT8 {Hash_t{0xb1f55b6eu}};  // 'InstBench.iSignAdd_2-INT8'
			static constexpr PipelineName_t  InstBench_iXorAdd_INT8 {Hash_t{0xdaa1694cu}};  // 'InstBench.iXorAdd-INT8'
			static constexpr PipelineName_t  InstBench_iXorAdd_UINT8 {Hash_t{0x658ebbf6u}};  // 'InstBench.iXorAdd-UINT8'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_High_ShaderInt8;

	static constexpr struct _InstBenchRT_Low
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x5ea470c2u}};}  // 'InstBenchRT.Low'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_fiBitCast_Add_FP32 {Hash_t{0x5bbca5fu}};  // 'InstBench.fiBitCast_Add-FP32'
			static constexpr PipelineName_t  InstBench_fiFloatToInt_Add_FP32 {Hash_t{0x10d880bbu}};  // 'InstBench.fiFloatToInt_Add-FP32'
			static constexpr PipelineName_t  InstBench_fiIntToFloat_Add_FP32 {Hash_t{0xbb7d4f04u}};  // 'InstBench.fiIntToFloat_Add-FP32'
			static constexpr PipelineName_t  InstBench_fiParAdd11_FP32 {Hash_t{0x8e0eddb3u}};  // 'InstBench.fiParAdd11-FP32'
			static constexpr PipelineName_t  InstBench_fiParAdd21_FP32 {Hash_t{0xbfe6c72eu}};  // 'InstBench.fiParAdd21-FP32'
			static constexpr PipelineName_t  InstBench_fiParFMulIAdd_FP32 {Hash_t{0x5075435du}};  // 'InstBench.fiParFMulIAdd-FP32'
			static constexpr PipelineName_t  InstBench_fiParMul_FP32 {Hash_t{0x3417b1cu}};  // 'InstBench.fiParMul-FP32'
			static constexpr PipelineName_t  InstBench_fiSeqAdd_BitCast_FP32 {Hash_t{0x2610c0c1u}};  // 'InstBench.fiSeqAdd_BitCast-FP32'
			static constexpr PipelineName_t  InstBench_fiSeqFMulIAdd_BitCast_FP32 {Hash_t{0x4b4f3faeu}};  // 'InstBench.fiSeqFMulIAdd_BitCast-FP32'
			static constexpr PipelineName_t  InstBench_fiSeqMul_BitCast_FP32 {Hash_t{0x4ff24998u}};  // 'InstBench.fiSeqMul_BitCast-FP32'
			static constexpr PipelineName_t  InstBench_fpACosAdd_FP32 {Hash_t{0x928ecbf5u}};  // 'InstBench.fpACosAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpACosAdd_FP32_MED {Hash_t{0xa194382bu}};  // 'InstBench.fpACosAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpACosAdd_2_FP32 {Hash_t{0xe5ac5f4au}};  // 'InstBench.fpACosAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpACosAdd_2_FP32_MED {Hash_t{0x774cd474u}};  // 'InstBench.fpACosAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpACosAdd_3_FP32 {Hash_t{0x2ef08cefu}};  // 'InstBench.fpACosAdd_3-FP32'
			static constexpr PipelineName_t  InstBench_fpACosAdd_3_FP32_MED {Hash_t{0x988ebf4au}};  // 'InstBench.fpACosAdd_3-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpACosH_FP32 {Hash_t{0x46ede266u}};  // 'InstBench.fpACosH-FP32'
			static constexpr PipelineName_t  InstBench_fpACosH_FP32_MED {Hash_t{0xf69325a7u}};  // 'InstBench.fpACosH-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpASinAdd_FP32 {Hash_t{0x6ce9da5fu}};  // 'InstBench.fpASinAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpASinAdd_FP32_MED {Hash_t{0xee1f2a76u}};  // 'InstBench.fpASinAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpASinAdd_2_FP32 {Hash_t{0xdbf4d5bcu}};  // 'InstBench.fpASinAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpASinAdd_2_FP32_MED {Hash_t{0x8be0c9aeu}};  // 'InstBench.fpASinAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpASinAdd_3_FP32 {Hash_t{0x10a80619u}};  // 'InstBench.fpASinAdd_3-FP32'
			static constexpr PipelineName_t  InstBench_fpASinAdd_3_FP32_MED {Hash_t{0x6422a290u}};  // 'InstBench.fpASinAdd_3-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpASinH_FP32 {Hash_t{0x2a4b549eu}};  // 'InstBench.fpASinH-FP32'
			static constexpr PipelineName_t  InstBench_fpASinH_FP32_MED {Hash_t{0xc8cbaf51u}};  // 'InstBench.fpASinH-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpATanAdd_FP32 {Hash_t{0x56058f28u}};  // 'InstBench.fpATanAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpATanAdd_FP32_MED {Hash_t{0x46473b5au}};  // 'InstBench.fpATanAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpATanAdd_2_FP32 {Hash_t{0xa08ba51au}};  // 'InstBench.fpATanAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpATanAdd_2_FP32_MED {Hash_t{0xd8612aaau}};  // 'InstBench.fpATanAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpATanAdd_3_FP32 {Hash_t{0x6bd776bfu}};  // 'InstBench.fpATanAdd_3-FP32'
			static constexpr PipelineName_t  InstBench_fpATanAdd_3_FP32_MED {Hash_t{0x37a34194u}};  // 'InstBench.fpATanAdd_3-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpATanH_FP32 {Hash_t{0x9dcf22e2u}};  // 'InstBench.fpATanH-FP32'
			static constexpr PipelineName_t  InstBench_fpATanH_FP32_MED {Hash_t{0xb3b4dff7u}};  // 'InstBench.fpATanH-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpAbsAdd_FP32 {Hash_t{0x7b911862u}};  // 'InstBench.fpAbsAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpAbsAdd_FP32_MED {Hash_t{0x1a403e1du}};  // 'InstBench.fpAbsAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpAdd_FP32 {Hash_t{0x2d41a6c3u}};  // 'InstBench.fpAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpAdd_FP32_MED {Hash_t{0x2367a2b9u}};  // 'InstBench.fpAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCbrtAdd_2_FP32 {Hash_t{0xd1a6f006u}};  // 'InstBench.fpCbrtAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpCbrtAdd_2_FP32_MED {Hash_t{0xaff13593u}};  // 'InstBench.fpCbrtAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCbrtAdd_3_FP32 {Hash_t{0x1afa23a3u}};  // 'InstBench.fpCbrtAdd_3-FP32'
			static constexpr PipelineName_t  InstBench_fpCbrtAdd_3_FP32_MED {Hash_t{0x40335eadu}};  // 'InstBench.fpCbrtAdd_3-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCbrtExpAdd_FP32 {Hash_t{0x46791246u}};  // 'InstBench.fpCbrtExpAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpCbrtExpAdd_FP32_MED {Hash_t{0xf648b657u}};  // 'InstBench.fpCbrtExpAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCbrtPowAdd_FP32 {Hash_t{0x38cb8d97u}};  // 'InstBench.fpCbrtPowAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpCbrtPowAdd_FP32_MED {Hash_t{0xffa10767u}};  // 'InstBench.fpCbrtPowAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCeilAdd_FP32 {Hash_t{0x6efbed19u}};  // 'InstBench.fpCeilAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpCeilAdd_FP32_MED {Hash_t{0xe7becf7du}};  // 'InstBench.fpCeilAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpClampAdd_FP32 {Hash_t{0xdee9b4eau}};  // 'InstBench.fpClampAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpClampAdd_FP32_MED {Hash_t{0xa395a054u}};  // 'InstBench.fpClampAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpClampSNormAdd_FP32 {Hash_t{0x99836fd5u}};  // 'InstBench.fpClampSNormAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpClampSNormAdd_FP32_MED {Hash_t{0xfdabc56cu}};  // 'InstBench.fpClampSNormAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpClampUNormAdd_FP32 {Hash_t{0x3966b008u}};  // 'InstBench.fpClampUNormAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpClampUNormAdd_FP32_MED {Hash_t{0x5a9f7264u}};  // 'InstBench.fpClampUNormAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCosAdd_FP32 {Hash_t{0xd6032a13u}};  // 'InstBench.fpCosAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpCosAdd_FP32_MED {Hash_t{0x53cffd7u}};  // 'InstBench.fpCosAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCosAdd_2_FP32 {Hash_t{0x3a859d43u}};  // 'InstBench.fpCosAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpCosAdd_2_FP32_MED {Hash_t{0xca45897eu}};  // 'InstBench.fpCosAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCosH_FP32 {Hash_t{0x59536effu}};  // 'InstBench.fpCosH-FP32'
			static constexpr PipelineName_t  InstBench_fpCosH_FP32_MED {Hash_t{0x29bae7aeu}};  // 'InstBench.fpCosH-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpCross3_FP32 {Hash_t{0x3abd5f21u}};  // 'InstBench.fpCross3-FP32'
			static constexpr PipelineName_t  InstBench_fpCross3_FP32_MED {Hash_t{0xdede4823u}};  // 'InstBench.fpCross3-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpDistanceAdd_FP32 {Hash_t{0x2405a6bdu}};  // 'InstBench.fpDistanceAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpDistanceAdd_FP32_MED {Hash_t{0x454e47b3u}};  // 'InstBench.fpDistanceAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpDivAdd_FP32 {Hash_t{0xb8b10a98u}};  // 'InstBench.fpDivAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpDivAdd_FP32_MED {Hash_t{0x7bd00868u}};  // 'InstBench.fpDivAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpDot_FP32 {Hash_t{0x6470fec6u}};  // 'InstBench.fpDot-FP32'
			static constexpr PipelineName_t  InstBench_fpDot_FP32_MED {Hash_t{0xbf4bd305u}};  // 'InstBench.fpDot-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpDualAdd_FP32 {Hash_t{0xa74280a9u}};  // 'InstBench.fpDualAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpDualAdd_FP32_MED {Hash_t{0x6d7cb671u}};  // 'InstBench.fpDualAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpDualMulAdd_FP32 {Hash_t{0xd8170176u}};  // 'InstBench.fpDualMulAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpDualMulAdd_FP32_MED {Hash_t{0xf4caabb5u}};  // 'InstBench.fpDualMulAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpExp2Add_FP32 {Hash_t{0x858ff1f3u}};  // 'InstBench.fpExp2Add-FP32'
			static constexpr PipelineName_t  InstBench_fpExp2Add_FP32_MED {Hash_t{0x37cbb337u}};  // 'InstBench.fpExp2Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpExpAdd_FP32 {Hash_t{0xc2494604u}};  // 'InstBench.fpExpAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpExpAdd_FP32_MED {Hash_t{0x9973d147u}};  // 'InstBench.fpExpAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpFMA_FP32 {Hash_t{0x2ac04710u}};  // 'InstBench.fpFMA-FP32'
			static constexpr PipelineName_t  InstBench_fpFMA_FP32_MED {Hash_t{0x35a32f0u}};  // 'InstBench.fpFMA-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpFloorAdd_FP32 {Hash_t{0xbbb3d444u}};  // 'InstBench.fpFloorAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpFloorAdd_FP32_MED {Hash_t{0x3b7a254bu}};  // 'InstBench.fpFloorAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpFractAdd_FP32 {Hash_t{0x65ca58a4u}};  // 'InstBench.fpFractAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpFractAdd_FP32_MED {Hash_t{0x9abcbbfbu}};  // 'InstBench.fpFractAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpInvSqrtAdd_FP32 {Hash_t{0x75ee5fc7u}};  // 'InstBench.fpInvSqrtAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpInvSqrtAdd_FP32_MED {Hash_t{0x5a3f0451u}};  // 'InstBench.fpInvSqrtAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpLengthAdd_FP32 {Hash_t{0x6d8c8c07u}};  // 'InstBench.fpLengthAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpLengthAdd_FP32_MED {Hash_t{0xe71af04du}};  // 'InstBench.fpLengthAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpLerpClamp_FP32 {Hash_t{0xc0395277u}};  // 'InstBench.fpLerpClamp-FP32'
			static constexpr PipelineName_t  InstBench_fpLerpClamp_FP32_MED {Hash_t{0x3ae7cd43u}};  // 'InstBench.fpLerpClamp-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpLog2Add_FP32 {Hash_t{0x5a9980bbu}};  // 'InstBench.fpLog2Add-FP32'
			static constexpr PipelineName_t  InstBench_fpLog2Add_FP32_MED {Hash_t{0x36acbb10u}};  // 'InstBench.fpLog2Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpLogAdd_FP32 {Hash_t{0xb4d7cda8u}};  // 'InstBench.fpLogAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpLogAdd_FP32_MED {Hash_t{0x250af006u}};  // 'InstBench.fpLogAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpMinMaxAdd_FP32 {Hash_t{0x634d0b04u}};  // 'InstBench.fpMinMaxAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpMinMaxAdd_FP32_MED {Hash_t{0x248c99aeu}};  // 'InstBench.fpMinMaxAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpModAdd_FP32 {Hash_t{0x4cd42eadu}};  // 'InstBench.fpModAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpModAdd_FP32_MED {Hash_t{0x213b4720u}};  // 'InstBench.fpModAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpMul_FP32 {Hash_t{0x5714a69bu}};  // 'InstBench.fpMul-FP32'
			static constexpr PipelineName_t  InstBench_fpMul_FP32_MED {Hash_t{0x5ac2b4c7u}};  // 'InstBench.fpMul-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpMulAdd_FP32 {Hash_t{0xc7809d94u}};  // 'InstBench.fpMulAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpMulAdd_FP32_MED {Hash_t{0xcfbd72bau}};  // 'InstBench.fpMulAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpNormalizeAdd_FP32 {Hash_t{0xe9742883u}};  // 'InstBench.fpNormalizeAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpNormalizeAdd_FP32_MED {Hash_t{0x6de00811u}};  // 'InstBench.fpNormalizeAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpPow17Add_FP32 {Hash_t{0xff22aa8eu}};  // 'InstBench.fpPow17Add-FP32'
			static constexpr PipelineName_t  InstBench_fpPow17Add_FP32_MED {Hash_t{0x650e50a1u}};  // 'InstBench.fpPow17Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpPow1ov17Add_FP32 {Hash_t{0x91a664e0u}};  // 'InstBench.fpPow1ov17Add-FP32'
			static constexpr PipelineName_t  InstBench_fpPow1ov17Add_FP32_MED {Hash_t{0xd0237acbu}};  // 'InstBench.fpPow1ov17Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpPow1ov33Add_FP32 {Hash_t{0xc8bfe5d1u}};  // 'InstBench.fpPow1ov33Add-FP32'
			static constexpr PipelineName_t  InstBench_fpPow1ov33Add_FP32_MED {Hash_t{0xf1f4d4f0u}};  // 'InstBench.fpPow1ov33Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpPow1ov7Add_FP32 {Hash_t{0xd17a6df6u}};  // 'InstBench.fpPow1ov7Add-FP32'
			static constexpr PipelineName_t  InstBench_fpPow1ov7Add_FP32_MED {Hash_t{0xf1d18284u}};  // 'InstBench.fpPow1ov7Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpPow33Add_FP32 {Hash_t{0xa63b2bbfu}};  // 'InstBench.fpPow33Add-FP32'
			static constexpr PipelineName_t  InstBench_fpPow33Add_FP32_MED {Hash_t{0x44d9fe9au}};  // 'InstBench.fpPow33Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpPow8Add_FP32 {Hash_t{0xe10b2bb0u}};  // 'InstBench.fpPow8Add-FP32'
			static constexpr PipelineName_t  InstBench_fpPow8Add_FP32_MED {Hash_t{0x2b440c0cu}};  // 'InstBench.fpPow8Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpQdrtInvsqrtAdd_FP32 {Hash_t{0x6b6e6251u}};  // 'InstBench.fpQdrtInvsqrtAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpQdrtInvsqrtAdd_FP32_MED {Hash_t{0x2f2b69c2u}};  // 'InstBench.fpQdrtInvsqrtAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpQdrtPowAdd_FP32 {Hash_t{0x2cede8ffu}};  // 'InstBench.fpQdrtPowAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpQdrtPowAdd_FP32_MED {Hash_t{0x962b8619u}};  // 'InstBench.fpQdrtPowAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpQdrtSqrtAdd_FP32 {Hash_t{0x6fb1f23du}};  // 'InstBench.fpQdrtSqrtAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpQdrtSqrtAdd_FP32_MED {Hash_t{0xa2058440u}};  // 'InstBench.fpQdrtSqrtAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpRoundAdd_FP32 {Hash_t{0xc4d20672u}};  // 'InstBench.fpRoundAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpRoundAdd_FP32_MED {Hash_t{0xdc6b5523u}};  // 'InstBench.fpRoundAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpRoundEvenAdd_FP32 {Hash_t{0x7d14d5a1u}};  // 'InstBench.fpRoundEvenAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpRoundEvenAdd_FP32_MED {Hash_t{0x4858b3ddu}};  // 'InstBench.fpRoundEvenAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve1Add_FP32 {Hash_t{0x6b5d339u}};  // 'InstBench.fpSRGBCurve1Add-FP32'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve1Add_FP32_MED {Hash_t{0x44ea2338u}};  // 'InstBench.fpSRGBCurve1Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve2Add_FP32 {Hash_t{0x3f38effcu}};  // 'InstBench.fpSRGBCurve2Add-FP32'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve2Add_FP32_MED {Hash_t{0xf9204ff6u}};  // 'InstBench.fpSRGBCurve2Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve3Add_FP32 {Hash_t{0x2843fbbfu}};  // 'InstBench.fpSRGBCurve3Add-FP32'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve3Add_FP32_MED {Hash_t{0x24b69673u}};  // 'InstBench.fpSRGBCurve3Add-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpScalarAdd_FP32 {Hash_t{0x54934136u}};  // 'InstBench.fpScalarAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpScalarAdd_FP32_MED {Hash_t{0xa4a5ec0cu}};  // 'InstBench.fpScalarAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpScalarMulAdd_FP32 {Hash_t{0xd6dfa05u}};  // 'InstBench.fpScalarMulAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpScalarMulAdd_FP32_MED {Hash_t{0x17b88fd1u}};  // 'InstBench.fpScalarMulAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSignAdd_FP32 {Hash_t{0x58c68a17u}};  // 'InstBench.fpSignAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpSignAdd_FP32_MED {Hash_t{0xefabb8ccu}};  // 'InstBench.fpSignAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSignAdd_2_FP32 {Hash_t{0x883f756au}};  // 'InstBench.fpSignAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpSignAdd_2_FP32_MED {Hash_t{0x8bcb7c40u}};  // 'InstBench.fpSignAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSinAdd_FP32 {Hash_t{0x28643bb9u}};  // 'InstBench.fpSinAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpSinAdd_FP32_MED {Hash_t{0x4ab7ed8au}};  // 'InstBench.fpSinAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSinAdd_2_FP32 {Hash_t{0x4dd17b5u}};  // 'InstBench.fpSinAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpSinAdd_2_FP32_MED {Hash_t{0x36e994a4u}};  // 'InstBench.fpSinAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSinH_FP32 {Hash_t{0x35f5d807u}};  // 'InstBench.fpSinH-FP32'
			static constexpr PipelineName_t  InstBench_fpSinH_FP32_MED {Hash_t{0x17e26d58u}};  // 'InstBench.fpSinH-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSmoothstepAdd_FP32 {Hash_t{0xde3229f1u}};  // 'InstBench.fpSmoothstepAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpSmoothstepAdd_FP32_MED {Hash_t{0xb38a47bdu}};  // 'InstBench.fpSmoothstepAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_FP32 {Hash_t{0xb49a2ddau}};  // 'InstBench.fpSqrtAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_FP32_MED {Hash_t{0x80979f9u}};  // 'InstBench.fpSqrtAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_2_FP32 {Hash_t{0xbed9067eu}};  // 'InstBench.fpSqrtAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_2_FP32_MED {Hash_t{0xc599ab72u}};  // 'InstBench.fpSqrtAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_3_FP32 {Hash_t{0x7585d5dbu}};  // 'InstBench.fpSqrtAdd_3-FP32'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_3_FP32_MED {Hash_t{0x2a5bc04cu}};  // 'InstBench.fpSqrtAdd_3-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpStepAdd_FP32 {Hash_t{0x5d534a05u}};  // 'InstBench.fpStepAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpStepAdd_FP32_MED {Hash_t{0xf8483b48u}};  // 'InstBench.fpStepAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpTanAdd_FP32 {Hash_t{0x12886eceu}};  // 'InstBench.fpTanAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpTanAdd_FP32_MED {Hash_t{0xe2effca6u}};  // 'InstBench.fpTanAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpTanAdd_2_FP32 {Hash_t{0x7fa26713u}};  // 'InstBench.fpTanAdd_2-FP32'
			static constexpr PipelineName_t  InstBench_fpTanAdd_2_FP32_MED {Hash_t{0x656877a0u}};  // 'InstBench.fpTanAdd_2-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpTanH_FP32 {Hash_t{0x8271ae7bu}};  // 'InstBench.fpTanH-FP32'
			static constexpr PipelineName_t  InstBench_fpTanH_FP32_MED {Hash_t{0x6c9d1dfeu}};  // 'InstBench.fpTanH-FP32_MED'
			static constexpr PipelineName_t  InstBench_fpTruncAdd_FP32 {Hash_t{0x92e15874u}};  // 'InstBench.fpTruncAdd-FP32'
			static constexpr PipelineName_t  InstBench_fpTruncAdd_FP32_MED {Hash_t{0xe94198b4u}};  // 'InstBench.fpTruncAdd-FP32_MED'
			static constexpr PipelineName_t  InstBench_iAbsAdd_INT32 {Hash_t{0x33763a40u}};  // 'InstBench.iAbsAdd-INT32'
			static constexpr PipelineName_t  InstBench_iAbsAdd_INT32_LOW {Hash_t{0x3d19a4ccu}};  // 'InstBench.iAbsAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iAbsAdd_INT32_MED {Hash_t{0x428a67afu}};  // 'InstBench.iAbsAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iAdd_INT32 {Hash_t{0xd2ce4ff7u}};  // 'InstBench.iAdd-INT32'
			static constexpr PipelineName_t  InstBench_iAdd_INT32_LOW {Hash_t{0x14978e0au}};  // 'InstBench.iAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iAdd_INT32_MED {Hash_t{0x6b044d69u}};  // 'InstBench.iAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iAdd_UINT32 {Hash_t{0x94cd5212u}};  // 'InstBench.iAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iAdd_UINT32_LOW {Hash_t{0x555ab845u}};  // 'InstBench.iAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iAdd_UINT32_MED {Hash_t{0x2ac97b26u}};  // 'InstBench.iAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iAndAdd_INT32 {Hash_t{0xfc1e3216u}};  // 'InstBench.iAndAdd-INT32'
			static constexpr PipelineName_t  InstBench_iAndAdd_INT32_LOW {Hash_t{0x7b939b84u}};  // 'InstBench.iAndAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iAndAdd_INT32_MED {Hash_t{0x40058e7u}};  // 'InstBench.iAndAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iAndAdd_UINT32 {Hash_t{0x43ee5081u}};  // 'InstBench.iAndAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iAndAdd_UINT32_LOW {Hash_t{0x5f351277u}};  // 'InstBench.iAndAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iAndAdd_UINT32_MED {Hash_t{0x20a6d114u}};  // 'InstBench.iAndAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iBitCount_INT32 {Hash_t{0x12ae04f5u}};  // 'InstBench.iBitCount-INT32'
			static constexpr PipelineName_t  InstBench_iBitCount_INT32_LOW {Hash_t{0x3cc88db7u}};  // 'InstBench.iBitCount-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iBitCount_INT32_MED {Hash_t{0x435b4ed4u}};  // 'InstBench.iBitCount-INT32_MED'
			static constexpr PipelineName_t  InstBench_iBitCount_UINT32 {Hash_t{0x7a035375u}};  // 'InstBench.iBitCount-UINT32'
			static constexpr PipelineName_t  InstBench_iBitCount_UINT32_LOW {Hash_t{0xe0a22877u}};  // 'InstBench.iBitCount-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iBitCount_UINT32_MED {Hash_t{0x9f31eb14u}};  // 'InstBench.iBitCount-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iClampAdd_INT32 {Hash_t{0x81b9a310u}};  // 'InstBench.iClampAdd-INT32'
			static constexpr PipelineName_t  InstBench_iClampAdd_INT32_LOW {Hash_t{0x6ddb3130u}};  // 'InstBench.iClampAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iClampAdd_INT32_MED {Hash_t{0x1248f253u}};  // 'InstBench.iClampAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iClampAdd_UINT32 {Hash_t{0xaaf05225u}};  // 'InstBench.iClampAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iClampAdd_UINT32_LOW {Hash_t{0x932f2d48u}};  // 'InstBench.iClampAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iClampAdd_UINT32_MED {Hash_t{0xecbcee2bu}};  // 'InstBench.iClampAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_INT32 {Hash_t{0x7df98a15u}};  // 'InstBench.iClampConstAdd-INT32'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_INT32_LOW {Hash_t{0x29eb4d5fu}};  // 'InstBench.iClampConstAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_INT32_MED {Hash_t{0x56788e3cu}};  // 'InstBench.iClampConstAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_UINT32 {Hash_t{0xda66e683u}};  // 'InstBench.iClampConstAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_UINT32_LOW {Hash_t{0x4e6661fdu}};  // 'InstBench.iClampConstAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_UINT32_MED {Hash_t{0x31f5a29eu}};  // 'InstBench.iClampConstAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iDivAdd_INT32 {Hash_t{0x6edd0150u}};  // 'InstBench.iDivAdd-INT32'
			static constexpr PipelineName_t  InstBench_iDivAdd_INT32_LOW {Hash_t{0x1d17b149u}};  // 'InstBench.iDivAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iDivAdd_INT32_MED {Hash_t{0x6284722au}};  // 'InstBench.iDivAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iDivAdd_UINT32 {Hash_t{0xdcc37717u}};  // 'InstBench.iDivAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iDivAdd_UINT32_LOW {Hash_t{0xba862850u}};  // 'InstBench.iDivAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iDivAdd_UINT32_MED {Hash_t{0xc515eb33u}};  // 'InstBench.iDivAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iFindLSB_INT32 {Hash_t{0x2d70e639u}};  // 'InstBench.iFindLSB-INT32'
			static constexpr PipelineName_t  InstBench_iFindLSB_INT32_LOW {Hash_t{0x3e81d6bu}};  // 'InstBench.iFindLSB-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iFindLSB_INT32_MED {Hash_t{0x7c7bde08u}};  // 'InstBench.iFindLSB-INT32_MED'
			static constexpr PipelineName_t  InstBench_iFindLSB_UINT32 {Hash_t{0xe8ee030cu}};  // 'InstBench.iFindLSB-UINT32'
			static constexpr PipelineName_t  InstBench_iFindLSB_UINT32_LOW {Hash_t{0x6ff89618u}};  // 'InstBench.iFindLSB-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iFindLSB_UINT32_MED {Hash_t{0x106b557bu}};  // 'InstBench.iFindLSB-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iFindMSB_INT32 {Hash_t{0x3a0bf27au}};  // 'InstBench.iFindMSB-INT32'
			static constexpr PipelineName_t  InstBench_iFindMSB_INT32_LOW {Hash_t{0xde7ec4eeu}};  // 'InstBench.iFindMSB-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iFindMSB_INT32_MED {Hash_t{0xa1ed078du}};  // 'InstBench.iFindMSB-INT32_MED'
			static constexpr PipelineName_t  InstBench_iFindMSB_UINT32 {Hash_t{0x72c6832u}};  // 'InstBench.iFindMSB-UINT32'
			static constexpr PipelineName_t  InstBench_iFindMSB_UINT32_LOW {Hash_t{0xf2f7776eu}};  // 'InstBench.iFindMSB-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iFindMSB_UINT32_MED {Hash_t{0x8d64b40du}};  // 'InstBench.iFindMSB-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_INT32 {Hash_t{0xf97be0e3u}};  // 'InstBench.iMinMaxAdd-INT32'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_INT32_LOW {Hash_t{0x2cc6f722u}};  // 'InstBench.iMinMaxAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_INT32_MED {Hash_t{0x53553441u}};  // 'InstBench.iMinMaxAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_UINT32 {Hash_t{0x8e3c33c0u}};  // 'InstBench.iMinMaxAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_UINT32_LOW {Hash_t{0x60d741c6u}};  // 'InstBench.iMinMaxAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_UINT32_MED {Hash_t{0x1f4482a5u}};  // 'InstBench.iMinMaxAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iModAdd_INT32 {Hash_t{0x389aa057u}};  // 'InstBench.iModAdd-INT32'
			static constexpr PipelineName_t  InstBench_iModAdd_INT32_LOW {Hash_t{0x654a93a4u}};  // 'InstBench.iModAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iModAdd_INT32_MED {Hash_t{0x1ad950c7u}};  // 'InstBench.iModAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iModAdd_UINT32 {Hash_t{0x42f1a515u}};  // 'InstBench.iModAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iModAdd_UINT32_LOW {Hash_t{0x6445ebb7u}};  // 'InstBench.iModAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iModAdd_UINT32_MED {Hash_t{0x1bd628d4u}};  // 'InstBench.iModAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iMul_INT32 {Hash_t{0xb704c331u}};  // 'InstBench.iMul-INT32'
			static constexpr PipelineName_t  InstBench_iMul_INT32_LOW {Hash_t{0xa3537727u}};  // 'InstBench.iMul-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iMul_INT32_MED {Hash_t{0xdcc0b444u}};  // 'InstBench.iMul-INT32_MED'
			static constexpr PipelineName_t  InstBench_iMul_UINT32 {Hash_t{0xe6afff1bu}};  // 'InstBench.iMul-UINT32'
			static constexpr PipelineName_t  InstBench_iMul_UINT32_LOW {Hash_t{0x103220c9u}};  // 'InstBench.iMul-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iMul_UINT32_MED {Hash_t{0x6fa1e3aau}};  // 'InstBench.iMul-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iMulAdd_INT32 {Hash_t{0x67147cecu}};  // 'InstBench.iMulAdd-INT32'
			static constexpr PipelineName_t  InstBench_iMulAdd_INT32_LOW {Hash_t{0x757e6fcbu}};  // 'InstBench.iMulAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iMulAdd_INT32_MED {Hash_t{0xaedaca8u}};  // 'InstBench.iMulAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iMulAdd_UINT32 {Hash_t{0x1e1d41cdu}};  // 'InstBench.iMulAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iMulAdd_UINT32_LOW {Hash_t{0xb958a382u}};  // 'InstBench.iMulAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iMulAdd_UINT32_MED {Hash_t{0xc6cb60e1u}};  // 'InstBench.iMulAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iMulExtended_INT32 {Hash_t{0xf21432adu}};  // 'InstBench.iMulExtended-INT32'
			static constexpr PipelineName_t  InstBench_iMulExtended_INT32_LOW {Hash_t{0xacb79792u}};  // 'InstBench.iMulExtended-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iMulExtended_INT32_MED {Hash_t{0xd32454f1u}};  // 'InstBench.iMulExtended-INT32_MED'
			static constexpr PipelineName_t  InstBench_iOrAdd_INT32 {Hash_t{0x2bfca231u}};  // 'InstBench.iOrAdd-INT32'
			static constexpr PipelineName_t  InstBench_iOrAdd_INT32_LOW {Hash_t{0xcf9a62eau}};  // 'InstBench.iOrAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iOrAdd_INT32_MED {Hash_t{0xb009a189u}};  // 'InstBench.iOrAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iOrAdd_UINT32 {Hash_t{0xe633077au}};  // 'InstBench.iOrAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iOrAdd_UINT32_LOW {Hash_t{0xf58b57d1u}};  // 'InstBench.iOrAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iOrAdd_UINT32_MED {Hash_t{0x8a1894b2u}};  // 'InstBench.iOrAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iShift_INT32 {Hash_t{0xa34e5e14u}};  // 'InstBench.iShift-INT32'
			static constexpr PipelineName_t  InstBench_iShift_INT32_LOW {Hash_t{0xd138707au}};  // 'InstBench.iShift-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iShift_INT32_MED {Hash_t{0xaeabb319u}};  // 'InstBench.iShift-INT32_MED'
			static constexpr PipelineName_t  InstBench_iShift_UINT32 {Hash_t{0xadbf61c1u}};  // 'InstBench.iShift-UINT32'
			static constexpr PipelineName_t  InstBench_iShift_UINT32_LOW {Hash_t{0x59a6687u}};  // 'InstBench.iShift-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iShift_UINT32_MED {Hash_t{0x7a09a5e4u}};  // 'InstBench.iShift-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iShiftConst_INT32 {Hash_t{0xbc108a38u}};  // 'InstBench.iShiftConst-INT32'
			static constexpr PipelineName_t  InstBench_iShiftConst_INT32_LOW {Hash_t{0x186c803fu}};  // 'InstBench.iShiftConst-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iShiftConst_INT32_MED {Hash_t{0x67ff435cu}};  // 'InstBench.iShiftConst-INT32_MED'
			static constexpr PipelineName_t  InstBench_iShiftConst_UINT32 {Hash_t{0x9f7853f6u}};  // 'InstBench.iShiftConst-UINT32'
			static constexpr PipelineName_t  InstBench_iShiftConst_UINT32_LOW {Hash_t{0x3e58768u}};  // 'InstBench.iShiftConst-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iShiftConst_UINT32_MED {Hash_t{0x7c76440bu}};  // 'InstBench.iShiftConst-UINT32_MED'
			static constexpr PipelineName_t  InstBench_iSignAdd_INT32 {Hash_t{0x3a1dec29u}};  // 'InstBench.iSignAdd-INT32'
			static constexpr PipelineName_t  InstBench_iSignAdd_INT32_LOW {Hash_t{0xde65f0fu}};  // 'InstBench.iSignAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iSignAdd_INT32_MED {Hash_t{0x72759c6cu}};  // 'InstBench.iSignAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iSignAdd_2_INT32 {Hash_t{0x539adec2u}};  // 'InstBench.iSignAdd_2-INT32'
			static constexpr PipelineName_t  InstBench_iSignAdd_2_INT32_LOW {Hash_t{0x6bdb7fb8u}};  // 'InstBench.iSignAdd_2-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iSignAdd_2_INT32_MED {Hash_t{0x1448bcdbu}};  // 'InstBench.iSignAdd_2-INT32_MED'
			static constexpr PipelineName_t  InstBench_iXorAdd_INT32 {Hash_t{0x8691cb14u}};  // 'InstBench.iXorAdd-INT32'
			static constexpr PipelineName_t  InstBench_iXorAdd_INT32_LOW {Hash_t{0x674407f6u}};  // 'InstBench.iXorAdd-INT32_LOW'
			static constexpr PipelineName_t  InstBench_iXorAdd_INT32_MED {Hash_t{0x18d7c495u}};  // 'InstBench.iXorAdd-INT32_MED'
			static constexpr PipelineName_t  InstBench_iXorAdd_UINT32 {Hash_t{0xad9abe54u}};  // 'InstBench.iXorAdd-UINT32'
			static constexpr PipelineName_t  InstBench_iXorAdd_UINT32_LOW {Hash_t{0xe122d5fbu}};  // 'InstBench.iXorAdd-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_iXorAdd_UINT32_MED {Hash_t{0x9eb11698u}};  // 'InstBench.iXorAdd-UINT32_MED'
			static constexpr PipelineName_t  InstBench_uAddCarry_UINT32 {Hash_t{0x48dd9610u}};  // 'InstBench.uAddCarry-UINT32'
			static constexpr PipelineName_t  InstBench_uAddCarry_UINT32_LOW {Hash_t{0xac52c2fcu}};  // 'InstBench.uAddCarry-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_uAddCarry_UINT32_MED {Hash_t{0xd3c1019fu}};  // 'InstBench.uAddCarry-UINT32_MED'
			static constexpr PipelineName_t  InstBench_uMulExtended_UINT32 {Hash_t{0xecd81be8u}};  // 'InstBench.uMulExtended-UINT32'
			static constexpr PipelineName_t  InstBench_uMulExtended_UINT32_LOW {Hash_t{0xaaa1e7d3u}};  // 'InstBench.uMulExtended-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_uMulExtended_UINT32_MED {Hash_t{0xd53224b0u}};  // 'InstBench.uMulExtended-UINT32_MED'
			static constexpr PipelineName_t  InstBench_uSubBorrow_UINT32 {Hash_t{0x3ef962deu}};  // 'InstBench.uSubBorrow-UINT32'
			static constexpr PipelineName_t  InstBench_uSubBorrow_UINT32_LOW {Hash_t{0xc083b56au}};  // 'InstBench.uSubBorrow-UINT32_LOW'
			static constexpr PipelineName_t  InstBench_uSubBorrow_UINT32_MED {Hash_t{0xbf107609u}};  // 'InstBench.uSubBorrow-UINT32_MED'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_Low;

	static constexpr struct _InstBenchRT_Low_ShaderFloat16
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xaeefde62u}};}  // 'InstBenchRT.Low.ShaderFloat16'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_f16ParFMA_FP16 {Hash_t{0xae3b2ad2u}};  // 'InstBench.f16ParFMA-FP16'
			static constexpr PipelineName_t  InstBench_f16ParMulAdd_FP16 {Hash_t{0x6f6e83ecu}};  // 'InstBench.f16ParMulAdd-FP16'
			static constexpr PipelineName_t  InstBench_f16SeqFMA_FP16 {Hash_t{0x7bfd06cu}};  // 'InstBench.f16SeqFMA-FP16'
			static constexpr PipelineName_t  InstBench_f16SeqMulAdd_FP16 {Hash_t{0xa4281e51u}};  // 'InstBench.f16SeqMulAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpACosAdd_FP16 {Hash_t{0xa7d56d6eu}};  // 'InstBench.fpACosAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpACosAdd_2_FP16 {Hash_t{0xd0f7f9d1u}};  // 'InstBench.fpACosAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpACosAdd_3_FP16 {Hash_t{0x1bab2a74u}};  // 'InstBench.fpACosAdd_3-FP16'
			static constexpr PipelineName_t  InstBench_fpACosH_FP16 {Hash_t{0x73b644fdu}};  // 'InstBench.fpACosH-FP16'
			static constexpr PipelineName_t  InstBench_fpASinAdd_FP16 {Hash_t{0x59b27cc4u}};  // 'InstBench.fpASinAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpASinAdd_2_FP16 {Hash_t{0xeeaf7327u}};  // 'InstBench.fpASinAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpASinAdd_3_FP16 {Hash_t{0x25f3a082u}};  // 'InstBench.fpASinAdd_3-FP16'
			static constexpr PipelineName_t  InstBench_fpASinH_FP16 {Hash_t{0x1f10f205u}};  // 'InstBench.fpASinH-FP16'
			static constexpr PipelineName_t  InstBench_fpATanAdd_FP16 {Hash_t{0x635e29b3u}};  // 'InstBench.fpATanAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpATanAdd_2_FP16 {Hash_t{0x95d00381u}};  // 'InstBench.fpATanAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpATanAdd_3_FP16 {Hash_t{0x5e8cd024u}};  // 'InstBench.fpATanAdd_3-FP16'
			static constexpr PipelineName_t  InstBench_fpATanH_FP16 {Hash_t{0xa8948479u}};  // 'InstBench.fpATanH-FP16'
			static constexpr PipelineName_t  InstBench_fpAbsAdd_FP16 {Hash_t{0x4ecabef9u}};  // 'InstBench.fpAbsAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpAdd_FP16 {Hash_t{0x181a0058u}};  // 'InstBench.fpAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpCbrtAdd_2_FP16 {Hash_t{0xe4fd569du}};  // 'InstBench.fpCbrtAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpCbrtAdd_3_FP16 {Hash_t{0x2fa18538u}};  // 'InstBench.fpCbrtAdd_3-FP16'
			static constexpr PipelineName_t  InstBench_fpCbrtExpAdd_FP16 {Hash_t{0x7322b4ddu}};  // 'InstBench.fpCbrtExpAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpCbrtPowAdd_FP16 {Hash_t{0xd902b0cu}};  // 'InstBench.fpCbrtPowAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpCeilAdd_FP16 {Hash_t{0x5ba04b82u}};  // 'InstBench.fpCeilAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpClampAdd_FP16 {Hash_t{0xebb21271u}};  // 'InstBench.fpClampAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpClampSNormAdd_FP16 {Hash_t{0xacd8c94eu}};  // 'InstBench.fpClampSNormAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpClampUNormAdd_FP16 {Hash_t{0xc3d1693u}};  // 'InstBench.fpClampUNormAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpCosAdd_FP16 {Hash_t{0xe3588c88u}};  // 'InstBench.fpCosAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpCosAdd_2_FP16 {Hash_t{0xfde3bd8u}};  // 'InstBench.fpCosAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpCosH_FP16 {Hash_t{0x6c08c864u}};  // 'InstBench.fpCosH-FP16'
			static constexpr PipelineName_t  InstBench_fpCross3_FP16 {Hash_t{0xfe6f9bau}};  // 'InstBench.fpCross3-FP16'
			static constexpr PipelineName_t  InstBench_fpDistanceAdd_FP16 {Hash_t{0x115e0026u}};  // 'InstBench.fpDistanceAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpDivAdd_FP16 {Hash_t{0x8deaac03u}};  // 'InstBench.fpDivAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpDot_FP16 {Hash_t{0x512b585du}};  // 'InstBench.fpDot-FP16'
			static constexpr PipelineName_t  InstBench_fpDualAdd_FP16 {Hash_t{0x92192632u}};  // 'InstBench.fpDualAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpDualMulAdd_FP16 {Hash_t{0xed4ca7edu}};  // 'InstBench.fpDualMulAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpExp2Add_FP16 {Hash_t{0xb0d45768u}};  // 'InstBench.fpExp2Add-FP16'
			static constexpr PipelineName_t  InstBench_fpExpAdd_FP16 {Hash_t{0xf712e09fu}};  // 'InstBench.fpExpAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpFMA_FP16 {Hash_t{0x1f9be18bu}};  // 'InstBench.fpFMA-FP16'
			static constexpr PipelineName_t  InstBench_fpFloorAdd_FP16 {Hash_t{0x8ee872dfu}};  // 'InstBench.fpFloorAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpFractAdd_FP16 {Hash_t{0x5091fe3fu}};  // 'InstBench.fpFractAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpInvSqrtAdd_FP16 {Hash_t{0x40b5f95cu}};  // 'InstBench.fpInvSqrtAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpLengthAdd_FP16 {Hash_t{0x58d72a9cu}};  // 'InstBench.fpLengthAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpLerpClamp_FP16 {Hash_t{0xf562f4ecu}};  // 'InstBench.fpLerpClamp-FP16'
			static constexpr PipelineName_t  InstBench_fpLog2Add_FP16 {Hash_t{0x6fc22620u}};  // 'InstBench.fpLog2Add-FP16'
			static constexpr PipelineName_t  InstBench_fpLogAdd_FP16 {Hash_t{0x818c6b33u}};  // 'InstBench.fpLogAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpMinMaxAdd_FP16 {Hash_t{0x5616ad9fu}};  // 'InstBench.fpMinMaxAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpModAdd_FP16 {Hash_t{0x798f8836u}};  // 'InstBench.fpModAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpMul_FP16 {Hash_t{0x624f0000u}};  // 'InstBench.fpMul-FP16'
			static constexpr PipelineName_t  InstBench_fpMulAdd_FP16 {Hash_t{0xf2db3b0fu}};  // 'InstBench.fpMulAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpNormalizeAdd_FP16 {Hash_t{0xdc2f8e18u}};  // 'InstBench.fpNormalizeAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpPow17Add_FP16 {Hash_t{0xca790c15u}};  // 'InstBench.fpPow17Add-FP16'
			static constexpr PipelineName_t  InstBench_fpPow1ov17Add_FP16 {Hash_t{0xa4fdc27bu}};  // 'InstBench.fpPow1ov17Add-FP16'
			static constexpr PipelineName_t  InstBench_fpPow1ov33Add_FP16 {Hash_t{0xfde4434au}};  // 'InstBench.fpPow1ov33Add-FP16'
			static constexpr PipelineName_t  InstBench_fpPow1ov7Add_FP16 {Hash_t{0xe421cb6du}};  // 'InstBench.fpPow1ov7Add-FP16'
			static constexpr PipelineName_t  InstBench_fpPow33Add_FP16 {Hash_t{0x93608d24u}};  // 'InstBench.fpPow33Add-FP16'
			static constexpr PipelineName_t  InstBench_fpPow8Add_FP16 {Hash_t{0xd4508d2bu}};  // 'InstBench.fpPow8Add-FP16'
			static constexpr PipelineName_t  InstBench_fpQdrtInvsqrtAdd_FP16 {Hash_t{0x5e35c4cau}};  // 'InstBench.fpQdrtInvsqrtAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpQdrtPowAdd_FP16 {Hash_t{0x19b64e64u}};  // 'InstBench.fpQdrtPowAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpQdrtSqrtAdd_FP16 {Hash_t{0x5aea54a6u}};  // 'InstBench.fpQdrtSqrtAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpRoundAdd_FP16 {Hash_t{0xf189a0e9u}};  // 'InstBench.fpRoundAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpRoundEvenAdd_FP16 {Hash_t{0x484f733au}};  // 'InstBench.fpRoundEvenAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve1Add_FP16 {Hash_t{0x33ee75a2u}};  // 'InstBench.fpSRGBCurve1Add-FP16'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve2Add_FP16 {Hash_t{0xa634967u}};  // 'InstBench.fpSRGBCurve2Add-FP16'
			static constexpr PipelineName_t  InstBench_fpSRGBCurve3Add_FP16 {Hash_t{0x1d185d24u}};  // 'InstBench.fpSRGBCurve3Add-FP16'
			static constexpr PipelineName_t  InstBench_fpScalarAdd_FP16 {Hash_t{0x61c8e7adu}};  // 'InstBench.fpScalarAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpScalarMulAdd_FP16 {Hash_t{0x38365c9eu}};  // 'InstBench.fpScalarMulAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpSignAdd_FP16 {Hash_t{0x6d9d2c8cu}};  // 'InstBench.fpSignAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpSignAdd_2_FP16 {Hash_t{0xbd64d3f1u}};  // 'InstBench.fpSignAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpSinAdd_FP16 {Hash_t{0x1d3f9d22u}};  // 'InstBench.fpSinAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpSinAdd_2_FP16 {Hash_t{0x3186b12eu}};  // 'InstBench.fpSinAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpSinH_FP16 {Hash_t{0xae7e9cu}};  // 'InstBench.fpSinH-FP16'
			static constexpr PipelineName_t  InstBench_fpSmoothstepAdd_FP16 {Hash_t{0xeb698f6au}};  // 'InstBench.fpSmoothstepAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_FP16 {Hash_t{0x81c18b41u}};  // 'InstBench.fpSqrtAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_2_FP16 {Hash_t{0x8b82a0e5u}};  // 'InstBench.fpSqrtAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_3_FP16 {Hash_t{0x40de7340u}};  // 'InstBench.fpSqrtAdd_3-FP16'
			static constexpr PipelineName_t  InstBench_fpStepAdd_FP16 {Hash_t{0x6808ec9eu}};  // 'InstBench.fpStepAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpTanAdd_FP16 {Hash_t{0x27d3c855u}};  // 'InstBench.fpTanAdd-FP16'
			static constexpr PipelineName_t  InstBench_fpTanAdd_2_FP16 {Hash_t{0x4af9c188u}};  // 'InstBench.fpTanAdd_2-FP16'
			static constexpr PipelineName_t  InstBench_fpTanH_FP16 {Hash_t{0xb72a08e0u}};  // 'InstBench.fpTanH-FP16'
			static constexpr PipelineName_t  InstBench_fpTruncAdd_FP16 {Hash_t{0xa7bafeefu}};  // 'InstBench.fpTruncAdd-FP16'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_Low_ShaderFloat16;

	static constexpr struct _InstBenchRT_Low_ShaderFloat64
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xfa02989u}};}  // 'InstBenchRT.Low.ShaderFloat64'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_fpAbsAdd_FP64 {Hash_t{0xef854912u}};  // 'InstBench.fpAbsAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpAdd_FP64 {Hash_t{0xb955f7b3u}};  // 'InstBench.fpAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpCeilAdd_FP64 {Hash_t{0xfaefbc69u}};  // 'InstBench.fpCeilAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpClampAdd_FP64 {Hash_t{0x4afde59au}};  // 'InstBench.fpClampAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpClampSNormAdd_FP64 {Hash_t{0xd973ea5u}};  // 'InstBench.fpClampSNormAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpClampUNormAdd_FP64 {Hash_t{0xad72e178u}};  // 'InstBench.fpClampUNormAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpCross3_FP64 {Hash_t{0xaea90e51u}};  // 'InstBench.fpCross3-FP64'
			static constexpr PipelineName_t  InstBench_fpDistanceAdd_FP64 {Hash_t{0xb011f7cdu}};  // 'InstBench.fpDistanceAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpDivAdd_FP64 {Hash_t{0x2ca55be8u}};  // 'InstBench.fpDivAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpDot_FP64 {Hash_t{0xf064afb6u}};  // 'InstBench.fpDot-FP64'
			static constexpr PipelineName_t  InstBench_fpDualAdd_FP64 {Hash_t{0x3356d1d9u}};  // 'InstBench.fpDualAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpDualMulAdd_FP64 {Hash_t{0x4c035006u}};  // 'InstBench.fpDualMulAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpFMA_FP64 {Hash_t{0xbed41660u}};  // 'InstBench.fpFMA-FP64'
			static constexpr PipelineName_t  InstBench_fpFloorAdd_FP64 {Hash_t{0x2fa78534u}};  // 'InstBench.fpFloorAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpFractAdd_FP64 {Hash_t{0xf1de09d4u}};  // 'InstBench.fpFractAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpInvSqrtAdd_FP64 {Hash_t{0xe1fa0eb7u}};  // 'InstBench.fpInvSqrtAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpLengthAdd_FP64 {Hash_t{0xf998dd77u}};  // 'InstBench.fpLengthAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpLerpClamp_FP64 {Hash_t{0x542d0307u}};  // 'InstBench.fpLerpClamp-FP64'
			static constexpr PipelineName_t  InstBench_fpMinMaxAdd_FP64 {Hash_t{0xf7595a74u}};  // 'InstBench.fpMinMaxAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpModAdd_FP64 {Hash_t{0xd8c07fddu}};  // 'InstBench.fpModAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpMul_FP64 {Hash_t{0xc300f7ebu}};  // 'InstBench.fpMul-FP64'
			static constexpr PipelineName_t  InstBench_fpMulAdd_FP64 {Hash_t{0x5394cce4u}};  // 'InstBench.fpMulAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpNormalizeAdd_FP64 {Hash_t{0x7d6079f3u}};  // 'InstBench.fpNormalizeAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpRoundAdd_FP64 {Hash_t{0x50c65702u}};  // 'InstBench.fpRoundAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpRoundEvenAdd_FP64 {Hash_t{0xe90084d1u}};  // 'InstBench.fpRoundEvenAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpScalarAdd_FP64 {Hash_t{0xc0871046u}};  // 'InstBench.fpScalarAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpScalarMulAdd_FP64 {Hash_t{0x9979ab75u}};  // 'InstBench.fpScalarMulAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpSignAdd_FP64 {Hash_t{0xccd2db67u}};  // 'InstBench.fpSignAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpSignAdd_2_FP64 {Hash_t{0x1c2b241au}};  // 'InstBench.fpSignAdd_2-FP64'
			static constexpr PipelineName_t  InstBench_fpSmoothstepAdd_FP64 {Hash_t{0x4a267881u}};  // 'InstBench.fpSmoothstepAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpSqrtAdd_FP64 {Hash_t{0x208e7caau}};  // 'InstBench.fpSqrtAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpStepAdd_FP64 {Hash_t{0xc9471b75u}};  // 'InstBench.fpStepAdd-FP64'
			static constexpr PipelineName_t  InstBench_fpTruncAdd_FP64 {Hash_t{0x6f50904u}};  // 'InstBench.fpTruncAdd-FP64'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_Low_ShaderFloat64;

	static constexpr struct _InstBenchRT_Low_ShaderFloatInt16
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x6180f6eu}};}  // 'InstBenchRT.Low.ShaderFloatInt16'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_fiBitCast_Add_FP16 {Hash_t{0x30e06cc4u}};  // 'InstBench.fiBitCast_Add-FP16'
			static constexpr PipelineName_t  InstBench_fiFloatToInt_Add_FP16 {Hash_t{0x25832620u}};  // 'InstBench.fiFloatToInt_Add-FP16'
			static constexpr PipelineName_t  InstBench_fiIntToFloat_Add_FP16 {Hash_t{0x8e26e99fu}};  // 'InstBench.fiIntToFloat_Add-FP16'
			static constexpr PipelineName_t  InstBench_fiParAdd11_FP16 {Hash_t{0xbb557b28u}};  // 'InstBench.fiParAdd11-FP16'
			static constexpr PipelineName_t  InstBench_fiParAdd21_FP16 {Hash_t{0x8abd61b5u}};  // 'InstBench.fiParAdd21-FP16'
			static constexpr PipelineName_t  InstBench_fiParFMulIAdd_FP16 {Hash_t{0x652ee5c6u}};  // 'InstBench.fiParFMulIAdd-FP16'
			static constexpr PipelineName_t  InstBench_fiParMul_FP16 {Hash_t{0x361add87u}};  // 'InstBench.fiParMul-FP16'
			static constexpr PipelineName_t  InstBench_fiSeqAdd_BitCast_FP16 {Hash_t{0x134b665au}};  // 'InstBench.fiSeqAdd_BitCast-FP16'
			static constexpr PipelineName_t  InstBench_fiSeqFMulIAdd_BitCast_FP16 {Hash_t{0x7e149935u}};  // 'InstBench.fiSeqFMulIAdd_BitCast-FP16'
			static constexpr PipelineName_t  InstBench_fiSeqMul_BitCast_FP16 {Hash_t{0x7aa9ef03u}};  // 'InstBench.fiSeqMul_BitCast-FP16'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_Low_ShaderFloatInt16;

	static constexpr struct _InstBenchRT_Low_ShaderFloatInt64
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xa757f885u}};}  // 'InstBenchRT.Low.ShaderFloatInt64'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_fiBitCast_Add_FP64 {Hash_t{0x91af9b2fu}};  // 'InstBench.fiBitCast_Add-FP64'
			static constexpr PipelineName_t  InstBench_fiFloatToInt_Add_FP64 {Hash_t{0x84ccd1cbu}};  // 'InstBench.fiFloatToInt_Add-FP64'
			static constexpr PipelineName_t  InstBench_fiIntToFloat_Add_FP64 {Hash_t{0x2f691e74u}};  // 'InstBench.fiIntToFloat_Add-FP64'
			static constexpr PipelineName_t  InstBench_fiParAdd11_FP64 {Hash_t{0x1a1a8cc3u}};  // 'InstBench.fiParAdd11-FP64'
			static constexpr PipelineName_t  InstBench_fiParAdd21_FP64 {Hash_t{0x2bf2965eu}};  // 'InstBench.fiParAdd21-FP64'
			static constexpr PipelineName_t  InstBench_fiParFMulIAdd_FP64 {Hash_t{0xc461122du}};  // 'InstBench.fiParFMulIAdd-FP64'
			static constexpr PipelineName_t  InstBench_fiParMul_FP64 {Hash_t{0x97552a6cu}};  // 'InstBench.fiParMul-FP64'
			static constexpr PipelineName_t  InstBench_fiSeqAdd_BitCast_FP64 {Hash_t{0xb20491b1u}};  // 'InstBench.fiSeqAdd_BitCast-FP64'
			static constexpr PipelineName_t  InstBench_fiSeqFMulIAdd_BitCast_FP64 {Hash_t{0xdf5b6edeu}};  // 'InstBench.fiSeqFMulIAdd_BitCast-FP64'
			static constexpr PipelineName_t  InstBench_fiSeqMul_BitCast_FP64 {Hash_t{0xdbe618e8u}};  // 'InstBench.fiSeqMul_BitCast-FP64'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_Low_ShaderFloatInt64;

	static constexpr struct _InstBenchRT_Low_ShaderInt16
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xa5ff099au}};}  // 'InstBenchRT.Low.ShaderInt16'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_iAbsAdd_INT16 {Hash_t{0x62d9cdbu}};  // 'InstBench.iAbsAdd-INT16'
			static constexpr PipelineName_t  InstBench_iAdd_INT16 {Hash_t{0xe795e96cu}};  // 'InstBench.iAdd-INT16'
			static constexpr PipelineName_t  InstBench_iAdd_UINT16 {Hash_t{0xa196f489u}};  // 'InstBench.iAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iAndAdd_INT16 {Hash_t{0xc945948du}};  // 'InstBench.iAndAdd-INT16'
			static constexpr PipelineName_t  InstBench_iAndAdd_UINT16 {Hash_t{0x76b5f61au}};  // 'InstBench.iAndAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iClampAdd_INT16 {Hash_t{0xb4e2058bu}};  // 'InstBench.iClampAdd-INT16'
			static constexpr PipelineName_t  InstBench_iClampAdd_UINT16 {Hash_t{0x9fabf4beu}};  // 'InstBench.iClampAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_INT16 {Hash_t{0x48a22c8eu}};  // 'InstBench.iClampConstAdd-INT16'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_UINT16 {Hash_t{0xef3d4018u}};  // 'InstBench.iClampConstAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iDivAdd_INT16 {Hash_t{0x5b86a7cbu}};  // 'InstBench.iDivAdd-INT16'
			static constexpr PipelineName_t  InstBench_iDivAdd_UINT16 {Hash_t{0xe998d18cu}};  // 'InstBench.iDivAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iFindLSB_INT16 {Hash_t{0x182b40a2u}};  // 'InstBench.iFindLSB-INT16'
			static constexpr PipelineName_t  InstBench_iFindLSB_UINT16 {Hash_t{0xddb5a597u}};  // 'InstBench.iFindLSB-UINT16'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_INT16 {Hash_t{0xcc204678u}};  // 'InstBench.iMinMaxAdd-INT16'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_UINT16 {Hash_t{0xbb67955bu}};  // 'InstBench.iMinMaxAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iModAdd_INT16 {Hash_t{0xdc106ccu}};  // 'InstBench.iModAdd-INT16'
			static constexpr PipelineName_t  InstBench_iModAdd_UINT16 {Hash_t{0x77aa038eu}};  // 'InstBench.iModAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iMul_INT16 {Hash_t{0x825f65aau}};  // 'InstBench.iMul-INT16'
			static constexpr PipelineName_t  InstBench_iMul_UINT16 {Hash_t{0xd3f45980u}};  // 'InstBench.iMul-UINT16'
			static constexpr PipelineName_t  InstBench_iMulAdd_INT16 {Hash_t{0x524fda77u}};  // 'InstBench.iMulAdd-INT16'
			static constexpr PipelineName_t  InstBench_iMulAdd_UINT16 {Hash_t{0x2b46e756u}};  // 'InstBench.iMulAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iOrAdd_INT16 {Hash_t{0x1ea704aau}};  // 'InstBench.iOrAdd-INT16'
			static constexpr PipelineName_t  InstBench_iOrAdd_UINT16 {Hash_t{0xd368a1e1u}};  // 'InstBench.iOrAdd-UINT16'
			static constexpr PipelineName_t  InstBench_iShift_INT16 {Hash_t{0x9615f88fu}};  // 'InstBench.iShift-INT16'
			static constexpr PipelineName_t  InstBench_iShift_UINT16 {Hash_t{0x98e4c75au}};  // 'InstBench.iShift-UINT16'
			static constexpr PipelineName_t  InstBench_iShiftConst_INT16 {Hash_t{0x894b2ca3u}};  // 'InstBench.iShiftConst-INT16'
			static constexpr PipelineName_t  InstBench_iShiftConst_UINT16 {Hash_t{0xaa23f56du}};  // 'InstBench.iShiftConst-UINT16'
			static constexpr PipelineName_t  InstBench_iSignAdd_INT16 {Hash_t{0xf464ab2u}};  // 'InstBench.iSignAdd-INT16'
			static constexpr PipelineName_t  InstBench_iSignAdd_2_INT16 {Hash_t{0x66c17859u}};  // 'InstBench.iSignAdd_2-INT16'
			static constexpr PipelineName_t  InstBench_iXorAdd_INT16 {Hash_t{0xb3ca6d8fu}};  // 'InstBench.iXorAdd-INT16'
			static constexpr PipelineName_t  InstBench_iXorAdd_UINT16 {Hash_t{0x98c118cfu}};  // 'InstBench.iXorAdd-UINT16'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_Low_ShaderInt16;

	static constexpr struct _InstBenchRT_Low_ShaderInt64
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x4b0fe71u}};}  // 'InstBenchRT.Low.ShaderInt64'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_iAbsAdd_INT64 {Hash_t{0xa7626b30u}};  // 'InstBench.iAbsAdd-INT64'
			static constexpr PipelineName_t  InstBench_iAdd_INT64 {Hash_t{0x46da1e87u}};  // 'InstBench.iAdd-INT64'
			static constexpr PipelineName_t  InstBench_iAdd_UINT64 {Hash_t{0xd90362u}};  // 'InstBench.iAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iAndAdd_INT64 {Hash_t{0x680a6366u}};  // 'InstBench.iAndAdd-INT64'
			static constexpr PipelineName_t  InstBench_iAndAdd_UINT64 {Hash_t{0xd7fa01f1u}};  // 'InstBench.iAndAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iClampAdd_INT64 {Hash_t{0x15adf260u}};  // 'InstBench.iClampAdd-INT64'
			static constexpr PipelineName_t  InstBench_iClampAdd_UINT64 {Hash_t{0x3ee40355u}};  // 'InstBench.iClampAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_INT64 {Hash_t{0xe9eddb65u}};  // 'InstBench.iClampConstAdd-INT64'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_UINT64 {Hash_t{0x4e72b7f3u}};  // 'InstBench.iClampConstAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iDivAdd_INT64 {Hash_t{0xfac95020u}};  // 'InstBench.iDivAdd-INT64'
			static constexpr PipelineName_t  InstBench_iDivAdd_UINT64 {Hash_t{0x48d72667u}};  // 'InstBench.iDivAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iFindLSB_INT64 {Hash_t{0xb964b749u}};  // 'InstBench.iFindLSB-INT64'
			static constexpr PipelineName_t  InstBench_iFindLSB_UINT64 {Hash_t{0x7cfa527cu}};  // 'InstBench.iFindLSB-UINT64'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_INT64 {Hash_t{0x6d6fb193u}};  // 'InstBench.iMinMaxAdd-INT64'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_UINT64 {Hash_t{0x1a2862b0u}};  // 'InstBench.iMinMaxAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iModAdd_INT64 {Hash_t{0xac8ef127u}};  // 'InstBench.iModAdd-INT64'
			static constexpr PipelineName_t  InstBench_iModAdd_UINT64 {Hash_t{0xd6e5f465u}};  // 'InstBench.iModAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iMul_INT64 {Hash_t{0x23109241u}};  // 'InstBench.iMul-INT64'
			static constexpr PipelineName_t  InstBench_iMul_UINT64 {Hash_t{0x72bbae6bu}};  // 'InstBench.iMul-UINT64'
			static constexpr PipelineName_t  InstBench_iMulAdd_INT64 {Hash_t{0xf3002d9cu}};  // 'InstBench.iMulAdd-INT64'
			static constexpr PipelineName_t  InstBench_iMulAdd_UINT64 {Hash_t{0x8a0910bdu}};  // 'InstBench.iMulAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iOrAdd_INT64 {Hash_t{0xbfe8f341u}};  // 'InstBench.iOrAdd-INT64'
			static constexpr PipelineName_t  InstBench_iOrAdd_UINT64 {Hash_t{0x7227560au}};  // 'InstBench.iOrAdd-UINT64'
			static constexpr PipelineName_t  InstBench_iShift_INT64 {Hash_t{0x375a0f64u}};  // 'InstBench.iShift-INT64'
			static constexpr PipelineName_t  InstBench_iShift_UINT64 {Hash_t{0x39ab30b1u}};  // 'InstBench.iShift-UINT64'
			static constexpr PipelineName_t  InstBench_iShiftConst_INT64 {Hash_t{0x2804db48u}};  // 'InstBench.iShiftConst-INT64'
			static constexpr PipelineName_t  InstBench_iShiftConst_UINT64 {Hash_t{0xb6c0286u}};  // 'InstBench.iShiftConst-UINT64'
			static constexpr PipelineName_t  InstBench_iSignAdd_INT64 {Hash_t{0xae09bd59u}};  // 'InstBench.iSignAdd-INT64'
			static constexpr PipelineName_t  InstBench_iSignAdd_2_INT64 {Hash_t{0xc78e8fb2u}};  // 'InstBench.iSignAdd_2-INT64'
			static constexpr PipelineName_t  InstBench_iXorAdd_INT64 {Hash_t{0x12859a64u}};  // 'InstBench.iXorAdd-INT64'
			static constexpr PipelineName_t  InstBench_iXorAdd_UINT64 {Hash_t{0x398eef24u}};  // 'InstBench.iXorAdd-UINT64'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_Low_ShaderInt64;

	static constexpr struct _InstBenchRT_Low_ShaderInt8
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x830a3993u}};}  // 'InstBenchRT.Low.ShaderInt8'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'InstBench.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  InstBench_iAbsAdd_INT8 {Hash_t{0xed9f0cf1u}};  // 'InstBench.iAbsAdd-INT8'
			static constexpr PipelineName_t  InstBench_iAdd_INT8 {Hash_t{0x5b72a3bau}};  // 'InstBench.iAdd-INT8'
			static constexpr PipelineName_t  InstBench_iAdd_UINT8 {Hash_t{0x31d13f15u}};  // 'InstBench.iAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iAndAdd_INT8 {Hash_t{0x3f02c3c7u}};  // 'InstBench.iAndAdd-INT8'
			static constexpr PipelineName_t  InstBench_iAndAdd_UINT8 {Hash_t{0x1f0142f4u}};  // 'InstBench.iAndAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iClampAdd_INT8 {Hash_t{0x9f0d7448u}};  // 'InstBench.iClampAdd-INT8'
			static constexpr PipelineName_t  InstBench_iClampAdd_UINT8 {Hash_t{0x62a6d3f2u}};  // 'InstBench.iClampAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_INT8 {Hash_t{0x66ac0d13u}};  // 'InstBench.iClampConstAdd-INT8'
			static constexpr PipelineName_t  InstBench_iClampConstAdd_UINT8 {Hash_t{0x9ee6faf7u}};  // 'InstBench.iClampConstAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iDivAdd_INT8 {Hash_t{0x2ebf1e0bu}};  // 'InstBench.iDivAdd-INT8'
			static constexpr PipelineName_t  InstBench_iDivAdd_UINT8 {Hash_t{0x8dc271b2u}};  // 'InstBench.iDivAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iFindLSB_INT8 {Hash_t{0xeab11d63u}};  // 'InstBench.iFindLSB-INT8'
			static constexpr PipelineName_t  InstBench_iFindLSB_UINT8 {Hash_t{0xce6f96dbu}};  // 'InstBench.iFindLSB-UINT8'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_INT8 {Hash_t{0x5a872500u}};  // 'InstBench.iMinMaxAdd-INT8'
			static constexpr PipelineName_t  InstBench_iMinMaxAdd_UINT8 {Hash_t{0x1a649001u}};  // 'InstBench.iMinMaxAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iModAdd_INT8 {Hash_t{0xdada3a3eu}};  // 'InstBench.iModAdd-INT8'
			static constexpr PipelineName_t  InstBench_iModAdd_UINT8 {Hash_t{0xdb85d0b5u}};  // 'InstBench.iModAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iMul_INT8 {Hash_t{0x2127a3e2u}};  // 'InstBench.iMul-INT8'
			static constexpr PipelineName_t  InstBench_iMul_UINT8 {Hash_t{0x541bb3d3u}};  // 'InstBench.iMul-UINT8'
			static constexpr PipelineName_t  InstBench_iMulAdd_INT8 {Hash_t{0x518e8907u}};  // 'InstBench.iMulAdd-INT8'
			static constexpr PipelineName_t  InstBench_iMulAdd_UINT8 {Hash_t{0x840b0c0eu}};  // 'InstBench.iMulAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iOrAdd_INT8 {Hash_t{0xd0400a26u}};  // 'InstBench.iOrAdd-INT8'
			static constexpr PipelineName_t  InstBench_iOrAdd_UINT8 {Hash_t{0xc8e3d2d3u}};  // 'InstBench.iOrAdd-UINT8'
			static constexpr PipelineName_t  InstBench_iShift_INT8 {Hash_t{0x6ae6c9feu}};  // 'InstBench.iShift-INT8'
			static constexpr PipelineName_t  InstBench_iShift_UINT8 {Hash_t{0x40512ef6u}};  // 'InstBench.iShift-UINT8'
			static constexpr PipelineName_t  InstBench_iShiftConst_INT8 {Hash_t{0xeeb18b2du}};  // 'InstBench.iShiftConst-INT8'
			static constexpr PipelineName_t  InstBench_iShiftConst_UINT8 {Hash_t{0x5f0ffadau}};  // 'InstBench.iShiftConst-UINT8'
			static constexpr PipelineName_t  InstBench_iSignAdd_INT8 {Hash_t{0x30054efdu}};  // 'InstBench.iSignAdd-INT8'
			static constexpr PipelineName_t  InstBench_iSignAdd_2_INT8 {Hash_t{0xb1f55b6eu}};  // 'InstBench.iSignAdd_2-INT8'
			static constexpr PipelineName_t  InstBench_iXorAdd_INT8 {Hash_t{0xdaa1694cu}};  // 'InstBench.iXorAdd-INT8'
			static constexpr PipelineName_t  InstBench_iXorAdd_UINT8 {Hash_t{0x658ebbf6u}};  // 'InstBench.iXorAdd-UINT8'
		} Graphics = {};

		// compute (1)
		static constexpr struct _Compute
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x3f019debu}};}  // 'Compute'
			static constexpr DSLayoutName_t  dsLayout {Hash_t{0x3cbf4eccu}};  // 'InstBenchRT.ds'
		} Compute = {};
	} InstBenchRT_Low_ShaderInt8;

	static constexpr struct _NaN_RT
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x8d517a9du}};}  // 'NaN.RT'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'NaN.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  NaN_FP32 {Hash_t{0xd128af7fu}};  // 'NaN.FP32'
			static constexpr PipelineName_t  NaN_FP32_LOW {Hash_t{0x4b5c5aeu}};  // 'NaN.FP32_LOW'
			static constexpr PipelineName_t  NaN_FP32_MED {Hash_t{0x7b2606cdu}};  // 'NaN.FP32_MED'
		} Graphics = {};
	} NaN_RT;

	static constexpr struct _NaN_RT_ShaderFloat16
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xafbcd091u}};}  // 'NaN.RT.ShaderFloat16'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'NaN.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  NaN_FP16 {Hash_t{0xe47309e4u}};  // 'NaN.FP16'
		} Graphics = {};
	} NaN_RT_ShaderFloat16;

	static constexpr struct _NaN_RT_ShaderFloat64
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xef3277au}};}  // 'NaN.RT.ShaderFloat64'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'NaN.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  NaN_FP64 {Hash_t{0x453cfe0fu}};  // 'NaN.FP64'
		} Graphics = {};
	} NaN_RT_ShaderFloat64;

	static constexpr struct _NaN_RT_ShaderFloatInt16
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0xdd7bef26u}};}  // 'NaN.RT.ShaderFloatInt16'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'NaN.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  NaN_FP16 {Hash_t{0xe47309e4u}};  // 'NaN.FP16'
		} Graphics = {};
	} NaN_RT_ShaderFloatInt16;

	static constexpr struct _NaN_RT_ShaderFloatInt64
	{
		constexpr operator RenderTechName_t () const { return RenderTechName_t{Hash_t{0x7c3418cdu}};}  // 'NaN.RT.ShaderFloatInt64'

		// graphics (0)
		static constexpr struct _Graphics
		{
			constexpr operator RenderTechPassName_t () const { return RenderTechPassName_t{Hash_t{0x39bfde0eu}};}  // 'Graphics'

			// RenderPass 'NaN.RP' subpass 'Main'

			static constexpr uint  attachmentsCount = 1;
			static constexpr AttachmentName_t  att_Color {Hash_t{0xa79767edu}};  // 'Color'

			// pipelines
			static constexpr PipelineName_t  NaN_FP64 {Hash_t{0x453cfe0fu}};  // 'NaN.FP64'
		} Graphics = {};
	} NaN_RT_ShaderFloatInt64;
}
