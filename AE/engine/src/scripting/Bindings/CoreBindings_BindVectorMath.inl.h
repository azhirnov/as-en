// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

namespace AE::Scripting
{
namespace
{

	template <typename T>
	struct InitVecFields
	{};

/*
=================================================
	InitVecFields (Vec2)
=================================================
*/
	template <typename T>
	struct InitVecFields< PackedVec<T,2> >
	{
	private:
		static void  _Ctor1 (void* mem, const T value)
		{
			PlacementNew< PackedVec<T,2> >( OUT mem, value );
		}

		template <typename B>
		static void  _Ctor2 (void* mem, const PackedVec<B,2> &value)
		{
			PlacementNew< PackedVec<T,2> >( OUT mem, PackedVec<T,2>{value} );
		}

		template <typename B>
		static void  _Ctor3 (void* mem, const PackedVec<B,3> &value)
		{
			PlacementNew< PackedVec<T,2> >( OUT mem, PackedVec<T,2>{value} );
		}

		template <typename B>
		static void  _Ctor4 (void* mem, const PackedVec<B,4> &value)
		{
			PlacementNew< PackedVec<T,2> >( OUT mem, PackedVec<T,2>{value} );
		}

		static void  _CtorArg2 (void* mem, T x, T y)
		{
			PlacementNew< PackedVec<T,2> >( OUT mem, x, y );
		}

	public:
		static void  Init (ClassBinder< PackedVec<T,2> > &binder)
		{
			using Vec_t = PackedVec< T, 2 >;

			binder.AddProperty( &Vec_t::x, "x" );
			binder.AddProperty( &Vec_t::y, "y" );

			binder.AddConstructor( &_Ctor1,		{"v"} );
			binder.AddConstructor( &_Ctor3<T>,	{"v3"} );
			binder.AddConstructor( &_Ctor4<T>,	{"v4"} );
			binder.AddConstructor( &_CtorArg2,	{"x", "y"} );

			if constexpr( not IsSame< T, short >)
			{
				binder.AddConstructor( &_Ctor2<short>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<short>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<short>,	{"v4"} );
			}
			if constexpr( not IsSame< T, ushort >)
			{
				binder.AddConstructor( &_Ctor2<ushort>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<ushort>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<ushort>,	{"v4"} );
			}
			if constexpr( not IsSame< T, int >)
			{
				binder.AddConstructor( &_Ctor2<int>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<int>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<int>,	{"v4"} );
			}
			if constexpr( not IsSame< T, uint >)
			{
				binder.AddConstructor( &_Ctor2<uint>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<uint>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<uint>,	{"v4"} );
			}
			if constexpr( not IsSame< T, float >)
			{
				binder.AddConstructor( &_Ctor2<float>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<float>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<float>,	{"v4"} );
			}
		}
	};

/*
=================================================
	InitVecFields (Vec3)
=================================================
*/
	template <typename T>
	struct InitVecFields< PackedVec<T,3> >
	{
	private:
		static void  _Ctor1 (void* mem, const T value)
		{
			PlacementNew< PackedVec<T,3> >( OUT mem, value );
		}

		template <typename B>
		static void  _Ctor2 (void* mem, const PackedVec<B,2> &value)
		{
			PlacementNew< PackedVec<T,3> >( OUT mem, PackedVec<T,3>{ static_cast<T>(value.x), static_cast<T>(value.y), T{0} });
		}

		template <typename B>
		static void  _Ctor3 (void* mem, const PackedVec<B,3> &value)
		{
			PlacementNew< PackedVec<T,3> >( OUT mem, PackedVec<T,3>{ value });
		}

		template <typename B>
		static void  _Ctor4 (void* mem, const PackedVec<B,4> &value)
		{
			PlacementNew< PackedVec<T,3> >( OUT mem, PackedVec<T,3>{ static_cast<T>(value.x), static_cast<T>(value.y), static_cast<T>(value.z) });
		}

		static void  _CtorArg2 (void* mem, const PackedVec<T,2> &xy, T z)
		{
			PlacementNew< PackedVec<T,3> >( OUT mem, xy, z );
		}

