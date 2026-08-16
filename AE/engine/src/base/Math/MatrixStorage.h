// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Matrix.h"
#include "base/Algorithms/ArrayUtils.h"

namespace AE::Base
{

	enum class EMatrixLayout
	{
		ColumnMajor,
		RowMajor,
	};


	template <typename T, uint Columns, uint Rows, EMatrixLayout Layout, usize Align = alignof(T)>
	struct MatrixStorage;


	using half2x2_storage			= MatrixStorage< half,  2, 2, EMatrixLayout::ColumnMajor, 2*2 >;
	using half2x3_storage			= MatrixStorage< half,  2, 3, EMatrixLayout::ColumnMajor, 2*4 >;
	using half2x4_storage			= MatrixStorage< half,  2, 4, EMatrixLayout::ColumnMajor, 2*4 >;
	using half3x2_storage			= MatrixStorage< half,  3, 2, EMatrixLayout::ColumnMajor, 2*2 >;
	using half3x3_storage			= MatrixStorage< half,  3, 3, EMatrixLayout::ColumnMajor, 2*4 >;
	using half3x4_storage			= MatrixStorage< half,  3, 4, EMatrixLayout::ColumnMajor, 2*4 >;
	using half4x2_storage			= MatrixStorage< half,  4, 2, EMatrixLayout::ColumnMajor, 2*2 >;
	using half4x3_storage			= MatrixStorage< half,  4, 3, EMatrixLayout::ColumnMajor, 2*4 >;
	using half4x4_storage			= MatrixStorage< half,  4, 4, EMatrixLayout::ColumnMajor, 2*4 >;

	using half2x2_storage_std140	= MatrixStorage< half,  2, 2, EMatrixLayout::ColumnMajor, 16 >;
	using half2x3_storage_std140	= MatrixStorage< half,  2, 3, EMatrixLayout::ColumnMajor, 16 >;
	using half2x4_storage_std140	= MatrixStorage< half,  2, 4, EMatrixLayout::ColumnMajor, 16 >;
	using half3x2_storage_std140	= MatrixStorage< half,  3, 2, EMatrixLayout::ColumnMajor, 16 >;
	using half3x3_storage_std140	= MatrixStorage< half,  3, 3, EMatrixLayout::ColumnMajor, 16 >;
	using half3x4_storage_std140	= MatrixStorage< half,  3, 4, EMatrixLayout::ColumnMajor, 16 >;
	using half4x2_storage_std140	= MatrixStorage< half,  4, 2, EMatrixLayout::ColumnMajor, 16 >;
	using half4x3_storage_std140	= MatrixStorage< half,  4, 3, EMatrixLayout::ColumnMajor, 16 >;
	using half4x4_storage_std140	= MatrixStorage< half,  4, 4, EMatrixLayout::ColumnMajor, 16 >;

	using packed_half2x2_storage	= MatrixStorage< half,  2, 2, EMatrixLayout::ColumnMajor, 2 >;
	using packed_half2x3_storage	= MatrixStorage< half,  2, 3, EMatrixLayout::ColumnMajor, 2 >;
	using packed_half2x4_storage	= MatrixStorage< half,  2, 4, EMatrixLayout::ColumnMajor, 2 >;
	using packed_half3x2_storage	= MatrixStorage< half,  3, 2, EMatrixLayout::ColumnMajor, 2 >;
	using packed_half3x3_storage	= MatrixStorage< half,  3, 3, EMatrixLayout::ColumnMajor, 2 >;
	using packed_half3x4_storage	= MatrixStorage< half,  3, 4, EMatrixLayout::ColumnMajor, 2 >;
	using packed_half4x2_storage	= MatrixStorage< half,  4, 2, EMatrixLayout::ColumnMajor, 2 >;
	using packed_half4x3_storage	= MatrixStorage< half,  4, 3, EMatrixLayout::ColumnMajor, 2 >;
	using packed_half4x4_storage	= MatrixStorage< half,  4, 4, EMatrixLayout::ColumnMajor, 2 >;

