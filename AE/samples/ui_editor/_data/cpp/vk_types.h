#ifndef VB_UVs2_SCs1_Col8_DEFINED
#	define VB_UVs2_SCs1_Col8_DEFINED
	// size: 12, align: 2
	struct VB_UVs2_SCs1_Col8
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x594166a8u}};

		packed_ushort4  UV_Scale;  // normalized
		packed_ubyte4   Color;     // normalized
	};
#endif
	StaticAssert( offsetof(VB_UVs2_SCs1_Col8, UV_Scale) == 0 );
	StaticAssert( offsetof(VB_UVs2_SCs1_Col8, Color) == 8 );
	StaticAssert( sizeof(VB_UVs2_SCs1_Col8) == 12 );

#ifndef VB_Position_f2_DEFINED
#	define VB_Position_f2_DEFINED
	// size: 8, align: 4
	struct VB_Position_f2
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xa843e002u}};

		packed_float2  Position;
	};
#endif
	StaticAssert( offsetof(VB_Position_f2, Position) == 0 );
	StaticAssert( sizeof(VB_Position_f2) == 8 );

#ifndef ui_global_ublock_DEFINED
#	define ui_global_ublock_DEFINED
	// size: 16, align: 8 (16)
	struct ui_global_ublock
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x43a60681u}};

		float2  posScale;
		float2  posBias;
	};
#endif
	StaticAssert( offsetof(ui_global_ublock, posScale) == 0 );
	StaticAssert( offsetof(ui_global_ublock, posBias) == 8 );
	StaticAssert( sizeof(ui_global_ublock) == 16 );

#ifndef VB_UVf2_Col8_DEFINED
#	define VB_UVf2_Col8_DEFINED
	// size: 12, align: 4
	struct VB_UVf2_Col8
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xf5d3da88u}};

		packed_float2  UV;
		packed_ubyte4  Color;  // normalized
	};
#endif
	StaticAssert( offsetof(VB_UVf2_Col8, UV) == 0 );
	StaticAssert( offsetof(VB_UVf2_Col8, Color) == 8 );
	StaticAssert( sizeof(VB_UVf2_Col8) == 12 );

