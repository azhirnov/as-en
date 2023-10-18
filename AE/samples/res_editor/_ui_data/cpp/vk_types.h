#ifndef ModelMaterial_DEFINED
#   define ModelMaterial_DEFINED
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
    STATIC_ASSERT( offsetof(ModelMaterial, flags) == 0 );
    STATIC_ASSERT( offsetof(ModelMaterial, albedoMap) == 4 );
    STATIC_ASSERT( offsetof(ModelMaterial, normalMap) == 8 );
    STATIC_ASSERT( offsetof(ModelMaterial, albedoRGBM) == 12 );
    STATIC_ASSERT( offsetof(ModelMaterial, emissiveRGBM) == 16 );
    STATIC_ASSERT( offsetof(ModelMaterial, specularRGBM) == 20 );
    STATIC_ASSERT( sizeof(ModelMaterial) == 24 );

#ifndef CubeVertex_DEFINED
#   define CubeVertex_DEFINED
    // size: 40, align: 2 (16)
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
    STATIC_ASSERT( offsetof(CubeVertex, Position) == 0 );
    STATIC_ASSERT( offsetof(CubeVertex, Texcoord) == 8 );
    STATIC_ASSERT( offsetof(CubeVertex, Normal) == 16 );
    STATIC_ASSERT( offsetof(CubeVertex, Tangent) == 24 );
    STATIC_ASSERT( offsetof(CubeVertex, BiTangent) == 32 );
    STATIC_ASSERT( sizeof(CubeVertex) == 40 );

#ifndef SphericalCubeVertex_DEFINED
#   define SphericalCubeVertex_DEFINED
    // size: 32, align: 2 (16)
    struct SphericalCubeVertex
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x35a6eeecu}};  // 'SphericalCubeVertex'

        packed_short4  Position;
        packed_short4  Texcoord;
        packed_short4  Tangent;
        packed_short4  BiTangent;
    };
#endif
    STATIC_ASSERT( offsetof(SphericalCubeVertex, Position) == 0 );
    STATIC_ASSERT( offsetof(SphericalCubeVertex, Texcoord) == 8 );
    STATIC_ASSERT( offsetof(SphericalCubeVertex, Tangent) == 16 );
    STATIC_ASSERT( offsetof(SphericalCubeVertex, BiTangent) == 24 );
    STATIC_ASSERT( sizeof(SphericalCubeVertex) == 32 );

#ifndef SceneOmniLight_DEFINED
#   define SceneOmniLight_DEFINED
    // size: 36 (48), align: 16
    struct alignas(16) SceneOmniLight
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x6e3bdc7bu}};  // 'SceneOmniLight'

        float3  position;
        float3  attenuation;
        uint  colorRGBM;
    };
#endif
    STATIC_ASSERT( offsetof(SceneOmniLight, position) == 0 );
    STATIC_ASSERT( offsetof(SceneOmniLight, attenuation) == 16 );
    STATIC_ASSERT( offsetof(SceneOmniLight, colorRGBM) == 32 );
    STATIC_ASSERT( sizeof(SceneOmniLight) == 48 );

#ifndef ModelNode_DEFINED
#   define ModelNode_DEFINED
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
    STATIC_ASSERT( offsetof(ModelNode, transform) == 0 );
    STATIC_ASSERT( offsetof(ModelNode, normalMat) == 64 );
    STATIC_ASSERT( offsetof(ModelNode, meshIdx) == 112 );
    STATIC_ASSERT( offsetof(ModelNode, materialIdx) == 116 );
    STATIC_ASSERT( sizeof(ModelNode) == 128 );

#ifndef ModelRTMesh_DEFINED
#   define ModelRTMesh_DEFINED
    // size: 24, align: 8
    struct ModelRTMesh
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x6fe9689cu}};  // 'ModelRTMesh'

        TDeviceAddress< packed_float3 *>  normals;
        TDeviceAddress< float2 *>  texcoords;
        TDeviceAddress< uint *>  indices;
    };
#endif
    STATIC_ASSERT( offsetof(ModelRTMesh, normals) == 0 );
    STATIC_ASSERT( offsetof(ModelRTMesh, texcoords) == 8 );
    STATIC_ASSERT( offsetof(ModelRTMesh, indices) == 16 );
    STATIC_ASSERT( sizeof(ModelRTMesh) == 24 );

#ifndef ModelRTInstances_DEFINED
#   define ModelRTInstances_DEFINED
    // size: 96, align: 8
    struct ModelRTInstances
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xf83ee5cdu}};  // 'ModelRTInstances'

        StaticArray< TDeviceAddress< ModelRTMesh >, 4 >    meshesPerInstance;
        StaticArray< TDeviceAddress< uint *>, 4 >    materialsPerInstance;
        StaticArray< TDeviceAddress< float3x3_storage *>, 4 >    normalMatPerInstance;
    };
