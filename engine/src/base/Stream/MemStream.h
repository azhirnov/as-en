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
		~MemRefRStream () override {}

		explicit MemRefRStream (StringView data) : MemRefRStream{data.data(), StringSizeOf(data)} {}

		template <typename T>
		explicit MemRefRStream (ArrayView<T> data) : MemRefRStream{data.data(), ArraySizeOf(data)} {}
		
		bool		IsOpen ()	const override final	{ return true; }
		Bytes		Position ()	const override final	{ return _pos; }
		Bytes		Size ()		const override final	{ return _size; }
		
		EStreamType	GetStreamType () const override		{ return EStreamType::Buffered | EStreamType::SequentialAccess | EStreamType::RandomAccess | EStreamType::FixedSize; }

		bool	SeekSet (Bytes pos) override final;
		Bytes	ReadSeq (OUT void *buffer, Bytes size) override final;
		Bytes	ReadRnd (Bytes offset, OUT void *buffer, Bytes size) override final;
		
		void	UpdateFastStream (OUT const void* &begin, OUT const void* &end) override;
		void	EndFastStream (const void* ptr) override;

		ND_ RC<MemRefRStream>  ToSubStream (Bytes offset, Bytes size);
		
		ND_ ArrayView<ubyte>  GetData ()		const	{ return ArrayView<ubyte>{ Cast<ubyte>(_dataPtr), usize(_size) }; }
		ND_ ArrayView<ubyte>  GetRemainData ()	const	{ return GetData().section( usize(_pos), UMax ); }

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
		explicit MemRStream (Array<ubyte> data);
		MemRStream (const void* ptr, Bytes size);
		~MemRStream () override {}

		ND_ bool  Decompress (RStream &srcFile);
		ND_ bool  LoadRemaining (RStream &srcFile, Bytes size = UMax);
		ND_ bool  Load (RStream &srcFile, Bytes offset, Bytes size);
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
			_data.reserve( usize(DefaultAllocationSize) );
		}
		
		explicit MemWStream (Bytes bufferSize) {
			_data.reserve( usize(bufferSize) );
		}

		~MemWStream () override {}
		
		bool	IsOpen ()	const override				{ return true; }
		Bytes	Position ()	const override				{ return _pos; }
		Bytes	Size ()		const override				{ return Bytes(_data.size()); }
		
		EStreamType	GetStreamType () const override		{ return EStreamType::Buffered | EStreamType::SequentialAccess | EStreamType::RandomAccess; }

		bool	SeekSet (Bytes pos) override;
		Bytes	Write2 (const void *buffer, Bytes size) override;

		void  Flush () override
		{
			// do nothing
		}

		void  UpdateFastStream (OUT void* &begin, OUT const void* &end, Bytes reserve = DefaultAllocationSize) override;
		void  EndFastStream (const void* ptr) override;

		ND_ ArrayView<ubyte>	GetData ()		const	{ return ArrayView<ubyte>{ _data.data(), usize(_pos) }; }
		ND_ Array<ubyte>		ReleaseData ()			{ auto temp = RVRef(_data);  return temp; }
		ND_ RC<MemRefRStream>	ToRStream ()	const	{ return MakeRC<MemRefRStream>( GetData() ); }

		void  Clear ();

		ND_ bool  Store (WStream &dstFile) const
		{
			CHECK_ERR( dstFile.IsOpen() );
			ASSERT( Position() > 0 );
			return dstFile.Write( _data.data(), Position() );
		}
	};


} // AE::Base
