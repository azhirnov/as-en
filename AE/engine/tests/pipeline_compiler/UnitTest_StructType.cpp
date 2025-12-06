// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	template <typename T>
	ND_ String  ToGLSL (T &ptr)
	{
		String	hdr		= "\n";
		String	fields	= "Buffer {\n";

		CHECK_ERR( ptr->ToGLSL( true, INOUT hdr, INOUT fields ));
		hdr << fields << "}\n";
		return hdr;
	}

	template <typename T>
	ND_ String  ToMSL (T &ptr)
	{
		String	hdr	= "\n";
		CHECK_ERR( ptr->ToMSL( INOUT hdr ));
		return hdr;
	}

	template <typename T>
	ND_ String  ToCPP (T &ptr)
	{
		String	src = "\n";
		CHECK_ERR( ptr->ToCPP( INOUT src ));
		return src;
	}

	template <typename T>
	ND_ String  ToHLSL (T &ptr)
	{
		String	src = "\n";
		String	test;
		CHECK_ERR( ptr->ToHLSL( INOUT src, OUT &test ));
		src << test;
		return src;
	}
//-----------------------------------------------------------------------------


	static void  StructType_Test1_Layout (EStructLayout layout, uint arraySize = 1)
	{
		// vector test
		struct TypeInfo
		{
			StringView	name;
			uint		size;
			uint		align;
		};
		const TypeInfo	types[] = {
			{"char2",	2,	2 },	{"packed_char2",	2,	1 },	{"char_norm2",	2,	2 },	{"packed_char_norm2",	2,	1 },
			{"uchar3",	4,	4 },	{"packed_uchar3",	3,	1 },	{"uchar_norm3",	4,	4 },	{"packed_uchar_norm3",	3,	1 },
			{"char4",	4,	4 },	{"packed_char4",	4,	1 },	{"char_norm4",	4,	4 },	{"packed_char_norm4",	4,	1 },
			{"short2",	4,	4 },	{"packed_short2",	4,	2 },	{"short_norm2",	4,	4 },	{"packed_short_norm2",	4,	2 },
			{"ushort3",	8,	8 },	{"packed_ushort3",	6,	2 },	{"ushort_norm3",8,	8 },	{"packed_ushort_norm3",	6,	2 },
			{"short4",	8,	8 },	{"packed_short4",	8,	2 },	{"short_norm4",	8,	8 },	{"packed_short_norm4",	8,	2 },
			{"uint2",	8,	8 },	{"packed_uint2",	8,	4 },
			{"int3",	16,	16},	{"packed_int3",		12,	4 },
			{"uint4",	16,	16},	{"packed_uint4",	16,	4 },
			{"long2",	16,	16},	{"packed_long2",	16,	8 },
			{"ulong3",	32,	32},	{"packed_ulong3",	24,	8 },
			{"long4",	32,	32},	{"packed_long4",	32,	8 },
			{"half2",	4,	4 },	{"packed_half2",	4,	2 },
			{"half3",	8,	8 },	{"packed_half3",	6,	2 },
			{"half4",	8,	8 },	{"packed_half4",	8,	2 },
			{"float2",	8,	8 },	{"packed_float2",	8,	4 },
			{"float3",	16,	16},	{"packed_float3",	12,	4 },
			{"float4",	16,	16},	{"packed_float4",	16,	4 },
			{"double2",	16,	16},	{"packed_double2",	16,	8 },
			{"double3",	32,	32},	{"packed_double3",	24,	8 },
			{"double4",	32,	32},	{"packed_double4",	32,	8 }
		};

		static uint	idx = 0;
		for (auto& t : types)
		{
			bool	compatible = true;

			if ( AnyEqual( layout, EStructLayout::Compatible_Std430, EStructLayout::Compatible_Std140 ) and
				 (HasSubString( t.name, "double" ) or HasSubString( t.name, "char" )) )
			{
				// int8 and double types are not supported
				compatible = false;
			}

			if ( layout == EStructLayout::Metal and HasSubString( t.name, "double" ))
			{
				// double type is not supported
				compatible = false;
			}

			if ( AnyEqual( layout, EStructLayout::HLSL_Const, EStructLayout::HLSL_Struct ) and HasSubString( t.name, "char" ))
			{
				// int8 types are not supported
				compatible = false;
			}

			if ( layout == EStructLayout::Compatible_Std140			and
				 arraySize > 1										and
				 (t.align < 16_b or not IsMultipleOf( t.size, 16_b )) )
			{
				// not compatible with Metal
				compatible = false;
			}

			if ( AnyEqual( layout, EStructLayout::Std140, EStructLayout::HLSL_Const )	and
				 StartsWith( t.name, "packed_" )										and
				 arraySize > 1															and
				 (t.align < 16_b or not IsMultipleOf( t.size, 16_b )) )
			{
				// not compatible with GLSL / HLSL
				compatible = false;
			}

			ShaderStructTypePtr	st{ new ShaderStructType{ "VecTest"s << ToString(idx++) }};

			String	str = String{t.name} << " val";
			if ( arraySize > 1 )
				str << '[' << ToString( arraySize ) << ']';
			str << ';';

			try{
				st->Set( layout, str );
				TEST( compatible );
			}
			catch(...){
				TEST( not compatible );
			}
			if ( not compatible )
				continue;

			Bytes	req_align {t.align};
			Bytes	req_size {t.size};

			if ( AnyEqual( layout, EStructLayout::Compatible_Std140, EStructLayout::Std140, EStructLayout::HLSL_Const ))
				req_align = Max( req_align, 16_b );

			if ( arraySize > 1 )
				req_size = AlignUp( req_size, req_align );

			TEST_Eq( st->Align(),		req_align );
			TEST_Eq( st->StaticSize(),	req_size * arraySize );
		}
	}

	static void  StructType_Test1 ()
	{
		StructType_Test1_Layout( EStructLayout::Compatible_Std140 );
		StructType_Test1_Layout( EStructLayout::Compatible_Std430 );
		StructType_Test1_Layout( EStructLayout::Metal );
		StructType_Test1_Layout( EStructLayout::Std140 );
		StructType_Test1_Layout( EStructLayout::Std430 );
		StructType_Test1_Layout( EStructLayout::HLSL_Const );
		StructType_Test1_Layout( EStructLayout::HLSL_Struct );
		TEST_PASSED();
	}
//-----------------------------------------------------------------------------


	static void  StructType_Test2_Layout (EStructLayout layout)
	{
		// matrix test
		struct TypeInfo
		{
			StringView	name;
			uint		size;
			uint		align;
		};
		const TypeInfo	types[] = {
			{"half2x2",		8,   4 },	{"packed_half2x2",		8,   2},
			{"half2x3",		16,  8 },	{"packed_half2x3",		12,  2},
			{"half2x4",		16,  8 },	{"packed_half2x4",		16,  2},
			{"half3x2",		12,  4 },	{"packed_half3x2",		12,  2},
			{"half3x3",		24,  8 },	{"packed_half3x3",		18,  2},
			{"half3x4",		24,  8 },	{"packed_half3x4",		24,  2},
			{"half4x2",		16,  4 },	{"packed_half4x2",		16,  2},
			{"half4x3",		32,  8 },	{"packed_half4x3",		24,  2},
			{"half4x4",		32,  8 },	{"packed_half4x4",		32,  2},
			{"float2x2",	16,  8 },	{"packed_float2x2",		16,  4},
			{"float2x3",	32,  16},	{"packed_float2x3",		24,  4},
			{"float2x4",	32,  16},	{"packed_float2x4",		32,  4},
			{"float3x2",	24,  8 },	{"packed_float3x2",		24,  4},
			{"float3x3",	48,  16},	{"packed_float3x3",		36,  4},
			{"float3x4",	48,  16},	{"packed_float3x4",		48,  4},
			{"float4x2",	32,  8 },	{"packed_float4x2",		32,  4},
			{"float4x3",	64,  16},	{"packed_float4x3",		48,  4},
			{"float4x4",	64,  16},	{"packed_float4x4",		64,  4},
			{"double2x2",	32,  16},	{"packed_double2x2",	32,  8},
			{"double2x3",	64,  32},	{"packed_double2x3",	48,  8},
			{"double2x4",	64,  32},	{"packed_double2x4",	64,  8},
			{"double3x2",	48,  16},	{"packed_double3x2",	48,  8},
			{"double3x3",	96,  32},	{"packed_double3x3",	72,  8},
			{"double3x4",	96,  32},	{"packed_double3x4",	96,  8},
			{"double4x2",	64,  16},	{"packed_double4x2",	64,  8},
			{"double4x3",	128, 32},	{"packed_double4x3",	96,  8},
			{"double4x4",	128, 32},	{"packed_double4x4",	128, 8}
		};

		static uint	idx = 0;
		for (auto& t : types)
		{
			bool	compatible = true;

			if ( AnyEqual( layout, EStructLayout::Metal, EStructLayout::Compatible_Std140, EStructLayout::Compatible_Std430 ) and
				 HasSubString( t.name, "double" ))
			{
				// double type is not supported in Metal
				compatible = false;
			}

			if ( layout == EStructLayout::Compatible_Std140 and
				 not StartsWith( t.name, "packed_" )		and
				 not IsMultipleOf( t.align, 16 ))
			{
				compatible = false;
			}

			ShaderStructTypePtr	st{ new ShaderStructType{ "MatTest"s << ToString(idx++) }};

			try{
				st->Set( layout, String{t.name} << " val;" );
				TEST( compatible );
			}
			catch(...){
				TEST( not compatible );
			}
			if ( not compatible )
				continue;

			Bytes	req_align	{t.align};
			Bytes	req_size	{t.size};

			if ( AnyEqual( layout, EStructLayout::Std140, EStructLayout::HLSL_Const ) and
				 not StartsWith( t.name, "packed_" ))
			{
				uint	rows = t.size / t.align;
				req_size = rows * AlignUp( t.align, 16_b );
			}

			if ( AnyEqual( layout, EStructLayout::Compatible_Std140, EStructLayout::Std140, EStructLayout::HLSL_Const ))
			{
				req_align = AlignUp( req_align, 16_b );
			}

			TEST_Eq( st->Align(),		req_align );
			TEST_Eq( st->StaticSize(),	req_size );
		}
	}


	static void  StructType_Test2 ()
	{
		StructType_Test2_Layout( EStructLayout::Compatible_Std140 );
		StructType_Test2_Layout( EStructLayout::Compatible_Std430 );
		StructType_Test2_Layout( EStructLayout::Metal );
		StructType_Test2_Layout( EStructLayout::Std140 );
		StructType_Test2_Layout( EStructLayout::Std430 );
		StructType_Test2_Layout( EStructLayout::HLSL_Struct );
		StructType_Test2_Layout( EStructLayout::HLSL_Const );
		TEST_PASSED();
	}


	static void  StructType_Test3 ()
	{
		StructType_Test1_Layout( EStructLayout::Compatible_Std140,	8 );
		StructType_Test1_Layout( EStructLayout::Compatible_Std430,	8 );
		StructType_Test1_Layout( EStructLayout::Metal,				8 );
		StructType_Test1_Layout( EStructLayout::Std140,				8 );
		StructType_Test1_Layout( EStructLayout::Std430,				8 );
		StructType_Test1_Layout( EStructLayout::HLSL_Const,			8 );
		StructType_Test1_Layout( EStructLayout::HLSL_Struct,		8 );
		TEST_PASSED();
	}
