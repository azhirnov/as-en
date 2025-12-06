// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	used only for auto-complete in shader code
*/

#pragma once

#ifndef __cplusplus
# error Only for C++ code!
#endif

#include <type_traits>

#define isinf	_IsInf
#define isnan	_IsNaN

#define highp
#define mediump
#define lowp
#define precise		// avoid optimizations
#define WGShared	// workgroup shared variable qualifier
#define invariant	// all shaders must output same result on same input, only for shader IO

#define coherent

#define out
#define inout
#define in
#define layout(...)

#define INOUT
#define OUT

#define length			__reserved__
#define input			__reserved__

// used for vec/mat type building (templates)
// AEStyleGLSLPreprocessor will replace it by GLSL vec/mat type without dimension suffix
#define float_vec_t		float
#define float_mat_t		float
#define double_vec_t	double
#define double_mat_t	double
#define int_vec_t		int
#define uint_vec_t		uint
#define bool_vec_t		bool
#define sbyte_vec_t		sbyte
#define ubyte_vec_t		ubyte
#define sshort_vec_t	sshort
#define ushort_vec_t	ushort
#define half_vec_t		half
#define half_mat_t		half
#define slong_vec_t		slong
#define ulong_vec_t		ulong


#ifdef AE_bfloat16
	enum class bfloat : short {};		// implicitly converted to float/double
#endif
#ifdef AE_float8_e5m2_e4m3
	enum class floatE5M2 : char {};
	enum class floatE4M3 : char {};
#endif

// Math types
#if 1
#include "aestyle_shared.h"

# ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wundefined-internal"
# endif

template <typename T>			ND_ T				abs (const T);
template <typename T>			ND_ T				acos (const T x);						// result in range [0, Pi], undefined if Abs(x) > 1
template <typename T>			ND_ T				acosh (const T x);						// result is non-negative inverse of cosh, undefined if x < 1
								ND_ bool			all (const bool2);
								ND_ bool			all (const bool3);
								ND_ bool			all (const bool4);
								ND_ bool			any (const bool2);
								ND_ bool			any (const bool3);
								ND_ bool			any (const bool4);
template <typename T>			ND_ T				asin (const T);							// result in range [-Pi/2, Pi/2], undefined if Abs(x) > 1
template <typename T>			ND_ T				asinh (const T);						// result is inverse of sinh
template <typename T>			ND_ T				atan (const T y, const T x);			// result in range [-Pi, Pi], undefined if x = 0
template <typename T>			ND_ T				atan (const T y_over_x);				// result in range [-Pi/2, Pi/2]
template <typename T>			ND_ T				atanh (const T);						// result is inverse of tanh, undefined if Abs(x) > 1
template <typename T>			ND_ T				bitCount (const T);

// For signed data types, the most significant bits will be set to the value of bit offset + base - 1 (i.e., it is sign extended to the width of the return type).
// The result will be undefined if offset or bits is negative, or if the sum of offset and bits is greater than the number of bits used to store the operand.
template <typename T>			ND_ T				bitfieldExtract (const T value, int offset, int bits);

