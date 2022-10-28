// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/ArrayView.h"
#include "base/Math/Bytes.h"
#include "base/Memory/MemUtils.h"
#include "base/Utils/RefCounter.h"

namespace AE::Base
{

	//
	// Read-only Stream
	//
	
	class RStream : public EnableRC< RStream >
	{
	// types
	public:
		enum class EStreamType : uint
		{
			Unknown				= 0,
			Buffered			= 1 << 0,		// allow fast stream
			ThreadSafe			= 1 << 1,
			SequentialAccess	= 1 << 2,		// allow ReadSeq()
			RandomAccess		= 1 << 3,		// allow ReadRnd() and SeekSet()
			FixedSize			= 1 << 4,		// total size is known, not supported for compressed stream
		};


	// methods
	public:
		RStream () {}

		RStream (const RStream &) = delete;
		RStream (RStream &&) = delete;

		virtual ~RStream () {}

		RStream&  operator = (const RStream &) = delete;
		RStream&  operator = (RStream &&) = delete;

		ND_ virtual bool		IsOpen ()			const = 0;
		ND_ virtual Bytes		Position ()			const = 0;
		ND_ virtual Bytes		Size ()				const = 0;
		ND_ virtual EStreamType	GetStreamType ()	const = 0;

		ND_ Bytes				RemainingSize ()	const { return Size() - Position(); }
		ND_ bool				IsThreadSafe ()		const { return AllBits( GetStreamType(), EStreamType::ThreadSafe ); }

			virtual bool	SeekSet (Bytes pos) = 0;

		// sequential read
		ND_ virtual Bytes	ReadSeq (OUT void *buffer, Bytes size) = 0;

		// random access read op
		ND_ virtual Bytes	ReadRnd (Bytes offset, OUT void *buffer, Bytes size)
		{
			if_likely( SeekSet( offset ))
				return ReadSeq( OUT buffer, size );
			return 0_b;
		}

			
		virtual void  UpdateFastStream (OUT const void* &begin, OUT const void* &end)
		{
			begin	= null;
			end		= null;
		}

		virtual void  EndFastStream (const void* ptr)
		{
			Unused( ptr );
		}
		

		bool  Read (OUT void *buffer, Bytes size)
		{
			return ReadSeq( buffer, size ) == size;
		}
		

		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Read (usize length, OUT BasicString<T,A> &str)
		{
			str.resize( length );

			Bytes	expected_size	{ sizeof(str[0]) * str.length() };
			Bytes	current_size	= ReadSeq( str.data(), expected_size );
		
			str.resize( usize(current_size / sizeof(str[0])) );

			return str.length() == length;
		}
		

		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Read (Bytes size, OUT BasicString<T,A> &str)
		{
			ASSERT( IsAligned( size, sizeof(T) ));
			return Read( usize(size) / sizeof(T), OUT str );
		}


		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Read (usize count, OUT Array<T,A> &arr)
		{
			arr.resize( count );

			Bytes	expected_size	{ sizeof(arr[0]) * arr.size() };
			Bytes	current_size	= ReadSeq( arr.data(), expected_size );
		
			arr.resize( usize(current_size / sizeof(arr[0])) );

			return arr.size() == count;
		}
		

		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Read (Bytes size, OUT Array<T,A> &arr)
		{
			ASSERT( IsAligned( size, sizeof(T) ));
			return Read( usize(size) / sizeof(T), OUT arr );
		}


		template <typename T>
		EnableIf<IsTrivial<T>, bool>  Read (OUT T &data)
		{
			return ReadSeq( AddressOf(data), Bytes::SizeOf(data) ) == Bytes::SizeOf(data);
		}
	};

	AE_BIT_OPERATORS( RStream::EStreamType );



	//
	// Write-only Stream
	//
	
	class WStream : public EnableRC< WStream >
	{
	// types
	public:
		using EStreamType = RStream::EStreamType;


	// methods
	public:
		WStream () {}

		WStream (const WStream &) = delete;
		WStream (WStream &&) = delete;

		virtual ~WStream () {}

		WStream&  operator = (const WStream &) = delete;
		WStream&  operator = (WStream &&) = delete;

		ND_ virtual bool		IsOpen ()			const = 0;
		ND_ virtual Bytes		Position ()			const = 0;
		ND_ virtual Bytes		Size ()				const = 0;
		ND_ virtual EStreamType	GetStreamType ()	const = 0;

		ND_ bool				IsThreadSafe ()		const { return AllBits( GetStreamType(), EStreamType::ThreadSafe ); }
		
			virtual bool	SeekSet (Bytes pos) = 0;
		ND_ virtual Bytes	Write2 (const void *buffer, Bytes size) = 0;
			virtual void	Flush () = 0;
			

		virtual void  UpdateFastStream (OUT void* &begin, OUT const void* &end, Bytes reserve = DefaultAllocationSize)
		{
			Unused( reserve );
			begin	= null;
			end		= null;
		}

		virtual void  EndFastStream (const void* ptr)
		{
			Unused( ptr );
		}


		bool  Write (const void *buffer, Bytes size)
		{
			return Write2( buffer, size ) == size;
		}
		

		template <typename T>
		EnableIf<IsTrivial<T>, bool>  Write (ArrayView<T> buf)
		{
			Bytes	size { sizeof(buf[0]) * buf.size() };

			return Write2( buf.data(), size ) == size;
		}
		
	
		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Write (const BasicString<T,A> str)
		{
			return Write( BasicStringView<T>{ str });
		}


		template <typename T>
		EnableIf<IsTrivial<T>, bool>  Write (BasicStringView<T> str)
		{
			if ( str.empty() )
				return true;

			Bytes	size { sizeof(str[0]) * str.length() };

			return Write2( str.data(), size ) == size;
		}


		template <typename T>
		EnableIf<IsTrivial<T>, bool>  Write (const T &data)
		{
			return Write2( AddressOf(data), Bytes::SizeOf(data) ) == Bytes::SizeOf(data);
		}
	};



	//
	// Stream Utils
	//

	class StreamUtils final : public Noninstancable
	{
	// types
	public:
		struct CmpResult
		{
			Bytes	processed;
			Bytes	diff;		// 0 if equal

			constexpr CmpResult () {}
			constexpr CmpResult (Bytes processed, Bytes diff) : processed{processed}, diff{diff} {}

			ND_ explicit operator bool () const { return diff == 0; }
		};

	// methods
	public:
		ND_ static Bytes  BufferredCopy (WStream &dstStream, RStream &srcStream, Bytes size, void* buffer, Bytes bufferSize);
		ND_ static Bytes  BufferredCopy (WStream &dstStream, RStream &srcStream, Bytes size);
		ND_ static Bytes  BufferredCopy (WStream &dstStream, RStream &srcStream, void* buffer, Bytes bufferSize);
		ND_ static Bytes  BufferredCopy (WStream &dstStream, RStream &srcStream);
		
		ND_ static CmpResult  Compare (RStream &lhs, RStream &rhs, Bytes size, void* lBuffer, void* rBuffer, Bytes bufferSize);
		ND_ static CmpResult  Compare (RStream &lhs, RStream &rhs, Bytes size);
		ND_ static CmpResult  Compare (RStream &lhs, RStream &rhs);
	};


} // AE::Base

