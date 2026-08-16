// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/SIMD/Math.h"
#include "base/SIMD/Vector.h"
#include "base/Math/MatrixStorage.h"
#include "base/Pointers/RestrictPtr.h"
#include "base/Containers/ArrayView.h"
#include "base/Memory/DynUntypedStorage.h"

namespace AE::Base
{

	template <typename T>
	struct LargeVectorView;

	template <typename T>
	struct LargeVectorStorage;

	template <typename T, EMatrixLayout Layout>
	struct LargeMatrixView;

	template <typename T, EMatrixLayout Layout>
	struct LargeMatrixStorage;


	template <typename T>
	LargeVectorView (LargeVectorStorage<T>) -> LargeVectorView<T>;

	template <typename T, EMatrixLayout L>
	LargeMatrixView (LargeMatrixStorage<T,L>) -> LargeMatrixView<T,L>;



	//
	// Large Vector Storage
	//

	template <typename T>
	struct LargeVectorStorage
	{
		StaticAssert( IsAnyFloatPoint<T> );
		StaticAssert( not IsConst<T> );

	// types
	public:
		using Self		= LargeVectorStorage< T >;
		using Value_t	= T;


	// variables
	private:
		DynUntypedStorage	_storage;
		uint				_count		= 0;


	// methods
	public:
		LargeVectorStorage ()											__NE___	{}
		explicit LargeVectorStorage (usize count,
									 Bytes align = SizeOf<T> * 16)		__NE___	{ CHECK( Alloc( count, align, null )); }

			bool  Alloc (usize count, Bytes align, IAllocator* alloc)	__NE___	{ if ( _storage.Alloc( SizeOf<T> * count, align, alloc )) { _count = uint(count);  return true; } return false; }
			void  Dealloc (IAllocator* alloc)							__NE___	{ _count = 0;  return _storage.Dealloc( alloc ); }

		ND_ RstPtr<T>		Data ()										__NE___	{ return _storage.Data<T>(); }
		ND_ RstPtr<const T>	Data ()										C_NE___	{ return _storage.Data<T>(); }
		ND_ Bytes			DataSize ()									C_NE___	{ return SizeOf<T> * _count; }

		ND_ uint			Length ()									C_NE___	{ return _count; }

		ND_ explicit operator ArrayView<T> ()							C_NE___	{ return ArrayView<T>{ _storage.Ptr<T>(), _count }; }
		ND_ explicit operator MutableArrayView<T> ()					__NE___	{ return MutableArrayView<T>{ _storage.Ptr<T>(), _count }; }
	};



	//
	// Large Vector View
	//

	template <typename T>
	struct LargeVectorView
	{
		StaticAssert( IsAnyFloatPoint<T> );

	// types
	public:
		using Self		= LargeVectorView< T >;
		using Value_t	= T;


	// variables
	private:
		RstPtr<T>	_ptr;
		uint		_count	= 0;


	// methods
	public:
		LargeVectorView ()													__NE___ {}
		explicit LargeVectorView (RstPtr<T> data, Bytes size, uint count)	__NE___ : _ptr{data}, _count{count} { ASSERT( size >= DataSize() );  Unused(size); }

		template <typename B=T> requires(IsConst<B>)
		LargeVectorView (const LargeVectorView<RemoveConst<T>> &other)		__NE___ : _ptr{other.Data()}, _count{other.Length()} {}

		LargeVectorView (const LargeVectorStorage<RemoveConst<T>> &other)	__NE___ : LargeVectorView{ other.Data(), other.DataSize(), other.Length() } {}
		LargeVectorView (LargeVectorStorage<RemoveConst<T>> &other)			__NE___  : LargeVectorView{ other.Data(), other.DataSize(), other.Length() } {}

		ND_ RstPtr<T>		Data ()											__NE___	{ return _ptr; }
		ND_ RstPtr<const T>	Data ()											C_NE___	{ return _ptr; }
		ND_ Bytes			DataSize ()										C_NE___	{ return SizeOf<T> * Length(); }

		ND_ uint			Length ()										C_NE___	{ return _count; }

		ND_ explicit operator ArrayView<RemoveConst<T>> ()					C_NE___	{ return { _ptr.get(), _count }; }

		template <typename B=T> requires(not IsConst<B>)
		ND_ explicit operator MutableArrayView<T> ()						__NE___	{ return MutableArrayView<T>{ _ptr.get(), _count }; }
	};
//-----------------------------------------------------------------------------



	//
	// Column-major Large Matrix Storage
	//

