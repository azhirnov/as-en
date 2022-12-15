#ifndef VB_Position_f2_DEFINED
#	define VB_Position_f2_DEFINED
	// size: 8, align: 4 (16)
	struct VB_Position_f2
	{
		static constexpr auto  TypeName = ShaderStructName{"VB_Position_f2"};

		packed_float2  Position;
	};
	STATIC_ASSERT( offsetof(VB_Position_f2, Position) == 0 );
	STATIC_ASSERT( sizeof(VB_Position_f2) == 8 );
#endif

#ifndef VB_Color8_DEFINED
#	define VB_Color8_DEFINED
	// size: 4, align: 1 (16)
	struct VB_Color8
	{
		static constexpr auto  TypeName = ShaderStructName{"VB_Color8"};

		packed_ubyte4  Color;
	};
	STATIC_ASSERT( offsetof(VB_Color8, Color) == 0 );
	STATIC_ASSERT( sizeof(VB_Color8) == 4 );
#endif

#ifndef VB_UVf2_Col8_DEFINED
#	define VB_UVf2_Col8_DEFINED
	// size: 12, align: 4 (16)
	struct VB_UVf2_Col8
	{
		static constexpr auto  TypeName = ShaderStructName{"VB_UVf2_Col8"};

		packed_float2  UV;
		packed_ubyte4  Color;
	};
	STATIC_ASSERT( offsetof(VB_UVf2_Col8, UV) == 0 );
	STATIC_ASSERT( offsetof(VB_UVf2_Col8, Color) == 8 );
	STATIC_ASSERT( sizeof(VB_UVf2_Col8) == 12 );
#endif