	using float2x2_storage			= MatrixStorage< float,  2, 2, EMatrixLayout::ColumnMajor, 4*2 >;
	using float2x3_storage			= MatrixStorage< float,  2, 3, EMatrixLayout::ColumnMajor, 4*4 >;
	using float2x4_storage			= MatrixStorage< float,  2, 4, EMatrixLayout::ColumnMajor, 4*4 >;
	using float3x2_storage			= MatrixStorage< float,  3, 2, EMatrixLayout::ColumnMajor, 4*2 >;
	using float3x3_storage			= MatrixStorage< float,  3, 3, EMatrixLayout::ColumnMajor, 4*4 >;
	using float3x4_storage			= MatrixStorage< float,  3, 4, EMatrixLayout::ColumnMajor, 4*4 >;
	using float4x2_storage			= MatrixStorage< float,  4, 2, EMatrixLayout::ColumnMajor, 4*2 >;
	using float4x3_storage			= MatrixStorage< float,  4, 3, EMatrixLayout::ColumnMajor, 4*4 >;
	using float4x4_storage			= MatrixStorage< float,  4, 4, EMatrixLayout::ColumnMajor, 4*4 >;

	using float2x2_storage_std140	= MatrixStorage< float,  2, 2, EMatrixLayout::ColumnMajor, 16 >;
	using float2x3_storage_std140	= MatrixStorage< float,  2, 3, EMatrixLayout::ColumnMajor, 16 >;
	using float2x4_storage_std140	= MatrixStorage< float,  2, 4, EMatrixLayout::ColumnMajor, 16 >;
	using float3x2_storage_std140	= MatrixStorage< float,  3, 2, EMatrixLayout::ColumnMajor, 16 >;
	using float3x3_storage_std140	= MatrixStorage< float,  3, 3, EMatrixLayout::ColumnMajor, 16 >;
	using float3x4_storage_std140	= MatrixStorage< float,  3, 4, EMatrixLayout::ColumnMajor, 16 >;
	using float4x2_storage_std140	= MatrixStorage< float,  4, 2, EMatrixLayout::ColumnMajor, 16 >;
	using float4x3_storage_std140	= MatrixStorage< float,  4, 3, EMatrixLayout::ColumnMajor, 16 >;
	using float4x4_storage_std140	= MatrixStorage< float,  4, 4, EMatrixLayout::ColumnMajor, 16 >;

	using packed_float2x2_storage	= MatrixStorage< float,  2, 2, EMatrixLayout::ColumnMajor, 4 >;
	using packed_float2x3_storage	= MatrixStorage< float,  2, 3, EMatrixLayout::ColumnMajor, 4 >;
	using packed_float2x4_storage	= MatrixStorage< float,  2, 4, EMatrixLayout::ColumnMajor, 4 >;
	using packed_float3x2_storage	= MatrixStorage< float,  3, 2, EMatrixLayout::ColumnMajor, 4 >;
	using packed_float3x3_storage	= MatrixStorage< float,  3, 3, EMatrixLayout::ColumnMajor, 4 >;
	using packed_float3x4_storage	= MatrixStorage< float,  3, 4, EMatrixLayout::ColumnMajor, 4 >;
	using packed_float4x2_storage	= MatrixStorage< float,  4, 2, EMatrixLayout::ColumnMajor, 4 >;
	using packed_float4x3_storage	= MatrixStorage< float,  4, 3, EMatrixLayout::ColumnMajor, 4 >;
	using packed_float4x4_storage	= MatrixStorage< float,  4, 4, EMatrixLayout::ColumnMajor, 4 >;

