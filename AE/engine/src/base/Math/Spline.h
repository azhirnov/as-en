// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"

namespace AE::Math
{

	//
	// B-Spline
	//
	template <typename VecType>
	struct BSpline
	{
		StaticAssert( IsFloatPointVec< VecType >);

	// types
	public:
		using Vec_t		= VecType;
		using Value_t	= VecToScalarType< VecType >;
	private:
		using T			= Value_t;


	// variables
	private:
		Vec_t	_c0, _c1, _c2, _c3;


	// methods
	public:
		BSpline ()																		__NE___	{}
		BSpline (const Vec_t &p0, const Vec_t &p1, const Vec_t &p2, const Vec_t &p3)	__NE___	{ Init( p0, p1, p2, p3 ); }

		void  Init (const Vec_t &p0, const Vec_t &p1, const Vec_t &p2, const Vec_t &p3)	__NE___
		{
			_c0 = (p0 + T(4.0) * p1 + p2) / T(6.0);
			_c1 = (p2 - p0) / T(2.0);
			_c2 = (p0 + p2) / T(2.0) - p1;
			_c3 = (p3 - p0) / T(6.0) + (p1 - p2) / T(2.0);
		}

		ND_ Vec_t  operator () (const T a)												C_NE___
		{
			return _c0 + (_c1 * a) + (_c2 * a * a) + (_c3 * a * a * a);
		}
	};



	//
	// Chordal Catmull-Rom
	//
	template <typename VecType>
	struct ChordalCatmullRom
	{
		StaticAssert( IsFloatPointVec< VecType >);

	// types
	public:
		using Vec_t		= VecType;
		using Value_t	= VecToScalarType< VecType >;
	private:
		using T			= Value_t;


	// variables
	private:
		Vec_t	_c0, _c1, _c2, _c3;


	// methods
	public:
		ChordalCatmullRom ()																	__NE___	{}
		ChordalCatmullRom (const Vec_t &p0, const Vec_t &p1, const Vec_t &p2, const Vec_t &p3)	__NE___	{ Init( p0, p1, p2, p3 ); }

		void  Init (const Vec_t &p0, const Vec_t &p1, const Vec_t &p2, const Vec_t &p3)			__NE___
		{
			_c0	= ( T(2.0) * p1 );
			_c1 = ( -p0 + p2 );
			_c2 = ( T(2.0) * p0 - T(5.0) * p1 + T(4.0) * p2 - p3 );
			_c3 = ( -p0 + T(3.0) * p1 - T(3.0) * p2 + p3 );
		}

		ND_ Vec_t  operator () (const T a)														C_NE___
		{
			return T(0.5) * ((_c0) + (_c1 * a) + (_c2 * a * a) + (_c3 * a * a * a));
		}
	};


} // AE::Math
