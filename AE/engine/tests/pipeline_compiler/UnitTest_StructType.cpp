// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  StructType_Test1_Layout (EStructLayout layout, uint arraySize = 1)
    {
        // vector test
        struct TypeInfo
        {
            StringView  name;
            uint        size;
            uint        align;
        };
        const TypeInfo  types[] = {
            {"char2",   2,  2 },    {"packed_char2",    2,  1 },    {"char_norm2",  2,  2 },    {"packed_char_norm2",   2,  1 },
            {"uchar3",  4,  4 },    {"packed_uchar3",   3,  1 },    {"uchar_norm3", 4,  4 },    {"packed_uchar_norm3",  3,  1 },
            {"char4",   4,  4 },    {"packed_char4",    4,  1 },    {"char_norm4",  4,  4 },    {"packed_char_norm4",   4,  1 },
            {"short2",  4,  4 },    {"packed_short2",   4,  2 },    {"short_norm2", 4,  4 },    {"packed_short_norm2",  4,  2 },
            {"ushort3", 8,  8 },    {"packed_ushort3",  6,  2 },    {"ushort_norm3",8,  8 },    {"packed_ushort_norm3", 6,  2 },
            {"short4",  8,  8 },    {"packed_short4",   8,  2 },    {"short_norm4", 8,  8 },    {"packed_short_norm4",  8,  2 },
            {"uint2",   8,  8 },    {"packed_uint2",    8,  4 },
            {"int3",    16, 16},    {"packed_int3",     12, 4 },
            {"uint4",   16, 16},    {"packed_uint4",    16, 4 },
            {"long2",   16, 16},    {"packed_long2",    16, 8 },
            {"ulong3",  32, 32},    {"packed_ulong3",   24, 8 },
            {"long4",   32, 32},    {"packed_long4",    32, 8 },
            {"half2",   4,  4 },    {"packed_half2",    4,  2 },
            {"half3",   8,  8 },    {"packed_half3",    6,  2 },
            {"half4",   8,  8 },    {"packed_half4",    8,  2 },
            {"float2",  8,  8 },    {"packed_float2",   8,  4 },
            {"float3",  16, 16},    {"packed_float3",   12, 4 },
            {"float4",  16, 16},    {"packed_float4",   16, 4 },
            {"double2", 16, 16},    {"packed_double2",  16, 8 },
            {"double3", 32, 32},    {"packed_double3",  24, 8 },
            {"double4", 32, 32},    {"packed_double4",  32, 8 }
        };

        static uint idx = 0;
        for (auto& t : types)
        {
            if ( AnyEqual( layout, EStructLayout::Metal, EStructLayout::Compatible_Std430, EStructLayout::Compatible_Std140 ) and HasSubString( t.name, "double" ))
                continue;

            if ( AnyEqual( layout, EStructLayout::Std140, EStructLayout::Compatible_Std140 ) and StartsWith( t.name, "packed_" ))
                continue;

            ShaderStructTypePtr st{ new ShaderStructType{ "VecTest"s << ToString(idx++) }};

            String  str = String{t.name} << " val";
            if ( arraySize > 1 )
                str << '[' << ToString( arraySize ) << ']';
            str << ';';

            st->Set( layout, str );

            TEST_Eq( st->StaticSize(), Bytes{t.size} * arraySize );

            if ( AnyEqual( layout, EStructLayout::Compatible_Std140, EStructLayout::Std140 ))
            {
                TEST_Eq( st->Align(), AlignUp( Bytes{t.align}, 16_b ));
            }else{
                TEST_Eq( st->Align(), Bytes{t.align} );
            }
        }
    }

    static void  StructType_Test1 ()
    {
        StructType_Test1_Layout( EStructLayout::Metal );
        StructType_Test1_Layout( EStructLayout::Compatible_Std430 );
        StructType_Test1_Layout( EStructLayout::Std430 );
        StructType_Test1_Layout( EStructLayout::Compatible_Std140 );
        StructType_Test1_Layout( EStructLayout::Std140 );
    }
//-----------------------------------------------------------------------------


    static void  StructType_Test2_Layout (EStructLayout layout)
    {
        // matrix test
        struct TypeInfo
        {
            StringView  name;
            uint        size;
            uint        align;
        };
        const TypeInfo  types[] = {
            {"half2x2",     8,   4 },   {"packed_half2x2",      8,   2},
            {"half2x3",     16,  8 },   {"packed_half2x3",      12,  2},
            {"half2x4",     16,  8 },   {"packed_half2x4",      16,  2},
            {"half3x2",     12,  4 },   {"packed_half3x2",      12,  2},
            {"half3x3",     24,  8 },   {"packed_half3x3",      18,  2},
            {"half3x4",     24,  8 },   {"packed_half3x4",      24,  2},
            {"half4x2",     16,  4 },   {"packed_half4x2",      16,  2},
            {"half4x3",     32,  8 },   {"packed_half4x3",      24,  2},
            {"half4x4",     32,  8 },   {"packed_half4x4",      32,  2},
            {"float2x2",    16,  8 },   {"packed_float2x2",     16,  4},
            {"float2x3",    32,  16},   {"packed_float2x3",     24,  4},
            {"float2x4",    32,  16},   {"packed_float2x4",     32,  4},
            {"float3x2",    24,  8 },   {"packed_float3x2",     24,  4},
            {"float3x3",    48,  16},   {"packed_float3x3",     36,  4},
            {"float3x4",    48,  16},   {"packed_float3x4",     48,  4},
            {"float4x2",    32,  8 },   {"packed_float4x2",     32,  4},
            {"float4x3",    64,  16},   {"packed_float4x3",     48,  4},
            {"float4x4",    64,  16},   {"packed_float4x4",     64,  4},
            {"double2x2",   32,  16},   {"packed_double2x2",    32,  8},
            {"double2x3",   64,  32},   {"packed_double2x3",    48,  8},
            {"double2x4",   64,  32},   {"packed_double2x4",    64,  8},
            {"double3x2",   48,  16},   {"packed_double3x2",    48,  8},
            {"double3x3",   96,  32},   {"packed_double3x3",    72,  8},
            {"double3x4",   96,  32},   {"packed_double3x4",    96,  8},
            {"double4x2",   64,  16},   {"packed_double4x2",    64,  8},
            {"double4x3",   128, 32},   {"packed_double4x3",    96,  8},
            {"double4x4",   128, 32},   {"packed_double4x4",    128, 8}
        };

        static uint idx = 0;
        for (auto& t : types)
        {
            if ( AnyEqual( layout, EStructLayout::Metal, EStructLayout::Compatible_Std430 ) and HasSubString( t.name, "double" ))
                continue;

            ShaderStructTypePtr st{ new ShaderStructType{ "MatTest"s << ToString(idx++) }};
            st->Set( layout, String{t.name} << " val;" );

            TEST_Eq( st->StaticSize(), Bytes{t.size} );
            TEST_Eq( st->Align(), Bytes{t.align} );
        }
    }


    static void  StructType_Test2 ()
    {
        StructType_Test2_Layout( EStructLayout::Metal );
        StructType_Test2_Layout( EStructLayout::Compatible_Std430 );
        StructType_Test2_Layout( EStructLayout::Std430 );
    }


    static void  StructType_Test3 ()
    {
        StructType_Test1_Layout( EStructLayout::Metal,              8 );
        StructType_Test1_Layout( EStructLayout::Compatible_Std430,  8 );
        StructType_Test1_Layout( EStructLayout::Std430,             8 );
    }
