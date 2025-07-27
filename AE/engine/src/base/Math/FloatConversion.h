// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/GLM.h"
#include "base/Algorithms/Cast.h"

namespace AE::Base
{

	//
	// Float Conversion
	//
	struct FloatConversion
	{
		enum class EMode
		{
			CopyBits		= 0,			// just copy bits
			CheckNanInf		= 1 << 1,		// convert special values (NaN, Inf)
			CheckOverflow	= 1 << 2,		// set Inf if can't convert src exponent to dst

			RoundToNearest	= 1 << 4,
		//	RoundToLargest	= 1 << 5,

			Normalize		= 1 << 6,
			AllowDenorm		= 1 << 7,

			FloorFast	= CheckNanInf,
			RoundFast	= CheckNanInf | RoundToNearest,

			Floor		= CheckNanInf | CheckOverflow | Normalize | AllowDenorm,
			Round		= Floor | RoundToNearest,

			_BITOPS_
		};


		template <EMode Mode, typename SrcBits, typename DstBits>
		__Cx__ static void	ConvertBits_Accurate (SrcBits src, OUT DstBits &dst)	__NE___;

		template <EMode Mode, typename SrcBits, typename DstBits,
				  typename SrcU = ToUnsignedInteger< SrcBits >,
				  typename DstU = ToUnsignedInteger< DstBits >
				 >
		__Cx__ static void	ConvertBits_Fast (SrcU src, OUT DstU &dst)				__NE___;

		template <EMode Mode, typename Src, typename Dst>
		__Cx__ static void	Convert (Src src, OUT Dst &dst)							__NE___;

		template <EMode Mode, typename Dst, typename Src>
		NdCx__ static Dst	Convert2 (Src src)										__NE___	{ Dst dst;  Convert<Mode>( src, OUT dst );  return dst; }

		template <EMode Mode, typename Dst, typename Src, typename DstBits>
		__Cx__ static Dst	Convert3 (Src src, OUT DstBits &dstBits)				__NE___	{ Dst dst;  Convert<Mode>( src, OUT dst );  dstBits = BitCast<DstBits>(dst);  return dst; }

		template <typename T>
		struct BitsForType;

	// utils
		template <typename Bits>
		__Cx__ static Bits	MinDelta (Bits x)										__NE___;
		
		template <typename Bits>
		__Cx__ static Bits	Next (Bits x)											__NE___;	// x + ulp
	};
//-----------------------------------------------------------------------------


	struct SFloat16;
	struct UFloat16;
	struct BFloat16;
	struct UFloat8;

	using half = SFloat16;



	//
	// Float32
	//
	struct Float32Bits
	{
	// types
		static constexpr uint	_ManBits	= 23;
		static constexpr uint	_ExpBits	= 8;
		static constexpr uint	_NaNExp		= (1u << _ExpBits) - 1;
		static constexpr uint	_MidExp		= (1u << (_ExpBits-1)) - 1;		// zero in signed form
		static constexpr uint	_MaxExp		= _NaNExp - 1;
		static constexpr uint	_MaxMan		= (1u << _ManBits) - 1;
		static constexpr uint	_Signed		= true;

	// variables
		uint	m	: _ManBits;	// mantissa bits
		uint	e	: _ExpBits;	// exponent bits (-127 .. +128)
		uint	s	: 1;		// sign bit

	// methods
		__Cx__ Float32Bits ()						__NE___ : m{0}, e{0}, s{0} {}
		__Cx__ explicit Float32Bits (float val)		__NE___ { auto f = BitCast<Float32Bits>(val);  m=f.m;  e=f.e;  s=f.s; }

		NdCx__ bool			IsNaN ()				C_NE___ { return e == _NaNExp and m != 0; }
		NdCx__ bool			IsInf ()				C_NE___	{ return e == _NaNExp and m == 0; }
		NdCx__ bool			IsSubnormal ()			C_NE___	{ return e == 0 and m != 0; }			// denormal
		NdCx__ bool			IsZero ()				C_NE___	{ return e == 0 and m == 0; }

		NdCx__ int			IntExp2 ()				C_NE___	{ return int(e) - int(_MidExp); }

		NdCx__ uint			AsInteger ()			C_NE___	{ return BitCast<uint>(*this); }
		NdCx__ float		AsFloatPoint ()			C_NE___	{ return BitCast<float>(*this); }

		NdCx__ float		MinDelta ()				C_NE___	{ return FloatConversion::MinDelta( *this ).AsFloatPoint(); }	// ULP
		
		NdCx__ bool  operator == (Float32Bits rhs)	C_NE___	{ return AsInteger() == rhs.AsInteger(); }

		NdCx__ explicit operator float ()			C_NE___	{ return AsFloatPoint(); }

		NdCx__ static auto	SmallestSubnormal ()	__NE___	{ Float32Bits b;  b.m=1;		b.e=0;			b.s=0;  return b; }		// 1.4012e-45
		NdCx__ static auto	SmallestNormal ()		__NE___	{ Float32Bits b;  b.m=0;		b.e=1;			b.s=0;  return b; }		// 1.1754e−38
		NdCx__ static auto	LargestNormal ()		__NE___	{ Float32Bits b;  b.m=_MaxMan;	b.e=_MaxExp;	b.s=0;  return b; }		// 3.4028e+38

		NdCx__ static auto	Min ()					__NE___	{ return SmallestSubnormal(); }
		NdCx__ static auto	Max ()					__NE___	{ return LargestNormal(); }

		NdCx__ static auto	Zero ()					__NE___	{ Float32Bits b;  b.m=0;		b.e=0;			b.s=0;  return b; }		// 0.0
		NdCx__ static auto	One ()					__NE___	{ Float32Bits b;  b.m=0;		b.e=_MidExp;	b.s=0;  return b; }		// 1.0

		NdCx__ static auto  Inf ()					__NE___	{ Float32Bits b;  b.m=0;		b.e=_NaNExp;	b.s=0;  return b; }
		NdCx__ static auto  NegInf ()				__NE___	{ Float32Bits b;  b.m=0;		b.e=_NaNExp;	b.s=1;  return b; }
		NdCx__ static auto	NaN ()					__NE___	{ Float32Bits b;  b.m=1;		b.e=_NaNExp;	b.s=0;  return b; }
	};
	StaticAssert( sizeof(Float32Bits) == sizeof(float) );
//-----------------------------------------------------------------------------