//-----------------------------------------------------------------------------


	static void  StructType_Test4 ()
	{
		ShaderStructTypePtr	st1{ new ShaderStructType{ "StType1" }};
		st1->Set( EStructLayout::Compatible_Std140,
				 "float4	ff;"
				 "uint2		uu;"
				 "int2		ii;" );

		TEST( not st1->HasDynamicArray() );
		TEST_Eq( st1->ArrayStride(),	0_b );
		TEST_Eq( st1->StaticSize(),		16_b + 16_b );
		TEST_Eq( st1->Align(),			16_b );


		ShaderStructTypePtr	st2{ new ShaderStructType{ "StType2" }};
		st2->Set( EStructLayout::Compatible_Std140,
				 "StType1	st;"
				 "uint4		ua [4];" );

		TEST( not st2->HasDynamicArray() );
		TEST_Eq( st2->ArrayStride(),	0_b );
		TEST_Eq( st2->StaticSize(),		(16_b + 16_b) + (16_b * 4) );
		TEST_Eq( st2->Align(),			16_b );


		const String	glsl = ToGLSL( st2 );
		const String	msl  = ToMSL( st2 );
		const String	cpp  = ToCPP( st2 );
		const String	hlsl = ToHLSL( st2 );

		const String	ref_glsl = R"#(
#define StType1_defined
struct StType1
{
	vec4   ff;  // offset: 0, align: 16, size: 16
	uvec2  uu;  // offset: 16, align: 8, size: 8
	ivec2  ii;  // offset: 24, align: 8, size: 8
};

Buffer {
	layout(offset=0, align=16)   StType1  st;      // size: 32
	layout(offset=32, align=16)  uvec4    ua [4];  // size: 64
}
)#";
		const String	ref_msl = R"#(
struct StType1
{
	float4  ff;  // offset: 0, align: 16, size: 16
	uint2   uu;  // offset: 16, align: 8, size: 8
	int2    ii;  // offset: 24, align: 8, size: 8
};
static_assert( sizeof(StType1) == 32, "size mismatch" );

struct StType2
{
	StType1  st;      // offset: 0, align: 16, size: 32
	uint4    ua [4];  // offset: 32, align: 16, size: 64
};
static_assert( sizeof(StType2) == 96, "size mismatch" );

)#";
		const String	ref_cpp = R"#(
#ifndef StType1_DEFINED
#	define StType1_DEFINED
	// size: 32, align: 16
	struct StType1
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xd9e31216u}};

		float4  ff;
		uint2   uu;
		int2    ii;
	};
#endif
	StaticAssert( offsetof(StType1, ff) == 0 );
	StaticAssert( offsetof(StType1, uu) == 16 );
	StaticAssert( offsetof(StType1, ii) == 24 );
	StaticAssert( sizeof(StType1) == 32 );

#ifndef StType2_DEFINED
#	define StType2_DEFINED
	// size: 96, align: 16
	struct StType2
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x40ea43acu}};

		StType1                    st;
		StaticArray< uint4, 4 >    ua;
	};
#endif
	StaticAssert( offsetof(StType2, st) == 0 );
	StaticAssert( offsetof(StType2, ua) == 32 );
	StaticAssert( sizeof(StType2) == 96 );

)#";
		const String	ref_hlsl = R"#(
// size: 32, align: 16
struct StType1
{
	vector<float,4>     ff;  // offset: 0, align: 16, size: 16
	vector<uint32_t,2>  uu;  // offset: 16, align: 8, size: 8
	vector<int32_t,2>   ii;  // offset: 24, align: 8, size: 8
};

// size: 96, align: 16
struct StType2
{
	StType1             st;      // offset: 0, align: 16, size: 32
	vector<uint32_t,4>  ua [4];  // offset: 32, align: 16, size: 64
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType1_DEFINED
#	define StType1_DEFINED
	// size: 32, align: 16
	struct StType1
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xd9e31216u}};

		float4  ff;
		uint2   uu;
		int2    ii;
	};
#endif
	StaticAssert( offsetof(StType1, ff) == 0 );
	StaticAssert( offsetof(StType1, uu) == 16 );
	StaticAssert( offsetof(StType1, ii) == 24 );
	StaticAssert( sizeof(StType1) == 32 );

#ifndef StType2_DEFINED
#	define StType2_DEFINED
	// size: 96, align: 16
	struct StType2
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x40ea43acu}};

		StType1                    st;
		StaticArray< uint4, 4 >    ua;
	};
#endif
	StaticAssert( offsetof(StType2, st) == 0 );
	StaticAssert( offsetof(StType2, ua) == 32 );
	StaticAssert( sizeof(StType2) == 96 );
//-----------------------------------------------------------------------------


	static void  StructType_Test5 ()
	{
		ShaderStructTypePtr	st1{ new ShaderStructType{ "StType3" }};
		st1->Set( EStructLayout::Compatible_Std430,
				  "float			f;"
				  "packed_uint3		u;"
				  "packed_int3		i;"	// offset 16
				  "packed_short2	s;"
				);						// offset 32

		TEST( not st1->HasDynamicArray() );
		TEST_Eq( st1->ArrayStride(),	0_b );
		TEST_Eq( st1->StaticSize(),		16_b + 16_b );
		TEST_Eq( st1->Align(),			4_b );


		ShaderStructTypePtr	st2{ new ShaderStructType{ "StType4" }};
		st2->Set( EStructLayout::Compatible_Std430,
				 "packed_half3	h3;"
				 "StType3		st;"		// offset 8
				 "packed_short2	ua [8];"	// offset 40
				);							// offset 72

		TEST( not st2->HasDynamicArray() );
		TEST_Eq( st2->ArrayStride(),	0_b );
		TEST_Eq( st2->StaticSize(),		72_b );
		TEST_Eq( st2->Align(),			4_b );


		const String	glsl = ToGLSL( st2 );
		const String	msl  = ToMSL( st2 );
		const String	cpp  = ToCPP( st2 );
		const String	hlsl = ToHLSL( st2 );

		const String	ref_glsl = R"#(
// size: 6, align: 2
struct packed_half3
{
	float16_t  x;
	float16_t  y;
	float16_t  z;
};
f16vec3  Unpack (const packed_half3 src) { return f16vec3( src.x, src.y, src.z ); }
packed_half3  Pack (const f16vec3 src) { return packed_half3( src.x, src.y, src.z ); }

// size: 12, align: 4
struct packed_uint3
{
	uint  x;
	uint  y;
	uint  z;
};
uvec3  Unpack (const packed_uint3 src) { return uvec3( src.x, src.y, src.z ); }
packed_uint3  Pack (const uvec3 src) { return packed_uint3( src.x, src.y, src.z ); }

// size: 12, align: 4
struct packed_int3
{
	int  x;
	int  y;
	int  z;
};
ivec3  Unpack (const packed_int3 src) { return ivec3( src.x, src.y, src.z ); }
packed_int3  Pack (const ivec3 src) { return packed_int3( src.x, src.y, src.z ); }

// size: 4, align: 2
struct packed_short2
{
	int16_t  x;
	int16_t  y;
};
i16vec2  Unpack (const packed_short2 src) { return i16vec2( src.x, src.y ); }
packed_short2  Pack (const i16vec2 src) { return packed_short2( src.x, src.y ); }

#define StType3_defined
struct StType3
{
	float          f;  // offset: 0, align: 4, size: 4
	packed_uint3   u;  // offset: 4, align: 4, size: 12
	packed_int3    i;  // offset: 16, align: 4, size: 12
	packed_short2  s;  // offset: 28, align: 2, size: 4
};

Buffer {
	layout(offset=0, align=2)   packed_half3   h3;      // size: 6
	layout(offset=8, align=4)   StType3        st;      // size: 32
	layout(offset=40, align=2)  packed_short2  ua [8];  // size: 32
}
)#";
		const String	ref_msl = R"#(
struct StType3
{
	float          f;  // offset: 0, align: 4, size: 4
	packed_uint3   u;  // offset: 4, align: 4, size: 12
	packed_int3    i;  // offset: 16, align: 4, size: 12
	packed_short2  s;  // offset: 28, align: 2, size: 4
};
static_assert( sizeof(StType3) == 32, "size mismatch" );

struct StType4
{
	packed_half3   h3;      // offset: 0, align: 2, size: 6
	StType3        st;      // offset: 8, align: 4, size: 32
	packed_short2  ua [8];  // offset: 40, align: 2, size: 32
};
static_assert( sizeof(StType4) == 72, "size mismatch" );

)#";
		const String	ref_cpp = R"#(
#ifndef StType3_DEFINED
#	define StType3_DEFINED
	// size: 32, align: 4
	struct StType3
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x37ed733au}};

		float          f;
		packed_uint3   u;
		packed_int3    i;
		packed_short2  s;
	};
#endif
	StaticAssert( offsetof(StType3, f) == 0 );
	StaticAssert( offsetof(StType3, u) == 4 );
	StaticAssert( offsetof(StType3, i) == 16 );
	StaticAssert( offsetof(StType3, s) == 28 );
	StaticAssert( sizeof(StType3) == 32 );

#ifndef StType4_DEFINED
#	define StType4_DEFINED
	// size: 72, align: 4
	struct StType4
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xa989e699u}};

		packed_half3                       h3;
		StType3                            st;
		StaticArray< packed_short2, 8 >    ua;
	};
#endif
	StaticAssert( offsetof(StType4, h3) == 0 );
	StaticAssert( offsetof(StType4, st) == 8 );
	StaticAssert( offsetof(StType4, ua) == 40 );
	StaticAssert( sizeof(StType4) == 72 );

)#";
		const String	ref_hlsl = R"#(
// size: 6, align: 2
struct packed_half3
{
	half  x;
	half  y;
	half  z;
};
vector<half,3>  Unpack (const packed_half3 src) { return vector<half,3>( src.x, src.y, src.z ); }
packed_half3  Pack (const vector<half,3> src) { return packed_half3( src.x, src.y, src.z ); }

// size: 12, align: 4
struct packed_uint32_t3
{
	uint32_t  x;
	uint32_t  y;
	uint32_t  z;
};
vector<uint32_t,3>  Unpack (const packed_uint32_t3 src) { return vector<uint32_t,3>( src.x, src.y, src.z ); }
packed_uint32_t3  Pack (const vector<uint32_t,3> src) { return packed_uint32_t3( src.x, src.y, src.z ); }

// size: 12, align: 4
struct packed_int32_t3
{
	int32_t  x;
	int32_t  y;
	int32_t  z;
};
vector<int32_t,3>  Unpack (const packed_int32_t3 src) { return vector<int32_t,3>( src.x, src.y, src.z ); }
packed_int32_t3  Pack (const vector<int32_t,3> src) { return packed_int32_t3( src.x, src.y, src.z ); }

// size: 4, align: 2
struct packed_int16_t2
{
	int16_t  x;
	int16_t  y;
};
vector<int16_t,2>  Unpack (const packed_int16_t2 src) { return vector<int16_t,2>( src.x, src.y ); }
packed_int16_t2  Pack (const vector<int16_t,2> src) { return packed_int16_t2( src.x, src.y ); }

// size: 32, align: 4
struct StType3
{
	float             f;  // offset: 0, align: 4, size: 4
	packed_uint32_t3  u;  // offset: 4, align: 4, size: 12
	packed_int32_t3   i;  // offset: 16, align: 4, size: 12
	packed_int16_t2   s;  // offset: 28, align: 2, size: 4
};

// size: 72, align: 4
struct StType4
{
	packed_half3     h3;      // offset: 0, align: 2, size: 6
	StType3          st;      // offset: 8, align: 4, size: 32
	packed_int16_t2  ua [8];  // offset: 40, align: 2, size: 32
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType3_DEFINED
#	define StType3_DEFINED
	// size: 32, align: 4
	struct StType3
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x37ed733au}};

		float          f;
		packed_uint3   u;
		packed_int3    i;
		packed_short2  s;
	};
#endif
	StaticAssert( offsetof(StType3, f) == 0 );
	StaticAssert( offsetof(StType3, u) == 4 );
	StaticAssert( offsetof(StType3, i) == 16 );
	StaticAssert( offsetof(StType3, s) == 28 );
	StaticAssert( sizeof(StType3) == 32 );

#ifndef StType4_DEFINED
#	define StType4_DEFINED
	// size: 72, align: 4
	struct StType4
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xa989e699u}};

		packed_half3                       h3;
		StType3                            st;
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
		ShaderStructTypePtr	st{ new ShaderStructType{ "StType5" }};
		st->Set( EStructLayout::Compatible_Std140,
				 "packed_float3		Position;"
				 "packed_float3		Normal;"
				 "packed_float3		Texcoord;" );

		TEST( not st->HasDynamicArray() );
		TEST_Eq( st->ArrayStride(),	0_b );
		TEST_Eq( st->StaticSize(),		(4_b * 3) + (4_b * 3) + (4_b * 3) );
		TEST_Eq( st->Align(),			16_b );


		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
