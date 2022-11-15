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
		// variables
			union {
				void *			ptr		= null;
				ubyte const		(*_dbgView)[400];
			};
			Bytes		size;
			
		// methods
			Data ()						__NE___	{}
			Data (void* ptr, Bytes size)__NE___ : ptr{ptr}, size{size} {}

			ND_ void*		End ()		__NE___	{ return ptr + size; }
			ND_ void const*	End ()		C_NE___	{ return ptr + size; }
			ND_ bool		Empty ()	C_NE___	{ return ptr == null; }
		};

		struct ConstData
		{
		// variables
			union {
				void const *	ptr		= null;
				ubyte const		(*_dbgView)[400];
			};
			Bytes		size;
			
		// methods
			ConstData ()							__NE___ {}
			ConstData (const void* ptr, Bytes size)	__NE___	: ptr{ptr}, size{size} {}
			explicit ConstData (const Data &other)	__NE___	: ptr{other.ptr}, size{other.size} {}

			ND_ void const*	End ()					C_NE___	{ return ptr + size; }
			ND_ bool		Empty ()				C_NE___	{ return ptr == null; }
		};

		static constexpr uint	Count = 4;

		using Parts_t = FixedArray< Data, Count >;


	// variables
	private:
		Parts_t		_parts;


	// methods
	private:
		explicit BufferMemView (ArrayView<Data> parts) __NE___ : _parts{parts} {}
	public:
		BufferMemView ()						__NE___	{}
		BufferMemView (const BufferMemView &)	= default;
		BufferMemView (BufferMemView &&)		= default;

		BufferMemView (void *ptr, Bytes size)	__NE___	{ PushBack( ptr, size ); }

		template <typename T>
		explicit BufferMemView (Array<T> &arr)	__NE___	: BufferMemView{ arr.data(), ArraySizeOf(arr) } {}

		BufferMemView&  operator = (const BufferMemView &)	= default;
		BufferMemView&  operator = (BufferMemView &&)		= default;
		
		ND_ explicit operator Array<char> ()	C_NE___	{ return _ToArray(); }

		ND_ auto	Parts ()					C_NE___	{ return ArrayView<ConstData>{ Cast<ConstData>(_parts.data()), _parts.size() }; }
		ND_ auto	Parts ()					__NE___	{ return ArrayView<Data>{ _parts }; }

		ND_ auto	begin ()					__NE___	{ return Parts().begin(); }
		ND_ auto	end ()						__NE___	{ return Parts().end(); }

		ND_ auto	begin ()					C_NE___	{ return Parts().begin(); }
		ND_ auto	end ()						C_NE___	{ return Parts().end(); }

		ND_ bool	Empty ()					C_NE___	{ return _parts.empty(); }

			void	Clear ()					__NE___		{ _parts.clear(); }


		bool  PushBack (void *ptr, Bytes size)	__NE___
		{
			return _parts.try_push_back( Data{ ptr, size });
		}


		ND_ Bytes  DataSize ()					C_NE___
		{
			Bytes	result;
			for (auto& part : _parts) {
				result += part.size;
			}
			return result;
		}

		ND_ BufferMemView  Section (Bytes offset, Bytes size)	__NE___;

		template <typename T>
		ND_ bool  operator == (ArrayView<T> rhs) C_NE___
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

		ND_ bool  operator == (const BufferMemView &rhs) C_NE___ { return Compare( rhs ) == 0_b; }


		// returns how much bytes are different
		ND_ Bytes  Compare (const BufferMemView &rhs)	C_NE___;

		// returns number of copied bytes
		ND_ Bytes  Copy (const BufferMemView &from)		__NE___;

		template <typename T>
		ND_ Bytes  Copy (ArrayView<T> from)				__NE___
		{
			BufferMemView	src{ const_cast<T*>(from.data()), ArraySizeOf(from) };
			return Copy( src );
		}

	private:
		ND_ Array<char>  _ToArray () C_NE___;
	};


} // AE::Graphics
