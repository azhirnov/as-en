// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Matrix.h"
#include "base/Algorithms/ArrayUtils.h"

namespace AE::Math
{

    enum class EMatrixOrder
    {
        ColumnMajor,
        RowMajor,
    };


    template <typename T, uint Columns, uint Rows, EMatrixOrder Order, usize Align = alignof(T)>
    struct MatrixStorage;


    using half2x2_storage           = MatrixStorage< half,  2, 2, EMatrixOrder::ColumnMajor, 2*2 >;
    using half2x3_storage           = MatrixStorage< half,  2, 3, EMatrixOrder::ColumnMajor, 2*4 >;
    using half2x4_storage           = MatrixStorage< half,  2, 4, EMatrixOrder::ColumnMajor, 2*4 >;
    using half3x2_storage           = MatrixStorage< half,  3, 2, EMatrixOrder::ColumnMajor, 2*2 >;
    using half3x3_storage           = MatrixStorage< half,  3, 3, EMatrixOrder::ColumnMajor, 2*4 >;
    using half3x4_storage           = MatrixStorage< half,  3, 4, EMatrixOrder::ColumnMajor, 2*4 >;
    using half4x2_storage           = MatrixStorage< half,  4, 2, EMatrixOrder::ColumnMajor, 2*2 >;
    using half4x3_storage           = MatrixStorage< half,  4, 3, EMatrixOrder::ColumnMajor, 2*4 >;
    using half4x4_storage           = MatrixStorage< half,  4, 4, EMatrixOrder::ColumnMajor, 2*4 >;

    using packed_half2x2_storage    = MatrixStorage< half,  2, 2, EMatrixOrder::ColumnMajor, 2 >;
    using packed_half2x3_storage    = MatrixStorage< half,  2, 3, EMatrixOrder::ColumnMajor, 2 >;
    using packed_half2x4_storage    = MatrixStorage< half,  2, 4, EMatrixOrder::ColumnMajor, 2 >;
    using packed_half3x2_storage    = MatrixStorage< half,  3, 2, EMatrixOrder::ColumnMajor, 2 >;
    using packed_half3x3_storage    = MatrixStorage< half,  3, 3, EMatrixOrder::ColumnMajor, 2 >;
    using packed_half3x4_storage    = MatrixStorage< half,  3, 4, EMatrixOrder::ColumnMajor, 2 >;
    using packed_half4x2_storage    = MatrixStorage< half,  4, 2, EMatrixOrder::ColumnMajor, 2 >;
    using packed_half4x3_storage    = MatrixStorage< half,  4, 3, EMatrixOrder::ColumnMajor, 2 >;
    using packed_half4x4_storage    = MatrixStorage< half,  4, 4, EMatrixOrder::ColumnMajor, 2 >;

    using float2x2_storage          = MatrixStorage< float,  2, 2, EMatrixOrder::ColumnMajor, 4*2 >;
    using float2x3_storage          = MatrixStorage< float,  2, 3, EMatrixOrder::ColumnMajor, 4*4 >;
    using float2x4_storage          = MatrixStorage< float,  2, 4, EMatrixOrder::ColumnMajor, 4*4 >;
    using float3x2_storage          = MatrixStorage< float,  3, 2, EMatrixOrder::ColumnMajor, 4*2 >;
    using float3x3_storage          = MatrixStorage< float,  3, 3, EMatrixOrder::ColumnMajor, 4*4 >;
    using float3x4_storage          = MatrixStorage< float,  3, 4, EMatrixOrder::ColumnMajor, 4*4 >;
    using float4x2_storage          = MatrixStorage< float,  4, 2, EMatrixOrder::ColumnMajor, 4*2 >;
    using float4x3_storage          = MatrixStorage< float,  4, 3, EMatrixOrder::ColumnMajor, 4*4 >;
    using float4x4_storage          = MatrixStorage< float,  4, 4, EMatrixOrder::ColumnMajor, 4*4 >;

