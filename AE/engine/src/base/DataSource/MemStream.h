// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/DataSource/Stream.h"
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
        const void*     _dataPtr    = null;
        Bytes           _pos;
        Bytes           _size;


    // methods
    public:
        MemRefRStream (const void* ptr, Bytes size)                             __NE___;

        explicit MemRefRStream (StringView data)                                __NE___ : MemRefRStream{data.data(), StringSizeOf(data)} {}

        template <typename T>
        explicit MemRefRStream (ArrayView<T> data)                              __NE___ : MemRefRStream{data.data(), ArraySizeOf(data)} {}


        // RStream //
        bool        IsOpen ()                                                   C_NE_OF { return true; }
        PosAndSize  PositionAndSize ()                                          C_NE_OF { return { _pos, _size }; }

        ESourceType GetSourceType ()                                            C_NE_OF;

        bool    SeekSet (Bytes pos)                                             __NE_OF;
        bool    SeekFwd (Bytes offset)                                          __NE_OF;

        Bytes   ReadSeq (OUT void* buffer, Bytes size)                          __NE_OF;

        void    UpdateFastStream (OUT const void* &begin, OUT const void* &end) __NE_OF;
        void    EndFastStream (const void* ptr)                                 __NE_OF;
        Bytes   GetFastStreamPosition (const void* ptr)                         __NE_OF;


        ND_ RC<MemRefRStream>   ToSubStream (Bytes offset, Bytes size)          C_Th___;

        ND_ ArrayView<ubyte>    GetData ()                                      C_NE___ { return ArrayView<ubyte>{ Cast<ubyte>(_dataPtr), usize(_size) }; }
        ND_ ArrayView<ubyte>    GetRemainData ()                                C_NE___ { return GetData().section( usize(_pos), UMax ); }

    protected:
        MemRefRStream ()                                                        __NE___ {}
        void  _Set (const void* ptr, Bytes size)                                __NE___;
    };



    //
    // Memory read-only Stream
    //

    class MemRStream final : public MemRefRStream
    {
    // variables
    private:
        Array<ubyte>    _data;

    // methods
    public:
        MemRStream ()                                                           __NE___ {}
        explicit MemRStream (Array<ubyte> data)                                 __NE___;
        MemRStream (const void* ptr, Bytes size)                                __NE___;

        ND_ bool  Decompress (RStream &srcStream)                               __NE___;

        ND_ bool  LoadRemaining (RStream &srcStream, Bytes size = UMax)         __NE___;

        ND_ bool  Load (RStream &srcStream, Bytes offset, Bytes size)           __NE___;
        ND_ bool  Load (RDataSource &srcDS, Bytes offset, Bytes size)           __NE___;

        ND_ bool  LoadAll (RStream &srcStream)                                  __NE___ { return Load( srcStream, 0_b, UMax ); }
        ND_ bool  LoadAll (RDataSource &srcDS)                                  __NE___ { return Load( srcDS, 0_b, UMax ); }
    };



    //
    // Memory Write-only Stream
    //

    class MemWStream final : public WStream
    {
    // variables
    private:
        Array<ubyte>    _data;
        Bytes           _pos;


    // methods
    public:
        MemWStream ()                                                           __NE___;
        explicit MemWStream (Bytes bufferSize)                                  __NE___;


        // WStream //
        bool        IsOpen ()                                                   C_NE_OV { return true; }
        Bytes       Position ()                                                 C_NE_OV { return _pos; }
        ESourceType GetSourceType ()                                            C_NE_OV;

        bool        SeekFwd (Bytes offset)                                      __NE_OV;
        Bytes       Reserve (Bytes additionalSize)                              __NE_OV;
        Bytes       WriteSeq (const void*, Bytes)                               __NE_OV;
        void        Flush ()                                                    __NE_OV {}

        void        UpdateFastStream (OUT void* &begin, OUT const void* &end,
                                      Bytes reserve = DefaultAllocationSize)    __NE_OV;

        void        EndFastStream (const void* ptr)                             __NE_OV;
        Bytes       GetFastStreamPosition (const void* ptr)                     __NE_OF;


            void    Clear ()                                                    __NE___;
        ND_ bool    Store (WStream &dstFile)                                    C_NE___;

        ND_ ArrayView<ubyte>    GetData ()                                      C_NE___ { return ArrayView<ubyte>{ _data.data(), usize(_pos) }; }
        ND_ Array<ubyte>        ReleaseData ()                                  __NE___;
        ND_ RC<MemRefRStream>   ToRStream ()                                    C_NE___ { return MakeRC<MemRefRStream>( GetData() ); }
    };


} // AE::Base
