// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/DataSource/DataStream.h"
#include "base/Algorithms/ArrayUtils.h"
#include "base/Memory/SharedMem.h"

namespace AE::Base
{

	//
	// Memory Reference read-only Data Source
	//

	class MemRefRDataSource : public RDataSource
	{
	// variables
	private:
		const void*		_dataPtr	= null;
		Bytes			_size;


	// methods
	public:
		MemRefRDataSource (const void* ptr, Bytes size)							__NE___;

		explicit MemRefRDataSource (StringView data)							__NE___ : MemRefRDataSource{ data.data(), StringSizeOf(data) } {}

		template <typename T>
		explicit MemRefRDataSource (ArrayView<T> data)							__NE___ : MemRefRDataSource{ data.data(), ArraySizeOf(data) } {}


		// Utils //
		ND_ bool  LoadRemainingFrom (RStream &srcStream, Bytes size = UMax)		__NE___;

		ND_ bool  LoadFrom (RStream &srcStream, Bytes offset, Bytes size)		__NE___;
		ND_ bool  LoadFrom (RDataSource &srcDS, Bytes offset, Bytes size)		__NE___;

		ND_ bool  LoadAllFrom (RStream &srcStream)								__NE___	{ return LoadFrom( srcStream, 0_b, UMax ); }
		ND_ bool  LoadAllFrom (RDataSource &srcDS)								__NE___	{ return LoadFrom( srcDS, 0_b, UMax ); }


		// RDataSource //
		bool		IsOpen ()													C_NE_OF	{ return _dataPtr != null; }
		ESourceType	GetSourceType ()											C_NE_OF;
		Bytes		Size ()														C_NE_OF	{ return _size; }

		Bytes		ReadBlock (Bytes pos, OUT void* buffer, Bytes size)			__NE_OF;


		ND_ ArrayView<ubyte>	GetData ()										C_NE___	{ return ArrayView<ubyte>{ Cast<ubyte>(_dataPtr), usize{_size} }; }


	protected:
		MemRefRDataSource ()													__NE___ {}
		void  _Set (const void* ptr, Bytes size)								__NE___;
		void  _Reset ()															__NE___;

		ND_ virtual bool  _Resize (Bytes newSize)								__NE___ { Unused( newSize );  return false; }
	};



	//
	// Array as Read-only Data Source
	//

	class ArrayRDataSource final : public MemRefRDataSource
	{
	// variables
	private:
		Array<ubyte>	_data;


	// methods
	public:
		ArrayRDataSource ()														__NE___ {}
		explicit ArrayRDataSource (Array<ubyte> data)							__NE___;
		ArrayRDataSource (const void* ptr, Bytes size)							__NE___;

		ND_ Array<ubyte>  ReleaseData ()										__NE___;

		ND_ bool  DecompressFrom (RStream &srcFile)								__NE___;

	private:
		bool  _Resize (Bytes newSize)											__NE_OV;
	};



	//
	// Shared Memory as Read-only Data Source
	//

	class SharedMemRDataSource final : public MemRefRDataSource
	{
	// variables
	private:
		RC<SharedMem>	_data;


	// methods
	public:
		SharedMemRDataSource ()													__NE___ {}
		explicit SharedMemRDataSource (RC<SharedMem> data)						__NE___;
		SharedMemRDataSource (const void* ptr, Bytes size)						__NE___;

		ND_ RC<SharedMem>	ReleaseData ()										__NE___;

	private:
		bool  _Resize (Bytes newSize)											__NE_OV;
	};



	//
	// Memory Reference Write-only Data Source
	//

	class MemRefWDataSource : public WDataSource
	{
	// variables
	protected:
		void*			_dataPtr	= null;
		Bytes			_size;
		const Bytes		_maxSize;


	// methods
	public:
		explicit MemRefWDataSource (MutableArrayView<ubyte> data)				__NE___	{ _Set( data.data(), ArraySizeOf(data) ); }
		MemRefWDataSource (void* ptr, Bytes size)								__NE___	{ _Set( ptr, size ); }

		ND_ bool	StoreTo (WStream &dstFile)									C_NE___;

		// WDataSource //
		bool		IsOpen ()													C_NE_OF	{ return true; }
		ESourceType	GetSourceType ()											C_NE_OF;
		Bytes		Capacity ()													C_NE_OF	{ return _maxSize; }

		Bytes		WriteBlock (Bytes pos, const void* buffer, Bytes size)		__NE_OF;
		void		Flush ()													__NE_OF	{}

	protected:
		explicit MemRefWDataSource (Bytes maxSize)								__NE___ : _maxSize{maxSize} {}

		void  _Set (void* ptr, Bytes size)										__NE___;
		void  _Reset ()															__NE___;

		ND_ virtual bool  _Resize (Bytes newSize)								__NE___ { Unused( newSize );  return false; }
	};



	//
	// Array as Write-only Data Source
	//

	class ArrayWDataSource final : public MemRefWDataSource
	{
	// variables
	private:
		Array<ubyte>	_data;

	// methods
	public:
		ArrayWDataSource ()														__NE___;
		explicit ArrayWDataSource (Bytes bufferSize, Bytes maxSize = UMax)		__NE___;
		explicit ArrayWDataSource (Array<ubyte> data, Bytes maxSize = 0_b)		__NE___;

		void   Clear ()															__NE___;

		ND_ ArrayView<ubyte>		GetData ()									C_NE___	{ return ArrayView<ubyte>{ _data.data(), usize(_size) }; }
		ND_ MutableArrayView<ubyte>	GetData ()									__NE___	{ return MutableArrayView<ubyte>{ _data.data(), usize(_size) }; }
		ND_ Array<ubyte>			ReleaseData ()								__NE___;

	private:
		bool  _Resize (Bytes newSize)											__NE_OV;
	};



	//
	// Shared Memory as Write-only Data Source
	//
	/*
	class SharedMemWDataSource final : public MemRefWDataSource
	{
	// variables
	private:
		RC<SharedMem>	_data;

	// methods
	public:
		SharedMemWDataSource ()													__NE___ {}
		explicit SharedMemWDataSource (Bytes bufferSize)						__NE___;
		explicit SharedMemWDataSource (RC<SharedMem> data)						__NE___;

		void   Clear ()															__NE___;

		ND_ RC<SharedMem>  ReleaseData ()										__NE___;

	private:
		bool  _Resize (Bytes newSize)											__NE_OV;
	};*/


} // AE::Base
