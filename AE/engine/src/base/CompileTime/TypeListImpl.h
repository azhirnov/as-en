// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"

namespace AE::Base::_hidden_
{
	//
	// TL_GetFirstIndex
	//
	template <typename RefType, usize I, typename ...Types>
	struct TL_GetFirstIndex;

	template <typename RefType, usize I>
	struct TL_GetFirstIndex< RefType, I >
	{
		inline static constexpr usize	value = UMax;
	};

	template <typename RefType, usize I, typename Head, typename... Tail>
	struct TL_GetFirstIndex< RefType, I, Head, Tail... >
	{
		inline static constexpr usize	value = Conditional< IsSame<RefType, Head>,
													std::integral_constant<usize, I>,
													TL_GetFirstIndex< RefType, I+1, Tail... >>::value;
	};

	template <typename RefType, typename ...Types>
	struct TL_GetFirstIndex2
	{
		inline static constexpr usize	value = TL_GetFirstIndex< RefType, 0, Types... >::value;
		StaticAssert( value != UMax );
	};


	//
	// TL_GetLastIndex
	//
	template <typename RefType, usize I, typename ...Types>
	struct TL_GetLastIndex;

	template <typename RefType, usize I>
	struct TL_GetLastIndex< RefType, I >
	{
		inline static constexpr usize	value = UMax;
	};

	template <typename RefType, usize I, typename Head, typename... Tail>
	struct TL_GetLastIndex< RefType, I, Head, Tail... >
	{
		using result = TL_GetLastIndex< RefType, I+1, Tail... >;

		inline static constexpr usize	value = Conditional< result::value == UMax and IsSame<RefType, Head>,
													std::integral_constant<usize, I>,
													result >::value;
	};


	//
	// TL_PopFront
	//
	template <template <typename ...> class Tmpl, typename ...Types>
	struct TL_PopFront;

	template <template <typename ...> class Tmpl, typename T0, typename ...Types>
	struct TL_PopFront< Tmpl, T0, Types... > {
		using type	= Tmpl< Types... >;
	};

	template <template <typename ...> class Tmpl>
	struct TL_PopFront< Tmpl > {
		using type	= Tmpl<>;
	};


	//
	// TL_EraseFront
	//
	template <usize Count, typename TL>
	struct TL_EraseFront
	{
		StaticAssert( Count < TL::Count );

		using type = typename Conditional< (Count > 0),
						TL_EraseFront< Count-1, typename TL::PopFront::type >,
						TypeToType< TL >
					 >::type;
	};


	//
	// TL_EraseBack
	//
	template <usize Count, typename TL>
	struct TL_EraseBack
	{
		using t0	= typename TL::Reverse::type;
		using t1	= typename TL_EraseFront< Count, t0 >::type;
		using type	= typename t1::Reverse::type;
	};


	//
	// TL_Erase
	//
	template <typename Dst, usize Index, typename ...Types>
	struct TL_Erase;

	template <typename Dst, usize Index, typename T0, typename ...Types>
	struct TL_Erase< Dst, Index, T0, Types... >
	{
		StaticAssert( Index <= sizeof...(Types) );

		using type = typename Conditional< (Index == 0),
						TypeToType< typename Dst::template PushBack< Types... >>,
						TL_Erase< typename Dst::template PushBack<T0>, Index-1, Types... >
					 >::type;
	};


	//
	// TL_EraseType
	//
	template <typename Dst, typename EraseT, typename ...Types>
	struct TL_EraseType;

	template <typename Dst, typename EraseT>
	struct TL_EraseType< Dst, EraseT > {
		using type = Dst;
	};

	template <typename Dst, typename EraseT, typename T0, typename ...Types>
	struct TL_EraseType< Dst, EraseT, T0, Types... >
	{
		using type = typename Conditional< (IsSame< T0, EraseT >),
						TL_EraseType< Dst, EraseT, Types... >,
						TL_EraseType< typename Dst::template PushBack<T0>, EraseT, Types... >
					 >::type;
	};


	//
	// TL_GetFirstSpecializationOf
	//
	template <template <typename...> class Templ, usize I, typename ...Types>
	struct TL_GetFirstSpecializationOf;

	template <template <typename...> class Templ, usize I>
	struct TL_GetFirstSpecializationOf< Templ, I >
	{
		inline static constexpr usize	value = UMax;
	};

	template <template <typename...> class Templ, usize I, typename Head, typename... Tail>
	struct TL_GetFirstSpecializationOf< Templ, I, Head, Tail... >
	{
		inline static constexpr usize	value = Conditional< IsSpecializationOf< Head, Templ >,
													std::integral_constant<usize, I>,
													TL_GetFirstSpecializationOf< Templ, I+1, Tail... >>::value;
	};


	//
	// TL_GetTypeByIndex
	//
	template <usize ReqIndex, usize I, typename ...Types>
	struct TL_GetTypeByIndex2;

	template <usize ReqIndex, usize I>
	struct TL_GetTypeByIndex2< ReqIndex, I >
	{
		StaticAssert( "index is out of range" );
	};

	template <usize ReqIndex, usize I, typename Head, typename... Tail>
	struct TL_GetTypeByIndex2< ReqIndex, I, Head, Tail... >
	{
		using  type = typename Conditional< ReqIndex == I,
							TypeToType< Head >,
							TL_GetTypeByIndex2< ReqIndex, I+1, Tail... >>::type;
	};

	template <usize ReqIndex, typename ...Types>
	struct TL_GetTypeByIndex {
		using  type = typename TL_GetTypeByIndex2< ReqIndex, 0, Types... >::type;
	};


	//
	// TL_Reverse
	//
	template <typename Dst, typename ...Types>
	struct TL_Reverse;

	template <typename Dst>
	struct TL_Reverse< Dst > {
		using type = Dst;
	};

	template <typename Dst, typename Head, typename ...Types>
	struct TL_Reverse< Dst, Head, Types... > {
		using type = typename TL_Reverse< typename Dst::template PushFront< Head >, Types... >::type;
	};


} // AE::Base::_hidden_
