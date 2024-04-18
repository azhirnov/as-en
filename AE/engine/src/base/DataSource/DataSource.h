// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread safe:	optional
	Exceptions:		no

	Asynchronous DataSource defined in 'threading' module:
	  [AsyncDataSource](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/threading/DataSource/AsyncDataSource.h).

	Network DataSource defined in 'VFS' module:
	  [NetworkStorageClient](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/vfs/Network/NetworkStorageClient.h)
	  [NetworkStorageServer](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/vfs/Network/NetworkStorageServer.h)
*/

#pragma once

#include "base/Math/Byte.h"
#include "base/Utils/RefCounter.h"
#include "base/Containers/ArrayView.h"
#include "base/Memory/MemChunkList.h"

namespace AE::Base
{
	// sequential access
	class RStream;
	class WStream;

	// random access
	class RDataSource;
	class WDataSource;



	//
	// Data Source interface
	//

	class IDataSource : public EnableRC< IDataSource >
	{
	// types
	public:
		enum class ESourceType : uint
		{
			Unknown				= 0,
			Buffered			= 1 << 0,		// allow fast stream or allow mapping to memory
			ThreadSafe			= 1 << 1,		// read/write op is thread safe
			SequentialAccess	= 1 << 2,		// allow ReadSeq() and SeekFwd()
			RandomAccess		= 1 << 3,		// allow ReadBlock() and SeekSet()
			FixedSize			= 1 << 4,		// total size is known, not supported for compressed stream
			ReadAccess			= 1 << 5,
			WriteAccess			= 1 << 6,
			Async				= 1 << 7,		// must be 'ThreadSafe' too
			DeferredOpen		= 1 << 8,		// async file can be opened even if it not exists, but read/write request will fail
			Prefetch			= 1 << 9,		// allow to use 'Prefetch()' for read stream
		};


	// methods
	protected:
		IDataSource ()									__NE___ {}

	public:
		IDataSource (const IDataSource &)				= delete;
		IDataSource (IDataSource &&)					= delete;

		IDataSource&  operator = (const IDataSource &)	= delete;
		IDataSource&  operator = (IDataSource &&)		= delete;


	// interface
	public:
		ND_ virtual bool			IsOpen ()			C_NE___ = 0;
		ND_ virtual ESourceType		GetSourceType ()	C_NE___ = 0;

		ND_ bool					IsThreadSafe ()		C_NE___	{ return AllBits( GetSourceType(), ESourceType::ThreadSafe ); }
	};

	AE_BIT_OPERATORS( IDataSource::ESourceType );



	//
	// Read-only Data Source
	//

	class RDataSource : public IDataSource
	{
	// interface
	public:
			ESourceType		GetSourceType ()										C_NE_OV;


		// Returns file size.
		// If 'GetSourceType()' doesn't returns 'FixedSize'
		// size may be unknown and 'UMax' will be returned.
		//
		ND_ virtual Bytes	Size ()													C_NE___ = 0;


		// Read file from 'pos' to 'pos + size' and write result to 'buffer'.
		// Returns size of readn data or zero on error or on end of file.
		//
		ND_ virtual Bytes	ReadBlock (Bytes pos, OUT void* buffer, Bytes size)		__NE___ = 0;


	// methods
	public:
		RDataSource ()																__NE___ {}

		ND_ bool  Read (Bytes pos, OUT void* buffer, Bytes size)					__NE___;

		template <typename T, typename A, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Read (Bytes pos, usize length, OUT BasicString<T,A> &str)			__NE___;

		template <typename T, typename A, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Read (Bytes pos, Bytes size, OUT BasicString<T,A> &str)			__NE___;

		template <typename T, typename A, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Read (Bytes pos, usize count, OUT Array<T,A> &arr)				__NE___;

		template <typename T, typename A, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Read (Bytes pos, Bytes size, OUT Array<T,A> &arr)					__NE___;

		template <typename T, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Read (Bytes pos, OUT T &data)										__NE___;

		ND_ bool  Read (Bytes pos, Bytes size, OUT MemChunkList &mem)				__NE___;
	};



	//
	// Write-only Data Source
	//

	class WDataSource : public IDataSource
	{
	// interface
	public:
			ESourceType		GetSourceType ()										C_NE_OV	{ return ESourceType::RandomAccess | ESourceType::WriteAccess; }

		ND_ virtual Bytes	Capacity ()												C_NE___	{ DBG_WARNING( "Capacity() is not supported" );  return 0_b; }

		ND_ virtual Bytes	Reserve (Bytes capacity)								__NE___	{ DBG_WARNING( "Reserve() is not supported" );  Unused( capacity );  return 0_b; }


