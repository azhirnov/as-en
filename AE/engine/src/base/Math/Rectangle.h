// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Range.h"

namespace AE::Math
{

    template <typename T>
    struct Rectangle
    {
        STATIC_ASSERT( IsScalar<T> );

    // types
    public:
        using Vec2_t    = Vec<T,2>;
        using Vec4_t    = Vec<T,4>;
        using Value_t   = T;
        using Self      = Rectangle<T>;


    // variables
    public:
        T   left, top, right, bottom;


    // methods
    public:
        constexpr Rectangle ()                                      __NE___ :
            left{T{0}}, top{T{0}}, right{T{0}}, bottom{T{0}}
        {
          #ifdef AE_COMPILETIME_OFFSETOF
            // check if supported cast Rectangle to array
            STATIC_ASSERT( offsetof(Self, left)  + sizeof(T) == offsetof(Self, top) );
            STATIC_ASSERT( offsetof(Self, top)   + sizeof(T) == offsetof(Self, right) );
            STATIC_ASSERT( offsetof(Self, right) + sizeof(T) == offsetof(Self, bottom) );
            STATIC_ASSERT( sizeof(T[3]) == (offsetof(Self, bottom) - offsetof(Self, left)) );
          #endif
        }

        constexpr Rectangle (T left, T top, T right, T bottom)      __NE___ :
            left{left}, top{top}, right{right}, bottom{bottom} {}

        Rectangle (const Vec2_t &leftTop, const Vec2_t &rightBottom) :
            left{leftTop.x}, top{leftTop.y}, right{rightBottom.x}, bottom{rightBottom.y} {}

        explicit Rectangle (const Vec2_t &size)                     __NE___ :
            Rectangle{ Vec2_t{}, size } {}

        constexpr Rectangle (const Self &other)                     __NE___ :
            left{other.left}, top{other.top}, right{other.right}, bottom{other.bottom} {}

        template <typename B>
        constexpr explicit Rectangle (const Rectangle<B> &other)    __NE___ :
            left{T(other.left)}, top{T(other.top)}, right{T(other.right)}, bottom{T(other.bottom)} {}

        constexpr Rectangle (const Range<T> &x, const Range<T> &y)  __NE___ :
            left{x.begin}, top{y.begin}, right{x.end}, bottom{y.end} {}

        ND_ const Vec2_t        LeftTop ()                          C_NE___ { return { left, top }; }
        ND_ const Vec2_t        RightBottom ()                      C_NE___ { return { right, bottom }; }
        ND_ const Vec2_t        LeftBottom ()                       C_NE___ { return { left, bottom }; }
        ND_ const Vec2_t        RightTop ()                         C_NE___ { return { right, top }; }

        ND_ const Vec2_t        Min ()                              C_NE___ { return LeftTop(); }
        ND_ const Vec2_t        Max ()                              C_NE___ { return RightBottom(); }

        ND_ const Vec2_t        Size ()                             C_NE___ { return { Width(), Height() }; }
        ND_ const Vec2_t        Center ()                           C_NE___ { return { CenterX(), CenterY() }; }
        ND_ const Vec2_t        Offset ()                           C_NE___ { return LeftTop(); }

        ND_ constexpr Range<T>  XRange ()                           C_NE___ { return Range<T>{ left, right }; }
        ND_ constexpr Range<T>  YRange ()                           C_NE___ { return Range<T>{ top, bottom }; }

        ND_ static constexpr Self   MaxSize ()                      __NE___ { return Self{ MinValue<T>(), MinValue<T>(), MaxValue<T>(), MaxValue<T>() }; }

        ND_ explicit operator Vec4_t ()                             C_NE___ { return Vec4_t{left, top, right, bottom}; }

        ND_ constexpr const T   Width ()                            C_NE___ { return right - left; }
        ND_ constexpr const T   Height ()                           C_NE___ { return bottom - top; }
        ND_ constexpr const T   CenterX ()                          C_NE___ { return Average( right, left ); }
        ND_ constexpr const T   CenterY ()                          C_NE___ { return Average( top, bottom ); }

        ND_ T const*            data ()                             C_NE___ { return std::addressof( left ); }
        ND_ T *                 data ()                             __NE___ { return std::addressof( left ); }

