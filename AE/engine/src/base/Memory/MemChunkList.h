// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Byte.h"
#include "base/Memory/MemUtils.h"
#include "base/Memory/AllocatorFwdDecl.h"

namespace AE::Base
{

    //
    // List of Memory Chunks
    //

    class MemChunkList final
    {
    // types
    public:
        using Self  = MemChunkList;

    private:
      #if AE_PLATFORM_BITS == 64
        static constexpr uint   _SizeBits   = 36;               // max: 64 Gb
        static constexpr uint   _ChunkBits  = 64 - _SizeBits;   // * _Align, max: 16 Gb
      #else
        static constexpr uint   _SizeBits   = 32;               // max: 4 Gb
        static constexpr uint   _ChunkBits  = 32;               // * _Align, max: 4 Gb
      #endif
        static constexpr usize  _Align      = AE_CACHE_LINE;


        template <bool Const>
        struct alignas(_Align) _Chunk
        {
            using Ptr_t = Conditional< Const, _Chunk<Const> const*, _Chunk<Const> * >;

            Ptr_t   next = null;
            ubyte   _data [1];

            ND_ void*           Data ()     __NE___ { return _data; }
            ND_ void const*     Data ()     C_NE___ { return _data; }
        };


    public:
        using Chunk         = _Chunk<false>;
        using ConstChunk    = _Chunk<true>;


    // variables
    private:
        usize       _totalSize  : _SizeBits;
        usize       _chunkSize  : _ChunkBits;
        Chunk *     _first      = null;


    // methods
    public:
        MemChunkList ()                                     __NE___ : _totalSize{0}, _chunkSize{0} {}
        MemChunkList (const Self &)                         __NE___ = default;
        MemChunkList (Self &&)                              __NE___ = default;
        explicit MemChunkList (Bytes chunkSize)             __NE___;

            Self&   operator = (const Self &)                   __NE___ = default;
            Self&   operator = (Self &&)                        __NE___ = default;

        template <typename Allocator>
        ND_ Chunk*  AddChunk (Allocator &)                      __NE___;

        template <typename Allocator>
            void    Destroy (Allocator &)                       __NE___;

        ND_ bool    Append (Self view)                          __NE___;

            void    SetSize (Bytes newSize)                     __NE___ { ASSERT( newSize <= Capacity() );  _totalSize = usize{newSize}; }

        ND_ Bytes   Size ()                                     C_NE___ { return Bytes{_totalSize}; }
        ND_ Bytes   ChunkDataSize ()                            C_NE___ { return _ChunkSize() - sizeof(Chunk*); }
        ND_ usize   ChunkCount ()                               C_NE___;
        ND_ Bytes   Capacity ()                                 C_NE___ { return ChunkCount() * ChunkDataSize(); }

        ND_ Chunk*              First ()                        __NE___ { return _first; }
        ND_ ConstChunk const*   First ()                        C_NE___ { return BitCast< ConstChunk const* >(_first); }


    // Read //
        ND_ Bytes  Read (Bytes pos, OUT void* buffer, Bytes size)           C_NE___;

        template <typename T, typename A,
                  ENABLEIF( IsTriviallySerializable<T> )>
        ND_ bool  Read (Bytes pos, usize length, OUT BasicString<T,A> &str) C_NE___;

        template <typename T, typename A,
                  ENABLEIF( IsTriviallySerializable<T> )>
        ND_ bool  Read (Bytes pos, Bytes size, OUT BasicString<T,A> &str)   C_NE___;

        template <typename T, typename A,
                  ENABLEIF( IsTriviallySerializable<T> )>
        ND_ bool  Read (Bytes pos, usize count, OUT Array<T,A> &arr)        C_NE___;

        template <typename T, typename A,
                  ENABLEIF( IsTriviallySerializable<T> )>
        ND_ bool  Read (Bytes pos, Bytes size, OUT Array<T,A> &arr)         C_NE___;

        template <typename T,
                  ENABLEIF( IsTriviallySerializable<T> )>
        ND_ bool  Read (Bytes pos, OUT T &data)                             C_NE___;


    // Write //
        ND_ Bytes  Write (Bytes pos, const void* buffer, Bytes size)        __NE___;

        template <typename T,
                  ENABLEIF( IsTriviallySerializable<T> )>
        ND_ bool  Write (Bytes pos, ArrayView<T> arr)                       __NE___;

