// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Hash.h"
#include "base/CompileTime/TemplateUtils.h"

namespace AE::Base
{

	//
	// Tuple
	//

	template <typename ...Types>
	struct Tuple final : std::tuple< Types... >
	{
	// types
		using Self		= Tuple< Types... >;
		using Base_t	= std::tuple< Types... >;
		using Indices_t	= MakeIndexSequence< sizeof... (Types) >;


	// methods
		constexpr Tuple () = default;

		constexpr Tuple (const Self &) = default;
		constexpr Tuple (Self &&) = default;

		constexpr Self&	operator = (const Self &) = default;
		constexpr Self&	operator = (Self &&) = default;

		template <typename ...UTypes>
		constexpr explicit Tuple (UTypes&& ...args) : Base_t{ FwdArg<UTypes>(args)... } {}

		template <typename ...UTypes>
		constexpr Tuple (const Tuple<UTypes...> &other) : Base_t{ other.AsBase() } {}

		template <typename ...UTypes>
		constexpr Tuple (Tuple<UTypes...>&& other) : Base_t{ RVRef(other).AsBase() } {}

		ND_ constexpr bool  operator == (const Self &rhs)	const	{ return AsBase() == rhs.AsBase(); }
		ND_ constexpr bool  operator != (const Self &rhs)	const	{ return AsBase() != rhs.AsBase(); }
		ND_ constexpr bool  operator >  (const Self &rhs)	const	{ return AsBase() >  rhs.AsBase(); }
		ND_ constexpr bool  operator <  (const Self &rhs)	const	{ return AsBase() <  rhs.AsBase(); }
		ND_ constexpr bool  operator >= (const Self &rhs)	const	{ return AsBase() >= rhs.AsBase(); }
		ND_ constexpr bool  operator <= (const Self &rhs)	const	{ return AsBase() <= rhs.AsBase(); }

		template <typename T>
		ND_ constexpr T&				Get () 		&		{ return std::get<T>( *this ); }

		template <typename T>
		ND_ constexpr T const&			Get ()		const&	{ return std::get<T>( *this ); }
		
		template <typename T>
		ND_ constexpr T &&				Get ()		&&		{ return std::get<T>( RVRef(*this) ); }

		template <usize I>
		ND_ constexpr decltype(auto)	Get ()		&		{ return std::get<I>( *this ); }
		
		template <usize I>
		ND_ constexpr decltype(auto)	Get ()		const&	{ return std::get<I>( *this ); }
		
		template <usize I>
		ND_ constexpr decltype(auto)	Get ()		&&		{ return std::get<I>( RVRef(*this) ); }

		ND_ constexpr usize				Count ()	const	{ return sizeof... (Types); }
		
		ND_ constexpr Base_t const&		AsBase ()	const&	{ return static_cast<const Base_t &>(*this); }
		ND_ constexpr Base_t &			AsBase ()	&		{ return static_cast<Base_t &>(*this); }
		ND_ constexpr Base_t &&			AsBase ()	&&		{ return static_cast<Base_t &&>( RVRef(*this) ); }

		ND_ HashVal						CalcHash ()	const	{ return _RecursiveCalcHash<0>(); }

		template <typename Fn>
		constexpr decltype(auto)  Apply (Fn &&fn)
		{
			return std::apply( FwdArg<Fn>(fn), static_cast<Base_t &>(*this) );
		}

		template <typename Fn>
		constexpr decltype(auto)  Apply (Fn &&fn) const
		{
			return std::apply( FwdArg<Fn>(fn), static_cast<const Base_t &>(*this) );
		}

	private:
		template <usize I>
		ND_ HashVal  _RecursiveCalcHash () const
		{
			if constexpr( I+1 < sizeof... (Types) )
				return HashOf( Get<I>() ) + _RecursiveCalcHash<I+1>();
			else
				return HashOf( Get<I>() );
		}
	};

	
	template <typename ...Types>
	Tuple (Types...) -> Tuple< Types... >;

	template <typename T1, typename T2>
	Tuple (std::pair<T1, T2>) -> Tuple<T1, T2>;



	//
	// Tuple Ref
	//
	
