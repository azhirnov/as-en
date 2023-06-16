#ifndef Vertex_draw2_DEFINED
#	define Vertex_draw2_DEFINED
	// size: 12, align: 4 (16)
	struct Vertex_draw2
	{
		static constexpr auto  TypeName = ShaderStructName{"Vertex_draw2"};

		packed_float2  Position;
		packed_ubyte4  Color;
	};
	STATIC_ASSERT( offsetof(Vertex_draw2, Position) == 0 );
	STATIC_ASSERT( offsetof(Vertex_draw2, Color) == 8 );
	STATIC_ASSERT( sizeof(Vertex_draw2) == 12 );
#endif

#ifndef PC_draw3_DEFINED
#	define PC_draw3_DEFINED
	// size: 48, align: 16
	struct PC_draw3
	{
		static constexpr auto  TypeName = ShaderStructName{"PC_draw3"};

		StaticArray< float4, 3 >    PositionColorArr;
	};
	STATIC_ASSERT( offsetof(PC_draw3, PositionColorArr) == 0 );
	STATIC_ASSERT( sizeof(PC_draw3) == 48 );
#endif