		static void  _CtorArg3 (void* mem, T x, T y, T z)
		{
			PlacementNew< PackedVec<T,3> >( OUT mem, x, y, z );
		}

	public:
		static void  Init (ClassBinder< PackedVec<T,3> > &binder)
		{
			using Vec_t = PackedVec< T, 3 >;

			binder.AddProperty( &Vec_t::x, "x" );
			binder.AddProperty( &Vec_t::y, "y" );
			binder.AddProperty( &Vec_t::z, "z" );

			binder.AddConstructor( &_Ctor1,		{"v"} );
			binder.AddConstructor( &_Ctor2<T>,	{"v2"} );
			binder.AddConstructor( &_Ctor4<T>,	{"v4"} );
			binder.AddConstructor( &_CtorArg2,	{"xy", "z"} );
			binder.AddConstructor( &_CtorArg3,	{"x", "y", "z"} );

			if constexpr( not IsSame< T, short >)
			{
				binder.AddConstructor( &_Ctor2<short>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<short>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<short>,	{"v4"} );
			}
			if constexpr( not IsSame< T, ushort >)
			{
				binder.AddConstructor( &_Ctor2<ushort>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<ushort>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<ushort>,	{"v4"} );
			}
			if constexpr( not IsSame< T, int >)
			{
				binder.AddConstructor( &_Ctor2<int>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<int>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<int>,	{"v4"} );
			}
			if constexpr( not IsSame< T, uint >)
			{
				binder.AddConstructor( &_Ctor2<uint>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<uint>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<uint>,	{"v4"} );
			}
			if constexpr( not IsSame< T, float >)
			{
				binder.AddConstructor( &_Ctor2<float>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<float>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<float>,	{"v4"} );
			}
		}
	};

/*
=================================================
	InitVecFields (Vec4)
=================================================
*/
	template <typename T>
	struct InitVecFields< PackedVec<T,4> >
	{
	private:
		static void  _Ctor1 (void* mem, const T value)
		{
			PlacementNew< PackedVec<T,4> >( OUT mem, value );
		}

		template <typename B>
		static void  _Ctor2 (void* mem, const PackedVec<B,2> &value)
		{
			PlacementNew< PackedVec<T,4> >( OUT mem, PackedVec<T,4>{ static_cast<T>(value.x), static_cast<T>(value.y), T{0}, T{0}} );
		}

		template <typename B>
		static void  _Ctor3 (void* mem, const PackedVec<B,3> &value)
		{
			PlacementNew< PackedVec<T,4> >( OUT mem, PackedVec<T,4>{ static_cast<T>(value.x), static_cast<T>(value.y), static_cast<T>(value.z), T{0} });
		}

		template <typename B>
		static void  _Ctor4 (void* mem, const PackedVec<B,4> &value)
		{
			PlacementNew< PackedVec<T,4> >( OUT mem, PackedVec<T,4>{value} );
		}

		static void  _CtorArg2 (void* mem, const PackedVec<T,2> &xy, const PackedVec<T,2> &zw)
		{
			PlacementNew< PackedVec<T,4> >( OUT mem, xy, zw );
		}

		static void  _CtorArg3 (void* mem, const PackedVec<T,3> &xyz, T w)
		{
			PlacementNew< PackedVec<T,4> >( OUT mem, xyz, w );
		}

		static void  _CtorArg4 (void* mem, T x, T y, T z, T w)
		{
			PlacementNew< PackedVec<T,4> >( OUT mem, x, y, z, w );
		}

		static void  _CtorArg5 (void* mem, T x, const PackedVec<T,3> &yzw)
		{
			PlacementNew< PackedVec<T,4> >( OUT mem, x, yzw );
		}

