#ifndef ModelMaterial_DEFINED
#	define ModelMaterial_DEFINED
	// size: 24, align: 4
	struct ModelMaterial
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x226dd4bau}};  // 'ModelMaterial'

		uint  flags;
		uint  albedoMap;
		uint  normalMap;
		uint  albedoRGBM;
		uint  emissiveRGBM;
		uint  specularRGBM;
	};
#endif
	StaticAssert( offsetof(ModelMaterial, flags) == 0 );
	StaticAssert( offsetof(ModelMaterial, albedoMap) == 4 );
	StaticAssert( offsetof(ModelMaterial, normalMap) == 8 );
	StaticAssert( offsetof(ModelMaterial, albedoRGBM) == 12 );
	StaticAssert( offsetof(ModelMaterial, emissiveRGBM) == 16 );
	StaticAssert( offsetof(ModelMaterial, specularRGBM) == 20 );
	StaticAssert( sizeof(ModelMaterial) == 24 );

#ifndef CubeVertex_DEFINED
#	define CubeVertex_DEFINED
	// size: 40, align: 2
	struct CubeVertex
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x38ec4b6bu}};  // 'CubeVertex'

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

#ifndef SphericalCubeVertex_DEFINED
#	define SphericalCubeVertex_DEFINED
	// size: 32, align: 2
	struct SphericalCubeVertex
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x35a6eeecu}};  // 'SphericalCubeVertex'

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

#ifndef SceneOmniLight_DEFINED
#	define SceneOmniLight_DEFINED
	// size: 36 (48), align: 16
	struct alignas(16) SceneOmniLight
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x6e3bdc7bu}};  // 'SceneOmniLight'

		float3  position;
		float3  attenuation;
		uint  colorRGBM;
	};
#endif
	StaticAssert( offsetof(SceneOmniLight, position) == 0 );
	StaticAssert( offsetof(SceneOmniLight, attenuation) == 16 );
	StaticAssert( offsetof(SceneOmniLight, colorRGBM) == 32 );
	StaticAssert( sizeof(SceneOmniLight) == 48 );

#ifndef ModelNode_DEFINED
#	define ModelNode_DEFINED
	// size: 120 (128), align: 16
	struct alignas(16) ModelNode
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xbf14b6ddu}};  // 'ModelNode'

		float4x4_storage  transform;
		float3x3_storage  normalMat;
		uint  meshIdx;
		uint  materialIdx;
	};
#endif
	StaticAssert( offsetof(ModelNode, transform) == 0 );
	StaticAssert( offsetof(ModelNode, normalMat) == 64 );
	StaticAssert( offsetof(ModelNode, meshIdx) == 112 );
	StaticAssert( offsetof(ModelNode, materialIdx) == 116 );
	StaticAssert( sizeof(ModelNode) == 128 );

#ifndef ModelRTMesh_DEFINED
#	define ModelRTMesh_DEFINED
	// size: 24, align: 8
	struct ModelRTMesh
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x6fe9689cu}};  // 'ModelRTMesh'

		TDeviceAddress< packed_float3 *>  normals;
		TDeviceAddress< float2 *>  texcoords;
		TDeviceAddress< uint *>  indices;
	};
#endif
	StaticAssert( offsetof(ModelRTMesh, normals) == 0 );
	StaticAssert( offsetof(ModelRTMesh, texcoords) == 8 );
	StaticAssert( offsetof(ModelRTMesh, indices) == 16 );
	StaticAssert( sizeof(ModelRTMesh) == 24 );

#ifndef ModelRTInstances_DEFINED
#	define ModelRTInstances_DEFINED
	// size: 96, align: 8
	struct ModelRTInstances
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xf83ee5cdu}};  // 'ModelRTInstances'

		StaticArray< TDeviceAddress< ModelRTMesh >, 4 >    meshesPerInstance;
		StaticArray< TDeviceAddress< uint *>, 4 >    materialsPerInstance;
		StaticArray< TDeviceAddress< float3x3_storage *>, 4 >    normalMatPerInstance;
	};
#endif
	StaticAssert( offsetof(ModelRTInstances, meshesPerInstance) == 0 );
	StaticAssert( offsetof(ModelRTInstances, materialsPerInstance) == 32 );
	StaticAssert( offsetof(ModelRTInstances, normalMatPerInstance) == 64 );
	StaticAssert( sizeof(ModelRTInstances) == 96 );

#ifndef SceneDirectionalLight_DEFINED
#	define SceneDirectionalLight_DEFINED
	// size: 36 (48), align: 16
	struct alignas(16) SceneDirectionalLight
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xbbb7657au}};  // 'SceneDirectionalLight'

		float3  direction;
		float3  attenuation;
		uint  colorRGBM;
	};
