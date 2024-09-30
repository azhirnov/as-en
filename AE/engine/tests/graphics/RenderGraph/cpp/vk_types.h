#ifndef Vertex_draw2_DEFINED
#	define Vertex_draw2_DEFINED
	// size: 12, align: 4
	struct Vertex_draw2
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x208fe808u}};

		packed_float2  Position;
		packed_ubyte4  Color;
	};
#endif
	StaticAssert( offsetof(Vertex_draw2, Position) == 0 );
	StaticAssert( offsetof(Vertex_draw2, Color) == 8 );
	StaticAssert( sizeof(Vertex_draw2) == 12 );

#ifndef PC_draw3_DEFINED
#	define PC_draw3_DEFINED
	// size: 48, align: 16
	struct PC_draw3
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x83006fc3u}};

		StaticArray< float4, 3 >    PositionColorArr;
	};
#endif
	StaticAssert( offsetof(PC_draw3, PositionColorArr) == 0 );
	StaticAssert( sizeof(PC_draw3) == 48 );

#ifndef Vertex_VRS_DEFINED
#	define Vertex_VRS_DEFINED
	// size: 12, align: 4 (16)
	struct Vertex_VRS
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xa70d7fceu}};

		packed_float2  Position;
		int  ShadingRate;
	};
#endif
	StaticAssert( offsetof(Vertex_VRS, Position) == 0 );
	StaticAssert( offsetof(Vertex_VRS, ShadingRate) == 8 );
	StaticAssert( sizeof(Vertex_VRS) == 12 );

