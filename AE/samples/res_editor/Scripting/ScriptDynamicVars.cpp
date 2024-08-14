// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptDynamicVars.h"
#include "res_editor/Scripting/ScriptExe.h"

namespace AE::ResEditor
{
	using namespace AE::Scripting;

namespace
{
	template <typename T1, typename T2, typename T3>
	static T1*  ScriptDynamicT_Ctor (T3 value) {
		return ScriptRC<T1>{ new T1{ MakeRCTh<T2>( value )}}.Detach();
	}


} // namespace

//-----------------------------------------------------------------------------
// ScriptDynamicDim

/*
=================================================
	Mul
=================================================
*/
	ScriptDynamicDim*  ScriptDynamicDim::Mul3 (const packed_int3 &value) C_Th___
	{
		CHECK_THROW( All( value > 0 ));

		auto	ds = MakeRC<DynamicDim>( _dynSize );
		ds->SetScale( value );

		ScriptDynamicDimPtr	result{new ScriptDynamicDim{ ds }};

		return result.Detach();
	}

/*
=================================================
	Div
=================================================
*/
	ScriptDynamicDim*  ScriptDynamicDim::Div3 (const packed_int3 &value) C_Th___
	{
		CHECK_THROW( All( value > 0 ));

		auto	ds = MakeRC<DynamicDim>( _dynSize );
		ds->SetScale( -value, DynamicDim::ERounding::Floor );

		ScriptDynamicDimPtr	result{new ScriptDynamicDim{ ds }};

		return result.Detach();
	}

/*
=================================================
	DivRound
=================================================
*/
	ScriptDynamicDim*  ScriptDynamicDim::DivRound3 (const packed_int3 &value) C_Th___
	{
		CHECK_THROW( All( value > 0 ));

		auto	ds = MakeRC<DynamicDim>( _dynSize );
		ds->SetScale( -value, DynamicDim::ERounding::Round );

		ScriptDynamicDimPtr	result{new ScriptDynamicDim{ ds }};

		return result.Detach();
	}

/*
=================================================
	DivCeil
=================================================
*/
	ScriptDynamicDim*  ScriptDynamicDim::DivCeil3 (const packed_int3 &value) C_Th___
	{
		CHECK_THROW( All( value > 0 ));

		auto	ds = MakeRC<DynamicDim>( _dynSize );
		ds->SetScale( -value, DynamicDim::ERounding::Ceil );

		ScriptDynamicDimPtr	result{ new ScriptDynamicDim{ RVRef(ds) }};

		return result.Detach();
	}

/*
=================================================
	X
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicDim::X () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _dynSize->GetDynamicX() }};
		return result.Detach();
	}

/*
=================================================
	Y
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicDim::Y () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _dynSize->GetDynamicY() }};
		return result.Detach();
	}

/*
=================================================
	XY
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicDim::XY () C_Th___
	{
		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ _dynSize->GetDynamicXY() }};
		return result.Detach();
	}

/*
=================================================
	Area
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicDim::Area () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _dynSize->GetDynamicArea() }};
		return result.Detach();
	}

/*
=================================================
	Volume
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicDim::Volume () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _dynSize->GetDynamicVolume() }};
		return result.Detach();
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptDynamicDim::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicDim>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );

		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicDim, DynamicDim, packed_uint3 >, {} );

		binder.AddMethod( &ScriptDynamicDim::Mul1,		"opMul",		{} );
		binder.AddMethod( &ScriptDynamicDim::Div1,		"opDiv",		{} );
		binder.AddMethod( &ScriptDynamicDim::Mul1,		"Mul",			{} );
		binder.AddMethod( &ScriptDynamicDim::Div1,		"Div",			{} );
		binder.AddMethod( &ScriptDynamicDim::DivRound1,	"DivRound",		{} );
		binder.AddMethod( &ScriptDynamicDim::DivCeil1,	"DivCeil",		{} );

		binder.AddMethod( &ScriptDynamicDim::Mul2,		"opMul",		{} );
		binder.AddMethod( &ScriptDynamicDim::Div2,		"opDiv",		{} );
		binder.AddMethod( &ScriptDynamicDim::Mul2,		"Mul",			{} );
		binder.AddMethod( &ScriptDynamicDim::Div2,		"Div",			{} );
		binder.AddMethod( &ScriptDynamicDim::DivRound2,	"DivRound",		{} );
		binder.AddMethod( &ScriptDynamicDim::DivCeil2,	"DivCeil",		{} );

		binder.AddMethod( &ScriptDynamicDim::Mul3,		"opMul",		{} );
		binder.AddMethod( &ScriptDynamicDim::Div3,		"opDiv",		{} );
		binder.AddMethod( &ScriptDynamicDim::Mul3,		"Mul",			{} );
		binder.AddMethod( &ScriptDynamicDim::Div3,		"Div",			{} );
		binder.AddMethod( &ScriptDynamicDim::DivRound3,	"DivRound",		{} );
		binder.AddMethod( &ScriptDynamicDim::DivCeil3,	"DivCeil",		{} );

		binder.AddMethod( &ScriptDynamicDim::X,			"X",			{} );
		binder.AddMethod( &ScriptDynamicDim::Y,			"Y",			{} );
		binder.AddMethod( &ScriptDynamicDim::XY,		"XY",			{} );
		binder.AddMethod( &ScriptDynamicDim::Area,		"Area",			{} );
		binder.AddMethod( &ScriptDynamicDim::Volume,	"Volume",		{} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ScriptDynamicMatrix4x4::Bind
=================================================
*
	void  ScriptDynamicMatrix4x4::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicMatrix4x4>	binder{ se };
		binder.CreateRef();
	}
*/
//-----------------------------------------------------------------------------



/*
=================================================
	ScriptDynamicUInt::Mul
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::Mul (uint value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Mul );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Div
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::Div (uint value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Div );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

	ScriptDynamicUInt*  ScriptDynamicUInt::DivNear (uint value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::DivNear );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

	ScriptDynamicUInt*  ScriptDynamicUInt::DivCeil (uint value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::DivCeil );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Add / Sub
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::Add (uint value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Add );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

	ScriptDynamicUInt*  ScriptDynamicUInt::Sub (uint value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Sub );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Pow
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::Pow (uint value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Pow );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Bind
=================================================
*/
	void  ScriptDynamicUInt::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicUInt>		binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicUInt, DynamicUInt, uint >, {} );
		binder.AddMethod( &ScriptDynamicUInt::Mul,		"Mul",		{} );
		binder.AddMethod( &ScriptDynamicUInt::Div,		"Div",		{} );
		binder.AddMethod( &ScriptDynamicUInt::DivNear,	"DivNear",	{} );
		binder.AddMethod( &ScriptDynamicUInt::DivCeil,	"DivCeil",	{} );
		binder.AddMethod( &ScriptDynamicUInt::Add,		"Add",		{} );
		binder.AddMethod( &ScriptDynamicUInt::Sub,		"Sub",		{} );
		binder.AddMethod( &ScriptDynamicUInt::Pow,		"Pow",		{} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	X
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt2::X () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _value->GetDynamicX() }};
		return result.Detach();
	}

