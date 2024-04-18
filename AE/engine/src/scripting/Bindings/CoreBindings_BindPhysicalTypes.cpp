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
	BindPhysicalQuantity
=================================================
*/
	template <typename T>
	static void  PhysicalQuantity_Ctor (OUT void* mem, float val) {
		PlacementNew<T>( OUT mem, val );
	}

	template <typename T>
	static T  PhysicalQuantity_Add (const T &lhs, const T &rhs) {
		return lhs + rhs;
	}

	template <typename T>
	static T  PhysicalQuantity_Sub (const T &lhs, const T &rhs) {
		return lhs - rhs;
	}

	template <typename T>
	static T  PhysicalQuantity_Mul (const T &lhs, float rhs) {
		return lhs * rhs;
	}

	template <typename T>
	static T  PhysicalQuantity_Div (const T &lhs, float rhs) {
		return lhs / rhs;
	}

	template <typename T>
	static float  PhysicalQuantity_GetNonScaled (const T &lhs) {
		return lhs.GetNonScaled();
	}

	template <typename T>
	static float  PhysicalQuantity_GetScaled (const T &lhs) {
		return lhs.GetScaled();
	}

	template <typename T1, typename T2, typename T3>
	static T3  PhysicalQuantity_Mul2 (const T1 &lhs, const T2 &rhs) {
		return lhs * rhs;
	}

	template <typename T1, typename T2, typename T3>
	static T3  PhysicalQuantity_Div2 (const T1 &lhs, const T2 &rhs) {
		return lhs / rhs;
	}


	template <typename T1, typename T2, typename T3>
	static void  BindPhysicalQuantity_Mul (const ScriptEnginePtr &se) __Th___
	{
		{
			ClassBinder<T1>		binder{ se };
			binder.Operators().Binary( EBinaryOperator::Div, &PhysicalQuantity_Mul2<T1,T2,T3> );
		}

		if constexpr( not IsSameTypes< T1, T2 >)
		{
			ClassBinder<T2>		binder{ se };
			binder.Operators().Binary( EBinaryOperator::Div, &PhysicalQuantity_Mul2<T2,T1,T3> );
		}
	}


	template <typename T1, typename T2, typename T3>
	static void  BindPhysicalQuantity_Div (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<T1>		binder{ se };
		binder.Operators().Binary( EBinaryOperator::Div, &PhysicalQuantity_Div2<T1,T2,T3> );
	}


	static void  BindPhysicalQuantity (const ScriptEnginePtr &se) __Th___
	{
		using PQ = Math::DefaultPhysicalQuantity<float>;

		#define AE_PHYSICAL_TYPES_VIS( _name_ )														\
		{																							\
			using T = PQ::_name_;																	\
			ClassBinder<T>		binder{ se };														\
			binder.CreateClassValue();																\
			binder.AddConstructor( &PhysicalQuantity_Ctor<T>, {"x"} );								\
			binder.Operators()																		\
				.Binary( EBinaryOperator::Add, &PhysicalQuantity_Add<T> )							\
				.Binary( EBinaryOperator::Sub, &PhysicalQuantity_Sub<T> )							\
				.Binary( EBinaryOperator::Mul, &PhysicalQuantity_Mul<T> )							\
				.Binary( EBinaryOperator::Div, &PhysicalQuantity_Div<T> );							\
			binder.AddMethodFromGlobal( &PhysicalQuantity_GetNonScaled<T>,	"GetNonScaled",	{} );	\
			binder.AddMethodFromGlobal( &PhysicalQuantity_GetScaled<T>,		"GetScaled",	{} );	\
		}
		AE_PHYSICAL_TYPES( AE_PHYSICAL_TYPES_VIS );
		#undef AE_PHYSICAL_TYPES_VIS

		BindPhysicalQuantity_Div< PQ::Meter,			PQ::Second,				PQ::MeterPerSecond			>( se );	// m / s = m/s
		BindPhysicalQuantity_Div< PQ::MeterPerSecond,	PQ::Second,				PQ::MeterPerSquareSecond	>( se );	// m/s / s = m/s2
		BindPhysicalQuantity_Div< PQ::Kilogram,			PQ::Second,				PQ::KilogramPerSecond		>( se );	// kg / s = kg/s
		BindPhysicalQuantity_Div< PQ::Kilogram,			PQ::CubicMeter,			PQ::KilogramPerCubicMeter	>( se );	// kg / m3 = kg/m3
		BindPhysicalQuantity_Div< PQ::Joule,			PQ::Second,				PQ::Watt					>( se );	// J / s = W
		BindPhysicalQuantity_Div< PQ::Joule,			PQ::Coulomb,			PQ::Volt					>( se );	// J / C = V
		BindPhysicalQuantity_Div< PQ::Volt,				PQ::Ampere,				PQ::Ohm						>( se );	// V / A = Ohm

		BindPhysicalQuantity_Mul< PQ::Meter,			PQ::Meter,				PQ::SquareMeter				>( se );	// m * m = m2
		BindPhysicalQuantity_Mul< PQ::Meter,			PQ::SquareMeter,		PQ::CubicMeter				>( se );	// m2 * m = m3
		BindPhysicalQuantity_Mul< PQ::Kilogram,			PQ::MeterPerSecond,		PQ::KilogramMeterPerSecond	>( se );	// kg * m/s = kg*m/s
		BindPhysicalQuantity_Mul< PQ::Kilogram,			PQ::MeterPerSquareSecond,PQ::Newton					>( se );	// kg * m/s2 = (N) kg*m/s2
		BindPhysicalQuantity_Mul< PQ::Ampere,			PQ::Second,				PQ::Coulomb					>( se );	// A * s = C
	}

