// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "UnitTest_Shared.h"


template <typename T, usize UID>
struct DebugInstanceCounter
{
    inline static slong     _counter        = 0;
    inline static slong     _emptyCtorCnt   = 0;
    inline static slong     _copyCtorCnt    = 0;
    inline static slong     _moveCtorCnt    = 0;
    inline static slong     _dtorCnt        = 0;

    slong                   _magicNumber;
    T                       value;

    DebugInstanceCounter () noexcept : _magicNumber( _counter + (1ll << 62)), value{}
    {
        TEST( _magicNumber != 0 );
        ++_counter;
        ++_emptyCtorCnt;
    }

    explicit DebugInstanceCounter (const T &val) noexcept : DebugInstanceCounter()
    {
        value = val;
    }

    DebugInstanceCounter (const DebugInstanceCounter &other) noexcept : _magicNumber{ other._magicNumber }, value{ other.value }
    {
        ++_counter;
        ++_copyCtorCnt;
    }

    DebugInstanceCounter (DebugInstanceCounter &&other) noexcept : _magicNumber{ other._magicNumber }, value{ RVRef(other.value) }
    {
        ++_counter;
        ++_moveCtorCnt;
    }

    ~DebugInstanceCounter () noexcept
    {
        TEST( _magicNumber != 0 );  // assert on second dtor call
        --_counter;
        ++_dtorCnt;
        _magicNumber = 0;
    }

    DebugInstanceCounter& operator = (const DebugInstanceCounter &right) noexcept
    {
        _magicNumber = right._magicNumber;
        value = right.value;
        return *this;
    }

    DebugInstanceCounter& operator = (DebugInstanceCounter &&right) noexcept
    {
        _magicNumber = right._magicNumber;
        value = RVRef( right.value );
        return *this;
    }

    bool operator == (const DebugInstanceCounter &right) const noexcept
    {
        return value == right.value;
    }

    bool operator != (const DebugInstanceCounter &right) const noexcept
    {
        return value != right.value;
    }

    bool operator < (const DebugInstanceCounter &right) const noexcept
    {
        return value < right.value;
    }

    bool operator > (const DebugInstanceCounter &right) const noexcept
    {
        return value > right.value;
    }


    static bool CheckStatistic () noexcept
    {
        String  str;
        str << "\nObjectID:    " << ToString( UID )
            << "\nObj counter: " << ToString( _counter )        // objects which is not destroyed
            << "\nEmpty ctors: " << ToString( _emptyCtorCnt )
            << "\nCopy ctors:  " << ToString( _copyCtorCnt )
            << "\nMove ctors:  " << ToString( _moveCtorCnt )
            << "\nDtors:       " << ToString( _dtorCnt );

        AE_LOGI( str );
        return _counter == 0;
    }

    static void ClearStatistic () noexcept
    {
        _counter        = 0;
        _emptyCtorCnt   = 0;
        _copyCtorCnt    = 0;
        _moveCtorCnt    = 0;
        _dtorCnt        = 0;
    }
};


namespace std
{

    template <typename T, size_t UID>
    struct hash< DebugInstanceCounter<T,UID> >
    {
        ND_ size_t  operator () (const DebugInstanceCounter<T,UID> &value) const noexcept {
            return hash<T>{}( value.value );
        }
    };

}   // std
