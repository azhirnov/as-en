// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Forward linked list of chunks with small arrays.
*/

#pragma once

#include "base/Memory/IAllocator.h"
#include "base/Memory/CopyPolicy.h"
#include "base/Containers/ArrayView.h"

namespace AE::Base
{
	template <typename T>
	class ChunkList;

namespace _hidden_
{
	template <typename T>
	class BaseChunkList
	{
	// types
	protected:
		using Count_t		= ByteSizeToUInt< sizeof(void*)/2 >;
		using CopyPolicy_t	= CopyPolicy::AutoDetect<T>;

		static constexpr usize	_Align		= Max( AE_CACHE_LINE, alignof(T) );
		static constexpr usize	_HeaderSize	= AlignUp( sizeof(void*)*2, alignof(T) );

		template <typename C>
		struct alignas(_Align) BaseChunk : public Noncopyable
		{
		// types
			using ChunkPtr_t = Conditional< IsConst<T>, C const*, C* >;

		// variables
			ChunkPtr_t		next		= null;
			Count_t			count		= 0;		// number of constructed elements in '_data'
			Count_t			capacity	= 0;
			union {
				T			_data  [1];				// needed to put elements with correct alignment
				char		_data2 [1];
			};

		// methods
			BaseChunk ()												__NE___ {}
			~BaseChunk ()												__NE___	{ CopyPolicy_t::Destroy( Data(), count ); }

			ND_ T &			operator [] (usize i)						__NE___	{ ASSERT( i < count );  return _data[i]; }
			ND_ T const&	operator [] (usize i)						C_NE___	{ ASSERT( i < count );  return _data[i]; }

			ND_ T *			Data ()										__NE___	{ ASSERT( capacity > 0 );  return _data; }
			ND_ T const*	Data ()										C_NE___	{ ASSERT( capacity > 0 );  return _data; }

			ND_ Bytes		ChunkSize ()								C_NE___	{ return CalcChunkSize( capacity ); }

			ND_ bool		IsEmpty ()									C_NE___	{ return count == 0; }
			ND_ bool		IsFull ()									C_NE___	{ return count >= capacity; }

			ND_ explicit operator ArrayView<T> ()						C_NE___	{ return ArrayView<T>{ std::addressof(_data), count }; }

			ND_ constexpr static usize  CalcCapacity (Bytes size)		__NE___	{ return usize{(size - _HeaderSize) / sizeof(T)}; }

			ND_ constexpr static Bytes  CalcChunkSize (usize capacity)	__NE___
			{
				Bytes	size { _HeaderSize + sizeof(T) * capacity };
				//ASSERT( (size % alignof(BaseChunk)) < sizeof(T) );
				return size;
			}
		};


		template <typename C>
		struct ElemIter
		{
		// variables
		private:
			using ChunkPtr_t = Conditional< IsConst<T>, C const*, C* >;

			ChunkPtr_t		_chunk	= null;
			Count_t			_count	= 0;		// --- cached to avoid cache miss when accessing begin of the chunk
			Count_t			_index	= 0;		//   /
			ChunkPtr_t		_next	= null;		// -'

		// methods
		public:
			ElemIter ()													__NE___ {}

			explicit ElemIter (ChunkPtr_t chunk, Count_t idx)			__NE___	:
				_chunk{chunk},
				_count{_chunk ? Min( _chunk->count, _chunk->capacity ) : Count_t{0}},
				_index{idx},
				_next{_chunk ? _chunk->next : null}
			{
				_Validate();
			}

			ElemIter (const ElemIter &)									__NE___ = default;
			ElemIter (ElemIter &&)										__NE___	= default;

				ElemIter&	operator = (const ElemIter &)				__NE___ = default;
				ElemIter&	operator = (ElemIter &&)					__NE___ = default;

				ElemIter&	operator ++ ()								__NE___	{ ++_index;  _Validate();  return *this; }
				ElemIter	operator ++ (int)							__NE___	{ ElemIter<C>  res {*this};  this->operator++();  return res; }

