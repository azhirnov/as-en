// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/IAllocator.h"
#include "base/Algorithms/StringUtils.h"
using namespace AE;
using namespace AE::Base;



struct UntypedAllocatorWithCounter
{
// types
public:
    static constexpr bool   IsThreadSafe = false;

    static constexpr Bytes  MaxAlign {sizeof(void*)};


// variables
private:
    uint            _allocCount     = 0;
    uint            _deallocCount   = 0;
    const String    _name;


// methods
public:
    explicit UntypedAllocatorWithCounter (StringView name) : _name{name} {}

    ~UntypedAllocatorWithCounter ()
    {
        ASSERT( _allocCount == _deallocCount );
        AE_LOGI( String{_name} << " allocations: "s << Base::ToString(_allocCount) );
    }

    ND_ void*  Allocate (Bytes size)
    {
        ++_allocCount;
        return ::operator new( usize(size), std::nothrow_t{} );
    }

    ND_ void*  Allocate (const SizeAndAlign sizeAndAlign)
    {
        CHECK( sizeAndAlign.align <= MaxAlign );
        ++_allocCount;
        return ::operator new( usize(sizeAndAlign.size), std::nothrow_t{} );
    }

    void  Deallocate (void* ptr)
    {
        if ( ptr != null )
            ++_deallocCount;
        ::operator delete( ptr, std::nothrow_t() );
    }

    // deallocation with explicit size may be faster
    void  Deallocate (void* ptr, Bytes size)
    {
        if ( ptr != null )
            ++_deallocCount;
        ::operator delete( ptr, usize(size) );
    }

    void  Deallocate (void* ptr, const SizeAndAlign sizeAndAlign)
    {
        CHECK( sizeAndAlign.align <= MaxAlign );
        if ( ptr != null )
            ++_deallocCount;
        ::operator delete( ptr, usize(sizeAndAlign.size) );
    }

    ND_ bool  operator == (const UntypedAllocatorWithCounter &) const
    {
        return false;
    }
};


template <typename T>
using StdAllocWithCounter = StdAllocatorRef< T, Ptr<UntypedAllocatorWithCounter> >;