    using packed_float2x2_storage   = MatrixStorage< float,  2, 2, EMatrixOrder::ColumnMajor, 4 >;
    using packed_float2x3_storage   = MatrixStorage< float,  2, 3, EMatrixOrder::ColumnMajor, 4 >;
    using packed_float2x4_storage   = MatrixStorage< float,  2, 4, EMatrixOrder::ColumnMajor, 4 >;
    using packed_float3x2_storage   = MatrixStorage< float,  3, 2, EMatrixOrder::ColumnMajor, 4 >;
    using packed_float3x3_storage   = MatrixStorage< float,  3, 3, EMatrixOrder::ColumnMajor, 4 >;
    using packed_float3x4_storage   = MatrixStorage< float,  3, 4, EMatrixOrder::ColumnMajor, 4 >;
    using packed_float4x2_storage   = MatrixStorage< float,  4, 2, EMatrixOrder::ColumnMajor, 4 >;
    using packed_float4x3_storage   = MatrixStorage< float,  4, 3, EMatrixOrder::ColumnMajor, 4 >;
    using packed_float4x4_storage   = MatrixStorage< float,  4, 4, EMatrixOrder::ColumnMajor, 4 >;

    using double2x2_storage         = MatrixStorage< double, 2, 2, EMatrixOrder::ColumnMajor, 8*2 >;
    using double2x3_storage         = MatrixStorage< double, 2, 3, EMatrixOrder::ColumnMajor, 8*4 >;
    using double2x4_storage         = MatrixStorage< double, 2, 4, EMatrixOrder::ColumnMajor, 8*4 >;
    using double3x2_storage         = MatrixStorage< double, 3, 2, EMatrixOrder::ColumnMajor, 8*2 >;
    using double3x3_storage         = MatrixStorage< double, 3, 3, EMatrixOrder::ColumnMajor, 8*4 >;
    using double3x4_storage         = MatrixStorage< double, 3, 4, EMatrixOrder::ColumnMajor, 8*4 >;
    using double4x2_storage         = MatrixStorage< double, 4, 2, EMatrixOrder::ColumnMajor, 8*2 >;
    using double4x3_storage         = MatrixStorage< double, 4, 3, EMatrixOrder::ColumnMajor, 8*4 >;
    using double4x4_storage         = MatrixStorage< double, 4, 4, EMatrixOrder::ColumnMajor, 8*4 >;

    using packed_double2x2_storage  = MatrixStorage< double, 2, 2, EMatrixOrder::ColumnMajor, 8 >;
    using packed_double2x3_storage  = MatrixStorage< double, 2, 3, EMatrixOrder::ColumnMajor, 8 >;
    using packed_double2x4_storage  = MatrixStorage< double, 2, 4, EMatrixOrder::ColumnMajor, 8 >;
    using packed_double3x2_storage  = MatrixStorage< double, 3, 2, EMatrixOrder::ColumnMajor, 8 >;
    using packed_double3x3_storage  = MatrixStorage< double, 3, 3, EMatrixOrder::ColumnMajor, 8 >;
    using packed_double3x4_storage  = MatrixStorage< double, 3, 4, EMatrixOrder::ColumnMajor, 8 >;
    using packed_double4x2_storage  = MatrixStorage< double, 4, 2, EMatrixOrder::ColumnMajor, 8 >;
    using packed_double4x3_storage  = MatrixStorage< double, 4, 3, EMatrixOrder::ColumnMajor, 8 >;
    using packed_double4x4_storage  = MatrixStorage< double, 4, 4, EMatrixOrder::ColumnMajor, 8 >;



    //
    // Column-major Matrix Storage
    //

    template <typename T, uint Columns, uint Rows, usize Align>
    struct MatrixStorage< T, Columns, Rows, EMatrixOrder::ColumnMajor, Align >
    {
        StaticAssert( IsAnyFloatPoint<T> );

    // types
    public:
        struct alignas(Align) _AlignedVec
        {
            T   data [Rows] = {};
        };

        using Self          = MatrixStorage< T, Columns, Rows, EMatrixOrder::ColumnMajor, Align >;
        using Transposed_t  = MatrixStorage< T, Rows, Columns, EMatrixOrder::ColumnMajor, Align >;
        using Column_t      = Vec< T, Rows >;
        using Row_t         = Vec< T, Columns >;
        using Dim_t         = Math::_hidden_::_MatrixDim;