//-----------------------------------------------------------------------------


    static void  StructType_Test4 ()
    {
        ShaderStructTypePtr st1{ new ShaderStructType{ "StType1" }};
        st1->Set( EStructLayout::Compatible_Std140,
                 "float4    ff;"
                 "uint2     uu;"
                 "int2      ii;" );

        TEST( not st1->HasDynamicArray() );
        TEST_Eq( st1->ArrayStride(),    0_b );
        TEST_Eq( st1->StaticSize(),     16_b + 16_b );
        TEST_Eq( st1->Align(),          16_b );


        ShaderStructTypePtr st2{ new ShaderStructType{ "StType2" }};
        st2->Set( EStructLayout::Compatible_Std140,
                 "StType1   st;"
                 "uint4     ua [4];" );

        TEST( not st2->HasDynamicArray() );
        TEST_Eq( st2->ArrayStride(),    0_b );
        TEST_Eq( st2->StaticSize(),     (16_b + 16_b) + (16_b * 4) );
        TEST_Eq( st2->Align(),          16_b );


        const String    glsl = ToGLSL( st2 );
        const String    msl  = ToMSL( st2 );
        const String    cpp  = ToCPP( st2 );

        const String    ref_glsl = R"#(
#define StType1_defined
struct StType1
{
    vec4  ff;  // offset: 0
    uvec2  uu;  // offset: 16
    ivec2  ii;  // offset: 24
};

Buffer {
    layout(offset=0, align=16) StType1  st;
    layout(offset=32, align=16) uvec4  ua [4];
}
)#";
        const String    ref_msl = R"#(
struct StType1
{
    float4  ff;  // offset: 0
    uint2  uu;  // offset: 16
    int2  ii;  // offset: 24
};
static_assert( sizeof(StType1) == 32, "size mismatch" );

struct StType2
{
    StType1  st;  // offset: 0
    uint4  ua [4];  // offset: 32
};
static_assert( sizeof(StType2) == 96, "size mismatch" );

)#";
        const String    ref_cpp = R"#(
#ifndef StType1_DEFINED
#   define StType1_DEFINED
    // size: 32, align: 16
    struct StType1
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xd9e31216u}};  // 'StType1'

        float4  ff;
        uint2  uu;
        int2  ii;
    };
#endif
    StaticAssert( offsetof(StType1, ff) == 0 );
    StaticAssert( offsetof(StType1, uu) == 16 );
    StaticAssert( offsetof(StType1, ii) == 24 );
    StaticAssert( sizeof(StType1) == 32 );

#ifndef StType2_DEFINED
#   define StType2_DEFINED
    // size: 96, align: 16
    struct StType2
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x40ea43acu}};  // 'StType2'

        StType1  st;
        StaticArray< uint4, 4 >    ua;
    };
#endif
    StaticAssert( offsetof(StType2, st) == 0 );
    StaticAssert( offsetof(StType2, ua) == 32 );
    StaticAssert( sizeof(StType2) == 96 );

)#";
        TEST( glsl == ref_glsl );
        TEST( msl == ref_msl );
        TEST( cpp == ref_cpp );
    }


#ifndef StType1_DEFINED
#   define StType1_DEFINED
    // size: 32, align: 16
    struct StType1
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xd9e31216u}};  // 'StType1'

        float4  ff;
        uint2  uu;
        int2  ii;
    };
#endif
    StaticAssert( offsetof(StType1, ff) == 0 );
    StaticAssert( offsetof(StType1, uu) == 16 );
    StaticAssert( offsetof(StType1, ii) == 24 );
    StaticAssert( sizeof(StType1) == 32 );

#ifndef StType2_DEFINED
#   define StType2_DEFINED
    // size: 96, align: 16
    struct StType2
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x40ea43acu}};  // 'StType2'

        StType1  st;
        StaticArray< uint4, 4 >    ua;
    };
#endif
    StaticAssert( offsetof(StType2, st) == 0 );
    StaticAssert( offsetof(StType2, ua) == 32 );
    StaticAssert( sizeof(StType2) == 96 );