			ND_ bool		operator != (const ElemIter &rhs)			C_NE___	{ return not (*this == rhs); }
			ND_ bool		operator == (const ElemIter &rhs)			C_NE___	{ return (_chunk == rhs._chunk) and (_index == rhs._index); }

			ND_ auto		GetChunk ()									__NE___	{ return _chunk; }
			ND_ auto		GetChunk ()									C_NE___	{ return _chunk; }
			ND_ Count_t		GetIndexInChunk ()							C_NE___	{ return _index; }

			ND_ T const&	operator * ()								C_NE___	{ NonNull( _chunk );  return (*_chunk)[_index]; }

			ND_ explicit operator ArrayView<T> ()						C_NE___	{ NonNull( _chunk );  return ArrayView<T>{ *_chunk }; }

		private:
			void  _Validate ()											__NE___
			{
				for_unlikely(; (_chunk != null) and (_index >= _count); )
				{
					_chunk	= _next;
					_index	= 0;
					_next	= _chunk ? _chunk->next : null;
					_count	= _chunk ? Min( _chunk->count, _chunk->capacity ) : Count_t{0};	// see LfChunkList
				}
			}
		};


		template <typename C>
		static void  _MoveToLast (INOUT C* &first)		__NE___
		{
			if ( first != null )
			{
				for (;;)
				{
					if_likely( first->next != null )
						first = first->next;
					else
						break;
				}
			}
		}

		template <typename C>
		ND_ static usize  _Count (C* chunk)				__NE___
		{
			usize	count = 0;
			for (; chunk != null; chunk = chunk->next) {
				count += chunk->count;
			}
			return count;
		}
	};

} // _hidden_



	//
	// List of Chunks
	//

	template <typename T>
	class ChunkList final : Base::_hidden_::BaseChunkList<T>
	{
		StaticAssert( not IsConst<T> );

	// types
	private:
		using Base_t	= Base::_hidden_::BaseChunkList<T>;
	public:
		using Self		= ChunkList< T >;
		using Value_t	= T;
		using Count_t	= typename Base_t::Count_t;

		struct Chunk : Base_t::template BaseChunk<Chunk>
		{
		// methods
			ND_ T &		emplace_back ()								__NE___;

			template <typename T0, typename ...Types>
				T &		emplace_back (T0 &&arg0, Types&& ...args)	__NE___;
		};

	private:
		using ChunkPtr_t		= typename Chunk::ChunkPtr_t;
		using Const_t			= ChunkList< const T >;
	public:
		using iterator			= typename Base_t::template ElemIter< Chunk >;
		using const_iterator	= typename Base_t::template ElemIter< const Chunk >;


	// variables
	private:
		ChunkPtr_t		_first	= null;


	// methods
	public:
		ChunkList ()												__NE___	{}
		ChunkList (Self &&)											__NE___ = default;
		ChunkList (const Self &)									__NE___ = default;

		explicit ChunkList (iterator it)							__NE___	: _first{it.GetChunk()} {}
		explicit ChunkList (ChunkPtr_t first)						__NE___	: _first{first} {}

			Self&	operator = (Self &&)							__NE___ = default;
			Self&	operator = (const Self &)						__NE___ = default;

		ND_ auto	begin ()										__NE___	{ return iterator{ _first, 0 }; }
		ND_ auto	end ()											__NE___	{ return iterator{}; }

		ND_ auto	begin ()										C_NE___	{ return const_iterator{ _first, 0 }; }
		ND_ auto	end ()											C_NE___	{ return const_iterator{}; }

		ND_ auto*	FirstChunk ()									__NE___	{ return _first; }
		ND_ auto*	FirstChunk ()									C_NE___	{ return _first; }

		ND_ auto*	operator -> ()									__NE___	{ NonNull( _first );  return _first; }
		ND_ auto*	operator -> ()									C_NE___	{ NonNull( _first );  return _first; }

		ND_ Self	LastChunk ()									__NE___	{ Self res {_first};  res.MoveToLast();  return res; }
		ND_ Const_t	LastChunk ()									C_NE___;

