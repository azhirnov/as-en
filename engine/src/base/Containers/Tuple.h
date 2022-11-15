// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

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
		constexpr Tuple ()									__NE___ = default;

		constexpr Tuple (const Self &)						__TH___ = default;
		constexpr Tuple (Self &&)							__NE___ = default;

		constexpr Self&	operator = (const Self &)			__TH___ = default;
		constexpr Self&	operator = (Self &&)				__NE___ = default;

		template <typename ...UTypes>
		constexpr explicit Tuple (UTypes&& ...args)			__TH___ : Base_t{ FwdArg<UTypes>(args)... } {}

		template <typename ...UTypes>
		constexpr Tuple (const Tuple<UTypes...> &other)		__TH___ : Base_t{ other.AsBase() } {}

		template <typename ...UTypes>
		constexpr Tuple (Tuple<UTypes...>&& other)			__TH___ : Base_t{ RVRef(other).AsBase() } {}

		ND_ constexpr bool  operator == (const Self &rhs)	C_NE___	{ return AsBase() == rhs.AsBase(); }
		ND_ constexpr bool  operator != (const Self &rhs)	C_NE___	{ return AsBase() != rhs.AsBase(); }
		ND_ constexpr bool  operator >  (const Self &rhs)	C_NE___	{ return AsBase() >  rhs.AsBase(); }
		ND_ constexpr bool  operator <  (const Self &rhs)	C_NE___	{ return AsBase() <  rhs.AsBase(); }
		ND_ constexpr bool  operator >= (const Self &rhs)	C_NE___	{ return AsBase() >= rhs.AsBase(); }
		ND_ constexpr bool  operator <= (const Self &rhs)	C_NE___	{ return AsBase() <= rhs.AsBase(); }

		template <typename T>
		ND_ constexpr T&				Get () 				r_NE___	{ return std::get<T>( *this ); }

		template <typename T>
		ND_ constexpr T const&			Get ()				CrNE___	{ return std::get<T>( *this ); }
		
		template <typename T>
		ND_ constexpr T &&				Get ()				rvNE___	{ return std::get<T>( RVRef(*this) ); }

		template <usize I>
		ND_ constexpr decltype(auto)	Get ()				r_NE___	{ return std::get<I>( *this ); }
		
		template <usize I>
		ND_ constexpr decltype(auto)	Get ()				CrNE___	{ return std::get<I>( *this ); }
		
		template <usize I>
		ND_ constexpr decltype(auto)	Get ()				rvNE___	{ return std::get<I>( RVRef(*this) ); }

		ND_ constexpr usize				Count ()			C_NE___	{ return sizeof... (Types); }
		
		ND_ constexpr Base_t const&		AsBase ()			CrNE___	{ return static_cast<const Base_t &>(*this); }
		ND_ constexpr Base_t &			AsBase ()			r_NE___	{ return static_cast<Base_t &>(*this); }
		ND_ constexpr Base_t &&			AsBase ()			rvNE___	{ return static_cast<Base_t &&>( RVRef(*this) ); }

		ND_ HashVal						CalcHash ()			C_NE___	{ return _RecursiveCalcHash<0>(); }

		template <typename Fn>
		constexpr decltype(auto)  Apply (Fn &&fn)			__TH___
		{
			return std::apply( FwdArg<Fn>(fn), static_cast<Base_t &>(*this) );
		}

		template <typename Fn>
		constexpr decltype(auto)  Apply (Fn &&fn)			C_TH___
		{
			return std::apply( FwdArg<Fn>(fn), static_cast<const Base_t &>(*this) );
		}

	private:
		template <usize I>
		ND_ HashVal  _RecursiveCalcHash ()					C_NE___
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
		constexpr TupleRef ()							__NE___ = default;
		
		template <typename ...UTypes>
		constexpr explicit TupleRef (UTypes&& ...args)	__TH___ : _base{ FwdArg<UTypes>(args)... } {}

		template <typename T>
		ND_ constexpr T&				Get ()			__NE___	{ return *_base.template Get<T*>(); }

		template <typename T>
		ND_ constexpr T const&			Get ()			C_NE___	{ return *_base.template Get<T*>(); }

		template <usize I>
		ND_ constexpr decltype(auto)	Get ()			__NE___	{ return *_base.template Get<I>(); }
		
		template <usize I>
		ND_ constexpr decltype(auto)	Get ()			C_NE___	{ return *_base.template Get<I>(); }

		ND_ constexpr usize				Count ()		C_NE___	{ return _base.Count(); }
		
		ND_ constexpr Tuple_t const&	AsTuple ()		CrNE___	{ return _base; }
		ND_ constexpr Tuple_t &			AsTuple ()		r_NE___	{ return _base; }
		ND_ constexpr Tuple_t &			AsTuple ()		rvNE___	{ return _base; }
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
		constexpr auto  _TupleConcat (Tuple1&& tuple1, Tuple2&& tuple2, IndexSequence<Idx1...>, IndexSequence<Idx2...>) __TH___
		{
			return Tuple{ std::get<Idx1>( FwdArg<Tuple1>( tuple1 )) ...,
						  std::get<Idx2>( FwdArg<Tuple2>( tuple2 )) ... };
		}
	}

	template <typename Tuple1, typename Tuple2>
	ND_ forceinline constexpr auto  TupleConcat (Tuple1&& tuple1, Tuple2&& tuple2) __TH___
	{
		STATIC_ASSERT( IsTuple<Tuple1> );
		STATIC_ASSERT( IsTuple<Tuple2> );
		return _hidden_::_TupleConcat( FwdArg<Tuple1>(tuple1), FwdArg<Tuple2>(tuple2),
									   typename RemoveCVRef< Tuple1 >::Indices_t{},
									   typename RemoveCVRef< Tuple2 >::Indices_t{} );
	}
	
	template <typename Tuple1, typename Tuple2, typename ...Tuples>
	ND_ forceinline constexpr auto  TupleConcat (Tuple1&& tuple1, Tuple2&& tuple2, Tuples&& ...tuples) __TH___
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
	inline void swap (AE::Base::Tuple<Types...> &lhs, AE::Base::Tuple<Types...> &rhs) __NE___
	{
		return swap( static_cast<tuple< Types... > &>(lhs), static_cast<tuple< Types... > &>(rhs) );
	}
	
	template <typename ...Types>
	struct hash< AE::Base::Tuple<Types...> >
	{
		ND_ size_t  operator () (const AE::Base::Tuple<Types...> &x) C_NE___ {
			return size_t(x.CalcHash());
		}
	};

} // std
