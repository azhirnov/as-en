// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/MemDataSource.h"
#include "base/DataSource/MemStream.h"

namespace AE::Base
{

/*
=================================================
    constructor
=================================================
*/
    MemRefRDataSource::MemRefRDataSource (const void* ptr, Bytes size) __NE___ :
        _dataPtr{ptr}, _size{size}
    {
        ASSERT( _dataPtr != null );
        ASSERT( _size > 0 );
    }

/*
=================================================
    _Set
=================================================
*/
    void  MemRefRDataSource::_Set (const void* ptr, Bytes size) __NE___
    {
        ASSERT( ptr != null );
        ASSERT( size > 0 );

        _dataPtr    = ptr;
        _size       = size;
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  MemRefRDataSource::GetSourceType () C_NE___
    {
        return  ESourceType::SequentialAccess   | ESourceType::RandomAccess |
                ESourceType::Buffered           | ESourceType::FixedSize    |
                ESourceType::ReadAccess;
    }

/*
=================================================
    ReadBlock
=================================================
*/
    Bytes  MemRefRDataSource::ReadBlock (Bytes offset, OUT void *buffer, Bytes size) __NE___
    {
        size = Min( offset + size, _size ) - offset;

        MemCopy( OUT buffer, _dataPtr + offset, size );

        return size;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    MemRDataSource::MemRDataSource (Array<ubyte> data) __NE___ : _data{RVRef(data)}
    {
        _Set( _data.data(), ArraySizeOf(_data) );
    }

    MemRDataSource::MemRDataSource (const void* ptr, Bytes size) __NE___
    {
        CATCH( _data.assign( Cast<ubyte>(ptr), Cast<ubyte>(ptr + size) ));

        _Set( _data.data(), ArraySizeOf(_data) );
    }

/*
=================================================
    Decompress
=================================================
*/
    bool  MemRDataSource::Decompress (RStream &srcStream) __NE___
    {
        MemWStream  dst;
        Bytes       size = DataSourceUtils::BufferedCopy( dst, srcStream );

        _data = dst.ReleaseData();
        _Set( _data.data(), size );

        return size > 0_b;
    }

/*
=================================================
    Load
=================================================
*/
    bool  MemRDataSource::Load (RStream &srcStream, Bytes offset, Bytes dataSize) __NE___
    {
        CHECK_ERR( srcStream.SeekSet( offset ));

        MemWStream  dst;
        Bytes       size = DataSourceUtils::BufferedCopy( dst, srcStream, dataSize );

        _data = dst.ReleaseData();
        _Set( _data.data(), size );

        return size > 0_b;
    }

    bool  MemRDataSource::Load (RDataSource &srcDS, Bytes offset, Bytes dataSize) __NE___
    {
        MemWStream  dst;
        Bytes       size = DataSourceUtils::BufferedCopy( dst, srcDS, offset, dataSize );

        _data = dst.ReleaseData();
        _Set( _data.data(), size );

        return size > 0_b;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    MemWDataSource::MemWDataSource () __NE___
    {
        CATCH( _data.reserve( usize(DefaultAllocationSize) ));
    }

    MemWDataSource::MemWDataSource (Array<ubyte> data, Bytes maxSize) __NE___ :
        _data{ RVRef(data) },
        _maxSize{ Max( maxSize, _data.size() )}
    {}

    MemWDataSource::MemWDataSource (Bytes bufferSize, Bytes maxSize) __NE___ :
        _maxSize{ maxSize }
    {
        CATCH( _data.reserve( usize(bufferSize) ));
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  MemWDataSource::GetSourceType () C_NE___
    {
        return  ESourceType::SequentialAccess   | ESourceType::RandomAccess |
                ESourceType::Buffered           | ESourceType::WriteAccess;
    }

/*
=================================================
    Reserve
=================================================
*/
    Bytes  MemWDataSource::Reserve (Bytes capacity) __NE___
    {
        CATCH( _data.reserve( Min( Max( _data.capacity(), usize(capacity) ), usize(_maxSize) )));

        return Bytes{ _data.capacity() };
    }

/*
=================================================
    WriteBlock
=================================================
*/
    Bytes  MemWDataSource::WriteBlock (Bytes offset, const void *buffer, Bytes size) __NE___
    {
        size = Min( offset + size, _maxSize ) - offset;

        CATCH_ERR( _data.resize( Max( _data.size(), usize(offset + size) )));

        MemCopy( OUT _data.data() + offset, buffer, size );
        return size;
    }

/*
=================================================
    Clear
=================================================
*/
    void  MemWDataSource::Clear () __NE___
    {
        _data.clear();
    }

/*
=================================================
    Store
=================================================
*/
    bool  MemWDataSource::Store (WStream &dstFile) C_NE___
    {
        MemRefRDataSource   src{ GetData() };

        return DataSourceUtils::BufferedCopy( dstFile, src ) == Bytes{_data.size()};
    }


} // AE::Base