// size: 12, align: 4
#define inplace_float3( _name_ ) \
	float  _name_ ## _x; \
	float  _name_ ## _y; \
	float  _name_ ## _z
#define GetInplaceFloat3( _fieldName_ )  vec3( (_fieldName_ ## _x), (_fieldName_ ## _y), (_fieldName_ ## _z) )
#define SetInplaceFloat3( _fieldName_, _src_ )  {(_fieldName_ ## _x = (_src_).x), (_fieldName_ ## _y = (_src_).y), (_fieldName_ ## _z = (_src_).z)}

Buffer {
	layout(offset=0, align=4)   inplace_float3  ( Position );  // size: 12
	layout(offset=12, align=4)  inplace_float3  ( Normal );    // size: 12
	layout(offset=24, align=4)  inplace_float3  ( Texcoord );  // size: 12
}
)#";
		const String	ref_msl = R"#(
struct StType5
{
	packed_float3  Position;  // offset: 0, align: 4, size: 12
	packed_float3  Normal;    // offset: 12, align: 4, size: 12
	packed_float3  Texcoord;  // offset: 24, align: 4, size: 12
};
static_assert( sizeof(StType5) == 36, "size mismatch" );

)#";
		const String	ref_cpp = R"#(
#ifndef StType5_DEFINED
#	define StType5_DEFINED
	// size: 36, align: 4 (16)
	struct StType5
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xde8ed60fu}};

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
		const String	ref_hlsl = R"#(
// size: 12, align: 4
#define inplace_float3( _name_ ) \
	float  _name_ ## _x; \
	float  _name_ ## _y; \
	float  _name_ ## _z
#define GetInplaceFloat3( _fieldName_ )  vector<float,3>( (_fieldName_ ## _x), (_fieldName_ ## _y), (_fieldName_ ## _z) )
#define SetInplaceFloat3( _fieldName_, _src_ )  {(_fieldName_ ## _x = (_src_).x), (_fieldName_ ## _y = (_src_).y), (_fieldName_ ## _z = (_src_).z)}

// size: 36, align: 4
struct StType5
{
	inplace_float3  ( Position );  // offset: 0, align: 4, size: 12
	inplace_float3  ( Normal );    // offset: 12, align: 4, size: 12
	inplace_float3  ( Texcoord );  // offset: 24, align: 4, size: 12
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType5_DEFINED
#	define StType5_DEFINED
	// size: 36, align: 4 (16)
	struct StType5
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xde8ed60fu}};

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
		ShaderStructTypePtr	st{ new ShaderStructType{ "StType6" }};
		st->Set( EStructLayout::Compatible_Std430,
				 "uint				Count;"
				 "packed_float2		Positions [3];"
				 "uint				Indices [3];"
				 "packed_float3x3	Mat [2];"
				 "float2			Positions2 [3];"
				 "uint				Indices2 [3];"
				 "float3x3			Mat2 [2];" );

		TEST( not st->HasDynamicArray() );
		TEST_Eq( st->ArrayStride(), 0_b );

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
// size: 8, align: 4
struct packed_float2
{
	float  x;
	float  y;
};
vec2  Unpack (const packed_float2 src) { return vec2( src.x, src.y ); }
packed_float2  Pack (const vec2 src) { return packed_float2( src.x, src.y ); }

// size: 12, align: 4
struct packed_float3
{
	float  x;
	float  y;
	float  z;
};
vec3  Unpack (const packed_float3 src) { return vec3( src.x, src.y, src.z ); }
packed_float3  Pack (const vec3 src) { return packed_float3( src.x, src.y, src.z ); }

// size: 36, align: 4
struct packed_float3x3
{
	packed_float3  c0;
	packed_float3  c1;
	packed_float3  c2;
};
mat3x3  Unpack (const packed_float3x3 src) { return mat3x3( Unpack(src.c0), Unpack(src.c1), Unpack(src.c2) ); }
packed_float3x3  Pack (const mat3x3 src) { return packed_float3x3( Pack(src[0]), Pack(src[1]), Pack(src[2]) ); }

Buffer {
	layout(offset=0, align=4)     uint             Count;           // size: 4
	layout(offset=4, align=4)     packed_float2    Positions [3];   // size: 24
	layout(offset=28, align=4)    uint             Indices [3];     // size: 12
	layout(offset=40, align=4)    packed_float3x3  Mat [2];         // size: 72
	layout(offset=112, align=8)   vec2             Positions2 [3];  // size: 24
	layout(offset=136, align=4)   uint             Indices2 [3];    // size: 12
	layout(offset=160, align=16)  mat3x3           Mat2 [2];        // size: 96
}
)#";
		const String	ref_msl = R"#(
// size: 72, align: 4
struct packed_float3x3
{
	packed_float3  c0;
	packed_float3  c1;
	packed_float3  c2;

	float3x3 cast () const { return float3x3( float3(c0), float3(c1), float3(c2) ); }
};
struct StType6
{
	uint             Count;           // offset: 0, align: 4, size: 4
	packed_float2    Positions [3];   // offset: 4, align: 4, size: 24
	uint             Indices [3];     // offset: 28, align: 4, size: 12
	packed_float3x3  Mat [2];         // offset: 40, align: 4, size: 72
	float2           Positions2 [3];  // offset: 112, align: 8, size: 24
	uint             Indices2 [3];    // offset: 136, align: 4, size: 12
	float3x3         Mat2 [2];        // offset: 160, align: 16, size: 96
};
static_assert( sizeof(StType6) == 256, "size mismatch" );

)#";
		const String	ref_cpp = R"#(
#ifndef StType6_DEFINED
#	define StType6_DEFINED
	// size: 256, align: 16
	struct StType6
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x478787b5u}};

		uint                                         Count;
		StaticArray< packed_float2, 3 >              Positions;
		StaticArray< uint, 3 >                       Indices;
		StaticArray< packed_float3x3_storage, 2 >    Mat;
		StaticArray< float2, 3 >                     Positions2;
		StaticArray< uint, 3 >                       Indices2;
		StaticArray< float3x3_storage, 2 >           Mat2;
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
		const String	ref_hlsl = R"#(
// size: 8, align: 4
struct packed_float2
{
	float  x;
	float  y;
};
vector<float,2>  Unpack (const packed_float2 src) { return vector<float,2>( src.x, src.y ); }
packed_float2  Pack (const vector<float,2> src) { return packed_float2( src.x, src.y ); }

// size: 12, align: 4
struct packed_float3
{
	float  x;
	float  y;
	float  z;
};
vector<float,3>  Unpack (const packed_float3 src) { return vector<float,3>( src.x, src.y, src.z ); }
packed_float3  Pack (const vector<float,3> src) { return packed_float3( src.x, src.y, src.z ); }

// size: 36, align: 4
struct packed_float3x3
{
	packed_float3  c0;
	packed_float3  c1;
	packed_float3  c2;
};
matrix<float,3,3>  Unpack (const packed_float3x3 src) { return matrix<float,3,3>( Unpack(src.c0), Unpack(src.c1), Unpack(src.c2) ); }
packed_float3x3  Pack (const matrix<float,3,3> src) { return packed_float3x3( Pack(src[0]), Pack(src[1]), Pack(src[2]) ); }

// size: 256, align: 16
struct StType6
{
	uint32_t           Count;           // offset: 0, align: 4, size: 4
	packed_float2      Positions [3];   // offset: 4, align: 4, size: 24
	uint32_t           Indices [3];     // offset: 28, align: 4, size: 12
	packed_float3x3    Mat [2];         // offset: 40, align: 4, size: 72
	vector<float,2>    Positions2 [3];  // offset: 112, align: 8, size: 24
	uint32_t           Indices2 [3];    // offset: 136, align: 4, size: 12
	matrix<float,3,3>  Mat2 [2];        // offset: 160, align: 16, size: 96
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType6_DEFINED
#	define StType6_DEFINED
	// size: 256, align: 16
	struct StType6
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x478787b5u}};

		uint                                         Count;
		StaticArray< packed_float2, 3 >              Positions;
		StaticArray< uint, 3 >                       Indices;
		StaticArray< packed_float3x3_storage, 2 >    Mat;
		StaticArray< float2, 3 >                     Positions2;
		StaticArray< uint, 3 >                       Indices2;
		StaticArray< float3x3_storage, 2 >           Mat2;
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
		ShaderStructTypePtr	st{ new ShaderStructType{ "StType8" }};
		st->Set( EStructLayout::Compatible_Std140,
				 "float4	pos [];" );

		TEST( st->HasDynamicArray() );
		TEST_Eq( st->ArrayStride(), 16_b );

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
Buffer {
	layout(offset=0, align=16)  vec4  pos [];
}
)#";
		const String	ref_msl = R"#(
struct StType8
{
	device float4*  pos;  // offset: 0, align: 16, size: 16
};

)#";
		const String	ref_cpp = R"#(
#ifndef StType8_DEFINED
#	define StType8_DEFINED
	// size: 0, align: 16
	struct StType8
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xa03faab2u}};
		static constexpr size_t SizeOf (size_t count)  { return (16 * count); }

	//	float4  pos [];
	};
#endif

)#";
		// equal to 'StructuredBuffer<float4>' without additional structures
		const String	ref_hlsl = R"#(
)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType8_DEFINED
#	define StType8_DEFINED
	// size: 0, align: 16
	struct StType8
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xa03faab2u}};
		static constexpr size_t SizeOf (size_t count)  { return (16 * count); }

	//	float4  pos [];
	};
#endif
//-----------------------------------------------------------------------------


	static void  StructType_Test9 ()
	{
		ShaderStructTypePtr	st1{ new ShaderStructType{ "StType9A" }};
		st1->AddUsage( ShaderStructType::EUsage::BufferReference );
		st1->Set( EStructLayout::Compatible_Std140,
				  "float4	pos;"
				  "float3	norm;" );

		ShaderStructTypePtr	st2{ new ShaderStructType{ "StType9B" }};
		st2->AddUsage( ShaderStructType::EUsage::BufferReference );
		st2->Set( EStructLayout::Compatible_Std140,
				  "float2	a;"
				  "int		b;" );

		ShaderStructTypePtr	st{ new ShaderStructType{ "StType9" }};
		st->Set( EStructLayout::Compatible_Std140,
				 "StType9A &	ref;"			// reference to single object
				 "float2 *		arr;"			// dynamic array of 'float2'
				 "StType9B *	st_arr;" );		// dynamic array of 'StType9B'

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		// GLSL will generate buffer reference types with '_AERef' and '_AEPtr' suffixes
		const String	ref_glsl = R"#(
layout(std140, buffer_reference, buffer_reference_align=16) buffer StType9A_AERef
{
	layout(offset=0, align=16)   vec4  pos;   // size: 16
	layout(offset=16, align=16)  vec3  norm;  // size: 16
};

layout(std430, buffer_reference, buffer_reference_align=8) buffer float2_AEPtr
{
	vec2  data [];
};

layout(std140, buffer_reference, buffer_reference_align=16) buffer StType9B_AERef
{
	layout(offset=0, align=8)  vec2  a;  // size: 8
	layout(offset=8, align=4)  int   b;  // size: 4
};

layout(std430, buffer_reference, buffer_reference_align=16) buffer StType9B_AEPtr
{
	StType9B_AERef  data [];
};

Buffer {
	layout(offset=0, align=8)   StType9A_AERef  ref;     // size: 8
	layout(offset=8, align=8)   float2_AEPtr    arr;     // size: 8
	layout(offset=16, align=8)  StType9B_AEPtr  st_arr;  // size: 8
}
)#";
		// MSL supports pointers
		const String	ref_msl = R"#(
struct StType9A
{
	float4  pos;   // offset: 0, align: 16, size: 16
	float3  norm;  // offset: 16, align: 16, size: 16
};
static_assert( sizeof(StType9A) == 32, "size mismatch" );

struct StType9B
{
	float2  a;  // offset: 0, align: 8, size: 8
	int     b;  // offset: 8, align: 4, size: 4
};
static_assert( sizeof(StType9B) == 16, "size mismatch" );

