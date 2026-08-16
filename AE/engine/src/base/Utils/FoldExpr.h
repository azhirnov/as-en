// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Common.h"

namespace AE::Base
{
struct FoldExpr
{
private:
	template <typename Seq>
	struct _ApplyExceptLastHelper;

	template <uint ...Indices>
	struct _ApplyExceptLastHelper< UIntSequence< Indices... >>
	{
		template <typename FN, typename ...Args>
		static exact_t  Call (FN&& fn, Tuple<Args...>&& t)
		{
			return FwdArg<FN>(fn)( RVRef(t).template Get<Indices>() ... );
		}
	};

public:
	template <typename Arg0, typename ...Args>
	NdCx__ static exact_t  First (Arg0 &&arg0, Args&& ...)		__NE___	{ return FwdArg<Arg0>(arg0); }

	# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
	#	pragma clang diagnostic push
	#	pragma clang diagnostic ignored "-Wunused-result"
	#endif

	template <typename ...Args>
	NdCx__ static exact_t  Last (Args&& ...args)				__NE___	{ return (FwdArg<Args>(args), ... ); }

	# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
	#	pragma clang diagnostic pop
	# endif


	template <typename FN, typename ...Args>
	NdCxIA static exact_t  ApplyExceptLast (FN&& fn, Args&& ...args) __NE___
	{
		StaticAssert( CountOf<Args...>() > 1 );
		CheckNothrow( NoExcept( Tuple<Args...>{ FwdArg<Args>(args) ... } ));

		using Helper = _ApplyExceptLastHelper< MakeUIntSequence< 0u, CountOf<Args...>()-1 >>;

		return Helper::Call( FwdArg<FN>(fn), Tuple<Args...>{ FwdArg<Args>(args) ... });
	}


}; // FoldExpr
} // AE::Base