	template <typename T>
	struct LargeMatrixStorage< T, EMatrixLayout::ColumnMajor >
	{
		StaticAssert( IsAnyFloatPoint<T> );
		StaticAssert( not IsConst<T> );

	// types
	public:
		static constexpr auto	Layout = EMatrixLayout::ColumnMajor;

		using Self		= LargeMatrixStorage< T, Layout >;
		using Column_t	= LargeVectorView< T >;
		using CColumn_t	= LargeVectorView< const T >;
		using Value_t	= T;


	// variables
	private:
		DynUntypedStorage	_storage;
		MatrixDim			_dim;


	// methods
	public:
		LargeMatrixStorage ()												__NE___	{}
		explicit LargeMatrixStorage (MatrixDim dim,
									 Bytes align = SizeOf<T> * 16)			__NE___	{ CHECK( Alloc( dim, align, null )); }
		explicit LargeMatrixStorage (uint cols, uint rows,
									 Bytes align = SizeOf<T> * 16)			__NE___	{ CHECK( Alloc( MatrixDim(cols, rows), align, null )); }

			bool  Alloc (MatrixDim dim, Bytes align, IAllocator* alloc)		__NE___	{ if ( _storage.Alloc( SizeOf<T> * dim.Size(), align, alloc )) { _dim = dim;  return true; } return false; }
			void  Dealloc (IAllocator* alloc)								__NE___	{ _dim = Default;  return _storage.Dealloc( alloc ); }

		ND_ Bytes			ColumnStride ()									C_NE___	{ return SizeOf<T> * _dim.rows; }

		ND_ Column_t		Column (uint index)								__NE___	{ ASSERT( index < Columns() );  return Column_t { _storage.Ptr<T>( ColumnStride() * index ), ColumnStride(), _dim.rows }; }
		ND_ CColumn_t		Column (uint index)								C_NE___	{ ASSERT( index < Columns() );  return CColumn_t{ _storage.Ptr<T>( ColumnStride() * index ), ColumnStride(), _dim.rows }; }

		ND_ RstPtr<T>		Data ()											__NE___	{ return _storage.Data<T>(); }
		ND_ RstPtr<const T>	Data ()											C_NE___	{ return _storage.Data<T>(); }
		ND_ Bytes			DataSize ()										C_NE___	{ return ColumnStride() * Columns(); }

		ND_ uint			Columns ()										C_NE___	{ return _dim.columns; }
		ND_ MatrixDim		Dimension ()									C_NE___	{ return _dim; }

		ND_ explicit operator ArrayView<RemoveConst<T>> ()					C_NE___	{ return { _storage.Ptr<T>(), _dim.Size() }; }

		template <typename B=T> requires(not IsConst<B>)
		ND_ explicit operator MutableArrayView<T> ()						__NE___	{ return MutableArrayView<T>{ _storage.Ptr<T>(), _dim.Size() }; }
	};



	//
	// Column-major Large Matrix View
	//

	template <typename T>
	struct LargeMatrixView< T, EMatrixLayout::ColumnMajor >
	{
		StaticAssert( IsAnyFloatPoint<T> );

	// types
	public:
		static constexpr auto	Layout = EMatrixLayout::ColumnMajor;

		using Self		= LargeMatrixView< T, Layout >;
		using Column_t	= LargeVectorView< T >;
		using CColumn_t	= LargeVectorView< const T >;
		using Value_t	= T;


	// variables
	private:
		RstPtr<T>	_ptr;
		MatrixDim	_dim;


	// methods
	public:
		LargeMatrixView (RstPtr<T> data, Bytes size, MatrixDim dim)				__NE___ : _ptr{data}, _dim{dim} { ASSERT( size >= DataSize() );  Unused(size); }
		LargeMatrixView (RstPtr<T> data, Bytes size, uint rows, uint cols)		__NE___ : LargeMatrixView{ data, size, MatrixDim{ cols, rows }} {}

		template <typename B=T> requires(IsConst<B>)
		LargeMatrixView (const LargeMatrixView<RemoveConst<T>, Layout> &other)	__NE___ : _ptr{other.Data()}, _dim{other.Dimension()} {}

		LargeMatrixView (const LargeMatrixStorage<RemoveConst<T>,Layout> &other)__NE___ : LargeMatrixView{ other.Data(), other.DataSize(), other.Dimension() } {}
		LargeMatrixView (LargeMatrixStorage<RemoveConst<T>,Layout> &other)		__NE___ : LargeMatrixView{ other.Data(), other.DataSize(), other.Dimension() } {}