	public:
		static void  Init (ClassBinder< PackedVec<T,4> > &binder)
		{
			using Vec_t = PackedVec< T, 4 >;

			binder.AddProperty( &Vec_t::x, "x" );
			binder.AddProperty( &Vec_t::y, "y" );
			binder.AddProperty( &Vec_t::z, "z" );
			binder.AddProperty( &Vec_t::w, "w" );

			binder.AddConstructor( &_Ctor1,		{"v"} );
			binder.AddConstructor( &_Ctor2<T>,	{"v2"} );
			binder.AddConstructor( &_Ctor3<T>,	{"v3"} );
			binder.AddConstructor( &_CtorArg2,	{"xy", "zw"} );
			binder.AddConstructor( &_CtorArg3,	{"xyz", "w"} );
			binder.AddConstructor( &_CtorArg4,	{"x", "y", "z", "w"} );
			binder.AddConstructor( &_CtorArg5,	{"x", "yzw"} );

			if constexpr( not IsSame< T, short >)
			{
				binder.AddConstructor( &_Ctor2<short>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<short>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<short>,	{"v4"} );
			}
			if constexpr( not IsSame< T, ushort >)
			{
				binder.AddConstructor( &_Ctor2<ushort>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<ushort>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<ushort>,	{"v4"} );
			}
			if constexpr( not IsSame< T, int >)
			{
				binder.AddConstructor( &_Ctor2<int>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<int>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<int>,	{"v4"} );
			}
			if constexpr( not IsSame< T, uint >)
			{
				binder.AddConstructor( &_Ctor2<uint>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<uint>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<uint>,	{"v4"} );
			}
			if constexpr( not IsSame< T, float >)
			{
				binder.AddConstructor( &_Ctor2<float>,	{"v2"} );
				binder.AddConstructor( &_Ctor3<float>,	{"v3"} );
				binder.AddConstructor( &_Ctor4<float>,	{"v4"} );
			}
		}
	};

/*
=================================================
	VecFunc
=================================================
*/
	template <typename V>
	struct VecFunc
	{
		static bool  Equal (const V &lhs, const V &rhs) {
			return Base::All( lhs == rhs );
		}

		static int  Cmp (const V &lhs, const V &rhs) {
			for (uint i = 0; i < VecSize<V>; ++i) {
				if ( lhs[i] > rhs[i] )	return +1;
				if ( lhs[i] < rhs[i] )	return -1;
			}
			return 0;
		}

		static V   Neg (const V& v)															{ return -v; }

		static V&  Add_a_v (V& lhs, const V &rhs)											{ return lhs += rhs; }
		static V&  Add_a_s (V& lhs, typename V::value_type rhs)								{ return lhs += rhs; }
		static V   Add_v_v (const V &lhs, const V &rhs)										{ return lhs + rhs; }
		static V   Add_v_s (const V &lhs, typename V::value_type rhs)						{ return lhs + rhs; }
		static V   Add_s_v (typename V::value_type lhs, const V &rhs)						{ return lhs + rhs; }

		static V&  Sub_a_v (V& lhs, const V &rhs)											{ return lhs -= rhs; }
		static V&  Sub_a_s (V& lhs, typename V::value_type rhs)								{ return lhs -= rhs; }
		static V   Sub_v_v (const V &lhs, const V &rhs)										{ return lhs - rhs; }
		static V   Sub_v_s (const V &lhs, typename V::value_type rhs)						{ return lhs - rhs; }
		static V   Sub_s_v (typename V::value_type lhs, const V &rhs)						{ return lhs - rhs; }

		static V&  Mul_a_v (V& lhs, const V &rhs)											{ return lhs *= rhs; }
		static V&  Mul_a_s (V& lhs, typename V::value_type rhs)								{ return lhs *= rhs; }
		static V   Mul_v_v (const V &lhs, const V &rhs)										{ return lhs * rhs; }
		static V   Mul_v_s (const V &lhs, typename V::value_type rhs)						{ return lhs * rhs; }
		static V   Mul_s_v (typename V::value_type lhs, const V &rhs)						{ return lhs * rhs; }

		static V&  Div_a_v (V& lhs, const V &rhs)											{ return lhs /= rhs; }
		static V&  Div_a_s (V& lhs, typename V::value_type rhs)								{ return lhs /= rhs; }
		static V   Div_v_v (const V &lhs, const V &rhs)										{ return lhs / rhs; }
		static V   Div_v_s (const V &lhs, typename V::value_type rhs)						{ return lhs / rhs; }
		static V   Div_s_v (typename V::value_type lhs, const V &rhs)						{ return lhs / rhs; }

