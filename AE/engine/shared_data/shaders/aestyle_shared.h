// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef __cplusplus
# error Only for C++ code!
#endif

#define ND_		[[nodiscard]]

#define AE_ENABLE_BYTE_TYPE		1
#define AE_ENABLE_SHORT_TYPE	1
#define AE_ENABLE_LONG_TYPE		1
#define AE_ENABLE_HALF_TYPE		0
#define AE_ENABLE_DOUBLE_TYPE	0

using sbyte		= signed char;
using ubyte		= unsigned char;
using sshort	= signed short;
using ushort	= unsigned short;
using uint		= unsigned int;
using sint		= signed int;
using ulong		= unsigned long long;
using slong		= signed long long;

#if AE_ENABLE_HALF_TYPE
	using half	= float;
#endif

#ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wundefined-internal"
#endif

template <typename T, int I>
struct _VecBase;

template <typename T, int I>
struct _Vec;

template <typename T, int C, int R>
struct _Matrix;

template <typename T>
struct _VecBase <T,2>
{
	T	x, y;
	T	r, g;

	ND_ T&  operator [] (int idx);
	ND_ T&  operator [] (uint idx);
	ND_ T   operator [] (int idx) const;
	ND_ T   operator [] (uint idx) const;
	ND_ int	length () const	{ return 2; }
};

template <typename T>
struct _VecBase <T,3>
{
	T	x, y, z;
	T	r, g, b;

	ND_ T&  operator [] (int idx);
	ND_ T&  operator [] (uint idx);
	ND_ T   operator [] (int idx) const;
	ND_ T   operator [] (uint idx) const;
	ND_ int	length () const	{ return 3; }
};

template <typename T>
struct _VecBase <T,4>
{
	T	x, y, z, w;
	T	r, g, b, a;

	ND_ T&  operator [] (int idx);
	ND_ T&  operator [] (uint idx);
	ND_ T   operator [] (int idx) const;
	ND_ T   operator [] (uint idx) const;
	ND_ int	length () const	{ return 4; }
};

template <typename T, int I>
struct _PVec
{
	_VecBase<T,I> const*	_ptr;

	template <typename B>
	operator _Vec<B,I> () const;

	_PVec&  operator = (const _Vec<T,I>);
};

#define VEC2_SWIZZLE						\
	_PVec<T,2>	xx, yy, xy, yx;				\

#define VEC3_SWIZZLE						\
	VEC2_SWIZZLE							\
											\
	_PVec<T,3>	rgb, xyz, zy, yzx;			\

#define VEC4_SWIZZLE						\
	VEC3_SWIZZLE							\
											\
	_PVec<T,2>	zw;							\
	_PVec<T,4>  xyzw;


template <typename T>
struct _Vec <T,2> : _VecBase<T,2>
{
	union {
		VEC2_SWIZZLE
	};

	_Vec ();
	explicit _Vec (const T xy);
	_Vec (const T x, const T y);
	template <typename T2> explicit _Vec (const _Vec<T2,2> &);
	//_Vec&  operator = (const _Vec<T,2> &);

	template <typename B=T> requires(std::is_same_v<B, bool>)
	ND_ _Vec  operator ! () const;

	template <typename I=T> requires(std::is_integral_v<I>)
	ND_ _Vec  operator % (const _Vec &b) const;
};

template <typename T>
struct _Vec <T,3> : _VecBase<T,3>
{
	union {
		VEC3_SWIZZLE
	};

	_Vec ();
	explicit _Vec (const T xyz);
	_Vec (const T x, const T y, const T z);
	_Vec (const _Vec<T,2> &xy, const T z);
	_Vec (const T x, const _Vec<T,2> &yz);
	template <typename T2> explicit _Vec (const _Vec<T2,3> &);
	//_Vec&  operator = (const _Vec<T,3> &);

	template <typename B=T> requires(std::is_same_v<B, bool>)
	ND_ _Vec  operator ! () const;

	template <typename I=T> requires(std::is_integral_v<I>)
	ND_ _Vec  operator % (const _Vec &b) const;
};

template <typename T>
struct _Vec <T,4> : _VecBase<T,4>
{
	union {
		VEC4_SWIZZLE
	};