        ND_ constexpr bool      IsEmpty ()                          C_NE___ { return Equals( left, right ) | Equals( top, bottom ); }
        ND_ constexpr bool      IsInvalid ()                        C_NE___ { return (right < left) | (bottom < top); }
        ND_ constexpr bool      IsValid ()                          C_NE___ { return (not IsEmpty()) & (not IsInvalid()); }

        ND_ constexpr bool      IsNormalized ()                     C_NE___;
            constexpr Self&     Normalize ()                        __NE___;

        ND_ bool                Intersects (const Vec2_t &point)    C_NE___;
        ND_ constexpr bool      Intersects (const Self &point)      C_NE___;

        ND_ constexpr Self      Intersection (const Self &other)    C_NE___;

        ND_ bool4               operator == (const Self &rhs)       C_NE___;
        ND_ bool4               operator != (const Self &rhs)       C_NE___;

            constexpr Self&     operator = (const Self &rhs)        __NE___ = default;

            constexpr Self&     LeftTop (const Vec2_t& v)           __NE___;
            constexpr Self&     RightBottom (const Vec2_t& v)       __NE___;

            constexpr Self&     Join (const Self &other)            __NE___;
            constexpr Self&     Join (const Vec2_t &point)          __NE___;

            constexpr Self&     Stretch (const Self &size)          __NE___;
            constexpr Self&     Stretch (const Vec2_t &size)        __NE___;
            constexpr Self&     Stretch (T size)                    __NE___ { return Stretch( Vec2_t{ size }); }

            constexpr Self&     Stretch2 (const Vec2_t &size)       __NE___;
            constexpr Self&     Stretch2 (T size)                   __NE___ { return Stretch2( Vec2_t{ size }); }
    };


    using RectU     = Rectangle< uint >;
    using RectI     = Rectangle< int >;
    using RectF     = Rectangle< float >;


/*
=================================================
    LeftTop / RightBottom
=================================================
*/
    template <typename T>
    constexpr Rectangle<T>&  Rectangle<T>::LeftTop (const Vec2_t& v) __NE___
    {
        left = v.x;
        top  = v.y;
        return *this;
    }

    template <typename T>
    constexpr Rectangle<T>&  Rectangle<T>::RightBottom (const Vec2_t& v) __NE___
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
        lhs.left += rhs.x;  lhs.right  += rhs.x;
        lhs.top  += rhs.y;  lhs.bottom += rhs.y;
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
        lhs.left -= rhs.x;  lhs.right  -= rhs.x;
        lhs.top  -= rhs.y;  lhs.bottom -= rhs.y;
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
        lhs.left *= rhs.x;  lhs.right  *= rhs.x;
        lhs.top  *= rhs.y;  lhs.bottom *= rhs.y;
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
        lhs.left /= rhs.x;  lhs.right  /= rhs.x;
        lhs.top  /= rhs.y;  lhs.bottom /= rhs.y;
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
    constexpr bool  Rectangle<T>::IsNormalized () C_NE___
    {
        return (left <= right) & (top <= bottom);
    }

/*
=================================================
    Normalize
=================================================
*/
    template <typename T>
    constexpr Rectangle<T>&  Rectangle<T>::Normalize () __NE___
    {
        if ( left > right ) std::swap( left, right );
        if ( top > bottom ) std::swap( top, bottom );
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
        return (point.x >= left) & (point.x < right) & (point.y >= top) & (point.y < bottom);
    }