//-----------------------------------------------------------------------------


    static void  StructType_Test5 ()
    {
        ShaderStructTypePtr st1{ new ShaderStructType{ "StType3" }};
        st1->Set( EStructLayout::Compatible_Std430,
                  "float            f;"
                  "packed_uint3     u;"
                  "packed_int3      i;" // offset 16
                  "packed_short2    s;"
                );                      // offset 32

        TEST( not st1->HasDynamicArray() );
        TEST_Eq( st1->ArrayStride(),    0_b );
        TEST_Eq( st1->StaticSize(),     16_b + 16_b );
        TEST_Eq( st1->Align(),          4_b );


        ShaderStructTypePtr st2{ new ShaderStructType{ "StType4" }};
        st2->Set( EStructLayout::Compatible_Std430,
                 "packed_half3  h3;"
                 "StType3       st;"        // offset 8
                 "packed_short2 ua [8];"    // offset 40
                );                          // offset 72

        TEST( not st2->HasDynamicArray() );
        TEST_Eq( st2->ArrayStride(),    0_b );
        TEST_Eq( st2->StaticSize(),     72_b );
        TEST_Eq( st2->Align(),          4_b );


        const String    glsl = ToGLSL( st2 );
        const String    msl  = ToMSL( st2 );
        const String    cpp  = ToCPP( st2 );

        const String    ref_glsl = R"#(
// size: 6 b, align: 2 b
struct packed_half3
{
    float16_t  x;
    float16_t  y;
    float16_t  z;
};
f16vec3  Cast (const packed_half3 src) { return f16vec3( src.x, src.y, src.z ); }
packed_half3  Cast (const f16vec3 src) { return packed_half3( src.x, src.y, src.z ); }

// size: 12 b, align: 4 b
struct packed_uint3
{
    uint  x;
    uint  y;
    uint  z;
};
uvec3  Cast (const packed_uint3 src) { return uvec3( src.x, src.y, src.z ); }
packed_uint3  Cast (const uvec3 src) { return packed_uint3( src.x, src.y, src.z ); }

// size: 12 b, align: 4 b
struct packed_int3
{
    int  x;
    int  y;
    int  z;
};
ivec3  Cast (const packed_int3 src) { return ivec3( src.x, src.y, src.z ); }
packed_int3  Cast (const ivec3 src) { return packed_int3( src.x, src.y, src.z ); }

// size: 4 b, align: 2 b
struct packed_short2
{
    int16_t  x;
    int16_t  y;
};
i16vec2  Cast (const packed_short2 src) { return i16vec2( src.x, src.y ); }
packed_short2  Cast (const i16vec2 src) { return packed_short2( src.x, src.y ); }

#define StType3_defined
struct StType3
{
    float  f;  // offset: 0
    packed_uint3  u;  // offset: 4
    packed_int3  i;  // offset: 16
    packed_short2  s;  // offset: 28
};

Buffer {
    layout(offset=0, align=2) packed_half3  h3;
    layout(offset=8, align=4) StType3  st;
    layout(offset=40, align=2) packed_short2  ua [8];
}
)#";
        const String    ref_msl = R"#(
struct StType3
{
    float  f;  // offset: 0
    packed_uint3  u;  // offset: 4
    packed_int3  i;  // offset: 16
    packed_short2  s;  // offset: 28
};
static_assert( sizeof(StType3) == 32, "size mismatch" );

struct StType4
{
    packed_half3  h3;  // offset: 0
    StType3  st;  // offset: 8
    packed_short2  ua [8];  // offset: 40
};
static_assert( sizeof(StType4) == 72, "size mismatch" );

)#";
        const String    ref_cpp = R"#(
#ifndef StType3_DEFINED
#   define StType3_DEFINED
    // size: 32, align: 4
    struct StType3
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x37ed733au}};  // 'StType3'

        float  f;
        packed_uint3  u;
        packed_int3  i;
        packed_short2  s;
    };
#endif
    StaticAssert( offsetof(StType3, f) == 0 );
    StaticAssert( offsetof(StType3, u) == 4 );
    StaticAssert( offsetof(StType3, i) == 16 );
    StaticAssert( offsetof(StType3, s) == 28 );
    StaticAssert( sizeof(StType3) == 32 );

#ifndef StType4_DEFINED
#   define StType4_DEFINED
    // size: 72, align: 4
    struct StType4
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xa989e699u}};  // 'StType4'

        packed_half3  h3;
        StType3  st;
        StaticArray< packed_short2, 8 >    ua;
    };
#endif
    StaticAssert( offsetof(StType4, h3) == 0 );
    StaticAssert( offsetof(StType4, st) == 8 );
    StaticAssert( offsetof(StType4, ua) == 40 );
    StaticAssert( sizeof(StType4) == 72 );

)#";
        TEST( glsl == ref_glsl );
        TEST( msl == ref_msl );
        TEST( cpp == ref_cpp );
    }


#ifndef StType3_DEFINED
#   define StType3_DEFINED
    // size: 32, align: 4
    struct StType3
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x37ed733au}};  // 'StType3'

        float  f;
        packed_uint3  u;
        packed_int3  i;
        packed_short2  s;
    };
#endif
    StaticAssert( offsetof(StType3, f) == 0 );
    StaticAssert( offsetof(StType3, u) == 4 );
    StaticAssert( offsetof(StType3, i) == 16 );
    StaticAssert( offsetof(StType3, s) == 28 );
    StaticAssert( sizeof(StType3) == 32 );

#ifndef StType4_DEFINED
#   define StType4_DEFINED
    // size: 72, align: 4
    struct StType4
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xa989e699u}};  // 'StType4'

        packed_half3  h3;
        StType3  st;
        StaticArray< packed_short2, 8 >    ua;
    };
#endif
    StaticAssert( offsetof(StType4, h3) == 0 );
    StaticAssert( offsetof(StType4, st) == 8 );
    StaticAssert( offsetof(StType4, ua) == 40 );
    StaticAssert( sizeof(StType4) == 72 );
//-----------------------------------------------------------------------------


    static void  StructType_Test6 ()
    {
        ShaderStructTypePtr st{ new ShaderStructType{ "StType5" }};
        st->Set( EStructLayout::Compatible_Std140,
                 "packed_float3     Position;"
                 "packed_float3     Normal;"
                 "packed_float3     Texcoord;" );

        TEST( not st->HasDynamicArray() );
        TEST_Eq( st->ArrayStride(), 0_b );
        TEST_Eq( st->StaticSize(),      (4_b * 3) + (4_b * 3) + (4_b * 3) );
        TEST_Eq( st->Align(),           16_b );


        const String    glsl = ToGLSL( st );
        const String    msl  = ToMSL( st );
        const String    cpp  = ToCPP( st );

        const String    ref_glsl = R"#(
// size: 12 b, align: 4 b
#define inplace_float3( _name_ ) \
    float  _name_ ## _x; \
    float  _name_ ## _y; \
    float  _name_ ## _z
#define GetInplaceFloat3( _fieldName_ )  vec3( (_fieldName_ ## _x), (_fieldName_ ## _y), (_fieldName_ ## _z) )
#define SetInplaceFloat3( _fieldName_, _src_ )  {(_fieldName_ ## _x = (_src_).x), (_fieldName_ ## _y = (_src_).y), (_fieldName_ ## _z = (_src_).z)}

Buffer {
    layout(offset=0, align=4) inplace_float3( Position );
    layout(offset=12, align=4) inplace_float3( Normal );
    layout(offset=24, align=4) inplace_float3( Texcoord );
}
)#";
        const String    ref_msl = R"#(
