// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Math/Vec.h"

namespace AE::Base
{

	//
	// Compile-Time Scale for PhysicalQuantity
	//

	struct PhysicalQuantity_Scale
	{
	private:
		template <typename T>
		NdCx__ static T  _Abs (T val) __NE___
		{
			return val < 0 ? -val : val;
		}

	public:
		template <typename T, int IntVal>
		struct Integer
		{
			static constexpr T	Value = T(IntVal);
		};

		template <typename Src, typename T>
		struct Cast;

		template <typename Lhs, typename Rhs>
		struct Add;

		template <typename Lhs, typename Rhs>
		struct Sub;

		template <typename Lhs, typename Rhs>
		struct Mul;

		template <typename Lhs, typename Rhs>
		struct Div;

		template <typename S>
		struct Inverse;

		template <typename S, uint Power>
		struct Pow;
	};



	//
	// Cast
	//
	template <typename Src, typename T>
	struct PhysicalQuantity_Scale::Cast
	{
		static constexpr auto	Value	= T(Src::Value);
	};


	//
	// Add
	//
	template <typename Lhs, typename Rhs>
	struct PhysicalQuantity_Scale::Add
	{
		static constexpr auto	Value	= Min( Lhs::Value, Rhs::Value );

		template <typename LT, typename RT>
		static constexpr auto  Get (LT lhs, RT rhs) __NE___
		{
			if constexpr( _Abs(Lhs::Value) < _Abs(Rhs::Value) )
				return lhs + rhs * RT(Rhs::Value / Lhs::Value);
			else
				return lhs * LT(Lhs::Value / Rhs::Value) + rhs;
		}
	};


	//
	// Sub
	//
	template <typename Lhs, typename Rhs>
	struct PhysicalQuantity_Scale::Sub
	{
		static constexpr auto	Value	= Min( Lhs::Value, Rhs::Value );

		template <typename LT, typename RT>
		static constexpr auto  Get (LT lhs, RT rhs) __NE___
		{
			if constexpr( _Abs(Lhs::Value) < _Abs(Rhs::Value) )
				return lhs - rhs * RT(Rhs::Value / Lhs::Value);
			else
				return lhs * LT(Lhs::Value / Rhs::Value) - rhs;
		}
	};


	//
	// Mul
	//
	template <typename Lhs, typename Rhs>
	struct PhysicalQuantity_Scale::Mul
	{
		static constexpr auto	Value	= Lhs::Value * Rhs::Value;

		template <typename LT, typename RT>
		static constexpr auto  Get (LT lhs, RT rhs) __NE___
		{
			return lhs * rhs;
		}
	};


	//
	// Div
	//
	template <typename Lhs, typename Rhs>
	struct PhysicalQuantity_Scale::Div
	{
		static constexpr auto	Value	= Lhs::Value / Rhs::Value;

		template <typename LT, typename RT>
		static constexpr auto  Get (LT lhs, RT rhs) __NE___
		{
			return lhs / rhs;
		}
	};


	//
	// Inverse
	//
	template <typename S>
	struct PhysicalQuantity_Scale::Inverse
	{
		static constexpr auto	Value	= decltype(S::Value){1} / S::Value;

		template <typename T>
		static constexpr T  Get (T val) __NE___
		{
			return T{1} / val;
		}
	};


	//
	// Pow
	//
	template <typename S, uint Power>
	struct PhysicalQuantity_Scale::Pow
	{
		static constexpr auto	Value	= S::Value * Pow<S, Power-1>::Value;

		template <typename T>
		static constexpr T  Get (T val) __NE___
		{
			return val * T(Pow<S, Power-1>::Value);
		}
	};

	template <typename S>
	struct PhysicalQuantity_Scale::Pow< S, 1 >
	{
		static constexpr auto	Value	= S::Value;

		template <typename T>
		static constexpr T  Get (T val) __NE___
		{
			return val;
		}
	};

	template <typename S>
	struct PhysicalQuantity_Scale::Pow< S, 0 >
	{};


} // AE::Base
