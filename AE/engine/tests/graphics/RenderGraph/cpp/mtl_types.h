#ifndef Vertex_draw2_DEFINED
#	define Vertex_draw2_DEFINED
	// size: 12, align: 4
	struct Vertex_draw2
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x208fe808u}};  // 'Vertex_draw2'

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
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x83006fc3u}};  // 'PC_draw3'

		StaticArray< float4, 3 >    PositionColorArr;
	};
#endif
	StaticAssert( offsetof(PC_draw3, PositionColorArr) == 0 );
	StaticAssert( sizeof(PC_draw3) == 48 );

