// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Stream/Stream.h"
#include "base/Algorithms/ArrayUtils.h"

namespace AE::Base
{

	//
	// Memory Reference read-only Stream
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
		MemRefRStream () {}
		MemRefRStream (const void* ptr, Bytes size);
		~MemRefRStream () {}

		explicit MemRefRStream (StringView data) : MemRefRStream{data.data(), StringSizeOf(data)} {}

		template <typename T>
		explicit MemRefRStream (ArrayView<T> data) : MemRefRStream{data.data(), ArraySizeOf(data)} {}
		
		ND_ bool	IsOpen ()	const override final	{ return true; }
		ND_ Bytes	Position ()	const override final	{ return _pos; }
		ND_ Bytes	Size ()		const override final	{ return _size; }

			bool	SeekSet (Bytes pos) override final;
		ND_ Bytes	Read2 (OUT void *buffer, Bytes size) override final;
		
			void	UpdateFastStream (OUT const void* &begin, OUT const void* &end) override;

		ND_ RC<MemRefRStream>  ToSubStream (Bytes offset, Bytes size);

	protected:
		void  _Set (const void* ptr, Bytes size);
	};
	


	//
	// Memory read-only Stream
	//

	class MemRStream final : public MemRefRStream
	{
	// variables
	private:
		Array<ubyte>	_data;

	// methods
	public:
		MemRStream () {}
		MemRStream (Array<ubyte> &&data);
		MemRStream (const void* ptr, Bytes size);
		~MemRStream () {}

		ND_ bool  Decompress (RStream &srcFile);
		ND_ bool  Load (RStream &srcFile, Bytes size = Bytes::Max());
	};



	//
	// Memory Write-only Stream
	//

	class MemWStream final : public WStream
	{
	// variables
	private:
		Array<ubyte>	_data;
		Bytes			_pos;


	// methods
	public:
		MemWStream () {
			_data.reserve( 4u << 10 );
		}
		
		explicit MemWStream (Bytes bufferSize) {
			_data.reserve( usize(bufferSize) );
		}

		~MemWStream () {}
		
		ND_ bool	IsOpen ()	const override		{ return true; }
		ND_ Bytes	Position ()	const override		{ return _pos; }
		ND_ Bytes	Size ()		const override		{ return Bytes(_data.size()); }
		
			bool	SeekSet (Bytes pos) override;
		ND_ Bytes	Write2 (const void *buffer, Bytes size) override;

		void  Flush () override
		{
			// do nothing
		}

		void  UpdateFastStream (OUT void* &begin, OUT const void* &end, Bytes reserve = DefaultAllocationSize) override;
		void  EndFastStream (const void* ptr) override;

		ND_ ArrayView<ubyte>	GetData ()		const	{ return ArrayView<ubyte>{ _data.data(), usize(_pos) }; }
		ND_ RC<MemRefRStream>	ToRStream ()	const	{ return MakeRC<MemRefRStream>( GetData() ); }

		void  Clear ();

		ND_ bool  Store (WStream &dstFile) const
		{
			CHECK_ERR( dstFile.IsOpen() );
			ASSERT( Position() > 0 );
			return dstFile.Write( _data.data(), Position() );
		}
	};


}	// AE::Base
