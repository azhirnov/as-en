#ifndef ModelMaterial_DEFINED
#	define ModelMaterial_DEFINED
	// size: 4, align: 4
	struct ModelMaterial
	{
		static constexpr auto  TypeName = ShaderStructName{"ModelMaterial"};

		uint  albedoMap;
	};
	STATIC_ASSERT( offsetof(ModelMaterial, albedoMap) == 0 );
	STATIC_ASSERT( sizeof(ModelMaterial) == 4 );
#endif

#ifndef CubeVertex_DEFINED
#	define CubeVertex_DEFINED
	// size: 40, align: 2 (16)
	struct CubeVertex
	{
		static constexpr auto  TypeName = ShaderStructName{"CubeVertex"};

		packed_short4  Position;
		packed_short4  Texcoord;
		packed_short4  Normal;
		packed_short4  Tangent;
		packed_short4  BiTangent;
	};
	STATIC_ASSERT( offsetof(CubeVertex, Position) == 0 );
	STATIC_ASSERT( offsetof(CubeVertex, Texcoord) == 8 );
	STATIC_ASSERT( offsetof(CubeVertex, Normal) == 16 );
	STATIC_ASSERT( offsetof(CubeVertex, Tangent) == 24 );
	STATIC_ASSERT( offsetof(CubeVertex, BiTangent) == 32 );
	STATIC_ASSERT( sizeof(CubeVertex) == 40 );
#endif

#ifndef SphericalCubeVertex_DEFINED
#	define SphericalCubeVertex_DEFINED
	// size: 32, align: 2 (16)
	struct SphericalCubeVertex
	{
		static constexpr auto  TypeName = ShaderStructName{"SphericalCubeVertex"};

		packed_short4  Position;
		packed_short4  Texcoord;
		packed_short4  Tangent;
		packed_short4  BiTangent;
	};
	STATIC_ASSERT( offsetof(SphericalCubeVertex, Position) == 0 );
	STATIC_ASSERT( offsetof(SphericalCubeVertex, Texcoord) == 8 );
	STATIC_ASSERT( offsetof(SphericalCubeVertex, Tangent) == 16 );
	STATIC_ASSERT( offsetof(SphericalCubeVertex, BiTangent) == 24 );
	STATIC_ASSERT( sizeof(SphericalCubeVertex) == 32 );
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

#ifndef model_pc_DEFINED
#	define model_pc_DEFINED
	// size: 4, align: 4 (16)
	struct model_pc
	{
		static constexpr auto  TypeName = ShaderStructName{"model_pc"};

		uint  nodeIdx;
	};
	STATIC_ASSERT( offsetof(model_pc, nodeIdx) == 0 );
	STATIC_ASSERT( sizeof(model_pc) == 4 );
#endif

#ifndef ModelNode_DEFINED
#	define ModelNode_DEFINED
	// size: 120 (128), align: 16
	struct ModelNode
	{
		static constexpr auto  TypeName = ShaderStructName{"ModelNode"};

		float4x4_storage  transform;
		float3x3_storage  normalMat;
		uint  meshIdx;
		uint  materialIdx;
	};
	STATIC_ASSERT( offsetof(ModelNode, transform) == 0 );
	STATIC_ASSERT( offsetof(ModelNode, normalMat) == 64 );
	STATIC_ASSERT( offsetof(ModelNode, meshIdx) == 112 );
	STATIC_ASSERT( offsetof(ModelNode, materialIdx) == 116 );
	STATIC_ASSERT( sizeof(ModelNode) == 128 );
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

#ifndef LinearDepth_draw_pc_DEFINED
#	define LinearDepth_draw_pc_DEFINED
	// size: 8, align: 8 (16)
	struct LinearDepth_draw_pc
	{
		static constexpr auto  TypeName = ShaderStructName{"LinearDepth_draw_pc"};

		float2  clipPlanes;
	};
	STATIC_ASSERT( offsetof(LinearDepth_draw_pc, clipPlanes) == 0 );
	STATIC_ASSERT( sizeof(LinearDepth_draw_pc) == 8 );
#endif