		// Write 'buffer' to a file from 'pos' to 'pos + size'.
		// Returns size of written data or zero on error.
		//
		ND_ virtual Bytes	WriteBlock (Bytes pos, const void* buffer, Bytes size)	__NE___ = 0;


		// Flush internal cache.
		// Will block until pending data is not completely written to the destination file.
		//
			virtual void	Flush ()												__NE___ = 0;


	// methods
	public:
		WDataSource ()																__NE___ {}

		ND_ bool  Write (Bytes pos, const void* buffer, Bytes size)					__NE___;

		template <typename T, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Write (Bytes pos, ArrayView<T> arr)								__NE___;

		template <typename T, typename A, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Write (Bytes pos, const BasicString<T,A> str)						__NE___;

		template <typename T, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Write (Bytes pos, BasicStringView<T> str)							__NE___;

		template <typename T, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Write (Bytes pos, const T &data)									__NE___;

		ND_ bool  Write (Bytes pos, const MemChunkList &mem)						__NE___;
	};



	//
	// Data Source Utils
	//

	class DataSourceUtils final : public Noninstanceable
	{
	// types
	public:
		struct CmpResult
		{
			Bytes	processed;			// 'Compare()': multiple of block size.
										// 'CompareStrict()': exact position to the first non equal byte.
			slong	diff		= 0;	// 0 if equal

			constexpr CmpResult ()								__NE___	{}
			constexpr CmpResult (Bytes processed, slong diff)	__NE___	: processed{processed}, diff{diff} {}

			ND_ explicit constexpr operator bool ()				C_NE___ { return diff == 0; }
		};

		struct TempBuffer
		{
			void*	ptr;
			Bytes	size;

			explicit TempBuffer (void* ptr, Bytes size)			__NE___	: ptr{ptr}, size{size} {}
		};

	private:
		static constexpr uint	_BufferSize = 1 << 12;


	// methods
	public:

		// stream -> stream
		ND_ static Bytes  BufferedCopy (WStream &dstStream, RStream &srcStream, Bytes size, const TempBuffer &buffer)					__NE___;
		ND_ static Bytes  BufferedCopy (WStream &dstStream, RStream &srcStream, Bytes size)												__NE___;
		ND_ static Bytes  BufferedCopy (WStream &dstStream, RStream &srcStream, const TempBuffer &buffer)								__NE___;
		ND_ static Bytes  BufferedCopy (WStream &dstStream, RStream &srcStream)															__NE___;

		// data_source -> stream
		ND_ static Bytes  BufferedCopy (WStream &dstStream, RDataSource &srcDS, Bytes offset, Bytes size, const TempBuffer &buffer)		__NE___;
		ND_ static Bytes  BufferedCopy (WStream &dstStream, RDataSource &srcDS, Bytes offset, Bytes size)								__NE___;
		ND_ static Bytes  BufferedCopy (WStream &dstStream, RDataSource &srcDS, const TempBuffer &buffer)								__NE___;
		ND_ static Bytes  BufferedCopy (WStream &dstStream, RDataSource &srcDS)															__NE___;

		ND_ static CmpResult  Compare (RStream &lhs, RStream &rhs, Bytes size, const TempBuffer &lBuffer, const TempBuffer &rBuffer)	__NE___;
		ND_ static CmpResult  Compare (RStream &lhs, RStream &rhs, Bytes size)															__NE___;
		ND_ static CmpResult  Compare (RStream &lhs, RStream &rhs)																		__NE___;

		ND_ static CmpResult  CompareStrict (RStream &lhs, RStream &rhs, Bytes size, const TempBuffer &lBuffer, const TempBuffer &rBuffer)__NE___;
		ND_ static CmpResult  CompareStrict (RStream &lhs, RStream &rhs, Bytes size)													__NE___;
		ND_ static CmpResult  CompareStrict (RStream &lhs, RStream &rhs)																__NE___;

	private:
		template <bool Strict>
		ND_ static CmpResult  _Compare (RStream &lhs, RStream &rhs, Bytes size, const TempBuffer &lBuffer, const TempBuffer &rBuffer)	__NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	GetSourceType
=================================================
*/
	inline IDataSource::ESourceType  RDataSource::GetSourceType () C_NE___
	{
		return ESourceType::RandomAccess | ESourceType::ReadAccess | ESourceType::FixedSize;
	}

/*
=================================================
	Read
=================================================
*/
	inline bool  RDataSource::Read (Bytes pos, OUT void* buffer, Bytes size) __NE___
	{
		return ReadBlock( pos, buffer, size ) == size;
	}