		static V   DivCeil_v_v (const V &lhs, const V &rhs)									{ return DivCeil( lhs, rhs ); }
		static V   DivCeil_v_s (const V &lhs, typename V::value_type rhs)					{ return DivCeil( lhs, rhs ); }

		static V&  And_a_v (V& lhs, const V &rhs)											{ return lhs &= rhs; }
		static V&  And_a_s (V& lhs, typename V::value_type rhs)								{ return lhs &= rhs; }
		static V   And_v_v (const V &lhs, const V &rhs)										{ return lhs & rhs; }
		static V   And_v_s (const V &lhs, typename V::value_type rhs)						{ return lhs & rhs; }
		static V   And_s_v (typename V::value_type lhs, const V &rhs)						{ return lhs & rhs; }

		static V&  Or_a_v (V& lhs, const V &rhs)											{ return lhs |= rhs; }
		static V&  Or_a_s (V& lhs, typename V::value_type rhs)								{ return lhs |= rhs; }
		static V   Or_v_v (const V &lhs, const V &rhs)										{ return lhs | rhs; }
		static V   Or_v_s (const V &lhs, typename V::value_type rhs)						{ return lhs | rhs; }
		static V   Or_s_v (typename V::value_type lhs, const V &rhs)						{ return lhs | rhs; }

		static V&  Xor_a_v (V& lhs, const V &rhs)											{ return lhs ^= rhs; }
		static V&  Xor_a_s (V& lhs, typename V::value_type rhs)								{ return lhs ^= rhs; }
		static V   Xor_v_v (const V &lhs, const V &rhs)										{ return lhs ^ rhs; }
		static V   Xor_v_s (const V &lhs, typename V::value_type rhs)						{ return lhs ^ rhs; }
		static V   Xor_s_v (typename V::value_type lhs, const V &rhs)						{ return lhs ^ rhs; }

		static V&  ShiftL_a_v (V& lhs, const V &rhs)										{ return lhs <<= rhs; }
		static V&  ShiftL_a_s (V& lhs, typename V::value_type rhs)							{ return lhs <<= rhs; }
		static V   ShiftL_v_v (const V &lhs, const V &rhs)									{ return lhs << rhs; }
		static V   ShiftL_v_s (const V &lhs, typename V::value_type rhs)					{ return lhs << rhs; }
		static V   ShiftL_s_v (typename V::value_type lhs, const V &rhs)					{ return lhs << rhs; }

		static V&  ShiftR_a_v (V& lhs, const V &rhs)										{ return lhs >>= rhs; }
		static V&  ShiftR_a_s (V& lhs, typename V::value_type rhs)							{ return lhs >>= rhs; }
		static V   ShiftR_v_v (const V &lhs, const V &rhs)									{ return lhs >> rhs; }
		static V   ShiftR_v_s (const V &lhs, typename V::value_type rhs)					{ return lhs >> rhs; }
		static V   ShiftR_s_v (typename V::value_type lhs, const V &rhs)					{ return lhs >> rhs; }

		static bool  All (const V &x)														{ return Base::All( x ); }
		static bool  Any (const V &x)														{ return Base::Any( x ); }

		static V	ToSNorm (const V& x)													{ return Base::ToSNorm( x ); }
		static V	ToUNorm (const V& x)													{ return Base::ToUNorm( x ); }

		static typename V::value_type  Sum (const V& x)										{ return Base::Sum<typename V::value_type>( x ); }
		static typename V::value_type  Area (const V& x)									{ return Base::Area( x ); }
		static typename V::value_type  Dot (const V& x, const V& y)							{ return Base::Dot( x, y ); }
		static typename V::value_type  Length (const V& x)									{ return Base::Length( x ); }
		static typename V::value_type  LengthSq (const V& x)								{ return Base::LengthSq( x ); }
		static typename V::value_type  Distance (const V& x, const V& y)					{ return Base::Distance( x, y ); }
		static typename V::value_type  DistanceSq (const V& x, const V& y)					{ return Base::DistanceSq( x, y ); }
		static typename V::value_type  VecToLinear (const V& x, const V& y)					{ return Base::VecToLinear( x, y ); }

