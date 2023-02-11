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
		STATIC_ASSERT( IsUnsignedInteger<T> );
		using type = ByteSizeToUInt< Clamp( sizeof(T), 2u, 8u )>;
	};

	template <typename T>
	struct Random_Uniform_IntType2 <T, false>
	{
		STATIC_ASSERT( IsSignedInteger<T> );
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

	struct Random
	{
	private:
		std::mt19937	_gen;

	public:
		Random () __NE___ : _gen{std::random_device{}()} {}


	// Uniform //
		template <typename T>
		ND_ EnableIf<IsFloatPoint<T>, T>  Uniform (T min, T max) __NE___
		{
			STATIC_ASSERT( IsScalar<T> );
			return std::uniform_real_distribution<T>{ min, max }( _gen );
		}
		
		template <typename T>
		ND_ EnableIf<IsInteger<T>, T>  Uniform (T min, T max) __NE___
		{
			STATIC_ASSERT( IsScalar<T> );
			return std::uniform_int_distribution<T>{ min, max }( _gen );
		}

		template <typename T, int I>
		ND_ Vec<T,I>  Uniform (const Vec<T,I> &min, const Vec<T,I> &max) __NE___
		{
			Vec<T,I>	ret;
			for (int i = 0; i < I; ++i) {
				ret[i] = Uniform( min[i], max[i] );
			}
			return ret;
		}

		template <typename T>
		ND_ EnableIf<IsFloatPoint<T>, T>  Uniform () __NE___
		{
			return Uniform( T{0}, T{1} );
		}

		template <typename T>
		ND_ EnableIf<IsInteger<T>, T>  Uniform () __NE___
		{
			using IT = typename Math::_hidden_::Random_Uniform_IntType<T>::type;
			return T( Uniform<IT>( Base::MinValue<T>(), Base::MaxValue<T>() ));
		}

		ND_ RGBA32f  UniformColor () __NE___
		{
			return RGBA32f{ HSVColor{ Uniform( 0.f, 0.75f )}};
		}


	// Bernoulli //
		// 'p' - the p distribution parameter (probability of generating true)
		ND_ bool   Bernoulli (double p)		__NE___	{ return std::bernoulli_distribution{p}( _gen ); }
		ND_ bool2  Bernoulli2 (double p)	__NE___	{ return {Bernoulli(p), Bernoulli(p)}; }
		ND_ bool3  Bernoulli3 (double p)	__NE___	{ return {Bernoulli(p), Bernoulli(p), Bernoulli(p)}; }
		ND_ bool4  Bernoulli4 (double p)	__NE___	{ return {Bernoulli(p), Bernoulli(p), Bernoulli(p), Bernoulli(p)}; }


	// Index //
		template <typename Container, typename T>
		ND_ usize  Index (const Container &chances, T value) __NE___
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
		template <typename R, typename I>
		struct _Binomial;

		template <typename Result, typename IntType>
		ND_ auto  Binomial (IntType trials, double probability) __NE___
		{
			return _Binomial< Result, IntType >{ *this, trials, probability };
		}


	// Normal //
		template <typename R, typename T>
		struct _Normal;
		
		template <typename R, typename T>
		struct _Normal2;

		template <typename Result, typename T>
		ND_ auto  Normal (T mean, T sigma) __NE___
		{
			return _Normal< Result, T >{ *this, mean, sigma };
		}
		
		template <typename Result, typename T>
		ND_ auto  Normal (T min, T max, T expected) __NE___
		{
			return _Normal2< Result, T >{ *this, min, max, expected };
		}
	};
	

	
	//
	// Binomial
	//

	template <typename R, typename IT>
	struct Random::_Binomial
	{
		friend struct Random;

	private:
		std::binomial_distribution<IT>	_dist;
		Random &						_rnd;

		_Binomial (Random &rnd, IT t, double p)		__NE___ : _dist{ t, p }, _rnd{rnd} {}

	public:
		_Binomial (const _Binomial &)						= delete;
		_Binomial& operator = (const _Binomial &)			= delete;

		_Binomial (_Binomial &&)					__NE___	= default;
		_Binomial& operator = (_Binomial &&)		__NE___	= default;

		ND_ R  operator () ()						C_NE___
		{
			return _dist( _rnd._gen );
		}
	};
	

	template <typename R, int I, typename IT>
	struct Random::_Binomial< Vec<R,I>, IT >
	{
		friend struct Random;

	private:
		std::binomial_distribution<IT>	_dist;
		Random &						_rnd;

		_Binomial (Random &rnd, IT t, double p)		__NE___ : _dist{ t, p }, _rnd{rnd} {}

	public:
		_Binomial (const _Binomial &)						= delete;
		_Binomial& operator = (const _Binomial &)			= delete;

		_Binomial (_Binomial &&)					__NE___	= default;
		_Binomial& operator = (_Binomial &&)		__NE___	= default;

		ND_ Vec<R,I>  operator () ()				C_NE___
		{
			Vec<R,I>	ret;
			for (int i = 0; i < I; ++i) {
				ret[i] = _dist( _rnd._gen );
			}
			return ret;
		}
	};

	

	//
	// Normal
	//

	template <typename R, typename T>
	struct Random::_Normal
	{
		friend struct Random;

	private:
		mutable std::normal_distribution<T>		_dist;
		Random &								_rnd;

		_Normal (Random& rnd, T mean, T stddev) __NE___	: _dist{ mean, stddev }, _rnd{rnd} {}

	public:
		_Normal (const _Normal &)						= delete;
		_Normal& operator = (const _Normal &)			= delete;

		_Normal (_Normal &&)					__NE___	 = default;
		_Normal& operator = (_Normal &&)		__NE___	= default;
		
		ND_ R  operator () ()					C_NE___
		{
			return _dist( _rnd._gen );
		}
	};
	

	template <typename R, int I, typename T>
	struct Random::_Normal< Vec<R,I>, T >
	{
		friend struct Random;

	private:
		mutable std::normal_distribution<T>		_dist;
		Random &								_rnd;

		_Normal (Random& rnd, T mean, T stddev) __NE___	: _dist{ mean, stddev }, _rnd{rnd} {}

	public:
		_Normal (const _Normal &)						= delete;
		_Normal& operator = (const _Normal &)			= delete;

		_Normal (_Normal &&)					__NE___	= default;
		_Normal& operator = (_Normal &&)		__NE___	= default;
		
		ND_ Vec<R,I>  operator () ()			C_NE___
		{
			Vec<R,I>	ret;
			for (int i = 0; i < I; ++i) {
				ret[i] = _dist( _rnd._gen );
			}
			return ret;
		}
	};

	

	//
	// Normal
	//

	template <typename R, typename T>
	struct Random::_Normal2
	{
		friend struct Random;

	private:
		mutable std::normal_distribution<T>		_dist;
		Random &								_rnd;
		const T									_min;
		const T									_max;

		_Normal2 (Random& rnd, T min, T max, T expected) __NE___ :
			_dist{ expected, (max - min) / T(8) }, _rnd{rnd}, _min{min}, _max{max} {}

	public:
		_Normal2 (const _Normal2 &)							= delete;
		_Normal2& operator = (const _Normal2 &)				= delete;

		_Normal2 (_Normal2 &&)						__NE___	= default;
		_Normal2& operator = (_Normal2 &&)			__NE___	= default;
		
		ND_ R  operator () ()						C_NE___
		{
			for (;;)
			{
				T ret = _dist( _rnd._gen );

				if ( ret < _min or ret > _max )
					continue;

				return ret;
			}
		}
	};
	

	template <typename R, int I, typename T>
	struct Random::_Normal2< Vec<R,I>, T >
	{
		friend struct Random;

	private:
		mutable std::normal_distribution<T>		_dist;
		Random &								_rnd;
		const T									_min;
		const T									_max;

		_Normal2 (Random& rnd, T min, T max, T expected) __NE___ :
			_dist{ expected, (max - min) / T(8) }, _rnd{rnd}, _min{min}, _max{max} {}

	public:
		_Normal2 (const _Normal2 &)						= delete;
		_Normal2& operator = (const _Normal2 &)			= delete;

		_Normal2 (_Normal2 &&)					__NE___	= default;
		_Normal2& operator = (_Normal2 &&)		__NE___	= default;
		
		ND_ Vec<R,I>  operator () ()			C_NE___
		{
			Vec<R,I>	ret;
			for (int i = 0; i < I;)
			{
				ret[i] = _dist( _rnd._gen );
				
				if ( ret[i] < _min or ret[i] > _max )
					continue;

				++i;
			}
			return ret;
		}
	};


} // AE::Math
