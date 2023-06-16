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

#ifndef Histogram_ssb_DEFINED
#	define Histogram_ssb_DEFINED
	// size: 2096, align: 16
	struct Histogram_ssb
	{
		static constexpr auto  TypeName = ShaderStructName{"Histogram_ssb"};

		uint  maxRGB;
		uint  maxLuma;
		StaticArray< uint4, 130 >    RGBLuma;
	};
	STATIC_ASSERT( offsetof(Histogram_ssb, maxRGB) == 0 );
	STATIC_ASSERT( offsetof(Histogram_ssb, maxLuma) == 4 );
	STATIC_ASSERT( offsetof(Histogram_ssb, RGBLuma) == 16 );
	STATIC_ASSERT( sizeof(Histogram_ssb) == 2096 );
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

#ifndef imgui_pc_DEFINED
#	define imgui_pc_DEFINED
	// size: 4, align: 4 (16)
	struct imgui_pc
	{
		static constexpr auto  TypeName = ShaderStructName{"imgui_pc"};

		uint  textureIdx;
	};
	STATIC_ASSERT( offsetof(imgui_pc, textureIdx) == 0 );
	STATIC_ASSERT( sizeof(imgui_pc) == 4 );
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