		static V  Cross (const V &x, const V &y)											{ return Base::Cross( x, y ); }
		static V  Min (const V &x, const V &y)												{ return Base::Min( x, y ); }
		static V  Max (const V &x, const V &y)												{ return Base::Max( x, y ); }
		static V  Clamp_1 (const V &x, const V &y, const V &z)								{ return Base::Clamp( x, y, z ); }
		static V  Clamp_2 (const V &x, typename V::value_type y, typename V::value_type z)	{ return Base::Clamp( x, y, z ); }
		static V  Wrap_1 (const V &x, const V &y, const V &z)								{ return Base::Wrap( x, y, z ); }
		static V  Wrap_2 (const V &x, typename V::value_type y, typename V::value_type z)	{ return Base::Wrap( x, y, z ); }
		static V  MirroredWrap_1 (const V &x, const V &y, const V &z)						{ return Base::MirroredWrap( x, y, z ); }
		static V  MirroredWrap_2 (const V &x, typename V::value_type y, typename V::value_type z)	{ return Base::MirroredWrap( x, y, z ); }

		static V	Normalize (const V &x)													{ return Base::Normalize( x ); }
		static V	Floor (const V &x)														{ return Base::Floor( x ); }
		static V	Ceil (const V &x)														{ return Base::Ceil( x ); }
		static V	Round (const V &x)														{ return Base::Round( x ); }
		static V	Fract (const V &x)														{ return Base::Fract( x ); }
		static V	Sqrt (const V &x)														{ return Base::Sqrt( x ); }
		static V	Abs (const V &x)														{ return Base::Abs( x ); }
		static V	Lerp (const V &x, const V &y, typename V::value_type f)					{ return Base::Lerp( x, y, f ); }
		static V	Ln (const V &x)															{ return Base::Ln( x ); }
		static V	Log2 (const V &x)														{ return Base::Log2( x ); }
		static V	Log_1 (const V &x, const V &y)											{ return Base::Log( x, y ); }
		static V	Log_2 (const V &x, typename V::value_type y)							{ return Base::Log( x, y ); }
		static V	Pow_1 (const V &x, const V &y)											{ return Base::Pow( x, y ); }
		static V	Pow_2 (const V &x, typename V::value_type y)							{ return Base::Pow( x, y ); }
		static V	Exp (const V &x)														{ return Base::Exp( x ); }
		static V	Exp2 (const V &x)														{ return Base::Exp2( x ); }
		static V	Exp10 (const V &x)														{ return Base::Exp10( x ); }

		static V	Not (const V &x)														{ return glm::operator ! (x); }
	};

	struct VecFunc2
	{
		template <typename V, typename R>
		using FloatOnly		= std::enable_if_t< IsFloatPoint<typename V::value_type>, R >;

		template <typename V, typename R>
		using ExceptFloat	= std::enable_if_t< not IsFloatPoint<typename V::value_type>, R >;

		template <typename V> static FloatOnly<V, V&>  Mod_a_v (V& lhs, const V &rhs)							{ return lhs = glm::mod( lhs, rhs ); }
		template <typename V> static FloatOnly<V, V&>  Mod_a_s (V& lhs, typename V::value_type rhs)				{ return lhs = glm::mod( lhs, rhs ); }
		template <typename V> static FloatOnly<V, V>   Mod_v_v (const V &lhs, const V &rhs)						{ return glm::mod( lhs, rhs ); }
		template <typename V> static FloatOnly<V, V>   Mod_v_s (const V &lhs, typename V::value_type rhs)		{ return glm::mod( lhs, rhs ); }
		template <typename V> static FloatOnly<V, V>   Mod_s_v (typename V::value_type lhs, const V &rhs)		{ return glm::mod( V(lhs), rhs ); }