	//
	// Float64
	//
	struct Float64Bits
	{
	// types
		static constexpr uint	_ManBits	= 52;
		static constexpr uint	_ExpBits	= 11;
		static constexpr uint	_NaNExp		= (1u << _ExpBits) - 1;
		static constexpr uint	_MidExp		= (1u << (_ExpBits-1)) - 1;		// zero in signed form
		static constexpr uint	_MaxExp		= _NaNExp - 1;
		static constexpr ulong	_MaxMan		= (1ull << _ManBits) - 1;
		static constexpr uint	_Signed		= true;

	// variables
		ulong	m	: _ManBits;	// mantissa bits
		ulong	e	: _ExpBits;	// exponent bits (-1023 .. +1024)
		ulong	s	: 1;		// sign bit

	// methods
		__Cx__ Float64Bits ()						__NE___ : m{0}, e{0}, s{0} {}
		__Cx__ explicit Float64Bits (double val)	__NE___ { auto f = BitCast<Float64Bits>(val);  m=f.m;  e=f.e;  s=f.s; }

		NdCx__ bool			IsNaN ()				C_NE___ { return e == _NaNExp and m != 0; }
		NdCx__ bool			IsInf ()				C_NE___	{ return e == _NaNExp and m == 0; }
		NdCx__ bool			IsSubnormal ()			C_NE___	{ return e == 0 and m != 0; }
		NdCx__ bool			IsZero ()				C_NE___	{ return e == 0 and m == 0; }

		NdCx__ int			IntExp2 ()				C_NE___	{ return int(e) - int(_MidExp); }

		NdCx__ ulong		AsInteger ()			C_NE___	{ return BitCast<ulong>(*this); }
		NdCx__ double		AsFloatPoint ()			C_NE___	{ return BitCast<double>(*this); }
		
		NdCx__ double		MinDelta ()				C_NE___	{ return FloatConversion::MinDelta( *this ).AsFloatPoint(); }	// ULP
		
		NdCx__ bool  operator == (Float64Bits rhs)	C_NE___	{ return AsInteger() == rhs.AsInteger(); }

		NdCx__ explicit operator double ()			C_NE___	{ return AsFloatPoint(); }

		NdCx__ static auto	SmallestSubnormal ()	__NE___	{ Float64Bits b;  b.m=1;		b.e=0;			b.s=0;  return b; }		// 4.9406e-324
		NdCx__ static auto	SmallestNormal ()		__NE___	{ Float64Bits b;  b.m=0;		b.e=1;			b.s=0;  return b; }		// 2.2250e-308
		NdCx__ static auto	LargestNormal ()		__NE___	{ Float64Bits b;  b.m=_MaxMan;	b.e=_MaxExp;	b.s=0;  return b; }		// 1.7976e+308

		NdCx__ static auto	Min ()					__NE___	{ return SmallestSubnormal(); }
		NdCx__ static auto	Max ()					__NE___	{ return LargestNormal(); }

		NdCx__ static auto	Zero ()					__NE___	{ Float64Bits b;  b.m=0;		b.e=0;			b.s=0;  return b; }		// 0.0
		NdCx__ static auto	One ()					__NE___	{ Float64Bits b;  b.m=0;		b.e=_MidExp;	b.s=0;  return b; }		// 1.0

		NdCx__ static auto  Inf ()					__NE___	{ Float64Bits b;  b.m=0;		b.e=_NaNExp;	b.s=0;  return b; }
		NdCx__ static auto  NegInf ()				__NE___	{ Float64Bits b;  b.m=0;		b.e=_NaNExp;	b.s=1;  return b; }
		NdCx__ static auto	NaN ()					__NE___	{ Float64Bits b;  b.m=1;		b.e=_NaNExp;	b.s=0;  return b; }
	};
	StaticAssert( sizeof(Float64Bits) == sizeof(double) );
//-----------------------------------------------------------------------------



	//
	// Half (SFloat16) (IEEE 754)
	//
	struct SFloat16
	{
	// types
	public:
		using Self	= SFloat16;

		struct Bits
		{
			static constexpr uint	_ManBits	= 10;
			static constexpr uint	_ExpBits	= 5;
			static constexpr uint	_NaNExp		= (1u << _ExpBits) - 1;
			static constexpr uint	_MidExp		= (1u << (_ExpBits-1)) - 1;		// zero in signed form
			static constexpr uint	_MaxExp		= _NaNExp - 1;
			static constexpr uint	_MaxMan		= (1u << _ManBits) - 1;
			static constexpr uint	_Signed		= true;

			ushort	m	: _ManBits;		// mantissa bits
			ushort	e	: _ExpBits;		// exponent	bits (-14 .. +15)
			ushort	s	: 1;			// sign bits
		};
		StaticAssert( sizeof(Bits) == sizeof(ushort) );

	private:
		enum class EValue : ushort {};
		using FC = FloatConversion;


	// variables
	public:
		Bits		_bits;


	// methods
	private:
		__Cx__ explicit SFloat16 (EValue val)				__NE___	: _bits{ BitCast<Bits>( val )} {}
		__Cx__ explicit SFloat16 (Bits bits)				__NE___	: _bits{ bits } {}

	public:
		__Cx__ SFloat16 ()									__NE___	: _bits{} {}
		__Cx__ SFloat16 (const Self &other)					__NE___	= default;
		__Cx__ explicit SFloat16 (float f)					__NE___ { Set( f ); }
	  #if AE_SIMD_NEON_HALF
		__Cx__ explicit SFloat16 (float16_t f)				__NE___ : _bits{ BitCast<Bits>( f )} {}
	  #endif

		__Cx__ Self&		operator =  (const Self &rhs)	__NE___	= default;

		NdCx__ bool			operator == (const Self &rhs)	C_NE___	{ return AsInteger() == rhs.AsInteger(); }

		// set/get
		NdCx__ ushort		AsInteger ()					C_NE___	{ return BitCast<ushort>( _bits ); }
		NdCx__ Bits			GetBits ()						C_NE___	{ return _bits; }

		template <typename T = float>
		NdCx__ T			Get ()							C_NE___	{ return FC::Convert2< FC::EMode::Round, T >( *this ); }

		template <typename T>
		__Cx__ Self&		Set (T val)						__NE___	{ FC::Convert3< FC::EMode::Round, Self >( val, OUT _bits );  return *this; }

		template <typename T = float>
		NdCx__ T			GetFast ()						C_NE___	{ return FC::Convert2< FC::EMode::RoundFast, T >( *this ); }