#endif
	StaticAssert( offsetof(SceneDirectionalLight, direction) == 0 );
	StaticAssert( offsetof(SceneDirectionalLight, attenuation) == 16 );
	StaticAssert( offsetof(SceneDirectionalLight, colorRGBM) == 32 );
	StaticAssert( sizeof(SceneDirectionalLight) == 48 );

#ifndef SceneConeLight_DEFINED
#	define SceneConeLight_DEFINED
	// size: 60 (64), align: 16
	struct alignas(16) SceneConeLight
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xbde8e869u}};  // 'SceneConeLight'

		float3  position;
		float3  direction;
		float3  attenuation;
		float2  cone;
		uint  colorRGBM;
	};
#endif
	StaticAssert( offsetof(SceneConeLight, position) == 0 );
	StaticAssert( offsetof(SceneConeLight, direction) == 16 );
	StaticAssert( offsetof(SceneConeLight, attenuation) == 32 );
	StaticAssert( offsetof(SceneConeLight, cone) == 48 );
	StaticAssert( offsetof(SceneConeLight, colorRGBM) == 56 );
	StaticAssert( sizeof(SceneConeLight) == 64 );

#ifndef SceneLights_DEFINED
#	define SceneLights_DEFINED
	// size: 1808, align: 16
	struct SceneLights
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x34c2b6e7u}};  // 'SceneLights'

		uint  directionalCount;
		uint  coneCount;
		uint  omniCount;
		StaticArray< SceneDirectionalLight, 8 >    directional;
		StaticArray< SceneConeLight, 16 >    cone;
		StaticArray< SceneOmniLight, 8 >    omni;
	};
#endif
	StaticAssert( offsetof(SceneLights, directionalCount) == 0 );
	StaticAssert( offsetof(SceneLights, coneCount) == 4 );
	StaticAssert( offsetof(SceneLights, omniCount) == 8 );
	StaticAssert( offsetof(SceneLights, directional) == 16 );
	StaticAssert( offsetof(SceneLights, cone) == 400 );
	StaticAssert( offsetof(SceneLights, omni) == 1424 );
	StaticAssert( sizeof(SceneLights) == 1808 );

#ifndef Histogram_ssb_DEFINED
#	define Histogram_ssb_DEFINED
	// size: 2096, align: 16
	struct Histogram_ssb
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x271de9a7u}};  // 'Histogram_ssb'

		uint  maxRGB;
		uint  maxLuma;
		StaticArray< uint4, 130 >    RGBLuma;
	};
#endif
	StaticAssert( offsetof(Histogram_ssb, maxRGB) == 0 );
	StaticAssert( offsetof(Histogram_ssb, maxLuma) == 4 );
	StaticAssert( offsetof(Histogram_ssb, RGBLuma) == 16 );
	StaticAssert( sizeof(Histogram_ssb) == 2096 );

#ifndef imgui_ub_DEFINED
#	define imgui_ub_DEFINED
	// size: 16, align: 8 (16)
	struct imgui_ub
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xb41e4542u}};  // 'imgui_ub'

		float2  scale;
		float2  translate;
	};
#endif
	StaticAssert( offsetof(imgui_ub, scale) == 0 );
	StaticAssert( offsetof(imgui_ub, translate) == 8 );
	StaticAssert( sizeof(imgui_ub) == 16 );

#ifndef imgui_pc_DEFINED
#	define imgui_pc_DEFINED
	// size: 4, align: 4 (16)
	struct imgui_pc
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xbe6e8191u}};  // 'imgui_pc'

		uint  textureIdx;
	};
#endif
	StaticAssert( offsetof(imgui_pc, textureIdx) == 0 );
	StaticAssert( sizeof(imgui_pc) == 4 );

#ifndef imgui_vertex_DEFINED
#	define imgui_vertex_DEFINED
	// size: 20, align: 4
	struct imgui_vertex
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x9e6b2802u}};  // 'imgui_vertex'

		packed_float2  Position;
		packed_float2  UV;
		packed_ubyte4  Color;
	};
#endif
	StaticAssert( offsetof(imgui_vertex, Position) == 0 );
	StaticAssert( offsetof(imgui_vertex, UV) == 8 );
	StaticAssert( offsetof(imgui_vertex, Color) == 16 );
	StaticAssert( sizeof(imgui_vertex) == 20 );

#ifndef LinearDepth_draw_pc_DEFINED
#	define LinearDepth_draw_pc_DEFINED
	// size: 8, align: 8 (16)
	struct LinearDepth_draw_pc
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xb92984e8u}};  // 'LinearDepth_draw_pc'

		float2  clipPlanes;
	};
#endif
	StaticAssert( offsetof(LinearDepth_draw_pc, clipPlanes) == 0 );
	StaticAssert( sizeof(LinearDepth_draw_pc) == 8 );