        template <typename T, typename A,
                  ENABLEIF( IsTriviallySerializable<T> )>
        ND_ bool  Write (Bytes pos, const BasicString<T,A> str)             __NE___;

        template <typename T,
                  ENABLEIF( IsTriviallySerializable<T> )>
        ND_ bool  Write (Bytes pos, BasicStringView<T> str)                 __NE___;

        template <typename T,
                  ENABLEIF( IsTriviallySerializable<T> )>
        ND_ bool  Write (Bytes pos, const T &data)                          __NE___;


    private:
            void    _Append (Chunk* newChunk)                   __NE___;
        ND_ Bytes   _ChunkSize ()                               C_NE___ { return Bytes{_chunkSize} * _Align; }
    };



/*
=================================================
    constructor
=================================================
*/
    inline MemChunkList::MemChunkList (Bytes chunkSize) __NE___ :
        _totalSize{0},
        _chunkSize{usize{ chunkSize / _Align }}
    {
        ASSERT( chunkSize == _ChunkSize() );
    }

/*
=================================================
    AddChunk
=================================================
*/
    template <typename Allocator>
    MemChunkList::Chunk*  MemChunkList::AddChunk (Allocator &alloc) __NE___
    {
        auto*   chunk = Cast<Chunk>( alloc.Allocate( SizeAndAlign{ _ChunkSize(), Bytes{_Align} }));
        if_likely( chunk != null )
        {
            chunk->next = null;
            _Append( chunk );
        }
        return chunk;
    }

/*
=================================================
    Destroy
=================================================
*/
    template <typename Allocator>
    void  MemChunkList::Destroy (Allocator &alloc) __NE___
    {
        Chunk*  chunk = _first;

        for (; chunk != null;)
        {
            Chunk*  ptr = chunk;
            chunk = chunk->next;

            //ptr->~Chunk();  // not needed
            alloc.Deallocate( ptr, SizeAndAlign{ _ChunkSize(), Bytes{_Align} });
        }

        _totalSize  = 0;
        _chunkSize  = 0;
        _first      = null;
    }

/*
=================================================
    Append
=================================================
*/
    inline bool  MemChunkList::Append (MemChunkList view) __NE___
    {
        CHECK_ERR( _ChunkSize() == view._ChunkSize() );

        _totalSize += view._totalSize;
        _Append( view._first );

        return true;
    }

    inline void  MemChunkList::_Append (Chunk* newChunk) __NE___
    {
        Chunk** next = &_first;
        for (; *next != null; next = &(*next)->next) {}
        *next = newChunk;
    }

/*
=================================================
    ChunkCount
=================================================
*/
    inline usize  MemChunkList::ChunkCount () C_NE___
    {
        auto*   chunk   = First();
        usize   count   = 0;

        for (; chunk != null; chunk = chunk->next, ++count) {}
        return count;
    }

/*
=================================================
    Read
=================================================
*/
    inline Bytes  MemChunkList::Read (Bytes pos, OUT void* const dst, const Bytes dstSize) C_NE___
    {
        CHECK_ERR( _first != null );
        CHECK_ERR( pos + dstSize <= Size() );
        CHECK_ERR( dst != null );

        auto*           chunk       = First();
        const Bytes     chunk_size  = ChunkDataSize();
        Bytes           dst_offset;

        for (; (chunk != null) and (pos > chunk_size);)
        {
            pos     -= chunk_size;
            chunk   = chunk->next;
        }

        ASSERT( chunk != null );
        {
            Bytes   size = Min( dstSize, chunk_size - pos );
            MemCopy( OUT dst, chunk->Data() + pos, size );

            chunk       = chunk->next;
            dst_offset  = size;
        }

        for (; (chunk != null) and (dst_offset < dstSize);)
        {
            Bytes   size = Min( dstSize - dst_offset, chunk_size );
            MemCopy( OUT dst + dst_offset, chunk->Data(), size );

            dst_offset  += size;
            chunk       = chunk->next;
        }

        return dst_offset;
    }