		template <typename T>
		__Cx__ Self&		SetFast (T val)					__NE___	{ FC::Convert3< FC::EMode::RoundFast, Self >( val, OUT _bits );  return *this; }
		
		NdCx__ int			IntExp2 ()						C_NE___	{ return int(_bits.e) - int(Bits::_MidExp); }

		NdCx__ bool			IsNaN ()						C_NE___	{ return _bits.e == Bits::_NaNExp and _bits.m != 0; }
		NdCx__ bool			IsInfinity ()					C_NE___	{ return _bits.e == Bits::_NaNExp and _bits.m == 0; }
		NdCx__ bool			IsFinite ()						C_NE___	{ return _bits.e != Bits::_NaNExp; }
		NdCx__ bool			IsNegative ()					C_NE___	{ return _bits.s == 1; }			// same as 'std::signbit'
		NdCx__ bool			IsSubnormal ()					C_NE___	{ return _bits.e == 0 and _bits.m != 0; }
		NdCx__ bool			IsZero ()						C_NE___	{ return _bits.e == 0 and _bits.m == 0; }

		NdCx__ Self			MinDelta ()						C_NE___	{ return Self{ FloatConversion::MinDelta( _bits )}; }	// ULP

		NdCx__ explicit operator float ()					C_NE___	{ return Get<float>(); }
		NdCx__ explicit operator double ()					C_NE___	{ return Get<double>(); }
	  #if AE_SIMD_NEON_HALF
		NdCx__ explicit operator float16_t ()				C_NE___	{ return BitCast<float16_t>( _bits ); }
	  #endif

		NdCx__ static Self  SmallestSubnormal ()			__NE___	{ return Self{EValue(0x0001)}; }	// 5.9e-8
		NdCx__ static Self  SmallestNormal ()				__NE___	{ return Self{EValue(0x0400)}; }	// TODO
		NdCx__ static Self  LargestNormal ()				__NE___	{ return Self{EValue(0x7BFF)}; }	// 65504
		NdCx__ static Self	Min ()							__NE___	{ return SmallestSubnormal(); }
		NdCx__ static Self	Max ()							__NE___	{ return LargestNormal(); }
		NdCx__ static Self  MaxNeg ()						__NE___	{ return Self{EValue(0xFBFF)}; }	// -65504
		NdCx__ static Self  Inf ()							__NE___	{ return Self{EValue(0x7C00)}; }
		NdCx__ static Self  NegInf ()						__NE___	{ return Self{EValue(0xFC00)}; }
		NdCx__ static Self	NaN ()							__NE___	{ return Self{EValue(0xFFFF)}; }
		NdCx__ static Self	Zero ()							__NE___	{ return Self{EValue(0)}; }			// 0.0
		NdCx__ static Self	One ()							__NE___	{ return Self{EValue(0x3C00)}; }	// 1.0
		NdCx__ static float Epsilon ()						__NE___	{ return 2.0e-10f; }
	};
//-----------------------------------------------------------------------------



	//
	// Unsigned 16 bit Float
	//
	struct UFloat16
	{
	// types
	public:
		using Self	= UFloat16;

		struct Bits
		{
			static constexpr uint	_ManBits	= 10;
			static constexpr uint	_ExpBits	= 6;
			static constexpr uint	_NaNExp		= (1u << _ExpBits) - 1;
			static constexpr uint	_MidExp		= (1u << (_ExpBits-1)) - 1;		// zero in signed form
			static constexpr uint	_MaxExp		= _NaNExp - 1;
			static constexpr uint	_MaxMan		= (1u << _ManBits) - 1;
			static constexpr uint	_Signed		= false;

			ushort	m	: _ManBits;	// mantissa bits
			ushort	e	: _ExpBits;	// exponent	bits (-31 .. +32)
		};
		StaticAssert( sizeof(ushort) == sizeof(Bits) );

	private:
		enum class EValue : ushort {};
		using FC = FloatConversion;


	// variables
	private:
		Bits		_bits;


	// methods
	private:
		__Cx__ explicit UFloat16 (EValue val)				__NE___	: _bits{ BitCast<Bits>( val )} {}
		__Cx__ explicit UFloat16 (Bits bits)				__NE___	: _bits{ bits } {}

	public:
		__Cx__ UFloat16 ()									__NE___	: _bits{} {}
		__Cx__ UFloat16 (const Self &other)					__NE___	= default;
		__Cx__ explicit UFloat16 (float val)				__NE___	{ Set( val ); }

		__Cx__ Self&		operator =  (const Self &rhs)	__NE___	= default;

		NdCx__ bool			operator == (const Self &rhs)	C_NE___	{ return AsInteger() == rhs.AsInteger(); }

		// set/get
		NdCx__ ushort		AsInteger ()					C_NE___	{ return BitCast<ushort>( _bits ); }
		NdCx__ Bits			GetBits ()						C_NE___	{ return _bits; }

		template <typename T = float>
		NdCx__ T			Get ()							C_NE___	{ return FC::Convert2< FC::EMode::Round, T >( *this ); }

		template <typename T>
		__Cx__ Self&		Set (T val)						__NE___	{ FC::Convert3< FC::EMode::Round, Self >( val, OUT _bits );  return *this; }

		template <typename T = float>
		NdCx__ T			GetFast ()						C_NE___	{ return FC::Convert2< FC::EMode::RoundFast, T >( *this ); }

		template <typename T>
		__Cx__ Self&		SetFast (T val)					__NE___	{ FC::Convert3< FC::EMode::RoundFast, Self >( val, OUT _bits );  return *this; }
		
		NdCx__ int			IntExp2 ()						C_NE___	{ return int(_bits.e) - int(Bits::_MidExp); }

		NdCx__ bool			IsNaN ()						C_NE___	{ return _bits.e == Bits::_NaNExp and _bits.m != 0; }
		NdCx__ bool			IsInfinity ()					C_NE___	{ return _bits.e == Bits::_NaNExp and _bits.m == 0; }
		NdCx__ bool			IsFinite ()						C_NE___	{ return _bits.e != Bits::_NaNExp; }
		NdCx__ bool			IsNegative ()					C_NE___	{ return false; }					// same as 'std::signbit'
		NdCx__ bool			IsSubnormal ()					C_NE___	{ return _bits.e == 0 and _bits.m != 0; }
		NdCx__ bool			IsZero ()						C_NE___	{ return _bits.e == 0 and _bits.m == 0; }
		
