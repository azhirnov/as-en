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

