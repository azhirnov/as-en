// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"

namespace AE::Base
{
namespace _hidden_
{
	// 'TL_GetFirstIndex' defined in 'Tuple.h'
	
	template <typename RefType, usize I, typename ...Types>
	struct TL_GetFirstIndex< RefType, I, Tuple<Types...> > :
		TL_GetFirstIndex< RefType, I, typename Tuple<Types...>::Base_t >
	{};


	template <typename RefType, usize I, typename TL>
	struct TL_GetLastIndex;
	
	template <typename RefType, usize I>
	struct TL_GetLastIndex< RefType, I, Tuple<> >
	{
		inline static constexpr usize	value = UMax;
	};
	
	template <typename RefType, usize I, typename Head, typename... Tail>
	struct TL_GetLastIndex< RefType, I, Tuple<Head, Tail...> >
	{
		using result = TL_GetLastIndex< RefType, I+1, Tuple<Tail...> >;

		inline static constexpr usize	value = Conditional< result::value == UMax and IsSameTypes<RefType, Head>,
													std::integral_constant<usize, I>,
													result >::value;
	};


	template <typename TL, typename T0, typename ...Types>
	struct TL_PopFront {
		using type		= typename TL_PopFront< TL, Types... >::result;
		using result	= typename type::template PushFront< T0 >;
	};

	template <typename TL, typename T0>
	struct TL_PopFront< TL, T0 > {
		using type		= TL;
		using result	= typename TL::template PushFront<T0>;
	};

	template <typename TL, typename T0, typename ...Types>
	struct TL_PopBack {
		using type = typename TL_PopBack< typename TL::template PushBack<T0>, Types... >::type;
	};
	
	template <typename TL, typename T0>
	struct TL_PopBack< TL, T0 > {
		using type = TL;
	};
	

	template <template <typename...> class Templ, usize I, typename TL>
	struct TL_GetFirstSpecializationOf;

	template <template <typename...> class Templ, usize I>
	struct TL_GetFirstSpecializationOf< Templ, I, Tuple<> >
	{
		inline static constexpr usize	value = UMax;
	};

	template <template <typename...> class Templ, usize I, typename Head, typename... Tail>
	struct TL_GetFirstSpecializationOf< Templ, I, Tuple<Head, Tail...> >
	{
		inline static constexpr usize	value = Conditional< IsSpecializationOf< Head, Templ >,
													std::integral_constant<usize, I>,
													TL_GetFirstSpecializationOf< Templ, I+1, Tuple<Tail...> > >::value;
	};

} // _hidden_


	//
	// Type List
	//

	template <typename... Types>
	struct TypeList
	{
	public:
		using							AsTuple			= Tuple< Types... >;

		template <typename T>
		inline static constexpr usize	FirstIndex		= Base::_hidden_::TL_GetFirstIndex< T, 0, AsTuple >::value;
		
		template <typename T>
		inline static constexpr usize	LastIndex		= Base::_hidden_::TL_GetLastIndex< T, 0, AsTuple >::value;

		template <template <typename...> class Templ>
		inline static constexpr usize	FirstSpecializationOf = Base::_hidden_::TL_GetFirstSpecializationOf< Templ, 0, AsTuple >::value;
		
		template <typename T>
		inline static constexpr usize	Index			= FirstIndex<T>;

		inline static constexpr usize	Count			= std::tuple_size_v< AsTuple >;

		template <typename T>
		inline static constexpr bool	HasType			= (Index<T> != UMax);
		
		template <usize I>		using	Get				= typename std::tuple_element<I, AsTuple>::type;
		template <usize I>		using	GetT			= std::tuple_element<I, AsTuple>;

		struct Front { using			type			= Get<0>; };
		struct Back  { using			type			= Get<Count-1>; };

		struct Self	 { using			type			= TypeList< Types... >; };

		struct PopFront	{ using			type			= typename Base::_hidden_::TL_PopFront< TypeList<>, Types... >::type; };
		struct PopBack	{ using			type			= typename Base::_hidden_::TL_PopBack< TypeList<>, Types... >::type; };

		template <typename T>	using	PushBack		= TypeList< Types..., T >;
		template <typename T>	using	PushFront		= TypeList< T, Types... >;


		template <template <typename> class Tmpl>
		static constexpr auto			ForEach_Or ()		__NE___	{ return (... or Tmpl<Types>::value); }
		
		template <template <typename> class Tmpl>
		static constexpr auto			ForEach_And ()		__NE___	{ return (... and Tmpl<Types>::value); }
		
		template <template <typename> class Tmpl>
		static constexpr auto			ForEach_Add	()		__NE___	{ return (... + Tmpl<Types>::value); }
		
		template <template <typename> class Tmpl>
		static constexpr auto			ForEach_Max	()		__NE___	{ return Math::Max( Tmpl<Types>::value... ); }
		
		template <template <typename> class Tmpl>
		static constexpr auto			ForEach_Min	()		__NE___	{ return Math::Min( Tmpl<Types>::value... ); }


		template <typename FN>
		static constexpr void 			Visit (FN&& fn)		__NE___	{ return _RecursiveVisit<0>( FwdArg<FN>(fn) ); }

	private:
		template <usize I, typename FN>
		static constexpr void  _RecursiveVisit (FN&& fn)	__NE___
		{
			if constexpr( I < Count )
			{
				using T = Get<I>;
				fn.template operator()<T,I>();
				_RecursiveVisit< I+1 >( FwdArg<FN>(fn) );
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

	
namespace _hidden_
{
	template <typename T>
	struct _IsTypeList {
		static constexpr bool	value = false;
	};

	template <typename... Types>
	struct _IsTypeList< TypeList<Types...>> {
		static constexpr bool	value = true;
	};


	template <typename Left, typename Right>
	struct _Merge;

	template <typename ...LeftTypes, typename ...RightTypes>
	struct _Merge< TypeList<LeftTypes...>, TypeList<RightTypes...> >
	{
		using type = TypeList< LeftTypes..., RightTypes... >;
	};

	template <typename A>
	struct AreSameTypes
	{
		template <typename B>
		struct Impl {
			static constexpr bool	value = IsSameTypes< A, B >;
		};
	};

} // _hidden_

	template <typename T>
	static constexpr bool	IsTypeList = Base::_hidden_::_IsTypeList< T >::value;
	

	// Type traits //

	template <typename T, typename ...Types>
	static constexpr bool	AllAreSameTypes			= TypeList< Types... >::template ForEach_And< Base::_hidden_::AreSameTypes<T>::Impl >();

	template <typename ...Types>
	static constexpr bool	AllNothrowCopyCtor		= TypeList< Types... >::template ForEach_And< IsNothrowCopyCtor_t >();
	
	template <typename ...Types>
	static constexpr bool	AllNothrowMoveCtor		= TypeList< Types... >::template ForEach_And< IsNothrowMoveCtor_t >();
	
	template <typename ...Types>
	static constexpr bool	AllNothrowDefaultCtor	= TypeList< Types... >::template ForEach_And< IsNothrowDefaultCtor_t >();


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
	using Merge = typename Base::_hidden_::_Merge< Left, Right >::type;

} // AE::Base::TypeListUtils
