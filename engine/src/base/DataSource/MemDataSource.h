// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/DataSource/Stream.h"
#include "base/DataSource/DataSourceAsStream.h"
#include "base/Algorithms/ArrayUtils.h"

namespace AE::Base
{

    //
    // Memory Reference read-only Data Source
    //

    class MemRefRDataSource : public RDataSource
    {
    // variables
    private:
        const void*     _dataPtr    = null;
        Bytes           _size;


    // methods
    public:
        MemRefRDataSource (const void* ptr, Bytes size) __NE___;

        explicit MemRefRDataSource (StringView data)    __NE___ : MemRefRDataSource{ data.data(), StringSizeOf(data) } {}

        template <typename T>
        explicit MemRefRDataSource (ArrayView<T> data)  __NE___ : MemRefRDataSource{ data.data(), ArraySizeOf(data) } {}


        // RDataSource //
        bool        IsOpen ()                           C_NE_OF { return _dataPtr != null; }
        ESourceType GetSourceType ()                    C_NE_OF;
        Bytes       Size ()                             C_NE_OF { return _size; }

        Bytes       ReadBlock (Bytes, OUT void *, Bytes)__NE_OF;


        ND_ ArrayView<ubyte>    GetData ()              C_NE___ { return ArrayView<ubyte>{ Cast<ubyte>(_dataPtr), usize(_size) }; }


    protected:
        MemRefRDataSource ()                            __NE___ {}
        void  _Set (const void* ptr, Bytes size)        __NE___;
    };



    //
    // Memory read-only Data Source
    //

    class MemRDataSource final : public MemRefRDataSource
    {
    // variables
    private:
        Array<ubyte>    _data;


    // methods
    public:
        MemRDataSource ()                                               __NE___ {}
        explicit MemRDataSource (Array<ubyte> data)                     __NE___;
        MemRDataSource (const void* ptr, Bytes size)                    __NE___;

        ND_ bool  Decompress (RStream &srcFile)                         __NE___;

        ND_ bool  Load (RStream &srcStream, Bytes offset, Bytes size)   __NE___;
        ND_ bool  Load (RDataSource &srcDS, Bytes offset, Bytes size)   __NE___;

        ND_ bool  LoadAll (RStream &srcStream)                          __NE___ { return Load( srcStream, 0_b, UMax ); }
        ND_ bool  LoadAll (RDataSource &srcDS)                          __NE___ { return Load( srcDS, 0_b, UMax ); }
    };



    //
    // Write-only In Memory Data Source
    //

    class MemWDataSource final : public WDataSource
    {
    // variables
    private:
        Array<ubyte>    _data;
        Bytes           _maxSize    = UMax;


    // methods
    public:
        MemWDataSource ()                           __NE___;

        explicit MemWDataSource (Array<ubyte> data, Bytes maxSize = 0_b)    __NE___;
        explicit MemWDataSource (Bytes bufferSize, Bytes maxSize = UMax)    __NE___;


        // WDataSource //
        bool        IsOpen ()                       C_NE_OV { return true; }
        ESourceType GetSourceType ()                C_NE_OV;
        Bytes       Capacity ()                     C_NE_OV { return Bytes{_data.size()}; }

        Bytes       Reserve (Bytes capacity)        __NE_OV;
        Bytes       WriteBlock (Bytes, const void *, Bytes) __NE_OV;

        void        Flush ()                        __NE_OV {}


            void    Clear ()                        __NE___;
        ND_ bool    Store (WStream &dstFile)        C_NE___;

        ND_ ArrayView<ubyte>    GetData ()          C_NE___ { return ArrayView<ubyte>{ _data.data(), usize(_data.size()) }; }
        ND_ Array<ubyte>        ReleaseData ()      __NE___ { auto temp = RVRef(_data);  return temp; }
    };


} // AE::Base