		ND_ Bytes			ColumnStride ()										C_NE___	{ return SizeOf<T> * _dim.rows; }

		ND_ Column_t		Column (uint index)									__NE___	{ ASSERT( index < Columns() );  return Column_t { _ptr + ColumnStride() * index, ColumnStride(), _dim.rows }; }
		ND_ CColumn_t		Column (uint index)									C_NE___	{ ASSERT( index < Columns() );  return CColumn_t{ _ptr + ColumnStride() * index, ColumnStride(), _dim.rows }; }

		ND_ RstPtr<T>		Data ()												__NE___	{ return _ptr; }
		ND_ RstPtr<const T>	Data ()												C_NE___	{ return _ptr; }
		ND_ Bytes			DataSize ()											C_NE___	{ return ColumnStride() * Columns(); }

		ND_ uint			Columns ()											C_NE___	{ return _dim.columns; }
		ND_ MatrixDim		Dimension ()										C_NE___	{ return _dim; }

		ND_ explicit operator ArrayView<RemoveConst<T>> ()						C_NE___	{ return { _ptr.get(), _dim.Size() }; }

		template <typename B=T> requires(not IsConst<B>)
		ND_ explicit operator MutableArrayView<T> ()							__NE___	{ return MutableArrayView<T>{ _ptr.get(), _dim.Size() }; }
	};
//-----------------------------------------------------------------------------



	//
	// Row-major Large Matrix Storage
	//

	template <typename T>
	struct LargeMatrixStorage< T, EMatrixLayout::RowMajor >
	{
		StaticAssert( IsAnyFloatPoint<T> );
		StaticAssert( not IsConst<T> );

	// types
	public:
		static constexpr auto	Layout = EMatrixLayout::RowMajor;

		using Self		= LargeMatrixStorage< T, Layout >;
		using Row_t		= LargeVectorView< T >;
		using CRow_t	= LargeVectorView< const T >;
		using Value_t	= T;


	// variables
	private:
		DynUntypedStorage	_storage;
		MatrixDim			_dim;


	// methods
	public:
		LargeMatrixStorage ()												__NE___	{}
		explicit LargeMatrixStorage (MatrixDim dim,
									 Bytes align = SizeOf<T> * 16)			__NE___	{ CHECK( Alloc( dim, align, null )); }
		explicit LargeMatrixStorage (uint rows, uint cols,
									 Bytes align = SizeOf<T> * 16)			__NE___	{ Alloc( MatrixDim{cols, rows}, align, null ); }

			bool  Alloc (MatrixDim dim, Bytes align, IAllocator* alloc)		__NE___	{ if ( _storage.Alloc( SizeOf<T> * dim.Size(), align, alloc )) { _dim = dim;  return true; } return false; }
			void  Dealloc (IAllocator* alloc)								__NE___	{ _dim = Default;  return _storage.Dealloc( alloc ); }

		ND_ Bytes			RowStride ()									C_NE___	{ return SizeOf<T> * _dim.columns; }

		ND_ Row_t			Row (uint index)								__NE___	{ ASSERT( index < Rows() );  return Row_t { _storage.Ptr<T>( RowStride() * index ), RowStride(), _dim.columns }; }
		ND_ CRow_t			Row (uint index)								C_NE___	{ ASSERT( index < Rows() );  return CRow_t{ _storage.Ptr<T>( RowStride() * index ), RowStride(), _dim.columns }; }

		ND_ RstPtr<T>		Data ()											__NE___	{ return _storage.Data<T>(); }
		ND_ RstPtr<const T>	Data ()											C_NE___	{ return _storage.Data<T>(); }
		ND_ Bytes			DataSize ()										C_NE___	{ return RowStride() * Rows(); }

		ND_ uint			Rows ()											C_NE___	{ return _dim.rows; }
		ND_ MatrixDim		Dimension ()									C_NE___	{ return _dim; }

		ND_ explicit operator ArrayView<RemoveConst<T>> ()					C_NE___	{ return { _storage.Ptr<T>(), _dim.Size() }; }

		template <typename B=T> requires(not IsConst<B>)
		ND_ explicit operator MutableArrayView<T> ()						__NE___	{ return MutableArrayView<T>{ _storage.Ptr<T>(), _dim.Size() }; }
	};



	//
	// Row-major Large Matrix View
	//

	template <typename T>
	struct LargeMatrixView< T, EMatrixLayout::RowMajor >
	{
		StaticAssert( IsAnyFloatPoint<T> );

