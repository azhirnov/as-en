#ifndef VB_UVs2_SCs1_Col8_DEFINED
#   define VB_UVs2_SCs1_Col8_DEFINED
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
#   define VB_Position_f2_DEFINED
    // size: 8, align: 4 (16)
    struct VB_Position_f2
    {
        static constexpr auto  TypeName = ShaderStructName{"VB_Position_f2"};

        packed_float2  Position;
    };
    STATIC_ASSERT( offsetof(VB_Position_f2, Position) == 0 );
    STATIC_ASSERT( sizeof(VB_Position_f2) == 8 );
#endif

#ifndef bloom_map_vertex_attr0_DEFINED
#   define bloom_map_vertex_attr0_DEFINED
    // size: 16, align: 4
    struct bloom_map_vertex_attr0
    {
        static constexpr auto  TypeName = ShaderStructName{"bloom_map_vertex_attr0"};

        packed_float4  Texcoord;
    };
    STATIC_ASSERT( offsetof(bloom_map_vertex_attr0, Texcoord) == 0 );
    STATIC_ASSERT( sizeof(bloom_map_vertex_attr0) == 16 );
#endif

#ifndef ui_global_ublock_DEFINED
#   define ui_global_ublock_DEFINED
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
#   define ui_material_ublock_DEFINED
    // size: 4, align: 4 (16)
    struct ui_material_ublock
    {
        static constexpr auto  TypeName = ShaderStructName{"ui_material_ublock"};

        uint  id;
    };
    STATIC_ASSERT( offsetof(ui_material_ublock, id) == 0 );
    STATIC_ASSERT( sizeof(ui_material_ublock) == 4 );
#endif

#ifndef bloom_ub_DEFINED
#   define bloom_ub_DEFINED
    // size: 40, align: 8 (16)
    struct bloom_ub
    {
        static constexpr auto  TypeName = ShaderStructName{"bloom_ub"};

        float2  resolution;
        float2  invResolution;
        float2  blurDir;
        float  upscale;
        float  invRefLuma;
        float  emissionScale;
        float  bloomScale;
    };
    STATIC_ASSERT( offsetof(bloom_ub, resolution) == 0 );
    STATIC_ASSERT( offsetof(bloom_ub, invResolution) == 8 );
    STATIC_ASSERT( offsetof(bloom_ub, blurDir) == 16 );
    STATIC_ASSERT( offsetof(bloom_ub, upscale) == 24 );
    STATIC_ASSERT( offsetof(bloom_ub, invRefLuma) == 28 );
    STATIC_ASSERT( offsetof(bloom_ub, emissionScale) == 32 );
    STATIC_ASSERT( offsetof(bloom_ub, bloomScale) == 36 );
    STATIC_ASSERT( sizeof(bloom_ub) == 40 );
#endif

#ifndef VB_UVf2_Col8_DEFINED
#   define VB_UVf2_Col8_DEFINED
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

#ifndef bloom_map_ub_DEFINED
#   define bloom_map_ub_DEFINED
    // size: 128, align: 16
    struct bloom_map_ub
    {
        static constexpr auto  TypeName = ShaderStructName{"bloom_map_ub"};

        float4x4_storage  transform;
        float4  lighting;
        float  textureSize;
        float  mapHeightPxRecip;
        float4  playerPos;
        float3  pixelSize_scale;
    };
    STATIC_ASSERT( offsetof(bloom_map_ub, transform) == 0 );
    STATIC_ASSERT( offsetof(bloom_map_ub, lighting) == 64 );
    STATIC_ASSERT( offsetof(bloom_map_ub, textureSize) == 80 );
    STATIC_ASSERT( offsetof(bloom_map_ub, mapHeightPxRecip) == 84 );
    STATIC_ASSERT( offsetof(bloom_map_ub, playerPos) == 96 );
    STATIC_ASSERT( offsetof(bloom_map_ub, pixelSize_scale) == 112 );
    STATIC_ASSERT( sizeof(bloom_map_ub) == 128 );
#endif

#ifndef bloom_entities_vertex_pos_DEFINED
#   define bloom_entities_vertex_pos_DEFINED
    // size: 12, align: 4
    struct bloom_entities_vertex_pos
    {
        static constexpr auto  TypeName = ShaderStructName{"bloom_entities_vertex_pos"};

        packed_float3  Position;
    };
    STATIC_ASSERT( offsetof(bloom_entities_vertex_pos, Position) == 0 );
    STATIC_ASSERT( sizeof(bloom_entities_vertex_pos) == 12 );
#endif

#ifndef bloom_map_vertex_DEFINED
#   define bloom_map_vertex_DEFINED
    // size: 32, align: 4
    struct bloom_map_vertex
    {
        static constexpr auto  TypeName = ShaderStructName{"bloom_map_vertex"};

        packed_float2  Position;
        packed_float4  Texcoord;
        packed_ubyte4  Color;
        packed_ubyte4  Type;
    };
    STATIC_ASSERT( offsetof(bloom_map_vertex, Position) == 0 );
    STATIC_ASSERT( offsetof(bloom_map_vertex, Texcoord) == 8 );
    STATIC_ASSERT( offsetof(bloom_map_vertex, Color) == 24 );
    STATIC_ASSERT( offsetof(bloom_map_vertex, Type) == 28 );
    STATIC_ASSERT( sizeof(bloom_map_vertex) == 32 );
