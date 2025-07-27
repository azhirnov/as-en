// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Range.h"

namespace AE::Base
{

	template <typename T>
	struct Rectangle
	{
		StaticAssert( IsScalar<T> );

	// types
	public:
		using Vec2_t	= Vec<T,2>;
		using Vec4_t	= Vec<T,4>;
		using Value_t	= T;
		using Self		= Rectangle<T>;


	// variables
	public:
		T	left, top, right, bottom;


	// methods
	public:
		__Cx__ Rectangle ()										__NE___ :
			left{T{0}}, top{T{0}}, right{T{0}}, bottom{T{0}}
		{
		  #ifdef AE_COMPILETIME_OFFSETOF
			// check if supported cast Rectangle to array
			StaticAssert( offsetof(Self, left)  + sizeof(T) == offsetof(Self, top) );
			StaticAssert( offsetof(Self, top)   + sizeof(T) == offsetof(Self, right) );
			StaticAssert( offsetof(Self, right) + sizeof(T) == offsetof(Self, bottom) );
			StaticAssert( sizeof(T[3]) == (offsetof(Self, bottom) - offsetof(Self, left)) );
		  #endif
		}

		__Cx__ Rectangle (T left, T top, T right, T bottom)		__NE___ :
			left{left}, top{top}, right{right}, bottom{bottom} {}

		Rectangle (const Vec2_t &leftTop, const Vec2_t &rightBottom) :
			left{leftTop.x}, top{leftTop.y}, right{rightBottom.x}, bottom{rightBottom.y} {}

		explicit Rectangle (const Vec2_t &size)					__NE___ :
			Rectangle{ Vec2_t{}, size } {}

		__Cx__ Rectangle (const Self &other)					__NE___ :
			left{other.left}, top{other.top}, right{other.right}, bottom{other.bottom} {}

		template <typename B>
		__Cx__ explicit Rectangle (const Rectangle<B> &other)	__NE___ :
			left{T(other.left)}, top{T(other.top)}, right{T(other.right)}, bottom{T(other.bottom)} {}

		__Cx__ Rectangle (const Range<T> &x, const Range<T> &y)	__NE___ :
			left{x.begin}, top{y.begin}, right{x.end}, bottom{y.end} {}

		Nd____ const Vec2_t	LeftTop ()							C_NE___	{ return { left, top }; }
		Nd____ const Vec2_t	RightBottom ()						C_NE___	{ return { right, bottom }; }
		Nd____ const Vec2_t	LeftBottom ()						C_NE___	{ return { left, bottom }; }
		Nd____ const Vec2_t	RightTop ()							C_NE___	{ return { right, top }; }

		Nd____ const Vec2_t	Min ()								C_NE___	{ return LeftTop(); }
		Nd____ const Vec2_t	Max ()								C_NE___	{ return RightBottom(); }

		Nd____ const Vec2_t	Size ()								C_NE___	{ return { Width(), Height() }; }
		Nd____ const Vec2_t	Center ()							C_NE___	{ return { CenterX(), CenterY() }; }
		Nd____ const Vec2_t	Offset ()							C_NE___	{ return LeftTop(); }

		NdCx__ Range<T>		XRange ()							C_NE___	{ return Range<T>{ left, right }; }
		NdCx__ Range<T>		YRange ()							C_NE___	{ return Range<T>{ top, bottom }; }

		NdCx__ static Self	MaxSize ()							__NE___	{ return Self{ MinValue<T>(), MinValue<T>(), MaxValue<T>(), MaxValue<T>() }; }

		Nd____ explicit operator Vec4_t ()						C_NE___	{ return Vec4_t{left, top, right, bottom}; }

		NdCx__ const T		Width ()							C_NE___	{ return right - left; }
		NdCx__ const T		Height ()							C_NE___	{ return bottom - top; }
		NdCx__ const T		CenterX ()							C_NE___	{ return Average( right, left ); }
		NdCx__ const T		CenterY ()							C_NE___	{ return Average( top, bottom ); }

		Nd____ T const*		data ()								C_NE___	{ return std::addressof( left ); }
		Nd____ T *			data ()								__NE___	{ return std::addressof( left ); }

		NdCx__ bool			IsEmpty ()							C_NE___	{ return Equal( left, right ) or Equal( top, bottom ); }
		NdCx__ bool			IsInvalid ()						C_NE___	{ return (right < left) or (bottom < top); }
		NdCx__ bool			IsValid ()							C_NE___	{ return (not IsEmpty()) and (not IsInvalid()); }

		NdCx__ bool			IsNormalized ()						C_NE___;
		__Cx__ Self&		Normalize ()						__NE___;

