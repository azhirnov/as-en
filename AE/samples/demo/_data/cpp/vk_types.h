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

#ifndef VB_UVs2_SCs1_Col8_DEFINED
#	define VB_UVs2_SCs1_Col8_DEFINED
	// size: 12, align: 2
	struct VB_UVs2_SCs1_Col8
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x594166a8u}};

		packed_ushort4  UV_Scale;
		packed_ubyte4  Color;
	};
#endif
	StaticAssert( offsetof(VB_UVs2_SCs1_Col8, UV_Scale) == 0 );
	StaticAssert( offsetof(VB_UVs2_SCs1_Col8, Color) == 8 );
	StaticAssert( sizeof(VB_UVs2_SCs1_Col8) == 12 );

#ifndef VB_UVf2_Col8_DEFINED
#	define VB_UVf2_Col8_DEFINED
	// size: 12, align: 4
	struct VB_UVf2_Col8
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xf5d3da88u}};

		packed_float2  UV;
		packed_ubyte4  Color;
	};
#endif
	StaticAssert( offsetof(VB_UVf2_Col8, UV) == 0 );
	StaticAssert( offsetof(VB_UVf2_Col8, Color) == 8 );
	StaticAssert( sizeof(VB_UVf2_Col8) == 12 );

#ifndef imgui_ub_DEFINED
#	define imgui_ub_DEFINED
	// size: 16, align: 8 (16)
	struct imgui_ub
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xb41e4542u}};

		float2  scale;
		float2  translate;
	};
#endif
	StaticAssert( offsetof(imgui_ub, scale) == 0 );
	StaticAssert( offsetof(imgui_ub, translate) == 8 );
	StaticAssert( sizeof(imgui_ub) == 16 );

#ifndef imgui_vertex_DEFINED
#	define imgui_vertex_DEFINED
	// size: 20, align: 4
	struct imgui_vertex
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x9e6b2802u}};

		packed_float2  Position;
		packed_float2  UV;
		packed_ubyte4  Color;
	};
#endif
	StaticAssert( offsetof(imgui_vertex, Position) == 0 );
	StaticAssert( offsetof(imgui_vertex, UV) == 8 );
	StaticAssert( offsetof(imgui_vertex, Color) == 16 );
	StaticAssert( sizeof(imgui_vertex) == 20 );

#ifndef CubeVertex_DEFINED
#	define CubeVertex_DEFINED
	// size: 40, align: 2
	struct CubeVertex
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x38ec4b6bu}};

		packed_short4  Position;
		packed_short4  Texcoord;
		packed_short4  Normal;
		packed_short4  Tangent;
		packed_short4  BiTangent;
	};
#endif
	StaticAssert( offsetof(CubeVertex, Position) == 0 );
	StaticAssert( offsetof(CubeVertex, Texcoord) == 8 );
	StaticAssert( offsetof(CubeVertex, Normal) == 16 );
	StaticAssert( offsetof(CubeVertex, Tangent) == 24 );
	StaticAssert( offsetof(CubeVertex, BiTangent) == 32 );
	StaticAssert( sizeof(CubeVertex) == 40 );

#ifndef sdf_font_ublock_DEFINED
#	define sdf_font_ublock_DEFINED
	// size: 48, align: 16
	struct sdf_font_ublock
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x5a07d037u}};

		float2  rotation0;
		float2  rotation1;
		float  sdfScale;
		float  sdfBias;
		float  pxRange;
		float4  bgColor;
	};
#endif
	StaticAssert( offsetof(sdf_font_ublock, rotation0) == 0 );
	StaticAssert( offsetof(sdf_font_ublock, rotation1) == 8 );
	StaticAssert( offsetof(sdf_font_ublock, sdfScale) == 16 );
	StaticAssert( offsetof(sdf_font_ublock, sdfBias) == 20 );
	StaticAssert( offsetof(sdf_font_ublock, pxRange) == 24 );
	StaticAssert( offsetof(sdf_font_ublock, bgColor) == 32 );
	StaticAssert( sizeof(sdf_font_ublock) == 48 );

#ifndef camera3d_ub_DEFINED
#	define camera3d_ub_DEFINED
	// size: 64, align: 16
	struct camera3d_ub
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xd53dbd02u}};

		float4x4_storage  mvp;
	};
#endif
	StaticAssert( offsetof(camera3d_ub, mvp) == 0 );
	StaticAssert( sizeof(camera3d_ub) == 64 );

#ifndef SphericalCubeVertex_DEFINED
#	define SphericalCubeVertex_DEFINED
	// size: 32, align: 2
	struct SphericalCubeVertex
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x35a6eeecu}};

		packed_short4  Position;
		packed_short4  Texcoord;
		packed_short4  Tangent;
		packed_short4  BiTangent;
	};
#endif
	StaticAssert( offsetof(SphericalCubeVertex, Position) == 0 );
	StaticAssert( offsetof(SphericalCubeVertex, Texcoord) == 8 );
	StaticAssert( offsetof(SphericalCubeVertex, Tangent) == 16 );
	StaticAssert( offsetof(SphericalCubeVertex, BiTangent) == 24 );
	StaticAssert( sizeof(SphericalCubeVertex) == 32 );