#endif
    STATIC_ASSERT( offsetof(ModelRTInstances, meshesPerInstance) == 0 );
    STATIC_ASSERT( offsetof(ModelRTInstances, materialsPerInstance) == 32 );
    STATIC_ASSERT( offsetof(ModelRTInstances, normalMatPerInstance) == 64 );
    STATIC_ASSERT( sizeof(ModelRTInstances) == 96 );

#ifndef SceneDirectionalLight_DEFINED
#   define SceneDirectionalLight_DEFINED
    // size: 36 (48), align: 16
    struct alignas(16) SceneDirectionalLight
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xbbb7657au}};  // 'SceneDirectionalLight'

        float3  direction;
        float3  attenuation;
        uint  colorRGBM;
    };
#endif
    STATIC_ASSERT( offsetof(SceneDirectionalLight, direction) == 0 );
    STATIC_ASSERT( offsetof(SceneDirectionalLight, attenuation) == 16 );
    STATIC_ASSERT( offsetof(SceneDirectionalLight, colorRGBM) == 32 );
    STATIC_ASSERT( sizeof(SceneDirectionalLight) == 48 );

#ifndef SceneConeLight_DEFINED
#   define SceneConeLight_DEFINED
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
    STATIC_ASSERT( offsetof(SceneConeLight, position) == 0 );
    STATIC_ASSERT( offsetof(SceneConeLight, direction) == 16 );
    STATIC_ASSERT( offsetof(SceneConeLight, attenuation) == 32 );
    STATIC_ASSERT( offsetof(SceneConeLight, cone) == 48 );
    STATIC_ASSERT( offsetof(SceneConeLight, colorRGBM) == 56 );
    STATIC_ASSERT( sizeof(SceneConeLight) == 64 );

#ifndef SceneLights_DEFINED
#   define SceneLights_DEFINED
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
    STATIC_ASSERT( offsetof(SceneLights, directionalCount) == 0 );
    STATIC_ASSERT( offsetof(SceneLights, coneCount) == 4 );
    STATIC_ASSERT( offsetof(SceneLights, omniCount) == 8 );
    STATIC_ASSERT( offsetof(SceneLights, directional) == 16 );
    STATIC_ASSERT( offsetof(SceneLights, cone) == 400 );
    STATIC_ASSERT( offsetof(SceneLights, omni) == 1424 );
    STATIC_ASSERT( sizeof(SceneLights) == 1808 );

#ifndef Histogram_ssb_DEFINED
#   define Histogram_ssb_DEFINED
    // size: 2096, align: 16
    struct Histogram_ssb
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x271de9a7u}};  // 'Histogram_ssb'

        uint  maxRGB;
        uint  maxLuma;
        StaticArray< uint4, 130 >    RGBLuma;
    };
#endif
    STATIC_ASSERT( offsetof(Histogram_ssb, maxRGB) == 0 );
    STATIC_ASSERT( offsetof(Histogram_ssb, maxLuma) == 4 );
    STATIC_ASSERT( offsetof(Histogram_ssb, RGBLuma) == 16 );
    STATIC_ASSERT( sizeof(Histogram_ssb) == 2096 );

#ifndef imgui_ub_DEFINED
#   define imgui_ub_DEFINED
    // size: 16, align: 8 (16)
    struct imgui_ub
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xb41e4542u}};  // 'imgui_ub'

        float2  scale;
        float2  translate;
    };
#endif
    STATIC_ASSERT( offsetof(imgui_ub, scale) == 0 );
    STATIC_ASSERT( offsetof(imgui_ub, translate) == 8 );
    STATIC_ASSERT( sizeof(imgui_ub) == 16 );

#ifndef imgui_pc_DEFINED
#   define imgui_pc_DEFINED
    // size: 4, align: 4 (16)
    struct imgui_pc
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xbe6e8191u}};  // 'imgui_pc'

        uint  textureIdx;
    };
#endif
    STATIC_ASSERT( offsetof(imgui_pc, textureIdx) == 0 );
    STATIC_ASSERT( sizeof(imgui_pc) == 4 );

#ifndef imgui_vertex_DEFINED
#   define imgui_vertex_DEFINED
    // size: 20, align: 4 (16)
    struct imgui_vertex
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x9e6b2802u}};  // 'imgui_vertex'

        packed_float2  Position;
        packed_float2  UV;
        packed_ubyte4  Color;
    };
#endif
    STATIC_ASSERT( offsetof(imgui_vertex, Position) == 0 );
    STATIC_ASSERT( offsetof(imgui_vertex, UV) == 8 );
    STATIC_ASSERT( offsetof(imgui_vertex, Color) == 16 );
    STATIC_ASSERT( sizeof(imgui_vertex) == 20 );

#ifndef LinearDepth_draw_pc_DEFINED
#   define LinearDepth_draw_pc_DEFINED
    // size: 8, align: 8 (16)
    struct LinearDepth_draw_pc
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xb92984e8u}};  // 'LinearDepth_draw_pc'

        float2  clipPlanes;
    };
#endif
    STATIC_ASSERT( offsetof(LinearDepth_draw_pc, clipPlanes) == 0 );
    STATIC_ASSERT( sizeof(LinearDepth_draw_pc) == 8 );

