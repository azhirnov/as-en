// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
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
			if ( AnyEqual( layout, EStructLayout::Metal, EStructLayout::Compatible_Std430, EStructLayout::Compatible_Std140 ) and HasSubString( t.name, "double" ))
				continue;

			if ( AnyEqual( layout, EStructLayout::Std140, EStructLayout::Compatible_Std140 ) and StartsWith( t.name, "packed_" ))
				continue;

			ShaderStructTypePtr	st{ new ShaderStructType{ "VecTest"s << ToString(idx++) }};

			String	str = String{t.name} << " val";
			if ( arraySize > 1 )
				str << '[' << ToString( arraySize ) << ']';
			str << ';';

			st->Set( layout, str );

			TEST( st->StaticSize() == Bytes{t.size} * arraySize );

			if ( AnyEqual( layout, EStructLayout::Compatible_Std140, EStructLayout::Std140 ))
			{
				TEST( st->Align() == AlignUp( Bytes{t.align}, 16_b ));
			}else{
				TEST( st->Align() == Bytes{t.align} );
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
			if ( AnyEqual( layout, EStructLayout::Metal, EStructLayout::Compatible_Std430 ) and HasSubString( t.name, "double" ))
				continue;

			ShaderStructTypePtr	st{ new ShaderStructType{ "MatTest"s << ToString(idx++) }};
			st->Set( layout, String{t.name} << " val;" );

			TEST( st->StaticSize() == Bytes{t.size} );
			TEST( st->Align() == Bytes{t.align} );
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
		StructType_Test1_Layout( EStructLayout::Metal,				8 );
		StructType_Test1_Layout( EStructLayout::Compatible_Std430,	8 );
		StructType_Test1_Layout( EStructLayout::Std430,				8 );
	}


	static void  StructType_Test4 ()
	{
		ShaderStructTypePtr	st1{ new ShaderStructType{ "StType1" }};
		st1->Set( EStructLayout::Compatible_Std140,
				 "float4	ff;"
				 "uint2		uu;"
				 "int2		ii;" );

		TEST( not st1->HasDynamicArray() );
		TEST( st1->ArrayStride() == 0_b );
		TEST( st1->StaticSize() == 16_b + 16_b );
		TEST( st1->Align() == 16_b );
		

		ShaderStructTypePtr	st2{ new ShaderStructType{ "StType2" }};
		st2->Set( EStructLayout::Compatible_Std140,
				 "StType1	st;"
				 "uint4		ua [4];" );
		
		TEST( not st2->HasDynamicArray() );
		TEST( st2->ArrayStride() == 0_b );
		TEST( st2->StaticSize() == (16_b + 16_b) + (16_b * 4) );
		TEST( st2->Align() == 16_b );


		const String	glsl = ToGLSL( st2 );
		const String	msl  = ToMSL( st2 );
		const String	cpp  = ToCPP( st2 );

		const String	ref_glsl = R"#(
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
		const String	ref_msl = R"#(
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
		const String	ref_cpp = R"#(
#ifndef StType1_DEFINED
#	define StType1_DEFINED
	// size: 32, align: 16
	struct StType1
	{
		static constexpr auto  TypeName = ShaderStructName{"StType1"};

		float4  ff;
		uint2  uu;
		int2  ii;
	};
	STATIC_ASSERT( offsetof(StType1, ff) == 0 );
	STATIC_ASSERT( offsetof(StType1, uu) == 16 );
	STATIC_ASSERT( offsetof(StType1, ii) == 24 );
	STATIC_ASSERT( sizeof(StType1) == 32 );
#endif

#ifndef StType2_DEFINED
#	define StType2_DEFINED
	// size: 96, align: 16
	struct StType2
	{
		static constexpr auto  TypeName = ShaderStructName{"StType2"};

		StType1  st;
		uint4  ua [4];
	};
	STATIC_ASSERT( offsetof(StType2, st) == 0 );
	STATIC_ASSERT( offsetof(StType2, ua) == 32 );
	STATIC_ASSERT( sizeof(StType2) == 96 );
#endif

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
	}

	
	// size: 32, align: 16
	struct StType1
	{
		static constexpr auto  TypeName = ShaderStructName{"StType1"};

		float4  ff;
		uint2  uu;
		int2  ii;
	};
	STATIC_ASSERT( offsetof(StType1, ff) == 0 );
	STATIC_ASSERT( offsetof(StType1, uu) == 16 );
	STATIC_ASSERT( offsetof(StType1, ii) == 24 );
	STATIC_ASSERT( sizeof(StType1) == 32 );

	// size: 96, align: 16
	struct StType2
	{
		static constexpr auto  TypeName = ShaderStructName{"StType2"};

		StType1  st;
		uint4  ua [4];
	};
	STATIC_ASSERT( offsetof(StType2, st) == 0 );
	STATIC_ASSERT( offsetof(StType2, ua) == 32 );
	STATIC_ASSERT( sizeof(StType2) == 96 );

	
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
		TEST( st1->ArrayStride() == 0_b );
		TEST( st1->StaticSize() == 16_b + 16_b );
		TEST( st1->Align() == 4_b );
		

		ShaderStructTypePtr	st2{ new ShaderStructType{ "StType4" }};
		st2->Set( EStructLayout::Compatible_Std430,
				 "packed_half3	h3;"
				 "StType3		st;"		// offset 8
				 "packed_short2	ua [8];"	// offset 40
				);							// offset 72
		
		TEST( not st2->HasDynamicArray() );
		TEST( st2->ArrayStride() == 0_b );
		TEST( st2->StaticSize() == 72_b );
		TEST( st2->Align() == 4_b );


		const String	glsl = ToGLSL( st2 );
		const String	msl  = ToMSL( st2 );
		const String	cpp  = ToCPP( st2 );

		const String	ref_glsl = R"#(