#endif

#ifndef bloom_map_vertex_pos_DEFINED
#   define bloom_map_vertex_pos_DEFINED
    // size: 8, align: 4
    struct bloom_map_vertex_pos
    {
        static constexpr auto  TypeName = ShaderStructName{"bloom_map_vertex_pos"};

        packed_float2  Position;
    };
    STATIC_ASSERT( offsetof(bloom_map_vertex_pos, Position) == 0 );
    STATIC_ASSERT( sizeof(bloom_map_vertex_pos) == 8 );
#endif

#ifndef bloom_map_vertex_attr1_DEFINED
#   define bloom_map_vertex_attr1_DEFINED
    // size: 4, align: 1
    struct bloom_map_vertex_attr1
    {
        static constexpr auto  TypeName = ShaderStructName{"bloom_map_vertex_attr1"};

        packed_ubyte4  Color;
    };
    STATIC_ASSERT( offsetof(bloom_map_vertex_attr1, Color) == 0 );
    STATIC_ASSERT( sizeof(bloom_map_vertex_attr1) == 4 );
#endif

#ifndef bloom_map_vertex_attr2_DEFINED
#   define bloom_map_vertex_attr2_DEFINED
    // size: 4, align: 1
    struct bloom_map_vertex_attr2
    {
        static constexpr auto  TypeName = ShaderStructName{"bloom_map_vertex_attr2"};

        packed_ubyte4  Type;
    };
    STATIC_ASSERT( offsetof(bloom_map_vertex_attr2, Type) == 0 );
    STATIC_ASSERT( sizeof(bloom_map_vertex_attr2) == 4 );
#endif

#ifndef bloom_entities_vertex_DEFINED
#   define bloom_entities_vertex_DEFINED
    // size: 36, align: 4
    struct bloom_entities_vertex
    {
        static constexpr auto  TypeName = ShaderStructName{"bloom_entities_vertex"};

        packed_float3  Position;
        packed_float4  Texcoord;
        packed_ubyte4  Color;
        packed_ubyte4  Type;
    };
    STATIC_ASSERT( offsetof(bloom_entities_vertex, Position) == 0 );
    STATIC_ASSERT( offsetof(bloom_entities_vertex, Texcoord) == 12 );
    STATIC_ASSERT( offsetof(bloom_entities_vertex, Color) == 28 );
    STATIC_ASSERT( offsetof(bloom_entities_vertex, Type) == 32 );
    STATIC_ASSERT( sizeof(bloom_entities_vertex) == 36 );
#endif

#ifndef bloom_entities_vertex_attr0_DEFINED
#   define bloom_entities_vertex_attr0_DEFINED
    // size: 16, align: 4
    struct bloom_entities_vertex_attr0
    {
        static constexpr auto  TypeName = ShaderStructName{"bloom_entities_vertex_attr0"};

        packed_float4  Texcoord;
    };
    STATIC_ASSERT( offsetof(bloom_entities_vertex_attr0, Texcoord) == 0 );
    STATIC_ASSERT( sizeof(bloom_entities_vertex_attr0) == 16 );
#endif

#ifndef bloom_entities_vertex_attr1_DEFINED
#   define bloom_entities_vertex_attr1_DEFINED
    // size: 4, align: 1
    struct bloom_entities_vertex_attr1
    {
        static constexpr auto  TypeName = ShaderStructName{"bloom_entities_vertex_attr1"};

        packed_ubyte4  Color;
    };
    STATIC_ASSERT( offsetof(bloom_entities_vertex_attr1, Color) == 0 );
    STATIC_ASSERT( sizeof(bloom_entities_vertex_attr1) == 4 );
#endif

#ifndef bloom_entities_vertex_attr2_DEFINED
#   define bloom_entities_vertex_attr2_DEFINED
    // size: 4, align: 1
    struct bloom_entities_vertex_attr2
    {
        static constexpr auto  TypeName = ShaderStructName{"bloom_entities_vertex_attr2"};

        packed_ubyte4  Type;
    };
    STATIC_ASSERT( offsetof(bloom_entities_vertex_attr2, Type) == 0 );
    STATIC_ASSERT( sizeof(bloom_entities_vertex_attr2) == 4 );
#endif

#ifndef imgui_ub_DEFINED
#   define imgui_ub_DEFINED
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
#   define imgui_vertex_DEFINED
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

#ifndef CubeVertex_DEFINED
#   define CubeVertex_DEFINED
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

#ifndef sdf_font_ublock_DEFINED
#   define sdf_font_ublock_DEFINED
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

#ifndef simple3d_ub_DEFINED
#   define simple3d_ub_DEFINED
    // size: 64, align: 16
    struct simple3d_ub
    {
        static constexpr auto  TypeName = ShaderStructName{"simple3d_ub"};

        float4x4_storage  mvp;
    };
    STATIC_ASSERT( offsetof(simple3d_ub, mvp) == 0 );
    STATIC_ASSERT( sizeof(simple3d_ub) == 64 );
#endif

#ifndef SphericalCubeVertex_DEFINED
#   define SphericalCubeVertex_DEFINED
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