struct StType5
{
    packed_float3  Position;  // offset: 0
    packed_float3  Normal;  // offset: 12
    packed_float3  Texcoord;  // offset: 24
};
static_assert( sizeof(StType5) == 36, "size mismatch" );

)#";
        const String    ref_cpp = R"#(
#ifndef StType5_DEFINED
#   define StType5_DEFINED
    // size: 36, align: 4 (16)
    struct StType5
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xde8ed60fu}};  // 'StType5'

        packed_float3  Position;
        packed_float3  Normal;
        packed_float3  Texcoord;
    };
#endif
    StaticAssert( offsetof(StType5, Position) == 0 );
    StaticAssert( offsetof(StType5, Normal) == 12 );
    StaticAssert( offsetof(StType5, Texcoord) == 24 );
    StaticAssert( sizeof(StType5) == 36 );

)#";
        TEST( glsl == ref_glsl );
        TEST( msl == ref_msl );
        TEST( cpp == ref_cpp );
    }


#ifndef StType5_DEFINED
#   define StType5_DEFINED
    // size: 36, align: 4 (16)
    struct StType5
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xde8ed60fu}};  // 'StType5'

        packed_float3  Position;
        packed_float3  Normal;
        packed_float3  Texcoord;
    };
#endif
    StaticAssert( offsetof(StType5, Position) == 0 );
    StaticAssert( offsetof(StType5, Normal) == 12 );
    StaticAssert( offsetof(StType5, Texcoord) == 24 );
    StaticAssert( sizeof(StType5) == 36 );
//-----------------------------------------------------------------------------


    static void  StructType_Test7 ()
    {
        ShaderStructTypePtr st{ new ShaderStructType{ "StType6" }};
        st->Set( EStructLayout::Compatible_Std430,
                 "uint              Count;"
                 "packed_float2     Positions [3];"
                 "uint              Indices [3];"
                 "packed_float3x3   Mat [2];"
                 "float2            Positions2 [3];"
                 "uint              Indices2 [3];"
                 "float3x3          Mat2 [2];" );

        TEST( not st->HasDynamicArray() );
        TEST_Eq( st->ArrayStride(), 0_b );

        const String    glsl = ToGLSL( st );
        const String    msl  = ToMSL( st );
        const String    cpp  = ToCPP( st );

        const String    ref_msl = R"#(
// size: 72 b, align: 4 b
struct packed_float3x3
{
    packed_float3  c0;
    packed_float3  c1;
    packed_float3  c2;

    float3x3 cast () const { return float3x3( float3(c0), float3(c1), float3(c2) ); }
};
struct StType6
{
    uint  Count;  // offset: 0
    packed_float2  Positions [3];  // offset: 4
    uint  Indices [3];  // offset: 28
    packed_float3x3  Mat [2];  // offset: 40
    float2  Positions2 [3];  // offset: 112
    uint  Indices2 [3];  // offset: 136
    float3x3  Mat2 [2];  // offset: 160
};
static_assert( sizeof(StType6) == 256, "size mismatch" );

)#";
        const String    ref_glsl = R"#(
// size: 24 b, align: 4 b
struct packed_float2
{
    float  x;
    float  y;
};
vec2  Cast (const packed_float2 src) { return vec2( src.x, src.y ); }
packed_float2  Cast (const vec2 src) { return packed_float2( src.x, src.y ); }

// size: 24 b, align: 4 b
struct packed_float3
{
    float  x;
    float  y;
    float  z;
};
vec3  Cast (const packed_float3 src) { return vec3( src.x, src.y, src.z ); }
packed_float3  Cast (const vec3 src) { return packed_float3( src.x, src.y, src.z ); }

// size: 72 b, align: 4 b
struct packed_float3x3
{
    packed_float3  c0;
    packed_float3  c1;
    packed_float3  c2;
};
mat3x3  Cast (const packed_float3x3 src) { return mat3x3( Cast(src.c0), Cast(src.c1), Cast(src.c2) ); }
packed_float3x3  Cast (const mat3x3 src) { return packed_float3x3( Cast(src[0]), Cast(src[1]), Cast(src[2]) ); }

Buffer {
    layout(offset=0, align=4) uint  Count;
    layout(offset=4, align=4) packed_float2  Positions [3];
    layout(offset=28, align=4) uint  Indices [3];
    layout(offset=40, align=4) packed_float3x3  Mat [2];
    layout(offset=112, align=8) vec2  Positions2 [3];
    layout(offset=136, align=4) uint  Indices2 [3];
    layout(offset=160, align=16) mat3x3  Mat2 [2];
}
)#";
        const String    ref_cpp = R"#(
#ifndef StType6_DEFINED
#   define StType6_DEFINED
    // size: 256, align: 16
    struct StType6
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x478787b5u}};  // 'StType6'

        uint  Count;
        StaticArray< packed_float2, 3 >    Positions;
        StaticArray< uint, 3 >    Indices;
        StaticArray< packed_float3x3_storage, 2 >    Mat;
        StaticArray< float2, 3 >    Positions2;
        StaticArray< uint, 3 >    Indices2;
        StaticArray< float3x3_storage, 2 >    Mat2;
    };
#endif
    StaticAssert( offsetof(StType6, Count) == 0 );
    StaticAssert( offsetof(StType6, Positions) == 4 );
    StaticAssert( offsetof(StType6, Indices) == 28 );
    StaticAssert( offsetof(StType6, Mat) == 40 );
    StaticAssert( offsetof(StType6, Positions2) == 112 );
    StaticAssert( offsetof(StType6, Indices2) == 136 );
    StaticAssert( offsetof(StType6, Mat2) == 160 );
    StaticAssert( sizeof(StType6) == 256 );

)#";
        TEST( glsl == ref_glsl );
        TEST( msl == ref_msl );
        TEST( cpp == ref_cpp );
    }


#ifndef StType6_DEFINED
#   define StType6_DEFINED
    // size: 256, align: 16
    struct StType6
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x478787b5u}};  // 'StType6'

        uint  Count;
        StaticArray< packed_float2, 3 >    Positions;
        StaticArray< uint, 3 >    Indices;
        StaticArray< packed_float3x3_storage, 2 >    Mat;
        StaticArray< float2, 3 >    Positions2;
        StaticArray< uint, 3 >    Indices2;
        StaticArray< float3x3_storage, 2 >    Mat2;
    };
