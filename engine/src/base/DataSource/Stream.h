// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Stream is used for sequential data access.
*/

#pragma once

#include "base/Containers/ArrayView.h"
#include "base/Memory/MemUtils.h"
#include "base/DataSource/DataSource.h"

namespace AE::Base
{

    //
    // Read-only Stream
    //

    class RStream : public IDataSource
    {
    // types
    public:
        struct PosAndSize
        {
            Bytes   pos;
            Bytes   size;   // UMax if 'FixedSize' flag is not returned by 'GetSourceType()'

            PosAndSize ()                       __NE___ {}
            PosAndSize (Bytes pos, Bytes size)  __NE___ : pos{pos}, size{size} {}

            ND_ Bytes  Remaining ()             C_NE___ { return size - pos; }
        };


    // interface
    public:
        ND_ virtual PosAndSize  PositionAndSize ()                                              C_NE___ = 0;

            virtual bool        SeekFwd (Bytes offset)                                          __NE___ = 0;

        // returns size of readn data
        ND_ virtual Bytes       ReadSeq (OUT void *buffer, Bytes size)                          __NE___ = 0;

            virtual bool        SeekSet (Bytes newPos)                                          __NE___;

            virtual void        UpdateFastStream (OUT const void* &begin, OUT const void* &end) __NE___;
            virtual void        EndFastStream (const void* ptr)                                 __NE___;
        ND_ virtual Bytes       GetFastStreamPosition (const void* ptr)                         __NE___;


    // methods
    public:
        RStream ()                                                                              __NE___ {}


        // IDataSource //
            ESourceType     GetSourceType ()                                                    C_NE_OV { return ESourceType::SequentialAccess | ESourceType::ReadAccess; }

        ND_ Bytes           Position ()                                                         C_NE___ { return PositionAndSize().pos; }
        ND_ Bytes           Size ()                                                             C_NE___ { return PositionAndSize().size; }
        ND_ Bytes           RemainingSize ()                                                    C_NE___ { auto tmp = PositionAndSize();  return tmp.Remaining(); }


        ND_ bool  Read (OUT void *buffer, Bytes size)                                           __NE___;

        template <typename T, typename A>
        ND_ EnableIf<IsTrivial<T>, bool>  Read (usize length, OUT BasicString<T,A> &str)        __NE___;

        template <typename T, typename A>
        ND_ EnableIf<IsTrivial<T>, bool>  Read (Bytes size, OUT BasicString<T,A> &str)          __NE___;

        template <typename T, typename A>
        ND_ EnableIf<IsTrivial<T>, bool>  Read (usize count, OUT Array<T,A> &arr)               __NE___;

        template <typename T, typename A>
        ND_ EnableIf<IsTrivial<T>, bool>  Read (Bytes size, OUT Array<T,A> &arr)                __NE___;

        template <typename T>
        ND_ EnableIf<IsTrivial<T>, bool>  Read (OUT T &data)                                    __NE___;
    };



    //
    // Write-only Stream
    //

    class WStream : public IDataSource
    {
    // interface
    public:
        // returns remaining size
        ND_ virtual Bytes   Reserve (Bytes additionalSize)                              __NE___ = 0;

        ND_ virtual Bytes   Position ()                                                 C_NE___ = 0;        // same as 'Size()'

            virtual bool    SeekFwd (Bytes offset)                                      __NE___ = 0;

        // returns size of written data
        ND_ virtual Bytes   WriteSeq (const void *buffer, Bytes size)                   __NE___ = 0;

            virtual void    Flush ()                                                    __NE___ = 0;

            virtual void    UpdateFastStream (OUT void* &begin, OUT const void* &end,
                                              Bytes reserve = DefaultAllocationSize)    __NE___;
            virtual void    EndFastStream (const void* ptr)                             __NE___;
        ND_ virtual Bytes   GetFastStreamPosition (const void* ptr)                     __NE___;


    // methods
    public:
        WStream ()                                                                      __NE___ {}


        // IDataSource //
        ESourceType     GetSourceType ()                                                C_NE_OV { return ESourceType::SequentialAccess | ESourceType::WriteAccess; }


        ND_ bool  Write (const void *buffer, Bytes size)                                __NE___;

        template <typename T>
        ND_ EnableIf<IsTrivial<T>, bool>  Write (ArrayView<T> buf)                      __NE___;

        template <typename T, typename A>
        ND_ EnableIf<IsTrivial<T>, bool>  Write (const BasicString<T,A> str)            __NE___;

        template <typename T>
        ND_ EnableIf<IsTrivial<T>, bool>  Write (BasicStringView<T> str)                __NE___;