	using double2x2_storage			= MatrixStorage< double, 2, 2, EMatrixLayout::ColumnMajor, 8*2 >;
	using double2x3_storage			= MatrixStorage< double, 2, 3, EMatrixLayout::ColumnMajor, 8*4 >;
	using double2x4_storage			= MatrixStorage< double, 2, 4, EMatrixLayout::ColumnMajor, 8*4 >;
	using double3x2_storage			= MatrixStorage< double, 3, 2, EMatrixLayout::ColumnMajor, 8*2 >;
	using double3x3_storage			= MatrixStorage< double, 3, 3, EMatrixLayout::ColumnMajor, 8*4 >;
	using double3x4_storage			= MatrixStorage< double, 3, 4, EMatrixLayout::ColumnMajor, 8*4 >;
	using double4x2_storage			= MatrixStorage< double, 4, 2, EMatrixLayout::ColumnMajor, 8*2 >;
	using double4x3_storage			= MatrixStorage< double, 4, 3, EMatrixLayout::ColumnMajor, 8*4 >;
	using double4x4_storage			= MatrixStorage< double, 4, 4, EMatrixLayout::ColumnMajor, 8*4 >;

	using double2x2_storage_std140	= MatrixStorage< double, 2, 2, EMatrixLayout::ColumnMajor, 8*2 >;
	using double2x3_storage_std140	= MatrixStorage< double, 2, 3, EMatrixLayout::ColumnMajor, 8*4 >;
	using double2x4_storage_std140	= MatrixStorage< double, 2, 4, EMatrixLayout::ColumnMajor, 8*4 >;
	using double3x2_storage_std140	= MatrixStorage< double, 3, 2, EMatrixLayout::ColumnMajor, 8*2 >;
	using double3x3_storage_std140	= MatrixStorage< double, 3, 3, EMatrixLayout::ColumnMajor, 8*4 >;
	using double3x4_storage_std140	= MatrixStorage< double, 3, 4, EMatrixLayout::ColumnMajor, 8*4 >;
	using double4x2_storage_std140	= MatrixStorage< double, 4, 2, EMatrixLayout::ColumnMajor, 8*2 >;
	using double4x3_storage_std140	= MatrixStorage< double, 4, 3, EMatrixLayout::ColumnMajor, 8*4 >;
	using double4x4_storage_std140	= MatrixStorage< double, 4, 4, EMatrixLayout::ColumnMajor, 8*4 >;

	using packed_double2x2_storage	= MatrixStorage< double, 2, 2, EMatrixLayout::ColumnMajor, 8 >;
	using packed_double2x3_storage	= MatrixStorage< double, 2, 3, EMatrixLayout::ColumnMajor, 8 >;
	using packed_double2x4_storage	= MatrixStorage< double, 2, 4, EMatrixLayout::ColumnMajor, 8 >;
	using packed_double3x2_storage	= MatrixStorage< double, 3, 2, EMatrixLayout::ColumnMajor, 8 >;
	using packed_double3x3_storage	= MatrixStorage< double, 3, 3, EMatrixLayout::ColumnMajor, 8 >;
	using packed_double3x4_storage	= MatrixStorage< double, 3, 4, EMatrixLayout::ColumnMajor, 8 >;
	using packed_double4x2_storage	= MatrixStorage< double, 4, 2, EMatrixLayout::ColumnMajor, 8 >;
	using packed_double4x3_storage	= MatrixStorage< double, 4, 3, EMatrixLayout::ColumnMajor, 8 >;
	using packed_double4x4_storage	= MatrixStorage< double, 4, 4, EMatrixLayout::ColumnMajor, 8 >;



	//
	// Column-major Matrix Storage
	//

	template <typename T, uint Columns, uint Rows, usize Align>
	struct MatrixStorage< T, Columns, Rows, EMatrixLayout::ColumnMajor, Align >
	{
		StaticAssert( IsAnyFloatPoint<T> );

	// types
	public:
		struct alignas(Align) _AlignedVec
		{
			T	data [Rows] = {};
		};

		using Self			= MatrixStorage< T, Columns, Rows, EMatrixLayout::ColumnMajor, Align >;
		using Transposed_t	= MatrixStorage< T, Rows, Columns, EMatrixLayout::ColumnMajor, Align >;
		using Column_t		= Vec< T, Rows >;
		using Row_t			= Vec< T, Columns >;

	private:
		using _Columns_t	= StaticArray< _AlignedVec, Columns >;

		StaticAssert( Align >= alignof(T) );
		StaticAssert( sizeof(_Columns_t) == (AlignUp( sizeof(T) * Rows, Align ) * Columns) );