struct StType9
{
	device StType9A*  ref;     // offset: 0, align: 8, size: 8
	device float2*    arr;     // offset: 8, align: 8, size: 8
	device StType9B*  st_arr;  // offset: 16, align: 8, size: 8
};
static_assert( sizeof(StType9) == 24, "size mismatch" );

)#";
		// in C++ it must be 'uint64' address to GPU memory
		const String	ref_cpp = R"#(
#ifndef StType9A_DEFINED
#	define StType9A_DEFINED
	// size: 32, align: 16
	struct StType9A
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xef0d42c0u}};

		float4  pos;
		float3  norm;
	};
#endif
	StaticAssert( offsetof(StType9A, pos) == 0 );
	StaticAssert( offsetof(StType9A, norm) == 16 );
	StaticAssert( sizeof(StType9A) == 32 );

#ifndef StType9B_DEFINED
#	define StType9B_DEFINED
	// size: 12 (16), align: 8 (16)
	struct alignas(8) StType9B
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x7604137au}};

		float2  a;
		int     b;
	};
#endif
	StaticAssert( offsetof(StType9B, a) == 0 );
	StaticAssert( offsetof(StType9B, b) == 8 );
	StaticAssert( sizeof(StType9B) == 16 );

#ifndef StType9_DEFINED
#	define StType9_DEFINED
	// size: 24, align: 8 (16)
	struct StType9
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xd7389a24u}};

		TDeviceAddress< StType9A >   ref;
		TDeviceAddress< float2 *>    arr;
		TDeviceAddress< StType9B *>  st_arr;
	};
#endif
	StaticAssert( offsetof(StType9, ref) == 0 );
	StaticAssert( offsetof(StType9, arr) == 8 );
	StaticAssert( offsetof(StType9, st_arr) == 16 );
	StaticAssert( sizeof(StType9) == 24 );

)#";
		const String	ref_hlsl = R"#(
// size: 32, align: 16
struct StType9A
{
	vector<float,4>  pos;              // offset: 0, align: 16, size: 16
	vector<float,3>  norm;             // offset: 16, align: 16, size: 16
	float            _norm_padding_w;  // offset: 28, align: 4, size: 4
};

// size: 12, align: 8
struct StType9B
{
	vector<float,2>  a;  // offset: 0, align: 8, size: 8
	int32_t          b;  // offset: 8, align: 4, size: 4
};

// size: 24, align: 8
struct StType9
{
	StType9A *         ref;     // offset: 0, align: 8, size: 8
	vector<float,2> *  arr;     // offset: 8, align: 8, size: 8
	StType9B *         st_arr;  // offset: 16, align: 8, size: 8
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType9A_DEFINED
#	define StType9A_DEFINED
	// size: 32, align: 16
	struct StType9A
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xef0d42c0u}};

		float4  pos;
		float3  norm;
	};
#endif
	StaticAssert( offsetof(StType9A, pos) == 0 );
	StaticAssert( offsetof(StType9A, norm) == 16 );
	StaticAssert( sizeof(StType9A) == 32 );

#ifndef StType9B_DEFINED
#	define StType9B_DEFINED
	// size: 12 (16), align: 8 (16)
	struct alignas(8) StType9B
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x7604137au}};

		float2  a;
		int     b;
	};
#endif
	StaticAssert( offsetof(StType9B, a) == 0 );
	StaticAssert( offsetof(StType9B, b) == 8 );
	StaticAssert( sizeof(StType9B) == 16 );

#ifndef StType9_DEFINED
#	define StType9_DEFINED
	// size: 24, align: 8 (16)
	struct StType9
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xd7389a24u}};

		TDeviceAddress< StType9A >   ref;
		TDeviceAddress< float2 *>    arr;
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
		ShaderStructTypePtr	st{ new ShaderStructType{ "StType.10" }};
		st->Set( EStructLayout::Compatible_Std140,
				 "float4	pos;" );

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
Buffer {
	layout(offset=0, align=16)  vec4  pos;  // size: 16
}
)#";
		const String	ref_msl = R"#(
struct StType_10
{
	float4  pos;  // offset: 0, align: 16, size: 16
};
static_assert( sizeof(StType_10) == 16, "size mismatch" );

)#";
		const String	ref_cpp = R"#(
#ifndef StType_10_DEFINED
#	define StType_10_DEFINED
	// size: 16, align: 16
	struct StType_10
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xc35deb7u}};

		float4  pos;
	};
#endif
	StaticAssert( offsetof(StType_10, pos) == 0 );
	StaticAssert( sizeof(StType_10) == 16 );

)#";
		const String	ref_hlsl = R"#(
// size: 16, align: 16
struct StType_10
{
	vector<float,4>  pos;  // offset: 0, align: 16, size: 16
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType_10_DEFINED
#	define StType_10_DEFINED
	// size: 16, align: 16
	struct StType_10
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xc35deb7u}};

		float4  pos;
	};
#endif
	StaticAssert( offsetof(StType_10, pos) == 0 );
	StaticAssert( sizeof(StType_10) == 16 );
//-----------------------------------------------------------------------------


	static void  StructType_Test11 ()
	{
		ShaderStructTypePtr	st{ new ShaderStructType{ "StType11" }};
		st->Set( EStructLayout::Compatible_Std140,
				 "float4x4	transform;"
				 "uint		meshIdx;"
				 "uint		materialIdx;" );

		TEST_Eq( st->StaticSize(), 80_b );
		TEST_Eq( st->Align(), 16_b );

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
Buffer {
	layout(offset=0, align=16)  mat4x4  transform;    // size: 64
	layout(offset=64, align=4)  uint    meshIdx;      // size: 4
	layout(offset=68, align=4)  uint    materialIdx;  // size: 4
}
)#";
		const String	ref_msl = R"#(
struct StType11
{
	float4x4  transform;    // offset: 0, align: 16, size: 64
	uint      meshIdx;      // offset: 64, align: 4, size: 4
	uint      materialIdx;  // offset: 68, align: 4, size: 4
};
static_assert( sizeof(StType11) == 80, "size mismatch" );

)#";
		const String	ref_cpp = R"#(
#ifndef StType11_DEFINED
#	define StType11_DEFINED
	// size: 72 (80), align: 16
	struct alignas(16) StType11
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x77d1b9f4u}};

		float4x4_storage_std140  transform;
		uint                     meshIdx;
		uint                     materialIdx;
	};
#endif
	StaticAssert( offsetof(StType11, transform) == 0 );
	StaticAssert( offsetof(StType11, meshIdx) == 64 );
	StaticAssert( offsetof(StType11, materialIdx) == 68 );
	StaticAssert( sizeof(StType11) == 80 );

)#";
		const String	ref_hlsl = R"#(
// size: 72, align: 16
struct StType11
{
	matrix<float,4,4>  transform;    // offset: 0, align: 16, size: 64
	uint32_t           meshIdx;      // offset: 64, align: 4, size: 4
	uint32_t           materialIdx;  // offset: 68, align: 4, size: 4
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType11_DEFINED
#	define StType11_DEFINED
	// size: 72 (80), align: 16
	struct alignas(16) StType11
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x77d1b9f4u}};

		float4x4_storage  transform;
		uint              meshIdx;
		uint              materialIdx;
	};
#endif
	StaticAssert( offsetof(StType11, transform) == 0 );
	StaticAssert( offsetof(StType11, meshIdx) == 64 );
	StaticAssert( offsetof(StType11, materialIdx) == 68 );
	StaticAssert( sizeof(StType11) == 80 );
//-----------------------------------------------------------------------------


	static void  StructType_Test12 ()
	{
		ShaderStructTypePtr	st{ new ShaderStructType{ "StType12" }};
		st->Set( EStructLayout::Compatible_Std430,
				 "uint		count;"
				 "float4	arr [];");

		TEST_Eq( st->StaticSize(),	16_b );
		TEST_Eq( st->Align(),		16_b );
		TEST_Eq( st->ArrayStride(),	16_b );

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
Buffer {
	layout(offset=0, align=4)    uint  count;   // size: 4
	layout(offset=16, align=16)  vec4  arr [];
}
)#";
		const String	ref_msl = R"#(
struct StType12
{
	uint            count;  // offset: 0, align: 4, size: 4
	device float4*  arr;    // offset: 16, align: 16, size: 16
};

)#";
		const String	ref_cpp = R"#(
#ifndef StType12_DEFINED
#	define StType12_DEFINED
	// size: 4 (16), align: 16
	struct alignas(16) StType12
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xeed8e84eu}};
		static constexpr size_t SizeOf (size_t count)  { return 16 + (16 * count); }

		uint    count;
	//	float4  arr [];
	};
#endif
	StaticAssert( offsetof(StType12, count) == 0 );
	StaticAssert( sizeof(StType12) == 16 );

)#";
		const String	ref_hlsl = R"#(
)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType12_DEFINED
#	define StType12_DEFINED
	// size: 4 (16), align: 16
	struct alignas(16) StType12
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xeed8e84eu}};
		static constexpr size_t SizeOf (size_t count)  { return 16 + (16 * count); }

		uint    count;
	//	float4  arr [];
	};
#endif
	StaticAssert( offsetof(StType12, count) == 0 );
	StaticAssert( sizeof(StType12) == 16 );
//-----------------------------------------------------------------------------


	static void  StructType_Test13 ()
	{
		ShaderStructTypePtr	st1{ new ShaderStructType{ "StType13A" }};
		st1->AddUsage( ShaderStructType::EUsage::BufferReference );
		st1->Set( EStructLayout::Compatible_Std140,
				  "float4	pos;"
				  "float3	norm;"
				  "ulong	l;" );

		ShaderStructTypePtr	st{ new ShaderStructType{ "StType13" }};
		st->Set( EStructLayout::Compatible_Std430,
				 "DeviceAddress *	untypedAddrArr;"
				 "DeviceAddress *	untypedAddrArrArr [8];"
				 "StType13A &		typedRef;"
				 "StType13A *		typedArr;"
				 "StType13A *		typedArrArr [7];" );

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
layout(std430, buffer_reference, buffer_reference_align=8) buffer DeviceAddress_AEPtr
{
	uvec2  data [];
};

layout(std140, buffer_reference, buffer_reference_align=16) buffer StType13A_AERef
{
	layout(offset=0, align=16)   vec4      pos;   // size: 16
	layout(offset=16, align=16)  vec3      norm;  // size: 16
	layout(offset=32, align=8)   uint64_t  l;     // size: 8
};

layout(std430, buffer_reference, buffer_reference_align=16) buffer StType13A_AEPtr
{
	StType13A_AERef  data [];
};

Buffer {
	layout(offset=0, align=8)   DeviceAddress_AEPtr  untypedAddrArr;         // size: 8
	layout(offset=8, align=8)   DeviceAddress_AEPtr  untypedAddrArrArr [8];  // size: 64
	layout(offset=72, align=8)  StType13A_AERef      typedRef;               // size: 8
	layout(offset=80, align=8)  StType13A_AEPtr      typedArr;               // size: 8
	layout(offset=88, align=8)  StType13A_AEPtr      typedArrArr [7];        // size: 56
}
)#";
		const String	ref_msl = R"#(
struct StType13A
{
	float4  pos;   // offset: 0, align: 16, size: 16
	float3  norm;  // offset: 16, align: 16, size: 16
	ulong   l;     // offset: 32, align: 8, size: 8
};
static_assert( sizeof(StType13A) == 48, "size mismatch" );

struct StType13
{
	device void*       untypedAddrArr;         // offset: 0, align: 8, size: 8
	device void*       untypedAddrArrArr [8];  // offset: 8, align: 8, size: 64
	device StType13A*  typedRef;               // offset: 72, align: 8, size: 8
	device StType13A*  typedArr;               // offset: 80, align: 8, size: 8
	device StType13A*  typedArrArr [7];        // offset: 88, align: 8, size: 56
};
static_assert( sizeof(StType13) == 144, "size mismatch" );

)#";
		const String	ref_cpp = R"#(