#endif
    StaticAssert( offsetof(StType6, Count) == 0 );
    StaticAssert( offsetof(StType6, Positions) == 4 );
    StaticAssert( offsetof(StType6, Indices) == 28 );
    StaticAssert( offsetof(StType6, Mat) == 40 );
    StaticAssert( offsetof(StType6, Positions2) == 112 );
    StaticAssert( offsetof(StType6, Indices2) == 136 );
    StaticAssert( offsetof(StType6, Mat2) == 160 );
    StaticAssert( sizeof(StType6) == 256 );
//-----------------------------------------------------------------------------


    static void  StructType_Test8 ()
    {
        ShaderStructTypePtr st{ new ShaderStructType{ "StType8" }};
        st->Set( EStructLayout::Compatible_Std140,
                 "float4    pos [];" );

        TEST( st->HasDynamicArray() );
        TEST_Eq( st->ArrayStride(), 16_b );

        const String    glsl = ToGLSL( st );
        const String    msl  = ToMSL( st );
        const String    cpp  = ToCPP( st );

        const String    ref_glsl = R"#(
Buffer {
    layout(offset=0, align=16) vec4  pos [];
}
)#";
        const String    ref_msl = R"#(
struct StType8
{
    device float4*  pos;  // offset: 0
};

)#";
        const String    ref_cpp = R"#(
#ifndef StType8_DEFINED
#   define StType8_DEFINED
    // size: 0, align: 16
    struct StType8
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xa03faab2u}};  // 'StType8'

    //  float4  pos [];
    };
#endif

)#";
        TEST( glsl == ref_glsl );
        TEST( msl == ref_msl );
        TEST( cpp == ref_cpp );
    }


#ifndef StType8_DEFINED
#   define StType8_DEFINED
    // size: 0, align: 16
    struct StType8
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xa03faab2u}};  // 'StType8'

    //  float4  pos [];
    };
#endif
//-----------------------------------------------------------------------------


    static void  StructType_Test9 ()
    {
        ShaderStructTypePtr st1{ new ShaderStructType{ "StType9A" }};
        st1->AddUsage( ShaderStructType::EUsage::BufferReference );
        st1->Set( EStructLayout::Compatible_Std140,
                  "float4   pos;"
                  "float3   norm;" );

        ShaderStructTypePtr st2{ new ShaderStructType{ "StType9B" }};
        st2->AddUsage( ShaderStructType::EUsage::BufferReference );
        st2->Set( EStructLayout::Compatible_Std140,
                  "float2   a;"
                  "int      b;" );

        ShaderStructTypePtr st{ new ShaderStructType{ "StType9" }};
        st->Set( EStructLayout::Compatible_Std140,
                 "StType9A &    ref;"
                 "float2 *      arr;"
                 "StType9B *    st_arr;" );

        const String    glsl = ToGLSL( st );
        const String    msl  = ToMSL( st );
        const String    cpp  = ToCPP( st );

        const String    ref_glsl = R"#(
layout(std140, buffer_reference, buffer_reference_align=16) buffer StType9A_AERef
{
    layout(offset=0, align=16) vec4  pos;
    layout(offset=16, align=16) vec3  norm;
};

layout(std430, buffer_reference, buffer_reference_align=8) buffer float2_AEPtr
{
    vec2  data [];
};

layout(std140, buffer_reference, buffer_reference_align=16) buffer StType9B_AERef
{
    layout(offset=0, align=8) vec2  a;
    layout(offset=8, align=4) int  b;
};

layout(std430, buffer_reference, buffer_reference_align=16) buffer StType9B_AEPtr
{
    StType9B_AERef  data [];
};

Buffer {
    layout(offset=0, align=8) StType9A_AERef  ref;
    layout(offset=8, align=8) float2_AEPtr  arr;
    layout(offset=16, align=8) StType9B_AEPtr  st_arr;
}
)#";
        const String    ref_msl = R"#(
struct StType9A
{
    float4  pos;  // offset: 0
    float3  norm;  // offset: 16
};
static_assert( sizeof(StType9A) == 32, "size mismatch" );

struct StType9B
{
    float2  a;  // offset: 0
    int  b;  // offset: 8
};
static_assert( sizeof(StType9B) == 16, "size mismatch" );

struct StType9
{
    device StType9A*  ref;  // offset: 0
    device float2*  arr;  // offset: 8
    device StType9B*  st_arr;  // offset: 16
};
static_assert( sizeof(StType9) == 24, "size mismatch" );

)#";
        const String    ref_cpp = R"#(
#ifndef StType9A_DEFINED
#   define StType9A_DEFINED
    // size: 32, align: 16
    struct StType9A
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xef0d42c0u}};  // 'StType9A'

        float4  pos;
        float3  norm;
    };
#endif
    StaticAssert( offsetof(StType9A, pos) == 0 );
    StaticAssert( offsetof(StType9A, norm) == 16 );
    StaticAssert( sizeof(StType9A) == 32 );

#ifndef StType9B_DEFINED
#   define StType9B_DEFINED
    // size: 12 (16), align: 8 (16)
    struct alignas(8) StType9B
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x7604137au}};  // 'StType9B'

        float2  a;
        int  b;
    };
#endif
    StaticAssert( offsetof(StType9B, a) == 0 );
    StaticAssert( offsetof(StType9B, b) == 8 );
    StaticAssert( sizeof(StType9B) == 16 );

#ifndef StType9_DEFINED
#   define StType9_DEFINED
    // size: 24, align: 8 (16)
    struct StType9
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xd7389a24u}};  // 'StType9'

        TDeviceAddress< StType9A >  ref;
        TDeviceAddress< float2 *>  arr;
        TDeviceAddress< StType9B *>  st_arr;
    };
#endif
    StaticAssert( offsetof(StType9, ref) == 0 );
    StaticAssert( offsetof(StType9, arr) == 8 );
    StaticAssert( offsetof(StType9, st_arr) == 16 );
    StaticAssert( sizeof(StType9) == 24 );

)#";
        TEST( glsl == ref_glsl );
        TEST( msl == ref_msl );
        TEST( cpp == ref_cpp );
    }


#ifndef StType9A_DEFINED
#   define StType9A_DEFINED
    // size: 32, align: 16
    struct StType9A
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xef0d42c0u}};  // 'StType9A'

        float4  pos;
        float3  norm;
    };
#endif
    StaticAssert( offsetof(StType9A, pos) == 0 );
    StaticAssert( offsetof(StType9A, norm) == 16 );
    StaticAssert( sizeof(StType9A) == 32 );