	// variables
	private:
		_Columns_t		_columns;

		//		  c0  c1  c2  c3
		//	r0	| 1 | 2 | 3 | X |	1 - left
		//	r1	|   |   |   | Y |	2 - up
		//	r2	|   |   |   | Z |	3 - forward
		//	r3	| 0 | 0 | 0 | W |


	// methods
	public:
		__Cx__ MatrixStorage () __NE___ {}

		template <typename Arg0, typename ...Args>
		__Cx__ explicit MatrixStorage (const Arg0 &arg0, const Args& ...args) __NE___
		{
			StaticAssert( (CountOf<Arg0, Args...>() == Columns * Rows) or
						  (CountOf<Arg0, Args...>() == Columns) );

			if constexpr( CountOf<Arg0, Args...>() == Columns * Rows )
				_CopyScalars<0>( arg0, args... );
			else
			if constexpr( CountOf<Arg0, Args...>() == Columns )
				_CopyColumns<0>( arg0, args... );
		}


		template <usize Align2>
		__Cx__ MatrixStorage (const MatrixStorage< T, Columns, Rows, EMatrixLayout::ColumnMajor, Align2 > &other) __NE___
		{
			_CopyColumnMajor< Columns, Rows >( other );
		}

		template <usize Align2>
		__Cx__ MatrixStorage (const MatrixStorage< T, Rows, Columns, EMatrixLayout::RowMajor, Align2 > &other) __NE___
		{
			_CopyRowMajor< Columns, Rows >( other );
		}

		template <glm::qualifier Q>
		MatrixStorage (const TMatrix< T, Columns, Rows, Q > &other) __NE___
		{
			_CopyColumnMajor< Columns, Rows >( other );
		}


		template <uint Columns2, uint Rows2, usize Align2>
		__Cx__ explicit MatrixStorage (const MatrixStorage< T, Columns2, Rows2, EMatrixLayout::ColumnMajor, Align2 > &other) __NE___
		{
			StaticAssert( Columns != Columns2 or Rows != Rows2 );
			_CopyColumnMajor< Columns2, Rows2 >( other );
		}

		template <uint Columns2, uint Rows2, usize Align2>
		__Cx__ explicit MatrixStorage (const MatrixStorage< T, Rows2, Columns2, EMatrixLayout::RowMajor, Align2 > &other) __NE___
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


		NdCx__ static Self  Identity () __NE___
		{
			constexpr uint	cnt = Min( Columns, Rows );
			Self			result;

			for (uint i = 0; i < cnt; ++i) {
				result._columns[i].data[i] = T{1};
			}
			return result;
		}


		// return column
		ND_ const Column_t  operator [] (uint index) C_NE___
		{
			ASSERT( index < Columns );
			auto&	d = _columns[index].data;

			if constexpr( Rows == 2 )
				return Column_t{ d[0], d[1] };

			if constexpr( Rows == 3 )
				return Column_t{ d[0], d[1], d[2] };

			if constexpr( Rows == 4 )
				return Column_t{ d[0], d[1], d[2], d[3] };
		}

		template <uint C>	ND_ const Column_t 	get ()		C_NE___	{ StaticAssert( C < Columns );  return (*this)[C]; }


		// return scalar
		NdCx__ const T	operator () (usize c, usize r)		C_NE___	{ ASSERT( c < Columns and r < Rows );  return _columns[c].data[r]; }
		NdCx__ T &		operator () (usize c, usize r)		__NE___	{ ASSERT( c < Columns and r < Rows );  return _columns[c].data[r]; }

		template <uint C, uint R>	NdCx__ const T	get ()	C_NE___	{ StaticAssert( C < Columns and R < Rows );  return _columns[C].data[R]; }
		template <uint C, uint R>	NdCx__ T &		get ()	__NE___	{ StaticAssert( C < Columns and R < Rows );  return _columns[C].data[R]; }


