// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::Base
{
namespace _hidden_
{
	template <typename RefType, usize I, typename TL>
	struct Tuple_GetFirstIndex;

	template <typename RefType, usize I>
	struct Tuple_GetFirstIndex< RefType, I, std::tuple<> >
	{
		inline static constexpr usize	value = UMax;
	};

	template <typename RefType, usize I, typename Head, typename... Tail>
	struct Tuple_GetFirstIndex< RefType, I, std::tuple<Head, Tail...> >
	{
		inline static constexpr usize	value = Conditional< IsSameTypes<RefType, Head>,
													std::integral_constant<usize, I>,
													Tuple_GetFirstIndex< RefType, I+1, std::tuple<Tail...> > >::value;
	};

} // _hidden_



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
		__Cx__ Tuple ()												__NE___ = default;

		__Cx__ Tuple (const Self &)									__NE___ = default;
		__Cx__ Tuple (Self &&)										__NE___ = default;

		template <typename ...UTypes>
		__CxIA explicit Tuple (UTypes&& ...args)					NoExcept( Base::IsNothrowCtor< Base_t, UTypes&&... >) : Base_t{ FwdArg<UTypes>(args)... } {}

		template <typename ...UTypes>
		__CxIA Tuple (const Tuple<UTypes...> &other)				NoExcept( Base::IsNothrowCtor< Base_t, decltype(other) >) : Base_t{ other.AsBase() } {}

		template <typename ...UTypes>
		__CxIA Tuple (Tuple<UTypes...>&& other)						__NE___ : Base_t{ RVRef(other).AsBase() } {}


		__Cx__ Self&	 operator = (const Self &)					__NE___ = default;
		__Cx__ Self&	 operator = (Self &&)						__NE___ = default;

		template <typename ...UTypes>
		__CxIA Self&	 operator = (const Tuple<UTypes...> &rhs)	__NE___ { AsBase() = rhs.AsBase();  return *this; }

		template <typename ...UTypes>
		__CxIA Self&	 operator = (Tuple<UTypes...>&&rhs)			__NE___ { AsBase() = RVRef(rhs).AsBase();  return *this; }


		NdCxIA bool  operator == (const Self &rhs)					C_NE___	{ return AsBase() == rhs.AsBase(); }
		NdCxIA bool  operator != (const Self &rhs)					C_NE___	{ return AsBase() != rhs.AsBase(); }
		NdCxIA bool  operator >  (const Self &rhs)					C_NE___	{ return AsBase() >  rhs.AsBase(); }
		NdCxIA bool  operator <  (const Self &rhs)					C_NE___	{ return AsBase() <  rhs.AsBase(); }
		NdCxIA bool  operator >= (const Self &rhs)					C_NE___	{ return AsBase() >= rhs.AsBase(); }
		NdCxIA bool  operator <= (const Self &rhs)					C_NE___	{ return AsBase() <= rhs.AsBase(); }

		template <typename T>
		NdCxIA T&				Get () 								r_NE___	{ return std::get<T>( *this ); }

		template <typename T>
		NdCxIA T const&			Get ()								CrNE___	{ return std::get<T>( *this ); }

		template <typename T>
		NdCxIA T &&				Get ()								rvNE___	{ return std::get<T>( RVRef(*this) ); }

		template <usize I>
		NdCxIA exact_t			Get ()								r_NE___	{ return std::get<I>( *this ); }

		template <usize I>
		NdCxIA exact_t			Get ()								CrNE___	{ return std::get<I>( *this ); }

		template <usize I>
		NdCxIA exact_t			Get ()								rvNE___	{ return std::get<I>( RVRef(*this) ); }


		NdCxIA usize			Count ()							C_NE___	{ return sizeof... (Types); }

		NdCxIA Base_t const&	AsBase ()							CrNE___	{ return static_cast<const Base_t &>(*this); }
		NdCxIA Base_t &			AsBase ()							r_NE___	{ return static_cast<Base_t &>(*this); }
		NdCxIA Base_t &&		AsBase ()							rvNE___	{ return static_cast<Base_t &&>( RVRef(*this) ); }

		Nd__IA HashVal			CalcHash ()							C_NE___	{ return _RecursiveCalcHash<0>(); }

		template <typename ...Args>
		__CxIA void				Set (Args&& ...args)				__NE___	{ _RecursiveSet<0>( FwdArg<Args>(args)... ); }

		template <typename Fn>
		__CxIA exact_t			Apply (Fn &&fn)						NoExcept(IsNothrowInvocable< Fn, Types&... >)
		{
			return std::apply( FwdArg<Fn>(fn), static_cast<Base_t &>(*this) );
		}

		template <typename Fn>
		__CxIA exact_t			Apply (Fn &&fn)						CNoExcept(IsNothrowInvocable< Fn, const Types&... >)
		{
			return std::apply( FwdArg<Fn>(fn), static_cast<const Base_t &>(*this) );
		}

		template <typename Fn>
		__CxIA void				ForEach (Fn &fn)					C_Th___	{ _ForEach<0>( fn ); }


	private:
		template <usize I>
		ND_ HashVal  _RecursiveCalcHash ()							C_NE___
		{
			if constexpr( I+1 < sizeof... (Types) )
				return HashOf( Get<I>() ) + _RecursiveCalcHash<I+1>();
			else
				return HashOf( Get<I>() );
		}

		template <usize I, typename Arg0, typename ...Args>
		constexpr void  _RecursiveSet (Arg0 &&arg0, Args&& ...args)	__NE___
		{
			CheckNothrow( IsNoExcept( Get<I>() = FwdArg<Arg0>(arg0) ));
			Get<I>() = FwdArg<Arg0>(arg0);

			if constexpr( I+1 < sizeof... (Types) )
				_RecursiveSet<I+1>( FwdArg<Args>(args)... );
		}

		template <usize I, typename Fn>
		constexpr void  _ForEach (Fn &fn)							C_Th___
		{
			fn( Get<I>() );
			if constexpr( I+1 < sizeof... (Types) )
				_ForEach<I+1>( fn );
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
		using Self			= TupleRef< Types... >;
		using Tuple_t		= Tuple< Types*... >;
		using CRef_t		= TupleRef< Types const... >;
		using TypeList_t	= std::tuple< Types... >;

		template <typename T>
		inline static constexpr usize	_Index	= Base::_hidden_::Tuple_GetFirstIndex< T, 0, TypeList_t >::value;


	// variables
	private:
		Tuple_t		_base;


	// methods
	public:
		__Cx__ TupleRef ()							__NE___ = default;

		// 'args' must be pointers
		template <typename ...UTypes>
		__CxIA explicit TupleRef (UTypes&& ...args)	__NE___ : _base{ FwdArg<UTypes>(args)... } {}


		template <typename T>
		NdCxIA T&				Get () 				r_NE___	{ return Get< _Index<T> >(); }

		template <typename T>
		NdCxIA T const&			Get ()				CrNE___	{ return Get< _Index<T> >(); }

		template <typename T>
		NdCxIA T &				Get ()				rvNE___	{ return Get< _Index<T> >(); }


		template <usize I>
		NdCxIA exact_t			Get ()				r_NE___	{ ASSERT( IsNotNull<I>() );  return *_base.template Get<I>(); }

		template <usize I>
		NdCxIA exact_t			Get ()				CrNE___	{ ASSERT( IsNotNull<I>() );  return *_base.template Get<I>(); }

		template <usize I>
		NdCxIA exact_t			Get ()				rvNE___	{ ASSERT( IsNotNull<I>() );  return *_base.template Get<I>(); }


		// for structured bindings
	  #if 1
		template <usize I>
		NdCxIA exact_t			get ()				r_NE___	{ ASSERT( IsNotNull<I>() );  return *_base.template Get<I>(); }

		template <usize I>
		NdCxIA exact_t			get ()				CrNE___	{ ASSERT( IsNotNull<I>() );  return *_base.template Get<I>(); }

		template <usize I>
		NdCxIA exact_t			get ()				rvNE___	{ ASSERT( IsNotNull<I>() );  return *_base.template Get<I>(); }
	  #endif


		template <usize I>
		NdCxIA bool				IsNotNull ()		C_NE___	{ return _base.template Get<I>() != null; }

		template <typename T>
		NdCxIA bool				IsNotNull ()		C_NE___	{ return _base.template Get< _Index<T> >() != null; }

		template <usize I>
		NdCxIA bool				IsNull ()			C_NE___	{ return _base.template Get<I>() == null; }

		template <typename T>
		NdCxIA bool				IsNull ()			C_NE___	{ return _base.template Get< _Index<T> >() == null; }


		NdCx__ usize			Count ()			C_NE___	{ return sizeof... (Types); }

		NdCxIA bool				AllNonNull ()		C_NE___	{ return _RecursiveNonNull<0>(); }
		NdCxIA bool				AnyNull ()			C_NE___	{ return not AllNonNull(); }
		NdCxIA bool				AllNull ()			C_NE___	{ return _RecursiveNull<0>(); }

		NdCx__ Tuple_t const&	AsTuple ()			CrNE___	{ return _base; }
		NdCx__ Tuple_t &		AsTuple ()			r_NE___	{ return _base; }
		NdCx__ Tuple_t &		AsTuple ()			rvNE___	{ return _base; }
		NdCx__ CRef_t const&	AsConst ()			CrNE___	{ return reinterpret_cast<CRef_t const&>(*this); }


	private:
		template <usize I>
		ND_ constexpr bool  _RecursiveNonNull ()	C_NE___
		{
			if constexpr( I+1 < sizeof...(Types) )
				return IsNotNull<I>() and _RecursiveNonNull<I+1>();
			else
				return IsNotNull<I>();
		}

		template <usize I>
		ND_ constexpr bool  _RecursiveNull ()		C_NE___
		{
			if constexpr( I+1 < sizeof...(Types) )
				return IsNull<I>() and _RecursiveNull<I+1>();
			else
				return IsNull<I>();
		}
	};

	template <typename ...Types>
	TupleRef (Types...) -> TupleRef< RemovePointer<Types> ... >;


/*
=================================================
	IsTuple
=================================================
*/
	namespace _hidden_
	{
		template <typename T>			struct _IsTuple						: CT_False	{};
		template <typename ...Types>	struct _IsTuple< Tuple<Types...> >	: CT_True	{};
	}

	template <typename T>
	static constexpr bool	IsTuple = Base::_hidden_::_IsTuple< RemoveCVRef<T> >::value;

/*
=================================================
	TupleConcat
=================================================
*/
	namespace _hidden_
	{
		template <typename Tuple1, typename Tuple2, usize ...Idx1, usize ...Idx2>
		NdCxIA auto  _TupleConcat (Tuple1&& tuple1, Tuple2&& tuple2, IndexSequence<Idx1...>, IndexSequence<Idx2...>) __Th___
		{
			return Tuple{ std::get<Idx1>( FwdArg<Tuple1>( tuple1 )) ...,
						  std::get<Idx2>( FwdArg<Tuple2>( tuple2 )) ... };
		}
	}

	template <typename Tuple1, typename Tuple2>
	NdCxIA auto  TupleConcat (Tuple1&& tuple1, Tuple2&& tuple2) __Th___
	{
		StaticAssert( IsTuple<Tuple1> );
		StaticAssert( IsTuple<Tuple2> );
		return Base::_hidden_::_TupleConcat( FwdArg<Tuple1>(tuple1), FwdArg<Tuple2>(tuple2),
											 typename RemoveCVRef< Tuple1 >::Indices_t{},
											 typename RemoveCVRef< Tuple2 >::Indices_t{} );
	}

	template <typename Tuple1, typename Tuple2, typename ...Tuples>
	NdCxIA auto  TupleConcat (Tuple1&& tuple1, Tuple2&& tuple2, Tuples&& ...tuples) __Th___
	{
		return TupleConcat( FwdArg<Tuple1>(tuple1),
							TupleConcat( FwdArg<Tuple2>(tuple2), FwdArg<Tuples>(tuples)... ));
	}

/*
=================================================
	StructSet (same as std::tie)
=================================================
*/
	template <typename ...Args>
	NdCxIA Tuple<Args&...>  StructSet (Args&... args) __NE___
	{
		return Tuple< Args& ...>{ args... };
	}

} // AE::Base


template <typename ...Types>
struct std::tuple_size< AE::Base::Tuple<Types...> > :
	public std::integral_constant< std::size_t, sizeof...(Types) >
{};

template <typename ...Types>
struct std::tuple_size< AE::Base::TupleRef<Types...> > :
	public std::integral_constant< std::size_t, sizeof...(Types) >
{};

template< size_t I, typename ...Types >
struct std::tuple_element< I, AE::Base::Tuple<Types...> > {
	using type = typename tuple_element< I, std::tuple<Types...> >::type;
};

template< size_t I, typename ...Types >
struct std::tuple_element< I, AE::Base::TupleRef<Types...> > {
	using type = typename tuple_element< I, std::tuple<Types...> >::type;
};

template <typename ...Types>
struct std::hash< AE::Base::Tuple<Types...> > {
	ND_ size_t  operator () (const AE::Base::Tuple<Types...> &x) C_NE___ {
		return size_t(x.CalcHash());
	}
};

namespace std {
	template <typename ...Types>
	void  swap (AE::Base::Tuple<Types...> &lhs, AE::Base::Tuple<Types...> &rhs) __NE___
	{
		return swap( static_cast<tuple< Types... > &>(lhs), static_cast<tuple< Types... > &>(rhs) );
	}
}