#ifndef StType9B_DEFINED
#   define StType9B_DEFINED
    // size: 12 (16), align: 8 (16)
    struct alignas(8) StType9B
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x7604137au}};  // 'StType9B'

        float2  a;
        int  b;
    };
#endif
    StaticAssert( offsetof(StType9B, a) == 0 );
    StaticAssert( offsetof(StType9B, b) == 8 );
    StaticAssert( sizeof(StType9B) == 16 );

#ifndef StType9_DEFINED
#   define StType9_DEFINED
    // size: 24, align: 8 (16)
    struct StType9
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xd7389a24u}};  // 'StType9'

        TDeviceAddress< StType9A >  ref;
        TDeviceAddress< float2 *>  arr;
        TDeviceAddress< StType9B *>  st_arr;
    };
#endif
    StaticAssert( offsetof(StType9, ref) == 0 );
    StaticAssert( offsetof(StType9, arr) == 8 );
    StaticAssert( offsetof(StType9, st_arr) == 16 );
    StaticAssert( sizeof(StType9) == 24 );
//-----------------------------------------------------------------------------


    static void  StructType_Test10 ()
    {
        ShaderStructTypePtr st{ new ShaderStructType{ "StType.10" }};
        st->Set( EStructLayout::Compatible_Std140,
                 "float4    pos;" );

        const String    glsl = ToGLSL( st );
        const String    msl  = ToMSL( st );
        const String    cpp  = ToCPP( st );

        const String    ref_glsl = R"#(
Buffer {
    layout(offset=0, align=16) vec4  pos;
}
)#";
        const String    ref_msl = R"#(
struct StType_10
{
    float4  pos;  // offset: 0
};
static_assert( sizeof(StType_10) == 16, "size mismatch" );

)#";
        const String    ref_cpp = R"#(
#ifndef StType_10_DEFINED
#   define StType_10_DEFINED
    // size: 16, align: 16
    struct StType_10
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xc35deb7u}};  // 'StType_10'

        float4  pos;
    };
#endif
    StaticAssert( offsetof(StType_10, pos) == 0 );
    StaticAssert( sizeof(StType_10) == 16 );

)#";
        TEST( glsl == ref_glsl );
        TEST( msl == ref_msl );
        TEST( cpp == ref_cpp );
    }


#ifndef StType_10_DEFINED
#   define StType_10_DEFINED
    // size: 16, align: 16
    struct StType_10
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xc35deb7u}};  // 'StType_10'

        float4  pos;
    };
#endif
    StaticAssert( offsetof(StType_10, pos) == 0 );
    StaticAssert( sizeof(StType_10) == 16 );
//-----------------------------------------------------------------------------


    static void  StructType_Test11 ()
    {
        ShaderStructTypePtr st{ new ShaderStructType{ "StType11" }};
        st->Set( EStructLayout::Compatible_Std140,
                 "float4x4  transform;"
                 "uint      meshIdx;"
                 "uint      materialIdx;" );

        TEST_Eq( st->StaticSize(), 80_b );
        TEST_Eq( st->Align(), 16_b );

        const String    glsl = ToGLSL( st );
        const String    msl  = ToMSL( st );
        const String    cpp  = ToCPP( st );

        const String    ref_glsl = R"#(
Buffer {
    layout(offset=0, align=16) mat4x4  transform;
    layout(offset=64, align=4) uint  meshIdx;
    layout(offset=68, align=4) uint  materialIdx;
}
)#";
        const String    ref_msl = R"#(
struct StType11
{
    float4x4  transform;  // offset: 0
    uint  meshIdx;  // offset: 64
    uint  materialIdx;  // offset: 68
};
static_assert( sizeof(StType11) == 80, "size mismatch" );

)#";
        const String    ref_cpp = R"#(
#ifndef StType11_DEFINED
#   define StType11_DEFINED
    // size: 72 (80), align: 16
    struct alignas(16) StType11
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x77d1b9f4u}};  // 'StType11'

        float4x4_storage  transform;
        uint  meshIdx;
        uint  materialIdx;
    };
#endif
    StaticAssert( offsetof(StType11, transform) == 0 );
    StaticAssert( offsetof(StType11, meshIdx) == 64 );
    StaticAssert( offsetof(StType11, materialIdx) == 68 );
    StaticAssert( sizeof(StType11) == 80 );

)#";
        TEST( glsl == ref_glsl );
        TEST( msl == ref_msl );
        TEST( cpp == ref_cpp );
    }


#ifndef StType11_DEFINED
#   define StType11_DEFINED
    // size: 72 (80), align: 16
    struct alignas(16) StType11
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x77d1b9f4u}};  // 'StType11'

        float4x4_storage  transform;
        uint  meshIdx;
        uint  materialIdx;
    };
#endif
    StaticAssert( offsetof(StType11, transform) == 0 );
    StaticAssert( offsetof(StType11, meshIdx) == 64 );
    StaticAssert( offsetof(StType11, materialIdx) == 68 );
    StaticAssert( sizeof(StType11) == 80 );
//-----------------------------------------------------------------------------


    static void  StructType_Test12 ()
    {
        ShaderStructTypePtr st{ new ShaderStructType{ "StType12" }};
        st->Set( EStructLayout::Compatible_Std430,
                 "uint      count;"
                 "float4    arr [];");

        TEST_Eq( st->StaticSize(),  16_b );
        TEST_Eq( st->Align(),       16_b );
        TEST_Eq( st->ArrayStride(), 16_b );

        const String    glsl = ToGLSL( st );
        const String    msl  = ToMSL( st );
        const String    cpp  = ToCPP( st );

        const String    ref_glsl = R"#(
Buffer {
    layout(offset=0, align=4) uint  count;
    layout(offset=16, align=16) vec4  arr [];
}
)#";
        const String    ref_msl = R"#(
struct StType12
{
    uint  count;  // offset: 0
    device float4*  arr;  // offset: 16
};

)#";
        const String    ref_cpp = R"#(
#ifndef StType12_DEFINED
#   define StType12_DEFINED
    // size: 4 (16), align: 16
    struct alignas(16) StType12
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xeed8e84eu}};  // 'StType12'

        uint  count;
    //  float4  arr [];
    };
#endif
    StaticAssert( offsetof(StType12, count) == 0 );
    StaticAssert( sizeof(StType12) == 16 );

)#";
        TEST( glsl == ref_glsl );
        TEST( msl == ref_msl );
        TEST( cpp == ref_cpp );
    }


