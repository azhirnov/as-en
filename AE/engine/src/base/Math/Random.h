// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Color.h"

namespace AE::Math
{
namespace _hidden_
{
    template <typename T, bool IsUnsigned>
    struct Random_Uniform_IntType2
    {
        StaticAssert( IsUnsignedInteger<T> );
        using type = ByteSizeToUInt< Clamp( sizeof(T), 2u, 8u )>;
    };

    template <typename T>
    struct Random_Uniform_IntType2 <T, false>
    {
        StaticAssert( IsSignedInteger<T> );
        using type = ByteSizeToInt< Clamp( sizeof(T), 2u, 8u )>;
    };

    template <typename T>
    struct Random_Uniform_IntType {
        using type = typename Random_Uniform_IntType2< T, IsUnsignedInteger<T> >::type;
    };

} // _hidden_



    //
    // Random
    //

    struct Random final
    {
    // variables
    private:
        std::mt19937    _gen;


    // methods
    public:
        Random ()                                                           __NE___ : _gen{std::random_device{}()} {}


    // Uniform //
        template <typename T>
        ND_ EnableIf<IsFloatPoint<T>, T>  Uniform (T min, T max)            __NE___
        {
            StaticAssert( IsScalar<T> );
            return std::uniform_real_distribution<T>{ min, max }( _gen );
        }

        template <typename T>
        ND_ EnableIf<IsInteger<T>, T>  Uniform (T min, T max)               __NE___
        {
            StaticAssert( IsScalar<T> );
            return std::uniform_int_distribution<T>{ min, max }( _gen );
        }

        template <typename T, int I>
        ND_ Vec<T,I>  Uniform (const Vec<T,I> &min, const Vec<T,I> &max)    __NE___
        {
            Vec<T,I>    ret;
            for (int i = 0; i < I; ++i) {
                ret[i] = Uniform( min[i], max[i] );
            }
            return ret;
        }

        template <typename T>
        ND_ EnableIf<IsFloatPoint<T>, T>  Uniform ()                        __NE___
        {
            return Uniform( T{0}, T{1} );
        }

        template <typename T>
        ND_ EnableIf<IsInteger<T>, T>  Uniform ()                           __NE___
        {
            using IT = typename Math::_hidden_::Random_Uniform_IntType<T>::type;
            return T( Uniform<IT>( Base::MinValue<T>(), Base::MaxValue<T>() ));
        }

        ND_ RGBA32f  UniformColor ()                                        __NE___
        {
            return RGBA32f{ HSVColor{ Uniform( 0.f, 0.75f )}};
        }


    // Bernoulli //
        // 'p' - the p distribution parameter (probability of generating true)
        ND_ bool   Bernoulli (double p)                                     __NE___ { return std::bernoulli_distribution{p}( _gen ); }
        ND_ bool2  Bernoulli2 (double p)                                    __NE___ { return {Bernoulli(p), Bernoulli(p)}; }
        ND_ bool3  Bernoulli3 (double p)                                    __NE___ { return {Bernoulli(p), Bernoulli(p), Bernoulli(p)}; }
        ND_ bool4  Bernoulli4 (double p)                                    __NE___ { return {Bernoulli(p), Bernoulli(p), Bernoulli(p), Bernoulli(p)}; }


    // Index //
        template <typename Container, typename T>
        ND_ usize  Index (const Container &chances, T value)                __NE___
        {
            for (usize i = 0; i < chances.size(); ++i)
            {
                if ( value <= chances[i] )
                    return i;

                value -= chances[i];
            }
            return chances.size();
        }


    // Binomial //
        template <typename R>
        struct _Binomial;

        template <typename Result, typename IntType>
        ND_ auto  Binomial (IntType trials, double probability)             __NE___ { return _Binomial< Result >{ *this, trials, probability }; }


    // Normal //
        template <typename R>
        struct _Normal;

        template <typename R>
        struct _Normal2;

        template <typename Result, typename T>
        ND_ auto  Normal (T mean, T sigma)                                  __NE___ { return _Normal< Result >{ *this, mean, sigma }; }

        template <typename Result, typename T>
        ND_ auto  Normal (T min, T max, T expected)                         __NE___ { return _Normal2< Result >{ *this, min, max, expected }; }
    };



    //
    // Binomial
    //

    template <typename T>
    struct Random::_Binomial
    {
        friend struct Random;
        StaticAssert( IsInteger<T> and IsScalar<T> );

    // variables
    private:
        std::binomial_distribution<T>   _dist;
        Random &                        _rnd;


    // methods
    private:
        _Binomial (Random &rnd, T t, double p)                              __NE___ : _dist{ t, p }, _rnd{rnd} {}

    public:
        _Binomial (const _Binomial &)                                       __NE___ = default;
        _Binomial& operator = (const _Binomial &)                                   = delete;

        _Binomial (_Binomial &&)                                            __NE___ = default;
        _Binomial& operator = (_Binomial &&)                                __NE___ = default;

        ND_ T  operator () ()                                               __NE___ { return _dist( _rnd._gen ); }
    };