    private:
        using _Columns_t    = StaticArray< _AlignedVec, Columns >;

        StaticAssert( Align >= alignof(T) );
        StaticAssert( sizeof(_Columns_t) == (AlignUp( sizeof(T) * Rows, Align ) * Columns) );


    // variables
    private:
        _Columns_t      _columns;

        //        c0  c1  c2  c3
        //  r0  | 1 | 2 | 3 | X |   1 - left
        //  r1  |   |   |   | Y |   2 - up
        //  r2  |   |   |   | Z |   3 - forward
        //  r3  | 0 | 0 | 0 | W |


    // methods
    public:
        constexpr MatrixStorage () __NE___ {}

        template <typename Arg0, typename ...Args>
        constexpr explicit MatrixStorage (const Arg0 &arg0, const Args& ...args) __NE___
        {
            if constexpr( CountOf<Arg0, Args...>() == Columns * Rows )
                _CopyScalars<0>( arg0, args... );
            else
            if constexpr( CountOf<Arg0, Args...>() == Columns )
                _CopyColumns<0>( arg0, args... );
            else
                StaticAssert(  (CountOf<Arg0, Args...>() == Columns * Rows) or
                                (CountOf<Arg0, Args...>() == Columns) );
        }


        template <usize Align2>
        constexpr MatrixStorage (const MatrixStorage< T, Columns, Rows, EMatrixOrder::ColumnMajor, Align2 > &other) __NE___
        {
            _CopyColumnMajor< Columns, Rows >( other );
        }

        template <usize Align2>
        constexpr MatrixStorage (const MatrixStorage< T, Rows, Columns, EMatrixOrder::RowMajor, Align2 > &other) __NE___
        {
            _CopyRowMajor< Columns, Rows >( other );
        }

        template <glm::qualifier Q>
        MatrixStorage (const TMatrix< T, Columns, Rows, Q > &other) __NE___
        {
            _CopyColumnMajor< Columns, Rows >( other );
        }


        template <uint Columns2, uint Rows2, usize Align2>
        constexpr explicit MatrixStorage (const MatrixStorage< T, Columns2, Rows2, EMatrixOrder::ColumnMajor, Align2 > &other) __NE___
        {
            StaticAssert( Columns != Columns2 or Rows != Rows2 );
            _CopyColumnMajor< Columns2, Rows2 >( other );
        }

        template <uint Columns2, uint Rows2, usize Align2>
        constexpr explicit MatrixStorage (const MatrixStorage< T, Rows2, Columns2, EMatrixOrder::RowMajor, Align2 > &other) __NE___
        {
            StaticAssert( Columns != Columns2 or Rows != Rows2 );
            _CopyRowMajor< Columns2, Rows2 >( other );
        }

        template <uint Columns2, uint Rows2, glm::qualifier Q>
        explicit MatrixStorage (const TMatrix< T, Columns2, Rows2, Q > &other) __NE___
        {
            StaticAssert( Columns != Columns2 or Rows != Rows2 );
            _CopyColumnMajor< Columns2, Rows2 >( other );
        }


        ND_ static constexpr Self  Identity () __NE___
        {
            constexpr uint  cnt = Min( Columns, Rows );
            Self            result;

            for (uint i = 0; i < cnt; ++i) {
                result._columns[i].data[i] = T{1};
            }
            return result;
        }


        // return column
        ND_ const Column_t  operator [] (uint index) C_NE___
        {
            ASSERT( index < Columns );
            auto&   d = _columns[index].data;

            if constexpr( Rows == 2 )
                return Column_t{ d[0], d[1] };

            if constexpr( Rows == 3 )
                return Column_t{ d[0], d[1], d[2] };

            if constexpr( Rows == 4 )
                return Column_t{ d[0], d[1], d[2], d[3] };
        }

        template <uint C>   ND_ const Column_t  get ()              C_NE___ { StaticAssert( C < Columns );  return (*this)[C]; }


