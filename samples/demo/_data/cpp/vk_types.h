#ifndef VB_UVs2_SCs1_Col8_DEFINED
#	define VB_UVs2_SCs1_Col8_DEFINED
	// size: 12, align: 2 (16)
	struct VB_UVs2_SCs1_Col8
	{
		static constexpr auto  TypeName = ShaderStructName{"VB_UVs2_SCs1_Col8"};

		packed_ushort4  UV_Scale;
		packed_ubyte4  Color;
	};
	STATIC_ASSERT( offsetof(VB_UVs2_SCs1_Col8, UV_Scale) == 0 );
	STATIC_ASSERT( offsetof(VB_UVs2_SCs1_Col8, Color) == 8 );
	STATIC_ASSERT( sizeof(VB_UVs2_SCs1_Col8) == 12 );
#endif

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

#ifndef ui_global_ublock_DEFINED
#	define ui_global_ublock_DEFINED
	// size: 16, align: 8 (16)
	struct ui_global_ublock
	{
		static constexpr auto  TypeName = ShaderStructName{"ui_global_ublock"};

		float2  posScale;
		float2  posBias;
	};
	STATIC_ASSERT( offsetof(ui_global_ublock, posScale) == 0 );
	STATIC_ASSERT( offsetof(ui_global_ublock, posBias) == 8 );
	STATIC_ASSERT( sizeof(ui_global_ublock) == 16 );
#endif

#ifndef ui_material_ublock_DEFINED
#	define ui_material_ublock_DEFINED
	// size: 4, align: 4 (16)
	struct ui_material_ublock
	{
		static constexpr auto  TypeName = ShaderStructName{"ui_material_ublock"};

		uint  id;
	};
	STATIC_ASSERT( offsetof(ui_material_ublock, id) == 0 );
	STATIC_ASSERT( sizeof(ui_material_ublock) == 4 );
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

#ifndef imgui_ub_DEFINED
#	define imgui_ub_DEFINED
	// size: 16, align: 8 (16)
	struct imgui_ub
	{
		static constexpr auto  TypeName = ShaderStructName{"imgui_ub"};

		float2  scale;
		float2  translate;
	};
	STATIC_ASSERT( offsetof(imgui_ub, scale) == 0 );
	STATIC_ASSERT( offsetof(imgui_ub, translate) == 8 );
	STATIC_ASSERT( sizeof(imgui_ub) == 16 );
#endif

#ifndef imgui_vertex_DEFINED
#	define imgui_vertex_DEFINED
	// size: 20, align: 4 (16)
	struct imgui_vertex
	{
		static constexpr auto  TypeName = ShaderStructName{"imgui_vertex"};

		packed_float2  Position;
		packed_float2  UV;
		packed_ubyte4  Color;
	};
	STATIC_ASSERT( offsetof(imgui_vertex, Position) == 0 );
	STATIC_ASSERT( offsetof(imgui_vertex, UV) == 8 );
	STATIC_ASSERT( offsetof(imgui_vertex, Color) == 16 );
	STATIC_ASSERT( sizeof(imgui_vertex) == 20 );
#endif

#ifndef simple3d_ub_DEFINED
#	define simple3d_ub_DEFINED
	// size: 64, align: 16
	struct simple3d_ub
	{
		static constexpr auto  TypeName = ShaderStructName{"simple3d_ub"};

		float4x4_storage  mvp;
	};
	STATIC_ASSERT( offsetof(simple3d_ub, mvp) == 0 );
	STATIC_ASSERT( sizeof(simple3d_ub) == 64 );
#endif

#ifndef CubeVertex_DEFINED
#	define CubeVertex_DEFINED
	// size: 36, align: 4 (16)
	struct CubeVertex
	{
		static constexpr auto  TypeName = ShaderStructName{"CubeVertex"};

		packed_float3  Position;
		packed_float3  Normal;
		packed_float3  Texcoord;
	};
	STATIC_ASSERT( offsetof(CubeVertex, Position) == 0 );
	STATIC_ASSERT( offsetof(CubeVertex, Normal) == 12 );
	STATIC_ASSERT( offsetof(CubeVertex, Texcoord) == 24 );
	STATIC_ASSERT( sizeof(CubeVertex) == 36 );
#endif

#ifndef SphericalCubeVertex_DEFINED
#	define SphericalCubeVertex_DEFINED
	// size: 32, align: 4 (16)
	struct SphericalCubeVertex
	{
		static constexpr auto  TypeName = ShaderStructName{"SphericalCubeVertex"};

		packed_float4  Position;
		packed_float4  Texcoord;
	};
	STATIC_ASSERT( offsetof(SphericalCubeVertex, Position) == 0 );
	STATIC_ASSERT( offsetof(SphericalCubeVertex, Texcoord) == 16 );
	STATIC_ASSERT( sizeof(SphericalCubeVertex) == 32 );
#endif

#ifndef sdf_font_ublock_DEFINED
#	define sdf_font_ublock_DEFINED
	// size: 48, align: 16
	struct sdf_font_ublock
	{
		static constexpr auto  TypeName = ShaderStructName{"sdf_font_ublock"};

		float2  rotation0;
		float2  rotation1;
		float  sdfScale;
		float  screenPxRange;
		float4  bgColor;
	};
	STATIC_ASSERT( offsetof(sdf_font_ublock, rotation0) == 0 );
	STATIC_ASSERT( offsetof(sdf_font_ublock, rotation1) == 8 );
	STATIC_ASSERT( offsetof(sdf_font_ublock, sdfScale) == 16 );
	STATIC_ASSERT( offsetof(sdf_font_ublock, screenPxRange) == 20 );
	STATIC_ASSERT( offsetof(sdf_font_ublock, bgColor) == 32 );
	STATIC_ASSERT( sizeof(sdf_font_ublock) == 48 );
#endif