	// types
	public:
		static constexpr auto	Layout = EMatrixLayout::RowMajor;

		using Self		= LargeMatrixView< T, Layout >;
		using Row_t		= LargeVectorView< T >;
		using CRow_t	= LargeVectorView< const T >;
		using Value_t	= T;


	// variables
	private:
		RstPtr<T>	_ptr;
		MatrixDim	_dim;


	// methods
	public:
		LargeMatrixView (RstPtr<T> data, Bytes size, MatrixDim dim)				__NE___ : _ptr{data}, _dim{dim} { ASSERT( size >= DataSize() );  Unused(size); }
		LargeMatrixView (RstPtr<T> data, Bytes size, uint rows, uint cols)		__NE___ : LargeMatrixView{ data, size, MatrixDim{ cols, rows }} {}

		template <typename B=T> requires(IsConst<B>)
		LargeMatrixView (const LargeMatrixView<RemoveConst<T>, Layout> &other)	__NE___ : _ptr{other.Data()}, _dim{other.Dimension()} {}

		LargeMatrixView (const LargeMatrixStorage<RemoveConst<T>,Layout> &other)__NE___ : LargeMatrixView{ other.Data(), other.DataSize(), other.Dimension() } {}
		LargeMatrixView (LargeMatrixStorage<RemoveConst<T>,Layout> &other)		__NE___ : LargeMatrixView{ other.Data(), other.DataSize(), other.Dimension() } {}

		ND_ Bytes			RowStride ()										C_NE___	{ return SizeOf<T> * _dim.columns; }

		ND_ Row_t			Row (uint index)									__NE___	{ ASSERT( index < Rows() );  return Row_t { _ptr + RowStride() * index, RowStride(), _dim.columns }; }
		ND_ CRow_t			Row (uint index)									C_NE___	{ ASSERT( index < Rows() );  return CRow_t{ _ptr + RowStride() * index, RowStride(), _dim.columns }; }

		ND_ RstPtr<T>		Data ()												__NE___	{ return _ptr; }
		ND_ RstPtr<const T>	Data ()												C_NE___	{ return _ptr; }
		ND_ Bytes			DataSize ()											C_NE___	{ return RowStride() * Rows(); }

		ND_ uint			Rows ()												C_NE___	{ return _dim.rows; }
		ND_ MatrixDim		Dimension ()										C_NE___	{ return _dim; }

		ND_ explicit operator ArrayView<RemoveConst<T>> ()						C_NE___	{ return { _ptr.get(), _dim.Size() }; }

		template <typename B=T> requires(not IsConst<B>)
		ND_ explicit operator MutableArrayView<T> ()							__NE___	{ return MutableArrayView<T>{ _ptr.get(), _dim.Size() }; }
	};
//-----------------------------------------------------------------------------



	struct GEMM
	{
		// C[MxN] += A[MxK] * B[KxN]
		static void  MulAdd (LargeMatrixView<const float,	EMatrixLayout::RowMajor>  matA,
							 LargeMatrixView<const float,	EMatrixLayout::RowMajor>  matB,
							 INOUT LargeMatrixView<float,	EMatrixLayout::RowMajor>  matC)			__NE___;

		static void  ReLU (INOUT LargeMatrixView<float,	EMatrixLayout::RowMajor>  mat,
						   float scale)																__NE___;

		ND_ static bool  IsValid (LargeMatrixView<const float,	EMatrixLayout::RowMajor>  mat)		__NE___;

		ND_ static bool  IsValidDim (LargeMatrixView<const float,	EMatrixLayout::RowMajor>  matA,
									 LargeMatrixView<const float,	EMatrixLayout::RowMajor>  matB,
									 LargeMatrixView<const float,	EMatrixLayout::RowMajor>  matC)	__NE___;
	};

	struct GEMV
	{
		// C[M] += A[K] * B[MxK]
		static void  MulAdd (LargeVectorView<const float>							vecA,
							 LargeMatrixView<const float, EMatrixLayout::RowMajor>	matB,
							 INOUT LargeVectorView<float>							vecC)			__NE___;

		static void  ReLU (INOUT LargeVectorView<float> vec, float scale)							__NE___;

		ND_ static bool  IsValid (LargeVectorView<const float>  vec)								__NE___;

		ND_ static bool  IsValidDim (LargeVectorView<const float>							vecA,
									 LargeMatrixView<const float, EMatrixLayout::RowMajor>	matB,
									 LargeVectorView<const float>							vecC)	__NE___;
	};


} // AE::Base
