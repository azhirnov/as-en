// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    FrameUID
        thread-safe:    no

    AtomicFrameUID
        thread-safe:    yes
*/

#pragma once

#include "base/Utils/Threading.h"

namespace AE::Base
{

    //
    // Frame Unique ID
    //
    class FrameUID
    {
        friend class AtomicFrameUID;

    // types
    private:
        // ulong - 58 bits enough for 360 Ghz * 9.3e+9 days
        //       - 28 bits enough for 360 Ghz * 8.6 hours
        // uint  - 26 bits enough for 360 Ghz * 2.1 hours

        using Value_t   = ulong;
        using SValue_t  = ToSignedInteger<Value_t>;

        static constexpr uint   _MaxFrames      = 4;    // GraphicsConfig::MaxFrames
        static constexpr uint   _FrameBits      = CT_IntLog2< _MaxFrames >;
        static constexpr uint   _MaxFrameBits   = _FrameBits + 1;
        static constexpr uint   _CounterBits    = CT_SizeOfInBits<Value_t> - _FrameBits - _MaxFrameBits;

        StaticAssert( ToBit<uint>( _FrameBits ) == _MaxFrames );
        StaticAssert( ToBitMask<uint>( _MaxFrameBits ) >= _MaxFrames );

        enum class _EFrameUID : Value_t {};

        struct _Bits
        {
            Value_t     counter     : _CounterBits;     // unique frame index,
            Value_t     index       : _FrameBits;       // non-unique frame index in range [0 .. GraphicsCreateInfo::MaxFrames)
            Value_t     maxFrames   : _MaxFrameBits;    // value in range [0 .. GraphicsCreateInfo::MaxFrames]
        };

        StaticAssert( sizeof(_Bits) == sizeof(Value_t) );
        StaticAssert( _CounterBits >= 28 );


    // variables
    private:
        union {
            _Bits       _bits;
            Value_t     _value  = 0;
        };


    // methods
    private:
        explicit FrameUID (Value_t uid, Value_t idx, Value_t maxFrames) __NE___ : _bits{ uid, idx, maxFrames } {}

    public:
        FrameUID ()                                                     __NE___ {}

        FrameUID (const FrameUID &)                                     __NE___ = default;
        FrameUID (FrameUID &&)                                          __NE___ = default;

        FrameUID&  operator = (const FrameUID &)                        __NE___ = default;
        FrameUID&  operator = (FrameUID &&)                             __NE___ = default;

        ND_ bool  operator == (const FrameUID &rhs)                     C_NE___ { return _value == rhs._value; }
        ND_ bool  operator != (const FrameUID &rhs)                     C_NE___ { return _value != rhs._value; }

        ND_ bool  operator >  (const FrameUID &rhs)                     C_NE___ { ASSERT( _bits.maxFrames == rhs._bits.maxFrames );  return _bits.counter > rhs._bits.counter; }
        ND_ bool  operator <  (const FrameUID &rhs)                     C_NE___ { ASSERT( _bits.maxFrames == rhs._bits.maxFrames );  return _bits.counter < rhs._bits.counter; }

        ND_ bool  operator <=  (const FrameUID &rhs)                    C_NE___ { return not (*this > rhs); }
        ND_ bool  operator >=  (const FrameUID &rhs)                    C_NE___ { return not (*this < rhs); }


        ND_ Value_t     Unique ()                                       C_NE___ { return _bits.counter; }
        ND_ uint        Index ()                                        C_NE___ { return uint(_bits.index); }
        ND_ uint        PrevIndex ()                                    C_NE___;
        ND_ uint        NextIndex ()                                    C_NE___;
        ND_ uint        MaxFrames ()                                    C_NE___ { return uint(_bits.maxFrames); }
        ND_ bool        IsValid ()                                      C_NE___ { return _bits.maxFrames > 0; }

        ND_ FrameUID    Next ()                                         C_NE___ { return FrameUID{*this}.Inc(); }
        ND_ FrameUID    NextCycle ()                                    C_NE___ { return FrameUID{ _bits.counter + _bits.maxFrames, _bits.index, MaxFrames() }; }
        ND_ SValue_t    Diff (FrameUID rhs)                             C_NE___ { return SValue_t(_bits.counter) - SValue_t(rhs._bits.counter); }

        template <typename T> ND_ T  Remap (T maxValue)                 C_NE___ { ASSERT_NE( MaxFrames(), maxValue );  return T(_bits.counter % maxValue); }
        template <typename T> ND_ T  Remap (T bias, T maxValue)         C_NE___ { return T((_bits.counter + bias) % maxValue); }

        // for double buffering, returns 0 or 1
        ND_ uint        Remap2 (Value_t bias = 0)                       C_NE___ { return uint((_bits.counter + bias) & 1); }

        FrameUID&       Inc ()                                          __NE___;

        ND_ uint        DistanceToIndex (uint idx)                      C_NE___ { return DistanceBetweenIndices( Index(), idx, MaxFrames() ); }
        ND_ uint        RevDistanceToIndex (uint idx)                   C_NE___ { return DistanceBetweenIndices( idx, Index(), MaxFrames() ); }

