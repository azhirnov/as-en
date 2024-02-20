// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "CPP_VM/Common.h"

namespace LFAS
{

    //
    // Memory Ranges
    //

    template <typename ThreadID>
    class MemRangesTempl
    {
    // types
    public:
        using ThreadID_t        = ThreadID;
        using Version_t         = ulong;
        using PerThreadVer_t    = HashMap< ThreadID_t, Version_t >;

        struct Range
        {
            Bytes           begin;
            Bytes           end;
        };

        struct RangeWithVersion : Range
        {
            Version_t       verCounter      = 0;
            Version_t       globalVersion   = UMax; // data in global memory
            PerThreadVer_t  unavailable;            // new data that stored in cache and should be flushed
            PerThreadVer_t  visible;                // data that loaded from global memory to cache (invalidated)
        };

        using Ranges_t  = Array< RangeWithVersion >;

        static constexpr Version_t  InitialVer = 0;


    // variables
    public:
        Ranges_t        _ranges;


    // methods
    public:
        MemRangesTempl () {}

        bool  Init (Bytes size, ThreadID_t tid);
        bool  Write (Bytes begin, Bytes end, ThreadID_t tid);
        bool  Read (Bytes begin, Bytes end, ThreadID_t tid);

        void  Acquire (ThreadID_t tid);
        void  Release (ThreadID_t tid);
        void  AcquireRelease (ThreadID_t tid);

        void  CheckUnreleased (ThreadID_t tid) const;

        ND_ uint  UniqueThreadCount () const;

        ND_ typename Ranges_t::iterator  FindFirst (const Range &otherRange);

