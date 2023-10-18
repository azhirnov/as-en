#ifndef VB_3_Pos_DEFINED
#   define VB_3_Pos_DEFINED
    // size: 12, align: 4 (16)
    struct VB_3_Pos
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xea1d5fa6u}};  // 'VB_3_Pos'

        packed_float3  Position;
    };
#endif
    STATIC_ASSERT( offsetof(VB_3_Pos, Position) == 0 );
    STATIC_ASSERT( sizeof(VB_3_Pos) == 12 );

#ifndef PushConst1_DEFINED
#   define PushConst1_DEFINED
    // size: 16, align: 8 (16)
    struct PushConst1
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x7f418efau}};  // 'PushConst1'

        float2  scale;
        float2  bias;
    };
#endif
    STATIC_ASSERT( offsetof(PushConst1, scale) == 0 );
    STATIC_ASSERT( offsetof(PushConst1, bias) == 8 );
    STATIC_ASSERT( sizeof(PushConst1) == 16 );

#ifndef PushConst2_DEFINED
#   define PushConst2_DEFINED
    // size: 16, align: 16
    struct PushConst2
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xe648df40u}};  // 'PushConst2'

        float4  color;
    };
#endif
    STATIC_ASSERT( offsetof(PushConst2, color) == 0 );
    STATIC_ASSERT( sizeof(PushConst2) == 16 );

#ifndef UBlock_DEFINED
#   define UBlock_DEFINED
    // size: 64, align: 16
    struct UBlock
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x697977dcu}};  // 'UBlock'

        float4x4_storage  mvp;
    };
#endif
    STATIC_ASSERT( offsetof(UBlock, mvp) == 0 );
    STATIC_ASSERT( sizeof(UBlock) == 64 );

#ifndef BufferRef_DEFINED
#   define BufferRef_DEFINED
    // size: 24 (32), align: 16
    struct alignas(16) BufferRef
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x87910438u}};  // 'BufferRef'

        float3  Position;
        float2  Texcoord;
    };
#endif
    STATIC_ASSERT( offsetof(BufferRef, Position) == 0 );
    STATIC_ASSERT( offsetof(BufferRef, Texcoord) == 16 );
    STATIC_ASSERT( sizeof(BufferRef) == 32 );

#ifndef vb_input1_DEFINED
#   define vb_input1_DEFINED
    // size: 16, align: 4 (16)
    struct vb_input1
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x892b138u}};  // 'vb_input1'

        packed_float3  Position;
        packed_ushort2  Texcoord;
    };
#endif
    STATIC_ASSERT( offsetof(vb_input1, Position) == 0 );
    STATIC_ASSERT( offsetof(vb_input1, Texcoord) == 12 );
    STATIC_ASSERT( sizeof(vb_input1) == 16 );

#ifndef VB_3_Attribs_DEFINED
#   define VB_3_Attribs_DEFINED
    // size: 8, align: 4 (16)
    struct VB_3_Attribs
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x93789f1bu}};  // 'VB_3_Attribs'

        packed_float2  Texcoord;
    };
#endif
    STATIC_ASSERT( offsetof(VB_3_Attribs, Texcoord) == 0 );
    STATIC_ASSERT( sizeof(VB_3_Attribs) == 8 );