    template <typename T>
    constexpr bool  Rectangle<T>::Intersects (const Self &other) C_NE___
    {
        return  ((left < other.right) & (right > other.left) & (bottom > other.top) & (top < other.bottom)) |
                ((other.right < left) & (other.left > right) & (other.top > bottom) & (other.bottom < top));
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
    constexpr Rectangle<T>  Rectangle<T>::Intersection (const Self &other) C_NE___
    {
        Rectangle<T>    res;
        res.left    = Math::Max( left,   other.left   );
        res.top     = Math::Max( top,    other.top    );
        res.right   = Math::Min( right,  other.right  );
        res.bottom  = Math::Min( bottom, other.bottom );
        return res;
    }

/*
=================================================
    Join
=================================================
*/
    template <typename T>
    constexpr Rectangle<T>&  Rectangle<T>::Join (const Self &other) __NE___
    {
        left    = Math::Min( left,   other.left   );
        top     = Math::Min( top,    other.top    );
        right   = Math::Max( right,  other.right  );
        bottom  = Math::Max( bottom, other.bottom );
        return *this;
    }

    template <typename T>
    constexpr Rectangle<T>&  Rectangle<T>::Join (const Vec2_t &point) __NE___
    {
        left    = Math::Min( left,   point.x );
        top     = Math::Min( top,    point.y );
        right   = Math::Max( right,  point.x );
        bottom  = Math::Max( bottom, point.y );
        return *this;
    }

/*
=================================================
    Stretch
=================================================
*/
    template <typename T>
    constexpr Rectangle<T>&  Rectangle<T>::Stretch (const Self &size) __NE___
    {
        left    -= size.left;
        top     -= size.top;
        right   += size.right;
        bottom  += size.bottom;
        return *this;
    }

    template <typename T>
    constexpr Rectangle<T>&  Rectangle<T>::Stretch (const Vec2_t &size) __NE___
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
    constexpr Rectangle<T>&  Rectangle<T>::Stretch2 (const Vec2_t &size) __NE___
    {
        left    -= size.x;
        top     -= size.y;
        right   += size.x;
        bottom  += size.y;
        return *this;
    }

/*
=================================================
    Equals
=================================================
*/
    template <typename T>
    ND_ bool4  Equals (const Rectangle<T> &lhs, const Rectangle<T> &rhs, const T err = Epsilon<T>()) __NE___
    {
        return bool4{ Math::Equals( lhs.left,   rhs.left,   err ),
                      Math::Equals( lhs.top,    rhs.top,    err ),
                      Math::Equals( lhs.right,  rhs.right,  err ),
                      Math::Equals( lhs.bottom, rhs.bottom, err )};
    }

    template <typename T>
    ND_ bool4  Equals (const Rectangle<T> &lhs, const Rectangle<T> &rhs, const Percent err) __NE___
    {
        return bool4{ Math::Equals( lhs.left,   rhs.left,   err ),
                      Math::Equals( lhs.top,    rhs.top,    err ),
                      Math::Equals( lhs.right,  rhs.right,  err ),
                      Math::Equals( lhs.bottom, rhs.bottom, err )};
    }

/*
=================================================
    BitEqual
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, bool>  BitEqual (const Rectangle<T> &lhs, const Rectangle<T> &rhs, const EnabledBitCount bitCount) __NE___
    {
        return bool4{ Math::BitEqual( lhs.left,   rhs.left,   bitCount ),
                      Math::BitEqual( lhs.top,    rhs.top,    bitCount ),
                      Math::BitEqual( lhs.right,  rhs.right,  bitCount ),
                      Math::BitEqual( lhs.bottom, rhs.bottom, bitCount )};
    }

    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, bool>  BitEqual (const Rectangle<T> &lhs, const Rectangle<T> &rhs) __NE___
    {
        return bool4{ Math::BitEqual( lhs.left,   rhs.left   ),
                      Math::BitEqual( lhs.top,    rhs.top    ),
                      Math::BitEqual( lhs.right,  rhs.right  ),
                      Math::BitEqual( lhs.bottom, rhs.bottom )};
    }


} // AE::Math


namespace AE::Base
{
    template <typename T>   struct TMemCopyAvailable< Rectangle<T> >        { static constexpr bool  value = IsMemCopyAvailable<T>; };
    template <typename T>   struct TZeroMemAvailable< Rectangle<T> >        { static constexpr bool  value = IsZeroMemAvailable<T>; };
    template <typename T>   struct TTriviallySerializable< Rectangle<T> >   { static constexpr bool  value = IsTriviallySerializable<T>; };

} // AE::Base


template <typename T>
struct std::hash< AE::Math::Rectangle<T> >
{
    ND_ size_t  operator () (const AE::Math::Rectangle<T> &value) C_NE___
    {
    #if AE_FAST_HASH
        return  size_t( AE::Base::HashOf( this, sizeof(*this) ));
    #else
        return  size_t( AE::Base::HashOf( value.left )  + AE::Base::HashOf( value.bottom ) +
                        AE::Base::HashOf( value.right ) + AE::Base::HashOf( value.top ));
    #endif
    }
};