#ifndef StType12_DEFINED
#   define StType12_DEFINED
    // size: 4 (16), align: 16
    struct alignas(16) StType12
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xeed8e84eu}};  // 'StType12'

        uint  count;
    //  float4  arr [];
    };
#endif
    StaticAssert( offsetof(StType12, count) == 0 );
    StaticAssert( sizeof(StType12) == 16 );
//-----------------------------------------------------------------------------


    static void  StructType_Test13 ()
    {
        ShaderStructTypePtr st1{ new ShaderStructType{ "StType13A" }};
        st1->AddUsage( ShaderStructType::EUsage::BufferReference );
        st1->Set( EStructLayout::Compatible_Std140,
                  "float4   pos;"
                  "float3   norm;"
                  "ulong    l;" );

        ShaderStructTypePtr st{ new ShaderStructType{ "StType13" }};
        st->Set( EStructLayout::Compatible_Std430,
                 "DeviceAddress *   untypedAddrArr;"
                 "DeviceAddress *   untypedAddrArrArr [8];"
                 "StType13A &       typedRef;"
                 "StType13A *       typedArr;"
                 "StType13A *       typedArrArr [7];" );

        const String    glsl = ToGLSL( st );
        const String    msl  = ToMSL( st );
        const String    cpp  = ToCPP( st );

        const String    ref_glsl = R"#(
layout(std430, buffer_reference, buffer_reference_align=8) buffer DeviceAddress_AEPtr
{
    uvec2  data [];
};

layout(std140, buffer_reference, buffer_reference_align=16) buffer StType13A_AERef
{
    layout(offset=0, align=16) vec4  pos;
    layout(offset=16, align=16) vec3  norm;
    layout(offset=32, align=8) uint64_t  l;
};

layout(std430, buffer_reference, buffer_reference_align=16) buffer StType13A_AEPtr
{
    StType13A_AERef  data [];
};

Buffer {
    layout(offset=0, align=8) DeviceAddress_AEPtr  untypedAddrArr;
    layout(offset=8, align=8) DeviceAddress_AEPtr  untypedAddrArrArr [8];
    layout(offset=72, align=8) StType13A_AERef  typedRef;
    layout(offset=80, align=8) StType13A_AEPtr  typedArr;
    layout(offset=88, align=8) StType13A_AEPtr  typedArrArr [7];
}
)#";
        const String    ref_msl = R"#(
struct StType13A
{
    float4  pos;  // offset: 0
    float3  norm;  // offset: 16
    ulong  l;  // offset: 32
};
static_assert( sizeof(StType13A) == 48, "size mismatch" );

struct StType13
{
    device void*  untypedAddrArr;  // offset: 0
    device void*  untypedAddrArrArr [8];  // offset: 8
    device StType13A*  typedRef;  // offset: 72
    device StType13A*  typedArr;  // offset: 80
    device StType13A*  typedArrArr [7];  // offset: 88
};
static_assert( sizeof(StType13) == 144, "size mismatch" );

)#";
        const String    ref_cpp = R"#(
#ifndef StType13A_DEFINED
#   define StType13A_DEFINED
    // size: 40 (48), align: 16
    struct alignas(16) StType13A
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x5b481bb5u}};  // 'StType13A'

        float4  pos;
        float3  norm;
        ulong  l;
    };
#endif
    StaticAssert( offsetof(StType13A, pos) == 0 );
    StaticAssert( offsetof(StType13A, norm) == 16 );
    StaticAssert( offsetof(StType13A, l) == 32 );
    StaticAssert( sizeof(StType13A) == 48 );

#ifndef StType13_DEFINED
#   define StType13_DEFINED
    // size: 144, align: 8
    struct StType13
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x99dfd8d8u}};  // 'StType13'

        TDeviceAddress< DeviceAddress *>  untypedAddrArr;
        StaticArray< TDeviceAddress< DeviceAddress *>, 8 >    untypedAddrArrArr;
        TDeviceAddress< StType13A >  typedRef;
        TDeviceAddress< StType13A *>  typedArr;
        StaticArray< TDeviceAddress< StType13A *>, 7 >    typedArrArr;
    };
#endif
    StaticAssert( offsetof(StType13, untypedAddrArr) == 0 );
    StaticAssert( offsetof(StType13, untypedAddrArrArr) == 8 );
    StaticAssert( offsetof(StType13, typedRef) == 72 );
    StaticAssert( offsetof(StType13, typedArr) == 80 );
    StaticAssert( offsetof(StType13, typedArrArr) == 88 );
    StaticAssert( sizeof(StType13) == 144 );

)#";
        TEST( glsl == ref_glsl );
        TEST( msl == ref_msl );
        TEST( cpp == ref_cpp );
    }


#ifndef StType13A_DEFINED
#   define StType13A_DEFINED
    // size: 40 (48), align: 16
    struct alignas(16) StType13A
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x5b481bb5u}};  // 'StType13A'

        float4  pos;
        float3  norm;
        ulong  l;
    };
#endif
    StaticAssert( offsetof(StType13A, pos) == 0 );
    StaticAssert( offsetof(StType13A, norm) == 16 );
    StaticAssert( offsetof(StType13A, l) == 32 );
    StaticAssert( sizeof(StType13A) == 48 );

#ifndef StType13_DEFINED
#   define StType13_DEFINED
    // size: 144, align: 8
    struct StType13
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x99dfd8d8u}};  // 'StType13'

        TDeviceAddress< DeviceAddress *>  untypedAddrArr;
        StaticArray< TDeviceAddress< DeviceAddress *>, 8 >    untypedAddrArrArr;
        TDeviceAddress< StType13A >  typedRef;
        TDeviceAddress< StType13A *>  typedArr;
        StaticArray< TDeviceAddress< StType13A *>, 7 >    typedArrArr;
    };
#endif
    StaticAssert( offsetof(StType13, untypedAddrArr) == 0 );
    StaticAssert( offsetof(StType13, untypedAddrArrArr) == 8 );
    StaticAssert( offsetof(StType13, typedRef) == 72 );
    StaticAssert( offsetof(StType13, typedArr) == 80 );
    StaticAssert( offsetof(StType13, typedArrArr) == 88 );
    StaticAssert( sizeof(StType13) == 144 );