	_Vec ();
	explicit _Vec (const T xyzw);
	_Vec (const T x, const T y, const T z, const T w);
	_Vec (const _Vec<T,2> &xy, const _Vec<T,2> &zw);
	_Vec (const _Vec<T,2> &xy, const T z, const T w);
	_Vec (const T x, const T y, const _Vec<T,2> &zw);
	_Vec (const _Vec<T,3> &xyz, const T w);
	_Vec (const T x, const _Vec<T,3> &yzw);
	template <typename T2> explicit _Vec (const _Vec<T2,4> &);
	//_Vec&  operator = (const _Vec<T,4>);

	template <typename B=T> requires(std::is_same_v<B, bool>)
	ND_ _Vec  operator ! () const;

	template <typename I=T> requires(std::is_integral_v<I>)
	ND_ _Vec  operator % (const _Vec &b) const;
};


template <typename T, int C, int R>
struct _MatrixBase
{
private:
	_Vec<T,R>	_cols [C];

public:
	ND_ _Vec<T,R>&			operator [] (int c);
	ND_ _Vec<T,R>&			operator [] (uint c);
	ND_ _Vec<T,R> const&	operator [] (int c)					const;
	ND_ _Vec<T,R> const&	operator [] (uint c)				const;
	ND_ int					length ()							const	{ return C; }
};

template <typename T>
struct _Matrix <T,2,2> : _MatrixBase<T,2,2>
{
	_Matrix ();
	_Matrix (T c00, T c01,
			 T c10, T c11);
	_Matrix (const _Vec<T,2> c0,
			 const _Vec<T,2> c1);
	_Matrix (const _Matrix &);
};

template <typename T>
struct _Matrix <T,2,3> : _MatrixBase<T,2,3>
{
	_Matrix ();
	_Matrix (T c00, T c01, T c02,
			 T c10, T c11, T c12);
	_Matrix (const _Vec<T,3> c0,
			 const _Vec<T,3> c1);
	_Matrix (const _Matrix &);
};

template <typename T>
struct _Matrix <T,2,4> : _MatrixBase<T,2,4>
{
	_Matrix ();
	_Matrix (T c00, T c01, T c02, T c03,
			 T c10, T c11, T c12, T c13);
	_Matrix (const _Vec<T,4> c0,
			 const _Vec<T,4> c1);
	_Matrix (const _Matrix &);
};

template <typename T>
struct _Matrix <T,3,2> : _MatrixBase<T,3,2>
{
	_Matrix ();
	_Matrix (T c00, T c01,
			 T c10, T c11,
			 T c20, T c21);
	_Matrix (const _Vec<T,2> c0,
			 const _Vec<T,2> c1,
			 const _Vec<T,2> c2);
	_Matrix (const _Matrix &);
};

template <typename T>
struct _Matrix <T,3,3> : _MatrixBase<T,3,3>
{
	_Matrix ();
	_Matrix (T c00, T c01, T c02,
			 T c10, T c11, T c12,
			 T c20, T c21, T c22);
	_Matrix (const _Vec<T,3> c0,
			 const _Vec<T,3> c1,
			 const _Vec<T,3> c2);
	_Matrix (const _Matrix &);
};

template <typename T>
struct _Matrix <T,3,4> : _MatrixBase<T,3,4>
{
	_Matrix ();
	_Matrix (T c00, T c01, T c02, T c03,
			 T c10, T c11, T c12, T c13,
			 T c20, T c21, T c22, T c23);
	_Matrix (const _Vec<T,4> c0,
			 const _Vec<T,4> c1,
			 const _Vec<T,4> c2);
	_Matrix (const _Matrix &);
};

template <typename T>
struct _Matrix <T,4,2> : _MatrixBase<T,4,2>
{
	_Matrix ();
	_Matrix (T c00, T c01,
			 T c10, T c11,
			 T c20, T c21,
			 T c30, T c31);
	_Matrix (const _Vec<T,2> c0,
			 const _Vec<T,2> c1,
			 const _Vec<T,2> c2,
			 const _Vec<T,2> c3);
	_Matrix (const _Matrix &);
};

template <typename T>
struct _Matrix <T,4,3> : _MatrixBase<T,4,3>
{
	_Matrix ();
	_Matrix (T c00, T c01, T c02,
			 T c10, T c11, T c12,
			 T c20, T c21, T c22,
			 T c30, T c31, T c32);
	_Matrix (const _Vec<T,3> c0,
			 const _Vec<T,3> c1,
			 const _Vec<T,3> c2,
			 const _Vec<T,3> c3);
	_Matrix (const _Matrix &);
};