        // return scalar
        ND_ constexpr const T   operator () (usize c, usize r)      C_NE___ { ASSERT( c < Columns and r < Rows );  return _columns[c].data[r]; }
        ND_ constexpr T &       operator () (usize c, usize r)      __NE___ { ASSERT( c < Columns and r < Rows );  return _columns[c].data[r]; }

        template <uint C, uint R>   ND_ constexpr const T   get ()  C_NE___ { StaticAssert( C < Columns and R < Rows );  return _columns[C].data[R]; }
        template <uint C, uint R>   ND_ constexpr T &       get ()  __NE___ { StaticAssert( C < Columns and R < Rows );  return _columns[C].data[R]; }


        template <uint Columns2, uint Rows2, glm::qualifier Q>
        ND_ explicit operator TMatrix< T, Columns2, Rows2, Q > () C_NE___
        {
            TMatrix< T, Columns2, Rows2, Q >    result;
            for (uint c = 0; c < Columns2; ++c)
            for (uint r = 0; r < Rows2; ++r) {
                result(c,r) = ((c < Columns) and (r < Rows)) ? (*this)(c,r) : (c == r ? T{1} : T{0});
            }
            return result;
        }

        ND_ static constexpr usize      size ()             __NE___ { return Columns; }
        ND_ static constexpr Dim_t      Dimension ()        __NE___ { return {Columns, Rows}; }

        ND_ static constexpr bool       IsColumnMajor ()    __NE___ { return true; }
        ND_ static constexpr bool       IsRowMajor ()       __NE___ { return not IsColumnMajor(); }


    private:
        template <uint I, typename Arg0, typename ...Args>
        constexpr void  _CopyScalars (const Arg0 &arg0, const Args& ...args) __NE___
        {
            StaticAssert( IsScalar<Arg0> );
            _columns[I / Rows].data[I % Rows] = arg0;

            if constexpr( I+1 < Columns * Rows )
                _CopyScalars< I+1 >( args... );
        }

        template <uint I, typename Arg0, typename ...Args>
        constexpr void  _CopyColumns (const Arg0 &arg0, const Args& ...args) __NE___
        {
            StaticAssert( IsSameTypes< Arg0, Column_t > );
            std::memcpy( OUT _columns[I].data, &arg0.x, sizeof(T)*Rows );

            if constexpr( I+1 < Columns )
                _CopyColumns< I+1 >( args... );
        }

        template <uint Columns2, uint Rows2, typename M>
        constexpr void  _CopyColumnMajor (const M &other) __NE___
        {
            for (uint c = 0; c < Columns; ++c)
            for (uint r = 0; r < Rows; ++r) {
                _columns[c].data[r] = ((c < Columns2) and (r < Rows2)) ? other(c,r) : (c == r ? T{1} : T{0});
            }
        }

        template <uint Columns2, uint Rows2, typename M>
        constexpr void _CopyRowMajor (const M &other) __NE___
        {
            for (uint c = 0; c < Columns; ++c)
            for (uint r = 0; r < Rows; ++r) {
                _columns[c].data[r] = ((r < Rows2) and (c < Columns2)) ? other(r,c) : (c == r ? T{1} : T{0});
            }
        }
    };



    //
    // Row-major Matrix Storage
    //

    template <typename T, uint Rows, uint Columns, usize Align>
    struct MatrixStorage< T, Rows, Columns, EMatrixOrder::RowMajor, Align >
    {
        StaticAssert( IsAnyFloatPoint<T> );

    // types
    public:
        struct alignas(Align) _AlignedVec
        {
            T   data [Columns] = {};
        };

        using Self          = MatrixStorage< T, Rows, Columns, EMatrixOrder::RowMajor, Align >;
        using Transposed_t  = MatrixStorage< T, Columns, Rows, EMatrixOrder::RowMajor, Align >;
        using ColumnMajor_t = MatrixStorage< T, Columns, Rows, EMatrixOrder::ColumnMajor, Align >;
        using Row_t         = Vec< T, Columns >;
        using Column_t      = Vec< T, Rows >;
        using Dim_t         = Math::_hidden_::_MatrixDim;