		NdCx__ Self			MinDelta ()						C_NE___	{ return Self{ FloatConversion::MinDelta( _bits )}; }	// ULP

		NdCx__ explicit operator float ()					C_NE___	{ return Get<float>(); }
		NdCx__ explicit operator double ()					C_NE___	{ return Get<double>(); }

		NdCx__ static Self  SmallestSubnormal ()			__NE___	{ return Self{EValue(0x0001)}; }	// 9.09e-13
		NdCx__ static Self  SmallestNormal ()				__NE___	{ return Self{EValue(0x0400)}; }	// TODO
		NdCx__ static Self  LargestNormal ()				__NE___	{ return Self{EValue(0xFBFF)}; }	// 4 292 870 144
		NdCx__ static Self	Min ()							__NE___	{ return SmallestSubnormal(); }
		NdCx__ static Self	Max ()							__NE___	{ return LargestNormal(); }
		NdCx__ static Self  Inf ()							__NE___	{ return Self{EValue(0xFC00)}; }
		NdCx__ static Self  NaN ()							__NE___	{ return Self{EValue(0xFFFF)}; }
		NdCx__ static Self	Zero ()							__NE___	{ return Self{EValue(0)}; }			// 0.0
		NdCx__ static Self	One ()							__NE___	{ return Self{EValue(0x7C00)}; }	// 1.0
		NdCx__ static float Epsilon ()						__NE___	{ return 2.0e-10f; }
	};
//-----------------------------------------------------------------------------



	//
	// Unsigned Float 8 bit
	//
	struct UFloat8
	{
	// types
	public:
		using Self	= UFloat8;

		struct Bits
		{
			static constexpr uint	_ManBits	= 4;
			static constexpr uint	_ExpBits	= 4;
			static constexpr int	_NaNExp		= (1u << _ExpBits) - 1;
			static constexpr uint	_MidExp		= (1u << (_ExpBits-1)) - 1;		// zero in signed form
			static constexpr uint	_MaxExp		= _NaNExp - 1;
			static constexpr uint	_MaxMan		= (1u << _ManBits) - 1;
			static constexpr uint	_Signed		= false;

			ubyte	m	: _ManBits;		// mantissa bits
			ubyte	e	: _ExpBits;		// exponent	bits (-7 .. +8)
		};
		StaticAssert( sizeof(ubyte) == sizeof(Bits) );

	private:
		enum class EValue : ubyte {};
		using FC = FloatConversion;


	// variables
	private:
		Bits		_bits;


	// methods
	private:
		__Cx__ explicit UFloat8 (EValue val)			__NE___	: _bits{ BitCast<Bits>( val )} {}
		__Cx__ explicit UFloat8 (Bits bits)				__NE___	: _bits{ bits } {}

	public:
		__Cx__ UFloat8 ()								__NE___	: _bits{} {}
		__Cx__ UFloat8 (const Self &)					__NE___	= default;
		__Cx__ explicit UFloat8 (float val)				__NE___	{ Set( val ); }

		__Cx__ Self&		operator = (Self rhs)		__NE___	{ _bits = rhs._bits;  return *this; }

		NdCx__ bool			operator == (Self rhs)		C_NE___	{ return AsInteger() == rhs.AsInteger(); }

		// set/get
		NdCx__ ubyte		AsInteger ()				C_NE___	{ return BitCast<ubyte>( _bits ); }
		NdCx__ Bits			GetBits ()					C_NE___	{ return _bits; }

		template <typename T = float>
		NdCx__ T			Get ()						C_NE___	{ return FC::Convert2< FC::EMode::Round, T >( *this ); }

		template <typename T>
		__Cx__ Self&		Set (T val)					__NE___	{ FC::Convert3< FC::EMode::Round, Self >( val, OUT _bits );  return *this; }

		template <typename T = float>
		NdCx__ T			GetFast ()					C_NE___	{ return FC::Convert2< FC::EMode::RoundFast, T >( *this ); }

		template <typename T>
		__Cx__ Self&		SetFast (T val)				__NE___	{ FC::Convert3< FC::EMode::RoundFast, Self >( val, OUT _bits );  return *this; }
		
		NdCx__ int			IntExp2 ()					C_NE___	{ return int(_bits.e) - int(Bits::_MidExp); }

		NdCx__ bool			IsNaN ()					C_NE___	{ return _bits.e == Bits::_NaNExp and _bits.m != 0; }
		NdCx__ bool			IsInfinity ()				C_NE___	{ return _bits.e == Bits::_NaNExp and _bits.m == 0; }
		NdCx__ bool			IsFinite ()					C_NE___	{ return _bits.e != Bits::_NaNExp; }
		NdCx__ bool			IsNegative ()				C_NE___	{ return false; }				// same as 'std::signbit'
		NdCx__ bool			IsSubnormal ()				C_NE___	{ return _bits.e == 0 and _bits.m != 0; }
		NdCx__ bool			IsZero ()					C_NE___	{ return _bits.e == 0 and _bits.m == 0; }
		
		NdCx__ Self			MinDelta ()					C_NE___	{ return Self{ FloatConversion::MinDelta( _bits )}; }	// ULP

		NdCx__ explicit operator float ()				C_NE___	{ return Get<float>(); }
		NdCx__ explicit operator double ()				C_NE___	{ return Get<double>(); }
		NdCx__ explicit operator SFloat16 ()			C_NE___	{ return Get<SFloat16>(); }
		NdCx__ explicit operator UFloat16 ()			C_NE___	{ return Get<UFloat16>(); }

		NdCx__ static Self  SmallestSubnormal ()		__NE___	{ return Self{EValue(0x01)}; }	// 1.5e-5
		NdCx__ static Self  SmallestNormal ()			__NE___	{ return Self{EValue(0x10)}; }	// TODO
		NdCx__ static Self  LargestNormal ()			__NE___	{ return Self{EValue(0xEF)}; }	// 248
		NdCx__ static Self	Min ()						__NE___	{ return SmallestSubnormal(); }
		NdCx__ static Self	Max ()						__NE___	{ return LargestNormal(); }
		NdCx__ static Self  Inf ()						__NE___	{ return Self{EValue(0xF0)}; }
		NdCx__ static Self  NaN ()						__NE___	{ return Self{EValue(0xFF)}; }
		NdCx__ static Self	Zero ()						__NE___	{ return Self{EValue(0)}; }		// 0.0
		NdCx__ static Self	One ()						__NE___	{ return Self{EValue(0x70)}; }	// 1.0
		NdCx__ static float Epsilon ()					__NE___	{ return 2.0e-3f; }
	};
//-----------------------------------------------------------------------------