	template <typename T, typename A, ENABLEIF2( IsTriviallySerializable<T> )>
	bool  RDataSource::Read (Bytes pos, usize length, OUT BasicString<T,A> &str) __NE___
	{
		NOTHROW_ERR( str.resize( length ));

		const Bytes		expected_size	{ sizeof(T) * length };
		const Bytes		current_size	= ReadBlock( pos, OUT str.data(), expected_size );

		NOTHROW_ERR( str.resize( usize(current_size / sizeof(T)) ));

		return str.length() == length;
	}

	template <typename T, typename A, ENABLEIF2( IsTriviallySerializable<T> )>
	bool  RDataSource::Read (Bytes pos, Bytes size, OUT BasicString<T,A> &str) __NE___
	{
		ASSERT( IsMultipleOf( size, sizeof(T) ));
		return Read( pos, usize(size) / sizeof(T), OUT str );
	}

	template <typename T, typename A, ENABLEIF2( IsTriviallySerializable<T> )>
	bool  RDataSource::Read (Bytes pos, usize count, OUT Array<T,A> &arr) __NE___
	{
		NOTHROW_ERR( arr.resize( count ));

		const Bytes		expected_size	{ sizeof(arr[0]) * arr.size() };
		const Bytes		current_size	= ReadBlock( pos, OUT arr.data(), expected_size );

		NOTHROW_ERR( arr.resize( usize(current_size / sizeof(arr[0])) ));

		return arr.size() == count;
	}

	template <typename T, typename A, ENABLEIF2( IsTriviallySerializable<T> )>
	bool  RDataSource::Read (Bytes pos, Bytes size, OUT Array<T,A> &arr) __NE___
	{
		ASSERT( IsMultipleOf( size, sizeof(T) ));
		return Read( pos, usize(size) / sizeof(T), OUT arr );
	}

	template <typename T, ENABLEIF2( IsTriviallySerializable<T> )>
	bool  RDataSource::Read (Bytes pos, OUT T &data) __NE___
	{
		return ReadBlock( pos, OUT AddressOf(data), Sizeof(data) ) == Sizeof(data);
	}

	inline bool  RDataSource::Read (Bytes pos, Bytes dataSize, OUT MemChunkList &mem) __NE___
	{
		const Bytes		chunk_size	= mem.ChunkDataSize();
		auto*			chunk		= mem.First();

		if_unlikely( (chunk == null) or (dataSize == 0) )
			return true;

		ASSERT( dataSize <= mem.Capacity() );

		bool	ok = true;
		for (; (chunk != null) and ok;)
		{
			Bytes	size = Min( dataSize, chunk_size );
			ok = (ReadBlock( pos, OUT chunk->Data(), size ) == size);

			chunk		= chunk->next;
			pos			+= size;
			dataSize	-= size;
		}
		return ok;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Write
=================================================
*/
	inline bool  WDataSource::Write (Bytes pos, const void* buffer, Bytes size) __NE___
	{
		return WriteBlock( pos, buffer, size ) == size;
	}

	template <typename T, ENABLEIF2( IsTriviallySerializable<T> )>
	bool  WDataSource::Write (Bytes pos, ArrayView<T> arr) __NE___
	{
		if_unlikely( arr.empty() )
			return true;

		const Bytes		size { sizeof(arr[0]) * arr.size() };

		return WriteBlock( pos, arr.data(), size ) == size;
	}

	template <typename T, typename A, ENABLEIF2( IsTriviallySerializable<T> )>
	bool  WDataSource::Write (Bytes pos, const BasicString<T,A> str) __NE___
	{
		return Write( pos, BasicStringView<T>{ str });
	}

	template <typename T, ENABLEIF2( IsTriviallySerializable<T> )>
	bool  WDataSource::Write (Bytes pos, BasicStringView<T> str) __NE___
	{
		if_unlikely( str.empty() )
			return true;

		const Bytes		size { sizeof(str[0]) * str.length() };

		return WriteBlock( pos, str.data(), size ) == size;
	}

	template <typename T, ENABLEIF2( IsTriviallySerializable<T> )>
	bool  WDataSource::Write (Bytes pos, const T &data) __NE___
	{
		return WriteBlock( pos, AddressOf(data), Sizeof(data) ) == Sizeof(data);
	}

	inline bool  WDataSource::Write (Bytes pos, const MemChunkList &mem) __NE___
	{
		const Bytes		chunk_size	= mem.ChunkDataSize();
		Bytes			data_size	= mem.Size();
		auto*			chunk		= mem.First();

		if_unlikely( (chunk == null) or (data_size == 0) )
			return true;

		bool	ok = true;
		for (; (chunk != null) and ok;)
		{
			Bytes	size = Min( data_size, chunk_size );
			ok = (WriteBlock( pos, chunk->Data(), size ) == size);

			chunk		= chunk->next;
			pos			+= size;
			data_size	-= size;
		}

		return ok;
	}


} // AE::Base