    private:
        using _Rows_t       = StaticArray< _AlignedVec, Rows >;

        StaticAssert( Align >= alignof(T) );
        StaticAssert( sizeof(_Rows_t) == (AlignUp( sizeof(T) * Columns, Align ) * Rows) );


    // variables
    private:
        _Rows_t     _rows;

        //        c0  c1  c2  c3
        //  r0  | 1         | 0 |   1 - left
        //      |---------------|
        //  r1  | 2         | 0 |   2 - up
        //      |---------------|
        //  r2  | 3         | 0 |   3 - forward
        //      |---------------|
        //  r3  | X | Y | Z | W |


    // methods
    public:
        constexpr MatrixStorage () __NE___ : _rows{} {}

        template <typename Arg0, typename ...Args>
        constexpr explicit MatrixStorage (const Arg0 &arg0, const Args& ...args) __NE___
        {
            if constexpr( CountOf<Arg0, Args...>() == Columns * Rows )
                _CopyScalars<0>( arg0, args... );
            else
            if constexpr( CountOf<Arg0, Args...>() == Rows )
                _CopyRows<0>( arg0, args... );
            else
                StaticAssert(  (CountOf<Arg0, Args...>() == Columns * Rows) or
                                (CountOf<Arg0, Args...>() == Rows) );
        }


        template <usize Align2>
        constexpr MatrixStorage (const MatrixStorage< T, Rows, Columns, EMatrixOrder::RowMajor, Align2 > &other) __NE___
        {
            _CopyRowMajor< Columns, Rows >( other );
        }

        template <usize Align2>
        constexpr MatrixStorage (const MatrixStorage< T, Columns, Rows, EMatrixOrder::ColumnMajor, Align2 > &other) __NE___
        {
            _CopyColumnMajor< Columns, Rows >( other );
        }

        template <glm::qualifier Q>
        MatrixStorage (const TMatrix< T, Columns, Rows, Q > &other) __NE___
        {
            _CopyColumnMajor< Columns, Rows >( other );
        }


        template <uint Columns2, uint Rows2, usize Align2>
        constexpr explicit MatrixStorage (const MatrixStorage< T, Rows2, Columns2, EMatrixOrder::RowMajor, Align2 > &other) __NE___
        {
            StaticAssert( Columns != Columns2 or Rows != Rows2 );
            _CopyRowMajor< Columns2, Rows2 >( other );
        }

        template <uint Columns2, uint Rows2, usize Align2>
        constexpr explicit MatrixStorage (const MatrixStorage< T, Columns2, Rows2, EMatrixOrder::ColumnMajor, Align2 > &other) __NE___
        {
            StaticAssert( Columns != Columns2 or Rows != Rows2 );
            _CopyColumnMajor< Columns2, Rows2 >( other );
        }

        template <uint Columns2, uint Rows2, glm::qualifier Q>
        explicit MatrixStorage (const TMatrix< T, Columns2, Rows2, Q > &other) __NE___
        {
            StaticAssert( Columns != Columns2 or Rows != Rows2 );
            _CopyColumnMajor< Columns2, Rows2 >( other );
        }


        ND_ static constexpr Self  Identity () __NE___
        {
            constexpr uint  cnt = Min( Columns, Rows );
            Self            result;

            for (uint i = 0; i < cnt; ++i) {
                result._rows[i].data[i] = T{1};
            }
            return result;
        }

        // returns row
        ND_ const Row_t  operator [] (uint index) C_NE___
        {
            ASSERT( index < Rows );
            auto&   d = _rows[index].data;

            if constexpr( Columns == 2 )
                return Row_t{ d[0], d[1] };

            if constexpr( Columns == 3 )
                return Row_t{ d[0], d[1], d[2] };

            if constexpr( Columns == 4 )
                return Row_t{ d[0], d[1], d[2], d[3] };
        }

        template <uint R>   ND_ const Row_t     get ()                  C_NE___ { StaticAssert( R < Rows );  return (*this)[R]; }


