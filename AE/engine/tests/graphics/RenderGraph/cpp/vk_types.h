#ifndef Vertex_draw2_DEFINED
#   define Vertex_draw2_DEFINED
    // size: 12, align: 4 (16)
    struct Vertex_draw2
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x208fe808u}};  // 'Vertex_draw2'

        packed_float2  Position;
        packed_ubyte4  Color;
    };
#endif
    STATIC_ASSERT( offsetof(Vertex_draw2, Position) == 0 );
    STATIC_ASSERT( offsetof(Vertex_draw2, Color) == 8 );
    STATIC_ASSERT( sizeof(Vertex_draw2) == 12 );

#ifndef Vertex_VRS_DEFINED
#   define Vertex_VRS_DEFINED
    // size: 12, align: 4 (16)
    struct Vertex_VRS
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xa70d7fceu}};  // 'Vertex_VRS'

        packed_float2  Position;
        int  ShadingRate;
    };
#endif
    STATIC_ASSERT( offsetof(Vertex_VRS, Position) == 0 );
    STATIC_ASSERT( offsetof(Vertex_VRS, ShadingRate) == 8 );
    STATIC_ASSERT( sizeof(Vertex_VRS) == 12 );

#ifndef PC_draw3_DEFINED
#   define PC_draw3_DEFINED
    // size: 48, align: 16
    struct PC_draw3
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x83006fc3u}};  // 'PC_draw3'

        StaticArray< float4, 3 >    PositionColorArr;
    };
#endif
    STATIC_ASSERT( offsetof(PC_draw3, PositionColorArr) == 0 );
    STATIC_ASSERT( sizeof(PC_draw3) == 48 );