	//
	// Brain floating point (IEEE 754)
	//
	struct BFloat16
	{
	// types
	public:
		using Self	= BFloat16;

		struct Bits
		{
			static constexpr uint	_ManBits	= 16 - Float32Bits::_ExpBits - 1;
			static constexpr uint	_ExpBits	= Float32Bits::_ExpBits;
			static constexpr uint	_NaNExp		= (1u << _ExpBits) - 1;
			static constexpr uint	_MidExp		= (1u << (_ExpBits-1)) - 1;		// zero in signed form
			static constexpr uint	_MaxExp		= _NaNExp - 1;
			static constexpr uint	_MaxMan		= (1u << _ManBits) - 1;
			static constexpr uint	_Signed		= true;

			ushort	m	: _ManBits;		// mantissa bits
			ushort	e	: _ExpBits;		// exponent	bits (-127 .. +128)
			ushort	s	: 1;			// sign bits
		};
		StaticAssert( sizeof(Bits) == sizeof(ushort) );

	private:
		enum class EValue : ushort {};
		using FC = FloatConversion;


	// variables
	public:
		Bits		_bits;


	// methods
	private:
		__Cx__ explicit BFloat16 (EValue val)				__NE___	: _bits{ BitCast<Bits>( val )} {}
		__Cx__ explicit BFloat16 (Bits bits)				__NE___	: _bits{ bits } {}

	public:
		__Cx__ BFloat16 ()									__NE___	: _bits{} {}
		__Cx__ BFloat16 (const Self &other)					__NE___	= default;
		__Cx__ explicit BFloat16 (float f)					__NE___ { Set( f ); }

		__Cx__ Self&		operator =  (const Self &rhs)	__NE___	= default;

		NdCx__ bool			operator == (const Self &rhs)	C_NE___	{ return AsInteger() == rhs.AsInteger(); }

		// set/get
		NdCx__ ushort		AsInteger ()					C_NE___	{ return BitCast<ushort>( _bits ); }
		NdCx__ Bits			GetBits ()						C_NE___	{ return _bits; }

		template <typename T = float>
		NdCx__ T			Get ()							C_NE___	{ return FC::Convert2< FC::EMode::Round, T >( *this ); }

		template <typename T>
		__Cx__ Self&		Set (T val)						__NE___	{ FC::Convert3< FC::EMode::Round, Self >( val, OUT _bits );  return *this; }

		template <typename T = float>
		NdCx__ T			GetFast ()						C_NE___	{ return FC::Convert2< FC::EMode::RoundFast, T >( *this ); }

		template <typename T>
		__Cx__ Self&		SetFast (T val)					__NE___	{ FC::Convert3< FC::EMode::RoundFast, Self >( val, OUT _bits );  return *this; }
		
		NdCx__ int			IntExp2 ()						C_NE___	{ return int(_bits.e) - int(Bits::_MidExp); }

		NdCx__ bool			IsNaN ()						C_NE___	{ return _bits.e == Bits::_NaNExp and _bits.m != 0; }
		NdCx__ bool			IsInfinity ()					C_NE___	{ return _bits.e == Bits::_NaNExp and _bits.m == 0; }
		NdCx__ bool			IsFinite ()						C_NE___	{ return _bits.e != Bits::_NaNExp; }
		NdCx__ bool			IsNegative ()					C_NE___	{ return _bits.s == 1; }			// same as 'std::signbit'
		NdCx__ bool			IsSubnormal ()					C_NE___	{ return _bits.e == 0 and _bits.m != 0; }
		NdCx__ bool			IsZero ()						C_NE___	{ return _bits.e == 0 and _bits.m == 0; }
		
		NdCx__ Self			MinDelta ()						C_NE___	{ return Self{ FloatConversion::MinDelta( _bits )}; }	// ULP

		NdCx__ explicit operator float ()					C_NE___	{ return Get<float>(); }
		NdCx__ explicit operator double ()					C_NE___	{ return Get<double>(); }

		NdCx__ static Self  SmallestSubnormal ()			__NE___	{ return Self{EValue(0x0001)}; }	// 9.18e-41
		NdCx__ static Self  SmallestNormal ()				__NE___	{ return Self{EValue(0x0080)}; }	// TODO
		NdCx__ static Self  LargestNormal ()				__NE___	{ return Self{EValue(0x7F7F)}; }	// TODO
		NdCx__ static Self	Min ()							__NE___	{ return SmallestSubnormal(); }
		NdCx__ static Self	Max ()							__NE___	{ return LargestNormal(); }
		NdCx__ static Self  MaxNeg ()						__NE___	{ return Self{EValue(0xFF7F)}; }	// TODO
		NdCx__ static Self  Inf ()							__NE___	{ return Self{EValue(0x7F80)}; }
		NdCx__ static Self  NegInf ()						__NE___	{ return Self{EValue(0xFF80)}; }
		NdCx__ static Self	SignalingNaN ()					__NE___	{ return Self{EValue(0xFF81)}; }
		NdCx__ static Self	QuietNaN ()						__NE___	{ return Self{EValue(0xFFC1)}; }
		NdCx__ static Self	Zero ()							__NE___	{ return Self{EValue(0)}; }			// 0.0
		NdCx__ static Self	One ()							__NE___	{ return Self{EValue(0x3F80)}; }	// 1.0
		NdCx__ static float Epsilon ()						__NE___	{ return 2.0e-10f; }				// TODO
	};
//-----------------------------------------------------------------------------



	template <>	struct TMemCopyAvailable< SFloat16 >		: CT_True {};
	template <>	struct TZeroMemAvailable< SFloat16 >		: CT_True {};
	template <>	struct TTriviallySerializable< SFloat16 >	: CT_True {};

	template <>	struct TMemCopyAvailable< UFloat16 >		: CT_True {};
	template <>	struct TZeroMemAvailable< UFloat16 >		: CT_True {};
	template <>	struct TTriviallySerializable< UFloat16 >	: CT_True {};

	template <>	struct TMemCopyAvailable< UFloat8 >			: CT_True {};
	template <>	struct TZeroMemAvailable< UFloat8 >			: CT_True {};
	template <>	struct TTriviallySerializable< UFloat8 >	: CT_True {};