        template <typename T>
        ND_ EnableIf<IsTrivial<T>, bool>  Write (const T &data)                         __NE___;
    };
//-----------------------------------------------------------------------------



/*
=================================================
    SeekSet
=================================================
*/
    inline bool  RStream::SeekSet (Bytes newPos) __NE___
    {
        const Bytes pos = Position();
        if ( newPos >= pos )
            return SeekFwd( newPos - pos );
        return false;
    }

/*
=================================================
    UpdateFastStream / EndFastStream / GetFastStreamPosition
=================================================
*/
    inline void  RStream::UpdateFastStream (OUT const void* &begin, OUT const void* &end) __NE___
    {
        begin   = null;
        end     = null;
    }

    inline void  RStream::EndFastStream (const void* ptr) __NE___
    {
        Unused( ptr );
    }

    inline Bytes  RStream::GetFastStreamPosition (const void* ptr) __NE___
    {
        Unused( ptr );
        return 0_b;
    }

/*
=================================================
    Read
=================================================
*/
    inline bool  RStream::Read (OUT void *buffer, Bytes size) __NE___
    {
        return ReadSeq( buffer, size ) == size;
    }

    template <typename T, typename A>
    EnableIf<IsTrivial<T>, bool>  RStream::Read (usize length, OUT BasicString<T,A> &str) __NE___
    {
        CATCH_ERR( str.resize( length ));

        Bytes   expected_size   { sizeof(str[0]) * str.length() };
        Bytes   current_size    = ReadSeq( str.data(), expected_size );

        str.resize( usize(current_size / sizeof(str[0])) );     // nothrow

        return str.length() == length;
    }

    template <typename T, typename A>
    EnableIf<IsTrivial<T>, bool>  RStream::Read (Bytes size, OUT BasicString<T,A> &str) __NE___
    {
        ASSERT( IsAligned( size, sizeof(T) ));
        return Read( usize(size) / sizeof(T), OUT str );
    }

    template <typename T, typename A>
    EnableIf<IsTrivial<T>, bool>  RStream::Read (usize count, OUT Array<T,A> &arr) __NE___
    {
        CATCH_ERR( arr.resize( count ));

        Bytes   expected_size   { sizeof(arr[0]) * arr.size() };
        Bytes   current_size    = ReadSeq( arr.data(), expected_size );

        arr.resize( usize(current_size / sizeof(arr[0])) );     // nothrow

        return arr.size() == count;
    }

    template <typename T, typename A>
    EnableIf<IsTrivial<T>, bool>  RStream::Read (Bytes size, OUT Array<T,A> &arr) __NE___
    {
        ASSERT( IsAligned( size, sizeof(T) ));
        return Read( usize(size) / sizeof(T), OUT arr );
    }

    template <typename T>
    EnableIf<IsTrivial<T>, bool>  RStream::Read (OUT T &data) __NE___
    {
        return ReadSeq( AddressOf(data), Sizeof(data) ) == Sizeof(data);
    }
//-----------------------------------------------------------------------------



/*
=================================================
    UpdateFastStream / EndFastStream / GetFastStreamPosition
=================================================
*/
    inline void  WStream::UpdateFastStream (OUT void* &begin, OUT const void* &end, Bytes reserve) __NE___
    {
        Unused( reserve );
        begin   = null;
        end     = null;
    }

    inline void  WStream::EndFastStream (const void* ptr) __NE___
    {
        Unused( ptr );
    }

    inline Bytes  WStream::GetFastStreamPosition (const void* ptr) __NE___
    {
        Unused( ptr );
        return 0_b;
    }

/*
=================================================
    Write
=================================================
*/
    inline bool  WStream::Write (const void *buffer, Bytes size) __NE___
    {
        return WriteSeq( buffer, size ) == size;
    }

    template <typename T>
    EnableIf<IsTrivial<T>, bool>  WStream::Write (ArrayView<T> buf) __NE___
    {
        Bytes   size { sizeof(buf[0]) * buf.size() };

        return WriteSeq( buf.data(), size ) == size;
    }

    template <typename T, typename A>
    EnableIf<IsTrivial<T>, bool>  WStream::Write (const BasicString<T,A> str) __NE___
    {
        return Write( BasicStringView<T>{ str });
    }

    template <typename T>
    EnableIf<IsTrivial<T>, bool>  WStream::Write (BasicStringView<T> str) __NE___
    {
        if ( str.empty() )
            return true;

        Bytes   size { sizeof(str[0]) * str.length() };

        return WriteSeq( str.data(), size ) == size;
    }

    template <typename T>
    EnableIf<IsTrivial<T>, bool>  WStream::Write (const T &data) __NE___
    {
        return WriteSeq( AddressOf(data), Sizeof(data) ) == Sizeof(data);
    }


} // AE::Base

