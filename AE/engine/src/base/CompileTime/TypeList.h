// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
		Warning
	Inside 'struct TypeList' don't use 'using' without template,
	use 'struct <type> { using type = ...; };' instead, otherwise
	it will cause compilation performance degradation.
*/

#pragma once

#include "base/CompileTime/TypeListImpl.h"

namespace AE::Base
{

	//
	// Type List
	//

	template <typename... Types>
	struct TypeList
	{
	public:
		struct							AsTuple				{ using type = Tuple< Types... >; };

		template <typename T>
		inline static constexpr usize	FirstIndex			= Base::_hidden_::TL_GetFirstIndex< T, 0, Types... >::value;

		template <typename T>
		inline static constexpr usize	LastIndex			= Base::_hidden_::TL_GetLastIndex< T, 0, Types... >::value;

		template <template <typename...> class Templ>
		inline static constexpr usize	FirstSpecializationOf = Base::_hidden_::TL_GetFirstSpecializationOf< Templ, 0, Types... >::value;

		template <typename T>
		inline static constexpr usize	IndexOrMax			= FirstIndex<T>;

		template <typename T>
		inline static constexpr usize	Index				= Base::_hidden_::TL_GetFirstIndex2< T, Types... >::value;

		inline static constexpr usize	Count				= sizeof...(Types);

		template <typename T>
		inline static constexpr bool	HasType				= (FirstIndex<T> != UMax);

		template <typename T>
		inline static constexpr bool	HasSingle			= HasType<T> and (FirstIndex<T> == LastIndex<T>);

		template <usize I>		using	GetT				= Base::_hidden_::TL_GetTypeByIndex< I, Types... >;					// usage: GetT<0>::type
		template <usize I>		using	Get					= typename Base::_hidden_::TL_GetTypeByIndex< I, Types... >::type;	// usage: Get<0>

		struct							Front				{ using	type = Get<0>; };
		struct							Back				{ using	type = Get<Count-1>; };

		struct							Self				{ using	type = TypeList< Types... >; };

		struct							Reverse				{ using type = typename Base::_hidden_::TL_Reverse< TypeList<>, Types... >::type; };

		struct							PopFront			{ using type = typename Base::_hidden_::TL_PopFront< TypeList, Types... >::type; };
		struct							PopBack				{ using type = typename Reverse::type::PopFront::type::Reverse::type; };

		template <usize Count>	using	EraseFront			= typename Base::_hidden_::TL_EraseFront< Count, TypeList<Types...> >::type;
		template <usize Count>	using	EraseBack			= typename Base::_hidden_::TL_EraseBack< Count, TypeList<Types...> >::type;

		template <usize Index>	using	Erase				= typename Base::_hidden_::TL_Erase< TypeList<>, Index, Types... >::type;
		template <typename T>	using	EraseType			= typename Base::_hidden_::TL_EraseType< TypeList<>, T, Types... >::type;

		template <typename ...T> using	PushBack			= TypeList< Types..., T... >;
		template <typename ...T> using	PushFront			= TypeList< T..., Types... >;

		template <template <typename> class Tmpl>
		using							Apply				= TypeList< Tmpl< Types >... >;

		template <template <typename> class Tmpl>
		using							Apply_t				= TypeList< typename Tmpl< Types >::type ... >;

		template <template <typename ...> class Tmpl>
		using							Transform			= Tmpl< Types... >;

		template <template <typename> class Tmpl>
		static __Cx__ auto				ForEach_Or ()		__NE___	{ return (... or Tmpl<Types>::value); }		// any

		template <template <typename> class Tmpl>
		static __Cx__ auto				ForEach_And ()		__NE___	{ return (... and Tmpl<Types>::value); }	// all

		template <template <typename> class Tmpl>
		static __Cx__ auto				ForEach_Add	()		__NE___	{ return (... + Tmpl<Types>::value); }

		template <template <typename> class Tmpl>
		static __CxIA auto				ForEach_Max	()		__NE___	{ return Base::Max( Tmpl<Types>::value... ); }

		template <template <typename> class Tmpl>
		static __CxIA auto				ForEach_Min	()		__NE___	{ return Base::Min( Tmpl<Types>::value... ); }


		template <typename FN>
		static __CxIA void 				Visit (FN&& fn)		__NE___	{ return _RecursiveVisit<0>( FwdArg<FN>(fn) ); }

		template <typename FN>
		static __CxIA void 				VisitTh (FN&& fn)	__Th___	{ return _RecursiveVisit2<0>( FwdArg<FN>(fn) ); }


	private:
		template <usize I, typename FN>
		static __Cx__ void  _RecursiveVisit (FN&& fn)	__NE___
		{
			if constexpr( I < Count )
			{
				using T = Get<I>;
				CheckNothrow( IsNoExcept( fn.template operator()<T,I>() ));

				fn.template operator()<T,I>();
				_RecursiveVisit< I+1 >( FwdArg<FN>(fn) );
			}
			Unused( fn );
		}

		template <usize I, typename FN>
		static __Cx__ void  _RecursiveVisit2 (FN&& fn)	__Th___
		{
			if constexpr( I < Count )
			{
				using T = Get<I>;
				CheckNothrow( not IsNoExcept( fn.template operator()<T,I>() ));	// use 'Visit'

				fn.template operator()<T,I>();
				_RecursiveVisit2< I+1 >( FwdArg<FN>(fn) );
			}
			Unused( fn );
		}
	};


	template <typename... Types>
	struct TypeList< std::tuple<Types...> > final : TypeList< Types... >
	{};

	template <typename... Types>
	struct TypeList< Tuple<Types...> > final : TypeList< Types... >
	{};


/*
=================================================
	IsTypeList
=================================================
*/
namespace _hidden_
{
	template <typename T>
	struct _IsTypeList : CT_False {};

	template <typename... Types>
	struct _IsTypeList< TypeList<Types...>> : CT_True {};


	template <typename Left, typename Right>
	struct TL_Merge;

	template <typename ...LeftTypes, typename ...RightTypes>
	struct TL_Merge< TypeList<LeftTypes...>, TypeList<RightTypes...> >
	{
		using type = TypeList< LeftTypes..., RightTypes... >;
	};
}
	template <typename T>
	static constexpr bool	IsTypeList = Base::_hidden_::_IsTypeList< T >::value;

/*
=================================================
	TypeListFill
=================================================
*/
namespace _hidden_
{
	template <typename T, usize Count>
	struct _TypeListFill
	{
		using type = typename _TypeListFill< T, Count-1 >::type::template PushBack< T >;
	};

	template <typename T>
	struct _TypeListFill< T, 0 >
	{
		using type = TypeList<>;
	};
}
	template <typename T, usize Count>
	using TypeListFill = typename Base::_hidden_::_TypeListFill< T, Count >::type;


} // AE::Base

namespace AE::Base::TypeListUtils
{

	template <typename T>
	struct GetTypeSize {
		static constexpr usize	value = sizeof(T);
	};

	template <typename T>
	struct GetTypeAlign {
		static constexpr usize	value = alignof(T);
	};


	template <typename Left, typename Right>
	using Merge			= typename Base::_hidden_::TL_Merge< Left, Right >::type;

	template <typename TL>
	using Front			= typename TL::Front::type;

	template <typename TL>
	using Back			= typename TL::Back::type;

	template <typename TL>
	using Reverse		= typename TL::Reverse::type;

	template <typename TL>
	using PopFront		= typename TL::PopFront::type;

	template <typename TL>
	using PopBack		= typename TL::PopBack::type;

} // AE::Base::TypeListUtils
