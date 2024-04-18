// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/BufferMemView.h"

namespace AE::Graphics
{

/*
=================================================
	Section
=================================================
*/
	BufferMemView  BufferMemView::Section (Bytes offset, Bytes size) __NE___
	{
		BufferMemView	result;
		Bytes			start_off;
		const Bytes		end_off		= offset + size;

		for (auto& part : _parts)
		{
			if ( IsIntersects( start_off, start_off + part.size, offset, end_off ))
			{
				result.PushBack( part.ptr + (offset - start_off), Min( start_off + part.size - offset, end_off - start_off ));
				offset = start_off + part.size;
			}
			start_off += part.size;
		}
		return result;
	}

/*
=================================================
	Compare
=================================================
*/
	Bytes  BufferMemView::Compare (const BufferMemView &rhs) C_NE___
	{
		const auto	lhs_parts	= Parts();
		const auto	rhs_parts	= rhs.Parts();
		const Bytes	lhs_size	= DataSize();
		const Bytes	rhs_size	= rhs.DataSize();

		if ( lhs_parts.empty() or rhs_parts.empty() )
			return UMax;

		if ( lhs_size != rhs_size )
			return Max( lhs_size, rhs_size ) - Min( lhs_size, rhs_size );

		Bytes	diff			= 0_b;
		auto	lhs_part_iter	= lhs_parts.begin();
		auto	rhs_part_iter	= rhs_parts.begin();

		auto*	lhs_ptr			= Cast<ubyte>(lhs_part_iter->ptr);
		auto*	lhs_end			= Cast<ubyte>(lhs_part_iter->End());
		auto*	rhs_ptr			= Cast<ubyte>(rhs_part_iter->ptr);
		auto*	rhs_end			= Cast<ubyte>(rhs_part_iter->End());

		DEBUG_ONLY(
			Bytes	counter;
		)
		for (;;)
		{
			if_unlikely( lhs_ptr >= lhs_end )
			{
				if ( ++lhs_part_iter == lhs_parts.end() )
					break;

				lhs_ptr	= Cast<ubyte>(lhs_part_iter->ptr);
				lhs_end	= Cast<ubyte>(lhs_part_iter->End());
			}
			if_unlikely( rhs_ptr >= rhs_end )
			{
				if ( ++rhs_part_iter == rhs_parts.end() )
					break;

				rhs_ptr	= Cast<ubyte>(rhs_part_iter->ptr);
				rhs_end	= Cast<ubyte>(rhs_part_iter->End());
			}

			diff += (*lhs_ptr == *rhs_ptr ? 0 : 1);		// TODO: optimize
			DEBUG_ONLY( ++counter; )

			++lhs_ptr;
			++rhs_ptr;
		}

		ASSERT( counter == DataSize() );
		return diff;
	}

/*
=================================================
	_ToArray
=================================================
*/
	Array<char>  BufferMemView::_ToArray () C_NE___
	{
		Array<char>		temp;
		temp.resize( usize(DataSize()) );

		Bytes	offset;
		for (auto& part : _parts)
		{
			MemCopy( OUT temp.data() + offset, part.ptr, part.size );
			offset += part.size;
		}
		return temp;
	}

/*
=================================================
	_Copy
=================================================
*/
	Bytes  BufferMemView::_Copy (const BufferMemView &srcMem, Bytes srcOffset,
								 BufferMemView &dstMem, Bytes dstOffset) __NE___
	{
		if ( srcMem._parts.empty() or dstMem._parts.empty() )
			return 0_b;

		Bytes	written		= 0_b;
		auto	src_part_it	= srcMem._parts.begin();
		auto	dst_part_it	= dstMem._parts.begin();

		// move
		for (; src_part_it != srcMem._parts.end(); ++src_part_it)
		{
			if ( srcOffset < src_part_it->size )
				break;

			srcOffset -= Min( srcOffset, src_part_it->size );
		}

		for (; dst_part_it != dstMem._parts.end(); ++dst_part_it)
		{
			if ( dstOffset < dst_part_it->size )
				break;

			dstOffset -= Min( dstOffset, dst_part_it->size );
		}

		if ( src_part_it == srcMem._parts.end() or dst_part_it == dstMem._parts.end() )
			return 0_b;  // out of range

		auto*	src_ptr		= Cast<ubyte>( src_part_it->ptr ) + srcOffset;
		auto*	src_end		= Cast<ubyte>( src_part_it->End() );
		auto*	dst_ptr		= Cast<ubyte>( dst_part_it->ptr ) + dstOffset;
		auto*	dst_end		= Cast<ubyte>( dst_part_it->End() );

		CHECK_ERR( (src_ptr < src_end) and (dst_ptr < dst_end) );

		// copy
		for (;;)
		{
			if_unlikely( src_ptr >= src_end )
			{
				if ( ++src_part_it == srcMem._parts.end() )
					break;

				src_ptr	= Cast<ubyte>( src_part_it->ptr );
				src_end	= Cast<ubyte>( src_part_it->End() );
			}
			if_unlikely( dst_ptr >= dst_end )
			{
				if ( ++dst_part_it == dstMem._parts.end() )
					break;

				dst_ptr	= Cast<ubyte>( dst_part_it->ptr );
				dst_end	= Cast<ubyte>( dst_part_it->End() );
			}

			const Bytes	part_size = Min( Bytes{src_end} - Bytes{src_ptr}, Bytes{dst_end} - Bytes{dst_ptr} );

			MemCopy( OUT dst_ptr, src_ptr, part_size );

			dst_ptr	+= part_size;
			src_ptr	+= part_size;
			written	+= part_size;
		}

		return written;
	}

/*
=================================================
	GetRange
=================================================
*/
	BufferMemView::ConstData  BufferMemView::GetRange (Bytes offset, Bytes size) C_NE___
	{
		for (auto& part : _parts)
		{
			if ( offset < part.size )
				return ConstData{ part.ptr + offset, Min( part.size - offset, size )};

			offset -= Min( offset, part.size );
		}
		return {};  // out of range
	}


} // AE::Graphics
