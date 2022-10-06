#ifndef VB_Position_f2_DEFINED
#	define VB_Position_f2_DEFINED
	// size: 8, align: 16
	struct VB_Position_f2
	{
		static constexpr auto  TypeName = ShaderStructName{"VB_Position_f2"};

		packed_float2  Position;
	};
	STATIC_ASSERT( offsetof(VB_Position_f2, Position) == 0 );
	STATIC_ASSERT( sizeof(VB_Position_f2) == 8 );
#endif

#ifndef VB_Position_s2_DEFINED
#	define VB_Position_s2_DEFINED
	// size: 4, align: 16
	struct VB_Position_s2
	{
		static constexpr auto  TypeName = ShaderStructName{"VB_Position_s2"};

		packed_short2  Position;
	};
	STATIC_ASSERT( offsetof(VB_Position_s2, Position) == 0 );
	STATIC_ASSERT( sizeof(VB_Position_s2) == 4 );
#endif

#ifndef VB_Position_f3_DEFINED
#	define VB_Position_f3_DEFINED
	// size: 12, align: 16
	struct VB_Position_f3
	{
		static constexpr auto  TypeName = ShaderStructName{"VB_Position_f3"};

		packed_float3  Position;
	};
	STATIC_ASSERT( offsetof(VB_Position_f3, Position) == 0 );
	STATIC_ASSERT( sizeof(VB_Position_f3) == 12 );
#endif

#ifndef VB_UVf2_Col_DEFINED
#	define VB_UVf2_Col_DEFINED
	// size: 12, align: 16
	struct VB_UVf2_Col
	{
		static constexpr auto  TypeName = ShaderStructName{"VB_UVf2_Col"};

		packed_float2  UV;
		packed_ubyte4  Color;
	};
	STATIC_ASSERT( offsetof(VB_UVf2_Col, UV) == 0 );
	STATIC_ASSERT( offsetof(VB_UVf2_Col, Color) == 8 );
	STATIC_ASSERT( sizeof(VB_UVf2_Col) == 12 );
#endif

#ifndef VB_Position_f4_DEFINED
#	define VB_Position_f4_DEFINED
	// size: 16, align: 16
	struct VB_Position_f4
	{
		static constexpr auto  TypeName = ShaderStructName{"VB_Position_f4"};

		packed_float4  Position;
	};
	STATIC_ASSERT( offsetof(VB_Position_f4, Position) == 0 );
	STATIC_ASSERT( sizeof(VB_Position_f4) == 16 );
#endif

#ifndef VB_Color_DEFINED
#	define VB_Color_DEFINED
	// size: 4, align: 16
	struct VB_Color
	{
		static constexpr auto  TypeName = ShaderStructName{"VB_Color"};

		packed_ubyte4  Color;
	};
	STATIC_ASSERT( offsetof(VB_Color, Color) == 0 );
	STATIC_ASSERT( sizeof(VB_Color) == 4 );
#endif