/*
=================================================
	BindPhysicalQuantityVec
=================================================
*/
	template <typename T>
	static void  PhysicalQuantityVec_Ctor1 (OUT void* mem, float val) {
		PlacementNew<T>( OUT mem, val );
	}

	template <typename T>
	static void  PhysicalQuantityVec_Ctor2 (OUT void* mem, float x, float y) {
		PlacementNew<T>( OUT mem, x, y );
	}

	template <typename T>
	static void  PhysicalQuantityVec_Ctor3 (OUT void* mem, float x, float y, float z) {
		PlacementNew<T>( OUT mem, x, y, z );
	}

	static void  BindPhysicalQuantityVec (const ScriptEnginePtr &se) __Th___
	{
		using PQ = Math::DefaultPhysicalQuantity<float>;

		#define AE_PHYSICAL_TYPES_VIS( _name_ )										\
		{																			\
			using T = Math::PhysicalQuantityPackedVec2< PQ::_name_ >;				\
			ClassBinder<T>		binder{ se };										\
			binder.CreateClassValue();												\
			binder.AddConstructor( &PhysicalQuantityVec_Ctor1<T>, {"value"} );		\
			binder.AddConstructor( &PhysicalQuantityVec_Ctor2<T>, {"x", "y"} );		\
		}{																			\
			using T = Math::PhysicalQuantityPackedVec3< PQ::_name_ >;				\
			ClassBinder<T>		binder{ se };										\
			binder.CreateClassValue();												\
			binder.AddConstructor( &PhysicalQuantityVec_Ctor1<T>, {"value"} );		\
			binder.AddConstructor( &PhysicalQuantityVec_Ctor3<T>, {"x", "y", "z"} );\
		}
		AE_PHYSICAL_TYPES( AE_PHYSICAL_TYPES_VIS );
		#undef AE_PHYSICAL_TYPES_VIS
	}

} // namespace


/*
=================================================
	BindPhysicalTypes
=================================================
*/
	void  CoreBindings::BindPhysicalTypes (const ScriptEnginePtr &se) __Th___
	{
		BindPhysicalQuantity( se );
		BindPhysicalQuantityVec( se );
	}


} // AE::Scripting