	template <>	struct TMemCopyAvailable< BFloat16 >		: CT_True {};
	template <>	struct TZeroMemAvailable< BFloat16 >		: CT_True {};
	template <>	struct TTriviallySerializable< BFloat16 >	: CT_True {};

	template <> struct TIsScalar< SFloat16 >		: CT_True {};
	template <> struct TIsFloatPoint< SFloat16 >	: CT_True {};
	template <> struct TIsSigned< SFloat16 >		: CT_True {};
	template <> struct TIsUnsigned< SFloat16 >		: CT_False {};

	template <> struct TIsScalar< UFloat16 >		: CT_True {};
	template <> struct TIsFloatPoint< UFloat16 >	: CT_True {};
	template <> struct TIsSigned< UFloat16 >		: CT_False {};
	template <> struct TIsUnsigned< UFloat16 >		: CT_True {};

	template <> struct TIsScalar< BFloat16 >		: CT_True {};
	template <> struct TIsFloatPoint< BFloat16 >	: CT_True {};
	template <> struct TIsSigned< BFloat16 >		: CT_True {};
	template <> struct TIsUnsigned< BFloat16 >		: CT_False {};

  #if AE_SIMD_NEON_HALF
	template <> struct TIsScalar< float16_t >		: CT_True {};
	template <> struct TIsFloatPoint< float16_t >	: CT_True {};
	template <> struct TIsSigned< float16_t >		: CT_True {};
	template <> struct TIsUnsigned< float16_t >		: CT_False {};
  #endif

	template <> struct TIsScalar< UFloat8 >			: CT_True  {};
	template <> struct TIsFloatPoint< UFloat8 >		: CT_True  {};
	template <> struct TIsSigned< UFloat8 >			: CT_False {};
	template <> struct TIsUnsigned< UFloat8 >		: CT_True  {};

/*
=================================================
	IsInfinity / IsNaN / IsFinite
=================================================
*/
	Nd__In bool  IsInfinity (const SFloat16 x)	__NE___	{ return x.IsInfinity(); }
	Nd__In bool  IsNaN (const SFloat16 x)		__NE___	{ return x.IsNaN(); }
	Nd__In bool  IsFinite (const SFloat16 x)	__NE___ { return x.IsFinite(); }

	Nd__In bool  IsInfinity (const UFloat16 x)	__NE___	{ return x.IsInfinity(); }
	Nd__In bool  IsNaN (const UFloat16 x)		__NE___	{ return x.IsNaN(); }
	Nd__In bool  IsFinite (const UFloat16 x)	__NE___ { return x.IsFinite(); }

	Nd__In bool  IsInfinity (const UFloat8 x)	__NE___	{ return x.IsInfinity(); }
	Nd__In bool  IsNaN (const UFloat8 x)		__NE___	{ return x.IsNaN(); }
	Nd__In bool  IsFinite (const UFloat8 x)		__NE___ { return x.IsFinite(); }