        ND_ Optional<FrameUID>  Sub (uint delta)                        C_NE___;
        ND_ Optional<FrameUID>  PrevCycle ()                            C_NE___ { return Sub( MaxFrames() ); }

        ND_ static FrameUID  FromIndex (uint idx, uint maxFrames)       __NE___;
        ND_ static FrameUID  FromUnique (Value_t uid, uint maxFrames)   __NE___;
        ND_ static FrameUID  Init (uint maxFrames)                      __NE___;

        ND_ static uint  DistanceBetweenIndices (uint, uint, uint)      __NE___;

        ND_ static constexpr uint  MaxFramesLimit ()                    __NE___ { return _MaxFrames; }
    };



    //
    // Atomic Frame Unique ID
    //
    class AtomicFrameUID final : public Noncopyable
    {
    // types
    private:
        using Value_t   = FrameUID::Value_t;
        using Self      = AtomicFrameUID;


    // variables
    private:
        Atomic< Value_t >   _value {0};


    // methods
    public:
        AtomicFrameUID ()                           __NE___ {}

        ND_ FrameUID    load ()                     C_NE___ { return BitCast<FrameUID>( _value.load() ); }
        void            store (FrameUID value)      __NE___;

        FrameUID        Inc ()                      __NE___;
    };
//-----------------------------------------------------------------------------



/*
=================================================
    PrevIndex / NextIndex
=================================================
*/
    inline uint  FrameUID::PrevIndex () C_NE___
    {
        return  uint(_bits.index ? _bits.index : _bits.maxFrames) - 1;
    }

    inline uint  FrameUID::NextIndex () C_NE___
    {
        return  uint(_bits.index+1 == _bits.maxFrames ? 0u : _bits.index+1);
    }

/*
=================================================
    Inc
=================================================
*/
    inline FrameUID&  FrameUID::Inc () __NE___
    {
        ASSERT( IsValid() );
        _bits.index = NextIndex();
        ++_bits.counter;
        ASSERT( (_bits.counter % MaxFrames()) == _bits.index );
        return *this;
    }

/*
=================================================
    Sub
=================================================
*/
    inline Optional<FrameUID>  FrameUID::Sub (uint delta) C_NE___
    {
        if_likely( _bits.counter >= delta )
        {
            FrameUID    id{ _bits.counter - delta, 0, MaxFrames() };
            id._bits.index = id._bits.counter % MaxFrames();
            ASSERT( id.Unique() < Unique() );
            return {id};
        }
        return NullOptional;
    }

/*
=================================================
    DistanceBetweenIndices
----
    Returns number of frames between 'lhs.Index()' and 'rhs.Index()'.
    Returns 'maxFrames' if indices are equal.
=================================================
*/
    inline uint  FrameUID::DistanceBetweenIndices (const uint lhs, const uint rhs, const uint maxFrames) __NE___
    {
        ASSERT( maxFrames > 0 );
        ASSERT( lhs < maxFrames );
        ASSERT( rhs < maxFrames );

        return lhs < rhs ?  rhs - lhs :
                            maxFrames - (lhs - rhs);
    }

/*
=================================================
    From*
=================================================
*/
    inline FrameUID  FrameUID::FromIndex (uint idx, uint maxFrames) __NE___
    {
        ASSERT( maxFrames <= _MaxFrames );
        ASSERT( idx < maxFrames );
        return FrameUID{ 0, idx, maxFrames };
    }

    inline FrameUID  FrameUID::FromUnique (Value_t uid, uint maxFrames) __NE___
    {
        ASSERT( maxFrames <= _MaxFrames );
        return FrameUID{ uid, uid % maxFrames, maxFrames };
    }

    inline FrameUID  FrameUID::Init (uint maxFrames) __NE___
    {
        ASSERT( maxFrames > 0 );
        ASSERT( maxFrames <= _MaxFrames );
        return FrameUID{ 0, 0, maxFrames };
    }
//-----------------------------------------------------------------------------



/*
=================================================
    store
=================================================
*/
    inline void  AtomicFrameUID::store (FrameUID value) __NE___
    {
        #ifdef AE_DEBUG
        FrameUID    old = BitCast<FrameUID>( _value.exchange( BitCast<Value_t>( value )));
        ASSERT( value == Default or old.Unique() <= value.Unique() );
        #else
        _value.store( BitCast<Value_t>( value ));
        #endif
    }

/*
=================================================
    Inc
=================================================
*/
    inline FrameUID  AtomicFrameUID::Inc () __NE___
    {
        Value_t new_val;
        for (Value_t exp = _value.load();;)
        {
            new_val = BitCast<Value_t>( BitCast<FrameUID>( exp ).Inc() );

            if_likely( _value.CAS( INOUT exp, new_val ))
                break;

            ThreadUtils::Pause();
        }
        return BitCast<FrameUID>( new_val );
    }


} // AE::Base
