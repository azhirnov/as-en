#ifndef imgui_vs_pc_DEFINED
#	define imgui_vs_pc_DEFINED
	// size: 24, align: 8 (16)
	struct imgui_vs_pc
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xc83f6c98u}};

		float2  transform_c0;
		float2  transform_c1;
		float2  transform_c2;
	};
#endif
	StaticAssert( offsetof(imgui_vs_pc, transform_c0) == 0 );
	StaticAssert( offsetof(imgui_vs_pc, transform_c1) == 8 );
	StaticAssert( offsetof(imgui_vs_pc, transform_c2) == 16 );
	StaticAssert( sizeof(imgui_vs_pc) == 24 );

#ifndef imgui_fs_pc_DEFINED
#	define imgui_fs_pc_DEFINED
	// size: 4, align: 4 (16)
	struct imgui_fs_pc
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xa8dffb1au}};

		uint  texIndex;
	};
#endif
	StaticAssert( offsetof(imgui_fs_pc, texIndex) == 0 );
	StaticAssert( sizeof(imgui_fs_pc) == 4 );

#ifndef imgui_vertex_DEFINED
#	define imgui_vertex_DEFINED
	// size: 20, align: 4
	struct imgui_vertex
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x9e6b2802u}};

		packed_float2  Position;
		packed_float2  UV;
		packed_ubyte4  Color;     // normalized
	};
#endif
	StaticAssert( offsetof(imgui_vertex, Position) == 0 );
	StaticAssert( offsetof(imgui_vertex, UV) == 8 );
	StaticAssert( offsetof(imgui_vertex, Color) == 16 );
	StaticAssert( sizeof(imgui_vertex) == 20 );