	Nd__In bool  IsInfinity (const BFloat16 x)	__NE___	{ return x.IsInfinity(); }
	Nd__In bool  IsNaN (const BFloat16 x)		__NE___	{ return x.IsNaN(); }
	Nd__In bool  IsFinite (const BFloat16 x)	__NE___ { return x.IsFinite(); }

/*
=================================================
	BitsForType
=================================================
*/
	template <>	struct FloatConversion::BitsForType<float>		{ using type = Float32Bits; };
	template <>	struct FloatConversion::BitsForType<double>		{ using type = Float64Bits; };
	template <>	struct FloatConversion::BitsForType<SFloat16>	{ using type = SFloat16::Bits; };
	template <>	struct FloatConversion::BitsForType<UFloat16>	{ using type = UFloat16::Bits; };
	template <>	struct FloatConversion::BitsForType<UFloat8>	{ using type = UFloat8::Bits; };
	template <>	struct FloatConversion::BitsForType<BFloat16>	{ using type = BFloat16::Bits; };

/*
=================================================
	ConvertBits_Accurate
=================================================
*/
	template <FloatConversion::EMode Mode, typename SrcBits, typename DstBits>
	__CxIF void  FloatConversion::ConvertBits_Accurate (const SrcBits src, OUT DstBits &dst) __NE___
	{
		StaticAssert( AllBits( Mode, EMode::CheckNanInf ));
		StaticAssert( AllBits( Mode, EMode::CheckOverflow ));

		using U = Conditional< (sizeof(SrcBits) > 4 or sizeof(DstBits) > 4), ulong, uint >;

		// sign
		if constexpr( SrcBits::_Signed and DstBits::_Signed ){
			dst.s = src.s;
		}else
		if constexpr( SrcBits::_Signed ){
			if ( src.s == 1 ){  // negative to zero
				dst.e = 0;
				dst.m = 0;
				return;
			}
		}else
		if constexpr( DstBits::_Signed ){
			dst.s = 0;
		}

		// zero
		if ( src.e == 0 and src.m == 0 )
		{
			dst.e = 0;
			dst.m = 0;
			return;
		}

		const bool	src_nan	= src.e == SrcBits::_NaNExp;

		U		m = src.m;
		int		e = int(src.e) - int(SrcBits::_MidExp) + int(DstBits::_MidExp);

		// decrease precision
		if constexpr( SrcBits::_ManBits >= DstBits::_ManBits )
		{
			constexpr uint	ManBitsDelta = SrcBits::_ManBits - DstBits::_ManBits;

			if constexpr( AllBits( Mode, EMode::AllowDenorm ))
			{
				// denormalized
				if_unlikely( e <= 0 and e >= -int(DstBits::_ManBits) )
				{
					m = (m | (U{1} << SrcBits::_ManBits)) >> (1 - e);
					m += (m & (U{1} << (ManBitsDelta-1))) << 1;	// round

					dst.e = 0; //m >> DstBits::_ManBits;	// 1 bit on mantissa overflow
					dst.m = m;
					return;
				}
			}

			// exponent overflow, convert to zero
			if_unlikely( e < 0 )
			{
				dst.e = 0;
				dst.m = 0;
				return;
			}

			// NaN / Inf
			if_unlikely( src_nan )
			{
				m >>= ManBitsDelta;
				m |=  (m == 0 and src.m != 0);  // don't lost NaN state

				dst.e = DstBits::_NaNExp;
				dst.m = m;
				return;
			}

			// normalized

			if constexpr( AllBits( Mode, EMode::RoundToNearest ))
			{
				m += (m & (U{1} << (ManBitsDelta-1))) << 1;		// round
				if_unlikely( m & (U{1} << SrcBits::_ManBits) )	// overflow
				{
					m =  0;
					e += 1;
				}
			}

			// exponent overflow, convert to inf
			if_unlikely( e >= int(DstBits::_NaNExp) )
			{
				dst.e = DstBits::_NaNExp;
				dst.m = 0;
				return;
			}

			dst.e = e;
			dst.m = m >> ManBitsDelta;
		}
		else
		// increase precision
		{
			constexpr uint	ManBitsDelta = DstBits::_ManBits - SrcBits::_ManBits;

			// normalize
			if constexpr( AllBits( Mode, EMode::Normalize | EMode::AllowDenorm ))
			{
				if_unlikely( src.e == 0 and e > 0 )
				{
					for (; not (m & (U{1} << SrcBits::_ManBits)) and e >= 0; )
					{
						m <<= 1;
						e -=  1;
					}
					e += 1;
					//m &= ~(U{1} << SrcBits::_ManBits);	// not needed for bitfield
				}
				// keep denorm on exponent overflow
			}else
			if constexpr( AllBits( Mode, EMode::Normalize ))
			{
				if_unlikely( src.e == 0 )
				{
					for (; not (m & (U{1} << SrcBits::_ManBits)); )
					{
						m <<= 1;
						e -=  1;
					}
					e += 1;
					//m &= ~(U{1} << SrcBits::_ManBits);	// not needed for bitfield
				}
				// negative exponent will be converted to zero later
			}

			// exponent overflow, convert to zero
			if_unlikely( e < 0 )
			{
				dst.e = 0;
				dst.m = 0;
				return;
			}

			// exponent overflow, convert to inf
			if_unlikely( e >= int(DstBits::_NaNExp) )
			{
				dst.e = DstBits::_NaNExp;
				dst.m = 0;
				return;
			}

			dst.e = src_nan ? DstBits::_NaNExp : e;
			dst.m = m << ManBitsDelta;
		}
	}

/*
=================================================
	ConvertBits_Fast
=================================================
*/
	template <FloatConversion::EMode Mode, typename SrcBits, typename DstBits, typename SrcU, typename DstU>
	__CxIF void  FloatConversion::ConvertBits_Fast (const SrcU src, OUT DstU &dst) __NE___
	{
		StaticAssert( AllBits( Mode, EMode::CheckNanInf ));

		StaticAssert( NoBits( Mode, EMode::CheckOverflow ));
		StaticAssert( NoBits( Mode, EMode::Normalize ));
		StaticAssert( NoBits( Mode, EMode::AllowDenorm ));

		constexpr auto	SrcMan		= SrcBits::_ManBits;
		constexpr auto	SrcExp		= SrcBits::_ExpBits;

		constexpr auto	DstMan		= DstBits::_ManBits;
		constexpr auto	DstExp		= DstBits::_ExpBits;

		constexpr SrcU	ZeroMask	= SrcBits::_Signed ?
											SrcU{UMax} & ~(SrcU{1} << (SrcExp + SrcMan)) :
											SrcU{UMax};

		// decrease precision
		if constexpr( SrcMan >= DstMan )
		{
			using U = Conditional< (sizeof(SrcBits) > 4 or sizeof(DstBits) > 4), ulong, uint >;

			constexpr uint	ManBitsDelta = SrcMan - DstMan;

			const auto	src_e	= (src >> SrcMan) & ((SrcU{1} << SrcExp) - 1);
			U			m		= src & ((U{1} << SrcMan) - 1);
			int			e		= int(src_e) - int(SrcBits::_MidExp) + int(DstBits::_MidExp);

			if constexpr( AllBits( Mode, EMode::RoundToNearest ))
			{
				m += (m & (U{1} << (ManBitsDelta-1))) << 1;		// round
				if_unlikely( m & (U{1} << SrcBits::_ManBits) )	// overflow
				{
					m =  0;
					e += 1;
				}
			}
			m = (m >> ManBitsDelta) & ((U{1} << DstMan) - 1);

			const DstU	branches[] = {
				DstU( (std::max( e, 0 ) << DstMan)	| m ),	// default
				DstU( (DstBits::_NaNExp << DstMan)	| m ), 	// nan/inf
				0											// zero
			};

			// branchless
			dst = branches[ uint{src_e == SrcBits::_NaNExp} | (uint{(src & ZeroMask) == 0} << 1) ];

			if constexpr( SrcBits::_Signed and DstBits::_Signed )
			{
				constexpr SrcU	SignMask = SrcU{1} << (SrcExp + SrcMan);
				constexpr auto	SignOff  = (SrcExp + SrcMan) - (DstExp + DstMan);

				dst |= (src & SignMask) >> SignOff;
			}
		}
		else
		// increase precision
		{
			StaticAssert( DstExp >= SrcExp );

			constexpr uint	ManBitsDelta = DstMan - SrcMan;

			const auto	src_e	= (src >> SrcMan) & ((SrcU{1} << SrcExp) - 1);
			const auto	dst_e	= (src_e + (DstBits::_MidExp - SrcBits::_MidExp)) & ((DstU{1} << DstExp) - 1);
			const DstU	dst_m	= DstU( src & ((SrcU{1} << SrcMan) - 1) ) << ManBitsDelta;
			const DstU	branches[] = {
				(dst_e << DstMan)			 | dst_m,	// default
				(DstBits::_NaNExp << DstMan) | dst_m, 	// nan/inf
				0										// zero
			};

			// branchless
			dst = branches[ uint{src_e == SrcBits::_NaNExp} | (uint{(src & ZeroMask) == 0} << 1) ];

			if constexpr( SrcBits::_Signed and DstBits::_Signed )
			{
				constexpr SrcU	SignMask = SrcU{1} << (SrcExp + SrcMan);
				constexpr auto	SignOff  = (DstExp + DstMan) - (SrcExp + SrcMan);

				dst |= (src & SignMask) << SignOff;
			}
		}
	}

/*
=================================================
	Convert
=================================================
*/
	template <FloatConversion::EMode Mode, typename Src, typename Dst>
	__CxIF void  FloatConversion::Convert (const Src src, OUT Dst &dst) __NE___
	{
		StaticAssert( IsAnyFloatPoint< Src >);
		StaticAssert( IsAnyFloatPoint< Dst >);

		using SrcBits	= typename BitsForType<Src>::type;
		using DstBits	= typename BitsForType<Dst>::type;
		using SrcU		= ToUnsignedInteger< SrcBits >;
		using DstU		= ToUnsignedInteger< DstBits >;

		if constexpr( AnyBits( Mode, EMode::Normalize | EMode::AllowDenorm ))
		{
			DstBits	dst_bits;
			ConvertBits_Accurate< Mode >( BitCast<SrcBits>(src), OUT dst_bits );
			dst = BitCast<Dst>( dst_bits );

			if constexpr( AllBits( Mode, EMode::CheckNanInf ))
			{
				ASSERT_Cx( IsNaN(src) ? IsNaN(dst) : true );
				ASSERT_Cx( IsInfinity(src) ? IsInfinity(dst) : true );
			}
		}
		else
		{
			DstU	dst_u;
			ConvertBits_Fast< Mode, SrcBits, DstBits >( BitCast<SrcU>(src), OUT dst_u );
			dst = BitCast<Dst>( dst_u );
			
			if constexpr( AllBits( Mode, EMode::CheckNanInf ))
			{
				// TODO: may loose mantissa bit and NaN converted to Inf
				ASSERT_Cx( (IsNaN(src) or IsInfinity(src)) ? (IsNaN(src) or IsInfinity(dst)) : true );
			}
		}

		if constexpr( SrcBits::_Signed and DstBits::_Signed )
		{
			if constexpr( IsClass<Src> and IsClass<Dst> ){
				ASSERT_Cx( src.IsNegative() == dst.IsNegative() );
			}else
			if constexpr( IsClass<Src> ){
				ASSERT_Cx( src.IsNegative() == std::signbit(dst) );
			}else
			if constexpr( IsClass<Dst> ){
				ASSERT_Cx( std::signbit(src) == dst.IsNegative() );
			}
		}
	}
	
/*
=================================================
	MinDelta
=================================================
*/
	template <typename Bits>
	__CxIF Bits  FloatConversion::MinDelta (const Bits cur) __NE___
	{
		int		e	= int(cur.e) - Bits::_ManBits;
		Bits	ulp;
		
		if constexpr( Bits::_Signed )
			ulp.s = cur.s;

		if ( cur.m & 1 )
		{
			ulp.m = Bits::_MaxMan;
			--e;
		}
		ulp.e = std::clamp( e, 0, int(Bits::_MaxExp) );

		// denormal
		if ( e <= 0 )
		{
			using U = Base::ByteSizeToUInt< sizeof(Bits) >;
			e = int(Bits::_ManBits-1) + e;
			ulp.m = e >= 0 ? U{1} << e : U{0};
		}
		return ulp;
	}
	
/*
=================================================
	Next
=================================================
*/
	template <typename Bits>
	__CxIF Bits  FloatConversion::Next (Bits b) __NE___
	{
		auto	m = b.m + 1;
		auto	e = b.e;

		if ( m > Bits::_MaxMan )
		{
			m = 0;
			++e;
		}

		b.m = m;
		b.e = e;
		return b;
	}

} // AE::Base
//-----------------------------------------------------------------------------