template <typename T>
struct _Matrix <T,4,4> : _MatrixBase<T,4,4>
{
	_Matrix ();
	_Matrix (T c00, T c01, T c02, T c03,
			 T c10, T c11, T c12, T c13,
			 T c20, T c21, T c22, T c23,
			 T c30, T c31, T c32, T c33);
	_Matrix (const _Vec<T,4> c0,
			 const _Vec<T,4> c1,
			 const _Vec<T,4> c2,
			 const _Vec<T,4> c3);
	_Matrix (const _Matrix &);
};


#undef VEC2_SWIZZLE
#undef VEC3_SWIZZLE
#undef VEC4_SWIZZLE

using bool2		= _Vec< bool, 2 >;
using bool3		= _Vec< bool, 3 >;
using bool4		= _Vec< bool, 4 >;

using float2	= _Vec< float, 2 >;
using float3	= _Vec< float, 3 >;
using float4	= _Vec< float, 4 >;

using sbyte2	= _Vec< sbyte, 2 >;
using sbyte3	= _Vec< sbyte, 3 >;
using sbyte4	= _Vec< sbyte, 4 >;

using ubyte2	= _Vec< ubyte, 2 >;
using ubyte3	= _Vec< ubyte, 3 >;
using ubyte4	= _Vec< ubyte, 4 >;

using sshort2	= _Vec< sshort, 2 >;
using sshort3	= _Vec< sshort, 3 >;
using sshort4	= _Vec< sshort, 4 >;

using ushort2	= _Vec< ushort, 2 >;
using ushort3	= _Vec< ushort, 3 >;
using ushort4	= _Vec< ushort, 4 >;

using int2		= _Vec< int, 2 >;
using int3		= _Vec< int, 3 >;
using int4		= _Vec< int, 4 >;

using uint2		= _Vec< uint, 2 >;
using uint3		= _Vec< uint, 3 >;
using uint4		= _Vec< uint, 4 >;

using slong2	= _Vec< slong, 2 >;
using slong3	= _Vec< slong, 3 >;
using slong4	= _Vec< slong, 4 >;

using ulong2	= _Vec< ulong, 2 >;
using ulong3	= _Vec< ulong, 3 >;
using ulong4	= _Vec< ulong, 4 >;

using float2x2	= _Matrix< float, 2, 2 >;
using float2x3	= _Matrix< float, 2, 3 >;
using float2x4	= _Matrix< float, 2, 4 >;
using float3x2	= _Matrix< float, 3, 2 >;
using float3x3	= _Matrix< float, 3, 3 >;
using float3x4	= _Matrix< float, 3, 4 >;
using float4x2	= _Matrix< float, 4, 2 >;
using float4x3	= _Matrix< float, 4, 3 >;
using float4x4	= _Matrix< float, 4, 4 >;

#if AE_ENABLE_HALF_TYPE
	using half2		= _Vec< half, 2 >;
	using half3		= _Vec< half, 3 >;
	using half4		= _Vec< half, 4 >;

	using half2x2	= _Matrix< half, 2, 2 >;
	using half2x3	= _Matrix< half, 2, 3 >;
	using half2x4	= _Matrix< half, 2, 4 >;
	using half3x2	= _Matrix< half, 3, 2 >;
	using half3x3	= _Matrix< half, 3, 3 >;
	using half3x4	= _Matrix< half, 3, 4 >;
	using half4x2	= _Matrix< half, 4, 2 >;
	using half4x3	= _Matrix< half, 4, 3 >;
	using half4x4	= _Matrix< half, 4, 4 >;
#endif

#if AE_ENABLE_DOUBLE_TYPE
	using double2	= _Vec< double, 2 >;
	using double3	= _Vec< double, 3 >;
	using double4	= _Vec< double, 4 >;

	using double2x2	= _Matrix< double, 2, 2 >;
	using double2x3	= _Matrix< double, 2, 3 >;
	using double2x4	= _Matrix< double, 2, 4 >;
	using double3x2	= _Matrix< double, 3, 2 >;
	using double3x3	= _Matrix< double, 3, 3 >;
	using double3x4	= _Matrix< double, 3, 4 >;
	using double4x2	= _Matrix< double, 4, 2 >;
	using double4x3	= _Matrix< double, 4, 3 >;
	using double4x4	= _Matrix< double, 4, 4 >;
#endif