		ND_ Self	NextChunk ()									__NE___;
		ND_ Const_t	NextChunk ()									C_NE___;

		ND_ bool	empty ()										C_NE___	{ return _first == null; }
		ND_ usize	Count ()										C_NE___	{ return Base_t::_Count( _first ); }

			void	Append (ChunkPtr_t newChunk)					__NE___;
			void	Append (Self newChunks)							__NE___	{ Append( newChunks._first ); }

			void	MoveToLast ()									__NE___	{ Base_t::_MoveToLast( INOUT _first ); }
		ND_ bool	IsLastChunk ()									C_NE___	{ return _first == null or _first->next == null; }

		// create new chunk and append it to the end
		ND_ Self	AddChunk (void* ptr, Bytes size)				__NE___;

		template <typename Allocator>
		ND_ Self	AddChunk (Allocator &alloc, Bytes size)			__NE___;

		template <typename Allocator>
		ND_ Self	AddChunk (Allocator &alloc, usize capacity)		__NE___;

		template <typename Allocator>
			void	Destroy (Allocator &alloc)						__NE___;
	};



	//
	// List of Chunks (const)
	//

	template <typename T>
	class ChunkList< const T > final : Base::_hidden_::BaseChunkList< const T >
	{
	// types
	private:
		using Base_t	= Base::_hidden_::BaseChunkList< const T >;
	public:
		using Self		= ChunkList< const T >;
		using Value_t	= const T;
		using Count_t	= typename Base_t::Count_t;

		struct Chunk : Base_t::template BaseChunk<Chunk>
		{};

	private:
		using ChunkPtr_t		= typename Chunk::ChunkPtr_t;

	public:
		using iterator			= typename Base_t::template ElemIter< Chunk >;
		using const_iterator	= iterator;


	// variables
	private:
		ChunkPtr_t		_first	= null;


	// methods
	public:
		ChunkList ()											__NE___	{}
		ChunkList (Self &&)										__NE___ = default;
		ChunkList (const Self &)								__NE___ = default;

		explicit ChunkList (iterator it)						__NE___	: _first{it.GetChunk()} {}
		explicit ChunkList (ChunkPtr_t first)					__NE___	: _first{first} {}
		ChunkList (const ChunkList<RemoveConst<T>> &other)		__NE___	: _first{Cast<Chunk>( other.FirstChunk() )} {}

			Self&	operator = (Self &&)						__NE___ = default;
			Self&	operator = (const Self &)					__NE___ = default;

		ND_ auto	begin ()									C_NE___	{ return const_iterator{ _first, 0 }; }
		ND_ auto	end ()										C_NE___	{ return const_iterator{ null, 0 }; }

		ND_ auto*	FirstChunk ()								C_NE___	{ return _first; }
		ND_ auto*	operator -> ()								C_NE___	{ NonNull( _first );  return _first; }

		ND_ Self	LastChunk ()								C_NE___	{ Self res {_first};  res.MoveToLast();  return res; }
		ND_ Self	NextChunk ()								C_NE___;

			void	MoveToLast ()								__NE___	{ Base_t::_MoveToLast( INOUT _first ); }
		ND_ bool	IsLastChunk ()								C_NE___	{ return _first == null or _first->next == null; }

		ND_ bool	empty ()									C_NE___	{ return _first == null; }
		ND_ usize	Count ()									C_NE___	{ return Base_t::_Count( _first ); }
	};
//-----------------------------------------------------------------------------



/*
=================================================
	AddChunk
=================================================
*/
	template <typename T>
	ChunkList<T>  ChunkList<T>::AddChunk (void* ptr, Bytes size) __NE___
	{
		NonNull( ptr );
		ASSERT( size > 0 );

		auto*	chunk = Cast<Chunk>(ptr);
		chunk->next		= null;
		chunk->count	= 0;
		chunk->capacity = Chunk::CalcCapacity( size );
		ASSERT( chunk->capacity > 0 );

		Append( chunk );
		return Self{ chunk };
	}