		Nd____ bool			Intersects (const Vec2_t &point)	C_NE___;
		NdCx__ bool			Intersects (const Self &other)		C_NE___;

		NdCx__ Self			Intersection (const Self &other)	C_NE___;

		Nd____ bool4		operator == (const Self &rhs)		C_NE___;
		Nd____ bool4		operator != (const Self &rhs)		C_NE___;

		__Cx__ Self&		operator = (const Self &rhs)		__NE___ = default;

		__Cx__ Self&		LeftTop (const Vec2_t& v)			__NE___;
		__Cx__ Self&		RightBottom (const Vec2_t& v)		__NE___;

		__Cx__ Self&		Join (const Self &other)			__NE___;
		__Cx__ Self&		Join (const Vec2_t &point)			__NE___;

		__Cx__ Self&		Stretch (const Self &size)			__NE___;
		__Cx__ Self&		Stretch (const Vec2_t &size)		__NE___;
		__Cx__ Self&		Stretch (T size)					__NE___	{ return Stretch( Vec2_t{ size }); }

		__Cx__ Self&		Stretch2 (const Vec2_t &size)		__NE___;
		__Cx__ Self&		Stretch2 (T size)					__NE___	{ return Stretch2( Vec2_t{ size }); }

		NdCx__ Self			Scale (T scale)						C_NE___;
	};


	using RectU		= Rectangle< uint >;
	using RectI		= Rectangle< int >;
	using RectF		= Rectangle< float >;


/*
=================================================
	LeftTop / RightBottom
=================================================
*/
	template <typename T>
	__Cx__ Rectangle<T>&  Rectangle<T>::LeftTop (const Vec2_t& v) __NE___
	{
		left = v.x;
		top  = v.y;
		return *this;
	}

	template <typename T>
	__Cx__ Rectangle<T>&  Rectangle<T>::RightBottom (const Vec2_t& v) __NE___
	{
		right  = v.x;
		bottom = v.y;
		return *this;
	}

/*
=================================================
	operator +
=================================================
*/
	template <typename T>
	Rectangle<T>&  operator += (Rectangle<T> &lhs, const Vec<T,2> &rhs) __NE___
	{
		lhs.left += rhs.x;	lhs.right  += rhs.x;
		lhs.top  += rhs.y;	lhs.bottom += rhs.y;
		return lhs;
	}

	template <typename T>
	Rectangle<T>  operator + (const Rectangle<T> &lhs, const Vec<T,2> &rhs) __NE___
	{
		return Rectangle<T>{ lhs.left  + rhs.x, lhs.top    + rhs.y,
							 lhs.right + rhs.x, lhs.bottom + rhs.y };
	}

	template <typename T>
	Rectangle<T>&  operator += (Rectangle<T> &lhs, const T &rhs) __NE___
	{
		return lhs += Vec<T,2>{rhs};
	}

	template <typename T>
	Rectangle<T>  operator + (const Rectangle<T> &lhs, const T &rhs) __NE___
	{
		return lhs + Vec<T,2>{rhs};
	}

/*
=================================================
	operator -
=================================================
*/
	template <typename T>
	Rectangle<T>&  operator -= (Rectangle<T> &lhs, const Vec<T,2> &rhs) __NE___
	{
		lhs.left -= rhs.x;	lhs.right  -= rhs.x;
		lhs.top  -= rhs.y;	lhs.bottom -= rhs.y;
		return lhs;
	}

	template <typename T>
	Rectangle<T>  operator - (const Rectangle<T> &lhs, const Vec<T,2> &rhs) __NE___
	{
		return Rectangle<T>{ lhs.left  - rhs.x, lhs.top    - rhs.y,
							 lhs.right - rhs.x, lhs.bottom - rhs.y };
	}

	template <typename T>
	Rectangle<T>&  operator -= (Rectangle<T> &lhs, const T &rhs) __NE___
	{
		return lhs -= Vec<T,2>{rhs};
	}

	template <typename T>
	Rectangle<T>  operator - (const Rectangle<T> &lhs, const T &rhs) __NE___
	{
		return lhs - Vec<T,2>{rhs};
	}

/*
=================================================
	operator *
=================================================
*/
	template <typename T>
	Rectangle<T>&  operator *= (Rectangle<T> &lhs, const Vec<T,2> &rhs) __NE___
	{
		lhs.left *= rhs.x;	lhs.right  *= rhs.x;
		lhs.top  *= rhs.y;	lhs.bottom *= rhs.y;
		return lhs;
	}

