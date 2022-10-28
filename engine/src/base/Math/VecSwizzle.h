// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/GLM.h"

namespace AE::Math
{

	//
	// Vector Swizzle
	//

	struct VecSwizzle
	{
	// variables
	private:
		ushort		_value	= 0;

		static constexpr uint	_Mask = 0x7;


	// methods
	public:
		constexpr VecSwizzle () {}
		
		explicit VecSwizzle (const PackedVec<uint,4> &comp) : _value{0}
		{
			ASSERT(All( comp <= 7u ));

			_value |= (comp.x & _Mask) << 12;
			_value |= (comp.y & _Mask) << 8;
			_value |= (comp.z & _Mask) << 4;
			_value |= (comp.w & _Mask);
		}
		
		template <typename T, int I, glm::qualifier Q>
		ND_ GLM_CONSTEXPR TVec<T,4,Q>		Transform (const TVec<T,I,Q> &src) const;
		ND_ GLM_CONSTEXPR PackedVec<uint,4>	ToVec () const;

		ND_ constexpr uint		Get ()			const	{ return _value; }
		ND_ constexpr bool		IsUndefined ()	const	{ return _value == 0; }
		
		ND_ constexpr bool		operator == (VecSwizzle rhs) const	{ return _value == rhs._value; }
		ND_ constexpr bool		operator >  (VecSwizzle rhs) const	{ return _value >  rhs._value; }
		
		ND_ static constexpr VecSwizzle  VecDefault (usize size);
		
		friend constexpr VecSwizzle  operator "" _vecSwizzle (const char *str, const usize len);


	private:
		explicit constexpr VecSwizzle (ushort val) : _value{val} {}

		ND_ static constexpr uint  _CharToValue (char c);
	};

	
/*
=================================================
	_CharToValue
=================================================
*/
	constexpr uint  VecSwizzle::_CharToValue (char c)
	{
		return	((c == 'x') | (c == 'X'))	? 1 :
				((c == 'y') | (c == 'Y'))	? 2 :
				((c == 'z') | (c == 'Z'))	? 3 :
				((c == 'w') | (c == 'W'))	? 4 :
				(c == '0')					? 5 :
				((c == '1') | (c == '+'))	? 6 :
				(c == '-')					? 7 :
											  0;
	}

/*
=================================================
	operator ""
=================================================
*/
	ND_ constexpr VecSwizzle  operator "" _vecSwizzle (const char *str, const usize len)
	{
		ASSERT( len > 0 and len <= 4 );

		VecSwizzle	res;
		uint		sw = 0;

		for (usize i = 0; (i < len) and (i < 4); ++i)
		{
			const char	c = str[i];
			const uint	v = VecSwizzle::_CharToValue( c );

			ASSERT( v != 0 );	// 'c' must be X, Y, Z, W, 0, 1, +, -
			sw |= (v << (3 - i) * 4);
		}
		res._value = CheckCast<ushort>(sw);
		return res;
	}
	
/*
=================================================
	Transform
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	GLM_CONSTEXPR TVec<T,4,Q>  VecSwizzle::Transform (const TVec<T,I,Q> &src) const
	{
		T	temp [8] = {};
		for (int i = 0; i < I; ++i) temp[i+1] = src[i];
		temp[6] = T{1};
		temp[7] = T{-1};

		const PackedVec<uint,4>	sw = ToVec();
		TVec<T,4,Q>				result;

		for (int i = 0; i < 4; ++i) {
			result[i] = temp[ sw[i] ];
		}
		return result;
	}
	
/*
=================================================
	ToVec
=================================================
*/
	inline GLM_CONSTEXPR PackedVec<uint,4>  VecSwizzle::ToVec () const
	{
		return PackedVec<uint,4>{ (_value >> 12) & _Mask, (_value >> 8) & _Mask, (_value >> 4) & _Mask, _value & _Mask };
	}

/*
=================================================
	operator *
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ GLM_CONSTEXPR TVec<T,4,Q>  operator * (const TVec<T,I,Q> &lhs, VecSwizzle rhs)
	{
		return rhs.Transform( lhs );
	}
	
/*
=================================================
	VecDefault
=================================================
*/
	inline constexpr VecSwizzle  VecSwizzle::VecDefault (usize size)
	{
		switch ( size )
		{
			case 1 :	return VecSwizzle{ 0x1000 };
			case 2 :	return VecSwizzle{ 0x1200 };
			case 3 :	return VecSwizzle{ 0x1230 };
			case 4 :	return VecSwizzle{ 0x1234 };
		}
		return VecSwizzle{};
	}


} // AE::Math


namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Math::VecSwizzle >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Math::VecSwizzle >	{ static constexpr bool  value = true; };

} // AE::Base


namespace std
{
	template <>
	struct hash< AE::Math::VecSwizzle >
	{
		ND_ size_t  operator () (const AE::Math::VecSwizzle &value) const
		{
			return size_t( value.Get() );
		}
	};

} // std
