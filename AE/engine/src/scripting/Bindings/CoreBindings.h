// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "scripting/Bindings/Array.h"

namespace AE::Scripting
{

	//
	// Core Bindings
	//

	struct CoreBindings final : Noninstanceable
	{
	// types
	private:
		using VecTypes = TypeList<
							packed_bool2,	packed_bool3,	packed_bool4,
							packed_sbyte2,	packed_sbyte3,	packed_sbyte4,
							packed_ubyte2,	packed_ubyte3,	packed_ubyte4,
							packed_short2,	packed_short3,	packed_short4,
							packed_ushort2,	packed_ushort3,	packed_ushort4,
							packed_int2,	packed_int3,	packed_int4,
							packed_uint2,	packed_uint3,	packed_uint4,
							packed_float2,	packed_float3,	packed_float4
						>;

		using RectTypes = TypeList< RectU, RectI, RectF >;

		using ColorTypes = TypeList< RGBA32f, RGBA32i, RGBA32u, RGBA8u >;

		using MatTypes = TypeList<
							packed_float2x2, packed_float2x3, packed_float2x4,
							packed_float3x2, packed_float3x3, packed_float3x4,
							packed_float4x2, packed_float4x3, packed_float4x4
						  >;


	// functions
	private:
		static void  _BindVectorMath1 (const ScriptEnginePtr &se)	__Th___;
		static void  _BindVectorMath2 (const ScriptEnginePtr &se)	__Th___;

	public:
		static void  BindStdTypes (const ScriptEnginePtr &se)		__Th___;
		static void  BindScalarMath (const ScriptEnginePtr &se)		__Th___;
		static void  BindVectorMath (const ScriptEnginePtr &se)		__Th___;
		static void  BindMatrixMath (const ScriptEnginePtr &se)		__Th___;	// requires 'Vec', 'Rect'
		static void  BindVectorSwizzle (const ScriptEnginePtr &se)	__Th___;	// requires 'String'
		static void  BindRect (const ScriptEnginePtr &se)			__Th___;
		static void  BindColor (const ScriptEnginePtr &se)			__Th___;
		static void  BindString (const ScriptEnginePtr &se)			__Th___;
		static void  BindArray (const ScriptEnginePtr &se)			__Th___;
		static void  BindMap (const ScriptEnginePtr &se)			__Th___;
		static void  BindLog (const ScriptEnginePtr &se)			__Th___;	// requires 'String'
		static void  BindPhysicalTypes (const ScriptEnginePtr &se)	__Th___;
		static void  BindRandom (const ScriptEnginePtr &se)			__Th___;

		static void  BindToString (const ScriptEnginePtr &se,					// requires 'String'
								   bool scalar,
								   bool vec,
								   bool rect,
								   bool color)						__Th___;
	};

} // AE::Scripting


//
// Math
//

AE_DECL_SCRIPT_OBJ( AE::Math::packed_bool2,			"bool2"		);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_sbyte2,		"sbyte2"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_ubyte2,		"ubyte2"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_short2,		"short2"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_ushort2,		"ushort2"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_int2,			"int2"		);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_uint2,			"uint2"		);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_slong2,		"slong2"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_ulong2,		"ulong2"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_float2,		"float2"	);

AE_DECL_SCRIPT_OBJ( AE::Math::packed_bool3,			"bool3"		);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_sbyte3,		"sbyte3"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_ubyte3,		"ubyte3"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_short3,		"short3"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_ushort3,		"ushort3"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_int3,			"int3"		);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_uint3,			"uint3"		);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_slong3,		"slong3"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_ulong3,		"ulong3"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_float3,		"float3"	);

AE_DECL_SCRIPT_OBJ( AE::Math::packed_bool4,			"bool4"		);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_sbyte4,		"sbyte4"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_ubyte4,		"ubyte4"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_short4,		"short4"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_ushort4,		"ushort4"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_int4,			"int4"		);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_uint4,			"uint4"		);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_slong4,		"slong4"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_ulong4,		"ulong4"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_float4,		"float4"	);

