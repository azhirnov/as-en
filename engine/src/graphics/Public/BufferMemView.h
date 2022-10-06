// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// Buffer Memory View
	//

	struct BufferMemView
	{
	// types
	public:
		struct Data
		{
			union {
				void *			ptr		= null;
				ubyte const		(*_dbgView)[400];
			};
			Bytes		size;

			ND_ void*		End ()		 { return ptr + size; }
			ND_ void const*	End () const { return ptr + size; }
		};

		struct ConstData
		{
			union {
				void const *	ptr		= null;
				ubyte const		(*_dbgView)[400];
			};
			Bytes		size;

			ND_ void const*  End () const { return ptr + size; }
		};

		static constexpr uint	Count = 4;

		using Parts_t = FixedArray< Data, Count >;


	// variables
	private:
		Parts_t		_parts;


	// methods
	private:
		explicit BufferMemView (ArrayView<Data> parts) : _parts{parts} {}
	public:
		BufferMemView () {}
		BufferMemView (const BufferMemView &) = default;
		BufferMemView (BufferMemView &&) = default;

		BufferMemView (void *ptr, Bytes size) { PushBack( ptr, size ); }

		template <typename T>
		explicit BufferMemView (Array<T> &arr) : BufferMemView{ arr.data(), ArraySizeOf(arr) } {}

		BufferMemView&  operator = (const BufferMemView &) = default;
		BufferMemView&  operator = (BufferMemView &&) = default;

		ND_ auto	Parts ()	const	{ return ArrayView<ConstData>{ Cast<ConstData>(_parts.data()), _parts.size() }; }
		ND_ auto	Parts ()			{ return ArrayView<Data>{ _parts }; }

		ND_ auto	begin ()			{ return Parts().begin(); }
		ND_ auto	end ()				{ return Parts().end(); }

		ND_ auto	begin ()	const	{ return Parts().begin(); }
		ND_ auto	end ()		const	{ return Parts().end(); }

		ND_ bool	Empty ()	const	{ return _parts.empty(); }

			void	Clear ()			{ _parts.clear(); }


		bool  PushBack (void *ptr, Bytes size)
		{
			return _parts.try_push_back( Data{ {ptr}, size });
		}


		ND_ Bytes  DataSize () const
		{
			Bytes	result;
			for (auto& part : _parts) {
				result += part.size;
			}
			return result;
		}

		ND_ BufferMemView  Section (Bytes offset, Bytes size);

		template <typename T>
		ND_ bool  operator == (ArrayView<T> rhs) const
		{
			usize	lhs_i	= 0;
			usize	rhs_i	= 0;
			
			for (auto& part : _parts)
			{
				ASSERT( usize(part.size) % sizeof(T) == 0 );
				const usize	cnt = usize(part.size / SizeOf<T>);
				
				if_unlikely( rhs_i + cnt <= rhs.size()  and
							 rhs.section( rhs_i, cnt ) != ArrayView<T>{ Cast<T>(part.ptr), cnt })
					return false;

				lhs_i += cnt;
				rhs_i += cnt;
			}
			return lhs_i == rhs_i;
		}

		ND_ bool  operator == (const BufferMemView &rhs) const { return Compare( rhs ) == 0_b; }


		// returns how much bytes are different
		ND_ Bytes  Compare (const BufferMemView &rhs) const;

		// returns number of copied bytes
		ND_ Bytes  Copy (const BufferMemView &from);

		template <typename T>
		ND_ Bytes  Copy (ArrayView<T> from)
		{
			BufferMemView	src{ const_cast<T*>(from.data()), ArraySizeOf(from) };
			return Copy( src );
		}
	};


}	// AE::Graphics
