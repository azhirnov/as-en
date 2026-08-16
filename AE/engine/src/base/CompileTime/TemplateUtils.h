// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

namespace AE::Base
{

	//
	// Value to Type
	//

	template <auto Value>
	struct ValueToType
	{
		static constexpr auto	value = Value;
	};


	//
	// Type to Type
	//

	template <typename T>
	struct TypeToType
	{
		using type		= T;
	};

	template <typename T>
	struct TypeToValue_t
	{
		using Value_t	= T;
	};


	//
	// Tag
	//

	template <typename T>
	struct Tag
	{};


	//
	// Deferred Template Type
	//

	template <template <typename ...> class Templ, typename ...Types>
	struct DeferredTemplate
	{
		using type	= Templ< Types... >;
	};

	template <template <typename ...> class Templ, typename ...Types>
	struct DeferredTemplate2
	{
		using type	= typename Templ< Types... >::type;
	};


	//
	// Make Integer Sequence
	//

	namespace _hidden_
	{
		template <typename T, T First, typename SeqType>
		struct MakeIntSequenceRange;

		template <typename T, T First, T ...I>
		struct MakeIntSequenceRange< T, First, std::integer_sequence<T, I...> >
		{
			using type = std::integer_sequence< T, (I + First)... >;
		};

	} // _hidden_

	template <usize First, usize Count>
	using MakeUSizeSequence	= typename Base::_hidden_::MakeIntSequenceRange< usize, First, std::make_integer_sequence< usize, Count > >::type;

	template <uint First, uint Count>
	using MakeUIntSequence	= typename Base::_hidden_::MakeIntSequenceRange< uint, First, std::make_integer_sequence< uint, Count > >::type;

	template <usize ...Indices>
	using USizeSequence		= std::integer_sequence< usize, Indices... >;

	template <uint ...Indices>
	using UIntSequence		= std::integer_sequence< uint, Indices... >;


} // AE::Base