		template <typename V> static ExceptFloat<V, V&>  Mod_a_v (V& lhs, const V &rhs)							{ return lhs %= rhs; }
		template <typename V> static ExceptFloat<V, V&>  Mod_a_s (V& lhs, typename V::value_type rhs)			{ return lhs %= rhs; }
		template <typename V> static ExceptFloat<V, V>   Mod_v_v (const V &lhs, const V &rhs)					{ return lhs % rhs; }
		template <typename V> static ExceptFloat<V, V>   Mod_v_s (const V &lhs, typename V::value_type rhs)		{ return lhs % rhs; }
		template <typename V> static ExceptFloat<V, V>   Mod_s_v (typename V::value_type lhs, const V &rhs)		{ return lhs % rhs; }
	};

/*
=================================================
	BindBoolVec
=================================================
*/
	template <typename T>
	static void  BindBoolVec (ClassBinder<T> &binder, const ScriptEnginePtr &se)
	{
		using Vec_t = T;
		using VF	= VecFunc< Vec_t >;

		AS_OP_EQUAL_T(	binder, VF::Equal	);
		AS_OP_CMP_T(	binder, VF::Cmp	);

		AS_OP_UNARY_T(	binder,	EUnaryOperator::Not, VF::Not );

		AS_GLOBAL_FN( se, VF::All,	"All",	{"x"} );
		AS_GLOBAL_FN( se, VF::Any,	"Any",	{"x"} );
	}

/*
=================================================
	BindIntFloatVec
=================================================
*/
	template <typename T>
	static void  BindIntFloatVec (ClassBinder<T> &binder, const ScriptEnginePtr &se)
	{
		using Vec_t		= T;
		using Scalar_t	= typename T::value_type;
		using VF		= VecFunc< Vec_t >;

		if constexpr( IsSame< Scalar_t, int >	or
					  IsSame< Scalar_t, uint >	or
					  IsSame< Scalar_t, float >)
		{
			if constexpr( IsSignedInteger<Scalar_t> or IsFloatPoint<Scalar_t> )
			{
				AS_GLOBAL_FN( se, VF::Abs,				"Abs",			{"x"} );
				AS_GLOBAL_FN( se, VF::MirroredWrap_1,	"MirroredWrap",	{"x", "min", "max"} );
				AS_GLOBAL_FN( se, VF::MirroredWrap_2,	"MirroredWrap",	{"x", "min", "max"} );

				AS_OP_UNARY_T( binder, EUnaryOperator::Inverse, VF::Neg );
			}

			AS_GLOBAL_FN( se, VF::Sum,				"Sum",			{"x"} );
			AS_GLOBAL_FN( se, VF::Area,				"Area",			{"x"} );
			AS_GLOBAL_FN( se, VF::Min,				"Min",			{"x", "y"} );
			AS_GLOBAL_FN( se, VF::Max,				"Max",			{"x", "y"} );
			AS_GLOBAL_FN( se, VF::Clamp_1,			"Clamp",		{"val", "min", "max"} );
			AS_GLOBAL_FN( se, VF::Clamp_2,			"Clamp",		{"val", "min", "max"} );
			AS_GLOBAL_FN( se, VF::Wrap_1,			"Wrap",			{"val", "min", "max"} );
			AS_GLOBAL_FN( se, VF::Wrap_2,			"Wrap",			{"val", "min", "max"} );
			AS_GLOBAL_FN( se, VF::VecToLinear,		"VecToLinear",	{"pos", "dim"} );
		}
	}

/*
=================================================
	BindIntVec
=================================================
*/
	template <typename T>
	static void  BindIntVec (ClassBinder<T> &binder, const ScriptEnginePtr &se)
	{
		using Vec_t = T;
		using VF	= VecFunc< Vec_t >;

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Add, VF::Add_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Add, VF::Add_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Add, VF::Add_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Add, VF::Add_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Add, VF::Add_s_v );

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Sub, VF::Sub_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Sub, VF::Sub_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Sub, VF::Sub_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Sub, VF::Sub_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Sub, VF::Sub_s_v );

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Mul, VF::Mul_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Mul, VF::Mul_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Mul, VF::Mul_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Mul, VF::Mul_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Mul, VF::Mul_s_v );

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Div, VF::Div_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Div, VF::Div_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Div, VF::Div_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Div, VF::Div_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Div, VF::Div_s_v );

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Mod, VecFunc2::template Mod_a_v< Vec_t >);
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Mod, VecFunc2::template Mod_a_s< Vec_t >);
		AS_OP_BINARY_T(		binder, EBinaryOperator::Mod, VecFunc2::template Mod_v_v< Vec_t >);
		AS_OP_BINARY_T(		binder, EBinaryOperator::Mod, VecFunc2::template Mod_v_s< Vec_t >);
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Mod, VecFunc2::template Mod_s_v< Vec_t >);

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::And, VF::And_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::And, VF::And_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::And, VF::And_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::And, VF::And_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::And, VF::And_s_v );

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Or,  VF::Or_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Or,  VF::Or_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Or,  VF::Or_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Or,  VF::Or_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Or,  VF::Or_s_v );

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Xor, VF::Xor_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Xor, VF::Xor_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Xor, VF::Xor_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Xor, VF::Xor_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Xor, VF::Xor_s_v );

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::ShiftLeft, VF::ShiftL_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::ShiftLeft, VF::ShiftL_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::ShiftLeft, VF::ShiftL_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::ShiftLeft, VF::ShiftL_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::ShiftLeft, VF::ShiftL_s_v );

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::ShiftRight, VF::ShiftR_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::ShiftRight, VF::ShiftR_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::ShiftRight, VF::ShiftR_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::ShiftRight, VF::ShiftR_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::ShiftRight, VF::ShiftR_s_v );

		AS_OP_EQUAL_T(		binder, VF::Equal );
		AS_OP_CMP_T(		binder, VF::Cmp );

		BindIntFloatVec( binder, se );

		AS_GLOBAL_FN( se, VF::DivCeil_v_v,		"DivCeil",	{"x", "y"} );
		AS_GLOBAL_FN( se, VF::DivCeil_v_s,		"DivCeil",	{"x", "y"} );
	}

