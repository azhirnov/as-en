// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Scripting/ScriptDynamicVars.h"
#include "Scripting/ScriptExe.h"

namespace AE::ResEditor
{
	using namespace AE::Scripting;

namespace
{
	template <typename T1, typename T2, typename T3>
	static T1*  ScriptDynamicT_Ctor (T3 value) {
		return ScriptRC<T1>{ new T1{ MakeRCTh<T2>( value )}}.Detach();
	}

	template <typename Dst, typename Src>
	static float  DynamicT_Cast (EnableRCBase* base) __NE___
	{
		Src	x = Cast<TDynamicScalar<Src>>(base)->Get();
		return Dst(x);
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
	FloorPOT
=================================================
*/
	ScriptDynamicDim*  ScriptDynamicDim::FloorPOT () C_Th___
	{
		ScriptDynamicDimPtr	result{ new ScriptDynamicDim{ _dynSize->GetFloorPOT() }};
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
	ToFloat2
=================================================
*/
	ScriptDynamicFloat2*  ScriptDynamicDim::ToFloat2 () C_Th___
	{
		ScriptDynamicFloat2Ptr	result{ new ScriptDynamicFloat2{ _dynSize->ToFloat2() }};
		return result.Detach();
	}

/*
=================================================
	Inverse
=================================================
*/
	ScriptDynamicFloat2*  ScriptDynamicDim::Inverse () C_Th___
	{
		ScriptDynamicFloat2Ptr	result{ new ScriptDynamicFloat2{ _dynSize->Inverse() }};
		return result.Detach();
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptDynamicDim::Bind (const ScriptEnginePtr &se) __Th___
	{
		{
			ClassBinder<ScriptDynamicDim>	binder{ se };
			binder.CreateRef( 0, False{"no ctor"} );

			binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicDim, DynamicDim, packed_uint3 >, {} );

			AS_METHOD( binder, ScriptDynamicDim::Mul1,		"opMul",		{} );
			AS_METHOD( binder, ScriptDynamicDim::Div1,		"opDiv",		{} );
			AS_METHOD( binder, ScriptDynamicDim::Mul1,		"Mul",			{} );
			AS_METHOD( binder, ScriptDynamicDim::Div1,		"Div",			{} );
			AS_METHOD( binder, ScriptDynamicDim::DivRound1,	"DivRound",		{} );
			AS_METHOD( binder, ScriptDynamicDim::DivCeil1,	"DivCeil",		{} );

			AS_METHOD( binder, ScriptDynamicDim::Mul2,		"opMul",		{} );
			AS_METHOD( binder, ScriptDynamicDim::Div2,		"opDiv",		{} );
			AS_METHOD( binder, ScriptDynamicDim::Mul2,		"Mul",			{} );
			AS_METHOD( binder, ScriptDynamicDim::Div2,		"Div",			{} );
			AS_METHOD( binder, ScriptDynamicDim::DivRound2,	"DivRound",		{} );
			AS_METHOD( binder, ScriptDynamicDim::DivCeil2,	"DivCeil",		{} );

			AS_METHOD( binder, ScriptDynamicDim::Mul3,		"opMul",		{} );
			AS_METHOD( binder, ScriptDynamicDim::Div3,		"opDiv",		{} );
			AS_METHOD( binder, ScriptDynamicDim::Mul3,		"Mul",			{} );
			AS_METHOD( binder, ScriptDynamicDim::Div3,		"Div",			{} );
			AS_METHOD( binder, ScriptDynamicDim::DivRound3,	"DivRound",		{} );
			AS_METHOD( binder, ScriptDynamicDim::DivCeil3,	"DivCeil",		{} );
			
			AS_METHOD( binder, ScriptDynamicDim::FloorPOT,	"FloorPOT",		{} );
			AS_METHOD( binder, ScriptDynamicDim::X,			"X",			{} );
			AS_METHOD( binder, ScriptDynamicDim::Y,			"Y",			{} );
			AS_METHOD( binder, ScriptDynamicDim::XY,		"XY",			{} );
			AS_METHOD( binder, ScriptDynamicDim::Area,		"Area",			{} );
			AS_METHOD( binder, ScriptDynamicDim::Volume,	"Volume",		{} );

			AS_METHOD( binder, ScriptDynamicDim::ToFloat2,	"ToFloat2",		{} );
			AS_METHOD( binder, ScriptDynamicDim::Inverse,	"Inverse",		{} );
		}
		{
			ClassBinder<ScriptDynamicUInt>	binder{ se };
			AS_METHOD( binder, ScriptDynamicUInt::ToDim2,	"Dimension2",	{} );
			AS_METHOD( binder, ScriptDynamicUInt::ToDim3,	"Dimension3",	{} );
		}
		{
			ClassBinder<ScriptDynamicUInt2>		binder{ se };
			AS_METHOD( binder, ScriptDynamicUInt2::ToDim,	"Dimension",	{} );
		}
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
	ScriptDynamicUInt::PowOf2
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::PowOf2a () __Th___
	{
		return PowOf2b( 1 );
	}

	ScriptDynamicUInt*  ScriptDynamicUInt::PowOf2b (uint value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::PowOf2 );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::FloorPOT
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::FloorPOT () __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( 0, EDynamicVarOperator::FloorPOT );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::CeilPOT
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::CeilPOT () __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( 0, EDynamicVarOperator::CeilPOT );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	ScriptDynamicUInt::NearPOT
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::NearPOT () __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( 0, EDynamicVarOperator::NearPOT );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicDim::ToDim*
=================================================
*/
	ScriptDynamicDim*  ScriptDynamicUInt::ToDim2 () __Th___
	{
		auto	ds = _value->ToDim2();
		ScriptDynamicDimPtr	result{ new ScriptDynamicDim{ RVRef(ds) }};
		return result.Detach();
	}

	ScriptDynamicDim*  ScriptDynamicUInt::ToDim3 () __Th___
	{
		auto	ds = _value->ToDim3();
		ScriptDynamicDimPtr	result{ new ScriptDynamicDim{ RVRef(ds) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Min
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::Min (uint value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Min );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Max
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::Max (uint value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Max );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Add1
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::Add1 (const ScriptDynamicUIntPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Add );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Sub1
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::Sub1 (const ScriptDynamicUIntPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Sub );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Mul1
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::Mul1 (const ScriptDynamicUIntPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Mul );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Div1
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::Div1 (const ScriptDynamicUIntPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Div );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Min1
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::Min1 (const ScriptDynamicUIntPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Min );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Max1
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt::Max1 (const ScriptDynamicUIntPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Max );

		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::ToFloat
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicUInt::ToFloat () __Th___
	{
		auto	du = MakeRC<DynamicFloat>( _value, &DynamicT_Cast<float,uint> );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt::Percent
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicUInt::Percent (const ScriptDynamicUIntPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	p0 = MakeRC<DynamicFloat>( _value,		&DynamicT_Cast<float,uint> );
		auto	p1 = MakeRC<DynamicFloat>( rhs->Get(),	&DynamicT_Cast<float,uint> );

		auto	p2 = p0->Clone();
		p2->SetOp( p1, EDynamicVarOperator::Sub );	// p0 - p1

		auto	p3 = p2->Clone();
		p3->SetOp( p1, EDynamicVarOperator::Div );	// (p0 - p1) / p1

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(p3) }};
		return result.Detach();
	}
	
/*
=================================================
	ScriptDynamicUInt::X1
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt::X1 () __Th___
	{
		auto	ds = _value->ToX1();
		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(ds) }};
		return result.Detach();
	}

	ScriptDynamicUInt3*  ScriptDynamicUInt::X11 () __Th___
	{
		auto	ds = _value->ToX11();
		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(ds) }};
		return result.Detach();
	}
		
	ScriptDynamicUInt2*  ScriptDynamicUInt::XX () __Th___
	{
		auto	ds = _value->ToXX();
		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(ds) }};
		return result.Detach();
	}

	ScriptDynamicUInt3*  ScriptDynamicUInt::XXX () __Th___
	{
		auto	ds = _value->ToXXX();
		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(ds) }};
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
		AS_METHOD( binder, ScriptDynamicUInt::Mul,		"Mul",			{} );
		AS_METHOD( binder, ScriptDynamicUInt::Div,		"Div",			{} );
		AS_METHOD( binder, ScriptDynamicUInt::DivNear,	"DivNear",		{} );
		AS_METHOD( binder, ScriptDynamicUInt::DivCeil,	"DivCeil",		{} );
		AS_METHOD( binder, ScriptDynamicUInt::Add,		"Add",			{} );
		AS_METHOD( binder, ScriptDynamicUInt::Sub,		"Sub",			{} );
		AS_METHOD( binder, ScriptDynamicUInt::Pow,		"Pow",			{} );
		AS_METHOD( binder, ScriptDynamicUInt::PowOf2a,	"PowOf2",		{} );
		AS_METHOD( binder, ScriptDynamicUInt::PowOf2b,	"PowOf2",		{"scale"} );
		AS_METHOD( binder, ScriptDynamicUInt::Min,		"Min",			{} );
		AS_METHOD( binder, ScriptDynamicUInt::Max,		"Max",			{} );
		AS_METHOD( binder, ScriptDynamicUInt::FloorPOT,	"FloorPOT",		{} );
		AS_METHOD( binder, ScriptDynamicUInt::CeilPOT,	"CeilPOT",		{} );
		AS_METHOD( binder, ScriptDynamicUInt::NearPOT,	"NearPOT",		{} );

		AS_METHOD( binder, ScriptDynamicUInt::Mul1,		"Mul",			{} );
		AS_METHOD( binder, ScriptDynamicUInt::Div1,		"Div",			{} );
		AS_METHOD( binder, ScriptDynamicUInt::Add1,		"Add",			{} );
		AS_METHOD( binder, ScriptDynamicUInt::Sub1,		"Sub",			{} );
		AS_METHOD( binder, ScriptDynamicUInt::Min1,		"Min",			{} );
		AS_METHOD( binder, ScriptDynamicUInt::Max1,		"Max",			{} );
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	Mul
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Mul (const packed_uint2 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Mul );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Add
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Add (const packed_uint2 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Add );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Sub
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Sub (const packed_uint2 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Sub );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Pow
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Pow (const packed_uint2 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Pow );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Min
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Min (const packed_uint2 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Min );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Max
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Max (const packed_uint2 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Max );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	ScriptDynamicUInt2::FloorPOT
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::FloorPOT () __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( 0, EDynamicVarOperator::FloorPOT );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt2::CeilPOT
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::CeilPOT () __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( 0, EDynamicVarOperator::CeilPOT );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	ScriptDynamicUInt2::NearPOT
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::NearPOT () __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( 0, EDynamicVarOperator::NearPOT );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	Div
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Div (const packed_uint2 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Div );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
	ScriptDynamicUInt2*  ScriptDynamicUInt2::DivNear (const packed_uint2 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::DivNear );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}

	ScriptDynamicUInt2*  ScriptDynamicUInt2::DivCeil (const packed_uint2 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::DivCeil );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}

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
	PowOf2
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::PowOf2 () __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( uint2{1}, EDynamicVarOperator::PowOf2 );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ToDim
=================================================
*/
	ScriptDynamicDim*  ScriptDynamicUInt2::ToDim () __Th___
	{
		auto	ds = _value->ToDim();
		ScriptDynamicDimPtr	result{ new ScriptDynamicDim{ RVRef(ds) }};
		return result.Detach();
	}
	
/*
=================================================
	Add1
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Add1 (const ScriptDynamicUInt2Ptr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Add );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Sub1
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Sub1 (const ScriptDynamicUInt2Ptr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Sub );
		
		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Mul1
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Mul1 (const ScriptDynamicUInt2Ptr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Mul );
		
		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Div1
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Div1 (const ScriptDynamicUInt2Ptr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Div );
		
		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
	ScriptDynamicUInt2*  ScriptDynamicUInt2::DivNear1 (const ScriptDynamicUInt2Ptr &rhs) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::DivNear );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}

	ScriptDynamicUInt2*  ScriptDynamicUInt2::DivCeil1 (const ScriptDynamicUInt2Ptr &rhs) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::DivCeil );

		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	Min1
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Min1 (const ScriptDynamicUInt2Ptr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Min );
		
		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Max1
=================================================
*/
	ScriptDynamicUInt2*  ScriptDynamicUInt2::Max1 (const ScriptDynamicUInt2Ptr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Max );
		
		ScriptDynamicUInt2Ptr	result{ new ScriptDynamicUInt2{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Area
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt2::Area () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _value->GetDynamicArea() }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt2::Bind
=================================================
*/
	void  ScriptDynamicUInt2::Bind (const ScriptEnginePtr &se) __Th___
	{
		{
			ClassBinder<ScriptDynamicUInt2>		binder{ se };
			binder.CreateRef();
			binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicUInt2, DynamicUInt2, const packed_uint2& >, {} );
			AS_METHOD( binder, ScriptDynamicUInt2::X,			"X",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Y,			"Y",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::PowOf2,		"PowOf2",		{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Mul,			"Mul",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Div,			"Div",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::DivNear,		"DivNear",		{} );
			AS_METHOD( binder, ScriptDynamicUInt2::DivCeil,		"DivCeil",		{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Add,			"Add",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Sub,			"Sub",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Pow,			"Pow",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Min,			"Min",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Max,			"Max",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::FloorPOT,	"FloorPOT",		{} );
			AS_METHOD( binder, ScriptDynamicUInt2::CeilPOT,		"CeilPOT",		{} );
			AS_METHOD( binder, ScriptDynamicUInt2::NearPOT,		"NearPOT",		{} );

			AS_METHOD( binder, ScriptDynamicUInt2::Mul1,		"Mul",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Div1,		"Div",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Add1,		"Add",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Sub1,		"Sub",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Min1,		"Min",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::Max1,		"Max",			{} );
			AS_METHOD( binder, ScriptDynamicUInt2::DivNear1,	"DivNear",		{} );
			AS_METHOD( binder, ScriptDynamicUInt2::DivCeil1,	"DivCeil",		{} );

			AS_METHOD( binder, ScriptDynamicUInt2::Area,		"Area",			{} );
		}{
			ClassBinder<ScriptDynamicUInt>		binder{ se };
			AS_METHOD( binder, ScriptDynamicUInt::X1,			"X1",			{} );
			AS_METHOD( binder, ScriptDynamicUInt::XX,			"XX",			{} );
		}
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
	Volume
=================================================
*/
	ScriptDynamicUInt*  ScriptDynamicUInt3::Volume () C_Th___
	{
		ScriptDynamicUIntPtr	result{ new ScriptDynamicUInt{ _value->GetDynamicVolume() }};
		return result.Detach();
	}
	
/*
=================================================
	Mul
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Mul (const packed_uint3 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Mul );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Add
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Add (const packed_uint3 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Add );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Sub
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Sub (const packed_uint3 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Sub );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Pow
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Pow (const packed_uint3 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Pow );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Min
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Min (const packed_uint3 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Min );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Max
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Max (const packed_uint3 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Max );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Div
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Div (const packed_uint3 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Div );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
	ScriptDynamicUInt3*  ScriptDynamicUInt3::DivNear (const packed_uint3 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::DivNear );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}

	ScriptDynamicUInt3*  ScriptDynamicUInt3::DivCeil (const packed_uint3 &value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::DivCeil );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt3::FloorPOT
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::FloorPOT () __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( 0, EDynamicVarOperator::FloorPOT );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt3::CeilPOT
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::CeilPOT () __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( 0, EDynamicVarOperator::CeilPOT );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	ScriptDynamicUInt3::NearPOT
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::NearPOT () __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( 0, EDynamicVarOperator::NearPOT );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	Add1
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Add1 (const ScriptDynamicUInt3Ptr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Add );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Sub1
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Sub1 (const ScriptDynamicUInt3Ptr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Sub );
		
		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Mul1
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Mul1 (const ScriptDynamicUInt3Ptr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Mul );
		
		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Div1
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Div1 (const ScriptDynamicUInt3Ptr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Div );
		
		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
	ScriptDynamicUInt3*  ScriptDynamicUInt3::DivNear1 (const ScriptDynamicUInt3Ptr &rhs) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::DivNear );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}

	ScriptDynamicUInt3*  ScriptDynamicUInt3::DivCeil1 (const ScriptDynamicUInt3Ptr &rhs) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::DivCeil );

		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	Min1
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Min1 (const ScriptDynamicUInt3Ptr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Min );
		
		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}
	
/*
=================================================
	Max1
=================================================
*/
	ScriptDynamicUInt3*  ScriptDynamicUInt3::Max1 (const ScriptDynamicUInt3Ptr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Max );
		
		ScriptDynamicUInt3Ptr	result{ new ScriptDynamicUInt3{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicUInt3::Bind
=================================================
*/
	void  ScriptDynamicUInt3::Bind (const ScriptEnginePtr &se) __Th___
	{
		{
			ClassBinder<ScriptDynamicUInt3>		binder{ se };
			binder.CreateRef();
			binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicUInt3, DynamicUInt3, const packed_uint3& >, {} );
			AS_METHOD( binder, ScriptDynamicUInt3::X,			"X",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Y,			"Y",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Z,			"Z",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Mul,			"Mul",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Div,			"Div",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::DivNear,		"DivNear",	{} );
			AS_METHOD( binder, ScriptDynamicUInt3::DivCeil,		"DivCeil",	{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Add,			"Add",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Sub,			"Sub",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Pow,			"Pow",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Min,			"Min",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Max,			"Max",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::FloorPOT,	"FloorPOT",	{} );
			AS_METHOD( binder, ScriptDynamicUInt3::CeilPOT,		"CeilPOT",	{} );
			AS_METHOD( binder, ScriptDynamicUInt3::NearPOT,		"NearPOT",	{} );

			AS_METHOD( binder, ScriptDynamicUInt3::Mul1,		"Mul",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Div1,		"Div",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Add1,		"Add",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Sub1,		"Sub",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Min1,		"Min",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Max1,		"Max",		{} );
			AS_METHOD( binder, ScriptDynamicUInt3::DivNear1,	"DivNear",	{} );
			AS_METHOD( binder, ScriptDynamicUInt3::DivCeil1,	"DivCeil",	{} );
			AS_METHOD( binder, ScriptDynamicUInt3::Volume,		"Volume",	{} );
		}{
			ClassBinder<ScriptDynamicUInt>		binder{ se };
			AS_METHOD( binder, ScriptDynamicUInt::X11,			"X11",		{} );
			AS_METHOD( binder, ScriptDynamicUInt::XXX,			"XXX",		{} );
		}
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
		AS_METHOD( binder, ScriptDynamicUInt4::X,	"X",	{} );
		AS_METHOD( binder, ScriptDynamicUInt4::Y,	"Y",	{} );
		AS_METHOD( binder, ScriptDynamicUInt4::Z,	"Z",	{} );
		AS_METHOD( binder, ScriptDynamicUInt4::W,	"W",	{} );
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
		AS_METHOD( binder, ScriptDynamicInt2::X,	"X",	{} );
		AS_METHOD( binder, ScriptDynamicInt2::Y,	"Y",	{} );
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
		AS_METHOD( binder, ScriptDynamicInt3::X,	"X",	{} );
		AS_METHOD( binder, ScriptDynamicInt3::Y,	"Y",	{} );
		AS_METHOD( binder, ScriptDynamicInt3::Z,	"Z",	{} );
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
		AS_METHOD( binder, ScriptDynamicInt4::X,	"X",	{} );
		AS_METHOD( binder, ScriptDynamicInt4::Y,	"Y",	{} );
		AS_METHOD( binder, ScriptDynamicInt4::Z,	"Z",	{} );
		AS_METHOD( binder, ScriptDynamicInt4::W,	"W",	{} );
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	ScriptDynamicFloat::Mul
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::Mul (float value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Mul );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::Div
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::Div (float value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Div );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

	ScriptDynamicFloat*  ScriptDynamicFloat::DivNear (float value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::DivNear );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

	ScriptDynamicFloat*  ScriptDynamicFloat::DivCeil (float value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::DivCeil );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::Add / Sub
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::Add (float value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Add );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

	ScriptDynamicFloat*  ScriptDynamicFloat::Sub (float value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Sub );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::Pow
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::Pow (float value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Pow );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::PowOf2
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::PowOf2a () __Th___
	{
		return PowOf2b( 1 );
	}

	ScriptDynamicFloat*  ScriptDynamicFloat::PowOf2b (float value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::PowOf2 );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::Min
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::Min (float value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Min );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::Max
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::Max (float value) __Th___
	{
		auto	du = _value->Clone();
		du->SetOp( value, EDynamicVarOperator::Max );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::Add1
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::Add1 (const ScriptDynamicFloatPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Add );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::Sub1
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::Sub1 (const ScriptDynamicFloatPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Sub );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::Mul1
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::Mul1 (const ScriptDynamicFloatPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Mul );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::Div1
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::Div1 (const ScriptDynamicFloatPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Div );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::Min1
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::Min1 (const ScriptDynamicFloatPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Min );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::Max1
=================================================
*/
	ScriptDynamicFloat*  ScriptDynamicFloat::Max1 (const ScriptDynamicFloatPtr &rhs) __Th___
	{
		CHECK_THROW( rhs and rhs->Get() );

		auto	du = _value->Clone();
		du->SetOp( rhs->Get(), EDynamicVarOperator::Max );

		ScriptDynamicFloatPtr	result{ new ScriptDynamicFloat{ RVRef(du) }};
		return result.Detach();
	}

/*
=================================================
	ScriptDynamicFloat::Bind
=================================================
*/
	void  ScriptDynamicFloat::Bind (const ScriptEnginePtr &se) __Th___
	{
		{
			ClassBinder<ScriptDynamicFloat>		binder{ se };
			binder.CreateRef();
			binder.AddFactoryCtor( &ScriptDynamicT_Ctor< ScriptDynamicFloat, DynamicFloat, float >, {} );
			AS_METHOD( binder, ScriptDynamicFloat::Mul,		"Mul",			{} );
			AS_METHOD( binder, ScriptDynamicFloat::Div,		"Div",			{} );
			AS_METHOD( binder, ScriptDynamicFloat::DivNear,	"DivNear",		{} );
			AS_METHOD( binder, ScriptDynamicFloat::DivCeil,	"DivCeil",		{} );
			AS_METHOD( binder, ScriptDynamicFloat::Add,		"Add",			{} );
			AS_METHOD( binder, ScriptDynamicFloat::Sub,		"Sub",			{} );
			AS_METHOD( binder, ScriptDynamicFloat::Pow,		"Pow",			{} );
			AS_METHOD( binder, ScriptDynamicFloat::PowOf2a,	"PowOf2",		{} );
			AS_METHOD( binder, ScriptDynamicFloat::PowOf2b,	"PowOf2",		{"scale"} );
			AS_METHOD( binder, ScriptDynamicFloat::Min,		"Min",			{} );
			AS_METHOD( binder, ScriptDynamicFloat::Max,		"Max",			{} );

			AS_METHOD( binder, ScriptDynamicFloat::Mul1,	"Mul",			{} );
			AS_METHOD( binder, ScriptDynamicFloat::Div1,	"Div",			{} );
			AS_METHOD( binder, ScriptDynamicFloat::Add1,	"Add",			{} );
			AS_METHOD( binder, ScriptDynamicFloat::Sub1,	"Sub",			{} );
			AS_METHOD( binder, ScriptDynamicFloat::Min1,	"Min",			{} );
			AS_METHOD( binder, ScriptDynamicFloat::Max1,	"Max",			{} );
		}{
			ClassBinder<ScriptDynamicUInt>		binder{ se };
			AS_METHOD( binder, ScriptDynamicUInt::ToFloat,		"ToFloat",		{} );
			AS_METHOD( binder, ScriptDynamicUInt::Percent,		"Percent",		{} );
		}
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
		AS_METHOD( binder, ScriptDynamicFloat2::X,	"X",	{} );
		AS_METHOD( binder, ScriptDynamicFloat2::Y,	"Y",	{} );
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
		AS_METHOD( binder, ScriptDynamicFloat3::X,	"X",	{} );
		AS_METHOD( binder, ScriptDynamicFloat3::Y,	"Y",	{} );
		AS_METHOD( binder, ScriptDynamicFloat3::Z,	"Z",	{} );
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
		AS_METHOD( binder, ScriptDynamicFloat4::X,	"X",	{} );
		AS_METHOD( binder, ScriptDynamicFloat4::Y,	"Y",	{} );
		AS_METHOD( binder, ScriptDynamicFloat4::Z,	"Z",	{} );
		AS_METHOD( binder, ScriptDynamicFloat4::W,	"W",	{} );
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
