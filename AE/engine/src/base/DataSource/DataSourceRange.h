// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/DataSource/DataSource.h"

namespace AE::Base
{

    //
    // Read-only Data Source Range
    //

    template <typename T>
    class RDataSourceRange final : public RDataSource
    {
    // variables
    private:
        T               _dataSource;
        const Bytes     _offset;
        const Bytes     _size;

        static constexpr ESourceType    _TypeMask = ESourceType::RandomAccess | ESourceType::FixedSize | ESourceType::ReadAccess;


    // methods
    public:
        template <typename B>
        explicit RDataSourceRange (B &&dataSource) __NE___ :
            _dataSource{ FwdArg<B>(dataSource) },
            _offset{ 0_b },
            _size{ _dataSource ? _dataSource->Size() : 0_b }
        {
            ASSERT( not IsOpen() or AllBits( _dataSource->GetSourceType(), _TypeMask ));
        }

        template <typename B>
        RDataSourceRange (B &&dataSource, Bytes offset, Bytes size) __NE___ :
            _dataSource{ FwdArg<B>(dataSource) },
            _offset{ Min( offset, _dataSource ? _dataSource->Size() : 0_b )},
            _size{ Min( size, _dataSource ? (_dataSource->Size() - _offset) : 0_b )}
        {
            ASSERT( not IsOpen() or AllBits( _dataSource->GetSourceType(), _TypeMask ));
        }


        // RDataSource //
        bool        IsOpen ()                           C_NE_OV { return _dataSource and _dataSource->IsOpen(); }
        ESourceType GetSourceType ()                    C_NE_OV;
        Bytes       Size ()                             C_NE_OV { return _size; }

        Bytes       ReadBlock (Bytes, OUT void*, Bytes) __NE_OV;
    };



    //
    // Write-only Data Source Range
    //

    template <typename T>
    class WDataSourceRange final : public WDataSource
    {
    // variables
    private:
        T               _dataSource;
        const Bytes     _offset;
        const Bytes     _capacity;      // TODO

        static constexpr ESourceType    _TypeMask = ESourceType::RandomAccess | ESourceType::WriteAccess;


    // methods
    public:
        template <typename B>
        explicit WDataSourceRange (B &&dataSource) __Th___ :
            _dataSource{ FwdArg<B>(dataSource) },   // throw
            _offset{ 0_b },
            _capacity{ _dataSource ? _dataSource->Capacity() : 0_b }
        {
            ASSERT( not IsOpen() or AllBits( _dataSource->GetSourceType(), _TypeMask ));
        }

        template <typename B>
        WDataSourceRange (B &&dataSource, Bytes offset, Bytes capacity) __Th___ :
            _dataSource{ FwdArg<B>(dataSource) },   // throw
            _offset{ Min( offset, _dataSource ? _dataSource->Capacity() : 0_b )},
            _capacity{ Min( capacity, _dataSource ? (_dataSource->Capacity() - _offset) : 0_b )}
        {
            ASSERT( not IsOpen() or AllBits( _dataSource->GetSourceType(), _TypeMask ));
        }


        // WDataSource //
        bool        IsOpen ()                               C_NE_OV { return _dataSource and _dataSource->IsOpen(); }
        ESourceType GetSourceType ()                        C_NE_OV;
        Bytes       Capacity ()                             C_NE_OV { return _capacity; }

        Bytes       WriteBlock (Bytes, const void*, Bytes)  __NE_OV;

        void        Flush ()                                __NE_OV { ASSERT( IsOpen() );  return _dataSource->Flush(); }
    };
//-----------------------------------------------------------------------------



/*
=================================================
    GetSourceType
=================================================
*/
    template <typename T>
    IDataSource::ESourceType  RDataSourceRange<T>::GetSourceType () C_NE___
    {
        ASSERT( IsOpen() );
        return  ESourceType::SequentialAccess   | ESourceType::RandomAccess |
                ESourceType::FixedSize          | ESourceType::ReadAccess   |
                (_dataSource->GetSourceType() & ESourceType::ThreadSafe);
    }

/*
=================================================
    ReadBlock
=================================================
*/
    template <typename T>
    Bytes  RDataSourceRange<T>::ReadBlock (Bytes pos, OUT void* buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        size = Min( pos + size, _size ) - pos;

        return _dataSource->ReadBlock( _offset + pos, OUT buffer, size );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    GetSourceType
=================================================
*/
    template <typename T>
    IDataSource::ESourceType  WDataSourceRange<T>::GetSourceType () C_NE___
    {
        ASSERT( IsOpen() );
        return  ESourceType::SequentialAccess   | ESourceType::RandomAccess |
                ESourceType::WriteAccess        |
                (_dataSource->GetSourceType() & ESourceType::ThreadSafe);
    }

/*
=================================================
    WriteBlock
=================================================
*/
    template <typename T>
    Bytes  WDataSourceRange<T>::WriteBlock (Bytes pos, const void* buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );
        return _dataSource->WriteBlock( _offset + pos, buffer, size );
    }


} // AE::Base