        // return scalar
        ND_ constexpr const T       operator () (usize r, usize c)      C_NE___ { ASSERT( c < Columns and r < Rows );  return _rows[r].data[c]; }
        ND_ constexpr T &           operator () (usize r, usize c)      __NE___ { ASSERT( c < Columns and r < Rows );  return _rows[r].data[c]; }

        template <uint R, uint C>   ND_ constexpr const T   get ()      C_NE___ { StaticAssert( C < Columns and R < Rows );  return _rows[R].data[C]; }
        template <uint R, uint C>   ND_ constexpr T &       get ()      __NE___ { StaticAssert( C < Columns and R < Rows );  return _rows[R].data[C]; }


        template <uint Columns2, uint Rows2, glm::qualifier Q>
        ND_ explicit operator TMatrix< T, Columns2, Rows2, Q > () C_NE___
        {
            TMatrix< T, Columns2, Rows2, Q >    result;
            for (uint c = 0; c < Columns2; ++c)
            for (uint r = 0; r < Rows2; ++r) {
                result(c,r) = ((c < Columns) and (r < Rows)) ? (*this)(r,c) : (c == r ? T{1} : T{0});
            }
            return result;
        }

        ND_ static constexpr usize      size ()             __NE___ { return Rows; }
        ND_ static constexpr Dim_t      Dimension ()        __NE___ { return {Columns, Rows}; }

        ND_ static constexpr bool       IsColumnMajor ()    __NE___ { return false; }
        ND_ static constexpr bool       IsRowMajor ()       __NE___ { return not IsColumnMajor(); }


    private:
        template <uint I, typename Arg0, typename ...Args>
        constexpr void  _CopyScalars (const Arg0 &arg0, const Args& ...args) __NE___
        {
            StaticAssert( IsScalar<Arg0> );
            _rows[I / Columns].data[I % Columns] = arg0;

            if constexpr( I+1 < Columns * Rows )
                _CopyScalars< I+1 >( args... );
        }

        template <uint I, typename Arg0, typename ...Args>
        constexpr void  _CopyRows (const Arg0 &arg0, const Args& ...args) __NE___
        {
            StaticAssert( IsSameTypes< Arg0, Row_t > );
            std::memcpy( OUT _rows[I].data, &arg0.x, sizeof(T)*Columns );

            if constexpr( I+1 < Rows )
                _CopyRows< I+1 >( args... );
        }

        template <uint Columns2, uint Rows2, typename M>
        constexpr void  _CopyRowMajor (const M &other) __NE___
        {
            for (uint r = 0; r < Rows; ++r)
            for (uint c = 0; c < Columns; ++c) {
                _rows[r].data[c] = ((c < Columns2) and (r < Rows2)) ? other(r,c) : (c == r ? T{1} : T{0});
            }
        }

        template <uint Columns2, uint Rows2, typename M>
        constexpr void  _CopyColumnMajor (const M &other) __NE___
        {
            for (uint r = 0; r < Rows; ++r)
            for (uint c = 0; c < Columns; ++c) {
                _rows[r].data[c] = ((r < Rows2) and (c < Columns2)) ? other(c,r) : (c == r ? T{1} : T{0});
            }
        }
    };


} // AE::Math


namespace AE::Base
{
    template <typename T, uint Columns, uint Rows, EMatrixOrder Order, usize Align>
    struct TMemCopyAvailable< MatrixStorage< T, Columns, Rows, Order, Align >>      { static constexpr bool  value = IsMemCopyAvailable<T>; };

    template <typename T, uint Columns, uint Rows, EMatrixOrder Order, usize Align>
    struct TZeroMemAvailable< MatrixStorage< T, Columns, Rows, Order, Align >>      { static constexpr bool  value = IsZeroMemAvailable<T>; };

    // alignment is same on all platforms
    template <typename T, uint Columns, uint Rows, EMatrixOrder Order, usize Align>
    struct TTriviallySerializable< MatrixStorage< T, Columns, Rows, Order, Align >> { static constexpr bool  value = IsTriviallySerializable<T>; };

} // AE::Base
