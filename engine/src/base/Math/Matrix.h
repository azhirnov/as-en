// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Quat.h"
#include "base/Math/Radians.h"
#include "base/Math/Rectangle.h"

namespace AE::Math
{
	using half2x2	= Matrix< half, 2, 2 >;
	using half2x3	= Matrix< half, 2, 3 >;
	using half2x4	= Matrix< half, 2, 4 >;
	using half3x2	= Matrix< half, 3, 2 >;
	using half3x3	= Matrix< half, 3, 3 >;
	using half3x4	= Matrix< half, 3, 4 >;
	using half4x2	= Matrix< half, 4, 2 >;
	using half4x3	= Matrix< half, 4, 3 >;
	using half4x4	= Matrix< half, 4, 4 >;

	using float2x2	= Matrix< float, 2, 2 >;
	using float2x3	= Matrix< float, 2, 3 >;
	using float2x4	= Matrix< float, 2, 4 >;
	using float3x2	= Matrix< float, 3, 2 >;
	using float3x3	= Matrix< float, 3, 3 >;
	using float3x4	= Matrix< float, 3, 4 >;
	using float4x2	= Matrix< float, 4, 2 >;
	using float4x3	= Matrix< float, 4, 3 >;
	using float4x4	= Matrix< float, 4, 4 >;
	
	using double2x2	= Matrix< double, 2, 2 >;
	using double2x3	= Matrix< double, 2, 3 >;
	using double2x4	= Matrix< double, 2, 4 >;
	using double3x2	= Matrix< double, 3, 2 >;
	using double3x3	= Matrix< double, 3, 3 >;
	using double3x4	= Matrix< double, 3, 4 >;
	using double4x2	= Matrix< double, 4, 2 >;
	using double4x3	= Matrix< double, 4, 3 >;
	using double4x4	= Matrix< double, 4, 4 >;

namespace _hidden_
{
	struct _MatrixDim
	{
		ubyte		columns;
		ubyte		rows;
	};
}
} // AE::Math


#define Columns	2
#define Rows	2
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns	2
#define Rows	3
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns	2
#define Rows	4
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns	3
#define Rows	2
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns	3
#define Rows	3
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns	3
#define Rows	4
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns	4
#define Rows	2
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns	4
#define Rows	3
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns	4
#define Rows	4
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows


namespace AE::Math
{
/*
=================================================
	operator *
=================================================
*/
	template <typename T, uint C, uint R, uint Q, glm::qualifier Qf>
	ND_ inline TMatrix<T,Q,R,Qf>  operator * (const TMatrix<T,C,R, Qf> &lhs, const TMatrix<T,Q,C,Qf> &rhs) __NE___
	{
		return TMatrix<T,Q,R,Qf>{ lhs._value * rhs._value };
	}

/*
=================================================
	Equals
=================================================
*/
	template <typename T, uint C, uint R, glm::qualifier Q>
	ND_ forceinline bool  Equals (const TMatrix<T,C,R,Q> &lhs, const TMatrix<T,C,R,Q> &rhs, const T &err = Epsilon<T>()) __NE___
	{
		bool	res = true;
		for (uint i = 0; i < C; ++i) {
			if ( not All( Equals( lhs[i], rhs[i] )) )
				res = false;
		}
		return res;
	}
	
/*
=================================================
	Quat()
=================================================
*/
	template <typename T>
	template <glm::qualifier Q>
	inline Quat<T>::Quat (const TMatrix<T,3,3,Q> &m) __NE___ : _value{glm::quat_cast(m)}
	{}
	
	template <typename T>
	template <glm::qualifier Q>
	inline Quat<T>::Quat (const TMatrix<T,4,4,Q> &m) __NE___ : _value{glm::quat_cast(m)}
	{}


} // AE::Math