/*
=================================================
	BindFloatVec
=================================================
*/
	template <typename T>
	static void  BindFloatVec (ClassBinder<T> &binder, const ScriptEnginePtr &se)
	{
		using Vec_t = T;
		using VF	= VecFunc< Vec_t >;

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Add, VF::Add_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Add, VF::Add_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Add, VF::Add_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Add, VF::Add_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Add, VF::Add_s_v );

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Sub, VF::Sub_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Sub, VF::Sub_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Sub, VF::Sub_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Sub, VF::Sub_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Sub, VF::Sub_s_v );

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Mul, VF::Mul_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Mul, VF::Mul_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Mul, VF::Mul_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Mul, VF::Mul_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Mul, VF::Mul_s_v );

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Div, VF::Div_a_v );
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Div, VF::Div_a_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Div, VF::Div_v_v );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Div, VF::Div_v_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Div, VF::Div_s_v );

		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Mod, VecFunc2::template Mod_a_v< Vec_t >);
		AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Mod, VecFunc2::template Mod_a_s< Vec_t >);
		AS_OP_BINARY_T(		binder, EBinaryOperator::Mod, VecFunc2::template Mod_v_v< Vec_t >);
		AS_OP_BINARY_T(		binder, EBinaryOperator::Mod, VecFunc2::template Mod_v_s< Vec_t >);
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Mod, VecFunc2::template Mod_s_v< Vec_t >);

		AS_OP_EQUAL_T(		binder, VF::Equal );
		AS_OP_CMP_T(		binder, VF::Cmp );

		BindIntFloatVec( binder, se );

		if constexpr( VecSize<Vec_t> == 3 ) {
			AS_GLOBAL_FN( se, VF::Cross,	"Cross",		{"x", "y"} );
		}

		AS_GLOBAL_FN( se, VF::ToSNorm,		"ToSNorm",		{"x"} );
		AS_GLOBAL_FN( se, VF::ToUNorm,		"ToUNorm",		{"x"} );

		AS_GLOBAL_FN( se, VF::Dot,			"Dot",			{"x", "y"} );
		AS_GLOBAL_FN( se, VF::Lerp,			"Lerp",			{"x", "y", "factor"} );
		AS_GLOBAL_FN( se, VF::Normalize,	"Normalize",	{"x"} );
		AS_GLOBAL_FN( se, VF::Floor,		"Floor",		{"x"} );
		AS_GLOBAL_FN( se, VF::Ceil,			"Ceil",			{"x"} );
		AS_GLOBAL_FN( se, VF::Round,		"Round",		{"x"} );
		AS_GLOBAL_FN( se, VF::Fract,		"Fract",		{"x"} );
		AS_GLOBAL_FN( se, VF::Sqrt,			"Sqrt",			{"x"} );
		AS_GLOBAL_FN( se, VF::Ln,			"Ln",			{"x"} );
		AS_GLOBAL_FN( se, VF::Log2,			"Log2",			{"x"} );
		AS_GLOBAL_FN( se, VF::Log_1,		"Log",			{"x", "base"} );
		AS_GLOBAL_FN( se, VF::Log_2,		"Log",			{"x", "base"} );
		AS_GLOBAL_FN( se, VF::Pow_1,		"Pow",			{"x", "pow"} );
		AS_GLOBAL_FN( se, VF::Pow_2,		"Pow",			{"x", "pow"} );
		AS_GLOBAL_FN( se, VF::Exp,			"Exp",			{"x"} );
		AS_GLOBAL_FN( se, VF::Exp2,			"Exp2",			{"x"} );
		AS_GLOBAL_FN( se, VF::Exp10,		"Exp10",		{"x"} );
		AS_GLOBAL_FN( se, VF::Length,		"Length",		{"x"} );
		AS_GLOBAL_FN( se, VF::LengthSq,		"LengthSq",		{"x"} );
		AS_GLOBAL_FN( se, VF::Distance,		"Distance",		{"x", "y"} );
		AS_GLOBAL_FN( se, VF::DistanceSq,	"DistanceSq",	{"x", "y"} );
	}