	template <typename T>
	template <typename Allocator>
	ChunkList<T>  ChunkList<T>::AddChunk (Allocator &alloc, Bytes size) __NE___
	{
		void*	ptr = alloc.Allocate( SizeAndAlign{ size, AlignOf<Chunk> });
		return ptr != null ? AddChunk( ptr, size ) : Default;
	}

	template <typename T>
	template <typename Allocator>
	ChunkList<T>  ChunkList<T>::AddChunk (Allocator &alloc, const usize capacity) __NE___
	{
		ASSERT( capacity > 0 );
		ASSERT( capacity < MaxValue<Count_t>() );

		void*	ptr = alloc.Allocate( SizeAndAlign{ Chunk::CalcChunkSize( capacity ), AlignOf<Chunk> });
		if_likely( ptr != null ) {
			auto*	chunk	= Cast<Chunk>(ptr);
			chunk->next		= null;
			chunk->count	= 0;
			chunk->capacity = Count_t(capacity);

			Append( chunk );
			return Self{ chunk };
		}
		return Default;
	}

/*
=================================================
	Destroy
=================================================
*/
	template <typename T>
	template <typename Allocator>
	void  ChunkList<T>::Destroy (Allocator &alloc) __NE___
	{
		Chunk*	chunk	= _first;
				_first	= null;

		for (; chunk != null;)
		{
			Chunk*	ptr		= chunk;
			Bytes	size	= chunk->ChunkSize();

			chunk = chunk->next;

			ptr->~Chunk();
			alloc.Deallocate( ptr, SizeAndAlign{ size, AlignOf<Chunk> });
		}
	}

/*
=================================================
	Append
=================================================
*/
	template <typename T>
	void  ChunkList<T>::Append (ChunkPtr_t newChunk) __NE___
	{
		Chunk**		next = &_first;

		// find last chunk
		for (; *next != null; next = &(*next)->next) {}

		*next = newChunk;
	}

/*
=================================================
	LastChunk
=================================================
*/
	template <typename T>
	ChunkList<const T>  ChunkList<T>::LastChunk () C_NE___
	{
		ChunkList<const T>  res {_first};
		res.MoveToLast();
		return res;
	}

/*
=================================================
	NextChunk
=================================================
*/
	template <typename T>
	ChunkList<T>  ChunkList<T>::NextChunk () __NE___
	{
		return _first != null ? ChunkList<T>{ _first->next } : Default;
	}

	template <typename T>
	ChunkList<const T>  ChunkList<T>::NextChunk () C_NE___
	{
		return _first != null ? ChunkList<const T>{ _first->next } : Default;
	}

	template <typename T>
	ChunkList<const T>  ChunkList<const T>::NextChunk () C_NE___
	{
		return _first != null ? ChunkList<const T>{ _first->next } : Default;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	emplace_back
=================================================
*/
	template <typename T>
	T&  ChunkList<T>::Chunk::emplace_back () __NE___
	{
		ASSERT( this->count < this->capacity );
		if constexpr( std::is_trivially_constructible_v<T> )
			return this->_data [this->count++];	// skip ctor
		else
			return *PlacementNew<T>( OUT std::addressof( this->_data [this->count++] ));
	}

	template <typename T>
	template <typename T0, typename ...Types>
	T&  ChunkList<T>::Chunk::emplace_back (T0 &&arg0, Types&& ...args) __NE___
	{
		ASSERT( this->count < this->capacity );
		if constexpr( std::is_trivially_constructible_v<T> and CountOf<Types...>() == 0 )
			return (this->_data [this->count++] = FwdArg<T0>(arg0));
		else
			return *PlacementNew<T>( OUT std::addressof( this->_data[this->count++] ), FwdArg<T0>(arg0), FwdArg<Types>(args)... );
	}
//-----------------------------------------------------------------------------


	template <typename T>	struct TMemCopyAvailable< ChunkList<T> >	: CT_True {};
	template <typename T>	struct TZeroMemAvailable< ChunkList<T> >	: CT_True {};


} // AE::Base
