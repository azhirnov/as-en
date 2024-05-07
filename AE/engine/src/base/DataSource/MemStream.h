// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/DataSource/DataStream.h"
#include "base/Algorithms/ArrayUtils.h"
#include "base/Memory/SharedMem.h"

namespace AE::Base
{

	//
	// Memory Reference Read-only Stream
	//

	class MemRefRStream : public RStream
	{
	// variables
	protected:
		const void*		_dataPtr	= null;
		Bytes			_pos;
		Bytes			_size;


	// methods
	public:
		MemRefRStream (const void* ptr, Bytes size)								__NE___;

		explicit MemRefRStream (StringView data)								__NE___ : MemRefRStream{data.data(), StringSizeOf(data)} {}

		template <typename T>
		explicit MemRefRStream (ArrayView<T> data)								__NE___ : MemRefRStream{data.data(), ArraySizeOf(data)} {}

		template <typename T>
		explicit MemRefRStream (MutableArrayView<T> data)						__NE___ : MemRefRStream{data.data(), ArraySizeOf(data)} {}


		// Utils //
		ND_ bool  LoadRemainingFrom (RStream &srcStream, Bytes size = UMax)		__NE___;

		ND_ bool  LoadFrom (RStream &srcStream, Bytes offset, Bytes size)		__NE___;
		ND_ bool  LoadFrom (RDataSource &srcDS, Bytes offset, Bytes size)		__NE___;

		ND_ bool  LoadAllFrom (RStream &srcStream)								__NE___	{ return LoadFrom( srcStream, 0_b, UMax ); }
		ND_ bool  LoadAllFrom (RDataSource &srcDS)								__NE___	{ return LoadFrom( srcDS, 0_b, UMax ); }


		// RStream //
		bool		IsOpen ()													C_NE_OF	{ return true; }
		PosAndSize	PositionAndSize ()											C_NE_OF	{ return { _pos, _size }; }

		ESourceType	GetSourceType ()											C_NE_OF;

		bool	SeekSet (Bytes pos)												__NE_OF;
		bool	SeekFwd (Bytes offset)											__NE_OF;

		Bytes	ReadSeq (OUT void* buffer, Bytes size)							__NE_OF;

		void	UpdateFastStream (OUT const void* &begin, OUT const void* &end) __NE_OF;
		void	EndFastStream (const void* ptr)									__NE_OF;
		Bytes	GetFastStreamPosition (const void* ptr)							__NE_OF;


		ND_ RC<MemRefRStream>	ToSubStream (Bytes offset, Bytes size = UMax)	C_Th___;

		ND_ ArrayView<ubyte>	GetData ()										C_NE___	{ return ArrayView<ubyte>{ Cast<ubyte>(_dataPtr), usize(_size) }; }
		ND_ ArrayView<ubyte>	GetRemainData ()								C_NE___	{ return GetData().section( usize(_pos), UMax ); }

	protected:
		MemRefRStream ()														__NE___ {}
		void  _Set (const void* ptr, Bytes size)								__NE___;
		void  _Reset ()															__NE___;

		ND_ virtual bool  _Resize (Bytes newSize)								__NE___ { Unused( newSize );  return false; }
	};



	//
	// Array as Read-only Stream
	//

	class ArrayRStream final : public MemRefRStream
	{
	// variables
	private:
		Array<ubyte>	_data;

	// methods
	public:
		ArrayRStream ()															__NE___ {}
		explicit ArrayRStream (Array<ubyte> data)								__NE___;
		ArrayRStream (const void* ptr, Bytes size)								__NE___;

		ND_ Array<ubyte>  ReleaseData ()										__NE___;

		ND_ bool  DecompressFrom (RStream &srcStream)							__NE___;

	private:
		bool  _Resize (Bytes newSize)											__NE_OV;
	};



	//
	// Shared Memory as Read-only Stream
	//

	class SharedMemRStream final : public MemRefRStream
	{
	// variables
	private:
		RC<SharedMem>	_data;

	// methods
	public:
		SharedMemRStream ()														__NE___ {}
		explicit SharedMemRStream (RC<SharedMem> data)							__NE___;
		SharedMemRStream (const void* ptr, Bytes size)							__NE___;

		ND_ RC<SharedMem>	ReleaseData ()										__NE___;

	private:
		bool  _Resize (Bytes newSize)											__NE_OV;
	};



	//
	// Memory Reference Write-only Stream
	//

	class MemRefWStream : public WStream
	{
	// variables
	protected:
		void*		_dataPtr	= null;
		Bytes		_pos;
		Bytes		_capacity;


	// methods
	public:
		explicit MemRefWStream (MutableArrayView<ubyte> data)					__NE___	{ _Set( data.data(), ArraySizeOf(data) ); }
		MemRefWStream (void* ptr, Bytes size)									__NE___	{ _Set( ptr, size ); }

		ND_ bool	StoreTo (WStream &dstFile)									C_NE___;

		// WStream //
		bool		IsOpen ()													C_NE_OF	{ return true; }
		Bytes		Position ()													C_NE_OF	{ return _pos; }
		ESourceType	GetSourceType ()											C_NE_OF;

		bool		SeekFwd (Bytes offset)										__NE_OF;
		Bytes		WriteSeq (const void*, Bytes)								__NE_OF;
		void		Flush ()													__NE_OF	{}

		void		UpdateFastStream (OUT void* &begin, OUT const void* &end,
									  Bytes reserve = DefaultAllocationSize)	__NE_OF;

		void		EndFastStream (const void* ptr)								__NE_OF;
		Bytes		GetFastStreamPosition (const void* ptr)						__NE_OF;

	protected:
		MemRefWStream ()														__NE___ {}
		void  _Set (void* ptr, Bytes size)										__NE___;
		void  _Reset ()															__NE___;

		ND_ virtual bool  _Resize (Bytes newSize)								__NE___ { Unused( newSize );  return false; }
	};



	//
	// Array as Write-only Stream
	//

	class ArrayWStream final : public MemRefWStream
	{
	// variables
	private:
		Array<ubyte>	_data;

	// methods
	public:
		ArrayWStream ()															__NE___;
		explicit ArrayWStream (Bytes bufferSize)								__NE___;
		explicit ArrayWStream (Array<ubyte> data)								__NE___;

		Bytes  Reserve (Bytes additionalSize)									__NE_OV;
		void   Clear ()															__NE___;

		ND_ ArrayView<ubyte>		GetData ()									C_NE___	{ return ArrayView<ubyte>{ _data.data(), usize{_pos} }; }
		ND_ MutableArrayView<ubyte>	GetData ()									__NE___	{ return MutableArrayView<ubyte>{ _data.data(), usize{_pos} }; }
		ND_ Array<ubyte>			ReleaseData ()								__NE___;

	private:
		bool  _Resize (Bytes newSize)											__NE_OV;
	};



	//
	// Shared Memory as Write-only Stream
	//

	class SharedMemWStream final : public MemRefWStream
	{
	// variables
	private:
		RC<SharedMem>	_data;

	// methods
	public:
		SharedMemWStream ()														__NE___ {}
		explicit SharedMemWStream (Bytes bufferSize)							__NE___;
		explicit SharedMemWStream (RC<SharedMem> data)							__NE___;

		void   Clear ()															__NE___;

		ND_ RC<SharedMem>  ReleaseData ()										__NE___;

	private:
		bool  _Resize (Bytes newSize)											__NE_OV;
	};


} // AE::Base