        ND_ static bool  IsIntersects (const Range &lhs, const Range &rhs);
    };


/*
=================================================
    FindFirst
=================================================
*/
    template <typename TID>
    inline typename MemRangesTempl<TID>::Ranges_t::iterator  MemRangesTempl<TID>::FindFirst (const Range &otherRange)
    {
    #if 0
        usize   left    = 0;
        usize   right   = _ranges.size();

        for (; left < right; )
        {
            usize   mid = (left + right) >> 1;

            if ( _ranges[mid].end < otherRange.begin )
                left = mid + 1;
            else
                right = mid;
        }

        if ( left < _ranges.size() and _ranges[left].end >= otherRange.begin )
            return _ranges.begin() + left;

        return _ranges.end();
    #else

        for (usize i = 0; i < _ranges.size(); ++i)
        {
            if ( _ranges[i].end <= otherRange.begin )
                continue;

            return _ranges.begin() + i;
        }
        return _ranges.end();
    #endif
    }

/*
=================================================
    IsIntersects
=================================================
*/
    template <typename TID>
    inline bool  MemRangesTempl<TID>::IsIntersects (const Range &lhs, const Range &rhs)
    {
        return not ((lhs.end < rhs.begin) or (lhs.begin > rhs.end));
    }

/*
=================================================
    Init
=================================================
*/
    template <typename TID>
    inline bool  MemRangesTempl<TID>::Init (Bytes size, ThreadID_t tid)
    {
        _ranges.clear();

        CHECK_ERR( size > 0 );

        auto&       r   = _ranges.emplace_back();
        r.begin             = 0_b;
        r.end               = size;
        r.globalVersion     = InitialVer;
        r.verCounter        = InitialVer + 1;
        r.visible[tid]      = r.verCounter;
        r.unavailable[tid]  = r.verCounter;

        return true;
    }

/*
=================================================
    Write
=================================================
*/
    template <typename TID>
    inline bool  MemRangesTempl<TID>::Write (Bytes begin, Bytes end, ThreadID_t tid)
    {
        CHECK_ERR( begin < end );

        Range   range   { begin, end };
        auto    iter    = FindFirst( range );

        if ( iter        == _ranges.end()   or
             iter->begin >  range.begin     or
             iter->end   <  range.begin )
        {
            RETURN_ERR( "outside of range!" );
        }

        for (; iter != _ranges.end() and iter->begin < range.end;)
        {
            ASSERT( IsIntersects( *iter, range ));
            ASSERT( range.begin >= iter->begin );

            //  |000000|...  +    (iter)
            //     |******|  =    (range)
            //  |00|...
            if ( iter->begin < range.begin )
            {
                RangeWithVersion    part = *iter;
                part.begin  = range.begin;
                part.end    = iter->end;
                iter->end   = range.begin;

                iter = _ranges.insert( iter + 1, part );
                continue;
            }

            if ( iter->begin == range.begin )
            {
                //  |00000|...  +    (iter)
                //  |********|  =    (range)
                //  |*****|...
                if ( iter->end <= range.end )
                {
                    iter->verCounter++;
                    iter->visible[tid]      = iter->verCounter;
                    iter->unavailable[tid]  = iter->verCounter;

                    CHECK( iter->verCounter != UMax );

                    range.begin = iter->end;
                    ++iter;
                    continue;
                }

                //  |00000000|...  +    (iter)
                //  |****|         =    (range)
                //  |****|000|...
                else
                {
                    RangeWithVersion    part = *iter;
                    part.begin              = iter->begin;
                    part.end                = range.end;
                    part.verCounter++;
                    part.visible[tid]       = part.verCounter;
                    part.unavailable[tid]   = part.verCounter;

                    CHECK( part.verCounter != UMax );

                    iter->begin = range.end;
                    range.begin = range.end;

                    iter = _ranges.insert( iter, part ) + 1;
                    break;
                }
            }

            ASSERT(false);
            break;
        }

        CHECK_ERR( range.begin >= range.end );
        return true;
    }

/*
=================================================
    Read
=================================================
*/
    template <typename TID>
    inline bool  MemRangesTempl<TID>::Read (Bytes begin, Bytes end, ThreadID_t tid)
    {
        CHECK_ERR( begin < end );

        Range   range   { begin, end };
        auto    iter    = FindFirst( range );

        if ( iter        == _ranges.end()   or
             iter->begin >  range.begin     or
             iter->end   <  range.begin )
        {
            RETURN_ERR( "outside of range!" );
        }

        for (; iter != _ranges.end() and iter->begin < range.end; ++iter)
        {
            ASSERT( IsIntersects( *iter, range ));

            const usize is_local = iter->unavailable.count( tid );

            if ( (iter->unavailable.size() - is_local) > 0 )
            {
                RETURN_ERR( "data race detected: changes in another thread must be flushed before reading!" );
            }

            auto    tid_iter = iter->visible.find( tid );

            if ( tid_iter == iter->visible.end() )
            {
                RETURN_ERR( "data is not visible!" );
            }
            else
            if ( (not is_local) and (tid_iter->second != iter->globalVersion) )
            {
                RETURN_ERR( "local data is different from global memory!" );
            }

            if ( tid_iter->second == InitialVer )
            {
                RETURN_ERR( "reading from uninitialized memory!" );
            }
        }

        return true;
    }

/*
=================================================
    Acquire
=================================================
*/
    template <typename TID>
    inline void  MemRangesTempl<TID>::Acquire (ThreadID_t tid)
    {
        for (auto& r : _ranges)
        {
            auto    iter = r.unavailable.find( tid );

            auto[r_iter, inserted] = r.visible.insert({ tid, r.globalVersion });

            if ( iter != r.unavailable.end() )
            {
                if ( r.globalVersion > iter->second )
                {
                    AE_LOGE( "data race detected: global memory will override uncommited local changes!" );

                    r_iter->second = r.globalVersion;
                }
                else
                    r_iter->second = iter->second;
            }
            else
            if ( not inserted )
                r_iter->second = Max( r_iter->second, r.globalVersion );
        }
    }

/*
=================================================
    Release
=================================================
*/
    template <typename TID>
    inline void  MemRangesTempl<TID>::Release (ThreadID_t tid)
    {
        for (auto& r : _ranges)
        {
            if ( auto iter = r.unavailable.find( tid );  iter != r.unavailable.end() )
            {
                r.globalVersion = iter->second;
                r.unavailable.erase( iter );
            }
        }
    }

/*
=================================================
    AcquireRelease
=================================================
*/
    template <typename TID>
    inline void  MemRangesTempl<TID>::AcquireRelease (ThreadID_t tid)
    {
        for (auto& r : _ranges)
        {
            if ( auto iter = r.unavailable.find( tid );  iter != r.unavailable.end() )
            {
                r.globalVersion = iter->second;
                r.unavailable.erase( iter );
            }

            CHECK( r.globalVersion != InitialVer );

            r.visible[tid] = r.globalVersion;
        }
    }

/*
=================================================
    CheckUnreleased
=================================================
*/
    template <typename TID>
    inline void  MemRangesTempl<TID>::CheckUnreleased (ThreadID_t tid) const
    {
        for (auto& r : _ranges)
        {
            if ( auto iter = r.unavailable.find( tid );  iter != r.unavailable.end() )
            {
                AE_LOGE( "local changes must be flushed!" );
            }
        }
    }

/*
=================================================
    UniqueThreadCount
=================================================
*/
    template <typename TID>
    inline uint  MemRangesTempl<TID>::UniqueThreadCount () const
    {
        HashSet< ThreadID_t >   threads;

        for (auto& r : _ranges)
        {
            for (auto& vis : r.visible)
            {
                threads.insert( vis.first );
            }
        }
        return uint(threads.size());
    }


} // LFAS
