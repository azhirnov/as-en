// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptEngine.inl.h"

namespace AE::Scripting
{
namespace
{
/*
=================================================
	HashFunc
=================================================
*/
	struct HashFunc
	{
		static uint				IHash1i (int x)						{ return uint{XXHash32( &x, sizeof(x) )}; }
		static uint				IHash1u (uint x)					{ return uint{XXHash32( &x, sizeof(x) )}; }
		static uint				IHash1f (float x)					{ return uint{XXHash32( &x, sizeof(x) )}; }

		static uint				IHash21i (const packed_int2 &v)		{ return uint{XXHash32( &v, sizeof(v) )}; }
		static uint				IHash21u (const packed_uint2 &v)	{ return uint{XXHash32( &v, sizeof(v) )}; }
		static uint				IHash21f (const packed_float2 &v)	{ return uint{XXHash32( &v, sizeof(v) )}; }

		static uint				IHash31i (const packed_int3 &v)		{ return uint{XXHash32( &v, sizeof(v) )}; }
		static uint				IHash31u (const packed_uint3 &v)	{ return uint{XXHash32( &v, sizeof(v) )}; }
		static uint				IHash31f (const packed_float3 &v)	{ return uint{XXHash32( &v, sizeof(v) )}; }

		static uint				IHash41i (const packed_int4 &v)		{ return uint{XXHash32( &v, sizeof(v) )}; }
		static uint				IHash41u (const packed_uint4 &v)	{ return uint{XXHash32( &v, sizeof(v) )}; }
		static uint				IHash41f (const packed_float4 &v)	{ return uint{XXHash32( &v, sizeof(v) )}; }

		static packed_uint2		IHash2i (const packed_int2 &v)		{ return packed_uint2{IHash1i( v.x * 48u ),		IHash1i( v.y * 831256u )}; }
		static packed_uint2		IHash2u (const packed_uint2 &v)		{ return packed_uint2{IHash1u( v.x * 48u ),		IHash1u( v.y * 831256u )}; }
		static packed_uint2		IHash2f (const packed_float2 &v)	{ return packed_uint2{IHash1f( v.x * 1.346f ),	IHash1f( v.y * 4.2784f )}; }

		static packed_uint3		IHash3i (const packed_int3 &v)		{ return packed_uint3{IHash1i( v.x * 37u ),		IHash1i( v.y * 3653u ),		IHash1i( v.z * 62587u )}; }
		static packed_uint3		IHash3u (const packed_uint3 &v)		{ return packed_uint3{IHash1u( v.x * 37u ),		IHash1u( v.y * 3653u ),		IHash1u( v.z * 62587u )}; }
		static packed_uint3		IHash3f (const packed_float3 &v)	{ return packed_uint3{IHash1f( v.x * 0.936f ),	IHash1f( v.y * 2.7364f ),	IHash1f( v.z * 5.9276f )}; }

		static packed_uint4		IHash4i (const packed_int4 &v)		{ return packed_uint4{IHash1i( v.x * 21u ),		IHash1i( v.y * 6344u ),		IHash1i( v.z * 50887u ),	IHash1i( v.w * 121u )}; }
		static packed_uint4		IHash4u (const packed_uint4 &v)		{ return packed_uint4{IHash1u( v.x * 21u ),		IHash1u( v.y * 6344u ),		IHash1u( v.z * 50887u ),	IHash1u( v.w * 121u )}; }
		static packed_uint4		IHash4f (const packed_float4 &v)	{ return packed_uint4{IHash1f( v.x * 0.8262f),	IHash1f( v.y * 3.5734f ),	IHash1f( v.z * 1.82438f ),	IHash1f( v.w * 9.89267f )}; }

		// float range [0, 1]
		static float			ItoF (uint n)						{ return BitCast<float>( (n >> 9) | 0x3f800000U ) - 1.0f; }
		static packed_float2	ItoF (const packed_uint2 &n)		{ return packed_float2{ItoF( n.x ), ItoF( n.y )}; }
		static packed_float3	ItoF (const packed_uint3 &n)		{ return packed_float3{ItoF( n.x ), ItoF( n.y ), ItoF( n.z )}; }
		static packed_float4	ItoF (const packed_uint4 &n)		{ return packed_float4{ItoF( n.x ), ItoF( n.y ), ItoF( n.z ), ItoF( n.w )}; }

		static float			FHash1i (int x)						{ return ItoF( IHash1i( x )); }
		static float			FHash1u (uint x)					{ return ItoF( IHash1u( x )); }
		static float			FHash1f (float x)					{ return ItoF( IHash1f( x )); }

		static float			FHash21i (const packed_int2 &v)		{ return ItoF( IHash21i( v )); }
		static float			FHash21u (const packed_uint2 &v)	{ return ItoF( IHash21u( v )); }
		static float			FHash21f (const packed_float2 &v)	{ return ItoF( IHash21f( v )); }

		static float			FHash31i (const packed_int3 &v)		{ return ItoF( IHash31i( v )); }
		static float			FHash31u (const packed_uint3 &v)	{ return ItoF( IHash31u( v )); }
		static float			FHash31f (const packed_float3 &v)	{ return ItoF( IHash31f( v )); }

		static float			FHash41i (const packed_int4 &v)		{ return ItoF( IHash41i( v )); }
		static float			FHash41u (const packed_uint4 &v)	{ return ItoF( IHash41u( v )); }
		static float			FHash41f (const packed_float4 &v)	{ return ItoF( IHash41f( v )); }

