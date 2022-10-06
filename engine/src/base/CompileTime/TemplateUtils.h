// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
		using type	= T;
	};

	
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
		template <usize First, typename SeqType>
		struct MakeIntSequenceRange;

		template <usize First, usize ...I>
		struct MakeIntSequenceRange< First, std::integer_sequence<usize, I...> >
		{
			using type = std::integer_sequence< usize, (I + First)... >;
		};

	}	// _hidden_

	template <usize First, usize Count>
	using MakeIntSequence = typename Base::_hidden_::MakeIntSequenceRange< First, std::make_integer_sequence< usize, Count > >::type;


}	// AE::Base
