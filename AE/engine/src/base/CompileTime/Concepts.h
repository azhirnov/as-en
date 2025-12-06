// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/TemplateUtils.h"

namespace AE::Base
{
	template <typename T1, typename T2>
	concept SameAs = std::same_as< T1, T2 >;


	template <typename To, typename From>
	concept ExplicitlyConvertibe = requires (From from)
	{
		To{ from };
	};

	template <typename To, typename From>
	concept ExplicitlyMoveConvertibe = requires (From from)
	{
		To{ RVRef(from) };
	};


	template <typename To, typename From>
	concept ImplicitlyConvertible = requires (From from)
	{
		[](To){}( from );
	};

	template <typename To, typename From>
	concept ImplicitlyMoveConvertible = requires (From from)
	{
		[](To){}( RVRef(from) );
	};


	template <typename To, typename From>
	concept ImplicitlyCopyable = requires (To to, From from)
	{
		to = from;
	};

	template <typename To, typename From>
	concept ImplicitlyMoveCopyable = requires (To to, From from)
	{
		to = RVRef( from );
	};


	// return 'true' if 'operator()' is not a template
	// and doesn't contains 'auto' arguments
	//
	template <typename T>
	concept IsNotTemplateCallOperator = requires
	{
		&T::operator();
	};


	template <typename T, typename ...Args>
	concept CanCallOperatorWith = requires(Args&& ...args)
	{
		T{}.operator()( FwdArg<Args>(args) ... );
	};


	namespace _hidden_
	{
		template <template <class...> class Tmpl, usize ...Indices>
		NdCx__ usize  CountTemplateArgs (std::index_sequence<Indices...>)
		{
			constexpr usize  count = sizeof...(Indices);
			if constexpr( requires{ typename Tmpl< ValueToType<Indices>... >; })
				return count;
			else
				return CountTemplateArgs< Tmpl >( std::make_index_sequence< count - 1 >{});
		}
	}

	template <template <class...> class Tmpl>
	NdCx__ usize  GetNumberOfTemplateArgs ()
	{
		constexpr usize	max_count = 20;
		return Base::_hidden_::CountTemplateArgs< Tmpl >( std::make_index_sequence< max_count >{});
	}


	template <typename T>
	concept AllowEnumBitOps = IsEnum<T> and
		((requires{
			T::_BITOPS_;
		 })
		 or
		 (requires{
			T::Unknown;
			T::_Last;
			T::All;
		 } and
		 (ulong(T::Unknown) == 0))
		);

} // AE::Base