	template <typename T>
	Rectangle<T>  operator * (const Rectangle<T> &lhs, const Vec<T,2> &rhs) __NE___
	{
		return Rectangle<T>{ lhs.left  * rhs.x, lhs.top    * rhs.y,
							 lhs.right * rhs.x, lhs.bottom * rhs.y };
	}

	template <typename T>
	Rectangle<T>&  operator *= (Rectangle<T> &lhs, const T &rhs) __NE___
	{
		return lhs *= Vec<T,2>{rhs};
	}

	template <typename T>
	Rectangle<T>  operator * (const Rectangle<T> &lhs, const T &rhs) __NE___
	{
		return lhs * Vec<T,2>{rhs};
	}

/*
=================================================
	operator /
=================================================
*/
	template <typename T>
	Rectangle<T>&  operator /= (Rectangle<T> &lhs, const Vec<T,2> &rhs) __NE___
	{
		lhs.left /= rhs.x;	lhs.right  /= rhs.x;
		lhs.top  /= rhs.y;	lhs.bottom /= rhs.y;
		return lhs;
	}

	template <typename T>
	Rectangle<T>  operator / (const Rectangle<T> &lhs, const Vec<T,2> &rhs) __NE___
	{
		return Rectangle<T>{ lhs.left  / rhs.x, lhs.top    / rhs.y,
							 lhs.right / rhs.x, lhs.bottom / rhs.y };
	}

	template <typename T>
	Rectangle<T>&  operator /= (Rectangle<T> &lhs, const T &rhs) __NE___
	{
		return lhs /= Vec<T,2>{rhs};
	}

	template <typename T>
	Rectangle<T>  operator / (const Rectangle<T> &lhs, const T &rhs) __NE___
	{
		return lhs / Vec<T,2>{rhs};
	}

/*
=================================================
	IsNormalized
=================================================
*/
	template <typename T>
	__Cx__ bool  Rectangle<T>::IsNormalized () C_NE___
	{
		return (left <= right) and (top <= bottom);
	}

/*
=================================================
	Normalize
=================================================
*/
	template <typename T>
	__Cx__ Rectangle<T>&  Rectangle<T>::Normalize () __NE___
	{
		if ( left > right )	std::swap( left, right );
		if ( top > bottom )	std::swap( top, bottom );
		return *this;
	}

/*
=================================================
	Intersects
=================================================
*/
	template <typename T>
	bool  Rectangle<T>::Intersects (const Vec2_t &point) C_NE___
	{
		return (point.x >= left) and (point.x < right) and (point.y >= top) and (point.y < bottom);
	}

	template <typename T>
	__Cx__ bool  Rectangle<T>::Intersects (const Self &other) C_NE___
	{
		return	((left < other.right) and (right > other.left) and (bottom > other.top) and (top < other.bottom)) or
				((other.right < left) and (other.left > right) and (other.top > bottom) and (other.bottom < top));
	}

/*
=================================================
	operator ==
=================================================
*/
	template <typename T>
	bool4  Rectangle<T>::operator == (const Self &rhs) C_NE___
	{
		return { left == rhs.left, top == rhs.top, right == rhs.right, bottom == rhs.bottom };
	}

	template <typename T>
	bool4  Rectangle<T>::operator != (const Self &rhs) C_NE___
	{
		return { left != rhs.left, top != rhs.top, right != rhs.right, bottom != rhs.bottom };
	}

/*
=================================================
	Intersection
=================================================
*/
	template <typename T>
	__Cx__ Rectangle<T>  Rectangle<T>::Intersection (const Self &other) C_NE___
	{
		Rectangle<T>	res;
		res.left	= Base::Max( left,   other.left   );
		res.top		= Base::Max( top,    other.top    );
		res.right	= Base::Min( right,  other.right  );
		res.bottom	= Base::Min( bottom, other.bottom );
		return res;
	}

/*
=================================================
	Join
=================================================
*/
	template <typename T>
	__Cx__ Rectangle<T>&  Rectangle<T>::Join (const Self &other) __NE___
	{
		left	= Base::Min( left,   other.left   );
		top		= Base::Min( top,    other.top    );
		right	= Base::Max( right,  other.right  );
		bottom	= Base::Max( bottom, other.bottom );
		return *this;
	}

	template <typename T>
	__Cx__ Rectangle<T>&  Rectangle<T>::Join (const Vec2_t &point) __NE___
	{
		left	= Base::Min( left,   point.x );
		top		= Base::Min( top,    point.y );
		right	= Base::Max( right,  point.x );
		bottom	= Base::Max( bottom, point.y );
		return *this;
	}

/*
=================================================
	Stretch
=================================================
*/
	template <typename T>
	__Cx__ Rectangle<T>&  Rectangle<T>::Stretch (const Self &size) __NE___
	{
		left	-= size.left;
		top		-= size.top;
		right	+= size.right;
		bottom	+= size.bottom;
		return *this;
	}