// size: 6 b, align: 2 b
struct float16_packed3
{
	float16_t  x;
	float16_t  y;
	float16_t  z;
};
#define float16_packed3_cast( _src_ )  f16vec3( (_src_.x), (_src_.y), (_src_.z) )

// size: 12 b, align: 4 b
struct uint_packed3
{
	uint  x;
	uint  y;
	uint  z;
};
#define uint_packed3_cast( _src_ )  uvec3( (_src_.x), (_src_.y), (_src_.z) )

// size: 12 b, align: 4 b
struct int_packed3
{
	int  x;
	int  y;
	int  z;
};
#define int_packed3_cast( _src_ )  ivec3( (_src_.x), (_src_.y), (_src_.z) )

// size: 4 b, align: 2 b
struct int16_packed2
{
	int16_t  x;
	int16_t  y;
};
#define int16_packed2_cast( _src_ )  i16vec2( (_src_.x), (_src_.y) )

struct StType3
{
	float  f;  // offset: 0
	uint_packed3  u;  // offset: 4
	int_packed3  i;  // offset: 16
	int16_packed2  s;  // offset: 28
};

Buffer {
	layout(offset=0, align=2) float16_packed3  h3;
	layout(offset=8, align=4) StType3  st;
	layout(offset=40, align=2) int16_packed2  ua [8];
}
)#";
		const String	ref_msl = R"#(
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
		const String	ref_cpp = R"#(
#ifndef StType3_DEFINED
#	define StType3_DEFINED
	// size: 32, align: 4
	struct StType3
	{
		static constexpr auto  TypeName = ShaderStructName{"StType3"};

		float  f;
		packed_uint3  u;
		packed_int3  i;
		packed_short2  s;
	};
	STATIC_ASSERT( offsetof(StType3, f) == 0 );
	STATIC_ASSERT( offsetof(StType3, u) == 4 );
	STATIC_ASSERT( offsetof(StType3, i) == 16 );
	STATIC_ASSERT( offsetof(StType3, s) == 28 );
	STATIC_ASSERT( sizeof(StType3) == 32 );
#endif

#ifndef StType4_DEFINED
#	define StType4_DEFINED
	// size: 72, align: 4
	struct StType4
	{
		static constexpr auto  TypeName = ShaderStructName{"StType4"};

		packed_half3  h3;
		StType3  st;
		packed_short2  ua [8];
	};
	STATIC_ASSERT( offsetof(StType4, h3) == 0 );
	STATIC_ASSERT( offsetof(StType4, st) == 8 );
	STATIC_ASSERT( offsetof(StType4, ua) == 40 );
	STATIC_ASSERT( sizeof(StType4) == 72 );
#endif

)#";
		TEST( glsl == ref_glsl );
		TEST( msl == ref_msl );
		TEST( cpp == ref_cpp );
	}

	
	// size: 32, align: 4
	struct StType3
	{
		static constexpr auto  TypeName = ShaderStructName{"StType3"};

		float  f;
		packed_uint3  u;
		packed_int3  i;
		packed_short2  s;
	};
	STATIC_ASSERT( offsetof(StType3, f) == 0 );
	STATIC_ASSERT( offsetof(StType3, u) == 4 );
	STATIC_ASSERT( offsetof(StType3, i) == 16 );
	STATIC_ASSERT( offsetof(StType3, s) == 28 );
	STATIC_ASSERT( sizeof(StType3) == 32 );

	// size: 72, align: 4
	struct StType4
	{
		static constexpr auto  TypeName = ShaderStructName{"StType4"};

		packed_half3  h3;
		StType3  st;
		packed_short2  ua [8];
	};
	STATIC_ASSERT( offsetof(StType4, h3) == 0 );
	STATIC_ASSERT( offsetof(StType4, st) == 8 );
	STATIC_ASSERT( offsetof(StType4, ua) == 40 );
	STATIC_ASSERT( sizeof(StType4) == 72 );
}


extern void  UnitTest_StructType ()
{
	ObjectStorage	obj;
	PipelineStorage	ppln;
	obj.pplnStorage		= &ppln;
	//obj.metalCompiler	= MakeUnique<MetalCompiler>( ArrayView<Path>{} );
	obj.spirvCompiler	= MakeUnique<SpirvCompiler>( Array<Path>{} );
	obj.spirvCompiler->SetDefaultResourceLimits();
	ObjectStorage::SetInstance( &obj );
	
	try {
		StructType_Test1();
		StructType_Test2();
		StructType_Test3();
		StructType_Test4();
		StructType_Test5();
	} catch(...) {
		TEST( false );
	}
	
	ObjectStorage::SetInstance( null );
	TEST_PASSED();
}