	template <typename ...Types>
	struct TupleRef
	{
	// types
	private:
		using Self		= TupleRef< Types... >;
		using Tuple_t	= Tuple< Types*... >;


	// variables
	private:
		Tuple_t		_base;


	// methods
	public:
		constexpr TupleRef () = default;
		
		template <typename ...UTypes>
		constexpr explicit TupleRef (UTypes&& ...args) : _base{ FwdArg<UTypes>(args)... } {}

		template <typename T>
		ND_ constexpr T&				Get ()				{ return *_base.template Get<T*>(); }

		template <typename T>
		ND_ constexpr T const&			Get ()		const	{ return *_base.template Get<T*>(); }

		template <usize I>
		ND_ constexpr decltype(auto)	Get ()				{ return *_base.template Get<I>(); }
		
		template <usize I>
		ND_ constexpr decltype(auto)	Get ()		const	{ return *_base.template Get<I>(); }

		ND_ constexpr usize				Count ()	const	{ return _base.Count(); }
		
		ND_ constexpr Tuple_t const&	AsTuple ()	const&	{ return _base; }
		ND_ constexpr Tuple_t &			AsTuple ()	&		{ return _base; }
		ND_ constexpr Tuple_t &			AsTuple ()	&&		{ return _base; }
	};

	
/*
=================================================
	IsTuple
=================================================
*/
	namespace _hidden_
	{
		template <typename T>			struct _IsTuple						{ static constexpr bool  value = false; };
		template <typename ...Types>	struct _IsTuple< Tuple<Types...> >	{ static constexpr bool  value = true; };
	}

	template <typename T>
	static constexpr bool	IsTuple = _hidden_::_IsTuple< RemoveCVRef<T> >::value;
	
/*
=================================================
	TupleConcat
=================================================
*/
	namespace _hidden_
	{
		template <typename Tuple1, typename Tuple2, usize ...Idx1, usize ...Idx2>
		constexpr auto  _TupleConcat (Tuple1&& tuple1, Tuple2&& tuple2, IndexSequence<Idx1...>, IndexSequence<Idx2...>)
		{
			return Tuple{ std::get<Idx1>( FwdArg<Tuple1>( tuple1 )) ...,
						  std::get<Idx2>( FwdArg<Tuple2>( tuple2 )) ... };
		}
	}

	template <typename Tuple1, typename Tuple2>
	ND_ forceinline constexpr auto  TupleConcat (Tuple1&& tuple1, Tuple2&& tuple2)
	{
		STATIC_ASSERT( IsTuple<Tuple1> );
		STATIC_ASSERT( IsTuple<Tuple2> );
		return _hidden_::_TupleConcat( FwdArg<Tuple1>(tuple1), FwdArg<Tuple2>(tuple2),
									   typename RemoveCVRef< Tuple1 >::Indices_t{},
									   typename RemoveCVRef< Tuple2 >::Indices_t{} );
	}
	
	template <typename Tuple1, typename Tuple2, typename ...Tuples>
	ND_ forceinline constexpr auto  TupleConcat (Tuple1&& tuple1, Tuple2&& tuple2, Tuples&& ...tuples)
	{
		return TupleConcat( FwdArg<Tuple1>(tuple1), TupleConcat( FwdArg<Tuple2>(tuple2), FwdArg<Tuples>(tuples)... ));
	}

} // AE::Base

namespace std
{
	template <typename ...Types>
	struct tuple_size< AE::Base::Tuple<Types...> > :
		public std::integral_constant< std::size_t, sizeof...(Types) >
	{};

	template< size_t I, typename ...Types >
	struct tuple_element< I, AE::Base::Tuple<Types...> >
	{
		using type = typename tuple_element< I, std::tuple<Types...> >::type;
	};

	template <typename ...Types>
	inline void swap (AE::Base::Tuple<Types...> &lhs, AE::Base::Tuple<Types...> &rhs) noexcept
	{
		return swap( static_cast<tuple< Types... > &>(lhs), static_cast<tuple< Types... > &>(rhs) );
	}
	
	template <typename ...Types>
	struct hash< AE::Base::Tuple<Types...> >
	{
		ND_ size_t  operator () (const AE::Base::Tuple<Types...> &x) const {
			return size_t(x.CalcHash());
		}
	};

} // std