/*
=================================================
	Y
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt2::Y () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _value->GetDynamicY() }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt2::Bind
=================================================
*/
	void  ScriptDynamicUInt2::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicUInt2>		binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicUInt2, DynamicUInt2, const packed_uint2& >, {} );
		binder.AddMethod( &ScriptDynamicUInt2::X,	"X",	{} );
		binder.AddMethod( &ScriptDynamicUInt2::Y,	"Y",	{} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	X
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt3::X () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _value->GetDynamicX() }};
		return result.Detach();
	}

/*
=================================================
	Y
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt3::Y () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _value->GetDynamicY() }};
		return result.Detach();
	}

/*
=================================================
	Z
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt3::Z () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _value->GetDynamicZ() }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt3::Bind
=================================================
*/
	void  ScriptDynamicUInt3::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicUInt3>		binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicUInt3, DynamicUInt3, const packed_uint3& >, {} );
		binder.AddMethod( &ScriptDynamicUInt3::X,	"X",	{} );
		binder.AddMethod( &ScriptDynamicUInt3::Y,	"Y",	{} );
		binder.AddMethod( &ScriptDynamicUInt3::Z,	"Z",	{} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	X
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt4::X () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _value->GetDynamicX() }};
		return result.Detach();
	}

/*
=================================================
	Y
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt4::Y () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _value->GetDynamicY() }};
		return result.Detach();
	}

/*
=================================================
	Z
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt4::Z () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _value->GetDynamicZ() }};
		return result.Detach();
	}

/*
=================================================
	W
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt4::W () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _value->GetDynamicW() }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt4::Bind
=================================================
*/
	void  ScriptDynamicUInt4::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicUInt4>		binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicUInt4, DynamicUInt4, const packed_uint4& >, {} );
		binder.AddMethod( &ScriptDynamicUInt4::X,	"X",	{} );
		binder.AddMethod( &ScriptDynamicUInt4::Y,	"Y",	{} );
		binder.AddMethod( &ScriptDynamicUInt4::Z,	"Z",	{} );
		binder.AddMethod( &ScriptDynamicUInt4::W,	"W",	{} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ScriptDynamicInt::Bind
=================================================
*/
	void  ScriptDynamicInt::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicInt>		binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicInt, DynamicInt, int >, {} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	X
=================================================
*/
	ScriptDynamicInt*  ScriptDynamicInt2::X () C_Th___
	{
		ScriptDynamicIntPtr	result{ new ScriptDynamicInt{ _value->GetDynamicX() }};
		return result.Detach();
	}

/*
=================================================
	Y
=================================================
*/
	ScriptDynamicInt*  ScriptDynamicInt2::Y () C_Th___
	{
		ScriptDynamicIntPtr	result{ new ScriptDynamicInt{ _value->GetDynamicY() }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicInt2::Bind
=================================================
*/
	void  ScriptDynamicInt2::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicInt2>		binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicInt2, DynamicInt2, const packed_int2& >, {} );
		binder.AddMethod( &ScriptDynamicInt2::X,	"X",	{} );
		binder.AddMethod( &ScriptDynamicInt2::Y,	"Y",	{} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	X
=================================================
*/
	ScriptDynamicInt*  ScriptDynamicInt3::X () C_Th___
	{
		ScriptDynamicIntPtr	result{ new ScriptDynamicInt{ _value->GetDynamicX() }};
		return result.Detach();
	}

/*
=================================================
	Y
=================================================
*/
	ScriptDynamicInt*  ScriptDynamicInt3::Y () C_Th___
	{
		ScriptDynamicIntPtr	result{ new ScriptDynamicInt{ _value->GetDynamicY() }};
		return result.Detach();
	}

/*
=================================================
	Z
=================================================
*/
	ScriptDynamicInt*  ScriptDynamicInt3::Z () C_Th___
	{
		ScriptDynamicIntPtr	result{ new ScriptDynamicInt{ _value->GetDynamicZ() }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicInt3::Bind
=================================================
*/
	void  ScriptDynamicInt3::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicInt3>		binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicInt3, DynamicInt3, const packed_int3& >, {} );
		binder.AddMethod( &ScriptDynamicInt3::X,	"X",	{} );
		binder.AddMethod( &ScriptDynamicInt3::Y,	"Y",	{} );
		binder.AddMethod( &ScriptDynamicInt3::Z,	"Z",	{} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	X
=================================================
*/
	ScriptDynamicInt*  ScriptDynamicInt4::X () C_Th___
	{
		ScriptDynamicIntPtr	result{ new ScriptDynamicInt{ _value->GetDynamicX() }};
		return result.Detach();
	}

/*
=================================================
	Y
=================================================
*/
	ScriptDynamicInt*  ScriptDynamicInt4::Y () C_Th___
	{
		ScriptDynamicIntPtr	result{ new ScriptDynamicInt{ _value->GetDynamicY() }};
		return result.Detach();
	}

/*
=================================================
	Z
=================================================
*/
	ScriptDynamicInt*  ScriptDynamicInt4::Z () C_Th___
	{
		ScriptDynamicIntPtr	result{ new ScriptDynamicInt{ _value->GetDynamicZ() }};
		return result.Detach();
	}

/*
=================================================
	W
=================================================
*/
	ScriptDynamicInt*  ScriptDynamicInt4::W () C_Th___
	{
		ScriptDynamicIntPtr	result{ new ScriptDynamicInt{ _value->GetDynamicW() }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicInt4::Bind
=================================================
*/
	void  ScriptDynamicInt4::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicInt4>		binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicInt4, DynamicInt4, const packed_int4& >, {} );
		binder.AddMethod( &ScriptDynamicInt4::X,	"X",	{} );
		binder.AddMethod( &ScriptDynamicInt4::Y,	"Y",	{} );
		binder.AddMethod( &ScriptDynamicInt4::Z,	"Z",	{} );
		binder.AddMethod( &ScriptDynamicInt4::W,	"W",	{} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ScriptDynamicFloat::Bind
=================================================
*/
	void  ScriptDynamicFloat::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicFloat>		binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicFloat, DynamicFloat, float >, {} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	X
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat2::X () C_Th___
	{
		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ _value->GetDynamicX() }};
		return result.Detach();
	}

/*
=================================================
	Y
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat2::Y () C_Th___
	{
		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ _value->GetDynamicY() }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat2::Bind
=================================================
*/
	void  ScriptDynamicFloat2::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicFloat2>	binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicFloat2, DynamicFloat2, const packed_float2& >, {} );
		binder.AddMethod( &ScriptDynamicFloat2::X,	"X",	{} );
		binder.AddMethod( &ScriptDynamicFloat2::Y,	"Y",	{} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	X
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat3::X () C_Th___
	{
		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ _value->GetDynamicX() }};
		return result.Detach();
	}

/*
=================================================
	Y
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat3::Y () C_Th___
	{
		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ _value->GetDynamicY() }};
		return result.Detach();
	}

/*
=================================================
	Z
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat3::Z () C_Th___
	{
		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ _value->GetDynamicZ() }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat3::Bind
=================================================
*/
	void  ScriptDynamicFloat3::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicFloat3>	binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicFloat3, DynamicFloat3, const packed_float3& >, {} );
		binder.AddMethod( &ScriptDynamicFloat3::X,	"X",	{} );
		binder.AddMethod( &ScriptDynamicFloat3::Y,	"Y",	{} );
		binder.AddMethod( &ScriptDynamicFloat3::Z,	"Z",	{} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	X
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat4::X () C_Th___
	{
		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ _value->GetDynamicX() }};
		return result.Detach();
	}

/*
=================================================
	Y
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat4::Y () C_Th___
	{
		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ _value->GetDynamicY() }};
		return result.Detach();
	}

/*
=================================================
	Z
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat4::Z () C_Th___
	{
		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ _value->GetDynamicZ() }};
		return result.Detach();
	}

/*
=================================================
	W
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat4::W () C_Th___
	{
		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ _value->GetDynamicW() }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat4::Bind
=================================================
*/
	void  ScriptDynamicFloat4::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicFloat4>	binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicFloat4, DynamicFloat4, const packed_float4& >, {} );
		binder.AddMethod( &ScriptDynamicFloat4::X,	"X",	{} );
		binder.AddMethod( &ScriptDynamicFloat4::Y,	"Y",	{} );
		binder.AddMethod( &ScriptDynamicFloat4::Z,	"Z",	{} );
		binder.AddMethod( &ScriptDynamicFloat4::W,	"W",	{} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ScriptDynamicULong::Bind
=================================================
*/
	void  ScriptDynamicULong::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptDynamicULong>		binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicULong, DynamicULong, ulong >, {} );
	}


} // AE::ResEditor