		template <uint Columns2, uint Rows2, glm::qualifier Q>
		ND_ explicit operator TMatrix< T, Columns2, Rows2, Q > () C_NE___
		{
			TMatrix< T, Columns2, Rows2, Q >	result;
			for (uint c = 0; c < Columns2; ++c)
			for (uint r = 0; r < Rows2; ++r) {
				result(c,r) = ((c < Columns) and (r < Rows)) ? (*this)(c,r) : (c == r ? T{1} : T{0});
			}
			return result;
		}

		template <uint Columns2, uint Rows2, glm::qualifier Q>
		void  Inject (const TMatrix< T, Columns2, Rows2, Q > &other) __NE___
		{
			StaticAssert( Rows2 <= Rows );
			StaticAssert( Columns2 <= Columns );

			for (uint r = 0; r < Rows2; ++r)
			for (uint c = 0; c < Columns2; ++c) {
				(*this)(c,r) = other(c,r);
			}
		}

		template <int I, glm::qualifier Q>
		void  SetTranslation (const TVec< T, I, Q > &vec) __NE___
		{
			StaticAssert( I <= 3 and I <= Columns );

			for (int i = 0; i < I; ++i)
				_columns[i].data[Rows-1] = vec[i];
		}

		NdCx__ static usize		size ()					__NE___	{ return Columns; }
		NdCx__ static MatrixDim	Dimension ()			__NE___	{ return MatrixDim{ Columns, Rows }; }

		NdCx__ static bool		IsColumnMajor ()		__NE___	{ return true; }
		NdCx__ static bool		IsRowMajor ()			__NE___	{ return not IsColumnMajor(); }


	private:
		template <uint I, typename Arg0, typename ...Args>
		__Cx__ void  _CopyScalars (const Arg0 &arg0, const Args& ...args) __NE___
		{
			StaticAssert( IsScalar<Arg0> );
			_columns[I / Rows].data[I % Rows] = arg0;

			if constexpr( I+1 < Columns * Rows )
				_CopyScalars< I+1 >( args... );
		}

		template <uint I, typename Arg0, typename ...Args>
		__Cx__ void  _CopyColumns (const Arg0 &arg0, const Args& ...args) __NE___
		{
			StaticAssert( IsSame< Arg0, Column_t > );
			std::memcpy( OUT _columns[I].data, &arg0.x, sizeof(T)*Rows );

			if constexpr( I+1 < Columns )
				_CopyColumns< I+1 >( args... );
		}

		template <uint Columns2, uint Rows2, typename M>
		__Cx__ void  _CopyColumnMajor (const M &other) __NE___
		{
			for (uint c = 0; c < Columns; ++c)
			for (uint r = 0; r < Rows; ++r) {
				_columns[c].data[r] = ((c < Columns2) and (r < Rows2)) ? other(c,r) : (c == r ? T{1} : T{0});
			}
		}

		template <uint Columns2, uint Rows2, typename M>
		__Cx__ void _CopyRowMajor (const M &other) __NE___
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
	struct MatrixStorage< T, Rows, Columns, EMatrixLayout::RowMajor, Align >
	{
		StaticAssert( IsAnyFloatPoint<T> );

	// types
	public:
		struct alignas(Align) _AlignedVec
		{
			T	data [Columns] = {};
		};

		using Self			= MatrixStorage< T, Rows, Columns, EMatrixLayout::RowMajor, Align >;
		using Transposed_t	= MatrixStorage< T, Columns, Rows, EMatrixLayout::RowMajor, Align >;
		using ColumnMajor_t	= MatrixStorage< T, Columns, Rows, EMatrixLayout::ColumnMajor, Align >;
		using Row_t			= Vec< T, Columns >;
		using Column_t		= Vec< T, Rows >;

	private:
		using _Rows_t		= StaticArray< _AlignedVec, Rows >;

		StaticAssert( Align >= alignof(T) );
		StaticAssert( sizeof(_Rows_t) == (AlignUp( sizeof(T) * Columns, Align ) * Rows) );


	// variables
	private:
		_Rows_t		_rows;

