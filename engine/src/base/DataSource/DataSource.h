// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread safe:	optional
	Exceptions:		no
*/

#pragma once

#include "base/Math/Bytes.h"
#include "base/Utils/RefCounter.h"
#include "base/Containers/ArrayView.h"

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
			Async				= 1 << 7,
		};


	// methods
	protected:
		IDataSource () __NE___ {}

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

	//	ND_ virtual RC<RStream>		AsRStream ()				{ return Default; }
	//	ND_ virtual RC<WStream>		AsWStream ()				{ return Default; }

	//	ND_ virtual RC<RDataSource>	AsRDataSource ()			{ return Default; }
	//	ND_ virtual RC<WDataSource>	AsWDataSource ()			{ return Default; }
	};
	
	AE_BIT_OPERATORS( IDataSource::ESourceType );



	//
	// Read-only Data Source
	//
	
	class RDataSource : public IDataSource
	{
	// interface
	public:
			ESourceType		GetSourceType ()	C_NE_OV	{ return ESourceType::RandomAccess | ESourceType::ReadAccess | ESourceType::FixedSize; }
		
		ND_ virtual Bytes	OffsetAlign ()		C_NE___	{ return 1_b; }
		ND_ virtual Bytes	Size ()				C_NE___ = 0;

		// returns size of readn data
		ND_ virtual Bytes	ReadBlock (Bytes offset, OUT void *buffer, Bytes size) __NE___ = 0;

		
	// methods
	public:
		RDataSource () __NE___ {}


		bool  Read (Bytes offset, OUT void *buffer, Bytes size) __NE___
		{
			return ReadBlock( offset, buffer, size ) == size;
		}
		

		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Read (Bytes offset, usize length, OUT BasicString<T,A> &str) __NE___
		{
			CATCH_ERR( str.resize( length ));

			Bytes	expected_size	{ sizeof(str[0]) * str.length() };
			Bytes	current_size	= ReadBlock( offset, str.data(), expected_size );
		
			str.resize( usize(current_size / sizeof(str[0])) );	// nothrow

			return str.length() == length;
		}
		

		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Read (Bytes offset, Bytes size, OUT BasicString<T,A> &str) __NE___
		{
			ASSERT( IsAligned( size, sizeof(T) ));
			return Read( offset, usize(size) / sizeof(T), OUT str );
		}


		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Read (Bytes offset, usize count, OUT Array<T,A> &arr) __NE___
		{
			CATCH_ERR( arr.resize( count ));

			Bytes	expected_size	{ sizeof(arr[0]) * arr.size() };
			Bytes	current_size	= ReadBlock( offset, arr.data(), expected_size );
		
			arr.resize( usize(current_size / sizeof(arr[0])) );	// nothrow

			return arr.size() == count;
		}
		

		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Read (Bytes offset, Bytes size, OUT Array<T,A> &arr) __NE___
		{
			ASSERT( IsAligned( size, sizeof(T) ));
			return Read( offset, usize(size) / sizeof(T), OUT arr );
		}


		template <typename T>
		EnableIf<IsTrivial<T>, bool>  Read (Bytes offset, OUT T &data) __NE___
		{
			return ReadBlock( offset, AddressOf(data), Sizeof(data) ) == Sizeof(data);
		}
	};



	//
	// Write-only Data Source
	//
	
	class WDataSource : public IDataSource
	{
	// interface
	public:
			ESourceType		GetSourceType ()			C_NE_OV	{ return ESourceType::RandomAccess | ESourceType::WriteAccess; }
			
		ND_ virtual Bytes	OffsetAlign ()				C_NE___	{ return 1_b; }
		ND_ virtual Bytes	Capacity ()					C_NE___	= 0;
		ND_ virtual Bytes	Reserve (Bytes capacity)	__NE___	= 0;

		// returns size of written data
		ND_ virtual Bytes	WriteBlock (Bytes offset, const void *buffer, Bytes size) __NE___ = 0;
		
			virtual void	Flush ()					__NE___ = 0;
			

	// methods
	public:
		WDataSource () __NE___ {}
		
		
		bool  Write (Bytes offset, const void *buffer, Bytes size) __NE___
		{
			return WriteBlock( offset, buffer, size ) == size;
		}
		

		template <typename T>
		EnableIf<IsTrivial<T>, bool>  Write (Bytes offset, ArrayView<T> arr) __NE___
		{
			if_unlikely( arr.empty() )
				return true;

			Bytes	size { sizeof(arr[0]) * arr.size() };

			return WriteBlock( offset, arr.data(), size ) == size;
		}
		
	
		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Write (Bytes offset, const BasicString<T,A> str) __NE___
		{
			return Write( offset, BasicStringView<T>{ str });
		}


		template <typename T>
		EnableIf<IsTrivial<T>, bool>  Write (Bytes offset, BasicStringView<T> str) __NE___
		{
			if_unlikely( str.empty() )
				return true;

			Bytes	size { sizeof(str[0]) * str.length() };

			return WriteBlock( offset, str.data(), size ) == size;
		}


		template <typename T>
		EnableIf<IsTrivial<T>, bool>  Write (Bytes offset, const T &data) __NE___
		{
			return WriteBlock( offset, AddressOf(data), Sizeof(data) ) == Sizeof(data);
		}
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
			Bytes	processed;
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
		static constexpr uint	_BufferSize = 1 << 13;


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


} // AE::Base