	template <typename T>
	__Cx__ Rectangle<T>&  Rectangle<T>::Stretch (const Vec2_t &size) __NE___
	{
		if constexpr( IsInteger<T> )
			ASSERT( Any(Abs(size) > Vec2_t{T{1}}) );

		const Vec2_t  half_size = size / T(2);

		return Stretch2( half_size );
	}

/*
=================================================
	Stretch2
=================================================
*/
	template <typename T>
	__Cx__ Rectangle<T>&  Rectangle<T>::Stretch2 (const Vec2_t &size) __NE___
	{
		left	-= size.x;
		top		-= size.y;
		right	+= size.x;
		bottom	+= size.y;
		return *this;
	}

/*
=================================================
	Scale
=================================================
*/
	template <typename T>
	__Cx__ Rectangle<T>  Rectangle<T>::Scale (const T scale) C_NE___
	{
		Vec2_t	size = Size() * (scale - 1.f);
		return Self{ left - size.x, top - size.y, right + size.x, bottom + size.y };
	}

/*
=================================================
	Equal
=================================================
*/
	template <typename T>
	ND_ bool4  Equal (const Rectangle<T> &lhs, const Rectangle<T> &rhs, const T err = Epsilon<T>()) __NE___
	{
		return bool4{ Base::Equal( lhs.left,   rhs.left,   err ),
					  Base::Equal( lhs.top,    rhs.top,    err ),
					  Base::Equal( lhs.right,  rhs.right,  err ),
					  Base::Equal( lhs.bottom, rhs.bottom, err )};
	}

	template <typename T>
	ND_ bool4  Equal (const Rectangle<T> &lhs, const Rectangle<T> &rhs, const Percent err) __NE___
	{
		return bool4{ Base::Equal( lhs.left,   rhs.left,   err ),
					  Base::Equal( lhs.top,    rhs.top,    err ),
					  Base::Equal( lhs.right,  rhs.right,  err ),
					  Base::Equal( lhs.bottom, rhs.bottom, err )};
	}

/*
=================================================
	BitEqual
=================================================
*/
	template <typename T> requires(IsFloatPoint<T>)
	NdCx__ bool4  BitEqual (const Rectangle<T> &lhs, const Rectangle<T> &rhs, const EnabledBitCount bitCount) __NE___
	{
		return bool4{ Base::BitEqual( lhs.left,   rhs.left,   bitCount ),
					  Base::BitEqual( lhs.top,    rhs.top,    bitCount ),
					  Base::BitEqual( lhs.right,  rhs.right,  bitCount ),
					  Base::BitEqual( lhs.bottom, rhs.bottom, bitCount )};
	}

	template <typename T> requires(IsFloatPoint<T>)
	NdCx__ bool4  BitEqual (const Rectangle<T> &lhs, const Rectangle<T> &rhs) __NE___
	{
		return bool4{ Base::BitEqual( lhs.left,   rhs.left   ),
					  Base::BitEqual( lhs.top,    rhs.top    ),
					  Base::BitEqual( lhs.right,  rhs.right  ),
					  Base::BitEqual( lhs.bottom, rhs.bottom )};
	}
//-----------------------------------------------------------------------------


	template <typename T>	struct TMemCopyAvailable< Rectangle<T> >		: CT_Bool< IsMemCopyAvailable<T>		>{};
	template <typename T>	struct TZeroMemAvailable< Rectangle<T> >		: CT_Bool< IsZeroMemAvailable<T>		>{};
	template <typename T>	struct TTriviallySerializable< Rectangle<T> >	: CT_Bool< IsTriviallySerializable<T>	>{};
	template <typename T>	struct TUnwrap< Rectangle<T> >					: TUnwrap<T> {};

} // AE::Base


template <typename T>
struct std::hash< AE::Base::Rectangle<T> >
{
	ND_ size_t  operator () (const AE::Base::Rectangle<T> &value) C_NE___
	{
	#if AE_FAST_HASH
		return	size_t( AE::Base::HashOf( this, sizeof(*this) ));
	#else
		return	size_t(	AE::Base::HashOf( value.left )  + AE::Base::HashOf( value.bottom ) +
						AE::Base::HashOf( value.right ) + AE::Base::HashOf( value.top ));
	#endif
	}
};