		static packed_float2	FHash2i (const packed_int2 &v)		{ return ItoF( IHash2i( v )); }
		static packed_float2	FHash2u (const packed_uint2 &v)		{ return ItoF( IHash2u( v )); }
		static packed_float2	FHash2f (const packed_float2 &v)	{ return ItoF( IHash2f( v )); }

		static packed_float3	FHash3i (const packed_int3 &v)		{ return ItoF( IHash3i( v )); }
		static packed_float3	FHash3u (const packed_uint3 &v)		{ return ItoF( IHash3u( v )); }
		static packed_float3	FHash3f (const packed_float3 &v)	{ return ItoF( IHash3f( v )); }

		static packed_float4	FHash4i (const packed_int4 &v)		{ return ItoF( IHash4i( v )); }
		static packed_float4	FHash4u (const packed_uint4 &v)		{ return ItoF( IHash4u( v )); }
		static packed_float4	FHash4f (const packed_float4 &v)	{ return ItoF( IHash4f( v )); }
	};

/*
=================================================
	Bind_IntHash
=================================================
*/
	static void  Bind_IntHash (const ScriptEnginePtr &se)
	{
		AS_GLOBAL_FN( se, HashFunc::IHash1i,	"IHash" );
		AS_GLOBAL_FN( se, HashFunc::IHash1u,	"IHash" );
		AS_GLOBAL_FN( se, HashFunc::IHash1f,	"IHash" );

		AS_GLOBAL_FN( se, HashFunc::IHash2i,	"IHash" );
		AS_GLOBAL_FN( se, HashFunc::IHash2u,	"IHash" );
		AS_GLOBAL_FN( se, HashFunc::IHash2f,	"IHash" );

		AS_GLOBAL_FN( se, HashFunc::IHash3i,	"IHash" );
		AS_GLOBAL_FN( se, HashFunc::IHash3u,	"IHash" );
		AS_GLOBAL_FN( se, HashFunc::IHash3f,	"IHash" );

		AS_GLOBAL_FN( se, HashFunc::IHash4i,	"IHash" );
		AS_GLOBAL_FN( se, HashFunc::IHash4u,	"IHash" );
		AS_GLOBAL_FN( se, HashFunc::IHash4f,	"IHash" );

		AS_GLOBAL_FN( se, HashFunc::IHash21i,	"IHash1" );
		AS_GLOBAL_FN( se, HashFunc::IHash21u,	"IHash1" );
		AS_GLOBAL_FN( se, HashFunc::IHash21f,	"IHash1" );

		AS_GLOBAL_FN( se, HashFunc::IHash31i,	"IHash1" );
		AS_GLOBAL_FN( se, HashFunc::IHash31u,	"IHash1" );
		AS_GLOBAL_FN( se, HashFunc::IHash31f,	"IHash1" );

		AS_GLOBAL_FN( se, HashFunc::IHash41i,	"IHash1" );
		AS_GLOBAL_FN( se, HashFunc::IHash41u,	"IHash1" );
		AS_GLOBAL_FN( se, HashFunc::IHash41f,	"IHash1" );
	}

/*
=================================================
	Bind_FloatHash
=================================================
*/
	static void  Bind_FloatHash (const ScriptEnginePtr &se)
	{
		AS_GLOBAL_FN( se, HashFunc::FHash1i,	"FHash" );
		AS_GLOBAL_FN( se, HashFunc::FHash1u,	"FHash" );
		AS_GLOBAL_FN( se, HashFunc::FHash1f,	"FHash" );

		AS_GLOBAL_FN( se, HashFunc::FHash2i,	"FHash" );
		AS_GLOBAL_FN( se, HashFunc::FHash2u,	"FHash" );
		AS_GLOBAL_FN( se, HashFunc::FHash2f,	"FHash" );

		AS_GLOBAL_FN( se, HashFunc::FHash3i,	"FHash" );
		AS_GLOBAL_FN( se, HashFunc::FHash3u,	"FHash" );
		AS_GLOBAL_FN( se, HashFunc::FHash3f,	"FHash" );

		AS_GLOBAL_FN( se, HashFunc::FHash4i,	"FHash" );
		AS_GLOBAL_FN( se, HashFunc::FHash4u,	"FHash" );
		AS_GLOBAL_FN( se, HashFunc::FHash4f,	"FHash" );

		AS_GLOBAL_FN( se, HashFunc::FHash21i,	"FHash1" );
		AS_GLOBAL_FN( se, HashFunc::FHash21u,	"FHash1" );
		AS_GLOBAL_FN( se, HashFunc::FHash21f,	"FHash1" );

		AS_GLOBAL_FN( se, HashFunc::FHash31i,	"FHash1" );
		AS_GLOBAL_FN( se, HashFunc::FHash31u,	"FHash1" );
		AS_GLOBAL_FN( se, HashFunc::FHash31f,	"FHash1" );

		AS_GLOBAL_FN( se, HashFunc::FHash41i,	"FHash1" );
		AS_GLOBAL_FN( se, HashFunc::FHash41u,	"FHash1" );
		AS_GLOBAL_FN( se, HashFunc::FHash41f,	"FHash1" );
	}

} // namespace


/*
=================================================
	BindHash
=================================================
*/
	void  CoreBindings::BindHash (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW( se and se->IsInitialized() );

		Bind_IntHash( se );
		Bind_FloatHash( se );
	}

} // AE::Scripting