#ifndef StType13A_DEFINED
#	define StType13A_DEFINED
	// size: 40 (48), align: 16
	struct alignas(16) StType13A
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x5b481bb5u}};

		float4  pos;
		float3  norm;
		ulong   l;
	};
#endif
	StaticAssert( offsetof(StType13A, pos) == 0 );
	StaticAssert( offsetof(StType13A, norm) == 16 );
	StaticAssert( offsetof(StType13A, l) == 32 );
	StaticAssert( sizeof(StType13A) == 48 );

#ifndef StType13_DEFINED
#	define StType13_DEFINED
	// size: 144, align: 8
	struct StType13
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x99dfd8d8u}};

		TDeviceAddress< DeviceAddress *>                      untypedAddrArr;
		StaticArray< TDeviceAddress< DeviceAddress *>, 8 >    untypedAddrArrArr;
		TDeviceAddress< StType13A >                           typedRef;
		TDeviceAddress< StType13A *>                          typedArr;
		StaticArray< TDeviceAddress< StType13A *>, 7 >        typedArrArr;
	};
#endif
	StaticAssert( offsetof(StType13, untypedAddrArr) == 0 );
	StaticAssert( offsetof(StType13, untypedAddrArrArr) == 8 );
	StaticAssert( offsetof(StType13, typedRef) == 72 );
	StaticAssert( offsetof(StType13, typedArr) == 80 );
	StaticAssert( offsetof(StType13, typedArrArr) == 88 );
	StaticAssert( sizeof(StType13) == 144 );

)#";
		const String	ref_hlsl = R"#(
// size: 40, align: 16
struct StType13A
{
	vector<float,4>  pos;              // offset: 0, align: 16, size: 16
	vector<float,3>  norm;             // offset: 16, align: 16, size: 16
	float            _norm_padding_w;  // offset: 28, align: 4, size: 4
	uint64_t         l;                // offset: 32, align: 8, size: 8
};

// size: 144, align: 8
struct StType13
{
	DeviceAddress *  untypedAddrArr;         // offset: 0, align: 8, size: 8
	DeviceAddress *  untypedAddrArrArr [8];  // offset: 8, align: 8, size: 64
	StType13A *      typedRef;               // offset: 72, align: 8, size: 8
	StType13A *      typedArr;               // offset: 80, align: 8, size: 8
	StType13A *      typedArrArr [7];        // offset: 88, align: 8, size: 56
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType13A_DEFINED
#	define StType13A_DEFINED
	// size: 40 (48), align: 16
	struct alignas(16) StType13A
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x5b481bb5u}};

		float4  pos;
		float3  norm;
		ulong   l;
	};
#endif
	StaticAssert( offsetof(StType13A, pos) == 0 );
	StaticAssert( offsetof(StType13A, norm) == 16 );
	StaticAssert( offsetof(StType13A, l) == 32 );
	StaticAssert( sizeof(StType13A) == 48 );

#ifndef StType13_DEFINED
#	define StType13_DEFINED
	// size: 144, align: 8
	struct StType13
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x99dfd8d8u}};

		TDeviceAddress< DeviceAddress *>                      untypedAddrArr;
		StaticArray< TDeviceAddress< DeviceAddress *>, 8 >    untypedAddrArrArr;
		TDeviceAddress< StType13A >                           typedRef;
		TDeviceAddress< StType13A *>                          typedArr;
		StaticArray< TDeviceAddress< StType13A *>, 7 >        typedArrArr;
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
		ShaderStructTypePtr	st{ new ShaderStructType{ "StType14" }};
		st->Set( EStructLayout::Compatible_Std430,
				 "packed_float3 *	normals;" );

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
// size: 12, align: 4
struct packed_float3
{
	float  x;
	float  y;
	float  z;
};
vec3  Unpack (const packed_float3 src) { return vec3( src.x, src.y, src.z ); }
packed_float3  Pack (const vec3 src) { return packed_float3( src.x, src.y, src.z ); }

layout(std430, buffer_reference, buffer_reference_align=4) buffer packed_float3_AEPtr
{
	packed_float3  data [];
};

Buffer {
	layout(offset=0, align=8)  packed_float3_AEPtr  normals;  // size: 8
}
)#";
		const String	ref_msl = R"#(
struct StType14
{
	device packed_float3*  normals;  // offset: 0, align: 8, size: 8
};
static_assert( sizeof(StType14) == 8, "size mismatch" );

)#";
		const String	ref_cpp = R"#(
#ifndef StType14_DEFINED
#	define StType14_DEFINED
	// size: 8, align: 8
	struct StType14
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x7bb4d7bu}};

		TDeviceAddress< packed_float3 *>  normals;
	};
#endif
	StaticAssert( offsetof(StType14, normals) == 0 );
	StaticAssert( sizeof(StType14) == 8 );

)#";
		const String	ref_hlsl = R"#(
// size: 12, align: 4
struct packed_float3
{
	float  x;
	float  y;
	float  z;
};
vector<float,3>  Unpack (const packed_float3 src) { return vector<float,3>( src.x, src.y, src.z ); }
packed_float3  Pack (const vector<float,3> src) { return packed_float3( src.x, src.y, src.z ); }

// size: 8, align: 8
struct StType14
{
	packed_float3 *  normals;  // offset: 0, align: 8, size: 8
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType14_DEFINED
#	define StType14_DEFINED
	// size: 8, align: 8
	struct StType14
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x7bb4d7bu}};

		TDeviceAddress< packed_float3 *>  normals;
	};
#endif
	StaticAssert( offsetof(StType14, normals) == 0 );
	StaticAssert( sizeof(StType14) == 8 );
//-----------------------------------------------------------------------------


	static void  StructType_Test15 ()
	{
		ShaderStructTypePtr	st1{ new ShaderStructType{ "StType15A" }};
		st1->Set( EStructLayout::Compatible_Std430,
				  "uint3 	a;"
				  "float	b;" );

		ShaderStructTypePtr	st{ new ShaderStructType{ "StType15" }};
		st->Set( EStructLayout::Compatible_Std430,
				 "StType15A	arr [8];" );

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
#define StType15A_defined
struct StType15A
{
	uvec3  a;             // offset: 0, align: 16, size: 16
	uint   _a_padding_w;  // offset: 12, align: 4, size: 4
	float  b;             // offset: 16, align: 4, size: 4
};

Buffer {
	layout(offset=0, align=16)  StType15A  arr [8];  // size: 256
}
)#";
		const String	ref_msl = R"#(
struct StType15A
{
	uint3  a;  // offset: 0, align: 16, size: 16
	float  b;  // offset: 16, align: 4, size: 4
};
static_assert( sizeof(StType15A) == 32, "size mismatch" );

struct StType15
{
	StType15A  arr [8];  // offset: 0, align: 16, size: 256
};
static_assert( sizeof(StType15) == 256, "size mismatch" );

)#";
		const String	ref_cpp = R"#(
#ifndef StType15A_DEFINED
#	define StType15A_DEFINED
	// size: 20 (32), align: 16
	struct alignas(16) StType15A
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xd12bc33u}};

		uint3  a;
		float  b;
	};
#endif
	StaticAssert( offsetof(StType15A, a) == 0 );
	StaticAssert( offsetof(StType15A, b) == 16 );
	StaticAssert( sizeof(StType15A) == 32 );

#ifndef StType15_DEFINED
#	define StType15_DEFINED
	// size: 256, align: 16
	struct StType15
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x70bc7dedu}};

		StaticArray< StType15A, 8 >    arr;
	};
#endif
	StaticAssert( offsetof(StType15, arr) == 0 );
	StaticAssert( sizeof(StType15) == 256 );

)#";
		const String	ref_hlsl = R"#(
// size: 20, align: 16
struct StType15A
{
	vector<uint32_t,3>  a;             // offset: 0, align: 16, size: 16
	uint32_t            _a_padding_w;  // offset: 12, align: 4, size: 4
	float               b;             // offset: 16, align: 4, size: 4
};

// size: 256, align: 16
struct StType15
{
	StType15A  arr [8];  // offset: 0, align: 16, size: 256
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType15A_DEFINED
#	define StType15A_DEFINED
	// size: 20 (32), align: 16
	struct alignas(16) StType15A
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xd12bc33u}};

		uint3  a;
		float  b;
	};
#endif
	StaticAssert( offsetof(StType15A, a) == 0 );
	StaticAssert( offsetof(StType15A, b) == 16 );
	StaticAssert( sizeof(StType15A) == 32 );

#ifndef StType15_DEFINED
#	define StType15_DEFINED
	// size: 256, align: 16
	struct StType15
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x70bc7dedu}};

		StaticArray< StType15A, 8 >    arr;
	};
#endif
	StaticAssert( offsetof(StType15, arr) == 0 );
	StaticAssert( sizeof(StType15) == 256 );
//-----------------------------------------------------------------------------


	static void  StructType_Test16 ()
	{
		ShaderStructTypePtr	st1{ new ShaderStructType{ "StType16A" }};
		st1->Set( EStructLayout::Compatible_Std430,
				  "float	a;"
				  "float	b;"
				  "float	c;" );

		ShaderStructTypePtr	st{ new ShaderStructType{ "StType16" }};
		st->Set( EStructLayout::Compatible_Std430,
				 "float2	aa;"
				 "float2	bb;"
				 "float		cc;"
				 "StType16A	arr [];" );

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
#define StType16A_defined
struct StType16A
{
	float  a;  // offset: 0, align: 4, size: 4
	float  b;  // offset: 4, align: 4, size: 4
	float  c;  // offset: 8, align: 4, size: 4
};

Buffer {
	layout(offset=0, align=8)   vec2       aa;      // size: 8
	layout(offset=8, align=8)   vec2       bb;      // size: 8
	layout(offset=16, align=4)  float      cc;      // size: 4
	layout(offset=20, align=4)  StType16A  arr [];
}
)#";
		const String	ref_msl = R"#(
struct StType16A
{
	float  a;  // offset: 0, align: 4, size: 4
	float  b;  // offset: 4, align: 4, size: 4
	float  c;  // offset: 8, align: 4, size: 4
};
static_assert( sizeof(StType16A) == 12, "size mismatch" );

struct StType16
{
	float2             aa;   // offset: 0, align: 8, size: 8
	float2             bb;   // offset: 8, align: 8, size: 8
	float              cc;   // offset: 16, align: 4, size: 4
	device StType16A*  arr;  // offset: 20, align: 4, size: 12
};

)#";
		const String	ref_cpp = R"#(
#ifndef StType16A_DEFINED
#	define StType16A_DEFINED
	// size: 12, align: 4
	struct StType16A
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x263feff0u}};

		float  a;
		float  b;
		float  c;
	};
#endif
	StaticAssert( offsetof(StType16A, a) == 0 );
	StaticAssert( offsetof(StType16A, b) == 4 );
	StaticAssert( offsetof(StType16A, c) == 8 );
	StaticAssert( sizeof(StType16A) == 12 );

#ifndef StType16_DEFINED
#	define StType16_DEFINED
	// size: 20 (24), align: 8
	struct alignas(8) StType16
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xe9b52c57u}};
		static constexpr size_t SizeOf (size_t count)  { return 20 + (12 * count); }

		float2     aa;
		float2     bb;
		float      cc;
	//	StType16A  arr [];
	};
#endif
	StaticAssert( offsetof(StType16, aa) == 0 );
	StaticAssert( offsetof(StType16, bb) == 8 );
	StaticAssert( offsetof(StType16, cc) == 16 );
	StaticAssert( sizeof(StType16) == 24 );

)#";
		const String	ref_hlsl = R"#(
)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType16A_DEFINED
#	define StType16A_DEFINED
	// size: 12, align: 4
	struct StType16A
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x263feff0u}};

		float  a;
		float  b;
		float  c;
	};
#endif
	StaticAssert( offsetof(StType16A, a) == 0 );
	StaticAssert( offsetof(StType16A, b) == 4 );
	StaticAssert( offsetof(StType16A, c) == 8 );
	StaticAssert( sizeof(StType16A) == 12 );