// The result will be undefined if offset or bits is negative, or if the sum of offset and bits is greater than the number of bits used to store the operand.
template <typename T>			ND_ T				bitfieldInsert (const T base, const T insert, int offset, int bits);
template <typename T>			ND_ T				bitfieldReverse (const T);
template <typename T>			ND_ T				ceil (const T);
template <typename T>			ND_ _Scalar<T>		clamp (const T x, const T minVal, const T maxVal);
template <typename T, int I>	ND_ _Vec<T,I>		clamp (const _Vec<T,I> x, const T minVal, const T maxVal);
template <typename T, int I>	ND_ _Vec<T,I>		clamp (const _Vec<T,I> x, const _Vec<T,I> minVal, const _Vec<T,I> maxVal);
template <typename T>			ND_ T				cos (const T);
template <typename T>			ND_ T				cosh (const T);
template <typename T>			ND_ _Vec<T,3>		cross (const _Vec<T,3> x, const _Vec<T,3> y) requires(std::is_floating_point_v<T>);
template <typename T>			ND_ T				degrees (const T);
template <typename T, int I>	ND_ T				distance (const _Vec<T,I>, const _Vec<T,I>);
template <typename T, int I>	ND_ T				dot (const _Vec<T,I>, const _Vec<T,I>);
template <typename T, int I>	ND_ _Vec<bool,I>	equal (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T>			ND_ T				exp (const T);
template <typename T>			ND_ T				exp2 (const T);
template <typename T>			ND_ T				faceforward (const T n, const T i, const T Nref);
template <typename T>			ND_ T				findLSB (const T);						// -1 on incorrect input
template <typename T>			ND_ T				findMSB (const T);						// -1 on incorrect input
								ND_ int				floatBitsToInt (const float);			// inverse intBitsToFloat
template <int I>				ND_ _Vec<int,I>		floatBitsToInt (const _Vec<float,I>);	// inverse intBitsToFloat
								ND_ uint			floatBitsToUint (const float);			// inverse uintBitsToFloat
template <int I>				ND_ _Vec<uint,I>	floatBitsToUint (const _Vec<float,I>);	// inverse uintBitsToFloat
template <typename T>			ND_ T				floor (const T);
template <typename T>			ND_ T				fma (const T a, const T b, const T c);
template <typename T>			ND_ T				fract (const T);
template <typename T>			ND_ T				frexp (const T x, OUT T &exp);
template <typename T, int I>	ND_ _Vec<bool,I>	greaterThan (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<bool,I>	greaterThanEqual (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T>			    void			umulExtended (const T x, const T y, OUT T &msb, OUT T &lsb);
template <typename T>			    void			imulExtended (const T x, const T y, OUT T &msb, OUT T &lsb);
								ND_ float			intBitsToFloat (const int);				// inverse floatBitsToInt
template <int I>				ND_ _Vec<float,I>	intBitsToFloat (const _Vec<int,I>);		// inverse floatBitsToInt
								ND_ float			uintBitsToFloat (const uint);			// inverse floatBitsToUint
template <int I>				ND_ _Vec<float,I>	uintBitsToFloat (const _Vec<uint,I>);	// inverse floatBitsToUint
template <typename T>			ND_ T				inversesqrt (const T);
template <typename T>			ND_ bool			isinf (const T);
template <typename T, int I>	ND_ _Vec<bool,I>	isinf (const _Vec<T,I>);
template <typename T>			ND_ bool			isinf (const T);
template <typename T, int I>	ND_ _Vec<bool,I>	isinf (const _Vec<T,I>);
template <typename T>			ND_ bool			isnan (const T);
template <typename T, int I>	ND_ _Vec<bool,I>	isnan (const _Vec<T,I>);
template <typename T, typename IT>	ND_ T			ldexp (const T x, const IT exp);
template <int I>				ND_ float			length (const _Vec<float,I>);
template <int I>				ND_ double			length (const _Vec<double,I>);
template <typename T, int I>	ND_ _Vec<bool,I>	lessThan (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<bool,I>	lessThanEqual (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T>			ND_ T				log (const T);									// result is undefined if x <= 0
template <typename T>			ND_ T				log2 (const T);									// result is undefined if x <= 0
template <typename T, int I>	ND_ _Vec<T,I>		max (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>		max (const _Vec<T,I> x, const T y);
template <typename T>			ND_ T				max (const T x, const T y);
template <typename T, int I>	ND_ _Vec<T,I>		min (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>		min (const _Vec<T,I> x, const T y);
template <typename A, typename B> ND_ auto			min (const A x, const B y) -> decltype(A(1) + B(1)) requires(std::is_scalar_v<A> and std::is_scalar_v<B>);
template <typename T, int I>	ND_ _Vec<T,I>		mix (const _Vec<T,I> x, const _Vec<T,I> y, const T a);
template <typename T>			ND_ T				mix (const T x, const T y, const T a);
template <typename T, int I>	ND_ _Vec<T,I>		mod (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T, int I>	ND_ _Vec<T,I>		mod (const _Vec<T,I> x, const T y);
template <typename T>			ND_ T				mod (const T x, const T y);
template <typename T>			ND_ T				modf (const T x, OUT T &i);
template <typename T, int I>	ND_ _Vec<T,I>		normalize (const _Vec<T,I>);
#ifdef __INTELLISENSE__
	template <int I>			ND_ _Vec<bool,I>	not (const _Vec<bool,I>);
#endif
template <typename T, int I>	ND_ _Vec<bool,I>	notEqual (const _Vec<T,I> x, const _Vec<T,I> y);
								ND_ double			packDouble2x32 (const uint2);
								ND_ uint2			unpackDouble2x32 (double);
								ND_ uint			packHalf2x16 (const float2);
								ND_ float2			unpackHalf2x16 (const uint);
								ND_ uint			packUnorm2x16 (const float2);
								ND_ uint			packSnorm2x16 (const float2);
								ND_ uint			packUnorm4x8 (const float4);
								ND_ uint			packSnorm4x8 (const float4);
								ND_ float2			unpackUnorm2x16 (const uint);
								ND_ float2			unpackSnorm2x16 (const uint);
								ND_ float4			unpackUnorm4x8 (const uint);
								ND_ float4			unpackSnorm4x8 (const uint);
template <typename T>			ND_ T				pow (const T x, const T y);
template <typename T>			ND_ T				radians (const T);
template <typename T>			ND_ T				reflect (const T i, const T n);
template <typename T>			ND_ T				refract (const T i, const T n, float eta);
template <typename T>			ND_ T				round (const T);
template <typename T>			ND_ T				roundEven (const T);
template <typename T>			ND_ T				sign (const T);
template <typename T>			ND_ T				sin (const T);
template <typename T>			ND_ T				sinh (const T);
template <typename T>			ND_ T				smoothstep (const T edge0, const T edge1, const T x);
template <typename T, int I>	ND_ _Vec<T,I>		smoothstep (const T edge0, const T edge1, const _Vec<T,I> x);
template <typename T, int I>	ND_ _Vec<T,I>		smoothstep (const _Vec<T,I> edge0, const _Vec<T,I> edge1, const _Vec<T,I> x);
template <typename T>			ND_ T				sqrt (const T);
template <typename T>			ND_ T				step (const T edge, const T x);
template <typename T, int I>	ND_ _Vec<T,I>		step (const T edge, const _Vec<T,I> x);
template <typename T, int I>	ND_ _Vec<T,I>		step (const _Vec<T,I> edge, const _Vec<T,I> x);
template <typename T>			ND_ T				tan (const T);
template <typename T>			ND_ T				tanh (const T);
template <typename T>			ND_ T				trunc (const T);
template <typename T>			ND_ T				uaddCarry (const T x, const T y, OUT T &carry);
template <typename T>			ND_ T				usubBorrow (const T x, const T y, OUT T barrow);

template <typename T, int C, int R>	ND_ T				determinant (const _Matrix<T,C,R>);
template <typename T, int C, int R>	ND_ _Matrix<T,C,R>	inverse (const _Matrix<T,C,R>);
template <typename T, int C, int R>	ND_ _Matrix<T,C,R>	matrixCompMult (const _Matrix<T,C,R> x, const _Matrix<T,C,R> y);
template <typename T, int C, int R>	ND_ _Matrix<T,C,R>	outerProduct (const _Vec<T,R> c, const _Vec<T,C> r);
template <typename T, int C, int R>	ND_ _Matrix<T,R,C>	transpose (const _Matrix<T,C,R>);
#endif

// GL_EXT_shader_explicit_arithmetic_types
#if 1
								ND_ slong			pack64 (const int2 v);
								ND_ ulong			pack64 (const uint2 v);
								ND_ slong			pack64 (const sshort4 v);
								ND_ ulong			pack64 (const ushort4 v);

								ND_ int				pack32 (const sshort2 v);
								ND_ uint			pack32 (const ushort2 v);
								ND_ int				pack32 (const sbyte4 v);
								ND_ uint			pack32 (const ubyte4 v);

								ND_ short			pack16 (const sbyte2 v);
								ND_ ushort			pack16 (const ubyte2 v);

								ND_ int2			unpack64 (const slong v);
								ND_ uint2			unpack64 (const ulong v);
							//	ND_ sshort4			unpack64 (const slong v);
							//	ND_ ushort4			unpack64 (const ulong v);

								ND_ sshort2			unpack32 (const int v);
								ND_ ushort2			unpack32 (const uint v);
							//	ND_ sbyte4			unpack32 (const int v);
							//	ND_ ubyte4			unpack32 (const uint v);

								ND_ sbyte2			unpack16 (const short v);
								ND_ ubyte2			unpack16 (const ushort v);

								ND_ slong			packInt2x32 (const int2 v);
								ND_ ulong			packUint2x32 (const uint2 v);
								ND_ int2			unpackInt2x32 (const slong v);
								ND_ uint2			unpackUint2x32 (const ulong v);

								ND_ int				packInt2x16 (const sshort2 v);
								ND_ slong			packInt4x16 (const sshort4 v);
								ND_ uint			packUint2x16 (const ushort2 v);
								ND_ ulong			packUint4x16 (const ushort4 v);

								ND_ sshort2			unpackInt2x16 (const int v);
								ND_ sshort4			unpackInt4x16 (const slong v);
								ND_ ushort2			unpackUint2x16 (const uint v);
								ND_ ushort4			unpackUint4x16 (const ulong v);

#if AE_ENABLE_HALF_TYPE
								ND_ uint			packFloat2x16 (const half2 v);
								ND_ half2			unpackFloat2x16 (const uint v);

								ND_ short			halfBitsToInt16 (const half);				// inverse int16BitsToHalf
	template <int I>			ND_ _Vec<short,I>	halfBitsToInt16 (const _Vec<half,I>);		// inverse int16BitsToHalf
								ND_ ushort			halfBitsToUint16 (const half);				// inverse uint16BitsToHalf
	template <int I>			ND_ _Vec<ushort,I>	halfBitsToUint16 (const _Vec<half,I>);		// inverse uint16BitsToHalf

								ND_ short			float16BitsToInt16 (const half);			// inverse int16BitsToFloat16
	template <int I>			ND_ _Vec<short,I>	float16BitsToInt16 (const _Vec<half,I>);	// inverse int16BitsToFloat16
								ND_ ushort			float16BitsToUint16 (const half);			// inverse uint16BitsToFloat16
	template <int I>			ND_ _Vec<ushort,I>	float16BitsToUint16 (const _Vec<half,I>);	// inverse uint16BitsToFloat16

								ND_ half			int16BitsToHalf (const short);				// inverse halfBitsToInt16
	template <int I>			ND_ _Vec<half,I>	int16BitsToHalf (const _Vec<short,I>);		// inverse halfBitsToInt16
								ND_ half			uint16BitsToHalf (const ushort);			// inverse halfBitsToUint16
	template <int I>			ND_ _Vec<half,I>	uint16BitsToHalf (const _Vec<ushort,I>);	// inverse halfBitsToUint16

								ND_ half			int16BitsToFloat16 (const short);			// inverse float16BitsToInt16
	template <int I>			ND_ _Vec<half,I>	int16BitsToFloat16 (const _Vec<short,I>);	// inverse float16BitsToInt16
								ND_ half			uint16BitsToFloat16 (const ushort);			// inverse float16BitsToUint16
	template <int I>			ND_ _Vec<half,I>	uint16BitsToFloat16 (const _Vec<ushort,I>);	// inverse float16BitsToUint16
#endif

								ND_ slong			doubleBitsToInt64 (const double);			// inverse int64BitsToDouble
template <int I>				ND_ _Vec<slong,I>	doubleBitsToInt64 (const _Vec<double,I>);	// inverse int64BitsToDouble
								ND_ ulong			doubleBitsToUint64 (const double);			// inverse uint64BitsToDouble
template <int I>				ND_ _Vec<ulong,I>	doubleBitsToUint64 (const _Vec<double,I>);	// inverse uint64BitsToDouble

								ND_ double			int64BitsToDouble (const slong);			// inverse doubleBitsToInt64
template <int I>				ND_ _Vec<double,I>	int64BitsToDouble (const _Vec<slong,I>);	// inverse doubleBitsToInt64
								ND_ double			uint64BitsToDouble (const ulong);			// inverse doubleBitsToUint64
template <int I>				ND_ _Vec<double,I>	uint64BitsToDouble (const _Vec<ulong,I>);	// inverse doubleBitsToUint64


#ifdef AE_bfloat16
# ifdef AE_bfloat16_dotprod
	template <int I>			ND_ bfloat			dot (_Vec<bfloat,I> x, _Vec<bfloat,I> y);
# endif

								ND_ short			bfloat16BitsToInt (const bfloat);			// inverse intBitsToBFloat16
								ND_ ushort			bfloat16BitsToUint (const bfloat);			// inverse uintBitsToBFloat16
								ND_ bfloat			intBitsToBFloat16 (const short);			// inverse bfloat16BitsToInt
								ND_ bfloat			uintBitsToBFloat16 (const ushort);			// inverse bfloat16BitsToUint

	template <int I>			ND_ _Vec<short,I>	bfloat16BitsToInt (const bfloat);			// inverse intBitsToBFloat16
	template <int I>			ND_ _Vec<ushort,I>	bfloat16BitsToUint (const bfloat);			// inverse uintBitsToBFloat16
	template <int I>			ND_ _Vec<bfloat,I>	intBitsToBFloat16 (const short);			// inverse bfloat16BitsToInt
	template <int I>			ND_ _Vec<bfloat,I>	uintBitsToBFloat16 (const ushort);			// inverse bfloat16BitsToUint
#endif

#ifdef AE_float8_e5m2_e4m3
								ND_ sbyte				floate5m2BitsToInt (const floatE5M2);			// inverse intBitsToFloate5m2
								ND_ ubyte				floate5m2BitsToUint (const floatE5M2);			// inverse uintBitsToFloate5m2
								ND_ floatE5M2			intBitsToFloate5m2 (const sbyte);				// inverse floate5m2BitsToInt
								ND_ floatE5M2			uintBitsToFloate5m2 (const ubyte);				// inverse floate5m2BitsToUint

	template <int I>			ND_ _Vec<sbyte,I>		floate5m2BitsToInt (const _Vec<floatE5M2,I>);	// inverse intBitsToFloate5m2
	template <int I>			ND_ _Vec<ubyte,I>		floate5m2BitsToUint (const _Vec<floatE5M2,I>);	// inverse uintBitsToFloate5m2
	template <int I>			ND_ _Vec<floatE5M2,I>	intBitsToFloate5m2 (const _Vec<sbyte,I>);		// inverse floate5m2BitsToInt
	template <int I>			ND_ _Vec<floatE5M2,I>	uintBitsToFloate5m2 (const _Vec<ubyte,I>);		// inverse floate5m2BitsToUint

								ND_ sbyte				floate4m3BitsToInt (const floatE4M3);			// inverse intBitsToFloate4m3
								ND_ ubyte				floate4m3BitsToUint (const floatE4M3);			// inverse uintBitsToFloate4m3
								ND_ floatE4M3			intBitsToFloate4m3 (const sbyte);				// inverse floate4m3BitsToInt
								ND_ floatE4M3			uintBitsToFloate4m3 (const ubyte);				// inverse floate4m3BitsToUint

	template <int I>			ND_ _Vec<sbyte,I>		floate4m3BitsToInt (const _Vec<floatE4M3,I>);	// inverse intBitsToFloate4m3
	template <int I>			ND_ _Vec<ubyte,I>		floate4m3BitsToUint (const _Vec<floatE4M3,I>);	// inverse uintBitsToFloate4m3
	template <int I>			ND_ _Vec<floatE4M3,I>	intBitsToFloate4m3 (const _Vec<sbyte,I>);		// inverse floate4m3BitsToInt
	template <int I>			ND_ _Vec<floatE4M3,I>	uintBitsToFloate4m3 (const _Vec<ubyte,I>);		// inverse floate4m3BitsToUint

# ifdef AE_ENABLE_HALF_TYPE
									void			saturatedConvert (out floatE5M2 result, half value);
									void			saturatedConvert (out floatE4M3 result, half value);
	template <int I>				void			saturatedConvert (out _Vec<floatE5M2,I> &result, const _Vec<half,I> value);
	template <int I>				void			saturatedConvert (out _Vec<floatE4M3,I> &result, const _Vec<half,I> value);
# endif
# ifdef AE_bfloat16
									void			saturatedConvert (out floatE5M2 result, bfloat value);
									void			saturatedConvert (out floatE4M3 result, bfloat value);
	template <int I>				void			saturatedConvert (out _Vec<floatE5M2,I> &result, const _Vec<bfloat,I> value);
	template <int I>				void			saturatedConvert (out _Vec<floatE4M3,I> &result, const _Vec<bfloat,I> value);
# endif
# ifdef AE_ENABLE_DOUBLE_TYPE
									void			saturatedConvert (out floatE5M2 result, double value);
									void			saturatedConvert (out floatE4M3 result, double value);
	template <int I>				void			saturatedConvert (out _Vec<floatE5M2,I> &result, const _Vec<double,I> value);
	template <int I>				void			saturatedConvert (out _Vec<floatE4M3,I> &result, const _Vec<double,I> value);
# endif
									void			saturatedConvert (out floatE5M2 result, float value);
									void			saturatedConvert (out floatE4M3 result, float value);
	template <int I>				void			saturatedConvert (out _Vec<floatE5M2,I> &result, const _Vec<float,I> value);
	template <int I>				void			saturatedConvert (out _Vec<floatE4M3,I> &result, const _Vec<float,I> value);

	//								void			saturatedConvert (out coopmat &result, coopmat value);
#endif // AE_float8_e5m2_e4m3

#ifdef AE_integer_dot_product
	template <typename T, int I> ND_ T				IntDot (_Vec<T,I> a, _Vec<T,I> b);
								 ND_ uint			IntDot4x8 (uint a, uint b);
								 ND_ int			IntDot4x8 (int a, int b);
	template <typename T, int I> ND_ T				IntDotAccSat (_Vec<T,I> a, _Vec<T,I> b, T c);
								 ND_ uint			IntDotAccSat4x8 (uint a, uint b, uint c);
								 ND_ int			IntDotAccSat4x8 (int a, int b, int c);
#endif

# ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic pop
# endif
#endif


struct gl
{
private:
	template <typename T> struct _SubpassBase		{};
	template <typename T> struct _TextureBase		{};
	template <typename T> struct _CombinedTexBase	{};
	template <typename T> struct _ImageBase			{};

	enum class _Uns_ {};

	// used some random constants because unsized arrays are not supported
	static constexpr uint	_MaxClipDistance				= 8;
	static constexpr uint	_MaxCullDistance				= 8;
	static constexpr uint	_MaxSampleMask					= 1;
	static constexpr uint	_MaxMeshVertices				= 1000;
	static constexpr uint	_MaxMeshPrimitives				= 1000;
	static constexpr uint	_MaxPrimitivePointIndices		= 1000;
	static constexpr uint	_MaxPrimitiveLineIndices		= 1000;
	static constexpr uint	_MaxPrimitiveTriangleIndices	= 1000;


public:
	enum class DeviceAddress : uint64_t {};

	template <typename T> struct SubpassInput 			: _SubpassBase<T> { SubpassInput ();		};
	template <typename T> struct SubpassInputMS 		: _SubpassBase<T> { SubpassInputMS ();		};

	template <typename T> struct Texture1D 				: _TextureBase<T> { Texture1D ();			using Size = int;	using Offset = int;		};
	template <typename T> struct Texture2D 				: _TextureBase<T> { Texture2D ();			using Size = int2;	using Offset = int2;	};
	template <typename T> struct Texture3D 				: _TextureBase<T> { Texture3D ();			using Size = int3;	using Offset = int3;	};
	template <typename T> struct TextureCube 			: _TextureBase<T> { TextureCube ();			using Size = int2;	using Offset = _Uns_;	};
	template <typename T> struct Texture1DArray 		: _TextureBase<T> { Texture1DArray ();		using Size = int2;	using Offset = int;		};
	template <typename T> struct Texture2DArray 		: _TextureBase<T> { Texture2DArray ();		using Size = int3;	using Offset = int2;	};
	template <typename T> struct TextureBuffer 			: _TextureBase<T> { TextureBuffer ();		using Size = int;	using Offset = int;  	};
	template <typename T> struct Texture2DMS 			: _TextureBase<T> { Texture2DMS ();			using Size = int2;	using Offset = int2; 	};
	template <typename T> struct Texture2DMSArray 		: _TextureBase<T> { Texture2DMSArray ();	using Size = int3;	using Offset = int3; 	};
	template <typename T> struct TextureCubeArray		: _TextureBase<T> { TextureCubeArray ();	using Size = int3;	using Offset = _Uns_;	};

	struct Sampler {};
	struct SamplerShadow {};

	template <typename T> struct CombinedTex1D 			: _CombinedTexBase<T> { CombinedTex1D ();			CombinedTex1D (Texture1D<T>, Sampler);					using Size = int;	using Offset = int;		using UNorm = float;	using Grad = float;		};
	template <typename T> struct CombinedTex2D 			: _CombinedTexBase<T> { CombinedTex2D ();			CombinedTex2D (Texture2D<T>, Sampler);					using Size = int2;	using Offset = int2;	using UNorm = float2;	using Grad = float2;	};
	template <typename T> struct CombinedTex3D 			: _CombinedTexBase<T> { CombinedTex3D ();			CombinedTex3D (Texture3D<T>, Sampler);					using Size = int3;	using Offset = int3;	using UNorm = float3;	using Grad = float3;	};
	template <typename T> struct CombinedTexCube 		: _CombinedTexBase<T> { CombinedTexCube ();			CombinedTexCube (TextureCube<T>, Sampler);				using Size = int2;	using Offset = _Uns_;	using UNorm = float3;	using Grad = float3;	};
	template <typename T> struct CombinedTex1DArray 	: _CombinedTexBase<T> { CombinedTex1DArray ();		CombinedTex1DArray (Texture1DArray<T>, Sampler);		using Size = int2;	using Offset = int;		using UNorm = float2;	using Grad = float;		};
	template <typename T> struct CombinedTex2DArray 	: _CombinedTexBase<T> { CombinedTex2DArray ();		CombinedTex2DArray (Texture2DArray<T>, Sampler);		using Size = int3;	using Offset = int2;	using UNorm = float3;	using Grad = float2;	};
	template <typename T> struct CombinedTexBuffer 		: _CombinedTexBase<T> { CombinedTexBuffer ();		CombinedTexBuffer (TextureBuffer<T>, Sampler);			using Size = int;	using Offset = int;  	using UNorm = float;	using Grad = _Uns_;		};
	template <typename T> struct CombinedTex2DMS 		: _CombinedTexBase<T> { CombinedTex2DMS ();			CombinedTex2DMS (Texture2DMS<T>, Sampler);				using Size = int2;	using Offset = int2; 	using UNorm = _Uns_;	using Grad = _Uns_;		};
	template <typename T> struct CombinedTex2DMSArray 	: _CombinedTexBase<T> { CombinedTex2DMSArray ();	CombinedTex2DMSArray (Texture2DMSArray<T>, Sampler);	using Size = int3;	using Offset = int3; 	using UNorm = _Uns_;	using Grad = _Uns_;		};
	template <typename T> struct CombinedTexCubeArray 	: _CombinedTexBase<T> { CombinedTexCubeArray ();	CombinedTexCubeArray (TextureCubeArray<T>, Sampler);	using Size = int3;	using Offset = _Uns_;	using UNorm = float4;	using Grad = float3;	};

	template <typename T> struct Image1D 				: _ImageBase<T> { Image1D ();			using Coord = int;  };
	template <typename T> struct Image2D 				: _ImageBase<T> { Image2D ();			using Coord = int2; };
	template <typename T> struct Image3D 				: _ImageBase<T> { Image3D ();			using Coord = int3; };
	template <typename T> struct ImageCube 				: _ImageBase<T> { ImageCube ();			using Coord = int3; };
	template <typename T> struct ImageBuffer 			: _ImageBase<T> { ImageBuffer ();		using Coord = int;  };
	template <typename T> struct Image1DArray 			: _ImageBase<T> { Image1DArray ();		using Coord = int2; };
	template <typename T> struct Image2DArray 			: _ImageBase<T> { Image2DArray ();		using Coord = int3; };
	template <typename T> struct ImageCubeArray 		: _ImageBase<T> { ImageCubeArray ();	using Coord = int3; };
	template <typename T> struct Image2DMS 				: _ImageBase<T> { Image2DMS ();			using Coord = int2; };
	template <typename T> struct Image2DMSArray 		: _ImageBase<T> { Image2DMSArray ();	using Coord = int3; };


	// Use it if array index is not dynamically uniform when accessing resource (image/buffer/...).
	// Dynamically uniform:
	//	- Data from uniform buffer, for arrays it must be constant or dynamically uniform indexing.
	//	- Data from push constants.
	//	- 'DrawID' / 'DrawIndex'.
	//	- 'WorkGroupID' and any value that is the same for the entire workgroup.
	//
	// Non-dynamically uniform:
	//	- 'VertexIndex', 'PrimitiveID', ...
	//	- 'InstanceIndex' - non-uniform on TBDR.

	// GL_EXT_nonuniform_qualifier
  #ifdef AE_nonuniform_qualifier
	ND_ int   Nonuniform (int);
	ND_ uint  Nonuniform (uint);
  #endif

	// GL_EXT_subgroupuniform_qualifier
  #ifdef AE_subgroup_uniform_qualifier
	// It can be applied to:
	//  * variable declarations qualified as 'in'
	//  * global variable declarations with no storage qualifier
	//  * local variable declarations with no storage qualifier
	//  * function parameter declarations and function return types.
	template <typename T>
	ND_ T	SubgroupUniform (const T &);
  #endif


  #ifdef AE_memory_scope_semantics
	enum class Scope
	{
		Device,				// gl_ScopeDevice
		Workgroup,			// gl_ScopeWorkgroup
		Subgroup,			// gl_ScopeSubgroup
		Invocation,			// gl_ScopeInvocation
		QueueFamily,		// gl_ScopeQueueFamily
	};

	enum class Semantics
	{
		Relaxed,			// gl_SemanticsRelaxed
		Acquire,			// gl_SemanticsAcquire
		Release,			// gl_SemanticsRelease
		AcquireRelease,		// gl_SemanticsAcquireRelease
		MakeAvailable,		// gl_SemanticsMakeAvailable
		MakeVisible,		// gl_SemanticsMakeVisible
		Volatile,			// gl_SemanticsVolatile
	};

	enum class StorageSemantics
	{
		None,				// gl_StorageSemanticsNone
		Buffer,				// gl_StorageSemanticsBuffer
		Shared,				// gl_StorageSemanticsShared
		Image,				// gl_StorageSemanticsImage
		Output,				// gl_StorageSemanticsOutput
	};
  #endif

  #ifdef AE_HAS_ATOMICS
	uint   AtomicAdd (INOUT uint  &mem, uint  data);
	sint   AtomicAdd (INOUT sint  &mem, sint  data);
	ulong  AtomicAdd (INOUT ulong &mem, ulong data);
	slong  AtomicAdd (INOUT slong &mem, slong data);

	#ifdef AE_memory_scope_semantics
	uint   AtomicAdd (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
	sint   AtomicAdd (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
	ulong  AtomicAdd (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
	slong  AtomicAdd (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
	#endif

	float  AtomicAdd (INOUT float  &mem, float  data);
	double AtomicAdd (INOUT double &mem, double data);

	#ifdef AE_memory_scope_semantics
	float  AtomicAdd (INOUT float  &mem, float  data, Scope scope, StorageSemantics storage, Semantics sem);
	double AtomicAdd (INOUT double &mem, double data, Scope scope, StorageSemantics storage, Semantics sem);
	#endif

	uint   AtomicMin (INOUT uint  &mem, uint  data);
	sint   AtomicMin (INOUT sint  &mem, sint  data);
	ulong  AtomicMin (INOUT ulong &mem, ulong data);
	slong  AtomicMin (INOUT slong &mem, slong data);

	#ifdef AE_memory_scope_semantics
	uint   AtomicMin (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
	sint   AtomicMin (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
	ulong  AtomicMin (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
	slong  AtomicMin (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
	#endif

	uint   AtomicMax (INOUT uint  &mem, uint  data);
	sint   AtomicMax (INOUT sint  &mem, sint  data);
	ulong  AtomicMax (INOUT ulong &mem, ulong data);
	slong  AtomicMax (INOUT slong &mem, slong data);

	#ifdef AE_memory_scope_semantics
	uint   AtomicMax (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
	sint   AtomicMax (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
	ulong  AtomicMax (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
	slong  AtomicMax (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
	#endif

	uint   AtomicAnd (INOUT uint  &mem, uint  data);
	sint   AtomicAnd (INOUT sint  &mem, sint  data);
	ulong  AtomicAnd (INOUT ulong &mem, ulong data);
	slong  AtomicAnd (INOUT slong &mem, slong data);

	#ifdef AE_memory_scope_semantics
	uint   AtomicAnd (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
	sint   AtomicAnd (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
	ulong  AtomicAnd (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
	slong  AtomicAnd (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
	#endif

	uint   AtomicOr (INOUT uint  &mem, uint  data);
	sint   AtomicOr (INOUT sint  &mem, sint  data);
	ulong  AtomicOr (INOUT ulong &mem, ulong data);
	slong  AtomicOr (INOUT slong &mem, slong data);

	#ifdef AE_memory_scope_semantics
	uint   AtomicOr (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
	sint   AtomicOr (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
	ulong  AtomicOr (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
	slong  AtomicOr (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
	#endif

	uint   AtomicXor (INOUT uint  &mem, uint  data);
	sint   AtomicXor (INOUT sint  &mem, sint  data);
	ulong  AtomicXor (INOUT ulong &mem, ulong data);
	slong  AtomicXor (INOUT slong &mem, slong data);

	#ifdef AE_memory_scope_semantics
	uint   AtomicXor (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
	sint   AtomicXor (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
	ulong  AtomicXor (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
	slong  AtomicXor (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
	#endif

	uint   AtomicExchange (INOUT uint  &mem, uint  data);
	sint   AtomicExchange (INOUT sint  &mem, sint  data);
	ulong  AtomicExchange (INOUT ulong &mem, ulong data);
	slong  AtomicExchange (INOUT slong &mem, slong data);

	#ifdef AE_memory_scope_semantics
	uint   AtomicExchange (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
	sint   AtomicExchange (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
	ulong  AtomicExchange (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
	slong  AtomicExchange (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
	#endif

	float  AtomicExchange (INOUT float  &mem, float  data);
	double AtomicExchange (INOUT double &mem, double data);

	#ifdef AE_memory_scope_semantics
	float  AtomicExchange (INOUT float  &mem, float  data, Scope scope, StorageSemantics storage, Semantics sem);
	double AtomicExchange (INOUT double &mem, double data, Scope scope, StorageSemantics storage, Semantics sem);
	#endif

	uint   AtomicCompSwap (INOUT uint  &mem, uint  compare, uint  data);
	sint   AtomicCompSwap (INOUT sint  &mem, sint  compare, sint  data);
	ulong  AtomicCompSwap (INOUT ulong &mem, ulong compare, ulong data);
	slong  AtomicCompSwap (INOUT slong &mem, slong compare, slong data);

	#ifdef AE_memory_scope_semantics
	uint   AtomicCompSwap (INOUT uint  &mem, uint  compare, uint  data, Scope scope, StorageSemantics storageEqual, Semantics semEqual, StorageSemantics storageUnequal, Semantics semUnequal);
	sint   AtomicCompSwap (INOUT sint  &mem, sint  compare, sint  data, Scope scope, StorageSemantics storageEqual, Semantics semEqual, StorageSemantics storageUnequal, Semantics semUnequal);
	ulong  AtomicCompSwap (INOUT ulong &mem, ulong compare, ulong data, Scope scope, StorageSemantics storageEqual, Semantics semEqual, StorageSemantics storageUnequal, Semantics semUnequal);
	slong  AtomicCompSwap (INOUT slong &mem, slong compare, slong data, Scope scope, StorageSemantics storageEqual, Semantics semEqual, StorageSemantics storageUnequal, Semantics semUnequal);
	#endif

	#ifdef AE_memory_scope_semantics
	ND_ uint   AtomicLoad (uint  &mem, Scope scope, StorageSemantics storage, Semantics sem);
	ND_ sint   AtomicLoad (sint  &mem, Scope scope, StorageSemantics storage, Semantics sem);
	ND_ ulong  AtomicLoad (ulong &mem, Scope scope, StorageSemantics storage, Semantics sem);
	ND_ slong  AtomicLoad (slong &mem, Scope scope, StorageSemantics storage, Semantics sem);

	ND_ float  AtomicLoad (float  &mem, Scope scope, StorageSemantics storage, Semantics sem);
	ND_ double AtomicLoad (double &mem, Scope scope, StorageSemantics storage, Semantics sem);
	#endif

	#ifdef AE_memory_scope_semantics
	void  AtomicStore (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
	void  AtomicStore (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
	void  AtomicStore (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
	void  AtomicStore (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);

	void  AtomicStore (INOUT float  &mem, float  data, Scope scope, StorageSemantics storage, Semantics sem);
	void  AtomicStore (INOUT double &mem, double data, Scope scope, StorageSemantics storage, Semantics sem);
	#endif
  #endif // AE_HAS_ATOMICS


	const struct {
		#define _GEN_IMAGE_ATOMIC( _type_, ... ) \
			_type_   AtomicAdd (__VA_ARGS__, _type_ data) const; \
			_type_   AtomicMin (__VA_ARGS__, _type_ data) const; \
			_type_   AtomicMax (__VA_ARGS__, _type_ data) const; \
			_type_   AtomicAnd (__VA_ARGS__, _type_ data) const; \
			_type_   AtomicOr (__VA_ARGS__, _type_ data) const; \
			_type_   AtomicXor (__VA_ARGS__, _type_ data) const; \
			_type_   AtomicExchange (__VA_ARGS__, _type_ data) const; \
			_type_   AtomicCompSwap (__VA_ARGS__, _type_ compare, _type_ data) const; \

		#define _GEN_IMAGE_ATOMIC_SCOPE( _type_, ... ) \
			_type_   AtomicAdd (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
			_type_   AtomicMin (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
			_type_   AtomicMax (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
			_type_   AtomicAnd (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
			_type_   AtomicOr (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
			_type_   AtomicXor (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
			_type_   AtomicExchange (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
			_type_   AtomicCompSwap (__VA_ARGS__, _type_ compare, _type_ data, Scope scope, StorageSemantics storageEqual, Semantics semEqual, StorageSemantics storageUnequal, Semantics semUnequal) const; \
			\
			ND_ _type_  AtomicLoad (__VA_ARGS__, Scope scope, StorageSemantics storage, Semantics sem) const; \
			void  AtomicStore (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \

		// GL_EXT_shader_atomic_float
		#define _GEN_IMAGE_ATOMIC_F1( _type_, ... ) \
			_type_   AtomicAdd (__VA_ARGS__, _type_ data) const; \
			_type_   AtomicExchange (__VA_ARGS__, _type_ data) const; \

		#define _GEN_IMAGE_ATOMIC_SCOPE_F1( _type_, ... ) \
			_type_   AtomicAdd (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
			_type_   AtomicExchange (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
			ND_ _type_  AtomicLoad (__VA_ARGS__, Scope scope, StorageSemantics storage, Semantics sem) const; \
			void  AtomicStore (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \

		// GL_EXT_shader_atomic_float2
		#define _GEN_IMAGE_ATOMIC_F2( _type_, ... ) \
			_type_   AtomicMin (__VA_ARGS__, _type_ data) const; \
			_type_   AtomicMax (__VA_ARGS__, _type_ data) const; \

		#define _GEN_IMAGE_ATOMIC_SCOPE_F2( _type_, ... ) \
			_type_   AtomicMin (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
			_type_   AtomicMax (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \

		#define _GEN_IMAGE( _type_, ... ) \
			ND_ _type_  Load (__VA_ARGS__) const; \
			\
			void  Store (__VA_ARGS__, _type_ data) const; \

		#define GEN_IMAGE_ATOMIC( _visitor_, _type_ ) \
			_visitor_( _type_, Image1D<_type_>        img, typename Image1D<_type_>::Coord        p )\
			_visitor_( _type_, Image2D<_type_>        img, typename Image2D<_type_>::Coord        p )\
			_visitor_( _type_, Image3D<_type_>        img, typename Image3D<_type_>::Coord        p )\
			_visitor_( _type_, ImageCube<_type_>      img, typename ImageCube<_type_>::Coord      p )\
			_visitor_( _type_, ImageBuffer<_type_>    img, typename ImageBuffer<_type_>::Coord    p )\
			_visitor_( _type_, Image1DArray<_type_>   img, typename Image1DArray<_type_>::Coord   p )\
			_visitor_( _type_, Image2DArray<_type_>   img, typename Image2DArray<_type_>::Coord   p )\
			_visitor_( _type_, ImageCubeArray<_type_> img, typename ImageCubeArray<_type_>::Coord p )\
			_visitor_( _type_, Image2DMS<_type_>      img, typename Image2DMS<_type_>::Coord      p, int sample )\
			_visitor_( _type_, Image2DMSArray<_type_> img, typename Image2DMSArray<_type_>::Coord p, int sample )\

		#define GEN_IMAGE( _type4_, _type_ ) \
			_GEN_IMAGE( _type4_, Image1D<_type_>        img, typename Image1D<_type_>::Coord        p )\
			_GEN_IMAGE( _type4_, Image2D<_type_>        img, typename Image2D<_type_>::Coord        p )\
			_GEN_IMAGE( _type4_, Image3D<_type_>        img, typename Image3D<_type_>::Coord        p )\
			_GEN_IMAGE( _type4_, ImageCube<_type_>      img, typename ImageCube<_type_>::Coord      p )\
			_GEN_IMAGE( _type4_, ImageBuffer<_type_>    img, typename ImageBuffer<_type_>::Coord    p )\
			_GEN_IMAGE( _type4_, Image1DArray<_type_>   img, typename Image1DArray<_type_>::Coord   p )\
			_GEN_IMAGE( _type4_, Image2DArray<_type_>   img, typename Image2DArray<_type_>::Coord   p )\
			_GEN_IMAGE( _type4_, ImageCubeArray<_type_> img, typename ImageCubeArray<_type_>::Coord p )\
			_GEN_IMAGE( _type4_, Image2DMS<_type_>      img, typename Image2DMS<_type_>::Coord      p, int sample )\
			_GEN_IMAGE( _type4_, Image2DMSArray<_type_> img, typename Image2DMSArray<_type_>::Coord p, int sample )\

	  #ifdef AE_HAS_ATOMICS
		GEN_IMAGE_ATOMIC( _GEN_IMAGE_ATOMIC,  uint )
		GEN_IMAGE_ATOMIC( _GEN_IMAGE_ATOMIC,  sint )
	  #endif
	  #ifdef AE_memory_scope_semantics
		GEN_IMAGE_ATOMIC( _GEN_IMAGE_ATOMIC_SCOPE,  uint )
		GEN_IMAGE_ATOMIC( _GEN_IMAGE_ATOMIC_SCOPE,  sint )
	  #endif
	  #ifdef AE_shader_atomic_float
		GEN_IMAGE_ATOMIC( _GEN_IMAGE_ATOMIC_F1,			float )		// GL_EXT_shader_atomic_float
	  #endif
	  #if defined(AE_shader_atomic_float) and defined(AE_memory_scope_semantics)
		GEN_IMAGE_ATOMIC( _GEN_IMAGE_ATOMIC_SCOPE_F1,	float )
	  #endif
	  #ifdef AE_shader_atomic_float2
		GEN_IMAGE_ATOMIC( _GEN_IMAGE_ATOMIC_F2,			float )		// GL_EXT_shader_atomic_float2
	  #endif
	  #if defined(AE_shader_atomic_float2) and defined(AE_memory_scope_semantics)
		GEN_IMAGE_ATOMIC( _GEN_IMAGE_ATOMIC_SCOPE_F2,	float )
	  #endif
		GEN_IMAGE( uint4,  uint )
		GEN_IMAGE( int4,   sint )
		GEN_IMAGE( float4, float )

		#undef _GEN_IMAGE_ATOMIC
		#undef _GEN_IMAGE_ATOMIC_F1
		#undef _GEN_IMAGE_ATOMIC_F2
		#undef _GEN_IMAGE_ATOMIC_SCOPE
		#undef _GEN_IMAGE_ATOMIC_SCOPE_F1
		#undef _GEN_IMAGE_ATOMIC_SCOPE_F2
		#undef _GEN_IMAGE
		#undef GEN_IMAGE_ATOMIC
		#undef GEN_IMAGE

		template <typename T>	ND_ int  GetSamples (Image2DMS<T>      img) const;
		template <typename T>	ND_ int  GetSamples (Image2DMSArray<T> img) const;

		template <typename T>	ND_ typename Image1D<T>::Coord			GetSize (Image1D<T>        img) const;
		template <typename T>	ND_ typename Image2D<T>::Coord			GetSize (Image2D<T>        img) const;
		template <typename T>	ND_ typename Image3D<T>::Coord			GetSize (Image3D<T>        img) const;
		template <typename T>	ND_ typename ImageCube<T>::Coord		GetSize (ImageCube<T>      img) const;
		template <typename T>	ND_ typename ImageBuffer<T>::Coord		GetSize (ImageBuffer<T>    img) const;
		template <typename T>	ND_ typename Image1DArray<T>::Coord		GetSize (Image1DArray<T>   img) const;
		template <typename T>	ND_ typename Image2DArray<T>::Coord		GetSize (Image2DArray<T>   img) const;
		template <typename T>	ND_ typename ImageCubeArray<T>::Coord	GetSize (ImageCubeArray<T> img) const;
		template <typename T>	ND_ typename Image2DMS<T>::Coord		GetSize (Image2DMS<T>      img) const;
		template <typename T>	ND_ typename Image2DMSArray<T>::Coord	GetSize (Image2DMSArray<T> img) const;

	} image {};

	const struct {
		// 1D, 2D, 3D 1DArr, 2DArr, Cube, CubeArr
		#define _GEN_TEXTURE1( _type_, _texType_ ) \
			ND_ _type_  Fetch (_texType_ tex, typename _texType_::Size p, int lod) const; \
			\
			ND_ typename _texType_::Size	GetSize (_texType_ tex, int lod) const; \
			ND_ int							QueryLevels (_texType_ tex) const; \

		// 1D, 2D, 3D 1DArr, 2DArr
		#define _GEN_TEXTURE2( _type_, _texType_ ) \
			ND_ _type_  FetchOffset (_texType_ tex, typename _texType_::Size p, int lod, typename _texType_::Offset offset) const; \

		// 1D, 2D, 3D 1DArr, 2DArr, Cube, CubeArr
		#define _GEN_TEXTURE3( _type_, _texType_ ) \
			ND_ _type_  Sample (_texType_ tex, typename _texType_::UNorm p) const; \
			ND_ _type_  Sample (_texType_ tex, typename _texType_::UNorm p, float bias) const; \
			\
			ND_ _type_  SampleLod (_texType_ tex, typename _texType_::UNorm p, float lod) const; \
			\
			ND_ _type_  SampleGrad (_texType_ tex, typename _texType_::UNorm p, typename _texType_::Grad dPdx, typename _texType_::Grad dPdy) const; \

		// 1D, 2D, 3D 1DArr, 2DArr
		#define _GEN_TEXTURE4( _type_, _texType_ ) \
			ND_ _type_  SampleOffset (_texType_ tex, typename _texType_::UNorm p, typename _texType_::Offset offset) const; \
			ND_ _type_  SampleOffset (_texType_ tex, typename _texType_::UNorm p, typename _texType_::Offset offset, float bias) const; \
			\
			ND_ _type_  SampleLodOffset (_texType_ tex, typename _texType_::UNorm p, float lod, typename _texType_::Offset offset) const; \
			\
			ND_ _type_  SampleGradOffset (_texType_ tex, typename _texType_::UNorm p, typename _texType_::Grad dPdx, typename _texType_::Grad dPdy, typename _texType_::Offset offset) const; \

		// 2D, 2DArr, Cube, CubeArr
		#define _GEN_TEXTURE5( _type_, _texType_ ) \
			ND_ _type_  Gather (_texType_ tex, typename _texType_::UNorm p) const; \
			ND_ _type_  Gather (_texType_ tex, typename _texType_::UNorm p, int comp) const; \

		// 2D, 2DArr
		#define _GEN_TEXTURE6( _type_, _texType_ ) \
			ND_ _type_  GatherOffset (_texType_ tex, typename _texType_::UNorm p, int2 offset) const; \
			ND_ _type_  GatherOffset (_texType_ tex, typename _texType_::UNorm p, int2 offset, int comp) const; \
			\
			ND_ _type_  GatherOffsets (_texType_ tex, typename _texType_::UNorm p, const int2 (&offsets)[4]) const; \
			ND_ _type_  GatherOffsets (_texType_ tex, typename _texType_::UNorm p, const int2 (&offsets)[4], int comp) const; \

		// 1D, 2D, 3D
		#define _GEN_TEXTURE7( _type_, _texType_ ) \
			ND_ float2  QueryLod (_texType_ tex, typename _texType_::UNorm p) const; \
			\
			ND_ _type_  SampleProj (_texType_ tex, float4 p) const; \
			ND_ _type_  SampleProj (_texType_ tex, float4 p, float bias) const; \
			ND_ _type_  SampleProjGrad (_texType_ tex, float4 p, typename _texType_::Grad dPdx, typename _texType_::Grad dPdy) const; \
			ND_ _type_  SampleProjGradOffset (_texType_ tex, float4 p, typename _texType_::Grad dPdx, typename _texType_::Grad dPdy, typename _texType_::Offset offset) const; \
			ND_ _type_  SampleProjLod (_texType_ tex, float4 p, float lod) const; \
			ND_ _type_  SampleProjLodOffset (_texType_ tex, float4 p, float lod, typename _texType_::Offset offset) const; \
			ND_ _type_  SampleProjOffset (_texType_ tex, float4 p, typename _texType_::Offset offset) const; \
			ND_ _type_  SampleProjOffset (_texType_ tex, float4 p, typename _texType_::Offset offset, float bias) const; \

		#define GEN_TEXTURE( _type4_, _type_ ) \
			_GEN_TEXTURE1( _type4_,	CombinedTex1D<_type_>        )\
			_GEN_TEXTURE1( _type4_,	CombinedTex2D<_type_>        )\
			_GEN_TEXTURE1( _type4_,	CombinedTex3D<_type_>        )\
			_GEN_TEXTURE1( _type4_,	CombinedTexCube<_type_>      )\
			_GEN_TEXTURE1( _type4_,	CombinedTex1DArray<_type_>   )\
			_GEN_TEXTURE1( _type4_,	CombinedTex2DArray<_type_>   )\
			_GEN_TEXTURE1( _type4_,	CombinedTexCubeArray<_type_> )\
			\
			_GEN_TEXTURE2( _type4_,	CombinedTex1D<_type_>        )\
			_GEN_TEXTURE2( _type4_,	CombinedTex2D<_type_>        )\
			_GEN_TEXTURE2( _type4_,	CombinedTex3D<_type_>        )\
			_GEN_TEXTURE2( _type4_,	CombinedTex1DArray<_type_>   )\
			_GEN_TEXTURE2( _type4_,	CombinedTex2DArray<_type_>   )\
			\
			_GEN_TEXTURE3( _type4_,	CombinedTex1D<_type_>        )\
			_GEN_TEXTURE3( _type4_,	CombinedTex2D<_type_>        )\
			_GEN_TEXTURE3( _type4_,	CombinedTex3D<_type_>        )\
			_GEN_TEXTURE3( _type4_,	CombinedTexCube<_type_>      )\
			_GEN_TEXTURE3( _type4_,	CombinedTex1DArray<_type_>   )\
			_GEN_TEXTURE3( _type4_,	CombinedTex2DArray<_type_>   )\
			_GEN_TEXTURE3( _type4_,	CombinedTexCubeArray<_type_> )\
			\
			_GEN_TEXTURE4( _type4_,	CombinedTex1D<_type_>        )\
			_GEN_TEXTURE4( _type4_,	CombinedTex2D<_type_>        )\
			_GEN_TEXTURE4( _type4_,	CombinedTex3D<_type_>        )\
			_GEN_TEXTURE4( _type4_,	CombinedTex1DArray<_type_>   )\
			_GEN_TEXTURE4( _type4_,	CombinedTex2DArray<_type_>   )\
			\
			_GEN_TEXTURE5( _type4_,	CombinedTex2D<_type_>        )\
			_GEN_TEXTURE5( _type4_,	CombinedTex2DArray<_type_>   )\
			_GEN_TEXTURE5( _type4_,	CombinedTexCube<_type_>      )\
			_GEN_TEXTURE5( _type4_,	CombinedTexCubeArray<_type_> )\
			\
			_GEN_TEXTURE6( _type4_,	CombinedTex2D<_type_>        )\
			_GEN_TEXTURE6( _type4_,	CombinedTex2DArray<_type_>   )\
			\
			_GEN_TEXTURE7( _type4_,	CombinedTex1D<_type_>        )\
			_GEN_TEXTURE7( _type4_,	CombinedTex2D<_type_>        )\
			_GEN_TEXTURE7( _type4_,	CombinedTex3D<_type_>        )\
			\
			_GEN_TEXTURE1( _type4_,	Texture1D<_type_>        )\
			_GEN_TEXTURE1( _type4_,	Texture2D<_type_>        )\
			_GEN_TEXTURE1( _type4_,	Texture3D<_type_>        )\
			_GEN_TEXTURE1( _type4_,	TextureCube<_type_>      )\
			_GEN_TEXTURE1( _type4_,	Texture1DArray<_type_>   )\
			_GEN_TEXTURE1( _type4_,	Texture2DArray<_type_>   )\
			_GEN_TEXTURE1( _type4_,	TextureCubeArray<_type_> )\
			\
			_GEN_TEXTURE2( _type4_,	Texture1D<_type_>        )\
			_GEN_TEXTURE2( _type4_,	Texture2D<_type_>        )\
			_GEN_TEXTURE2( _type4_,	Texture3D<_type_>        )\
			_GEN_TEXTURE2( _type4_,	Texture1DArray<_type_>   )\
			_GEN_TEXTURE2( _type4_,	Texture2DArray<_type_>   )\

		GEN_TEXTURE( uint4,  uint );
		GEN_TEXTURE( int4,   sint );
		GEN_TEXTURE( float4, float );

		#undef GEN_TEXTURE
		#undef _GEN_TEXTURE7
		#undef _GEN_TEXTURE6
		#undef _GEN_TEXTURE5
		#undef _GEN_TEXTURE4
		#undef _GEN_TEXTURE3
		#undef _GEN_TEXTURE2
		#undef _GEN_TEXTURE1

		template <typename T>	ND_ int  GetSamples (CombinedTex2DMS<T>      tex) const;
		template <typename T>	ND_ int  GetSamples (CombinedTex2DMSArray<T> tex) const;

		template <typename T>	ND_ int  GetSamples (Texture2DMS<T>      tex) const;
		template <typename T>	ND_ int  GetSamples (Texture2DMSArray<T> tex) const;

		template <typename T>	ND_ typename CombinedTexBuffer<T>::Size		GetSize (CombinedTexBuffer<T>    tex) const;
		template <typename T>	ND_ typename CombinedTex2DMS<T>::Size		GetSize (CombinedTex2DMS<T>      tex) const;
		template <typename T>	ND_ typename CombinedTex2DMSArray<T>::Size	GetSize (CombinedTex2DMSArray<T> tex) const;

		template <typename T>	ND_ typename TextureBuffer<T>::Size		GetSize (TextureBuffer<T>    tex) const;
		template <typename T>	ND_ typename Texture2DMS<T>::Size		GetSize (Texture2DMS<T>      tex) const;
		template <typename T>	ND_ typename Texture2DMSArray<T>::Size	GetSize (Texture2DMSArray<T> tex) const;

		template <typename T>	ND_ _Vec<T,4>  Fetch (CombinedTexBuffer<T>    tex, typename CombinedTexBuffer<T>::Offset    p) const;
		template <typename T>	ND_ _Vec<T,4>  Fetch (CombinedTex2DMS<T>      tex, typename CombinedTex2DMS<T>::Offset      p, int samples) const;
		template <typename T>	ND_ _Vec<T,4>  Fetch (CombinedTex2DMSArray<T> tex, typename CombinedTex2DMSArray<T>::Offset p, int samples) const;

		template <typename T>	ND_ _Vec<T,4>  Fetch (TextureBuffer<T>    tex, typename TextureBuffer<T>::Offset    p) const;
		template <typename T>	ND_ _Vec<T,4>  Fetch (Texture2DMS<T>      tex, typename Texture2DMS<T>::Offset      p, int samples) const;
		template <typename T>	ND_ _Vec<T,4>  Fetch (Texture2DMSArray<T> tex, typename Texture2DMSArray<T>::Offset p, int samples) const;

		template <typename T>	ND_ float2  QueryLod (TextureCube<T>      tex, float3 p) const;
		template <typename T>	ND_ float2  QueryLod (TextureCubeArray<T> tex, float3 p) const;
		template <typename T>	ND_ float2  QueryLod (Texture1DArray<T>   tex, float  p) const;
		template <typename T>	ND_ float2  QueryLod (Texture2DArray<T>   tex, float2 p) const;

		template <typename T>	ND_ float2  QueryLod (CombinedTexCube<T>      tex, float3 p) const;
		template <typename T>	ND_ float2  QueryLod (CombinedTexCubeArray<T> tex, float3 p) const;
		template <typename T>	ND_ float2  QueryLod (CombinedTex1DArray<T>   tex, float  p) const;
		template <typename T>	ND_ float2  QueryLod (CombinedTex2DArray<T>   tex, float2 p) const;

	} texture {};

	const struct {
		template <typename T>	_Vec<T,4>  Load (SubpassInput<T>   sp) const;
		template <typename T>	_Vec<T,4>  Load (SubpassInputMS<T> sp, int sample) const;
	} subpass {};

	const struct {
	  #ifdef AE_shader_subgroup_basic
		const struct {
			void  All ()		const;	// all memory accesses, scope: shader invocation
			void  Buffer ()		const;
			void  Image ()		const;

		  #if defined(SH_COMPUTE) or defined(SH_MESH_TASK) or defined(SH_MESH)
			void  Shared ()		const;	// for 'WGShared' variables
		  #endif
		} memoryBarrier {};

			void	ExecutionBarrier () const;
		ND_ bool	Elect () const;											// exactly one invocation will return true

		// in
		const	uint	Size;
		const	uint	Index;			// in FS QuadIndex = Index & 3
	  # ifdef SH_COMPUTE
		const	uint	GroupCount;		// in workgroup
		const	uint	GroupIndex;		// in workgroup
	  # endif
	  #endif

	  #ifdef AE_shader_subgroup_vote
		ND_ bool	All (bool) const;										// returns true if all active invocation has 'value == true'
		ND_ bool	Any (bool) const;										// returns true if any active invocation have 'value == true'

		template <typename T>	ND_ bool	AllEqual (const T) const;		// returns true if all active invocation have a 'value' that is equal
	  #endif

	  #ifdef AE_shader_subgroup_ballot
		template <typename T>	ND_ T		Broadcast (const T value, uint id) const;	// returns the 'value' from the invocation whose ID is equal to 'id',
																						// 'AE_subgroupBroadcastDynamicId' allows dynamically uniform 'id', but it is slow
		template <typename T>	ND_ T		BroadcastFirst (const T) const;	// returns the 'value' from the active invocation with the lowest ID

		ND_ uint4	Ballot (bool) const;									// returns a set of bitfields containing the result of evaluating the expression 'value' in all active invocations in the subgroup
		ND_ bool	InverseBallot (const uint4) const;
		ND_ bool	BallotBitExtract (const uint4 value, uint index) const;
		ND_ uint	BallotBitCount (const uint4) const;
		ND_ uint	BallotInclusiveBitCount (const uint4) const;			// see Inclusive<op>
		ND_ uint	BallotExclusiveBitCount (const uint4) const;			// see Exclusive<op>
		ND_ uint	BallotFindLSB (const uint4) const;
		ND_ uint	BallotFindMSB (const uint4) const;

		// in
		const	uint4	EqMask;
		const	uint4	GeMask;
		const	uint4	GtMask;
		const	uint4	LeMask;
		const	uint4	LtMask;
	  #endif

	  #ifdef AE_shader_subgroup_shuffle
		template <typename T>	ND_ T		Shuffle (const T value, uint id) const;
		template <typename T>	ND_ T		ShuffleXor (const T value, uint mask) const;
	  #endif
	  #ifdef AE_shader_subgroup_shuffle_relative
		template <typename T>	ND_ T		ShuffleUp (const T value, uint delta) const;
		template <typename T>	ND_ T		ShuffleDown (const T value, uint delta) const;
	  #endif

	  #ifdef AE_shader_subgroup_arithmetic
		template <typename T>	ND_ T		Add (const T) const;				//	same as:
		template <typename T>	ND_ T		Mul (const T) const;				//		input[ subgroup.Index ] = x
		template <typename T>	ND_ T		Min (const T) const;				//		for (accum = 0, i = 0; i < subgroup.Size; ++i)
		template <typename T>	ND_ T		Max (const T) const;				//			accum <op>= input[i]
		template <typename T>	ND_ T		And (const T) const;				//		return accum
		template <typename T>	ND_ T		Or  (const T) const;
		template <typename T>	ND_ T		Xor (const T) const;

		template <typename T>	ND_ T		InclusiveAdd (const T) const;		//	same as:
		template <typename T>	ND_ T		InclusiveMul (const T) const;		//		input[ subgroup.Index ] = x
		template <typename T>	ND_ T		InclusiveMin (const T) const;		//		for (accum = 0, i = 0; i < subgroup.Size; ++i)
		template <typename T>	ND_ T		InclusiveAnd (const T) const;		//			accum <op>= input[i]
		template <typename T>	ND_ T		InclusiveMax (const T) const;		//			result[i] = accum
		template <typename T>	ND_ T		InclusiveOr  (const T) const;		//		return result[ subgroup.Index ]
		template <typename T>	ND_ T		InclusiveXor (const T) const;

		template <typename T>	ND_ T		ExclusiveAdd (const T) const;		//	same as:
		template <typename T>	ND_ T		ExclusiveMul (const T) const;		//		input[ subgroup.Index ] = x
		template <typename T>	ND_ T		ExclusiveMin (const T) const;		//		for (accum = 0, i = 0; i < subgroup.Size; ++i)
		template <typename T>	ND_ T		ExclusiveMax (const T) const;		//			result[i] = accum
		template <typename T>	ND_ T		ExclusiveAnd (const T) const;		//			accum <op>= input[i]
		template <typename T>	ND_ T		ExclusiveOr  (const T) const;		//		return result[ subgroup.Index ]
		template <typename T>	ND_ T		ExclusiveXor (const T) const;
	  #endif

	  #ifdef AE_shader_subgroup_clustered
		template <typename T>	ND_ T		ClusteredAdd (const T value, uint clasterSize) const;
		template <typename T>	ND_ T		ClusteredMul (const T value, uint clasterSize) const;
		template <typename T>	ND_ T		ClusteredMin (const T value, uint clasterSize) const;
		template <typename T>	ND_ T		ClusteredMax (const T value, uint clasterSize) const;
		template <typename T>	ND_ T		ClusteredAnd (const T value, uint clasterSize) const;
		template <typename T>	ND_ T		ClusteredOr  (const T value, uint clasterSize) const;
		template <typename T>	ND_ T		ClusteredXor (const T value, uint clasterSize) const;
	  #endif

	} subgroup {};

	struct {

	  #ifdef AE_shader_subgroup_quad
		// quad ids:	bits:
		//   0  1		00  01
		//   2  3		10  11
		template <typename T>	ND_ T		Broadcast (const T value, uint id) const;	// 'AE_subgroupBroadcastDynamicId' allows dynamically uniform 'id'
																						// otherwise it must be constant
		template <typename T>	ND_ T		SwapHorizontal (const T) const;
		template <typename T>	ND_ T		SwapVertical (const T) const;
		template <typename T>	ND_ T		SwapDiagonal (const T) const;
	  #endif

	  #ifdef AE_shader_quad
		ND_ bool  All (bool) const;
		ND_ bool  Any (bool) const;
	  #endif

	} quadGroup {};


  #ifdef SH_VERT
	// in
	const	int		InstanceIndex		= {};	// BaseInstance + InstanceID
	const	int		VertexIndex			= {};	// BaseVertex + VertexID

	#ifdef AE_shader_draw_parameters
	const	int		BaseInstance		= {};
	const	int		BaseVertex			= {};
	const	int		DrawIndex			= {};
	#endif

	// out
			float4	Position;
	#ifdef AE_large_points
			float	PointSize;
	#endif
	#ifdef AE_clip_distance
			float  	ClipDistance [_MaxClipDistance];
	#endif
	#ifdef AE_cull_distance
			float  	CullDistance [_MaxCullDistance];
	#endif
	#ifdef AE_shader_viewport_layer_array
			int		Layer;
			int		ViewportIndex;
	#endif
	#ifdef AE_multiview
			int		ViewIndex;
	#endif
  #endif


  #ifdef SH_TESS_CTRL
	// sync
	void  PatchBarrier ();

	// in
	const	int		InvocationID		= {};
	const	int		PatchVerticesIn		= {};

	// out
			float	TessLevelInner [2];
			float	TessLevelOuter [4];
  #endif


  #ifdef SH_TESS_EVAL
	// in
	const	int		PrimitiveID			= {};
	const	int		PatchVerticesIn		= {};
	const	float3	TessCoord			= {};
	const	float	TessLevelInner [2]	= {};
	const	float	TessLevelOuter [4]	= {};

	// out
			float4	Position;
	#ifdef AE_large_points
			float	PointSize;
	#endif
	#ifdef AE_clip_distance
			float  	ClipDistance [_MaxClipDistance];
	#endif
	#ifdef AE_cull_distance
			float  	CullDistance [_MaxCullDistance];
	#endif
			int		Layer;

  #endif


  #ifdef SH_GEOM
	void  EmitStreamVertex (int stream);
	void  EmitVertex ();
	void  EndPrimitive ();
	void  EndStreamPrimitive (int stream);

	// in
	const	int		PrimitiveIDIn		= {};
	const	int		InvocationID		= {};

	// out
			int		PrimitiveID;
			int		ViewportIndex;
			float4	Position;
	#ifdef AE_large_points
			float	PointSize;
	#endif
	#ifdef AE_clip_distance
			float  	ClipDistance [_MaxClipDistance];
	#endif
	#ifdef AE_cull_distance
			float  	CullDistance [_MaxCullDistance];
	#endif
			int		Layer;

  #endif


	// GL_EXT_fragment_shading_rate
  #if defined(AE_fragment_shading_rate) and (defined(SH_VERT) or defined(SH_GEOM) or defined(SH_FRAG) or defined(SH_MESH))
	enum class ShadingRateFlag : uint
	{
		Y1	= 0,
		Y2	= 1,
		Y4	= 2,

		X1	= 0,
		X2	= 4,
		X4	= 8,
	};
  # ifdef SH_FRAG
	// in
	const	ShadingRateFlag		ShadingRate		= ShadingRateFlag(0);
  # endif
  # if defined(SH_VERT) or defined(SH_GEOM)
	// out
			ShadingRateFlag		PrimitiveShadingRate;
  # endif
  #endif

	// GL_ARB_shader_draw_parameters
  #if defined(AE_shader_draw_parameters) and (defined(SH_MESH_TASK) or defined(SH_MESH) or defined(SH_VERT))
	// in
	const	int		DrawID		= {};	// dynamically uniform
  #endif


	// GLSL_EXT_mesh_shader
  #ifdef SH_MESH_TASK
	void  EmitMeshTasks (uint groupCountX, uint groupCountY, uint groupCountZ);
  #endif


	// GLSL_EXT_mesh_shader
  #ifdef SH_MESH
	void  SetMeshOutputs (uint vertexCount, uint primitiveCount);

	// GL_EXT_multiview
	#ifdef AE_multiview
	// in
	const	int		ViewIndex	= {};
	#endif

	// out
			uint	PrimitivePointIndices	 [_MaxPrimitivePointIndices];
			uint2	PrimitiveLineIndices	 [_MaxPrimitiveLineIndices];
			uint3	PrimitiveTriangleIndices [_MaxPrimitiveTriangleIndices];

	struct MeshPerVertex
	{
		float4		Position;
	  #ifdef AE_large_points
		float		PointSize;
	  #endif
	  #ifdef AE_clip_distance
		float  		ClipDistance [_MaxClipDistance];
	  #endif
	  #ifdef AE_cull_distance
			float  	CullDistance [_MaxCullDistance];
	  #endif
	};
	MeshPerVertex	MeshVertices [_MaxMeshVertices];

	struct MeshPerPrimitive
	{
		int				PrimitiveID;
		int				Layer;
		int				ViewportIndex;
	  #ifdef AE_multiview
		bool			CullPrimitive;			// GL_EXT_multiview
	  #endif
	  #ifdef AE_fragment_shading_rate
		ShadingRateFlag	PrimitiveShadingRate;	// GL_EXT_fragment_shading_rate
	  #endif
	};
	MeshPerPrimitive  MeshPrimitives [_MaxMeshPrimitives];
  #endif


  #ifdef SH_FRAG
	// derivatives
	template <typename T>	ND_ T  dFdx (const T &p);			// QuadBroadcast( p, (Index&2)|1 ) - QuadBroadcast( p, Index&2 )
	template <typename T>	ND_ T  dFdy (const T &p);			// QuadBroadcast( p, (Index&1)|2 ) - QuadBroadcast( p, Index&1 )
	template <typename T>	ND_ T  dFdxFine (const T &);
	template <typename T>	ND_ T  dFdyFine (const T &);
	template <typename T>	ND_ T  dFdxCoarse (const T &);		// 'coarse' compute the same derivative for all quad
	template <typename T>	ND_ T  dFdyCoarse (const T &);
	template <typename T>	ND_ T  fwidth (const T &p);			// Abs(dFdx(p)) + Abs(dFdy(p))
	template <typename T>	ND_ T  fwidthCoarse (const T &);
	template <typename T>	ND_ T  fwidthFine (const T &);

	ND_ float   InterpolateAtCentroid (float);
	ND_ float2  InterpolateAtCentroid (float2);
	ND_ float3  InterpolateAtCentroid (float3);
	ND_ float4  InterpolateAtCentroid (float4);

	ND_ float   InterpolateAtOffset (float);
	ND_ float2  InterpolateAtOffset (float2);
	ND_ float3  InterpolateAtOffset (float3);
	ND_ float4  InterpolateAtOffset (float4);

	ND_ float   InterpolateAtSample (float);
	ND_ float2  InterpolateAtSample (float2);
	ND_ float3  InterpolateAtSample (float3);
	ND_ float4  InterpolateAtSample (float4);

	const	_Uns_	Discard							= {};

	// GL_EXT_demote_to_helper_invocation
	#ifdef AE_demote_to_helper_invocation
	const	_Uns_	Demote							= {};

	// Returns true if the invocation is currently a helper invocation, otherwise returns false.
	ND_		bool	IsHelperInvocation ();
	#endif


	// in
	const 	float4	FragCoord						= {};
	const 	bool	FrontFacing						= {};
	const 	bool	HelperInvocation				= {};
	const	int		Layer							= {};
	const	float2	PointCoord						= {};
	const	int		SampleID						= {};
	const	int		SampleMaskIn [_MaxSampleMask]	= {};
	const	float2	SamplePosition					= {};
	const	int		ViewportIndex					= {};
	const	int		PrimitiveID						= {};

	// out
			float	FragDepth;
			int		SampleMask [_MaxSampleMask];
  #endif


  #ifdef SH_TILE
	// TODO
  #endif


  #if defined(SH_COMPUTE) or defined(SH_MESH_TASK) or defined(SH_MESH)
	//template <typename T> using WGShared = T;	// TODO

	// sync
	void  WorkgroupBarrier ();

	#ifdef AE_memory_scope_semantics
	// if uses non-zero 'sem', then it must not use 'storage' semantics of zero.
	void  ExecutionBarrier (gl::Scope execution, gl::Scope memory, gl::StorageSemantics storage, gl::Semantics sem);

	void  MemoryBarrier (gl::Scope memory, gl::StorageSemantics storage, gl::Semantics sem);
	#endif

	const struct {
		void  All ()		const;	// all memory accesses, scope: shader invocation
		void  Buffer ()		const;
		void  Image ()		const;
		void  Shared ()		const;	// for 'WGShared' variables
		void  Workgroup ()	const;	// all memory accesses, scope: workgroup

		#ifdef AE_shader_subgroup_basic
		void  Subgroup ()	const;	// all memory accesses, scope: subgroup
		#endif
	} memoryBarrier;

	// in
	const 	uint3	GlobalInvocationID		= {};
	const	uint3	LocalInvocationID		= {};
	const	uint	LocalInvocationIndex	= {};
	const	uint3	NumWorkGroups			= {};
	const	uint3	WorkGroupID				= {};
	constexpr uint3	WorkGroupSize			= {};

  #else

	// sync
	#ifdef AE_memory_scope_semantics
	void  MemoryBarrier (gl::Scope memory, gl::StorageSemantics storage, gl::Semantics sem);
	#endif

	const struct {
		void  All ()		const;	// all memory accesses, scope: shader invocation
		void  Buffer ()		const;
		void  Image ()		const;
	} memoryBarrier;

  #endif


  #if defined(SH_RAY_GEN)	or defined(SH_RAY_AHIT)	or defined(SH_RAY_CHIT)	or \
	  defined(SH_RAY_MISS)	or defined(SH_RAY_INT)	or defined(SH_RAY_CALL)

	#ifndef AE_ray_query
	#  define AE_ray_query
	#endif

	template <typename T>	using RayPayload		= T;
	template <typename T>	using RayPayloadIn		= T;
	template <typename T>	using HitAttribute		= T;
	template <typename T>	using CallableData		= T;
	template <typename T>	using CallableDataIn	= T;
	template <typename T>	using ShaderRecord		= T;

	enum class TriangleHitKind
	{
		FrontFacing,
		BackFacing,
	};

	// in
	const	uint3		LaunchID;
	const	uint3		LaunchSize;
  #endif

	// shared RayTracing/RayQuery types
  #ifdef AE_ray_query
	struct AccelerationStructure
	{
		AccelerationStructure ();
		explicit AccelerationStructure (DeviceAddress);
	};

	enum class RayFlags
	{
		None,
		Opaque,						// Force all intersections with the trace to be opaque.
		NoOpaque,					// Force all intersections with the trace to be non-opaque.
		TerminateOnFirstHit,
		SkipClosestHitShader,		// Do not execute a closest hit shader.
		CullBackFacingTriangles,	// Do not intersect with the back face of triangles.
		CullFrontFacingTriangles,	// Do not intersect with the front face of triangles.
		CullOpaque,					// Do not intersect with opaque geometry.
		CullNoOpaque,				// Do not intersect with non-opaque geometry.

		// GL_EXT_ray_flags_primitive_culling
	  #ifdef AE_ray_flags_primitive_culling
		SkipTriangles,				// Do not intersect with any triangle geometries.
		SkipAABB,					// Do not intersect with any aabb geometries.
	  #endif
	};
  #endif

  #if defined(SH_RAY_GEN) or defined(SH_RAY_CHIT) or defined(SH_RAY_MISS)
	void	TraceRay (AccelerationStructure &tlas,
					  RayFlags rayFlags, uint cullMask,
					  uint sbtRecordOffset, uint sbtRecordStride, uint missIndex,
					  const float3 origin, float tMin,
					  const float3 direction, float tMax,
					  int payload);
  #endif
  #if defined(SH_RAY_GEN) or defined(SH_RAY_CHIT) or defined(SH_RAY_MISS) or defined(SH_RAY_CALL)
	void	ExecuteCallable (uint sbtRecordIndex, int callable);
  #endif
  #ifdef SH_RAY_INT
	// Invokes the current hit shader once an intersection shader has determined
	// that a ray intersection has occurred. If the intersection occurred within
	// the current ray interval, the any-hit shader corresponding to the current
	// intersection shader is invoked. If the intersection is not ignored in the
	// any-hit shader, <hitT> is committed as the new 'RayTmax' value of the
	// current ray, <hitKind> is committed as the new value for 'HitKind', and
	// true is returned. If either of those checks fails, then false is returned.
	// If the value of <hitT> falls outside the current ray interval, the hit is
	// rejected and false is returned.
	ND_ bool  ReportIntersection (float hitT, TriangleHitKind hitKind);
  #endif

  #if defined(SH_RAY_AHIT) or defined(SH_RAY_CHIT) or defined(SH_RAY_INT)
	//   Ray hit info
	const	float				HitT;
	const	TriangleHitKind		HitKind;
  #endif

  #if defined(SH_RAY_AHIT) or defined(SH_RAY_CHIT) or defined(SH_RAY_INT)
	//   Geometry instance ids
	const	int			PrimitiveID;
	const	int			InstanceID;
	const	int			InstanceCustomIndex;		// 'RTSceneBuild::InstanceVk::instanceCustomIndex'
	const	int			GeometryIndex;
	//   World space parameters
	const	float3		WorldRayOrigin;
	const	float3		WorldRayDirection;
	const	float3		ObjectRayOrigin;
	const	float3		ObjectRayDirection;
	//   Ray parameters
	const	float		RayTmin;
	const	float		RayTmax;
	const	RayFlags	IncomingRayFlags;
	//   Transform matrices
	const	float4x3	ObjectToWorld;				// 'RTSceneBuild::InstanceVk::transform' without per-geometry transform
	const	float3x4	ObjectToWorld3x4;			// == MatTranspose( ObjectToWorld )
	const	float4x3	WorldToObject;
	const	float3x4	WorldToObject3x4;			// == MatTranspose( WorldToObject )
  #endif

  #ifdef SH_RAY_MISS
	//   World space parameters
	const	float3		WorldRayOrigin;
	const	float3		WorldRayDirection;
	//   Ray parameters
	const	float		RayTmin;
	const	float		RayTmax;
	const	RayFlags	IncomingRayFlags;
  #endif

  #ifdef SH_RAY_AHIT
	// This keyword terminates the calling any-hit shader and continues the ray
	// query without modifying 'RayTmax' and 'HitKind'.
	const	_Uns_		IgnoreIntersection;

	// This keyword terminates the calling any-hit shader, stops the ray
	// traversal, and invokes the closest-hit shader.
	const	_Uns_		TerminateRay;
  #endif

  // GL_EXT_ray_cull_mask
  #if defined(AE_ray_cull_mask) and (defined(SH_RAY_MISS) or defined(SH_RAY_AHIT) or defined(SH_RAY_CHIT) or defined(SH_RAY_INT))
	// in
	const	uint		CullMask;
  #endif

  // GL_EXT_ray_tracing_position_fetch
  #if defined(AE_ray_tracing_position_fetch) and (defined(SH_RAY_AHIT) or defined(SH_RAY_CHIT))
	// in
	const	float3		HitTriangleVertexPositions [3];
  #endif

  #ifdef AE_ray_query
	struct RayQuery		{ RayQuery(); };

	enum class RayQueryCommittedIntersection
	{
		None,
		Triangle,
		Generated,
	};

	enum class RayQueryCandidateIntersection
	{
		Triangle,
		AABB,
	};

	const struct {
		// Initializes a ray query object but does not start traversal,
		// discarding all previous state. Traversal is considered incomplete.
			void		Initialize (RayQuery &rq, AccelerationStructure &tlas,
									RayFlags rayFlags, uint cullMask, const float3 origin,
									float tMin, const float3 direction, float tMax) const;

		// Allows traversal to proceed when traversal is incomplete.
		// Returns 'true' if traversal is incomplete after the operation, and 'false'
		// if traversal is complete.
		ND_ bool		Proceed (RayQuery &rq) const;

		// Terminates execution of ray query when traversal is incomplete.
			void		Terminate (RayQuery &rq) const;

		//  Generates and commits an intersection at <tHit>. Only valid when a candidate AABB intersection is found.
			void		GenerateIntersection (RayQuery &rq, float tHit) const;

		// Commits current candidate triangle intersection to be included in
		// determination of the closest hit for a ray query. Only valid when a
		// candidate triangle intersection is found.
			void		ConfirmIntersection (RayQuery &rq) const;

		// Returns type of committed or candidate intersection.
		// 'RayQueryCommittedIntersection' or 'RayQueryCandidateIntersection'.
		// <committed> must be a compile time constant value.
		ND_ uint		GetIntersectionType (RayQuery &rq, bool committed) const;

		//  Returns the parametric <tMin> value for the ray query.
		ND_ float		GetRayTMin (RayQuery &rq) const;

		// Returns the ray flags for the ray query.
		ND_ RayFlags	GetRayFlags (RayQuery &rq) const;

		// Returns the world-space origin of ray for the ray query.
		ND_ float3		GetWorldRayOrigin (RayQuery &rq) const;

		// Returns the world-space direction of ray for the ray query.
		ND_ float3		GetWorldRayDirection (RayQuery &rq) const;

		// Returns the parametric <t> value for current intersection.
		ND_ float		GetIntersectionT (RayQuery &rq, bool committed) const;

		// Returns the 'RTSceneBuild::InstanceVk::instanceCustomIndex' value.
		ND_ int			GetIntersectionInstanceCustomIndex (RayQuery &rq, bool committed) const;

		// Returns the index of the instance for current intersection.
		ND_ int			GetIntersectionInstanceId (RayQuery &rq, bool committed) const;

		// Returns the 'RTSceneBuild::InstanceVk::instanceSBTOffset' value.
		ND_ uint		GetIntersectionInstanceSBTOffset (RayQuery &rq, bool committed) const;

		// Returns implementation defined index of geometry for current intersection.
		ND_ int			GetIntersectionGeometryIndex (RayQuery &rq, bool committed) const;

		// Returns the index of the primitive (triangle or bounding box) within the
		// geometry of the bottom-level acceleration structure being processed.
		ND_ int			GetIntersectionPrimitiveIndex (RayQuery &rq, bool committed) const;

		// Returns two component floating point barycentric coordinates of current intersection of ray.
		ND_ float2		GetIntersectionBarycentrics (RayQuery &rq, bool committed) const;

		// Returns 'true' if the current intersection is a front facing triangle.
		ND_ bool		GetIntersectionFrontFace (RayQuery &rq, bool committed) const;

		// Returns 'true' if the current candidate intersection is an opaque AABB.
		ND_ bool		GetIntersectionCandidateAABBOpaque (RayQuery &rq) const;

		// Returns object-space direction of ray for current intersection.
		ND_ float3		GetIntersectionObjectRayDirection (RayQuery &rq, bool committed) const;

		// Returns object-space origin of ray for current intersection.
		ND_ float3		GetIntersectionObjectRayOrigin (RayQuery &rq, bool committed) const;

		// Returns object to world transformation matrix for current intersection.
		ND_ float4x3	GetIntersectionObjectToWorld (RayQuery &rq, bool committed) const;

		// Returns world to object transformation matrix for current intersection.
		ND_ float4x3	GetIntersectionWorldToObject (RayQuery &rq, bool committed) const;

		// GL_EXT_ray_tracing_position_fetch
		#ifdef AE_ray_tracing_position_fetch
			void		GetIntersectionTriangleVertexPositions (RayQuery &rq, bool committed, OUT float3 (&positions)[3]) const;
		#endif

	} rayQuery {};
  #endif


	// build ray tracing acceleration structure
  #ifdef AE_RTAS_BUILD
	enum class GeometryInstanceFlags : uint
	{
		TriangleCullDisable,
		TriangleFrontCCW,
		ForceOpaque,
		ForceNonOpaque,
	};
  #endif

	// GL_KHR_cooperative_matrix
	// whole subgroup / workgroup must execute same command
  #if defined(AE_cooperative_matrix) and defined(AE_memory_scope_semantics)

	enum class MatrixUse
	{
		A,				// 'a' argument in CoopMatMulAdd()
		B,				// 'b' argument in CoopMatMulAdd()
		Accumulator,	// 'c' argument in CoopMatMulAdd()

		C	= Accumulator
	};

	enum class MatrixOperands
	{
		None					= 0,
		SaturatingAccumulation	= 0x10,
	};

	enum class CooperativeMatrixLayout
	{
		RowMajor,
		ColumnMajor
	};

	template <typename T, Scope ScopeType, uint Rows, uint Columns, MatrixUse Use>
	struct CoopMat
	{
		explicit CoopMat (T scalar);

		template <typename T2, Scope ScopeType, uint Rows, uint Columns, MatrixUse Use>
		explicit CoopMat (const CoopMat<T2, ScopeType, Rows, Columns, Use> &);

		ND_ uint	length()				const	{ return Rows * Columns; }

		ND_ T &		operator [] (int i);
		ND_ T		operator [] (int i)		const;

		ND_ CoopMat		operator - ()					const;

		ND_ CoopMat		operator + (const CoopMat &)	const;
		ND_ CoopMat		operator - (const CoopMat &)	const;
		ND_ CoopMat		operator * (const CoopMat &)	const;
		ND_ CoopMat		operator / (const CoopMat &)	const;

		ND_ CoopMat		operator * (T)					const;

		// for integer types
		ND_ CoopMat		operator & (const CoopMat &)	const;
		ND_ CoopMat		operator ^ (const CoopMat &)	const;
		ND_ CoopMat		operator | (const CoopMat &)	const;
		ND_ CoopMat		operator ~ ()					const;
	};

	// read 'm' from 'buf[ firstElement * stride * rows ]' for row major or
	// 'buf[ firstElement * stride * cols ]' to column major
	template <typename T, Scope S, uint R, uint C, MatrixUse U, typename B>
	void  CoopMatLoad  (OUT CoopMat<T,S,R,C,U>	&m,
						volatile coherent B*	buf,
						uint					firstElement,		// in elements (B)
						uint					stride,				// in elements (B)
						CooperativeMatrixLayout	layout);

	// write 'm' to 'buf[ firstElement * stride * rows ]' for row major or
	// 'buf[ firstElement * stride * cols ]' to column major
	template <typename T, Scope S, uint R, uint C, MatrixUse U, typename B>
	void  CoopMatStore (CoopMat<T,S,R,C,U>			m,
						OUT volatile coherent B*	buf,
						uint						firstElement,	// in elements (B)
						uint						stride,			// in elements (B)
						CooperativeMatrixLayout		layout);

	// r = a * b + c
	template <typename T, Scope S, uint M, uint N, uint K>
	ND_ CoopMat<T,S,M,N,MatrixUse::C>  CoopMatMulAdd (CoopMat<T,S,M,K,MatrixUse::A>  a,
													  CoopMat<T,S,K,N,MatrixUse::B>  b,
													  CoopMat<T,S,M,N,MatrixUse::C>  c,
													  MatrixOperands				 matrixOperands = MatrixOperands::None);

  #endif // AE_cooperative_matrix and AE_memory_scope_semantics

	// GLSL_NV_cooperative_vector
  #if defined(AE_cooperative_vector)

	template <typename T, uint NumComps>
	struct CoopVec
	{
		CoopVec ();

		template <typename B>
		explicit CoopVec (const CoopVec<B,NumComps> &);

		explicit CoopVec (T);

		ND_ uint		length()						const	{ return NumComps; }

		ND_ T &			operator [] (int i);
		ND_ T			operator [] (int i)				const;

		ND_ CoopVec		operator - ()					const;

		ND_ CoopVec		operator + (const CoopVec &)	const;
		ND_ CoopVec		operator - (const CoopVec &)	const;
		ND_ CoopVec		operator * (const CoopVec &)	const;
		ND_ CoopVec		operator / (const CoopVec &)	const;

		ND_ CoopVec		operator * (T)					const;

		// for integer types
		ND_ CoopVec		operator & (const CoopVec &)	const;
		ND_ CoopVec		operator ^ (const CoopVec &)	const;
		ND_ CoopVec		operator | (const CoopVec &)	const;
		ND_ CoopVec		operator ~ ()					const;

		ND_ friend CoopVec  fma   (const CoopVec &a, const CoopVec &b, const CoopVec &c);
		ND_ friend CoopVec  exp   (const CoopVec &);
		ND_ friend CoopVec  log   (const CoopVec &);
		ND_ friend CoopVec  tanh  (const CoopVec &);
		ND_ friend CoopVec  atan  (const CoopVec &);
		ND_ friend CoopVec  atan  (const CoopVec &,  const CoopVec &);
		ND_ friend CoopVec  min   (const CoopVec &,  const CoopVec &);
		ND_ friend CoopVec  max   (const CoopVec &,  const CoopVec &);
		ND_ friend CoopVec  clamp (const CoopVec &x, const CoopVec &minVal, const CoopVec &maxVal);
		ND_ friend CoopVec  step  (const CoopVec &a, const CoopVec &b);
	};

	// VkComponentTypeKHR
	enum class ComponentType
	{
		Float16,
		Float32,
		Float64,
		SInt8,
		SInt16,
		SInt32,
		SInt64,
		UInt8,
		UInt16,
		UInt32,
		UInt64,
		SInt8Packed,		// u32 as 4x s8
		UInt8Packed,		// u32 as 4x u8
		FloatE4M3,			// fp8
		FloatE5M2,			// fp8
	};

	enum class CoopVectorMatrixLayout
	{
		RowMajor,
		ColumnMajor,
		InferencingOptimal,
		TrainingOptimal
	};

	template <typename ResultTy, uint ResultComps,
			  typename InputTy,  uint InputComps,
			  typename MatrixTy,
			  typename BiasTy
			 >
	void  CoopVecMatMulAdd (OUT CoopVec<ResultTy, ResultComps>	&result,	// [M],  'ResultTy' must be: half, float, int, uint
							CoopVec<InputTy, InputComps>		input,		// [K],  'InputTy'  must be: half, float, int, uint
							ComponentType			inputInterpretation,	// bitcast for packed type, conversion for other
							const MatrixTy[]		matrix,					// [MxK],  'MatrixTy' ignored, used bitcast
							uint					matrixOffset,			// 64b align, in bytes
							ComponentType			matrixInterpretation,
							const BiasTy[]			bias,					// [M],  'BiasTy' ignored, used bitcast
							uint					biasOffset,				// 16b align, in bytes
							ComponentType			biasInterpretation,
							uint					M,
							uint					K,
							CoopVectorMatrixLayout	matrixLayout,
							bool					transpose,
							uint					matrixStride);			// 16b align, in bytes, ignored for optimal layouts

	template <typename ResultTy, uint ResultComps,
			  typename InputTy,  uint InputComps,
			  typename MatrixTy
			 >
	void  CoopVecMatMul (OUT CoopVec<ResultTy, ResultComps> &result,		// [M],  'ResultTy' must be: half, float, int, uint
						 CoopVec<InputTy, InputComps>		input,			// [K],  'InputTy'  must be: half, float, int, uint
						 ComponentType			inputInterpretation,		// bitcast for packed type, conversion for other
						 const MatrixTy[]		matrix,						// [MxK],  'MatrixTy' ignored, used bitcast
						 uint					matrixOffset,				// 64b align, in bytes
						 ComponentType			matrixInterpretation,
						 uint					M,
						 uint					K,
						 CoopVectorMatrixLayout	matrixLayout,
						 bool					transpose,
						 uint					matrixStride);				// 16b align, in bytes, ignored for optimal layouts

	template <typename T, uint NumComps, typename ArrayElemTy>
	void  CoopVecLoad (OUT CoopVec<T, NumComps>			&v,
					   volatile coherent ArrayElemTy[]	buffer,
					   uint								bufferOffset);		// 16b align, in bytes

	template <typename T, uint NumComps, typename ArrayElemTy>
	void  CoopVecStore (const CoopVec<T, NumComps>			&v,
						OUT volatile coherent ArrayElemTy[]	buffer,
						uint								bufferOffset);	// 16b align, in bytes

	// requires 'cooperativeVectorTraining' feature
  # if defined(AE_cooperative_vector_training)

	// The following function computes the outer product between column vectors v1
	// and v2, i.e. v1*transpose(v2), and the resulting MxN matrix is atomically
	// (with device scope) accumulated in memory.
	template <typename T, uint M, uint N>
	void  CoopVecOuterProductAccum (const CoopVec<T, M>		&v1,			// [M],  'T' must be half of float
									const CoopVec<T, N>		&v2,			// [N]
									INOUT T[]				buffer,			// matrix [MxN]
									uint					bufferOffset,	// 16b align, in bytes
									uint					stride,			// ignored
									CoopVectorMatrixLayout	matrixLayout,	// must be 'gl::CoopVectorMatrixLayout::TrainingOptimal'
									ComponentType			matrixInterpretation);

	// The following function component-wise atomically (with device scope) adds
	// components of the vector 'v' to the corresponding elements of an array in memory.
	template <typename T, uint N>
	void  CoopVecReduceSumAccum (const CoopVec<T, N>	&v,					// [N],  'T' must be half of float
								 INOUT T[]				buffer,				// [N]
								 uint					bufferOffset);		// 16b align, in bytes

  # endif // AE_cooperative_vector_training
  #endif // AE_cooperative_vector

	// GL_ARB_fragment_shader_interlock
  #if defined(SH_FRAG) and defined(AE_fragment_shader_interlock)
	void  BeginInvocationInterlock ();
	void  EndInvocationInterlock ();
  #endif

	// GL_EXT_fragment_shader_barycentric
  #if defined(SH_FRAG) and defined(AE_fragment_shader_barycentric)
	// in
	const	float3	BaryCoord ();
	const	float3	BaryCoordNoPersp ();

	// PerVertex<>
  #endif

	// GLSL_EXT_fragment_invocation_density
  #if defined(SH_FRAG) and defined(AE_fragment_invocation_density)
	// in
	const	int2	FragSize;
	const	int		FragInvocationCount;
  #endif

	// NVidia extensions
	struct {

		// GL_NV_shader_sm_builtins
		#ifdef AE_NV_shader_sm_builtins
		// in
		const	uint	WarpsPerSM;
		const	uint	SMCount;
		const	uint	WarpID;		// range: 0 .. WarpsPerSM
		const	uint	SMID;		// range: 0 .. SMCount
		#endif
	} NV;

	// ARM extensions
	struct {

		// GL_ARM_shader_core_builtins
		#ifdef AE_ARM_shader_core_builtins
		// in
		const	uint	CoreID;		// range: 0 .. CoreMaxID
		const	uint	CoreCount;
		const	uint	CoreMaxID;
		const	uint	WarpID;		// range: 0 .. WarpMaxID
		const	uint	WarpMaxID;
		#endif
	} ARM;

  #ifdef AE_expect_assume
	void  Assume (bool condition);

	template <typename T>
	ND_ T  Expect (T value, T expected);
  #endif

	// GLSL_EXT_debug_printf
	void  Printf (const char*, ...);

} gl;


// GL_EXT_fragment_shading_rate
#if defined(AE_fragment_shading_rate) and (defined(SH_VERT) or defined(SH_GEOM) or defined(SH_FRAG) or defined(SH_MESH))
	ND_ gl::ShadingRateFlag  operator | (gl::ShadingRateFlag lhs, gl::ShadingRateFlag rhs);
	ND_ gl::ShadingRateFlag  operator & (gl::ShadingRateFlag lhs, gl::ShadingRateFlag rhs);
#endif

#ifdef AE_RTAS_BUILD
	ND_ gl::GeometryInstanceFlags  operator | (gl::GeometryInstanceFlags lhs, gl::GeometryInstanceFlags rhs);
#endif

#ifdef AE_ray_query
	ND_ gl::RayFlags  operator | (gl::RayFlags lhs, gl::RayFlags rhs);
#endif

#ifdef AE_memory_scope_semantics
	ND_ gl::Scope				operator | (gl::Scope lhs, gl::Scope rhs);
	ND_ gl::Semantics			operator | (gl::Semantics lhs, gl::Semantics rhs);
	ND_ gl::StorageSemantics	operator | (gl::StorageSemantics lhs, gl::StorageSemantics rhs);
#endif


// GL_EXT_control_flow_attributes
//	[[unroll]]
//	[[dont_unroll]] or [[loop]]
//	[[dependency_infinite]]
//	[[dependency_length(4)]]
//	[[flatten]]
//	[[dont_flatten]] or [[branch]]

// GL_EXT_control_flow_attributes2
//	[[unroll, min_iterations(2)]]
//	[[unroll, max_iterations(4)]]
//	[[unroll, iteration_multiple(4)]]
//	[[unroll, peel_count(3)]]
//	[[unroll, partial_count(2)]]

// GL_EXT_subgroup_uniform_control_flow
//	void main() [[subroup_uniform_control_flow]]

// GL_EXT_maximal_reconvergence
//	void main() [[maximally_reconverges]]

#ifdef __INTELLISENSE__
#	define SH_VERT
#	define SH_TESS_CTRL
#	define SH_TESS_EVAL
#	define SH_GEOM
#	define SH_FRAG
#	define SH_COMPUTE
#	define SH_TILE
#	define SH_MESH_TASK
#	define SH_MESH
#	define SH_RAY_GEN
#	define SH_RAY_AHIT
#	define SH_RAY_CHIT
#	define SH_RAY_MISS
#	define SH_RAY_INT
#	define SH_RAY_CALL
#endif