    template <typename T, int I>
    struct Random::_Binomial< Vec<T,I> >
    {
        friend struct Random;
        StaticAssert( IsInteger<T> and IsScalar<T> );

    // variables
    private:
        std::binomial_distribution<T>   _dist;
        Random &                        _rnd;


    // methods
    private:
        _Binomial (Random &rnd, T t, double p)                              __NE___ : _dist{ t, p }, _rnd{rnd} {}

    public:
        _Binomial (const _Binomial &)                                       __NE___ = default;
        _Binomial& operator = (const _Binomial &)                                   = delete;

        _Binomial (_Binomial &&)                                            __NE___ = default;
        _Binomial& operator = (_Binomial &&)                                __NE___ = default;

        ND_ Vec<T,I>  operator () ()                                        __NE___
        {
            Vec<T,I>    ret;
            for (int i = 0; i < I; ++i) {
                ret[i] = _dist( _rnd._gen );
            }
            return ret;
        }
    };



    //
    // Normal
    //

    template <typename T>
    struct Random::_Normal
    {
        friend struct Random;
        StaticAssert( IsFloatPoint<T> and IsScalar<T> );

    // variables
    private:
        std::normal_distribution<T>     _dist;
        Random &                        _rnd;


    // methods
    private:
        _Normal (Random& rnd, T mean, T stddev)                             __NE___ : _dist{ mean, stddev }, _rnd{rnd} {}

    public:
        _Normal (const _Normal &)                                           __NE___ = default;
        _Normal& operator = (const _Normal &)                                       = delete;

        _Normal (_Normal &&)                                                __NE___ = default;
        _Normal& operator = (_Normal &&)                                    __NE___ = default;

        ND_ T  operator () ()                                               __NE___ { return _dist( _rnd._gen ); }
    };


    template <typename T, int I>
    struct Random::_Normal< Vec<T,I> >
    {
        friend struct Random;
        StaticAssert( IsFloatPoint<T> and IsScalar<T> );

    // variables
    private:
        std::normal_distribution<T>     _dist;
        Random &                        _rnd;


    // methods
    private:
        _Normal (Random& rnd, T mean, T stddev)                             __NE___ : _dist{ mean, stddev }, _rnd{rnd} {}

    public:
        _Normal (const _Normal &)                                           __NE___ = default;
        _Normal& operator = (const _Normal &)                                       = delete;

        _Normal (_Normal &&)                                                __NE___ = default;
        _Normal& operator = (_Normal &&)                                    __NE___ = default;

        ND_ Vec<T,I>  operator () ()                                        __NE___
        {
            Vec<T,I>    ret;
            for (int i = 0; i < I; ++i) {
                ret[i] = _dist( _rnd._gen );
            }
            return ret;
        }
    };



    //
    // Normal2
    //

    template <typename T>
    struct Random::_Normal2
    {
        friend struct Random;
        StaticAssert( IsFloatPoint<T> and IsScalar<T> );

    // variables
    private:
        std::normal_distribution<T>     _dist;
        Random &                        _rnd;
        const T                         _min;
        const T                         _max;


    // methods
    private:
        _Normal2 (Random& rnd, T min, T max, T expected)                    __NE___ :
            _dist{ expected, (max - min) / T(8) },
            _rnd{rnd}, _min{min}, _max{max}
        {}

    public:
        _Normal2 (const _Normal2 &)                                                 = delete;
        _Normal2& operator = (const _Normal2 &)                                     = delete;

        _Normal2 (_Normal2 &&)                                              __NE___ = default;
        _Normal2& operator = (_Normal2 &&)                                  __NE___ = default;

        ND_ T  operator () ()                                               __NE___
        {
            for (;;)
            {
                T ret = _dist( _rnd._gen );

                if ( ret < _min or ret > _max ) // TODO: wtf?
                    continue;

                return ret;
            }
        }
    };


    template <typename T, int I>
    struct Random::_Normal2< Vec<T,I> >
    {
        friend struct Random;

    // variables
    private:
        std::normal_distribution<T>     _dist;
        Random &                        _rnd;
        const T                         _min;
        const T                         _max;


    // methods
    private:
        _Normal2 (Random& rnd, T min, T max, T expected)                    __NE___ :
            _dist{ expected, (max - min) / T(8) },
            _rnd{rnd}, _min{min}, _max{max}
        {}

    public:
        _Normal2 (const _Normal2 &)                                                 = delete;
        _Normal2& operator = (const _Normal2 &)                                     = delete;

        _Normal2 (_Normal2 &&)                                              __NE___ = default;
        _Normal2& operator = (_Normal2 &&)                                  __NE___ = default;

        ND_ Vec<T,I>  operator () ()                                        __NE___
        {
            Vec<T,I>    ret;
            for (int i = 0; i < I;)
            {
                ret[i] = _dist( _rnd._gen );

                if ( ret[i] < _min or ret[i] > _max )   // TODO: wtf?
                    continue;

                ++i;
            }
            return ret;
        }
    };


} // AE::Math