		//		  c0  c1  c2  c3
		//	r0	| 1         | 0 |	1 - left
		//      |---------------|
		//	r1	| 2         | 0 |	2 - up
		//      |---------------|
		//	r2	| 3         | 0 |	3 - forward
		//      |---------------|
		//	r3	| X | Y | Z | W |


	// methods
	public:
		__Cx__ MatrixStorage () __NE___ : _rows{} {}

		template <typename Arg0, typename ...Args>
		__Cx__ explicit MatrixStorage (const Arg0 &arg0, const Args& ...args) __NE___
		{
			StaticAssert( (CountOf<Arg0, Args...>() == Columns * Rows) or
						  (CountOf<Arg0, Args...>() == Rows) );

			if constexpr( CountOf<Arg0, Args...>() == Columns * Rows )
				_CopyScalars<0>( arg0, args... );
			else
			if constexpr( CountOf<Arg0, Args...>() == Rows )
				_CopyRows<0>( arg0, args... );
		}


		template <usize Align2>
		__Cx__ MatrixStorage (const MatrixStorage< T, Rows, Columns, EMatrixLayout::RowMajor, Align2 > &other) __NE___
		{
			_CopyRowMajor< Columns, Rows >( other );
		}

		template <usize Align2>
		__Cx__ MatrixStorage (const MatrixStorage< T, Columns, Rows, EMatrixLayout::ColumnMajor, Align2 > &other) __NE___
		{
			_CopyColumnMajor< Columns, Rows >( other );
		}

		template <glm::qualifier Q>
		MatrixStorage (const TMatrix< T, Columns, Rows, Q > &other) __NE___
		{
			_CopyColumnMajor< Columns, Rows >( other );
		}


		template <uint Columns2, uint Rows2, usize Align2>
		__Cx__ explicit MatrixStorage (const MatrixStorage< T, Rows2, Columns2, EMatrixLayout::RowMajor, Align2 > &other) __NE___
		{
			StaticAssert( Columns != Columns2 or Rows != Rows2 );
			_CopyRowMajor< Columns2, Rows2 >( other );
		}

		template <uint Columns2, uint Rows2, usize Align2>
		__Cx__ explicit MatrixStorage (const MatrixStorage< T, Columns2, Rows2, EMatrixLayout::ColumnMajor, Align2 > &other) __NE___
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


		NdCx__ static Self  Identity () __NE___
		{
			constexpr uint	cnt = Min( Columns, Rows );
			Self			result;

			for (uint i = 0; i < cnt; ++i) {
				result._rows[i].data[i] = T{1};
			}
			return result;
		}

		// returns row
		ND_ const Row_t  operator [] (uint index) C_NE___
		{
			ASSERT( index < Rows );
			auto&	d = _rows[index].data;

			if constexpr( Columns == 2 )
				return Row_t{ d[0], d[1] };

			if constexpr( Columns == 3 )
				return Row_t{ d[0], d[1], d[2] };

			if constexpr( Columns == 4 )
				return Row_t{ d[0], d[1], d[2], d[3] };
		}

		template <uint R>	ND_ const Row_t 	get ()			C_NE___	{ StaticAssert( R < Rows );  return (*this)[R]; }


		// return scalar
		NdCx__ const T		operator () (usize r, usize c)		C_NE___	{ ASSERT( c < Columns and r < Rows );  return _rows[r].data[c]; }
		NdCx__ T &			operator () (usize r, usize c)		__NE___	{ ASSERT( c < Columns and r < Rows );  return _rows[r].data[c]; }

		template <uint R, uint C>	NdCx__ const T	get ()		C_NE___	{ StaticAssert( C < Columns and R < Rows );  return _rows[R].data[C]; }
		template <uint R, uint C>	NdCx__ T &		get ()		__NE___	{ StaticAssert( C < Columns and R < Rows );  return _rows[R].data[C]; }


		template <uint Columns2, uint Rows2, glm::qualifier Q>
		ND_ explicit operator TMatrix< T, Columns2, Rows2, Q > () C_NE___
		{
			TMatrix< T, Columns2, Rows2, Q >	result;
			for (uint c = 0; c < Columns2; ++c)
			for (uint r = 0; r < Rows2; ++r) {
				result(c,r) = ((c < Columns) and (r < Rows)) ? (*this)(r,c) : (c == r ? T{1} : T{0});
			}
			return result;
		}