template <typename T>
using _Scalar = std::enable_if_t< std::is_scalar_v<T>, T >;


template <typename T, int I>	ND_ _Vec<T,I>	operator +  (const _Vec<T,I> x);
template <typename T, int I>	ND_ _PVec<T,I>	operator +  (const _PVec<T,I> x);

template <typename T, int I>	ND_ _Vec<T,I>	operator -  (const _Vec<T,I> x);
template <typename T, int I>	ND_ _PVec<T,I>	operator -  (const _PVec<T,I> x);


template <typename T, int I>	_Vec<T,I> &		operator += (_Vec<T,I>  &x, const _Vec<T,I> y);
template <typename T, int I>	_Vec<T,I> &		operator += (_Vec<T,I>  &x, const T y);
template <typename T, int I>	_PVec<T,I> &	operator += (_PVec<T,I> &x, const _PVec<T,I> y);
template <typename T, int I>	_PVec<T,I> &	operator += (_PVec<T,I> &x, const T y);

template <typename T, int I>	_Vec<T,I> &		operator -= (_Vec<T,I>  &x, const _Vec<T,I> y);
template <typename T, int I>	_Vec<T,I> &		operator -= (_Vec<T,I>  &x, const T y);
template <typename T, int I>	_PVec<T,I> &	operator -= (_PVec<T,I> &x, const _PVec<T,I> y);
template <typename T, int I>	_PVec<T,I> &	operator -= (_PVec<T,I> &x, const T y);

template <typename T, int I>	_Vec<T,I> &		operator *= (_Vec<T,I>  &x, const _Vec<T,I> y);
template <typename T, int I>	_Vec<T,I> &		operator *= (_Vec<T,I>  &x, const T y);
template <typename T, int I>	_PVec<T,I> &	operator *= (_PVec<T,I> &x, const _PVec<T,I> y);
template <typename T, int I>	_PVec<T,I> &	operator *= (_PVec<T,I> &x, const T y);

template <typename T, int I>	_Vec<T,I> &		operator /= (_Vec<T,I>  &x, const _Vec<T,I> y);
template <typename T, int I>	_Vec<T,I> &		operator /= (_Vec<T,I>  &x, const T y);
template <typename T, int I>	_PVec<T,I> &	operator /= (_PVec<T,I> &x, const _PVec<T,I> y);
template <typename T, int I>	_PVec<T,I> &	operator /= (_PVec<T,I> &x, const T y);

template <typename T, int I>	_Vec<T,I> &		operator &= (_Vec<T,I>  &x, const _Vec<T,I> y);
template <typename T, int I>	_Vec<T,I> &		operator &= (_Vec<T,I>  &x, const T y);
template <typename T, int I>	_PVec<T,I> &	operator &= (_PVec<T,I> &x, const _PVec<T,I> y);
template <typename T, int I>	_PVec<T,I> &	operator &= (_PVec<T,I> &x, const T y);

template <typename T, int I>	_Vec<T,I> &		operator |= (_Vec<T,I>  &x, const _Vec<T,I> y);
template <typename T, int I>	_Vec<T,I> &		operator |= (_Vec<T,I>  &x, const T y);
template <typename T, int I>	_PVec<T,I> &	operator |= (_PVec<T,I> &x, const _PVec<T,I> y);
template <typename T, int I>	_PVec<T,I> &	operator |= (_PVec<T,I> &x, const T y);

template <typename T, int I>	_Vec<T,I> &		operator ^= (_Vec<T,I>  &x, const _Vec<T,I> y);
template <typename T, int I>	_Vec<T,I> &		operator ^= (_Vec<T,I>  &x, const T y);
template <typename T, int I>	_PVec<T,I> &	operator ^= (_PVec<T,I> &x, const _PVec<T,I> y);
template <typename T, int I>	_PVec<T,I> &	operator ^= (_PVec<T,I> &x, const T y);

template <typename T, int I>	_Vec<T,I> &		operator >>= (_Vec<T,I>  &x, const _Vec<T,I> y);
template <typename T, int I>	_Vec<T,I> &		operator >>= (_Vec<T,I>  &x, const T y);
template <typename T, int I>	_PVec<T,I> &	operator >>= (_PVec<T,I> &x, const _PVec<T,I> y);
template <typename T, int I>	_PVec<T,I> &	operator >>= (_PVec<T,I> &x, const T y);