/*
=================================================
	DefineVector_Func
=================================================
*/
	struct DefineVector_Func
	{
		ScriptEnginePtr	_se;

		explicit DefineVector_Func (const ScriptEnginePtr &se) : _se{se}
		{}

		template <typename T, usize Index>
		void  operator () ()
		{
			ClassBinder<T>	binder{ _se };

			binder.CreateClassValue();
		}
	};

/*
=================================================
	BindFloatVector_Func
=================================================
*/
	struct BindFloatVector_Func
	{
		ScriptEnginePtr	_se;

		explicit BindFloatVector_Func (const ScriptEnginePtr &se) : _se{se}
		{}

		template <typename T, usize Index>
		void  operator () ()
		{
			using Value_t = typename T::value_type;

			ClassBinder<T>	binder{ _se };

			if constexpr( IsSame< Value_t, bool >)
			{
				InitVecFields<T>::Init( binder );
				BindBoolVec( binder, _se );
			}else
			if constexpr( IsFloatPoint< Value_t >)
			{
				InitVecFields<T>::Init( binder );
				BindFloatVec( binder, _se );
			}
		}
	};

/*
=================================================
	BindIntVector_Func
=================================================
*/
	struct BindIntVector_Func
	{
		ScriptEnginePtr	_se;

		explicit BindIntVector_Func (const ScriptEnginePtr &se) : _se{se}
		{}

		template <typename T, usize Index>
		void  operator () ()
		{
			using Value_t = typename T::value_type;

			ClassBinder<T>	binder{ _se };

			if constexpr( IsSame< Value_t, bool > or IsFloatPoint< Value_t >)
			{}else
			if constexpr( IsInteger< Value_t >)
			{
				InitVecFields<T>::Init( binder );
				BindIntVec( binder, _se );
			}
		}
	};

} // namespace
} // AE::Scripting
