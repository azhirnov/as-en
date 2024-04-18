// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Impl/ClassBinder.h"

namespace AE::Scripting::_hidden_
{
/*
=================================================
	_UnaryToStr
=================================================
*/
	const char *  OperatorBinderHelper::_UnaryToStr (EUnaryOperator op)
	{
		switch ( op )
		{
			case EUnaryOperator::Inverse :	return "opNeg";
			case EUnaryOperator::Not :		return "opCom";
			case EUnaryOperator::PreInc :	return "opPreInc";
			case EUnaryOperator::PreDec :	return "opPreDec";
			case EUnaryOperator::PostInc :	return "opPostInc";
			case EUnaryOperator::PostDec :	return "opPostDec";
		}
		RETURN_ERR( "unknown unary operator!" );
	}

/*
=================================================
	_BinToStr
=================================================
*/
	const char *  OperatorBinderHelper::_BinToStr (EBinaryOperator op)
	{
		switch ( op )
		{
			case EBinaryOperator::Assign :		break;
			case EBinaryOperator::Add :			return "opAdd";
			case EBinaryOperator::Sub :			return "opSub";
			case EBinaryOperator::Mul :			return "opMul";
			case EBinaryOperator::Div :			return "opDiv";
			case EBinaryOperator::Mod :			return "opMod";
			case EBinaryOperator::And :			return "opAnd";
			case EBinaryOperator::Or :			return "opOr";
			case EBinaryOperator::Xor :			return "opXor";
			case EBinaryOperator::ShiftLeft :	return "opShl";
			case EBinaryOperator::ShiftRight :	return "opShr";
		}
		RETURN_ERR( "unknown binary operator!" );
	}

/*
=================================================
	_BinAssignToStr
=================================================
*/
	const char *  OperatorBinderHelper::_BinAssignToStr (EBinaryOperator op)
	{
		switch ( op )
		{
			case EBinaryOperator::Assign :		return "opAssign";
			case EBinaryOperator::Add :			return "opAddAssign";
			case EBinaryOperator::Sub :			return "opSubAssign";
			case EBinaryOperator::Mul :			return "opMulAssign";
			case EBinaryOperator::Div :			return "opDivAssign";
			case EBinaryOperator::Mod :			return "opModAssign";
			case EBinaryOperator::And :			return "opAndAssign";
			case EBinaryOperator::Or :			return "opOrAssign";
			case EBinaryOperator::Xor :			return "opXorAssign";
			case EBinaryOperator::ShiftLeft :	return "opShlAssign";
			case EBinaryOperator::ShiftRight :	return "opShrAssign";
		}
		RETURN_ERR( "unknown binary assign operator!" );
	}

/*
=================================================
	_BinRightToStr
=================================================
*/
	const char *  OperatorBinderHelper::_BinRightToStr (EBinaryOperator op)
	{
		switch ( op )
		{
			case EBinaryOperator::Assign :		break;
			case EBinaryOperator::Add :			return "opAdd_r";
			case EBinaryOperator::Sub :			return "opSub_r";
			case EBinaryOperator::Mul :			return "opMul_r";
			case EBinaryOperator::Div :			return "opDiv_r";
			case EBinaryOperator::Mod :			return "opMod_r";
			case EBinaryOperator::And :			return "opAnd_r";
			case EBinaryOperator::Or :			return "opOr_r";
			case EBinaryOperator::Xor :			return "opXor_r";
			case EBinaryOperator::ShiftLeft :	return "opShl_r";
			case EBinaryOperator::ShiftRight :	return "opShr_r";
		}
		RETURN_ERR( "unknown binary operator!" );
	}

} // AE::Scripting::_hidden_