template <>
class std::numeric_limits< AE::Base::SFloat16 > final
{
public:
	static constexpr bool	is_signed		= true;
	static constexpr bool	is_specialized	= true;
	static constexpr int	radix			= 2;
	static constexpr int	digits			= 11;
	static constexpr int	digits10		= 3;	// TODO: check
	static constexpr int	max_digits10	= 1;	// TODO: check
	static constexpr int	max_exponent	= 16;
	static constexpr int	max_exponent10	= 1;	// TODO: check
	static constexpr int	min_exponent	= -15;
	static constexpr int	min_exponent10	= -1;	// TODO: check

	NdCx__ static AE::Base::SFloat16  min () __NE___ { return AE::Base::SFloat16::Min(); }
	NdCx__ static AE::Base::SFloat16  max () __NE___ { return AE::Base::SFloat16::Max(); }
};

template <>
class std::numeric_limits< AE::Base::UFloat16 > final
{
public:
	static constexpr bool	is_signed		= false;
	static constexpr bool	is_specialized	= true;
	static constexpr int	radix			= 2;
	static constexpr int	digits			= 11;
	static constexpr int	digits10		= 3;	// TODO: check
	static constexpr int	max_digits10	= 1;	// TODO: check
	static constexpr int	max_exponent	= 16;
	static constexpr int	max_exponent10	= 1;	// TODO: check
	static constexpr int	min_exponent	= -15;
	static constexpr int	min_exponent10	= -1;	// TODO: check

	NdCx__ static AE::Base::UFloat16  min () __NE___ { return AE::Base::UFloat16::Min(); }
	NdCx__ static AE::Base::UFloat16  max () __NE___ { return AE::Base::UFloat16::Max(); }
};

template <>
class std::numeric_limits< AE::Base::UFloat8 > final
{
public:
	static constexpr bool	is_signed		= false;
	static constexpr bool	is_specialized	= true;
	static constexpr int	radix			= 2;
	static constexpr int	digits			= 4;	// TODO: check
	static constexpr int	digits10		= 3;	// TODO: check
	static constexpr int	max_digits10	= 1;	// TODO: check
	static constexpr int	max_exponent	= 7;
	static constexpr int	max_exponent10	= 1;	// TODO: check
	static constexpr int	min_exponent	= -7;
	static constexpr int	min_exponent10	= -1;	// TODO: check

	NdCx__ static AE::Base::UFloat8  min () __NE___ { return AE::Base::UFloat8::Min(); }
	NdCx__ static AE::Base::UFloat8  max () __NE___ { return AE::Base::UFloat8::Max(); }
};

template <>
class std::numeric_limits< AE::Base::BFloat16 > final
{
public:
	static constexpr bool	is_signed		= true;
	static constexpr bool	is_specialized	= true;
	static constexpr int	radix			= 2;	// TODO: check
	static constexpr int	digits			= 11;	// TODO: check
	static constexpr int	digits10		= 3;	// TODO: check
	static constexpr int	max_digits10	= 1;	// TODO: check
	static constexpr int	max_exponent	= 16;	// TODO: check
	static constexpr int	max_exponent10	= 1;	// TODO: check
	static constexpr int	min_exponent	= -15;	// TODO: check
	static constexpr int	min_exponent10	= -1;	// TODO: check

	NdCx__ static AE::Base::BFloat16  min () __NE___ { return AE::Base::BFloat16::Min(); }
	NdCx__ static AE::Base::BFloat16  max () __NE___ { return AE::Base::BFloat16::Max(); }
};