    template <typename T, typename A, ENABLEIF2( IsTriviallySerializable<T> )>
    bool  MemChunkList::Read (const Bytes pos, const usize length, OUT BasicString<T,A> &str) C_NE___
    {
        NOTHROW_ERR( str.resize( length ));

        const Bytes     expected_size   { sizeof(T) * length };
        const Bytes     current_size    = Read( pos, OUT str.data(), expected_size );

        NOTHROW_ERR( str.resize( usize(current_size / sizeof(T)) ));

        return str.length() == length;
    }

    template <typename T, typename A, ENABLEIF2( IsTriviallySerializable<T> )>
    bool  MemChunkList::Read (const Bytes pos, const Bytes size, OUT BasicString<T,A> &str) C_NE___
    {
        ASSERT( IsMultipleOf( size, sizeof(T) ));
        return Read( pos, usize(size) / sizeof(T), OUT str );
    }

    template <typename T, typename A, ENABLEIF2( IsTriviallySerializable<T> )>
    bool  MemChunkList::Read (const Bytes pos, const usize count, OUT Array<T,A> &arr) C_NE___
    {
        NOTHROW_ERR( arr.resize( count ));

        const Bytes     expected_size   { sizeof(arr[0]) * arr.size() };
        const Bytes     current_size    = Read( pos, OUT arr.data(), expected_size );

        NOTHROW_ERR( arr.resize( usize(current_size / sizeof(arr[0])) ));

        return arr.size() == count;
    }

    template <typename T, typename A, ENABLEIF2( IsTriviallySerializable<T> )>
    bool  MemChunkList::Read (const Bytes pos, const Bytes size, OUT Array<T,A> &arr) C_NE___
    {
        ASSERT( IsMultipleOf( size, sizeof(T) ));
        return Read( pos, usize(size) / sizeof(T), OUT arr );
    }

    template <typename T, ENABLEIF2( IsTriviallySerializable<T> )>
    bool  MemChunkList::Read (const Bytes pos, OUT T &data) C_NE___
    {
        return Read( pos, OUT AddressOf(data), Sizeof(data) );
    }

/*
=================================================
    Write
=================================================
*/
    inline Bytes  MemChunkList::Write (Bytes pos, const void* const src, const Bytes srcSize) __NE___
    {
        CHECK_ERR( _first != null );
        CHECK_ERR( pos + srcSize <= Size() );
        CHECK_ERR( src != null );

        auto*           chunk       = First();
        const Bytes     chunk_size  = ChunkDataSize();
        Bytes           src_offset;

        for (; (chunk != null) and (pos > chunk_size);)
        {
            pos     -= chunk_size;
            chunk   = chunk->next;
        }

        ASSERT( chunk != null );
        {
            Bytes   size = Min( srcSize, chunk_size - pos );
            MemCopy( OUT chunk->Data() + pos, src, size );

            chunk       = chunk->next;
            src_offset  = size;
        }

        for (; (chunk != null) and (src_offset < srcSize);)
        {
            Bytes   size = Min( srcSize - src_offset, chunk_size );
            MemCopy( OUT chunk->Data(), src + src_offset, size );

            src_offset  += size;
            chunk       = chunk->next;
        }

        return src_offset;
    }

    template <typename T, ENABLEIF2( IsTriviallySerializable<T> )>
    bool  MemChunkList::Write (const Bytes pos, ArrayView<T> arr) __NE___
    {
        if_unlikely( arr.empty() )
            return true;

        const Bytes     size { sizeof(arr[0]) * arr.size() };

        return Write( pos, arr.data(), size ) == size;
    }

    template <typename T, typename A, ENABLEIF2( IsTriviallySerializable<T> )>
    bool  MemChunkList::Write (const Bytes pos, const BasicString<T,A> str) __NE___
    {
        return Write( pos, BasicStringView<T>{ str });
    }

    template <typename T, ENABLEIF2( IsTriviallySerializable<T> )>
    bool  MemChunkList::Write (const Bytes pos, BasicStringView<T> str) __NE___
    {
        if_unlikely( str.empty() )
            return true;

        const Bytes     size { sizeof(str[0]) * str.length() };

        return Write( pos, str.data(), size ) == size;
    }

    template <typename T, ENABLEIF2( IsTriviallySerializable<T> )>
    bool  MemChunkList::Write (const Bytes pos, const T &data) __NE___
    {
        return Write( pos, AddressOf(data), Sizeof(data) ) == Sizeof(data);
    }


} // AE::Base
