#ifndef VB_Position_f2_DEFINED
#   define VB_Position_f2_DEFINED
    // size: 8, align: 4
    struct VB_Position_f2
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xa843e002u}};  // 'VB_Position_f2'

        packed_float2  Position;
    };
#endif
    StaticAssert( offsetof(VB_Position_f2, Position) == 0 );
    StaticAssert( sizeof(VB_Position_f2) == 8 );

#ifndef VB_Color8_DEFINED
#   define VB_Color8_DEFINED
    // size: 4, align: 1
    struct VB_Color8
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xccba8f8eu}};  // 'VB_Color8'

        packed_ubyte4  Color;
    };
#endif
    StaticAssert( offsetof(VB_Color8, Color) == 0 );
    StaticAssert( sizeof(VB_Color8) == 4 );

#ifndef VB_UVf2_Col8_DEFINED
#   define VB_UVf2_Col8_DEFINED
    // size: 12, align: 4
    struct VB_UVf2_Col8
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xf5d3da88u}};  // 'VB_UVf2_Col8'

        packed_float2  UV;
        packed_ubyte4  Color;
    };
#endif
    StaticAssert( offsetof(VB_UVf2_Col8, UV) == 0 );
    StaticAssert( offsetof(VB_UVf2_Col8, Color) == 8 );
    StaticAssert( sizeof(VB_UVf2_Col8) == 12 );