#ifndef StType16_DEFINED
#	define StType16_DEFINED
	// size: 20 (24), align: 8
	struct alignas(8) StType16
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xe9b52c57u}};
		static constexpr size_t SizeOf (size_t count)  { return 20 + (12 * count); }

		float2     aa;
		float2     bb;
		float      cc;
	//	StType16A  arr [];
	};
#endif
	StaticAssert( offsetof(StType16, aa) == 0 );
	StaticAssert( offsetof(StType16, bb) == 8 );
	StaticAssert( offsetof(StType16, cc) == 16 );
	StaticAssert( sizeof(StType16) == 24 );
//-----------------------------------------------------------------------------


	static void  StructType_Test17 ()
	{
		ShaderStructTypePtr	st{ new ShaderStructType{ "StType17" }};
		st->Set( EStructLayout::InternalIO,
				 "float2		a;"
				 "half2			b;"
				 "flat float	c;"
				 "mediump float	d;"
				 "ubyte_norm4	e;"
				 "ushort3		f;" );

		String	glsl, msl, hlsl;
		try {
			ShaderStructType::UniqueTypes_t		unique;
			glsl = st->ToShaderIO_GLSL( EShader::Fragment, true, unique );		// throw

			unique.clear();
			msl = st->ToShaderIO_MSL( EShader::Fragment, true, unique );		// throw

			unique.clear();
			hlsl = st->ToShaderIO_HLSL( EShader::Fragment, true, unique );		// throw
		}
		catch (...) {
			TEST(false);
		}

		const String	ref_glsl = R"#(// stage input
in FragmentInput {
  layout(location=0)         vec2     a;
  layout(location=1)         f16vec2  b;
  layout(location=2) flat    float    c;
  layout(location=3) mediump float    d;
  layout(location=4) mediump vec4     e;
  layout(location=5) flat    u16vec3  f;
} In;

)#";
		const String	ref_msl	= R"#(struct StType17
{
  float4             position  [[position]];
  float2             a;
  half2              b;
  float              c         [[flat]];
  float              d;
  rgba8unorm<half4>  e;
  ushort3            f         [[flat]];
};

)#";
		const String	ref_hlsl = R"#()#";	// TODO

		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}
//-----------------------------------------------------------------------------


	static void  StructType_Test18 ()
	{
		ShaderStructTypePtr	st{ new ShaderStructType{ "StType18" }};
		st->Set( EStructLayout::Std430,
				 "float3		a;"		// size: 16
				 "float			b;"		// size: 4		offset: 16
				 "packed_float3	c;"		// size: 12		offset: 20
				 "packed_float3	d;"		// size: 12		offset: 32	- non-packed for GLSL
				 "uint			e;"		// size: 4		offset: 44
				 "float2		f;"		// size: 8		offset: 48
				 "packed_float3	g;"		// size: 12		offset: 56
				 "packed_uint2	h;"		// size: 8		offset: 68
				);

		const String	glsl = ToGLSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
// size: 12, align: 4
struct packed_float3
{
	float  x;
	float  y;
	float  z;
};
vec3  Unpack (const packed_float3 src) { return vec3( src.x, src.y, src.z ); }
packed_float3  Pack (const vec3 src) { return packed_float3( src.x, src.y, src.z ); }

// size: 8, align: 4
struct packed_uint2
{
	uint  x;
	uint  y;
};
uvec2  Unpack (const packed_uint2 src) { return uvec2( src.x, src.y ); }
packed_uint2  Pack (const uvec2 src) { return packed_uint2( src.x, src.y ); }

Buffer {
	layout(offset=0, align=16)   vec3           a;  // size: 16
	layout(offset=16, align=4)   float          b;  // size: 4
	layout(offset=20, align=4)   packed_float3  c;  // size: 12
	layout(offset=32, align=16)  vec3           d;  // size: 12
	layout(offset=44, align=4)   uint           e;  // size: 4
	layout(offset=48, align=8)   vec2           f;  // size: 8
	layout(offset=56, align=4)   packed_float3  g;  // size: 12
	layout(offset=68, align=4)   packed_uint2   h;  // size: 8
}
)#";
		const String	ref_cpp = R"#(
#ifndef StType18_DEFINED
#	define StType18_DEFINED
	// size: 76 (80), align: 16
	struct alignas(16) StType18
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xe0d0150u}};

		float3         a;
		float          b;
		packed_float3  c;
		packed_float3  d;
		uint           e;
		float2         f;
		packed_float3  g;
		packed_uint2   h;
	};
#endif
	StaticAssert( offsetof(StType18, a) == 0 );
	StaticAssert( offsetof(StType18, b) == 16 );
	StaticAssert( offsetof(StType18, c) == 20 );
	StaticAssert( offsetof(StType18, d) == 32 );
	StaticAssert( offsetof(StType18, e) == 44 );
	StaticAssert( offsetof(StType18, f) == 48 );
	StaticAssert( offsetof(StType18, g) == 56 );
	StaticAssert( offsetof(StType18, h) == 68 );
	StaticAssert( sizeof(StType18) == 80 );

)#";
		const String	ref_hlsl = R"#(
// size: 12, align: 4
struct packed_float3
{
	float  x;
	float  y;
	float  z;
};
vector<float,3>  Unpack (const packed_float3 src) { return vector<float,3>( src.x, src.y, src.z ); }
packed_float3  Pack (const vector<float,3> src) { return packed_float3( src.x, src.y, src.z ); }

// size: 8, align: 4
struct packed_uint32_t2
{
	uint32_t  x;
	uint32_t  y;
};
vector<uint32_t,2>  Unpack (const packed_uint32_t2 src) { return vector<uint32_t,2>( src.x, src.y ); }
packed_uint32_t2  Pack (const vector<uint32_t,2> src) { return packed_uint32_t2( src.x, src.y ); }

// size: 76, align: 16
struct StType18
{
	vector<float,3>   a;             // offset: 0, align: 16, size: 16
	float             _a_padding_w;  // offset: 12, align: 4, size: 4
	float             b;             // offset: 16, align: 4, size: 4
	packed_float3     c;             // offset: 20, align: 4, size: 12
	vector<float,3>   d;             // offset: 32, align: 16, size: 12
	uint32_t          e;             // offset: 44, align: 4, size: 4
	vector<float,2>   f;             // offset: 48, align: 8, size: 8
	packed_float3     g;             // offset: 56, align: 4, size: 12
	packed_uint32_t2  h;             // offset: 68, align: 4, size: 8
};

)#";
		TEST( glsl == ref_glsl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType18_DEFINED
#	define StType18_DEFINED
	// size: 76 (80), align: 16
	struct alignas(16) StType18
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xe0d0150u}};

		float3         a;
		float          b;
		packed_float3  c;
		packed_float3  d;
		uint           e;
		float2         f;
		packed_float3  g;
		packed_uint2   h;
	};
#endif
	StaticAssert( offsetof(StType18, a) == 0 );
	StaticAssert( offsetof(StType18, b) == 16 );
	StaticAssert( offsetof(StType18, c) == 20 );
	StaticAssert( offsetof(StType18, d) == 32 );
	StaticAssert( offsetof(StType18, e) == 44 );
	StaticAssert( offsetof(StType18, f) == 48 );
	StaticAssert( offsetof(StType18, g) == 56 );
	StaticAssert( offsetof(StType18, h) == 68 );
	StaticAssert( sizeof(StType18) == 80 );
//-----------------------------------------------------------------------------


	static void  StructType_Test19 ()
	{
		ShaderStructTypePtr	st{ new ShaderStructType{ "StType19" }};
		st->Set( EStructLayout::Compatible_Std430,
				 "float3		a;"
				 "float			b;"
				 "packed_float3	c;"
				 "packed_float3	d;"
				 "float			e;" );

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
// size: 12, align: 4
struct packed_float3
{
	float  x;
	float  y;
	float  z;
};
vec3  Unpack (const packed_float3 src) { return vec3( src.x, src.y, src.z ); }
packed_float3  Pack (const vec3 src) { return packed_float3( src.x, src.y, src.z ); }

Buffer {
	layout(offset=0, align=16)  vec3           a;  // size: 16
	layout(offset=16, align=4)  float          b;  // size: 4
	layout(offset=20, align=4)  packed_float3  c;  // size: 12
	layout(offset=32, align=4)  packed_float3  d;  // size: 12
	layout(offset=44, align=4)  float          e;  // size: 4
}
)#";
		const String	ref_msl = R"#(
struct StType19
{
	float3         a;  // offset: 0, align: 16, size: 16
	float          b;  // offset: 16, align: 4, size: 4
	packed_float3  c;  // offset: 20, align: 4, size: 12
	packed_float3  d;  // offset: 32, align: 4, size: 12
	float          e;  // offset: 44, align: 4, size: 4
};
static_assert( sizeof(StType19) == 48, "size mismatch" );

)#";
		const String	ref_cpp = R"#(
#ifndef StType19_DEFINED
#	define StType19_DEFINED
	// size: 48, align: 16
	struct StType19
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x790a31c6u}};

		float3         a;
		float          b;
		packed_float3  c;
		packed_float3  d;
		float          e;
	};
#endif
	StaticAssert( offsetof(StType19, a) == 0 );
	StaticAssert( offsetof(StType19, b) == 16 );
	StaticAssert( offsetof(StType19, c) == 20 );
	StaticAssert( offsetof(StType19, d) == 32 );
	StaticAssert( offsetof(StType19, e) == 44 );
	StaticAssert( sizeof(StType19) == 48 );

)#";
		const String	ref_hlsl = R"#(
// size: 12, align: 4
struct packed_float3
{
	float  x;
	float  y;
	float  z;
};
vector<float,3>  Unpack (const packed_float3 src) { return vector<float,3>( src.x, src.y, src.z ); }
packed_float3  Pack (const vector<float,3> src) { return packed_float3( src.x, src.y, src.z ); }

// size: 48, align: 16
struct StType19
{
	vector<float,3>  a;             // offset: 0, align: 16, size: 16
	float            _a_padding_w;  // offset: 12, align: 4, size: 4
	float            b;             // offset: 16, align: 4, size: 4
	packed_float3    c;             // offset: 20, align: 4, size: 12
	packed_float3    d;             // offset: 32, align: 4, size: 12
	float            e;             // offset: 44, align: 4, size: 4
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType19_DEFINED
#	define StType19_DEFINED
	// size: 48, align: 16
	struct StType19
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x790a31c6u}};

		float3         a;
		float          b;
		packed_float3  c;
		packed_float3  d;
		float          e;
	};
#endif
	StaticAssert( offsetof(StType19, a) == 0 );
	StaticAssert( offsetof(StType19, b) == 16 );
	StaticAssert( offsetof(StType19, c) == 20 );
	StaticAssert( offsetof(StType19, d) == 32 );
	StaticAssert( offsetof(StType19, e) == 44 );
	StaticAssert( sizeof(StType19) == 48 );