//-----------------------------------------------------------------------------


    static void  StructType_Test14 ()
    {
        ShaderStructTypePtr st{ new ShaderStructType{ "StType14" }};
        st->Set( EStructLayout::Compatible_Std430,
                 "packed_float3 *   normals;" );

        const String    glsl = ToGLSL( st );
        const String    msl  = ToMSL( st );
        const String    cpp  = ToCPP( st );

        const String    ref_glsl = R"#(
// size: 8 b, align: 8 b
struct packed_float3
{
    float  x;
    float  y;
    float  z;
};
vec3  Cast (const packed_float3 src) { return vec3( src.x, src.y, src.z ); }
packed_float3  Cast (const vec3 src) { return packed_float3( src.x, src.y, src.z ); }

layout(std430, buffer_reference, buffer_reference_align=4) buffer packed_float3_AEPtr
{
    packed_float3  data [];
};

Buffer {
    layout(offset=0, align=8) packed_float3_AEPtr  normals;
}
)#";
        const String    ref_msl = R"#(
struct StType14
{
    device packed_float3*  normals;  // offset: 0
};
static_assert( sizeof(StType14) == 8, "size mismatch" );

)#";
        const String    ref_cpp = R"#(
#ifndef StType14_DEFINED
#   define StType14_DEFINED
    // size: 8, align: 8
    struct StType14
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x7bb4d7bu}};  // 'StType14'

        TDeviceAddress< packed_float3 *>  normals;
    };
#endif
    StaticAssert( offsetof(StType14, normals) == 0 );
    StaticAssert( sizeof(StType14) == 8 );

)#";
        TEST( glsl == ref_glsl );
        TEST( msl == ref_msl );
        TEST( cpp == ref_cpp );
    }


#ifndef StType14_DEFINED
#   define StType14_DEFINED
    // size: 8, align: 8
    struct StType14
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x7bb4d7bu}};  // 'StType14'

        TDeviceAddress< packed_float3 *>  normals;
    };
#endif
    StaticAssert( offsetof(StType14, normals) == 0 );
    StaticAssert( sizeof(StType14) == 8 );
//-----------------------------------------------------------------------------


    static void  StructType_Test15 ()
    {
        ShaderStructTypePtr st1{ new ShaderStructType{ "StType15A" }};
        st1->Set( EStructLayout::Compatible_Std430,
                  "uint3    a;"
                  "float    b;" );

        ShaderStructTypePtr st{ new ShaderStructType{ "StType15" }};
        st->Set( EStructLayout::Compatible_Std430,
                 "StType15A arr [8];" );

        const String    glsl = ToGLSL( st );
        const String    msl  = ToMSL( st );
        const String    cpp  = ToCPP( st );

        const String    ref_glsl = R"#(
#define StType15A_defined
struct StType15A
{
    uvec3  a;  // offset: 0
    uint  _a_padding_w;  // offset: 12
    float  b;  // offset: 16
};

Buffer {
    layout(offset=0, align=16) StType15A  arr [8];
}
)#";
        const String    ref_msl = R"#(
struct StType15A
{
    uint3  a;  // offset: 0
    float  b;  // offset: 16
};
static_assert( sizeof(StType15A) == 32, "size mismatch" );

struct StType15
{
    StType15A  arr [8];  // offset: 0
};
static_assert( sizeof(StType15) == 256, "size mismatch" );

)#";
        const String    ref_cpp = R"#(
#ifndef StType15A_DEFINED
#   define StType15A_DEFINED
    // size: 20 (32), align: 16
    struct alignas(16) StType15A
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xd12bc33u}};  // 'StType15A'

        uint3  a;
        float  b;
    };
#endif
    StaticAssert( offsetof(StType15A, a) == 0 );
    StaticAssert( offsetof(StType15A, b) == 16 );
    StaticAssert( sizeof(StType15A) == 32 );

#ifndef StType15_DEFINED
#   define StType15_DEFINED
    // size: 256, align: 16
    struct StType15
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x70bc7dedu}};  // 'StType15'

        StaticArray< StType15A, 8 >    arr;
    };
#endif
    StaticAssert( offsetof(StType15, arr) == 0 );
    StaticAssert( sizeof(StType15) == 256 );

)#";
        TEST( glsl == ref_glsl );
        TEST( msl == ref_msl );
        TEST( cpp == ref_cpp );
    }


#ifndef StType15A_DEFINED
#   define StType15A_DEFINED
    // size: 20 (32), align: 16
    struct alignas(16) StType15A
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0xd12bc33u}};  // 'StType15A'

        uint3  a;
        float  b;
    };
#endif
    StaticAssert( offsetof(StType15A, a) == 0 );
    StaticAssert( offsetof(StType15A, b) == 16 );
    StaticAssert( sizeof(StType15A) == 32 );

#ifndef StType15_DEFINED
#   define StType15_DEFINED
    // size: 256, align: 16
    struct StType15
    {
        static constexpr auto  TypeName = ShaderStructName{HashVal32{0x70bc7dedu}};  // 'StType15'

        StaticArray< StType15A, 8 >    arr;
    };
#endif
    StaticAssert( offsetof(StType15, arr) == 0 );
    StaticAssert( sizeof(StType15) == 256 );
//-----------------------------------------------------------------------------
}


extern void  UnitTest_StructType ()
{
    ObjectStorage   obj;
    PipelineStorage ppln;
    obj.defaultFeatureSet   = "DefaultFS";
    obj.pplnStorage         = &ppln;
    obj.spirvCompiler       = MakeUnique<SpirvCompiler>( Array<Path>{} );
    obj.spirvCompiler->SetDefaultResourceLimits();
    ObjectStorage::SetInstance( &obj );

    #ifdef AE_METAL_TOOLS
        obj.metalCompiler = MakeUnique<MetalCompiler>( ArrayView<Path>{} );
    #endif

    ScriptFeatureSetPtr fs {new ScriptFeatureSet{ "DefaultFS" }};
    fs->fs.SetAll( EFeature::RequireTrue );

    try {
        StructType_Test1();
        StructType_Test2();
        StructType_Test3();
        StructType_Test4();
        StructType_Test5();
        StructType_Test6();
        StructType_Test7();
        StructType_Test8();
        StructType_Test9();
        StructType_Test10();
        StructType_Test11();
        StructType_Test12();
        StructType_Test13();
        StructType_Test14();
        StructType_Test15();
    } catch(...) {
        TEST( false );
    }

    ObjectStorage::SetInstance( null );
    TEST_PASSED();
}