template <typename T, int I>	_Vec<T,I> &		operator <<= (_Vec<T,I>  &x, const _Vec<T,I> y);
template <typename T, int I>	_Vec<T,I> &		operator <<= (_Vec<T,I>  &x, const T y);
template <typename T, int I>	_PVec<T,I> &	operator <<= (_PVec<T,I> &x, const _PVec<T,I> y);
template <typename T, int I>	_PVec<T,I> &	operator <<= (_PVec<T,I> &x, const T y);


template <typename T, int I>	ND_ _Vec<T,I>	operator +  (const _Vec<T,I>  x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator +  (const T          x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator +  (const _Vec<T,I>  x, const T y);
template <typename T, int I>	ND_ _PVec<T,I>	operator +  (const _PVec<T,I> x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator +  (const T          x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator +  (const _PVec<T,I> x, const T y);

template <typename T, int I>	ND_ _Vec<T,I>	operator -  (const _Vec<T,I>  x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator -  (const T          x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator -  (const _Vec<T,I>  x, const T y);
template <typename T, int I>	ND_ _PVec<T,I>	operator -  (const _PVec<T,I> x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator -  (const T          x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator -  (const _PVec<T,I> x, const T y);

template <typename T, int I>	ND_ _Vec<T,I>	operator *  (const _Vec<T,I>  x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator *  (const T          x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator *  (const _Vec<T,I>  x, const T y);
template <typename T, int I>	ND_ _PVec<T,I>	operator *  (const _PVec<T,I> x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator *  (const T          x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator *  (const _PVec<T,I> x, const T y);

template <typename T, int I>	ND_ _Vec<T,I>	operator /  (const _Vec<T,I>  x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator /  (const T          x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator /  (const _Vec<T,I>  x, const T y);
template <typename T, int I>	ND_ _PVec<T,I>	operator /  (const _PVec<T,I> x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator /  (const T          x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator /  (const _PVec<T,I> x, const T y);

template <typename T, int I>	ND_ _Vec<T,I>	operator &  (const _Vec<T,I>  x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator &  (const T          x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator &  (const _Vec<T,I>  x, const T y);
template <typename T, int I>	ND_ _PVec<T,I>	operator &  (const _PVec<T,I> x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator &  (const T          x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator &  (const _PVec<T,I> x, const T y);

template <typename T, int I>	ND_ _Vec<T,I>	operator |  (const _Vec<T,I>  x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator |  (const T          x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator |  (const _Vec<T,I>  x, const T y);
template <typename T, int I>	ND_ _PVec<T,I>	operator |  (const _PVec<T,I> x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator |  (const T          x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator |  (const _PVec<T,I> x, const T y);

template <typename T, int I>	ND_ _Vec<T,I>	operator ^  (const _Vec<T,I>  x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator ^  (const T          x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator ^  (const _Vec<T,I>  x, const T y);
template <typename T, int I>	ND_ _PVec<T,I>	operator ^  (const _PVec<T,I> x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator ^  (const T          x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator ^  (const _PVec<T,I> x, const T y);

template <typename T, int I>	ND_ _Vec<T,I>	operator >> (const _Vec<T,I>  x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator >> (const T          x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator >> (const _Vec<T,I>  x, const T y);
template <typename T, int I>	ND_ _PVec<T,I>	operator >> (const _PVec<T,I> x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator >> (const T          x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator >> (const _PVec<T,I> x, const T y);

template <typename T, int I>	ND_ _Vec<T,I>	operator << (const _Vec<T,I>  x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator << (const T          x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>	operator << (const _Vec<T,I>  x, const T y);
template <typename T, int I>	ND_ _PVec<T,I>	operator << (const _PVec<T,I> x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator << (const T          x, const _PVec<T,I> y);
template <typename T, int I>	ND_ _PVec<T,I>	operator << (const _PVec<T,I> x, const T y);


template <typename T, int C, int R, int Q>	ND_ _Matrix<T,Q,R>  operator * (const _Matrix<T,C,R> &x, const _Matrix<T,Q,C> &y);
template <typename T, int C, int R>			ND_ _Vec<T,R>		operator * (const _Matrix<T,C,R> &x, const _Vec<T,C> y);
template <typename T, int C, int R>			ND_ _Vec<T,C>		operator * (const _Vec<T,R> x, const _Matrix<T,C,R> &y);


#ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic pop
#endif