//-----------------------------------------------------------------------------


	static void  StructType_Test20 ()
	{
		ShaderStructTypePtr	st1{ new ShaderStructType{ "StType20A" }};
		st1->Set( EStructLayout::Std430,
				 "float3		a;"		// size: 16
				 "float			b;"		// size: 4		offset: 16
				 "packed_float3	c;"		// size: 12		offset: 20	- packed
				 "packed_float3	d;"		// size: 12		offset: 32	- non-packed for GLSL
				 "uint			e;"		// size: 4		offset: 44
				 "float2		f;"		// size: 8		offset: 48
				 "packed_float3	g;"		// size: 12		offset: 56	- packed
				 "packed_uint2	h;"		// size: 8		offset: 68
				 "float4		aa;"	//				offset: 80
				 "packed_float3	k;"		// size: 12		offset: 96	- non-packed for GLSL
				 "float2		l;"		// size: 8		offset: 112
				 "packed_float3	m;"		// size: 12		offset: 120	- packed
				// end									offset: 132
				);

		ShaderStructTypePtr	st{ new ShaderStructType{ "StType20" }};
		st->Set( EStructLayout::Std430,
				 "StType20A		a;"
				 "uint			b;"
				 "StType20A		arr [];" );

		const String	glsl = ToGLSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
// size: 12, align: 4
struct packed_float3
{
	float  x;
	float  y;
	float  z;
};
vec3  Unpack (const packed_float3 src) { return vec3( src.x, src.y, src.z ); }
packed_float3  Pack (const vec3 src) { return packed_float3( src.x, src.y, src.z ); }

// size: 8, align: 4
struct packed_uint2
{
	uint  x;
	uint  y;
};
uvec2  Unpack (const packed_uint2 src) { return uvec2( src.x, src.y ); }
packed_uint2  Pack (const uvec2 src) { return packed_uint2( src.x, src.y ); }

#define StType20A_defined
struct StType20A
{
	vec3           a;             // offset: 0, align: 16, size: 16
	float          _a_padding_w;  // offset: 12, align: 4, size: 4
	float          b;             // offset: 16, align: 4, size: 4
	packed_float3  c;             // offset: 20, align: 4, size: 12
	vec3           d;             // offset: 32, align: 16, size: 12
	uint           e;             // offset: 44, align: 4, size: 4
	vec2           f;             // offset: 48, align: 8, size: 8
	packed_float3  g;             // offset: 56, align: 4, size: 12
	packed_uint2   h;             // offset: 68, align: 4, size: 8
	vec4           aa;            // offset: 80, align: 16, size: 16
	vec3           k;             // offset: 96, align: 16, size: 16
	float          _k_padding_w;  // offset: 108, align: 4, size: 4
	vec2           l;             // offset: 112, align: 8, size: 8
	packed_float3  m;             // offset: 120, align: 4, size: 12
};

Buffer {
	layout(offset=0, align=16)    StType20A  a;       // size: 144
	layout(offset=144, align=4)   uint       b;       // size: 4
	layout(offset=160, align=16)  StType20A  arr [];
}
)#";
		const String	ref_cpp = R"#(
#ifndef StType20A_DEFINED
#	define StType20A_DEFINED
	// size: 132 (144), align: 16
	struct alignas(16) StType20A
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x7223f62fu}};

		float3         a;
		float          b;
		packed_float3  c;
		packed_float3  d;
		uint           e;
		float2         f;
		packed_float3  g;
		packed_uint2   h;
		float4         aa;
		float3         k;
		float2         l;
		packed_float3  m;
	};
#endif
	StaticAssert( offsetof(StType20A, a) == 0 );
	StaticAssert( offsetof(StType20A, b) == 16 );
	StaticAssert( offsetof(StType20A, c) == 20 );
	StaticAssert( offsetof(StType20A, d) == 32 );
	StaticAssert( offsetof(StType20A, e) == 44 );
	StaticAssert( offsetof(StType20A, f) == 48 );
	StaticAssert( offsetof(StType20A, g) == 56 );
	StaticAssert( offsetof(StType20A, h) == 68 );
	StaticAssert( offsetof(StType20A, aa) == 80 );
	StaticAssert( offsetof(StType20A, k) == 96 );
	StaticAssert( offsetof(StType20A, l) == 112 );
	StaticAssert( offsetof(StType20A, m) == 120 );
	StaticAssert( sizeof(StType20A) == 144 );

#ifndef StType20_DEFINED
#	define StType20_DEFINED
	// size: 148 (160), align: 16
	struct alignas(16) StType20
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x2bfbdaa1u}};
		static constexpr size_t SizeOf (size_t count)  { return 160 + (144 * count); }

		StType20A  a;
		uint       b;
	//	StType20A  arr [];
	};
#endif
	StaticAssert( offsetof(StType20, a) == 0 );
	StaticAssert( offsetof(StType20, b) == 144 );
	StaticAssert( sizeof(StType20) == 160 );

)#";
		const String	ref_hlsl = R"#(
)#";
		TEST( glsl == ref_glsl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType20A_DEFINED
#	define StType20A_DEFINED
	// size: 132 (144), align: 16
	struct alignas(16) StType20A
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x7223f62fu}};

		float3         a;
		float          b;
		packed_float3  c;
		packed_float3  d;
		uint           e;
		float2         f;
		packed_float3  g;
		packed_uint2   h;
		float4         aa;
		float3         k;
		float2         l;
		packed_float3  m;
	};
#endif
	StaticAssert( offsetof(StType20A, a) == 0 );
	StaticAssert( offsetof(StType20A, b) == 16 );
	StaticAssert( offsetof(StType20A, c) == 20 );
	StaticAssert( offsetof(StType20A, d) == 32 );
	StaticAssert( offsetof(StType20A, e) == 44 );
	StaticAssert( offsetof(StType20A, f) == 48 );
	StaticAssert( offsetof(StType20A, g) == 56 );
	StaticAssert( offsetof(StType20A, h) == 68 );
	StaticAssert( offsetof(StType20A, aa) == 80 );
	StaticAssert( offsetof(StType20A, k) == 96 );
	StaticAssert( offsetof(StType20A, l) == 112 );
	StaticAssert( offsetof(StType20A, m) == 120 );
	StaticAssert( sizeof(StType20A) == 144 );

#ifndef StType20_DEFINED
#	define StType20_DEFINED
	// size: 148 (160), align: 16
	struct alignas(16) StType20
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x2bfbdaa1u}};
		static constexpr size_t SizeOf (size_t count)  { return 160 + (144 * count); }

		StType20A  a;
		uint       b;
	//	StType20A  arr [];
	};
#endif
	StaticAssert( offsetof(StType20, a) == 0 );
	StaticAssert( offsetof(StType20, b) == 144 );
	StaticAssert( sizeof(StType20) == 160 );
//-----------------------------------------------------------------------------


	static void  StructType_Test21 ()
	{
		ShaderStructTypePtr	st1{ new ShaderStructType{ "StType21A" }};
		st1->Set( EStructLayout::Compatible_Std430,
				  "uint			a;"
				  "uint2		b;" );

		ShaderStructTypePtr	st2{ new ShaderStructType{ "StType21B" }};
		st2->Set( EStructLayout::Compatible_Std430,
				  "StType21A	a;"
				  "uint			b;" );

		ShaderStructTypePtr	st{ new ShaderStructType{ "StType21" }};
		st->Set( EStructLayout::Compatible_Std430,
				 "uint			a;"
				 "StType21B		arr [4];" );

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
#define StType21A_defined
struct StType21A
{
	uint   a;  // offset: 0, align: 4, size: 4
	uvec2  b;  // offset: 8, align: 8, size: 8
};

#define StType21B_defined
struct StType21B
{
	StType21A  a;  // offset: 0, align: 8, size: 16
	uint       b;  // offset: 16, align: 4, size: 4
};

Buffer {
	layout(offset=0, align=4)  uint       a;        // size: 4
	layout(offset=8, align=8)  StType21B  arr [4];  // size: 96
}
)#";
		const String	ref_msl = R"#(
struct StType21A
{
	uint   a;  // offset: 0, align: 4, size: 4
	uint2  b;  // offset: 8, align: 8, size: 8
};
static_assert( sizeof(StType21A) == 16, "size mismatch" );

struct StType21B
{
	StType21A  a;  // offset: 0, align: 8, size: 16
	uint       b;  // offset: 16, align: 4, size: 4
};
static_assert( sizeof(StType21B) == 24, "size mismatch" );

struct StType21
{
	uint       a;        // offset: 0, align: 4, size: 4
	StType21B  arr [4];  // offset: 8, align: 8, size: 96
};
static_assert( sizeof(StType21) == 104, "size mismatch" );

)#";
		const String	ref_cpp = R"#(
#ifndef StType21A_DEFINED
#	define StType21A_DEFINED
	// size: 16, align: 8
	struct StType21A
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x6b38c76eu}};

		uint   a;
		uint2  b;
	};
#endif
	StaticAssert( offsetof(StType21A, a) == 0 );
	StaticAssert( offsetof(StType21A, b) == 8 );
	StaticAssert( sizeof(StType21A) == 16 );

#ifndef StType21B_DEFINED
#	define StType21B_DEFINED
	// size: 20 (24), align: 8
	struct alignas(8) StType21B
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xf23196d4u}};

		StType21A  a;
		uint       b;
	};
#endif
	StaticAssert( offsetof(StType21B, a) == 0 );
	StaticAssert( offsetof(StType21B, b) == 16 );
	StaticAssert( sizeof(StType21B) == 24 );

#ifndef StType21_DEFINED
#	define StType21_DEFINED
	// size: 104, align: 8
	struct StType21
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x5cfcea37u}};

		uint                           a;
		StaticArray< StType21B, 4 >    arr;
	};
#endif
	StaticAssert( offsetof(StType21, a) == 0 );
	StaticAssert( offsetof(StType21, arr) == 8 );
	StaticAssert( sizeof(StType21) == 104 );

)#";
		const String	ref_hlsl = R"#(
// size: 16, align: 8
struct StType21A
{
	uint32_t            a;  // offset: 0, align: 4, size: 4
	vector<uint32_t,2>  b;  // offset: 8, align: 8, size: 8
};

// size: 20, align: 8
struct StType21B
{
	StType21A  a;  // offset: 0, align: 8, size: 16
	uint32_t   b;  // offset: 16, align: 4, size: 4
};

// size: 104, align: 8
struct StType21
{
	uint32_t   a;        // offset: 0, align: 4, size: 4
	StType21B  arr [4];  // offset: 8, align: 8, size: 96
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}


#ifndef StType21A_DEFINED
#	define StType21A_DEFINED
	// size: 16, align: 8
	struct StType21A
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x6b38c76eu}};

		uint   a;
		uint2  b;
	};
#endif
	StaticAssert( offsetof(StType21A, a) == 0 );
	StaticAssert( offsetof(StType21A, b) == 8 );
	StaticAssert( sizeof(StType21A) == 16 );

#ifndef StType21B_DEFINED
#	define StType21B_DEFINED
	// size: 20 (24), align: 8
	struct alignas(8) StType21B
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xf23196d4u}};

		StType21A  a;
		uint       b;
	};
#endif
	StaticAssert( offsetof(StType21B, a) == 0 );
	StaticAssert( offsetof(StType21B, b) == 16 );
	StaticAssert( sizeof(StType21B) == 24 );

#ifndef StType21_DEFINED
#	define StType21_DEFINED
	// size: 104, align: 8
	struct StType21
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0x5cfcea37u}};

		uint                           a;
		StaticArray< StType21B, 4 >    arr;
	};
#endif
	StaticAssert( offsetof(StType21, a) == 0 );
	StaticAssert( offsetof(StType21, arr) == 8 );
	StaticAssert( sizeof(StType21) == 104 );
//-----------------------------------------------------------------------------


	static void  StructType_Test22 ()
	{
		ShaderStructTypePtr	st{ new ShaderStructType{ "StType22" }};
		st->Set( EStructLayout::Compatible_Std430,
				 "uint			a;"
				 "Atomic<uint>	atomic;"
				 "float			f;" );

		const String	glsl = ToGLSL( st );
		const String	msl  = ToMSL( st );
		const String	cpp  = ToCPP( st );
		const String	hlsl = ToHLSL( st );

		const String	ref_glsl = R"#(
Buffer {
	layout(offset=0, align=4)  uint   a;       // size: 4
	layout(offset=4, align=4)  uint   atomic;  // size: 4
	layout(offset=8, align=4)  float  f;       // size: 4
}
)#";
		const String	ref_msl = R"#(
struct StType22
{
	uint   a;       // offset: 0, align: 4, size: 4
	uint   atomic;  // offset: 4, align: 4, size: 4
	float  f;       // offset: 8, align: 4, size: 4
};
static_assert( sizeof(StType22) == 12, "size mismatch" );

)#";
		const String	ref_cpp = R"#(
#ifndef StType22_DEFINED
#	define StType22_DEFINED
	// size: 12, align: 4
	struct StType22
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xc5f5bb8du}};

		uint   a;
		uint   atomic;
		float  f;
	};
#endif
	StaticAssert( offsetof(StType22, a) == 0 );
	StaticAssert( offsetof(StType22, atomic) == 4 );
	StaticAssert( offsetof(StType22, f) == 8 );
	StaticAssert( sizeof(StType22) == 12 );

)#";
		const String	ref_hlsl = R"#(