AE_DECL_SCRIPT_OBJ( AE::Math::packed_float2x2,		"float2x2"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_float2x3,		"float2x3"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_float2x4,		"float2x4"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_float3x2,		"float3x2"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_float3x3,		"float3x3"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_float3x4,		"float3x4"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_float4x2,		"float4x2"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_float4x3,		"float4x3"	);
AE_DECL_SCRIPT_OBJ( AE::Math::packed_float4x4,		"float4x4"	);

AE_DECL_SCRIPT_OBJ( AE::Math::VecSwizzle,			"VecSwizzle");

AE_DECL_SCRIPT_OBJ( AE::Math::RGBA32f,				"RGBA32f"	);
AE_DECL_SCRIPT_OBJ( AE::Math::RGBA32u,				"RGBA32u"	);
AE_DECL_SCRIPT_OBJ( AE::Math::RGBA32i,				"RGBA32i"	);
AE_DECL_SCRIPT_OBJ( AE::Math::RGBA8u,				"RGBA8u"	);
AE_DECL_SCRIPT_OBJ( AE::Math::DepthStencil,			"DepthStencil" );
AE_DECL_SCRIPT_OBJ( AE::Math::HSVColor,				"HSVColor"	);

AE_DECL_SCRIPT_OBJ( AE::Math::RectI,				"RectI"	);
AE_DECL_SCRIPT_OBJ( AE::Math::RectU,				"RectU"	);
AE_DECL_SCRIPT_OBJ( AE::Math::RectF,				"RectF"	);



//
// Physical Types
//

#define AE_PHYSICAL_TYPES( _visitor_ )\
	_visitor_( NonDimensional			)\
	_visitor_( Second					)\
	_visitor_( Kilogram					)\
	_visitor_( Meter					)\
	_visitor_( Ampere					)\
	_visitor_( Kelvin					)\
	_visitor_( Mole						)\
	_visitor_( Candela					)\
	_visitor_( Currency					)\
	_visitor_( Bit						)\
	\
	/*_visitor_( Frequency				)*/\
	_visitor_( SquareMeter				)\
	_visitor_( CubicMeter				)\
	_visitor_( MeterPerSecond			)\
	_visitor_( MeterPerSquareSecond		)\
	_visitor_( KilogramPerSecond		)\
	_visitor_( KilogramMeterPerSecond	)\
	_visitor_( KilogramPerCubicMeter	)\
	_visitor_( Newton					)\
	/*_visitor_( NewtonMeter			)*/\
	_visitor_( Joule					)\
	_visitor_( Pascal					)\
	_visitor_( Hertz					)\
	_visitor_( Watt						)\
	_visitor_( Coulomb					)\
	_visitor_( Volt						)\
	_visitor_( Ohm						)\
	_visitor_( Farad					)\
	_visitor_( Weber					)\
	_visitor_( Henry					)\
	_visitor_( Tesla					)\
	_visitor_( Siemens					)\
	/*_visitor_( Lumen					)*/\
	_visitor_( Lux						)\
	_visitor_( AmperPerMeter			)\
	_visitor_( KilogramPerMole			)\
	_visitor_( BitPerSecond				)\
	_visitor_( BytePerSecond			)\
	_visitor_( Byte						)\
	_visitor_( Liter					)\
	_visitor_( Bar						)\
	_visitor_( Atmosphere				)\

#define AE_PHYSICAL_TYPES_VIS( _name_ )\
	AE_DECL_SCRIPT_TYPE( AE::Math::DefaultPhysicalQuantity<float>::_name_, AE_TOSTRING(_name_) );											\
	AE_DECL_SCRIPT_OBJ( AE::Math::PhysicalQuantityPackedVec2< AE::Math::DefaultPhysicalQuantity<float>::_name_>, AE_TOSTRING(_name_ ## 2) );\
	AE_DECL_SCRIPT_OBJ( AE::Math::PhysicalQuantityPackedVec3< AE::Math::DefaultPhysicalQuantity<float>::_name_>, AE_TOSTRING(_name_ ## 3) );

AE_PHYSICAL_TYPES( AE_PHYSICAL_TYPES_VIS );
#undef AE_PHYSICAL_TYPES_VIS
