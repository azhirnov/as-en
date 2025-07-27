#pragma once

#include "base/CompileTime/Concepts.h"
#include "base/CompileTime/TypeList.h"

namespace AE::Base
{

/*
=================================================
	IsCallOperatorSpecialization
=================================================
*/
	template <typename T, typename ...Args>
	concept IsCallOperatorSpecialization = requires
	{
		&T::template operator()< Args... >;
	};

/*
=================================================
	IsCallOperatorSpecializationFromTypeList
=================================================
*/
namespace _hidden_
{
	template <typename ObjT, typename Args>
	struct _IsCallOperatorSpecialization2 : CT_False {};
		
	template <typename ObjT, typename ...Args>
		requires( IsCallOperatorSpecialization< ObjT, Args... >)
	struct _IsCallOperatorSpecialization2< ObjT, TypeList<Args...> > : CT_True {};
}
	template <typename ObjT, typename ArgsTL>
	concept IsCallOperatorSpecializationFromTypeList =	IsTypeList< ArgsTL > and
														Base::_hidden_::_IsCallOperatorSpecialization2< ObjT, ArgsTL >::value;
	
/*
=================================================
	IsCallOperatorSpecializationWith
----
	check if 'ObjT{}.operator< FnArgs ...>()( InputArgs... )' can be compiled
=================================================
*/
namespace _hidden_
{
	template <typename ObjT, typename Args>
	struct _CallOperatorSpecialization_FnPtr;
	
	template <typename ObjT, typename ...Args>
		requires( IsCallOperatorSpecialization< ObjT, Args... >)
	struct _CallOperatorSpecialization_FnPtr< ObjT, TypeList<Args...> >
	{
		using type = decltype( &ObjT::template operator()< Args... > );
	};

	
	template <typename ObjT, typename FN, typename ...Args>
	concept _CanUseCallOperatorWithArgs1 = requires (Args ...args)
	{
		( ObjT{}.* static_cast< FN >(&ObjT::operator()) ) ( FwdArg<Args>(args) ... );
	};

	template <typename ObjT, typename FN, typename Args>
	struct _CanUseCallOperatorWithArgs2 : CT_False {};
		
	template <typename ObjT, typename FN, typename ...Args>
		requires( _CanUseCallOperatorWithArgs1< ObjT, FN, Args... >)
	struct _CanUseCallOperatorWithArgs2< ObjT, FN, TypeList<Args...> > : CT_True {};
}

	template <typename ObjT, typename FnArgsTL, typename InputArgsTL>
	concept IsCallOperatorSpecializationWith =	IsTypeList< FnArgsTL >		and
												IsTypeList< InputArgsTL >	and
												Base::_hidden_::_CanUseCallOperatorWithArgs2<
													ObjT,
													typename Base::_hidden_::_CallOperatorSpecialization_FnPtr< ObjT, FnArgsTL >::type,
													InputArgsTL >::value;

/*
=================================================
	IsTemplateArgWithoutRef		- detect 'auto' argument
	IsTemplateArgConstRef		- detect 'const auto&' argument
	IsTemplateArgRef			- detect 'auto&' argument
	IsTemplateArgRValueRef		- detect 'auto&&' argument
----
	used to check which type of 'auto' argument used in lambda
=================================================
*/
namespace _hidden_
{
	struct LambdaArgType2;

	struct LambdaArgType1
	{
		LambdaArgType1 () {}

	private:
		LambdaArgType1 (LambdaArgType2 &&);
		LambdaArgType1 (const LambdaArgType2 &);
	};

	struct LambdaArgType2 : LambdaArgType1
	{
		LambdaArgType2 () {}
	};

	template <usize Count>
	using LambdaArg1List = TypeListFill< LambdaArgType1, Count >;

	template <typename ObjT, usize ArgCount>
	static constexpr bool  IsTemplateArgWithoutRef =
		not	IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< LambdaArgType2,			ArgCount >>  and
		not	IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< const LambdaArgType2,	ArgCount >>  and
		not	IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< LambdaArgType2 &,		ArgCount >>  and
		not	IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< const LambdaArgType2 &,	ArgCount >>  and
		not	IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< LambdaArgType2 &&,		ArgCount >>  and
			IsCallOperatorSpecializationWith< ObjT,  TypeListFill<int, ArgCount>, TypeListFill< int,					ArgCount >>;

	template <typename ObjT, usize ArgCount>
	static constexpr bool  IsTemplateArgConstRef	=
			IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< LambdaArgType2,			ArgCount >>  and
			IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< const LambdaArgType2,	ArgCount >>  and
			IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< LambdaArgType2 &,		ArgCount >>  and
			IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< const LambdaArgType2 &,	ArgCount >>  and
			IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< LambdaArgType2 &&,		ArgCount >>;

	template <typename ObjT, usize ArgCount>
	static constexpr bool  IsTemplateArgRef	=
		not	IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< LambdaArgType2,			ArgCount >>  and
			IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< LambdaArgType2 &,		ArgCount >>  and
		not IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< const LambdaArgType2,	ArgCount >>  and
		not	IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< const LambdaArgType2 &,	ArgCount >>  and
		not	IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< LambdaArgType2 &&,		ArgCount >>;

	template <typename ObjT, usize ArgCount>
	static constexpr bool  IsTemplateArgRValueRef	=
			IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< LambdaArgType2,			ArgCount >>  and
		not	IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< const LambdaArgType2,	ArgCount >>  and
		not	IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< LambdaArgType2 &,		ArgCount >>  and
		not	IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< const LambdaArgType2 &,	ArgCount >>  and
			IsCallOperatorSpecializationWith< ObjT,  LambdaArg1List< ArgCount >,  TypeListFill< LambdaArgType2 &&,		ArgCount >>;
}
	using Base::_hidden_::IsTemplateArgWithoutRef;
	using Base::_hidden_::IsTemplateArgConstRef;
	using Base::_hidden_::IsTemplateArgRef;
	using Base::_hidden_::IsTemplateArgRValueRef;


} // AE::Base