// size: 12, align: 4
struct StType22
{
	uint32_t            a;       // offset: 0, align: 4, size: 4
	Atomic< uint32_t >  atomic;  // offset: 4, align: 4, size: 4
	float               f;       // offset: 8, align: 4, size: 4
};

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
		TEST( hlsl == ref_hlsl );
		TEST_PASSED();
	}

#ifndef StType22_DEFINED
#	define StType22_DEFINED
	// size: 12, align: 4
	struct StType22
	{
		static constexpr auto   TypeName = ShaderStructName{HashVal32{0xc5f5bb8du}};

		uint   a;
		uint   atomic;
		float  f;
	};
#endif
	StaticAssert( offsetof(StType22, a) == 0 );
	StaticAssert( offsetof(StType22, atomic) == 4 );
	StaticAssert( offsetof(StType22, f) == 8 );
	StaticAssert( sizeof(StType22) == 12 );
//-----------------------------------------------------------------------------


	static void  StructType_TestLayoutCompatibility ()
	{
		ShaderStructTypePtr	st430{ new ShaderStructType{ "StType21_std430" }};
		st430->Set( EStructLayout::Std430, "float  a;" );

		ShaderStructTypePtr	st140{ new ShaderStructType{ "StType21_std140" }};
		st140->Set( EStructLayout::Std140, "float  a;" );

		ShaderStructTypePtr	stC430{ new ShaderStructType{ "StType21_C430" }};
		stC430->Set( EStructLayout::Compatible_Std430, "float  a;" );

		ShaderStructTypePtr	stC140{ new ShaderStructType{ "StType21_C140" }};
		stC140->Set( EStructLayout::Compatible_Std140, "float  a;" );

		ShaderStructTypePtr	stMtl{ new ShaderStructType{ "StType21_mtl" }};
		stMtl->Set( EStructLayout::Metal, "float  a;" );

		ShaderStructTypePtr	stIO{ new ShaderStructType{ "StType21_IO" }};
		stIO->Set( EStructLayout::InternalIO, "float  a;" );

		ShaderStructTypePtr	stHLSLc{ new ShaderStructType{ "StType21_hlslC" }};
		stHLSLc->Set( EStructLayout::HLSL_Const, "float  a;" );

		ShaderStructTypePtr	stHLSLsb{ new ShaderStructType{ "StType21_hlslSB" }};
		stHLSLsb->Set( EStructLayout::HLSL_Struct, "float  a;" );

		StaticAssert( uint(EStructLayout::_Count) == 8 );

		// Std140 is compatible with Std140 and Compatible_Std140
		{
			// error
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std140_std430" }};
				st->Set( EStructLayout::Std140, "StType21_std430  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std140_C430" }};
				st->Set( EStructLayout::Std140, "StType21_C430  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std140_mtl" }};
				st->Set( EStructLayout::Std140, "StType21_mtl  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std140_IO" }};
				st->Set( EStructLayout::Std140, "StType21_IO  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std140_hlslC" }};
				st->Set( EStructLayout::Std140, "StType21_hlslC  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std140_hlslSB" }};
				st->Set( EStructLayout::Std140, "StType21_hlslSB  a;" );
				TEST(false);
			}catch(...) {}

			// ok
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std140_C140" }};
				st->Set( EStructLayout::Std140, "StType21_C140  a;" );
			}catch(...){
				TEST(false);
			}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std140_std140" }};
				st->Set( EStructLayout::Std140, "StType21_std140  a;" );
			}catch(...){
				TEST(false);
			}
		}

		// Std430 is compatible with Std430 and Compatible_Std430
		{
			// error
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std430_std140" }};
				st->Set( EStructLayout::Std430, "StType21_std140  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std430_C140" }};
				st->Set( EStructLayout::Std430, "StType21_C140  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std430_mtl" }};
				st->Set( EStructLayout::Std430, "StType21_mtl  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std430_IO" }};
				st->Set( EStructLayout::Std430, "StType21_IO  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std430_hlslC" }};
				st->Set( EStructLayout::Std430, "StType21_hlslC  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std430_hlslSB" }};
				st->Set( EStructLayout::Std430, "StType21_hlslSB  a;" );
				TEST(false);
			}catch(...) {}

			// ok
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std430_std430" }};
				st->Set( EStructLayout::Std430, "StType21_std430  a;" );
			}catch(...){
				TEST(false);
			}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_std430_C430" }};
				st->Set( EStructLayout::Std430, "StType21_C430  a;" );
			}catch(...){
				TEST(false);
			}
		}

		// Compatible_Std140 is only compatible with Compatible_Std140
		{
			// error
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C140_std430" }};
				st->Set( EStructLayout::Compatible_Std140, "StType21_std430  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C140_C430" }};
				st->Set( EStructLayout::Compatible_Std140, "StType21_C430  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C140_mtl" }};
				st->Set( EStructLayout::Compatible_Std140, "StType21_mtl  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C140_IO" }};
				st->Set( EStructLayout::Compatible_Std140, "StType21_IO  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C140_std140" }};
				st->Set( EStructLayout::Compatible_Std140, "StType21_std140  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C140_hlslC" }};
				st->Set( EStructLayout::Compatible_Std140, "StType21_hlslC  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C140_hlslSB" }};
				st->Set( EStructLayout::Compatible_Std140, "StType21_hlslSB  a;" );
				TEST(false);
			}catch(...) {}

			// ok
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C140_C140" }};
				st->Set( EStructLayout::Compatible_Std140, "StType21_C140  a;" );
			}catch(...){
				TEST(false);
			}
		}

		// Compatible_Std430 is only compatible with Compatible_Std430
		{
			// error
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C430_std140" }};
				st->Set( EStructLayout::Compatible_Std430, "StType21_std140  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C430_C140" }};
				st->Set( EStructLayout::Compatible_Std430, "StType21_C140  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C430_mtl" }};
				st->Set( EStructLayout::Compatible_Std430, "StType21_mtl  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C430_IO" }};
				st->Set( EStructLayout::Compatible_Std430, "StType21_IO  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C430_std430" }};
				st->Set( EStructLayout::Compatible_Std430, "StType21_std430  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C430_hlslC" }};
				st->Set( EStructLayout::Compatible_Std430, "StType21_hlslC  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C430_hlslSB" }};
				st->Set( EStructLayout::Compatible_Std430, "StType21_hlslSB  a;" );
				TEST(false);
			}catch(...) {}

			// ok
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_C430_C430" }};
				st->Set( EStructLayout::Compatible_Std430, "StType21_C430  a;" );
			}catch(...){
				TEST(false);
			}
		}

		// Metal is only compatible with Metal
		{
			// error
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_Mtl_std140" }};
				st->Set( EStructLayout::Metal, "StType21_std140  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_Mtl_C140" }};
				st->Set( EStructLayout::Metal, "StType21_C140  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_Mtl_C430" }};
				st->Set( EStructLayout::Metal, "StType21_C430  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_Mtl_IO" }};
				st->Set( EStructLayout::Metal, "StType21_IO  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_Mtl_std430" }};
				st->Set( EStructLayout::Metal, "StType21_std430  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_Mtl_hlslC" }};
				st->Set( EStructLayout::Metal, "StType21_hlslC  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_Mtl_hlslSB" }};
				st->Set( EStructLayout::Metal, "StType21_hlslSB  a;" );
				TEST(false);
			}catch(...) {}

			// ok
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_Mtl_mtl" }};
				st->Set( EStructLayout::Metal, "StType21_mtl  a;" );
			}catch(...){
				TEST(false);
			}
		}

		// InternalIO is only compatible with InternalIO
		{
			// error
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_IO_std140" }};
				st->Set( EStructLayout::InternalIO, "StType21_std140  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_IO_C140" }};
				st->Set( EStructLayout::InternalIO, "StType21_C140  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_IO_mtl" }};
				st->Set( EStructLayout::InternalIO, "StType21_mtl  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_IO_C430" }};
				st->Set( EStructLayout::InternalIO, "StType21_C430  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_IO_std430" }};
				st->Set( EStructLayout::InternalIO, "StType21_std430  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_IO_hlslC" }};
				st->Set( EStructLayout::InternalIO, "StType21_hlslC  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_IO_hlslSB" }};
				st->Set( EStructLayout::InternalIO, "StType21_hlslSB  a;" );
				TEST(false);
			}catch(...) {}

			// ok
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_IO_IO" }};
				st->Set( EStructLayout::InternalIO, "StType21_IO  a;" );
			}catch(...){
				TEST(false);
			}
		}

		// HLSL_Const is compatible with HLSL_Const and Compatible_std140
		{
			// error
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslC_std140" }};
				st->Set( EStructLayout::HLSL_Const, "StType21_std140  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslC_mtl" }};
				st->Set( EStructLayout::HLSL_Const, "StType21_mtl  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslC_C430" }};
				st->Set( EStructLayout::HLSL_Const, "StType21_C430  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslC_std430" }};
				st->Set( EStructLayout::HLSL_Const, "StType21_std430  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslC_IO" }};
				st->Set( EStructLayout::HLSL_Const, "StType21_IO  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslC_hlslSB" }};
				st->Set( EStructLayout::HLSL_Const, "StType21_hlslSB  a;" );
				TEST(false);
			}catch(...) {}

			// ok
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslC_hlslC" }};
				st->Set( EStructLayout::HLSL_Const, "StType21_hlslC  a;" );
			}catch(...){
				TEST(false);
			}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslC_C140" }};
				st->Set( EStructLayout::HLSL_Const, "StType21_C140  a;" );
			}catch(...){
				TEST(false);
			}
		}

		// HLSL_Struct is compatible with HLSL_Struct and Compatible_std430
		{
			// error
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslSB_std140" }};
				st->Set( EStructLayout::HLSL_Struct, "StType21_std140  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslSB_mtl" }};
				st->Set( EStructLayout::HLSL_Struct, "StType21_mtl  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslSB_C140" }};
				st->Set( EStructLayout::HLSL_Struct, "StType21_C140  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslSB_std430" }};
				st->Set( EStructLayout::HLSL_Struct, "StType21_std430  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslSB_IO" }};
				st->Set( EStructLayout::HLSL_Struct, "StType21_IO  a;" );
				TEST(false);
			}catch(...) {}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslSB_hlslC" }};
				st->Set( EStructLayout::HLSL_Struct, "StType21_hlslC  a;" );
				TEST(false);
			}catch(...) {}

			// ok
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslSB_hlslSB" }};
				st->Set( EStructLayout::HLSL_Struct, "StType21_hlslSB  a;" );
			}catch(...){
				TEST(false);
			}
			try{
				ShaderStructTypePtr	st{ new ShaderStructType{ "StType21_hlslSB_C430" }};
				st->Set( EStructLayout::HLSL_Struct, "StType21_C430  a;" );
			}catch(...){
				TEST(false);
			}
		}

		TEST_PASSED();
	}
//-----------------------------------------------------------------------------
}


extern void  UnitTest_StructType ()
{
	ObjectStorage	obj;
	PipelineStorage	ppln;
	obj.defaultFeatureSet	= "DefaultFS";
	obj.pplnStorage			= &ppln;
	obj.spirvCompiler		= MakeUnique<SpirvCompiler>( Array<Path>{} );
	obj.spirvCompiler->SetDefaultResourceLimits();
	ObjectStorage::SetInstance( &obj );

	#ifdef AE_METAL_TOOLS
		obj.metalCompiler = MakeUnique<MetalCompiler>( ArrayView<Path>{} );
	#endif
	#ifdef AE_ENABLE_SLANG
		obj.slangCompiler = MakeUnique<SLangCompiler>( ArrayView<Path>{} );
	#endif

	ScriptFeatureSetPtr	fs {new ScriptFeatureSet{ "DefaultFS" }};
	fs->fs.Init( FeatureSet::EFeature::RequireTrue );

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
		StructType_Test16();
		StructType_Test17();
		StructType_Test18();
		StructType_Test19();
		StructType_Test20();
		StructType_Test21();
		StructType_Test22();
		StructType_TestLayoutCompatibility();
	} catch(...) {
		TEST( false );
	}

	ObjectStorage::SetInstance( null );
	TEST_PASSED();
}