		template <uint Columns2, uint Rows2, glm::qualifier Q>
		void  Inject (const TMatrix< T, Columns2, Rows2, Q > &other) __NE___
		{
			StaticAssert( Rows2 <= Rows );
			StaticAssert( Columns2 <= Columns );

			for (uint c = 0; c < Columns2; ++c)
			for (uint r = 0; r < Rows2; ++r) {
				(*this)(r,c) = other(c,r);
			}
		}

		template <int I, glm::qualifier Q>
		void  SetTranslation (const TVec< T, I, Q > &vec) __NE___
		{
			StaticAssert( I <= 3 and I <= Rows );

			for (int i = 0; i < I; ++i)
				_rows[i].data[Columns-1] = vec[i];
		}

		NdCx__ static usize		size ()				__NE___	{ return Rows; }
		NdCx__ static MatrixDim	Dimension ()		__NE___	{ return MatrixDim{ Columns, Rows }; }

		NdCx__ static bool		IsColumnMajor ()	__NE___	{ return false; }
		NdCx__ static bool		IsRowMajor ()		__NE___	{ return not IsColumnMajor(); }


	private:
		template <uint I, typename Arg0, typename ...Args>
		__Cx__ void  _CopyScalars (const Arg0 &arg0, const Args& ...args) __NE___
		{
			StaticAssert( IsScalar<Arg0> );
			_rows[I / Columns].data[I % Columns] = arg0;

			if constexpr( I+1 < Columns * Rows )
				_CopyScalars< I+1 >( args... );
		}

		template <uint I, typename Arg0, typename ...Args>
		__Cx__ void  _CopyRows (const Arg0 &arg0, const Args& ...args) __NE___
		{
			StaticAssert( IsSame< Arg0, Row_t > );
			std::memcpy( OUT _rows[I].data, &arg0.x, sizeof(T)*Columns );

			if constexpr( I+1 < Rows )
				_CopyRows< I+1 >( args... );
		}

		template <uint Columns2, uint Rows2, typename M>
		__Cx__ void  _CopyRowMajor (const M &other) __NE___
		{
			for (uint r = 0; r < Rows; ++r)
			for (uint c = 0; c < Columns; ++c) {
				_rows[r].data[c] = ((c < Columns2) and (r < Rows2)) ? other(r,c) : (c == r ? T{1} : T{0});
			}
		}

		template <uint Columns2, uint Rows2, typename M>
		__Cx__ void  _CopyColumnMajor (const M &other) __NE___
		{
			for (uint r = 0; r < Rows; ++r)
			for (uint c = 0; c < Columns; ++c) {
				_rows[r].data[c] = ((r < Rows2) and (c < Columns2)) ? other(c,r) : (c == r ? T{1} : T{0});
			}
		}
	};
//-----------------------------------------------------------------------------


	template <typename T, uint Columns, uint Rows, EMatrixLayout Layout, usize Align>
	struct TMemCopyAvailable< MatrixStorage< T, Columns, Rows, Layout, Align >>		: CT_Bool< IsMemCopyAvailable<T> >{};

	template <typename T, uint Columns, uint Rows, EMatrixLayout Layout, usize Align>
	struct TZeroMemAvailable< MatrixStorage< T, Columns, Rows, Layout, Align >>		: CT_Bool< IsZeroMemAvailable<T> >{};

	// alignment is same on all platforms
	template <typename T, uint Columns, uint Rows, EMatrixLayout Layout, usize Align>
	struct TTriviallySerializable< MatrixStorage< T, Columns, Rows, Layout, Align >>: CT_Bool< IsTriviallySerializable<T> >{};

	template <typename T, uint Columns, uint Rows, EMatrixLayout Layout, usize Align>
	struct TUnwrap< MatrixStorage< T, Columns, Rows, Layout, Align >>				: TUnwrap<T> {};

} // AE::Base
